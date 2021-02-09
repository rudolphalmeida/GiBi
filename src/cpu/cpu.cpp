/*
 * CPU implementation.
 * Author: Rudolph Almeida <rudolf1.almeida@gmail.com>
 * */

#include "cpu/cpu.h"
#include "cpu/instructions.h"
#include "gibi.h"

CPU::CPU(std::shared_ptr<Bus> bus)
    : a{0x01},
      b{0x00},
      c{0x13},
      d{0x00},
      e{0xD8},
      h{0x01},
      l{0x4D},
      f{0xB0},
      sp{0xFFFE},
      pc{0x00},  // This should start at 0x100 for emulation tests
      interrupt_master{true},
      bus{std::move(bus)},
      opcodes{opcodeImpl()},
      extendedOpcodes{extendedOpcodeImpl()},
      state{CPUState::EXECUTING} {}

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

// TODO: Complete ISR
uint CPU::handle_interrupts() {
    return 0;
}

/* Fetch and execute a single opcode. Might execute two opcodes if executing
 * the EI instruction or an extended opcode if opcode is 0xCB
 */
uint CPU::execute() {
    byte code = fetchByte();
    auto& opcode = opcodes.at(code);

    uint branchTakeCycles = opcode(*this, bus);
    return opcode.tCycles + branchTakeCycles;
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

    // Ignore the 7 bit from RRCA and set to oldCarry
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

void CPU::orR8(byte value) {
    byte result = A() | value;
    A() = result;

    auto& F = this->F();
    F.zf = result == 0;
    F.n = false;
    F.h = false;
    F.cy = false;
}

byte CPU::rlcR8(byte value) {
    bool oldBit7 = isSet(value, 7);
    byte result = value << 1;

    auto& F = this->F();

    if (oldBit7) {
        result = setBit(result, 0);
        F.cy = true;
    }

    F.n = false;
    F.h = false;
    F.zf = result == 0;

    return result;
}

byte CPU::rrcR8(byte value) {
    bool oldBit0 = isSet(value, 0);
    byte result = value >> 1;

    auto& F = this->F();

    if (oldBit0) {
        result = setBit(result, 7);
        F.cy = true;
    }

    F.n = false;
    F.h = false;
    F.zf = result == 0;

    return result;
}

void CPU::push(word value) {
    auto [upper, lower] = decomposeWord(value);

    // The GameBoy stack always grows down from the SP
    bus->write(--SP(), upper);
    bus->write(--SP(), lower);
}

word CPU::pop() {
    byte lower = bus->read(SP()++);
    byte upper = bus->read(SP()++);

    return composeWord(upper, lower);
}

// TODO: Complete extended opcode execute
uint CPU::executeExtended() {
    byte code = fetchByte();
    auto& opcode = extendedOpcodes.at(code);

    opcode(*this, bus);
    return opcode.tCycles;
}
