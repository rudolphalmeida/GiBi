/*
 * Implements the Pixel Processing Unit (PPU) on the GameBoy.
 *
 * Author: Rudolph Almeida <rudolf1.almeida@gmail.com>
 * */

#ifndef GIBI_PPU_H
#define GIBI_PPU_H

#include <memory>
#include <vector>

#include "cpu/interrupts.h"
#include "gibi.h"
#include "lcdc.h"
#include "lcdstatus.h"
#include "mmu/memory.h"

class Bus;

// The GameBoy (DMG-01) had a color depth of 4 which were actually 4 shades of gray, but appeared
// green on the screen. We want the user to be able to select the palette that is to be used (For
// e.g. 4 shades of green, or gray) so we map the shades to "names" and not actual "colors" in the
// PPU. The renderer will map these to actual colors
enum class DisplayColor : byte { White = 0, LightGray = 1, DarkGray = 2, Black = 3 };

/*
 * Alongside the PPU state, the PPU class is also responsible for rendering the frame to a buffer,
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

    std::vector<DisplayColor> pixelBuffer;

    uint dots;

    // Draws a single scanline of the background and the window layer
    void drawScanline(byte line);
    void drawBackgroundScanline(byte line);
    void drawWindowScanline(byte line);

    void drawSprites() const;

   public:

    explicit PPU(std::shared_ptr<IntF> intf, std::shared_ptr<Bus> bus);

    void tick(uint cycles);
    [[nodiscard]] byte read(word address) const override;

    void write(word address, byte data) override;

    [[nodiscard]] const std::vector<DisplayColor>& buffer() const {
        return pixelBuffer;
    }
};

// A palette allowed any of the four gray shades to be mapped to any of the "actual" color
// shades
struct Palette {
    DisplayColor color0, color1, color2, color3;

    explicit Palette(byte data)
        : color0{static_cast<DisplayColor>(data & 0b11)},
          color1{static_cast<DisplayColor>((data & 0b1100) >> 2)},
          color2{static_cast<DisplayColor>((data & 0b110000) >> 4)},
          color3{static_cast<DisplayColor>((data & 0b11000000) >> 6)} {}

    [[nodiscard]] DisplayColor fromID(byte id) const {
        switch (id) {
            case 0: return color0;
            case 1: return color1;
            case 2: return color2;
            case 3: return color3;
            default: return color0; // Not really needed
        }
    }
};

#endif  // GIBI_PPU_H
