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

std::vector<Opcode> opcodeImpl() {
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
    ops.emplace_back(0x7F, "LD A, A", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.A() = cpu.A();
        return 0;
    });
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
    ops.emplace_back(0xBF, "CP A, A", 1, 4, false, [](CPU& cpu, SPBus&) {
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
    ops.emplace_back(0xD0, "RET NC", 1, 8, false, [](CPU& cpu, SPBus&) {
        if (!cpu.F().cy) {
            cpu.PC() = cpu.pop();
            return 12;
        } else {
            return 0;
        }
    });
    ops.emplace_back(0xD1, "POP DE", 1, 12, false, [](CPU& cpu, SPBus&) {
        cpu.DE(cpu.pop());
        return 0;
    });
    ops.emplace_back(0xD2, "JP NC, u16", 3, 12, false, [](CPU& cpu, SPBus&) {
        if (!cpu.F().cy) {
            cpu.PC() = cpu.fetchWord();
            return 4;
        } else {
            cpu.fetchWord();
            return 0;
        }
    });
    ops.emplace_back(0xD3, "NOP", 1, 4, false, [](CPU&, SPBus&) { return 0; });
    ops.emplace_back(0xD4, "CALL NC, u16", 3, 12, false, [](CPU& cpu, SPBus&) {
        if (!cpu.F().cy) {
            word jumpAddress = cpu.fetchWord();
            cpu.push(cpu.PC());
            cpu.PC() = jumpAddress;
            return 12;
        } else {
            cpu.fetchWord();
            return 0;
        }
    });
    ops.emplace_back(0xD5, "PUSH DE", 1, 16, false, [](CPU& cpu, SPBus&) {
        cpu.push(cpu.DE());
        return 0;
    });
    ops.emplace_back(0xD6, "SUB A, u8", 2, 8, false, [](CPU& cpu, SPBus&) {
        cpu.subR8(cpu.fetchByte());
        return 0;
    });
    ops.emplace_back(0xD7, "RST 10h", 1, 16, false, [](CPU& cpu, SPBus&) {
        cpu.push(cpu.PC());
        cpu.PC() = 0x10;
        return 0;
    });
    ops.emplace_back(0xD8, "RET C", 1, 8, false, [](CPU& cpu, SPBus&) {
        if (cpu.F().cy) {
            cpu.PC() = cpu.pop();
            return 12;
        } else {
            return 0;
        }
    });
    ops.emplace_back(0xD9, "RETI", 1, 16, false, [](CPU& cpu, SPBus&) {
        cpu.IME() = true;
        cpu.PC() = cpu.pop();
        return 0;
    });
    ops.emplace_back(0xDA, "JP C, u16", 3, 12, false, [](CPU& cpu, SPBus&) {
        if (cpu.F().cy) {
            cpu.PC() = cpu.fetchWord();
            return 12;
        } else {
            return 0;
        }
    });
    ops.emplace_back(0xDB, "NOP", 1, 4, false, [](CPU&, SPBus&) { return 0; });
    ops.emplace_back(0xDC, "CALL C, u16", 3, 12, false, [](CPU& cpu, SPBus&) {
        if (cpu.F().cy) {
            word jumpAddress = cpu.fetchWord();
            cpu.push(cpu.PC());
            cpu.PC() = jumpAddress;
            return 12;
        } else {
            cpu.fetchWord();
            return 0;
        }
    });
    ops.emplace_back(0xDD, "NOP", 1, 4, false, [](CPU&, SPBus&) { return 0; });
    ops.emplace_back(0xDE, "SBC A, u8", 2, 8, false, [](CPU& cpu, SPBus&) {
        cpu.sbcR8(cpu.fetchByte());
        return 0;
    });
    ops.emplace_back(0xDF, "RST 18h", 1, 16, false, [](CPU& cpu, SPBus&) {
        cpu.push(cpu.PC());
        cpu.PC() = 0x18;
        return 0;
    });
    ops.emplace_back(0xE0, "LD (FF00+u8), A", 2, 12, false, [](CPU& cpu, SPBus& bus) {
        bus->write(0xFF00 + cpu.fetchByte(), cpu.A());
        return 0;
    });
    ops.emplace_back(0xE1, "POP HL", 1, 12, false, [](CPU& cpu, SPBus&) {
        cpu.HL(cpu.pop());
        return 0;
    });
    ops.emplace_back(0xE2, "LD (FF00+C), A", 1, 8, false, [](CPU& cpu, SPBus& bus) {
        bus->write(0xFF00 + cpu.C(), cpu.A());
        return 0;
    });
    ops.emplace_back(0xE3, "NOP", 1, 4, false, [](CPU&, SPBus&) { return 0; });
    ops.emplace_back(0xE4, "NOP", 1, 4, false, [](CPU&, SPBus&) { return 0; });
    ops.emplace_back(0xE5, "PUSH HL", 1, 16, false, [](CPU& cpu, SPBus&) {
        cpu.push(cpu.HL());
        return 0;
    });
    ops.emplace_back(0xE6, "AND A, u8", 2, 8, false, [](CPU& cpu, SPBus&) {
        cpu.andR8(cpu.fetchByte());
        return 0;
    });
    ops.emplace_back(0xE7, "RST 20h", 1, 16, false, [](CPU& cpu, SPBus&) {
        cpu.push(cpu.PC());
        cpu.PC() = 0x20;
        return 0;
    });
    ops.emplace_back(0xE8, "ADD SP, i8", 2, 16, false, [](CPU& cpu, SPBus&) {
        auto value = static_cast<sbyte>(cpu.fetchByte());
        word sp = cpu.SP();

        uint result = static_cast<uint>(sp + value);

        auto& F = cpu.F();
        F.zf = false;
        F.n = false;
        // Reference: https://github.com/jgilchrist/gbemu/blob/master/src/cpu/opcodes.cc
        F.h = ((sp ^ value ^ (result & 0xFFFF)) & 0x10) == 0x10;
        F.cy = ((sp ^ value ^ (result & 0xFFFF)) & 0x100) == 0x100;

        cpu.SP() = static_cast<word>(result);

        return 0;
    });
    ops.emplace_back(0xE9, "JP HL", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.PC() = cpu.HL();
        return 0;
    });
    ops.emplace_back(0xEA, "LD (u16), A", 3, 16, false, [](CPU& cpu, SPBus& bus) {
        bus->write(cpu.fetchWord(), cpu.A());
        return 0;
    });
    ops.emplace_back(0xEB, "NOP", 1, 4, false, [](CPU&, SPBus&) { return 0; });
    ops.emplace_back(0xEC, "NOP", 1, 4, false, [](CPU&, SPBus&) { return 0; });
    ops.emplace_back(0xED, "NOP", 1, 4, false, [](CPU&, SPBus&) { return 0; });
    ops.emplace_back(0xEE, "XOR A, u8", 2, 8, false, [](CPU& cpu, SPBus&) {
        cpu.xorR8(cpu.fetchByte());
        return 0;
    });
    ops.emplace_back(0xEF, "RST 28h", 1, 16, false, [](CPU& cpu, SPBus&) {
        cpu.push(cpu.PC());
        cpu.PC() = 0x28;
        return 0;
    });
    ops.emplace_back(0xF0, "LD A, (FF00+u8)", 2, 12, false, [](CPU& cpu, SPBus& bus) {
        cpu.A() = bus->read(0xFF00 + cpu.fetchByte());
        return 0;
    });
    ops.emplace_back(0xF1, "POP AF", 1, 12, false, [](CPU& cpu, SPBus&) {
        cpu.AF(cpu.pop());
        return 0;
    });
    ops.emplace_back(0xF2, "LD A, (FF00+C)", 1, 8, false, [](CPU& cpu, SPBus& bus) {
        cpu.A() = bus->read(0xFF00 + cpu.C());
        return 0;
    });
    ops.emplace_back(0xF3, "DI", 1, 4, false, [](CPU& cpu, SPBus&) {
        cpu.IME() = false;
        return 0;
    });
    ops.emplace_back(0xF4, "NOP", 1, 4, false, [](CPU&, SPBus&) { return 0; });
    ops.emplace_back(0xF5, "PUSH AF", 1, 16, false, [](CPU& cpu, SPBus&) {
        cpu.push(cpu.AF());
        return 0;
    });
    ops.emplace_back(0xF6, "OR A, u8", 2, 8, false, [](CPU& cpu, SPBus&) {
        cpu.orR8(cpu.fetchByte());
        return 0;
    });
    ops.emplace_back(0xF7, "RST 30h", 1, 16, false, [](CPU& cpu, SPBus&) {
        cpu.push(cpu.PC());
        cpu.PC() = 0x30;
        return 0;
    });
    ops.emplace_back(0xF8, "LD HL, SP+i8", 2, 12, false, [](CPU& cpu, SPBus&) {
        auto value = static_cast<sbyte>(cpu.fetchByte());
        word sp = cpu.SP();

        uint result = static_cast<uint>(sp + value);

        auto& F = cpu.F();
        F.zf = false;
        F.n = false;
        F.h = ((sp ^ value ^ (result & 0xFFFF)) & 0x10) == 0x10;
        F.cy = ((sp ^ value ^ (result & 0xFFFF)) & 0x100) == 0x100;

        cpu.HL(static_cast<word>(result));

        return 0;
    });
    ops.emplace_back(0xF9, "LD SP, HL", 1, 8, false, [](CPU& cpu, SPBus&) {
        cpu.SP() = cpu.HL();
        return 0;
    });
    ops.emplace_back(0xFA, "LD A, (16)", 3, 16, false, [](CPU& cpu, SPBus& bus) {
        cpu.A() = bus->read(cpu.fetchWord());
        return 0;
    });
    ops.emplace_back(0xFB, "EI", 1, 4, false, [](CPU& cpu, SPBus&) {
        // The side effect of the EI instruction is delayed by one opcode
        uint nextOpCycles = cpu.execute();
        cpu.IME() = true;
        return nextOpCycles;
    });
    ops.emplace_back(0xFC, "NOP", 1, 4, false, [](CPU&, SPBus&) { return 0; });
    ops.emplace_back(0xFD, "NOP", 1, 4, false, [](CPU&, SPBus&) { return 0; });
    ops.emplace_back(0xFE, "CP A, u8", 2, 8, false, [](CPU& cpu, SPBus&) {
        byte A = cpu.A();
        cpu.subR8(cpu.fetchByte());
        cpu.A() = A;
        return 0;
    });
    ops.emplace_back(0xFF, "RST 38h", 1, 16, false, [](CPU& cpu, SPBus&) {
        cpu.push(cpu.PC());
        cpu.PC() = 0x38;
        return 0;
    });

    return ops;
}

