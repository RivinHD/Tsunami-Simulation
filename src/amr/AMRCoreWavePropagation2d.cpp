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
#include <filesystem> // requieres C++17 and up
namespace fs = std::filesystem;

using namespace amrex;

void tsunami_lab::amr::AMRCoreWavePropagation2d::FillFinePatch( int level,
                                                                Real time,
                                                                MultiFab& mf )
{
    BL_ASSERT( level > 0 );

    Vector<MultiFab*> cmf;
    Vector<Real> ctime;
    GetData( level - 1, time, cmf, ctime );

    CpuBndryFuncFab bndry_func( nullptr );  // Without EXT_DIR, we can pass a nullptr.
    PhysBCFunct<CpuBndryFuncFab> cphysbc( geom[level - 1], physicalBoundary, bndry_func );
    PhysBCFunct<CpuBndryFuncFab> fphysbc( geom[level], physicalBoundary, bndry_func );

    // decomp is the starting component of the destination. Therefore scomp = dcomp
    InterpFromCoarseLevel( mf, time, *cmf[0], 0, 0, 4, geom[level - 1], geom[level],
                           cphysbc, 0, fphysbc, 0, refRatio( level - 1 ),
                           interpolator, physicalBoundary, 0 );

    // do a pairwise constant interpolation to fill cell near the shore i.e. |bathymetry| < bathymetryMinValue
    MultiFab tmf( mf.boxArray(), mf.DistributionMap(), 4, mf.nGrow() );
    InterpFromCoarseLevel( tmf, time, *cmf[0], 0, 0, 4, geom[level - 1], geom[level],
                           cphysbc, 0, fphysbc, 0, refRatio( level - 1 ),
                           &pc_interp, physicalBoundary, 0 );

    FixFinePatch( mf, tmf );
}

void tsunami_lab::amr::AMRCoreWavePropagation2d::FixFinePatch( MultiFab& mf, const MultiFab& cons_mf )
{
#ifdef AMREX_USE_OMP
#pragma omp parallel
#endif
    for( MFIter mfi( mf, false ); mfi.isValid(); ++mfi )
    {
        Box bx = mfi.tilebox();

        Array4<Real> height = mf.array( mfi, HEIGHT );
        Array4<Real> momentumX = mf.array( mfi, MOMENTUM_X );
        Array4<Real> momentumY = mf.array( mfi, MOMENTUM_Y );
        Array4<Real> bathymetry = mf.array( mfi, BATHYMERTRY );
        Array4<const Real> consHeight = cons_mf.array( mfi, HEIGHT );
        Array4<const Real> consMomentumX = cons_mf.array( mfi, MOMENTUM_X );
        Array4<const Real> consMomentumY = cons_mf.array( mfi, MOMENTUM_Y );
        Array4<const Real> consBathymetry = cons_mf.array( mfi, BATHYMERTRY );

        ParallelFor( bx,
                     [=] AMREX_GPU_DEVICE( int i, int j, int k )
        {
            Real value = bathymetry( i, j, k );
            bool less = std::abs( value ) < bathymetryMinValue;
            height( i, j, k ) = less ? consHeight( i, j, k ) : height( i, j, k );
            momentumX( i, j, k ) = less ? consMomentumX( i, j, k ) : momentumX( i, j, k );
            momentumY( i, j, k ) = less ? consMomentumY( i, j, k ) : momentumY( i, j, k );
            bathymetry( i, j, k ) = less ? consBathymetry( i, j, k ) : bathymetry( i, j, k );
            height( i, j, k ) *= bathymetry( i, j, k ) < 0;  // Set height on coast to zero
        } );
    }
}

void tsunami_lab::amr::AMRCoreWavePropagation2d::GetData( int level,
                                                          Real time,
                                                          Vector<MultiFab*>& data,
                                                          Vector<Real>& datatime )
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

