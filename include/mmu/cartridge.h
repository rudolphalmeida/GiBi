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

#include <optional>
#include <utility>
#include <vector>

#include "gibi.h"
#include "memory.h"

// Smaller games of size less than 32KiB did not require a MBC chip for banking.
// The whole game could simple fit in 0x0000-0x7FFF. Optionally, up to 8KiB of RAM
// could be connected to 0xA000-0xBFFF
class NoMBC : public Memory {
   private:
    std::vector<byte> rom;
    // The RAM is optional and we don't want to allocate unless it is needed
    std::optional<std::vector<byte>> ram;

   public:
    // If both RAM and ROM are needed
    NoMBC(std::vector<byte> rom, std::vector<byte> ram) : rom{std::move(rom)}, ram{ram} {}
    // If only ROM is needed
    explicit NoMBC(std::vector<byte> rom) : rom{std::move(rom)}, ram{std::nullopt} {}

    [[nodiscard]] byte read(word address) const override;
    void write(word address, byte data) override;
};

#endif  // GIBI_INCLUDE_MMU_CARTRIDGE_H_
