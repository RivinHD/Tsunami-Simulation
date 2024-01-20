/**
 * @author Fabian Hofer, Vincent Gerlach
 *
 * Two-dimensional adaptive mesh refinement wave propagation.
 **/

#include "../../include/amr/AMRCoreWavePropagation2d.h"
#include "../../include/amr/Kernels.h"

#include <AMReX_MultiFabUtil.H>
#include <AMReX_FillPatchUtil.H>
#include <AMReX_PhysBCFunct.H>
#include <AMReX_PlotFileUtil.H>
#include <AMReX_ParmParse.H>

using namespace amrex;

void tsunami_lab::amr::AMRCoreWavePropagation2d::FillCoarsePatch( int level,
                                                                  amrex::Real time,
                                                                  amrex::MultiFab& mf,
                                                                  int icomp,
                                                                  int ncomp )
{
    BL_ASSERT( level > 0 );

    Vector<MultiFab*> cmf;
    Vector<Real> ctime;
    GetData( level - 1, time, cmf, ctime );

    if( cmf.size() != 1 )
    {
        amrex::Abort( "FillCoarsePatch: how did this happen?" );
    }

    CpuBndryFuncFab bndry_func( nullptr );  // Without EXT_DIR, we can pass a nullptr.
    PhysBCFunct<CpuBndryFuncFab> cphysbc( geom[level - 1], physicalBoundary, bndry_func );
    PhysBCFunct<CpuBndryFuncFab> fphysbc( geom[level], physicalBoundary, bndry_func );

    amrex::InterpFromCoarseLevel( mf, time, *cmf[0], 0, icomp, ncomp, geom[level - 1], geom[level],
                                  cphysbc, 0, fphysbc, 0, refRatio( level - 1 ),
                                  interpolator, physicalBoundary, 0 );
}

void tsunami_lab::amr::AMRCoreWavePropagation2d::GetData( int level,
                                                          amrex::Real time,
                                                          amrex::Vector<amrex::MultiFab*>& data,
                                                          amrex::Vector<amrex::Real>& datatime )
{
    data.clear();
    datatime.clear();

    const Real teps = ( tNew[level] - tOld[level] ) * 1.e-3;

    if( time > tNew[level] - teps && time < tNew[level] + teps )
    {
        data.push_back( &gridNew[level] );
        datatime.push_back( tNew[level] );
    }
    else if( time > tOld[level] - teps && time < tOld[level] + teps )
    {
        data.push_back( &gridOld[level] );
        datatime.push_back( tOld[level] );
    }
    else
    {
        data.push_back( &gridOld[level] );
        data.push_back( &gridNew[level] );
        datatime.push_back( tOld[level] );
        datatime.push_back( tNew[level] );
    }
}

void tsunami_lab::amr::AMRCoreWavePropagation2d::FillPatch( int level, amrex::Real time, amrex::MultiFab& mf, int icomp, int ncomp )
{
    if( level == 0 )
    {
        Vector<MultiFab*> smf;
        Vector<Real> stime;
        GetData( 0, time, smf, stime );

        CpuBndryFuncFab bndry_func( nullptr );  // Without EXT_DIR, we can pass a nullptr.
        PhysBCFunct<CpuBndryFuncFab> physbc( geom[level], physicalBoundary, bndry_func );
        amrex::FillPatchSingleLevel( mf, time, smf, stime, 0, icomp, ncomp,
                                     geom[level], physbc, 0 );
    }
    else
    {
        Vector<MultiFab*> cmf, fmf;
        Vector<Real> ctime, ftime;
        GetData( level - 1, time, cmf, ctime );
        GetData( level, time, fmf, ftime );

        CpuBndryFuncFab bndry_func( nullptr );  // Without EXT_DIR, we can pass a nullptr.
        PhysBCFunct<CpuBndryFuncFab> cphysbc( geom[level - 1], physicalBoundary, bndry_func );
        PhysBCFunct<CpuBndryFuncFab> fphysbc( geom[level], physicalBoundary, bndry_func );

        amrex::FillPatchTwoLevels( mf, time, cmf, ctime, fmf, ftime,
                                   0, icomp, ncomp, geom[level - 1], geom[level],
                                   cphysbc, 0, fphysbc, 0, refRatio( level - 1 ),
                                   interpolator, physicalBoundary, 0 );
    }
}

