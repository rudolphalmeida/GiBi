cmake_minimum_required(VERSION 3.17)
project(GiBi)

set(CMAKE_CXX_STANDARD 17)

set(HEADER_FILES
        include/gibi.h
        include/cpu/cpu.h
        include/cpu/instr_timings.h
        include/cpu/opcode.h
        include/mmu/memory.h
        include/mmu/cartridge.h
        include/mmu/bus.h)
set(SRC_FILES
        src/cpu/cpu.cpp
        src/mmu/bus.cpp
        src/mmu/cartridge.cpp)

find_package(spdlog CONFIG REQUIRED)

add_executable(GiBi src/main.cpp ${HEADER_FILES} ${SRC_FILES})
target_include_directories(GiBi PRIVATE include)
target_link_libraries(GiBi PRIVATE spdlog::spdlog spdlog::spdlog_header_only)
