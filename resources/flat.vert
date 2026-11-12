#version 410 core

layout(location = 0) in vec2 position;

uniform mat4 mvp;

void main(void)
{
    gl_Position = mvp * vec4(position, 0.0, 1.0);
}
