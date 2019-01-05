#ifndef CAMERA_H
#define CAMERA_H

#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class camera
{
public:
    static void use_free_camera(GLFWwindow *window);
};

#endif // CAMERA_H