void tsunami_lab::amr::AMRCoreWavePropagation2d::FillPatch( int level, Real time, MultiFab& mf )
{
    if( level == 0 )
    {
        Vector<MultiFab*> smf;
        Vector<Real> stime;
        GetData( 0, time, smf, stime );

        CpuBndryFuncFab bndry_func( nullptr );  // Without EXT_DIR, we can pass a nullptr.
        PhysBCFunct<CpuBndryFuncFab> physbc( geom[level], physicalBoundary, bndry_func );
        // decomp is the starting component of the destination. Therefore scomp = dcomp
        FillPatchSingleLevel( mf, time, smf, stime, 0, 0, 4,
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

        // decomp is the starting component of the destination. Therefore scomp = dcomp
        FillPatchTwoLevels( mf, time, cmf, ctime, fmf, ftime,
                            0, 0, 4, geom[level - 1], geom[level],
                            cphysbc, 0, fphysbc, 0, refRatio( level - 1 ),
                            interpolator, physicalBoundary, 0 );

        // do a pairwise constant interpolation to fill cell near the shore i.e. |bathymetry| < bathymetryMinValue
        MultiFab tmf( mf.boxArray(), mf.DistributionMap(), 4, mf.nGrow() );
        FillPatchTwoLevels( tmf, time, cmf, ctime, fmf, ftime,
                            0, 0, 4, geom[level - 1], geom[level],
                            cphysbc, 0, fphysbc, 0, refRatio( level - 1 ),
                            &pc_interp, physicalBoundary, 0 );

        FixFinePatch( mf, tmf );
    }
}

void tsunami_lab::amr::AMRCoreWavePropagation2d::timeStepWithSubcycling( int level, Real time, int iteration )
{
    // ===== REGRID =====

    // help keep track of whether a level was already regridded
    // from a coarser level call to regrid
    static Vector<int> lastRegridStep( max_level + 1, 0 );

    // regrid changes level "level+1" so we don't regrid on max_level
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
        Print() << "[Level " << level << " step " << step[level] + 1 << "] ";
        Print() << "ADVANCE with time = " << tNew[level] << " dt = " << dt[level] << std::endl;
    }

    // ===== ADVANCE =====

    // Advance a single level for a single time step, and update flux registers
    tOld[level] = tNew[level];
    tNew[level] += dt[level];

    AdvanceGridAtLevel( level, time, dt[level], iteration, nSubSteps[level] );

    ++step[level];

    if( Verbose() )
    {
        Print() << "[Level " << level << " step " << step[level] << "] ";
        Print() << "Advanced " << CountCells( level ) << " cells" << std::endl;
    }

    // ===== SUBCYCLING =====

    if( level < finest_level )
    {
        // recursive call for next-finer level
        for( int i = 1; i <= nSubSteps[level + 1]; ++i )
        {
            timeStepWithSubcycling( level + 1, time + ( i - 1 ) * dt[level + 1], i );
        }

        // average level+1 down to level
        AverageDownTo( level );
    }
}

