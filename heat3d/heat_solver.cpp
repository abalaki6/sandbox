#include "heat_solver.hpp"



void heat_solver::bind_vertex_location()
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


    const size_t NUM_POINTS = NUM_PAR * X * Y * Z;
    float *pos = new float[NUM_POINTS]; // fo debug 6 , replace to 3 in release
    // predefine locations
    size_t i;

    auto dx = heat_parameters::get_instance().get_dx();
    auto dy = heat_parameters::get_instance().get_dy();
    auto dz = heat_parameters::get_instance().get_dz();

    double *c = state;
    #pragma omp parallel for private(i), shared(X,Y,Z,pos,dx,dy,dz)
    for(i = 0; i < X; i++)
    {
        // replace 6 to 3 after debugging
        auto *loc = pos + NUM_PAR * i * Y * Z; 

        float x = i * dx;
        for(size_t j = 0; j < Y; j++)
        {
            float y = j * dy;
            for(size_t k = 0; k < Z; k++)
            {
                float z = k * dz;
                // define grid
                loc[0] = x;
                loc[1] = y;
                loc[2] = z;
                // for debug define color map as well
                #if NUM_PAR == SIX
                    c[0] = 1.0f;
                    c++;
                #endif
                // iterate to next element after debug replace 6 to 3
                loc += NUM_PAR; 
            }
        }
    }
    

    GLuint VBO[2], VAO;
    
    glGenVertexArrays(1, &VAO);
    glGenBuffers(2, VBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, NUM_POINTS * sizeof(float), pos, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, NUM_PAR * sizeof(float), (void*)0);
    glEnableVertexAttribArray(glGetAttribLocation(program.get_id(), "aPos"));
    
    #if NUM_PAR == SIX
        // color attribute
        glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
        glBufferData(GL_ARRAY_BUFFER, NUM_POINTS  / 2 * sizeof(float), NULL, GL_DYNAMIC_DRAW);
        glVertexAttribPointer(1, 3, GL_UNSIGNED_BYTE, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(glGetAttribLocation(program.get_id(), "aColor"));
    #else
        glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
        glBufferData(GL_ARRAY_BUFFER, NUM_POINTS * sizeof(float), NULL, GL_DYNAMIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, NUM_PAR * sizeof(float), (void*)0);
        glEnableVertexAttribArray(glGetAttribLocation(program.get_id(), "aColor"));

    #endif


    program.use_program();


    // after mapping vertces delete CPU buffer
    // delete[] pos;
    temp = pos;
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


    this->VAO = VAO;
}

heat_solver::heat_solver(const shader& program)
:program(program)
{
    auto &parser = heat_parameters::get_instance();
    // render in domain [-1, 1]^3
    X = 1 / parser.get_dx() + 1;
    Y = 1 / parser.get_dy() + 1;
    Z = 1 / parser.get_dz() + 1;

    state = new double[X*Y*Z];
    buffer = new double[X*Y*Z];
    heat_map = new float[X*Y*Z*3];

    bind_vertex_location();
}

void heat_solver::evolve()
{
    auto num_iter = heat_parameters::get_instance().get_num_iter();
    auto dt = heat_parameters::get_instance().get_dt();
    auto dx = heat_parameters::get_instance().get_dx();
    auto dy = heat_parameters::get_instance().get_dy();
    auto dz = heat_parameters::get_instance().get_dz();
    auto alpha = heat_parameters::get_instance().get_alpha();

    memcpy(buffer, state, X*Y*Z * sizeof(float));

    size_t i,j, k, iter=0;
    while(run_thread)
    {
        #pragma omp parallel for private(i,j,k) shared(state, buffer, alpha, dx, dy dz, dt)
        for(i = 0; i < X; i++)
        {
            size_t x_0 = i * Y * Z;
            // add special cases for boudaries
            for(j = 0; j < Y; j++)
            {
                size_t y_0 = j * Z; 
                // add special cases for boudaries
                for(k = 0; k < Z; k++)
                {
                    // todo implementation
                    size_t z_0 = k;

                    // for demo make faint
                    buffer[x_0 + y_0 + z_0] = state[x_0 + y_0 + z_0] * 0.9999;
                }
            }
        }

        // swap state with buffer
        double *t = state;
        state = buffer;
        buffer = t;
        
        iter++;
        if(iter == num_iter)
        {
            printf("updating map\n");
            iter = 0;
            update_color_map();
        }
    }
}

void heat_solver::update_color_map()
{

    cv::Mat dest, converted, form;
    // #pragma omp parallel for private(i), shared(n, state, heat_state)
    for(size_t i = 0; i < Z; i++)
    {
        // convert slice to 8UC
        cv::Mat source(X,Y, CV_64F, state + i*X*Y);
        source.convertTo(converted, CV_8UC1, 255.);
        // apply heat map to slice
        cv::applyColorMap(converted, dest, cv::COLORMAP_JET);
        cv::cvtColor(dest, dest, cv::COLOR_BGR2RGB);
        dest.convertTo(form, CV_32F, 1/255.);
        // store slice in map
        memcpy(heat_map + i*3*X*Y*sizeof(float), (void*)dest.data, 3*X*Y*sizeof(float)); 
        // cur_map_ptr += n;
        // cur_state_ptr += n;
    }
    printf("%u, %f, %u %u %u\n", dest.data[0], state[0], heat_map[0], heat_map[1], heat_map[2]);
}


void heat_solver::run()
{
    run_thread = true;
    update_color_map();
    worker = new std::thread(&heat_solver::evolve, this);
}

void heat_solver::stop()
{
    run_thread = false;
    worker->join();

    delete worker;
}


void heat_solver::render()
{
    glBindVertexArray(VAO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, 3 * X * Y * Z * sizeof(uchar), heat_map);
    glDrawArrays(GL_POINTS, 0, X * Y * Z);
}

double* heat_solver::get_current_state_3channes()
{
    return nullptr;
}

heat_solver::~heat_solver()
{
    delete[] buffer;
    delete[] heat_map;
    delete[] state;
}