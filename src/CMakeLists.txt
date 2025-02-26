cmake_minimum_required(VERSION 3.20)

set(CHIP8_DISASSEMBLER_SOURCES
    disassembler.c)

add_executable(disassembler ${CHIP8_DISASSEMBLER_SOURCES})