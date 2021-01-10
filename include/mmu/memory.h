/*
 * Pure abstract class for all things that are memory-mapped, which includes the
 * RAM, ROM, PPU, timers, serial, and cartridges
 * Author: Rudolph Almeida <rudolf1.almeida@gmail.com>
 * */

#ifndef GIBI_INCLUDE_MMU_MEMORY_H_
#define GIBI_INCLUDE_MMU_MEMORY_H_

#include "gibi.h"

class Memory {
   public:
    [[nodiscard]] virtual byte read(word address) const = 0;
    virtual void write(word address, byte data) = 0;
};

#endif  // GIBI_INCLUDE_MMU_MEMORY_H_
