/*
 * Opcodes available for the Sharp LR35902 used in the GameBoy CPU
 * Author: Rudolph Almeida <rudolf1.almeida@gmail.com>
 * */

#ifndef GIBI_INCLUDE_CPU_OPCODE_H_
#define GIBI_INCLUDE_CPU_OPCODE_H_

#include <memory>
#include <string>
#include <utility>

#include "cpu.h"
#include "gibi.h"
#include "instr_timings.h"

class Opcode {
   private:
    // TODO: Add the MMU to the template when implementing it
    std::function<uint(CPU&)> proc;  // The code to be executed for each opcode

   public:
    byte value;
    std::string repr;  // String representation of the opcode
    byte length;       // In bytes
    byte tCycles;      // Clock cycles taken by the opcode at 4.19MHz
    bool extended;     // Is the opcode 0xCB prefixed?

    Opcode(byte value,
           std::string repr,
           byte length,
           byte tCycles,
           bool extended,
           std::function<uint(CPU&)> proc)
        : value{value},
          repr{std::move(repr)},
          length{length},
          tCycles{tCycles},
          extended{extended},
          proc{std::move(proc)} {}

    // Execute the opcode
    // TODO: Should pass in the MMU as well
    uint operator()(CPU& cpu) const {
        uint branch_taken_cycles = proc(cpu);

        if (extended) {
            return CB_CLOCK_CYCLES[value];
        } else {
            return NON_CB_CLOCK_CYCLES[value] + branch_taken_cycles;
        }
    }
};

#endif  // GIBI_INCLUDE_CPU_OPCODE_H_
