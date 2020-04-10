#pragma once
#include "common.h"
#include "noise.h"
#include <math.h>

#define min(x, y) ((x < y) ? x : y)
#define max(x, y) ((x < y) ? y : x)

uint32_t expandBits(uint32_t x) {
    x = (x | (x << 16)) & 0x030000FF;
    x = (x | (x <<  8)) & 0x0300F00F;
    x = (x | (x <<  4)) & 0x030C30C3;
    x = (x | (x <<  2)) & 0x09249249;
    return x;
}

uint32_t morton(uint32_t x, uint32_t y, uint32_t z) {
    uint32_t xx = expandBits((uint32_t) x);
    uint32_t yy = expandBits((uint32_t) y);
    uint32_t zz = expandBits((uint32_t) z);

    return xx | (yy << 1) | (zz << 2);
}

#define RES 6.0
#define THRESHOLD 0.0

#define WID 0.1

void setPerlin(int* chunk, int seed) {
    seedNoise(seed);
    for (int x = 0; x < MAP_X_SIZE; x++) {
        for (int y = 0; y < MAP_Y_SIZE; y++) {
            for (int z = 0; z < MAP_Z_SIZE; z++) {
                double n = octaveNoise(((double) x) / ((double) MAP_X_SIZE),
                                 ((double) y) / ((double) MAP_Y_SIZE),
                                 ((double) z) / ((double) MAP_Z_SIZE),
                                 4);

                uint32_t mort = morton(x, y, z);
                if (n > THRESHOLD) {
                    chunk[mort] = (rand() % 4 + 1);
                    if (rand() % 50 == 1) {
                        chunk[mort] = 5;
                    }
                } else {
                    chunk[mort] = 0;
                }
            }
        }
    }
}

void setPerlinTerrain(int* chunk, int seed) {
    seedNoise(seed);
    for (int x = 0; x < MAP_X_SIZE; x++) {
        for (int z = 0; z < MAP_Z_SIZE; z++) {
            double n = noise(((double)x) * RES / ((double) MAP_X_SIZE), 0.0, ((double)z) * RES / ((double) MAP_Z_SIZE));
            n = (n + 1.0) / 2.0;

            for (int y = 0; y < MAP_Y_SIZE; y++) {
                uint32_t mort = morton(x, y, z);
                if (y <= (n * MAP_Y_SIZE)) {
                    chunk[mort] = 1;
                } else {
                    chunk[mort] = 0;
                }

            }
        }
        /*
        for (int y = 0; y < MAP_Y_SIZE; y++) {
            for (int z = 0; z < MAP_Z_SIZE; z++)  {
                double n = noise(((double)x) * RES / ((double) MAP_X_SIZE), ((double)z) * RES / ((double) MAP_Z_SIZE), 0.0);
                n = (n + 1.0) / 2.0;
                uint32_t mort = morton(x, y, z);
                if (n > THRESHOLD) {
                    chunk[mort] = 1;
                } else {
                    chunk[mort] = 0;
                }
            }
        }
         */
    }
}