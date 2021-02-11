/*
 * Interrupt registers used for generating and handling interrupts in the GameBoy
 * Author: Rudolph Almeida <rudolf1.almeida@gmail.com>
 * */

#ifndef GIBI_INTERRUPTS_H
#define GIBI_INTERRUPTS_H

#include "gibi.h"

/*
 * The GameBoy had five different types of interrupts
 * The numbers correspond to the bit index of the corresponding interrupt in the IE and IF registers
 * */
enum class Interrupts : byte { VBlank = 0, LCDStat = 1, Timer = 2, Serial = 3, JoyPad = 4 };

/*
 * The IntF register (0xFF0F memory mapped) is used for requesting and checking pending interrupts.
 * When a particular interrupt is requested, the corresponding bit in the IF is set, and when the
 * interrupt is serviced, it is reset.
 * */
struct IntF {
    byte data{};

    [[nodiscard]] bool isRequested(Interrupts interrupt) const {
        return isSet(data, static_cast<byte>(interrupt));
    }

    void request(Interrupts interrupt) { data = setBit(data, static_cast<byte>(interrupt)); }

    void reset(Interrupts interrupt) { data = resetBit(data, static_cast<byte>(interrupt)); }
};

/*
 * The IntE register (0xFFFF memory mapped) is used for enabling and disabling particular
 * interrupts. An interrupt is only serviced if it is both enabled in the IE register, and requested
 * in the IF register
 * */
struct IntE {
    byte data{};

    [[nodiscard]] bool isEnabled(Interrupts interrupt) const {
        return isSet(data, static_cast<byte>(interrupt));
    }

    // We probably don't need these two functions, since the interrupts are generally enabled and
    // disabled by the program
    void enable(Interrupts interrupt) { data = setBit(data, static_cast<byte>(interrupt)); }

    void disable(Interrupts interrupt) { data = resetBit(data, static_cast<byte>(interrupt)); }
};

#endif  // GIBI_INTERRUPTS_H
