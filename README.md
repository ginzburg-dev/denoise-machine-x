# DenoiseMachineX

[![linux_build_and_tests](https://github.com/ginzburg-dev/denoise-machine-x/actions/workflows/linux_build_and_tests.yaml/badge.svg?branch=main)](https://github.com/ginzburg-dev/denoise-machine-x/actions/workflows/linux_build_and_tests.yaml)
[![License](https://img.shields.io/badge/License-Apache_2.0-blue.svg)](https://opensource.org/licenses/Apache-2.0)

A modular, high-performance C++ image denoiser with support for multi-layer, multi-frame images.

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

```bash
git clone https://github.com/ginzburg-dev/denoise-machine-x.git
cd denoise-machine-x
mkdir build && cd build
cmake ..
cmake --build .
