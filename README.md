# Simple gravity

A tiny N-body gravity simulation: a "sun" and five orbiting bodies, rendered
with [raylib](https://www.raylib.com/). Each body's trail is drawn from a
circular buffer, and live kinetic / potential / total energy is shown on screen.

TODO: Add openMP to handle computations in parallel.
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
```

## Requirements

- A desktop Linux environment with OpenGL and an X11 display.
- A C++17 compiler (`g++`), `make`, and `raylib`, all handled by `setup.sh`.

## Manual install

If you'd rather install raylib yourself instead of running `setup.sh`:

| Distro            | Command                                                  |
|-------------------|----------------------------------------------------------|
| Debian / Ubuntu   | `sudo apt install build-essential libraylib-dev`         |
| Fedora / RHEL     | `sudo dnf install gcc-c++ make raylib-devel`             |
| Arch              | `sudo pacman -S base-devel raylib`                       |
| openSUSE          | `sudo zypper install gcc-c++ make raylib-devel`          |

Then build and run with `make run`.