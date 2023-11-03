#include "../../include/setups/MiddleStates1d.h"
#include "../../include/constants.h"

tsunami_lab::setups::MiddleStates::MiddleStates( t_real i_heightLeft, t_real i_heightRight, t_real i_momentumLeft, t_real i_momentumRight, t_real i_location )
{
	m_heightLeft = i_heightLeft;
	m_heightRight = i_heightRight;
	m_momentumLeft = i_momentumLeft;
	m_momentumRight = i_momentumRight;
	m_location = i_location;
}

tsunami_lab::t_real tsunami_lab::setups::MiddleStates::getHeight( t_real i_x, t_real ) const
{
	if( i_x <= m_location )
	{
		return m_heightLeft;
	}
	else
	{
		return m_heightRight;
	}
}

tsunami_lab::t_real tsunami_lab::setups::MiddleStates::getMomentumX( t_real i_x, t_real ) const
{
	if( i_x <= m_location )
	{
		return m_momentumLeft;
	}
	else
	{
		return m_momentumRight;
	}
}

tsunami_lab::t_real tsunami_lab::setups::MiddleStates::getMomentumY( t_real, t_real ) const
{
	return 0;
}
