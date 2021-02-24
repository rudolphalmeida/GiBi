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
 */
uint CPU::decodeAndExecute() {
    byte code = fetchByte();

    if (code == 0xCB) {
        return decodeAndExecuteExtended();
    }

    // Extract components of opcode as defined in the reference
    byte x = bitValue(code, 7) << 1 | bitValue(code, 6);
    bool q = isSet(code, 3);
    byte p = bitValue(code, 5) << 1 | bitValue(code, 4);
    byte y = p << 1 | bitValue(code, 3);
    byte z = code & 0b111;

    uint branchTakenCycles{};

    switch (x) {
        case 0b00: {
            if (z == 0b0) {
                if (y == 0b0) {  // NOP
                    break;
                } else if (y == 0b1) {  // LD (u16), SP
                    ld_u16_sp();
                    break;
                } else if (y == 0b10) {  // STOP
                    state = CPUState::HALTED;
                    fetchByte();
                    break;
                } else if (y == 0b11) {  // JR
                    jr();
                    break;
                }

                if (isSet(y, 2)) {  // JR <condition>
                    byte conditionCode = bitValue(y, 1) << 1 | bitValue(y, 0);
                    if (checkCondition(conditionCode)) {
                        jr();
                        branchTakenCycles = 4;
                        break;
                    } else {
                        fetchByte();
                        break;
                    }
                }
            }

            if (z == 0b1) {
                // r16 is decoded from group 1
                if (q) {  // ADD HL, r16
                    word r16{};
                    switch (p) {
                        case 0:
                            r16 = BC();
                            break;
                        case 1:
                            r16 = DE();
                            break;
                        case 2:
                            r16 = HL();
                            break;
                        case 3:
                            r16 = SP();
                            break;
                        default:  // Not really needed
                            break;
                    }
                    addToHL(r16);
                    break;
                } else {  // LD r16, u16
                    word u16 = fetchWord();
                    switch (p) {
                        case 0:
                            BC(u16);
                            break;
                        case 1:
                            DE(u16);
                            break;
                        case 2:
                            HL(u16);
                            break;
                        case 3:
                            SP() = u16;
                            break;
                        default:
                            break;
                    }
                    break;
                }
            }

            if (z == 0b10) {
                // r16 is decoded from group 2
                if (q) {  // LD A, (r16)
                    switch (p) {
                        case 0:
                            A() = bus->read(BC());
                            break;
                        case 1:
                            A() = bus->read(DE());
                            break;
                        case 2:
                            A() = bus->read(HL());
                            HL(HL() + 1);
                            break;
                        case 3:
                            A() = bus->read(HL());
                            HL(HL() - 1);
                            break;
                        default:
                            break;
                    }
                } else {  // LD (r16), A
                    switch (p) {
                        case 0:
                            bus->write(BC(), A());
                            break;
                        case 1:
                            bus->write(DE(), A());
                            break;
                        case 2:
                            bus->write(HL(), A());
                            HL(HL() + 1);
                            break;
                        case 3:
                            bus->write(HL(), A());
                            HL(HL() - 1);
                            break;
                        default:
                            break;
                    }
                }
            }

            break;
        }
        case 0b01:
            break;
        case 0b10:
            break;
        case 0b11:
            break;
    }

    return NON_CB_CLOCK_CYCLES[code] + branchTakenCycles;
}

uint CPU::decodeAndExecuteExtended() {
    return 0;
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
