/**
 * @author Fabian Hofer, Vincent Gerlach (AT uni-jena.de)
 *
 * Tests the subcritical Flow setup.
 **/
#include "../../include/setups/SubcriticalFlow1d.h"
#include <catch2/catch.hpp>

TEST_CASE( "Test the one-dimensional subcritical setup.", "[SubcriticalFlow1d]" )
{
    tsunami_lab::setups::SubcriticalFlow1d l_subcriticalFlow;

    // out of range
    REQUIRE( l_subcriticalFlow.getHeight( 2, 0 ) == 2.0f );

    REQUIRE( l_subcriticalFlow.getMomentumX( 2, 0 ) == 4.42f );

    REQUIRE( l_subcriticalFlow.getMomentumY( 2, 0 ) == 0.0f );

    REQUIRE( l_subcriticalFlow.getBathymetry( 2, 0 ) == -2.0f );

    REQUIRE( l_subcriticalFlow.getHeight( 2, 5 ) == 2.0f );

    REQUIRE( l_subcriticalFlow.getMomentumX( 2, 5 ) == 4.42f );

    REQUIRE( l_subcriticalFlow.getMomentumY( 2, 2 ) == 0.0f );

    REQUIRE( l_subcriticalFlow.getBathymetry( 2, 2 ) == -2.0f );

    REQUIRE( l_subcriticalFlow.getHeight( 20, 0 ) == 2.0f );

    REQUIRE( l_subcriticalFlow.getMomentumX( 20, 0 ) == 4.42f );

    REQUIRE( l_subcriticalFlow.getMomentumY( 20, 0 ) == 0.0f );

    REQUIRE( l_subcriticalFlow.getBathymetry( 20, 0 ) == -2.0f );

    REQUIRE( l_subcriticalFlow.getHeight( 20, 5 ) == 2.0f );

    REQUIRE( l_subcriticalFlow.getMomentumX( 20, 5 ) == 4.42f );

    REQUIRE( l_subcriticalFlow.getMomentumY( 20, 2 ) == 0.0f );

    REQUIRE( l_subcriticalFlow.getBathymetry( 20, 2 ) == -2.0f );


    // in range (8, 12)
    REQUIRE( l_subcriticalFlow.getHeight( 10, 0 ) == 1.8f );

    REQUIRE( l_subcriticalFlow.getMomentumX( 10, 0 ) == 4.42f );

    REQUIRE( l_subcriticalFlow.getMomentumY( 10, 0 ) == 0.0f );

    REQUIRE( l_subcriticalFlow.getBathymetry( 10, 0 ) == -1.8f );

    REQUIRE( l_subcriticalFlow.getHeight( 10, 5 ) == 1.8f );

    REQUIRE( l_subcriticalFlow.getMomentumX( 10, 5 ) == 4.42f );

    REQUIRE( l_subcriticalFlow.getMomentumY( 10, 2 ) == 0.0f );

    REQUIRE( l_subcriticalFlow.getBathymetry( 10, 2 ) == -1.8f );
}