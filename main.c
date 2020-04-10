#include "glUtils.h"
#include "map.h"
#include "octree.h"
#include <pthread.h>

#define FRAMERATE 60

#define FRAMELIMIT

float vertices[] = {
        1.0f, 1.0f,
        1.0f, -1.0f,
        -1.0f, -1.0f,
        -1.0f, 1.0f,
        1.0f, 1.0f,
        -1.0f, -1.0f
};

int chunk[MAP_X_SIZE * MAP_Y_SIZE * MAP_Z_SIZE];

float blockData[] = {
        0.1f, 0.8f, 0.4f, 1.f,    0.f, 0.f, 0.f, 0.f,
        0.2f, 0.1f, 0.3f, 1.f,    0.f, 0.f, 0.f, 0.f,
        0.3f, 0.2f, 0.2f, 1.f,    0.f, 0.f, 0.f, 0.f,
        0.5f, 0.2f, 0.9f, 1.f,    0.f, 0.f, 0.f, 0.f,
        1.f, 1.f, 1.f, 1.f,    1.f, 1.f, 1.f, 1.f,

};

GLuint vao;
GLuint texID;
GLuint timeID;
GLuint cameraPosID;
GLuint cameraAngleID;
GLuint frameID;

int seed = 43522432;

#define TEX_HEIGHT_FACTOR 24
#define TEX_WIDTH_FACTOR 64

#define WORKGROUPS_Y 16 //(HEIGHT / TEX_HEIGHT_FACTOR)
#define WORKGROUPS_X 8 //(WIDTH / TEX_WIDTH_FACTOR)

static void VSyncOn() {
    glfwSwapInterval(1);
}

static void VSyncOff() {
    glfwSwapInterval(0);
}

void initVBO() {
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
}

double gtme() {
    return glfwGetTime();
}

void renderLoop(void* data) {
    void** arr = (void**)data;

    GLFWwindow* window = (GLFWwindow*) arr[0];
    GLuint* progs = (GLuint*) arr[1];
    uint32_t progcount = *((uint32_t*) arr[2]);

    /* Grab OpenGL Context */
    glfwMakeContextCurrent(window);

    float fpsFrames = 0;
    unsigned int frame = 1;
    float lastSec = (float) glfwGetTime();
    float lastRebuild = lastSec;

    double totalPerfTime = 50.0;
#ifdef FRAMELIMIT
    float lastTime = lastSec;
#endif

    while (!glfwWindowShouldClose(window)) {
        if (resize) {
            glViewport(0, 0, rWidth, rHeight);
            resize = false;
        }

#ifdef FRAMELIMIT
        if (glfwGetTime() > lastTime + (1.0 / FRAMERATE) * 5) {
            printf("Skipped frame!\n");
            continue;
        }
#endif

        float currTime = (float) glfwGetTime();

        fpsFrames++;
        float deltaT = currTime - lastSec;
        float ms = (deltaT / fpsFrames) * 1000.0;
        totalPerfTime += ms;
        if (deltaT > 1.0) {
            printf("%f ms/frame\n", ms);
            printf("Avg: %f ms/frame\n", (totalPerfTime / ((double) frame)));
            printf("Out of %d frames\n", frame);
            fpsFrames = 0;
            lastSec = currTime;
        }

        /*
        float deltaREEEEE = currTime - lastRebuild;
        if (deltaREEEEE > 2.0) {
            printf("REGENERATING MAP\n");
            seed++;
            setPerlin(chunk, seed);
            glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(chunk), chunk, GL_DYNAMIC_DRAW);
            lastRebuild = currTime;
        }
         */

        glUseProgram(progs[1]);

        glUniform3f(cameraPosID, cameraPos[0], cameraPos[1], cameraPos[2]);
        glUniform3f(cameraAngleID, cameraAngles[0], cameraAngles[1], cameraAngles[2]);

        glUniform1f(timeID, currTime);
        glUniform1ui(frameID, frame);
        frame++;

        glDispatchCompute((GLuint)WORKGROUPS_X, (GLuint)WORKGROUPS_Y, 1);

        //glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(progs[0]);
        glDrawArrays(GL_TRIANGLES, 0, 6);

#ifdef FRAMELIMIT
        double sleepTime = (lastTime + 1.0 / FRAMERATE) - currTime;
        if (sleepTime > 0) msleep((sleepTime * 1000));
#endif

        glfwSwapBuffers(window);

#ifdef FRAMELIMIT
        lastTime += 1.0 / FRAMERATE;
#endif
    }
}

void inputLoop(GLFWwindow* window) {
    float lastTime = (float) glfwGetTime();

    while (!glfwWindowShouldClose(window)) {
        float currTime = (float) glfwGetTime();
        float elapsed = currTime - lastTime;
        lastTime = currTime;

        updatePos(elapsed);

        glfwWaitEvents();
    }
}

