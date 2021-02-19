/*
 * Timings and constants for many different operations in the GameBoy.
 * All timings are clock cycles at 4.19MHz. Dividing by 4 gives us the machine
 * cycles at 1.05MHz
 * Author: Rudolph Almeida <rudolf1.almeida@gmail.com>
 * */

#ifndef GIBI_INCLUDE_TIMINGS_H_
#define GIBI_INCLUDE_TIMINGS_H_

#include "gibi.h"

const uint CYCLES_PER_FRAME = 69905;

/* clang-format off */
const uint NON_CB_CLOCK_CYCLES[] = {
//  0   1   2   3   4   5   6   7   8   9   a   b  c   d   e  f
    4,  12, 8,  8,  4,  4,  8,  4,  20, 8,  8,  8, 4,  4,  8, 4,  // 0
    0,  12, 8,  8,  4,  4,  8,  4,  12, 8,  8,  8, 4,  4,  8, 4,  // 1
    8,  12, 8,  8,  4,  4,  8,  4,  8,  8,  8,  8, 4,  4,  8, 4,  // 2
    8,  12, 8,  8,  12, 12, 12, 4,  8,  8,  8,  8, 4,  4,  8, 4,  // 3
    4,  4,  4,  4,  4,  4,  8,  4,  4,  4,  4,  4, 4,  4,  8, 4,  // 4
    4,  4,  4,  4,  4,  4,  8,  4,  4,  4,  4,  4, 4,  4,  8, 4,  // 5
    4,  4,  4,  4,  4,  4,  8,  4,  4,  4,  4,  4, 4,  4,  8, 4,  // 6
    8,  8,  8,  8,  8,  8,  0,  8,  4,  4,  4,  4, 4,  4,  8, 4,  // 7
    4,  4,  4,  4,  4,  4,  8,  4,  4,  4,  4,  4, 4,  4,  8, 4,  // 8
    4,  4,  4,  4,  4,  4,  8,  4,  4,  4,  4,  4, 4,  4,  8, 4,  // 9
    4,  4,  4,  4,  4,  4,  8,  4,  4,  4,  4,  4, 4,  4,  8, 4,  // a
    4,  4,  4,  4,  4,  4,  8,  4,  4,  4,  4,  4, 4,  4,  8, 4,  // b
    8,  12, 12, 16, 12, 16, 8,  16, 8,  16, 12, 0, 12, 24, 8, 16, // c
    8,  12, 12, 0,  12, 16, 8,  16, 8,  16, 12, 0, 12, 0,  8, 16, // d
    12, 12, 8,  0,  0,  16, 8,  16, 16, 4,  16, 0, 0,  0,  8, 16, // e
    12, 12, 8,  4,  0,  16, 8,  16, 12, 8,  16, 4, 0,  0,  8, 16, // f
};

const uint CB_CLOCK_CYCLES[] = {
//  0  1  2  3  4  5   6  7  8  9  a  b  c  d   e  f
    8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8, // 0
    8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8, // 1
    8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8, // 2
    8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8, // 3
    8, 8, 8, 8, 8, 8, 12, 8, 8, 8, 8, 8, 8, 8, 12, 8, // 4
    8, 8, 8, 8, 8, 8, 12, 8, 8, 8, 8, 8, 8, 8, 12, 8, // 5
    8, 8, 8, 8, 8, 8, 12, 8, 8, 8, 8, 8, 8, 8, 12, 8, // 6
    8, 8, 8, 8, 8, 8, 12, 8, 8, 8, 8, 8, 8, 8, 12, 8, // 7
    8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8, // 8
    8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8, // 9
    8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8, // a
    8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8, // b
    8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8, // c
    8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8, // d
    8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8, // e
    8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8, // f
};
/* clang-format on */

const word VBLANK_HANDLER_ADDRESS = 0x40;
const word LCDSTAT_HANDLER_ADDRESS = 0x48;
const word TIMER_HANDLER_ADDRESS = 0x50;
const word SERIAL_HANDLER_ADDRESS = 0x58;
const word JOYPAD_HANDLER_ADDRESS = 0x60;

const uint ISR_CLOCK_CYCLES = 20;  // TODO: Confirm if this is the right value

const uint LCD_WIDTH = 160;
const uint LCD_HEIGHT = 144;
const uint TOTAL_SCANLINES = 154;
const uint BG_MAP_SIZE = 256;

const uint TILE_WIDTH_PX = 8;
const uint TILE_HEIGHT_PX = 8; // For background and window tiles. Can be 16 for sprites
const uint TILES_PER_LINE = 32;

const uint SIZEOF_TILE = 16; // Each 8x8 tile has 8 lines where each line is 2 bytes

const uint ACCESSING_OAM_CLOCKS = 80;
const uint ACCESSING_VRAM_CLOCKS = 172;
const uint HBLANK_CLOCKS = 204;
const uint CLOCKS_PER_SCANLINE = ACCESSING_OAM_CLOCKS + ACCESSING_VRAM_CLOCKS + HBLANK_CLOCKS;

#endif  // GIBI_INCLUDE_TIMINGS_H_
