/*
 * Author: Rudolph Almeida <rudolf1.almeida@gmail.com>
 * */

#include "mmu/bus.h"
#include "gibi.h"

byte Bus::read(word address) const {
    if (inRange(address, 0x0000, 0x7FFF)) {
        return cart->read(address);
    } else if (inRange(address, 0x8000, 0x9FFF)) {
        // PPU
        return 0xFF;
    } else if (inRange(address, 0xA000, 0xBFFF)) {
        return cart->read(address);
    } else if (inRange(address, 0xC000, 0xDFFF)) {
        return wram[address - 0xC000];
    } else if (inRange(address, 0xE000, 0xFDFF)) {
        return wram[address - 0xE000];
    } else if (inRange(address, 0xFE00, 0xFE9F)) {
        // PPU
        return 0xFF;
    } else if (inRange(address, 0xFEA0, 0xFEFF)) {
        return 0xFF;  // Unusable space
    } else if (inRange(address, 0xFF80, 0xFFFE)) {
        return hram[address - 0xFF80];
    } else if (inRange(address, 0xFFFF, 0xFFFF)) {
        // IE register
        return 0xFF;
    }

    return 0xFF;
}

void Bus::write(word address, byte data) {
    if (inRange(address, 0x0000, 0x7FFF)) {
        cart->write(address, data);
    } else if (inRange(address, 0x8000, 0x9FFF)) {
        // PPU
    } else if (inRange(address, 0xA000, 0xBFFF)) {
        cart->write(address, data);
    } else if (inRange(address, 0xC000, 0xDFFF)) {
        wram[address - 0xC000] = data;
    } else if (inRange(address, 0xE000, 0xFDFF)) {
        wram[address - 0xE000] = data;
    } else if (inRange(address, 0xFE00, 0xFE9F)) {
        // PPU
    } else if (inRange(address, 0xFEA0, 0xFEFF)) {
        // Do Nothing
    } else if (inRange(address, 0xFF80, 0xFFFE)) {
        hram[address - 0xFF80] = data;
    } else if (inRange(address, 0xFFFF, 0xFFFF)) {
        // IE register
    }
}
