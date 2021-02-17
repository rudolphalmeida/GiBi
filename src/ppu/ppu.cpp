#include <memory>

#include "cpu/interrupts.h"
#include "gibi.h"
#include "ppu/lcdc.h"
#include "ppu/lcdstatus.h"
#include "ppu/ppu.h"

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
      intf{std::move(intf)} {}

void PPU::tick(uint) {}

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
