cmake_minimum_required(VERSION 3.14)
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

# ---- raylib (built from source, pinned) ----
include(FetchContent)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)   # don't build raylib's examples
set(BUILD_GAMES    OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG        6.0
    GIT_SHALLOW    TRUE
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
# standalone mode, so these don't link raylib; they just need its include dir.
add_executable(benchmark-serial benchmark.cpp)
target_compile_definitions(benchmark-serial PRIVATE RAYMATH_STANDALONE)
target_include_directories(benchmark-serial PRIVATE ${raylib_SOURCE_DIR}/src)
target_link_libraries(benchmark-serial PRIVATE m)

add_executable(benchmark-omp benchmark.cpp)
target_compile_definitions(benchmark-omp PRIVATE RAYMATH_STANDALONE USE_OPENMP)
target_include_directories(benchmark-omp PRIVATE ${raylib_SOURCE_DIR}/src)
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
