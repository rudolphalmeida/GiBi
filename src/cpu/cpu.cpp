/*
 * CPU implementation.
 * Author: Rudolph Almeida <rudolf1.almeida@gmail.com>
 * */

#include "cpu/cpu.h"
#include "cpu/opcode_info.h"
#include "gibi.h"

template <>
word CPU::readR16<1>(byte code);

template <>
void CPU::writeR16<1>(byte code, word value);

template <>
word CPU::readR16<2>(byte code);

template <>
void CPU::writeR16<2>(byte code, word value);

template <>
word CPU::readR16<3>(byte code);

template <>
void CPU::writeR16<3>(byte code, word value);

byte leastSignificantBit(byte value) {
    byte mask{0b1};
    byte bitIndex = 0;

    while (mask) {
        if (value & mask) {
            return bitIndex;
        }
        bitIndex++;
        mask <<= 1;
    }

    return bitIndex;
}

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
      interrupt_master{false},
      bus{std::move(bus)},
      state{CPUState::EXECUTING} {}

uint CPU::tick() {
    uint isrTCycles = handle_interrupts();
    if (isrTCycles) {
        return isrTCycles;
    } else if (state == CPUState::HALTED) {
        return NON_CB_CLOCK_CYCLES[0];  // Execute a NOP
    } else {
        return decodeAndExecute();  // Execute a single opcode
    }
}

uint CPU::handle_interrupts() {
    if (state != CPUState::HALTED && !interrupt_master) {
        return 0;
    }

    byte intf = bus->read(0xFF0F);
    byte inte = bus->read(0xFFFF);

    // Set only *enabled* and *requested* interrupts
    byte ii = intf & inte;
    if (!ii) {  // No pending interrupts

        return 0;
    }

    // When there are pending interrupts, the CPU starts executing again and jumps to the interrupt
    // with the highest priority
    state = CPUState::EXECUTING;

    // However if there are pending interrupts, but *all* interrupts are disabled, the CPU still
    // needs to be executing, however we don't service any interrupt.
    if (!interrupt_master) {
        return 0;
    }

    interrupt_master = false;  // Disable interrupts when an interrupt is executing

    // Find the interrupt with the highest priority. The priority goes from right to left, i.e the
    // interrupt with lower bit index has the higher priority
    auto n = leastSignificantBit(ii);
    intf = resetBit(intf, n);  // Mark the interrupt as serviced
    bus->write(0xFF0F, intf);

    // Jump to interrupt handler
    push(PC());
    switch (static_cast<Interrupts>(n)) {
        case Interrupts::VBlank: {
            PC() = VBLANK_HANDLER_ADDRESS;
            break;
        }
        case Interrupts::LCDStat: {
            PC() = LCDSTAT_HANDLER_ADDRESS;
            break;
        }
        case Interrupts::Timer: {
            PC() = TIMER_HANDLER_ADDRESS;
            break;
        }
        case Interrupts::Serial: {
            PC() = SERIAL_HANDLER_ADDRESS;
            break;
        }
        case Interrupts::JoyPad: {
            PC() = JOYPAD_HANDLER_ADDRESS;
            break;
        }
    }

    return ISR_CLOCK_CYCLES;
}

