#include "../../include/setups/ShockShock1d.h"
#define t_real tsunami_lab::t_real

tsunami_lab::setups::ShockShock1d::ShockShock1d(t_real i_heightLeft,
                                                t_real i_heightRight,
                                                t_real i_momentumLeft,
                                                t_real i_locationShock)
{
    m_heightLeft = i_heightLeft;
    m_heightRight = i_heightRight;
    m_momentumLeft = i_momentumLeft;
    m_locationShock = i_locationShock;
}

t_real tsunami_lab::setups::ShockShock1d::getHeight(t_real i_x,
                                                    t_real ) const
{
    if (i_x <= m_locationShock)
    {
        return m_heightLeft;
    }
    else
    {
        return m_heightRight;
    }
}

t_real tsunami_lab::setups::ShockShock1d::getMomentumX(t_real i_x,
                                                       t_real ) const
{
    if (i_x <= m_locationShock)
    {
        return m_momentumLeft;
    }
    else
    {
        return -m_momentumLeft;
    }
}

t_real tsunami_lab::setups::ShockShock1d::getMomentumY(t_real,
                                                       t_real) const
{
    return 0;
}
