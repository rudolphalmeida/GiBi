#include <memory>

#include "cpu/interrupts.h"
#include "gibi.h"
#include "ppu/lcdc.h"
#include "ppu/lcdstatus.h"
#include "ppu/ppu.h"

PPU::PPU(std::shared_ptr<Bus> bus, std::shared_ptr<IntF> intf)
    : lcdc{},
      stat{},
      scy{0x00},
      scx{0x00},
      ly{0x00},
      lyc{0x00},
      wy{0x00},
      wx{0x00},
      bgp{0xFC},
      obp0{0xFF},
      obp1{0xFF},
      vram(0x2000),
      oam(0xA0),
      intf{std::move(intf)},
      bus{std::move(bus)} {}

void PPU::tick(uint cycles) {}
