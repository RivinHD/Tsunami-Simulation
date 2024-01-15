#ifndef TSUNAMI_SIMULATION_KERNELS_H
#define TSUNAMI_SIMULATION_KERNELS_H

#include <AMReX_Array4.H>

AMREX_GPU_HOST_DEVICE
AMREX_FORCE_INLINE
void
state_error( int i, int j, int k,
             amrex::Array4<char> const& tag,
             amrex::Array4<amrex::Real const> const& state,
             amrex::Real gridErr, char tagval )
{
    if( state( i, j, k ) > gridErr )
        tag( i, j, k ) = tagval;
}

#endif // TSUNAMI_SIMULATION_KERNELS_H