void tsunami_lab::amr::AMRCoreWavePropagation2d::AdvanceGridAtLevel( int level,
                                                                     Real time,
                                                                     Real dtLevel,
                                                                     int /*iteration*/,
                                                                     int /*nCycle*/ )
{
    std::swap( gridOld[level], gridNew[level] );

    MultiFab& state = gridNew[level];

    // size in x & y direction
    const Real dx = geom[level].CellSize( 0 );
    const Real dy = geom[level].CellSize( 1 );

    // scaling in each dimension
    Real dtdx = dtLevel / dx;
    Real dtdy = dtLevel / dy;

    // State with ghost cells
    MultiFab stateTemp( grids[level], dmap[level], 4, nGhostRow );
    FillPatch( level, time, stateTemp );
    state.ParallelCopy( stateTemp, 0, 0, 4, nGhostRow, nGhostRow );

#ifdef AMREX_USE_OMP
#pragma omp parallel
#endif
    for( MFIter mfi( state, false ); mfi.isValid(); ++mfi )
    {
        // ===== COPY AND UPDATE X SWEEP =====
        const Box& bx = mfi.tilebox();

        // define the grid components and fluxes
        Array4<Real const> height = state.const_array( mfi, HEIGHT );
        Array4<Real const> momentumX = state.const_array( mfi, MOMENTUM_X );
        Array4<Real const> bathymetry = state.const_array( mfi, BATHYMERTRY );
        Array4<Real      > gridOut = stateTemp.array( mfi );

        // compute the x-sweep and the x fluxes
        launch( grow( bx, 1 ),
                [=] AMREX_GPU_DEVICE( const Box & tbx )
        {
            xSweep( tbx, dtdx, height, momentumX, bathymetry, gridOut );
        } );
    }

    state.ParallelCopy( stateTemp, 0, 0, 4, 0, 0 );
    state.FillBoundary();

#ifdef AMREX_USE_OMP
#pragma omp parallel
#endif
    for( MFIter mfi( state, true ); mfi.isValid(); ++mfi )
    {
        // ===== UPDATE Y SWEEP AND SCALE FLUX =====
        const Box& bx = mfi.tilebox();

        // swap the grid components
        Array4<Real const> height = stateTemp.const_array( mfi, HEIGHT );
        Array4<Real const> momentumY = stateTemp.const_array( mfi, MOMENTUM_Y );
        Array4<Real const> bathymetry = stateTemp.const_array( mfi, BATHYMERTRY );
        Array4<Real      > gridOut = state.array( mfi );

        // compute the y-sweep and the y fluxes
        launch( grow( bx, 1 ),
                [=] AMREX_GPU_DEVICE( const Box & tbx )
        {
            ySweep( tbx, dtdy, height, momentumY, bathymetry, gridOut );
        } );
    }
}

void tsunami_lab::amr::AMRCoreWavePropagation2d::WritePlotFile() const
{
    const fs::path file( Concatenate( plotFile, step[0], 5 ) );
    const fs::path dir( plotFolder );
    fs::path fullpath = dir / file;

    Vector<const MultiFab*> mf;
    for( int i = 0; i <= finest_level; ++i )
    {
        mf.push_back( &gridNew[i] );
    }
    Vector<std::string> varnames = { "Height", "MomentumX", "MomentumY", "Bathymetry", "Error" };

    Print() << "Writing plotfile " << fullpath << std::endl;

    WriteMultiLevelPlotfile( fullpath.string(), finest_level + 1, mf, varnames,
                             geom, tNew[0], step, refRatio() );
}

