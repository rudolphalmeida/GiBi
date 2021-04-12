/*
 * Author: Rudolph Almeida <rudolf1.almeida@gmail.com>
 * */

#include "mmu/cartridge.h"

#include <utility>
#include "gibi.h"

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
            std::exit(-1);
        }
    }
}

uint determineROMBanks(byte code) {
    // clang-format off
    switch (code) {
        case 0x00: return 2;
        case 0x01: return 4;
        case 0x02: return 8;
        case 0x03: return 16;
        case 0x04: return 32;
        case 0x05: return 64;
        case 0x06: return 128;
        case 0x07: return 256;
        case 0x08: return 512;
        case 0x52: return 72;
        case 0x53: return 80;
        case 0x54: return 96;
        default: std::exit(-1);
    }
    // clang-format on
}

uint determineRAMSize(byte code) {
    // clang-format off
    switch (code) {
        case 0x00: return 0;
        case 0x01: return 2;
        case 0x02: return 8;
        case 0x03: return 32;
        case 0x04: return 128;
        case 0x05: return 64;
        default: std::exit(-1);
    }
    // clang-format on
}

std::unique_ptr<Memory> initMBC(CartType type,
                                std::vector<byte>&& rom,
                                std::optional<std::vector<byte>>&& ram) {
    switch (type) {
        case CartType::ROM: {
            if (ram) {
                // Cart does not support RAM. Ignoring..
            }
            return std::make_unique<NoMBC>(rom);
        }
        case CartType::ROM_RAM: {
            return std::make_unique<NoMBC>(rom, ram);
        }
        case CartType::MBC1: {
            return std::make_unique<MBC1>(rom, ram);
        }
            //        case CartType::MBC2:
            //            break;
        default: {
            // Unimplemented MBC
            std::exit(-1);
        }
    }
}

MBC1::MBC1(std::vector<byte> rom, std::optional<std::vector<byte>> ram)
    : rom{std::move(rom)},
      ram{std::move(ram)},
      bankMode{MBC1::BankMode::ROM},
      bank{0x01},
      ramEnabled{false} {}

uint MBC1::romBank() const {
    switch (bankMode) {
        case BankMode::ROM:
            return bank & 0x7F;
        case BankMode::RAM:
            return bank & 0x1F;
    }

    return 0;  // This should'nt be needed
}

uint MBC1::ramBank() const {
    switch (bankMode) {
        case BankMode::ROM:
            return 0x00;
        case BankMode::RAM:
            return (bank & 0x60) >> 5;
    }

    return 0;  // This should'nt be needed
}

byte MBC1::read(word address) const {
    if (inRange(address, 0x0000, 0x3FFF)) {
        return rom[address];
    } else if (inRange(address, 0x4000, 0x7FFF)) {
        word mappedAddress = romBank() * 0x4000 + address - 0x4000;
        return rom[mappedAddress];
    } else if (inRange(address, 0xA000, 0xBFFF)) {
        if (ram && ramEnabled) {
            word mappedAddress = ramBank() * 0x2000 + address - 0xA000;
            return ram.value()[mappedAddress];
        } else {
            return 0xFF;
        }
    }

    return 0xFF;
}

void MBC1::write(word address, byte data) {
    if (inRange(address, 0x0000, 0x1FFF)) {
        ramEnabled = (data & 0x0F) == 0x0A;
    } else if (inRange(address, 0x2000, 0x3FFF)) {
        byte n = data & 0x1F;
        n = n == 0x00 ? 0x01 : n;
        bank = (bank & 0x60) | n;
    } else if (inRange(address, 0x4000, 0x5FFF)) {
        byte n = data & 0x03;
        bank = (bank & 0x9F) | (n << 5);
    } else if (inRange(address, 0x6000, 0x7FFF)) {
        switch (data) {
            case 0x00: {
                bankMode = BankMode::ROM;
                break;
            }
            case 0x01: {
                bankMode = BankMode::RAM;
                break;
            }
            default: {
                // Invalid mode select value
            }
        }
    } else if (inRange(address, 0xA000, 0xBFFF)) {
        if (ram && ramEnabled) {
            word mappedAddress = ramBank() * 0x2000 + address - 0xA000;
            ram.value()[mappedAddress] = data;
        }
    }
}
