/*
 * Programmatically try to decode GBU opcodes and their arguments. This is to avoid the huge vector
 * of opcodes or two very big switch statements.
 *
 * Reference: https://gb-archive.github.io/salvage/decoding_gbz80_opcodes/Decoding%20Gamboy%20Z80%20Opcodes.html
 * & SM83_decoding.pdf
 *
 * Author: Rudolph Almeida <rudolf1.almeida@gmail.com>
 * */

#ifndef GIBI_DECODER_H
#define GIBI_DECODER_H

#include "gibi.h"
#include "opcode.h"


Opcode decodeUnprefixedOpcode(byte code);

//Opcode decodePrefixedOpcode(byte code);

#endif  // GIBI_DECODER_H
