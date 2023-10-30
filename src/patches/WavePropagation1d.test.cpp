/**
 * @author Alexander Breuer (alex.breuer AT uni-jena.de)
 *
 * @section DESCRIPTION
 * Unit tests for the one-dimensional wave propagation patch.
 **/
#include <catch2/catch.hpp>
#include "../../include/patches/WavePropagation1d.h"

TEST_CASE( "Test the 1d wave propagation solver.", "[WaveProp1d]" ) {
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
   * (see derivation in Roe solver):
   *    left          | right
   *      9.394671362 | -9.394671362
   *    -88.25985     | -88.25985
   */

  // construct solver and setup a dambreak problem
  tsunami_lab::patches::WavePropagation1d m_waveProp_FWave( 100 );
  m_waveProp_FWave.setSolver(tsunami_lab::patches::Solver::FWave);

  for( std::size_t l_ce = 0; l_ce < 50; l_ce++ ) {
    m_waveProp_FWave.setHeight( l_ce,
                          0,
                          10 );
    m_waveProp_FWave.setMomentumX( l_ce,
                             0,
                             0 );
  }
  for( std::size_t l_ce = 50; l_ce < 100; l_ce++ ) {
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
  for( std::size_t l_ce = 0; l_ce < 49; l_ce++ ) {
    REQUIRE( m_waveProp_FWave.getHeight()[l_ce]   == Approx(10) );
    REQUIRE( m_waveProp_FWave.getMomentumX()[l_ce] == Approx(0) );
  }

  // dam-break
  REQUIRE( m_waveProp_FWave.getHeight()[49]   == Approx(10 - 0.1 * 9.394671362) );
  REQUIRE( m_waveProp_FWave.getMomentumX()[49] == Approx( 0 + 0.1 * 88.25985) );

  REQUIRE( m_waveProp_FWave.getHeight()[50]   == Approx(8 + 0.1 * 9.394671362) );
  REQUIRE( m_waveProp_FWave.getMomentumX()[50] == Approx(0 + 0.1 * 88.25985) );

  // steady state
  for( std::size_t l_ce = 51; l_ce < 100; l_ce++ ) {
    REQUIRE( m_waveProp_FWave.getHeight()[l_ce]   == Approx(8) );
    REQUIRE( m_waveProp_FWave.getMomentumX()[l_ce] == Approx(0) );
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

  // construct solver and setup a dambreak problem
  tsunami_lab::patches::WavePropagation1d m_waveProp_Roe( 100 );
  m_waveProp_Roe.setSolver(tsunami_lab::patches::Solver::Roe);

  for( std::size_t l_ce = 0; l_ce < 50; l_ce++ ) {
    m_waveProp_Roe.setHeight( l_ce,
                          0,
                          10 );
    m_waveProp_Roe.setMomentumX( l_ce,
                             0,
                             0 );
  }
  for( std::size_t l_ce = 50; l_ce < 100; l_ce++ ) {
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
  for( std::size_t l_ce = 0; l_ce < 49; l_ce++ ) {
    REQUIRE( m_waveProp_Roe.getHeight()[l_ce]   == Approx(10) );
    REQUIRE( m_waveProp_Roe.getMomentumX()[l_ce] == Approx(0) );
  }

  // dam-break
  REQUIRE( m_waveProp_Roe.getHeight()[49]   == Approx(10 - 0.1 * 9.394671362) );
  REQUIRE( m_waveProp_Roe.getMomentumX()[49] == Approx( 0 + 0.1 * 88.25985) );

  REQUIRE( m_waveProp_Roe.getHeight()[50]   == Approx(8 + 0.1 * 9.394671362) );
  REQUIRE( m_waveProp_Roe.getMomentumX()[50] == Approx(0 + 0.1 * 88.25985) );

  // steady state
  for( std::size_t l_ce = 51; l_ce < 100; l_ce++ ) {
    REQUIRE( m_waveProp_Roe.getHeight()[l_ce]   == Approx(8) );
    REQUIRE( m_waveProp_Roe.getMomentumX()[l_ce] == Approx(0) );
  }
}