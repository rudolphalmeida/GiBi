cmake_minimum_required(VERSION 3.17)
project(GiBi)

set(CMAKE_CXX_STANDARD 17)

set(HEADER_FILES
        include/gibi.h
        include/GameBoy.h
        include/cpu/cpu.h
        include/constants.h
        include/cpu/opcode.h
        include/mmu/memory.h
        include/mmu/cartridge.h
        include/mmu/bus.h include/cpu/instructions.h include/cpu/interrupts.h include/cpu/timer.h)
set(SRC_FILES
        src/cpu/cpu.cpp
        src/mmu/bus.cpp
        src/mmu/cartridge.cpp
        src/GameBoy.cpp src/cpu/timer.cpp)

find_package(args CONFIG REQUIRED)

add_executable(GiBi src/main.cpp ${HEADER_FILES} ${SRC_FILES})
target_include_directories(GiBi PRIVATE include)
target_link_libraries(GiBi PRIVATE taywee::args)
target_compile_options(GiBi PRIVATE
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
        -Wall -Wextra -Werror>
        $<$<CXX_COMPILER_ID:MSVC>:
        /W4 /WX>)