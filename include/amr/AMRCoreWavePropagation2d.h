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
class tsunami_lab::amr::AMRCoreWavePropagation2d : public amrex::AmrCore
{
private:
    //! number of components i.e. Height, MomentumX, MomentumY, Bathymetry
    const int nComponents = 5;

    //! number of ghost cell around the boundary of the domain
    const int nGhostRow = 1;

    //! Minimum Height of the bathymetry
    const amrex::Real bathymetryMinValue = 20;

    //! interpolator going from coarse to fine
    // this should never be const
    amrex::Interpolater* interpolator = &amrex::protected_interp;

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

    //! Stores fluxes at coarse-fine interface for synchronization
    //! This will be sized "nlevs_max+1"
    //! NOTE: The flux register associated with flux_reg[level] is associated
    //! with the level/level-1 interface (and has grid spacing associated with level-1)
    //! Therefore flux_reg[0] and flux_reg[nlevs_max] are never actually
    //! used in the reflux operation
    amrex::Vector<std::unique_ptr<amrex::FluxRegister>> fluxRegister;

    //! time to simulate
    amrex::Real simulationTime = std::numeric_limits<amrex::Real>::max();

    //! how often each level regrids the higher levels of refinement
    //! (after a level advances that many time steps)
    int regridFrequency = 2;

    //! root name of plot file
    std::string plotFile{ "plt" };

    //! folder for plot files
    std::string plotFolder{ "solution" };

    //! frequency to write the output
    int writeFrequency = -1;

    //! the error bound for each level
    amrex::Vector<amrex::Real> gridErr;

    //! the setup to init the data with
    tsunami_lab::setups::Setup* setup;

    /**
     * Fill an entire multifab by interpolating from the coarser level
     * This comes into play when a new level of refinement appears
     *
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

    /**
     * Fix an entire multifab that was interpolating from the coarser level
     * This comes into play when FillCoarsePath was called
     *
     * @param level the level to fill
     * @param mf the multiFab to interpolate to
    */
    void FixFinePatch( int level,
                       amrex::MultiFab& mf );

    /**
     * Utility to copy in data from gridOld and/or gridNew into another multifab
     *
     * @param level the level to get data from
     * @param time the current time
     * @param data the MutliFab to which the data is written
     * @param datatime the Vector to which the time is written
    */
    void GetData( int level, amrex::Real time, amrex::Vector<amrex::MultiFab*>& data,
                  amrex::Vector<amrex::Real>& datatime );

    /**
     * Fill a patch with data from the grid
     *
     * @param level the level to fill
     * @param time the current time
     * @param mf the multifab to fill
     * @param icomp component index to start filling
     * @param ncomp number of components of the multifab
    */
    void FillPatch( int level, amrex::Real time, amrex::MultiFab& mf, int icomp, int ncomp );

    /**
     * Advance a level by dt - includes a recursive call for finer levels
     *
     * @param level the level to sub cycle
     * @param time the current time
     * @param iteration the current iteration step
    */
    void timeStepWithSubcycling( int level,
                                 amrex::Real time,
                                 int iteration );

    /**
    * Advance phi at a single level for a single time step, update flux registers
    *
    * @param level the level to advance
    * @param time the current time
    * @param dtLevel the time step of this level
    * @param iteration the current iteration
    * @param the current cycle step
    */
    void AdvanceGridAtLevel( int level,
                             amrex::Real time,
                             amrex::Real dtLevel,
                             int iteration,
                             int nCycle );

    /**
     * Write plotfile to disk
    */
    void WritePlotFile() const;

    /**
     * More flexible version of AverageDown() that lets you average down across multiple levels
     *
     * @param coarseLevel the level to average down to
    */
    void AverageDownTo( int coarseLevel );

    /**
     * Read in parameters from input file
    */
    void ReadParameters();

    /**
     * Read the data from the setup into the grid
     *
     * @param level the level to setup
    */
    void InitData( int level );

public:
    /**
     * The Components that is stored in the MutliFab grid
    */
    enum Component
    {
        HEIGHT = 0,
        MOMENTUM_X = 1,
        MOMENTUM_Y = 2,
        BATHYMERTRY = 3,
        ERROR = 4
    };

    /**
     * Construct Wave Propagation with AMR using the given setup
     *
     * @param setup the setup to initialize the data with
    */
    AMRCoreWavePropagation2d( tsunami_lab::setups::Setup* setup );

    /**
     * Set the time step of the simulation
     *
     * @param timeStep set the timeStep to step forward
     * @param level the level to set
    */
    void setTimeStep( amrex::Real timeStep, int level );

    /**
     * Advance solution to final time
    */
    void Evolve();

    /**
     * Tag cells for refinement. TagBoxArray tags is built on level level grids.
     *
     * @param level the level to estimate and tag
     * @param tags the box that holds the tag values
     * @param time the current time
     * @param ngrow the ghost cells need for estimation calculation
    */
    void ErrorEst( int level,
                   amrex::TagBoxArray& tags,
                   amrex::Real time,
                   int ngrow );

    /**
     * Make a new level from scratch using provided BoxArray and DistributionMapping.
     * Only used during initialization.
     *
     * @param level the level to make from scratch
     * @param time the current time
     * @param ba the provided BoxArray
     * @param dm the provided DistributionMapping
    */
    void MakeNewLevelFromScratch( int level,
                                  amrex::Real time,
                                  const amrex::BoxArray& ba,
                                  const amrex::DistributionMapping& dm );

    /**
     * Make a new level using provided BoxArray and DistributionMapping and fill
     * with interpolated coarse level data.
     *
     * @param level the level to make
     * @param time the current time
     * @param ba the provided BoxArray
     * @param dm the provided DistributionMapping
    */
    void MakeNewLevelFromCoarse( int level,
                                 amrex::Real time,
                                 const amrex::BoxArray& ba,
                                 const amrex::DistributionMapping& dm );

    /**
     * Remake an existing level using provided BoxArray and DistributionMapping
     * and fill with existing fine and coarse data.
     *
     * @param level the level to Remake
     * @param time the current time
     * @param ba the provided BoxArray
     * @param dm the provided DistributionMapping
    */
    void RemakeLevel( int level,
                      amrex::Real time,
                      const amrex::BoxArray& ba,
                      const amrex::DistributionMapping& dm );

    /**
     * Delete level data
     *
     * @param level the level to delete
    */
    void ClearLevel( int level );

    /**
     * Set the reflection of the physical boundary of a given side
     *
     * @param side the side to set
     * @param enable true if reflection should be enabled, false for outflow boundary
    */
    void setReflection( tsunami_lab::patches::WavePropagation::Side side,
                        bool enable );
};


#endif //TSUNAMI_SIMULATION_AMRCOREWAVEPROPAGATION2D_H
