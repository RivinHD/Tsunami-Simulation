/**
 * @author Fabian Hofer, Vincent Gerlach (AT uni-jena.de)
 *
 * @section DESCRIPTION
 * Declaration of circular dam break setup.
 **/

#ifndef CIRCULARDAMBREAK_2D_H
#define CIRCULARDAMBREAK_2D_H

#include "Setup.h"

namespace tsunami_lab
{
    namespace setups
    {
        class CircularDamBreak2d;
    }
}

/**
 * 1d dam break setup.
 **/
class tsunami_lab::setups::CircularDamBreak2d : public Setup
{
private:
    //! height inside the circular dam
    t_real heightCenter;

    //! height on the outside of the circular dam
    t_real heightOutside;

    //! location of circular dam
    t_real locationCenter[2];

    //! scale of the circular dam
    t_real scaleCenter;

public:
    /**
     * Default constructor with the default example.
    */
    CircularDamBreak2d();

    /**
     * Constructor.
     *
     * @param i_heightCenter water height inside of the circular dam.
     * @param i_heightRight water height outside of the circular dam.
     * @param i_locationDam[2] location (x-coordinate, y-coordinate) of the circular dam.
     * @param i_scaleCenter radius of the circular dam
     **/
    CircularDamBreak2d( t_real i_heightCenter,
                        t_real i_heightOutside,
                        t_real i_locationCenter[2],
                        t_real i_scaleCenter );

    /**
     * Gets the water height at a given point.
     *
     * @param i_x x-coordinate of the queried point.
     * @param i_y y-coordinate of the queried point.
     * @return height at the given point.
     **/
    t_real getHeight( t_real i_x,
                      t_real i_y ) const;

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
     * @return bathymetry at a given point.
     **/
    t_real getBathymetry( t_real,
                          t_real ) const;

};

#endif