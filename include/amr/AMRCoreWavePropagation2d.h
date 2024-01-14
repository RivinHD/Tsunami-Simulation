/**
 * @author Fabian Hofer, Vincent Gerlach
 *
 * Two-dimensional adaptive mesh refinement wave propagation.
 **/

#ifndef TSUNAMI_SIMULATION_AMRCOREWAVEPROPAGATION2D_H
#define TSUNAMI_SIMULATION_AMRCOREWAVEPROPAGATION2D_H

#include <AMReX_AmrCore.H>
#include <AMReX_BCRec.H>
#include <AMReX_FluxRegister.H>
#include <AMReX_FillPatcher.H>

#include "../../include/patches/WavePropagation.h"

namespace tsunami_lab
{
    namespace amr
    {
        class AMRCoreWavePropagation2d;
    }
}

class tsunami_lab::amr::AMRCoreWavePropagation2d : public amrex::AmrCore, public tsunami_lab::patches::WavePropagation
{
    //! Tag cells for refinement. TagBoxArray tags is built on level lev grids.
    void ErrorEst (int lev, TagBoxArray& tags, Real time, int ngrow);

    //! Make a new level from scratch using provided BoxArray and DistributionMapping.
    //! Only used during initialization.
    void MakeNewLevelFromScratch (int lev, Real time, const BoxArray& ba, const DistributionMapping& dm);

    //! Make a new level using provided BoxArray and DistributionMapping and fill
    //  with interpolated coarse level data.
    void MakeNewLevelFromCoarse (int lev, Real time, const BoxArray& ba, const DistributionMapping& dm);

    //! Remake an existing level using provided BoxArray and DistributionMapping
    //  and fill with existing fine and coarse data.
    void RemakeLevel (int lev, Real time, const BoxArray& ba, const DistributionMapping& dm);

    //! Delete level data
    void ClearLevel (int lev);
};


#endif //TSUNAMI_SIMULATION_AMRCOREWAVEPROPAGATION2D_H
