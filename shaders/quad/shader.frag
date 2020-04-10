#version 460

in vec2 pixelPos;

uniform sampler2D tex;

out vec4 outColor;

void main() {
    outColor = texture(tex, pixelPos); //vec4(abs(pixelPos), 0.0, 1.0);
}