/**
 * @author Fabian Hofer, Vincent Gerlach (AT uni-jena.de)
 *
 * @section DESCRIPTION
 * Tests the supercritical Flow setup.
 **/
#include "../../include/setups/SupercriticalFlow1d.h"
#include <catch2/catch.hpp>

TEST_CASE( "Test the one-dimensional supercritical setup.", "[SupercriticalFlow1d]" )
{
	tsunami_lab::setups::SupercriticalFlow1d l_supercriticalFlow;

	// out of range
	REQUIRE( l_supercriticalFlow.getHeight( 2, 0 ) == 0.33f );

	REQUIRE( l_supercriticalFlow.getMomentumX( 2, 0 ) == 0.18f );

	REQUIRE( l_supercriticalFlow.getMomentumY( 2, 0 ) == 0.0f );

	REQUIRE( l_supercriticalFlow.getBathymetry( 2, 0 ) == -0.33f );

	REQUIRE( l_supercriticalFlow.getHeight( 2, 5 ) == 0.33f );

	REQUIRE( l_supercriticalFlow.getMomentumX( 2, 5 ) == 0.18f );

	REQUIRE( l_supercriticalFlow.getMomentumY( 2, 2 ) == 0.0f );

	REQUIRE( l_supercriticalFlow.getBathymetry( 2, 2 ) == -0.33f );

	REQUIRE( l_supercriticalFlow.getHeight( 20, 0 ) == 0.33f );

	REQUIRE( l_supercriticalFlow.getMomentumX( 20, 0 ) == 0.18f );

	REQUIRE( l_supercriticalFlow.getMomentumY( 20, 0 ) == 0.0f );

	REQUIRE( l_supercriticalFlow.getBathymetry( 20, 0 ) == -0.33f );

	REQUIRE( l_supercriticalFlow.getHeight( 20, 5 ) == 0.33f );

	REQUIRE( l_supercriticalFlow.getMomentumX( 20, 5 ) == 0.18f );

	REQUIRE( l_supercriticalFlow.getMomentumY( 20, 2 ) == 0.0f );

	REQUIRE( l_supercriticalFlow.getBathymetry( 20, 2 ) == -0.33f );


	// in range (8, 12)
	REQUIRE( l_supercriticalFlow.getHeight( 10, 0 ) == 0.12f );

	REQUIRE( l_supercriticalFlow.getMomentumX( 10, 0 ) == 0.18f );

	REQUIRE( l_supercriticalFlow.getMomentumY( 10, 0 ) == 0.0f );

	REQUIRE( l_supercriticalFlow.getBathymetry( 10, 0 ) == -0.12f );

	REQUIRE( l_supercriticalFlow.getHeight( 10, 5 ) == 0.12f );

	REQUIRE( l_supercriticalFlow.getMomentumX( 10, 5 ) == 0.18f );

	REQUIRE( l_supercriticalFlow.getMomentumY( 10, 2 ) == 0.0f );

	REQUIRE( l_supercriticalFlow.getBathymetry( 10, 2 ) == -0.12f );
}