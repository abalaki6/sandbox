#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <random>
#include <sys/time.h>
 
#define SIZEX 600
#define SIZEY 400

static int mouse_x, mouse_y;
static bool pressed = false;
static bool run_evolve = false;

inline double intRand(const double & min, const double & max) {
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
    double alpha = 0.05;
    double *u = *data;
    double *next_state = *buffer;
    struct timeval start, end;
    gettimeofday(&start, NULL);
    memcpy(next_state, u, SIZEX * SIZEY * sizeof(double));
    for(int k = 0; k < 150; k++)
    {
    #pragma omp parallel for private(i, j), shared(u, next_state, dt, dr, alpha)
        for(i=1; i < SIZEY-1; i++)
        {
            for(j=1; j < SIZEX-1; j++)
            {
                next_state[i*SIZEX + j]+= alpha * (u[(i-1)*SIZEX + j] + u[(i+1)*SIZEX + j] + u[i*SIZEX + j + 1] + u[i*SIZEX + j - 1] - 4*u[i*SIZEX + j]); 
            }
        }
        u = next_state;
    }
    *buffer = *data;
    *data = next_state;
    gettimeofday(&end, NULL);
    double delta = ((end.tv_sec  - start.tv_sec) * 1000000u + 
         end.tv_usec - start.tv_usec) / 1.e3;
    std::cout << "time (ms): " << delta << std::endl;
}


int main( int argc, char** argv ) 
{

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
            cv::circle(image, cv::Point2d(x,y), 15, cv::Scalar(2.0), -1);
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