//
// Created by Rudolph Almeida on 10/01/2021.
//

#ifndef GIBI_INCLUDE_GAMEBOY_H_
#define GIBI_INCLUDE_GAMEBOY_H_

#include <memory>

#include "cpu/cpu.h"
#include "gibi.h"
#include "mmu/bus.h"

// Read a binary file from disk
std::vector<byte> readBinaryToVec(const char* filename);

class GameBoy {
   public:
    GameBoy(int argc, char** argv);
};

#endif  // GIBI_INCLUDE_GAMEBOY_H_
