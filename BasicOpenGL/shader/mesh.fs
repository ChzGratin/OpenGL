#version 330 core

in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D diffuseMap0;

void main()
{
    FragColor = texture(diffuseMap0, TexCoord);
}