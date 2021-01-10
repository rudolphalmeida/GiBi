/*
 * CPU implementation.
 * Author: Rudolph Almeida <rudolf1.almeida@gmail.com>
 * */

#include <cpu/cpu.h>

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
