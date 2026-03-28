CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra
LDFLAGS := -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
TARGET := main
SRC := main.cpp circular_buffer.cpp
WATCH_FILES := $(wildcard *.cpp *.h *.hpp)

.PHONY: all build run clean watch watch-run

all: build

build: $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) $(LDFLAGS) -o $(TARGET)

run: build
	./$(TARGET)

clean:
	rm -f $(TARGET)

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
