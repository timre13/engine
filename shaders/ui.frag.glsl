#version 330 core

out vec4 outColor;

uniform vec3 uiColor;

void main()
{
    outColor = vec4(uiColor, 1.0f);
}

