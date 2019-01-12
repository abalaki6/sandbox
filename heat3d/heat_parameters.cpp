#include "heat_parameters.hpp"

namespace po = boost::program_options;

heat_parameters& heat_parameters::get_instance()
{
    static heat_parameters singleton;
    return singleton;
}

heat_parameters::heat_parameters()
: 
_delta_t(1e-4),
_delta_x(1e-2),
_delta_y(1e-2),
_delta_z(1e-2),
_alpha(0.005),
_num_iter(10),
_debug(false),
_width(800),
_height(600),
_camera_yaw(-90),
_camera_pitch(0),
_camera_speed(1.5),
_camera_mouse_sensitivity(0.1),
_camera_fov(90.),
_capture_name("heat3d_video.avi"),
_fps(120),
_source_name("state.bin")
{
    desc.add_options()
        (
            "help,h", "print this message.")
        (
            "dt", 
            po::value<double>()->default_value(_delta_t), 
            "timestep per iteration."
        )
        (
            "dx", 
            po::value<double>()->default_value(_delta_x), 
            "space step in Ox, for stability dt < dr^2."
        )
        (
            "dy", 
            po::value<double>()->default_value(_delta_y), 
            "space step in Oy, for stability dt < dr^2."
        )
        (
            "dz", 
            po::value<double>()->default_value(_delta_z), 
            "space step in Oz, for stability dt < dr^2."
        )
        (
            "alpha,a", 
            po::value<double>()->default_value(_alpha),
            "thermal diffusivity of the system."
        )
        (
            "debug,d", "run with debug information."
        )
        (
            "iter,i",
            po::value<size_t>()->default_value(_num_iter),
            "number of iterations per frame."
        )
        (
            "width",
            po::value<int>()->default_value(_width),
            "width of the window in startup."
        )
        (
            "height",
            po::value<int>()->default_value(_height),
            "height of the window in startup."
        )
        (
            "yaw", 
            po::value<double>()->default_value(_camera_yaw), 
            "initial yaw value of the camera."
        )
        (
            "pitch", 
            po::value<double>()->default_value(_camera_pitch), 
            "initial pitch value of the camera."
        )
        (
            "speed", 
            po::value<double>()->default_value(_camera_speed), 
            "speed that camera moves in the front direction."
        )
        (
            "mouse", 
            po::value<double>()->default_value(_camera_mouse_sensitivity), 
            "mouse sesitivity while zooming."
        )
        (
            "fov", 
            po::value<double>()->default_value(_camera_fov), 
            "fov of the camera."
        )
        (
            "video",
            po::value<std::string>()->default_value(_capture_name),
            "name of recorded video, video is recorded if debug flag specified."
        )
        (
            "fps",
            po::value<size_t>()->default_value(_fps),
            "fps that video is captured."
        )
        (
            "fstate",
            po::value<std::string>()->default_value(_source_name),
            "name of binary file with (1/dx+1)x(1/dy+1)x(1/dz+1) grid of float32 initial state."
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
            _debug = true;

        _delta_t = vmap["dt"].as<double>();
        _delta_x = vmap["dx"].as<double>();
        _delta_y = vmap["dy"].as<double>();
        _delta_z = vmap["dz"].as<double>();
        _alpha = vmap["alpha"].as<double>();
        _num_iter = vmap["iter"].as<size_t>();
        
        _width = vmap["width"].as<int>();
        _height = vmap["height"].as<int>();
        
        _camera_yaw = vmap["yaw"].as<double>();
        _camera_pitch = vmap["pitch"].as<double>();
        _camera_speed = vmap["speed"].as<double>();
        _camera_mouse_sensitivity = vmap["mouse"].as<double>();
        _camera_fov = vmap["fov"].as<double>();


        _capture_name = vmap["video"].as<std::string>();
        _fps = vmap["fps"].as<size_t>();
        _source_name = vmap["fstate"].as<std::string>();
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