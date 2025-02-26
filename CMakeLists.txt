cmake_minimum_required(VERSION 3.20)

project(chip8_emu LANGUAGES C)

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

add_subdirectory(src)