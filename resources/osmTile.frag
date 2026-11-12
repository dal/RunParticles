#version 410 core

uniform sampler2D tex0;

in vec2 vTexCoord;
out vec4 fragColor;

void main()
{
    vec4 texelColor = texture(tex0, vTexCoord);
    vec4 scaledColor = texelColor * vec4(0.3, 0.59, 0.11, 1.0);
    float luminance = scaledColor.r + scaledColor.g + scaledColor.b;
    luminance = 1.0 - luminance;
    fragColor = vec4(luminance, luminance, luminance, texelColor.a);
}
