/**
 * @author Alexander Breuer (alex.breuer AT uni-jena.de)
 *
 * @section DESCRIPTION
 * One-dimensional wave propagation patch.
 **/
#include "../../include/patches/WavePropagation1d.h"
#include "../../include/solvers/Roe.h"
#include "../../include/solvers/FWave.h"

tsunami_lab::patches::WavePropagation1d::WavePropagation1d( t_idx i_nCells )
{
	m_nCells = i_nCells;

	// allocate memory including a single ghost cell on each side
	for( unsigned short l_st = 0; l_st < 2; l_st++ )
	{
		m_h[l_st] = new t_real[m_nCells + 2];
		m_hu[l_st] = new t_real[m_nCells + 2];
	}
	m_bathymetry = new t_real[m_nCells + 2]{ 0 };
	m_totalHeight = new t_real[m_nCells + 2]{ 0 };

	// init to zero
	for( unsigned short l_st = 0; l_st < 2; l_st++ )
	{
		for( t_idx l_ce = 0; l_ce < m_nCells + 2; l_ce++ )
		{
			m_h[l_st][l_ce] = 0;
			m_hu[l_st][l_ce] = 0;
		}
	}
}

tsunami_lab::patches::WavePropagation1d::~WavePropagation1d()
{
	for( unsigned short l_st = 0; l_st < 2; l_st++ )
	{
		delete[] m_h[l_st];
		delete[] m_hu[l_st];
	}
	delete[] m_bathymetry;
	delete[] m_totalHeight;
}

void tsunami_lab::patches::WavePropagation1d::timeStep( t_real i_scaling )
{
	// pointers to old and new data
	t_real* l_hOld = m_h[m_step];
	t_real* l_huOld = m_hu[m_step];

	m_step = ( m_step + 1 ) % 2;
	t_real* l_hNew = m_h[m_step];
	t_real* l_huNew = m_hu[m_step];

	// init new cell quantities
	for( t_idx l_ce = 1; l_ce < m_nCells + 1; l_ce++ )
	{
		l_hNew[l_ce] = l_hOld[l_ce];
		l_huNew[l_ce] = l_huOld[l_ce];
	}

	// only possible for f-wave solver
	if( hasBathymetry )
	{
		// iterate over edges and update with Riemann solutions
		for( t_idx l_ed = 0; l_ed < m_nCells + 1; l_ed++ )
		{
			// determine left and right cell-id
			t_idx l_ceL = l_ed;
			t_idx l_ceR = l_ed + 1;

			// noting to compute both shore cells
			if( l_hOld[l_ceL] == 0 && l_hOld[l_ceR] == 0 )
			{
				continue;
			}

			// compute reflection
			t_real heightLeft;
			t_real heightRight;
			t_real momentumLeft;
			t_real momentumRight;
			t_real bathymetryLeft;
			t_real bathymetryRight;

			Reflection reflection = calculateReflection( l_hOld,
														 l_huOld,
														 l_ceL,
														 heightLeft,
														 heightRight,
														 momentumLeft,
														 momentumRight,
														 bathymetryLeft,
														 bathymetryRight );

			// compute net-updates
			t_real l_netUpdates[2][2];

			tsunami_lab::solvers::FWave::netUpdates( heightLeft,
													 heightRight,
													 momentumLeft,
													 momentumRight,
													 bathymetryRight,
													 bathymetryLeft,
													 l_netUpdates[0],
													 l_netUpdates[1] );

			// update the cells' quantities
			l_hNew[l_ceL] -= i_scaling * l_netUpdates[0][0] * ( Reflection::RIGHT != reflection );
			l_huNew[l_ceL] -= i_scaling * l_netUpdates[0][1] * ( Reflection::RIGHT != reflection );

			l_hNew[l_ceR] -= i_scaling * l_netUpdates[1][0] * ( Reflection::LEFT != reflection );
			l_huNew[l_ceR] -= i_scaling * l_netUpdates[1][1] * ( Reflection::LEFT != reflection );
		}
	}
	else
	{
		// uses a function pointer to choose between the solvers
		void ( *netUpdates )( t_real, t_real, t_real, t_real, t_real*, t_real* ) = solvers::FWave::netUpdates;
		if( solver == Solver::ROE )
		{
			netUpdates = solvers::Roe::netUpdates;
		}
		// iterate over edges and update with Riemann solutions
		for( t_idx l_ed = 0; l_ed < m_nCells + 1; l_ed++ )
		{
			// determine left and right cell-id
			t_idx l_ceL = l_ed;
			t_idx l_ceR = l_ed + 1;

			// noting to compute both shore cells
			if( l_hOld[l_ceL] == 0 && l_hOld[l_ceR] == 0 )
			{
				continue;
			}

			// compute reflection
			t_real heightLeft;
			t_real heightRight;
			t_real momentumLeft;
			t_real momentumRight;

			Reflection reflection = calculateReflection( l_hOld,
														 l_huOld,
														 l_ceL,
														 heightLeft,
														 heightRight,
														 momentumLeft,
														 momentumRight );

			// compute net-updates
			t_real l_netUpdates[2][2];

			netUpdates( heightLeft,
						heightRight,
						momentumLeft,
						momentumRight,
						l_netUpdates[0],
						l_netUpdates[1] );

			// update the cells' quantities
			l_hNew[l_ceL] -= i_scaling * l_netUpdates[0][0] * ( Reflection::RIGHT != reflection );
			l_huNew[l_ceL] -= i_scaling * l_netUpdates[0][1] * ( Reflection::RIGHT != reflection );

			l_hNew[l_ceR] -= i_scaling * l_netUpdates[1][0] * ( Reflection::LEFT != reflection );
			l_huNew[l_ceR] -= i_scaling * l_netUpdates[1][1] * ( Reflection::LEFT != reflection );
		}
	}
}

