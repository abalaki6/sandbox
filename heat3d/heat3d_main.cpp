#include "shader.hpp"
#include "heat_parameters.hpp"
#include "heat_solver.hpp"
#include "camera.hpp"

#include <opencv2/opencv.hpp>

GLFWwindow* init_gl();
int main(int argc, char const *argv[]);


int main(int argc, char const *argv[])
{
    // cli parser
    heat_parameters::get_instance().parse_cl(argc, argv);
    // init GLAD and GLFW contenct
    GLFWwindow* window = init_gl();
    // load shaders
    shader program("shaders/fragment.glsl", "shaders/vertex.glsl");
    program.set_window(window);
    // set up free camera
    camera::use_free_camera(window, &program);
    // create heat model with provided cli
    auto model = heat_solver(program);
    
    // video capture
    cv::VideoWriter *vcap = nullptr;
    char *screen_pixels = nullptr;
    if(DEBUG)
    {
        vcap = new cv::VideoWriter(heat_parameters::get_instance().get_capture_name(),
            CV_FOURCC('M','J','P','G'),
            120,
            cv::Size(heat_parameters::get_instance().get_window_width(),
                    heat_parameters::get_instance().get_window_height()),
            true);

        if(!vcap)
        {
            std::cerr << "ERROR::HEAT3D_MAIN: failed to create video writer." << std::endl;
        }
        else
        {
            screen_pixels = new char[3 *
                heat_parameters::get_instance().get_window_width() *
                heat_parameters::get_instance().get_window_height()];
        }
    }

    glEnable(GL_DEPTH_TEST);  
    
    std::string name = "echo";
    cv::namedWindow(name, cv::WINDOW_AUTOSIZE);


    model.run();
    while (!glfwWindowShouldClose(window))
    {
        camera::get_camera().process_input(); 
        // check for escape event
        if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, true);
        }
        // clean buffers
        glClearColor(1.f, 1.f, 1.f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        camera::get_camera().update_shader();
        // single evolution
        model.render();

        if(vcap)
        {
            glReadPixels(0,
                0,
                heat_parameters::get_instance().get_window_width(),
                heat_parameters::get_instance().get_window_height(),
                GL_BGR,
                GL_UNSIGNED_BYTE,
                screen_pixels
            );
            cv::Mat frame(
                heat_parameters::get_instance().get_window_height(),
                heat_parameters::get_instance().get_window_width(),
                CV_8UC3,
                screen_pixels
            );
            cv::flip(frame, frame, 0);
            vcap->write(frame);
        }


        glfwSwapBuffers(program.get_window());
        glfwPollEvents();
    }

    model.stop();

    if(screen_pixels)
    {
        delete screen_pixels;
        vcap->release();
    }

    glfwTerminate();
    return 0;
}

GLFWwindow* init_gl()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    auto w = heat_parameters::get_instance().get_window_width();
    auto h = heat_parameters::get_instance().get_window_height();
    GLFWwindow* window = glfwCreateWindow(w, h, "heat3d demo", NULL, NULL);
    if(window == NULL)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(-1);
    }
    glfwMakeContextCurrent(window);

    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int w, int h)
    {
        // glViewport(0, 0, w, h);
    }
    );

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        exit(-1);
    }

    return window;
}
