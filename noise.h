#pragma once
#include <math.h>
#include <stdlib.h>

int p[512];

double fade(double t) { return t * t * t * (t * (t * 6 - 15) + 10); }
double lerp(double t, double a, double b) { return a + t * (b - a); }
double grad(int hash, double x, double y, double z) {
    int h = hash & 15;
    double u = h<8 ? x : y,
            v = h<4 ? y : h==12||h==14 ? x : z;
    return ((h&1) == 0 ? u : -u) + ((h&2) == 0 ? v : -v);
}

double noise(double x, double y, double z) {
    int X = (int)floor(x) & 255,
            Y = (int)floor(y) & 255,
            Z = (int)floor(z) & 255;
    x -= floor(x);
    y -= floor(y);
    z -= floor(z);
    double u = fade(x),
            v = fade(y),
            w = fade(z);
    int A = p[X  ]+Y, AA = p[A]+Z, AB = p[A+1]+Z,
            B = p[X+1]+Y, BA = p[B]+Z, BB = p[B+1]+Z;

    return lerp(w, lerp(v, lerp(u, grad(p[AA  ], x  , y  , z   ),
                                grad(p[BA  ], x-1, y  , z   )),
                        lerp(u, grad(p[AB  ], x  , y-1, z   ),
                             grad(p[BB  ], x-1, y-1, z   ))),
                lerp(v, lerp(u, grad(p[AA+1], x  , y  , z-1 ),
                             grad(p[BA+1], x-1, y  , z-1 )),
                     lerp(u, grad(p[AB+1], x  , y-1, z-1 ),
                          grad(p[BB+1], x-1, y-1, z-1 ))));
}

float gaussian(float a, float b, float c, float x) {
    return a * expf(-1 * (powf(x - b, 2) / powf(2 * c, 2)));
}

double octaveNoise(double x, double y, double z, int octaveCount) {
    double total = 0;
    double freq = 1;
    double ampl = 1;
    double maxVal = 0;
    for (int i = 0; i < octaveCount; i++) {
        total += noise(x * freq, y * freq, z * freq) * ampl;
        maxVal += ampl;
        ampl *= 0.5;
        freq *= 2;
    }
    return total / maxVal;
}

#define FOR(x, y) for (x = 0; x < y; x++)

void seedNoise(int seed) {
    int permutation[256];
    for (int i = 0; i < 256; i++) {
        permutation[i] = i;
    }

    srand(seed);
    size_t x;
    for (x = 0; x < 256 - 1; x++) {
        size_t j = x + rand() / (RAND_MAX / (256 - x) + 1);
        int t = permutation[j];
        permutation[j] = permutation[x];
        permutation[x] = t;
    }

    for (int i = 0; i < 256; i++) p[256 + i] = p[i] = permutation[i];
}

#undef FOR