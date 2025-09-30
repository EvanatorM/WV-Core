cmake_minimum_required(VERSION 3.24)
project(WVCore LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(WVCore STATIC
    src/temp.cpp
)

target_include_directories(WVCore PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>    
)

target_compile_features(WVCore PUBLIC cxx_std_20)

# Add platform-specific preprocessor definitions
if(WIN32)
    target_compile_definitions(WVCore PUBLIC PLATFORM_WINDOWS)
elseif(APPLE)
    target_compile_definitions(WVCore PUBLIC PLATFORM_MACOS)
elseif(UNIX)
    target_compile_definitions(WVCore PUBLIC PLATFORM_LINUX)
else()
    message(FATAL_ERROR "Unknown platform!")
endif()
