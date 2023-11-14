/**
 * @author Fabian Hofer, Vincent Gerlach
 *
 * @section DESCRIPTION
 * Summarizes a collection of user-defined stations.
 **/

#include "../../include/io/Stations.h"
#include <fstream>
#include <cmath>
#include <fstream>

namespace fs = std::filesystem;

const std::string SOLUTION_FOLDER = "solutions";

tsunami_lab::io::Stations::Stations( t_idx i_nx,
                                     t_idx i_ny,
                                     t_idx i_stride,
                                     t_real i_scaleX,
                                     t_real i_scaleY )
{
    m_nx = i_nx;
    m_ny = i_ny;
    m_stride = i_stride;
    m_scaleX = i_scaleX;
    m_scaleY = i_scaleY;
    m_time = 0;

    std::ifstream l_file( "resources/config.json" );

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

    // add stations
    if( config.contains( "output_frequency" ) )
        m_outputFrequency = config["output_frequency"];
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
            l_fileStation << "timestep,totalHeight" << "\n";

            // forward arguments and construct station directly in the vector
            m_stations.emplace_back( l_name, l_x, l_y, l_path );
        }
    }
}

void tsunami_lab::io::Stations::write( const t_real* i_totalHeight )
{
    for( const Station& station : m_stations )
    {
        // map station index to cell index
        t_idx l_cellIndexX = roundf( ( m_nx / m_scaleX ) * station.m_x );
        t_idx l_cellIndexY = roundf( ( m_ny / m_scaleY ) * station.m_y );
        t_idx l_cellIndex = m_stride * l_cellIndexY + l_cellIndexX;

        std::ofstream l_file;
        l_file.open( station.m_path, std::ios::app );

        l_file << m_time << "," << i_totalHeight[l_cellIndex] << "\n";
        l_file.close();
    }
    m_time++;
}

tsunami_lab::t_real tsunami_lab::io::Stations::getOutputFrequency()
{
    return m_outputFrequency;
}