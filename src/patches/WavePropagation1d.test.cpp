/**
 * @author Alexander Breuer, Fabian Hofer, Vincent Gerlach (alex.breuer AT uni-jena.de)
 *
 * @section DESCRIPTION
 * Unit tests for the one-dimensional wave propagation patch.
 **/
#include <catch2/catch.hpp>

#define private public
#include "../../include/patches/WavePropagation1d.h"
#undef private

TEST_CASE( "Test the 1d wave propagation solver.", "[WaveProp1d]" )
{
    /*
     * Test case using the FWave solver:
     *
     *   Single dam break problem between cell 49 and 50.
     *     left | right
     *       10 | 8
     *        0 | 0
     *
     *   Elsewhere steady state.
     *
     * The net-updates at the respective edge are given as
     * (see derivation in Fwave solver):
     *    left          | right
     *      9.394671362 | -9.394671362
     *    -88.25985     | -88.25985
     */

     // construct solver and setup a dam break problem
    tsunami_lab::patches::WavePropagation1d m_waveProp_FWave( 100 );
    m_waveProp_FWave.setSolver( tsunami_lab::patches::Solver::FWAVE );

    for( std::size_t l_ce = 0; l_ce < 50; l_ce++ )
    {
        m_waveProp_FWave.setHeight( l_ce,
                                    0,
                                    10 );
        m_waveProp_FWave.setMomentumX( l_ce,
                                       0,
                                       0 );
    }
    for( std::size_t l_ce = 50; l_ce < 100; l_ce++ )
    {
        m_waveProp_FWave.setHeight( l_ce,
                                    0,
                                    8 );
        m_waveProp_FWave.setMomentumX( l_ce,
                                       0,
                                       0 );
    }

    // set outflow boundary condition
    m_waveProp_FWave.setGhostOutflow();

    // perform a time step
    m_waveProp_FWave.timeStep( 0.1 );

    // steady state
    for( std::size_t l_ce = 0; l_ce < 49; l_ce++ )
    {
        REQUIRE( m_waveProp_FWave.getHeight()[l_ce] == Approx( 10 ) );
        REQUIRE( m_waveProp_FWave.getMomentumX()[l_ce] == Approx( 0 ) );
    }

    // dam-break
    REQUIRE( m_waveProp_FWave.getHeight()[49] == Approx( 10 - 0.1 * 9.394671362 ) );
    REQUIRE( m_waveProp_FWave.getMomentumX()[49] == Approx( 0 + 0.1 * 88.25985 ) );

    REQUIRE( m_waveProp_FWave.getHeight()[50] == Approx( 8 + 0.1 * 9.394671362 ) );
    REQUIRE( m_waveProp_FWave.getMomentumX()[50] == Approx( 0 + 0.1 * 88.25985 ) );

    // steady state
    for( std::size_t l_ce = 51; l_ce < 100; l_ce++ )
    {
        REQUIRE( m_waveProp_FWave.getHeight()[l_ce] == Approx( 8 ) );
        REQUIRE( m_waveProp_FWave.getMomentumX()[l_ce] == Approx( 0 ) );
    }

    /*
   * Test case with Roe Solver:
   *
   *   Single dam break problem between cell 49 and 50.
   *     left | right
   *       10 | 8
   *        0 | 0
   *
   *   Elsewhere steady state.
   *
   * The net-updates at the respective edge are given as
   * (see derivation in Roe solver):
   *    left          | right
   *      9.394671362 | -9.394671362
   *    -88.25985     | -88.25985
   */

   // construct solver and setup a dam break problem
    tsunami_lab::patches::WavePropagation1d m_waveProp_Roe( 100 );
    m_waveProp_Roe.setSolver( tsunami_lab::patches::Solver::ROE );

    for( std::size_t l_ce = 0; l_ce < 50; l_ce++ )
    {
        m_waveProp_Roe.setHeight( l_ce,
                                  0,
                                  10 );
        m_waveProp_Roe.setMomentumX( l_ce,
                                     0,
                                     0 );
    }
    for( std::size_t l_ce = 50; l_ce < 100; l_ce++ )
    {
        m_waveProp_Roe.setHeight( l_ce,
                                  0,
                                  8 );
        m_waveProp_Roe.setMomentumX( l_ce,
                                     0,
                                     0 );
    }

    // set outflow boundary condition
    m_waveProp_Roe.setGhostOutflow();

    // perform a time step
    m_waveProp_Roe.timeStep( 0.1 );

    // steady state
    for( std::size_t l_ce = 0; l_ce < 49; l_ce++ )
    {
        REQUIRE( m_waveProp_Roe.getHeight()[l_ce] == Approx( 10 ) );
        REQUIRE( m_waveProp_Roe.getMomentumX()[l_ce] == Approx( 0 ) );
    }

    // dam-break
    REQUIRE( m_waveProp_Roe.getHeight()[49] == Approx( 10 - 0.1 * 9.394671362 ) );
    REQUIRE( m_waveProp_Roe.getMomentumX()[49] == Approx( 0 + 0.1 * 88.25985 ) );

    REQUIRE( m_waveProp_Roe.getHeight()[50] == Approx( 8 + 0.1 * 9.394671362 ) );
    REQUIRE( m_waveProp_Roe.getMomentumX()[50] == Approx( 0 + 0.1 * 88.25985 ) );

    // steady state
    for( std::size_t l_ce = 51; l_ce < 100; l_ce++ )
    {
        REQUIRE( m_waveProp_Roe.getHeight()[l_ce] == Approx( 8 ) );
        REQUIRE( m_waveProp_Roe.getMomentumX()[l_ce] == Approx( 0 ) );
    }
}

