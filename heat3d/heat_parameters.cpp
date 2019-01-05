#include "heat_parameters.hpp"

namespace po = boost::program_options;

heat_parameters& heat_parameters::get_instance()
{
    static heat_parameters singleton;
    return singleton;
}

heat_parameters::heat_parameters()
: 
delta_t(1e-6),
delta_x(1e-3),
delta_y(1e-3),
delta_z(1e-3),
alpha(0.05),
num_iter(20),
debug(false),
width(800),
height(600),
camera_yaw(-90),
camera_pitch(0),
camera_speed(2.5),
camera_mouse_sensitivity(0.1),
camera_fov(45.)
{
    desc.add_options()
        (
            "help,h", "print this message.")
        (
            "dt", 
            po::value<double>()->default_value(delta_t), 
            "timestep per iteration."
        )
        (
            "dx", 
            po::value<double>()->default_value(delta_x), 
            "space step in Ox, for stability dt < dr^2."
        )
        (
            "dy", 
            po::value<double>()->default_value(delta_y), 
            "space step in Oy, for stability dt < dr^2."
        )
        (
            "dz", 
            po::value<double>()->default_value(delta_z), 
            "space step in Oz, for stability dt < dr^2."
        )
        (
            "alpha,a", 
            po::value<double>()->default_value(alpha),
            "thermal diffusivity of the system."
        )
        (
            "debug,d", "run with debug information."
        )
        (
            "iter,i",
            po::value<size_t>()->default_value(num_iter),
            "number of iterations per frame."
        )
        (
            "width,w",
            po::value<int>()->default_value(width),
            "width of the window in startup."
        )
        (
            "height,h",
            po::value<int>()->default_value(height),
            "height of the window in startup."
        )
        (
            "yaw", 
            po::value<double>()->default_value(camera_yaw), 
            "initial yaw value of the camera."
        )
        (
            "pitch", 
            po::value<double>()->default_value(camera_pitch), 
            "initial pitch value of the camera."
        )
        (
            "speed", 
            po::value<double>()->default_value(camera_speed), 
            "speed that camera moves in the front direction."
        )
        (
            "mouse", 
            po::value<double>()->default_value(camera_mouse_sensitivity), 
            "mouse sesitivity while zooming."
        )
        (
            "fov", 
            po::value<double>()->default_value(camera_fov), 
            "fov of the camera."
        );
}

void heat_parameters::parse_cl(const int argc, const char **argv)
{
    try{

        po::variables_map vmap;
        po::store(po::parse_command_line(argc, argv, desc), vmap);
        po::notify(vmap);

        if(vmap.count("help"))
        {
            std::cout << desc;
            exit(0);
        }
        if(vmap.count("debug"))
            debug = true;

        delta_t = vmap["dt"].as<double>();
        delta_x = vmap["dx"].as<double>();
        delta_y = vmap["dy"].as<double>();
        delta_z = vmap["dz"].as<double>();
        alpha = vmap["alpha"].as<double>();
        num_iter = vmap["iter"].as<size_t>();
        
        width = vmap["width"].as<int>();
        height = vmap["height"].as<int>();
        
        camera_yaw = vmap["yaw"].as<double>();
        camera_pitch = vmap["pitch"].as<double>();
        camera_speed = vmap["speed"].as<double>();
        camera_mouse_sensitivity = vmap["mouse"].as<double>();
        camera_fov = vmap["fov"].as<double>();

    }
    catch(std::exception& e)
    {
        std::cerr << "HEAT_PARAMETERS::PARSE_CL: " << e.what() << std::endl;
        exit(1);
    }
    catch(...)
    {
        std::cerr << "HEAT_PARAMETERS::PARSE_CL: Unknown error" << std::endl;
        exit(2);
    }
}