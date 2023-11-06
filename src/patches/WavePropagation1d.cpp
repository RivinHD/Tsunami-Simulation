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
		for( t_idx l_ce = 0; l_ce < m_nCells; l_ce++ )
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

			// compute net-updates
			t_real l_netUpdates[2][2];
			t_real heightLeft;
			t_real heightRight;
			t_real momentumLeft;
			t_real momentumRight;
			t_real bathymetryLeft;
			t_real bathymetryRight;

			calulateReflection( l_hOld,
								l_huOld,
								l_ceL,
								heightLeft,
								heightRight,
								momentumLeft,
								momentumRight,
								bathymetryLeft,
								bathymetryRight );

			tsunami_lab::solvers::FWave::netUpdates( heightLeft,
													 heightRight,
													 momentumLeft,
													 momentumRight,
													 bathymetryRight,
													 bathymetryLeft,
													 l_netUpdates[0],
													 l_netUpdates[1] );

			// update the cells' quantities
			l_hNew[l_ceL] -= i_scaling * l_netUpdates[0][0];
			l_huNew[l_ceL] -= i_scaling * l_netUpdates[0][1];

			l_hNew[l_ceR] -= i_scaling * l_netUpdates[1][0];
			l_huNew[l_ceR] -= i_scaling * l_netUpdates[1][1];
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

			// compute net-updates
			t_real l_netUpdates[2][2];

			t_real heightLeft;
			t_real heightRight;
			t_real momentumLeft;
			t_real momentumRight;
			t_real _;

			calulateReflection( l_hOld,
								l_huOld,
								l_ceL,
								heightLeft,
								heightRight,
								momentumLeft,
								momentumRight,
								_,
								_ );

			netUpdates( heightLeft,
						heightRight,
						momentumLeft,
						momentumRight,
						l_netUpdates[0],
						l_netUpdates[1] );

			// update the cells' quantities
			l_hNew[l_ceL] -= i_scaling * l_netUpdates[0][0];
			l_huNew[l_ceL] -= i_scaling * l_netUpdates[0][1];

			l_hNew[l_ceR] -= i_scaling * l_netUpdates[1][0];
			l_huNew[l_ceR] -= i_scaling * l_netUpdates[1][1];
		}
	}
}

void tsunami_lab::patches::WavePropagation1d::setGhostOutflow()
{
	t_real* l_h = m_h[m_step];
	t_real* l_hu = m_hu[m_step];

	// set left boundary
	l_h[0] = l_h[1];
	l_hu[0] = l_hu[1] * ( 2 * !hasReflection[Side::LEFT] - 1 );
	m_bathymetry[0] = m_bathymetry[1];


	// set right boundary
	l_h[m_nCells + 1] = l_h[m_nCells];
	l_hu[m_nCells + 1] = l_hu[m_nCells] * ( 2 * !hasReflection[Side::RIGHT] - 1 );
	m_bathymetry[m_nCells + 1] = m_bathymetry[m_nCells];
}