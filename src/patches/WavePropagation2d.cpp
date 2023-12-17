/**
 * @author Alexander Breuer (alex.breuer AT uni-jena.de)
 *
 * One-dimensional wave propagation patch.
 **/
#include "../../include/patches/WavePropagation2d.h"
#include "../../include/solvers/Roe.h"
#include "../../include/solvers/FWave.h"
#include <iostream>

tsunami_lab::patches::WavePropagation2d::WavePropagation2d( t_idx i_xCells,
                                                            t_idx i_yCells )
{
    m_xCells = i_xCells;
    m_yCells = i_yCells;
    stride = i_xCells + 2;
    totalCells = ( i_xCells + 2 ) * ( i_yCells + 2 );

    // calculates xCells dividable by ITERATIONS_CACHE and remaining cells
    remaining_xCells = m_xCells % ITERATIONS_CACHE;
    full_xCells = m_xCells - remaining_xCells;

    // allocate memory including a single ghost cell on each side
    size_t alignment = ITERATIONS_CACHE * sizeof( t_real );
    for( unsigned short l_st = 0; l_st < 2; l_st++ )
    {
        m_h[l_st] = aligned_alloc<t_real>( m_hPtr[l_st], totalCells, alignment );
        m_hu[l_st] = aligned_alloc<t_real>( m_huPtr[l_st], totalCells, alignment );
        m_hv[l_st] = aligned_alloc<t_real>( m_hvPtr[l_st], totalCells, alignment );
    }
    m_bathymetry = aligned_alloc<t_real>( m_bathymetryPtr, totalCells, alignment );
    m_totalHeight = aligned_alloc<t_real>( m_totalHeightPtr, totalCells, alignment );
}

tsunami_lab::patches::WavePropagation2d::~WavePropagation2d()
{
    for( unsigned short l_st = 0; l_st < 2; l_st++ )
    {
        delete[] m_hPtr[l_st];
        delete[] m_huPtr[l_st];
        delete[] m_hvPtr[l_st];
    }
    delete[] m_bathymetryPtr;
    delete[] m_totalHeightPtr;
}

