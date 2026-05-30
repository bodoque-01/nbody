#pragma once
#include "raymath.h"

// Shared simulation setup: the body type and the constants used to lay out and
// evolve the system. Included by both the renderer (main.cpp) and the benchmark.
struct planet {
    Vector2 position;
    int mass;
    Vector2 velocity;
    int radius;
};

const int screenWidth = 1520;
const int screenHeight = 1080;

const double gravitational_constant = 200.0;
const float simulationDt = 1.0f / 240.0f;

const int sunMass = 10000;
const int planetMass = 5;
const int sunRadius = 8;
const int planetRadius = 1;

const float goldenAngle = 2.39996f;
const float particleRadius = 5.0f;
const float c = (particleRadius * 2) * 1.5f;
const float innerRadius = 80.0f; // push the whole spiral out so inner particles don't crowd the center

const float startX = (float)screenWidth / 2.0f;
const float startY = (float)screenHeight / 2.0f;
