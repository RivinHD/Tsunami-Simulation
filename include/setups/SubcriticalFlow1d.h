/**
 * @author Fabian Hofer, Vincent Gerlach (AT uni-jena.de)
 *
 * Subcritical flow setup for hydraulic jumps
 **/

#ifndef SUBCRITICAL_FLOW1D_H
#define SUBCRITICAL_FLOW_1D_H

#include "Setup.h"

namespace tsunami_lab
{
    namespace setups
    {
        class SubcriticalFlow1d;
    }
}

/**
 * setup for subcritical flow
*/
class tsunami_lab::setups::SubcriticalFlow1d : public Setup
{
private:
    //! The fixed momentum of the water
    t_real momentum = 0;

    //! the range were the dynamic bathymetry appears
    t_real range[2]{ 0 };

    //! the bathymetry that exists outside of the specified range
    t_real bathymetryOutRange = 0;

    //! the function to create the dynamic bathymetry with
    t_real( *bathymetryInRange )( t_real ) = nullptr;

public:
    /**
     * Default supercritical flow example
    */
    SubcriticalFlow1d();

    /**
     * Customized supercritical flow example
     *
     * @param momentum the fixed momentum of the water
     * @param range the range were the bathymetry is created by the bathymetryInRange function, range[0] < range[1]
     * @param bathymetryOutRange fixed bathymetry outside of the range
     * @param bathymetryInRange function for creating dynamic bathymetry to first parameter is the x-coordinate of the bathymetry i.e. the values between range[0] and range[1]
     */
    SubcriticalFlow1d( t_real momentum,
                       t_real range[2],
                       t_real bathymetryOutRange,
                       t_real( *bathymetryInRange )( t_real ) );

    /**
     * default destructor
    */
    ~SubcriticalFlow1d();

    /**
     * Gets the water height at a given point.
     *
     * @param i_x x-coordinate of the queried point.
     * @return water height at the given point.
     **/
    t_real getHeight( t_real i_x, t_real ) const override;


    /**
     * Gets the momentum in x-direction.
     *
     * @return momentum in x-direction.
     **/
    t_real getMomentumX( t_real, t_real ) const override;


    /**
     * Gets the momentum in y-direction.
     *
     * @return momentum in y-direction. (Always )
     **/
    t_real getMomentumY( t_real, t_real ) const override;

    /**
     * Gets the bathymetry at a given point.
     *
     * @param i_x x-coordinate of the queried point.
     * @return bathymetry at a given point.
     **/
    t_real getBathymetry( t_real i_x, t_real ) const override;
};
#endif // !SUBCRITICAL_FLOW1D_H
