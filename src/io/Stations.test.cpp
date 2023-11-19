/**
 * @author Fabian Hofer, Vincent Gerlach
 *
 * Unit tests for Stations.
 **/

#include <catch2/catch.hpp>
#include "../../include/constants.h"
#include <sstream>
#include <nlohmann/json.hpp>
#include <fstream>

#define private public
#include "../../include/io/Stations.h"
#undef public

using json = nlohmann::json;

TEST_CASE( "Test Stations json methods.", "[StationsJson]" )
{
    std::ifstream l_file( "resources/config.test.json" );
    json config;
    l_file >> config;

    REQUIRE( config["output_frequency"] == 1 );
    REQUIRE( config["stations"].size() == 5 );
    REQUIRE( config["stations"][0]["name"] == "station01" );
    REQUIRE( config["stations"][1]["x"] == 15 );
    REQUIRE( config["stations"][2]["y"] == 7 );
}

TEST_CASE( "Test Stations getOutputFrequency method.", "[StationsOutputFrequency]" )
{
    tsunami_lab::io::Stations l_station = tsunami_lab::io::Stations( 10,
                                                                     10,
                                                                     10,
                                                                     10,
                                                                     10 );
    tsunami_lab::t_real frequency = l_station.getOutputFrequency();
    REQUIRE( frequency == 1 );
}

TEST_CASE( "Test Stations write method.", "[StationsWrite]" )
{
    tsunami_lab::io::Stations l_station = tsunami_lab::io::Stations( 10,
                                                                     10,
                                                                     10,
                                                                     10,
                                                                     10 );
    tsunami_lab::t_real totalHeight[10] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    l_station.write( totalHeight );
}