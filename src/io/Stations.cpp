/**
 * @author Fabian Hofer, Vincent Gerlach
 *
 * @section DESCRIPTION
 * Summarizes a collection of user-defined stations.
 **/

#include "../../include/io/Stations.h"
#include <fstream>

namespace fs = std::filesystem;

const std::string SOLUTION_FOLDER = "solutions";

tsunami_lab::io::Stations::Stations(t_idx i_nx,
                                    t_idx i_ny,
                                    t_idx i_stride,
                                    t_real i_scaleX,
                                    t_real i_scaleY)
{
    m_nx = i_nx;
    m_ny = i_ny;
    m_stride = i_stride;
    m_scaleX = i_scaleX;
    m_scaleY = i_scaleY;

    std::ifstream l_file("resources/config.json");

    json config;
    try
    {
        l_file >> config;
    }
    catch( const std::exception &e)
    {
        std::cerr << "Error parsing JSON: " << e.what() << std::endl;
        exit(1);
    }

    if(config.contains("output_frequency"))
        m_outputfrequency = config["output_frequency"];
    if(config.contains("stations"))
    {
        for( size_t i = 0; i < config["output_frequency"].size(); i++)
        {
            std::string l_name = config["output_frequency"][i]["name"];
            t_real l_x = config["output_frequency"][i]["x"];
            t_real l_y = config["output_frequency"][i]["y"];
            std::string l_path = "solutions/stations/" + l_name;

            // create station folder inside solution folder
            if( !fs::exists( SOLUTION_FOLDER ))
            {
                fs::create_directory( SOLUTION_FOLDER );
            }
            if( fs::exists( SOLUTION_FOLDER + "/station" ) )
            {
                fs::remove_all( SOLUTION_FOLDER + "/station" );
            }
            fs::create_directory(SOLUTION_FOLDER + "/station" );

            std::ofstream l_fileStation;
            l_fileStation.open( l_path );
            l_fileStation << "x, y";

            // forward arguments and construct station directly in the vector
            m_stations.emplace_back(l_name, l_x, l_y, l_path);
        }
    }
}

void tsunami_lab::io::Stations::write()
{
    for(const Station& station : m_stations)
    {
        std::ofstream l_file;
        l_file.open(station.m_path);

        l_file << station.m_x << "," << station.m_y;

        l_file.close();
    }
}