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

# Define macro for importing GitHub repos
include(FetchContent)

macro(_add_engine_git_module NAME REPO COMMIT)
  message(STATUS "Module: ${NAME} @ ${COMMIT}")
  FetchContent_Declare(${NAME}
    GIT_REPOSITORY ${REPO}
    GIT_TAG        ${COMMIT}      # exact commit from lockfile
    GIT_SHALLOW    TRUE
    UPDATE_DISCONNECTED FALSE
  )
  FetchContent_MakeAvailable(${NAME})
endmacro()

_add_engine_git_module(GLM https://github.com/g-truc/glm.git master)
target_include_directories(WVCore PUBLIC ${glm_SOURCE_DIR})
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