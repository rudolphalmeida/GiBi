/*
 * Implements the JoyPad controller in the GameBoy
 * */

#ifndef GIBI_JOYPAD_H
#define GIBI_JOYPAD_H

#include <memory>

#include "cpu/interrupts.h"
#include "gibi.h"
#include "mmu/memory.h"

// The eight keys of the GameBoy are arranged in a 2x4 matrix, and using the JOYP register,
// either the button keys or the direction keys can be selected
enum class JoypadKeys : byte {
    Right = 0x01,
    Left = 0x02,
    Up = 0x03,
    Down = 0x04,
    A = 0x10,
    B = 0x20,
    Select = 0x30,
    Start = 0x40,
};

class JoyPad : public Memory {
   private:
    byte keys;  // The matrix of keys. Bit is reset when pressed
    byte data;  // 0xFF00 register

    std::shared_ptr<IntF> intf;

   public:
    explicit JoyPad(std::shared_ptr<IntF> intf);

    void keydown(JoypadKeys key);
    void keyup(JoypadKeys key);

    [[nodiscard]] byte read(word address) const override;
    void write(word address, byte data) override;
};

#endif  // GIBI_JOYPAD_H
