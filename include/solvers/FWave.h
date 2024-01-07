/**
 * @author Fabian Hofer, Vincent Gerlach (AT uni-jena.de)
 *
 * The f-wave solver for solving the Initial Value Problem (IVP) for the shallow water equations.
 **/
#ifndef TSUNAMISIMULATION_FWAVE_H
#define TSUNAMISIMULATION_FWAVE_H

#include "../constants.h"

namespace tsunami_lab
{
    namespace solvers
    {
        class FWave;
    }
}

/**
 * The f-wave solver for solving the Initial Value Problem (IVP) for the shallow water equations.
*/
class tsunami_lab::solvers::FWave
{
private:
    //! square root of gravity
    static t_real constexpr m_gSqrt = 3.131557121;
    static t_real constexpr m_g = 9.80665;

    /**
     * Computes the eigenvalues for the left and right wave.
     *
     * @param i_hL height of the left side.
     * @param i_hR height of the right side.
     * @param i_uL particle velocity of the left side.
     * @param i_uR particles velocity of the right side.
     * @param o_eigenvalue1 output: Roe eigenvalue one.
     * @param o_eigenvalue2 output: Roe eigenvalue two.
     **/
#pragma omp declare simd
    static void computeEigenvalues( t_real i_hL,
                                    t_real i_hR,
                                    t_real i_uL,
                                    t_real i_uR,
                                    t_real& o_eigenvalue1,
                                    t_real& o_eigenvalue2 );

    /**
     * Computes difference of the flux function after inserting the left and right quantities.
     *
     * @param i_hL height of the left side.
     * @param i_hR height of the right side.
     * @param i_uL particle velocity of the left side.
     * @param i_uR particles velocity of the right side.
     * @param i_huL momentum of the left side.
     * @param i_huR momentum of the right side.
     * @param o_deltaFlux output: difference of left and right quantities
     */
#pragma omp declare simd
    static void computeDeltaFlux( t_real i_hL,
                                  t_real i_hR,
                                  t_real i_uL,
                                  t_real i_uR,
                                  t_real i_huL,
                                  t_real i_huR,
                                  t_real o_deltaFlux[2] );

    /**
     * Compute the eigencoefficients for left and right wave.
     *
     * @param i_eigenvalue1 Roe eigenvalue one.
     * @param i_eigenvalue2 Roe eigenvalue two.
     * @param i_deltaFlux the compute delta Flux with the same inputs used for the eigenvalues
     * @param o_eigencoefficient1 ouput: the eigencoefficient ot eigenvalue one.
     * @param o_eigencoefficient2 ouput: the eigencoefficient ot eigenvalue two.
     */
#pragma omp declare simd
    static void computeEigencoefficients( t_real i_eigenvalue1,
                                          t_real i_eigenvalue2,
                                          t_real i_deltaFlux[2],
                                          t_real& o_eigencoefficient1,
                                          t_real& o_eigencoefficient2 );

    /**
     *
     * @param i_hL height of the left side.
     * @param i_hR height of the right side.
     * @param i_bL height of bathymetry of the left side.
     * @param i_bR height of bathymetry of the right side.
     * @param o_bathymetryEffect output: effect of the bathymetry
     */
#pragma omp declare simd
    static void computeBathymetryEffects( t_real i_hL,
                                          t_real i_hR,
                                          t_real i_bL,
                                          t_real i_bR,
                                          t_real o_bathymetryEffect[2] );

public:
    /**
     * Computes the net-updates without bathymetry.
     *
     * @param i_hL height of the left side.
     * @param i_hR height of the right side.
     * @param i_huL momentum of the left side.
     * @param i_huR momentum of the right side.
     * @param o_netUpdate will be set to the net-updates for the 0: left 1: right side; 0: height, 1: momentum.
     **/
#pragma omp declare simd
    static void netUpdates( t_real i_hL,
                            t_real i_hR,
                            t_real i_huL,
                            t_real i_huR,
                            t_real o_netUpdate[2][2] );

    /**
     * Computes the net-updates with bathymetry.
     *
     * @param i_hL height of the left side.
     * @param i_hR height of the right side.
     * @param i_huL momentum of the left side.
     * @param i_huR momentum of the right side.
     * @param i_bL height of bathymetry of the left side.
     * @param i_bR height of bathymetry of the right side.
     * @param o_netUpdate will be set to the net-updates for the 0: left 1: right side; 0: height, 1: momentum.
     **/
#pragma omp declare simd
    static void netUpdates( t_real i_hL,
                            t_real i_hR,
                            t_real i_huL,
                            t_real i_huR,
                            t_real i_bL,
                            t_real i_bR,
                            t_real o_netUpdate[2][2] );
};

#endif //TSUNAMISIMULATION_FWAVE_H
