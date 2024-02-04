/**
 * @author Fabian Hofer, Vincent Gerlach (AT uni-jena.de)
 *
 * Unit test for circular dam break setup.
 **/

#include <catch2/catch.hpp>
#include "../../include/setups/TsunamiEvent2d.h"

TEST_CASE( "Test the two-dimensional tsunami event setup.", "[TsunamiEvent2d]" )
{

    const char* variables[3]{ "x", "y", "z" };
    tsunami_lab::setups::TsunamiEvent2d l_tsunami( "resources/artificialtsunami_bathymetry_1000.nc",
                                                   variables,
                                                   "resources/artificialtsunami_displ_1000.nc",
                                                   variables,
                                                   10000,
                                                   10000 );

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

    // 5000 is not available find closest which is 5005
    REQUIRE( l_tsunami.getHeight( 5000, 5000 ) == 100 );

    REQUIRE( l_tsunami.getMomentumX( 5000, 5000 ) == 0 );

    REQUIRE( l_tsunami.getMomentumY( 5000, 5000 ) == 0 );

    REQUIRE( l_tsunami.getBathymetry( 5000, 5000 ) == Approx( -100.15704 ) );

    REQUIRE( l_tsunami.getBathymetry( 5005, 5005 ) == l_tsunami.getBathymetry( 5000, 5000 ) );

    // 4750 is not available find closest which is 4755
    REQUIRE( l_tsunami.getHeight( 4750, 4750 ) == 100 );

    REQUIRE( l_tsunami.getMomentumX( 4750, 4750 ) == 0 );

    REQUIRE( l_tsunami.getMomentumY( 4750, 4750 ) == 0 );

    REQUIRE( l_tsunami.getBathymetry( 4750, 4750 ) == Approx( -96.20238 ) );

    REQUIRE( l_tsunami.getBathymetry( 4755, 4755 ) == l_tsunami.getBathymetry( 4750, 4750 ) );

    // 5300/5200 is not available find closest which is 5295/5195
    REQUIRE( l_tsunami.getHeight( 5300, 5200 ) == 100 );

    REQUIRE( l_tsunami.getMomentumX( 5300, 5200 ) == 0 );

    REQUIRE( l_tsunami.getMomentumY( 5300, 5200 ) == 0 );

    REQUIRE( l_tsunami.getBathymetry( 5300, 5200 ) == Approx( -104.07117 ) );

    REQUIRE( l_tsunami.getBathymetry( 5295, 5195 ) == l_tsunami.getBathymetry( 5300, 5200 ) );

}