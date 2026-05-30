#include "raylib.h"
#include "raymath.h"
#include "config.h"
#include <cmath>
#include <omp.h>

int main(void)
{
    bool isPaused = false;
    SetConfigFlags(FLAG_BORDERLESS_WINDOWED_MODE);
    InitWindow(0, 0, "PETROVA LINE"); // 0,0 means "use the monitor's native resolution"

    SetTargetFPS(60); // Set our game to run at 60 frames-per-second

    const int N = 400;

    planet bodies[N];
    Color bodyColors[N];

    // Body 0 is the sun (zero velocity): the center of the system.
    bodies[0]     = planet{ Vector2{startX, startY}, sunMass, Vector2{0, 0}, sunRadius };
    bodyColors[0] = Color{255, 255, 0, 255};


    for (int i = 1; i < N; i++)
    {
        // This whole thing of the position is the golden angle distribution, which is a way to distribute points evenly in a circular pattern.
        float theta = i * goldenAngle;
        float r = innerRadius + c * sqrtf((float)i);

        Vector2 position = {
            startX + r * cosf(theta),
            startY + r * sinf(theta)
        };

        // Perpendicular (tangential) launch velocity so the particle orbits the sun.
        double distance = Vector2Distance(bodies[0].position, position);
        double speed = sqrt(gravitational_constant * bodies[0].mass / distance);
        Vector2 velocity = Vector2Scale(Vector2{ -sinf(theta), cosf(theta) }, (float)speed);

        bodies[i] = planet{ position, planetMass, velocity, planetRadius };
        bodyColors[i] = ColorLerp(RED, PINK, GetRandomValue(0, 100) / 100.0f);
    }


    const int maxSubstepsPerFrame = 8;
    float accumulator = 0.0f;
    // Main game loop
    while (!WindowShouldClose())
    {
        if (IsKeyPressed(KEY_SPACE)) {
            isPaused = !isPaused;
        }
        // for body n in bodies:
            // Calculate sum of forces over that body
            // calculate acceleration for body
            // change velocity of body.
        if (!isPaused) {
            accumulator += GetFrameTime();
            int stepsTaken = 0;

            while (accumulator >= simulationDt && stepsTaken < maxSubstepsPerFrame) {
                Vector2 accelerations[N] = {};



                #pragma omp parallel for
                for (int i = 0; i < N; i++) {
                    for (int j = 0; j < N; j++) {
                        Vector2 r = Vector2Subtract(bodies[j].position, bodies[i].position);
                        float distSqr = Vector2LengthSqr(r);
                        if (distSqr <= 0.25f) continue;

                        float dist = sqrtf(distSqr);
                        double forceMagnitude = (bodies[i].mass * bodies[j].mass * gravitational_constant) / distSqr;
                        Vector2 r_norm = Vector2Scale(r, 1.0f / dist);
                        Vector2 force = Vector2Scale(r_norm, (float)forceMagnitude);

                        Vector2 acceleration_i = Vector2Scale(force, 1.0f / (float)bodies[i].mass);
                        accelerations[i] = Vector2Add(accelerations[i], acceleration_i);
                    }
                }

                for (int i = 0; i < N; i++) {
                    bodies[i].velocity = Vector2Add(bodies[i].velocity, Vector2Scale(accelerations[i], simulationDt));
                    bodies[i].position = Vector2Add(bodies[i].position, Vector2Scale(bodies[i].velocity, simulationDt));
                }

                accumulator -= simulationDt;
                stepsTaken++;
            }
        }

        // Draw
        BeginDrawing();

            ClearBackground(BLACK);
            for (int i = 0; i < N; i++) {
                DrawCircle(bodies[i].position.x, bodies[i].position.y, bodies[i].radius, bodyColors[i]);
            }

        EndDrawing();
    }

    // De-Initialization
    CloseWindow();

    return 0;
}