void tsunami_lab::amr::AMRCoreWavePropagation2d::AverageDownTo( int coarseLevel )
{
    // Average down the first 3 Components: Height, MomentumX, MomentumY
    average_down( gridNew[coarseLevel + 1], gridNew[coarseLevel],
                  geom[coarseLevel + 1], geom[coarseLevel],
                  0, 3, refRatio( coarseLevel ) );
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
        pp.query( "plot_file", plotFile );
        pp.query( "plot_folder", plotFolder );
        pp.query( "plot_int", writeFrequency );

        Vector<int> errorBuf;
        int n = pp.countval( "n_err_buf" );
        if( n > 0 )
        {
            pp.getarr( "n_err_buf", errorBuf, 0, n );
        }
        n_error_buf.resize( n );
        for( int i = 0; i < n; i++ )
        {
            n_error_buf[i] = IntVect( errorBuf[i] );
        }
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

void tsunami_lab::amr::AMRCoreWavePropagation2d::InitData( int level )
{
    // size in x & y direction
    const Real dx = geom[level].CellSize( 0 );
    const Real dy = geom[level].CellSize( 1 );

#ifdef AMREX_USE_OMP
#pragma omp parallel
#endif
    for( MFIter mfi( gridNew[level], true ); mfi.isValid(); ++mfi )
    {
        Box bx = mfi.tilebox();

        Array4<Real> height = gridNew[level].array( mfi, HEIGHT );
        Array4<Real> momentumX = gridNew[level].array( mfi, MOMENTUM_X );
        Array4<Real> momentumY = gridNew[level].array( mfi, MOMENTUM_Y );
        Array4<Real> bathymetry = gridNew[level].array( mfi, BATHYMERTRY );

        ParallelFor( bx,
                     [=] AMREX_GPU_DEVICE( int i, int j, int k )
        {
            Real x = i * dx;
            Real y = j * dy;
            height( i, j, k ) = setup->getHeight( x, y );
            momentumX( i, j, k ) = setup->getMomentumX( x, y );
            momentumY( i, j, k ) = setup->getMomentumY( x, y );
            bathymetry( i, j, k ) = setup->getBathymetry( x, y );
        } );
    }
    Real hMax = gridNew[level].max( HEIGHT );
    Real speedMax = std::sqrt( 9.81 * hMax );
    std::cout << "Max speed " << speedMax << std::endl;

    Real dxy = std::min( geom[level].CellSize( 0 ), geom[level].CellSize( 1 ) );
    Real dt = 0.45 * dxy / speedMax;
    setTimeStep( dt, level );
}

tsunami_lab::amr::AMRCoreWavePropagation2d::AMRCoreWavePropagation2d( tsunami_lab::setups::Setup* setup )
    : setup( setup )
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
    physicalBoundary.resize( 4 );

    // set the refinement ratio for each level for subcycling
    for( int lev = 1; lev <= max_level; ++lev )
    {
        nSubSteps[lev] = MaxRefRatio( lev - 1 );
    }

    // set the interpolation method
    for( int dim = 0; dim < AMREX_SPACEDIM; ++dim )
    {
        for( int n = 0; n < 4; ++n )
        {
            physicalBoundary[n].setLo( dim, BCType::foextrap );
            physicalBoundary[n].setHi( dim, BCType::foextrap );
        }
    }

    // init the domain
    InitFromScratch( 0.0 );
}

void tsunami_lab::amr::AMRCoreWavePropagation2d::setTimeStep( Real timeStep, int level )
{
    tOld[level] = tNew[level] - timeStep;
    dt[level] = timeStep;
}

void tsunami_lab::amr::AMRCoreWavePropagation2d::Evolve()
{
    Real currentTime = tNew[0];
    int writes = 0;

    for( int iStep = step[0]; currentTime < simulationTime; iStep++ )
    {
        if( currentTime >= writes * writeFrequency )
        {
            writes++;
            WritePlotFile();
        }

        Print() << "\nCoarse STEP " << iStep + 1 << " starts ..." << std::endl;

        // do time step with subcycling
        int level = 0;
        int iteration = 1;
        timeStepWithSubcycling( level, currentTime, iteration );

        // advance time
        currentTime += dt[0];

        Print() << "Coarse STEP " << iStep + 1 << " ends." << " TIME = " << currentTime
            << " DT = " << dt[0] << " Sum(Height) = " << gridNew[0].sum( HEIGHT ) << std::endl;

        // sync up time
        for( ; level <= finest_level; ++level )
        {
            tNew[level] = currentTime;
        }
    }
}

void tsunami_lab::amr::AMRCoreWavePropagation2d::ErrorEst( int level,
                                                           TagBoxArray& tags,
                                                           Real /*time*/,
                                                           int /*ngrow*/ )
{
    if( level >= gridErr.size() ) return;

    const int tagval = TagBox::SET;
    MultiFab& state = gridNew[level];
    Real gridError = gridErr[level] * gridErr[level];

#ifdef AMREX_USE_OMP
#pragma omp parallel
#endif
    for( MFIter mfi( state, false ); mfi.isValid(); ++mfi )
    {
        const Box& bx = mfi.tilebox();

        Array4<const Real> height = state.const_array( mfi, HEIGHT );
        Array4<const Real> momentumX = state.const_array( mfi, MOMENTUM_X );
        Array4<const Real> momentumY = state.const_array( mfi, MOMENTUM_Y );
        Array4<const Real> bathymetry = state.const_array( mfi, BATHYMERTRY );
        Array4<Real> error = state.array( mfi, ERROR );
        const auto tagfab = tags.array( mfi );

        ParallelFor( bx,
                     [=] AMREX_GPU_DEVICE( int i, int j, int k ) noexcept
        {
            state_error( i, j, k, tagfab, height, momentumX, momentumY, bathymetry, error, gridError, tagval );
        } );
    }
}

