#include <memory>
#include <utility>

#include "cpu/interrupts.h"
#include "gibi.h"
#include "mmu/bus.h"
#include "options.h"
#include "ppu/ppu.h"

PPU::PPU(std::shared_ptr<IntF> intf, std::shared_ptr<Bus> bus, std::shared_ptr<Options> options)
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
      bus{std::move(bus)},
      pixelBuffer(LCD_WIDTH * LCD_HEIGHT),
      dots{0},
      options{std::move(options)} {}

void PPU::tick(uint cycles) {
    if (!lcdc.displayEnabled()) {
        return;
    }

    for (uint i = 0; i < cycles; i++) {
        dots++;

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

                    if (ly >= LCD_HEIGHT) {  // Going into VBlank
                        if (stat.mode1VBlankInterruptEnabled()) {
                            intf->request(Interrupts::LCDStat);
                        }

                        intf->request(Interrupts::VBlank);
                        stat.setMode(LCDMode::VBlank);
                    } else {  // Going into OAM Search
                        // Even if both conditions are met, only one interrupt fires
                        // From PanDocs:
                        // The interrupt is triggered when transitioning from "No conditions met" to
                        // "Any condition met", which can cause the interrupt to not fire.
                        // Example : the Mode 0 and LY=LYC interrupts are enabled ; since the latter
                        // triggers during Mode 2 (right after Mode 0), the interrupt will trigger
                        // for Mode 0 but fail to for LY=LYC.
                        if (stat.mode2OAMInterruptEnabled() ||
                            (stat.coincidenceInterruptEnabled() && ly == lyc)) {
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

                    if (ly >= TOTAL_SCANLINES) {  // Starting new frame. Going into OAM Search
                        if (stat.mode2OAMInterruptEnabled()) {
                            intf->request(Interrupts::LCDStat);
                        }

                        ly = 0;
                        stat.setMode(LCDMode::AccessingOAM);
                    }
                }

                break;
            }
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

void PPU::drawScanline(byte line) {
    if (!lcdc.displayEnabled()) {
        return;
    }

    if (lcdc.bgWindowDisplayPriority() && !options->disableBackground) {
        drawBackgroundScanline(line);
    } else if (!lcdc.bgWindowDisplayPriority()) {
        // If LCDC.0 is reset then color 0 from BGP is to be drawn
        Palette palette{bgp};
        for (uint screenX = 0; screenX < LCD_WIDTH; screenX++) {
            DisplayColor actualColor = palette.color0;
            pixelBuffer[screenX + line * LCD_WIDTH] = actualColor;
        }
    }

    if (lcdc.bgWindowDisplayPriority() && lcdc.windowEnabled() && !options->disableWindows) {
        drawWindowScanline(line);
    }

    if (lcdc.objEnabled() && !options->disableSprites) {
        drawSprites(line);
    }
}

void PPU::drawBackgroundScanline(byte line) {
    Palette palette{bgp};

    word tileSetAddress = static_cast<word>(lcdc.tileData());
    word tileMapAddress = static_cast<word>(lcdc.bgTileMap());

    uint screenY = line;

    for (uint screenX = 0; screenX < LCD_WIDTH; screenX++) {
        // Displace the coordinate in the background map by the position of the viewport that is
        // shown on the screen and wrap around the BG map if it overflows the BG map
        uint bgMapX = (screenX + scx) % BG_MAP_SIZE;
        uint bgMapY = (screenY + scy) % BG_MAP_SIZE;

        // Find the "tile coordinate" i.e. the tile position where the pixel falls
        uint tileX = bgMapX / TILE_WIDTH_PX;
        uint tileY = bgMapY / TILE_HEIGHT_PX;

        // Find the coordinate of the pixel inside the tile it falls on
        uint tilePixelX = bgMapX % TILE_WIDTH_PX;
        uint tilePixelY = bgMapY % TILE_HEIGHT_PX;

        // Index of the tile in the 1D tile map
        uint tileIndex = tileY * TILES_PER_LINE + tileX;
        word tileIndexAddress = tileMapAddress + tileIndex;
        byte tileId = bus->read(tileIndexAddress);

        /* VRAM tile data is divided into three sections of 128 tiles each:
         *   0x8000 - 0x87FF
         *   0x8800 - 0x8FFF
         *   0x9000 - 0x9FFF
         *
         * Indexing into these is performed using an 8-bit integer using two addressing modes
         * controlled by LCDC.4
         *
         * 0x8000 method (TileData1): Use 0x8000 as the base address and unsigned indices
         * 0x8800 method (TileData0): Use 0x9000 as the base address and signed indices
         *
         * ^ This is only applicable to BG and window tiles. Sprites always use 0x8000 addressing
         * */
        uint tileDataMemOffset = lcdc.tileData() == TileDataBase::TileData0
                                     ? (static_cast<sbyte>(tileId) + 128) * SIZEOF_TILE
                                     : tileId * SIZEOF_TILE;

        // Offset of the particular pixels 2 bytes in the whole tile
        uint tileDataLineOffset = tilePixelY * 2;
        word tileLineDataStartAddress = tileSetAddress + tileDataMemOffset + tileDataLineOffset;

        byte pixel1 = bus->read(tileLineDataStartAddress);
        byte pixel2 = bus->read(tileLineDataStartAddress + 1);

        byte colorId = static_cast<byte>((bitValue(pixel2, 7 - tilePixelX) << 1) |
                                         bitValue(pixel1, 7 - tilePixelX));
        DisplayColor actualColor = palette.fromID(colorId);

        pixelBuffer[screenX + screenY * LCD_WIDTH] = actualColor;
    }
}

void PPU::drawWindowScanline(byte line) {
    Palette palette{bgp};

    word tileSetAddress = static_cast<word>(lcdc.tileData());
    word tileMapAddress = static_cast<word>(lcdc.windowTileMap());

    uint screenY = line;
    uint scrolledY = screenY + wy;

    if (scrolledY >= LCD_HEIGHT)
        return;

    for (uint screenX = 0; screenX < LCD_WIDTH; screenX++) {
        uint scrolledX = screenX + wx - 7;

        uint tileX = scrolledX / TILE_WIDTH_PX;
        uint tileY = scrolledY / TILE_HEIGHT_PX;

        uint tilePixelX = scrolledX % TILE_WIDTH_PX;
        uint tilePixelY = scrolledY % TILE_HEIGHT_PX;

        uint tileIndex = tileY * TILES_PER_LINE + tileX;
        word tileIndexAddress = tileMapAddress + tileIndex;

        byte tileId = bus->read(tileIndexAddress);

        uint tileDataMemOffset = lcdc.tileData() == TileDataBase::TileData0
                                     ? (static_cast<sbyte>(tileId) + 128) * SIZEOF_TILE
                                     : tileId * SIZEOF_TILE;

        uint tileDataLineOffset = tilePixelY * 2;

        word tileLineDataStartAddress = tileSetAddress + tileDataMemOffset + tileDataLineOffset;

        byte pixel1 = bus->read(tileLineDataStartAddress);
        byte pixel2 = bus->read(tileLineDataStartAddress + 1);

        byte colorId = static_cast<byte>((bitValue(pixel2, 7 - tilePixelX) << 1) |
                                         bitValue(pixel1, 7 - tilePixelX));
        DisplayColor actualColor = palette.fromID(colorId);

        pixelBuffer[screenX + screenY * LCD_WIDTH] = actualColor;
    }
}

void PPU::drawSprites(byte line) {
    for (uint sprite = 0, spriteCount = 0;
         sprite < NUM_SPRITES_PER_FRAME && spriteCount < NUM_SPRITES_PER_LINE; sprite++) {
        if (drawSprite(sprite, line))
            spriteCount++;
    }
}

bool PPU::drawSprite(uint sprite, byte lineY) {
    // In Non-CGB mode, the sprites are ordered from highest to lowest priority in the OAM. We
    // iterate over the sprites in reverse, so as to draw the earlier, higher priority sprites over
    // the ones with lower priority
    uint spriteIndex = NUM_SPRITES_PER_FRAME - sprite - 1;

    word offsetInOAM = spriteIndex * SIZEOF_SPRITE_IN_OAM;
    word addressInOAM = OAM_START + offsetInOAM;

    byte spriteY = bus->read(addressInOAM);
    uint startY = spriteY < 16 ? 0 : spriteY - 16;
    uint spriteHeight = lcdc.objHeight();

    // Check if sprite tile is overlapping with current scanline
    if (lineY < startY || lineY >= (startY + spriteHeight))
        return false;

    byte spriteX = bus->read(addressInOAM + 1);
    uint startX = spriteX < TILE_WIDTH_PX ? 0 : spriteX - TILE_WIDTH_PX;

    // Sprites hidden outside horizontal bounds still count towards the per line count
    if (spriteX == 0 || spriteX > (LCD_WIDTH + TILE_WIDTH_PX))
        return true;

    // Sprites always use 0x8000 addressing mode
    word tileSetAddress = static_cast<word>(TileDataBase::TileData1);

    byte tileNumber = bus->read(addressInOAM + 2);
    byte spriteAttribs = bus->read(addressInOAM + 3);

    bool flipX = isSet(spriteAttribs, 5);
    bool flipY = isSet(spriteAttribs, 6);
    bool hiddenBehindBG = isSet(spriteAttribs, 7);

    Palette palette{isSet(spriteAttribs, 4) ? obp1 : obp0};

    uint tileOffset = tileNumber * SIZEOF_TILE;
    word tileAddress = tileSetAddress + tileOffset;

    SpriteTile spriteTile{tileAddress, bus, spriteHeight};
    uint spriteLineY = flipY ? (startY + spriteHeight - lineY - 1) : (lineY - startY);

    for (uint lineX = startX; lineX < (startX + TILE_WIDTH_PX) && (lineX < LCD_WIDTH); lineX++) {
        uint spriteLineX = flipX ? (startX + TILE_WIDTH_PX - lineX - 1) : (lineX - startX);

        DisplayColor colorInTileData = spriteTile.pixelValue(spriteLineX, spriteLineY);
        // White color is transparent for sprites
        if (colorInTileData == DisplayColor::White)
            continue;

        DisplayColor bgOrWindowColor = pixelBuffer[lineX + lineY * LCD_WIDTH];
        if (hiddenBehindBG && bgOrWindowColor != DisplayColor::White)
            continue;

        pixelBuffer.at(lineX + lineY * LCD_WIDTH) = palette.fromColor(colorInTileData);
    }

    return true;
}

SpriteTile::SpriteTile(word startAddress, const std::shared_ptr<Bus>& bus, uint heightOfTile)
    : spriteData(heightOfTile * PPU::TILE_WIDTH_PX) {
    for (uint tileLine = 0; tileLine < heightOfTile; tileLine++) {
        uint lineByteIndexInTile = tileLine * 2;
        word byteAddress = startAddress + lineByteIndexInTile;

        byte pixel1 = bus->read(byteAddress);
        byte pixel2 = bus->read(byteAddress + 1);

        for (uint x = 0; x < PPU::TILE_WIDTH_PX; x++) {
            byte colorCode =
                static_cast<byte>((bitValue(pixel2, 7 - x) << 1) | bitValue(pixel1, 7 - x));
            spriteData.at(x + tileLine * PPU::TILE_WIDTH_PX) = static_cast<DisplayColor>(colorCode);
        }
    }
}
