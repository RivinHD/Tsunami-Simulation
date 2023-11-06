/**
 * @author Alexander Breuer (alex.breuer AT uni-jena.de)
 *
 * @section DESCRIPTION
 * Base class of the wave propagation patches.
 **/
#ifndef TSUNAMI_LAB_PATCHES_WAVE_PROPAGATION
#define TSUNAMI_LAB_PATCHES_WAVE_PROPAGATION

#include "../constants.h"

namespace tsunami_lab {
  namespace patches {
    class WavePropagation;

    /**
     * solver options for wave propagation.
     */
    enum Solver
    {
      FWAVE,
      ROE
    };
  }
}

class tsunami_lab::patches::WavePropagation {


  public:
    /**
     * Virtual destructor for base class.
     **/
    virtual ~WavePropagation(){};

    /**
     * Performs a time step.
     *
     * @param i_scaling scaling of the time step.
     **/
    virtual void timeStep( t_real i_scaling ) = 0;

    /**
     * Sets the values of the ghost cells according to outflow boundary conditions.
     **/
    virtual void setGhostOutflow() = 0;

    /**
     * Gets the stride in y-direction. x-direction is stride-1.
     *
     * @return stride in y-direction.
     **/
    virtual t_idx getStride() = 0;

    /**
     * Gets cells' water heights.
     *
     * @return water heights.
     */
    virtual t_real const * getHeight() = 0;

    /**
     * Gets the cells' momenta in x-direction.
     *
     * @return momenta in x-direction.
     **/
    virtual t_real const * getMomentumX() = 0;

    /**
     * Gets the cells' total height of water height + bathymetry.
     *
     * @return total height.
     */
    virtual t_real const * getTotalHeight() = 0;

    /**
     * Gets the cells' momenta in y-direction.
     *
     * @return momenta in y-direction.
     **/
    virtual t_real const * getMomentumY() = 0;

    /**
     * Gets the cells' bathymetry height
     *
     * @return bathymetry height
     **/
    virtual t_real const * getBathymetry() = 0;

    /**
     * Sets the height of the cell to the given value.
     *
     * @param i_ix id of the cell in x-direction.
     * @param i_iy id of the cell in y-direction.
     * @param i_h water height.
     **/
    virtual void setHeight( t_idx  i_ix,
                            t_idx  i_iy,
                            t_real i_h ) = 0;

    /**
     * Sets the momentum in x-direction to the given value.
     *
     * @param i_ix id of the cell in x-direction.
     * @param i_iy id of the cell in y-direction.
     * @param i_hu momentum in x-direction.
     **/
    virtual void setMomentumX( t_idx  i_ix,
                               t_idx  i_iy,
                               t_real i_hu ) = 0;

    /**
     * Sets the momentum in y-direction to the given value.
     *
     * @param i_ix id of the cell in x-direction.
     * @param i_iy id of the cell in y-direction.
     * @param i_hv momentum in y-direction.
     **/
    virtual void setMomentumY( t_idx  i_ix,
                               t_idx  i_iy,
                               t_real i_hv ) = 0;

    /**
     * Set the solver for the netUpdate
     * 
     * @param solver used solver
     */
    virtual void setSolver(Solver solver) = 0;

    /**
     * Set the bathymetry of the cell to the given value.
     *
     * @param i_ix id of the cell in x-direction.
     * @param i_iy id of the cell in y-direction.
     * @param i_b bathymetry
     */
    virtual void setBathymetry( t_idx  i_ix,
                                t_idx  i_iy,
                                t_real i_b ) = 0;

    virtual void enableBathymetry(bool enable) = 0;

    virtual void updateWaterHeight() = 0;
};

#endif