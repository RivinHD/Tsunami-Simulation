/**
 * @author Alexander Breuer (alex.breuer AT uni-jena.de), Fabian Hofer, Vincent Gerlach
 *
 * Entry-point for simulations.
 **/

 // #define SKIP_ARGUMENTS
 // #define TSUNAMI_SIMULATION_DISABLE_IO

#include "../include/patches/WavePropagation1d.h"
#include "../include/patches/WavePropagation2d.h"
#include "../include/amr/AMRCoreWavePropagation2d.h"
#include "../include/setups/DamBreak1d.h"
#include "../include/setups/RareRare1d.h"
#include "../include/setups/ShockShock1d.h"
#include "../include/setups/SubcriticalFlow1d.h"
#include "../include/setups/SupercriticalFlow1d.h"
#include "../include/setups/TsunamiEvent1d.h"
#include "../include/setups/CircularDamBreak2d.h"
#include "../include/setups/ArtificialTsunami2d.h"
#include "../include/setups/TsunamiEvent2d.h"
#include "../include/io/ArgSetup.h"
#include <cstdlib>
#include <iostream>
#include <cmath>
#include <fstream>
#include <limits>
#include <string>
#include <chrono>
#ifndef TSUNAMI_SIMULATION_DISABLE_IO
#include "../include/setups/CheckPoint.h"
#include "../include/io/Csv.h"
#include "../include/io/NetCdf.h"
#include "../include/io/Stations.h"
#include <filesystem> // requieres C++17 and up
namespace fs = std::filesystem;
#endif // !TSUNAMI_SIMULATION_DISABLE_IO

#define SKIP_ARGUMENTS

const std::string SOLUTION_FOLDER = "solutions";

enum Arguments
{
    SOLVER = 's',
    USE_BATHYMETRY = 'B',
    REFLECTION = 'r',
    TIME = 't',
    IO_FORMAT = 'f',
    USE_AXIS_SPHERICAL = 'S',
    WRITE_INTERVALL = 'w',
    AVERAGE_SEVERAL = 'k',
    CHECKPOINT_INTERVALL = 'c'
};

const int requiredArguments = 1;
const int optionalArguments = 1;
const std::vector<ArgSetup> optionalFlags = {
        ArgSetup( Arguments::SOLVER, 1, 1 ),
        ArgSetup( Arguments::USE_BATHYMETRY, 0, 0 ),
        ArgSetup( Arguments::REFLECTION, 1, 4 ),
        ArgSetup( Arguments::TIME, 1, 1 ),
        ArgSetup( Arguments::IO_FORMAT, 1, 1 ),
        ArgSetup( Arguments::USE_AXIS_SPHERICAL, 0, 0 ),
        ArgSetup( Arguments::WRITE_INTERVALL, 1, 1 ),
        ArgSetup( Arguments::AVERAGE_SEVERAL, 1, 1 ),
        ArgSetup( Arguments::CHECKPOINT_INTERVALL, 1, 1 )
};

