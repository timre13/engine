#version 330 core

in vec2 texCoords;
out vec4 outColor;

uniform sampler2D tex;
uniform vec3 textColor;

void main()
{
    outColor = vec4(textColor, 1.0f) * vec4(1.0f, 1.0f, 1.0f, texture(tex, texCoords).r);
}

