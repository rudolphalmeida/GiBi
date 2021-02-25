/*
 * CPU implementation.
 * Author: Rudolph Almeida <rudolf1.almeida@gmail.com>
 * */

#include <cstring>
#include <sstream>

#include "cpu/cpu.h"
#include "cpu/opcode_info.h"
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
      pc{0x100},  // This should start at 0x100 for emulation tests
      interrupt_master{false},
      bus{std::move(bus)},
      state{CPUState::EXECUTING} {}

uint CPU::tick() {
    uint isrTCycles = handle_interrupts();
    if (isrTCycles) {
        return isrTCycles;
    } else if (state == CPUState::HALTED) {
        return CB_CLOCK_CYCLES[0];  // Execute a NOP
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
    // `ffs` indexes start from 1 so we decrement it
    auto n = ffs(ii) - 1;
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

/*
 * Decode and execute a single opcode. Might execute two opcodes if executing
 * the EI instruction or an extended opcode if opcode is 0xCB,
 * Reference:
 * https://gb-archive.github.io/salvage/decoding_gbz80_opcodes/Decoding%20Gamboy%20Z80%20Opcodes.html
 * SM83_decoding.pdf:
 * https://cdn.discordapp.com/attachments/465586075830845475/742438340078469150/SM83_decoding.pdf
 */
uint CPU::decodeAndExecute() {
    // clang-format off
    byte code = fetchByte();

    if (code == 0xCB) return decodeAndExecuteExtended();

    // Extract components of opcode as defined in the reference
    byte b76 = bitValue(code, 7) << 1 | bitValue(code, 6);
    bool b3 = isSet(code, 3);
    byte b54 = bitValue(code, 5) << 1 | bitValue(code, 4);
    byte b543 = b54 << 1 | bitValue(code, 3);
    byte b210 = code & 0b111;

    uint branchTakenCycles{};

    switch (b76) {
        case 0b00: {
            if (b210 == 0b00) {
                // NOP
                if (b543 == 0b0) { break; }
                // LD (u16), SP
                else if (b543 == 0b1) { ld_u16_sp(); break; }
                // STOP
                else if (b543 == 0b10) { state = CPUState::HALTED; fetchByte(); break; }
                // JR
                else if (b543 == 0b11) { jr(); break; }

                if (isSet(b543, 2)) {  // JR <condition>
                    byte conditionCode = bitValue(b543, 1) << 1 | bitValue(b543, 0);
                    if (checkCondition(conditionCode)) {
                        jr();
                        branchTakenCycles = 4;
                    } else {
                        fetchByte();
                    }
                    break;
                }
            } else if (b210 == 0b01) {
                // r16 is decoded from group 1
                if (b3) {  // ADD HL, r16
                    word r16{};
                    switch (b54) {
                        case 0: r16 = BC(); break;
                        case 1: r16 = DE(); break;
                        case 2: r16 = HL(); break;
                        case 3: r16 = SP(); break;
                        default: break;
                    }
                    addToHL(r16);
                } else {  // LD r16, u16
                    word u16 = fetchWord();
                    switch (b54) {
                        case 0: BC(u16); break;
                        case 1: DE(u16); break;
                        case 2: HL(u16); break;
                        case 3: SP() = u16; break;
                        default: break;
                    }
                }
                break;
            } else if (b210 == 0b10) {
                // r16 is decoded from group 2
                word address{};
                switch (b54) {
                    case 0: address = BC(); break;
                    case 1: address = DE(); break;
                    case 2: address = HL(); HL(HL() + 1); break;
                    case 3: address = HL(); HL(HL() - 1); break;
                    default: break;
                }

                // LD A, (r16)
                if (b3) { A() = bus->read(address); }
                // LD (r16), A
                else { bus->write(address, A()); }

                break;
            } else if (b210 == 0b11) {
                if (b3) {  // DEC r16
                    switch (b54) {
                        case 0: BC(BC() - 1); break;
                        case 1: DE(DE() - 1); break;
                        case 2: HL(HL() - 1); break;
                        case 3: SP() = SP() - 1; break;
                        default: break;
                    }
                } else {  // INC r16
                    switch (b54) {
                        case 0: BC(BC() + 1); break;
                        case 1: DE(DE() + 1); break;
                        case 2: HL(HL() + 1); break;
                        case 3: SP() = SP() + 1; break;
                        default: break;
                    }
                }

                break;
            } else if (b210 == 0b100) {  // INC r8
                if (b543 != 6) {
                    byte& r8 = decodeR8(b543);
                    incR8(r8);
                } else {
                    byte value = bus->read(HL());
                    incR8(value);
                    bus->write(HL(), value);
                }
            } else if (b210 == 0b101) {  // DEC r8
                if (b543 != 6) {
                    byte& r8 = decodeR8(b543);
                    decR8(r8);
                } else {
                    byte value = bus->read(HL());
                    decR8(value);
                    bus->write(HL(), value);
                }
            } else if (b210 == 0b110) {  // LD r8, u8
                if (b543 != 6) {
                    decodeR8(b543) = fetchByte();
                } else {
                    bus->write(HL(), fetchByte());
                }
            } else if (b210 == 0b111) {  // Operations on ALU
                switch (b543) {
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
            }

            break;
        }
        case 0b01: {
            if (b543 == 0b110 && b210 == 0b110) { // HALT
                state = CPUState::HALTED;
            } else {  // LD r8, r8
                // LD (HL), (HL) is not a valid code as its encoding overlaps with HALT so we don't
                // care about it here
                byte& destR8 = decodeR8(b543);
                const byte& srcR8 = decodeR8(b210);
                destR8 = srcR8;
            }

            break;
        }
        case 0b10: {
            const byte& src = b210 == 6 ? bus->read(HL()) : decodeR8(b210);
            switch (b543) {
                case 0: addR8(src); break;
                case 1: adcR8(src); break;
                case 2: subR8(src); break;
                case 3: sbcR8(src); break;
                case 4: andR8(src); break;
                case 5: xorR8(src); break;
                case 6: orR8(src); break;
                case 7: byte value = A(); subR8(src); A() = value; break;
            }

            break;
        }
        case 0b11:
            break;
    }

    return NON_CB_CLOCK_CYCLES[code] + branchTakenCycles;
    // clang-format on
}

uint CPU::decodeAndExecuteExtended() {
    return 0;
}

byte& CPU::decodeR8(byte y) {
    switch (y) {
        case 0:
            return B();
        case 1:
            return C();
        case 2:
            return D();
        case 3:
            return E();
        case 4:
            return H();
        case 5:
            return L();
        default:
            return A();  // Really case 7
    }
}

bool CPU::checkCondition(byte conditionCode) const {
    switch (conditionCode) {
        case 0:
            return !F().zf;
        case 1:
            return F().zf;
        case 2:
            return !F().cy;
        case 3:
            return F().cy;
        default:
            return false;  // Never needed
    }
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
    f.h = willHalfCarry8BitSub(reg, 1u);
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
    bus->write(--SP(), upper);
    bus->write(--SP(), lower);
}

word CPU::pop() {
    byte lower = bus->read(SP()++);
    byte upper = bus->read(SP()++);

    return composeWord(upper, lower);
}

void CPU::ld_u16_sp() {
    word address = fetchWord();

    auto [upper, lower] = decomposeWord(SP());

    bus->write(address, lower);
    bus->write(address, upper);
}

void CPU::jr() {
    auto offset = static_cast<sbyte>(fetchByte());
    PC() = PC() + offset;
}
