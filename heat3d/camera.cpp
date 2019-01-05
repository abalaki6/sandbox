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

    return _camera;
}

void camera::process_input()
{
    
}