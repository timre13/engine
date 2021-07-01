#version 330 core

out vec4 outColor;

uniform vec3 characterColor;

void main()
{
    outColor = vec4(characterColor, 1.0f);
}

