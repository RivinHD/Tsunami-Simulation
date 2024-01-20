#ifndef TSUNAMI_SIMULATION_KERNELS_H
#define TSUNAMI_SIMULATION_KERNELS_H

#include <AMReX_Array4.H>
#include "AMRCoreWavePropagation2d.h"
#include "../constants.h"
#include "../solvers/FWave.h"

AMREX_GPU_HOST_DEVICE
AMREX_FORCE_INLINE
void state_error( int i, int j, int k,
                  amrex::Array4<char> const& tag,
                  amrex::Array4<amrex::Real const> const& state,
                  amrex::Real gridErr,
                  char tagval )
{
    if( state( i, j, k ) > gridErr )
        tag( i, j, k ) = tagval;
}

AMREX_GPU_HOST_DEVICE
AMREX_FORCE_INLINE
void xSweep( amrex::Box const& bx,
             amrex::Real scaling,
             amrex::Array4<amrex::Real const> const& height,
             amrex::Array4<amrex::Real const> const& momentumX,
             amrex::Array4<amrex::Real const> const& momentumY,
             amrex::Array4<amrex::Real const> const& bathymetry,
             amrex::Array4<amrex::Real> const& gridOut,
             amrex::Array4<amrex::Real> const& fluxX )
{
    using Component = tsunami_lab::amr::AMRCoreWavePropagation2d::Component;

    const auto lo = amrex::lbound( bx );
    const auto hi = amrex::ubound( bx );

    for( int j = lo.y; j <= hi.y; ++j )
    {
        for( int i = lo.x; i <= hi.x; ++i )
        {
            // noting to compute both shore cells
            if( height( i, j, 0 ) == 0 && height( i + 1, j, 0 ) == 0 )
            {
                continue;
            }

            // calculate the reflection
            bool leftReflection = ( height( i + 1, j, 0 ) == amrex::Real( 0.0 ) );
            amrex::Real heightRight = leftReflection ? height( i, j, 0 ) : height( i + 1, j, 0 );
            amrex::Real momentumRight = leftReflection ? -momentumX( i, j, 0 ) : momentumX( i + 1, j, 0 );
            amrex::Real bathymetryRight = leftReflection ? bathymetry( i, j, 0 ) : bathymetry( i + 1, j, 0 );

            bool rightReflection = ( height( i, j, 0 ) == amrex::Real( 0.0 ) );
            amrex::Real heightLeft = rightReflection ? height( i + 1, j, 0 ) : height( i, j, 0 );
            amrex::Real momentumLeft = rightReflection ? -momentumX( i + 1, j, 0 ) : momentumX( i, j, 0 );
            amrex::Real bathymetryLeft = rightReflection ? bathymetry( i + 1, j, 0 ) : bathymetry( i, j, 0 );

            // compute net-updates
            tsunami_lab::t_real netUpdates[2][2];

            tsunami_lab::solvers::FWave::netUpdates( heightLeft,
                                                     heightRight,
                                                     momentumLeft,
                                                     momentumRight,
                                                     bathymetryLeft,
                                                     bathymetryRight,
                                                     netUpdates );

            fluxX( i, j, 0, Component::HEIGHT ) = -scaling * netUpdates[0][0] * !rightReflection;
            gridOut( i, j, 0, Component::HEIGHT ) -= scaling * netUpdates[0][0] * !rightReflection;
            fluxX( i, j, 0, Component::MOMENTUM_X ) = -scaling * netUpdates[0][1] * !rightReflection;
            gridOut( i, j, 0, Component::MOMENTUM_X ) -= scaling * netUpdates[0][1] * !rightReflection;

            fluxX( i + 1, j, 0, Component::HEIGHT ) = -scaling * netUpdates[1][0] * !leftReflection;
            gridOut( i + 1, j, 0, Component::HEIGHT ) -= scaling * netUpdates[1][0] * !leftReflection;
            fluxX( i + 1, j, 0, Component::MOMENTUM_X ) = -scaling * netUpdates[1][1] * !leftReflection;
            gridOut( i + 1, j, 0, Component::MOMENTUM_X ) -= scaling * netUpdates[1][1] * !leftReflection;

            gridOut( i, j, 0, Component::MOMENTUM_Y ) = momentumY( i, j, 0 ); // Copy
        }
    }
}

