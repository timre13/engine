#version 330 core

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec2 inUv;
layout (location = 2) in vec3 inNormal;

uniform vec2 position;
uniform vec2 size;

void main()
{
    gl_Position = vec4(size.x, size.y, 0.0f, 1.0f) * vec4(inPos.x+position.x, inPos.z-position.y, 0.0f, 1.0f);
}

