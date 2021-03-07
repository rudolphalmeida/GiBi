/*
 * Driver class for the emulator. Runs the emulator on a frame-by-frame basis using
 * the CPU timings to drive the other components. Also handles windowing and display.
 *
 * Author: Rudolph Almeida <rudolf1.almeida@gmail.com>
 * */

#ifndef GIBI_INCLUDE_GAMEBOY_H_
#define GIBI_INCLUDE_GAMEBOY_H_

#include <memory>

#include <SDL.h>

#include "cpu/cpu.h"
#include "cpu/interrupts.h"
#include "gibi.h"
#include "mmu/bus.h"
#include "options.h"
#include "ppu/ppu.h"
#include "ui.h"

std::shared_ptr<Options> parseCommandLine(int argc, const char** argv);

class GameBoy {
   private:
    std::shared_ptr<IntF> intf;
    std::shared_ptr<IntE> inte;
    std::shared_ptr<Bus> bus;
    std::shared_ptr<PPU> ppu;
    CPU cpu;

    void initComponents();

    std::shared_ptr<Options> options;

    bool shouldQuit{};

    UI ui;

   public:
    GameBoy(int argc, const char** argv);

    const uint CYCLES_PER_FRAME = 69905;

    // Run the emulator till the user quits (or it crashes)
    int run();

    // The main input-update-render loop. Should run at 60FPS
    void gameLoop();

    // Run one frame worth of clock cycles for each component
    void update();

    // Use the clock cycles used by the CPU to drive the other components
    uint tick();
};

#endif  // GIBI_INCLUDE_GAMEBOY_H_
