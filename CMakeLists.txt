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

# ---- Dependencies: OpenEXR + Imath (prefer CONFIG, fallback to modules)
# Try config packages first (v3+)
find_package(Imath CONFIG QUIET)
find_package(OpenEXR CONFIG QUIET)

# Fallback to classic find modules (v2.x dev packages)
if(NOT Imath_FOUND)
    find_package(Imath CONFIG REQUIRED)       # provides Imath::Imath and/or IlmBase::Imath
endif()
if(NOT OpenEXR_FOUND)
    find_package(OpenEXR CONFIG REQUIRED)     # provides OpenEXR::OpenEXR or legacy targets
endif()


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

# Include headers
include_directories(${CMAKE_SOURCE_DIR}/include)

# ---- Sources (CPP always; CU optional when BUILD_CUDA=ON)
file(GLOB_RECURSE SRC_CPP CONFIGURE_DEPENDS
  ${CMAKE_SOURCE_DIR}/src/*.cpp
)

# keep CLI separate so we can add it explicitly to the exe
set(CLI_MAIN ${CMAKE_SOURCE_DIR}/cli/main.cpp)


if(BUILD_CUDA)
  enable_language(CUDA)
  find_package(CUDAToolkit REQUIRED)
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


