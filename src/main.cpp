/**
 * @author Alexander Breuer (alex.breuer AT uni-jena.de)
 *
 * @section DESCRIPTION
 * Entry-point for simulations.
 **/
#include "../include/patches/WavePropagation1d.h"
#include "../include/setups/DamBreak1d.h"
#include "../include/setups/RareRare1d.h"
#include "../include/setups/ShockShock1d.h"
#include "../include/io/Csv.h"
#include <cstdlib>
#include <iostream>
#include <cmath>
#include <fstream>
#include <limits>
#include <string>
#include <filesystem> // requieres C++17 and up

#define ARG_SOLVER "-s"

namespace fs = std::filesystem;

const std::string SOLUTION_FOLDER = "solutions";

int main( int   i_argc,
          char *i_argv[] ) {
  // number of cells in x- and y-direction
  tsunami_lab::t_idx l_nx = 0;
  tsunami_lab::t_idx l_ny = 1;

  // set cell size
  tsunami_lab::t_real l_dxy = 1;

  tsunami_lab::patches::Solver solver = tsunami_lab::patches::Solver::FWave;

  std::cout << "#####################################################" << std::endl;
  std::cout << "###                  Tsunami Lab                  ###" << std::endl;
  std::cout << "###                                               ###" << std::endl;
  std::cout << "### https://scalable.uni-jena.de                  ###" << std::endl;
  std::cout << "### https://rivinhd.github.io/Tsunami-Simulation/ ###" << std::endl;
  std::cout << "#####################################################" << std::endl;

  // error: wrong number of arguments.
  if( i_argc < 2 || i_argc == 3 || i_argc > 4) {
    std::cerr << "invalid number of arguments, usage:" << std::endl
              << "./build N_CELLS_X [-s <fwave|roe>]" << std::endl
              << "where N_CELLS_X is the number of cells in x-direction." << std::endl
              << "optional flag: '-s' set used solvers requires 'fwave' or 'roe' as inputs" << std::endl;
    return EXIT_FAILURE;
  }
  // flag: set solver.
  else if ( i_argc == 4)
  {
    // unknown flag.
    if ( ARG_SOLVER != std::string(i_argv[2]))
    {
      std::cerr << "unknown flag: " << i_argv[2] << std::endl; 
      return EXIT_FAILURE;
    }
    // set solver: roe
    if ( "roe" == std::string(i_argv[3]))
    {
      std::cout << "Set Solver: Roe" << std::endl;
      solver = tsunami_lab::patches::Solver::Roe;
    }
    // set solver: fwave
    else if ( "fwave" == std::string(i_argv[3]))
    {
      std::cout << "Set Solver: FWave" << std::endl;
    }
    else
    {
      std::cerr << "unknown argument for flag -s" << std::endl
                << "valid arguments are 'fwave', 'roe'" << std::endl;
      return EXIT_FAILURE;
    }
    
  }

  l_nx = atoi( i_argv[1] );
  if( l_nx < 1 ) {
      std::cerr << "invalid number of cells" << std::endl;
      return EXIT_FAILURE;
  }
  // choose default solver: fwave
  l_dxy = 10.0 / l_nx;

  std::cout << "runtime configuration" << std::endl;
  std::cout << "  number of cells in x-direction: " << l_nx << std::endl;
  std::cout << "  number of cells in y-direction: " << l_ny << std::endl;
  std::cout << "  cell size:                      " << l_dxy << std::endl;

  // construct setup
  tsunami_lab::setups::Setup *l_setup;

  tsunami_lab::t_real l_hl = 14;
  tsunami_lab::t_real l_hr = 3.5;
  // tsunami_lab::t_real l_ml = 2000;
  tsunami_lab::t_real l_location = 5;

  l_setup = new tsunami_lab::setups::DamBreak1d(l_hl, l_hr, l_location);
  // l_setup = new tsunami_lab::setups::RareRare1d(l_hl, l_ml, l_location);
  // l_setup = new tsunami_lab::setups::ShockShock1d(l_hl, l_ml, l_location);


  // construct solver
  tsunami_lab::patches::WavePropagation *l_waveProp;
  l_waveProp = new tsunami_lab::patches::WavePropagation1d( l_nx );
  
  // set the solver to use
  l_waveProp->setSolver(solver);

  // maximum observed height in the setup
  tsunami_lab::t_real l_hMax = std::numeric_limits< tsunami_lab::t_real >::lowest();

  // set up solver
  for( tsunami_lab::t_idx l_cy = 0; l_cy < l_ny; l_cy++ ) {
    tsunami_lab::t_real l_y = l_cy * l_dxy; 

    for( tsunami_lab::t_idx l_cx = 0; l_cx < l_nx; l_cx++ ) {
      tsunami_lab::t_real l_x = l_cx * l_dxy; 

      // get initial values of the setup
      tsunami_lab::t_real l_h = l_setup->getHeight( l_x,
                                                    l_y );
      l_hMax = std::max( l_h, l_hMax );

      tsunami_lab::t_real l_hu = l_setup->getMomentumX( l_x,
                                                        l_y );
      tsunami_lab::t_real l_hv = l_setup->getMomentumY( l_x,
                                                        l_y );

      // set initial values in wave propagation solver
      l_waveProp->setHeight( l_cx,
                             l_cy,
                             l_h );

      l_waveProp->setMomentumX( l_cx,
                                l_cy,
                                l_hu );

      l_waveProp->setMomentumY( l_cx,
                                l_cy,
                                l_hv );

    }
  }

  // derive maximum wave speed in setup; the momentum is ignored
  tsunami_lab::t_real l_speedMax = std::sqrt( 9.81 * l_hMax );
  std::cout << "Max speed" << l_speedMax << std::endl;

  // derive constant time step; changes at simulation time are ignored
  tsunami_lab::t_real l_dt = 0.5 * l_dxy / l_speedMax;

  // derive scaling for a time step
  tsunami_lab::t_real l_scaling = l_dt / l_dxy;

  // set up time and print control
  tsunami_lab::t_idx  l_timeStep = 0;
  tsunami_lab::t_idx  l_nOut = 0;
  tsunami_lab::t_real l_endTime = 1.25;
  tsunami_lab::t_real l_simTime = 0;

  // create solution folder
  if (!fs::is_directory(SOLUTION_FOLDER)) 
  {
    if (fs::exists(SOLUTION_FOLDER))
    {
      fs::rename(SOLUTION_FOLDER.c_str(), (SOLUTION_FOLDER + ".file").c_str());
    }
    fs::create_directory(SOLUTION_FOLDER); 
  }

  std::cout << "entering time loop" << std::endl;

  // iterate over time
  while( l_simTime < l_endTime ){
    if( l_timeStep % 25 == 0 ) {
      std::cout << "  simulation time / #time steps: "
                << l_simTime << " / " << l_timeStep << std::endl;

      std::string l_path = SOLUTION_FOLDER + "/solution_" + std::to_string(l_nOut) + ".csv";
      std::cout << "  writing wave field to " << l_path << std::endl;

      std::ofstream l_file;
      l_file.open( l_path  );

      tsunami_lab::io::Csv::write( l_dxy,
                                   l_nx,
                                   1,
                                   1,
                                   l_waveProp->getHeight(),
                                   l_waveProp->getMomentumX(),
                                   nullptr,
                                   l_file );
      l_file.close();
      l_nOut++;
    }

    l_waveProp->setGhostOutflow();
    l_waveProp->timeStep( l_scaling );

    l_timeStep++;
    l_simTime += l_dt;
  }

  std::cout << "finished time loop" << std::endl;

  // free memory
  std::cout << "freeing memory" << std::endl;
  delete l_setup;
  delete l_waveProp;

  std::cout << "finished, exiting" << std::endl;
  return EXIT_SUCCESS;
}
