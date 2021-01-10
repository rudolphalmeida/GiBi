/*
 * Implement the physical cartridges that can be inserted into the GameBoy system
 * Due to the limited address space of the GameBoy, carts often came with special
 * chips called memory banking controllers, or MBCs, which allowed the programmer
 * to include additional memory banks that the program could swap in and out at will.
 * This could be used to increase the memory up to 2MB.
 *
 * Author: Rudolph Almeida <rudolf1.almeida@gmail.com>
 * */

#ifndef GIBI_INCLUDE_MMU_CARTRIDGE_H_
#define GIBI_INCLUDE_MMU_CARTRIDGE_H_

#include <array>
#include <optional>
#include <utility>

#include "gibi.h"
#include "memory.h"

// Smaller games of size less than 32KiB did not require a MBC chip for banking.
// The whole game could simple fit in 0x0000-0x7FFF. Optionally, up to 8KiB of RAM
// could be connected to 0xA000-0xBFFF
class NoMBC : public Memory {
   private:
    std::array<byte, 0x8000> rom;
    // The RAM is optional and we don't want to allocate unless it is needed
    std::optional<std::array<byte, 0x2000>> ram;

   public:
    // If both RAM and ROM are needed
    NoMBC(std::array<byte, 0x8000> rom, std::array<byte, 0x2000> ram) : rom{rom}, ram{ram} {}
    // If only ROM is needed
    explicit NoMBC(std::array<byte, 0x8000> rom) : rom{rom}, ram{std::nullopt} {}

    [[nodiscard]] byte read(word address) const override {
        if (inRange(address, 0x0000, 0x8000)) {
            return rom[address];
        } else if (inRange(address, 0xA000, 0xC000) && ram) {
            return ram.value()[address];
        }

        return 0xFF;  // This really shouldn't be needed
    }

    void write(word address, byte data) override {
        if (inRange(address, 0x0000, 0x8000)) {
            rom[address] = data;
        } else if (inRange(address, 0xA000, 0xC000) && ram) {
            ram.value()[address] = data;
        }
    }
};

#endif  // GIBI_INCLUDE_MMU_CARTRIDGE_H_
