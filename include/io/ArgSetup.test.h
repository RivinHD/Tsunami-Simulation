/**
 * @author Fabian Hofer, Vincent Gerlach (AT uni-jena.de)
 *
 * @section DESCRIPTION
 * The unit test for the ArgSetup
 **/
#include <catch2/catch.hpp>
#include "ArgSetup.h"

TEST_CASE( "Test ArgSetup", "[ArgSetup]" )
{
	const std::vector<ArgSetup> argumentList{
		ArgSetup( 'a', 0 ),
		ArgSetup( 'b', 1 ),
		ArgSetup( 'c', 10 ),
		ArgSetup( '~', 3 )
	};

	REQUIRE( ArgSetup::getArgumentsLength( argumentList ) == 14 );


	int outMap[94] = { -1 };
	ArgSetup::generateCountMap( argumentList, outMap );

	REQUIRE( outMap['a'] == 0 );
	REQUIRE( outMap['b'] == 1 );
	REQUIRE( outMap['c'] == 10 );
	REQUIRE( outMap['~'] == 3 );
	REQUIRE( outMap[0] == -1 );


	try
	{
		// this call should throw an error because < START_ARG_CHAR
		const std::vector<ArgSetup> errorArgumentList{
			ArgSetup( 0 , 0 ),
		};

		REQUIRE( false );
	}
	catch( const std::invalid_argument& err )
	{
		REQUIRE( true );
	}

	try
	{
		// this call should throw an error because > END_ARG_CHAR
		const std::vector<ArgSetup> errorArgumentList{
			ArgSetup( 255 , 0 ),
		};

		REQUIRE( false );
	}
	catch( const std::invalid_argument& err )
	{
		REQUIRE( true );
	}
}