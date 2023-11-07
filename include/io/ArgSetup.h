/**
 * @author Fabian Hofer, Vincent Gerlach (AT uni-jena.de)
 *
 * @section DESCRIPTION
 * The f-wave solver for solving the Initial Value Problem (IVP) for the shallow water equations.
 **/
#ifndef TSUNAMISIMULATION_ARGSETUP
#define TSUNAMISIMULATION_ARGSETUP

#include <vector>

#define START_ARG_CHAR '!'
#define END_ARG_CHAR '~'

class ArgSetup
{
public:
	const static int LENGTH_ARG_CHAR = END_ARG_CHAR - START_ARG_CHAR + 1;
	unsigned char flag;
	short numberOfOptions;

	ArgSetup( const char& flag, short numberOfOptions )
		: flag( flag ), numberOfOptions( numberOfOptions )
	{
	}

	static int getOptionalArgLength( const std::vector<ArgSetup>& argList )
	{
		int count = 0;
		for( std::vector<ArgSetup>::const_iterator it = argList.begin(); it != argList.end(); it++ )
		{
			count += it->numberOfOptions + 1;
		}
		return count;
	}

	static void generateCountMap( const std::vector<ArgSetup>& argList, int outMap[LENGTH_ARG_CHAR] )
	{
		for( std::vector<ArgSetup>::const_iterator it = argList.begin(); it != argList.end(); it++ )
		{
			outMap[+( it->flag )] = it->numberOfOptions;
		}
	}
};
#endif // TSUNAMISIMULATION_ARGSETUP