// This decoding is based on SM83_decoding.pdf
uint CPU::decodeAndExecute() {
    uint branchTakenCycles{};

    byte opcode = fetchByte();

    byte b54 = bitValue(opcode, 5) << 1 | bitValue(opcode, 4);
    byte b43 = bitValue(opcode, 4) << 1 | bitValue(opcode, 3);
    byte b543 = b54 << 1 | bitValue(opcode, 3);
    byte b210 = opcode & 0b111;

    // clang-format off
    switch (opcode) {
        case 0x00:  // NOP
            break;
        case 0x01: case 0x11: case 0x21: case 0x31:  // LD r16, u16
            writeR16<1>(b54, fetchWord());
            break;
        case 0x02: case 0x12: case 0x22: case 0x32:  // LD (r16), A
            bus->write(readR16<2>(b54), A());
            break;
        case 0x03: case 0x13: case 0x23: case 0x33:  // INC r16
            writeR16<1>(b54, readR16<1>(b54) + 1);
            break;
        case 0x04: case 0x14: case 0x24: case 0x34:
        case 0x0C: case 0x1C: case 0x2C: case 0x3C:  // INC r8
            writeR8(b543, incR8(readR8(b543)));
            break;
        case 0x05: case 0x15: case 0x25: case 0x35:
        case 0x0D: case 0x1D: case 0x2D: case 0x3D:  // DEC r8
            writeR8(b543, decR8(readR8(b543)));
            break;
        case 0x06: case 0x16: case 0x26: case 0x36:
        case 0x0E: case 0x1E: case 0x2E: case 0x3E:  // LD r8, u8
            writeR8(b543, fetchByte());
            break;
        case 0x07: case 0x17: case 0x27: case 0x37:
        case 0x0F: case 0x1F: case 0x2F: case 0x3F:  // ALC r8
            accumalatorOpcodes(b543);
            break;
        case 0x08:  // LD (u16), SP
            ld_u16_sp();
            break;
        case 0x09: case 0x19: case 0x29: case 0x39:  // ADD HL, r16
            addToHL(readR16<1>(b54));
            break;
        case 0x0A: case 0x1A: case 0x2A: case 0x3A:  // LD A, (r16)
            A() = bus->read(readR16<2>(b54));
            break;
        case 0x0B: case 0x1B: case 0x2B: case 0x3B:  // DEC r16
            writeR16<1>(b54, readR16<1>(b54) - 1);
            break;
        case 0x10:  // STOP
            state = CPUState::HALTED;
            fetchByte();  // STOP is 2 bytes for some reason...
            break;
        case 0x18:  // JR
            jr();
            break;
        case 0x20: case 0x30: case 0x28: case 0x38:  // JR <cond>
            if (checkCondition(b43)) {
                
                jr();
                branchTakenCycles = 4;
            } else {
                
                fetchByte();
            }
            break;
        case 0x76:  // HALT
            
            state = CPUState::HALTED;
            break;
        case 0x40: case 0x41: case 0x42: case 0x43: case 0x44: case 0x45: case 0x46: case 0x47:
        case 0x48: case 0x49: case 0x4A: case 0x4B: case 0x4C: case 0x4D: case 0x4E: case 0x4F:
        case 0x50: case 0x51: case 0x52: case 0x53: case 0x54: case 0x55: case 0x56: case 0x57:
        case 0x58: case 0x59: case 0x5A: case 0x5B: case 0x5C: case 0x5D: case 0x5E: case 0x5F:
        case 0x60: case 0x61: case 0x62: case 0x63: case 0x64: case 0x65: case 0x66: case 0x67:
        case 0x68: case 0x69: case 0x6A: case 0x6B: case 0x6C: case 0x6D: case 0x6E: case 0x6F:
        case 0x70: case 0x71: case 0x72: case 0x73: case 0x74: case 0x75:            case 0x77:
        case 0x78: case 0x79: case 0x7A: case 0x7B: case 0x7C: case 0x7D: case 0x7E: case 0x7F:
            // LD r8, r8
            writeR8(b543, readR8(b210));
            break;
        case 0x80: case 0x81: case 0x82: case 0x83: case 0x84: case 0x85: case 0x86: case 0x87:
        case 0x88: case 0x89: case 0x8A: case 0x8B: case 0x8C: case 0x8D: case 0x8E: case 0x8F:
        case 0x90: case 0x91: case 0x92: case 0x93: case 0x94: case 0x95: case 0x96: case 0x97:
        case 0x98: case 0x99: case 0x9A: case 0x9B: case 0x9C: case 0x9D: case 0x9E: case 0x9F:
        case 0xA0: case 0xA1: case 0xA2: case 0xA3: case 0xA4: case 0xA5: case 0xA6: case 0xA7:
        case 0xA8: case 0xA9: case 0xAA: case 0xAB: case 0xAC: case 0xAD: case 0xAE: case 0xAF:
        case 0xB0: case 0xB1: case 0xB2: case 0xB3: case 0xB4: case 0xB5: case 0xB6: case 0xB7:
        case 0xB8: case 0xB9: case 0xBA: case 0xBB: case 0xBC: case 0xBD: case 0xBE: case 0xBF:
            // ALU A, r8
            aluR8(b543, readR8(b210));
            break;
        case 0xC0: case 0xD0: case 0xC8: case 0xD8:  // RET <cond>
            if (checkCondition(b43)) {
                
                PC() = pop();
                branchTakenCycles = 12;
            } else {
                
            }
            break;
        case 0xC1: case 0xD1: case 0xE1: case 0xF1:  // POP r16
            writeR16<3>(b54, pop());
            break;
        case 0xC2: case 0xD2: case 0xCA: case 0xDA:  // JP <cond>
            if (checkCondition(b43)) {
                
                PC() = fetchWord();
                branchTakenCycles = 4;
            } else {
                
                fetchWord();
            }
            break;
        case 0xC3:  // JP u16
            PC() = fetchWord();
            break;
        case 0xC4: case 0xD4: case 0xCC: case 0xDC:  // CALL <cond>, u16
            if (checkCondition(b43)) {
                
                call(fetchWord());
                branchTakenCycles = 12;
            } else {
                
                fetchWord();
            }
            break;
        case 0xC5: case 0xD5: case 0xE5: case 0xF5:  // PUSH r16
            push(readR16<3>(b54));
            break;
        case 0xC6: case 0xD6: case 0xE6: case 0xF6:
        case 0xCE: case 0xDE: case 0xEE: case 0xFE:  // ALU A, u8
            aluR8(b543, fetchByte());
            break;
        case 0xC7: case 0xD7: case 0xE7: case 0xF7:
        case 0xCF: case 0xDF: case 0xEF: case 0xFF:  // RST
            call(b543 << 3);
            break;
        case 0xC9:  // RET
            PC() = pop();
            break;
        case 0xCB:  // CB <opcode>
            branchTakenCycles = decodeAndExecuteExtended();
            break;
        case 0xCD:  // CALL u16
            call(fetchWord());
            break;
        case 0xD9:  // RETI
            
            IME() = true;
            PC() = pop();
            break;
        case 0xE0:  // LD (FF00 + u8), A
            bus->write(0xFF00 + fetchByte(), A());
            break;
        case 0xF0:  // LD A, (FF00 + u8)
            A() = bus->read(0xFF00 + fetchByte());
            break;
        case 0xE2:  // LD (FF00 + C), A
            bus->write(0xFF00 + C(), A());
            break;
        case 0xF2:  // LD A, (FF00 + C)
            A() = bus->read(0xFF00 + C());
            break;
        case 0xF3:  // DI
            IME() = false;
            break;
        case 0xE8:  // ADD SP, i8
            addToSP(static_cast<sbyte>(fetchByte()));
            break;
        case 0xF8:  // LD HL, SP + i8
            ld_hl_sp_i8(static_cast<sbyte>(fetchByte()));
            break;
        case 0xE9:  // JP HL
            PC() = HL();
            break;
        case 0xF9:  // LD SP, HL
            SP() = HL();
            break;
        case 0xEA:  // LD (u16), A
            bus->write(fetchWord(), A());
            break;
        case 0xFA:  // LD A, (u16)
            A() = bus->read(fetchWord());
            break;
        case 0xFB:  // EI
            IME() = true;
            // EI schedules interrupt handling to be enabled after the
            // next machine cycle. To prevent any interrupts to be ran
            // after enabling IME, we execute an opcode here itself
            branchTakenCycles = decodeAndExecute();
            break;
        default:
            
            break;
    }
    // clang-format on

    return NON_CB_CLOCK_CYCLES[opcode] + branchTakenCycles;
}