void tsunami_lab::amr::AMRCoreWavePropagation2d::MakeNewLevelFromScratch( int level,
                                                                          Real time,
                                                                          const BoxArray& ba,
                                                                          const DistributionMapping& dm )
{

    // init the multifab
    gridNew[level].define( ba, dm, nComponents, nGhostRow );
    gridOld[level].define( ba, dm, nComponents, nGhostRow );

    // set the time
    tNew[level] = time;
    tOld[level] = time - dt[level];

    InitData( level );
}

void tsunami_lab::amr::AMRCoreWavePropagation2d::MakeNewLevelFromCoarse( int level,
                                                                         Real time,
                                                                         const BoxArray& ba,
                                                                         const DistributionMapping& dm )
{
    // init the multifab
    gridNew[level].define( ba, dm, nComponents, nGhostRow );
    gridOld[level].define( ba, dm, nComponents, nGhostRow );

    // set the time
    tNew[level] = time;
    tOld[level] = time - dt[level];

    FillFinePatch( level, time, gridNew[level] );
}

void tsunami_lab::amr::AMRCoreWavePropagation2d::RemakeLevel( int level,
                                                              Real time,
                                                              const BoxArray& ba,
                                                              const DistributionMapping& dm )
{
    MultiFab new_state( ba, dm, nComponents, nGhostRow );
    MultiFab old_state( ba, dm, nComponents, nGhostRow );

    FillPatch( level, time, new_state );

    std::swap( new_state, gridNew[level] );
    std::swap( old_state, gridOld[level] );

    tNew[level] = time;
    tOld[level] = time - dt[level];
}

void tsunami_lab::amr::AMRCoreWavePropagation2d::ClearLevel( int level )
{
    gridNew[level].clear();
    gridOld[level].clear();
}

void tsunami_lab::amr::AMRCoreWavePropagation2d::setReflection( tsunami_lab::patches::WavePropagation::Side side,
                                                                bool enable )
{
    int dim = ( side == tsunami_lab::patches::WavePropagation::Side::BOTTOM
                || side == tsunami_lab::patches::WavePropagation::Side::TOP );
    if( side == tsunami_lab::patches::WavePropagation::Side::BOTTOM
        || side == tsunami_lab::patches::WavePropagation::Side::LEFT )
    {
        physicalBoundary[HEIGHT].setLo( dim, enable ? BCType::reflect_even : BCType::foextrap );
        physicalBoundary[MOMENTUM_X].setLo( dim, enable ? BCType::reflect_odd : BCType::foextrap );
        physicalBoundary[MOMENTUM_Y].setLo( dim, enable ? BCType::reflect_odd : BCType::foextrap );
        physicalBoundary[BATHYMERTRY].setLo( dim, enable ? BCType::reflect_even : BCType::foextrap );
    }
    else
    {
        physicalBoundary[HEIGHT].setHi( dim, enable ? BCType::reflect_even : BCType::foextrap );
        physicalBoundary[MOMENTUM_X].setHi( dim, enable ? BCType::reflect_odd : BCType::foextrap );
        physicalBoundary[MOMENTUM_Y].setHi( dim, enable ? BCType::reflect_odd : BCType::foextrap );
        physicalBoundary[BATHYMERTRY].setHi( dim, enable ? BCType::reflect_even : BCType::foextrap );
    }
}
