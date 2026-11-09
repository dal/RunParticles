#version 410 core

layout(location = 0) in vec3 position; // xy: map position, z: time in seconds

uniform mat4 mvp;

out float vTime;

void main(void)
{
    vTime = position.z;
    gl_Position = mvp * vec4(position.xy, 0.0, 1.0);
}
