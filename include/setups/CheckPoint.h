/**
 * @author Fabian Hofer, Vincent Gerlach (AT uni-jena.de)
 *
 * Declaration of ceckpoint setup.
 **/

#ifndef TSUNAMI_SIMULATION_CHECKPOINT_2D_H
#define TSUNAMI_SIMULATION_CHECKPOINT_2D_H

#include "Setup.h"
#include "../io/NetCdf.h"

namespace tsunami_lab
{
    namespace setups
    {
        class Checkpoint;
    }
}

class tsunami_lab::setups::Checkpoint : public Setup
{
private:
    //! the variables to parse for the 
    static const char* variables[8];

    //! the data to store the read values
    tsunami_lab::io::NetCdf::VarArray data[8];

    //! scale of the simulation in x-direction
    t_real scaleX = 1;

    //! scale of the simulation in y-direction
    t_real scaleY = 1;

public:
    /**
     * Default constructor with the default example.
     *
     * @param filepath the filepath to the checkpoint file
     * @param scale_x the scale in x direction of the simulation
     * @param scale_y the scale in y direction of the simulation
     * @param timeStep output of the checkpointed time step
     * @param writeCount output of the checkpointed write count
     * @param simulationTime ouput of the checkpointed simulation time
     * @param argv the command line parameter of the saved execution to restore
    */
    Checkpoint( const char* filepath,
                t_real scale_x,
                t_real scale_y,
                t_idx& timeStep,
                t_idx& writeCount,
                t_real& simulationTime,
                std::vector<char*>& argv );

    /**
     * Gets the water height at a given point.
     *
     * @param i_x x-coordinate of the queried point.
     * @param i_y y-coordinate of the queried point.
     * @return height at the given point.
     **/
    t_real getHeight( t_real i_x,
                      t_real i_y ) const override;

    /**
     * Gets the momentum in x-direction.
     *
     * @param i_x x-coordinate of the queried point.
     * @param i_y y-coordinate of the queried point.
     * @return momentum in x-direction.
     **/
    t_real getMomentumX( t_real i_x,
                         t_real i_y ) const override;

    /**
     * Gets the momentum in y-direction.
     *
     * @param i_x x-coordinate of the queried point.
     * @param i_y y-coordinate of the queried point.
     * @return momentum in y-direction.
     **/
    t_real getMomentumY( t_real i_x,
                         t_real i_y ) const override;

    /**
    * Gets the bathymetry at a given point.
    *
    * @param i_x x-coordinate of the queried point.
    * @param i_y y-coordinate of the queried point.
    * @return bathymetry at a given point.
    **/
    t_real getBathymetry( t_real i_x,
                          t_real i_y ) const override;
};

#endif