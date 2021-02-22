#include <memory>

#include "constants.h"
#include "cpu/interrupts.h"
#include "gibi.h"
#include "mmu/bus.h"
#include "ppu/ppu.h"

PPU::PPU(std::shared_ptr<IntF> intf, std::shared_ptr<Bus> bus)
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

void PPU::drawScanline(byte line) {
    if (!lcdc.displayEnabled()) {
        return;
    }

    if (lcdc.bgWindowDisplayPriority()) {
        drawBackgroundScanline(line);
    }

    if (lcdc.windowEnabled()) {
        drawWindowScanline(line);
    }
}

// Based on jgilchrist/gbemu (https://github.com/jgilchrist/gbemu/)
void PPU::drawBackgroundScanline(byte line) {
    Palette palette{bgp};

    word tileSetAddress = static_cast<word>(lcdc.tileData());
    word tileMapAddress = static_cast<word>(lcdc.bgTileMap());

    uint screenY = line;

    for (uint screenX = 0; screenX < LCD_WIDTH; screenX++) {
        // Position of the pixel in the framebuffer
        uint scrolledX = screenX + scx;
        uint scrolledY = screenY + scy;

        // Index of the pixel in background map
        uint bgMapX = scrolledX % BG_MAP_SIZE;
        uint bgMapY = scrolledY % BG_MAP_SIZE;

        // Index of the tile in the background map
        uint tileX = bgMapX / TILE_WIDTH_PX;
        uint tileY = bgMapY / TILE_HEIGHT_PX;

        // Index in the tile
        uint tilePixelX = bgMapX % TILE_WIDTH_PX;
        uint tilePixelY = bgMapY % TILE_HEIGHT_PX;

        uint tileIndex = tileY * TILES_PER_LINE + tileX;
        word tileIndexAddress = tileMapAddress + tileIndex;

        byte tileId = bus->read(tileIndexAddress);

        // Calculate offset from start of tile data
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

void PPU::drawWindowScanline(byte line) {
    Palette palette{bgp};

    word tileSetAddress = static_cast<word>(lcdc.tileData());
    word tileMapAddress = static_cast<word>(lcdc.windowTileMap());

    uint screenY = line;
    uint scrolledY = screenY - wy;

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

void PPU::drawSprites() {
    if (!lcdc.objEnabled())
        return;

    for (uint sprite = 0; sprite < NUM_SPRITES_PER_FRAME; sprite++) {
        drawSprite(sprite);
    }
}

void PPU::drawSprite(uint sprite) {
    word offSetInOAM = sprite * SIZEOF_SPRITE_IN_OAM;
    word addressInOAM = OAM_START + offSetInOAM;
    byte spriteX = bus->read(addressInOAM);
    byte spriteY = bus->read(addressInOAM + 1);

    // The maximum dimensions of a tile are 8x16. So the only way to hide a sprite is by putting it
    // outside the drawn area which is upto 16px around the vertical and 8px horizontal
    if (spriteY == 0 || spriteY >= (LCD_HEIGHT + 16))
        return;
    if (spriteX == 0 || spriteX >= (LCD_WIDTH + 8))
        return;

    uint spriteHeight = lcdc.objHeight();

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
    int startY = spriteY - 16;
    int startX = spriteX - 8;

    for (uint y = 0; y < spriteHeight; y++) {
        for (uint x = 0; x < TILE_WIDTH_PX; x++) {
            uint maybeFlippedY = !flipY ? y : spriteHeight - y - 1;
            uint maybeFlippedX = !flipX ? x : TILE_WIDTH_PX - x - 1;

            DisplayColor colorInTileData = spriteTile.pixelValue(maybeFlippedX, maybeFlippedY);

            if (colorInTileData == DisplayColor::White)
                continue;  // White is transparent for sprites

            uint screenX = startX + x;
            uint screenY = startY + y;

            if (!((screenX < LCD_WIDTH) && (screenY < LCD_HEIGHT)))
                continue;

            DisplayColor bgOrWindowPixel = pixelBuffer[screenX + screenY * LCD_WIDTH];

            if (hiddenBehindBG && bgOrWindowPixel != DisplayColor::White)
                continue;

            pixelBuffer[screenX + screenY * LCD_WIDTH] =
                palette.fromID(static_cast<byte>(colorInTileData));
        }
    }
}

SpriteTile::SpriteTile(word startAddress, const std::shared_ptr<Bus>& bus, uint heightOfTile)
    : spriteData(heightOfTile * TILE_WIDTH_PX), heightOfTile{heightOfTile} {
    for (uint tileLine = 0; tileLine < heightOfTile; tileLine++) {
        uint lineByteIndexInTile = tileLine * 2;
        word byteAddress = startAddress + lineByteIndexInTile;

        byte pixel1 = bus->read(byteAddress);
        byte pixel2 = bus->read(byteAddress + 1);

        for (uint x = 0; x < TILE_WIDTH_PX; x++) {
            byte colorCode =
                static_cast<byte>((bitValue(pixel2, 7 - x) << 1) | bitValue(pixel1, 7 - x));
            spriteData[x + tileLine * heightOfTile] = static_cast<DisplayColor>(colorCode);
        }
    }
}
