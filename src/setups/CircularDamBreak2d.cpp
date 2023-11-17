/**
 * @author Fabian Hofer, Vincent Gerlach (AT uni-jena.de)
 *
 * Implementation of circular dam break setup.
 **/

#include "../../include/setups/CircularDamBreak2d.h"
#include <cmath>

tsunami_lab::setups::CircularDamBreak2d::CircularDamBreak2d()
    : heightCenter( 10 ), heightOutside( 5 ), locationCenter{ 50, 50 }, scaleCenter( 10 )
{
}

tsunami_lab::setups::CircularDamBreak2d::CircularDamBreak2d( t_real i_heightCenter,
                                                             t_real i_heightOutside,
                                                             t_real i_locationCenter[2],
                                                             t_real i_scaleCenter )
    : heightCenter( i_heightCenter ), heightOutside( i_heightOutside ), locationCenter{ i_locationCenter[0], i_locationCenter[1] }, scaleCenter( i_scaleCenter )
{
}

tsunami_lab::t_real tsunami_lab::setups::CircularDamBreak2d::getHeight( t_real i_x,
                                                                        t_real i_y ) const
{
    bool isInside = std::sqrt( std::pow( i_x - locationCenter[0], 2 ) + std::pow( i_y - locationCenter[1], 2 ) ) < scaleCenter;
    return isInside ? heightCenter : heightOutside;
}

tsunami_lab::t_real tsunami_lab::setups::CircularDamBreak2d::getMomentumX( t_real, t_real ) const
{
    return 0;
}

tsunami_lab::t_real tsunami_lab::setups::CircularDamBreak2d::getMomentumY( t_real, t_real ) const
{
    return 0;
}

tsunami_lab::t_real tsunami_lab::setups::CircularDamBreak2d::getBathymetry( t_real, t_real ) const
{
    return 0;
}
