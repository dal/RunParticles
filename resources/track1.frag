#version 410 core

uniform vec4 uColor;

in vec2 gsTexCoord;
out vec4 fragColor;

void main(void)
{
    float yVal = 0.5;
    fragColor = uColor * yVal;
}
