/*
 * Opcode implementations for each opcode available to the CPU. Separated into
 * two vectors indexed by the opcode code.
 *
 * Author: Rudolph Almeida <rudolf1.almeida@gmail.com>
 * */

#ifndef GIBI_INCLUDE_CPU_INSTRUCTIONS_H_
#define GIBI_INCLUDE_CPU_INSTRUCTIONS_H_

#include <memory>
#include <vector>

#include "gibi.h"
#include "mmu/bus.h"
#include "opcode.h"

using SPBus = std::shared_ptr<Bus>;

const std::vector<Opcode> nonExtendedOpcodes = {
    Opcode(0x00, "NOP", 1, 4, false, [](CPU&, SPBus&) { return 0; }),
    Opcode{0x01, "LD BC, u16", 3, 12, false,
           [](CPU& cpu, SPBus&) {
               cpu.BC(cpu.fetchWord());
               return 0;
           }},
    Opcode{0x02, "LD (BC), A", 1, 8, false,
           [](CPU& cpu, SPBus& bus) {
               bus->write(cpu.BC(), cpu.A());
               return 0;
           }},
    Opcode{0x03, "INC BC", 1, 8, false,
           [](CPU& cpu, SPBus&) {
               cpu.BC(cpu.BC() + 1);
               return 0;
           }},
    Opcode{0x04, "INC B", 1, 4, false,
           [](CPU& cpu, SPBus&) {
               cpu.B() += 1;
               return 0;
           }},
    Opcode{0x05, "DEC B", 1, 4, false,
           [](CPU& cpu, SPBus&) {
               cpu.B() -= 1;
               return 0;
           }},
    Opcode{0x06, "LD B, u8", 2, 8, false,
           [](CPU& cpu, SPBus&) {
               cpu.B() = cpu.fetchByte();
               return 0;
           }},
    Opcode{0x07, "RLCA", 1, 4, false,
           [](CPU& cpu, SPBus&) {
               cpu.rlca();
               return 0;
           }},
    Opcode{0x08, "LD (u16), SP", 3, 20, false, [](CPU& cpu, SPBus& bus) {
               word address = cpu.fetchWord();
               bus->write(address, cpu.SP());
               return 0;
           }}};

#endif  // GIBI_INCLUDE_CPU_INSTRUCTIONS_H_
