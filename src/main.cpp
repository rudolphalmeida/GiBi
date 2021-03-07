/*
 * GiBi is a GameBoy (Original and GBC) emulator built for fun and learning
 * Author: Rudolph Almeida <rudolf1.almeida@gmail.com>
 * */

#include "gameboy.h"

int main(int argc, const char** argv) {
    GameBoy gameboy(argc, argv);
    return gameboy.run();
}