void tsunami_lab::amr::AMRCoreWavePropagation2d::setGlobalValue( amrex::MultiFab& mf,
                                                                 int x,
                                                                 int y,
                                                                 int z,
                                                                 int comp,
                                                                 amrex::Real value )
{

    for( MFIter mfi( mf ); mfi.isValid(); ++mfi ) // Loop over grids
    {
        const Box& box = mfi.validbox();

        Array4<Real> const& arr = mf.array( mfi );

        const auto lo = lbound( box );
        const auto hi = ubound( box );

        // Sets value only when inside the correct box
        if( lo.x <= x && hi.x >= x
            && lo.y <= y && hi.y >= y )
        {
            arr( x, y, z, comp ) = value;
            break;
        }
    }
}

void tsunami_lab::amr::AMRCoreWavePropagation2d::timeStepWithSubcycling( int level, amrex::Real time, int iteration )
{
    // ===== REGRID =====

    // help keep track of whether a level was already regridded
    // from a coarser level call to regrid
    static Vector<int> lastRegridStep( max_level + 1, 0 );

    // regrid changes level "lev+1" so we don't regrid on max_level
    // also make sure we don't regrid fine levels again if
    // it was taken care of during a coarser regrid
    if( level < max_level
        && step[level] > lastRegridStep[level]
        && step[level] % regridFrequency == 0 )
    {
        // regrid could add newly refine levels (if finest_level < max_level)
        // so we save the previous finest level index
        int oldFinest = finest_level;
        regrid( level, time );

        // mark that we have regridded this level already
        for( int k = level; k <= finest_level; ++k )
        {
            lastRegridStep[k] = step[k];
        }

        // if there are newly created levels, set the time step
        for( int k = oldFinest + 1; k <= finest_level; ++k )
        {
            dt[k] = dt[k - 1] / MaxRefRatio( k - 1 );
        }
    }

    if( Verbose() )
    {
        amrex::Print() << "[Level " << level << " step " << step[level] + 1 << "] ";
        amrex::Print() << "ADVANCE with time = " << tNew[level] << " dt = " << dt[level] << std::endl;
    }

    // ===== ADVANCE =====

    // Advance a single level for a single time step, and update flux registers
    tOld[level] = tNew[level];
    tNew[level] += dt[level];

    AdvanceGridAtLevel( level, time, dt[level], iteration, nSubSteps[level] );

    ++step[level];

    if( Verbose() )
    {
        amrex::Print() << "[Level " << level << " step " << step[level] << "] ";
        amrex::Print() << "Advanced " << CountCells( level ) << " cells" << std::endl;
    }

    // ===== SUBCYCLING =====

    if( level < finest_level )
    {
        // recursive call for next-finer level
        for( int i = 1; i <= nSubSteps[level + 1]; ++i )
        {
            timeStepWithSubcycling( level + 1, time + ( i - 1 ) * dt[level + 1], i );
        }

        // update level based on coarse-fine flux mismatch
        fluxRegister[level + 1]->Reflux( gridNew[level], 1.0, 0, 0, nComponents, geom[level] );

        AverageDownTo( level ); // average level+1 down to level
    }
}

