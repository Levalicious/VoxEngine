#pragma once
#include "common.h"

GLFWwindow* setupWindow(bool fullscreen) {
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW.\n");
        exit(12);
    }

    // glfwWindowHint(GLFW_SAMPLES, 2); /* No antialiasing? */
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); /* OpenGL version 4.6 */
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window;
    if (fullscreen) {
        window = glfwCreateWindow(WIDTH, HEIGHT, "SVO", glfwGetPrimaryMonitor(), NULL); /* Make window fullscreen */
    } else {
        window = glfwCreateWindow(WIDTH, HEIGHT, "SVO", NULL, NULL);

    }
    if (window == NULL) {
        fprintf(stderr, "Failed to open GLFW window.\n");
        glfwTerminate();
        exit(12);
    }
    return window;
}

void setupInputs(GLFWwindow* window, GLFWframebuffersizefun framebufSize,
                 GLFWcursorposfun cursorPos,
                 GLFWscrollfun procScroll,
                 GLFWkeyfun procKey,
                 GLFWmousebuttonfun procClick) {

    glfwSetFramebufferSizeCallback(window, framebufSize);
    glfwSetCursorPosCallback(window, cursorPos);
    glfwSetScrollCallback(window, procScroll);
    glfwSetKeyCallback(window, procKey);
    glfwSetMouseButtonCallback(window, procClick);
}

void initGlew(GLFWwindow* window) {
    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;
    glewInit();
}

GLuint loadVertexShader(char* path) {
    GLuint out = glCreateShader(GL_VERTEX_SHADER);
    char* shaderSource = readFile(path);

    glShaderSource(out, 1, (const GLchar *const *) &shaderSource, NULL);
    glCompileShader(out);

    GLint status;
    glGetShaderiv(out, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE) {
        char buffer[1024];
        glGetShaderInfoLog(out, 1024, NULL, buffer);
        fprintf(stderr, "%s\n", buffer);
        glfwTerminate();
        exit(status);
    }
    free(shaderSource);
    return out;
}

GLuint loadComputeShader(char* path) {
    GLuint out = glCreateShader(GL_COMPUTE_SHADER);
    char* shaderSource = readFile(path);

    glShaderSource(out, 1, (const GLchar *const *) &shaderSource, NULL);
    glCompileShader(out);

    GLint status;
    glGetShaderiv(out, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE) {
        char buffer[1024];
        glGetShaderInfoLog(out, 1024, NULL, buffer);
        fprintf(stderr, "%s\n", buffer);
        glfwTerminate();
        exit(status);
    }
    free(shaderSource);
    return out;
}

GLuint loadFragmentShader(char* path) {
    GLuint out = glCreateShader(GL_FRAGMENT_SHADER);
    char* shaderSource = readFile(path);

    glShaderSource(out, 1, (const GLchar *const *) &shaderSource, NULL);
    glCompileShader(out);

    GLint status;
    glGetShaderiv(out, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE) {
        char buffer[1024];
        glGetShaderInfoLog(out, 1024, NULL, buffer);
        fprintf(stderr, "%s\n", buffer);
        glfwTerminate();
        exit(status);
    }
    free(shaderSource);
    return out;
}

GLuint makeProgram(GLuint* vertexShader, size_t vShdrCount, GLuint* computeShader, size_t cShdrCount, GLuint* fragmentShader, size_t fShdrCount) {
    GLuint shaderProgram = glCreateProgram();

    for (size_t i = 0; i < vShdrCount; ++i) {
        glAttachShader(shaderProgram, vertexShader[i]);
    }

    for (size_t i = 0; i < cShdrCount; ++i) {
        glAttachShader(shaderProgram, computeShader[i]);
    }

    for (size_t i = 0; i < fShdrCount; ++i) {
        glAttachShader(shaderProgram, fragmentShader[i]);
    }

    /* Specify fragment shader output buffer */
    glBindFragDataLocation(shaderProgram, 0, "outColor");

    /* Link shader program */
    glLinkProgram(shaderProgram);

    GLint status;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &status);
    if (status != GL_TRUE) {
        char buffer[1024];
        glGetShaderInfoLog(shaderProgram, 1024, NULL, buffer);
        fprintf(stderr, "%s\n", buffer);
        glfwTerminate();
        exit(status);
    }

    /* Use shader program */
    glUseProgram(shaderProgram);

    return shaderProgram;
}

void printWorkGroupInfo() {
    int workGroupCount[3];
    workGroupCount[0] = 0;

    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &workGroupCount[0]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &workGroupCount[1]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &workGroupCount[2]);

    int workGroupSize[3];

    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &workGroupSize[0]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &workGroupSize[1]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &workGroupSize[2]);

    int workGroupInv;
    glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &workGroupInv);

    printf("Max global work group size: x = %i, y = %i, z = %i\n", workGroupCount[0], workGroupCount[1], workGroupCount[2]);
    printf("Max local work group size: x = %i, y = %i, z = %i\n", workGroupSize[0], workGroupSize[1], workGroupSize[2]);
    printf("Max local invocations %i\n", workGroupInv);
}