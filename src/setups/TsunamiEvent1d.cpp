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
                                                     tsunami_lab::t_real i_scale)
{
    m_delta = i_delta;
    m_scale = i_scale;

    t_real o_hBathy = 0;
    std::ifstream bathy_profile( filePath );
    while(tsunami_lab::io::Csv::readBathymetry(bathy_profile, o_hBathy))
    {
        m_bathy.push_back(o_hBathy);
    }
    m_csvDataPoint = m_bathy.size() - 1;




}

tsunami_lab::t_real tsunami_lab::setups::TsunamiEvent1d::getHeight(tsunami_lab::t_real i_x,
                                                                   tsunami_lab::t_real) const
{
    t_real l_x = (i_x / m_scale) * m_csvDataPoint;
    t_idx indexL = std::floor(l_x);
    t_idx indexR = std::ceil(l_x);
    t_real l_bathyL = m_bathy[indexL];
    t_real l_bathyR = m_bathy[indexR];
    t_real l_bathy = (l_bathyR - l_bathyL) * (l_x - indexL) + l_bathyL;

    if(l_bathy < 0)
    {
        return -l_bathy < m_delta ? m_delta : -l_bathy;
    }
    return 0;
}

tsunami_lab::t_real tsunami_lab::setups::TsunamiEvent1d::getMomentumX(tsunami_lab::t_real,
                                                                      tsunami_lab::t_real) const
{
    return m_momentum;
}

tsunami_lab::t_real tsunami_lab::setups::TsunamiEvent1d::getMomentumY(tsunami_lab::t_real,
                                                                      tsunami_lab::t_real) const {
    return 0;
}

tsunami_lab::t_real tsunami_lab::setups::TsunamiEvent1d::getBathymetry(tsunami_lab::t_real i_x,
                                                                       tsunami_lab::t_real) const
{
    t_real l_x = (i_x / m_scale) * m_csvDataPoint;
    t_idx indexL = std::floor(l_x);
    t_idx indexR = std::ceil(l_x);
    t_real l_bathyL = m_bathy[indexL];
    t_real l_bathyR = m_bathy[indexR];
    t_real l_bathy = (l_bathyR - l_bathyL) * (l_x - indexL) + l_bathyL;
    t_real verticalDisplacement = getVerticalDisplacement(i_x, 0);

    if(l_bathy < 0)
    {
        return l_bathy < -m_delta ? l_bathy + verticalDisplacement : -m_delta + verticalDisplacement;
    }
    return l_bathy < m_delta ? m_delta + verticalDisplacement : l_bathy + verticalDisplacement;
}

tsunami_lab::t_real tsunami_lab::setups::TsunamiEvent1d::getVerticalDisplacement(tsunami_lab::t_real i_x,
                                                                                 tsunami_lab::t_real ) const
{
    if(175000 < i_x && i_x < 250000)
    {
        return 1000 * std::sin( ((i_x - 175000) / 37500 * M_PI) + M_PI);
    }
    return 0;
}