TEST_CASE( "Test the 1d wave propagation reflection", "[WaveProp1d]" )
{
    /*
    * Test Case with reflection on the left but no reflection on the right side and one reflection that should appear in the middle
    */
    tsunami_lab::patches::WavePropagation1d m_waveProp( 5 );

    m_waveProp.setReflection( tsunami_lab::patches::WavePropagation1d::Side::LEFT, true );
    m_waveProp.setReflection( tsunami_lab::patches::WavePropagation1d::Side::RIGHT, false );

    tsunami_lab::t_real l_h[5] = { 100, 0, 0, 200, 200 };
    tsunami_lab::t_real l_hu[5] = { 123, -234, 1, 423, 423 };


    for( std::size_t l_ce = 0; l_ce < 5; l_ce++ )
    {
        m_waveProp.setHeight( l_ce,
                              0,
                              l_h[l_ce] );
        m_waveProp.setMomentumX( l_ce,
                                 0,
                                 l_hu[l_ce] );
    }

    m_waveProp.setGhostOutflow();
    const tsunami_lab::t_real* m_height = m_waveProp.getHeight();
    const tsunami_lab::t_real* m_momentum = m_waveProp.getMomentumX();

    // Left Ghost and 1. Cell
    REQUIRE( m_height[-1] == tsunami_lab::t_real( 0.0 ) );
    REQUIRE( m_momentum[-1] == l_hu[0] );

    // Right Ghost and 5. Cell
    REQUIRE( m_height[5] == l_h[4] );
    REQUIRE( m_momentum[5] == l_hu[4] );

    tsunami_lab::t_real heightLeft;
    tsunami_lab::t_real heightRight;
    tsunami_lab::t_real momentumLeft;
    tsunami_lab::t_real momentumRight;
    tsunami_lab::patches::WavePropagation1d::Reflection reflection;

    // 1. Cell (water) and 2. Cell (shore)
    reflection = m_waveProp.calculateReflection( l_h,
                                                 l_hu,
                                                 0,
                                                 heightLeft,
                                                 heightRight,
                                                 momentumLeft,
                                                 momentumRight );

    REQUIRE( heightLeft == l_h[0] );
    REQUIRE( heightRight == l_h[0] );
    REQUIRE( momentumLeft == l_hu[0] );
    REQUIRE( momentumRight == -l_hu[0] );
    REQUIRE( reflection == tsunami_lab::patches::WavePropagation1d::Reflection::LEFT );

    // 2. Cell (shore) and 3. Cell (shore)
    // !!! Output of height and momentum does not matter with 2 shore cells 
    reflection = m_waveProp.calculateReflection( l_h,
                                                 l_hu,
                                                 1,
                                                 heightLeft,
                                                 heightRight,
                                                 momentumLeft,
                                                 momentumRight );

    REQUIRE( reflection == tsunami_lab::patches::WavePropagation1d::Reflection::BOTH );

    // 3. Cell (shore) and 4. Cell (water)
    reflection = m_waveProp.calculateReflection( l_h,
                                                 l_hu,
                                                 2,
                                                 heightLeft,
                                                 heightRight,
                                                 momentumLeft,
                                                 momentumRight );

    REQUIRE( heightLeft == l_h[3] );
    REQUIRE( heightRight == l_h[3] );
    REQUIRE( momentumLeft == -l_hu[3] );
    REQUIRE( momentumRight == l_hu[3] );
    REQUIRE( reflection == tsunami_lab::patches::WavePropagation1d::Reflection::RIGHT );

    // 4. Cell (water) and 5. Cell (water)
    reflection = m_waveProp.calculateReflection( l_h,
                                                 l_hu,
                                                 3,
                                                 heightLeft,
                                                 heightRight,
                                                 momentumLeft,
                                                 momentumRight );

    REQUIRE( heightLeft == l_h[3] );
    REQUIRE( heightRight == l_h[4] );
    REQUIRE( momentumLeft == l_hu[3] );
    REQUIRE( momentumRight == l_hu[4] );
    REQUIRE( reflection == tsunami_lab::patches::WavePropagation1d::Reflection::NONE );


    // Test with bathymetry
    tsunami_lab::t_real bathymetryLeft;
    tsunami_lab::t_real bathymetryRight;

    m_waveProp.setBathymetry( 0, 0, 10 );
    m_waveProp.setBathymetry( 1, 0, 1239 );
    m_waveProp.setBathymetry( 2, 0, 134 );
    m_waveProp.setBathymetry( 3, 0, 67 );
    m_waveProp.setBathymetry( 4, 0, 70 );
    m_waveProp.setGhostOutflow();

    // Test Ghost Cell Bathymetry
    const tsunami_lab::t_real* m_bathymetry = m_waveProp.getBathymetry();
    REQUIRE( m_bathymetry[-1] == tsunami_lab::t_real( 10 ) );
    REQUIRE( m_bathymetry[5] == tsunami_lab::t_real( 70 ) );

    // 1. Cell (water) and 2. Cell (shore)
    reflection = m_waveProp.calculateReflection( m_waveProp.m_h[m_waveProp.m_step],
                                                 m_waveProp.m_hu[m_waveProp.m_step],
                                                 1,
                                                 heightLeft,
                                                 heightRight,
                                                 momentumLeft,
                                                 momentumRight,
                                                 bathymetryLeft,
                                                 bathymetryRight );

    REQUIRE( heightLeft == l_h[0] );
    REQUIRE( heightRight == l_h[0] );
    REQUIRE( momentumLeft == l_hu[0] );
    REQUIRE( momentumRight == -l_hu[0] );
    REQUIRE( bathymetryLeft == 10 );
    REQUIRE( bathymetryLeft == bathymetryRight );
    REQUIRE( reflection == tsunami_lab::patches::WavePropagation1d::Reflection::LEFT );

    // 2. Cell (shore) and 3. Cell (shore)
    // !!! Output of height and momentum does not matter with 2 shore cells 
    reflection = m_waveProp.calculateReflection( m_waveProp.m_h[m_waveProp.m_step],
                                                 m_waveProp.m_hu[m_waveProp.m_step],
                                                 2,
                                                 heightLeft,
                                                 heightRight,
                                                 momentumLeft,
                                                 momentumRight,
                                                 bathymetryLeft,
                                                 bathymetryRight );

    REQUIRE( reflection == tsunami_lab::patches::WavePropagation1d::Reflection::BOTH );

    // 3. Cell (shore) and 4. Cell (water)
    reflection = m_waveProp.calculateReflection( m_waveProp.m_h[m_waveProp.m_step],
                                                 m_waveProp.m_hu[m_waveProp.m_step],
                                                 3,
                                                 heightLeft,
                                                 heightRight,
                                                 momentumLeft,
                                                 momentumRight,
                                                 bathymetryLeft,
                                                 bathymetryRight );

    REQUIRE( heightLeft == l_h[3] );
    REQUIRE( heightRight == l_h[3] );
    REQUIRE( momentumLeft == -l_hu[3] );
    REQUIRE( momentumRight == l_hu[3] );
    REQUIRE( bathymetryLeft == bathymetryRight );
    REQUIRE( bathymetryRight == 67 );
    REQUIRE( reflection == tsunami_lab::patches::WavePropagation1d::Reflection::RIGHT );

    // 4. Cell (water) and 5. Cell (water)
    reflection = m_waveProp.calculateReflection( m_waveProp.m_h[m_waveProp.m_step],
                                                 m_waveProp.m_hu[m_waveProp.m_step],
                                                 4,
                                                 heightLeft,
                                                 heightRight,
                                                 momentumLeft,
                                                 momentumRight,
                                                 bathymetryLeft,
                                                 bathymetryRight );

    REQUIRE( heightLeft == l_h[3] );
    REQUIRE( heightRight == l_h[4] );
    REQUIRE( momentumLeft == l_hu[3] );
    REQUIRE( momentumRight == l_hu[4] );
    REQUIRE( bathymetryLeft == 67 );
    REQUIRE( bathymetryRight == 70 );
    REQUIRE( reflection == tsunami_lab::patches::WavePropagation1d::Reflection::NONE );

}