#include <iostream> 
#include "raylib.h"
#include "raymath.h"
#include <cmath>
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
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 1520;
    const int screenHeight = 1080;
    const double gravitational_constant = 10000.;

    InitWindow(screenWidth, screenHeight, "Simple gravity");

    SetTargetFPS(60); // Set our game to run at 60 frames-per-second

    const int N = 6;
    planet sun = {Vector2{screenWidth/2,screenHeight/2}, 10000, Vector2{0, 0}, 15};
    //Set initial earth velocity to v = sqrt(G * m_sun / r), in some perpendicular direction to the sun (initial conditions, derivation is just equaling centripetal force to gravitational force).
    double distance = Vector2Distance(sun.position, Vector2{screenWidth/4, screenHeight/2});
    double initial_velocity = sqrt(gravitational_constant * sun.mass / distance);
    Vector2 initial_velocity_vector = Vector2{0,-initial_velocity};

    double distance2 = Vector2Distance(sun.position, Vector2{screenWidth/4, screenHeight/4});
    double initial_velocity2 = sqrt(gravitational_constant * sun.mass / distance2);
    Vector2 initial_velocity_vector2 = Vector2{0,-initial_velocity2};

    double distance3 = Vector2Distance(sun.position, Vector2{3 * screenWidth/4, screenHeight/2});
    double initial_velocity3 = sqrt(gravitational_constant * sun.mass / distance3);
    Vector2 initial_velocity_vector3 = Vector2{0,initial_velocity3};

    double distance4 = Vector2Distance(sun.position, Vector2{screenWidth/2, screenHeight/4});
    double initial_velocity4 = sqrt(gravitational_constant * sun.mass / distance4);
    Vector2 initial_velocity_vector4 = Vector2{initial_velocity4, 0};

    double distance5 = Vector2Distance(sun.position, Vector2{screenWidth/2, 3 * screenHeight/4});
    double initial_velocity5 = sqrt(gravitational_constant * sun.mass / distance5);
    Vector2 initial_velocity_vector5 = Vector2{-initial_velocity5, 0};

    planet earth = {Vector2{screenWidth/4, screenHeight/2}, 10, initial_velocity_vector, 5};
    planet earth2 = {Vector2{screenWidth/4, screenHeight/4}, 30, initial_velocity_vector2, 8};
    planet earth3 = {Vector2{3 * screenWidth/4, screenHeight/2}, 20, initial_velocity_vector3, 6};
    planet earth4 = {Vector2{screenWidth/2, screenHeight/4}, 15, initial_velocity_vector4, 5};
    planet earth5 = {Vector2{screenWidth/2, 3 * screenHeight/4}, 25, initial_velocity_vector5, 7};
    planet bodies[N] = {sun, earth, earth2, earth3, earth4, earth5};
    CircularBuffer positionHistory[N] = { CircularBuffer(100),  CircularBuffer(100),  CircularBuffer(100),  CircularBuffer(100),  CircularBuffer(100), CircularBuffer(100)};
    Color bodyColors[N] = {
        Color{255,255,0,255},
        Color{0,255,150,255},
        Color{150,150,255,255},
        Color{255,140,0,255},
        Color{180,255,180,255},
        Color{255,180,220,255}
    };


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
        //----------------------------------------------------------------------------------
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
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}