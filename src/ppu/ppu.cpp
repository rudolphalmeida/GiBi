#include <memory>

#include "cpu/interrupts.h"
#include "gibi.h"
#include "ppu/ppu.h"
#include "constants.h"

PPU::PPU(std::shared_ptr<IntF> intf)
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
      dots{0} {}

void PPU::tick(uint cycles) {
    if (!lcdc.displayEnabled()) {
        return;
    }

    dots += cycles;

    switch (stat.mode()) {
        case LCDMode::AccessingOAM: {
            if (dots >= ACCESSING_OAM_CLOCKS) {
                dots %= ACCESSING_OAM_CLOCKS;
                stat.setMode(LCDMode::AccessingVRAM);
            }
            break;
        }
        case LCDMode::AccessingVRAM: {
            if (dots >= ACCESSING_VRAM_CLOCKS) {
                dots %= ACCESSING_VRAM_CLOCKS;

                if (stat.mode0HBlankInterruptEnabled()) {
                    intf->request(Interrupts::LCDStat);
                }

                stat.setMode(LCDMode::HBlank);
            }
            break;
        }
        case LCDMode::HBlank: {
            if (dots >= HBLANK_CLOCKS) {
                dots %= HBLANK_CLOCKS;

                drawScanline(ly);
                ly += 1;

                if (ly > 143) { // Going into VBlank
                    if (stat.mode1VBlankInterruptEnabled()) {
                        intf->request(Interrupts::LCDStat);
                    }

                    intf->request(Interrupts::VBlank);
                    stat.setMode(LCDMode::VBlank);
                } else { // Going into OAM Search
                    // Even if both conditions are met, only one interrupt fires
                    // From PanDocs:
                    // The interrupt is triggered when transitioning from "No conditions met" to
                    // "Any condition met", which can cause the interrupt to not fire.
                    // Example : the Mode 0 and LY=LYC interrupts are enabled ; since the latter
                    // triggers during Mode 2 (right after Mode 0), the interrupt will trigger
                    // for Mode 0 but fail to for LY=LYC.
                    if (stat.mode2OAMInterruptEnabled() || (stat.coincidenceInterruptEnabled() && ly == lyc)) {
                        intf->request(Interrupts::LCDStat);
                    }

                    if (ly == lyc) {
                        stat.setData(setBit(stat.getData(), 2));
                    } else {
                        stat.setData(resetBit(stat.getData(), 2));
                    }

                    stat.setMode(LCDMode::AccessingOAM);
                }
            }

            break;
        }
        case LCDMode::VBlank: {
            if (dots >= CLOCKS_PER_SCANLINE) {
                dots %= CLOCKS_PER_SCANLINE;
                ly += 1;

                if (ly > 153) { // Starting new frame. Going into OAM Search
                    if (stat.mode2OAMInterruptEnabled()) {
                        intf->request(Interrupts::LCDStat);
                    }

                    ly = 0;
                    stat.setMode(LCDMode::AccessingOAM);

                    drawSprites();
                    // Render to screen here

                    // Clear the buffer here
                }
            }

            break;
        }
    }
}

byte PPU::read(word address) const {
    if (inRange(address, 0x8000, 0x9FFF)) {
        return vram[address - 0x8000];
    } else if (inRange(address, 0xFE00, 0xFE9F)) {
        return oam[address - 0xFE00];
    } else if (address == 0xFF40) {
        return lcdc.getData();
    } else if (address == 0xFF41) {
        return stat.getData();
    } else if (address == 0xFF42) {
        return scy;
    } else if (address == 0xFF43) {
        return scx;
    } else if (address == 0xFF44) {
        return ly;
    } else if (address == 0xFF45) {
        return lyc;
    } else if (address == 0xFF47) {
        return bgp;
    } else if (address == 0xFF48) {
        return obp0;
    } else if (address == 0xFF49) {
        return obp1;
    } else if (address == 0xFF4A) {
        return wy;
    } else if (address == 0xFF4B) {
        return wx;
    }

    return 0xFF;
}
void PPU::write(word address, byte data) {
    if (inRange(address, 0x8000, 0x9FFF)) {
        vram[address - 0x8000] = data;
    } else if (inRange(address, 0xFE00, 0xFE9F)) {
        oam[address - 0xFE00] = data;
    } else if (address == 0xFF40) {
        lcdc.setData(data);
    } else if (address == 0xFF41) {
        stat.setData(data);
    } else if (address == 0xFF42) {
        scy = data;
    } else if (address == 0xFF43) {
        scx = data;
    } else if (address == 0xFF44) {
        ly = data;
    } else if (address == 0xFF45) {
        lyc = data;
    } else if (address == 0xFF47) {
        bgp = data;
    } else if (address == 0xFF48) {
        obp0 = data;
    } else if (address == 0xFF49) {
        obp1 = data;
    } else if (address == 0xFF4A) {
        wy = data;
    } else if (address == 0xFF4B) {
        wx = data;
    }
}

void PPU::drawScanline(byte) const {}

void PPU::drawSprites() const {}
