/*
 * The bus connecting all components to the CPU. The physical bus contains 8 data
 * lines and 16 address lines. All components except for the CPU are controlled
 * and communicated with via memory-mapped registers.
 *
 * Author: Rudolph Almeida <rudolf1.almeida@gmail.com>
 * */

#ifndef GIBI_INCLUDE_MMU_BUS_H_
#define GIBI_INCLUDE_MMU_BUS_H_

#include <memory>
#include <utility>

#include "cartridge.h"
#include "gibi.h"
#include "memory.h"

// Abstracts the memory-map and delegates reads and writes to the appropriate
// component or region of memory
class Bus : public Memory {
   private:
    Cartridge cart;
    std::vector<byte> wram;
    std::vector<byte> hram;

   public:
    // Bus needs to have ownership the Cartridge
    explicit Bus(Cartridge&& cart) : cart{std::move(cart)}, wram(0x2000), hram(0x7F) {}

    [[nodiscard]] byte read(word address) const override;

    void write(word address, byte data) override;
};

#endif  // GIBI_INCLUDE_MMU_BUS_H_
