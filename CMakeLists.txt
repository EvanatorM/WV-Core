cmake_minimum_required(VERSION 3.24)
project(WVCore LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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


# Add glad
_add_engine_git_module(glad https://github.com/Dav1dde/glad.git 5bf3eda)

# Create library
add_library(WVCore STATIC
    src/app/App.cpp
    ${glad_SOURCE_DIR}/src/glad.c
)

#target_precompile_headers(WVCore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/wv/wvpch.h)
#target_precompile_headers(WVCore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/wv/core.h)

target_include_directories(WVCore PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>    
)

target_compile_features(WVCore PUBLIC cxx_std_20)

# Finish adding glad
target_include_directories(WVCore PUBLIC ${glad_SOURCE_DIR}/include)

# Add glm
_add_engine_git_module(GLM https://github.com/g-truc/glm.git 0af55cc)
target_include_directories(WVCore PUBLIC ${glm_SOURCE_DIR})

# Add GLFW
_add_engine_git_module(GLFW https://github.com/glfw/glfw.git 7b6aead)
target_link_libraries(WVCore PUBLIC glfw)
target_include_directories(WVCore PUBLIC ${glfw_SOURCE_DIR}/include)

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