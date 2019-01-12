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
    double _delta_t;                   // timestep
    double _delta_x;                   // spacestep in x axis
    double _delta_y;                   // spacestep in y axis
    double _delta_z;                   // spacestep in z axis
    double _alpha;                     // thermal diffusivity

    size_t _num_iter;                  // number of iterations per frame
    bool _debug;                       // debug mode for extra input

    int _width;                        // window's width in pixels
    int _height;                       // window's height in pixels

    double _camera_yaw;                // moving "side"
    double _camera_pitch;              // moving "up"
    double _camera_speed;              // speed that cemera moves is direction
    double _camera_mouse_sensitivity;  // how sensitive zooming is
    double _camera_fov;                // equivaletly to zoom

    std::string _capture_name;         // name of output video file
    size_t _fps;                       // fps of capturing
    std::string _source_name;          // name of the sourse of initial state

    boost::program_options::options_description desc;
    heat_parameters();
public:
    static heat_parameters& get_instance();

    void parse_cl(const int argc, const char **argv);


    // getters
    inline double get_dt() const {return _delta_t;}
    inline double get_dx() const {return _delta_x;}
    inline double get_dy() const {return _delta_y;}
    inline double get_dz() const {return _delta_z;}
    inline double get_alpha() const {return _alpha;}
    inline size_t get_num_iter() const {return _num_iter;}
    inline bool is_debug() const {return _debug;}
    inline int get_window_width() const {return _width;}
    inline int get_window_height() const {return _height;}
    inline double get_camera_yaw() const {return _camera_yaw;}
    inline double get_camera_pitch() const {return _camera_pitch;}
    inline double get_camera_speed() const {return _camera_speed;}
    inline double get_camera_fov() const {return _camera_fov;}
    inline double get_camera_mouse_sensitivity() const {return _camera_mouse_sensitivity;}
    inline std::string get_capture_name() const {return _capture_name;}
    inline size_t get_fps() const {return _fps;}
    inline std::string get_source_name() const {return _source_name;}
    // static getters
    static inline double dt() {return heat_parameters::get_instance().get_dt();}
    static inline double dx() {return heat_parameters::get_instance().get_dx();}
    static inline double dy() {return heat_parameters::get_instance().get_dy();}
    static inline double dz() {return heat_parameters::get_instance().get_dz();}
    static inline double alpha() {return heat_parameters::get_instance().get_alpha();}
    static inline size_t num_iter() {return heat_parameters::get_instance().get_num_iter();}
    static inline bool debug() {return heat_parameters::get_instance().is_debug();}
    static inline int window_width() {return heat_parameters::get_instance().get_window_width();}
    static inline int window_height() {return heat_parameters::get_instance().get_window_height();}
    static inline double camera_yaw() {return heat_parameters::get_instance().get_camera_yaw();}
    static inline double camera_pitch() {return heat_parameters::get_instance().get_camera_pitch();}
    static inline double camera_speed() {return heat_parameters::get_instance().get_camera_speed();}
    static inline double camera_fov() {return heat_parameters::get_instance().get_camera_fov();}
    static inline double camera_mouse_sensitivity() {return heat_parameters::get_instance().get_camera_mouse_sensitivity();}
    static inline std::string capture_name() {return heat_parameters::get_instance().get_capture_name();}
    static inline size_t fps() {return heat_parameters::get_instance().get_fps();}
    static inline std::string source_name() {return heat_parameters::get_instance().get_source_name();}




    ~heat_parameters(){};

    heat_parameters(const heat_parameters&) = delete;
    void operator=(const heat_parameters&) = delete;
};

#endif //HEAT_PARAMETERS_H