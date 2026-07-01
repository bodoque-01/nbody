# Simple gravity

A N body simulation with several tiny particles, painted all in gradients of red/pink in honor of the Petrova Line from Project Hail Mary.
They orbit a comparatively massive body in the center.

## Quick start

On a desktop Linux machine with a graphical display:

```bash
git clone git@github.com:bodoque-01/nbody.git gravity
cd gravity
./setup.sh
```

That's it. `setup.sh` detects your package manager (apt or dnf), installs a C++
toolchain and raylib (building raylib from source if your distro doesn't package
a new enough version), compiles the program, and launches it.

To install and build without launching:

```bash
./setup.sh --no-run
```

## Controls

| Key     | Action       |
| ------- | ------------ |
| `Space` | Pause/resume |
| `Esc`   | Quit         |

## Local development (make)

For fast iteration while hacking on the code, use the Makefile. It builds
against the raylib already on your system (from `setup.sh` or your distro's
package), so there's no per-build raylib fetch/compile:

```bash
make run     # build (if needed) and run
make build   # just build
make clean   # remove the binary
make benchmark-compare # Runs benchmarking comparing a simple run of the heavy physics for loop without OpenMP (single-threaded) and one with OpenMP parallelization.
make watch   # auto-rebuild on save
```

## Building it yourself with CMake (recommended, no raylib install)

The most portable way to build is the included `CMakeLists.txt`. It fetches and
builds a pinned raylib from source as part of the build, so you never install
raylib system-wide and every build gets a known-good version. The commands are
identical on every distro.

First install the build tools and the OpenGL/X11 development libraries raylib
needs to compile:

| Distro          | Command                                                                                                                                              |
| --------------- | ---------------------------------------------------------------------------------------------------------------------------------------------------- |
| Debian / Ubuntu | `sudo apt update && sudo apt install -y build-essential git cmake libgl1-mesa-dev libx11-dev libxrandr-dev libxi-dev libxcursor-dev libxinerama-dev` |
| Fedora / RHEL   | `sudo dnf install -y gcc-c++ make git cmake mesa-libGL-devel libX11-devel libXrandr-devel libXi-devel libXcursor-devel libXinerama-devel`            |

Then build and run:

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
./build/main                                    # run the demo
cmake --build build --target benchmark-compare  # run the benchmarks
```

The first configure downloads and compiles raylib, so it takes a couple of
minutes; later builds are incremental. Nothing is installed globally, everything
just lands in `build/`.

## Requirements

- A desktop Linux environment with OpenGL and an X11 display.
- A C++17 compiler (`g++`), plus either `make` + a system raylib, or `cmake` +
  `git` for the self-contained CMake build above.

> **Note:** this project needs **raylib 5.0 or newer** (it uses `ColorLerp` and
> `FLAG_BORDERLESS_WINDOWED_MODE`, both added in 5.0). The CMake build handles
> this for you. If you install raylib yourself, watch the version: some distro
> packages are older, for example Ubuntu 24.04's `libraylib-dev` is 4.5.0 and
> will **fail to compile** this program.

## Using a system-wide raylib

If you'd rather manage raylib yourself instead of using `setup.sh` or the CMake
build, install it system-wide by following raylib's official guide:

- https://github.com/raysan5/raylib/wiki/Working-on-GNU-Linux

Make sure the version you install is **5.0 or newer** (on recent Fedora
`sudo dnf install raylib-devel` is new enough; Ubuntu's apt package is not, so
build from source or just use the CMake build above). Once raylib is available,
build and run with `make run`.
