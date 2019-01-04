#include "heat_parameters.hpp"

namespace po = boost::program_options;

heat_parameters& heat_parameters::get_instance()
{
    static heat_parameters singleton;
    return singleton;
}

heat_parameters::heat_parameters()
: 
delta_t(1),
delta_x(1),
delta_y(1),
delta_z(1),
alpha(0.05),
num_iter(20),
debug(false)
{
    desc.add_options()
        ("help,h", "print this message.")
        ("dt", po::value<double>(), "timestep per iteration.")
        ("dx", po::value<double>(), "space step in Ox per iteration, for stability dt < dr^2.")
        ("dy", po::value<double>(), "space step in Oy per iteration, for stability dt < dr^2.")
        ("dz", po::value<double>(), "space step in Oz per iteration, for stability dt < dr^2.")
        ("alpha,a", po::value<double>(), "thermal diffusivity of the system.")
        ("d", "run with debug information.")
        ("iter,i", po::value<size_t>(), "number of iterations per frame");
}
