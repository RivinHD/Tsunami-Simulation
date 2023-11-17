/**
 * @author Fabian Hofer, Vincent Gerlach (AT uni-jena.de)
 *
 * One-dimensional rare-rare problem.
 **/

#ifndef RARERARE1D_H
#define RARERARE1D_H

#include "Setup.h"

namespace tsunami_lab
{
    namespace setups
    {
        class RareRare1d;
    }
}

/**
 * 1d rare-rare setup.
 */
class tsunami_lab::setups::RareRare1d : public Setup
{
private:
    //! height of left side
    t_real m_heightLeft = 0;
    //! momentum of left side
    t_real m_momentumLeft = 0;
    //! momentum of right side
    t_real m_locationRare = 0;
public:
    /**
     * Construct a new rare-rare 1d object.
     *
     * @param i_heightLeft water height of left side of the rare location.
     * @param i_momentumLeft momentum of the water of the left side.
     * @param i_locationRare location (x-coordinate) of the rare.
     */
    RareRare1d( t_real i_heightLeft,
                t_real i_momentumLeft,
                t_real i_locationRare );

    /**
     * Destroy the rare-rare 1d object.
     */
    ~RareRare1d()
    {
    };

    /**
     * Get the ware height at a given point.
     *
     * @return height at the given point.
     */
    t_real getHeight( t_real, t_real ) const;

    /**
     * Get the momentum in x-direction at a given point.
     *
     * @param i_x x-coordinate of the queried point.
     *
     * @return momentum in x-direction at a given point.
     */
    t_real getMomentumX( t_real i_x, t_real ) const;

    /**
     * Get the momentum in y-direction at a given point.
     *
     * @return momentum in y-direction at a given point.
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

#endif // RARERARE1D_H