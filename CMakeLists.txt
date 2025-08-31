cmake_minimum_required(VERSION 3.26)

project(DenoiseMachineX VERSION 0.1.0 LANGUAGES CXX CUDA)

option(BUILD_CUDA     "Build CUDA backends"   ON)
option(BUILD_TESTING  "Build tests"           ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Output dirs
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

include(FetchContent)

# ---- Imath & OpenEXR (v3)
#FetchContent_Declare(Imath
#  GIT_REPOSITORY https://github.com/AcademySoftwareFoundation/Imath.git
#  GIT_TAG v3.1.10
#)
#FetchContent_MakeAvailable(Imath)
#
#FetchContent_Declare(OpenEXR
#  GIT_REPOSITORY https://github.com/AcademySoftwareFoundation/openexr.git
#  GIT_TAG v3.2.4
#)
#FetchContent_MakeAvailable(OpenEXR)

# ---- Dependencies from vcpkg
find_package(Imath CONFIG REQUIRED)       # provides Imath::Imath
find_package(OpenEXR CONFIG REQUIRED)     # provides OpenEXR::OpenEXR
# find_package(ZLIB REQUIRED)             # usually not needed explicitly

# ---- Sources
file(GLOB_RECURSE SRC_CPP CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/src/*.cpp)
set(CLI_MAIN ${CMAKE_SOURCE_DIR}/cli/main.cpp)

set(SRC_CU)
if(BUILD_CUDA)
  enable_language(CUDA)
  find_package(CUDAToolkit REQUIRED)
  file(GLOB_RECURSE SRC_CU CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/src/*.cu)
endif()

# ---- Executable
add_executable(dmxdenoiser ${SRC_CPP} ${SRC_CU} ${CLI_MAIN})

# Public includes: your headers + Imath/OpenEXR parents (so OpenEXR/… and Imath/… resolve)
target_include_directories(dmxdenoiser
  BEFORE PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${imath_SOURCE_DIR}/src>          # parent of Imath/
    $<BUILD_INTERFACE:${imath_BINARY_DIR}/config>
    $<BUILD_INTERFACE:${openexr_SOURCE_DIR}/src/lib>    # parent of OpenEXR/
    $<BUILD_INTERFACE:${openexr_BINARY_DIR}/config>
)

target_link_libraries(dmxdenoiser
  PUBLIC
    Imath::Imath
    OpenEXR::OpenEXR
)

if(BUILD_CUDA)
  target_compile_definitions(dmxdenoiser PUBLIC DMX_ENABLE_CUDA=1)
  target_link_libraries(dmxdenoiser PRIVATE CUDA::cudart CUDA::cuda_driver)
  set_target_properties(dmxdenoiser PROPERTIES
    CUDA_SEPARABLE_COMPILATION ON
    CUDA_ARCHITECTURES native
  )
else()
  target_compile_definitions(dmxdenoiser PUBLIC DMX_ENABLE_CUDA=0)
endif()

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
    add_executable(${test_name} ${SRC_CPP} ${SRC_CU} ${test_src})
    target_include_directories(${test_name}
      BEFORE PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${imath_SOURCE_DIR}/src
        ${imath_BINARY_DIR}/config
        ${openexr_SOURCE_DIR}/src/lib
        ${openexr_BINARY_DIR}/config
    )
    target_link_libraries(${test_name} PRIVATE gtest_main Imath::Imath OpenEXR::OpenEXR)
    if(BUILD_CUDA)
      target_compile_definitions(${test_name} PUBLIC DMX_ENABLE_CUDA=1)
      target_link_libraries(${test_name} PRIVATE CUDA::cudart CUDA::cuda_driver)
      set_target_properties(${test_name} PROPERTIES
        CUDA_SEPARABLE_COMPILATION ON
        CUDA_ARCHITECTURES native
      )
    else()
      target_compile_definitions(${test_name} PUBLIC DMX_ENABLE_CUDA=0)
    endif()
    if(MSVC)
      target_compile_definitions(${test_name}
        PUBLIC _SILENCE_CXX20_OLD_SHARED_PTR_ATOMIC_SUPPORT_DEPRECATION_WARNING)
    endif()
    add_test(NAME ${test_name} COMMAND ${test_name})
  endforeach()
endif()
