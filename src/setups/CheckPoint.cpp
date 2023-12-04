#include "../../include/setups/CheckPoint.h"
#include "../../include/io/NetCdf.h"
#include <sstream>
#include <vector>
#include <iterator>


const char* tsunami_lab::setups::Checkpoint::variables[7]{ "totalHeight", "bathymetry", "momentumX", "momentumY", "time", "checkpoint", "timeStep" };

tsunami_lab::setups::Checkpoint::Checkpoint( const char* filepath,
                                             t_real scale_x,
                                             t_real scale_y,
                                             t_idx& timeStep,
                                             t_real& simulationTime,
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
    else
    {
        simulationTime = static_cast<float*>( data[4].array )[data[4].length - 1];
    }
    if( data[6].length < 1 )
    {
        std::cout << yellow << "WARNING: Could not read time step because there are no values to read. Defaulting to zero." << reset << std::endl;
        timeStep = 0;
    }
    else
    {
        timeStep = static_cast<int*>( data[6].array )[data[6].length - 1];
    }
    if( data[5].length < 1 )
    {
        std::cerr << red << "ERROR: Could not read checkpoint because there are no values to read. Aborting!" << reset << std::endl;
        exit( EXIT_FAILURE );
    }
    else
    {
        std::istringstream buffer( static_cast<char**>( data[5].array )[0] );
        argv = std::vector<char*>{ std::istream_iterator<char*>( buffer ), std::istream_iterator<char*>() };
    }
}

tsunami_lab::t_real tsunami_lab::setups::Checkpoint::getHeight( t_real i_x,
                                                                t_real i_y ) const
{
    // the size of bathymetry and totalHeight is the same
    t_idx index_x = i_x / scaleX * data[0].stride;
    t_idx index_y = i_y / scaleY * data[0].length / data[0].stride;
    t_idx index = index_y * data[0].stride + index_x;

    return static_cast<float*>( data[0].array )[index] - static_cast<float*>( data[1].array )[index];
}

tsunami_lab::t_real tsunami_lab::setups::Checkpoint::getMomentumX( t_real i_x,
                                                                   t_real i_y ) const
{
    t_idx index_x = i_x / scaleX * data[2].stride;
    t_idx index_y = i_y / scaleY * data[2].length / data[2].stride;
    t_idx index = index_y * data[2].stride + index_x;

    return static_cast<float*>( data[2].array )[index];
}

tsunami_lab::t_real tsunami_lab::setups::Checkpoint::getMomentumY( t_real i_x,
                                                                   t_real i_y ) const
{
    t_idx index_x = i_x / scaleX * data[3].stride;
    t_idx index_y = i_y / scaleY * data[3].length / data[3].stride;
    t_idx index = index_y * data[3].stride + index_x;

    return static_cast<float*>( data[3].array )[index];
}

tsunami_lab::t_real tsunami_lab::setups::Checkpoint::getBathymetry( t_real i_x,
                                                                    t_real i_y ) const
{
    t_idx index_x = i_x / scaleX * data[1].stride;
    t_idx index_y = i_y / scaleY * data[1].length / data[1].stride;
    t_idx index = index_y * data[1].stride + index_x;

    return static_cast<float*>( data[1].array )[index];
}
