/**
 * @author Fabian Hofer, Vincent Gerlach (AT uni-jena.de)
 *
 * One-dimensional shock-shock problem.
 **/

#ifndef SHOCKSHOCK1D_H
#define SHOCKSHOCK1D_H

#include "Setup.h"

namespace tsunami_lab
{
    namespace setups
    {
        class ShockShock1d;
    }
}

/**
 * 1d shock-shock setup.
 */
class tsunami_lab::setups::ShockShock1d : public Setup
{
private:
    //! height on the left side.
    t_real m_heightLeft = 0;
    //! momentum on the left side.
    t_real m_momentumLeft = 0;
    //! location of were the shock waves collide.
    t_real m_locationShock = 0;
public:
    /**
     * Construct a new shock-shock 1d object.
     *
     * @param i_heightLeft water height on the left side of the shock location.
     * @param i_momentumLeft momentum of the water of the left side.
     * @param i_locationShock location (x-coordinate) of the shock.
     */
    ShockShock1d( t_real i_heightLeft,
                  t_real i_momentumLeft,
                  t_real i_locationShock );

    /**
     * Destroy the shock-shock 1d object.
     */
    ~ShockShock1d()
    {
    };

    /**
     * Get the water height at a given point.
     *
     * @return height at the given point.
     */
    t_real getHeight( t_real, t_real ) const;

    /**
     * Get the wate momentum in x-direction at a given point.
     *
     * @param i_x x-coordinate of the queried point.
     *
     * @return momentum in x-direction.
     */
    t_real getMomentumX( t_real i_x, t_real ) const;

    /**
     * Get the wate momentum in y-direction at a given point.
     *
     * @return momentum in y-direction.
     */
    t_real getMomentumY( t_real, t_real ) const;

    /**
     * Gets the bathymetry at a given point.
     *
     * @return bathymetry at a given point.
     **/
    t_real getBathymetry( t_real,
                          t_real ) const;
};


#endif // SHOCKSHOCK1D_H