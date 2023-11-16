/**
 * @author Alexander Breuer (alex.breuer AT uni-jena.de)
 *
 * @section DESCRIPTION
 * Entry-point for simulations.
 **/
#include "../include/patches/WavePropagation1d.h"
#include "../include/patches/WavePropagation2d.h"
#include "../include/setups/DamBreak1d.h"
#include "../include/setups/RareRare1d.h"
#include "../include/setups/ShockShock1d.h"
#include "../include/setups/SubcriticalFlow1d.h"
#include "../include/setups/SupercriticalFlow1d.h"
#include "../include/setups/TsunamiEvent1d.h"
#include "../include/setups/CircularDamBreak2d.h"
#include "../include/io/Csv.h"
#include "../include/io/ArgSetup.h"
#include "../include/io/Stations.h"
#include <cstdlib>
#include <iostream>
#include <cmath>
#include <fstream>
#include <limits>
#include <string>
#include <filesystem> // requieres C++17 and up
#include <chrono>
#include <thread>

namespace fs = std::filesystem;

#define SKIP_ARGUMENTS

const std::string SOLUTION_FOLDER = "solutions";
bool KILL_THREAD = false;

const std::string reset = "\033[0m";
const std::string cyan = "\033[36;49m";
const std::string magenta = "\033[35;49m";
const std::string green = "\033[32;49m";

enum Arguments
{
    SOLVER = 's',
    USE_BATHYMETRY = 'B',
    USE_REFLECTION = 'r',
};
const int requiredArguments = 1;
const std::vector<ArgSetup> optionalArguments = {
    ArgSetup( Arguments::SOLVER, 1 ),
    ArgSetup( Arguments::USE_BATHYMETRY, 0 ),
    ArgSetup( Arguments::USE_REFLECTION, 1 ),
};

void printHelp()
{
    std::cerr << "./build/simulation " << magenta << "N_CELLS_X (N_CELLS_Y) " << reset << "["
        << green << "-s " << cyan << "<fwave|roe>" << reset << "] ["
        << green << "-B" << reset << "] ["
        << green << "-r " << cyan << "<left|right|top|bottom|x|y|all>" << reset << "]"
        << std::endl << std::endl
        << "REQUIERED INPUT:" << std::endl
        << magenta << "\tN_CELLS_X" << reset << " is the number of cells in x-direction." << std::endl
        << std::endl
        << "OPTIONAL INPUT:" << std::endl
        << magenta << "\tN_CELLS_Y" << reset << " is the number of cells in y-direction." << std::endl
        << std::endl
        << "NOTE: optional flags has be put after the required input" << std::endl
        << "OPTIONAL FLAGS:" << std::endl
        << green << "\t-s" << reset << " set used solvers requires " << cyan << "'fwave'" << reset << " or " << cyan << "'roe'" << reset << " as inputs" << std::endl
        << green << "\t-B" << reset << " enables the input for bathymetry" << std::endl
        << green << "\t-r" << reset << " enables the reflection on the given side of the simulation" << std::endl
        << "\t   where " << cyan << "left | right | top | bottom'" << reset << " enables their respective sides" << std::endl
        << "\t   where " << cyan << "x | y" << reset << " enables the left & right | top & bottom side" << std::endl
        << "\t   where " << cyan << "all" << reset << " enables all sides" << std::endl;
}

void writeStations( tsunami_lab::io::Stations* stations, tsunami_lab::patches::WavePropagation* solver )
{
    while( true )
    {
        if( KILL_THREAD )
        {
            break;
        }
        stations->write( solver->getTotalHeight() );
        std::this_thread::sleep_for( std::chrono::milliseconds( (int)stations->getOutputFrequency() ) );
    }
}

