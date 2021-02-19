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
#include "cpu/interrupts.h"
#include "gibi.h"
#include "mmu/bus.h"

// Read a binary file from disk
std::vector<byte> readBinaryToVec(const char* filename);

class GameBoy {
   private:
    std::string romPath{};
    std::string savePath{};

    std::shared_ptr<IntF> intf;
    std::shared_ptr<IntE> inte;
    std::shared_ptr<Bus> bus;
    std::shared_ptr<PPU> ppu;
    CPU cpu;

    void initComponents();

   public:
    GameBoy(int argc, char** argv);

    // Run the emulator till the user quits (or it crashes)
    int run();

    // The main input-update-render loop. Should run at 60FPS
    [[noreturn]] void gameLoop();

    // Run one frame worth of clock cycles for each component
    void update();

    // Use the clock cycles used by the CPU to drive the other components
    uint tick();
};

#endif  // GIBI_INCLUDE_GAMEBOY_H_
