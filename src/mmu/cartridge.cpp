/*
 * Author: Rudolph Almeida <rudolf1.almeida@gmail.com>
 * */

#include "mmu/cartridge.h"
#include "gibi.h"

byte NoMBC::read(word address) const {
    if (inRange(address, 0x0000, 0x7FFF)) {
        return rom[address];
    } else if (inRange(address, 0xA000, 0xBFFF) && ram) {
        return ram.value()[address];
    }

    return 0xFF;  // This really shouldn't be needed
}

void NoMBC::write(word address, byte data) {
    if (inRange(address, 0x0000, 0x7FFF)) {
        rom[address] = data;
    } else if (inRange(address, 0xA000, 0xBFFF) && ram) {
        ram.value()[address] = data;
    }
}
