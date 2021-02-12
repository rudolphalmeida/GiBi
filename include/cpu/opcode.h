/*
 * Opcodes available for the Sharp LR35902 used in the GameBoy CPU
 * Author: Rudolph Almeida <rudolf1.almeida@gmail.com>
 * */

#ifndef GIBI_INCLUDE_CPU_OPCODE_H_
#define GIBI_INCLUDE_CPU_OPCODE_H_

#include <memory>
#include <string>
#include <utility>
#include <functional>

#include "constants.h"
#include "gibi.h"
#include "mmu/bus.h"

class CPU;

class Opcode {
   private:
    std::function<uint(CPU&, std::shared_ptr<Bus>&)>
        proc;  // The code to be executed for each opcode

   public:
    byte value;
    std::string repr;  // String representation of the opcode
    byte length;       // In bytes
    byte tCycles;      // Base clock cycles taken by the opcode at 4.19MHz
    bool extended;     // Is the opcode 0xCB prefixed?

    Opcode(byte value,
           std::string repr,
           byte length,
           byte tCycles,
           bool extended,
           std::function<uint(CPU&, std::shared_ptr<Bus>&)> proc)
        : proc{std::move(proc)},
          value{value},
          repr{std::move(repr)},
          length{length},
          tCycles{tCycles},
          extended{extended} {}

    // Execute the opcode
    uint operator()(CPU& cpu, std::shared_ptr<Bus> bus) const {
        uint branch_taken_cycles = proc(cpu, bus);

        if (extended) {
            return CB_CLOCK_CYCLES[value];
        } else {
            return NON_CB_CLOCK_CYCLES[value] + branch_taken_cycles;
        }
    }
};

#endif  // GIBI_INCLUDE_CPU_OPCODE_H_
