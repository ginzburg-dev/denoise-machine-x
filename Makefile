.PHONY: build-mac-cpu-dev build-mac-metal-dev build-mac-cpu-debug build-mac-metal-debug

# Mac
build-mac-cpu-dev:
	rm -rf tests/test_files/*
	cmake --preset mac-cpu-dev
	cmake --build --preset mac-cpu-dev
	ctest --preset mac-cpu-dev

build-mac-cpu-debug:
	rm -rf tests/test_files/*
	cmake --preset mac-cpu-debug
	cmake --build --preset mac-cpu-debug
	ctest --preset mac-cpu-debug

# Linux
build-linux-cpu-dev:
	rm -rf tests/test_files/*
	cmake --preset linux-cpu-dev
	cmake --build --preset linux-cpu-dev
	ctest --preset linux-cpu-dev

build-linux-cpu-debug:
	rm -rf tests/test_files/*
	cmake --preset linux-cpu-debug
	cmake --build --preset linux-cpu-debug
	ctest --preset linux-cpu-debug
