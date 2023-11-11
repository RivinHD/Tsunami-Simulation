/**
 * @author Fabian Hofer, Vincent Gerlach (AT uni-jena.de)
 *
 * @section DESCRIPTION
 * Tests the TsunamiEvent1d setup.
 **/
#include <catch2/catch.hpp>
#include "../../include/setups/TsunamiEvent1d.h"

TEST_CASE( "Test the one-dimensional tsunami-event setup.", "[RareRare1d]" )
{
    tsunami_lab::setups::TsunamiEvent1d l_tsunamiEvent( "resources/bathy_profile.csv",
                                                        20,
                                                        1763 );

    REQUIRE( l_tsunamiEvent.getHeight( 2, 0 ) == 20 );

    REQUIRE( l_tsunamiEvent.getMomentumX( 2, 0 ) == 0 );

    REQUIRE( l_tsunamiEvent.getMomentumY( 2, 0 ) == 0 );

    REQUIRE( l_tsunamiEvent.getVerticalDisplacement( 2, 0 ) == 0 );

    REQUIRE( l_tsunamiEvent.getBathymetry( 2, 0 ) == -20 );

    REQUIRE( l_tsunamiEvent.getHeight( 19, 0 ) == 26.3175566727f );

    REQUIRE( l_tsunamiEvent.getMomentumX( 19, 0 ) == 0 );

    REQUIRE( l_tsunamiEvent.getMomentumY( 19, 0 ) == 0 );

    REQUIRE( l_tsunamiEvent.getVerticalDisplacement( 19, 0 ) == 0 );

    REQUIRE( l_tsunamiEvent.getBathymetry( 19, 0 ) == -26.3175566727f );

    REQUIRE( l_tsunamiEvent.getVerticalDisplacement( 200000, 0 ) == Approx( -8.660254038f ) );
}