AMREX_GPU_HOST_DEVICE
AMREX_FORCE_INLINE
void ySweep( amrex::Box const& bx,
             amrex::Real scaling,
             amrex::Array4<amrex::Real const> const& height,
             amrex::Array4<amrex::Real const> const& momentumX,
             amrex::Array4<amrex::Real const> const& momentumY,
             amrex::Array4<amrex::Real const> const& bathymetry,
             amrex::Array4<amrex::Real> const& gridOut,
             amrex::Array4<amrex::Real> const& fluxY )
{
    using Component = tsunami_lab::amr::AMRCoreWavePropagation2d::Component;

    const auto lo = amrex::lbound( bx );
    const auto hi = amrex::ubound( bx );

    for( int i = lo.x; i <= hi.x; ++i )
    {
        for( int j = lo.y; j <= hi.y; ++j )
        {
            // noting to compute both shore cells
            if( height( i, j, 0 ) == 0 && height( i, j + 1, 0 ) == 0 )
            {
                continue;
            }

            // calculate the reflection
            bool leftReflection = ( height( i, j + 1, 0 ) == amrex::Real( 0.0 ) );
            amrex::Real heightRight = leftReflection ? height( i, j, 0 ) : height( i, j + 1, 0 );
            amrex::Real momentumRight = leftReflection ? -momentumY( i, j, 0 ) : momentumY( i, j + 1, 0 );
            amrex::Real bathymetryRight = leftReflection ? bathymetry( i, j, 0 ) : bathymetry( i, j + 1, 0 );

            bool rightReflection = ( height( i, j, 0 ) == amrex::Real( 0.0 ) );
            amrex::Real heightLeft = rightReflection ? height( i, j + 1, 0 ) : height( i, j, 0 );
            amrex::Real momentumLeft = rightReflection ? -momentumY( i, j + 1, 0 ) : momentumY( i, j, 0 );
            amrex::Real bathymetryLeft = rightReflection ? bathymetry( i, j + 1, 0 ) : bathymetry( i, j, 0 );

            // compute net-updates
            tsunami_lab::t_real netUpdates[2][2];

            tsunami_lab::solvers::FWave::netUpdates( heightLeft,
                                                     heightRight,
                                                     momentumLeft,
                                                     momentumRight,
                                                     bathymetryLeft,
                                                     bathymetryRight,
                                                     netUpdates );

            fluxY( i, j, 0, Component::HEIGHT ) = -scaling * netUpdates[0][0] * !rightReflection;
            gridOut( i, j, 0, Component::HEIGHT ) -= scaling * netUpdates[0][0] * !rightReflection;
            fluxY( i, j, 0, Component::MOMENTUM_Y ) = -scaling * netUpdates[0][1] * !rightReflection;
            gridOut( i, j, 0, Component::MOMENTUM_Y ) -= scaling * netUpdates[0][1] * !rightReflection;

            fluxY( i, j + 1, 0, Component::HEIGHT ) = -scaling * netUpdates[1][0] * !leftReflection;
            gridOut( i, j + 1, 0, Component::HEIGHT ) -= scaling * netUpdates[1][0] * !leftReflection;
            fluxY( i, j + 1, 0, Component::MOMENTUM_Y ) = -scaling * netUpdates[1][1] * !leftReflection;
            gridOut( i, j + 1, 0, Component::MOMENTUM_Y ) -= scaling * netUpdates[1][1] * !leftReflection;

            gridOut( i, j, 0, Component::MOMENTUM_X ) = momentumX( i, j, 0 ); // Copy
        }
    }
}

#endif // TSUNAMI_SIMULATION_KERNELS_H