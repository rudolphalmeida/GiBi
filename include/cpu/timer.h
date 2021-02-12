/*
 * Implements the internal timing facilities provided by the GameBoy CPU. This is distinct from the
 * RTC (Real Time Clock) provided by some MBC carts
 * Author: Rudolph Almeida <rudolf1.almeida@gmail.com>
 * */

#ifndef GIBI_TIMER_H
#define GIBI_TIMER_H

#include <memory>

#include "gibi.h"
#include "interrupts.h"
#include "mmu/memory.h"

struct Clock {
    uint frequency;  // Ticks/s
    uint n;          // Internal counter

    explicit Clock(uint frequency) : frequency{frequency}, n{} {}

    uint tick(uint cycles) {
        n += cycles;
        uint res = n / frequency;
        n %= frequency;
        return res;
    }
};

/*
 * Implements the internal timer found in the GameBoy CPU. The timer is controlled by a few registers:
 * 0xFF04 - DIV: Increments at a rate of 16384Hz. Resets when written to.
 * 0xFF05 - TIMA: Increments at the rate specified by TAC. When it overflows, it will be reset to
 *                value in TMA
 * 0xFF06 - TMA: When TIMA overflows, it is reset to this value
 * 0xFF07 - TAC: Timer control. Specifies frequency of TIMA and enables or disables timer
 * */
class Timer: public Memory {
   private:
    byte div;
    byte tima;
    byte tac;
    byte tma;

    std::shared_ptr<IntF> intf;

    Clock divClock;
    Clock timaClock;

   public:
    explicit Timer(std::shared_ptr<IntF> intf);

    void tick(uint cycles);

    [[nodiscard]] byte read(word address) const override;
    void write(word address, byte data) override;
};

#endif  // GIBI_TIMER_H
