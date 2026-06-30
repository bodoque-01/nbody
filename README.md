# Simple gravity

A N body simulation with several tiny particles, painted all in gradients of red/pink in honor of the Petrova Line from Project Hail Mary.
They orbit a comparatively massive body in the center. 

## Quick start

On a desktop Linux machine with a graphical display:

```bash
git clone <repo-url> gravity
cd gravity
./setup.sh
```

That's it. `setup.sh` detects your package manager (apt, dnf, pacman, or
zypper), installs a C++ toolchain and raylib (building raylib from source if
your distro doesn't package it), compiles the program, and launches it.

To install and build without launching:

```bash
./setup.sh --no-run
```

## Controls

| Key       | Action       |
|-----------|--------------|
| `Space`   | Pause/resume |
| `Esc`     | Quit         |

## Running it again later

Once dependencies are installed, you don't need `setup.sh` anymore:

```bash
make run     # build (if needed) and run
make build   # just build
make clean   # remove the binary
make benchmark-compare # Runs benchmarking comparing a simple run of the heavy physics for loop without OpenMP (single-threaded) and one with OpenMP parallelization.
```

## Requirements

- A desktop Linux environment with OpenGL and an X11 display.
- A C++17 compiler (`g++`), `make`, and `raylib`, all (HOPEFULLY LOL) handled by `setup.sh`.

## Manual install

If you'd rather install raylib yourself instead of running `setup.sh`:

| Distro            | Command                                                  |
|-------------------|----------------------------------------------------------|
| Debian / Ubuntu   | `sudo apt install build-essential libraylib-dev`         |
| Fedora / RHEL     | `sudo dnf install gcc-c++ make raylib-devel`             |

Then build and run with `make run`.
