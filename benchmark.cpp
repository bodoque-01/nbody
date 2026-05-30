// Standalone microbenchmark for the N-body physics step (no rendering), the heavy one in which we calculate the acceleration for each body by summing the forces from all other bodies. 
// This is O(N^2) and the main bottleneck in the simulation, so it's a good candidate for OpenMP parallelization. 
// The benchmark runs a fixed number of steps and measures the time taken, printing results in ms/step and steps/sec for various values of N (number of bodies).
// Build two binaries for a fair OpenMP comparison, as in, we DON'T compare several threads against a single thread in the same binary. 
// Why? Because the OpenMP runtime itself has some overhead and it would be unfair to include that in the single-threaded timing. 
// Instead we build two separate binaries, one with OpenMP and one without, and run them separately to get clean measurements.
//
//   make benchmark-serial   # plain loops, no OpenMP runtime
//   make benchmark-omp      # -fopenmp, parallel force loop
//   make benchmark-compare  # run both and print results
#include "raymath.h"
#include <cmath>
#include <chrono>
#include <cstdio>
#include <vector>

#ifdef USE_OPENMP
#include <omp.h>
#endif

struct planet {
    Vector2 position;
    int mass;
    Vector2 velocity;
    int radius;
};

static void initBodies(std::vector<planet>& bodies, int N) {
    const int screenWidth = 1520;
    const int screenHeight = 1080;
    const double gravitational_constant = 200.0;
    const float goldenAngle = 2.39996f;
    const float particleRadius = 5.0f;
    const float c = (particleRadius * 2) * 1.5f;
    const float innerRadius = 80.0f;
    const float startX = (float)screenWidth / 2.0f;
    const float startY = (float)screenHeight / 2.0f;

    bodies.assign(N, planet{});
    bodies[0] = planet{
        Vector2{(float)screenWidth / 2.0f, (float)screenHeight / 2.0f},
        10000, Vector2{0, 0}, 8
    };
    for (int i = 1; i < N; i++) {
        float theta = i * goldenAngle;
        float r = innerRadius + c * sqrtf((float)i);
        Vector2 position = {startX + r * cosf(theta), startY + r * sinf(theta)};
        double distance = Vector2Distance(bodies[0].position, position);
        double speed = sqrt(gravitational_constant * bodies[0].mass / distance);
        Vector2 velocity = Vector2Scale(Vector2{-sinf(theta), cosf(theta)}, (float)speed);
        bodies[i] = planet{position, 5, velocity, 1};
    }
}

static void step(std::vector<planet>& bodies, std::vector<Vector2>& acc, int N,
                 double gravitational_constant, float dt) {
    for (int i = 0; i < N; i++) acc[i] = Vector2{0, 0};

#ifdef USE_OPENMP
    #pragma omp parallel for
#endif
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            Vector2 r = Vector2Subtract(bodies[j].position, bodies[i].position);
            float distSqr = Vector2LengthSqr(r);
            if (distSqr <= 0.25f) continue;

            float dist = sqrtf(distSqr);
            double forceMagnitude =
                (bodies[i].mass * bodies[j].mass * gravitational_constant) / distSqr;
            Vector2 r_norm = Vector2Scale(r, 1.0f / dist);
            Vector2 force = Vector2Scale(r_norm, (float)forceMagnitude);
            Vector2 acceleration_i = Vector2Scale(force, 1.0f / (float)bodies[i].mass);
            acc[i] = Vector2Add(acc[i], acceleration_i);
        }
    }

    for (int i = 0; i < N; i++) {
        bodies[i].velocity = Vector2Add(bodies[i].velocity, Vector2Scale(acc[i], dt));
        bodies[i].position = Vector2Add(bodies[i].position, Vector2Scale(bodies[i].velocity, dt));
    }
}

int main(void) {
    const double gravitational_constant = 200.0;
    const float dt = 1.0f / 240.0f;
    const int steps = 2000;
    const int Ns[] = {100, 250, 500, 1000, 2000};

#ifdef USE_OPENMP
    printf("build: OpenMP (%d threads), steps/measurement = %d\n", omp_get_max_threads(), steps);
#else
    printf("build: serial (no OpenMP), steps/measurement = %d\n", steps);
#endif
    printf("%-8s %-14s %-14s\n", "N", "ms/step", "steps/sec");

    for (int N : Ns) {
        std::vector<planet> bodies;
        initBodies(bodies, N);
        std::vector<Vector2> acc(N);

        step(bodies, acc, N, gravitational_constant, dt);

        auto t0 = std::chrono::steady_clock::now();
        for (int s = 0; s < steps; s++) {
            step(bodies, acc, N, gravitational_constant, dt);
        }
        auto t1 = std::chrono::steady_clock::now();

        double ms = std::chrono::duration<double, std::milli>(t1 - t0).count();
        printf("%-8d %-14.4f %-14.0f\n", N, ms / steps, steps / (ms / 1000.0));
    }
    return 0;
}