void tsunami_lab::patches::WavePropagation2d::timeStep( t_real i_scaling )
{
    isDirtyTotalHeight = true;

    // pointers to old and new data
    t_real* l_hOld = m_h[m_step];
    t_real* l_huOld = m_hu[m_step];

    m_step = ( m_step + 1 ) & 1;  // & 1 is alias for % 2
    t_real* l_hNew = m_h[m_step];
    t_real* l_huNew = m_hu[m_step];

    // init new cell quantities
    for( t_idx l_ce = 0; l_ce < totalCells; l_ce++ )
    {
        l_hNew[l_ce] = l_hOld[l_ce];
        l_huNew[l_ce] = l_huOld[l_ce];
    }

    // only possible for f-wave solver
    if( hasBathymetry )
    {
        // iterates through the row
        for( t_idx i = 0; i < m_yCells + 1; i++ )
        {
            // iterates along the row
            for( t_idx j = 0; j < m_xCells + 1; j++ )
            {
                t_idx k = stride * i + j;

                // determine left and right cell-id
                t_idx l_ceL = k;
                t_idx l_ceR = k + 1;

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
                                                             l_ceR,
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
                                                         bathymetryLeft,
                                                         bathymetryRight,
                                                         l_netUpdates );

                // update the cells' quantities
                l_hNew[l_ceL] -= i_scaling * l_netUpdates[0][0] * ( Reflection::RIGHT != reflection );
                l_huNew[l_ceL] -= i_scaling * l_netUpdates[0][1] * ( Reflection::RIGHT != reflection );

                l_hNew[l_ceR] -= i_scaling * l_netUpdates[1][0] * ( Reflection::LEFT != reflection );
                l_huNew[l_ceR] -= i_scaling * l_netUpdates[1][1] * ( Reflection::LEFT != reflection );
            }
        }
    }
    else
    {
        // uses a function pointer to choose between the solvers
        void ( *netUpdates )( t_real, t_real, t_real, t_real, t_real[2][2] ) = solvers::FWave::netUpdates;
        if( solver == Solver::ROE )
        {
            netUpdates = solvers::Roe::netUpdates;
        }

        // iterates through the row
        for( t_idx i = 0; i < m_yCells + 1; i++ )
        {
            // iterates over along the row
            for( t_idx j = 0; j < m_xCells + 1; j++ )
            {
                t_idx k = stride * i + j;

                // determine left and right cell-id
                t_idx l_ceL = k;
                t_idx l_ceR = k + 1;

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
                                                             l_ceR,
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
                            l_netUpdates );

                // update the cells' quantities
                l_hNew[l_ceL] -= i_scaling * l_netUpdates[0][0] * ( Reflection::RIGHT != reflection );
                l_huNew[l_ceL] -= i_scaling * l_netUpdates[0][1] * ( Reflection::RIGHT != reflection );

                l_hNew[l_ceR] -= i_scaling * l_netUpdates[1][0] * ( Reflection::LEFT != reflection );
                l_huNew[l_ceR] -= i_scaling * l_netUpdates[1][1] * ( Reflection::LEFT != reflection );
            }
        }
    }
    //swapping the h buffer new and old to write new data in previous old
    m_h[m_step] = l_hOld;
    m_step = ( m_step + 1 ) & 1;  // & 1 is alias for % 2
    m_h[m_step] = l_hNew;

    // pointers to old and new data
    t_real* l_hvOld = m_hv[m_step];
    l_hOld = m_h[m_step];

    m_step = ( m_step + 1 ) & 1;  // & 1 is alias for % 2
    t_real* l_hvNew = m_hv[m_step];
    l_hNew = m_h[m_step];

    // copy the calculated cell quantities
    for( t_idx l_ce = 0; l_ce < totalCells; l_ce++ )
    {
        l_hNew[l_ce] = l_hOld[l_ce];
        l_hvNew[l_ce] = l_hvOld[l_ce];
    }

    // only possible for f-wave solver
    if( hasBathymetry )
    {
        //  iterates over the x direction
        for( t_idx i = 1; i < full_xCells; i += ITERATIONS_CACHE )
        {
            // iterate over the rows i.e. y-coordinates
            for( t_idx j = 0; j < m_yCells + 1; j++ )
            {
                // iterations for more efficient cache usage
                for( t_idx k = 0; k < ITERATIONS_CACHE; k++ )
                {
                    // determine left and right cell-id
                    t_idx l_ceT = stride * j + i + k;
                    t_idx l_ceB = l_ceT + stride;

                    // noting to compute both shore cells
                    if( l_hOld[l_ceT] == 0 && l_hOld[l_ceB] == 0 )
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
                                                                 l_hvOld,
                                                                 l_ceT,
                                                                 l_ceB,
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
                                                             bathymetryLeft,
                                                             bathymetryRight,
                                                             l_netUpdates );

                    // update the cells' quantities
                    l_hNew[l_ceT] -= i_scaling * l_netUpdates[0][0] * ( Reflection::RIGHT != reflection );
                    l_hvNew[l_ceT] -= i_scaling * l_netUpdates[0][1] * ( Reflection::RIGHT != reflection );

                    l_hNew[l_ceB] -= i_scaling * l_netUpdates[1][0] * ( Reflection::LEFT != reflection );
                    l_hvNew[l_ceB] -= i_scaling * l_netUpdates[1][1] * ( Reflection::LEFT != reflection );
                }
            }
        }

        // iterate over the rows i.e. y-coordinates
        for( t_idx j = 0; j < m_yCells + 1; j++ )
        {
            // remaining iterations for more efficient cache usage
            for( t_idx k = 0; k < remaining_xCells; k++ )
            {
                // determine left and right cell-id
                t_idx l_ceT = stride * j + full_xCells + k;
                t_idx l_ceB = l_ceT + stride;

                // noting to compute both shore cells
                if( l_hOld[l_ceT] == 0 && l_hOld[l_ceB] == 0 )
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
                                                             l_hvOld,
                                                             l_ceT,
                                                             l_ceB,
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
                                                         bathymetryLeft,
                                                         bathymetryRight,
                                                         l_netUpdates );

                // update the cells' quantities
                l_hNew[l_ceT] -= i_scaling * l_netUpdates[0][0] * ( Reflection::RIGHT != reflection );
                l_hvNew[l_ceT] -= i_scaling * l_netUpdates[0][1] * ( Reflection::RIGHT != reflection );

                l_hNew[l_ceB] -= i_scaling * l_netUpdates[1][0] * ( Reflection::LEFT != reflection );
                l_hvNew[l_ceB] -= i_scaling * l_netUpdates[1][1] * ( Reflection::LEFT != reflection );
            }
        }
    }
    else
    {
        // uses a function pointer to choose between the solvers
        void ( *netUpdates )( t_real, t_real, t_real, t_real, t_real[2][2] ) = solvers::FWave::netUpdates;
        if( solver == Solver::ROE )
        {
            netUpdates = solvers::Roe::netUpdates;
        }

        //  iterates over the x direction
        for( t_idx i = 1; i < full_xCells; i += ITERATIONS_CACHE )
        {
            // iterate over the rows i.e. y-coordinates
            for( t_idx j = 1; j < m_yCells + 1; j++ )
            {
                // iterations for more efficient cache usage
                for( t_idx k = 0; k < ITERATIONS_CACHE; k++ )
                {
                    // determine left and right cell-id
                    t_idx l_ceT = stride * j + i + k;
                    t_idx l_ceB = l_ceT + stride;

                    // noting to compute both shore cells
                    if( l_hOld[l_ceT] == 0 && l_hOld[l_ceB] == 0 )
                    {
                        continue;
                    }

                    // compute reflection
                    t_real heightLeft;
                    t_real heightRight;
                    t_real momentumLeft;
                    t_real momentumRight;

                    Reflection reflection = calculateReflection( l_hOld,
                                                                 l_hvOld,
                                                                 l_ceT,
                                                                 l_ceB,
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
                                l_netUpdates );

                    // update the cells' quantities
                    l_hNew[l_ceT] -= i_scaling * l_netUpdates[0][0] * ( Reflection::RIGHT != reflection );
                    l_hvNew[l_ceT] -= i_scaling * l_netUpdates[0][1] * ( Reflection::RIGHT != reflection );

                    l_hNew[l_ceB] -= i_scaling * l_netUpdates[1][0] * ( Reflection::LEFT != reflection );
                    l_hvNew[l_ceB] -= i_scaling * l_netUpdates[1][1] * ( Reflection::LEFT != reflection );
                }
            }
        }

        // iterate over the rows i.e. y-coordinates
        for( t_idx j = 1; j < m_yCells + 1; j++ )
        {
            // remaining iterations for more efficient cache usage
            for( t_idx k = 0; k < remaining_xCells; k++ )
            {
                // determine left and right cell-id
                t_idx l_ceT = stride * j + full_xCells + k;
                t_idx l_ceB = l_ceT + stride;

                // noting to compute both shore cells
                if( l_hOld[l_ceT] == 0 && l_hOld[l_ceB] == 0 )
                {
                    continue;
                }

                // compute reflection
                t_real heightLeft;
                t_real heightRight;
                t_real momentumLeft;
                t_real momentumRight;

                Reflection reflection = calculateReflection( l_hOld,
                                                             l_hvOld,
                                                             l_ceT,
                                                             l_ceB,
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
                            l_netUpdates );

                // update the cells' quantities
                l_hNew[l_ceT] -= i_scaling * l_netUpdates[0][0] * ( Reflection::RIGHT != reflection );
                l_hvNew[l_ceT] -= i_scaling * l_netUpdates[0][1] * ( Reflection::RIGHT != reflection );

                l_hNew[l_ceB] -= i_scaling * l_netUpdates[1][0] * ( Reflection::LEFT != reflection );
                l_hvNew[l_ceB] -= i_scaling * l_netUpdates[1][1] * ( Reflection::LEFT != reflection );
            }
        }
    }
}

