#include "raylib.h"
#include "raymath.h"
#include <cmath>
#include <omp.h>

struct planet {
    Vector2 position;
    int mass;
    Vector2 velocity;
    int radius;
};
 
int main(void)
{
    bool isPaused = false;
    SetConfigFlags(FLAG_BORDERLESS_WINDOWED_MODE);
    InitWindow(0, 0, "Simple gravity"); // 0,0 means "use the monitor's native resolution"
    const int screenWidth = 1520;
    const int screenHeight = 1080;

    const double gravitational_constant = 200.;

     // center of the screen
    const float startX = (float)screenWidth / 2.0f;
    const float startY = (float)screenHeight / 2.0f;
    
    const float goldenAngle = 2.39996f; 
    const float particleRadius = 5.0f;
    const float c = (particleRadius * 2) * 1.5f;
    const float innerRadius = 80.0f; // push the whole spiral out so inner particles don't crowd the center


    SetTargetFPS(60); // Set our game to run at 60 frames-per-second

    const int N = 400;

    planet bodies[N];
    Color bodyColors[N];

    // Body 0 is the sun (zero velocity): the center of the system.
    bodies[0]     = planet{ Vector2{screenWidth/2.0f, screenHeight/2.0f}, 10000, Vector2{0, 0}, 8 };
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

        bodies[i] = planet{ position, 5, velocity, 1 };
        bodyColors[i] = ColorLerp(RED, PINK, GetRandomValue(0, 100) / 100.0f);
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
