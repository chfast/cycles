cmake_minimum_required(VERSION 3.15)
project(cycles)

add_executable(cycles cycles.cpp)
target_compile_features(cycles PRIVATE cxx_std_17)
