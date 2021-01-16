/*
 * CPU implementation.
 * Author: Rudolph Almeida <rudolf1.almeida@gmail.com>
 * */

#include "cpu/cpu.h"
#include "cpu/instructions.h"
#include "gibi.h"

uint CPU::tick() {
    uint isrTCycles = handle_interrupts();
    if (isrTCycles) {
        return isrTCycles;
    } else if (state == CPUState::HALTED) {
        return CB_CLOCK_CYCLES[0];  // Execute a NOP
    } else {
        return execute();  // Execute a single opcode
    }
}

uint CPU::handle_interrupts() {
    return 0;
}

uint CPU::execute() {
    return 0;
}

void CPU::AF(word af) {
    auto [a, f] = decomposeWord(af);
    this->a = a;
    this->f = StatusRegister(f);
}

void CPU::BC(word bc) {
    auto [b, c] = decomposeWord(bc);
    this->b = b;
    this->c = c;
}

void CPU::DE(word de) {
    auto [d, e] = decomposeWord(de);
    this->d = d;
    this->e = e;
}

void CPU::HL(word hl) {
    auto [h, l] = decomposeWord(hl);
    this->h = h;
    this->l = l;
}

byte CPU::fetchByte() {
    return bus->read(pc++);
}

word CPU::fetchWord() {
    byte lsb = fetchByte();
    byte msb = fetchByte();

    return composeWord(msb, lsb);
}

template <typename T>
void CPU::load(T& dest, T src) {
    dest = src;
}

void CPU::addToHL(word value) {
    uint result = HL() + value;

    f.n = false;
    f.h = willHalfCarry16BitAdd(HL(), value);
    f.cy = (result & 0x10000u) != 0;

    HL(static_cast<word>(result));
}

void CPU::decR8(byte& reg) {
    f.h = (reg & 0x0Fu) == 0x0Fu;
    f.n = true;

    reg = reg - 1;

    f.zf = reg == 0;
}

void CPU::incR8(byte& reg) {
    f.h = willHalfCarry8BitAdd(reg, 1);
    f.n = false;

    reg = reg + 1;

    f.zf = reg == 0;
}

void CPU::rrca() {
    f.cy = isSet(A(), 0);
    A() = A() >> 1u;  // Shift all to right and set bit 7 to old bit 0
    if (f.cy) {
        set(A(), 7);
    } else {
        reset(A(), 7);
    }

    f.zf = false;
    f.n = false;
    f.h = false;
}

void CPU::rlca() {
    f.cy = isSet(A(), 7);
    A() = A() << 1u;  // Shift all to left and set bit 0 to old bit 7
    if (f.cy) {
        set(A(), 0);
    } else {
        reset(A(), 0);
    }

    f.zf = false;
    f.n = false;
    f.h = false;
}
