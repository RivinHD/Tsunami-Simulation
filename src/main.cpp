/**
 * @author Alexander Breuer (alex.breuer AT uni-jena.de)
 *
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
#include "../include/setups/ArtificialTsunami2d.h"
#include "../include/io/Csv.h"
#include "../include/io/NetCdf.h"
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
#include <atomic>

namespace fs = std::filesystem;

const std::string SOLUTION_FOLDER = "solutions";
std::atomic_bool KILL_THREAD = false;

enum Arguments
{
    SOLVER = 's',
    USE_BATHYMETRY = 'B',
    REFLECTION = 'r',
    TIME = 't',
    IO_FORMAT = 'f'

};
const int requiredArguments = 1;
const int optionalArguments = 1;
const std::vector<ArgSetup> optionalFlags = {
    ArgSetup( Arguments::SOLVER, 1, 1 ),
    ArgSetup( Arguments::USE_BATHYMETRY, 0, 0 ),
    ArgSetup( Arguments::REFLECTION, 1, 4 ),
    ArgSetup( Arguments::TIME, 1, 1 ),
    ArgSetup( Arguments::IO_FORMAT, 1, 1 )
};

void printHelp()
{
    const char* reset = "\033[0m";
    const char* cyan = "\033[36;49m";
    const char* magenta = "\033[35;49m";
    const char* green = "\033[32;49m";

    std::cerr << "./build/simulation " << magenta << "N_CELLS_X (N_CELLS_Y) " << reset << "["
        << green << "-s " << cyan << "<fwave|roe>" << reset << "] ["
        << green << "-B" << reset << "] ["
        << green << "-r " << cyan << "<left|right|top|bottom|x|y|all>" << reset << "] ["
        << green << "-t" << cyan << " <seconds>" << reset << "] ["
        << green << "-f" << cyan << " <csv|netCDF>"
        << std::endl << std::endl
        << "REQUIERED INPUT:" << std::endl
        << magenta << "\tN_CELLS_X" << reset << " is the number of cells in x-direction." << std::endl
        << std::endl
        << "OPTIONAL INPUT:" << std::endl
        << magenta << "\tN_CELLS_Y" << reset << " is the number of cells in y-direction." << std::endl
        << std::endl
        << "NOTE: optional flags must be set after the inputs.." << std::endl
        << "OPTIONAL FLAGS:" << std::endl
        << green << "\t-s" << reset << " set used solvers requires " << cyan << "fwave" << reset << " or " << cyan << "roe" << reset << " as inputs. The default is fwave." << std::endl
        << green << "\t-B" << reset << " enables the use of bathymetry." << std::endl
        << green << "\t-r" << reset << " enables the reflection on the specified side of the simulation. Several arguments can be passed (maximum 4)." << std::endl
        << "\t   where " << cyan << "left | right | top | bottom" << reset << " enables their respective sides." << std::endl
        << "\t   where " << cyan << "x" << reset << " enables the left & right and " << cyan << "y" << reset << " enables the top & bottom side." << std::endl
        << "\t   where " << cyan << "all" << reset << " enables all sides." << std::endl
        << green << "\t-t" << reset << " defines the total time in seconds that is used for the simulation. The default is 5 seconds." << std::endl
        << green << "\t-f" << reset << " defines the output format. Requires " << cyan << "csv" << reset << " or " << cyan << "netCDF" << reset << ". The default is netCDF." << std::endl;
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
        std::this_thread::sleep_for( std::chrono::seconds( (int)stations->getOutputFrequency() ) );
    }
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

    // default arguments values
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
    tsunami_lab::t_real l_endTime = 5;

#ifndef SKIP_ARGUMENTS
    // error: wrong number of arguments.
    int minArgLength = 1 + requiredArguments;
    int maxArgLength = minArgLength + optionalArguments + ArgSetup::getArgumentsLength( optionalFlags );
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
        printHelp();
        return EXIT_FAILURE;
    }
    // Argument 2 (optional): N_CELLS_Y
    if( i_argc > 2 && i_argv[2][0] != '-' )
    {
        use2D = true;
        l_ny = atoi( i_argv[2] );
        ++minArgLength;
    }
    if( use2D && l_ny < 1 )
    {
        std::cerr << "N_CELLS_Y: invalid number of cells" << std::endl;
        printHelp();
        return EXIT_FAILURE;
    }

    // parse optional Argumentes
    int argMapParameterCount[ArgSetup::LENGTH_ARG_CHAR] = { 0 };
    ArgSetup::generateCountMap( optionalFlags, argMapParameterCount );
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
        float floatParameter;
        int startIndex;
        while( arg[++argI] != '\0' )  // starts with argI = 1
        {
            if( arg[argI] < START_ARG_CHAR || arg[argI] > END_ARG_CHAR )
            {
                std::cerr << "The Flag: " << green << arg[argI] << reset << " is not a valid flag (Out of Bounds)" << std::endl;
                return EXIT_FAILURE;
            }
            if( i + argMapParameterCount[arg[argI] - START_ARG_CHAR] >= i_argc )
            {
                std::cerr << "The Flag: " << green << arg[argI] << reset << " has not enough Inputs" << std::endl;
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
                        std::cerr << "'" << stringParameter << "' is an unknown argument for flag -s" << std::endl
                            << "valid arguments are 'fwave', 'roe'" << std::endl;
                        return EXIT_FAILURE;
                    }
                    break;

                case Arguments::USE_BATHYMETRY:
                    useBathymetry = true;
                    break;

                case Arguments::REFLECTION:
                    startIndex = i;
                    stringParameter = std::string( i_argv[i + 1] );
                    do
                    {
                        i += 1;
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
                            reflectLeft = true;
                            reflectRight = true;
                        }
                        else if( stringParameter == "y" )
                        {
                            reflectTop = true;
                            reflectBottom = true;
                        }
                        else if( stringParameter == "all" )
                        {
                            reflectLeft = true;
                            reflectRight = true;
                            reflectTop = true;
                            reflectBottom = true;
                        }
                        else if( i == startIndex + 1 )
                        {
                            std::cerr << "'" << stringParameter << "' is an unknown argument for flag -r" << std::endl
                                << "valid arguments are 'left', 'right', 'top', 'bottom', 'x', 'y', 'all'" << std::endl
                                << "the arguments 'top' and 'bottom' only take effect if the simulation is 2d" << std::endl;
                            return EXIT_FAILURE;
                        }
                        else
                        {
                            break;
                        }
                        if( i + 1 >= i_argc )
                        {
                            break;
                        }
                        stringParameter = std::string( i_argv[i + 1] );
                    } while( stringParameter[0] != '-' && i < startIndex + 4 );
                    break;
                case Arguments::TIME:
                    floatParameter = atof( i_argv[++i] );
                    if( floatParameter <= 0 || std::isnan( floatParameter ) || std::isinf( floatParameter ) )
                    {
                        std::cerr << "invalid argument for flag -r" << std::endl
                            << "the time should be a number larger than 0" << std::endl;
                        return EXIT_FAILURE;
                    }
                    l_endTime = floatParameter;
                    break;
                case Arguments::IO_FORMAT:
                    stringParameter = std::string( i_argv[++i] );
                    if( stringParameter == "csv" )
                    {
                        isCsv = true;
                    }
                    else if( stringParameter == "netCDF" )
                    {
                        isCsv = false;
                    }
                    else
                    {
                        std::cerr << "'" << stringParameter << "' is an unknown argument for flag -f" << std::endl
                            << "valid arguments are 'csv', 'netCDF'" << std::endl;
                        return EXIT_FAILURE;
                    }
                    break;

                default:
                    std::cerr << "unknown flag: " << arg[argI] << std::endl;
                    printHelp();
                    return EXIT_FAILURE;
                    break;
            }
        }
    }
#endif // !SKIP_ARGUMENTS
#ifdef SKIP_ARGUMENTS
    l_nx = 500;
    l_ny = 500;
    reflectLeft = false;
    reflectRight = false;
    reflectBottom = false;
    reflectTop = false;
    useBathymetry = true;
    use2D = false;
    l_endTime = 5;
    std::cout << i_argv[i_argc - 1] << std::endl;
#endif // SKIP_ARGUMENTS

    // Print activated Features
    if( use2D )
    {
        std::cout << "Simulation is set to " << green << "2D" << reset << std::endl;
    }
    else
    {
        std::cout << "Simulation is set to" << green << "1D" << reset << std::endl;
    }

    if( useBathymetry )
    {
        std::cout << green << "Activated Bathymetry" << reset << std::endl;
    }

    std::cout << "Set Solver: ";
    if( solver == tsunami_lab::patches::Solver::ROE )
    {
        std::cout << green << "Roe" << reset << std::endl;
    }
    else
    {
        std::cout << green << "FWave" << reset << std::endl;
    }

    if( ( useBathymetry ) && solver == tsunami_lab::patches::Solver::ROE )
    {
        std::cerr << "ERROR: Roe solver does not have options for bathymetry" << std::endl;
        return EXIT_FAILURE;
    }

    std::string reflectionsText = "";
    bool reflectionAppended = false;
    reflectionsText += reflectLeft ? "Left" : "";
    reflectionAppended |= reflectLeft;
    reflectionsText += reflectRight ? ( reflectionAppended ? ", Right" : "Right" ) : "";
    reflectionAppended |= reflectRight;
    reflectionsText += reflectTop ? ( reflectionAppended ? ", Top" : "Top" ) : "";
    reflectionAppended |= reflectTop;
    reflectionsText += reflectBottom ? ( reflectionAppended ? ", Bottom" : "Bottom" ) : "";
    reflectionAppended |= reflectBottom;
    if( reflectionsText != "" )
    {
        std::cout << "Activated Reflection on " << green << reflectionsText << reset << " side" << std::endl;
    }

    std::cout << "Simulation Time is set to " << green << l_endTime << " seconds" << reset << std::endl
        << "Output format is set to " << green << ( isCsv ? "csv" : "netCDF" ) << reset << std::endl;
    // End print

    tsunami_lab::t_real l_scaleX = 10000;
    tsunami_lab::t_real l_scaleY = 10000;
    if( use2D )
    {
        l_dxy = std::min( l_scaleX / l_nx, l_scaleY / l_ny );
    }
    else
    {
        l_dxy = l_scaleX / l_nx;
    }

    std::cout << "runtime configuration" << std::endl;
    std::cout << "  number of cells in x-direction: " << l_nx << std::endl;
    std::cout << "  number of cells in y-direction: " << l_ny << std::endl;
    std::cout << "  cell size:                      " << l_dxy << std::endl;

    // construct setup
    tsunami_lab::setups::Setup* l_setup;
    l_setup = new tsunami_lab::setups::ArtificialTsunami2d();


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

    // derive maximum wave speed in setup; the momentum is ignored
    tsunami_lab::t_real l_speedMax = std::sqrt( 9.81 * l_hMax );
    std::cout << "Max speed " << l_speedMax << std::endl;

    // derive constant time step; changes at simulation time are ignored
    tsunami_lab::t_real l_dt = std::min( 0.45 * l_dxy / l_speedMax, 0.01 );

    // derive scaling for a time step
    tsunami_lab::t_real l_scaling = l_dt / l_dxy;

    // set up time and print control
    tsunami_lab::t_idx  l_timeStep = 0;
    tsunami_lab::t_idx  l_nOut = 0;
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

    tsunami_lab::io::NetCdf* netCdfWriter = nullptr;
    if( !isCsv )
    {
        netCdfWriter = new tsunami_lab::io::NetCdf( SOLUTION_FOLDER + "/simulation/solution.nc",
                                                    l_nx,
                                                    l_ny,
                                                    l_scaleX,
                                                    l_scaleY,
                                                    l_waveProp->getStride() );
    }

    // iterate over time
    while( l_simTime < l_endTime )
    {
        if( l_timeStep % 25 == 0 )
        {
            std::cout << "  simulation time / #time steps: "
                << l_simTime << " / " << l_timeStep << std::endl;


            if( isCsv )
            {
                std::string l_path = SOLUTION_FOLDER + "/simulation/solution_" + std::to_string( l_nOut ) + ".csv";
                std::cout << "  writing wave field to " << l_path << std::endl;

                std::ofstream l_file;
                l_file.open( l_path );
                tsunami_lab::io::Csv::write( l_dxy,
                                             l_nx,
                                             l_ny,
                                             l_waveProp->getStride(),
                                             l_waveProp->getHeight(),
                                             l_waveProp->getMomentumX(),
                                             l_waveProp->getMomentumY(),
                                             l_waveProp->getBathymetry(),
                                             l_waveProp->getTotalHeight(),
                                             l_file );
                l_file.close();
                l_nOut++;
            }
            else
            {
                netCdfWriter->write( l_simTime,
                                     l_waveProp->getTotalHeight(),
                                     l_waveProp->getBathymetry(),
                                     l_waveProp->getMomentumX(),
                                     l_waveProp->getMomentumY() );
            }
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
    delete netCdfWriter;

    // kill thread
    KILL_THREAD = true;
    // wait for threads
    writeStationsThread.join();

    std::cout << "finished, exiting" << std::endl;
    return EXIT_SUCCESS;
}
