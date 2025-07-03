cmake_minimum_required(VERSION 3.15)
project(DenoiseMachineX VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_XX_STANDARD _REQUIRED ON)

# Output binaries to "bin/"
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/dmxdenoiser)

# Include headers
include_directories(${CMAKE_SOURCE_DIR}/include)

# Source files
file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS
    src/*.cpp
)

# CLI executable
add_executable(dmxdenoiser ${SRC_FILES} src/cli/main.cpp)

