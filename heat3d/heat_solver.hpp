#ifndef HEAT_SOLVER_H
#define HEAT_SOLVER_H

#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <thread>
#include <memory.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "heat_parameters.hpp"
#include <opencv2/opencv.hpp>
#include "shader.hpp"



class heat_solver
{
    size_t X;                // number of cells in Ox
    size_t Y;                // number of cells in Oy
    size_t Z;                // number of cells in Oz

    double *state;           // XxYxZ array of current state
    double *buffer;          // XxYxZ buffer
    float *heat_map;         // XxyxZx3 array of current state using heat mapping

    const shader &program;   // program to render
    GLuint VAO;              // VAO of buffer in glsl

    std::thread *worker;     // thread that does map updates
    bool run_thread;         // flag to stop computations

    void load_initial_state();
    void bind_vertex_location();
public:
    heat_solver(const shader&);

    void evolve();
    void update_color_map();
    void render();

    void run();
    void stop();

    double *get_current_state_3channes();

    ~heat_solver();
};


#endif // HEAT_SOLVER_H