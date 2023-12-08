/**
 * @author Fabian Hofer, Vincent Gerlach (AT uni-jena.de)
 *
 * Unit test for checkpoint setup.
 **/

#include <catch2/catch.hpp>
#include "../../include/setups/CheckPoint.h"
#include "../../include/io/NetCdf.h"
#include <vector>

#define VALN1(X) X
#define VALN2(X) VALN1(X), VALN1(X)
#define VALN4(X) VALN2(X), VALN2(X)
#define VALN8(X) VALN4(X), VALN4(X)
#define VALN16(X) VALN8(X), VALN8(X)
#define VALN32(X) VALN16(X), VALN16(X)
#define VALN64(X) VALN32(X), VALN32(X)
#define VALN128(X) VALN64(X), VALN64(X)
#define VALN256(X) VALN128(X), VALN128(X)
#define VALN512(X) VALN256(X), VALN256(X)
#define VALN1024(X) VALN512(X), VALN512(X)
#define VALN2048(X) VALN1024(X), VALN1024(X)

TEST_CASE( "Test the write-read checkpoint setup.", "[CheckPoint]" )
{
    tsunami_lab::t_real bathymetry[5000] = { VALN2048( -21 ), VALN2048( -32 ), VALN512( -15 ), VALN256( -123 ), VALN128( -12 ), VALN8( -82 ) };
    tsunami_lab::t_real totalHeight[5000] = { VALN2048( 0 ), VALN2048( 0 ), VALN512( 45 ), VALN256( 0 ), VALN128( 0 ), VALN8( 0 ) };
    tsunami_lab::t_real momentumX[5000] = { VALN2048( 12 ), VALN2048( 23 ), VALN512( 45 ), VALN256( -5 ), VALN128( 0 ), VALN8( 0 ) };
    tsunami_lab::t_real momentumY[5000] = { VALN2048( 0 ), VALN2048( 234 ), VALN512( 9 ), VALN256( 214 ), VALN128( 7 ), VALN8( 23 ) };
    tsunami_lab::t_idx stride = 100;
    tsunami_lab::io::NetCdf* writer = new tsunami_lab::io::NetCdf( "resources/checkpoint.test.nc",
                                                                   100,
                                                                   50,
                                                                   1000,
                                                                   500,
                                                                   stride,
                                                                   bathymetry,
                                                                   "100 50 -B -t 100",
                                                                   123,
                                                                   totalHeight,
                                                                   momentumX,
                                                                   momentumY,
                                                                   20,
                                                                   5 );
    delete writer;

    tsunami_lab::t_idx writeCount;
    tsunami_lab::t_real simulationTime;
    tsunami_lab::t_real hMax;
    std::vector<char*> argv;
    tsunami_lab::setups::Checkpoint checkpoint = tsunami_lab::setups::Checkpoint( "resources/checkpoint.test.nc",
                                                                                  writeCount,
                                                                                  simulationTime,
                                                                                  hMax,
                                                                                  argv );
    REQUIRE( writeCount == 5 );

    REQUIRE( simulationTime == 20 );

    REQUIRE( hMax == 123 );

    std::string commandLine = argv[0];
    for( size_t i = 1; i < argv.size(); i++ )
    {
        commandLine += " ";
        commandLine += argv[i];
    }
    REQUIRE( commandLine == "100 50 -B -t 100" );

    for( size_t iy = 0; iy < 50; iy++ )
    {
        for( size_t ix = 0; ix < 100; ix++ )
        {
            tsunami_lab::t_idx index = iy * stride + ix;
            REQUIRE( checkpoint.getHeight( ix, iy ) == ( totalHeight[index] - bathymetry[index] ) );
            REQUIRE( checkpoint.getMomentumX( ix, iy ) == momentumX[index] );
            REQUIRE( checkpoint.getMomentumY( ix, iy ) == momentumY[index] );
            REQUIRE( checkpoint.getBathymetry( ix, iy ) == bathymetry[index] );
        }
    }
}