std::vector<Opcode> extendedOpcodeImpl() {
    std::vector<Opcode> ops;
    ops.reserve(256);

    /*
     * The opcode docs at https://izik1.github.io/gbops/index.html mention the extended opcodes
     * as being 2 bytes long, but that is including the 0xCB prefix, and we ignore that in the
     * length parameter here. Similarly the timing includes 4 cycles for the 0xCB fetch but we
     * ignore that here
     * */

    ops.emplace_back(0x00, "RLC B", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.B() = cpu.rlcR8(cpu.B());
        return 0;
    });
    ops.emplace_back(0x01, "RLC C", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.C() = cpu.rlcR8(cpu.C());
        return 0;
    });
    ops.emplace_back(0x02, "RLC D", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.D() = cpu.rlcR8(cpu.D());
        return 0;
    });
    ops.emplace_back(0x03, "RLC E", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.E() = cpu.rlcR8(cpu.E());
        return 0;
    });
    ops.emplace_back(0x04, "RLC H", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.H() = cpu.rlcR8(cpu.H());
        return 0;
    });
    ops.emplace_back(0x05, "RLC L", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.L() = cpu.rlcR8(cpu.L());
        return 0;
    });
    ops.emplace_back(0x06, "RLC [HL]", 1, 12, true, [](CPU& cpu, SPBus& bus) {
        bus->write(cpu.HL(), cpu.rlcR8(bus->read(cpu.HL())));
        return 0;
    });
    ops.emplace_back(0x07, "RLC A", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.A() = cpu.rlcR8(cpu.A());
        return 0;
    });
    ops.emplace_back(0x08, "RRC B", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.B() = cpu.rrcR8(cpu.B());
        return 0;
    });
    ops.emplace_back(0x09, "RRC C", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.C() = cpu.rrcR8(cpu.C());
        return 0;
    });
    ops.emplace_back(0x0A, "RRC D", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.D() = cpu.rrcR8(cpu.D());
        return 0;
    });
    ops.emplace_back(0x0B, "RRC E", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.E() = cpu.rrcR8(cpu.E());
        return 0;
    });
    ops.emplace_back(0x0C, "RRC H", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.H() = cpu.rrcR8(cpu.H());
        return 0;
    });
    ops.emplace_back(0x0D, "RRC L", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.L() = cpu.rrcR8(cpu.L());
        return 0;
    });
    ops.emplace_back(0x0E, "RRC [HL]", 1, 12, true, [](CPU& cpu, SPBus& bus) {
        bus->write(cpu.HL(), cpu.rrcR8(bus->read(cpu.HL())));
        return 0;
    });
    ops.emplace_back(0x0F, "RRC A", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.A() = cpu.rrcR8(cpu.A());
        return 0;
    });
    ops.emplace_back(0x10, "RL B", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.B() = cpu.rlR8(cpu.B());
        return 0;
    });
    ops.emplace_back(0x11, "RL C", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.C() = cpu.rlR8(cpu.C());
        return 0;
    });
    ops.emplace_back(0x12, "RL D", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.D() = cpu.rlR8(cpu.D());
        return 0;
    });
    ops.emplace_back(0x13, "RL E", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.E() = cpu.rlR8(cpu.E());
        return 0;
    });
    ops.emplace_back(0x14, "RL H", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.H() = cpu.rlR8(cpu.H());
        return 0;
    });
    ops.emplace_back(0x15, "RL L", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.L() = cpu.rlR8(cpu.L());
        return 0;
    });
    ops.emplace_back(0x16, "RL [HL]", 1, 12, true, [](CPU& cpu, SPBus& bus) {
        bus->write(cpu.HL(), cpu.rlR8(bus->read(cpu.HL())));
        return 0;
    });
    ops.emplace_back(0x17, "RL A", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.A() = cpu.rlR8(cpu.A());
        return 0;
    });
    ops.emplace_back(0x18, "RR B", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.B() = cpu.rrR8(cpu.B());
        return 0;
    });
    ops.emplace_back(0x19, "RR C", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.C() = cpu.rrR8(cpu.C());
        return 0;
    });
    ops.emplace_back(0x1A, "RR D", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.D() = cpu.rrR8(cpu.D());
        return 0;
    });
    ops.emplace_back(0x1B, "RR E", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.E() = cpu.rrR8(cpu.E());
        return 0;
    });
    ops.emplace_back(0x1C, "RR H", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.H() = cpu.rrR8(cpu.H());
        return 0;
    });
    ops.emplace_back(0x1D, "RR L", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.L() = cpu.rrR8(cpu.L());
        return 0;
    });
    ops.emplace_back(0x1E, "RR [HL]", 1, 12, true, [](CPU& cpu, SPBus& bus) {
        bus->write(cpu.HL(), cpu.rrR8(bus->read(cpu.HL())));
        return 0;
    });
    ops.emplace_back(0x1F, "RR A", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.A() = cpu.rrR8(cpu.A());
        return 0;
    });
    ops.emplace_back(0x20, "SLA B", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.B() = cpu.slaR8(cpu.B());
        return 0;
    });
    ops.emplace_back(0x21, "SLA C", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.C() = cpu.slaR8(cpu.C());
        return 0;
    });
    ops.emplace_back(0x22, "SLA D", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.D() = cpu.slaR8(cpu.D());
        return 0;
    });
    ops.emplace_back(0x23, "SLA E", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.E() = cpu.slaR8(cpu.E());
        return 0;
    });
    ops.emplace_back(0x24, "SLA H", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.H() = cpu.slaR8(cpu.H());
        return 0;
    });
    ops.emplace_back(0x25, "SLA L", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.L() = cpu.slaR8(cpu.L());
        return 0;
    });
    ops.emplace_back(0x26, "SLA [HL]", 1, 12, true, [](CPU& cpu, SPBus& bus) {
        bus->write(cpu.HL(), cpu.slaR8(bus->read(cpu.HL())));
        return 0;
    });
    ops.emplace_back(0x27, "SLA A", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.A() = cpu.slaR8(cpu.A());
        return 0;
    });
    ops.emplace_back(0x28, "SRA B", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.B() = cpu.sraR8(cpu.B());
        return 0;
    });
    ops.emplace_back(0x29, "SRA C", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.C() = cpu.sraR8(cpu.C());
        return 0;
    });
    ops.emplace_back(0x2A, "SRA D", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.D() = cpu.sraR8(cpu.D());
        return 0;
    });
    ops.emplace_back(0x2B, "SRA E", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.E() = cpu.sraR8(cpu.E());
        return 0;
    });
    ops.emplace_back(0x2C, "SRA H", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.H() = cpu.sraR8(cpu.H());
        return 0;
    });
    ops.emplace_back(0x2D, "SRA L", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.L() = cpu.sraR8(cpu.L());
        return 0;
    });
    ops.emplace_back(0x2E, "SRA [HL]", 1, 12, true, [](CPU& cpu, SPBus& bus) {
        bus->write(cpu.HL(), cpu.sraR8(bus->read(cpu.HL())));
        return 0;
    });
    ops.emplace_back(0x2F, "SRA A", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.A() = cpu.sraR8(cpu.A());
        return 0;
    });
    ops.emplace_back(0x30, "SWAP B", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.B() = cpu.swapR8(cpu.B());
        return 0;
    });
    ops.emplace_back(0x31, "SWAP C", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.C() = cpu.swapR8(cpu.C());
        return 0;
    });
    ops.emplace_back(0x32, "SWAP D", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.D() = cpu.swapR8(cpu.D());
        return 0;
    });
    ops.emplace_back(0x33, "SWAP E", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.E() = cpu.swapR8(cpu.E());
        return 0;
    });
    ops.emplace_back(0x34, "SWAP H", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.H() = cpu.swapR8(cpu.H());
        return 0;
    });
    ops.emplace_back(0x35, "SWAP L", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.L() = cpu.swapR8(cpu.L());
        return 0;
    });
    ops.emplace_back(0x36, "SWAP [HL]", 1, 12, true, [](CPU& cpu, SPBus& bus) {
        bus->write(cpu.HL(), cpu.swapR8(bus->read(cpu.HL())));
        return 0;
    });
    ops.emplace_back(0x37, "SWAP A", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.A() = cpu.swapR8(cpu.A());
        return 0;
    });
    ops.emplace_back(0x38, "SRL B", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.B() = cpu.srlR8(cpu.B());
        return 0;
    });
    ops.emplace_back(0x39, "SRL C", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.C() = cpu.srlR8(cpu.C());
        return 0;
    });
    ops.emplace_back(0x3A, "SRL D", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.D() = cpu.srlR8(cpu.D());
        return 0;
    });
    ops.emplace_back(0x3B, "SRL E", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.E() = cpu.srlR8(cpu.E());
        return 0;
    });
    ops.emplace_back(0x3C, "SRL H", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.H() = cpu.srlR8(cpu.H());
        return 0;
    });
    ops.emplace_back(0x3D, "SRL L", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.L() = cpu.srlR8(cpu.L());
        return 0;
    });
    ops.emplace_back(0x3E, "SRL [HL]", 1, 12, true, [](CPU& cpu, SPBus& bus) {
        bus->write(cpu.HL(), cpu.srlR8(bus->read(cpu.HL())));
        return 0;
    });
    ops.emplace_back(0x3F, "SRL A", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.A() = cpu.srlR8(cpu.A());
        return 0;
    });
    ops.emplace_back(0x40, "BIT 0, B", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.F().zf = !isSet(cpu.B(), 0);
        cpu.F().n = false;
        cpu.F().h = true;
        return 0;
    });
    ops.emplace_back(0x41, "BIT 0, C", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.F().zf = !isSet(cpu.C(), 0);
        cpu.F().n = false;
        cpu.F().h = true;
        return 0;
    });
    ops.emplace_back(0x42, "BIT 0, D", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.F().zf = !isSet(cpu.D(), 0);
        cpu.F().n = false;
        cpu.F().h = true;
        return 0;
    });
    ops.emplace_back(0x43, "BIT 0, E", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.F().zf = !isSet(cpu.E(), 0);
        cpu.F().n = false;
        cpu.F().h = true;
        return 0;
    });
    ops.emplace_back(0x44, "BIT 0, H", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.F().zf = !isSet(cpu.H(), 0);
        cpu.F().n = false;
        cpu.F().h = true;
        return 0;
    });
    ops.emplace_back(0x45, "BIT 0, L", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.F().zf = !isSet(cpu.L(), 0);
        cpu.F().n = false;
        cpu.F().h = true;
        return 0;
    });
    ops.emplace_back(0x46, "BIT 0, [HL]", 1, 8, true, [](CPU& cpu, SPBus& bus) {
        cpu.F().zf = !isSet(bus->read(cpu.HL()), 0);
        cpu.F().n = false;
        cpu.F().h = true;
        return 0;
    });
    ops.emplace_back(0x47, "BIT 0, A", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.F().zf = !isSet(cpu.A(), 0);
        cpu.F().n = false;
        cpu.F().h = true;
        return 0;
    });
    ops.emplace_back(0x48, "BIT 1, B", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.F().zf = !isSet(cpu.B(), 1);
        cpu.F().n = false;
        cpu.F().h = true;
        return 0;
    });
    ops.emplace_back(0x49, "BIT 1, C", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.F().zf = !isSet(cpu.C(), 1);
        cpu.F().n = false;
        cpu.F().h = true;
        return 0;
    });
    ops.emplace_back(0x4A, "BIT 1, D", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.F().zf = !isSet(cpu.D(), 1);
        cpu.F().n = false;
        cpu.F().h = true;
        return 0;
    });
    ops.emplace_back(0x4B, "BIT 1, E", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.F().zf = !isSet(cpu.E(), 1);
        cpu.F().n = false;
        cpu.F().h = true;
        return 0;
    });
    ops.emplace_back(0x4C, "BIT 1, H", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.F().zf = !isSet(cpu.H(), 1);
        cpu.F().n = false;
        cpu.F().h = true;
        return 0;
    });
    ops.emplace_back(0x4D, "BIT 1, L", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.F().zf = !isSet(cpu.L(), 1);
        cpu.F().n = false;
        cpu.F().h = true;
        return 0;
    });
    ops.emplace_back(0x4E, "BIT 1, [HL]", 1, 8, true, [](CPU& cpu, SPBus& bus) {
        cpu.F().zf = !isSet(bus->read(cpu.HL()), 1);
        cpu.F().n = false;
        cpu.F().h = true;
        return 0;
    });
    ops.emplace_back(0x4F, "BIT 1, A", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.F().zf = !isSet(cpu.A(), 1);
        cpu.F().n = false;
        cpu.F().h = true;
        return 0;
    });
    ops.emplace_back(0x50, "BIT 2, B", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.F().zf = !isSet(cpu.B(), 2);
        cpu.F().n = false;
        cpu.F().h = true;
        return 0;
    });
    ops.emplace_back(0x51, "BIT 2, C", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.F().zf = !isSet(cpu.C(), 2);
        cpu.F().n = false;
        cpu.F().h = true;
        return 0;
    });
    ops.emplace_back(0x52, "BIT 2, D", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.F().zf = !isSet(cpu.D(), 2);
        cpu.F().n = false;
        cpu.F().h = true;
        return 0;
    });
    ops.emplace_back(0x53, "BIT 2, E", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.F().zf = !isSet(cpu.E(), 2);
        cpu.F().n = false;
        cpu.F().h = true;
        return 0;
    });
    ops.emplace_back(0x54, "BIT 2, H", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.F().zf = !isSet(cpu.H(), 2);
        cpu.F().n = false;
        cpu.F().h = true;
        return 0;
    });
    ops.emplace_back(0x55, "BIT 2, L", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.F().zf = !isSet(cpu.L(), 2);
        cpu.F().n = false;
        cpu.F().h = true;
        return 0;
    });
    ops.emplace_back(0x56, "BIT 2, [HL]", 1, 8, true, [](CPU& cpu, SPBus& bus) {
        cpu.F().zf = !isSet(bus->read(cpu.HL()), 2);
        cpu.F().n = false;
        cpu.F().h = true;
        return 0;
    });
    ops.emplace_back(0x57, "BIT 2, A", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.F().zf = !isSet(cpu.A(), 2);
        cpu.F().n = false;
        cpu.F().h = true;
        return 0;
    });
    ops.emplace_back(0x58, "BIT 3, B", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.F().zf = !isSet(cpu.B(), 3);
        cpu.F().n = false;
        cpu.F().h = true;
        return 0;
    });
    ops.emplace_back(0x59, "BIT 3, C", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.F().zf = !isSet(cpu.C(), 3);
        cpu.F().n = false;
        cpu.F().h = true;
        return 0;
    });
    ops.emplace_back(0x5A, "BIT 3, D", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.F().zf = !isSet(cpu.D(), 3);
        cpu.F().n = false;
        cpu.F().h = true;
        return 0;
    });
    ops.emplace_back(0x5B, "BIT 3, E", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.F().zf = !isSet(cpu.E(), 3);
        cpu.F().n = false;
        cpu.F().h = true;
        return 0;
    });
    ops.emplace_back(0x5C, "BIT 3, H", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.F().zf = !isSet(cpu.H(), 3);
        cpu.F().n = false;
        cpu.F().h = true;
        return 0;
    });
    ops.emplace_back(0x5D, "BIT 3, L", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.F().zf = !isSet(cpu.L(), 3);
        cpu.F().n = false;
        cpu.F().h = true;
        return 0;
    });
    ops.emplace_back(0x5E, "BIT 3, [HL]", 1, 8, true, [](CPU& cpu, SPBus& bus) {
        cpu.F().zf = !isSet(bus->read(cpu.HL()), 3);
        cpu.F().n = false;
        cpu.F().h = true;
        return 0;
    });
    ops.emplace_back(0x5F, "BIT 3, A", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.F().zf = !isSet(cpu.A(), 3);
        cpu.F().n = false;
        cpu.F().h = true;
        return 0;
    });
    ops.emplace_back(0x60, "BIT 4, B", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.F().zf = !isSet(cpu.B(), 4);
        cpu.F().n = false;
        cpu.F().h = true;
        return 0;
    });
    ops.emplace_back(0x61, "BIT 4, C", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.F().zf = !isSet(cpu.C(), 4);
        cpu.F().n = false;
        cpu.F().h = true;
        return 0;
    });
    ops.emplace_back(0x62, "BIT 4, D", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.F().zf = !isSet(cpu.D(), 4);
        cpu.F().n = false;
        cpu.F().h = true;
        return 0;
    });
    ops.emplace_back(0x63, "BIT 4, E", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.F().zf = !isSet(cpu.E(), 4);
        cpu.F().n = false;
        cpu.F().h = true;
        return 0;
    });
    ops.emplace_back(0x64, "BIT 4, H", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.F().zf = !isSet(cpu.H(), 4);
        cpu.F().n = false;
        cpu.F().h = true;
        return 0;
    });
    ops.emplace_back(0x65, "BIT 4, L", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.F().zf = !isSet(cpu.L(), 4);
        cpu.F().n = false;
        cpu.F().h = true;
        return 0;
    });
    ops.emplace_back(0x66, "BIT 4, [HL]", 1, 8, true, [](CPU& cpu, SPBus& bus) {
        cpu.F().zf = !isSet(bus->read(cpu.HL()), 4);
        cpu.F().n = false;
        cpu.F().h = true;
        return 0;
    });
    ops.emplace_back(0x67, "BIT 4, A", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.F().zf = !isSet(cpu.A(), 4);
        cpu.F().n = false;
        cpu.F().h = true;
        return 0;
    });
    ops.emplace_back(0x68, "BIT 5, B", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.F().zf = !isSet(cpu.B(), 5);
        cpu.F().n = false;
        cpu.F().h = true;
        return 0;
    });
    ops.emplace_back(0x69, "BIT 5, C", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.F().zf = !isSet(cpu.C(), 5);
        cpu.F().n = false;
        cpu.F().h = true;
        return 0;
    });
    ops.emplace_back(0x6A, "BIT 5, D", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.F().zf = !isSet(cpu.D(), 5);
        cpu.F().n = false;
        cpu.F().h = true;
        return 0;
    });
    ops.emplace_back(0x6B, "BIT 5, E", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.F().zf = !isSet(cpu.E(), 5);
        cpu.F().n = false;
        cpu.F().h = true;
        return 0;
    });
    ops.emplace_back(0x6C, "BIT 5, H", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.F().zf = !isSet(cpu.H(), 5);
        cpu.F().n = false;
        cpu.F().h = true;
        return 0;
    });
    ops.emplace_back(0x6D, "BIT 5, L", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.F().zf = !isSet(cpu.L(), 5);
        cpu.F().n = false;
        cpu.F().h = true;
        return 0;
    });
    ops.emplace_back(0x6E, "BIT 5, [HL]", 1, 8, true, [](CPU& cpu, SPBus& bus) {
        cpu.F().zf = !isSet(bus->read(cpu.HL()), 5);
        cpu.F().n = false;
        cpu.F().h = true;
        return 0;
    });
    ops.emplace_back(0x6F, "BIT 5, A", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.F().zf = !isSet(cpu.A(), 5);
        cpu.F().n = false;
        cpu.F().h = true;
        return 0;
    });
    ops.emplace_back(0x70, "BIT 6, B", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.F().zf = !isSet(cpu.B(), 6);
        cpu.F().n = false;
        cpu.F().h = true;
        return 0;
    });
    ops.emplace_back(0x71, "BIT 6, C", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.F().zf = !isSet(cpu.C(), 6);
        cpu.F().n = false;
        cpu.F().h = true;
        return 0;
    });
    ops.emplace_back(0x72, "BIT 6, D", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.F().zf = !isSet(cpu.D(), 6);
        cpu.F().n = false;
        cpu.F().h = true;
        return 0;
    });
    ops.emplace_back(0x73, "BIT 6, E", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.F().zf = !isSet(cpu.E(), 6);
        cpu.F().n = false;
        cpu.F().h = true;
        return 0;
    });
    ops.emplace_back(0x74, "BIT 6, H", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.F().zf = !isSet(cpu.H(), 6);
        cpu.F().n = false;
        cpu.F().h = true;
        return 0;
    });
    ops.emplace_back(0x75, "BIT 6, L", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.F().zf = !isSet(cpu.L(), 6);
        cpu.F().n = false;
        cpu.F().h = true;
        return 0;
    });
    ops.emplace_back(0x76, "BIT 6, [HL]", 1, 8, true, [](CPU& cpu, SPBus& bus) {
        cpu.F().zf = !isSet(bus->read(cpu.HL()), 6);
        cpu.F().n = false;
        cpu.F().h = true;
        return 0;
    });
    ops.emplace_back(0x77, "BIT 6, A", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.F().zf = !isSet(cpu.A(), 6);
        cpu.F().n = false;
        cpu.F().h = true;
        return 0;
    });
    ops.emplace_back(0x78, "BIT 7, B", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.F().zf = !isSet(cpu.B(), 7);
        cpu.F().n = false;
        cpu.F().h = true;
        return 0;
    });
    ops.emplace_back(0x79, "BIT 7, C", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.F().zf = !isSet(cpu.C(), 7);
        cpu.F().n = false;
        cpu.F().h = true;
        return 0;
    });
    ops.emplace_back(0x7A, "BIT 7, D", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.F().zf = !isSet(cpu.D(), 7);
        cpu.F().n = false;
        cpu.F().h = true;
        return 0;
    });
    ops.emplace_back(0x7B, "BIT 7, E", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.F().zf = !isSet(cpu.E(), 7);
        cpu.F().n = false;
        cpu.F().h = true;
        return 0;
    });
    ops.emplace_back(0x7C, "BIT 7, H", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.F().zf = !isSet(cpu.H(), 7);
        cpu.F().n = false;
        cpu.F().h = true;
        return 0;
    });
    ops.emplace_back(0x7D, "BIT 7, L", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.F().zf = !isSet(cpu.L(), 7);
        cpu.F().n = false;
        cpu.F().h = true;
        return 0;
    });
    ops.emplace_back(0x7E, "BIT 7, [HL]", 1, 8, true, [](CPU& cpu, SPBus& bus) {
        cpu.F().zf = !isSet(bus->read(cpu.HL()), 7);
        cpu.F().n = false;
        cpu.F().h = true;
        return 0;
    });
    ops.emplace_back(0x7F, "BIT 7, A", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.F().zf = !isSet(cpu.A(), 7);
        cpu.F().n = false;
        cpu.F().h = true;
        return 0;
    });
    ops.emplace_back(0x80, "RES 0, B", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.B() = resetBit(cpu.B(), 0);
        return 0;
    });
    ops.emplace_back(0x81, "RES 0, C", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.C() = resetBit(cpu.C(), 0);
        return 0;
    });
    ops.emplace_back(0x82, "RES 0, D", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.D() = resetBit(cpu.D(), 0);
        return 0;
    });
    ops.emplace_back(0x83, "RES 0, E", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.E() = resetBit(cpu.E(), 0);
        return 0;
    });
    ops.emplace_back(0x84, "RES 0, H", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.H() = resetBit(cpu.H(), 0);
        return 0;
    });
    ops.emplace_back(0x85, "RES 0, L", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.L() = resetBit(cpu.L(), 0);
        return 0;
    });
    ops.emplace_back(0x86, "RES 0, [HL]", 1, 12, true, [](CPU& cpu, SPBus& bus) {
        bus->write(cpu.HL(), resetBit(bus->read(cpu.HL()), 0));
        return 0;
    });
    ops.emplace_back(0x87, "RES 0, A", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.A() = resetBit(cpu.A(), 0);
        return 0;
    });
    ops.emplace_back(0x88, "RES 1, B", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.B() = resetBit(cpu.B(), 1);
        return 0;
    });
    ops.emplace_back(0x89, "RES 1, C", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.C() = resetBit(cpu.C(), 1);
        return 0;
    });
    ops.emplace_back(0x8A, "RES 1, D", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.D() = resetBit(cpu.D(), 1);
        return 0;
    });
    ops.emplace_back(0x8B, "RES 1, E", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.E() = resetBit(cpu.E(), 1);
        return 0;
    });
    ops.emplace_back(0x8C, "RES 1, H", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.H() = resetBit(cpu.H(), 1);
        return 0;
    });
    ops.emplace_back(0x8D, "RES 1, L", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.L() = resetBit(cpu.L(), 1);
        return 0;
    });
    ops.emplace_back(0x8E, "RES 1, [HL]", 1, 12, true, [](CPU& cpu, SPBus& bus) {
        bus->write(cpu.HL(), resetBit(bus->read(cpu.HL()), 1));
        return 0;
    });
    ops.emplace_back(0x8F, "RES 1, A", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.A() = resetBit(cpu.A(), 1);
        return 0;
    });
    ops.emplace_back(0x90, "RES 2, B", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.B() = resetBit(cpu.B(), 2);
        return 0;
    });
    ops.emplace_back(0x91, "RES 2, C", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.C() = resetBit(cpu.C(), 2);
        return 0;
    });
    ops.emplace_back(0x92, "RES 2, D", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.D() = resetBit(cpu.D(), 2);
        return 0;
    });
    ops.emplace_back(0x93, "RES 2, E", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.E() = resetBit(cpu.E(), 2);
        return 0;
    });
    ops.emplace_back(0x94, "RES 2, H", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.H() = resetBit(cpu.H(), 2);
        return 0;
    });
    ops.emplace_back(0x95, "RES 2, L", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.L() = resetBit(cpu.L(), 2);
        return 0;
    });
    ops.emplace_back(0x96, "RES 2, [HL]", 1, 12, true, [](CPU& cpu, SPBus& bus) {
        bus->write(cpu.HL(), resetBit(bus->read(cpu.HL()), 2));
        return 0;
    });
    ops.emplace_back(0x97, "RES 2, A", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.A() = resetBit(cpu.A(), 2);
        return 0;
    });
    ops.emplace_back(0x98, "RES 3, B", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.B() = resetBit(cpu.B(), 3);
        return 0;
    });
    ops.emplace_back(0x99, "RES 3, C", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.C() = resetBit(cpu.C(), 3);
        return 0;
    });
    ops.emplace_back(0x9A, "RES 3, D", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.D() = resetBit(cpu.D(), 3);
        return 0;
    });
    ops.emplace_back(0x9B, "RES 3, E", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.E() = resetBit(cpu.E(), 3);
        return 0;
    });
    ops.emplace_back(0x9C, "RES 3, H", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.H() = resetBit(cpu.H(), 3);
        return 0;
    });
    ops.emplace_back(0x9D, "RES 3, L", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.L() = resetBit(cpu.L(), 3);
        return 0;
    });
    ops.emplace_back(0x9E, "RES 3, [HL]", 1, 12, true, [](CPU& cpu, SPBus& bus) {
        bus->write(cpu.HL(), resetBit(bus->read(cpu.HL()), 3));
        return 0;
    });
    ops.emplace_back(0x9F, "RES 3, A", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.A() = resetBit(cpu.A(), 3);
        return 0;
    });
    ops.emplace_back(0xA0, "RES 4, B", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.B() = resetBit(cpu.B(), 4);
        return 0;
    });
    ops.emplace_back(0xA1, "RES 4, C", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.C() = resetBit(cpu.C(), 4);
        return 0;
    });
    ops.emplace_back(0xA2, "RES 4, D", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.D() = resetBit(cpu.D(), 4);
        return 0;
    });
    ops.emplace_back(0xA3, "RES 4, E", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.E() = resetBit(cpu.E(), 4);
        return 0;
    });
    ops.emplace_back(0xA4, "RES 4, H", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.H() = resetBit(cpu.H(), 4);
        return 0;
    });
    ops.emplace_back(0xA5, "RES 4, L", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.L() = resetBit(cpu.L(), 4);
        return 0;
    });
    ops.emplace_back(0xA6, "RES 4, [HL]", 1, 12, true, [](CPU& cpu, SPBus& bus) {
        bus->write(cpu.HL(), resetBit(bus->read(cpu.HL()), 4));
        return 0;
    });
    ops.emplace_back(0xA7, "RES 4, A", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.A() = resetBit(cpu.A(), 4);
        return 0;
    });
    ops.emplace_back(0xA8, "RES 5, B", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.B() = resetBit(cpu.B(), 5);
        return 0;
    });
    ops.emplace_back(0xA9, "RES 5, C", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.C() = resetBit(cpu.C(), 5);
        return 0;
    });
    ops.emplace_back(0xAA, "RES 5, D", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.D() = resetBit(cpu.D(), 5);
        return 0;
    });
    ops.emplace_back(0xAB, "RES 5, E", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.E() = resetBit(cpu.E(), 5);
        return 0;
    });
    ops.emplace_back(0xAC, "RES 5, H", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.H() = resetBit(cpu.H(), 5);
        return 0;
    });
    ops.emplace_back(0xAD, "RES 5, L", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.L() = resetBit(cpu.L(), 5);
        return 0;
    });
    ops.emplace_back(0xAE, "RES 5, [HL]", 1, 12, true, [](CPU& cpu, SPBus& bus) {
        bus->write(cpu.HL(), resetBit(bus->read(cpu.HL()), 5));
        return 0;
    });
    ops.emplace_back(0xAF, "RES 5, A", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.A() = resetBit(cpu.A(), 5);
        return 0;
    });
    ops.emplace_back(0xB0, "RES 6, B", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.B() = resetBit(cpu.B(), 6);
        return 0;
    });
    ops.emplace_back(0xB1, "RES 6, C", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.C() = resetBit(cpu.C(), 6);
        return 0;
    });
    ops.emplace_back(0xB2, "RES 6, D", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.D() = resetBit(cpu.D(), 6);
        return 0;
    });
    ops.emplace_back(0xB3, "RES 6, E", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.E() = resetBit(cpu.E(), 6);
        return 0;
    });
    ops.emplace_back(0xB4, "RES 6, H", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.H() = resetBit(cpu.H(), 6);
        return 0;
    });
    ops.emplace_back(0xB5, "RES 6, L", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.L() = resetBit(cpu.L(), 6);
        return 0;
    });
    ops.emplace_back(0xB6, "RES 6, [HL]", 1, 12, true, [](CPU& cpu, SPBus& bus) {
        bus->write(cpu.HL(), resetBit(bus->read(cpu.HL()), 6));
        return 0;
    });
    ops.emplace_back(0xB7, "RES 6, A", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.A() = resetBit(cpu.A(), 6);
        return 0;
    });
    ops.emplace_back(0xB8, "RES 7, B", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.B() = resetBit(cpu.B(), 7);
        return 0;
    });
    ops.emplace_back(0xB9, "RES 7, C", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.C() = resetBit(cpu.C(), 7);
        return 0;
    });
    ops.emplace_back(0xBA, "RES 7, D", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.D() = resetBit(cpu.D(), 7);
        return 0;
    });
    ops.emplace_back(0xBB, "RES 7, E", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.E() = resetBit(cpu.E(), 7);
        return 0;
    });
    ops.emplace_back(0xBC, "RES 7, H", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.H() = resetBit(cpu.H(), 7);
        return 0;
    });
    ops.emplace_back(0xBD, "RES 7, L", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.L() = resetBit(cpu.L(), 7);
        return 0;
    });
    ops.emplace_back(0xBE, "RES 7, [HL]", 1, 12, true, [](CPU& cpu, SPBus& bus) {
        bus->write(cpu.HL(), resetBit(bus->read(cpu.HL()), 7));
        return 0;
    });
    ops.emplace_back(0xBF, "RES 7, A", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.A() = resetBit(cpu.A(), 7);
        return 0;
    });
    ops.emplace_back(0xC0, "SET 0, B", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.B() = setBit(cpu.B(), 0);
        return 0;
    });
    ops.emplace_back(0xC1, "SET 0, C", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.C() = setBit(cpu.C(), 0);
        return 0;
    });
    ops.emplace_back(0xC2, "SET 0, D", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.D() = setBit(cpu.D(), 0);
        return 0;
    });
    ops.emplace_back(0xC3, "SET 0, E", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.E() = setBit(cpu.E(), 0);
        return 0;
    });
    ops.emplace_back(0xC4, "SET 0, H", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.H() = setBit(cpu.H(), 0);
        return 0;
    });
    ops.emplace_back(0xC5, "SET 0, L", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.L() = setBit(cpu.L(), 0);
        return 0;
    });
    ops.emplace_back(0xC6, "SET 0, [HL]", 1, 12, true, [](CPU& cpu, SPBus& bus) {
        bus->write(cpu.HL(), setBit(bus->read(cpu.HL()), 0));
        return 0;
    });
    ops.emplace_back(0xC7, "SET 0, A", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.A() = setBit(cpu.A(), 0);
        return 0;
    });
    ops.emplace_back(0xC8, "SET 1, B", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.B() = setBit(cpu.B(), 1);
        return 0;
    });
    ops.emplace_back(0xC9, "SET 1, C", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.C() = setBit(cpu.C(), 1);
        return 0;
    });
    ops.emplace_back(0xCA, "SET 1, D", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.D() = setBit(cpu.D(), 1);
        return 0;
    });
    ops.emplace_back(0xCB, "SET 1, E", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.E() = setBit(cpu.E(), 1);
        return 0;
    });
    ops.emplace_back(0xCC, "SET 1, H", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.H() = setBit(cpu.H(), 1);
        return 0;
    });
    ops.emplace_back(0xCD, "SET 1, L", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.L() = setBit(cpu.L(), 1);
        return 0;
    });
    ops.emplace_back(0xCE, "SET 1, [HL]", 1, 12, true, [](CPU& cpu, SPBus& bus) {
        bus->write(cpu.HL(), setBit(bus->read(cpu.HL()), 1));
        return 0;
    });
    ops.emplace_back(0xCF, "SET 1, A", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.A() = setBit(cpu.A(), 1);
        return 0;
    });
    ops.emplace_back(0xD0, "SET 2, B", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.B() = setBit(cpu.B(), 2);
        return 0;
    });
    ops.emplace_back(0xD1, "SET 2, C", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.C() = setBit(cpu.C(), 2);
        return 0;
    });
    ops.emplace_back(0xD2, "SET 2, D", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.D() = setBit(cpu.D(), 2);
        return 0;
    });
    ops.emplace_back(0xD3, "SET 2, E", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.E() = setBit(cpu.E(), 2);
        return 0;
    });
    ops.emplace_back(0xD4, "SET 2, H", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.H() = setBit(cpu.H(), 2);
        return 0;
    });
    ops.emplace_back(0xD5, "SET 2, L", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.L() = setBit(cpu.L(), 2);
        return 0;
    });
    ops.emplace_back(0xD6, "SET 2, [HL]", 1, 12, true, [](CPU& cpu, SPBus& bus) {
        bus->write(cpu.HL(), setBit(bus->read(cpu.HL()), 2));
        return 0;
    });
    ops.emplace_back(0xD7, "SET 2, A", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.A() = setBit(cpu.A(), 2);
        return 0;
    });
    ops.emplace_back(0xD8, "SET 3, B", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.B() = setBit(cpu.B(), 3);
        return 0;
    });
    ops.emplace_back(0xD9, "SET 3, C", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.C() = setBit(cpu.C(), 3);
        return 0;
    });
    ops.emplace_back(0xDA, "SET 3, D", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.D() = setBit(cpu.D(), 3);
        return 0;
    });
    ops.emplace_back(0xDB, "SET 3, E", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.E() = setBit(cpu.E(), 3);
        return 0;
    });
    ops.emplace_back(0xDC, "SET 3, H", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.H() = setBit(cpu.H(), 3);
        return 0;
    });
    ops.emplace_back(0xDD, "SET 3, L", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.L() = setBit(cpu.L(), 3);
        return 0;
    });
    ops.emplace_back(0xDE, "SET 3, [HL]", 1, 12, true, [](CPU& cpu, SPBus& bus) {
        bus->write(cpu.HL(), setBit(bus->read(cpu.HL()), 3));
        return 0;
    });
    ops.emplace_back(0xDF, "SET 3, A", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.A() = setBit(cpu.A(), 3);
        return 0;
    });
    ops.emplace_back(0xE0, "SET 4, B", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.B() = setBit(cpu.B(), 4);
        return 0;
    });
    ops.emplace_back(0xE1, "SET 4, C", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.C() = setBit(cpu.C(), 4);
        return 0;
    });
    ops.emplace_back(0xE2, "SET 4, D", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.D() = setBit(cpu.D(), 4);
        return 0;
    });
    ops.emplace_back(0xE3, "SET 4, E", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.E() = setBit(cpu.E(), 4);
        return 0;
    });
    ops.emplace_back(0xE4, "SET 4, H", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.H() = setBit(cpu.H(), 4);
        return 0;
    });
    ops.emplace_back(0xE5, "SET 4, L", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.L() = setBit(cpu.L(), 4);
        return 0;
    });
    ops.emplace_back(0xE6, "SET 4, [HL]", 1, 12, true, [](CPU& cpu, SPBus& bus) {
        bus->write(cpu.HL(), setBit(bus->read(cpu.HL()), 4));
        return 0;
    });
    ops.emplace_back(0xE7, "SET 4, A", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.A() = setBit(cpu.A(), 4);
        return 0;
    });
    ops.emplace_back(0xE8, "SET 5, B", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.B() = setBit(cpu.B(), 5);
        return 0;
    });
    ops.emplace_back(0xE9, "SET 5, C", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.C() = setBit(cpu.C(), 5);
        return 0;
    });
    ops.emplace_back(0xEA, "SET 5, D", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.D() = setBit(cpu.D(), 5);
        return 0;
    });
    ops.emplace_back(0xEB, "SET 5, E", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.E() = setBit(cpu.E(), 5);
        return 0;
    });
    ops.emplace_back(0xEC, "SET 5, H", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.H() = setBit(cpu.H(), 5);
        return 0;
    });
    ops.emplace_back(0xED, "SET 5, L", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.L() = setBit(cpu.L(), 5);
        return 0;
    });
    ops.emplace_back(0xEE, "SET 5, [HL]", 1, 12, true, [](CPU& cpu, SPBus& bus) {
        bus->write(cpu.HL(), setBit(bus->read(cpu.HL()), 5));
        return 0;
    });
    ops.emplace_back(0xEF, "SET 5, A", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.A() = setBit(cpu.A(), 5);
        return 0;
    });
    ops.emplace_back(0xF0, "SET 6, B", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.B() = setBit(cpu.B(), 6);
        return 0;
    });
    ops.emplace_back(0xF1, "SET 6, C", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.C() = setBit(cpu.C(), 6);
        return 0;
    });
    ops.emplace_back(0xF2, "SET 6, D", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.D() = setBit(cpu.D(), 6);
        return 0;
    });
    ops.emplace_back(0xF3, "SET 6, E", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.E() = setBit(cpu.E(), 6);
        return 0;
    });
    ops.emplace_back(0xF4, "SET 6, H", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.H() = setBit(cpu.H(), 6);
        return 0;
    });
    ops.emplace_back(0xF5, "SET 6, L", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.L() = setBit(cpu.L(), 6);
        return 0;
    });
    ops.emplace_back(0xF6, "SET 6, [HL]", 1, 12, true, [](CPU& cpu, SPBus& bus) {
        bus->write(cpu.HL(), setBit(bus->read(cpu.HL()), 6));
        return 0;
    });
    ops.emplace_back(0xF7, "SET 6, A", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.A() = setBit(cpu.A(), 6);
        return 0;
    });
    ops.emplace_back(0xF8, "SET 7, B", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.B() = setBit(cpu.B(), 7);
        return 0;
    });
    ops.emplace_back(0xF9, "SET 7, C", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.C() = setBit(cpu.C(), 7);
        return 0;
    });
    ops.emplace_back(0xFA, "SET 7, D", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.D() = setBit(cpu.D(), 7);
        return 0;
    });
    ops.emplace_back(0xFB, "SET 7, E", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.E() = setBit(cpu.E(), 7);
        return 0;
    });
    ops.emplace_back(0xFC, "SET 7, H", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.H() = setBit(cpu.H(), 7);
        return 0;
    });
    ops.emplace_back(0xFD, "SET 7, L", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.L() = setBit(cpu.L(), 7);
        return 0;
    });
    ops.emplace_back(0xFE, "SET 7, [HL]", 1, 12, true, [](CPU& cpu, SPBus& bus) {
        bus->write(cpu.HL(), setBit(bus->read(cpu.HL()), 7));
        return 0;
    });
    ops.emplace_back(0xFF, "SET 7, A", 1, 4, true, [](CPU& cpu, SPBus&) {
        cpu.A() = setBit(cpu.A(), 7);
        return 0;
    });

    return ops;
}

#endif  // GIBI_INCLUDE_CPU_INSTRUCTIONS_H_
