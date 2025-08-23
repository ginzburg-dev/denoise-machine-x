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
cd build
ctest -j16 -C Debug -T test --output-on-failure --verbose
cd ..