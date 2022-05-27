#version 330 core

out vec4 outColor;

uniform vec3 lineColor;

void main()
{
    outColor = vec4(lineColor.rgb, 1.0);
}

