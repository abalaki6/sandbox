#ifndef HEAT_PARAMETERS_H
#define HEAT_PARAMETERS_H

#pragma once
#include <boost/config.hpp>
#include <boost/program_options.hpp>
#include <exception>
#include <iostream>


class heat_parameters
{
    double delta_t;  // timestep
    double delta_x;  // spacestep in x axis
    double delta_y;  // spacestep in y axis
    double delta_z;  // spacestep in z axis
    double alpha;    // thermal diffusivity

    size_t num_iter; // number of iterations per frame
    bool debug;      // debug mode for extra input

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

    ~heat_parameters(){};

    heat_parameters(const heat_parameters&) = delete;
    void operator=(const heat_parameters&) = delete;
};

#endif //HEAT_PARAMETERS_H