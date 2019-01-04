#version 460

layout (location 0) in vec3 aPos;
in vec3 aColor;

out vec3 oColor;

void main()
{
    
    gl_Position = vec4(v_pos, 1.0)
}