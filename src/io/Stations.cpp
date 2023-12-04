/**
 * @author Fabian Hofer, Vincent Gerlach
 *
 * Summarizes a collection of user-defined stations.
 **/

#include "../../include/io/Stations.h"
#include <fstream>
#include <cmath>
#include <limits>

namespace fs = std::filesystem;

const std::string SOLUTION_FOLDER = "solutions";

tsunami_lab::io::Stations::Stations( t_idx i_nx,
                                     t_idx i_ny,
                                     t_idx i_stride,
                                     t_real i_scaleX,
                                     t_real i_scaleY,
                                     bool isCheckpoint )
{
    m_nx = i_nx;
    m_ny = i_ny;
    m_stride = i_stride;
    m_scaleX = i_scaleX;
    m_scaleY = i_scaleY;

#ifdef TSUNAMI_SIMULATION_TEST
    std::ifstream l_file( "resources/config.test.json" );
#endif // TSUNAMI_SIMULATION_TEST
#ifndef TSUNAMI_SIMULATION_TEST
    std::ifstream l_file( "resources/config.json" );
#endif // !TSUNAMI_SIMULATION_TEST

    json config;
    try
    {
        l_file >> config;
    }
    catch( const std::exception& e )
    {
        std::cerr << "Error parsing JSON: " << e.what() << std::endl;
        exit( 1 );
    }

    if( !isCheckpoint )
    {
        // create station folder inside solution folder
        if( !fs::exists( SOLUTION_FOLDER ) )
        {
            fs::create_directory( SOLUTION_FOLDER );
        }
        if( fs::exists( SOLUTION_FOLDER + "/station" ) )
        {
            fs::remove_all( SOLUTION_FOLDER + "/station" );
        }
        fs::create_directory( SOLUTION_FOLDER + "/station" );
    }

    if( config.contains( "output_frequency" ) )
        m_outputFrequency = config["output_frequency"];
    // add stations
    if( config.contains( "stations" ) )
    {
        for( size_t i = 0; i < config["stations"].size(); i++ )
        {
            std::string l_name = config["stations"][i]["name"];
            t_real l_x = config["stations"][i]["x"];
            t_real l_y = config["stations"][i]["y"];
            std::string l_path = SOLUTION_FOLDER + "/station/" + l_name;

            std::ofstream l_fileStation;
            l_fileStation.open( l_path, std::ios::app );
            l_fileStation << "timestep,totalHeight,momentumX,momentumY" << std::endl;

            // map station index to cell index
            t_idx l_cellIndexX = roundf( ( m_nx / m_scaleX ) * l_x );
            if( l_cellIndexX >= m_nx )
            {
                l_cellIndexX = m_nx - 1;
            }
            t_idx l_cellIndexY = roundf( ( m_ny / m_scaleY ) * l_y );
            if( l_cellIndexY >= m_ny )
            {
                l_cellIndexY = m_ny - 1;
            }
            t_idx l_cellIndex = m_stride * l_cellIndexY + l_cellIndexX;

            // forward arguments and construct station directly in the vector
            m_stations.emplace_back( l_name, l_cellIndex, l_path );
        }
    }
}

void tsunami_lab::io::Stations::write( t_real time,
                                       const t_real* i_totalHeight,
                                       const t_real* momentumX,
                                       const t_real* momentumY ) const
{
    for( const Station& station : m_stations )
    {

        std::ofstream l_file;
        l_file.open( station.m_path, std::ios::app );

        l_file << time
            << "," << ( i_totalHeight != nullptr ? i_totalHeight[station.m_index] : std::numeric_limits<float>::quiet_NaN() )
            << "," << ( momentumX != nullptr ? momentumX[station.m_index] : std::numeric_limits<float>::quiet_NaN() )
            << "," << ( momentumX != nullptr ? momentumY[station.m_index] : std::numeric_limits<float>::quiet_NaN() )
            << std::endl;
        l_file.close();
    }
}

tsunami_lab::t_real tsunami_lab::io::Stations::getOutputFrequency()
{
    return m_outputFrequency;
}