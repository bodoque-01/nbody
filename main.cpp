#include "raylib.h"
#include "raymath.h"
#include <cmath>
#include <omp.h>
#include "circular_buffer.h"

struct planet {
    Vector2 position;
    int mass;
    Vector2 velocity;
    int radius;
};
 
int main(void)
{
    bool isPaused = false;
    const int screenWidth = 1520;
    const int screenHeight = 1080;
    const double gravitational_constant = 10000.;
     // center of the screen, upper half)
    const float startX = (float)screenWidth / 2.0f;
    const float startY = (float)screenHeight / 3.0f;
    const float goldenAngle = 2.39996f; 
    const float particleRadius = 32.0f;
    const float c = (particleRadius * 2) * 1.5f; 


    InitWindow(screenWidth, screenHeight, "Simple gravity");

    SetTargetFPS(60); // Set our game to run at 60 frames-per-second

    const int N = 6;

    planet bodies[N];
    Color bodyColors[N];
    CircularBuffer positionHistory[N] = { CircularBuffer(100),  CircularBuffer(100),  CircularBuffer(100),  CircularBuffer(100),  CircularBuffer(100), CircularBuffer(100)};

    // Body 0 is the sun (zero velocity): the center of the system.
    bodies[0]     = planet{ Vector2{screenWidth/2.0f, screenHeight/2.0f}, 10000, Vector2{0, 0}, 15 };
    bodyColors[0] = Color{255, 255, 0, 255};

    for (int i = 1; i < N; i++)
    {
        // This whole thing of the position is the golden angle distribution, which is a way to distribute points evenly in a circular pattern.
        float theta = i * goldenAngle;
        float r = c * sqrtf((float)i);

        Vector2 position = {
            startX + r * cosf(theta),
            startY + r * sinf(theta)
        };

        // Perpendicular (tangential) launch velocity so the particle orbits the sun.
        double distance = Vector2Distance(bodies[0].position, position);
        double speed = (distance > 0) ? sqrt(gravitational_constant * bodies[0].mass / distance) : 0.0;
        Vector2 velocity = Vector2Scale(Vector2{ -sinf(theta), cosf(theta) }, (float)speed);

        bodies[i] = planet{ position, 500, velocity, 5 };
        bodyColors[i] = WHITE;
    }


    double KE = 0;
    double PE = 0;
    double E = KE + PE;
    const float fixedDt = 1.0f / 240.0f;
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

            while (accumulator >= fixedDt && stepsTaken < maxSubstepsPerFrame) {
                Vector2 accelerations[N] = {};
                KE = 0;
                PE = 0;
                E = 0;

                for (int i = 0; i < N; i++) {
                    for (int j = i + 1; j < N; j++) {
                        Vector2 r = Vector2Subtract(bodies[j].position, bodies[i].position);
                        float distSqr = Vector2LengthSqr(r);
                        if (distSqr <= 0.0001f) continue;

                        float dist = sqrtf(distSqr);
                        double forceMagnitude = (bodies[i].mass * bodies[j].mass * gravitational_constant) / distSqr;
                        Vector2 r_norm = Vector2Scale(r, 1.0f / dist);
                        Vector2 force = Vector2Scale(r_norm, (float)forceMagnitude);

                        Vector2 acceleration_i = Vector2Scale(force, 1.0f / (float)bodies[i].mass);
                        Vector2 acceleration_j = Vector2Scale(force, -1.0f / (float)bodies[j].mass);

                        accelerations[i] = Vector2Add(accelerations[i], acceleration_i);
                        accelerations[j] = Vector2Add(accelerations[j], acceleration_j);
                        PE += (-gravitational_constant * bodies[i].mass * bodies[j].mass) / dist;
                    }
                    KE += 0.5 * bodies[i].mass * pow(Vector2Length(bodies[i].velocity), 2);
                }
                E = KE + PE;

                for (int i = 0; i < N; i++) {
                    positionHistory[i].push(Vector2{bodies[i].position.x, bodies[i].position.y});
                    bodies[i].velocity = Vector2Add(bodies[i].velocity, Vector2Scale(accelerations[i], fixedDt));
                    bodies[i].position = Vector2Add(bodies[i].position, Vector2Scale(bodies[i].velocity, fixedDt));
                }

                accumulator -= fixedDt;
                stepsTaken++;
            }
        }

        // Draw
        BeginDrawing();

            ClearBackground(BLACK);
            for (int i = 0; i < N; i++) {
                DrawCircle(bodies[i].position.x, bodies[i].position.y, bodies[i].radius, bodyColors[i]);
                for (std::size_t j = 0; j < positionHistory[i].size(); j++) {
                    Vector2 pos = positionHistory[i].at(j);
                    DrawPixel(pos.x, pos.y, bodyColors[i]);
                }
                DrawText(TextFormat("KE: %.2f", KE), 20, 20, 20, WHITE);
                DrawText(TextFormat("PE: %.2f", PE), 20, 44, 20, WHITE);
                DrawText(TextFormat("E:  %.2f", E),  20, 68, 20, YELLOW);
            }
            if (isPaused) {
                DrawText("PAUSED", screenWidth/2, 20, 30, WHITE);
            }

        EndDrawing();
    }

    // De-Initialization
    CloseWindow();

    return 0;
}
