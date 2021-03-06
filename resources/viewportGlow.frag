#version 330 core

out vec4 fragColor;
uniform sampler2D tex0;

void main()
{
    gl_FragColor = texture2D(tex0, gl_TexCoord[0].st);
}

