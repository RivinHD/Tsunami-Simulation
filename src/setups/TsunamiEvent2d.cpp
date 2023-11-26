#include "../../include/setups/TsunamiEvent2d.h"
#include <cmath>

tsunami_lab::t_real tsunami_lab::setups::TsunamiEvent2d::getValueAscending( const t_real* const data[3],
                                                                            const t_idx size[3],
                                                                            const t_idx zStride,
                                                                            const t_real x,
                                                                            const t_real y ) const
{
    // Because the x dimension and y dimension are sorted (ascending) the current x and y position can be found using a binary search (lower bound)
    const t_real* xBegin = data[0];
    const t_real* xEnd = xBegin + size[0];
    const t_real* yBegin = data[1];
    const t_real* yEnd = yBegin + size[1];
    const t_real* xHigh = std::lower_bound( xBegin, xEnd, x );
    const t_real* yHigh = std::lower_bound( yBegin, yEnd, y );
    const t_real* xLower = xHigh - 1; // can be done because array is sorted ascending
    const t_real* yLower = yHigh - 1;

    if( xHigh == xEnd || yHigh == yEnd )
    {
        std::cerr << "WARNING: Could not found lower bound. Defaulting to zero" << std::endl;
        return 0;
    }

    // calculate the index to get the data
    t_idx xIndex;
    t_idx yIndex;
    if( xHigh >= xEnd )
    {
        xIndex = xLower - xBegin;
    }
    else
    {
        xIndex = ( std::abs( *xLower - x ) < std::abs( *xHigh - x ) ? xLower : xHigh ) - xBegin;
    }
    if( yHigh >= yEnd )
    {
        yIndex = xLower - xBegin;
    }
    else
    {
        yIndex = ( std::abs( *yLower - y ) < std::abs( *yHigh - y ) ? yLower : yHigh ) - yBegin;
    }
    const t_idx index = yIndex * zStride + xIndex;

    if( index >= size[2] )
    {
        std::cerr << "WARNING: Index out of range. Defaulting to zero" << std::endl;
        return 0;
    }

    return data[2][index];
}

tsunami_lab::setups::TsunamiEvent2d::TsunamiEvent2d( const char* bathymetryFilePath,
                                                     const char* ( &bathymetryVariable )[3],
                                                     const char* displacementFilePath,
                                                     const char* ( &displacementVariable )[3],
                                                     t_real scaleX,
                                                     t_real scaleY,
                                                     t_real delta )
    : scaleX( scaleX ), scaleY( scaleY ), delta( delta )
{
    tsunami_lab::io::NetCdf reader = tsunami_lab::io::NetCdf();

    // read & check the bathymetry data
    reader.read( bathymetryFilePath, bathymetryVariable, bathymetryData );

    if( bathymetryData[0].type != tsunami_lab::io::NetCdf::FLOAT
        && bathymetryData[1].type != tsunami_lab::io::NetCdf::FLOAT
        && bathymetryData[2].type != tsunami_lab::io::NetCdf::FLOAT )
    {
        std::cerr << "The read data for bathymetry is not of type float" << std::endl;
        exit( 2 );
    }

    // read & check the displacement data
    reader.read( displacementFilePath, displacementVariable, displacementData );

    if( displacementData[0].type != tsunami_lab::io::NetCdf::FLOAT
        && displacementData[1].type != tsunami_lab::io::NetCdf::FLOAT
        && displacementData[2].type != tsunami_lab::io::NetCdf::FLOAT )
    {
        std::cerr << "The read data for displacement is not of type float" << std::endl;
        exit( 2 );
    }

    // assign the data to bathymetry and displacement
    for( size_t i = 0; i < 3; i++ )
    {
        bathymetry[i] = static_cast<float*>( bathymetryData[i].array );
        bathymetrySize[i] = bathymetryData[i].length;
        displacement[i] = static_cast<float*>( displacementData[i].array );
        displacementSize[i] = displacementData[i].length;
    }
}

tsunami_lab::t_real tsunami_lab::setups::TsunamiEvent2d::getHeight( t_real i_x,
                                                                    t_real i_y ) const
{
    // calculate the x coordinate scaled to the bathymetry coordinate system.
    t_real x = i_x / scaleX * std::abs( bathymetry[0][0] - bathymetry[0][bathymetrySize[0] - 1] ) + bathymetry[0][0];
    t_real y = i_y / scaleY * std::abs( bathymetry[1][0] - bathymetry[1][bathymetrySize[1] - 1] ) + bathymetry[1][0];

    // obtain the closest value
    t_real b = getValueAscending( bathymetry, bathymetrySize, bathymetryData[2].stride, x, y );
    return ( b < 0 ) * std::max( -b, delta );
}

tsunami_lab::t_real tsunami_lab::setups::TsunamiEvent2d::getMomentumX( t_real,
                                                                       t_real ) const
{
    return 0;
}

tsunami_lab::t_real tsunami_lab::setups::TsunamiEvent2d::getMomentumY( t_real,
                                                                       t_real ) const
{
    return 0;
}

tsunami_lab::t_real tsunami_lab::setups::TsunamiEvent2d::getBathymetry( t_real i_x,
                                                                        t_real i_y ) const
{
    // calculate the x coordinate scaled to the bathymetry coordinate system.
    t_real x = i_x / scaleX * std::abs( bathymetry[0][0] - bathymetry[0][bathymetrySize[0] - 1] ) + bathymetry[0][0];
    t_real y = i_y / scaleY * std::abs( bathymetry[1][0] - bathymetry[1][bathymetrySize[1] - 1] ) + bathymetry[1][0];

    // obtain the closest value
    t_real b = getValueAscending( bathymetry, bathymetrySize, bathymetryData[2].stride, x, y );
    t_real d = 0;
    if( displacement[0][0] <= x && x <= displacement[0][displacementSize[0] - 1]
        && displacement[1][0] <= y && y <= displacement[1][displacementSize[1] - 1] )
    {
        d = getValueAscending( displacement, displacementSize, displacementData[2].stride, x, y );
    }

    return ( b < 0 ? std::min( b, delta ) : std::max( b, delta ) ) + d;
}
