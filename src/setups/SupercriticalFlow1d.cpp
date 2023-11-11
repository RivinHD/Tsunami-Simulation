#include "../../include/setups/SupercriticalFlow1d.h"
#include <stdexcept>

tsunami_lab::setups::SupercriticalFlow1d::SupercriticalFlow1d()
	: momentum( 0.18 ), range{ 8, 12 }, bathymetryOutRange( -0.33 )
{
	bathymetryInRange = []( t_real x ) -> t_real
		{
			return -0.12 - 0.05 * ( x - 10 ) * ( x - 10 );
		};
}

tsunami_lab::setups::SupercriticalFlow1d::SupercriticalFlow1d( t_real momentum,
															   t_real range[2],
															   t_real bathymetryOutRange,
															   t_real( *bathymetryInRange )( t_real ) )
	:momentum( momentum ), bathymetryOutRange( bathymetryOutRange ), bathymetryInRange( bathymetryInRange )
{
	if( range[0] > range[1] )
	{
		throw std::logic_error( "range[0] is not smaller than range[1]" );
	}
	if( bathymetryInRange == nullptr )
	{
		throw std::runtime_error( "The bathymetryInRange function should not be a null" );
	}
	tsunami_lab::setups::SupercriticalFlow1d::range[0] = range[0];
	tsunami_lab::setups::SupercriticalFlow1d::range[1] = range[1];
}

tsunami_lab::setups::SupercriticalFlow1d::~SupercriticalFlow1d()
{
}

tsunami_lab::t_real tsunami_lab::setups::SupercriticalFlow1d::getHeight( t_real i_x,
																		 t_real ) const
{
	return -getBathymetry( i_x, 0 );
}

tsunami_lab::t_real tsunami_lab::setups::SupercriticalFlow1d::getMomentumX( t_real,
																			t_real ) const
{
	return momentum;
}

tsunami_lab::t_real tsunami_lab::setups::SupercriticalFlow1d::getMomentumY( t_real,
																			t_real ) const
{
	return 0;
}

tsunami_lab::t_real tsunami_lab::setups::SupercriticalFlow1d::getBathymetry( t_real i_x,
																			 t_real ) const
{
	if( range[0] < i_x && i_x < range[1] )
	{
		return bathymetryInRange( i_x );
	}
	return bathymetryOutRange;
}
