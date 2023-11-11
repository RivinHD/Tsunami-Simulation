/**
 * @author Fabian Hofer, Vincent Gerlach (AT uni-jena.de)
 *
 * @section DESCRIPTION
 * One-dimensional rare-rare problem.
 **/

#include "../../include/setups/RareRare1d.h"

tsunami_lab::setups::RareRare1d::RareRare1d( tsunami_lab::t_real i_heightLeft,
                                             tsunami_lab::t_real i_momentumLeft,
                                             tsunami_lab::t_real i_locationRare )
{
    m_heightLeft = i_heightLeft;
    m_momentumLeft = i_momentumLeft;
    m_locationRare = i_locationRare;
}

tsunami_lab::t_real tsunami_lab::setups::RareRare1d::getHeight( tsunami_lab::t_real,
                                                                tsunami_lab::t_real ) const
{
    return m_heightLeft;
}

tsunami_lab::t_real tsunami_lab::setups::RareRare1d::getMomentumX( tsunami_lab::t_real i_x,
                                                                   tsunami_lab::t_real ) const
{
    if( i_x <= m_locationRare )
    {
        return -m_momentumLeft;
    }
    else
    {
        return m_momentumLeft;
    }
}

tsunami_lab::t_real tsunami_lab::setups::RareRare1d::getMomentumY( tsunami_lab::t_real,
                                                                   tsunami_lab::t_real ) const
{
    return 0;
}

tsunami_lab::t_real tsunami_lab::setups::RareRare1d::getBathymetry( t_real,
                                                                    t_real ) const
{
    return 0;
}
