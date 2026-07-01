# 3.24+ is required for FetchContent's FIND_PACKAGE_ARGS (reuse a system raylib).
cmake_minimum_required(VERSION 3.24)
project(simple_gravity CXX)

# Portable, self-contained build: raylib is fetched and built from source at a
# pinned version, so no system-wide raylib install is needed and every build
# gets a known-good version (>= 5.0, which this program requires). Nothing is
# installed globally; everything lands in the build/ directory.
#
#   cmake -B build -DCMAKE_BUILD_TYPE=Release
#   cmake --build build -j
#   ./build/main                       # run the demo
#   cmake --build build --target benchmark-compare
#
# You still need a C++17 compiler, cmake, git, and (because raylib is built
# from source) the OpenGL/X11 -dev packages. See the README for the one-liner.

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "" FORCE)
endif()

# ---- raylib ----
# Reuse a system-installed raylib when one that's new enough is present
# (FIND_PACKAGE_ARGS runs find_package(raylib 5.0) first); otherwise download and
# build a pinned version from source. The 5.0 floor means an old system package
# like Ubuntu 24.04's raylib 4.5.0 is rejected and we fetch instead, while a
# recent one (e.g. Fedora's) is used as-is with no download. Nothing is installed
# globally either way.
include(FetchContent)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)   # if built from source, skip examples
set(BUILD_GAMES    OFF CACHE BOOL "" FORCE)
set(RAYLIB_DOWNLOAD_VERSION 6.0)
FetchContent_Declare(
    raylib
    URL https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_DOWNLOAD_VERSION}.tar.gz
    DOWNLOAD_EXTRACT_TIMESTAMP OFF
    FIND_PACKAGE_ARGS 5.0
)
FetchContent_MakeAvailable(raylib)

# ---- OpenMP (used by the physics loop) ----
find_package(OpenMP REQUIRED)

# ---- the renderer ----
add_executable(main main.cpp)
target_link_libraries(main PRIVATE raylib OpenMP::OpenMP_CXX)

# Convenience: `cmake --build build --target run`
add_custom_target(run COMMAND main DEPENDS main USES_TERMINAL)

# ---- benchmarks ----
# Two separate binaries so the OpenMP runtime overhead doesn't taint the
# single-threaded timing (see benchmark.cpp). raymath is used header-only in
# standalone mode, so these don't link raylib; they just borrow its include dir.
# Pull that from the raylib target itself so it works whether raylib was fetched
# or found as a system package.
set(RAYLIB_INCLUDES $<TARGET_PROPERTY:raylib,INTERFACE_INCLUDE_DIRECTORIES>)

add_executable(benchmark-serial benchmark.cpp)
target_compile_definitions(benchmark-serial PRIVATE RAYMATH_STANDALONE)
target_include_directories(benchmark-serial PRIVATE ${RAYLIB_INCLUDES})
target_link_libraries(benchmark-serial PRIVATE m)

add_executable(benchmark-omp benchmark.cpp)
target_compile_definitions(benchmark-omp PRIVATE RAYMATH_STANDALONE USE_OPENMP)
target_include_directories(benchmark-omp PRIVATE ${RAYLIB_INCLUDES})
target_link_libraries(benchmark-omp PRIVATE m OpenMP::OpenMP_CXX)

# `cmake --build build --target benchmark-compare`
add_custom_target(benchmark-compare
    COMMAND ${CMAKE_COMMAND} -E echo "=== serial, no OpenMP ==="
    COMMAND benchmark-serial
    COMMAND ${CMAKE_COMMAND} -E echo ""
    COMMAND ${CMAKE_COMMAND} -E echo "=== OpenMP ==="
    COMMAND benchmark-omp
    DEPENDS benchmark-serial benchmark-omp
    USES_TERMINAL
)
