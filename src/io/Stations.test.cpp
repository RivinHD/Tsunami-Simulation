/**
 * @author Fabian Hofer, Vincent Gerlach
 *
 * @section DESCRIPTION
 * Unit tests for Stations.
 **/

#include <catch2/catch.hpp>
#include "../../include/constants.h"
#include <sstream>
#include <../../../submodules/json/single_include/nlohmann/json.hpp>
#define private public
#include "../../include/io/Csv.h"
#undef public

using json = nlohmann::json;

TEST_CASE( "Test Stations json methods.", "[StationsJson]" )
{
    std::ifstream l_file( "resources/config.json" );
    json config;
    l_file >> config;

    REQUIRE( config["output_frequency"] == 1 );
    REQUIRE( config["stations"].size() == 3 );
    REQUIRE( config["stations"][0]["name"] == "station01" );
    REQUIRE( config["stations"][1]["x"] == 15 );
    REQUIRE( config["stations"][2]["y"] == 7 );
}

TEST_CASE( "Test Stations write method.", "[StationsWrite]" ){

}