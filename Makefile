# Local development build.
#
# This is the fast iteration loop: it builds against a raylib that's already
# installed on your system (via setup.sh, or your distro's package), so there's
# no per-build raylib fetch/compile. It also has `watch`/`watch-run` for
# auto-rebuilding on save.
#
# For a portable, self-contained build that needs no system raylib, use CMake
# instead (see the README).

CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -fopenmp
LDFLAGS := -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -fopenmp
TARGET := main
SRC := main.cpp
WATCH_FILES := $(wildcard *.cpp *.h *.hpp)

BENCH_CXXFLAGS := -std=c++17 -Wall -Wextra -O2 -DRAYMATH_STANDALONE
BENCH_SRC := benchmark.cpp

.PHONY: all build run clean watch watch-run \
	benchmark benchmark-serial benchmark-omp benchmark-compare

all: build

build: $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) $(LDFLAGS) -o $(TARGET)

run: build
	./$(TARGET)

benchmark-serial: $(BENCH_SRC)
	$(CXX) $(BENCH_CXXFLAGS) $(BENCH_SRC) -lm -o benchmark-serial

benchmark-omp: $(BENCH_SRC)
	$(CXX) $(BENCH_CXXFLAGS) -fopenmp -DUSE_OPENMP $(BENCH_SRC) -lm -fopenmp -o benchmark-omp

benchmark: benchmark-serial benchmark-omp

benchmark-compare: benchmark
	@echo "=== serial (no OpenMP) ==="
	./benchmark-serial
	@echo ""
	@echo "=== OpenMP ==="
	./benchmark-omp

clean:
	rm -f $(TARGET) benchmark benchmark-serial benchmark-omp

watch:
	@command -v inotifywait >/dev/null 2>&1 || { \
		echo "inotifywait not found. Install with: sudo dnf install inotify-tools"; \
		exit 1; \
	}
	@echo "Watching files: $(WATCH_FILES)"
	@echo "Press Ctrl+C to stop"
	@while inotifywait -q -e close_write,create,delete,move $(WATCH_FILES); do \
		echo "Changes detected. Rebuilding..."; \
		$(MAKE) build && echo "Build OK" || echo "Build failed"; \
	done

watch-run:
	@command -v watchexec >/dev/null 2>&1 || { \
		echo "watchexec not found. Install with: sudo dnf install watchexec"; \
		exit 1; \
	}
	@watchexec --clear --exts cpp,h -- "$(MAKE) run"
