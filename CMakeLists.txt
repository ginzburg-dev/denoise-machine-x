cmake_minimum_required(VERSION 3.26)

project(DenoiseMachineX VERSION 0.1.0 LANGUAGES CXX)

option(BUILD_CUDA       "Build CUDA backends"       ON)
option(BUILD_TESTING    "Build tests"               ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Log level by Default 0 = TRACE
set(DMX_MIN_LOG_LEVEL "0" CACHE STRING "Compile-time minimum log level")
add_compile_definitions(DMX_MIN_LOG_LEVEL=${DMX_MIN_LOG_LEVEL})

# Output dirs
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

include(FetchContent)

# ---- Dependencies from vcpkg
find_package(Imath CONFIG REQUIRED)       # provides Imath::Imath
find_package(OpenEXR CONFIG REQUIRED)     # provides OpenEXR::OpenEXR
# find_package(ZLIB REQUIRED)             # usually not needed explicitly

# ---- Sources
file(GLOB_RECURSE SRC_CPP CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/src/*.cpp)
set(CLI_MAIN ${CMAKE_SOURCE_DIR}/cli/main.cpp)

if(BUILD_CUDA)
  enable_language(CUDA)
  find_package(CUDAToolkit REQUIRED)
  add_compile_definitions(DMX_ENABLE_CUDA=1)
  file(GLOB_RECURSE SRC_CU CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/src/*.cu)
else()
  add_compile_definitions(DMX_ENABLE_CUDA=0)
  set(SRC_CU)
endif()

# ---- Executable
add_library(dmxcore ${SRC_CPP} ${SRC_CU})

# Public includes: your headers + Imath/OpenEXR parents (so OpenEXR/… and Imath/… resolve)
target_include_directories(dmxcore
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(dmxcore
  PUBLIC
    Imath::Imath
    OpenEXR::OpenEXR
)

if(BUILD_CUDA)
  target_compile_definitions(dmxcore PUBLIC DMX_ENABLE_CUDA=1)
  target_link_libraries(dmxcore PRIVATE CUDA::cudart CUDA::cuda_driver)
  set_target_properties(dmxcore PROPERTIES
    CUDA_SEPARABLE_COMPILATION ON
    CUDA_ARCHITECTURES native
  )
else()
  target_compile_definitions(dmxcore PUBLIC DMX_ENABLE_CUDA=0)
endif()

add_executable(dmxdenoiser ${CLI_MAIN})
target_link_libraries(dmxdenoiser PRIVATE dmxcore)

# Silence MSVC deprecation spam from OpenEXR threadpool atomics
if(MSVC)
  add_compile_definitions(_SILENCE_CXX20_OLD_SHARED_PTR_ATOMIC_SUPPORT_DEPRECATION_WARNING)
endif()

# ---- Tests
if(BUILD_TESTING)
  include(CTest)
  FetchContent_Declare(googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.14.0
  )
  FetchContent_MakeAvailable(googletest)

  file(GLOB TEST_SOURCES CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/tests/*.cpp)
  foreach(test_src ${TEST_SOURCES})
    get_filename_component(test_name ${test_src} NAME_WE)
    add_executable(${test_name} ${test_src})
    target_link_libraries(${test_name}
      PRIVATE
        gtest_main
        dmxcore       # <- reuse your main library/exe as a link target
    )
    if(MSVC)
      target_compile_definitions(${test_name}
        PUBLIC _SILENCE_CXX20_OLD_SHARED_PTR_ATOMIC_SUPPORT_DEPRECATION_WARNING)
    endif()
    add_test(NAME ${test_name} COMMAND ${test_name})
  endforeach()
endif()
