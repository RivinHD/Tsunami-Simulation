#include "../../include/setups/ShockShock1d.h"

tsunami_lab::setups::ShockShock1d::ShockShock1d( t_real i_heightLeft,
												 t_real i_momentumLeft,
												 t_real i_locationShock )
{
	m_heightLeft = i_heightLeft;
	m_momentumLeft = i_momentumLeft;
	m_locationShock = i_locationShock;
}

tsunami_lab::t_real tsunami_lab::setups::ShockShock1d::getHeight( t_real,
																  t_real ) const
{
	return m_heightLeft;
}

tsunami_lab::t_real tsunami_lab::setups::ShockShock1d::getMomentumX( t_real i_x,
																	 t_real ) const
{
	if( i_x <= m_locationShock )
	{
		return m_momentumLeft;
	}
	else
	{
		return -m_momentumLeft;
	}
}

tsunami_lab::t_real tsunami_lab::setups::ShockShock1d::getMomentumY( t_real,
																	 t_real ) const
{
	return 0;
}

tsunami_lab::t_real tsunami_lab::setups::ShockShock1d::getBathymetry( t_real,
																	  t_real ) const
{
	return 0;
}
