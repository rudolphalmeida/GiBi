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
#include "cpu/interrupts.h"
#include "cpu/timer.h"
#include "gibi.h"
#include "joypad.h"
#include "memory.h"
#include "ppu/ppu.h"
#include "serial.h"

// Abstracts the memory-map and delegates reads and writes to the appropriate
// component or region of memory
class Bus : public Memory {
   private:
    Cartridge cart;
    std::vector<byte> wram;
    std::vector<byte> hram;

    std::shared_ptr<IntF> intf;
    std::shared_ptr<IntE> inte;

    Timer timer;
    JoyPad joyPad;
    Serial serial;

    std::shared_ptr<PPU> ppu;

    //    PPU ppu;

   public:
    // Bus needs to have ownership the Cartridge
    explicit Bus(Cartridge&& cart, const std::shared_ptr<IntF>& intf, std::shared_ptr<IntE> inte)
        : cart{std::move(cart)},
          wram(0x2000),
          hram(0x7F),
          intf{intf},
          inte{std::move(inte)},
          timer{intf},
          joyPad{intf},
          serial{},
          ppu{nullptr} {}

    void connectPPU(std::shared_ptr<PPU> ppu1) { ppu = std::move(ppu1); }

    void tick(uint cycles);

    JoyPad& getJoyPad() { return joyPad; }

    [[nodiscard]] byte read(word address) const override;

    void write(word address, byte data) override;
};

#endif  // GIBI_INCLUDE_MMU_BUS_H_
