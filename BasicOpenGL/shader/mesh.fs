#version 330 core

in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D diffuseMap0;
uniform sampler2D diffuseMap1;

void main()
{
    vec4 c1 = texture(diffuseMap0, TexCoord);
    vec4 c2 = texture(diffuseMap1, TexCoord);

    FragColor = mix(c1, c2, 0.5);
}