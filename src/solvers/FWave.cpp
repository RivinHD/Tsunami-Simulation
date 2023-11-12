/**
 * @author Fabian Hofer, Vincent Gerlach (AT uni-jena.de)
 *
 * @section DESCRIPTION
 * The f-wave solver uses two shock waves to approximate the true solution.
 **/
#include "../../include/solvers/FWave.h"
#include <cmath>

void tsunami_lab::solvers::FWave::computeEigenvalues( t_real   i_hL,
                                                      t_real   i_hR,
                                                      t_real   i_uL,
                                                      t_real   i_uR,
                                                      t_real& o_eigenvalue1,
                                                      t_real& o_eigenvalue2 )
{
    // pre-compute square-root ops
    t_real l_hSqrtL = std::sqrt( i_hL );
    t_real l_hSqrtR = std::sqrt( i_hR );

    // compute averages
    t_real l_h = t_real( 0.5 ) * ( i_hL + i_hR );
    t_real l_u = l_hSqrtL * i_uL + l_hSqrtR * i_uR;
    l_u /= l_hSqrtL + l_hSqrtR;

    // compute eigenvalues
    t_real l_ghSqrtRoe = m_gSqrt * std::sqrt( l_h );
    o_eigenvalue1 = l_u - l_ghSqrtRoe;
    o_eigenvalue2 = l_u + l_ghSqrtRoe;
}

void tsunami_lab::solvers::FWave::computeDeltaFlux( t_real i_hL,
                                                    t_real i_hR,
                                                    t_real i_uL,
                                                    t_real i_uR,
                                                    t_real o_deltaFlux[2] )
{
    t_real i_huL = i_hL * i_uL;
    t_real i_huR = i_hR * i_uR;

    o_deltaFlux[0] = i_huR - i_huL;
    o_deltaFlux[1] = ( i_hR * i_uR * i_uR + 0.5f * m_g * i_hR * i_hR )
        - ( i_hL * i_uL * i_uL + 0.5f * m_g * i_hL * i_hL );
}

void tsunami_lab::solvers::FWave::computeEigencoefficients( t_real i_eigenvalue1,
                                                            t_real i_eigenvalue2,
                                                            t_real i_deltaFlux[2],
                                                            t_real& o_eigencoefficient1,
                                                            t_real& o_eigencoefficient2 )
{
    // compute inverse matrix
    t_real denominator = 1 / ( i_eigenvalue2 - i_eigenvalue1 );
    t_real invertedMatrix[2][2] = {
            {i_eigenvalue2 * denominator, -1 * denominator},
            {-i_eigenvalue1 * denominator, 1 * denominator}
    };

    // compute eigencoefficients
    o_eigencoefficient1 = invertedMatrix[0][0] * i_deltaFlux[0] + invertedMatrix[0][1] * i_deltaFlux[1];
    o_eigencoefficient2 = invertedMatrix[1][0] * i_deltaFlux[0] + invertedMatrix[1][1] * i_deltaFlux[1];
}

void tsunami_lab::solvers::FWave::computeBathymetryEffects( t_real i_hL, t_real i_hR,
                                                            t_real i_bL, t_real i_bR,
                                                            t_real o_bathymetryEffect[2] )
{
    o_bathymetryEffect[0] = 0;
    o_bathymetryEffect[1] = -m_g * ( i_bR - i_bL ) * ( t_real( 0.5 ) * ( i_hL + i_hR ) );
}

// net update without bathymetry
void tsunami_lab::solvers::FWave::netUpdates( t_real i_hL,
                                              t_real i_hR,
                                              t_real i_huL,
                                              t_real i_huR,
                                              t_real o_netUpdateL[2],
                                              t_real o_netUpdateR[2] )
{

    // compute particle velocities
    t_real l_uL = i_huL / i_hL;
    t_real l_uR = i_huR / i_hR;

    // compute eigenvalues
    t_real eigenvalue1 = 0;
    t_real eigenvalue2 = 0;
    computeEigenvalues( i_hL, i_hR, l_uL, l_uR, eigenvalue1, eigenvalue2 );

    // create eigenvectors
    t_real eigenvector1[2] = { 1, eigenvalue1 };
    t_real eigenvector2[2] = { 1, eigenvalue2 };

    // compute delta flux
    t_real deltaFlux[2];
    computeDeltaFlux( i_hL, i_hR, l_uL, l_uR, deltaFlux );

    // compute eigencoefficients
    t_real eigencoefficient1 = 0;
    t_real eigencoefficient2 = 0;
    computeEigencoefficients( eigenvalue1, eigenvalue2, deltaFlux, eigencoefficient1, eigencoefficient2 );

    // compute waves / net updates
    for( unsigned short l_qt = 0; l_qt < 2; l_qt++ )
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

// net update with bathymetry
void tsunami_lab::solvers::FWave::netUpdates( t_real i_hL,
                                              t_real i_hR,
                                              t_real i_huL,
                                              t_real i_huR,
                                              t_real i_bL,
                                              t_real i_bR,
                                              t_real o_netUpdateL[2],
                                              t_real o_netUpdateR[2] )
{

    // compute particle velocities
    t_real l_uL = i_huL / i_hL;
    t_real l_uR = i_huR / i_hR;

    // compute eigenvalues
    t_real eigenvalue1 = 0;
    t_real eigenvalue2 = 0;
    computeEigenvalues( i_hL, i_hR, l_uL, l_uR, eigenvalue1, eigenvalue2 );

    // create eigenvectors
    t_real eigenvector1[2] = { 1, eigenvalue1 };
    t_real eigenvector2[2] = { 1, eigenvalue2 };

    // compute delta flux
    t_real deltaFlux[2];
    computeDeltaFlux( i_hL, i_hR, l_uL, l_uR, deltaFlux );

    //compute bathymetry
    t_real bathymetry[2];
    computeBathymetryEffects( i_hL, i_hR, i_bL, i_bR, bathymetry );
    t_real bathymetryDeltaFlux[2] = {
        deltaFlux[0] + bathymetry[0],
        deltaFlux[1] + bathymetry[1]
    };

    // compute eigencoefficients
    t_real eigencoefficient1 = 0;
    t_real eigencoefficient2 = 0;
    computeEigencoefficients( eigenvalue1, eigenvalue2, bathymetryDeltaFlux, eigencoefficient1, eigencoefficient2 );

    // compute waves / net updates
    for( unsigned short l_qt = 0; l_qt < 2; l_qt++ )
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

