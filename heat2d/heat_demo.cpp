#include <boost/config.hpp>
#include <boost/program_options.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <random>
#include <sys/time.h>
#include <exception>
 
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
static bool DEBUG = false;

inline double double_rand(const double & min, const double & max) 
{
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
    // ~ 62 bits of accuracy (as in of conservation only)
    int i,j;
    double dr = 1. / SIZEX;
    double dt = dr * dr;
    double *u = *data;
    double *next_state = *buffer;
    
    struct timeval start, end;
    gettimeofday(&start, NULL);

    for(int k = 0; k < ITER; k++)
    {
    #pragma omp parallel for private(i, j), shared(u, next_state, dt, dr, alpha)
        for(i=0; i < SIZEY; i++)
        {
            if(i == 0)
            {
                next_state[0]= u[0] + alpha * (
                    u[1] + 
                    u[SIZEX] - 
                    2*u[0]); // j == 0

                next_state[SIZEX - 1]= u[SIZEX - 1] + alpha * (
                    u[2 * SIZEX -1] + 
                    u[SIZEX - 2] - 
                    2*u[SIZEX - 1]); // j == SIZEX - 1

                for(j = 1; j < SIZEX-1; j++)
                    next_state[j]= u[j] + alpha * (
                        u[SIZEX + j] +
                        u[j + 1] +
                        u[j - 1] -
                        3*u[j]);
            }
            else if(i == SIZEY - 1)
            {
                next_state[i*SIZEX]= u[i*SIZEX] + alpha * (
                    u[(i-1)*SIZEX] + 
                    u[i*SIZEX + 1] - 
                    2*u[i*SIZEX]); // j == 0

                next_state[i*SIZEX + SIZEX - 1]= u[i*SIZEX + SIZEX - 1] + alpha * (
                    u[(i-1)*SIZEX + SIZEX - 1] + 
                    u[i*SIZEX + SIZEX - 2] - 
                    2*u[i*SIZEX + SIZEX - 1]); // j == SIZEX - 1

                for(j = 1; j < SIZEX-1; j++)
                    next_state[i*SIZEX + j]= u[i*SIZEX + j] + alpha * (
                        u[(i-1)*SIZEX + j] + 
                        u[i*SIZEX + j + 1] + 
                        u[i*SIZEX + j - 1] - 
                        3*u[i*SIZEX + j]);
            }
            else
            {
                next_state[i * SIZEX]= u[i * SIZEX] + alpha * (
                    u[(i-1)*SIZEX] + 
                    u[(i+1)*SIZEX] + 
                    u[i*SIZEX + 1] - 
                    3*u[i*SIZEX]); // j == 0

                next_state[i * SIZEX + SIZEX - 1]= u[i * SIZEX + SIZEX - 1] + alpha * (
                    u[(i-1)*SIZEX + SIZEX - 1] + 
                    u[(i+1)*SIZEX + SIZEX - 1] + 
                    u[i*SIZEX + SIZEX -2] - 
                    3*u[i*SIZEX + SIZEX -1]); // j == SIZEX - 1
                
                for(j=1; j < SIZEX-1; j++)
                    next_state[i*SIZEX + j]= u[i * SIZEX + j] + alpha * (
                        u[(i-1)*SIZEX + j] +
                        u[(i+1)*SIZEX + j] +
                        u[i*SIZEX + j + 1] + 
                        u[i*SIZEX + j - 1] - 
                        4*u[i*SIZEX + j]); 
            }

        }
        if(k != ITER)
        {
            auto tmp = next_state;
            next_state = u;
            u = tmp;
        }
    }
    *buffer = *data;
    *data = next_state;

    if(DEBUG)
    {
        double sum = 0.0;
        #pragma omp parallel for private(i) reduction(+:sum), shared(next_state)
        for(i=0; i < SIZEY*SIZEX; i++) sum += next_state[i];
        
        gettimeofday(&end, NULL);
        
        double delta = ((end.tv_sec  - start.tv_sec) * 1000000u + 
            end.tv_usec - start.tv_usec) / 1.e3;
        printf("time (ms): %.5f \t total: %.3f \t average: %.4f\n", delta, sum, sum / SIZEX / SIZEY);
        // std::cout << "time (ms): " << delta << "\t\ttotal: " << sum << std::endl;
    }
}

void parse_arguments(int argc, char** argv)
{
    try
    {
        po::options_description desc("Allowed arguments");
        desc.add_options()
            ("help,h", "print this message.")
            ("height,y", po::value<int>()->default_value(600), "set height of the window.")
            ("width,x", po::value<int>()->default_value(800), "set width of the window")
            ("alpha,a", po::value<double>()->default_value(0.05), "alpha parameter of heat equation.")
            ("iter,i", po::value<int>()->default_value(20), "number of iterations per render.")
            ("size,s", po::value<int>()->default_value(15), "radius of the brush when drawing.")
            ("temp,t", po::value<double>()->default_value(1.3), "temperature of the brush.")
            ("debug,d", "debug flag to print extra states' info");

        po::variables_map vmap;
        po::store(po::parse_command_line(argc, argv, desc), vmap);
        po::notify(vmap);

        if(vmap.count("help"))
        {
            std::cout << desc;
            exit(0);
        }
        SIZEY = vmap["height"].as<int>();
        SIZEX = vmap["width"].as<int>();
        alpha = vmap["alpha"].as<double>();
        ITER = vmap["iter"].as<int>();
        RADIUS = vmap["size"].as<int>();
        TEMPERATURE = vmap["temp"].as<double>();
        if(vmap.count("debug")) DEBUG = true;
    }
    catch(std::exception& e)
    {
        std::cerr << "error: " << e.what() << std::endl;
        exit(1);
    }
    catch(...)
    {
        std::cerr << "error: Unknown error" << std::endl;
        exit(2);
    }
}

int main( int argc, char** argv ) 
{
    parse_arguments(argc, argv);

    double *data = new double[SIZEX*SIZEY];
    double *buffer = new double[SIZEX*SIZEY];
    cv::Mat image(SIZEY, SIZEX, CV_64F, data);
    cv::Mat scaled, color;

    std::string name = "Heat equation visualization";
    cv::namedWindow(name, cv::WINDOW_AUTOSIZE);
    cv::setMouseCallback(name, mouse_callback, nullptr);
    int val,x,y;

    auto vcap = new cv::VideoWriter("heat2d_video.avi",
            CV_FOURCC('M','J','P','G'),
            120,
            cv::Size(SIZEX, SIZEY),
            true);

    while( (val = cv::waitKey(1000/60)) != 27) // escape
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
        vcap->write(color);
        cv::imshow(name, color);
    }

    vcap->release();

    delete[] data;
    delete[] buffer;
    return 0;
}