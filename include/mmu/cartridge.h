/*
 * Implement the physical cartridges that can be inserted into the GameBoy system
 * Due to the limited address space of the GameBoy, carts often came with special
 * chips called memory banking controllers, or MBCs, which allowed the programmer
 * to include additional memory banks that the program could swap in and out at will.
 * This could be used to increase the memory up to 8MB.
 *
 * Author: Rudolph Almeida <rudolf1.almeida@gmail.com>
 * */

#ifndef GIBI_INCLUDE_MMU_CARTRIDGE_H_
#define GIBI_INCLUDE_MMU_CARTRIDGE_H_

#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "gibi.h"
#include "memory.h"

// Games could run on either the DMG, or the CGB, or both. Not implementing SGB
enum class ModelMode { DMG, CGB, DMG_CGB };

// What MBC the cart has if any
enum class CartType { ROM, ROM_RAM, MBC1, MBC2 };

std::pair<CartType, bool> determineMBCType(byte code);
uint determineROMBanks(byte code);
uint determineRAMSize(byte code);

std::unique_ptr<Memory> initMBC(CartType type,
                                std::vector<byte>&& rom,
                                std::optional<std::vector<byte>>&& ram);

// Cartridges had a header located at 0x0100-0x014F which contained information
// about the game, it's memory and MBC requirements, licence codes, etc.
class Cartridge : public Memory {
   private:
    std::unique_ptr<Memory> mbc;
    CartType mbcType;

    bool savable;  // Does the cart have battery backup for game saves?

    uint numRomBanks;  // Number of ROM banks of 16KB each
    uint ramSizeInKB;  // Size in KBytes. If >8 then #RAM Banks = size / 8;

    //    std::string title;
    //    ModelMode mode;  // TODO: Let user choose this for games that support both

   public:
    explicit Cartridge(std::vector<byte> rom, std::optional<std::vector<byte>> ram = std::nullopt);

    [[nodiscard]] byte read(word address) const override;
    void write(word address, byte data) override;
};

// Smaller games of size less than 32KiB did not require a MBC chip for banking.
// The whole game could simple fit in 0x0000-0x7FFF. Optionally, up to 8KiB of RAM
// could be connected to 0xA000-0xBFFF
class NoMBC : public Memory {
   private:
    std::vector<byte> rom;
    // The RAM is optional and we don't want to allocate unless it is needed
    std::optional<std::vector<byte>> ram;

   public:
    // The RAM is optional. Could be either user supplied if the game supports
    // saving or allocated by the emulator if not supplied but required
    explicit NoMBC(std::vector<byte> rom, std::optional<std::vector<byte>> ram = std::nullopt)
        : rom{std::move(rom)}, ram{std::move(ram)} {}

    [[nodiscard]] byte read(word address) const override;
    void write(word address, byte data) override;
};

// The MBC1 was the first MBC controller for the GameBoy and allows for ROM and RAM banking
// Allows for a max 2MB ROM and 32KB RAM
class MBC1 : public Memory {
   public:
    enum class BankMode { ROM, RAM };

   private:
    std::vector<byte> rom;
    std::optional<std::vector<byte>> ram;
    BankMode bankMode;
    uint bank;
    bool ramEnabled;

   public:
    MBC1(std::vector<byte> rom, std::optional<std::vector<byte>> ram);

    [[nodiscard]] uint romBank() const;

    [[nodiscard]] uint ramBank() const;

    [[nodiscard]] byte read(word address) const override;

    void write(word address, byte data) override;
};

#endif  // GIBI_INCLUDE_MMU_CARTRIDGE_H_
