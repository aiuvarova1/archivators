cmake_minimum_required(VERSION 3.15)
project(kdz)

set(CMAKE_CXX_STANDARD 14)

add_executable(kdz main.cpp Fano.cpp Fano.h LZ.cpp LZ.h)