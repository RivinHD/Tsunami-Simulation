/**
 * @author Fabian Hofer, Vincent Gerlach
 *
 * Two-dimensional adaptive mesh refinement wave propagation.
 **/

#include "../../include/amr/AMRCoreWavePropagation2d.h"

using namespace amrex;

tsunami_lab::amr::AMRCoreWavePropagation2d::AMRCoreWavePropagation2d( tsunami_lab::t_real nx,
                                                                      tsunami_lab::t_real ny )
{
}

void tsunami_lab::amr::AMRCoreWavePropagation2d::ErrorEst( int lev,
                                                           TagBoxArray& tags,
                                                           Real time,
                                                           int ngrow )
{
}

void tsunami_lab::amr::AMRCoreWavePropagation2d::MakeNewLevelFromScratch( int lev,
                                                                          Real time,
                                                                          const BoxArray& ba,
                                                                          const DistributionMapping& dm )
{
}

void tsunami_lab::amr::AMRCoreWavePropagation2d::MakeNewLevelFromCoarse( int lev,
                                                                         Real time,
                                                                         const BoxArray& ba,
                                                                         const DistributionMapping& dm )
{
}

void tsunami_lab::amr::AMRCoreWavePropagation2d::RemakeLevel( int lev,
                                                              Real time,
                                                              const BoxArray& ba,
                                                              const DistributionMapping& dm )
{
}

void tsunami_lab::amr::AMRCoreWavePropagation2d::ClearLevel( int lev )
{
}

void tsunami_lab::amr::AMRCoreWavePropagation2d::timeStep( t_real i_scaling )
{
}

void tsunami_lab::amr::AMRCoreWavePropagation2d::setGhostOutflow()
{
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
}

void tsunami_lab::amr::AMRCoreWavePropagation2d::setMomentumX( t_idx i_ix,
                                                               t_idx i_iy,
                                                               t_real i_hu )
{
}

void tsunami_lab::amr::AMRCoreWavePropagation2d::setMomentumY( t_idx i_ix,
                                                               t_idx i_iy,
                                                               t_real i_hv )
{
}

void tsunami_lab::amr::AMRCoreWavePropagation2d::setSolver( tsunami_lab::patches::Solver solver )
{
}

void tsunami_lab::amr::AMRCoreWavePropagation2d::setBathymetry( t_idx i_ix,
                                                                t_idx i_iy,
                                                                t_real i_b )
{
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
}
