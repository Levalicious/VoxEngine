#version 460 core

in vec2 position;

out vec2 pixelPos;

void main() {
    gl_Position = vec4(position, 0.0, 1.0);
    pixelPos = position * 0.5 + 0.5;
}