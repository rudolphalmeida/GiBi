/*
 * Implements the Sharp LR35902 Processor used by the GameBoy (DMG-01, GBC)
 * Author: Rudolph Almeida <rudolf1.almeida@gmail.com>
 * */

#ifndef GIBI_INCLUDE_CPU_CPU_H_
#define GIBI_INCLUDE_CPU_CPU_H_

#include <memory>
#include <utility>

#include <spdlog/spdlog.h>

#include "gibi.h"
#include "mmu/bus.h"
#include "timings.h"

// The GameBoy CPU had a 8-bit flag register of which only the upper nibble was
// used. The lower nibble was always 0. The four flags included were:
// 1) Zero flag (zf): Set when the result of an opcode is zero
// 2) Add/Sub flag (n): Set when previous operation was an add or subtract
// 3) Half-Carry flag (h): Set when there was a half-carry or borrow in previous opcode
// 4) Carry flag (cy): Set when there was a carry/borrow in previous opcode
// The `n` and `h` flags are mainly used for BCD math using the DAA instruction.
// The `zf` and `cy` flag are used for control-flow in conditional jumps.
struct StatusRegister {
    bool zf;
    bool n;
    bool h;
    bool cy;

    // Parse a StatusRegister object from a byte
    explicit StatusRegister(byte f) {
        this->zf = (f & (1u << 7u)) != 0;
        this->n = (f & (1u << 6u)) != 0;
        this->h = (f & (1u << 5u)) != 0;
        this->cy = (f & (1u << 4u)) != 0;
    }

    // Convert a StatusRegister to a byte where the lower nibble is all zero
    [[nodiscard]] byte toByte() const {
        byte f{};
        if (zf)
            f |= (1u << 7u);
        if (n)
            f |= (1u << 6u);
        if (h)
            f |= (1u << 5u);
        if (cy)
            f |= (1u << 4u);

        return f;
    }
};

// The state of execution of the CPU. The CPU can either be in a halted state,
// by the HALT or STOP opcode, in which it executes NOPs until put into an
// executing state by an interrupt
enum class CPUState { HALTED, EXECUTING };

// The registers and state of the CPU
class CPU {
   private:
    // This 8-bit registers can be combined to form 4 16-bit registers, namely,
    // AF, BC, DE, and HL
    // Additionally another 8-bit register that opcodes used is called (HL), which
    // is the byte pointed to by the address in HL
    byte a, b, c, d, e, h, l;
    StatusRegister f;

    word sp, pc;

    // This register is used to enable/disable all interrupts. Can be reset using
    // the DI opcode, and enabled using the EI or RETI opcodes.
    //    bool interrupt_master;

    std::shared_ptr<Bus> bus;

    // Run the ISR. Checks if there are any pending interrupts, changes CPU state to
    // executing, and jumps to the interrupt vector with the highest priority
    uint handle_interrupts();

    // Run a single opcode and return the number of clock cycles (t) it took
    uint execute();

   public:

    CPUState state;

    CPU() : CPU(nullptr) {}

    // Initialize the CPU with init values for the DMG-01 model
    explicit CPU(std::shared_ptr<Bus> bus)
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
          //          interrupt_master{true},
          bus{std::move(bus)},
          state{CPUState::EXECUTING} {}

    word& SP() { return sp; }
    [[nodiscard]] const word& SP() const { return sp; }

    word& PC() { return pc; }
    [[nodiscard]] const word& PC() const { return pc; }

    // Accessors for individual registers
    byte& A() { return a; }
    [[nodiscard]] const byte& A() const { return a; }

    StatusRegister& F() { return f; }
    [[nodiscard]] const StatusRegister& F() const { return f; }

    byte& B() { return b; }
    [[nodiscard]] const byte& B() const { return b; }

    byte& C() { return c; }
    [[nodiscard]] const byte& C() const { return c; }

    byte& D() { return d; }
    [[nodiscard]] const byte& D() const { return d; }

    byte& E() { return e; }
    [[nodiscard]] const byte& E() const { return e; }

    byte& H() { return h; }
    [[nodiscard]] const byte& H() const { return h; }

    byte& L() { return l; }
    [[nodiscard]] const byte& L() const { return l; }

    // Accessors for combined registers
    [[nodiscard]] word AF() const { return composeWord(a, f.toByte()); }
    [[nodiscard]] word BC() const { return composeWord(b, c); }
    [[nodiscard]] word DE() const { return composeWord(d, e); }
    [[nodiscard]] word HL() const { return composeWord(h, l); }

    void AF(word af);
    void BC(word bc);
    void DE(word de);
    void HL(word hl);

    // Run the next "atomic" action for the CPU. This could be the ISR, a NOP if
    // the CPU is halted, or a single opcode
    uint tick();

    // These methods should all be private but friend class Opcode doesn't help.
    // TODO: Make these methods private

    // Fetch the byte at PC and increment PC
    byte fetchByte();

    // Fetch a word with LSB at PC and increment PC twice
    word fetchWord();

    // Procedures required by opcodes

    // RLCA - Rotate Register A Left
    void rlca();

    // RRCA - Rotate Register A Right
    void rrca();

    // RLA - Rotate Register A Left through Carry
    void rla();

    // RRA - Rotate Register A Right through Carry
    void rra();

    // Used for INC r8 opcodes
    void incR8(byte& reg);

    // Used for DEC r8 opcodes
    void decR8(byte& reg);

    // Used for ADD HL, ** opcodes
    void addToHL(word value);

    // Decimal Adjust Akku
    // Reference: https://ehaskins.com/2018-01-30%20Z80%20DAA/
    void daa();

    // CPL - Complement Accumalator
    void cpl();
};

#endif  // GIBI_INCLUDE_CPU_CPU_H_
