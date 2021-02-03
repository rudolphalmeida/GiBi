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
    auto [A, F] = decomposeWord(af);
    this->a = A;
    this->f = StatusRegister(F);
}

void CPU::BC(word bc) {
    auto [B, C] = decomposeWord(bc);
    this->b = B;
    this->c = C;
}

void CPU::DE(word de) {
    auto [D, E] = decomposeWord(de);
    this->d = D;
    this->e = E;
}

void CPU::HL(word hl) {
    auto [H, L] = decomposeWord(hl);
    this->h = H;
    this->l = L;
}

byte CPU::fetchByte() {
    return bus->read(pc++);
}

word CPU::fetchWord() {
    byte lsb = fetchByte();
    byte msb = fetchByte();

    return composeWord(msb, lsb);
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
        setBit(A(), 7);
    } else {
        resetBit(A(), 7);
    }

    f.zf = false;
    f.n = false;
    f.h = false;
}

void CPU::rlca() {
    f.cy = isSet(A(), 7);
    A() = A() << 1u;  // Shift all to left and set bit 0 to old bit 7
    if (f.cy) {
        setBit(A(), 0);
    } else {
        resetBit(A(), 0);
    }

    f.zf = false;
    f.n = false;
    f.h = false;
}

void CPU::rla() {
    bool oldCarry = f.cy;
    rlca();  // This will set all flags

    // Ignore the 0 bit from RLCA and set to oldCarry
    if (oldCarry) {
        setBit(A(), 0);
    } else {
        resetBit(A(), 0);
    }
}

void CPU::rra() {
    bool oldCarry = f.cy;
    rrca();  // This will set all flags

    // Ignore the 7 bit from RLCA and set to oldCarry
    if (oldCarry) {
        setBit(A(), 7);
    } else {
        resetBit(A(), 7);
    }
}

void CPU::daa() {
    byte correction = 0u;

    if (f.h || (!f.n && (a & 0xFu) > 9)) {
        correction |= 0x6u;
    }

    if (f.cy || (!f.n && (a > 0x99))) {
        correction |= 0x60u;
        f.cy = true;
    }

    if (f.n) {
        A() = A() - correction;
    } else {
        A() = A() + correction;
    }

    A() = A() & 0xFFu;

    f.zf = A() == 0;
    f.h = false;
}

void CPU::cpl() {
    A() = ~A();
    f.n = true;
    f.h = true;
}

void CPU::addR8(byte value) {
    uint result = A() + value;

    auto& F = this->F();
    F.zf = result == 0;
    F.n = false;
    F.h = willHalfCarry8BitAdd(A(), value);
    F.cy = (result & 0x100u) != 0;

    A() = static_cast<byte>(result);
}

void CPU::adcR8(byte value) {
    auto& F = this->F();
    auto carry = F.cy ? 1 : 0;

    uint result = A() + value + carry;

    F.zf = result == 0;
    F.n = false;
    F.h = ((value & 0xFu) + (A() & 0xFu) + carry) > 0xFu;
    F.cy = result > 0xFFu;

    A() = static_cast<byte>(result);
}

void CPU::subR8(byte value) {
    byte reg = A();
    byte result = reg - value;

    A() = result;

    auto& F = this->F();
    F.zf = result == 0;
    F.n = true;
    F.h = willHalfCarry8BitSub(reg, value);
    F.cy = reg < value;
}

void CPU::sbcR8(byte value) {
    auto& F = this->F();
    auto carry = F.cy ? 1 : 0;

    byte reg = A();
    uint overflowedResult = reg - value - carry;
    byte result = static_cast<byte>(overflowedResult);
    A() = result;

    F.zf = result == 0;
    F.n = true;
    F.h = ((reg & 0xF) - (value & 0xF) - carry) < 0;
    F.cy = overflowedResult < 0;
}

void CPU::andR8(byte value) {
    byte result = A() & value;
    A() = result;

    auto& F = this->F();
    F.zf = result == 0;
    F.n = false;
    F.cy = false;
    F.h = true;
}

void CPU::xorR8(byte value) {
    byte result = A() ^ value;
    A() = result;

    auto& F = this->F();
    F.zf = result == 0;
    F.n = false;
    F.h = false;
    F.cy = false;
}
