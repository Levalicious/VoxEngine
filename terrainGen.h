
#pragma once
#include "common.h"
#include "jc_voronoi.h"
#include <string.h>

/* Bad former code: https://pastebin.com/ACuLSpga */

#define rllcPrd 16

typedef vec2 tri[3];
typedef vec2 edge[2];

float len(vec2 a, vec2 b) {
    return sqrtf(((a[0] - b[0]) * (a[0] - b[0])) + ((a[1] - b[1]) * (a[1] - b[1])));
}

bool inCircumcircle(tri t, vec2 point) {
    float a, b, c;
    a = len(t[0], t[1]);
    b = len(t[1], t[2]);
    c = len(t[2], t[0]);

    float r = (a * b * c) / sqrtf((a + b + c) * (b + c - a) * (c + a - b) * (a + b - c));

    float A, B, C;
    A = acosf(((b * b) + (c * c) - (a * a)) / (2.f * b * c));
    B = acosf(((c * c) + (a * a) - (b * b)) / (2.f * c * a));
    C = acosf(((a * a) + (b * b) - (c * c)) / (2.f * a * b));

    float s2A, s2B, s2C;
    s2A = sinf(2.f * A);
    s2B = sinf(2.f * B);
    s2C = sinf(2.f * C);

    vec2 center;
    center[0] = (t[0][0] * s2A + t[1][0] * s2B + t[2][0] * s2C) / (s2A + s2B + s2C);
    center[1] = (t[0][1] * s2A + t[1][1] * s2B + t[2][1] * s2C) / (s2A + s2B + s2C);

    return len(center, point) < r;
}

const static float maxAge = 90000.f;
const static float maxElev = 90000.f;
const static float maxVel = 100.f;

void heightmap(float** map, int mapSize, int seed, int plates) {
    srand(seed);
    /* Generate points */
    jcv_point* platePoints = malloc(sizeof(jcv_point) * plates);
    float* plateAges = malloc(sizeof(float) * plates);
    float* plateElev = malloc(sizeof(float) * plates);
    float* plateXVel = malloc(sizeof(float) * plates);
    float* plateYVel = malloc(sizeof(float) * plates);

    for (int i = 0; i < plates; i++) {
        platePoints[i].x = ((float)rand() / (float)(RAND_MAX)) * mapSize;
        platePoints[i].y = ((float)rand() / (float)(RAND_MAX)) * mapSize;
        plateAges[i] = ((float)rand() / (float)(RAND_MAX)) * maxAge;
        plateElev[i] = ((float)rand() / (float)(RAND_MAX)) * maxElev;
        plateXVel[i] = ((float)rand() / (float)(RAND_MAX)) * maxVel;
        plateYVel[i] = ((float)rand() / (float)(RAND_MAX)) * maxVel;
    }

    jcv_diagram dia;
    memset(&dia, 0, sizeof(jcv_diagram));
    jcv_rect rect;
    memset(&rect, 0, sizeof(jcv_rect));
    rect.max.x = mapSize;
    rect.max.y = mapSize;
    rect.min.x = 0;
    rect.min.y = 0;
    jcv_diagram_generate(plates, platePoints, &rect, NULL, &dia);

    /* This ^^ gives me voronoi-shaped plates with elevations, ages, and velocities.
     * The plate boundaries need to be noise perturbed, the plates need to be separated
     * into continental and oceanic (oceanic are younger), and then there needs to be some
     * way to simulate fault features:
     * https://leatherbee.org/index.php/2018/11/19/terrain-generation-5-fault-features/ */



}
