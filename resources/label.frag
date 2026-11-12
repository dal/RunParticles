#version 410 core

uniform sampler2D tex0;
uniform vec4 uColor;

in vec2 vTexCoord;
out vec4 fragColor;

void main()
{
    // The label texture is premultiplied RGBA
    fragColor = texture(tex0, vTexCoord) * uColor;
}