void printHelp()
{
    const char* reset = "\033[0m";
    const char* cyan = "\033[36;49m";
    const char* magenta = "\033[35;49m";
    const char* green = "\033[32;49m";

    std::cerr << "./build/simulation " << magenta << "N_CELLS_X (N_CELLS_Y) " << reset << "["
        << green << "-B" << reset << "] ["
        << green << "-S" << reset << "] ["
        << green << "-c" << cyan << " <minutes>" << reset << "] ["
        << green << "-f" << cyan << " <csv|netCDF>" << reset << "] ["
        << green << "-r " << cyan << "<left|right|top|bottom|x|y|all>" << reset << "] ["
        << green << "-k" << cyan << " <NUMBER>" << reset << "] ["
        << green << "-s " << cyan << "<fwave|roe>" << reset << "] ["
        << green << "-t" << cyan << " <seconds>" << reset << "] ["
        << green << "-w" << cyan << " <seconds>" << reset << "]"
        << std::endl << std::endl
        << "REQUIRED INPUT:" << std::endl
        << magenta << "\tN_CELLS_X" << reset << " is the number of cells in x-direction." << std::endl
        << std::endl
        << "OPTIONAL INPUT:" << std::endl
        << magenta << "\tN_CELLS_Y" << reset << " is the number of cells in y-direction." << std::endl
        << std::endl
        << "NOTE: optional flags must be set after the inputs.." << std::endl
        << "OPTIONAL FLAGS:" << std::endl
        << green << "\t-B" << reset << " enables the use of bathymetry." << std::endl
        << green << "\t-S" << reset << " use degrees_east (longitude) and degrees_north (latitude) as unit for the x-axis and y-axis solution output instead of meters." << std::endl
        << green << "\t-c" << reset << " set the intervals in Realtime seconds when to create a checkpoint. The default is 20 minutes." << std::endl
        << green << "\t-f" << reset << " defines the output format. Requires " << cyan << "csv" << reset << " or " << cyan << "netCDF" << reset << ". The default is netCDF." << std::endl
        << green << "\t-k" << reset << " defines the number of cells to average several neighboring cells of the computational grid into one cell" << std::endl
        << green << "\t-r" << reset << " enables the reflection on the specified side of the simulation. Several arguments can be passed (maximum 4)." << std::endl
        << "\t   where " << cyan << "left | right | top | bottom" << reset << " enables their respective sides." << std::endl
        << "\t   where " << cyan << "x" << reset << " enables the left & right and " << cyan << "y" << reset << " enables the top & bottom side." << std::endl
        << "\t   where " << cyan << "all" << reset << " enables all sides." << std::endl
        << green << "\t-s" << reset << " set used solvers requires " << cyan << "fwave" << reset << " or " << cyan << "roe" << reset << " as inputs. The default is fwave." << std::endl
        << green << "\t-t" << reset << " defines the total time in seconds that is used for the simulation. The default is 5 seconds." << std::endl
        << green << "\t-w" << reset << " defines the time how often the simulation is written to the disk. The default is 0.25 seconds." << std::endl;
}

