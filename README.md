# DenoiseMachineX

[![linux-build-and-test](https://github.com/ginzburg-dev/denoise-machine-x/actions/workflows/linux_build_and_test.yaml/badge.svg?branch=main)](https://github.com/ginzburg-dev/denoise-machine-x/actions/workflows/linux_build_and_test.yaml)
[![License](https://img.shields.io/badge/License-Apache_2.0-blue.svg)](https://opensource.org/licenses/Apache-2.0)

A modular, open-source, high-performance C++ denoiser for removing rendering noise.

Featuring spatial and temporal filtering, multi-layer and multi-frame support, designed for flexibility and easy integration in modern pipelines

---

## Features

- Modern C++20 codebase
- Multi-layer and multi-frame image abstraction
- Pixel-wise and channel-wise access
- Modular architecture for filters and IO
- GPU/CUDA acceleration (planned/optional)
- Unit-tested with GitHub Actions

---

## Getting Started

### Prerequisites

- C++20 compiler (e.g., GCC 10+, Clang 11+, MSVC 2019+)
- CMake 3.15+
- (Optional) CUDA Toolkit for GPU acceleration

### Build

This project uses OpenEXR and Imath for image file IO.

**macOS:**  
    brew install openexr imath

**Ubuntu/Debian:**  
    sudo apt install libopenexr-dev libimath-dev

**Windows (recommended):**  
    - Install [vcpkg](https://vcpkg.io/)  
    - vcpkg install openexr

**If OpenEXR is installed in a non-standard location:**  
    cmake -DCMAKE_PREFIX_PATH=/path/to/openexr;/path/to/imath ..

```bash
git clone https://github.com/ginzburg-dev/denoise-machine-x.git
cd denoise-machine-x
mkdir build && cd build
cmake ..
cmake --build .
