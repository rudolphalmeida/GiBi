/*
 * Implements the Pixel Processing Unit (PPU) on the GameBoy.
 *
 * Author: Rudolph Almeida <rudolf1.almeida@gmail.com>
 * */

#ifndef GIBI_PPU_H
#define GIBI_PPU_H

#include <memory>

#include "gibi.h"
#include "lcdc.h"
#include "lcdstatus.h"
#include "mmu/memory.h"
#include "cpu/interrupts.h"

class Bus;

/*
 * Alongside the PPU state, the PPU class is also responsible for rendering the frame to a texture,
 * which will be displayed in the window
 * */
class PPU : public Memory {
   private:
    LCDC lcdc;
    LCDStatus stat;

    byte scy, scx;    // Specify the top-left corner of the display in the 256x256 BG Map
    byte ly;          // The current line to which the data is being transferred i.e. being drawn
    byte lyc;         // The value to compare to LY and raise a LCDStat interrupt
    byte wy, wx;      // The window position Y, and the window position X - 7
    byte bgp;         // Assigns the four shades of gray to BG & Window
    byte obp0, obp1;  // Specify the two palettes available to sprites

    std::vector<byte> vram;
    std::vector<byte> oam;

    std::shared_ptr<IntF> intf;
    std::shared_ptr<Bus> bus;

    uint dots;

   public:
    explicit PPU(std::shared_ptr<IntF> intf, std::shared_ptr<Bus> bus);

    void tick(uint cycles);

    [[nodiscard]] byte read(word address) const override;
    void write(word address, byte data) override;

    void drawScanline(byte line) const;
    void drawBackgroundScanline(byte line) const;
    void drawWindowScanline(byte line) const;

    void drawSprites() const;
};

#endif  // GIBI_PPU_H
