/**
 * @author Alexander Breuer (alex.breuer AT uni-jena.de), Fabian Hofer, Vincent Gerlach
 *
 * Entry-point for simulations.
 **/

 // #define SKIP_ARGUMENTS
 // #define TSUNAMI_SIMULATION_DISABLE_IO

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
#include <AMReX_ParmParse.H>
#include <cstdlib>
#include <iostream>
#include <cmath>
#include <limits>
#include <string>
#include <chrono>
#include <filesystem> // requieres C++17 and up
namespace fs = std::filesystem;

int main( int   i_argc,
          char* i_argv[] )
{

    int argc = 2;
    std::string arg1 = fs::absolute( "resources/inputs.amrex" ).string();
    char** argv = new char* [argc];
    argv[0] = i_argv[0];
    argv[1] = const_cast<char*>( arg1.c_str() );

    amrex::Initialize( argc, argv );
    {
        const char* reset = "\033[0m";
        const char* green = "\033[32;49m";

        std::cout << "#####################################################" << std::endl;
        std::cout << "###                  Tsunami Lab                  ###" << std::endl;
        std::cout << "###                                               ###" << std::endl;
        std::cout << "### https://scalable.uni-jena.de                  ###" << std::endl;
        std::cout << "### https://rivinhd.github.io/Tsunami-Simulation/ ###" << std::endl;
        std::cout << "#####################################################" << std::endl;

        amrex::ParmParse ppAmr( "amr" );

        // create plot folder
        std::string plotFolder;
        ppAmr.query( "plot_folder", plotFolder );
        if( fs::exists( plotFolder ) )
        {
            fs::remove_all( plotFolder );
        }
        fs::create_directory( plotFolder );

        amrex::ParmParse ppGeometry( "geometry" );
        amrex::Vector<amrex::Real> scale;
        ppGeometry.getarr( "prob_hi", scale, 0, 2 );

        const char* variables[3]{ "x", "y", "z" };
        tsunami_lab::setups::Setup* setup =
            new tsunami_lab::setups::TsunamiEvent2d( "resources/gebco20/tohoku_gebco20_usgs_250m_bath.nc",
                                                     variables,
                                                     "resources/gebco20/tohoku_gebco20_usgs_250m_displ.nc",
                                                     variables,
                                                     scale[0],
                                                     scale[1] );

        // construct solver
        tsunami_lab::amr::AMRCoreWavePropagation2d* waveProp = new tsunami_lab::amr::AMRCoreWavePropagation2d( setup );

        // set Reflection
        bool reflectionLeft;
        bool reflectRight;
        bool reflectTop;
        bool reflectBottom;
        ppGeometry.query( "reflection_left", reflectionLeft );
        ppGeometry.query( "reflection_right", reflectRight );
        ppGeometry.query( "reflection_top", reflectTop );
        ppGeometry.query( "reflection_bottom", reflectBottom );
        waveProp->setReflection( tsunami_lab::patches::WavePropagation::Side::LEFT, reflectionLeft );
        waveProp->setReflection( tsunami_lab::patches::WavePropagation::Side::RIGHT, reflectRight );
        waveProp->setReflection( tsunami_lab::patches::WavePropagation::Side::TOP, reflectTop );
        waveProp->setReflection( tsunami_lab::patches::WavePropagation::Side::BOTTOM, reflectBottom );

        // free no longer needed setup
        delete setup;

        // derive constant time step; changes at simulation time are ignored

        const auto startTime = std::chrono::high_resolution_clock::now();

        std::cout << "entering time loop" << std::endl;

        // iterate over time
        waveProp->Evolve();

        std::cout << "finished time loop" << std::endl;

        // free memory
        std::cout << "freeing memory" << std::endl;
        delete waveProp;
        delete[] argv;

        // Print the calculation time
        const auto duration = std::chrono::high_resolution_clock::now() - startTime;
        const auto hours = std::chrono::duration_cast<std::chrono::hours>( duration );
        const auto minutes = std::chrono::duration_cast<std::chrono::minutes>( duration - hours );
        const auto seconds = std::chrono::duration_cast<std::chrono::seconds>( duration - hours - minutes );
        std::cout << "The Simulation took " << green << hours.count() << " h "
            << minutes.count() << " min " << seconds.count() << " sec" << reset << " to finish." << std::endl;

        std::cout << "finished, exiting" << std::endl;
    }
    amrex::Finalize();
    return EXIT_SUCCESS;
}
