#ifndef HEAT_SOLVER_H
#define HEAT_SOLVER_H

#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "heat_parameters.hpp"
#include <iostream>
#include <opencv2/opencv.hpp>
#include "shader.hpp"



class heat_solver
{
    size_t X;                // number of cells in Ox
    size_t Y;                // number of cells in Oy
    size_t Z;                // number of cells in Oz

    double *state;           // XxYxZ array of current state
    double *buffer;          // XxYxZ buffer
    double *heat_map;        // XxyxZx3 array of current state using heat mapping

    const shader &program;   // program to render
    GLuint posVAO;           // VAO of possitions buffer in glsl

    GLuint bind_vertex_location();
public:
    heat_solver(const shader&);

    void evolve();
    void update_color_map();
    void render();

    double *get_current_state_3channes();

    ~heat_solver();
};


#endif // HEAT_SOLVER_H