#include <memory>

#include "cpu/timer.h"
#include "gibi.h"

Timer::Timer(std::shared_ptr<IntF> intf) : div{}, tima{}, tma{}, tac{}, intf{std::move(intf)} {}

void Timer::tick(uint cycles) {
    while (cycles) {
        div++;

        byte bitPosition{};
        // clang-format off
        switch (tac & 0b11) {
            case 0b00: bitPosition = 9; break;
            case 0b01: bitPosition = 3; break;
            case 0b10: bitPosition = 5; break;
            case 0b11: bitPosition = 7; break;
        }
        // clang-format on

        byte counterBit = bitValue(div, bitPosition);
        byte timerEnabled = bitValue(tac, 2);

        if (!(counterBit & timerEnabled) && previousAND == 1) {
            if (timaOverflowed) {
                if (timaReloadClocksLeft == 0) {
                    tima = tma;
                    intf->request(Interrupts::Timer);
                } else {
                    timaReloadClocksLeft--;
                }
            } else {
                tima++;
                if (tima == 0) {
                    timaOverflowed = true;
                    timaReloadClocksLeft = 4;
                }
            }
        }

        previousAND = counterBit & timerEnabled;

        cycles--;
    }
}

byte Timer::read(word address) const {
    // clang-format off
    switch (address) {
        case 0xFF04: return (div & 0xFF00) >> 8;
        case 0xFF05: return tima;
        case 0xFF06: return tma;
        case 0xFF07: return tac;
        default: return 0xFF;  // This should ideally never happen if Bus is correct
    }
    // clang-format on
}

void Timer::write(word address, byte data) {
    // clang-format off
    switch (address) {
        case 0xFF04: div = 0; break;
        case 0xFF05: tima = data; break;
        case 0xFF06: tma = data; break;
        case 0xFF07: tac = data | 0b11111000; break;
        default: break;
    }
    // clang-format on
}
