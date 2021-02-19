/*
 * Project-wide utilities and constants
 * Author: Rudolph Almeida <rudolf1.almeida@gmail.com>
 * */

#ifndef GIBI_INCLUDE_GIBI_H_
#define GIBI_INCLUDE_GIBI_H_

#include <cstdint>
#include <fstream>
#include <ios>
#include <utility>
#include <vector>

using byte = uint8_t;
using word = uint16_t;
using uint = uint32_t;
using sbyte = int8_t;  // Signed byte

// Check if bit of value is set (i.e. 1)
template <typename U>
inline bool isSet(U value, byte bit) {
    return (value & (0b1u << bit)) != 0;
}

// Similar to isSet excepts returns 1 if set or 0 is not set
template <typename U>
inline byte bitValue(U value, byte bit) {
    return isSet(value,  bit) ? 1 : 0;
}

// Set bit of value to 1
template <typename U>
inline byte setBit(U value, byte bit) {
    return (value | (0b1u << bit));
}

// Reset bit of value to 0
template <typename U>
inline byte resetBit(U value, byte bit) {
    return (value & ~(0b1u << bit));
}

// Compose a word from two bytes
inline word composeWord(byte msb, byte lsb) {
    return (word(msb) << 8u) | word(lsb);
}

// Decompose a word into two bytes
inline std::pair<byte, byte> decomposeWord(word value) {
    auto msb = byte((value >> 8u) & 0xFFu);
    auto lsb = byte(value & 0xFFu);

    return {msb, lsb};
}

// Check if addition of two bytes results in a half-carry
inline bool willHalfCarry8BitAdd(byte left, byte right) {
    return ((left & 0xFu) + (right & 0xFu)) > 0xFu;
}

// Check if addition of two words results in a half-carry
inline bool willHalfCarry16BitAdd(word left, word right) {
    return (left & 0xFFFu) + (right & 0xFFFu) > 0xFFFu;
}

// Check if subtraction of two bytes results in a half-borrow
inline bool willHalfCarry8BitSub(byte left, byte right) {
    return ((left & 0xF) - (right & 0xF) < 0);
}

// Check if an address is in a particular range: [start, end]
inline bool inRange(word address, word start, word end) {
    return (address >= start) && (address <= end);
}

#endif  // GIBI_INCLUDE_GIBI_H_