void tsunami_lab::amr::AMRCoreWavePropagation2d::AdvanceGridAtLevel( int level,
                                                                     amrex::Real time,
                                                                     amrex::Real dtLevel,
                                                                     int /*iteration*/,
                                                                     int /*nCycle*/ )
{
    std::swap( gridOld[level], gridNew[level] );

    MultiFab& stateNew = gridNew[level];

    // size in x & y direction
    const Real dx = geom[level].CellSize( 0 );
    const Real dy = geom[level].CellSize( 1 );

    // scaling in each dimension
    Real dtdx = dtLevel / dx;
    Real dtdy = dtLevel / dy;

    // storage for fluxes :)
    MultiFab fluxes[AMREX_SPACEDIM];
    for( int i = 0; i < AMREX_SPACEDIM; ++i )
    {
        BoxArray ba = grids[level];
        ba.surroundingNodes( i );
        fluxes[i].define( ba, dmap[level], stateNew.nComp(), 0 );
    }

    // State with ghost cells
    MultiFab stateBorder( grids[level], dmap[level], stateNew.nComp(), nGhostRow );
    FillPatch( level, time, stateBorder, 0, stateBorder.nComp() );

    amrex::Print() << "DEBUG 1 Sum(Height) = " << stateBorder.sum( HEIGHT ) << std::endl;

#ifdef AMREX_USE_OMP
#pragma omp parallel
#endif
    {
        FArrayBox tmpfab;
        for( MFIter mfi( stateNew, true ); mfi.isValid(); ++mfi )
        {
            AMREX_ASSERT( stateNew.nComp() == nComponents );

            // ===== FLUX CALC AND UPDATE =====
            const Box& bx = mfi.tilebox();

            // define the grid components and fluxes
            Array4<Real const> height = stateBorder.const_array( mfi, HEIGHT );
            Array4<Real const> momentumX = stateBorder.const_array( mfi, MOMENTUM_X );
            Array4<Real const> momentumY = stateBorder.const_array( mfi, MOMENTUM_Y );
            Array4<Real const> bathymetry = stateBorder.const_array( mfi, BATHYMERTRY );
            Array4<Real      > gridOut = stateNew.array( mfi );

            Array4<Real> fluxx = fluxes[0].array( mfi );
            Array4<Real> fluxy = fluxes[1].array( mfi );

            // compute the x-sweep and the x fluxes :|
            amrex::launch( bx,
                           [=] AMREX_GPU_DEVICE( const Box & tbx )
            {
                xSweep( tbx, dtdx, height, momentumX, momentumY, bathymetry, gridOut, fluxx );
            } );

            // swap the grid components
            height = stateNew.const_array( mfi, HEIGHT );
            momentumX = stateNew.const_array( mfi, MOMENTUM_X );
            momentumY = stateNew.const_array( mfi, MOMENTUM_Y );
            bathymetry = stateNew.const_array( mfi, BATHYMERTRY );
            gridOut = stateBorder.array( mfi );

            // compute the y-sweep and the y fluxes
            amrex::launch( bx,
                           [=] AMREX_GPU_DEVICE( const Box & tbx )
            {
                ySweep( tbx, dtdy, height, momentumX, momentumY, bathymetry, gridOut, fluxy );
            } );

            // scale the fluxes :(
            amrex::ParallelFor(
                amrex::surroundingNodes( bx, Direction::x ),
                amrex::surroundingNodes( bx, Direction::y ),
                [=] AMREX_GPU_DEVICE( int i, int j, int k )
            {
                fluxx( i, j, k ) *= dtLevel * dy;
            },
                [=] AMREX_GPU_DEVICE( int i, int j, int k )
            {
                fluxy( i, j, k ) *= dtLevel * dx;
            } );
        }
    }

    std::swap( gridOld[level], gridNew[level] );

    amrex::Print() << "DEBUG 2 Sum(Height) = " << gridNew[0].sum( HEIGHT ) << std::endl;

    if( fluxRegister[level + 1] )
    {
        for( int i = 0; i < AMREX_SPACEDIM; ++i )
        {
            // update the lev+1/lev flux register (index lev+1) :)
            fluxRegister[level + 1]->CrseInit( fluxes[i], i, 0, 0, fluxes[i].nComp(), -1.0 );
        }
    }
    if( fluxRegister[level] )
    {
        for( int i = 0; i < AMREX_SPACEDIM; ++i )
        {
            // update the lev/lev-1 flux register (index lev)
            fluxRegister[level]->FineAdd( fluxes[i], i, 0, 0, fluxes[i].nComp(), 1.0 );
        }
    }
}

void tsunami_lab::amr::AMRCoreWavePropagation2d::WritePlotFile() const
{
    const std::string& plotfilename = amrex::Concatenate( "plt", step[0], 5 );
    Vector<const MultiFab*> mf;
    for( int i = 0; i <= finest_level; ++i )
    {
        mf.push_back( &gridNew[i] );
    }
    Vector<std::string> varnames = { "Height", "MomentumX", "MomentumY", "Bathymetry" };

    amrex::Print() << "Writing plotfile " << plotfilename << std::endl;

    amrex::WriteMultiLevelPlotfile( plotfilename, finest_level + 1, mf, varnames,
                                    Geom(), tNew[0], step, refRatio() );
}

