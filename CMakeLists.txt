cmake_minimum_required(VERSION 3.20)
project(WV_CORE LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(WV_CORE STATIC
    src/Test.cpp
)

target_include_directories(WV_CORE PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)