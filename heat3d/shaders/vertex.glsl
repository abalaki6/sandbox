#version 460

in vec3 aPos;
in vec3 aColor;

out vec3 outColor;

void main()
{
    gl_Position = vec4(aPos, 1.0);
    outColor = aColor;
}
