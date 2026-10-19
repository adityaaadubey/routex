cmake_minimum_required(VERSION 3.10)
project(RouteX VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Include directories
include_directories(${PROJECT_SOURCE_DIR}/include)

# Source files
set(SOURCES
    src/main.cpp
)

# Executable target
add_executable(RouteX ${SOURCES})

# Target include directories
target_include_directories(RouteX PRIVATE ${PROJECT_SOURCE_DIR}/include)

# Compiler flags for warnings and optimization
if(MSVC)
    target_compile_options(RouteX PRIVATE /W4 /O2)
else()
    target_compile_options(RouteX PRIVATE -Wall -Wextra -O2)
endif()
