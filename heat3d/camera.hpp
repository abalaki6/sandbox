#ifndef CAMERA_H
#define CAMERA_H

#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <math.h>
#include "shader.hpp"
#include "heat_parameters.hpp"

class camera
{
    enum camera_direciton {FRONT, BACK, LEFT, RIGHT};
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

    double aspect;

    void (camera::*input_handler)();

    camera();

    void normalize_basis();
    void process_input_free_camera();
    void update_position(const camera_direciton, const float dt);
    void zoom_hanlder(double dy);
public:
    static camera& get_camera();

    static camera& use_free_camera(GLFWwindow *window, shader* program);

    void process_input();
    void update_shader();
    glm::mat4 get_view_mat();
    glm::mat4 get_proj_mat();



    camera(const camera&) = delete;
    void operator=(const camera&) = delete;
};

#endif // CAMERA_H