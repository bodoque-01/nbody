#!/usr/bin/env bash
#
# One-shot setup for the "Simple gravity" demo.
#
# Installs a C++ toolchain and raylib, builds the program, and runs it.
# Works on Debian/Ubuntu (apt), Fedora/RHEL (dnf), Arch (pacman) and
# openSUSE (zypper). If your distro doesn't package raylib, it is built
# from source automatically.
#
# Usage:
#   ./setup.sh          # install deps, build, and run
#   ./setup.sh --no-run # install deps and build only
#
set -euo pipefail

cd "$(dirname "$0")"

RUN_AFTER_BUILD=1
[ "${1:-}" = "--no-run" ] && RUN_AFTER_BUILD=0

# Run a command as root, using sudo only when needed.
as_root() {
    if [ "$(id -u)" -eq 0 ]; then
        "$@"
    elif command -v sudo >/dev/null 2>&1; then
        sudo "$@"
    else
        echo "error: this step needs root, but neither root nor sudo is available." >&2
        echo "Run the following manually:" >&2
        echo "  $*" >&2
        exit 1
    fi
}

# True if raylib >= 5.0 is already discoverable. This program uses APIs added in
# raylib 5.0 (ColorLerp, FLAG_BORDERLESS_WINDOWED_MODE), so an older packaged
# raylib (e.g. Ubuntu 24.04's 4.5.0) must be treated as "not available" so we
# fall through and build a new enough one from source.
raylib_available() {
    if command -v pkg-config >/dev/null 2>&1 && \
       pkg-config --atleast-version=5.0 raylib; then
        return 0
    fi
    for inc in /usr/include/raylib.h /usr/local/include/raylib.h; do
        # RAYLIB_VERSION_MAJOR was introduced in 5.0; if it's missing or < 5,
        # the header is too old (or too old to advertise its version).
        if [ -f "$inc" ] && \
           awk '/define RAYLIB_VERSION_MAJOR/ { found=1; if ($3 + 0 >= 5) ok=1 }
                END { exit ok ? 0 : 1 }' "$inc"; then
            return 0
        fi
    done
    return 1
}

# Detect the package manager.
PM=""
for candidate in apt-get dnf pacman zypper; do
    if command -v "$candidate" >/dev/null 2>&1; then
        PM="$candidate"
        break
    fi
done

if [ -z "$PM" ]; then
    echo "Could not detect a supported package manager (apt/dnf/pacman/zypper)." >&2
    echo "Please install a C++ compiler, make, and raylib manually, then run: make run" >&2
    exit 1
fi

echo ">> Detected package manager: $PM"
echo ">> Installing build tools..."

case "$PM" in
    apt-get)
        as_root apt-get update
        as_root apt-get install -y build-essential make pkg-config
        echo ">> Trying to install raylib from apt..."
        as_root apt-get install -y libraylib-dev || true
        ;;
    dnf)
        as_root dnf install -y gcc-c++ make pkgconf-pkg-config
        echo ">> Trying to install raylib from dnf..."
        as_root dnf install -y raylib-devel || true
        ;;
    pacman)
        as_root pacman -Sy --noconfirm --needed base-devel pkgconf
        echo ">> Trying to install raylib from pacman..."
        as_root pacman -S --noconfirm --needed raylib || true
        ;;
    zypper)
        as_root zypper install -y -t pattern devel_C_C++ || true
        as_root zypper install -y gcc-c++ make pkg-config
        echo ">> Trying to install raylib from zypper..."
        as_root zypper install -y raylib-devel || true
        ;;
esac

# If the distro didn't provide raylib, build it from source.
if ! raylib_available; then
    echo ">> raylib not found in system packages; building it from source..."

    # Build dependencies for raylib itself.
    case "$PM" in
        apt-get) as_root apt-get install -y git cmake libgl1-mesa-dev libx11-dev \
                     libxrandr-dev libxi-dev libxcursor-dev libxinerama-dev ;;
        dnf)     as_root dnf install -y git cmake mesa-libGL-devel libX11-devel \
                     libXrandr-devel libXi-devel libXcursor-devel libXinerama-devel ;;
        pacman)  as_root pacman -S --noconfirm --needed git cmake mesa libx11 \
                     libxrandr libxi libxcursor libxinerama ;;
        zypper)  as_root zypper install -y git cmake Mesa-libGL-devel libX11-devel \
                     libXrandr-devel libXi-devel libXcursor-devel libXinerama-devel ;;
    esac

    TMP_RAYLIB="$(mktemp -d)"
    trap 'rm -rf "$TMP_RAYLIB"' EXIT
    git clone --depth 1 https://github.com/raysan5/raylib.git "$TMP_RAYLIB/raylib"
    cmake -S "$TMP_RAYLIB/raylib" -B "$TMP_RAYLIB/build" \
        -DBUILD_SHARED_LIBS=ON -DCMAKE_BUILD_TYPE=Release
    cmake --build "$TMP_RAYLIB/build" -j "$(nproc)"
    as_root cmake --install "$TMP_RAYLIB/build"
    as_root ldconfig || true
fi

if ! raylib_available; then
    echo "error: raylib still not available after install/build. Aborting." >&2
    exit 1
fi

echo ">> Building the program..."
make build

if [ "$RUN_AFTER_BUILD" -eq 1 ]; then
    echo ">> Launching the demo (close the window or press Ctrl+C to quit)..."
    make run
else
    echo ">> Done. Run it with: make run"
fi
