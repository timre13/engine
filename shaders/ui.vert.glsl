#version 330 core

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec2 inUv;
layout (location = 2) in vec3 inNormal;

uniform vec2 position;
uniform vec2 size;

void main()
{
    vec2 scaledPos = vec2(size.x, size.y) * vec2(inPos.x, inPos.z);
    gl_Position = vec4(scaledPos.x+position.x, scaledPos.y-position.y, 0.0f, 1.0f);
}

