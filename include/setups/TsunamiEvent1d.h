/**
 * @author Fabian Hofer, Vincent Gerlach (AT uni-jena.de)
 *
 * @section DESCRIPTION
 * One-dimensional initialization of h, hu and b using the bathymetry reader and the artificial displacement.
 **/

#ifndef TSUNAMISIMULATION_TSUNAMIEVENT1D_H
#define TSUNAMISIMULATION_TSUNAMIEVENT1D_H

#include "Setup.h"
#include <string>
#include <cmath>
#include <math.h>
#include <vector>

namespace tsunami_lab
{
    namespace setups
    {
        class TsunamiEvent1d;
    }
}

/**
 * TsunamiEvent1d setup.
 */
class tsunami_lab::setups::TsunamiEvent1d : public Setup
{
private:
    //! number of cells csv data points
    t_idx m_csvDataPoint = 0;

    //! momentum of the cell
    t_real m_momentum = 0;

    //! bathymetry height of the cell
    std::vector<t_real> m_bathymetry;

    //! delta to avoid numerical issues
    t_real m_delta = 20;

    //! scale of our grid
    t_real m_scale = 10;

public:
    /**
     * Constructor.
     *
     * @param i_filePath string to the bathymetry_profile.csv
     * @param i_delta avoids running into numerical issues due to missing support for wetting and drying in our solver
     * @param i_scale width of grid
     **/
    TsunamiEvent1d( std::string i_filePath,
                    t_real i_delta = 20,
                    t_real i_scale = 10 );


    /**
     * Gets the water height at a given point.
     *
     * @param i_x x-coordinate of the queried point.
     * @return height at the given point.
     **/
    t_real getHeight( t_real i_x,
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
     * @return bathymetry at a given point.
     **/
    t_real getBathymetry( t_real i_x,
                          t_real ) const;


    /**
     * Gets the vertical displacement at a given point
     *
     * @param i_x x-coordinate of the queried point.
     * @return vertical displacement at a given point.
     **/
    t_real getVerticalDisplacement( t_real i_x,
                                    t_real ) const;
};

#endif //TSUNAMISIMULATION_TSUNAMIEVENT1D_H
