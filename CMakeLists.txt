cmake_minimum_required(VERSION 3.23)

project(DenoiseMachineX
    VERSION 0.1.0
    LANGUAGES CXX
)

# ---- Options
option(BUILD_CUDA     "Build CUDA backends"     ON)
option(BUILD_TESTING  "Build tests"             ON)

# ---- C++ setup
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g")

# Output dirs (bin/ lib/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)


if(BUILD_CUDA)
  # Try to locate nvcc explicitly
  find_program(NVCC_EXECUTABLE
    NAMES nvcc nvcc.exe
    HINTS "$ENV{CUDA_PATH}/bin"
    PATHS
      "C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v13.0/bin"
      "C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v12.4/bin"
      "C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v12.3/bin"
  )

  if(NVCC_EXECUTABLE AND EXISTS "${NVCC_EXECUTABLE}")
    set(CMAKE_CUDA_COMPILER "${NVCC_EXECUTABLE}" CACHE FILEPATH "nvcc" FORCE)
    message(STATUS "Using CUDA compiler: ${CMAKE_CUDA_COMPILER}")
    enable_language(CUDA)
    find_package(CUDAToolkit REQUIRED)
    # Good defaults
    set(CMAKE_CUDA_STANDARD 17)
    set(CMAKE_CUDA_STANDARD_REQUIRED ON)
    set(CMAKE_CUDA_ARCHITECTURES native) # or 75;86;89
    add_compile_definitions(DMX_ENABLE_CUDA=1)
  else()
    message(WARNING
      "CUDA not configured: nvcc not found. "
      "Set CUDA_PATH or install CUDA, or pass -DCMAKE_CUDA_COMPILER=... . "
      "Building without GPU.")
    set(BUILD_CUDA OFF)
    add_compile_definitions(DMX_ENABLE_CUDA=0)
  endif()
endif()


if(BUILD_CUDA)
  # 1) Locate nvcc (your existing logic is fine)
  find_program(NVCC_EXECUTABLE
    NAMES nvcc nvcc.exe
    HINTS "$ENV{CUDA_PATH}/bin"
  )

  if(NVCC_EXECUTABLE)
    set(CMAKE_CUDA_COMPILER "${NVCC_EXECUTABLE}" CACHE FILEPATH "nvcc" FORCE)

    # 2) Reuse the MSVC compiler CMake already detected
    if(MSVC AND CMAKE_CXX_COMPILER)
      set(CMAKE_CUDA_HOST_COMPILER
          "${CMAKE_CXX_COMPILER}"
          CACHE FILEPATH "CUDA host compiler" FORCE)
    endif()

    enable_language(CUDA)
    find_package(CUDAToolkit REQUIRED)

    set(CMAKE_CUDA_STANDARD 17)
    set(CMAKE_CUDA_STANDARD_REQUIRED ON)
    set(CMAKE_CUDA_ARCHITECTURES native)
    add_compile_definitions(DMX_ENABLE_CUDA=1)
  else()
    message(WARNING "nvcc not found → building without GPU. Set CUDA_PATH or pass -DCMAKE_CUDA_COMPILER=...")
    set(BUILD_CUDA OFF)
    add_compile_definitions(DMX_ENABLE_CUDA=0)
  endif()
endif()


# Prefer config packages first
find_package(Imath   CONFIG QUIET)
find_package(OpenEXR CONFIG QUIET)

# Fallback to vendoring
if(NOT TARGET Imath::Imath)
  include(FetchContent)
  message(STATUS "Fetching Imath…")
  FetchContent_Declare(
    imath
    GIT_REPOSITORY https://github.com/AcademySoftwareFoundation/Imath.git
    GIT_TAG        v3.1.11
  )
  set(IMATH_BUILD_TESTS OFF CACHE BOOL "" FORCE)
  set(IMATH_BUILD_TOOLS OFF CACHE BOOL "" FORCE)
  set(IMATH_INSTALL OFF CACHE BOOL "" FORCE)
  FetchContent_MakeAvailable(imath)

  # IMPORTANT: tell downstream (OpenEXR) where Imath's config is
  # Use the *build* config dir:
  set(Imath_DIR "${imath_BINARY_DIR}/config" CACHE PATH "" FORCE)
endif()

