#include <boost/config.hpp>
#include <boost/program_options.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <random>
#include <sys/time.h>
 

namespace po = boost::program_options;

static int SIZEX = 800;
static int SIZEY = 600;
static int ITER = 20;
static int RADIUS = 15;
static double TEMPERATURE = 2.0;
static double alpha = 0.05; 
static int mouse_x, mouse_y;
static bool pressed = false;
static bool run_evolve = false;

inline double double_rand(const double & min, const double & max) {
    static thread_local std::mt19937 generator;
    std::uniform_real_distribution<double> distribution(min,max);
    return distribution(generator);
}

void mouse_callback(int event, int x, int y, int flags, void *data)
{
    switch(event)
    {
        case CV_EVENT_MOUSEMOVE:
            mouse_x = x;
            mouse_y = y;

            // printf("(%d,%d)\n", x, y);
            break;
        case CV_EVENT_LBUTTONDOWN:
            pressed = true;
            break;
        case CV_EVENT_LBUTTONUP:
            pressed = false;
            break;
    }
}

void evolve(double **data, double **buffer)
{
    int i,j;
    double dr = 1. / SIZEX;
    double dt = dr * dr;
    double *u = *data;
    double *next_state = *buffer;
    struct timeval start, end;
    gettimeofday(&start, NULL);
    memcpy(next_state, u, SIZEX * SIZEY * sizeof(double));
    for(int k = 0; k < ITER; k++)
    {
    #pragma omp parallel for private(i, j), shared(u, next_state, dt, dr, alpha)
        for(i=0; i < SIZEY; i++)
        {
            // leaks in corners in 15th bit of accuracy
            if(i == 0)
            {
                next_state[0]+= alpha * (u[1] + u[SIZEX] - 2 * u[0]); // j == 0
                next_state[SIZEX - 1]+= alpha * (u[2 * SIZEX -1] + u[SIZEX - 2] - 2 * u[SIZEX - 1]); // j == SIZEX - 1
                for(j = 1; j < SIZEX-1; j++)
                    next_state[j]+= alpha * (u[SIZEX + j] + u[j + 1] + u[j - 1] - 3*u[j]);
            }
            else if(i == SIZEY - 1)
            {
                next_state[i*SIZEX]+= alpha * (u[(i-1)*SIZEX] + u[i*SIZEX + 1] - 2 * u[i*SIZEX]); // j == 0
                next_state[i*SIZEX + SIZEX - 1]+= alpha * (u[(i-1)*SIZEX + SIZEX - 1] + u[i*SIZEX + SIZEX - 2] - 2 * u[i*SIZEX + SIZEX - 1]); // j == SIZEX - 1
                for(j = 1; j < SIZEX-1; j++)
                    next_state[i*SIZEX + j]+= alpha * (u[(i-1)*SIZEX + j] + u[i*SIZEX + j + 1] + u[i*SIZEX + j - 1] - 3*u[i*SIZEX + j]);
            }
            else
            {
                next_state[i * SIZEX]+= alpha * (u[(i-1)*SIZEX] + u[(i+1)*SIZEX] + u[i*SIZEX + 1] - 3*u[i*SIZEX]); // j == 0
                next_state[i * SIZEX + SIZEX - 1]+= alpha * (u[(i-1)*SIZEX + SIZEX - 1] + u[(i+1)*SIZEX + SIZEX - 1] + u[i*SIZEX + SIZEX -2] - 3*u[i*SIZEX + SIZEX -1]); // j == SIZEX - 1
                // 20 bits of accuracy
                for(j=1; j < SIZEX-1; j++)
                    next_state[i*SIZEX + j]+= alpha * (u[(i-1)*SIZEX + j] + u[(i+1)*SIZEX + j] + u[i*SIZEX + j + 1] + u[i*SIZEX + j - 1] - 4*u[i*SIZEX + j]); 
            }

        }
        u = next_state;
    }
    *buffer = *data;
    *data = next_state;

    double sum = 0.0;
    #pragma omp parallel for private(i,j) reduction(+:sum), shared(next_state)
    for(int i=1; i < SIZEY-1; i++) for(int j=1; j < SIZEX-1; j++)
        sum += next_state[i*SIZEX + j];
    gettimeofday(&end, NULL);
    double delta = ((end.tv_sec  - start.tv_sec) * 1000000u + 
         end.tv_usec - start.tv_usec) / 1.e3;
    std::cout << "time (ms): " << delta << "\t\ttotal: " << sum << std::endl;
}


int main( int argc, char** argv ) 
{
    po::options_description desc("Allowed arguments");
    desc.add_options()
        ("help", "to get this message")
        ("height", po::value<int>(), "set height of the window, default 600 px")
        ("width", po::value<int>(), "set width of the window, default 800 px")
        ("alpha", po::value<double>(), "alpha parameter of heat equation, default 0.05")
        ("iter", po::value<int>(), "number of iterations per render, default 20")
        ("size", po::value<int>(), "radius of the brush when drawing, default 15")
        ("temp", po::value<double>(), "temperature of the brush, default 2.0");

    po::variables_map vmap;
    po::store(po::parse_command_line(argc, argv, desc), vmap);
    po::notify(vmap);

    if(vmap.count("help"))
    {
        std::cout << desc;
        return 0;
    }
    if(vmap.count("height"))
        SIZEY = vmap["height"].as<int>();
    if(vmap.count("width"))
        SIZEX = vmap["width"].as<int>();
    if(vmap.count("alpha"))
        alpha = vmap["alpha"].as<double>();
    if(vmap.count("iter"))
        ITER = vmap["iter"].as<int>();
    if(vmap.count("size"))
        RADIUS = vmap["size"].as<int>();
    if(vmap.count("temp"))
        TEMPERATURE = vmap["temp"].as<double>();

    double *data = new double[SIZEX*SIZEY];
    double *buffer = new double[SIZEX*SIZEY];
    cv::Mat image(SIZEY, SIZEX, CV_64F, data);
    cv::Mat scaled, color;

    std::string name = "Display Window";
    cv::namedWindow(name, cv::WINDOW_AUTOSIZE);
    cv::setMouseCallback(name, mouse_callback, nullptr);
    int val,x,y;
    while( (val = cv::waitKey(1000/60)) != 27)
    {
        if(val == 32) //space
        {
            run_evolve = !run_evolve;
        }
        if((x != mouse_x || y != mouse_y) && pressed)
        {
            x = mouse_x;
            y = mouse_y;
            cv::circle(image, cv::Point2d(x,y), RADIUS, cv::Scalar(TEMPERATURE), -1);
        }
        image.convertTo(scaled, CV_8UC1,255, 0);
        cv::applyColorMap(scaled, color, cv::COLORMAP_JET);
        if(run_evolve)
        {
            evolve(&data, &buffer);
        }
        image = cv::Mat(SIZEY, SIZEX, CV_64F, data);
        cv::imshow(name, color);
    }

    delete[] data;
    delete[] buffer;
    return 0;
}