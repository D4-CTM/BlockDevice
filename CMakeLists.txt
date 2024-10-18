cmake_minimum_required(VERSION 3.5)
project("FileManager")

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED 14)

add_executable(${PROJECT_NAME}  main.cpp
                                ./tools/Partition.hpp
)
