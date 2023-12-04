/**
 * @author Alexander Breuer (alex.breuer AT uni-jena.de)
 *
 * One-dimensional wave propagation patch.
 **/
#ifndef TSUNAMI_LAB_PATCHES_WAVE_PROPAGATION_2D
#define TSUNAMI_LAB_PATCHES_WAVE_PROPAGATION_2D

#include "WavePropagation.h"
#include <cmath>

namespace tsunami_lab
{
    namespace patches
    {
        class WavePropagation2d;
    }
}

/**
* One-dimensional wave propagation patch.
*/
class tsunami_lab::patches::WavePropagation2d : public WavePropagation
{
private:
    //! current step which indicates the active values in the arrays below
    unsigned short m_step = 0;

    //! number of total cells including water and ghost cells
    t_idx totalCells = 1;

    //! number of cells in x direction without ghost cells
    t_idx m_xCells = 1;

    //! number of cells in y direction without ghost cells
    t_idx m_yCells = 1;

    //! the size in x-direction of the cells
    t_idx stride = 1;

    //! water heights for the current and next time step for all cells
    t_real* m_h[2] = { nullptr, nullptr };

    //! momenta for the current and next time step for all cells
    t_real* m_hu[2] = { nullptr, nullptr };

    //! momenta for the current and next time step for all cells
    t_real* m_hv[2] = { nullptr, nullptr };

    //! the solver used for the netUpdates
    Solver solver = Solver::FWAVE;

    //! bathymetry for the current an next time step for all cells
    t_real* m_bathymetry = nullptr;

    //! check if bathymetry exists
    bool hasBathymetry = false;

    //! total height of water height + bathymetry
    t_real* m_totalHeight = nullptr;

    //! is true if the total needs to be updated
    bool isDirtyTotalHeight = true;

    //! reflection for the left (index: 0), right (index: 1), top (index: 2) and bottom (index: 3)
    bool hasReflection[4] = { false, false, false, false };

    //! Iteration that should be performed to use the cache more efficiently e.g. 32 -> 32 * sizeof(t_real) = 32 * 4 byte = 128 byte Cacheline
    static constexpr t_idx ITERATIONS_CACHE = 16;

    /*
    * The Sides on which the reflection appears
    * LEFT and RIGHT can be added to obtain BOTH
    */
    enum Reflection
    {
        NONE = 0,
        LEFT = 1,
        RIGHT = 2,
        BOTH = 3
    };

    /**
     * Calculates the right height, momentum, bathymetry with respect to a reflection i.e. one cell is a shore and the other is a water cell
     * A shore cell is the cell with water 0 i.e. the height is zero
     *
     * @param i_h the height array
     * @param i_hu the momentum array
     * @param i_ceL the current index of the left cell
     * @param i_ceR the current index of the right cell
     * @param o_heightLeft ouput of the height left that should be used for the calculations
     * @param o_heightRight ouput of the height right that should be used for the calculations
     * @param o_momentumLeft ouput of the momentum left that should be used for the calculations
     * @param o_momentumRight ouput of the momentum right that should be used for the calculations
     * @return the side where the reflection hits the shore cell e.g. the left cell is water and right is shore than the reflection hits the left side
    */
    Reflection calculateReflection( t_real* i_h,
                                    t_real* i_hu,
                                    t_idx i_ceL,
                                    t_idx i_ceR,
                                    t_real& o_heightLeft,
                                    t_real& o_heightRight,
                                    t_real& o_momentumLeft,
                                    t_real& o_momentumRight );

    /**
     * Calculates the right height, momentum, bathymetry with respect to a reflection i.e. one cell is a shore and the other is a water cell
     * A shore cell is the cell with water 0 i.e. the height is zero
     *
     * @param i_h the height array
     * @param i_hu the momentum array
     * @param i_ceL the current index of the left cell
     * @param i_ceR the current index of the right cell
     * @param o_heightLeft ouput of the height left that should be used for the calculations
     * @param o_heightRight ouput of the height right that should be used for the calculations
     * @param o_momentumLeft ouput of the momentum left that should be used for the calculations
     * @param o_momentumRight ouput of the momentum right that should be used for the calculations
     * @param o_bathymetryLeft ouput of the bathymetry left that should be used for the calculations
     * @param o_bathymetryRight ouput of the bathymetry right that should be used for the calculations
     * @return the side where the reflection hits the shore cell e.g. the left cell is water and right is shore than the reflection hits the left side
    */
    Reflection calculateReflection( t_real* i_h,
                                    t_real* i_hu,
                                    t_idx i_ceL,
                                    t_idx i_ceR,
                                    t_real& o_heightLeft,
                                    t_real& o_heightRight,
                                    t_real& o_momentumLeft,
                                    t_real& o_momentumRight,
                                    t_real& o_bathymetryLeft,
                                    t_real& o_bathymetryRight );

public:
    /**
     * Constructs the 2d wave propagation solver.
     *
     * @param i_xCells number of x-cells.
     * @param i_yCells number of y-cells.
     **/
    WavePropagation2d( t_idx i_xCells, t_idx i_yCells );

