#include "heat_solver.hpp"



void heat_solver::bind_vertex_location()
{
    // replcate to 3 after done debugging grid rendering
    const size_t NUM_PAR = 3;
    const size_t NUM_POINTS = NUM_PAR * X * Y * Z;
    float *pos = new float[NUM_POINTS];
    size_t i;

    auto dx = heat_parameters::get_instance().get_dx();
    auto dy = heat_parameters::get_instance().get_dy();
    auto dz = heat_parameters::get_instance().get_dz();

    #pragma omp parallel for private(i), shared(X,Y,Z,pos,dx,dy,dz)
    for(i = 0; i < X; i++)
    {
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
                // iterate to next element
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
    program.enable_attr_arr("aPos");
    // color attribute
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, NUM_POINTS * sizeof(float), NULL, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    program.enable_attr_arr("aColor");

    program.use_program();


    // after mapping vertces delete CPU buffer
    delete[] pos;

    this->VAO = VAO;
}

void heat_solver::load_initial_state()
{
    std::string filename = heat_parameters::source_name();

    int fd = open(filename.c_str(), O_RDONLY);
    
    struct stat sb;
    memset(&sb, 0, sizeof(struct stat));
    fstat(fd, &sb);
    
    if(fd == -1)
    {
        std::cerr << "ERROR::HEAT_SOLVER::LOAD_INITIAL_STATE: Failed to open file with initial state." << std::endl;
        exit(3);
    }

    void* buffer = mmap(NULL, sb.st_size, PROT_READ, MAP_SHARED, fd, 0);
    memcpy(state, buffer, sb.st_size);

    munmap(buffer, sb.st_size);
    close(fd);
}

heat_solver::heat_solver(const shader& program)
:program(program)
{
    auto &parser = heat_parameters::get_instance();
    // render in domain [0, 1]^3
    X = 1 / parser.get_dx() + 1;
    Y = 1 / parser.get_dy() + 1;
    Z = 1 / parser.get_dz() + 1;

    bind_vertex_location();
    
    state = new double[X*Y*Z];
    buffer = new double[X*Y*Z];
    heat_map = new float[X*Y*Z*3];

    load_initial_state();
}

void heat_solver::evolve()
{
    auto num_iter = heat_parameters::num_iter();
    auto dt = heat_parameters::dt();
    auto dx = heat_parameters::dx();
    auto dy = heat_parameters::dy();
    auto dz = heat_parameters::dz();
    auto alpha = heat_parameters::alpha();

    memcpy(buffer, state, X*Y*Z * sizeof(float));

    size_t i,j, k, iter=0;

    size_t ux, lx, uy, ly, uz, lz;
    while(run_thread)
    {
        auto u = state; // to simplify notation
        #pragma omp parallel for private(i,j,k) shared(state, buffer, alpha, dx, dy dz, dt)
        for(i = 0; i < X; i++)
        {
            size_t x_0 = i * Y * Z;

            ux = (i != X - 1) * Y*Z;
            lx = -(i != 0) * Y*Z;

            for(j = 0; j < Y; j++)
            {
                size_t y_0 = j * Z; 

                uy = (j != Y - 1) * Z;
                ly = -(j != 0) * Z;

                for(k = 0; k < Z; k++)
                {
                    size_t z_0 = k;
                    size_t l = x_0 + y_0 + z_0;

                    uz = (k != Z - 1);
                    lz = -(k != 0);
                
                    uz = (k == Z - 1 ? 0 : 1);
                    lz = (k == 0 ? 0 : -1);
                    
                    buffer[l] = u[l] + alpha * dt * (
                        /* z axis */ (u[l+uz] + u[l+lz] - 2 * u[l]) / (dz*dz) + 
                        /* y axis */ (u[l+uy] + u[l+ly] - 2 * u[l]) / (dy*dy) + 
                        /* x axis */ (u[l+ux] + u[l+lx] - 2 * u[l]) / (dx*dx)
                    );
                }
            }
        }

        // swap state with buffer
        double *t = state;
        state = buffer;
        buffer = t;
        
        if(DEBUG)
        {
            float sum = 0.0f;
            #pragma omp parallel for redice(+:sum) private(i,j,k), shared(state)
            for(i = 0; i < X; i++) for(j = 0; j < Y; j++) for(k = 0; k < Z; k++)
                sum += state[i * Y * Z + j * Z + k];
            printf("total energy : %f\n", sum);
        }


        iter++;
        if(iter == num_iter)
        {
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
        memcpy(heat_map + i*3*X*Y, (void*)form.data, 3*X*Y*sizeof(float)); 
    }
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
    glBufferSubData(GL_ARRAY_BUFFER, 0, 3 * X * Y * Z * sizeof(float), heat_map);
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