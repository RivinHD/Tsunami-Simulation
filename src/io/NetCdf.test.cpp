/**
 * @author Fabian Hofer, Vincent Gerlach
 *
 * Unit tests for NetCDF read and write.
 **/


#include <catch2/catch.hpp>
#include "../../include/constants.h"


#define private public
#include "../../include/io/NetCdf.h"
#undef public

TEST_CASE( "The the read method for netCDF", "[netCDF]" )
{
    tsunami_lab::io::NetCdf* reader = new tsunami_lab::io::NetCdf();
    const char* variables[2] = { "b", "h" };
    tsunami_lab::io::NetCdf::VarArray data[2];
    tsunami_lab::io::NetCdf::VarArray singleDataArr[1];

    SECTION( "Run Tests" )
    {
        REQUIRE( reader->isReadMode );

        reader->read( "resources/ReadNetCDF.test.nc", variables, data );

        REQUIRE( reader->readDataArrays.size() == 2 );

        // Check the data for "b"
        REQUIRE( data[0].length == 24 );
        REQUIRE( data[0].stride == 3 );
        REQUIRE( data[0].type == tsunami_lab::io::NetCdf::VarType::FLOAT );
        const float b[] = { -2,  -3,  -5,
                            -7, -11, -13,
                           -17, -19, -23,
                           -29, -31, -37,
                           -41, -43, -47,
                           -53, -59, -61,
                           -67, -71, -73,
                           -79, -83, -89 };

        float* bData = static_cast<float*>( data[0].array );
        for( size_t i = 0; i < 24; i++ )
        {
            REQUIRE( bData[i] == b[i] );
        }

        // Check the data for "h"
        REQUIRE( data[1].length == 24 );
        REQUIRE( data[1].stride == 3 );
        REQUIRE( data[1].type == tsunami_lab::io::NetCdf::VarType::FLOAT );
        const float h0[] = { 2,  3,  5,
                             7, 11, 13,
                            17, 19, 23,
                            29, 31, 37,
                            41, 43, 47,
                            53, 59, 61,
                            67, 71, 73,
                            79, 83, 89 };

        float* hData = static_cast<float*>( data[1].array );
        for( size_t i = 0; i < 24; i++ )
        {
            REQUIRE( hData[i] == h0[i] );
        }

        // Read with timeStep of 1
        reader->read( "resources/ReadNetCDF.test.nc", variables, data, 1 );

        REQUIRE( reader->readDataArrays.size() == 4 );

        // Check the data for "b"
        REQUIRE( data[0].length == 24 );
        REQUIRE( data[0].stride == 3 );
        REQUIRE( data[0].type == tsunami_lab::io::NetCdf::VarType::FLOAT );

        bData = static_cast<float*>( data[0].array );
        for( size_t i = 0; i < 24; i++ )
        {
            REQUIRE( bData[i] == b[i] );
        }

        // Check the data for "h"
        REQUIRE( data[1].length == 24 );
        REQUIRE( data[1].stride == 3 );
        REQUIRE( data[1].type == tsunami_lab::io::NetCdf::VarType::FLOAT );
        const float h1[] = { 2,    3,    5,
                             7,   11,   13,
                            17,   19,   23,
                            29,   31,   37,
                          41.5, 43.5, 47.5,
                          53.5,   60, 61.5,
                          67.5, 71.5, 73.5,
                            79,   83,   89 };

        hData = static_cast<float*>( data[1].array );
        for( size_t i = 0; i < 24; i++ )
        {
            REQUIRE( hData[i] == h1[i] );
        }

        // test the simple read method with one variable and a timeStep of 2
        reader->read( "resources/ReadNetCDF.test.nc", "h", singleDataArr, 2 );

        REQUIRE( reader->readDataArrays.size() == 5 );

        tsunami_lab::io::NetCdf::VarArray& singleData = singleDataArr[0];
        REQUIRE( singleData.length == 24 );
        REQUIRE( singleData.stride == 3 );
        REQUIRE( singleData.type == tsunami_lab::io::NetCdf::VarType::FLOAT );
        const float h2[] = { 2,     3,     5,
                             7,    11,    13,
                            17,    19,    23,
                         29.05,  31.1, 37.05,
                         41.25, 43.25, 47.25,
                         53.25,  60.5, 61.25,
                         67.25, 71.25, 73.25,
                         79.05,  83.1, 89.05 };
        hData = static_cast<float*>( singleData.array );
        for( size_t i = 0; i < 24; i++ )
        {
            REQUIRE( hData[i] == h2[i] );
        }
    }

    // free memory
    delete reader;

    REQUIRE( data[0].array == nullptr );
    REQUIRE( data[1].array == nullptr );
    REQUIRE( singleDataArr[0].array == nullptr );
}