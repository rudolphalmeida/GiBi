/*
 * Implements the Sharp LR35902 Processor used by the GameBoy (DMG-01, GBC)
 * Author: Rudolph Almeida <rudolf1.almeida@gmail.com>
 * */

#ifndef GIBI_INCLUDE_CPU_CPU_H_
#define GIBI_INCLUDE_CPU_CPU_H_

#include <memory>
#include <utility>

#include "gibi.h"
#include "mmu/bus.h"

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
    bool interrupt_master;

    std::shared_ptr<Bus> bus;

    // Run the ISR. Checks if there are any pending interrupts, changes CPU state to
    // executing, and jumps to the interrupt vector with the highest priority
    uint handle_interrupts();

    // Check the condition table and return the boolean of the condition
    [[nodiscard]] bool checkCondition(byte conditionCode) const;

    CPUState state;

    static const word VBLANK_HANDLER_ADDRESS = 0x40;
    static const word LCDSTAT_HANDLER_ADDRESS = 0x48;
    static const word TIMER_HANDLER_ADDRESS = 0x50;
    static const word SERIAL_HANDLER_ADDRESS = 0x58;
    static const word JOYPAD_HANDLER_ADDRESS = 0x60;

    static const uint ISR_CLOCK_CYCLES = 20;  // TODO: Confirm if this is the right value

    // Decode and execute a single opcode and return the number of clock cycles (t) it took
    uint decodeAndExecute();

    // Decode and execute a 0xCB prefixed opcode and return the number of clock cycles it took
    uint decodeAndExecuteExtended();

    byte readR8(byte code);
    void writeR8(byte code, byte value);

    // group is based on SM83_opcode.pdf grouping codes
    template <size_t group>
    word readR16(byte code);

    template <size_t group>
    void writeR16(byte code, word value);

    word& SP() { return sp; }
    [[nodiscard]] const word& SP() const { return sp; }

    word& PC() { return pc; }
    [[nodiscard]] const word& PC() const { return pc; }

    bool& IME() { return interrupt_master; }
    [[nodiscard]] const bool& IME() const { return interrupt_master; }

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

    // Fetch the byte at PC and increment PC
    byte fetchByte();

    // Fetch a word with LSB at PC and increment PC twice
    word fetchWord();

    // Procedures required by opcodes

    // Push a word "down" the stack
    void push(word value);

    // Pop a word from the stack
    word pop();

    // LD (u16), SP - Load SP into immediate address
    void ld_u16_sp();

    // Relative jump from signed immediate integer
    void jr();

    void accumalatorOpcodes(byte code);

    // RLCA - Rotate Register A Left
    void rlca();

    // RRCA - Rotate Register A Right
    void rrca();

    // RLA - Rotate Register A Left through Carry
    void rla();

    // RRA - Rotate Register A Right through Carry
    void rra();

    // Used for INC r8 opcodes
    byte incR8(byte reg);

    // Used for DEC r8 opcodes
    byte decR8(byte reg);

    // Used for ADD HL, ** opcodes
    void addToHL(word value);

    // Used for ADD SP, i8 opcode
    void addToSP(sbyte displacement);

    // Used for LD HL, SP + i8 opcode
    void ld_hl_sp_i8(sbyte displacement);

    // Used for CALL opcodes
    void call(word procAddress);

    // Used for ALU A, r8 opcodes
    void aluR8(byte code, byte operand);

    // Used for ADD A, r8 opcodes
    void addR8(byte value);

    // Used for ADC A, r8 opcodes
    void adcR8(byte value);

    // Used for SUB A, R8 opcodes
    void subR8(byte value);

    // Used for SBC A, R8 opcodes
    void sbcR8(byte value);

    // Used for AND A, R8 opcodes
    void andR8(byte value);

    // Used for XOR A, R8 opcodes
    void xorR8(byte value);

    // USed for OR A, R8 opcodes
    void orR8(byte value);

    // Decimal Adjust Akku
    // Reference: https://ehaskins.com/2018-01-30%20Z80%20DAA/
    void daa();

    // CPL - Complement Accumalator
    void cpl();

    // Extended Opcodes

    // RLC R8 opcodes
    byte rlcR8(byte value);

    // RRC R8 opcodes
    byte rrcR8(byte value);

    // RL R8 opcodes
    byte rlR8(byte value);

    // RR R8 opcodes
    byte rrR8(byte value);

    // SLA R8 opcodes
    byte slaR8(byte value);

    // SRA R8 opcodes
    byte sraR8(byte value);

    // SWAP R8 opcodes
    byte swapR8(byte value);

    // SRL R8 opcodes
    byte srlR8(byte value);

    // BIT, RES, & SET opcodes
    void bit(byte reg, byte bit);

   public:
    CPU() : CPU(nullptr) {}

    // Initialize the CPU with init values for the DMG-01 model
    explicit CPU(std::shared_ptr<Bus> bus);

    // Run the next "atomic" action for the CPU. This could be the ISR, a NOP if
    // the CPU is halted, or a single opcode
    uint tick();
};

#endif  // GIBI_INCLUDE_CPU_CPU_H_
