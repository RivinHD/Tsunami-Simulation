/**
 * @author Fabian Hofer, Vincent Gerlach (AT uni-jena.de)
 *
 * @section DESCRIPTION
 * One-dimensional initialization of h, hu and b using the bathymetry reader and the artificial displacement.
 **/

#include "../../include/setups/TsunamiEvent1d.h"
#include "../../include/io/Csv.h"

tsunami_lab::setups::TsunamiEvent1d::TsunamiEvent1d( std::string filePath,
                                                     tsunami_lab::t_real i_delta,
                                                     tsunami_lab::t_real i_scale )
{
    m_delta = i_delta;
    m_scale = i_scale;

    t_real o_hBathymetry = 0;
    std::ifstream bathymetry_profile( filePath );
    while( tsunami_lab::io::Csv::readBathymetry( bathymetry_profile, o_hBathymetry ) )
    {
        m_bathymetry.push_back( o_hBathymetry );
    }
    m_csvDataPoint = m_bathymetry.size() - 1;
}

tsunami_lab::t_real tsunami_lab::setups::TsunamiEvent1d::getHeight( tsunami_lab::t_real i_x,
                                                                    tsunami_lab::t_real ) const
{
    // linear interpolation between two bathymetries
    t_real l_x = ( i_x / m_scale ) * m_csvDataPoint;
    t_idx indexL = std::floor( l_x );
    t_idx indexR = std::ceil( l_x );
    t_real l_bathymetryL = m_bathymetry[indexL];
    t_real l_bathymetryR = m_bathymetry[indexR];
    t_real l_bathymetry = ( l_bathymetryR - l_bathymetryL ) * ( l_x - indexL ) + l_bathymetryL;

    if( l_bathymetry < 0 )
    {
        return -l_bathymetry < m_delta ? m_delta : -l_bathymetry;
    }
    return 0;
}

tsunami_lab::t_real tsunami_lab::setups::TsunamiEvent1d::getMomentumX( tsunami_lab::t_real,
                                                                       tsunami_lab::t_real ) const
{
    return m_momentum;
}

tsunami_lab::t_real tsunami_lab::setups::TsunamiEvent1d::getMomentumY( tsunami_lab::t_real,
                                                                       tsunami_lab::t_real ) const
{
    return 0;
}

tsunami_lab::t_real tsunami_lab::setups::TsunamiEvent1d::getBathymetry( tsunami_lab::t_real i_x,
                                                                        tsunami_lab::t_real ) const
{
    // linear interpolation between two bathymetries
    t_real l_x = ( i_x / m_scale ) * m_csvDataPoint;
    t_idx indexL = std::floor( l_x );
    t_idx indexR = std::ceil( l_x );
    t_real l_bathymetryL = m_bathymetry[indexL];
    t_real l_bathymetryR = m_bathymetry[indexR];
    t_real l_bathymetry = ( l_bathymetryR - l_bathymetryL ) * ( l_x - indexL ) + l_bathymetryL;
    t_real verticalDisplacement = getVerticalDisplacement( i_x, 0 );

    if( l_bathymetry < 0 )
    {
        return l_bathymetry < -m_delta ? l_bathymetry + verticalDisplacement : -m_delta + verticalDisplacement;
    }
    return l_bathymetry < m_delta ? m_delta + verticalDisplacement : l_bathymetry + verticalDisplacement;
}

tsunami_lab::t_real tsunami_lab::setups::TsunamiEvent1d::getVerticalDisplacement( tsunami_lab::t_real i_x,
                                                                                  tsunami_lab::t_real ) const
{
    if( 175000 < i_x && i_x < 250000 )
    {
        return 1000 * std::sin( ( ( i_x - 175000 ) / 37500 * M_PI ) + M_PI );
    }
    return 0;
}


