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

    InitWindow(screenWidth, screenHeight, "Simple gravity");

    SetTargetFPS(60); // Set our game to run at 60 frames-per-second

    const int N = 6;

    // Per-body initial conditions. The first entry is the sun (zero velocity), center of the system.
    struct PlanetInit {
        Vector2 position;
        int mass;
        int radius;
        Vector2 velocityDir;
        Color color;
    };
    const PlanetInit inits[N] = {
        {Vector2{screenWidth/2.0f,     screenHeight/2.0f},     10000, 15, Vector2{0,  0}, Color{255,255,0,255}},
        {Vector2{screenWidth/4.0f,     screenHeight/2.0f},     10,    5,  Vector2{0, -1}, Color{0,255,150,255}},
        {Vector2{screenWidth/4.0f,     screenHeight/4.0f},     30,    8,  Vector2{0, -1}, Color{150,150,255,255}},
        {Vector2{3 * screenWidth/4.0f, screenHeight/2.0f},     20,    6,  Vector2{0,  1}, Color{255,140,0,255}},
        {Vector2{screenWidth/2.0f,     screenHeight/4.0f},     15,    5,  Vector2{1,  0}, Color{180,255,180,255}},
        {Vector2{screenWidth/2.0f,     3 * screenHeight/4.0f}, 25,    7,  Vector2{-1, 0}, Color{255,180,220,255}},
    };

    planet bodies[N];
    Color bodyColors[N];
    CircularBuffer positionHistory[N] = { CircularBuffer(100),  CircularBuffer(100),  CircularBuffer(100),  CircularBuffer(100),  CircularBuffer(100), CircularBuffer(100)};
    for (int i = 0; i < N; i++) {
        const PlanetInit& in = inits[i];
        double distance = Vector2Distance(inits[0].position, in.position);
        double speed = (distance > 0) ? sqrt(gravitational_constant * inits[0].mass / distance) : 0.0;
        bodies[i] = planet{ in.position, in.mass, Vector2Scale(in.velocityDir, (float)speed), in.radius };
        bodyColors[i] = in.color;
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
