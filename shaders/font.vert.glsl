#version 330 core

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec2 inUv;
layout (location = 2) in vec3 inNormal;

uniform vec2 characterPos;
uniform float fontScale;

void main()
{
    gl_Position = vec4((inPos.x+characterPos.x)*fontScale-0.98f, (-inPos.z+characterPos.y)*fontScale+0.97f, 0.0f, 1.0f);
}

