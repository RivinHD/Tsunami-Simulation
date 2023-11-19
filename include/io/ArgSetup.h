/**
 * @author Fabian Hofer, Vincent Gerlach (AT uni-jena.de)
 *
 * A small class for better handling of arguments
 **/
#ifndef TSUNAMISIMULATION_ARGSETUP
#define TSUNAMISIMULATION_ARGSETUP

#include <vector>
#include <stdexcept>
#include <sstream>

#define START_ARG_CHAR '!'
#define END_ARG_CHAR '~'

 /**
 * Setup for better handling of arguments.
 */
class ArgSetup
{
public:
    //! Length of the possible flag array/map
    const static int LENGTH_ARG_CHAR = END_ARG_CHAR - START_ARG_CHAR + 1;

    //! the character to use for the flag
    unsigned char flag;

    //! the number of options E.g. is 2 when -f Option1 Option2
    short numberOfOptions;

    //! the maximum number of options that one flag can have
    short maxNumberOfOptions;

    /**
     * Generate a new Argument with the given flag and and the number of parameter
     *
     * @param flag the flag to call this argument with
     * @param numberOfOptions the number of options that one flag handles. Options are separated by a space. E.g. -f Option1 Option2 ...
    */
    ArgSetup( const char& flag, short numberOfOptions, short maxNumberOfOptions )
        : flag( flag ), numberOfOptions( numberOfOptions ), maxNumberOfOptions( maxNumberOfOptions )
    {
        if( flag < START_ARG_CHAR && flag > END_ARG_CHAR )
        {
            throw std::invalid_argument( "The flag is out of possible flag ranges.\nCheck START_ARG_CHAR and END_ARG_CHAR for the possible range" );
        }
    }

    /**
     * Get the total space required for all arguments, including the flag and the number of options/parameter
     *
     * @param argList the list containing all the arguments that are supported by the application
     * @return sum of all flags and there options
    */
    static int getArgumentsLength( const std::vector<ArgSetup>& argList )
    {
        int count = 0;
        for( std::vector<ArgSetup>::const_iterator it = argList.begin(); it != argList.end(); it++ )
        {
            count += it->maxNumberOfOptions + 1;
        }
        return count;
    }

    /**
     * Generate a map were the number of Options can be rapidly retrieve by a array call
     *
     * @param argList the list containing all the arguments that are supported by the application
     * @param outMap a mapping to a simple array were the element at postion 'flag - START_ARG_CHAR' contains the number of options
    */
    static void generateCountMap( const std::vector<ArgSetup>& argList, int outMap[LENGTH_ARG_CHAR] )
    {
        for( std::vector<ArgSetup>::const_iterator it = argList.begin(); it != argList.end(); it++ )
        {
            outMap[( it->flag ) - START_ARG_CHAR] = it->numberOfOptions;
        }
    }
};
#endif // TSUNAMISIMULATION_ARGSETUP