void mainLoop(GLFWwindow* window, GLuint* progs, uint32_t progCount) {
#ifdef FRAMELIMIT
    VSyncOff();
#else
    VSyncOff();
#endif

    glfwMakeContextCurrent(NULL);

    /* Start input loop */
    pthread_t inputThread;
    void** arr = malloc(sizeof(void*) * 3);
    arr[0] = window;
    arr[1] = progs;
    arr[2] = &progCount;
    int inThread = pthread_create(&inputThread, NULL, renderLoop, (void*) arr);

    inputLoop(window);
    pthread_join(inputThread, NULL);
    free(arr);
    return;
    float fpsFrames = 0;
    float lastTime = (float) glfwGetTime();
    float lastSec = lastTime;

    /* Create key callback loop */

    while (!glfwWindowShouldClose(window)) {
        float currTime = (float) glfwGetTime();
        fpsFrames++;
        float elapsed = currTime - lastTime;
        lastTime = currTime;
        float deltaT = currTime - lastSec;
        if (deltaT > 1.0) {
            printf("%f ms/frame\n", (deltaT / fpsFrames) * 1000.0);
            fpsFrames = 0;
            lastSec = currTime;
        }

        //if (glfwGetTime() > lastTime + (1.0 / FRAMERATE) * 5) continue;
        //{
            glUseProgram(progs[1]);
        updatePos(elapsed);
        glUniform3f(cameraPosID, cameraPos[0], cameraPos[1], cameraPos[2]);
            glDispatchCompute((GLuint)WORKGROUPS_X, (GLuint)WORKGROUPS_Y, 1);
        //}
        glUniform1f(timeID, currTime);

        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        //{
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glUseProgram(progs[0]);
            //glBindVertexArray(vao);
            //glActiveTexture(GL_TEXTURE0);
            //glBindTexture(GL_TEXTURE_2D, texID);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        //}



        glfwSwapBuffers(window);
        glfwPollEvents();

        //glFinish();
        //updatePos(glfwGetTime() - lastTime);


        /*
        double sleepTime = (lastTime + 1.0 / FRAMERATE) - glfwGetTime();
        if (sleepTime > 0) msleep((sleepTime * 1000));
         */

        /*
        while (glfwGetTime() < lastTime + 1.0 / FRAMERATE) {

        }
         */
        //lastTime += 1.0 / FRAMERATE;
    }
}

int main() {


    GLFWwindow* window = setupWindow(false);
    setupInputs(window, framebufferSizeCallback, processCursor, processScroll, processKeys, processClick);
    initGlew(window);

    //glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    initVBO();

    printWorkGroupInfo();

    GLuint vShdr = loadVertexShader("../shaders/quad/shader.vert");
    GLuint fShdr = loadFragmentShader("../shaders/quad/shader.frag");

    GLuint quadProg = makeProgram(&vShdr, 1, NULL, 0, &fShdr, 1);


    glGenVertexArrays(1, &vao);

    glBindVertexArray(vao);
    GLint posAttrib = glGetAttribLocation(quadProg, "position");

    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glEnableVertexAttribArray(posAttrib);


    glGenTextures(1, &texID);

    GLfloat* blank = calloc(WIDTH * HEIGHT * 4, sizeof(GLfloat));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, WIDTH, HEIGHT, 0, GL_RGBA, GL_FLOAT, blank);
    glBindImageTexture(0, texID, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

    free(blank);


    GLuint cShdr = loadComputeShader("../shaders/rayTrace/monteCarloRays.glsl");
    GLuint cProg = makeProgram(NULL, 0, &cShdr, 1, NULL, 0);

    timeID = glGetUniformLocation(cProg, "time");
    glUniform1f(timeID, 0.5);

    frameID = glGetUniformLocation(cProg, "frame");
    glUniform1ui(timeID, 0);

    cameraPosID = glGetUniformLocation(cProg, "cameraPos");
    glUniform3f(cameraPosID, cameraPos[0], cameraPos[1], cameraPos[2]);

    cameraAngleID = glGetUniformLocation(cProg, "cameraAngles");
    glUniform3f(cameraAngleID, cameraAngles[0], cameraAngles[1], cameraAngles[2]);


    double buildTime = glfwGetTime();
    setPerlin(chunk, seed);
    buildTime = glfwGetTime() - buildTime;
    printf("Chunk generation took %f seconds\n", buildTime);

    GLuint voxChunk;
    glGenBuffers(1, &voxChunk);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, voxChunk);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(chunk), chunk, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, voxChunk);

    GLuint lightsHandle;
    glGenBuffers(1, &lightsHandle);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, lightsHandle);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(blockData), blockData, GL_STATIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, lightsHandle);

    GLint status;
    glGetProgramiv(quadProg, GL_VALIDATE_STATUS, &status);
    if (status != GL_TRUE) {
        char buffer[1024];
        glGetShaderInfoLog(quadProg, 1024, NULL, buffer);
        fprintf(stderr, "%s\n", buffer);
        glfwTerminate();
        exit(status);
    }

    glClearColor(0.4f, 0.4f, 0.6f, 1.0f );

    GLuint* progArray = malloc(sizeof(GLuint) * 2);
    progArray[0] = quadProg;
    progArray[1] = cProg;
    mainLoop(window, progArray, 2);

    free(progArray);

    glfwTerminate();
    return 0;
}
