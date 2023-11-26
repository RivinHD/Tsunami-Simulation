/**
 * @author Alexander Breuer (alex.breuer AT uni-jena.de)
 *
 * IO-routines for writing a snapshot as Comma Separated Values (CSV).
 **/

#include "../../include/io/Csv.h"
#include <sstream>
#include <cmath>


void tsunami_lab::io::Csv::write( t_real i_dxy,
                                  t_idx i_nx,
                                  t_idx i_ny,
                                  t_idx i_stride,
                                  t_real const* i_h,
                                  t_real const* i_hu,
                                  t_real const* i_hv,
                                  t_real const* i_b,
                                  t_real const* i_hTotal,
                                  std::ostream& io_stream )
{
    // write the CSV header
    io_stream << "x,y";
    if( i_h != nullptr ) io_stream << ",height";
    if( i_hu != nullptr ) io_stream << ",momentum_x";
    if( i_hv != nullptr ) io_stream << ",momentum_y";
    if( i_b != nullptr ) io_stream << ",Bathymetry";
    if( i_hTotal != nullptr ) io_stream << ",totalHeight";
    io_stream << "\n";

    t_idx nanCount = 0;

    // iterate over all cells
    for( t_idx l_iy = 0; l_iy < i_ny; l_iy++ )
    {
        for( t_idx l_ix = 0; l_ix < i_nx; l_ix++ )
        {
            // derive coordinates of cell center
            t_real l_posX = ( l_ix + 0.5 ) * i_dxy;
            t_real l_posY = ( l_iy + 0.5 ) * i_dxy;

            t_idx l_id = l_iy * i_stride + l_ix;

            // write data
            io_stream << l_posX << "," << l_posY;
            if( i_h != nullptr )
            {
                io_stream << "," << i_h[l_id];
                nanCount += std::isnan( i_h[l_id] );
            }
            if( i_hu != nullptr )
            {
                io_stream << "," << i_hu[l_id];
                nanCount += std::isnan( i_hu[l_id] );
            }
            if( i_hv != nullptr )
            {
                io_stream << "," << i_hv[l_id];
                nanCount += std::isnan( i_hv[l_id] );
            }
            if( i_b != nullptr )
            {
                io_stream << "," << i_b[l_id];
                nanCount += std::isnan( i_b[l_id] );
            }
            if( i_hTotal != nullptr )
            {
                io_stream << "," << i_hTotal[l_id];
                nanCount += std::isnan( i_hTotal[l_id] );
            }
            io_stream << "\n";
        }
    }
    io_stream << std::flush;

    if( nanCount > 0 )
    {
        const char* reset = "\033[0m";
        const char* yellow = "\033[33;49m";

        std::cout << yellow << "  WARNING: " << nanCount << " nan values were written to this file!" << reset << std::endl;
#ifdef TSUNAMI_SIMULATION_TEST
        exit( EXIT_FAILURE );
#endif // TSUNAMI_SIMULATION_TEST
    }
}

bool tsunami_lab::io::Csv::next_middle_states( std::ifstream& stream,
                                               t_real& o_hLeft,
                                               t_real& o_hRight,
                                               t_real& o_huLeft,
                                               t_real& o_huRight,
                                               t_real& o_hStar )
{
    std::string line;

    // read next complete line
    while( std::getline( stream, line ) )
    {
        // skip commented lines
        if( line[0] == '#' )
        {
            continue;
        }

        // parse lines divided by ',' to single values
        std::istringstream lineStream( line );
        std::string hLeft;
        std::getline( lineStream, hLeft, ',' );
        o_hLeft = atof( hLeft.c_str() );
        std::string hRight;
        std::getline( lineStream, hRight, ',' );
        o_hRight = atof( hRight.c_str() );
        std::string huLeft;
        std::getline( lineStream, huLeft, ',' );
        o_huLeft = atof( huLeft.c_str() );
        std::string huRight;
        std::getline( lineStream, huRight, ',' );
        o_huRight = atof( huRight.c_str() );
        std::string hStar;
        std::getline( lineStream, hStar );
        o_hStar = atof( hStar.c_str() );
        return true;
    }
    // no lines left to read
    return false;
}

bool tsunami_lab::io::Csv::readBathymetry( std::ifstream& stream,
                                           t_real& o_hBathymetry )
{
    std::string line;

    // read next complete line
    while( std::getline( stream, line ) )
    {
        // skip commented lines
        if( line[0] == '#' )
        {
            continue;
        }

        // parse lines divided by ',' to single values
        std::istringstream lineStream( line );
        std::string longitude;
        std::getline( lineStream, longitude, ',' );
        std::string latitude;
        std::getline( lineStream, latitude, ',' );
        std::string location;
        std::getline( lineStream, location, ',' );
        std::string h_bathymetry;
        std::getline( lineStream, h_bathymetry, ',' );
        o_hBathymetry = atof( h_bathymetry.c_str() );
        return true;
    }
    // no lines left to read
    return false;
}