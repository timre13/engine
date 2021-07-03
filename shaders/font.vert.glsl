#version 330 core

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec2 inUv;
layout (location = 2) in vec3 inNormal;

uniform vec2 characterPos;
uniform float horizontalFontScale;
uniform float verticalFontScale;

void main()
{
    gl_Position = vec4((inPos.x+characterPos.x)*horizontalFontScale-0.98f, (-inPos.z+characterPos.y)*verticalFontScale+0.97f, 0.0f, 1.0f);
}

