/*
 * Driver class for the emulator. Runs the emulator by frame-by-frame basis using
 * the CPU timings to drive the other components. Also handles windowing and display.
 *
 * Author: Rudolph Almeida <rudolf1.almeida@gmail.com>
 * */

#ifndef GIBI_INCLUDE_GAMEBOY_H_
#define GIBI_INCLUDE_GAMEBOY_H_

#include <memory>

#include "cpu/cpu.h"
#include "gibi.h"
#include "mmu/bus.h"

// Read a binary file from disk
std::vector<byte> readBinaryToVec(const char* filename);

class GameBoy {
   private:
    std::string romPath{};
    std::string savePath{};

    std::shared_ptr<Bus> bus;
    CPU cpu;

    void initComponents();

   public:
    GameBoy(int argc, char** argv);
    int run();
};

#endif  // GIBI_INCLUDE_GAMEBOY_H_
