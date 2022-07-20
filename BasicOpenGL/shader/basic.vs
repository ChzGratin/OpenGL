#version 330 core
/*
struct Vertex
{
    glm::vec3 position;
    glm::vec3 color;
};
*/
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec4 vertColor;

void main()
{
    gl_Position = vec4(aPos, 1.0);
    vertColor = vec4(aColor, 1.0);
}