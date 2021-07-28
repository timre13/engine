#version 330 core

layout (location = 0) in vec2 inPos;

uniform mat4 projectionMat;

void main()
{
    gl_Position = projectionMat * vec4(inPos.xy, 0.0f, 1.0f);
}

