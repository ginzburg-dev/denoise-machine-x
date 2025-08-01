cmake_minimum_required(VERSION 3.15)
project(DenoiseMachineX VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g")

# Output binaries to "bin/"
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/dmxdenoiser)

find_package(OpenEXR REQUIRED)
find_package(Imath REQUIRED)

if(NOT OpenEXR_FOUND OR NOT Imath_FOUND)
    message(FATAL_ERROR
        "\nOpenEXR and/or Imath not found!\n"
        "Please install them before configuring this project.\n"
        "On macOS, run:\n"
        "    brew install openexr imath\n"
        "On Ubuntu/Debian, run:\n"
        "    sudo apt install libopenexr-dev libimath-dev\n"
        "On Windows (vcpkg):\n"
        "    vcpkg install openexr\n"
    )
endif()

# First, try to find just the right config files
find_package(Imath CONFIG)
if (NOT TARGET Imath::Imath)
    # Couldn't find Imath::Imath, maybe it's older and has IlmBase?
    find_package(IlmBase CONFIG)
endif ()
find_package(OpenEXR CONFIG)

# Include headers
include_directories(${CMAKE_SOURCE_DIR}/include)

# Source files
file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS
    src/*.cpp
)

# CLI executable
add_executable(dmxdenoiser ${SRC_FILES} cli/main.cpp)

target_link_libraries(dmxdenoiser
    PRIVATE
        OpenEXR::OpenEXR
        Imath::Imath
        #Imath::Half
)

target_link_libraries (dmxdenoiser
    PRIVATE
        # For OpenEXR/Imath 3.x:
        $<$<TARGET_EXISTS:OpenEXR::OpenEXR>:OpenEXR::OpenEXR>
        $<$<TARGET_EXISTS:Imath::Imath>:Imath::Imath>
        $<$<TARGET_EXISTS:Imath::Half>:Imath::Half>
        # For OpenEXR 2.4/2.5:
        $<$<TARGET_EXISTS:OpenEXR::IlmImf>:OpenEXR::IlmImf>
        $<$<TARGET_EXISTS:IlmBase::Imath>:IlmBase::Imath>
        $<$<TARGET_EXISTS:IlmBase::Half>:IlmBase::Half>
        $<$<TARGET_EXISTS:IlmBase::IlmThread>:IlmBase::IlmThread>
        $<$<TARGET_EXISTS:IlmBase::Iex>:IlmBase::Iex>
    )

# Add Google test packages
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.14.0  # Use latest stable release
)
FetchContent_MakeAvailable(googletest)

#tests
enable_testing()

file(GLOB TEST_SOURCES tests/*.cpp)

#Source files for tests
file(GLOB_RECURSE CORE_SRC_FILES CONFIGURE_DEPENDS
    src/*.cpp
)
list(REMOVE_ITEM CORE_SRC_FILES ${CMAKE_SOURCE_DIR}/src/cli/main.cpp)

foreach(test_src ${TEST_SOURCES})
    get_filename_component(test_name ${test_src} NAME_WE)
    add_executable(${test_name} ${CORE_SRC_FILES} ${test_src})
    target_include_directories(${test_name} PRIVATE ${CMAKE_SOURCE_DIR}/include)
    target_link_libraries(${test_name} PRIVATE OpenEXR::OpenEXR Imath::Imath)
    target_link_libraries(${test_name}
        PRIVATE
            gtest_main  # for main(), or gtest for custom main()
            #dmxdenoiser # your actual library/target
    )
    add_test(NAME ${test_name} COMMAND ${test_name})
endforeach()



