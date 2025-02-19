cmake_minimum_required(VERSION 3.31)
project(Mandelbrot_Project VERSION 0.1)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(Mandelbrot main.cpp)

add_subdirectory(external/raylib)
target_include_directories(Mandelbrot PRIVATE external/raylib/src)
target_link_libraries(Mandelbrot raylib)