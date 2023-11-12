/**
 * @author Fabian Hofer, Vincent Gerlach (AT uni-jena.de)
 *
 * @section DESCRIPTION
 * Unit tests of the f-wave Riemann solver.
 **/
#include "../../include/constants.h"
#include <catch2/catch.hpp>
#define private public
#include "../../include/solvers/FWave.h"
#undef public

using namespace tsunami_lab;

TEST_CASE( "Test the derivation of the F-Wave eigenvalues.", "[FWaveEigenvalue]" )
{
    /*
     * Test case:
     *  h: 10 | 9
     *  u: -3 | 3
     *
     *  height: 9.5
     *  velocity: (sqrt(10) * -3 + 3 * 3) / ( sqrt(10) + sqrt(9) )
     *               = -0.0790021169691720
     * eigenvalue: s1 = -0.079002116969172024 - sqrt(9.80665 * 9.5) = -9.7311093998375095
     *             s2 = -0.079002116969172024 + sqrt(9.80665 * 9.5) =  9.5731051658991654
     */
    t_real l_eigenvalue1 = 0;
    t_real l_eigenvalue2 = 0;
    tsunami_lab::solvers::FWave::computeEigenvalues( 10,
                                                     9,
                                                     -3,
                                                     3,
                                                     l_eigenvalue1,
                                                     l_eigenvalue2 );

    REQUIRE( l_eigenvalue1 == Approx( -9.7311093998375095 ) );
    REQUIRE( l_eigenvalue2 == Approx( 9.5731051658991654 ) );
}

TEST_CASE( "Test the computation of the delta flux.", "[FWaveDeltaFlux]" )
{
    /*
     * Test case:
     *  h: 10 | 9
     *  u: -3 | 3
     *
     * height: 9.5
     * delta f: {9 * 3, 9 * 3 * 3 + 0.5 * m_g * 9 * 9} - {10 * -3, 10 * -3 * -3 + 0.5 * m_g * 10 * 10}
     * delta f: {57, -102.163175}
     */
    t_real l_deltaFlux[2] = { 0 };
    tsunami_lab::solvers::FWave::computeDeltaFlux( 10,
                                                   9,
                                                   -3,
                                                   3,
                                                   l_deltaFlux );
    REQUIRE( l_deltaFlux[0] == Approx( 57 ) );
    REQUIRE( l_deltaFlux[1] == Approx( -102.163175 ) );
}

TEST_CASE( "Test the computation of the effect of the bathymetry.", "[FWaveBathymetry" )
{
    /*
     * Test case:
     * h: 10 | 10
     * b:  5 | 10
     *
     * l_bathymetryEffect[0] = 0
     * l_bathymetryEffect[1] = -m_g * (10 - 5) * (10 + 10) * 0.5
     *                       = -m_g * 5 * 10 = -m_g * 50
     *                       = -490.3325
     */
    t_real l_bathymetryEffect[2] = { 0 };
    tsunami_lab::solvers::FWave::computeBathymetryEffects( 10,
                                                           10,
                                                           5,
                                                           10,
                                                           l_bathymetryEffect );
    REQUIRE( l_bathymetryEffect[0] == 0 );
    REQUIRE( l_bathymetryEffect[1] == Approx( -490.3325 ) );
}

TEST_CASE( "Test the computation of the eigencoefficients.", "[FWaveEigencoefficients]" )
{
    /*
     * Test case:
     *  eigenvalue1: 4
     *  eigenvalue2: 5
     *  delta flux: {10, 2}
     *
     *    Matrix of right eigenvectors:
     *
     *      | 1   1 |
     *  R = |       |
     *      | 4   5 |
     *
     * inverted = {{5, -1}, {-4, 1}}
     *
     * inverted * {10, 2} = {48, -38}
     */
    t_real l_eigencoefficient1 = 0;
    t_real l_eigencoefficient2 = 0;
    t_real l_deltaFlux[2] = { 10, 2 };
    tsunami_lab::solvers::FWave::computeEigencoefficients( 4,
                                                           5,
                                                           l_deltaFlux,
                                                           l_eigencoefficient1,
                                                           l_eigencoefficient2 );
    REQUIRE( l_eigencoefficient1 == Approx( 48 ) );
    REQUIRE( l_eigencoefficient2 == Approx( -38 ) );
}