if(NOT TARGET OpenEXR::OpenEXR)
  if(NOT DEFINED FETCHCONTENT_BASE_DIR)
    include(FetchContent)
  endIf()
  message(STATUS "Fetching OpenEXR…")
  FetchContent_Declare(
    openexr
    GIT_REPOSITORY https://github.com/AcademySoftwareFoundation/openexr.git
    GIT_TAG        v3.2.4
  )
  set(OPENEXR_BUILD_UTILS OFF CACHE BOOL "" FORCE)
  set(OPENEXR_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
  set(OPENEXR_BUILD_TESTS OFF CACHE BOOL "" FORCE)
  set(OPENEXR_INSTALL OFF CACHE BOOL "" FORCE)

  # Now OpenEXR's CMake will find the Imath we built above
  FetchContent_MakeAvailable(openexr)
endif()

# Sanity: targets must exist now (either system or vendored)
if(NOT TARGET Imath::Imath)
  message(FATAL_ERROR "Imath::Imath target not available")
endif()
if(NOT TARGET OpenEXR::OpenEXR)
  message(FATAL_ERROR "OpenEXR::OpenEXR target not available")
endif()


# Include headers
include_directories(${CMAKE_SOURCE_DIR}/include)

# ---- Sources (CPP always; CU optional when BUILD_CUDA=ON)
file(GLOB_RECURSE SRC_CPP CONFIGURE_DEPENDS
  ${CMAKE_SOURCE_DIR}/src/*.cpp
)

# keep CLI separate so we can add it explicitly to the exe
set(CLI_MAIN ${CMAKE_SOURCE_DIR}/cli/main.cpp)


if(BUILD_CUDA)
  add_compile_definitions(DMX_ENABLE_CUDA=1)
  file(GLOB_RECURSE SRC_CU CONFIGURE_DEPENDS
    ${CMAKE_SOURCE_DIR}/src/*.cu
  )
else()
  add_compile_definitions(DMX_ENABLE_CUDA=0)
  set(SRC_CU)
endif()


# ---- Executable (DIRECTLY links all sources → static registrars run)
add_executable(dmxdenoiser ${SRC_CPP} ${SRC_CU} ${CLI_MAIN})


# CLI executable
#add_executable(dmxdenoiser ${SRC_FILES} cli/main.cpp)

target_include_directories(dmxdenoiser
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include
)

# Link OpenEXR/Imath via whatever targets exist
target_link_libraries(dmxdenoiser
    PUBLIC
        $<$<TARGET_EXISTS:OpenEXR::OpenEXR>:OpenEXR::OpenEXR>
        $<$<TARGET_EXISTS:Imath::Imath>:Imath::Imath>
        $<$<TARGET_EXISTS:Imath::Half>:Imath::Half>
        $<$<TARGET_EXISTS:OpenEXR::IlmImf>:OpenEXR::IlmImf>
        $<$<TARGET_EXISTS:IlmBase::Imath>:IlmBase::Imath>
        $<$<TARGET_EXISTS:IlmBase::Half>:IlmBase::Half>
        $<$<TARGET_EXISTS:IlmBase::IlmThread>:IlmBase::IlmThread>
        $<$<TARGET_EXISTS:Iex::Iex>:Iex::Iex>
)

if(BUILD_CUDA)
  target_link_libraries(dmxdenoiser PRIVATE CUDA::cudart CUDA::cuda_driver)
  set_target_properties(dmxdenoiser PROPERTIES CUDA_SEPARABLE_COMPILATION ON)
endif()



# ---- Tests
if(BUILD_TESTING)
    include(FetchContent)
    include(CTest)
    enable_testing()
    
    FetchContent_Declare(
      googletest
      GIT_REPOSITORY https://github.com/google/googletest.git
      GIT_TAG        v1.14.0
    )
    FetchContent_MakeAvailable(googletest)
    
    file(GLOB TEST_SOURCES CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/tests/*.cpp)
    
    foreach(test_src ${TEST_SOURCES})
      get_filename_component(test_name ${test_src} NAME_WE)
      # Build each test with the same src set (direct linkage)
      add_executable(${test_name} ${SRC_CPP} ${SRC_CU} ${test_src})
      target_include_directories(${test_name} PRIVATE ${CMAKE_SOURCE_DIR}/include)
      target_link_libraries(${test_name} PRIVATE
        gtest_main
        $<$<TARGET_EXISTS:OpenEXR::OpenEXR>:OpenEXR::OpenEXR>
        $<$<TARGET_EXISTS:Imath::Imath>:Imath::Imath>
        $<$<TARGET_EXISTS:Imath::Half>:Imath::Half>
        $<$<TARGET_EXISTS:OpenEXR::IlmImf>:OpenEXR::IlmImf>
        $<$<TARGET_EXISTS:IlmBase::Imath>:IlmBase::Imath>
        $<$<TARGET_EXISTS:IlmBase::Half>:IlmBase::Half>
        $<$<TARGET_EXISTS:IlmBase::IlmThread>:IlmBase::IlmThread>
        $<$<TARGET_EXISTS:Iex::Iex>:Iex::Iex>
      )
      if(BUILD_CUDA)
        target_link_libraries(${test_name} PRIVATE CUDA::cudart CUDA::cuda_driver)
        set_target_properties(${test_name} PROPERTIES CUDA_SEPARABLE_COMPILATION ON)
      endif()
      add_test(NAME ${test_name} COMMAND ${test_name})
    endforeach()
endif()


