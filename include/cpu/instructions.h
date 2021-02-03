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

std::vector<Opcode> nonExtendedOpcodes() {
    std::vector<Opcode> ops;
    ops.reserve(256);

    ops.emplace_back(0x00, "NOP", 1, 4, false, [](CPU&, SPBus&) { return 0; });
    ops.emplace_back(0x01, "LD BC, u16", 3, 12, false, [](CPU& cpu, SPBus&) {
        cpu.BC(cpu.fetchWord());
        return 0;
    });
    ops.emplace_back(0x02, "LD (BC), A", 1, 8, false, [](CPU& cpu, SPBus& bus) {
        bus->write(cpu.BC(), cpu.A());
        return 0;
    });
    ops.emplace_back(0x03, "INC BC", 1, 8, false, [](CPU& cpu, SPBus&) {
        cpu.BC(cpu.BC() + 1);
        return 0;
    });
    ops.emplace_back(0x04, "INC B", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.incR8(cpu.B());
        return 0;
    });
    ops.emplace_back(0x05, "DEC B", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.decR8(cpu.B());
        return 0;
    });
    ops.emplace_back(0x06, "LD B, u8", 2, 8, false, [](CPU& cpu, SPBus&) {
        cpu.B() = cpu.fetchByte();
        return 0;
    });
    ops.emplace_back(0x07, "RLCA", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.rlca();
        return 0;
    });
    ops.emplace_back(0x08, "LD (u16), SP", 3, 20, false, [](CPU& cpu, SPBus& bus) {
        auto [upper, lower] = decomposeWord(cpu.SP());
        word address = cpu.fetchWord();

        bus->write(address, lower);
        bus->write(address + 1, upper);
        return 0;
    });
    ops.emplace_back(0x09, "ADD HL, BC", 1, 8, false, [](CPU& cpu, SPBus&) {
        cpu.addToHL(cpu.BC());
        return 0;
    });
    ops.emplace_back(0x0A, "LD A, (BC)", 1, 8, false, [](CPU& cpu, SPBus& bus) {
        cpu.A() = bus->read(cpu.BC());
        return 0;
    });
    ops.emplace_back(0x0B, "DEC BC", 1, 8, false, [](CPU& cpu, SPBus&) {
        cpu.BC(cpu.BC() - 1);
        return 0;
    });
    ops.emplace_back(0x0C, "INC C", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.incR8(cpu.C());
        return 0;
    });
    ops.emplace_back(0x0D, "DEC C", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.decR8(cpu.C());
        return 0;
    });
    ops.emplace_back(0x0E, "LD C, u8", 2, 8, false, [](CPU& cpu, SPBus&) {
        cpu.C() = cpu.fetchByte();
        return 0;
    });
    ops.emplace_back(0x0F, "RRCA", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.rrca();
        return 0;
    });
    ops.emplace_back(0x10, "STOP", 2, 4, false, [](CPU& cpu, SPBus&) {
        cpu.state = CPUState::HALTED;
        cpu.fetchByte();  // Why is STOP 2 bytes?
        return 0;
    });
    ops.emplace_back(0x11, "LD DE, u16", 3, 12, false, [](CPU& cpu, SPBus&) {
        cpu.DE(cpu.fetchWord());
        return 0;
    });
    ops.emplace_back(0x12, "LD (DE), A", 1, 8, false, [](CPU& cpu, SPBus& bus) {
        bus->write(cpu.DE(), cpu.A());
        return 0;
    });
    ops.emplace_back(0x13, "INC DE", 1, 8, false, [](CPU& cpu, SPBus&) {
        cpu.DE(cpu.DE() + 1);
        return 0;
    });
    ops.emplace_back(0x14, "INC D", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.incR8(cpu.D());
        return 0;
    });
    ops.emplace_back(0x15, "DEC D", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.decR8(cpu.D());
        return 0;
    });
    ops.emplace_back(0x16, "LD D, u8", 2, 8, false, [](CPU& cpu, SPBus&) {
        cpu.D() = cpu.fetchByte();
        return 0;
    });
    ops.emplace_back(0x17, "RLA", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.rla();
        return 0;
    });
    ops.emplace_back(0x18, "JR i8", 2, 12, false, [](CPU& cpu, SPBus&) {
        cpu.PC() = cpu.PC() + static_cast<sbyte>(cpu.fetchByte());
        return 0;
    });
    ops.emplace_back(0x19, "ADD HL, DE", 1, 8, false, [](CPU& cpu, SPBus&) {
        cpu.addToHL(cpu.DE());
        return 0;
    });
    ops.emplace_back(0x1A, "LD A, (DE)", 1, 8, false, [](CPU& cpu, SPBus& bus) {
        cpu.A() = bus->read(cpu.DE());
        return 0;
    });
    ops.emplace_back(0x1B, "DEC DE", 1, 8, false, [](CPU& cpu, SPBus&) {
        cpu.DE(cpu.DE() - 1);
        return 0;
    });
    ops.emplace_back(0x1C, "INC E", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.incR8(cpu.E());
        return 0;
    });
    ops.emplace_back(0x1D, "DEC E", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.decR8(cpu.E());
        return 0;
    });
    ops.emplace_back(0x1E, "LD E, u8", 2, 8, false, [](CPU& cpu, SPBus&) {
        cpu.E() = cpu.fetchByte();
        return 0;
    });
    ops.emplace_back(0x1F, "RRA", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.rra();
        return 0;
    });
    ops.emplace_back(0x20, "JR NZ, i8", 2, 8, false, [](CPU& cpu, SPBus&) {
        if (!cpu.F().zf) {
            cpu.PC() = cpu.PC() + static_cast<sbyte>(cpu.fetchByte());
            return 4;
        } else {
            cpu.fetchByte();
            return 0;
        }
    });
    ops.emplace_back(0x21, "LD HL, u16", 3, 12, false, [](CPU& cpu, SPBus&) {
        cpu.HL(cpu.fetchWord());
        return 0;
    });
    ops.emplace_back(0x22, "LD (HL+), A", 1, 8, false, [](CPU& cpu, SPBus& bus) {
        bus->write(cpu.HL(), cpu.A());
        cpu.HL(cpu.HL() + 1);
        return 0;
    });
    ops.emplace_back(0x23, "INC HL", 1, 8, false, [](CPU& cpu, SPBus&) {
        cpu.HL(cpu.HL() + 1);
        return 0;
    });
    ops.emplace_back(0x24, "INC H", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.incR8(cpu.H());
        return 0;
    });
    ops.emplace_back(0x25, "DEC H", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.decR8(cpu.H());
        return 0;
    });
    ops.emplace_back(0x26, "LD H, u8", 2, 8, false, [](CPU& cpu, SPBus&) {
        cpu.H() = cpu.fetchByte();
        return 0;
    });
    ops.emplace_back(0x27, "DAA", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.daa();
        return 0;
    });
    ops.emplace_back(0x28, "JR Z, i8", 2, 8, false, [](CPU& cpu, SPBus&) {
        if (cpu.F().zf) {
            cpu.PC() = cpu.PC() + static_cast<sbyte>(cpu.fetchByte());
            return 4;
        } else {
            cpu.fetchByte();
            return 0;
        }
    });
    ops.emplace_back(0x29, "ADD HL, HL", 1, 8, false, [](CPU& cpu, SPBus&) {
        cpu.addToHL(cpu.HL());
        return 0;
    });
    ops.emplace_back(0x2A, "LD A, (HL+)", 1, 8, false, [](CPU& cpu, SPBus& bus) {
        cpu.A() = bus->read(cpu.HL());
        cpu.HL(cpu.HL() + 1);
        return 0;
    });
    ops.emplace_back(0x2B, "DEC HL", 1, 8, false, [](CPU& cpu, SPBus&) {
        cpu.HL(cpu.HL() - 1);
        return 0;
    });
    ops.emplace_back(0x2C, "INC L", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.incR8(cpu.L());
        return 0;
    });
    ops.emplace_back(0x2D, "DEC L", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.decR8(cpu.L());
        return 0;
    });
    ops.emplace_back(0x2E, "LD L, u8", 2, 8, false, [](CPU& cpu, SPBus&) {
        cpu.L() = cpu.fetchByte();
        return 0;
    });
    ops.emplace_back(0x2F, "CPL", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.cpl();
        return 0;
    });
    ops.emplace_back(0x30, "JR NC, i8", 2, 8, false, [](CPU& cpu, SPBus&) {
        if (!cpu.F().cy) {
            cpu.PC() = cpu.PC() + static_cast<sbyte>(cpu.fetchByte());
            return 4;
        } else {
            cpu.fetchByte();
            return 0;
        }
    });
    ops.emplace_back(0x31, "LD SP, u16", 3, 12, false, [](CPU& cpu, SPBus&) {
        cpu.SP() = cpu.fetchWord();
        return 0;
    });
    ops.emplace_back(0x32, "LD (HL-), A", 1, 8, false, [](CPU& cpu, SPBus& bus) {
        bus->write(cpu.HL(), cpu.A());
        cpu.HL(cpu.HL() - 1);
        return 0;
    });
    ops.emplace_back(0x33, "INC SP", 1, 8, false, [](CPU& cpu, SPBus&) {
        cpu.SP() = cpu.SP() + 1;
        return 0;
    });
    ops.emplace_back(0x34, "INC (HL)", 1, 12, false, [](CPU& cpu, SPBus& bus) {
        byte value = bus->read(cpu.HL());
        cpu.incR8(value);
        bus->write(cpu.HL(), value);
        return 0;
    });
    ops.emplace_back(0x35, "DEC (HL)", 1, 12, false, [](CPU& cpu, SPBus& bus) {
        byte value = bus->read(cpu.HL());
        cpu.decR8(value);
        bus->write(cpu.HL(), value);
        return 0;
    });
    ops.emplace_back(0x36, "LD (HL), u8", 2, 12, false, [](CPU& cpu, SPBus& bus) {
        bus->write(cpu.HL(), cpu.fetchByte());
        return 0;
    });
    ops.emplace_back(0x37, "SCF", 1, 4, false, [](CPU& cpu, SPBus&) {
        auto& F = cpu.F();
        F.cy = true;
        F.n = false;
        F.h = false;
        return 0;
    });
    ops.emplace_back(0x38, "JR C, i8", 2, 8, false, [](CPU& cpu, SPBus&) {
        if (cpu.F().cy) {
            cpu.PC() = cpu.PC() + static_cast<sbyte>(cpu.fetchByte());
            return 4;
        } else {
            cpu.fetchByte();
            return 0;
        }
    });
    ops.emplace_back(0x39, "ADD HL, SP", 1, 8, false, [](CPU& cpu, SPBus&) {
        cpu.addToHL(cpu.SP());
        return 0;
    });
    ops.emplace_back(0x3A, "LD A, (HL-)", 1, 8, false, [](CPU& cpu, SPBus& bus) {
        cpu.A() = bus->read(cpu.HL());
        cpu.HL(cpu.HL() - 1);
        return 0;
    });
    ops.emplace_back(0x3B, "DEC SP", 1, 8, false, [](CPU& cpu, SPBus&) {
        cpu.SP() = cpu.SP() - 1;
        return 0;
    });
    ops.emplace_back(0x3C, "INC A", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.incR8(cpu.A());
        return 0;
    });
    ops.emplace_back(0x3D, "DEC A", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.decR8(cpu.A());
        return 0;
    });
    ops.emplace_back(0x3E, "LD A, u8", 1, 8, false, [](CPU& cpu, SPBus&) {
        cpu.A() = cpu.fetchByte();
        return 0;
    });
    ops.emplace_back(0x3F, "CCF", 1, 4, false, [](CPU& cpu, SPBus&) {
        auto& F = cpu.F();
        F.cy = !F.cy;
        F.n = false;
        F.h = false;
        return 0;
    });
    ops.emplace_back(0x40, "LD B, B", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.B() = cpu.B();
        return 0;
    });
    ops.emplace_back(0x41, "LD B, C", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.B() = cpu.C();
        return 0;
    });
    ops.emplace_back(0x42, "LD B, D", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.B() = cpu.D();
        return 0;
    });
    ops.emplace_back(0x43, "LD B, E", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.B() = cpu.E();
        return 0;
    });
    ops.emplace_back(0x44, "LD B, H", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.B() = cpu.H();
        return 0;
    });
    ops.emplace_back(0x45, "LD B, L", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.B() = cpu.L();
        return 0;
    });
    ops.emplace_back(0x46, "LD B, (HL)", 1, 8, false, [](CPU& cpu, SPBus& bus) {
        cpu.B() = bus->read(cpu.HL());
        return 0;
    });
    ops.emplace_back(0x47, "LD B, A", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.B() = cpu.A();
        return 0;
    });
    ops.emplace_back(0x48, "LD C, B", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.C() = cpu.B();
        return 0;
    });
    ops.emplace_back(0x49, "LD C, C", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.C() = cpu.C();
        return 0;
    });
    ops.emplace_back(0x4A, "LD C, D", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.C() = cpu.D();
        return 0;
    });
    ops.emplace_back(0x4B, "LD C, E", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.C() = cpu.E();
        return 0;
    });
    ops.emplace_back(0x4C, "LD C, H", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.C() = cpu.H();
        return 0;
    });
    ops.emplace_back(0x4D, "LD C, L", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.C() = cpu.L();
        return 0;
    });
    ops.emplace_back(0x4E, "LD C, (HL)", 1, 8, false, [](CPU& cpu, SPBus& bus) {
        cpu.C() = bus->read(cpu.HL());
        return 0;
    });
    ops.emplace_back(0x4F, "LD C, A", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.C() = cpu.A();
        return 0;
    });
    ops.emplace_back(0x50, "LD D, B", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.D() = cpu.B();
        return 0;
    });
    ops.emplace_back(0x51, "LD D, C", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.D() = cpu.C();
        return 0;
    });
    ops.emplace_back(0x52, "LD D, D", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.D() = cpu.D();
        return 0;
    });
    ops.emplace_back(0x53, "LD D, E", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.D() = cpu.E();
        return 0;
    });
    ops.emplace_back(0x54, "LD D, H", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.D() = cpu.H();
        return 0;
    });
    ops.emplace_back(0x55, "LD D, L", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.D() = cpu.L();
        return 0;
    });
    ops.emplace_back(0x56, "LD D, (HL)", 1, 8, false, [](CPU& cpu, SPBus& bus) {
        cpu.D() = bus->read(cpu.HL());
        return 0;
    });
    ops.emplace_back(0x57, "LD D, A", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.D() = cpu.A();
        return 0;
    });
    ops.emplace_back(0x58, "LD E, B", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.E() = cpu.B();
        return 0;
    });
    ops.emplace_back(0x59, "LD E, C", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.E() = cpu.C();
        return 0;
    });
    ops.emplace_back(0x5A, "LD E, D", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.E() = cpu.D();
        return 0;
    });
    ops.emplace_back(0x5B, "LD E, E", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.E() = cpu.E();
        return 0;
    });
    ops.emplace_back(0x5C, "LD E, H", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.E() = cpu.H();
        return 0;
    });
    ops.emplace_back(0x5D, "LD E, L", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.E() = cpu.L();
        return 0;
    });
    ops.emplace_back(0x5E, "LD E, (HL)", 1, 8, false, [](CPU& cpu, SPBus& bus) {
        cpu.E() = bus->read(cpu.HL());
        return 0;
    });
    ops.emplace_back(0x5F, "LD E, A", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.E() = cpu.A();
        return 0;
    });
    ops.emplace_back(0x60, "LD H, B", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.H() = cpu.B();
        return 0;
    });
    ops.emplace_back(0x61, "LD H, C", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.H() = cpu.C();
        return 0;
    });
    ops.emplace_back(0x62, "LD H, D", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.H() = cpu.D();
        return 0;
    });
    ops.emplace_back(0x63, "LD H, E", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.H() = cpu.E();
        return 0;
    });
    ops.emplace_back(0x64, "LD H, H", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.H() = cpu.H();
        return 0;
    });
    ops.emplace_back(0x65, "LD H, L", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.H() = cpu.L();
        return 0;
    });
    ops.emplace_back(0x66, "LD H, (HL)", 1, 8, false, [](CPU& cpu, SPBus& bus) {
        cpu.H() = bus->read(cpu.HL());
        return 0;
    });
    ops.emplace_back(0x67, "LD H, A", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.H() = cpu.A();
        return 0;
    });
    ops.emplace_back(0x68, "LD L, B", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.L() = cpu.B();
        return 0;
    });
    ops.emplace_back(0x69, "LD L, C", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.L() = cpu.C();
        return 0;
    });
    ops.emplace_back(0x6A, "LD L, D", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.L() = cpu.D();
        return 0;
    });
    ops.emplace_back(0x6B, "LD L, E", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.L() = cpu.E();
        return 0;
    });
    ops.emplace_back(0x6C, "LD L, H", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.L() = cpu.H();
        return 0;
    });
    ops.emplace_back(0x6D, "LD L, L", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.L() = cpu.L();
        return 0;
    });
    ops.emplace_back(0x6E, "LD L, (HL)", 1, 8, false, [](CPU& cpu, SPBus& bus) {
        cpu.L() = bus->read(cpu.HL());
        return 0;
    });
    ops.emplace_back(0x6F, "LD L, A", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.L() = cpu.A();
        return 0;
    });
    ops.emplace_back(0x70, "LD (HL), B", 1, 8, false, [](CPU& cpu, SPBus& bus) {
        bus->write(cpu.HL(), cpu.B());
        return 0;
    });
    ops.emplace_back(0x71, "LD (HL), C", 1, 8, false, [](CPU& cpu, SPBus& bus) {
        bus->write(cpu.HL(), cpu.C());
        return 0;
    });
    ops.emplace_back(0x72, "LD (HL), D", 1, 8, false, [](CPU& cpu, SPBus& bus) {
        bus->write(cpu.HL(), cpu.D());
        return 0;
    });
    ops.emplace_back(0x73, "LD (HL), E", 1, 8, false, [](CPU& cpu, SPBus& bus) {
        bus->write(cpu.HL(), cpu.E());
        return 0;
    });
    ops.emplace_back(0x74, "LD (HL), H", 1, 8, false, [](CPU& cpu, SPBus& bus) {
        bus->write(cpu.HL(), cpu.H());
        return 0;
    });
    ops.emplace_back(0x75, "LD (HL), L", 1, 8, false, [](CPU& cpu, SPBus& bus) {
        bus->write(cpu.HL(), cpu.L());
        return 0;
    });
    ops.emplace_back(0x76, "HALT", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.state = CPUState::HALTED;
        return 0;
    });
    ops.emplace_back(0x77, "LD (HL), A", 1, 8, false, [](CPU& cpu, SPBus& bus) {
        bus->write(cpu.HL(), cpu.A());
        return 0;
    });
    ops.emplace_back(0x78, "LD A, B", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.A() = cpu.B();
        return 0;
    });
    ops.emplace_back(0x79, "LD A, C", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.A() = cpu.C();
        return 0;
    });
    ops.emplace_back(0x7A, "LD A, D", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.A() = cpu.D();
        return 0;
    });
    ops.emplace_back(0x7B, "LD A, E", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.A() = cpu.E();
        return 0;
    });
    ops.emplace_back(0x7C, "LD A, H", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.A() = cpu.H();
        return 0;
    });
    ops.emplace_back(0x7D, "LD A, L", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.A() = cpu.L();
        return 0;
    });
    ops.emplace_back(0x7E, "LD A, (HL)", 1, 8, false, [](CPU& cpu, SPBus& bus) {
        cpu.A() = bus->read(cpu.HL());
        return 0;
    });
    ops.push_back(Opcode(0x7F, "LD A, A", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.A() = cpu.A();
        return 0;
    }));
    ops.emplace_back(0x80, "ADD A, B", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.addR8(cpu.B());
        return 0;
    });
    ops.emplace_back(0x81, "ADD A, C", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.addR8(cpu.C());
        return 0;
    });
    ops.emplace_back(0x82, "ADD A, D", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.addR8(cpu.D());
        return 0;
    });
    ops.emplace_back(0x83, "ADD A, E", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.addR8(cpu.E());
        return 0;
    });
    ops.emplace_back(0x84, "ADD A, H", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.addR8(cpu.H());
        return 0;
    });
    ops.emplace_back(0x85, "ADD A, L", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.addR8(cpu.L());
        return 0;
    });
    ops.emplace_back(0x86, "ADD A, [HL]", 1, 8, false, [](CPU& cpu, SPBus& bus) {
        cpu.addR8(bus->read(cpu.HL()));
        return 0;
    });
    ops.emplace_back(0x87, "ADD A, A", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.addR8(cpu.A());
        return 0;
    });
    ops.emplace_back(0x88, "ADC A, B", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.adcR8(cpu.B());
        return 0;
    });
    ops.emplace_back(0x89, "ADC A, C", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.adcR8(cpu.C());
        return 0;
    });
    ops.emplace_back(0x8A, "ADC A, D", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.adcR8(cpu.D());
        return 0;
    });
    ops.emplace_back(0x8B, "ADC A, E", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.adcR8(cpu.E());
        return 0;
    });
    ops.emplace_back(0x8C, "ADC A, H", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.adcR8(cpu.H());
        return 0;
    });
    ops.emplace_back(0x8D, "ADC A, L", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.adcR8(cpu.L());
        return 0;
    });
    ops.emplace_back(0x8E, "ADC A, [HL]", 1, 8, false, [](CPU& cpu, SPBus& bus) {
        cpu.adcR8(bus->read(cpu.HL()));
        return 0;
    });
    ops.emplace_back(0x8F, "ADC A, A", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.adcR8(cpu.A());
        return 0;
    });
    ops.emplace_back(0x90, "SUB A, B", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.subR8(cpu.B());
        return 0;
    });
    ops.emplace_back(0x91, "SUB A, C", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.subR8(cpu.C());
        return 0;
    });
    ops.emplace_back(0x92, "SUB A, D", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.subR8(cpu.D());
        return 0;
    });
    ops.emplace_back(0x93, "SUB A, E", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.subR8(cpu.E());
        return 0;
    });
    ops.emplace_back(0x94, "SUB A, H", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.subR8(cpu.H());
        return 0;
    });
    ops.emplace_back(0x95, "SUB A, L", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.subR8(cpu.L());
        return 0;
    });
    ops.emplace_back(0x96, "SUB A, [HL]", 1, 8, false, [](CPU& cpu, SPBus& bus) {
        cpu.subR8(bus->read(cpu.HL()));
        return 0;
    });
    ops.emplace_back(0x97, "SUB A, A", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.subR8(cpu.A());
        return 0;
    });
    ops.emplace_back(0x98, "SBC A, B", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.sbcR8(cpu.B());
        return 0;
    });
    ops.emplace_back(0x99, "SBC A, C", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.sbcR8(cpu.C());
        return 0;
    });
    ops.emplace_back(0x9A, "SBC A, D", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.sbcR8(cpu.D());
        return 0;
    });
    ops.emplace_back(0x9B, "SBC A, E", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.sbcR8(cpu.E());
        return 0;
    });
    ops.emplace_back(0x9C, "SBC A, H", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.sbcR8(cpu.H());
        return 0;
    });
    ops.emplace_back(0x9D, "SBC A, L", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.sbcR8(cpu.L());
        return 0;
    });
    ops.emplace_back(0x9E, "SBC A, [HL]", 1, 8, false, [](CPU& cpu, SPBus& bus) {
        cpu.sbcR8(bus->read(cpu.HL()));
        return 0;
    });
    ops.emplace_back(0x9F, "SBC A, A", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.sbcR8(cpu.A());
        return 0;
    });
    ops.emplace_back(0xA0, "AND A, B", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.andR8(cpu.B());
        return 0;
    });
    ops.emplace_back(0xA1, "AND A, C", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.andR8(cpu.C());
        return 0;
    });
    ops.emplace_back(0xA2, "AND A, D", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.andR8(cpu.D());
        return 0;
    });
    ops.emplace_back(0xA3, "AND A, E", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.andR8(cpu.E());
        return 0;
    });
    ops.emplace_back(0xA4, "AND A, H", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.andR8(cpu.H());
        return 0;
    });
    ops.emplace_back(0xA5, "AND A, L", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.andR8(cpu.L());
        return 0;
    });
    ops.emplace_back(0xA6, "AND A, [HL]", 1, 8, false, [](CPU& cpu, SPBus& bus) {
        cpu.andR8(bus->read(cpu.HL()));
        return 0;
    });
    ops.emplace_back(0xA7, "AND A, A", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.andR8(cpu.A());
        return 0;
    });
    ops.emplace_back(0xA8, "XOR A, B", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.xorR8(cpu.B());
        return 0;
    });
    ops.emplace_back(0xA9, "XOR A, C", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.xorR8(cpu.C());
        return 0;
    });
    ops.emplace_back(0xAA, "XOR A, D", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.xorR8(cpu.D());
        return 0;
    });
    ops.emplace_back(0xAB, "XOR A, E", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.xorR8(cpu.E());
        return 0;
    });
    ops.emplace_back(0xAC, "XOR A, H", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.xorR8(cpu.H());
        return 0;
    });
    ops.emplace_back(0xAD, "XOR A, L", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.xorR8(cpu.L());
        return 0;
    });
    ops.emplace_back(0xAE, "XOR A, [HL]", 1, 8, false, [](CPU& cpu, SPBus& bus) {
        cpu.xorR8(bus->read(cpu.HL()));
        return 0;
    });
    ops.emplace_back(0xAF, "XOR A, A", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.xorR8(cpu.A());
        return 0;
    });
    ops.emplace_back(0xB0, "OR A, B", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.orR8(cpu.B());
        return 0;
    });
    ops.emplace_back(0xB1, "OR A, C", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.orR8(cpu.C());
        return 0;
    });
    ops.emplace_back(0xB2, "OR A, D", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.orR8(cpu.D());
        return 0;
    });
    ops.emplace_back(0xB3, "OR A, E", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.orR8(cpu.E());
        return 0;
    });
    ops.emplace_back(0xB4, "OR A, H", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.orR8(cpu.H());
        return 0;
    });
    ops.emplace_back(0xB5, "OR A, L", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.orR8(cpu.L());
        return 0;
    });
    ops.emplace_back(0xB6, "OR A, [HL]", 1, 8, false, [](CPU& cpu, SPBus& bus) {
        cpu.orR8(bus->read(cpu.HL()));
        return 0;
    });
    ops.emplace_back(0xB7, "OR A, A", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.orR8(cpu.A());
        return 0;
    });
    ops.emplace_back(0xB8, "CP A, B", 1, 4, false, [](CPU& cpu, SPBus&) {
        byte value = cpu.A();
        cpu.subR8(cpu.B());
        cpu.A() = value;
        return 0;
    });
    ops.emplace_back(0xB9, "CP A, C", 1, 4, false, [](CPU& cpu, SPBus&) {
        byte value = cpu.A();
        cpu.subR8(cpu.C());
        cpu.A() = value;
        return 0;
    });
    ops.emplace_back(0xBA, "CP A, D", 1, 4, false, [](CPU& cpu, SPBus&) {
        byte value = cpu.A();
        cpu.subR8(cpu.D());
        cpu.A() = value;
        return 0;
    });
    ops.emplace_back(0xBB, "CP A, E", 1, 4, false, [](CPU& cpu, SPBus&) {
        byte value = cpu.A();
        cpu.subR8(cpu.E());
        cpu.A() = value;
        return 0;
    });
    ops.emplace_back(0xBC, "CP A, H", 1, 4, false, [](CPU& cpu, SPBus&) {
        byte value = cpu.A();
        cpu.subR8(cpu.H());
        cpu.A() = value;
        return 0;
    });
    ops.emplace_back(0xBD, "CP A, L", 1, 4, false, [](CPU& cpu, SPBus&) {
        byte value = cpu.A();
        cpu.subR8(cpu.L());
        cpu.A() = value;
        return 0;
    });
    ops.emplace_back(0xBE, "CP A, [HL]", 1, 8, false, [](CPU& cpu, SPBus& bus) {
        byte value = cpu.A();
        cpu.subR8(bus->read(cpu.HL()));
        cpu.A() = value;
        return 0;
    });
    ops.emplace_back(0xBF, "CP, A, A", 1, 4, false, [](CPU& cpu, SPBus&) {
        byte value = cpu.A();
        cpu.subR8(cpu.A());
        cpu.A() = value;
        return 0;
    });
    ops.emplace_back(0xC0, "RET NZ", 1, 8, false, [](CPU& cpu, SPBus&) {
        if (!cpu.F().zf) {
            cpu.PC() = cpu.pop();
            return 12;
        } else {
            return 0;
        }
    });
    ops.emplace_back(0xC1, "POP BC", 1, 12, false, [](CPU& cpu, SPBus&) {
        cpu.BC(cpu.pop());
        return 0;
    });
    ops.emplace_back(0xC2, "JP NZ, u16", 3, 12, false, [](CPU& cpu, SPBus&) {
        if (!cpu.F().zf) {
            cpu.PC() = cpu.fetchWord();
            return 4;
        } else {
            cpu.fetchWord();
            return 0;
        }
    });
    ops.emplace_back(0xC3, "JP u16", 3, 16, false, [](CPU& cpu, SPBus&) {
        cpu.PC() = cpu.fetchWord();
        return 0;
    });
    ops.emplace_back(0xC4, "CALL NZ, u16", 3, 12, false, [](CPU& cpu, SPBus&) {
        if (!cpu.F().zf) {
            word jumpAddress = cpu.fetchWord();
            cpu.push(cpu.PC());
            cpu.PC() = jumpAddress;
            return 12;
        } else {
            cpu.fetchWord();
            return 0;
        }
    });
    ops.emplace_back(0xC5, "PUSH BC", 1, 16, false, [](CPU& cpu, SPBus&) {
        cpu.push(cpu.BC());
        return 0;
    });
    ops.emplace_back(0xC6, "ADD A, u8", 2, 8, false, [](CPU& cpu, SPBus&) {
        cpu.addR8(cpu.fetchByte());
        return 0;
    });
    ops.emplace_back(0xC7, "RST 00h", 1, 16, false, [](CPU& cpu, SPBus&) {
        cpu.push(cpu.PC());
        cpu.PC() = 0x00;
        return 0;
    });
    ops.emplace_back(0xC8, "RET Z", 1, 8, false, [](CPU& cpu, SPBus&) {
        if (cpu.F().zf) {
            cpu.PC() = cpu.pop();
            return 12;
        } else {
            return 0;
        }
    });
    ops.emplace_back(0xC9, "RET", 1, 16, false, [](CPU& cpu, SPBus&) {
        cpu.PC() = cpu.pop();
        return 0;
    });
    ops.emplace_back(0xCA, "JP Z, u16", 3, 12, false, [](CPU& cpu, SPBus&) {
        if (cpu.F().zf) {
            cpu.PC() = cpu.fetchWord();
            return 4;
        } else {
            cpu.fetchWord();
            return 0;
        }
    });
    ops.emplace_back(0xCB, "CB", 1, 4, false,
                     [](CPU& cpu, SPBus&) { return cpu.executeExtended(); });
    ops.emplace_back(0xCC, "CALL Z, u16", 3, 12, false, [](CPU& cpu, SPBus&) {
        if (cpu.F().zf) {
            word jumpAddress = cpu.fetchWord();
            cpu.push(cpu.PC());
            cpu.PC() = jumpAddress;
            return 12;
        } else {
            cpu.fetchWord();
            return 0;
        }
    });
    ops.emplace_back(0xCD, "CALL u16", 3, 24, false, [](CPU& cpu, SPBus&) {
        word jumpAddress = cpu.fetchWord();
        cpu.push(cpu.PC());
        cpu.PC() = jumpAddress;
        return 0;
    });
    ops.emplace_back(0xCE, "ADC A, u8", 2, 8, false, [](CPU& cpu, SPBus&) {
        cpu.adcR8(cpu.fetchByte());
        return 0;
    });
    ops.emplace_back(0xCF, "RST 08h", 1, 16, false, [](CPU& cpu, SPBus&) {
        cpu.push(cpu.PC());
        cpu.PC() = 0x08;
        return 0;
    });

    return ops;
}

#endif  // GIBI_INCLUDE_CPU_INSTRUCTIONS_H_
