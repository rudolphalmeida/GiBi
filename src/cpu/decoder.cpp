//
// Created by Rudolph Almeida on 24/2/21.
//

#include <cpu/decoder.h>

Opcode decodeUnprefixedOpcode(byte code) {
    uint opcodeLength = NON_CB_LENGTHS[code];
    uint opcodeClocks = NON_CB_CLOCK_CYCLES[code];
}

//Opcode decodePrefixedOpcode(byte code) {
//    return Opcode(0, __1::basic_string(), 0, 0, false, __1::function());
//}
