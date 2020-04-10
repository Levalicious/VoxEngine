#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <math.h>
#include <time.h>
#include <errno.h>

#define MAP_X_SIZE 64
#define MAP_Y_SIZE 64
#define MAP_Z_SIZE 64

#define WIDTH 1920
#define HEIGHT 1080

#define SPEED 16.f

typedef float vec3[3];
typedef float vec2[2];

vec3 cameraPos = { ((float) MAP_X_SIZE / 2), ((float) MAP_Y_SIZE / 2), -30.f };
vec3 cameraAngles = { 0.f, 0.f, 1.f };
vec3 cameraSpeed = { 0.f, 0.f, 0.f };
bool moveKeys[6];

vec2 lastCursorPos = { 0.f, 0.f };

bool resize = false;
int rWidth = WIDTH;
int rHeight = HEIGHT;

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    resize = true;
    rWidth = width;
    rHeight = height;
}

void processScroll(GLFWwindow* window, double xOffset, double yOffset) {

}

void updatePos(float elapsed) {
    cameraPos[0] += cameraSpeed[0] * elapsed;
    cameraPos[1] += cameraSpeed[1] * elapsed;
    cameraPos[2] += cameraSpeed[2] * elapsed;
}

void processKeys(GLFWwindow* window, int button, int scanCode, int action, int mods) {
    switch (button) {
        case GLFW_KEY_ESCAPE: {
            if (action == GLFW_PRESS) {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            } else if (action == GLFW_RELEASE) {

            }
            break;
        }
        case GLFW_KEY_W: {
            if (action == GLFW_PRESS) {
                printf("Pressed W\n");
                cameraSpeed[2] += SPEED;
            } else if (action == GLFW_REPEAT) {

            } else if (action == GLFW_RELEASE) {
                printf("Released W\n");
                cameraSpeed[2] -= SPEED;
            }
            break;
        }
        case GLFW_KEY_S: {
            if (action == GLFW_PRESS) {
                printf("Pressed S\n");
                cameraSpeed[2] -= SPEED;
            } else if (action == GLFW_REPEAT) {

            } else if (action == GLFW_RELEASE) {
                printf("Released S\n");
                cameraSpeed[2] += SPEED;
            }
            break;
        }
        case GLFW_KEY_A: {
            if (action == GLFW_PRESS) {
                printf("Pressed A\n");
                cameraSpeed[0] -= SPEED;
            } else if (action == GLFW_REPEAT) {

            } else if (action == GLFW_RELEASE) {
                printf("Released A\n");
                cameraSpeed[0] += SPEED;
            }
            break;
        }
        case GLFW_KEY_D: {
            if (action == GLFW_PRESS) {
                printf("Pressed D\n");
                cameraSpeed[0] += SPEED;
            } else if (action == GLFW_REPEAT) {

            } else if (action == GLFW_RELEASE) {
                printf("Released D\n");
                cameraSpeed[0] -= SPEED;
            }
            break;
        }
        case GLFW_KEY_SPACE: {
            if (action == GLFW_PRESS) {
                printf("Pressed SPACE\n");
                cameraSpeed[1] += SPEED;
            } else if (action == GLFW_REPEAT) {

            } else if (action == GLFW_RELEASE) {
                printf("Released SPACE\n");
                cameraSpeed[1] -= SPEED;
            }
            break;
        }
        case GLFW_KEY_LEFT_SHIFT: {
            if (action == GLFW_PRESS) {
                printf("Pressed LSHIFT\n");
                cameraSpeed[1] -= SPEED;
            } else if (action == GLFW_REPEAT) {

            } else if (action == GLFW_RELEASE) {
                printf("Released LSHIFT\n");
                cameraSpeed[1] += SPEED;
            }
            break;
        }
        case GLFW_KEY_TAB: {
            if (action == GLFW_PRESS) {

            } else if (action == GLFW_RELEASE) {
                glfwSetWindowShouldClose(window, GL_TRUE);
            }
            break;
        }
        default: return;
    }
}

void processCursor(GLFWwindow* window, double xPos, double yPos) {
    float diffX = xPos - lastCursorPos[0];
    float diffY = yPos - lastCursorPos[1];

    lastCursorPos[0] = xPos;
    lastCursorPos[1] = yPos;

    cameraAngles[0] += diffX * 0.05;
    cameraAngles[1] += diffY * 0.05;
}

void processClick(GLFWwindow* window, int button, int action, int mods) {
    switch (button) {
        case GLFW_MOUSE_BUTTON_LEFT: {
            if (action == GLFW_PRESS) {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            } else if (action == GLFW_RELEASE) {

            }
            break;
        }
        case GLFW_MOUSE_BUTTON_RIGHT: {
            if (action == GLFW_PRESS) {

            } else if (action == GLFW_RELEASE) {

            }
            break;
        }
        default: return;
    }
}

static char* readFile(const char* path) {
    FILE* file = fopen(path, "rb");
    if (file == NULL) {
        fprintf(stderr, "Could not open file \"%s\".\n", path);
        exit(-1);
    }

    fseek(file, 0L, SEEK_END);
    size_t fileSize = ftell(file);
    rewind(file);

    char* buffer = (char*) malloc(fileSize + 1);
    if (buffer == NULL) {
        fprintf(stderr, "Not enough memory to read \"%s\".\n", path);
        exit(-1);
    }

    size_t bytesRead = fread(buffer, sizeof(char), fileSize, file);
    if (bytesRead < fileSize) {
        fprintf(stderr, "Could not read file \"%s\".\n", path);
        exit(-1);
    }

    buffer[bytesRead] = '\0';

    fclose(file);
    return buffer;
}

int msleep(long msec)
{
    struct timespec ts;
    int res;

    if (msec < 0)
    {
        errno = EINVAL;
        return -1;
    }

    ts.tv_sec = msec / 1000;
    ts.tv_nsec = (msec % 1000) * 1000000;

    do {
        res = nanosleep(&ts, &ts);
    } while (res && errno == EINTR);

    return res;
}