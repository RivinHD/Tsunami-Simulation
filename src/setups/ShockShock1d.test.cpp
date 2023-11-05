/**
 * @author Fabian Hofer, Vincent Gerlach (AT uni-jena.de)
 *
 * @section DESCRIPTION
 * Tests the shock-shock setup.
 **/
#include "../../include/setups/ShockShock1d.h"
#include <catch2/catch.hpp>

TEST_CASE("Test the one-dimensional shock-shock setup.", "[ShockShock1d]")
{
    tsunami_lab::setups::ShockShock1d l_shockShock(25,
                                                   35,
                                                   3);

    // left side
    REQUIRE(l_shockShock.getHeight(2, 0) == 25);

    REQUIRE(l_shockShock.getMomentumX(2, 0) == 35);

    REQUIRE(l_shockShock.getMomentumY(2, 0) == 0);

    REQUIRE(l_shockShock.getHeight(2, 5) == 25);

    REQUIRE(l_shockShock.getMomentumX(2, 5) == 35);

    REQUIRE(l_shockShock.getMomentumY(2, 2) == 0);

    // right side
    REQUIRE(l_shockShock.getHeight(4, 0) == 25);

    REQUIRE(l_shockShock.getMomentumX(4, 0) == -35);

    REQUIRE(l_shockShock.getMomentumY(4, 0) == 0);

    REQUIRE(l_shockShock.getHeight(4, 5) == 25);

    REQUIRE(l_shockShock.getMomentumX(4, 5) == -35);

    REQUIRE(l_shockShock.getMomentumY(4, 2) == 0);
}