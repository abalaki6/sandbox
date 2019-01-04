#version 460

layout (location 0) in vec3 aColor;

out vec4 FragColor;

void main()
{
    FragColor = vec4(aColor, 1.0);
}