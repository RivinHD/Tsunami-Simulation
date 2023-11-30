/**
 * @author Fabian Hofer, Vincent Gerlach (AT uni-jena.de)
 *
 * Implementation of artificial tsunami setup.
 **/

#include "../../include/setups/ArtificialTsunami2d.h"
#include <cmath>

tsunami_lab::setups::ArtificialTsunami2d::ArtificialTsunami2d()
    : bathymetryHeight( -100 ), centerOffset( 5000 )
{
}

tsunami_lab::t_real tsunami_lab::setups::ArtificialTsunami2d::getHeight( t_real,
                                                                         t_real ) const
{
    return -bathymetryHeight;
}

tsunami_lab::t_real tsunami_lab::setups::ArtificialTsunami2d::getMomentumX( t_real,
                                                                            t_real ) const
{
    return 0;
}

tsunami_lab::t_real tsunami_lab::setups::ArtificialTsunami2d::getMomentumY( t_real,
                                                                            t_real ) const
{
    return 0;
}

tsunami_lab::t_real tsunami_lab::setups::ArtificialTsunami2d::getBathymetry( t_real i_x,
                                                                             t_real i_y ) const
{
    i_x -= centerOffset;
    i_y -= centerOffset;
    if( -500 <= i_x && i_x <= 500 && -500 <= i_y && i_y <= 500 )
    {
        t_real f = std::sin( ( i_x / 500 + 1 ) * M_PI );
        t_real g = -std::pow( i_y / 500, 2 ) + 1;
        return bathymetryHeight + 5 * f * g;
    }
    return bathymetryHeight;
}