// Based on opcode table group 2 of reference
void CPU::aluR8(byte code, byte operand) {
    // clang-format off
    switch (code & 0b111) {
        case 0: addR8(operand); break;
        case 1: adcR8(operand); break;
        case 2: subR8(operand); break;
        case 3: sbcR8(operand); break;
        case 4: andR8(operand); break;
        case 5: xorR8(operand); break;
        case 6: orR8(operand); break;
        case 7: byte value = A(); subR8(operand); A() = value; break;
    }
    // clang-format on
}

// Based on opcode table group 1 of reference
void CPU::accumalatorOpcodes(byte code) {
    // clang-format off
    switch (code & 0b111) {
        case 0: rlca(); break;
        case 1: rrca(); break;
        case 2: rla(); break;
        case 3: rra(); break;
        case 4: daa(); break;
        case 5: cpl(); break;
        // SCF
        case 6: F().cy = true; F().n = false; F().h = false; break;
        // CCF
        case 7: F().cy = !F().cy; F().n = false; F().h = false; break;
    }
    // clang-format on
}

byte CPU::readR8(byte code) {
    // clang-format off
    switch (code & 0b111) {
        case 0: return B();
        case 1: return C();
        case 2: return D();
        case 3: return E();
        case 4: return H();
        case 5: return L();
        case 6: return bus->read(HL());
        case 7: return A();
    }

    return 0xFF;  // Shouldn't really  be required
                  // clang-format on
}

void CPU::writeR8(byte code, byte value) {
    // clang-format off
    switch (code & 0b111) {
        case 0: B() = value; break;
        case 1: C() = value; break;
        case 2: D() = value; break;
        case 3: E() = value; break;
        case 4: H() = value; break;
        case 5: L() = value; break;
        case 6: bus->write(HL(), value); break;
        case 7: A() = value; break;
    }
    // clang-format on
}

