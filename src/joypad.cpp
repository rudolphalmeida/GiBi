#include <joypad.h>
#include <memory>

#include "cpu/interrupts.h"
#include "gibi.h"

JoyPad::JoyPad(std::shared_ptr<IntF> intf) : keys{0xFF}, data{0x00}, intf{std::move(intf)} {}

void JoyPad::keydown(JoypadKeys key) {
    keys &= ~static_cast<byte>(key);
    intf->request(Interrupts::JoyPad);
}

void JoyPad::keyup(JoypadKeys key) {
    keys |= static_cast<byte>(key);
}

byte JoyPad::read(word address) const {
    if (address == 0xFF00) {
        if ((data & 0x10) == 0) {
            return data | (keys & 0xF);  // Direction Keys
        }

        if ((data & 0x20) == 0) {
            return data | (keys >> 4u);  // Button Keys
        }

        return data;
    }

    return 0xFF;  // Shouldn't be needed
}

void JoyPad::write(word address, byte tdata) {
    if (address == 0xFF00) {
        this->data = tdata;
    }
}
