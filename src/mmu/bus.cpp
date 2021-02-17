/*
 * Author: Rudolph Almeida <rudolf1.almeida@gmail.com>
 * */

#include "mmu/bus.h"
#include "gibi.h"

byte Bus::read(word address) const {
    if (inRange(address, 0x0000, 0x7FFF)) {
        return cart.read(address);
    } else if (inRange(address, 0x8000, 0x9FFF)) {
        return ppu.read(address);
    } else if (inRange(address, 0xA000, 0xBFFF)) {
        return cart.read(address);
    } else if (inRange(address, 0xC000, 0xDFFF)) {
        return wram[address - 0xC000];
    } else if (inRange(address, 0xE000, 0xFDFF)) {
        return wram[address - 0xE000];
    } else if (inRange(address, 0xFE00, 0xFE9F)) {
        return ppu.read(address);
    } else if (inRange(address, 0xFEA0, 0xFEFF)) {
        return 0xFF;  // Unusable space
    } else if (address == 0xFF00) {
        return joyPad.read(address);
    } else if (inRange(address, 0xFF01, 0xFF02)) {
        return serial.read(address);
    } else if (inRange(address, 0xFF04, 0xFF07)) {
        return timer.read(address);
    } else if (address == 0xFF0F) {
        return intf->data;
    } else if (inRange(address, 0xFF40, 0xFF4B) && address != 0xFF46) {
        return ppu.read(address);
    } else if (inRange(address, 0xFF80, 0xFFFE)) {
        return hram[address - 0xFF80];
    } else if (address == 0xFFFF) {
        return inte->data;
    }

    return 0xFF;
}

void Bus::write(word address, byte data) {
    if (inRange(address, 0x0000, 0x7FFF)) {
        cart.write(address, data);
    } else if (inRange(address, 0x8000, 0x9FFF)) {
        ppu.write(address, data);
    } else if (inRange(address, 0xA000, 0xBFFF)) {
        cart.write(address, data);
    } else if (inRange(address, 0xC000, 0xDFFF)) {
        wram[address - 0xC000] = data;
    } else if (inRange(address, 0xE000, 0xFDFF)) {
        wram[address - 0xE000] = data;
    } else if (inRange(address, 0xFE00, 0xFE9F)) {
        ppu.write(address, data);
    } else if (inRange(address, 0xFEA0, 0xFEFF)) {
        // Do Nothing
    } else if (address == 0xFF00) {
        joyPad.write(address, data);
    } else if (inRange(address, 0xFF01, 0xFF02)) {
        return serial.write(address, data);
    } else if (inRange(address, 0xFF04, 0xFF07)) {
        timer.write(address, data);
    } else if (address == 0xFF0F) {
        intf->data = data;
    } else if (address == 0xFF46) {
        // Run DMA. Since the program is probably waiting for the DMA to complete by running
        // a busy-wait from HRAM for about 160 machine cycles, we don't care about factoring in
        // these clock cycles in other components
        word baseAddress = static_cast<word>(data) << 8;
        for (word i = 0; i <= 0x9F; ++i) {
            byte value = read(baseAddress + i);
            write(0xFE00 + i, value);
        }
    } else if (inRange(address, 0xFF40, 0xFF4B)) {
        return ppu.write(address, data);
    } else if (inRange(address, 0xFF80, 0xFFFE)) {
        hram[address - 0xFF80] = data;
    } else if (address == 0xFFFF) {
        inte->data = data;
    }
}

void Bus::tick(uint cycles) {
    timer.tick(cycles);
}
