/*
 * Opcode implementations for each opcode available to the CPU. Separated into
 * two vectors indexed by the opcode code.
 *
 * Author: Rudolph Almeida <rudolf1.almeida@gmail.com>
 * */

#ifndef GIBI_INCLUDE_CPU_INSTRUCTIONS_H_
#define GIBI_INCLUDE_CPU_INSTRUCTIONS_H_

#include <map>

#include "gibi.h"
#include "mmu/bus.h"
#include "opcode.h"

const std::vector<Opcode> nonExtendedOpcodes = {
    Opcode(0x00, "NOP", 1, 4, false, [](CPU& cpu, std::shared_ptr<Bus> bus) { return 0; }),
    Opcode{0x01, "LD BC, u16", 3, 12, false,
           [](CPU& cpu, std::shared_ptr<Bus> bus) {
               cpu.BC(cpu.fetchWord());
               return 0;
           }},
    Opcode{0x02, "LD (BC), A", 1, 8, false,
           [](CPU& cpu, std::shared_ptr<Bus> bus) {
               bus->write(cpu.BC(), cpu.A());
               return 0;
           }},
    Opcode{0x03, "INC BC", 1, 8, false,
           [](CPU& cpu, std::shared_ptr<Bus> bus) {
               cpu.BC(cpu.BC() + 1);
               return 0;
           }},
};

#endif  // GIBI_INCLUDE_CPU_INSTRUCTIONS_H_
