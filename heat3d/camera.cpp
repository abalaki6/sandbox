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
    _camera.speed = params.get_camera_speed();
    _camera.mouse_sensitivity = params.get_camera_mouse_sensitivity();
    _camera.aspect = (double)params.get_window_width() / params.get_window_height();

    _camera.position = {0.40,0.34,1.25};
    _camera.w_up = {0,1,0};
    _camera.direction = {0.13,0.19,-0.97};

    _camera.normalize_basis();
    _camera.input_handler = &camera::process_input_free_camera;

    glfwSetScrollCallback(window, [](GLFWwindow* window, double dx, double dy)
    {
        camera::get_camera().zoom_hanlder(dy);
    });

    glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos)
    {
        // camera::get_camera().mouse_event_static(xpos, ypos);
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
    return glm::perspective(glm::radians(fov), aspect, 0.1, 1000.0); 
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

    mouse_event_dynamic(dt);

    normalize_basis();
}

void camera::mouse_event_dynamic(float dt)
{
    static bool first_time = true;
    static double x=0, y=0;
    if(first_time)
    {
        x = heat_parameters::get_instance().get_window_width() / 2;
        y = heat_parameters::get_instance().get_window_height() / 2;
    }

    double xpos, ypos;
    glfwGetCursorPos(program->get_window(), &xpos, &ypos);

    if(abs(xpos - x) > 80)
        yaw += (xpos - x) * mouse_sensitivity * dt;
    if(abs(ypos - y) > 80)
        pitch += (y - ypos) * mouse_sensitivity * dt;

    
    if(pitch > 89.9) pitch = 89.9;
    if(pitch < -89.9) pitch = -89.9;
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

    if(DEBUG)
    {
        std::cout << "moves to " << dir << std::endl
        << "position: " << glm::to_string(position) << std::endl
        << "direction: " << glm::to_string(direction) << std::endl
        << "up" << glm::to_string(up) << std::endl;
    }
}

void camera::zoom_hanlder(double dy)
{
    fov -= dy;
    
    if (fov<= 1.0f)
        fov = 1.0f;
    if (fov >= 140.0f)
        fov = 140.0f;

    if(DEBUG)
    {
        std::cout << "fov=" << fov << std::endl;
    }
}

void camera::mouse_event_static(double xpos, double ypos)
{
    static bool first_time = true;
    static double prev_x, prev_y;
    if(first_time)
    {
        prev_x = xpos;
        prev_y = ypos;
        first_time = false;
        return;
    }
    yaw += (xpos - prev_x) * mouse_sensitivity;
    pitch += (prev_y - ypos) * mouse_sensitivity;

    prev_x = xpos;
    prev_y = ypos;

    if(pitch > 89.9) pitch = 89.9;
    if(pitch < -89.9) pitch = -89.9;

    normalize_basis();
}

void camera::update_shader()
{
    auto model = get_proj_mat() * get_view_mat();
    program->set_mat4("PV", model);
}