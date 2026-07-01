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
your distro doesn't package a new enough version), compiles the program, and
launches it.

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
- A C++17 compiler (`g++`), `make`, and **raylib 5.0 or newer**, all (HOPEFULLY LOL) handled by `setup.sh`.

> **Note:** this project needs **raylib ≥ 5.0** (it uses `ColorLerp` and
> `FLAG_BORDERLESS_WINDOWED_MODE`, both added in 5.0). Many distro packages are
> older than that — for example Ubuntu 24.04's `libraylib-dev` is 4.5.0 and will
> **fail to compile** this program. When in doubt, build raylib from source as
> shown below (which is exactly what `setup.sh` does).

## Manual install

If you'd rather install raylib yourself instead of running `setup.sh`, the most
reliable way on any distro is to build it from source, since it guarantees a
new enough version.

First install a C++ toolchain and raylib's build dependencies:

| Distro            | Command                                                                                                                                        |
|-------------------|------------------------------------------------------------------------------------------------------------------------------------------------|
| Debian / Ubuntu   | `sudo apt update && sudo apt install -y build-essential git cmake pkg-config libgl1-mesa-dev libx11-dev libxrandr-dev libxi-dev libxcursor-dev libxinerama-dev` |
| Fedora / RHEL     | `sudo dnf install -y gcc-c++ make git cmake pkgconf-pkg-config mesa-libGL-devel libX11-devel libXrandr-devel libXi-devel libXcursor-devel libXinerama-devel`    |

Then build and install raylib from source:

```bash
git clone --depth 1 https://github.com/raysan5/raylib.git
cmake -S raylib -B raylib/build -DBUILD_SHARED_LIBS=ON -DCMAKE_BUILD_TYPE=Release
cmake --build raylib/build -j"$(nproc)"
sudo cmake --install raylib/build
sudo ldconfig
```

Finally, build and run the program with `make run`.

If your distro already packages raylib **5.0 or newer**, you can install that
instead of building from source (e.g. `sudo dnf install raylib-devel` on recent
Fedora) and skip straight to `make run`.