template <>
word CPU::readR16<1>(byte code) {
    // clang-format off
    switch (code & 0b11) {
        case 0: return BC();
        case 1: return DE();
        case 2: return HL();
        case 3: return SP();
    }

    return 0xFFFF;
    // clang-format on
}

template <>
void CPU::writeR16<1>(byte code, word value) {
    // clang-format off
    switch (code & 0b11) {
        case 0: BC(value); break;
        case 1: DE(value); break;
        case 2: HL(value); break;
        case 3: SP() = value; break;
    }
    // clang-format on
}

template <>
word CPU::readR16<2>(byte code) {
    // clang-format off
    word value{0xFFFF};
    switch (code & 0b11) {
        case 0: value = BC(); break;
        case 1: value = DE(); break;
        case 2: value = HL(); HL(HL() + 1); break;
        case 3: value = HL(); HL(HL() - 1); break;
    }

    return value;
    // clang-format on
}

template <>
void CPU::writeR16<2>(byte code, word value) {
    // clang-format off
    switch (code & 0b11) {
        case 0: BC(value); break;
        case 1: DE(value); break;
        case 2: bus->write(HL(), value); HL(HL() + 1); break;
        case 3: bus->write(HL(), value); HL(HL() - 1); break;
    }
    // clang-format on
}

template <>
word CPU::readR16<3>(byte code) {
    // clang-format off
    switch (code & 0b11) {
        case 0: return BC();
        case 1: return DE();
        case 2: return HL();
        case 3: return AF();
    }

    return 0xFFFF;
    // clang-format on
}

template <>
void CPU::writeR16<3>(byte code, word value) {
    // clang-format off
    switch (code & 0b11) {
        case 0: BC(value); break;
        case 1: DE(value); break;
        case 2: HL(value); break;
        case 3: AF(value); break;
    }
    // clang-format on
}

uint CPU::decodeAndExecuteExtended() {
    // clang-format off
    byte code = fetchByte();

    

    // Extract components of opcode as defined in the reference
    byte b76 = bitValue(code, 7) << 1 | bitValue(code, 6);
    byte b54 = bitValue(code, 5) << 1 | bitValue(code, 4);
    byte b543 = b54 << 1 | bitValue(code, 3);
    byte b210 = code & 0b111;

    switch (b76 & 0b11) {
        case 0b00: {
            //            byte& operand = decodeR8(b210);
            byte operand = readR8(b210);
            switch (b543) {
                case 0: writeR8(b210, rlcR8(operand)); break;
                case 1: writeR8(b210, rrcR8(operand)); break;
                case 2: writeR8(b210, rlR8(operand)); break;
                case 3: writeR8(b210, rrR8(operand)); break;
                case 4: writeR8(b210, slaR8(operand)); break;
                case 5: writeR8(b210, sraR8(operand)); break;
                case 6: writeR8(b210, swapR8(operand)); break;
                case 7: writeR8(b210, srlR8(operand)); break;
                default: break;
            }
            break;
        }
        case 0b01: bit(readR8(b210), b543); break;
        case 0b10: writeR8(b210, resetBit(readR8(b210), b543)); break;
        case 0b11: writeR8(b210, setBit(readR8(b210), b543)); break;
    }

    return CB_CLOCK_CYCLES[code];
    // clang-format on
}

bool CPU::checkCondition(byte conditionCode) const {
    switch (conditionCode & 0b11) {
        case 0:
            return !F().zf;
        case 1:
            return F().zf;
        case 2:
            return !F().cy;
        case 3:
            return F().cy;
    }

    return false;
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
    byte value = bus->read(pc++);

    return value;
}

word CPU::fetchWord() {
    byte lsb = fetchByte();
    byte msb = fetchByte();

    word value = composeWord(msb, lsb);

    return value;
}

void CPU::addToHL(word value) {
    uint result = HL() + value;

    f.n = false;
    f.h = willHalfCarry16BitAdd(HL(), value);
    f.cy = (result & 0x10000u) != 0;

    HL(static_cast<word>(result));
}

byte CPU::decR8(byte reg) {
    f.h = willHalfCarry8BitSub(reg, 1u);
    f.n = true;

    reg = reg - 1;

    f.zf = reg == 0;

    return reg;
}

byte CPU::incR8(byte reg) {
    f.h = willHalfCarry8BitAdd(reg, 1);
    f.n = false;

    reg = reg + 1;

    f.zf = reg == 0;

    return reg;
}

void CPU::rrca() {
    A() = rrcR8(A());
    F().zf = false;
}

