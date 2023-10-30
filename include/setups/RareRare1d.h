/**
 * @author Fabian Hofer, Vincent Gerlach (AT uni-jena.de)
 *
 * @section DESCRIPTION
 * One-dimensional rare-rare problem.
 **/

#ifndef RARERARE1D_H
#define RARERARE1D_H

#include "Setup.h"

namespace tsunami_lab {
  namespace setups {
    class RareRare1d;
  }
}

class tsunami_lab::setups::RareRare1d : public Setup
{
private:
  t_real m_heightLeft = 0;
  t_real m_heightRight = 0;
  t_real m_momentumLeft = 0;
  t_real m_locationRare = 0;
public:
  RareRare1d( t_real i_heightLeft,
                t_real i_heightRight,
                t_real i_momentumLeft,
                t_real i_locationRare);
  ~RareRare1d();

  t_real getHeight( t_real i_x, t_real i_y ) const;
  t_real getMomentumX( t_real i_x, t_real i_y ) const;
  t_real getMomentumY( t_real i_x, t_real i_y ) const;
};

#endif // RARERARE1D_H