/**
 * @author Fabian Hofer, Vincent Gerlach (AT uni-jena.de)
 *
 * @section DESCRIPTION
 * Tests the middle_states setup.
 **/
#include <catch2/catch.hpp>
#include "../../include/setups/MiddleStates1d.h"

TEST_CASE( "Test the one-dimensional MiddleStates setup.", "[MiddleStates1d]" )
{
    tsunami_lab::setups::MiddleStates1d l_location( 25,
                                                    12,
                                                    35,
                                                    123,
                                                    3 );

    // left side
    REQUIRE( l_location.getHeight( 2, 0 ) == 25 );

    REQUIRE( l_location.getMomentumX( 2, 0 ) == 35 );

    REQUIRE( l_location.getMomentumY( 2, 0 ) == 0 );

    REQUIRE( l_location.getHeight( 2, 5 ) == 25 );

    REQUIRE( l_location.getMomentumX( 2, 5 ) == 35 );

    REQUIRE( l_location.getMomentumY( 2, 2 ) == 0 );

    // right side
    REQUIRE( l_location.getHeight( 4, 0 ) == 12 );

    REQUIRE( l_location.getMomentumX( 4, 0 ) == 123 );

    REQUIRE( l_location.getMomentumY( 4, 0 ) == 0 );

    REQUIRE( l_location.getHeight( 4, 5 ) == 12 );

    REQUIRE( l_location.getMomentumX( 4, 5 ) == 123 );

    REQUIRE( l_location.getMomentumY( 4, 2 ) == 0 );
}