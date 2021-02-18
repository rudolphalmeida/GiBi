/*
 * LCD Status (0xFF41) is used to store the current mode of the LCD, and enable LCDStat
 * interrupts
 *
 * Author: Rudolph Almeida <rudolf1.almeida@gmail.com>
 * */
#ifndef GIBI_LCDSTATUS_H
#define GIBI_LCDSTATUS_H

#include "gibi.h"
#include "mmu/memory.h"

enum class LCDMode : byte { HBlank = 0, VBlank = 1, AccessingOAM = 2, AccessingVRAM = 3 };

class LCDStatus : public Memory {
   private:
    byte data;

   public:
    // TODO: What does STAT default to?
    LCDStatus() : data{} { setMode(LCDMode::AccessingOAM); }

    [[nodiscard]] byte getData() const { return data; }

    void setData(byte d) { data = d; }

    // Corresponds to STAT.6
    [[nodiscard]] bool coincidenceInterruptEnabled() const { return isSet(data, 6); }

    // Corresponds to STAT.5
    [[nodiscard]] bool mode2OAMInterruptEnabled() const { return isSet(data, 5); }

    // Corresponds to STAT.4
    [[nodiscard]] bool mode1VBlankInterruptEnabled() const { return isSet(data, 4); }

    // Corresponds to STAT.3
    [[nodiscard]] bool mode0HBlankInterruptEnabled() const { return isSet(data, 3); }

    // Corresponds to STAT.2
    [[nodiscard]] bool coincidenceFlag() const { return isSet(data, 2); }

    // Corresponds to STAT.10
    [[nodiscard]] LCDMode mode() const {
        switch (data & 0b11) {
            case 0b00:
                return LCDMode::HBlank;
            case 0b01:
                return LCDMode::VBlank;
            case 0b10:
                return LCDMode::AccessingOAM;
            case 0b11:
                return LCDMode::AccessingVRAM;
            default:
                return LCDMode::VBlank;  // Really don't need this and nothing makes sense
        }
    }

    void setMode(LCDMode mode) { data = (data & 0b01111100) | static_cast<byte>(mode); }

    [[nodiscard]] byte read(word) const override { return data; }

    void write(word, byte d) override { data = (data & 0b00000111) | (d & 0b01111000); }

};

#endif  // GIBI_LCDSTATUS_H
