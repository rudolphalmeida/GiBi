#include <memory>

#include "cpu/interrupts.h"
#include "gibi.h"
#include "joypad.h"

JoyPad::JoyPad(std::shared_ptr<IntF> intf) : intf{std::move(intf)} {}

void JoyPad::keydown(JoypadKeys key) {
    keys = resetBit(keys, static_cast<byte>(key));
}

void JoyPad::keyup(JoypadKeys key) {
    keys = setBit(keys, static_cast<byte>(key));
}

void JoyPad::tick(uint cycles) {
    joypadClkCycles += cycles;

    if (joypadClkCycles >= JOYPAD_CLK_CYLES) {
        joypadClkCycles %= JOYPAD_CLK_CYLES;
        update();
    }
}

void JoyPad::update() {
    byte current = joyp & 0xF0;

    switch (current & 0x30) {
        case 0x10: {
            byte joypadTop = (keys >> 4) & 0x0F;
            current |= joypadTop;
            break;
        }
        case 0x20: {
            byte joypadBottom = keys & 0x0F;
            current |= joypadBottom;
            break;
        }
        case 0x30: {
            current |= 0x0F;
            break;
        }
    }

    if ((joyp & ~current & 0x0F) != 0)
        intf->request(Interrupts::JoyPad);

    joyp = current;
}

byte JoyPad::read(word address) const {
    if (address == 0xFF00) {
        return joyp;
    }

    return 0xFF;
}

void JoyPad::write(word address, byte data) {
    if (address == 0xFF00) {
        // Only bits 5 and 4 of the register are R/W. Bits 7 and 6 are unused and bits 3-0 are R
        // only
        joyp = (joyp & 0xCF) | (data & 0x30);
        update();
    }
}
