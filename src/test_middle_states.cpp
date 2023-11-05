/**numberOfTests
 * @author Fabian Hofer, Vincent Gerlach (AT uni-jena.de)
 *
 * @section DESCRIPTION
 * Entry-point for middle states tests.
 **/
#include "../include/patches/WavePropagation1d.h"
#include "../include/io/Csv.h"
#include "../include/constants.h"
#include "../include/setups/MiddleStates1d.h"
#include <cstdlib>
#include <iostream>
#include <cmath>
#include <fstream>
#include <limits>

#define CATCH_CONFIG_RUNNER
#include <catch2/catch.hpp>
#undef CATCH_CONFIG_RUNNER

 // Setup the Configuration Variables for the Test against middle_states.csv
const tsunami_lab::t_idx numberOfCells = 10;
const unsigned int numberOfTests = 1000000;
const double testAccuracy = 0.99;
const double accuracyMargin = 0.03;
const tsunami_lab::patches::Solver solver = tsunami_lab::patches::Solver::FWave;

int main( int i_argc, char* i_argv[] )
{
	// Run the MiddleStates Test with Catch2
	std::cout << "Run MiddleStates Test" << std::endl;
	int l_result = Catch::Session().run( i_argc, i_argv );
	return ( l_result < 0xff ? l_result : 0xff );
}

TEST_CASE( "Test against the middle_states.csv", "[MiddleStates]" )
{
	// Read the middle_states.csv
	std::ifstream middle_states( "resources/middle_states.csv" );

	unsigned int successfulTests = 0;
	unsigned int evaluatedTests = 0;
	unsigned int nanProblems = 0;

	// parse each line of the middle_states.csv and test against the simulation
	tsunami_lab::t_real hLeft, hRight, huLeft, huRight, hStar;
	while( evaluatedTests < numberOfTests
		   && tsunami_lab::io::Csv::next_middle_states( middle_states,
														hLeft,
														hRight,
														huLeft,
														huRight,
														hStar ) )
	{
		tsunami_lab::t_real l_dxy = 10.0 / numberOfCells;
		tsunami_lab::t_real l_location = 5.0;
		tsunami_lab::t_real startHeightDifference = abs( hLeft - hRight );

		// construct setup
		tsunami_lab::setups::Setup* l_setup = new tsunami_lab::setups::MiddleStates1d( hLeft, hRight, huLeft, huRight, l_location );

		// construct solver
		tsunami_lab::patches::WavePropagation* l_waveProp;
		l_waveProp = new tsunami_lab::patches::WavePropagation1d( numberOfCells );

		// set the solver to use
		l_waveProp->setSolver( solver );

		// maximum observed height in the setup
		tsunami_lab::t_real l_hMax = std::numeric_limits< tsunami_lab::t_real >::lowest();

		// set up solver
		for( tsunami_lab::t_idx l_cy = 0; l_cy < numberOfCells; l_cy++ )
		{
			tsunami_lab::t_real l_y = l_cy * l_dxy;

			for( tsunami_lab::t_idx l_cx = 0; l_cx < numberOfCells; l_cx++ )
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

		// derive maximum wave speed in setup; the momentum is ignored
		tsunami_lab::t_real l_speedMax = std::sqrt( 9.81 * l_hMax );

		// derive constant time step; changes at simulation time are ignored
		tsunami_lab::t_real l_dt = 0.5 * l_dxy / l_speedMax;

		// derive scaling for a time step
		tsunami_lab::t_real l_scaling = l_dt / l_dxy;

		// set up time and print control
		tsunami_lab::t_idx  l_timeStep = 0;
		tsunami_lab::t_real l_endTime = 1.25;
		tsunami_lab::t_real l_simTime = 0;

		// iterate over time
		while( l_simTime < l_endTime )
		{
			l_waveProp->setGhostOutflow();
			l_waveProp->timeStep( l_scaling );

			l_timeStep++;
			l_simTime += l_dt;
		}

		// test hStar against read value from middle_states.csv
		tsunami_lab::t_idx l_iy = 1;
		tsunami_lab::t_idx i_stride = 1;
		tsunami_lab::t_idx l_id = l_iy * i_stride + static_cast<tsunami_lab::t_real>( l_location * l_dxy );
		const tsunami_lab::t_real* heights = l_waveProp->getHeight();
		tsunami_lab::t_real delta = abs( hStar - heights[l_id] );
		tsunami_lab::t_real relativDeviation = delta / ( startHeightDifference + 1 );
		bool isSameHeight = ( relativDeviation <= accuracyMargin );
		successfulTests += isSameHeight;
		nanProblems += std::isnan( delta );
		if( !isSameHeight )
		{
			std::cout << "FAILED: Deviation to high from Test " << evaluatedTests << " (relativ deviation:" << relativDeviation << ")" << std::endl;
		}

		// free memory
		delete l_setup;
		delete l_waveProp;
		++evaluatedTests;
	}

	// close the file and print the results
	middle_states.close();
	std::cout << successfulTests << " Tests were successful of " << evaluatedTests << " with " << nanProblems << " Nan evaluations" << std::endl
		<< "Accuracy of " << successfulTests / static_cast<double>( evaluatedTests ) << " with Margin of " << accuracyMargin << " and " << numberOfCells << " Cells" << std::endl;
	REQUIRE( successfulTests / static_cast<double>( evaluatedTests ) >= testAccuracy );
}