void tsunami_lab::patches::WavePropagation2d::setGhostOutflow()
{
    t_real* l_h = m_h[m_step];
    t_real* l_hu = m_hu[m_step];
    t_real* l_hv = m_hv[m_step];

    for( t_idx i = 1; i < m_yCells + 1; i++ )
    {
        t_idx y = stride * i;

        // set left boundary
        l_h[y] = l_h[y + 1] * !hasReflection[Side::LEFT];
        l_hu[y] = l_hu[y + 1];
        l_hv[y] = l_hv[y + 1];
        m_bathymetry[y] = m_bathymetry[y + 1];

        // set right boundary
        l_h[y + m_xCells + 1] = l_h[y + m_xCells] * !hasReflection[Side::RIGHT];
        l_hu[y + m_xCells + 1] = l_hu[y + m_xCells];
        l_hv[y + m_xCells + 1] = l_hv[y + m_xCells];
        m_bathymetry[y + m_xCells + 1] = m_bathymetry[y + m_xCells];
    }

    for( size_t i = 0; i < stride; i++ )
    {
        // set top complete row of ghost cells
        t_idx waterRowT = stride + i;
        l_h[i] = l_h[waterRowT] * !hasReflection[Side::TOP];
        l_hu[i] = l_hu[waterRowT];
        l_hv[i] = l_hv[waterRowT];
        m_bathymetry[i] = m_bathymetry[waterRowT];

        // set bottom complete row of ghost cells
        t_idx ghostRowB = ( m_yCells + 1 ) * stride + i;
        t_idx waterRowB = m_yCells * stride + i;
        l_h[ghostRowB] = l_h[waterRowB] * !hasReflection[Side::BOTTOM];
        l_hu[ghostRowB] = l_hu[waterRowB];
        l_hv[ghostRowB] = l_hv[waterRowB];
        m_bathymetry[ghostRowB] = m_bathymetry[waterRowB];
    }
}

