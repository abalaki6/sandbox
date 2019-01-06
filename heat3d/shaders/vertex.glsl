#version 460

in vec3 aPos;
in vec3 aColor;

out vec3 outColor;

uniform mat4 PV

void main()
{
    gl_Position = PV * vec4(aPos, 1.0);
    outColor = aColor;
}