TEST_CASE( "Test the derivation of the F-Wave net-updates.", "[FWaveUpdate]" )
{
    /*
     * Test case:
     *
     *      left | right
     *  h:    10 | 9
     *  u:    -3 | 3
     *  hu:  -30 | 27
     *
     * The derivation of the eigenvalues (s1, s2) and eigencoefficient (a1, a1) is given above.
     *
     * The net-updates are given through the scaled eigenvectors.
     *
     *                      |  1 |   | 33.5590017014261447899292 |
     * update #1: s1 * a1 * |    | = |                           |
     *                      | s1 |   | -326.56631690591093200508 |
     *
     *                      |  1 |   | 23.4409982985738561366777 |
     * update #2: s2 * a2 * |    | = |                           |
     *                      | s2 |   | 224.403141905910928927533 |
     */
    float l_netUpdatesL[2] = { -5, 3 };
    float l_netUpdatesR[2] = { 4, 7 };

    tsunami_lab::solvers::FWave::netUpdates( 10,
                                             9,
                                             -30,
                                             27,
                                             l_netUpdatesL,
                                             l_netUpdatesR );

    REQUIRE( l_netUpdatesL[0] == Approx( 33.5590017014261447899292 ) );
    REQUIRE( l_netUpdatesL[1] == Approx( -326.56631690591093200508 ) );

    REQUIRE( l_netUpdatesR[0] == Approx( 23.4409982985738561366777 ) );
    REQUIRE( l_netUpdatesR[1] == Approx( 224.403141905910928927533 ) );

    /*
     * Test case (dam break):
     *
     *     left | right
     *   h:  10 | 8
     *   hu:  0 | 0
     *
     * eigenvalues are given as:
     *
     *   s1 = -sqrt(9.80665 * 9)
     *   s2 =  sqrt(9.80665 * 9)
     *
     * Inversion of the matrix of right Eigenvectors:
     *
     *   wolframalpha.com query: invert {{1, 1}, {-sqrt(9.80665 * 9), sqrt(9.80665 * 9)}}
     *
     *          | 0.5 -0.0532217 |
     *   Rinv = |                |
     *          | 0.5 -0.0532217 |
     *
     * Multiplication with the jump in quantities gives the eigencoefficient:
     *
     *        | 8 - 10 |   | -1 |   | a1 |
     * Rinv * |        | = |    | = |    |
     *        |  0 - 0 |   | -1 |   | a2 |
     *
     * The net-updates are given through the scaled eigenvectors.
     *
     *                      |  1 |   |   9.394671362 |
     * update #1: s1 * a1 * |    | = |               |
     *                      | s1 |   | -88.25985     |
     *
     *                      |  1 |   |  -9.394671362 |
     * update #2: s2 * a2 * |    | = |               |
     *                      | s2 |   | -88.25985     |
     */
    tsunami_lab::solvers::FWave::netUpdates( 10,
                                             8,
                                             0,
                                             0,
                                             l_netUpdatesL,
                                             l_netUpdatesR );

    REQUIRE( l_netUpdatesL[0] == Approx( 9.394671362 ) );
    REQUIRE( l_netUpdatesL[1] == -Approx( 88.25985 ) );

    REQUIRE( l_netUpdatesR[0] == -Approx( 9.394671362 ) );
    REQUIRE( l_netUpdatesR[1] == -Approx( 88.25985 ) );

    /*
    * Test case (dam break):
    *
    *        left | right
    *   h:     10 | 1
    *   hu:  -100 | 0
    *
    * eigenvalues are calculated from computeEigenvalues as:
    *
    *   s1 = -14.9416
    *   s2 = -0.253316
    *
    * eigencoefficient are calculated form computeEigencoefficients as:
    *
    *   a1 = 99.4054
    *   a2 = 0.594551
    *
    * The net-updates are given through the scaled eigenvectors.
    *
    *                      |  1 |   |     100     |
    * update #1: s1 * a1 * |    | = |             |
    *                      | s1 |   | -1485.4292  |
    *
    *                      |  1 |   |  0  |
    * update #2: s2 * a2 * |    | = |     |
    *                      | s2 |   |  0  |
    */

    tsunami_lab::solvers::FWave::netUpdates( 10,
                                             1,
                                             -100,
                                             0,
                                             l_netUpdatesL,
                                             l_netUpdatesR );

    REQUIRE( l_netUpdatesL[0] == Approx( 100 ) );
    REQUIRE( l_netUpdatesL[1] == Approx( -1485.4292 ) );

    REQUIRE( l_netUpdatesR[0] == Approx( 0 ) );
    REQUIRE( l_netUpdatesR[1] == Approx( 0 ) );

    /*
     * Test case (trivial steady state):
     *
     *     left | right
     *   h:  10 | 10
     *  hu:   0 |  0
     */
    tsunami_lab::solvers::FWave::netUpdates( 10,
                                             10,
                                             0,
                                             0,
                                             l_netUpdatesL,
                                             l_netUpdatesR );

    REQUIRE( l_netUpdatesL[0] == Approx( 0 ) );
    REQUIRE( l_netUpdatesL[1] == Approx( 0 ) );

    REQUIRE( l_netUpdatesR[0] == Approx( 0 ) );
    REQUIRE( l_netUpdatesR[1] == Approx( 0 ) );
}