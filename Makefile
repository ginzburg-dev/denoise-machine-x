

# Mac

.PHONY: build-mac-cpu-dev

build-mac-cpu-dev:
	rm -rf tests/test_files/*
	cmake --preset mac-cpu-dev
	cmake --build --preset mac-cpu-dev -j8
	ctest --preset mac-cpu-dev -j8 --output-log tests/test_files/test_logs/AllTests.log

.PHONY: build-mac-cpu-debug

build-mac-cpu-debug:
	rm -rf tests/test_files/*
	cmake --preset mac-cpu-debug
	cmake --build --preset mac-cpu-debug -j8
	ctest --preset mac-cpu-debug -j8 --output-log tests/test_files/test_logs/AllTests.log

# Linux

.PHONY: build-linux-cpu-dev

build-linux-cpu-dev:
	rm -rf tests/test_files/*
	cmake --preset linux-cpu-dev
	cmake --build --preset linux-cpu-dev -j8
	ctest --preset linux-cpu-dev -j8 --output-log tests/test_files/test_logs/AllTests.log

.PHONY: build-linux-cpu-debug

build-linux-cpu-debug:
	rm -rf tests/test_files/*
	cmake --preset linux-cpu-debug
	cmake --build --preset linux-cpu-debug -j8
	ctest --preset linux-cpu-debug -j8 --output-log tests/test_files/test_logs/AllTests.log
