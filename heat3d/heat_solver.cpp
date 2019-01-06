#include "heat_solver.hpp"

GLuint heat_solver::bind_vertex_location()
{
    float vertices[] = {
        // positions         // colors
         0.5f, -.5f, -5.0f,  1.0f, 0.0f, 0.0f,  // bottom right
        -0.5f, -.5f, -1.0f,  0.0f, 1.0f, 0.0f,  // bottom left
         0.0f,  .5f, -1.0f,  0.0f, 0.0f, 1.0f   // top 

    };
    
    GLuint VBO, VAO;
    
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(glGetAttribLocation(program.get_id(), "aPos"));
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(glGetAttribLocation(program.get_id(), "aColor"));

    program.use_program();

    return VAO;
}

heat_solver::heat_solver(const shader& program)
:program(program)
{
    auto &parser = heat_parameters::get_instance();
    // render in domain [-1, 1]^3
    X = 2 / parser.get_dx();
    Y = 2 / parser.get_dy();
    Z = 2 / parser.get_dz();

    posVAO = bind_vertex_location();
}

void heat_solver::evolve()
{

}

void heat_solver::update_color_map()
{

}

void heat_solver::render()
{
    glBindVertexArray(posVAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

double* heat_solver::get_current_state_3channes()
{
    return nullptr;
}

heat_solver::~heat_solver()
{
}