tsunami_lab::patches::WavePropagation2d::Reflection tsunami_lab::patches::WavePropagation2d::calculateReflection( t_real* i_h,
                                                                                                                  t_real* i_hu,
                                                                                                                  t_idx i_ceL,
                                                                                                                  t_idx i_ceR,
                                                                                                                  t_real& o_heightLeft,
                                                                                                                  t_real& o_heightRight,
                                                                                                                  t_real& o_momentumLeft,
                                                                                                                  t_real& o_momentumRight )
{
    bool leftReflection = i_h[i_ceR] == t_real( 0.0 );
    o_heightRight = leftReflection ? i_h[i_ceL] : i_h[i_ceR];
    o_momentumRight = leftReflection ? -i_hu[i_ceL] : i_hu[i_ceR];

    bool rightReflection = i_h[i_ceL] == t_real( 0.0 );
    o_heightLeft = rightReflection ? i_h[i_ceR] : i_h[i_ceL];
    o_momentumLeft = rightReflection ? -i_hu[i_ceR] : i_hu[i_ceL];

    return static_cast<Reflection>( leftReflection * Reflection::LEFT + rightReflection * Reflection::RIGHT );
}

tsunami_lab::patches::WavePropagation2d::Reflection tsunami_lab::patches::WavePropagation2d::calculateReflection( t_real* i_h,
                                                                                                                  t_real* i_hu,
                                                                                                                  t_idx i_ceL,
                                                                                                                  t_idx i_ceR,
                                                                                                                  t_real& o_heightLeft,
                                                                                                                  t_real& o_heightRight,
                                                                                                                  t_real& o_momentumLeft,
                                                                                                                  t_real& o_momentumRight,
                                                                                                                  t_real& o_bathymetryLeft,
                                                                                                                  t_real& o_bathymetryRight )
{
    bool leftReflection = ( i_h[i_ceR] == t_real( 0.0 ) );
    o_heightRight = leftReflection ? i_h[i_ceL] : i_h[i_ceR];
    o_momentumRight = leftReflection ? -i_hu[i_ceL] : i_hu[i_ceR];
    o_bathymetryRight = leftReflection ? m_bathymetry[i_ceL] : m_bathymetry[i_ceR];

    bool rightReflection = ( i_h[i_ceL] == t_real( 0.0 ) );
    o_heightLeft = rightReflection ? i_h[i_ceR] : i_h[i_ceL];
    o_momentumLeft = rightReflection ? -i_hu[i_ceR] : i_hu[i_ceL];
    o_bathymetryLeft = rightReflection ? m_bathymetry[i_ceR] : m_bathymetry[i_ceL];

    return static_cast<Reflection>( leftReflection * Reflection::LEFT + rightReflection * Reflection::RIGHT );
}

const tsunami_lab::t_real* tsunami_lab::patches::WavePropagation2d::getTotalHeight()
{
    if( isDirtyTotalHeight )
    {
        for( t_idx i = 1; i < m_yCells + 1; i++ )
        {
            for( t_idx j = 1; j < m_xCells + 1; j++ )
            {
                t_idx k = stride * i + j;
                m_totalHeight[k] = m_h[m_step][k] + m_bathymetry[k];
            }
        }
    }
    isDirtyTotalHeight = false;
    return m_totalHeight + 1 + stride;
}
