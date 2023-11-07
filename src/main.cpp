/**
 * @author Alexander Breuer (alex.breuer AT uni-jena.de)
 *
 * @section DESCRIPTION
 * Entry-point for simulations.
 **/
#include "../include/patches/WavePropagation1d.h"
#include "../include/setups/DamBreak1d.h"
#include "../include/setups/RareRare1d.h"
#include "../include/setups/ShockShock1d.h"
#include "../include/io/Csv.h"
#include "../include/io/ArgSetup.h"
#include <cstdlib>
#include <iostream>
#include <cmath>
#include <fstream>
#include <limits>
#include <string>
#include <filesystem> // requieres C++17 and up

namespace fs = std::filesystem;

#define SKIP_ARGUMENTS

const std::string SOLUTION_FOLDER = "solutions";

enum Arguments
{
	SOLVER = 's',
	USE_BATHYMETRY = 'B',
	USE_REFLECT_LEFT = 'L',
	USE_REFLECT_RIGHT = 'R'
};
const int requieredArguments = 1;
const std::vector<ArgSetup> optionalArguments = {
	ArgSetup( Arguments::SOLVER, 1 ),
	ArgSetup( Arguments::USE_BATHYMETRY, 0 ),
	ArgSetup( Arguments::USE_REFLECT_LEFT, 0 ),
	ArgSetup( Arguments::USE_REFLECT_RIGHT, 1 )
};

