#ifndef CAMERA_H
#define CAMERA_H

#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "shader.hpp"


class camera
{
    GLFWwindow *window;
    shader &program;
    
public:
    static camera& get_camera();

    static camera& use_free_camera(GLFWwindow *window, shader& program);

    void process_input();
};

#endif // CAMERA_H