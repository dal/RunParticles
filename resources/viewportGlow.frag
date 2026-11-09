#version 410 core

uniform sampler2D tex0;

in vec2 vTexCoord;
out vec4 fragColor;

void main()
{
    fragColor = texture(tex0, vTexCoord);
}
