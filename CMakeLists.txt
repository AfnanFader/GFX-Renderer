cmake_minimum_required(VERSION 3.22)
project(GFX-Renderer)

# C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Build Config
set(CMAKE_BUILD_TYPE debug)

add_executable(GFX-Renderer src/GFX_main.cpp)