void tsunami_lab::patches::WavePropagation1d::setGhostOutflow()
{
	t_real* l_h = m_h[m_step];
	t_real* l_hu = m_hu[m_step];

	// set left boundary
	l_h[0] = l_h[1] * !hasReflection[Side::LEFT];
	l_hu[0] = l_hu[1];
	m_bathymetry[0] = m_bathymetry[1];

	// set right boundary
	l_h[m_nCells + 1] = l_h[m_nCells] * !hasReflection[Side::RIGHT];
	l_hu[m_nCells + 1] = l_hu[m_nCells];
	m_bathymetry[m_nCells + 1] = m_bathymetry[m_nCells];
}

tsunami_lab::patches::WavePropagation1d::Reflection tsunami_lab::patches::WavePropagation1d::calculateReflection( t_real* i_h,
																												  t_real* i_hu,
																												  t_idx i_ceL,
																												  t_real& o_heightLeft,
																												  t_real& o_heightRight,
																												  t_real& o_momentumLeft,
																												  t_real& o_momentumRight )
{
	t_idx l_ceR = i_ceL + 1;
	bool leftReflection = i_h[l_ceR] == t_real( 0.0 );
	o_heightRight = leftReflection ? i_h[i_ceL] : i_h[l_ceR];
	o_momentumRight = leftReflection ? -i_hu[i_ceL] : i_hu[l_ceR];

	bool rightReflection = i_h[i_ceL] == t_real( 0.0 );
	o_heightLeft = rightReflection ? i_h[l_ceR] : i_h[i_ceL];
	o_momentumLeft = rightReflection ? -i_hu[l_ceR] : i_hu[i_ceL];

	return static_cast<Reflection>( leftReflection * Reflection::LEFT + rightReflection * Reflection::RIGHT );
}

tsunami_lab::patches::WavePropagation1d::Reflection tsunami_lab::patches::WavePropagation1d::calculateReflection( t_real* i_h,
																												  t_real* i_hu,
																												  t_idx i_ceL,
																												  t_real& o_heightLeft,
																												  t_real& o_heightRight,
																												  t_real& o_momentumLeft,
																												  t_real& o_momentumRight,
																												  t_real& o_bathymetryLeft,
																												  t_real& o_bathymetryRight )
{
	t_idx l_ceR = i_ceL + 1;
	bool leftReflection = ( i_h[l_ceR] == t_real( 0.0 ) );
	o_heightRight = leftReflection ? i_h[i_ceL] : i_h[l_ceR];
	o_momentumRight = leftReflection ? -i_hu[i_ceL] : i_hu[l_ceR];
	o_bathymetryRight = leftReflection ? m_bathymetry[i_ceL] : m_bathymetry[l_ceR];

	bool rightReflection = ( i_h[i_ceL] == t_real( 0.0 ) );
	o_heightLeft = rightReflection ? i_h[l_ceR] : i_h[i_ceL];
	o_momentumLeft = rightReflection ? -i_hu[l_ceR] : i_hu[i_ceL];
	o_bathymetryLeft = rightReflection ? m_bathymetry[l_ceR] : m_bathymetry[i_ceL];

	return static_cast<Reflection>( leftReflection * Reflection::LEFT + rightReflection * Reflection::RIGHT );
}