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
    AMRCoreWavePropagation2d( tsunami_lab::t_real nx,
                              tsunami_lab::t_real ny );

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
