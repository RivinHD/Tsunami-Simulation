/**
 * @author Fabian Hofer, Vincent Gerlach (AT uni-jena.de)
 *
 * @section DESCRIPTION
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

class tsunami_lab::solvers::FWave {
private:
    //! square root of gravity
    static t_real constexpr m_gSqrt = 3.131557121;
    static t_real constexpr m_g = 9.80665;

    /**
     * Computes the eigenvalues for the left and right wave
     *
     * @param i_hL height of the left side.
     * @param i_hR height of the right side.
     * @param i_uL particle velocity of the leftside.
     * @param i_uR particles velocity of the right side.
     * @param o_eigenvalue1 output: Roe eigenvalue one.
     * @param o_eigenvalue2 output: Roe eigenvalue two.
     **/
    static void computeEigenvalues(t_real i_hL,
                             t_real i_hR,
                             t_real i_uL,
                             t_real i_uR,
                             t_real & o_eigenvalue1,
                             t_real & o_eigenvalue2);

    /**
     * Computes delta flux of left i_qL (i_hL, i_uL) and right i_qR (i_hR, i_uR)
     *
     * @param i_hL height of the left side.
     * @param i_hR height of the right side.
     * @param i_uL particle velocity of the leftside.
     * @param i_uR particles velocity of the right side.
     * @param o_deltaFlux output: difference of left and right quantities
     */
     static void computeDeltaFlux(t_real i_hL,
                             t_real i_hR,
                             t_real i_uL,
                             t_real i_uR,
                             t_real o_deltaFlux[2]);

    /**
     * Compute the eigencoefficients for left and right wave
     * 
     * @param i_eigenvalue1 Roe eigenvalue one.
     * @param i_eigenvalue2 Roe eigenvalue two.
     * @param i_deltaFlux the compute delta Flux with the same inputs used for the eigenvalues
     * @param o_eigencoefficient1 ouput: the eigencoefficient ot eigenvalue one.
     * @param o_eigencoefficient2 ouput: the eigencoefficient ot eigenvalue two.
     */
    static void computeEigencoefficients(t_real i_eigenvalue1,
                                         t_real i_eigenvalue2,
                                         t_real i_deltaFlux[2],
                                         t_real & o_eigencoefficient1,
                                         t_real & o_eigencoefficient2);

public:
    /**
     * Computes the net-updates.
     *
     * @param i_hL height of the left side.
     * @param i_hR height of the right side.
     * @param i_huL momentum of the left side.
     * @param i_huR momentum of the right side.
     * @param o_netUpdateL will be set to the net-updates for the left side; 0: height, 1: momentum.
     * @param o_netUpdateR will be set to the net-updates for the right side; 0: height, 1: momentum.
     **/
    static void netUpdates( t_real i_hL,
                            t_real i_hR,
                            t_real i_huL,
                            t_real i_huR,
                            t_real o_netUpdateL[2],
                            t_real o_netUpdateR[2] );
};

#endif //TSUNAMISIMULATION_FWAVE_H
