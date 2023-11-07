/**
 * @author Alexander Breuer (alex.breuer AT uni-jena.de)
 *
 * @section DESCRIPTION
 * One-dimensional wave propagation patch.
 **/
#ifndef TSUNAMI_LAB_PATCHES_WAVE_PROPAGATION_1D
#define TSUNAMI_LAB_PATCHES_WAVE_PROPAGATION_1D

#include "WavePropagation.h"
#include <cmath>

namespace tsunami_lab
{
	namespace patches
	{
		class WavePropagation1d;
	}
}

class tsunami_lab::patches::WavePropagation1d : public WavePropagation
{
private:
	//! current step which indicates the active values in the arrays below
	unsigned short m_step = 0;

	//! number of cells discretizing the computational domain
	t_idx m_nCells = 0;

	//! water heights for the current and next time step for all cells
	t_real* m_h[2] = { nullptr, nullptr };

	//! momenta for the current and next time step for all cells
	t_real* m_hu[2] = { nullptr, nullptr };

	//! the solver used for the netUpdates
	Solver solver = Solver::FWAVE;

	//! bathymetry for the current an next time step for all cells
	t_real* m_bathymetry;

	//! check if bathymetry exists
	bool hasBathymetry;

	//! total height of water height + bathymetry
	t_real* m_totalHeight;

	// TODO docu
	bool hasReflection[2] = { false, false };

	// TODO docu
	enum Reflection
	{
		NONE = 0,
		LEFT,
		RIGHT
	};

	// TODO docu
	Reflection calculateReflection( t_real* i_h,
									t_real* i_hu,
									t_idx i_ceL,
									t_real& o_heightLeft,
									t_real& o_heightRight,
									t_real& o_momentumLeft,
									t_real& o_momentumRight );
	// TODO docu
	Reflection calculateReflection( t_real* i_h,
									t_real* i_hu,
									t_idx i_ceL,
									t_real& o_heightLeft,
									t_real& o_heightRight,
									t_real& o_momentumLeft,
									t_real& o_momentumRight,
									t_real& o_bathymetryLeft,
									t_real& o_bathymetryRight );

public:
	/**
	 * Constructs the 1d wave propagation solver.
	 *
	 * @param i_nCells number of cells.
	 **/
	WavePropagation1d( t_idx i_nCells );

	/**
	 * Destructor which frees all allocated memory.
	 **/
	~WavePropagation1d();

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
	 * Gets the stride in y-direction. x-direction is stride-1.
	 *
	 * @return stride in y-direction.
	 **/
	t_idx getStride()
	{
		return m_nCells + 2;
	}

	/**
	 * Gets cells' water heights.
	 *
	 * @return water heights.
	 */
	t_real const* getHeight()
	{
		return m_h[m_step] + 1;
	}

	t_real const* getTotalHeight()
	{
		for( t_idx i = 1; i < m_nCells + 1; i++ )
		{
			m_totalHeight[i] = m_h[m_step][i] + m_bathymetry[i];
		}
		return m_totalHeight + 1;
	}

	/**
	 * Gets the cells' momenta in x-direction.
	 *
	 * @return momenta in x-direction.
	 **/
	t_real const* getMomentumX()
	{
		return m_hu[m_step] + 1;
	}

	/**
	 * Dummy function which returns a nullptr.
	 **/
	t_real const* getMomentumY()
	{
		return nullptr;
	}

	/**
	 * Gets the cells' bathymetry.
	 *
	 * @return
	 */
	t_real const* getBathymetry()
	{
		return m_bathymetry + 1;
	}

	/**
	 * Sets the height of the cell to the given value.
	 *
	 * @param i_ix id of the cell in x-direction.
	 * @param i_h water height.
	 **/
	void setHeight( t_idx  i_ix,
					t_idx,
					t_real i_h )
	{
		m_h[m_step][i_ix + 1] = i_h;
	}

	/**
	 * Sets the momentum in x-direction to the given value.
	 *
	 * @param i_ix id of the cell in x-direction.
	 * @param i_hu momentum in x-direction.
	 **/
	void setMomentumX( t_idx  i_ix,
					   t_idx,
					   t_real i_hu )
	{
		m_hu[m_step][i_ix + 1] = i_hu;
	}

	/**
	 * Dummy function since there is no y-momentum in the 1d solver.
	 **/
	void setMomentumY( t_idx,
					   t_idx,
					   t_real )
	{
	};

	/**
	 * Set the solver for the netUpdate
	 * Default: FWave
	 *
	 * @param solver used solver
	 */
	void setSolver( Solver solver )
	{
		WavePropagation1d::solver = solver;
	}

	/**
	 * Set the bathymetry of the cell to the given value.
	 *
	 * @param t_bathymetry bathymetry.
	 */
	void setBathymetry( t_idx i_ix,
						t_idx,
						t_real i_bathymetry )
	{
		m_bathymetry[i_ix] = i_bathymetry;
	}

	/**
	 * Dummy function to check if bathymetry is enabled.
	 */
	void enableBathymetry( bool enable )
	{
		hasBathymetry = enable;
	}

	void updateWaterHeight()
	{
		for( t_idx i = 1; i < m_nCells + 1; i++ )
		{
			m_h[m_step][i] -= m_bathymetry[i];
			m_h[m_step][i] *= ( m_h[m_step][i] > 0 );  // sets water with bathymetry higher than water to zero
		}
	}

	void setReflection( Side side, bool enable )
	{
		hasReflection[side] = enable;
	}
};

#endif