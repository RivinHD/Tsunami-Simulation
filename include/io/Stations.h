/**
 * @author Fabian Hofer, Vincent Gerlach
 *
 * Summarizes a collection of user-defined stations.
 **/

#ifndef TSUNAMISIMULATION_STATIONS_H
#define TSUNAMISIMULATION_STATIONS_H

#include <nlohmann/json.hpp>
#include <string>
#include "../constants.h"
#include <iostream>
#include <vector>
#include <filesystem> // requieres C++17 and up


 // for convenience
using json = nlohmann::json;

namespace tsunami_lab
{
    namespace io
    {
        class Stations;
    }
}

/**
 * Set a station that measure the height in a given frequency.
 * The frequency is independent of the simulation time.
*/
class tsunami_lab::io::Stations
{
private:

    /**
     * struct to save attributes of single station
    */
    struct Station
    {
        /**
         * struct to save attributes of single station
         *
         * @param i_name name of station
         * @param i_index mapped cell index of the station
         * @param i_path path to the station file to be written to
        */
        Station( std::string i_name, t_idx i_index, std::string i_path )
            : m_name( i_name ), m_index( i_index ), m_path( i_path )
        {
        }

        //! name of station
        std::string m_name;

        //! cell index
        t_idx m_index;

        //! path to the station file to be written to
        std::string m_path;
    };

    //! list of all stations
    std::vector<Station> m_stations;

    //! number of cells in x dimension
    t_idx m_nx;

    //! number of cells in y dimension
    t_idx m_ny;

    //! stride size
    t_idx m_stride;

    //! with of the grid
    t_real m_scaleX;

    //! height of the grid
    t_real m_scaleY;

    //! shared output frequency of stations
    t_real m_outputFrequency;

public:

    /**
     * Constructor of user-defined stations.
     *
     * @param i_nx number of cells in x dimension
     * @param i_ny number of cells in y dimension
     * @param i_stride stride size
     * @param i_scaleX width of the grid
     * @param i_scaleY height of the grid
     * @param isCheckpoint is true if the simulation is started from a checkpoint
     */
    Stations( t_idx i_nx,
              t_idx i_ny,
              t_idx i_stride,
              t_real i_scaleX,
              t_real i_scaleY,
              bool isCheckpoint = false );

    /**
     * Write totalHeight of each station to its csv file with a shared time step.
     *
     * @param time the current simulation time
     * @param totalHeight pointer to array of all totalHeights
     * @param momentumX pointer to array of all momentumX
     * @param momentumY pointer to array of all momentumY
     */
    void write( t_real time,
                const t_real* totalHeight,
                const t_real* momentumX,
                const t_real* momentumY ) const;

    /**
     * Returns shared output frequency of stations.
     *
     * @return shared output frequency
     */
    t_real getOutputFrequency();


};
#endif //TSUNAMISIMULATION_STATIONS_H