void printHelp()
{
	std::cerr << "./build/simulation N_CELLS_X [-s <fwave|roe>] [-B] [-L] [-R]" << std::endl << std::endl
		<< "REQUIERED INPUT:" << std::endl
		<< "\tN_CELLS_X is the number of cells in x-direction." << std::endl << std::endl
		<< "NOTE: optional flags has be put after the requiered input" << std::endl
		<< "OPTIONAL FLAGS:" << std::endl
		<< "\t-s set used solvers requires 'fwave' or 'roe' as inputs" << std::endl
		<< "\t-B enables the input for bathymetry" << std::endl
		<< "\t-L enables the reflection on the left side of the simulation" << std::endl
		<< "\t-R enables the reflectoin on the right side of the simulation" << std::endl;
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

#ifndef SKIP_ARGUMENTS
	// error: wrong number of arguments.
	int minArgLength = 1 + requieredArguments;
	int maxArgLength = minArgLength + ArgSetup::getOptionalArgLength( optionalArguments );
	if( i_argc < minArgLength || i_argc > maxArgLength )
	{
		std::cerr << "invalid number of arguments, usage:" << std::endl;
		printHelp();
		return EXIT_FAILURE;
	}

	// parse requiered Argumentes
	// Argument 1: N_CELLS_X
	l_nx = atoi( i_argv[1] );
	if( l_nx < 1 )
	{
		std::cerr << "invalid number of cells" << std::endl;
		return EXIT_FAILURE;
	}

	int argMapParamterCount[ArgSetup::LENGTH_ARG_CHAR] = { 0 };
	ArgSetup::generateCountMap( optionalArguments, argMapParamterCount );
	// parse optional Argumentes
	for( int i = minArgLength; i < i_argc; i++ )
	{
		char* arg = i_argv[i];
		if( arg[0] == '\0' || ( arg[0] == '-' && arg[1] == '\0' ) )
		{
			printHelp();
			return EXIT_FAILURE;
		}

		unsigned int argi = 0;
		std::string stringParamter;
		while( arg[++argi] != '\0' )  // startes with argi = 1
		{
			if( arg[argi] < START_ARG_CHAR || arg[argi] > END_ARG_CHAR )
			{
				std::cerr << "The Flag: " << arg[argi] << " is not a valid flag (Out of Bounds)" << std::endl;
				return EXIT_FAILURE;
			}
			if( i + argMapParamterCount[arg[argi] - START_ARG_CHAR] >= i_argc )
			{
				std::cerr << "The Flag: " << arg[argi] << " has not enough Inputs" << std::endl;
				return EXIT_FAILURE;
			}
			switch( arg[argi] )
			{
				case Arguments::SOLVER:
					stringParamter = std::string( i_argv[++i] );
					if( "roe" == stringParamter )
					{
						std::cout << "Set Solver: Roe" << std::endl;
						solver = tsunami_lab::patches::Solver::ROE;
					}
					else if( "fwave" == stringParamter )
					{
						std::cout << "Set Solver: FWave" << std::endl;
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

				case Arguments::USE_REFLECT_LEFT:
					reflectLeft = true;
					std::cout << "Activated Reflect on Left side" << std::endl;
					break;

				case Arguments::USE_REFLECT_RIGHT:
					reflectRight = true;
					std::cout << "Activated Reflect on Right side" << std::endl;
					break;

				default:
					std::cerr << "unknown flag: " << arg[argi] << std::endl;
					printHelp();
					return EXIT_FAILURE;
					break;
			}
		}
	}
#endif // SKIP_ARGUMENTS
#ifdef SKIP_ARGUMENTS
	l_nx = 1000;
	reflectRight = true;
	useBathymetry = true;
	std::cout << i_argv[i_argc - 1] << std::endl;
#endif // SKIP_ARGUMENTS

	if( useBathymetry && solver == tsunami_lab::patches::Solver::ROE )
	{
		std::cerr << "ERROR: Roe solver does not have options for bathymetry" << std::endl;
		return EXIT_FAILURE;
	}

	l_dxy = 10.0 / l_nx;

	std::cout << "runtime configuration" << std::endl;
	std::cout << "  number of cells in x-direction: " << l_nx << std::endl;
	std::cout << "  number of cells in y-direction: " << l_ny << std::endl;
	std::cout << "  cell size:                      " << l_dxy << std::endl;

	// construct setup
	tsunami_lab::setups::Setup* l_setup;

	tsunami_lab::t_real l_hl = 12;
	tsunami_lab::t_real l_hr = 8;
	// tsunami_lab::t_real l_ml = 2000;
	tsunami_lab::t_real l_location = 3;

	l_setup = new tsunami_lab::setups::DamBreak1d( l_hl, l_hr, l_location );
	// l_setup = new tsunami_lab::setups::RareRare1d(l_hl, l_ml, l_location);
	// l_setup = new tsunami_lab::setups::ShockShock1d(l_hl, l_ml, l_location);


	// construct solver
	tsunami_lab::patches::WavePropagation* l_waveProp;
	l_waveProp = new tsunami_lab::patches::WavePropagation1d( l_nx );

	// set the solver to use
	l_waveProp->setSolver( solver );

	// set if bathymetry exists
	l_waveProp->enableBathymetry( useBathymetry );

	// set Reflection
	l_waveProp->setReflection( tsunami_lab::patches::WavePropagation::Side::LEFT, reflectLeft );
	l_waveProp->setReflection( tsunami_lab::patches::WavePropagation::Side::RIGHT, reflectRight );

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

		}
	}

	// TODO remove test bathymetry DUNE
	l_waveProp->setBathymetry( 700, 0, 1 );
	l_waveProp->setBathymetry( 701, 0, 1.3 );
	l_waveProp->setBathymetry( 702, 0, 1.5 );
	l_waveProp->setBathymetry( 703, 0, 1.2 );
	l_waveProp->setBathymetry( 704, 0, 1.1 );

	// TODO remove test reflection
	l_waveProp->setBathymetry( 100, 0, 13 );
	l_waveProp->setBathymetry( 101, 0, 13 );
	l_waveProp->setBathymetry( 102, 0, 13 );
	l_waveProp->setBathymetry( 103, 0, 13 );

	// recacluate the water with bathmetry
	l_waveProp->updateWaterHeight();


	// derive maximum wave speed in setup; the momentum is ignored
	tsunami_lab::t_real l_speedMax = std::sqrt( 9.81 * l_hMax );
	std::cout << "Max speed" << l_speedMax << std::endl;

	// derive constant time step; changes at simulation time are ignored
	tsunami_lab::t_real l_dt = 0.5 * l_dxy / l_speedMax;

	// derive scaling for a time step
	tsunami_lab::t_real l_scaling = l_dt / l_dxy;

	// set up time and print control
	tsunami_lab::t_idx  l_timeStep = 0;
	tsunami_lab::t_idx  l_nOut = 0;
	tsunami_lab::t_real l_endTime = 1.25;
	tsunami_lab::t_real l_simTime = 0;


	// create solution folder
	if( fs::exists( SOLUTION_FOLDER ) )
	{
		fs::remove_all( SOLUTION_FOLDER );
	}
	fs::create_directory( SOLUTION_FOLDER );

	std::cout << "entering time loop" << std::endl;

	// iterate over time
	while( l_simTime < l_endTime )
	{
		if( l_timeStep % 25 == 0 )
		{
			std::cout << "  simulation time / #time steps: "
				<< l_simTime << " / " << l_timeStep << std::endl;

			std::string l_path = SOLUTION_FOLDER + "/solution_" + std::to_string( l_nOut ) + ".csv";
			std::cout << "  writing wave field to " << l_path << std::endl;

			std::ofstream l_file;
			l_file.open( l_path );

			tsunami_lab::io::Csv::write( l_dxy,
										 l_nx,
										 1,
										 1,
										 l_waveProp->getHeight(),
										 l_waveProp->getMomentumX(),
										 nullptr,
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

	std::cout << "finished, exiting" << std::endl;
	return EXIT_SUCCESS;
}
