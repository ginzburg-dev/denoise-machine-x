PRESET ?= mac-cpu-debug
JOBS ?= 4

.PHONY: configure build test check ci \
	build-mac-cpu-dev build-mac-cpu-debug \
	build-linux-cpu-dev build-linux-cpu-debug

configure:
	cmake --preset $(PRESET)

build: configure
	cmake --build --preset $(PRESET) --parallel $(JOBS)

test: build
	ctest --preset $(PRESET) --parallel $(JOBS)

check: test

# The same Linux CPU pipeline used by GitHub Actions.
ci:
	$(MAKE) check PRESET=linux-cpu-debug JOBS=$(JOBS)

build-mac-cpu-dev:
	$(MAKE) check PRESET=mac-cpu-dev JOBS=$(JOBS)

build-mac-cpu-debug:
	$(MAKE) check PRESET=mac-cpu-debug JOBS=$(JOBS)

build-linux-cpu-dev:
	$(MAKE) check PRESET=linux-cpu-dev JOBS=$(JOBS)

build-linux-cpu-debug:
	$(MAKE) check PRESET=linux-cpu-debug JOBS=$(JOBS)
