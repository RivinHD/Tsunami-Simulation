/**
 * @author Fabian Hofer, Vincent Gerlach (AT uni-jena.de)
 *
 * Declaration of artificial tsunami setup.
 **/

#ifndef ARTIFICIALTSUNAMI_2D_H
#define ARTIFICIALTSUNAMI_2D_H

#include "Setup.h"

namespace tsunami_lab
{
    namespace setups
    {
        class ArtificialTsunami2d;
    }
}

/**
 * artificial tsunami setup. This Setup only provides one hard-coded example.
 **/
class tsunami_lab::setups::ArtificialTsunami2d : public Setup
{
private:
    //! the start height of bathymetry before displacement
    t_real bathymetryHeight;

    //! the offset of the center of the displacement
    t_real centerOffset;

public:
    /**
     * Default constructor with the default example.
    */
    ArtificialTsunami2d();


    /**
     * Gets the water height at a given point.
     *
     * @return height at the given point.
     **/
    t_real getHeight( t_real,
                      t_real ) const;

    /**
     * Gets the momentum in x-direction.
     *
     * @return momentum in x-direction.
     **/
    t_real getMomentumX( t_real,
                         t_real ) const;

    /**
     * Gets the momentum in y-direction.
     *
     * @return momentum in y-direction.
     **/
    t_real getMomentumY( t_real,
                         t_real ) const;
    /**
     * Gets the bathymetry at a given point.
     *
     * @param i_x x-coordinate of the queried point.
     * @param i_y y-coordinate of the queried point.
     * @return bathymetry at a given point.
     **/
    t_real getBathymetry( t_real i_x,
                          t_real i_y ) const;

};

#endif