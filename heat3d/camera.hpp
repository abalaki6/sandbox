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
    shader *program;
    
    glm::vec3 position;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 direction;

    glm::vec3 w_up;

    double yaw;
    double pitch;
    double speed;
    double mouse_sensitivity;
    double fov;

    camera();
public:
    static camera& get_camera();

    static camera& use_free_camera(GLFWwindow *window, shader* program);

    void process_input();


    camera(const camera&) = delete;
    void operator=(const camera&) = delete;
};

#endif // CAMERA_H