    /**
     * Destructor which frees all allocated memory.
     **/
    ~WavePropagation2d();

    /**
     * Performs a time step.
     *
     * @param i_scaling scaling of the time step (dt / dx).
     **/
    void timeStep( t_real i_scaling );

    /**
     * Sets the values of the ghost cells according to outflow boundary conditions.
     **/
    void setGhostOutflow();

    /**
     * Gets cells' water heights.
     *
     * @return water heights.
     */
    t_real const* getHeight()
    {
        return m_h[m_step] + 1 + stride;
    }

    /**
     * Gets the combined height of bathymetry and water height
     *
     * @return combined height
    */
    t_real const* getTotalHeight();

    /**
     * Gets the cells' momenta in x-direction.
     *
     * @return momenta in x-direction.
     **/
    t_real const* getMomentumX()
    {
        return m_hu[m_step] + 1 + stride;
    }

    /**
     * Dummy function which returns a nullptr.
     **/
    t_real const* getMomentumY()
    {
        return m_hv[m_step] + 1 + stride;
    }

    /**
     * Gets the cells' bathymetry.
     *
     * @return
     */
    t_real const* getBathymetry()
    {
        return m_bathymetry + 1 + stride;
    }

    /**
     * Gets the stride in y-direction. x-direction is stride-1.
     *
     * @return stride in y-direction.
     **/
    t_idx getStride()
    {
        return stride;
    }

    /**
     * Sets the height of the cell to the given value.
     *
     * @param i_ix id of the cell in x-direction.
     * @param i_iy id of the cell in y-direction.
     * @param i_h water height.
     **/
    void setHeight( t_idx i_ix,
                    t_idx i_iy,
                    t_real i_h )
    {
        m_h[m_step][stride * ( i_iy + 1 ) + i_ix + 1] = i_h;
    }

    /**
     * Sets the momentum in x-direction to the given value.
     *
     * @param i_ix id of the cell in x-direction.
     * @param i_iy id of the cell in y-direction.
     * @param i_hu momentum in x-direction.
     **/
    void setMomentumX( t_idx i_ix,
                       t_idx i_iy,
                       t_real i_hu )
    {
        m_hu[m_step][stride * ( i_iy + 1 ) + i_ix + 1] = i_hu;
    }

    /**
     * Sets the momentum in y-direction to the given value.
     *
     * @param i_ix id of the cell in x-direction.
     * @param i_iy id of the cell in y-direction.
     * @param i_hv momentum in y-direction.
     **/
    void setMomentumY( t_idx i_ix,
                       t_idx i_iy,
                       t_real i_hv )
    {
        m_hv[m_step][stride * ( i_iy + 1 ) + i_ix + 1] = i_hv;
    };

    /**
     * Set the solver for the netUpdate
     * Default: FWave
     *
     * @param solver used solver
     */
    void setSolver( Solver solver )
    {
        WavePropagation2d::solver = solver;
    }

    /**
     * Set the bathymetry of the cell to the given value.
     *
     * @param i_ix id of the cell in x-direction.
     * @param i_iy id of the cell in y-direction.
     * @param i_bathymetry bathymetry data to set
     */
    void setBathymetry( t_idx i_ix,
                        t_idx i_iy,
                        t_real i_bathymetry )
    {
        m_bathymetry[stride * ( i_iy + 1 ) + i_ix + 1] = i_bathymetry;
    }

    /**
     * enables or disable the bathymetry
     *
     * @param enable true=enabled, false=disabled
    */
    void enableBathymetry( bool enable )
    {
        hasBathymetry = enable;
    }

    /**
     * updates the water height with respect to the bathymetry.
     * If the bathymetry is higher than the water height than the water is set to zero.
    */
    void updateWaterHeight()
    {
        if( !hasBathymetry )
        {
            return;
        }

        for( t_idx i = 1; i < m_yCells + 1; i++ )
        {
            for( t_idx j = 1; j < m_xCells + 1; j++ )
            {
                t_idx k = stride * i + j;

                m_h[m_step][k] -= m_bathymetry[k];
                m_h[m_step][k] *= ( m_h[m_step][k] > 0 );  // sets water with bathymetry higher than water to zero
            }
        }
    }

    /**
     * enables or disable the reflection of one side
     *
     * @param side Side to enable {LEFT, RIGHT}
     * @param enable true=enabled, false=disabled
    */
    void setReflection( Side side, bool enable )
    {
        hasReflection[side] = enable;
    }
};

#endif