void CPU::rlca() {
    A() = rlcR8(A());
    F().zf = false;
}

void CPU::rla() {
    A() = rlR8(A());
    F().zf = false;
}

void CPU::rra() {
    A() = rrR8(A());
    F().zf = false;
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
    F.zf = (result & 0xFF) == 0;
    F.n = false;
    F.h = willHalfCarry8BitAdd(A(), value);
    F.cy = (result & 0x100u) != 0;

    A() = static_cast<byte>(result);
}

void CPU::adcR8(byte value) {
    auto& F = this->F();
    auto carry = F.cy ? 1 : 0;

    uint result = A() + value + carry;

    F.zf = (result & 0xFF) == 0;
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
    int overflowedResult = reg - value - carry;
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
    } else {
        F.cy = false;
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
    } else {
        F.cy = false;
    }

    F.n = false;
    F.h = false;
    F.zf = result == 0;

    return result;
}

byte CPU::rlR8(byte value) {
    auto& F = this->F();
    bool oldCarry = F.cy;

    F.cy = isSet(value, 7);

    byte result = value << 1;
    if (oldCarry) {
        result = setBit(result, 0);
    }

    F.n = false;
    F.h = false;
    F.zf = result == 0;

    return result;
}

byte CPU::rrR8(byte value) {
    auto& F = this->F();
    bool oldCarry = F.cy;

    F.cy = isSet(value, 0);

    byte result = value >> 1;
    if (oldCarry) {
        result = setBit(result, 7);
    }

    F.n = false;
    F.h = false;
    F.zf = result == 0;

    return result;
}

byte CPU::slaR8(byte value) {
    auto& F = this->F();
    F.cy = isSet(value, 7);

    byte result = value << 1;

    F.zf = result == 0;
    F.n = false;
    F.h = false;

    return result;
}

byte CPU::sraR8(byte value) {
    auto& F = this->F();
    F.cy = isSet(value, 0);

    bool oldBit7 = isSet(value, 7);

    byte result = value >> 1;
    if (oldBit7) {
        result = setBit(result, 7);
    }

    F.zf = result == 0;
    F.n = false;
    F.h = false;

    return result;
}

byte CPU::swapR8(byte value) {
    byte result = (value << 4) | (value >> 4);

    auto& F = this->F();
    F.zf = result == 0;
    F.n = false;
    F.h = false;
    F.cy = false;

    return result;
}

byte CPU::srlR8(byte value) {
    auto& F = this->F();
    F.cy = isSet(value, 0);

    byte result = value >> 1;

    F.zf = result == 0;
    F.n = false;
    F.h = false;

    return result;
}

void CPU::push(word value) {
    auto [upper, lower] = decomposeWord(value);

    // The GameBoy stack always grows down from the SP
    bus->write(SP() - 1, upper);
    bus->write(SP() - 2, lower);
    SP() = SP() - 2;
}

word CPU::pop() {
    byte lower = bus->read(SP());
    byte upper = bus->read(SP() + 1);
    SP() = SP() + 2;

    word value = composeWord(upper, lower);

    return value;
}

void CPU::ld_u16_sp() {
    word address = fetchWord();

    auto [upper, lower] = decomposeWord(SP());

    bus->write(address, lower);
    bus->write(address + 1, upper);
}

void CPU::jr() {
    auto offset = static_cast<sbyte>(fetchByte());
    PC() = PC() + offset;
}

void CPU::addToSP(sbyte displacement) {
    uint result = static_cast<uint>(SP() + displacement);

    F().zf = false;
    F().n = false;
    // Reference: https://github.com/jgilchrist/gbemu/blob/master/src/cpu/opcodes.cc
    F().h = ((SP() ^ displacement ^ (result & 0xFFFF)) & 0x10) == 0x10;
    F().cy = ((SP() ^ displacement ^ (result & 0xFFFF)) & 0x100) == 0x100;

    SP() = static_cast<word>(result);
}

void CPU::ld_hl_sp_i8(sbyte displacement) {
    uint result = static_cast<uint>(SP() + displacement);

    F().zf = false;
    F().n = false;
    F().h = ((SP() ^ displacement ^ (result & 0xFFFF)) & 0x10) == 0x10;
    F().cy = ((SP() ^ displacement ^ (result & 0xFFFF)) & 0x100) == 0x100;

    HL(static_cast<word>(result));
}

void CPU::call(word procAddress) {
    push(PC());
    PC() = procAddress;
}

void CPU::bit(byte reg, byte bit) {
    F().zf = !isSet(reg, bit);
    F().n = false;
    F().h = true;
}
