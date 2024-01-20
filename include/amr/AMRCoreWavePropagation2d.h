/**
 * @author Fabian Hofer, Vincent Gerlach
 *
 * Two-dimensional adaptive mesh refinement wave propagation.
 **/

#ifndef TSUNAMI_SIMULATION_AMRCOREWAVEPROPAGATION2D_H
#define TSUNAMI_SIMULATION_AMRCOREWAVEPROPAGATION2D_H

#ifdef AMREX_USE_OMP
#include <omp.h>
#endif

#include <AMReX_AmrCore.H>
#include <AMReX_FluxRegister.H>
#include <AMReX_BCRec.H>
#include <AMReX_Interpolater.H>

#include "../../include/patches/WavePropagation.h"
#include "../../include/setups/Setup.h"

namespace tsunami_lab
{
    namespace amr
    {
        class AMRCoreWavePropagation2d;
    }
}

/**
 * Two-dimensional adaptiv multi resolution wave propagation
*/
class tsunami_lab::amr::AMRCoreWavePropagation2d : public amrex::AmrCore, public tsunami_lab::patches::WavePropagation
{
private:
    //! number of components i.e. Height, MomentumX, MomentumY, Bathymetry
    const int nComponents = 4;

    //! number of ghost cell around the boundary of the domain
    const int nGhostRow = 1;

    //! interpolator going from coarse to fine
    // this should never be const
    amrex::Interpolater* interpolator = &amrex::lincc_interp;

    //! which step?
    amrex::Vector<int> step;

    //! how many substeps on each level?
    amrex::Vector<int> nSubSteps;

    //! keep track of new time step at each level
    amrex::Vector<amrex::Real> tNew;

    //! keep track of old time step at each level
    amrex::Vector<amrex::Real> tOld;

    //! keep track of time step at each level
    amrex::Vector<amrex::Real> dt;

    //! array of multifabs to store the solution at each level of refinement
    //! after advancing a level we use "swap"
    amrex::Vector<amrex::MultiFab> gridNew;

    //! array of multifabs to store the solution at each level of refinement
    //! after advancing a level we use "swap"
    amrex::Vector<amrex::MultiFab> gridOld;

    //! this is essentially a 2*DIM integer array storing the physical boundary
    //! condition types at the lo/hi walls in each direction
    //! 4-components: Height, MomentumX, MomentumY, Bathymetry
    amrex::Vector<amrex::BCRec> physicalBoundary;

    // stores fluxes at coarse-fine interface for synchronization
    // this will be sized "nlevs_max+1"
    // NOTE: the flux register associated with flux_reg[lev] is associated
    // with the lev/lev-1 interface (and has grid spacing associated with lev-1)
    // therefore flux_reg[0] and flux_reg[nlevs_max] are never actually
    // used in the reflux operation
    amrex::Vector<std::unique_ptr<amrex::FluxRegister>> fluxRegister;

    //! time to simulate
    amrex::Real simulationTime = std::numeric_limits<amrex::Real>::max();

    // how often each level regrids the higher levels of refinement
    // (after a level advances that many time steps)
    int regridFrequency = 2;

    //! frequency to write the output
    int writeFrequency = -1;

    amrex::Vector<amrex::Real> gridErr;

    /**
     * fill an entire multifab by interpolating from the coarser level
     * this comes into play when a new level of refinement appears
     * @param level the level to fill
     * @param time the current time
     * @param mf the multiFab to interpolate to
     * @param icomp component index to start interpolating
     * @param ncomp number of components of the multifab
    */
    void FillCoarsePatch( int level,
                          amrex::Real time,
                          amrex::MultiFab& mf,
                          int icomp,
                          int ncomp );

    // 

    /**
     * utility to copy in data from gridOld and/or gridNew into another multifab
     * @param level the level to get data from
     * @param time the current time
     * @param data
     * @param datatime
    */
    void GetData( int level, amrex::Real time, amrex::Vector<amrex::MultiFab*>& data,
                  amrex::Vector<amrex::Real>& datatime );

