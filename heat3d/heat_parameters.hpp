#ifndef HEAT_PARAMETERS_H
#define HEAT_PARAMETERS_H

#pragma once
#include <boost/config.hpp>
#include <boost/program_options.hpp>
#include <exception>
#include <iostream>

#define DEBUG heat_parameters::get_instance().is_debug()

class heat_parameters
{
    double delta_t;                   // timestep
    double delta_x;                   // spacestep in x axis
    double delta_y;                   // spacestep in y axis
    double delta_z;                   // spacestep in z axis
    double alpha;                     // thermal diffusivity

    size_t num_iter;                  // number of iterations per frame
    bool debug;                       // debug mode for extra input

    int width;                        // window's width in pixels
    int height;                       // window's height in pixels

    double camera_yaw;                // moving "side"
    double camera_pitch;              // moving "up"
    double camera_speed;              // speed that cemera moves is direction
    double camera_mouse_sensitivity;  // how sensitive zooming is
    double camera_fov;                // equivaletly to zoom

    std::string capture_name;         // name of output video file

    boost::program_options::options_description desc;
    heat_parameters();
public:
    static heat_parameters& get_instance();

    void parse_cl(const int argc, const char **argv);


    // getters
    inline double get_dt() const {return delta_t;}
    inline double get_dx() const {return delta_x;}
    inline double get_dy() const {return delta_y;}
    inline double get_dz() const {return delta_z;}
    inline double get_alpha() const {return alpha;}
    inline size_t get_num_iter() const {return num_iter;}
    inline bool is_debug() const {return debug;}
    inline int get_window_width() const {return width;}
    inline int get_window_height() const {return height;}
    inline double get_camera_yaw() const {return camera_yaw;}
    inline double get_camera_pitch() const {return camera_pitch;}
    inline double get_camera_speed() const {return camera_speed;}
    inline double get_camera_fov() const {return camera_fov;}
    inline double get_camera_mouse_sensitivity() const {return camera_mouse_sensitivity;}
    inline std::string get_capture_name() const {return capture_name;}
    ~heat_parameters(){};

    heat_parameters(const heat_parameters&) = delete;
    void operator=(const heat_parameters&) = delete;
};

#endif //HEAT_PARAMETERS_H