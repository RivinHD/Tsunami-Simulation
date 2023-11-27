/**
 * @author Fabian Hofer, Vincent Gerlach (AT uni-jena.de)
 *
 * Two-dimensional initialization of h, hu and b using the bathymetry reader and the artificial displacement.
 **/

#ifndef TSUNAMISIMULATION_TSUNAMIEVENT2D_H
#define TSUNAMISIMULATION_TSUNAMIEVENT2D_H

#include "Setup.h"
#include "../../include/io/NetCdf.h"

namespace tsunami_lab
{
    namespace setups
    {
        class TsunamiEvent2d;
    }
}

/**
 * TsunamiEvent2d setup.
 */
class tsunami_lab::setups::TsunamiEvent2d : public Setup
{
private:

    //! scale in x direction of our grid
    t_real scaleX = 1;

    //! scale in y direction of our grid
    t_real scaleY = 1;

    //! delta to avoid numerical issues
    t_real delta = 20;

    //! the bathymetry data from the file where index 0 is x, 1 is y, 2 is z/data
    tsunami_lab::io::NetCdf::VarArray bathymetryData[3];

    //! the the converted bathymetry data where the array is stored.
    t_real* bathymetry[3] = { nullptr, nullptr, nullptr };

    t_idx bathymetrySize[3] = { 0, 0, 0 };

    //! the vertical displacement data from the file where index 0 is x, 1 is y, 2 is z/data
    tsunami_lab::io::NetCdf::VarArray displacementData[3];

    //! the the converted displacement data where the array is stored.
    t_real* displacement[3] = { nullptr,nullptr,nullptr };

    //! the size of each displacement array.
    t_idx displacementSize[3] = { 0,0,0 };

    /**
     * get the closest data point from an ascending sorted array
     *
     * @param data data arrays where index 0 is x, 1 is y, 2 is z/data
     * @param size the size of the data arrays where index 0 is x, 1 is y, 2 is z/data
     * @param zStride the stride of the third/z array
     * @param x the x-coordinate to obtain the closest value
     * @param y the y-coordinate to obtain the closest value
     * @return the closest value found or 0 if no lower not be found or calculated index is out of range
    */
    t_real getValueAscending( const t_real* const data[3],
                              const t_idx size[3],
                              const t_idx zStride,
                              const t_real x,
                              const t_real y ) const;

public:

    /**
     * Creates the 2d tsunami event that is read from two netCdf files.
     *
     * @param bathymetryFilePath filepath to the bathymetry file
     * @param bathymetryVariable variables to read in the bathymetry file. The first should be the X dimension, the second the Y dimension and the third the bathymetry data
     * @param displacementFilePath filepath to the displacement file
     * @param displacementVariable variables to read in the displacement file. The first should be the X dimension, the second the Y dimension and the third the displacement data
     * @param scaleX width of the grid
     * @param scaleY height of the grid
     * @param delta avoids running into numerical issues due to missing support for wetting and drying in our solver
    */
    TsunamiEvent2d( const char* bathymetryFilePath,
                    const char* ( &bathymetryVariable )[3],
                    const char* displacementFilePath,
                    const char* ( &displacementVariable )[3],
                    t_real scaleX,
                    t_real scaleY,
                    t_real delta = 20 );

    /**
     * Destroys the 2d tsunami event.
    */
    ~TsunamiEvent2d()
    {
    }


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
     * @param i_x x-coordinate of the queried point.
     * @param i_y y-coordinate of the queried point.
     * @return bathymetry at a given point.
     **/
    t_real getBathymetry( t_real i_x,
                          t_real i_y ) const;
};

#endif // !TSUNAMISIMULATION_TSUNAMIEVENT2D_H
