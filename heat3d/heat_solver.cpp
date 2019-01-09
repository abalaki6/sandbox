#include "heat_solver.hpp"



GLuint heat_solver::bind_vertex_location()
{
    #ifdef SIX
    #define TEMP_SIX SIX
    #undef SIX
    #endif

    #ifdef THREE
    #define TEMP_THREE THREE
    #undef THREE
    #endif

    #ifdef NUM_PAR
    #define TEMP_NUM_PAR NUM_PAR
    #undef NUM_PAR
    #endif

    #define SIX 6
    #define THREE 3
    // replcate to 3 after done debugging grid rendering
    #define NUM_PAR SIX

    float *pos = new float[NUM_PAR*X*Y*Z]; // fo debug 6 , replace to 3 in release

    // predefine locations
    size_t i;

    auto dx = heat_parameters::get_instance().get_dx();
    auto dy = heat_parameters::get_instance().get_dy();
    auto dz = heat_parameters::get_instance().get_dz();

    #pragma omp parallel for private(i,j,k), shared(X,Y,Z,pos,dx,dy,dz)
    for(i = 0; i < X; i++)
    {
        // replace 6 to 3 after debugging
        auto *loc = pos + NUM_PAR * i * Y * Z; 

        auto x = i * dx;
        for(size_t j = 0; j < Y; j++)
        {
            auto y = j * Y;
            for(size_t k = 0; k < Z; k++)
            {
                auto z = k * dz;
                // define grid
                loc[0] = x;
                loc[1] = y;
                loc[2] = z;
                // for debug define color map as well
                #if NUM_PAR == SIX
                    loc[3] = 1.0f;
                    loc[4] = 0.0f;
                    loc[5] = 0.0f;
                #endif
                // iterate to next element after debug replace 6 to 3
                loc += NUM_PAR; 
            }
        }
    }
    
    GLuint VBO, VAO;
    
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, NUM_PAR * X * Y * Z, pos, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, NUM_PAR * sizeof(float), (void*)0);
    glEnableVertexAttribArray(glGetAttribLocation(program.get_id(), "aPos"));
    
    #if NUM_PAR == SIX
        // color attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, NUM_PAR * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(glGetAttribLocation(program.get_id(), "aColor"));
    #endif

    program.use_program();


    // after mapping vertces delete CPU buffer
    delete pos;
    // undef temporal constants
    #undef SIX
    #undef THREE
    #undef NUM_PAR
    // define back old marocs
    #ifdef TEMP_SIX
    #define SIX TEMP_SIX
    #undef TEMP_SIX
    #endif

    #ifdef TEMP_THREE
    #define THREE TEMP_THREE
    #undef TEMP_THREE
    #endif

    #ifdef TEMP_NUM_PAR
    #define NUM_PAR TEMP_NUM_PAR
    #undef TEMP_NUM_PAR
    #endif

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
    auto num_iter = heat_parameters::get_instance().get_num_iter();
    auto dt = heat_parameters::get_instance().get_dt();
    auto dx = heat_parameters::get_instance().get_dx();
    auto dy = heat_parameters::get_instance().get_dy();
    auto dz = heat_parameters::get_instance().get_dz();
    auto alpha = heat_parameters::get_instance().get_alpha();

    size_t i,j, k;
    for(size_t it = 0; it < num_iter; it++)
    {

        #pragma omp parallel for private(i,j,k) shared(state, buffer, alpha, dx, dy dz, dt)
        for(i = 0; i < X; i++)
        {
            // add special cases for boudaries
            for(j = 0; i < Y; j++)
            {
                // add special cases for boudaries
                for(k = 0; k < Z; k++)
                {
                    // todo implementation
                }
            }
        }

        // swap state with buffer
        double *t = state;
        state = buffer;
        buffer = t;
    }
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