/**
 * @author Alexander Breuer (alex.breuer AT uni-jena.de), Fabian Hofer, Vincent Gerlach
 *
 * @section DESCRIPTION
 * Unit tests for the CSV-interface.
 **/
#include <catch2/catch.hpp>
#include "../../include/constants.h"
#include <iostream>
#include <nlohmann/json.hpp>
#define private public
#include "../../include/io/Csv.h"
#include <sstream>
#undef public

TEST_CASE( "Test the CSV-writer for 1D settings.", "[CsvWrite1d]" )
{
    // define a simple example
    tsunami_lab::t_real l_h[7] = { 0, 1, 2, 3, 4, 5, 6 };
    tsunami_lab::t_real l_hu[7] = { 6, 5, 4, 3, 2, 1, 0 };

    std::stringstream l_stream0;
    tsunami_lab::io::Csv::write( 0.5,
                                 5,
                                 1,
                                 7,
                                 l_h + 1,
                                 l_hu + 1,
                                 nullptr,
                                 nullptr,
                                 nullptr,
                                 l_stream0 );

    std::string l_ref0 = R"V0G0N(x,y,height,momentum_x
0.25,0.25,1,5
0.75,0.25,2,4
1.25,0.25,3,3
1.75,0.25,4,2
2.25,0.25,5,1
)V0G0N";

    REQUIRE( l_stream0.str().size() == l_ref0.size() );
    REQUIRE( l_stream0.str() == l_ref0 );
}

TEST_CASE( "Test the CSV-writer for 2D settings.", "[CsvWrite2d]" )
{
    // define a simple example
    tsunami_lab::t_real l_h[16] = { 0,  1,  2,  3,
                                      4,  5,  6,  7,
                                      8,  9, 10, 11,
                                     12, 13, 14, 15 };
    tsunami_lab::t_real l_hu[16] = { 15, 14, 13, 12,
                                     11, 10,  9,  8,
                                      7,  6,  5,  4,
                                      3,  2,  1,  0 };
    tsunami_lab::t_real l_hv[16] = { 0,  4,  8, 12,
                                      1,  5,  9, 13,
                                      2,  6, 10, 14,
                                      3,  7, 11, 15 };

    std::stringstream l_stream1;
    tsunami_lab::io::Csv::write( 10,
                                 2,
                                 2,
                                 4,
                                 l_h + 4 + 1,
                                 l_hu + 4 + 1,
                                 l_hv + 4 + 1,
                                 nullptr,
                                 nullptr,
                                 l_stream1 );

    std::string l_ref1 = R"V0G0N(x,y,height,momentum_x,momentum_y
5,5,5,10,5
15,5,6,9,9
5,15,9,6,6
15,15,10,5,10
)V0G0N";

    REQUIRE( l_stream1.str().size() == l_ref1.size() );
    REQUIRE( l_stream1.str() == l_ref1 );
}

TEST_CASE( "Test the CSV-reader for middle states", "[CsvRead]" )
{
    std::ifstream middle_states( "resources/middle_states.csv" );
    tsunami_lab::t_real hLeft, hRight, huLeft, huRight, hStar;
    bool success;
    unsigned int linesCount = 0;

    // first successful line of middle states
    success = tsunami_lab::io::Csv::next_middle_states( middle_states,
                                                        hLeft,
                                                        hRight,
                                                        huLeft,
                                                        huRight,
                                                        hStar );
    linesCount += success;

    REQUIRE( success );
    REQUIRE( hLeft == 8899.326826472694f );
    REQUIRE( hRight == 8899.326826472694f );
    REQUIRE( huLeft == 122.0337839252433f );
    REQUIRE( huRight == -122.0337839252433f );
    REQUIRE( hStar == 8899.739847378269f );

    while( success = tsunami_lab::io::Csv::next_middle_states( middle_states,
                                                               hLeft,
                                                               hRight,
                                                               huLeft,
                                                               huRight,
                                                               hStar ) )
    {
        linesCount += success;
        if( linesCount == 500001 )
        {
            REQUIRE( success );
            REQUIRE( hLeft == 7974.350705934805f );
            REQUIRE( hRight == 8378.341124205988f );
            REQUIRE( huLeft == -185.7324996463998f );
            REQUIRE( huRight == -322.8256083091958f );
            REQUIRE( hStar == 8175.312258280472f );
        }
    }

    REQUIRE( linesCount == 1000000 );
}

TEST_CASE( "Test the Bathymetry-reader for bathy_profile.csv", "[CsvRead]" )
{
    std::ifstream bathy_profile( "resources/bathy_profile.csv" );
    tsunami_lab::t_real hBathy;
    bool success;
    unsigned int linesCount = 0;

    // first successful line of middle states
    success = tsunami_lab::io::Csv::readBathymetry( bathy_profile, hBathy );
    linesCount += success;

    REQUIRE( success );
    REQUIRE( hBathy == 14.7254650696f );

    while( success = tsunami_lab::io::Csv::readBathymetry( bathy_profile, hBathy ) )
    {
        linesCount += success;
        if( linesCount == 900 )
        {
            REQUIRE( success );
            REQUIRE( hBathy == -7260.18122445f );

        }
    }
    REQUIRE( linesCount == 1763 );
}