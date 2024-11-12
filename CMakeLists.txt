cmake_minimum_required(VERSION 3.5)
project("FileManager")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED 17)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_SOURCE_DIR}/tools)

add_executable(${PROJECT_NAME}  main.cpp
                                ./tools/BlockDevice.hpp
                                ./tools/BlockDevice.cpp
                                ./tools/TextColor.h
                                ./tools/Command_Line.hpp
                                ./tools/Command_Line.cpp
)