void tsunami_lab::amr::AMRCoreWavePropagation2d::AverageDownTo( int coarse_lev )
{
    amrex::average_down( gridNew[coarse_lev + 1], gridNew[coarse_lev],
                         geom[coarse_lev + 1], geom[coarse_lev],
                         0, nComponents, refRatio( coarse_lev ) );
}

void tsunami_lab::amr::AMRCoreWavePropagation2d::ReadParameters()
{
    {
        ParmParse pp;  // Traditionally, stop_time do not have prefix.
        pp.query( "stop_time", simulationTime );
    }

    {
        ParmParse pp( "amr" ); // Traditionally, these have prefix, amr.

        pp.query( "regrid_int", regridFrequency );
        /*pp.query( "plot_file", plot_file );
        pp.query( "plot_int", plot_int );
        pp.query( "chk_file", chk_file );
        pp.query( "chk_int", chk_int );
        pp.query( "restart", restart_chkfile );*/
    }

    {
        ParmParse pp( "tsunami" ); // Traditionally, these have prefix, tsunami.

        int n = pp.countval( "griderr" );
        if( n > 0 )
        {
            pp.getarr( "griderr", gridErr, 0, n );
        }
    }
}

void tsunami_lab::amr::AMRCoreWavePropagation2d::InitData( tsunami_lab::setups::Setup* setup )
{
    amrex::Real hMax = std::numeric_limits<amrex::Real>::lowest();

#ifdef AMREX_USE_OMP
#pragma omp parallel reduction(max: hMax)
#endif
    for( MFIter mfi( gridNew[0], true ); mfi.isValid(); ++mfi )
    {
        Box tbx = mfi.tilebox();

        // size in x & y direction
        const Real dx = geom[0].CellSize( 0 );
        const Real dy = geom[0].CellSize( 1 );

        Array4<amrex::Real> height = gridNew[0].array( mfi, HEIGHT );
        Array4<amrex::Real> momentumX = gridNew[0].array( mfi, MOMENTUM_X );
        Array4<amrex::Real> momentumY = gridNew[0].array( mfi, MOMENTUM_Y );
        Array4<amrex::Real> bathymetry = gridNew[0].array( mfi, BATHYMERTRY );

        amrex::Real* hMaxPtr = &hMax;

        amrex::ParallelFor( tbx,
                            [=] AMREX_GPU_DEVICE( int i, int j, int k )
        {
            amrex::Real x = i * dx;
            amrex::Real y = j * dy;
            amrex::Real h = setup->getHeight( x, y );
            height( i, j, k ) = h;
            momentumX( i, j, k ) = setup->getMomentumX( x, y );
            momentumY( i, j, k ) = setup->getMomentumY( x, y );
            bathymetry( i, j, k ) = setup->getBathymetry( x, y );
            *hMaxPtr = std::max( h, *hMaxPtr );
        } );
    }

    amrex::Real speedMax = std::sqrt( 9.81 * hMax );
    std::cout << "Max speed " << speedMax << std::endl;

    amrex::Real dxy = std::min( geom[0].CellSize( 0 ), geom[0].CellSize( 1 ) );
    amrex::Real dt = 0.45 * dxy / speedMax;
    setTimeStep( dt );
}

tsunami_lab::amr::AMRCoreWavePropagation2d::AMRCoreWavePropagation2d( tsunami_lab::setups::Setup* setup )
{
    ReadParameters();

    // resize to the levels
    int nLevelsMax = max_level + 1;
    step.resize( nLevelsMax, 0 );
    nSubSteps.resize( nLevelsMax, 1 );
    tNew.resize( nLevelsMax, 0. );
    tOld.resize( nLevelsMax, -0 );
    dt.resize( nLevelsMax, 0 );
    gridNew.resize( nLevelsMax );
    gridOld.resize( nLevelsMax );
    physicalBoundary.resize( nComponents );
    fluxRegister.resize( nLevelsMax + 1 );

    // set the refinement ratio for each level for subcycling
    for( int lev = 1; lev <= max_level; ++lev )
    {
        nSubSteps[lev] = MaxRefRatio( lev - 1 );
    }

    // set the interpolation method
    for( int dim = 0; dim < AMREX_SPACEDIM; ++dim )
    {
        for( int n = 0; n < nComponents; ++n )
        {
            physicalBoundary[n].setLo( dim, BCType::int_dir );
            physicalBoundary[n].setHi( dim, BCType::int_dir );
        }
    }

    // init the domain
    InitFromScratch( 0.0 );

    InitData( setup );
}

