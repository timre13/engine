#version 330 core

layout (location = 0) in vec4 vertex; // Position, Texture coords
out vec2 texCoords;

uniform mat4 projectionMat;

void main()
{
    gl_Position = projectionMat * vec4(vertex.xy, 0.0f, 1.0f);
    texCoords = vertex.zw;
}

