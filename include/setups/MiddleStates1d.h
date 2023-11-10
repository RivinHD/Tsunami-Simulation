/**
 * @author Fabian Hofer, Vincent Gerlach (AT uni-jena.de)
 *
 * @section DESCRIPTION
 * One-dimensional middle-states problem.
 **/

#ifndef MIDDKESTATES1D_H
#define MIDDKESTATES1D_H

#include "Setup.h"

namespace tsunami_lab
{
	namespace setups
	{
		class MiddleStates1d;
	}
}

/**
 * 1d meddle-states setup.
 */
class tsunami_lab::setups::MiddleStates1d : public Setup
{
private:
	//! height of left side
	t_real m_heightLeft = 0;
	//! height of right side
	t_real m_heightRight = 0;
	//! momentum of left side
	t_real m_momentumLeft = 0;
	//! momentum of right side
	t_real m_momentumRight = 0;
	//! momentum of right side
	t_real m_location = 0;
public:
	/**
	 * Construct a new middleStates 1d object.
	 *
	 * @param i_heightLeft water height of left side of the rare location.
	 * @param i_heightRight momentum of the water of the right side.
	 * @param i_momentumLeft momentum of the water of the left side.
	 * @param i_momentumRight momentum of the water of the right side.
	 * @param i_location location (x-coordinate) of the middle state.
	*/
	MiddleStates1d( t_real i_heightLeft,
					t_real i_heightRight,
					t_real i_momentumLeft,
					t_real i_momentumRight,
					t_real i_location );

	/**
	 * Destroy the middleStates 1d object.
	 */
	~MiddleStates1d()
	{
	};

	/**
	 * Get the ware height at a given point.
	 *
	 * @param i_x x-coordinate of the queried point.
	 * @return height at the given point.
	*/
	t_real getHeight( t_real i_x, t_real ) const;

	/**
	 * Get the momentum in x-direction at a given point.
	 *
	 * @param i_x x-coordinate of the queried point.
	 * @return momentum in x-direction at a given point.
	 */
	t_real getMomentumX( t_real i_x, t_real ) const;

	/**
	 * Get the momentum in y-direction at a given point.
	 *
	 * @return momentum in y-direction at a given point.
	 */
	t_real getMomentumY( t_real, t_real ) const;
};

#endif // MIDDKESTATES1D_H