/*
 * CPU implementation.
 * Author: Rudolph Almeida <rudolf1.almeida@gmail.com>
 * */

#include "cpu/cpu.h"
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
