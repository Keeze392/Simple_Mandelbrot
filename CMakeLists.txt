cmake_minimum_required(VERSION 3.31)
project(Mandelbrot_Project VERSION 0.1)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Ensure Raylib is downloaded and built into the external folder
include(FetchContent)
FetchContent_Declare(
    raylib
    URL https://github.com/raysan5/raylib/archive/refs/tags/5.5.tar.gz
)

# Populate Raylib
FetchContent_MakeAvailable(raylib)

# Add the executable for your project
add_executable(Mandelbrot main.cpp)

# Include and link Raylib
target_link_libraries(Mandelbrot PRIVATE raylib)
