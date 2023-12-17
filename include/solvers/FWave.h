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
     * @param o_deltaFlux output: difference of left and right quantities
     */
    static void computeDeltaFlux( t_real i_hL,
                                  t_real i_hR,
                                  t_real i_uL,
                                  t_real i_uR,
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
     * @param o_netUpdateL will be set to the net-updates for the left side; 0: height, 1: momentum.
     * @param o_netUpdateR will be set to the net-updates for the right side; 0: height, 1: momentum.
     **/
    static void netUpdates( t_real i_hL,
                            t_real i_hR,
                            t_real i_huL,
                            t_real i_huR,
                            t_real o_netUpdateL[2],
                            t_real o_netUpdateR[2] );

    /**
     * Computes the net-updates with bathymetry.
     *
     * @param i_hL height of the left side.
     * @param i_hR height of the right side.
     * @param i_huL momentum of the left side.
     * @param i_huR momentum of the right side.
     * @param i_bL height of bathymetry of the left side.
     * @param i_bR height of bathymetry of the right side.
     * @param o_netUpdateL will be set to the net-updates for the left side; 0: height, 1: momentum.
     * @param o_netUpdateR will be set to the net-updates for the right side; 0: height, 1: momentum.
     **/
    static void netUpdates( t_real i_hL,
                            t_real i_hR,
                            t_real i_huL,
                            t_real i_huR,
                            t_real i_bL,
                            t_real i_bR,
                            t_real o_netUpdateL[2],
                            t_real o_netUpdateR[2] );

    /**
     * Computes the net-updates with bathymetry.
     *

     **/

     /**
      * Computes the net-updates with bathymetry for the next N values in the array.
      *
      * @tparam N must be a number between 1 and 4.
      * @param i_hL height of the left side.
      * @param i_hR height of the right side.
      * @param i_huL momentum of the left side.
      * @param i_huR momentum of the right side.
      * @param i_bL height of bathymetry of the left side.
      * @param i_bR height of bathymetry of the right side.
      * @param o_netUpdateL will be set to the net-updates for the left side; 0: height, 1: momentum.
      * @param o_netUpdateR will be set to the net-updates for the right side; 0: height, 1: momentum.
     */
    template <unsigned short N>
    static void netUpdates( t_real i_hL[N],
                            t_real i_hR[N],
                            t_real i_huL[N],
                            t_real i_huR[N],
                            t_real i_bL[N],
                            t_real i_bR[N],
                            t_real o_netUpdateL[2 * N],
                            t_real o_netUpdateR[2 * N] );
};

#endif //TSUNAMISIMULATION_FWAVE_H

template<unsigned short N>
inline void tsunami_lab::solvers::FWave::netUpdates( t_real i_hL[N],
                                                     t_real i_hR[N],
                                                     t_real i_huL[N],
                                                     t_real i_huR[N],
                                                     t_real i_bL[N],
                                                     t_real i_bR[N],
                                                     t_real o_netUpdateL[2 * N],
                                                     t_real o_netUpdateR[2 * N] )
{
    for( unsigned short i = 0; i < N; i++ )
    {
        t_real hL = i_hL[i];
        t_real hR = i_hR[i];
        t_real huL = i_huL[i];
        t_real huR = i_huR[i];
        t_real bL = i_bL[i];
        t_real bR = i_bR[i];

        // compute particle velocities
        t_real uL = huL / hL;
        t_real uR = huR / hR;

        // compute eigenvalues
        t_real eigenvalue1 = 0;
        t_real eigenvalue2 = 0;
        computeEigenvalues( hL, hR, uL, uR, eigenvalue1, eigenvalue2 );

        // create eigenvectors
        t_real eigenvector1[2] = { 1, eigenvalue1 };
        t_real eigenvector2[2] = { 1, eigenvalue2 };

        // compute delta flux
        t_real deltaFlux[2];
        computeDeltaFlux( hL, hR, uL, uR, deltaFlux );

        //compute bathymetry
        t_real bathymetry[2];
        computeBathymetryEffects( hL, hR, bL, bR, bathymetry );
        t_real bathymetryDeltaFlux[2] = {
            deltaFlux[0] + bathymetry[0],
            deltaFlux[1] + bathymetry[1]
        };

        // compute eigencoefficients
        t_real eigencoefficient1 = 0;
        t_real eigencoefficient2 = 0;
        computeEigencoefficients( eigenvalue1, eigenvalue2, bathymetryDeltaFlux, eigencoefficient1, eigencoefficient2 );

        // compute waves / net updates
        for( unsigned short l_qt = 2 * i; l_qt < ( 2 * i + 2 ); l_qt++ )
        {
            // init
            o_netUpdateL[l_qt] = 0;
            o_netUpdateR[l_qt] = 0;

            // 1st wave
            if( eigenvalue1 < 0 )
            {
                o_netUpdateL[l_qt] += eigencoefficient1 * eigenvector1[l_qt];
            }
            else
            {
                o_netUpdateR[l_qt] += eigencoefficient1 * eigenvector1[l_qt];
            }

            // 2nd wave
            if( eigenvalue2 < 0 )
            {
                o_netUpdateL[l_qt] += eigencoefficient2 * eigenvector2[l_qt];
            }
            else
            {
                o_netUpdateR[l_qt] += eigencoefficient2 * eigenvector2[l_qt];
            }
        }
    }
}