    // compute a new multifab by coping in phi from valid region and filling ghost cells
    // works for single level and 2-level cases (fill fine grid ghost by interpolating from coarse)

    /**
     * Fill a patch with data from the grid
     * @param level the level to fill
     * @param time the current time
     * @param mf the multifab to fill
     * @param icomp component index to start filling
     * @param ncomp number of components of the multifab
    */
    void FillPatch( int level, amrex::Real time, amrex::MultiFab& mf, int icomp, int ncomp );

    void setGlobalValue( amrex::MultiFab& mf,
                         int x,
                         int y,
                         int z,
                         int comp,
                         amrex::Real value );

    // Advance a level by dt - includes a recursive call for finer levels
    void timeStepWithSubcycling( int level,
                                 amrex::Real time,
                                 int iteration );

    // Advance phi at a single level for a single time step, update flux registers
    void AdvanceGridAtLevel( int level,
                             amrex::Real time,
                             amrex::Real dtLevel,
                             int iteration,
                             int nCycle );

    // write plotfile to disk
    void WritePlotFile() const;

    // more flexible version of AverageDown() that lets you average down across multiple levels
    void AverageDownTo( int coarse_lev );

    // read in some parameters from inputs file
    void ReadParameters();

    void InitData( tsunami_lab::setups::Setup* setup );

public:
    /**
     * The Components that is stored in the MutliFab grid
    */
    enum Component
    {
        HEIGHT = 0,
        MOMENTUM_X = 1,
        MOMENTUM_Y = 2,
        BATHYMERTRY = 3
    };

    AMRCoreWavePropagation2d( tsunami_lab::setups::Setup* setup );

    // set the time step of the simulation
    void setTimeStep( amrex::Real timeStep );

    // advance solution to final time
    void Evolve();

    //! Tag cells for refinement. TagBoxArray tags is built on level lev grids.
    void ErrorEst( int lev, amrex::TagBoxArray& tags,
                   amrex::Real time,
                   int ngrow );

    //! Make a new level from scratch using provided BoxArray and DistributionMapping.
    //! Only used during initialization.
    void MakeNewLevelFromScratch( int lev,
                                  amrex::Real time,
                                  const amrex::BoxArray& ba,
                                  const amrex::DistributionMapping& dm );

    //! Make a new level using provided BoxArray and DistributionMapping and fill
    //  with interpolated coarse level data.
    void MakeNewLevelFromCoarse( int lev,
                                 amrex::Real time,
                                 const amrex::BoxArray& ba,
                                 const amrex::DistributionMapping& dm );

    //! Remake an existing level using provided BoxArray and DistributionMapping
    //  and fill with existing fine and coarse data.
    void RemakeLevel( int lev,
                      amrex::Real time,
                      const amrex::BoxArray& ba,
                      const amrex::DistributionMapping& dm );

    //! Delete level data
    void ClearLevel( int lev );

    void timeStep( t_real i_scaling ) override;
    void setGhostOutflow() override;
    t_idx getStride() override;
    t_real const* getHeight() override;
    t_real const* getMomentumX() override;
    t_real const* getTotalHeight() override;
    t_real const* getMomentumY() override;
    t_real const* getBathymetry() override;
    void setHeight( t_idx i_ix,
                    t_idx i_iy,
                    t_real i_h ) override;
    void setMomentumX( t_idx i_ix,
                       t_idx i_iy,
                       t_real i_hu ) override;
    void setMomentumY( t_idx i_ix,
                       t_idx i_iy,
                       t_real i_hv ) override;
    void setSolver( tsunami_lab::patches::Solver solver ) override;
    void setBathymetry( t_idx i_ix,
                        t_idx i_iy,
                        t_real i_b ) override;
    void enableBathymetry( bool enable ) override;
    void updateWaterHeight() override;
    void setReflection( Side side,
                        bool enable ) override;
};


#endif //TSUNAMI_SIMULATION_AMRCOREWAVEPROPAGATION2D_H