int main( int   i_argc,
          char* i_argv[] )
{
    const char* reset = "\033[0m";
    const char* green = "\033[32;49m";

    std::cout << "#####################################################" << std::endl;
    std::cout << "###                  Tsunami Lab                  ###" << std::endl;
    std::cout << "###                                               ###" << std::endl;
    std::cout << "### https://scalable.uni-jena.de                  ###" << std::endl;
    std::cout << "### https://rivinhd.github.io/Tsunami-Simulation/ ###" << std::endl;
    std::cout << "#####################################################" << std::endl;

    // default arguments values"
    tsunami_lab::t_idx l_nx = 0;
    tsunami_lab::t_idx l_ny = 1;
    tsunami_lab::t_real l_dxy = 1;
    tsunami_lab::patches::Solver solver = tsunami_lab::patches::Solver::FWAVE;
    bool useBathymetry = false;
    bool reflectLeft = false;
    bool reflectRight = false;
    bool reflectTop = false;
    bool reflectBottom = false;
    bool use2D = false;
    bool isCsv = false;
    bool useAxisSpherical = false;
    tsunami_lab::t_idx l_averageCellNumber = 1;
    tsunami_lab::t_real l_endTime = 5; // in seconds
    tsunami_lab::t_real l_writeTime = tsunami_lab::t_real( 0.25 );
    tsunami_lab::t_real checkpointIntervall = 20; // in minutes

    // setup the variables
    tsunami_lab::setups::Setup* l_setup = nullptr;
    tsunami_lab::t_real l_scaleX = 10000;
    tsunami_lab::t_real l_scaleY = 10000;
    bool useCheckpoint = false;
    tsunami_lab::t_real l_simTime = 0;
    tsunami_lab::t_real checkpointHMax = 0;

    l_nx = 2000;
    l_ny = 2000;
    reflectLeft = false;
    reflectRight = false;
    reflectBottom = false;
    reflectTop = false;
    useBathymetry = true;
    use2D = true;
    useAxisSpherical = false;
    l_endTime = 13000;
    std::cout << i_argv[i_argc - 1] << std::endl;

    // construct setup
    if( l_setup == nullptr )
    {
        /*const char* variables[3]{ "x", "y", "z" };
        l_setup = new tsunami_lab::setups::TsunamiEvent2d( "resources/artificialtsunami_bathymetry_1000.nc",
                                                           variables,
                                                           "resources/artificialtsunami_displ_1000.nc",
                                                           variables,
                                                           l_scaleX,
                                                           l_scaleY );*/
        l_setup = new tsunami_lab::setups::CircularDamBreak2d();
    }

    int argc = 2;
    std::string arg1 = fs::absolute( "resources/inputs.amrex" ).string();
    char** argv = new char* [argc];
    argv[0] = i_argv[0];
    argv[1] = const_cast<char*>( arg1.c_str() );
    amrex::Initialize( argc, argv );

    // construct solver
    tsunami_lab::amr::AMRCoreWavePropagation2d* l_waveProp = new tsunami_lab::amr::AMRCoreWavePropagation2d( l_setup );


    // set the solver to use
    //l_waveProp->setSolver( solver );

    // set if bathymetry exists
    // l_waveProp->enableBathymetry( useBathymetry );

    // set Reflection
    l_waveProp->setReflection( tsunami_lab::patches::WavePropagation::Side::LEFT, reflectLeft );
    l_waveProp->setReflection( tsunami_lab::patches::WavePropagation::Side::RIGHT, reflectRight );
    l_waveProp->setReflection( tsunami_lab::patches::WavePropagation::Side::TOP, reflectTop );
    l_waveProp->setReflection( tsunami_lab::patches::WavePropagation::Side::BOTTOM, reflectBottom );

    // maximum observed height in the setup
    tsunami_lab::t_real l_hMax = std::numeric_limits< tsunami_lab::t_real >::lowest();

    tsunami_lab::t_real cellSize = l_dxy;
    // checkpoint setup uses index instead of coordinate
    if( useCheckpoint )
    {
        cellSize = 1;
    }

    // free no longer needed setup
    delete l_setup;

    // derive constant time step; changes at simulation time are ignored

    const auto startTime = std::chrono::high_resolution_clock::now();

    std::cout << "entering time loop" << std::endl;
    tsunami_lab::t_idx l_cellUpdates = 0;

    // iterate over time
    l_waveProp->Evolve();

    std::cout << "finished time loop" << std::endl;

    // free memory
    std::cout << "freeing memory" << std::endl;
    delete l_waveProp;

    amrex::Finalize();

    delete[] argv;

    // Print the calculation time
    const auto duration = std::chrono::high_resolution_clock::now() - startTime;
    const auto hours = std::chrono::duration_cast<std::chrono::hours>( duration );
    const auto minutes = std::chrono::duration_cast<std::chrono::minutes>( duration - hours );
    const auto seconds = std::chrono::duration_cast<std::chrono::seconds>( duration - hours - minutes );
    std::cout << "The Simulation took " << green << hours.count() << " h " << minutes.count() << " min " << seconds.count() << " sec" << reset << " to finish." << std::endl;
    std::cout << "Time per iteration: " << green << std::chrono::duration_cast<std::chrono::milliseconds>( duration ).count() / l_cellUpdates << reset << " milliseconds." << std::endl;
    std::cout << "Time per cell:      " << green << std::chrono::duration_cast<std::chrono::nanoseconds>( duration ).count() / l_cellUpdates / l_nx / l_ny << reset << " nanoseconds." << std::endl;

    std::cout << "finished, exiting" << std::endl;
    return EXIT_SUCCESS;
}
