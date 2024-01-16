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

using namespace amrex;

void tsunami_lab::amr::AMRCoreWavePropagation2d::FillCoarsePatch( int lev,
                                                                  amrex::Real time,
                                                                  amrex::MultiFab& mf,
                                                                  int icomp,
                                                                  int ncomp )
{
    BL_ASSERT( lev > 0 );

    Vector<MultiFab*> cmf;
    Vector<Real> ctime;
    GetData( lev - 1, time, cmf, ctime );

    if( cmf.size() != 1 )
    {
        amrex::Abort( "FillCoarsePatch: how did this happen?" );
    }

    CpuBndryFuncFab bndry_func( nullptr );  // Without EXT_DIR, we can pass a nullptr.
    PhysBCFunct<CpuBndryFuncFab> cphysbc( geom[lev - 1], physicalBoundary, bndry_func );
    PhysBCFunct<CpuBndryFuncFab> fphysbc( geom[lev], physicalBoundary, bndry_func );

    amrex::InterpFromCoarseLevel( mf, time, *cmf[0], 0, icomp, ncomp, geom[lev - 1], geom[lev],
                                  cphysbc, 0, fphysbc, 0, refRatio( lev - 1 ),
                                  interpolator, physicalBoundary, 0 );
}

void tsunami_lab::amr::AMRCoreWavePropagation2d::GetData( int lev,
                                                          amrex::Real time,
                                                          amrex::Vector<amrex::MultiFab*>& data,
                                                          amrex::Vector<amrex::Real>& datatime )
{
    data.clear();
    datatime.clear();

    const Real teps = ( tNew[lev] - tOld[lev] ) * 1.e-3;

    if( time > tNew[lev] - teps && time < tNew[lev] + teps )
    {
        data.push_back( &gridNew[lev] );
        datatime.push_back( tNew[lev] );
    }
    else if( time > tOld[lev] - teps && time < tOld[lev] + teps )
    {
        data.push_back( &gridOld[lev] );
        datatime.push_back( tOld[lev] );
    }
    else
    {
        data.push_back( &gridOld[lev] );
        data.push_back( &gridNew[lev] );
        datatime.push_back( tOld[lev] );
        datatime.push_back( tNew[lev] );
    }
}

