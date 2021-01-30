/*
 * Author: Rudolph Almeida <rudolf1.almeida@gmail.com>
 * */

#include <spdlog/spdlog.h>

#include "gibi.h"
#include "mmu/cartridge.h"

byte NoMBC::read(word address) const {
    if (inRange(address, 0x0000, 0x7FFF)) {
        return rom[address];
    } else if (inRange(address, 0xA000, 0xBFFF) && ram) {
        return ram.value()[address];
    }

    return 0xFF;  // This really shouldn't be needed
}

void NoMBC::write(word address, byte data) {
    if (inRange(address, 0x0000, 0x7FFF)) {
        rom[address] = data;
    } else if (inRange(address, 0xA000, 0xBFFF) && ram) {
        ram.value()[address] = data;
    }
}

Cartridge::Cartridge(std::vector<byte> rom, std::optional<std::vector<byte>> ram) {
    // The byte at 0x0147 contains information about the MBC used
    auto [type, isSavable] = determineMBCType(rom[0x0147]);
    this->mbcType = type;
    this->savable = isSavable;

    numRomBanks = determineROMBanks(rom[0x0148]);
    ramSizeInKB = determineRAMSize(rom[0x0149]);

    // If RAM is required but not supplied
    if (ramSizeInKB != 0 && !ram) {
        spdlog::info("RAM not supplied. Allocating...");
        std::vector<byte> data(ramSizeInKB, 0);
        ram = std::optional(data);
    }

    this->mbc = initMBC(mbcType, std::move(rom), std::move(ram));
}

byte Cartridge::read(word address) const {
    return mbc->read(address);
}

void Cartridge::write(word address, byte data) {
    mbc->write(address, data);
}

std::pair<CartType, bool> determineMBCType(byte code) {
    switch (code) {
        case 0x00: {
            return {CartType::ROM, false};
        }
        case 0x01:
        case 0x02: {
            return {CartType::MBC1, false};
        }
        case 0x03: {
            return {CartType::MBC1, true};
        }
        case 0x05: {
            return {CartType::MBC2, false};
        }
        case 0x06: {
            return {CartType::MBC2, true};
        }
        case 0x08: {
            return {CartType::ROM_RAM, false};
        }
        case 0x09: {
            return {CartType::ROM_RAM, true};
        }
        default: {
            spdlog::critical("Unknown or unsupported cartridge type");
            std::exit(-1);
        }
    }
}

uint determineROMBanks(byte code) {
    switch (code) {
        case 0x00:
            return 2;
        case 0x01:
            return 4;
        case 0x02:
            return 8;
        case 0x03:
            return 16;
        case 0x04:
            return 32;
        case 0x05:
            return 64;
        case 0x06:
            return 128;
        case 0x07:
            return 256;
        case 0x08:
            return 512;
        case 0x52:
            return 72;
        case 0x53:
            return 80;
        case 0x54:
            return 96;
        default: {
            spdlog::critical("Unknown ROM Size");
            std::exit(-1);
        }
    }
}

uint determineRAMSize(byte code) {
    switch (code) {
        case 0x00:
            return 0;
        case 0x01:
            return 2;
        case 0x02:
            return 8;
        case 0x03:
            return 32;
        case 0x04:
            return 128;
        case 0x05:
            return 64;
        default: {
            spdlog::critical("Unknown RAM size");
            std::exit(-1);
        }
    }
}

std::unique_ptr<Memory> initMBC(CartType type,
                                std::vector<byte>&& rom,
                                std::optional<std::vector<byte>>&& ram) {
    switch (type) {
        case CartType::ROM: {
            if (ram) {
                spdlog::error("Cart does not support RAM. Ignoring...");
            }
            return std::make_unique<NoMBC>(rom);
        }
        case CartType::ROM_RAM: {
            return std::make_unique<NoMBC>(rom, ram);
        }
            //        case CartType::MBC1:
            //            break;
            //        case CartType::MBC2:
            //            break;
        default: {
            spdlog::error("Unimplemented MBC");
            std::exit(-1);
        }
    }
}