int main( int   i_argc,
          char* i_argv[] )
{
    // number of cells in x- and y-direction
    tsunami_lab::t_idx l_nx = 0;
    tsunami_lab::t_idx l_ny = 1;

    // set cell size
    tsunami_lab::t_real l_dxy = 1;

    std::cout << "#####################################################" << std::endl;
    std::cout << "###                  Tsunami Lab                  ###" << std::endl;
    std::cout << "###                                               ###" << std::endl;
    std::cout << "### https://scalable.uni-jena.de                  ###" << std::endl;
    std::cout << "### https://rivinhd.github.io/Tsunami-Simulation/ ###" << std::endl;
    std::cout << "#####################################################" << std::endl;

    // default arguments values
    tsunami_lab::patches::Solver solver = tsunami_lab::patches::Solver::FWAVE;
    bool useBathymetry = false;
    bool reflectLeft = false;
    bool reflectRight = false;
    bool reflectTop = false;
    bool reflectBottom = false;
    bool use2D = false;

#ifndef SKIP_ARGUMENTS
    // error: wrong number of arguments.
    int minArgLength = 1 + requiredArguments;
    int maxArgLength = minArgLength + ArgSetup::getArgumentsLength( optionalArguments );
    if( i_argc < minArgLength || i_argc > maxArgLength )
    {
        std::cerr << "invalid number of arguments, usage:" << std::endl;
        printHelp();
        return EXIT_FAILURE;
    }

    // parse required Argumentes
    // Argument 1: N_CELLS_X
    l_nx = atoi( i_argv[1] );
    if( l_nx < 1 )
    {
        std::cerr << "N_CELLS_X: invalid number of cells" << std::endl;
        return EXIT_FAILURE;
    }
    // Argument 2 (optional): N_CELLS_Y
    if( i_argv[2][0] != '-' )
    {
        use2D = true;
        l_ny = atoi( i_argv[2] );
        ++minArgLength;
    }
    if( use2D && l_ny < 1 )
    {
        std::cerr << "N_CELLS_Y: invalid number of cells" << std::endl;
    }

    // parse optional Argumentes
    int argMapParameterCount[ArgSetup::LENGTH_ARG_CHAR] = { 0 };
    ArgSetup::generateCountMap( optionalArguments, argMapParameterCount );
    for( int i = minArgLength; i < i_argc; i++ )
    {
        char* arg = i_argv[i];
        if( arg[0] == '\0' || ( arg[0] == '-' && arg[1] == '\0' ) )
        {
            printHelp();
            return EXIT_FAILURE;
        }

        unsigned int argI = 0;
        std::string stringParameter;
        while( arg[++argI] != '\0' )  // starts with argI = 1
        {
            if( arg[argI] < START_ARG_CHAR || arg[argI] > END_ARG_CHAR )
            {
                std::cerr << "The Flag: " << arg[argI] << " is not a valid flag (Out of Bounds)" << std::endl;
                return EXIT_FAILURE;
            }
            if( i + argMapParameterCount[arg[argI] - START_ARG_CHAR] >= i_argc )
            {
                std::cerr << "The Flag: " << arg[argI] << " has not enough Inputs" << std::endl;
                return EXIT_FAILURE;
            }
            switch( arg[argI] )
            {
                case Arguments::SOLVER:
                    stringParameter = std::string( i_argv[++i] );
                    if( "roe" == stringParameter )
                    {
                        solver = tsunami_lab::patches::Solver::ROE;
                    }
                    else if( "fwave" == stringParameter )
                    {
                        solver = tsunami_lab::patches::Solver::FWAVE;
                    }
                    else
                    {
                        std::cerr << "unknown argument for flag -s" << std::endl
                            << "valid arguments are 'fwave', 'roe'" << std::endl;
                        return EXIT_FAILURE;
                    }
                    break;

                case Arguments::USE_BATHYMETRY:
                    useBathymetry = true;
                    std::cout << "Activated Bathymetry" << std::endl;
                    break;

                case Arguments::USE_REFLECTION:
                    //TODO finish
                    stringParameter = std::string( i_argv[++i] );
                    if( stringParameter == "left" )
                    {
                        reflectLeft = true;
                    }
                    else if( stringParameter == "right" )
                    {
                        reflectRight = true;
                    }
                    else if( stringParameter == "top" )
                    {
                        reflectTop = true;
                    }
                    else if( stringParameter == "bottom" )
                    {
                        reflectBottom = true;
                    }
                    else if( stringParameter == "x" )
                    {

                    }
                    else if( stringParameter == "y" )
                    {

                    }
                    else if( stringParameter == "all" )
                    {

                    }
                    else
                    {
                        std::cerr << "unknown argument for flag -r" << std::endl
                            << "valid arguments are 'left', 'right', 'top', 'bottom', 'x', 'y', 'all'" << std::endl
                            << "the arguments 'top' and 'bottom' only take effect if the simulation is 2d" << std::endl;
                        return EXIT_FAILURE;
                    }
                    reflectLeft = true;
                    std::cout << "Activated Reflect on Left side" << std::endl;
                    break;

                default:
                    std::cerr << "unknown flag: " << arg[argI] << std::endl;
                    printHelp();
                    return EXIT_FAILURE;
                    break;
            }
        }
    }
#endif // SKIP_ARGUMENTS
#ifdef SKIP_ARGUMENTS
    l_nx = 500;
    l_ny = 500;
    reflectLeft = false;
    reflectRight = false;
    useBathymetry = true;
    use2D = true;
    std::cout << i_argv[i_argc - 1] << std::endl;
#endif // SKIP_ARGUMENTS

    // Print activated Features
    if( use2D )
    {
        std::cout << "Simulation is set to 2D" << std::endl;
    }
    else
    {
        std::cout << "Simulation is set to 1D" << std::endl;
    }

    std::cout << "Set Solver: ";
    if( solver == tsunami_lab::patches::Solver::ROE )
    {
        std::cout << "Roe" << std::endl;
    }
    else
    {
        std::cout << "FWave" << std::endl;
    }

    if( ( useBathymetry ) && solver == tsunami_lab::patches::Solver::ROE )
    {
        std::cerr << "ERROR: Roe solver does not have options for bathymetry" << std::endl;
        return EXIT_FAILURE;
    }
    // End print

    tsunami_lab::t_real l_scaleX = 100;
    tsunami_lab::t_real l_scaleY = 100;
    l_dxy = std::min( l_scaleX / l_nx, l_scaleY / l_ny );

    std::cout << "runtime configuration" << std::endl;
    std::cout << "  number of cells in x-direction: " << l_nx << std::endl;
    std::cout << "  number of cells in y-direction: " << l_ny << std::endl;
    std::cout << "  cell size:                      " << l_dxy << std::endl;

    // construct setup
    tsunami_lab::setups::Setup* l_setup;
    l_setup = new tsunami_lab::setups::CircularDamBreak2d();


    // construct solver
    tsunami_lab::patches::WavePropagation* l_waveProp;
    if( use2D )
    {
        l_waveProp = new tsunami_lab::patches::WavePropagation2d( l_nx, l_ny );
    }
    else
    {
        l_waveProp = new tsunami_lab::patches::WavePropagation1d( l_nx );
    }

    // initialize stations
    tsunami_lab::io::Stations l_stations = tsunami_lab::io::Stations( l_nx,
                                                                      l_ny,
                                                                      l_waveProp->getStride(),
                                                                      l_scaleX,
                                                                      l_scaleY );
    // create a thread that runs the stations write function
    std::thread writeStationsThread( writeStations, &l_stations, l_waveProp );

    // set the solver to use
    l_waveProp->setSolver( solver );

    // set if bathymetry exists
    l_waveProp->enableBathymetry( useBathymetry );

    // set Reflection
    l_waveProp->setReflection( tsunami_lab::patches::WavePropagation::Side::LEFT, reflectLeft );
    l_waveProp->setReflection( tsunami_lab::patches::WavePropagation::Side::RIGHT, reflectRight );
    l_waveProp->setReflection( tsunami_lab::patches::WavePropagation::Side::TOP, reflectTop );
    l_waveProp->setReflection( tsunami_lab::patches::WavePropagation::Side::BOTTOM, reflectBottom );

    // maximum observed height in the setup
    tsunami_lab::t_real l_hMax = std::numeric_limits< tsunami_lab::t_real >::lowest();

    // set up solver
    for( tsunami_lab::t_idx l_cy = 0; l_cy < l_ny; l_cy++ )
    {
        tsunami_lab::t_real l_y = l_cy * l_dxy;

        for( tsunami_lab::t_idx l_cx = 0; l_cx < l_nx; l_cx++ )
        {
            tsunami_lab::t_real l_x = l_cx * l_dxy;

            // get initial values of the setup
            tsunami_lab::t_real l_h = l_setup->getHeight( l_x,
                                                          l_y );
            l_hMax = std::max( l_h, l_hMax );

            tsunami_lab::t_real l_hu = l_setup->getMomentumX( l_x,
                                                              l_y );
            tsunami_lab::t_real l_hv = l_setup->getMomentumY( l_x,
                                                              l_y );
            tsunami_lab::t_real l_b = l_setup->getBathymetry( l_x,
                                                              l_y );

            // set initial values in wave propagation solver
            l_waveProp->setHeight( l_cx,
                                   l_cy,
                                   l_h );

            l_waveProp->setMomentumX( l_cx,
                                      l_cy,
                                      l_hu );

            l_waveProp->setMomentumY( l_cx,
                                      l_cy,
                                      l_hv );

            l_waveProp->setBathymetry( l_cx,
                                       l_cy,
                                       l_b );
        }
    }

    for( size_t i = 0; i < l_ny; i++ )
    {
        for( size_t j = 0; j < l_nx; j++ )
        {
            tsunami_lab::t_real value = std::sin( 2 * 3.14 * j / ( 1.0f * l_nx ) ) + 3 * std::cos( 2 * 3.14 * i / ( 1.0f * l_ny ) );
            value -= 5;
            value = std::min( value, 3.0f );
            l_waveProp->setBathymetry( i, j, value );
        }
    }
    for( size_t i = 120; i < 130; i++ )
    {
        for( size_t j = 200; j < 400; j++ )
        {
            l_waveProp->setBathymetry( i, j, 10 );
        }
    }
    l_waveProp->updateWaterHeight();

    // derive maximum wave speed in setup; the momentum is ignored
    tsunami_lab::t_real l_speedMax = std::sqrt( 9.81 * l_hMax );
    std::cout << "Max speed " << l_speedMax << std::endl;

    // derive constant time step; changes at simulation time are ignored
    tsunami_lab::t_real l_dt = 0.45 * l_dxy / l_speedMax;

    // derive scaling for a time step
    tsunami_lab::t_real l_scaling = l_dt / l_dxy;

    // set up time and print control
    tsunami_lab::t_idx  l_timeStep = 0;
    tsunami_lab::t_idx  l_nOut = 0;
    tsunami_lab::t_real l_endTime = 25;
    tsunami_lab::t_real l_simTime = 0;


    // create simulation folder inside solution folder
    if( !fs::exists( SOLUTION_FOLDER ) )
    {
        fs::create_directory( SOLUTION_FOLDER );
    }
    if( fs::exists( SOLUTION_FOLDER + "/simulation" ) )
    {
        fs::remove_all( SOLUTION_FOLDER + "/simulation" );
    }
    fs::create_directory( SOLUTION_FOLDER + "/simulation" );

    std::cout << "entering time loop" << std::endl;

    // iterate over time
    while( l_simTime < l_endTime )
    {
        if( l_timeStep % 25 == 0 )
        {
            std::cout << "  simulation time / #time steps: "
                << l_simTime << " / " << l_timeStep << std::endl;

            std::string l_path = SOLUTION_FOLDER + "/simulation/solution_" + std::to_string( l_nOut ) + ".csv";
            std::cout << "  writing wave field to " << l_path << std::endl;

            std::ofstream l_file;
            l_file.open( l_path );

            tsunami_lab::io::Csv::write( l_dxy,
                                         l_nx,
                                         l_ny,
                                         l_waveProp->getStride(),
                                         l_waveProp->getTotalHeight(),
                                         l_waveProp->getMomentumX(),
                                         l_waveProp->getMomentumY(),
                                         l_waveProp->getBathymetry(),
                                         l_waveProp->getTotalHeight(),
                                         l_file );
            l_file.close();
            l_nOut++;
        }

        l_waveProp->setGhostOutflow();
        l_waveProp->timeStep( l_scaling );

        l_timeStep++;
        l_simTime += l_dt;
    }
    std::cout << "finished time loop" << std::endl;

    // free memory
    std::cout << "freeing memory" << std::endl;
    delete l_setup;
    delete l_waveProp;

    // kill thread
    KILL_THREAD = true;
    // wait for thread
    writeStationsThread.join();

    std::cout << "finished, exiting" << std::endl;
    return EXIT_SUCCESS;
}