void tsunami_lab::amr::AMRCoreWavePropagation2d::FillPatch( int lev, amrex::Real time, amrex::MultiFab& mf, int icomp, int ncomp )
{
    if( lev == 0 )
    {
        Vector<MultiFab*> smf;
        Vector<Real> stime;
        GetData( 0, time, smf, stime );

        CpuBndryFuncFab bndry_func( nullptr );  // Without EXT_DIR, we can pass a nullptr.
        PhysBCFunct<CpuBndryFuncFab> physbc( geom[lev], physicalBoundary, bndry_func );
        amrex::FillPatchSingleLevel( mf, time, smf, stime, 0, icomp, ncomp,
                                     geom[lev], physbc, 0 );
    }
    else
    {
        Vector<MultiFab*> cmf, fmf;
        Vector<Real> ctime, ftime;
        GetData( lev - 1, time, cmf, ctime );
        GetData( lev, time, fmf, ftime );

        CpuBndryFuncFab bndry_func( nullptr );  // Without EXT_DIR, we can pass a nullptr.
        PhysBCFunct<CpuBndryFuncFab> cphysbc( geom[lev - 1], physicalBoundary, bndry_func );
        PhysBCFunct<CpuBndryFuncFab> fphysbc( geom[lev], physicalBoundary, bndry_func );

        amrex::FillPatchTwoLevels( mf, time, cmf, ctime, fmf, ftime,
                                   0, icomp, ncomp, geom[lev - 1], geom[lev],
                                   cphysbc, 0, fphysbc, 0, refRatio( lev - 1 ),
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

tsunami_lab::amr::AMRCoreWavePropagation2d::AMRCoreWavePropagation2d( t_idx nx,
                                                                      t_idx ny,
                                                                      amrex::Real timeStep )
{
    // set inital paramters
    verbose = 0;
    max_level = 0;
    ref_ratio = { IntVect( 2 ) };
    blocking_factor = { IntVect( 8 ) };
    max_grid_size = { IntVect( 128 ) };
    n_error_buf = { IntVect( 1 ) };
    grid_eff = static_cast<Real>( 0.7 );
    n_proper = 1;
    use_fixed_upto_level = 0;
    use_fixed_coarse_grids = false;
    refine_grid_layout = true;
    refine_grid_layout_dims = IntVect( 1 );
    check_input = true;
    use_new_chop = false;
    iterate_on_new_grids = true;

    // resize to the levels
    int nLevelsMax = max_level + 1;
    step.resize( nLevelsMax, 0 );
    nSubSteps.resize( nLevelsMax, 1 );
    tNew.resize( nLevelsMax, 0. );
    tOld.resize( nLevelsMax, -timeStep );
    dt.resize( nLevelsMax, timeStep );
    gridNew.resize( nLevelsMax );
    gridOld.resize( nLevelsMax );
    physicalBoundary.resize( nComponents );
    fluxRegister.resize( nLevelsMax + 1 );

    // set the refinement ratio for each level for subcyling
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

    // creates the domain
    IntVect domLow( 0, 0 );
    IntVect domHigh( nx - 1, ny - 1 );
    Box domain = Box( domLow, domHigh );

    // defines the physical box size in each direction
    RealBox realBox( { 0., 0. },
                     { 1., 1. } );

    // geometry object for real data and set it to the coarse level
    SetGeometry( 0, Geometry( domain, realBox, CoordSys::cartesian, { 0, 0 } ) );

    // init the domain
    InitFromScratch( 0.0 );
}

void tsunami_lab::amr::AMRCoreWavePropagation2d::ErrorEst( int lev,
                                                           TagBoxArray& tags,
                                                           Real time,
                                                           int ngrow )
{
    static Vector<Real> gridErr( { 1.01, 1.1, 1.5 } );
    return; // TODO: AMR Remove to enable ErrorEst

    if( lev >= gridErr.size() ) return;

    //    const int clearval = TagBox::CLEAR;
    const int   tagval = TagBox::SET;

    const MultiFab& state = gridNew[lev];

#ifdef AMREX_USE_OMP
#pragma omp parallel if(Gpu::notInLaunchRegion())
#endif
    {
        for( MFIter mfi( state, TilingIfNotGPU() ); mfi.isValid(); ++mfi )
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
    setGlobalValue( gridNew[0], i_ix, i_iy, 0, Component::HEIGHT, i_h );
}

void tsunami_lab::amr::AMRCoreWavePropagation2d::setMomentumX( t_idx i_ix,
                                                               t_idx i_iy,
                                                               t_real i_hu )
{
    setGlobalValue( gridNew[0], i_ix, i_iy, 0, Component::MOMENTUM_X, i_hu );
}

void tsunami_lab::amr::AMRCoreWavePropagation2d::setMomentumY( t_idx i_ix,
                                                               t_idx i_iy,
                                                               t_real i_hv )
{
    setGlobalValue( gridNew[0], i_ix, i_iy, 0, Component::MOMENTUM_Y, i_hv );
}

void tsunami_lab::amr::AMRCoreWavePropagation2d::setSolver( tsunami_lab::patches::Solver solver )
{
}

void tsunami_lab::amr::AMRCoreWavePropagation2d::setBathymetry( t_idx i_ix,
                                                                t_idx i_iy,
                                                                t_real i_b )
{
    setGlobalValue( gridNew[0], i_ix, i_iy, 0, Component::BATHYMERTRY, i_b );
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
    physicalBoundary[Component::HEIGHT].setLo( dim, enable ? BCType::reflect_even : BCType::int_dir );
    physicalBoundary[Component::MOMENTUM_X].setLo( dim, enable ? BCType::reflect_odd : BCType::int_dir );
    physicalBoundary[Component::MOMENTUM_Y].setLo( dim, enable ? BCType::reflect_odd : BCType::int_dir );
    physicalBoundary[Component::HEIGHT].setLo( dim, enable ? BCType::reflect_even : BCType::int_dir );
}
