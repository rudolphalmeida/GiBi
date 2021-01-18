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
               cpu.incR8(cpu.B());
               return 0;
           }},
    Opcode{0x05, "DEC B", 1, 4, false,
           [](CPU& cpu, SPBus&) {
               cpu.decR8(cpu.B());
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
    Opcode{0x08, "LD (u16), SP", 3, 20, false,
           [](CPU& cpu, SPBus& bus) {
               bus->write(cpu.fetchWord(), cpu.SP());
               return 0;
           }},
    Opcode{0x09, "ADD HL, BC", 1, 8, false,
           [](CPU& cpu, SPBus&) {
               cpu.addToHL(cpu.BC());
               return 0;
           }},
    Opcode{0x0A, "LD A, (BC)", 1, 8, false,
           [](CPU& cpu, SPBus& bus) {
               cpu.A() = bus->read(cpu.BC());
               return 0;
           }},
    Opcode{0x0B, "DEC BC", 1, 8, false,
           [](CPU& cpu, SPBus&) {
               cpu.BC(cpu.BC() - 1);
               return 0;
           }},
    Opcode{0x0C, "INC C", 1, 4, false,
           [](CPU& cpu, SPBus&) {
               cpu.incR8(cpu.C());
               return 0;
           }},
    Opcode{0x0D, "DEC C", 1, 4, false,
           [](CPU& cpu, SPBus&) {
               cpu.decR8(cpu.C());
               return 0;
           }},
    Opcode{0x0E, "LD C, u8", 2, 8, false,
           [](CPU& cpu, SPBus&) {
               cpu.C() = cpu.fetchByte();
               return 0;
           }},
    Opcode{0x0F, "RRCA", 1, 4, false,
           [](CPU& cpu, SPBus&) {
               cpu.rrca();
               return 0;
           }},
    Opcode{0x10, "STOP", 2, 4, false,
           [](CPU& cpu, SPBus&) {
               cpu.fetchByte();  // Why is STOP 2 bytes?
               return 0;
           }},
    Opcode{0x11, "LD DE, u16", 3, 12, false,
           [](CPU& cpu, SPBus&) {
               cpu.DE(cpu.fetchWord());
               return 0;
           }},
    Opcode{0x12, "LD (DE), A", 1, 8, false,
           [](CPU& cpu, SPBus& bus) {
               bus->write(cpu.DE(), cpu.A());
               return 0;
           }},
    Opcode{0x13, "INC DE", 1, 8, false,
           [](CPU& cpu, SPBus&) {
               cpu.DE(cpu.DE() + 1);
               return 0;
           }},
    Opcode{0x14, "INC D", 1, 4, false,
           [](CPU& cpu, SPBus&) {
               cpu.incR8(cpu.D());
               return 0;
           }},
    Opcode{0x15, "DEC D", 1, 4, false,
           [](CPU& cpu, SPBus&) {
               cpu.decR8(cpu.D());
               return 0;
           }},
    Opcode{0x16, "LD D, u8", 2, 8, false,
           [](CPU& cpu, SPBus&) {
               cpu.D() = cpu.fetchByte();
               return 0;
           }},
    Opcode{0x17, "RLA", 1, 4, false,
           [](CPU& cpu, SPBus&) {
               cpu.rla();
               return 0;
           }},
    Opcode{0x18, "JR i8", 2, 12, false,
           [](CPU& cpu, SPBus&) {
               cpu.PC() = cpu.PC() + static_cast<sbyte>(cpu.fetchByte());
               return 0;
           }},
    Opcode{0x19, "ADD HL, DE", 1, 8, false,
           [](CPU& cpu, SPBus&) {
               cpu.addToHL(cpu.DE());
               return 0;
           }},
    Opcode{0x1A, "LD A, (DE)", 1, 8, false,
           [](CPU& cpu, SPBus& bus) {
               cpu.A() = bus->read(cpu.DE());
               return 0;
           }},
    Opcode{0x1B, "DEC DE", 1, 8, false,
           [](CPU& cpu, SPBus&) {
               cpu.DE(cpu.DE() - 1);
               return 0;
           }},
    Opcode{0x1C, "INC E", 1, 4, false,
           [](CPU& cpu, SPBus&) {
               cpu.incR8(cpu.E());
               return 0;
           }},
    Opcode{0x1D, "DEC E", 1, 4, false,
           [](CPU& cpu, SPBus&) {
               cpu.decR8(cpu.E());
               return 0;
           }},
    Opcode{0x1E, "LD E, u8", 2, 8, false,
           [](CPU& cpu, SPBus&) {
               cpu.E() = cpu.fetchByte();
               return 0;
           }},
    Opcode{0x1F, "RRA", 1, 4, false, [](CPU& cpu, SPBus&) {
               cpu.rra();
               return 0;
           }},
    Opcode{0x20, "JR NZ, i8", 2, 8, false, [](CPU& cpu, SPBus&) {
               if (!cpu.F().zf) {
                   cpu.PC() = cpu.PC() + static_cast<sbyte>(cpu.fetchByte());
                   return 4;
               } else {
                   cpu.fetchByte();
                   return 0;
               }
           }},
    Opcode{0x21, "LD HL, u16", 3, 12, false, [](CPU& cpu, SPBus&) {
               cpu.HL(cpu.fetchWord());
               return 0;
           }},
    Opcode{0x22, "LD (HL+), A", 1, 8, false, [](CPU& cpu, SPBus& bus) {
               bus->write(cpu.HL(), cpu.A());
               cpu.HL(cpu.HL() + 1);
               return 0;
           }},
    Opcode{0x23, "INC HL", 1, 8, false, [](CPU& cpu, SPBus&) {
               cpu.HL(cpu.HL() + 1);
               return 0;
           }},
    Opcode{0x24, "INC H", 1, 4, false, [](CPU& cpu, SPBus&) {
               cpu.incR8(cpu.H());
               return 0;
           }},
    Opcode{0x25, "DEC H", 1, 4, false, [](CPU& cpu, SPBus&) {
               cpu.decR8(cpu.H());
               return 0;
           }},
    Opcode{0x26, "LD H, u8", 2, 8, false, [](CPU& cpu, SPBus&) {
               cpu.H() = cpu.fetchByte();
               return 0;
           }},
    Opcode{0x27, "DAA", 1, 4, false, [](CPU& cpu, SPBus&) {
               cpu.daa();
               return 0;
           }},
    Opcode{0x28, "JR Z, i8", 2, 8, false, [](CPU& cpu, SPBus&) {
               if (cpu.F().zf) {
                   cpu.PC() = cpu.PC() + static_cast<sbyte>(cpu.fetchByte());
                   return 4;
               } else {
                   cpu.fetchByte();
                   return 0;
               }
           }},
    Opcode{0x29, "ADD HL, HL", 1, 8, false, [](CPU& cpu, SPBus&) {
               cpu.addToHL(cpu.HL());
               return 0;
           }},
    Opcode{0x2A, "LD A, (HL+)", 1, 8, false, [](CPU& cpu, SPBus& bus) {
               cpu.A() = bus->read(cpu.HL());
               cpu.HL(cpu.HL() + 1);
               return 0;
           }},
    Opcode{0x2B, "DEC HL", 1, 8, false, [](CPU& cpu, SPBus&) {
               cpu.HL(cpu.HL() - 1);
               return 0;
           }},
    Opcode{0x2C, "INC L", 1, 4, false, [](CPU& cpu, SPBus&) {
               cpu.incR8(cpu.L());
               return 0;
           }},
    Opcode{0x2D, "DEC L", 1, 4, false, [](CPU& cpu, SPBus&) {
               cpu.decR8(cpu.L());
               return 0;
           }},
    Opcode{0x2E, "LD L, u8", 2, 8, false, [](CPU& cpu, SPBus&) {
               cpu.L() = cpu.fetchByte();
               return 0;
           }},
    Opcode{0x2F, "CPL", 1, 4, false, [](CPU& cpu, SPBus&) {
               cpu.cpl();
               return 0;
           }},
};

#endif  // GIBI_INCLUDE_CPU_INSTRUCTIONS_H_
