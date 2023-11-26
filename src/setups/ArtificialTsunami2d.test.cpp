/**
 * @author Fabian Hofer, Vincent Gerlach (AT uni-jena.de)
 *
 * Unit test for circular dam break setup.
 **/

#include <catch2/catch.hpp>
#include "../../include/setups/ArtificialTsunami2d.h"

TEST_CASE( "Test the two-dimensional artificial tsunami setup.", "[ArtificialTsunami2d]" )
{
    tsunami_lab::setups::ArtificialTsunami2d l_tsunami;

    // outside of displacement
    REQUIRE( l_tsunami.getHeight( 2, 0 ) == 100 );

    REQUIRE( l_tsunami.getMomentumX( 2, 0 ) == 0 );

    REQUIRE( l_tsunami.getMomentumY( 2, 0 ) == 0 );

    REQUIRE( l_tsunami.getBathymetry( 2, 0 ) == -100 );

    REQUIRE( l_tsunami.getHeight( 8000, 5000 ) == 100 );

    REQUIRE( l_tsunami.getMomentumX( 8000, 5000 ) == 0 );

    REQUIRE( l_tsunami.getMomentumY( 8000, 5000 ) == 0 );

    REQUIRE( l_tsunami.getBathymetry( 8000, 5000 ) == -100 );

    REQUIRE( l_tsunami.getHeight( 5000, 8000 ) == 100 );

    REQUIRE( l_tsunami.getMomentumX( 5000, 8000 ) == 0 );

    REQUIRE( l_tsunami.getMomentumY( 5000, 8000 ) == 0 );

    REQUIRE( l_tsunami.getBathymetry( 5000, 8000 ) == -100 );

    REQUIRE( l_tsunami.getHeight( 9000, 9000 ) == 100 );

    REQUIRE( l_tsunami.getMomentumX( 9000, 9000 ) == 0 );

    REQUIRE( l_tsunami.getMomentumY( 9000, 9000 ) == 0 );

    REQUIRE( l_tsunami.getBathymetry( 9000, 9000 ) == -100 );

    // inside of displacement
    REQUIRE( l_tsunami.getHeight( 5000, 5000 ) == 100 );

    REQUIRE( l_tsunami.getMomentumX( 5000, 5000 ) == 0 );

    REQUIRE( l_tsunami.getMomentumY( 5000, 5000 ) == 0 );

    REQUIRE( l_tsunami.getBathymetry( 5000, 5000 ) == -100 );

    REQUIRE( l_tsunami.getHeight( 4750, 4750 ) == 100 );

    REQUIRE( l_tsunami.getMomentumX( 4750, 4750 ) == 0 );

    REQUIRE( l_tsunami.getMomentumY( 4750, 4750 ) == 0 );

    REQUIRE( l_tsunami.getBathymetry( 4750, 4750 ) == Approx( -96.25 ) );

    REQUIRE( l_tsunami.getHeight( 5300, 5200 ) == 100 );

    REQUIRE( l_tsunami.getMomentumX( 5300, 5200 ) == 0 );

    REQUIRE( l_tsunami.getMomentumY( 5300, 5200 ) == 0 );

    REQUIRE( l_tsunami.getBathymetry( 5300, 5200 ) == Approx( -103.9944373694 ) );
}