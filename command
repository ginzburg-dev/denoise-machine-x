./dmxdenoiser 
    --start 1 
    --end 100 
    --gpu 0
    --frames 3
    --beauty default
    --albedo albedo
    --normal tech.####.exr:N 
    --depth tech.####.exr:z
    --aluxary
    --output filtered.####.exr


# Install openexr on Windows
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat
.\vcpkg\vcpkg.exe install --triplet x64-windows

grep CMAKE_BUILD_TYPE build-release/CMakeCache.txt
# should print: CMAKE_BUILD_TYPE:STRING=Release

rm -f CMakeCache.txt CMakeFiles 2>/dev/null || true

# Debug tree
Debug build & run tests:
cmake -S . -B build/debug   -DCMAKE_BUILD_TYPE=Debug
(cmake --build build-debug -j 8 && cd build-debug && ctest --output-on-failure --verbose -j 8 && cd .. && cd ..)

# Release tree
Release build & run tests:
cmake -S . -B build-release -DCMAKE_BUILD_TYPE=Release
(cmake --build build-release -j 8 && cd build-release && ctest --output-on-failure --verbose -j 8 && cd .. && cd ..)

# Windows
# 1. cleanup:
if (Test-Path build) { Remove-Item build -Recurse -Force }
if (Test-Path CMakeCache.txt) { Remove-Item CMakeCache.txt }
if (Test-Path CMakeFiles) { Remove-Item CMakeFiles -Recurse -Force }

cmake -S . -B build -G "Visual Studio 17 2022" -A x64 `
  -DCMAKE_TOOLCHAIN_FILE="$PWD\vcpkg\scripts\buildsystems\vcpkg.cmake"
cmake --build build -j 8 --config Release

# Build and run:
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat" -arch=x64 -host_arch=x64
cmake -S . -B build -G Ninja `
  -DCMAKE_TOOLCHAIN_FILE="$PWD/vcpkg/scripts/buildsystems/vcpkg.cmake" `
  -DCMAKE_CXX_COMPILER="C:/Program Files (x86)/Microsoft Visual Studio/2022/BuildTools/VC/Tools/MSVC/14.44.35207/bin/Hostx64/x64/cl.exe" `
  -DCMAKE_CUDA_COMPILER="C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v13.0/bin/nvcc.exe" `
  -DCMAKE_CUDA_ARCHITECTURES=native
cmake --build build -j8
cd build && ctest --output-on-failure --verbose -j 8 && cd ..

call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat" -arch=x64 -host_arch=x64 && ^
cmake -S . -B build -G Ninja ^
  -DCMAKE_BUILD_TYPE=Release ^
  -DCMAKE_TOOLCHAIN_FILE="%CD%\vcpkg\scripts\buildsystems\vcpkg.cmake" ^
  -DCMAKE_CUDA_COMPILER="C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v13.0/bin/nvcc.exe" ^
  -DCMAKE_CUDA_ARCHITECTURES=native && ^
cmake --build build -j16  && ^
ctest --test-dir build --output-on-failure --verbose -j16

exrinfo -a <image.exr> - exr header info

cmake --preset mac-cpu-debug
cmake --build --preset mac-cpu-debug -j16
ctest --preset mac-cpu-debug --parallel 16 --verbose
ctest -R ConvolutionFilter_test --preset mac-cpu-debug --parallel 16 --verbose

cmake --preset mac-cpu-dev
cmake --build --preset mac-cpu-dev -j16
ctest --preset mac-cpu-dev --parallel 16 --verbose
ctest -R ConvolutionFilter_test --preset mac-cpu-dev --parallel 16 --verbose