void tsunami_lab::amr::AMRCoreWavePropagation2d::setTimeStep( amrex::Real timeStep )
{
    int nLevelsMax = max_level + 1;
    for( size_t i = 0; i < nLevelsMax; i++ )
    {
        tOld[i] = -timeStep;
        dt[i] = timeStep;
    }
}

void tsunami_lab::amr::AMRCoreWavePropagation2d::Evolve()
{
    Real currentTime = tNew[0];
    int lastPlotStep = 0;

    for( int iStep = step[0]; currentTime < simulationTime; iStep++ )
    {
        if( ( iStep ) % 60 == 0 )
        {
            lastPlotStep = iStep + 1;
            WritePlotFile();
        }

        amrex::Print() << "\nCoarse STEP " << iStep + 1 << " starts ..." << std::endl;

        // do time step with subcycling
        int level = 0;
        int iteration = 1;
        timeStepWithSubcycling( level, currentTime, iteration );

        // advance time
        currentTime += dt[0];

        amrex::Print() << "Coarse STEP " << iStep + 1 << " ends." << " TIME = " << currentTime
            << " DT = " << dt[0] << " Sum(Height) = " << gridNew[0].sum( HEIGHT ) << std::endl;

        // sync up time
        for( ; level <= finest_level; ++level )
        {
            tNew[level] = currentTime;
        }
    }
}

void tsunami_lab::amr::AMRCoreWavePropagation2d::ErrorEst( int lev,
                                                           TagBoxArray& tags,
                                                           Real /*time*/,
                                                           int /*ngrow*/ )
{
    return; // TODO: AMR Remove to enable ErrorEst

    if( lev >= gridErr.size() ) return;

    //    const int clearval = TagBox::CLEAR;
    const int   tagval = TagBox::SET;

    const MultiFab& state = gridNew[lev];

#ifdef AMREX_USE_OMP
#pragma omp parallel
#endif
    {
        for( MFIter mfi( state, true ); mfi.isValid(); ++mfi )
        {
            const Box& bx = mfi.tilebox();
            const auto statefab = state.array( mfi );
            const auto tagfab = tags.array( mfi );
            Real gridError = gridErr[lev];

            amrex::ParallelFor( bx,
                                [=] AMREX_GPU_DEVICE( int i, int j, int k ) noexcept
            {
                state_error( i, j, k, tagfab, statefab, gridError, tagval );
            } );
        }
    }
}

void tsunami_lab::amr::AMRCoreWavePropagation2d::MakeNewLevelFromScratch( int lev,
                                                                          Real time,
                                                                          const BoxArray& ba,
                                                                          const DistributionMapping& dm )
{
    std::cout << "DEBUG 1" << std::endl;

    // init the multifab
    gridNew[lev].define( ba, dm, nComponents, nGhostRow );
    gridOld[lev].define( ba, dm, nComponents, nGhostRow );

    std::cout << "DEBUG 1.1" << std::endl;

    // set the time
    tNew[lev] = time;
    tOld[lev] = time - 1.e200; //TODO: dt[lev];

    std::cout << "DEBUG 2" << std::endl;

    if( lev > 0 )
    {
        // init the flux register
        fluxRegister[lev].reset( new FluxRegister( ba, dm, refRatio( lev - 1 ), lev, nComponents ) );
    }

    std::cout << "DEBUG 3" << std::endl;
}

void tsunami_lab::amr::AMRCoreWavePropagation2d::MakeNewLevelFromCoarse( int lev,
                                                                         Real time,
                                                                         const BoxArray& ba,
                                                                         const DistributionMapping& dm )
{
    // init the multifab
    gridNew[lev].define( ba, dm, nComponents, nGhostRow );
    gridOld[lev].define( ba, dm, nComponents, nGhostRow );

    // set the time
    tNew[lev] = time;
    tOld[lev] = time - 1.e200; //TODO: dt[lev];

    if( lev > 0 )
    {
        // init the flux register
        fluxRegister[lev].reset( new FluxRegister( ba, dm, refRatio( lev - 1 ), lev, nComponents ) );
    }

    FillCoarsePatch( lev, time, gridNew[lev], 0, nComponents );
}

