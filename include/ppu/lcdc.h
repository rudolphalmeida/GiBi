/*
 * LCDC (0xFF40) is the main LCD Control register. It's bits are used to control what layers are
 * shown on the screen, from where, and how.
 *
 * Author: Rudolph Almeida <rudolf1.almeida@gmail.com>
 * */

#ifndef GIBI_LCDC_H
#define GIBI_LCDC_H

#include "gibi.h"
#include "mmu/memory.h"

enum class TileMapBase : word { TileMap0 = 0x9800, TileMap1 = 0x9C00 };

enum class TileDataBase : word { TileData0 = 0x8800, TileData1 = 0x8000 };

class LCDC : public Memory {
   private:
    byte data;

   public:
    LCDC() : data{0x91} {}

    // Corresponds to LCDC.7
    [[nodiscard]] bool displayEnabled() const { return isSet(data, 7); }

    // Corresponds to LCDC.6
    [[nodiscard]] TileMapBase windowTileMap() const {
        return isSet(data, 6) ? TileMapBase::TileMap1 : TileMapBase::TileMap0;
    }

    // Corresponds to LCDC.5
    [[nodiscard]] bool windowEnabled() const { return isSet(data, 5); }

    // Corresponds to LCDC.4
    [[nodiscard]] TileDataBase tileData() const {
        return isSet(data, 4) ? TileDataBase::TileData1 : TileDataBase::TileData0;
    }

    // Corresponds to LCDC.3
    [[nodiscard]] TileMapBase bgTileMap() const {
        return isSet(data, 3) ? TileMapBase::TileMap1 : TileMapBase::TileMap0;
    }

    // Corresponds to LCDC.2. Sprites can either be 8x8 or 8x16
    // TODO: Check this function
    [[nodiscard]] uint objHeight() const { return isSet(data, 2) ? 16 : 8; }

    // Corresponds to LCDC.1
    [[nodiscard]] bool objEnabled() const { return isSet(data, 1); }

    // Corresponds to LCDC.0
    [[nodiscard]] bool bgWindowDisplayPriority() const { return isSet(data, 0); }

    [[nodiscard]] byte read(word) const override { return data; }

    void write(word, byte d) override { data = d; }
};

#endif  // GIBI_LCDC_H
