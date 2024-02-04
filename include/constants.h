/**
 * @author Alexander Breuer (alex.breuer AT uni-jena.de)
 *
 * Constants / typedefs used throughout the code.
 **/
#ifndef TSUNAMI_LAB_CONSTANTS_H
#define TSUNAMI_LAB_CONSTANTS_H

#include <cstddef>
#include <AMReX_REAL.H>

namespace tsunami_lab
{
    //! integral type for cell-ids, pointer arithmetic, etc.
    typedef std::size_t t_idx;

    //! floating point type
    typedef amrex_real t_real;
}

#endif