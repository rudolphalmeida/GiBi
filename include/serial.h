/*
 * While the emulator cannot use serial to communicate with other instances or GameBoys, a lot of
 * emulator tests output their results to serial. This implementation just captures the output and
 * prints it to the screen
 *
 * Author: Rudolph Almeida <rudolf1.almeida@gmail.com>
 * */

#ifndef GIBI_SERIAL_H
#define GIBI_SERIAL_H

#include <iostream>

#include "gibi.h"
#include "mmu/memory.h"

class Serial : public Memory {
   private:
    byte data{};
    byte control{};

   public:
    [[nodiscard]] byte read(word address) const override {
        switch (address) {
            case 0xFF01:
                return data;
            case 0xFF02:
                return control;
            default:
                return 0xFF;  // Should never happen
        }
    }

    void write(word address, byte d) override {
        switch (address) {
            case 0xFF01: {
                this->data = d;
                break;
            }
            case 0xFF02: {
                this->control = d;
                // Uncomment if debugging
//                if (isSet(d, 7)) {
//                    std::cout << static_cast<char>(data);
//                }
                break;
            }
            default: {
                break;
            }
        }
    }
};

#endif  // GIBI_SERIAL_H
