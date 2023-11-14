/**
 * @author Fabian Hofer, Vincent Gerlach (AT uni-jena.de)
 *
 * @section DESCRIPTION
 * Unit test for circular dam break setup.
 **/

#include <catch2/catch.hpp>
#include "../../include/setups/CircularDamBreak2d.h"

TEST_CASE( "Test the two-dimensional circular dam break setup.", "[CircularDamBreak2d]" )
{
    tsunami_lab::setups::CircularDamBreak2d l_damBreak;

    // outside
    REQUIRE( l_damBreak.getHeight( 2, 0 ) == 5 );

    REQUIRE( l_damBreak.getMomentumX( 2, 0 ) == 0 );

    REQUIRE( l_damBreak.getMomentumY( 2, 0 ) == 0 );

    REQUIRE( l_damBreak.getHeight( 80, 50 ) == 5 );

    REQUIRE( l_damBreak.getMomentumX( 80, 50 ) == 0 );

    REQUIRE( l_damBreak.getMomentumY( 80, 50 ) == 0 );

    REQUIRE( l_damBreak.getHeight( 50, 80 ) == 5 );

    REQUIRE( l_damBreak.getMomentumX( 50, 80 ) == 0 );

    REQUIRE( l_damBreak.getMomentumY( 50, 80 ) == 0 );

    REQUIRE( l_damBreak.getHeight( 90, 90 ) == 5 );

    REQUIRE( l_damBreak.getMomentumX( 90, 90 ) == 0 );

    REQUIRE( l_damBreak.getMomentumY( 90, 90 ) == 0 );

    // inside
    REQUIRE( l_damBreak.getHeight( 50, 50 ) == 10 );

    REQUIRE( l_damBreak.getMomentumX( 50, 50 ) == 0 );

    REQUIRE( l_damBreak.getMomentumY( 50, 50 ) == 0 );

    REQUIRE( l_damBreak.getHeight( 41, 50 ) == 10 );

    REQUIRE( l_damBreak.getMomentumX( 41, 50 ) == 0 );

    REQUIRE( l_damBreak.getMomentumY( 41, 50 ) == 0 );

    REQUIRE( l_damBreak.getHeight( 50, 59 ) == 10 );

    REQUIRE( l_damBreak.getMomentumX( 50, 59 ) == 0 );

    REQUIRE( l_damBreak.getMomentumY( 50, 59 ) == 0 );
}