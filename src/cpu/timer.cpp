#include <memory>

#include "cpu/timer.h"
#include "gibi.h"

Timer::Timer(std::shared_ptr<IntF> intf)
    : div{}, tima{}, tac{}, tma{}, intf{std::move(intf)}, divClock{256}, timaClock{1024} {}

void Timer::tick(uint cycles) {
    div = static_cast<byte>(divClock.tick(cycles));

    if ((tac & 0x04) != 0) {  // TIMA clock is enabled
        uint n = timaClock.tick(cycles);
        while (n) {
            tima += 1;
            if (tima == 0) {  // TIMA overflowed
                tima = tma;
                intf->request(Interrupts::Timer);
            }

            n--;
        }
    }
}

byte Timer::read(word address) const {
    switch (address) {
        case 0xFF04:
            return div;
        case 0xFF05:
            return tima;
        case 0xFF06:
            return tma;
        case 0xFF07:
            return tac;
        default:
            return 0xFF;  // This should ideally never happen if Bus is correct
    }
}

void Timer::write(word address, byte data) {
    switch (address) {
        case 0xFF04: {
            div = 0;
            divClock.n = 0;
            break;
        }
        case 0xFF05: {
            tima = 0;
            break;
        }
        case 0xFF06: {
            tma = 0;
            break;
        }
        case 0xFF07: {
            if ((tac & 0x03) != (data & 0x03)) {
                timaClock.n = 0;
                switch (data & 0x03) {
                    case 0b00: {
                        timaClock.frequency = 1024;
                        break;
                    }
                    case 0b01: {
                        timaClock.frequency = 16;
                        break;
                    }
                    case 0b10: {
                        timaClock.frequency = 64;
                        break;
                    }
                    case 0b11: {
                        timaClock.frequency = 256;
                        break;
                    }
                }
                tima = tma;
            }

            tac = data;
        }
        default: {
            // Shouldn't really happen
        }
    }
}
