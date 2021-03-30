/*
 * Implements the internal timing facilities provided by the GameBoy CPU. This is distinct from the
 * RTC (Real Time Clock) provided by some MBC carts
 *
 * Reference: https://hacktix.github.io/GBEDG/timers/ & Pandocs
 * Author: Rudolph Almeida <rudolf1.almeida@gmail.com>
 * */

#ifndef GIBI_TIMER_H
#define GIBI_TIMER_H

#include <memory>

#include "gibi.h"
#include "interrupts.h"
#include "mmu/memory.h"

/*
 * Implements the internal timer found in the GameBoy CPU. The timer is controlled by a few
 * registers: 0xFF04 - DIV: Increments at a rate of 16384Hz. Resets when written to. 0xFF05 - TIMA:
 * Increments at the rate specified by TAC. When it overflows, it will be reset to value in TMA
 * 0xFF06 - TMA: When TIMA overflows, it is reset to this value
 * 0xFF07 - TAC: Timer control. Specifies frequency of TIMA and enables or disables timer
 * */
class Timer : public Memory {
   private:
    // The DIV register is a 16-bit counter of which only the upper 8 bits are mapped
    word div;   // 0xFF04
    byte tima;  // 0xFF05
    byte tma;   // 0xFF06
    byte tac;   // 0xFF07

    byte previousAND{};
    bool timaOverflowed{};
    uint timaReloadClocksLeft{};

    std::shared_ptr<IntF> intf;

   public:
    explicit Timer(std::shared_ptr<IntF> intf);

    void tick(uint cycles);

    [[nodiscard]] byte read(word address) const override;
    void write(word address, byte data) override;
};

#endif  // GIBI_TIMER_H
