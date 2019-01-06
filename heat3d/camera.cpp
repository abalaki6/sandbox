#include "camera.hpp"

camera::camera()
:
program(nullptr),
window(nullptr),
yaw(0),
pitch(0),
speed(0),
mouse_sensitivity(0),
fov(0)
{}


camera& camera::get_camera()
{
    static camera _camera;

    return _camera;
}

camera& camera::use_free_camera(GLFWwindow* window, shader* program)
{
    camera& _camera = camera::get_camera();

    _camera.window = window;
    _camera.program = program;

    auto &params = heat_parameters::get_instance();
    _camera.yaw = params.get_camera_yaw();
    _camera.pitch = params.get_camera_pitch();
    _camera.fov = params.get_camera_fov();
    _camera.mouse_sensitivity = params.get_camera_mouse_sensitivity();
    _camera.aspect = (double)params.get_window_width() / params.get_window_height();

    _camera.position = {0,0,-1};
    _camera.w_up = {0,1,0};
    _camera.direction = {0,0,1};

    _camera.normalize_basis();
    _camera.input_handler = &camera::process_input_free_camera;

    glfwSetScrollCallback(window, [](GLFWwindow* window, double dx, double dy)
    {
        camera::get_camera().zoom_hanlder(dy);
    });

    return _camera;
}

void camera::process_input()
{
    (this->*input_handler)();   
}

glm::mat4 camera::get_view_mat()
{
    return glm::lookAt(position, position + direction, up);
}

glm::mat4 camera::get_proj_mat()
{
    return glm::perspective(glm::radians(fov), aspect, 0.1, 100.0); 
}

void camera::normalize_basis()
{
    glm::vec3 vec;
    vec.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    vec.y = sin(glm::radians(pitch));
    vec.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

    direction = glm::normalize(vec);
    right = glm::normalize(glm::cross(direction, w_up));
    up = glm::normalize(glm::cross(right, direction));
}

void camera::process_input_free_camera()
{
    static float prev_time = glfwGetTime();
    
    float cur_time = glfwGetTime();
    float dt = cur_time - prev_time;
    prev_time = cur_time;

    // process camera keyboard events
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        update_position(camera_direciton::FRONT, dt);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        update_position(camera_direciton::BACK, dt);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        update_position(camera_direciton::LEFT, dt);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        update_position(camera_direciton::RIGHT, dt); 
}

void camera::update_position(const camera_direciton dir, const float dt)
{
    float vel = speed * dt;
    switch(dir)
    {
        case camera_direciton::FRONT:
            position += direction * vel;
            break;
        case camera_direciton::BACK:
            position -= direction * vel;
            break;
        case camera_direciton::LEFT:
            position -= right * vel;
            break;
        case camera_direciton::RIGHT:
            position += right * vel;
            break;
    }
}

void camera::zoom_hanlder(double dy)
{
    fov -= dy;
    
    if (fov<= 1.0f)
        fov = 1.0f;
    if (fov >= 140.0f)
        fov = 140.0f;
}

void camera::update_shader()
{
    auto model = glm::matrixCompMult(get_proj_mat(), get_view_mat());

    program->use_program();
    program->set_mat4("PV", model);
}