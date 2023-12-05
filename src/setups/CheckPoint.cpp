#include "../../include/setups/CheckPoint.h"
#include "../../include/io/NetCdf.h"
#include <sstream>
#include <vector>
#include <iterator>


const char* tsunami_lab::setups::Checkpoint::variables[8]{ "totalHeight", "bathymetry", "momentumX", "momentumY", "time", "checkpoint", "writeCount", "hMax" };

tsunami_lab::setups::Checkpoint::Checkpoint( const char* filepath,
                                             t_real scale_x,
                                             t_real scale_y,
                                             t_idx& writeCount,
                                             t_real& simulationTime,
                                             t_real& hMax,
                                             std::vector<char*>& argv )
    : scaleX( scale_x ), scaleY( scale_y )
{
    // reading the checkpoint
    tsunami_lab::io::NetCdf reader = tsunami_lab::io::NetCdf();
    reader.read( filepath, variables, data );

    // checking for warning or errors
    const char* reset = "\033[0m";
    const char* yellow = "\033[33;49m";
    const char* red = "\033[31;49m";

    if( data[4].length < 1 )
    {
        std::cout << yellow << "WARNING: Could not read simulation time because there are no values to read. Defaulting to zero." << reset << std::endl;
        simulationTime = 0;
    }
    else if( data[4].type != tsunami_lab::io::NetCdf::FLOAT )
    {
        std::cout << yellow << "WARNING: Could not read simulation time because the type is wrong. Defaulting to zero." << reset << std::endl;
        simulationTime = 0;
    }
    else
    {
        simulationTime = static_cast<float*>( data[4].array )[0];
    }

    if( data[6].length < 1 )
    {
        std::cout << yellow << "WARNING: Could not read write count because there are no values to read. Defaulting to zero." << reset << std::endl;
        writeCount = 0;
    }
    else if( data[6].type != tsunami_lab::io::NetCdf::INT )
    {
        std::cout << yellow << "WARNING: Could not read write count because the type is wrong. Defaulting to zero." << reset << std::endl;
        writeCount = 0;
    }
    else
    {
        writeCount = static_cast<int*>( data[6].array )[0];
    }

    if( data[7].length < 1 )
    {
        std::cout << yellow << "WARNING: Could not read hMax because there are no values to read. Defaulting to one." << reset << std::endl;
        hMax = 1;
    }
    else if( data[7].type != tsunami_lab::io::NetCdf::FLOAT )
    {
        std::cout << yellow << "WARNING: Could not read hMax because the type is wrong. Defaulting to one." << reset << std::endl;
        hMax = 1;
    }
    else
    {
        hMax = static_cast<float*>( data[7].array )[0];
    }

    if( data[5].length < 1 )
    {
        std::cerr << red << "ERROR: Could not read checkpoint because there are no values to read. Aborting!" << reset << std::endl;
        exit( EXIT_FAILURE );
    }
    else if( data[5].type != tsunami_lab::io::NetCdf::CHAR )
    {
        std::cerr << red << "WARNING: Could not read checkpoint because the type is wrong. Aborting!" << reset << std::endl;
        exit( EXIT_FAILURE );
    }
    else
    {
        char* text = static_cast<char*>( data[5].array );
        size_t wordStart = 0;
        size_t wordLength = 0;
        for( size_t i = 0; i < data[5].length; i++ )
        {
            if( text[i] == ' ' )
            {
                text[i] = '\0';
                if( wordLength == 0 ) // skip double spaces
                {
                    wordStart++;
                    continue;
                }
                argv.push_back( &text[wordStart] );
                wordStart = i + 1;
                wordLength = 0;
                continue;
            }
            wordLength++;
        }
        argv.push_back( &text[wordStart] );
    }
}

tsunami_lab::t_real tsunami_lab::setups::Checkpoint::getHeight( t_real indexX,
                                                                t_real indexY ) const
{
    t_idx index = indexY * data[1].stride + indexX;
    // the size of bathymetry and totalHeight is the same
    return static_cast<float*>( data[0].array )[index] - static_cast<float*>( data[1].array )[index];
}

tsunami_lab::t_real tsunami_lab::setups::Checkpoint::getMomentumX( t_real indexX,
                                                                   t_real indexY ) const
{
    t_idx index = indexY * data[1].stride + indexX;
    return static_cast<float*>( data[2].array )[index];
}

tsunami_lab::t_real tsunami_lab::setups::Checkpoint::getMomentumY( t_real indexX,
                                                                   t_real indexY ) const
{
    t_idx index = indexY * data[1].stride + indexX;
    return static_cast<float*>( data[3].array )[index];
}

tsunami_lab::t_real tsunami_lab::setups::Checkpoint::getBathymetry( t_real indexX,
                                                                    t_real indexY ) const
{
    t_idx index = indexY * data[1].stride + indexX;
    return static_cast<float*>( data[1].array )[index];
}