void tsunami_lab::amr::AMRCoreWavePropagation2d::RemakeLevel( int lev,
                                                              Real time,
                                                              const BoxArray& ba,
                                                              const DistributionMapping& dm )
{
    MultiFab new_state( ba, dm, nComponents, nGhostRow );
    MultiFab old_state( ba, dm, nComponents, nGhostRow );

    FillPatch( lev, time, new_state, 0, nComponents );

    std::swap( new_state, gridNew[lev] );
    std::swap( old_state, gridOld[lev] );

    tNew[lev] = time;
    tOld[lev] = time - 1.e200;

    if( lev > 0 )
    {
        fluxRegister[lev].reset( new FluxRegister( ba, dm, refRatio( lev - 1 ), lev, nComponents ) );
    }
}

void tsunami_lab::amr::AMRCoreWavePropagation2d::ClearLevel( int lev )
{
    gridNew[lev].clear();
    gridOld[lev].clear();
    fluxRegister[lev].reset( nullptr );
}

void tsunami_lab::amr::AMRCoreWavePropagation2d::timeStep( t_real i_scaling )
{
}

void tsunami_lab::amr::AMRCoreWavePropagation2d::setGhostOutflow()
{
    // done by amrex auto,matically
}

tsunami_lab::t_idx tsunami_lab::amr::AMRCoreWavePropagation2d::getStride()
{
    return t_idx();
}

tsunami_lab::t_real const* tsunami_lab::amr::AMRCoreWavePropagation2d::getHeight()
{
    return nullptr;
}

tsunami_lab::t_real const* tsunami_lab::amr::AMRCoreWavePropagation2d::getMomentumX()
{
    return nullptr;
}

tsunami_lab::t_real const* tsunami_lab::amr::AMRCoreWavePropagation2d::getTotalHeight()
{
    return nullptr;
}

tsunami_lab::t_real const* tsunami_lab::amr::AMRCoreWavePropagation2d::getMomentumY()
{
    return nullptr;
}

tsunami_lab::t_real const* tsunami_lab::amr::AMRCoreWavePropagation2d::getBathymetry()
{
    return nullptr;
}

void tsunami_lab::amr::AMRCoreWavePropagation2d::setHeight( t_idx i_ix,
                                                            t_idx i_iy,
                                                            t_real i_h )
{
    setGlobalValue( gridNew[0], i_ix, i_iy, 0, HEIGHT, i_h );
}

void tsunami_lab::amr::AMRCoreWavePropagation2d::setMomentumX( t_idx i_ix,
                                                               t_idx i_iy,
                                                               t_real i_hu )
{
    setGlobalValue( gridNew[0], i_ix, i_iy, 0, MOMENTUM_X, i_hu );
}

void tsunami_lab::amr::AMRCoreWavePropagation2d::setMomentumY( t_idx i_ix,
                                                               t_idx i_iy,
                                                               t_real i_hv )
{
    setGlobalValue( gridNew[0], i_ix, i_iy, 0, MOMENTUM_Y, i_hv );
}

void tsunami_lab::amr::AMRCoreWavePropagation2d::setSolver( tsunami_lab::patches::Solver solver )
{
}

void tsunami_lab::amr::AMRCoreWavePropagation2d::setBathymetry( t_idx i_ix,
                                                                t_idx i_iy,
                                                                t_real i_b )
{
    setGlobalValue( gridNew[0], i_ix, i_iy, 0, BATHYMERTRY, i_b );
}

void tsunami_lab::amr::AMRCoreWavePropagation2d::enableBathymetry( bool enable )
{
}

void tsunami_lab::amr::AMRCoreWavePropagation2d::updateWaterHeight()
{
}

void tsunami_lab::amr::AMRCoreWavePropagation2d::setReflection( Side side,
                                                                bool enable )
{
    int dim = side == Side::BOTTOM || side == Side::TOP;
    physicalBoundary[HEIGHT].setLo( dim, enable ? BCType::reflect_even : BCType::int_dir );
    physicalBoundary[MOMENTUM_X].setLo( dim, enable ? BCType::reflect_odd : BCType::int_dir );
    physicalBoundary[MOMENTUM_Y].setLo( dim, enable ? BCType::reflect_odd : BCType::int_dir );
    physicalBoundary[HEIGHT].setLo( dim, enable ? BCType::reflect_even : BCType::int_dir );
}
