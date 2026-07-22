cmake_minimum_required(VERSION 3.26)

project(DenoiseMachineX VERSION 0.1.0 LANGUAGES CXX)

option(BUILD_CUDA       "Build CUDA backends"             ON)
option(BUILD_TESTING    "Build tests"                     ON)

option(DMX_ENABLE_HEAVY_TESTS "Enable heavy/slow tests"   ON)
if(DMX_ENABLE_HEAVY_TESTS)
    add_compile_definitions(DMX_ENABLE_HEAVY_TESTS=1)
else()
    add_compile_definitions(DMX_ENABLE_HEAVY_TESTS=0)
endif()


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Log level by Default 0 = TRACE
set(DMX_MIN_LOG_LEVEL "0" CACHE STRING "Compile-time minimum log level")
add_compile_definitions(DMX_MIN_LOG_LEVEL=${DMX_MIN_LOG_LEVEL})

# Output dirs
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

if(POLICY CMP0169)
  cmake_policy(SET CMP0169 OLD)
endif()

# Fetch OpenEXR and Imath
include(FetchContent)
FetchContent_Declare(Imath
  GIT_REPOSITORY https://github.com/AcademySoftwareFoundation/Imath.git
  GIT_TAG v3.1.10
)
FetchContent_MakeAvailable(Imath)
FetchContent_Declare(OpenEXR
  GIT_REPOSITORY https://github.com/AcademySoftwareFoundation/openexr.git
  GIT_TAG v3.2.4
)
FetchContent_MakeAvailable(OpenEXR)

# Sources
file(GLOB_RECURSE SRC_CPP CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE SRC_CU  CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/src/*.cu)
set(CLI_MAIN ${CMAKE_SOURCE_DIR}/cli/dmxdenoiser/main.cpp)

if(BUILD_CUDA)
  enable_language(CUDA)
  find_package(CUDAToolkit REQUIRED)
  add_compile_definitions(DMX_ENABLE_CUDA=1)
  if(NOT DEFINED CMAKE_CUDA_ARCHITECTURES)
    set(CMAKE_CUDA_ARCHITECTURES "75;86;87;89;90;120" CACHE STRING "CUDA architectures")
  endif()
else()
  add_compile_definitions(DMX_ENABLE_CUDA=0)
  set(SRC_CU)
endif()

# Executable
add_executable(dmxdenoiser
  ${CLI_MAIN}
  ${SRC_CPP}
  ${SRC_CU}
)

target_include_directories(dmxdenoiser PRIVATE
  ${CMAKE_SOURCE_DIR}/include
  ${imath_SOURCE_DIR}/src
  ${imath_BINARY_DIR}/config
  ${openexr_SOURCE_DIR}/src/lib
  ${openexr_BINARY_DIR}/config
)

target_link_libraries(dmxdenoiser
  PRIVATE
    Imath::Imath
    OpenEXR::OpenEXR
)

if(BUILD_CUDA)
  target_link_libraries(dmxdenoiser PRIVATE CUDA::cudart CUDA::cuda_driver)
  set_target_properties(dmxdenoiser PROPERTIES
    CUDA_SEPARABLE_COMPILATION ON
    CUDA_ARCHITECTURES "${CMAKE_CUDA_ARCHITECTURES}"
  )
endif()

# Silence MSVC deprecation spam from OpenEXR threadpool atomics
if(MSVC)
  add_compile_definitions(_SILENCE_CXX20_OLD_SHARED_PTR_ATOMIC_SUPPORT_DEPRECATION_WARNING)
endif()

# Tests
if(BUILD_TESTING)
  include(CTest)
  enable_testing()

  FetchContent_Declare(googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.14.0
  )
  FetchContent_MakeAvailable(googletest)

  file(GLOB TEST_SOURCES CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/tests/*.cpp)
  foreach(test_src ${TEST_SOURCES})
    get_filename_component(test_name ${test_src} NAME_WE)
    add_executable(${test_name}
      ${SRC_CPP}
      ${SRC_CU}
      ${test_src}
    )

    target_include_directories(${test_name} PRIVATE
      ${CMAKE_SOURCE_DIR}/include
      ${imath_SOURCE_DIR}/src
      ${imath_BINARY_DIR}/config
      ${openexr_SOURCE_DIR}/src/lib
      ${openexr_BINARY_DIR}/config
    )

    target_link_libraries(${test_name} PRIVATE
      gtest_main
      Imath::Imath
      OpenEXR::OpenEXR
    )

    if(BUILD_CUDA)
      target_link_libraries(${test_name} PRIVATE CUDA::cudart CUDA::cuda_driver)
      set_target_properties(${test_name} PROPERTIES
        CUDA_SEPARABLE_COMPILATION ON
        CUDA_ARCHITECTURES "${CMAKE_CUDA_ARCHITECTURES}"
      )
    endif()

    if(MSVC)
      target_compile_definitions(${test_name}
        PUBLIC _SILENCE_CXX20_OLD_SHARED_PTR_ATOMIC_SUPPORT_DEPRECATION_WARNING)
    endif()

    add_test(NAME ${test_name} COMMAND ${test_name})
  endforeach()
endif()
