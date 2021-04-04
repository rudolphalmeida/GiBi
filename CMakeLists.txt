cmake_minimum_required(VERSION 3.17)
project(GiBi)

set(CMAKE_CXX_STANDARD 17)

set(HEADER_FILES
        include/gibi.h
        include/gameboy.h
        include/cpu/cpu.h
        include/cpu/opcode_info.h
        include/mmu/memory.h
        include/mmu/cartridge.h
        include/mmu/bus.h
        include/cpu/interrupts.h
        include/cpu/timer.h
        include/joypad.h
        include/serial.h
        include/ppu/lcdc.h
        include/ppu/lcdstatus.h
        include/ppu/ppu.h
        include/options.h
        include/ui.h)
set(SRC_FILES
        src/cpu/cpu.cpp
        src/mmu/bus.cpp
        src/mmu/cartridge.cpp
        src/gameboy.cpp
        src/cpu/timer.cpp
        src/joypad.cpp
        src/ppu/ppu.cpp src/ui.cpp)

find_package(args CONFIG REQUIRED)
find_package(SDL2 CONFIG REQUIRED)

add_executable(GiBi src/main.cpp ${HEADER_FILES} ${SRC_FILES})
target_include_directories(GiBi PRIVATE include)
target_link_libraries(GiBi PRIVATE taywee::args SDL2::SDL2main SDL2::SDL2-static)
target_compile_options(GiBi PRIVATE
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
        -Wall -Wextra -Werror>
        $<$<CXX_COMPILER_ID:MSVC>:
        /W4 /WX>)