
cmake_minimum_required(VERSION 3.20)

project(CPU_Engine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2 REQUIRED IMPORTED_TARGET sdl2)

add_executable(CPU_Engine
        CPU_Engine/main.cpp
        CPU_Engine/Display.cpp
        CPU_Engine/Framebuffer.cpp
        CPU_Engine/vec.h
)

target_link_libraries(CPU_Engine PRIVATE
        PkgConfig::SDL2
)

target_compile_definitions(CPU_Engine PRIVATE
        SDL_MAIN_HANDLED
)
