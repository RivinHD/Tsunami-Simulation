/**
 * @author Fabian Hofer, Vincent Gerlach (AT uni-jena.de)
 *
 * @section DESCRIPTION
 * One-dimensional shock-shock problem.
 **/

#ifndef SHOCKSHOCK1D_H
#define SHOCKSHOCK1D_H

#include "Setup.h"

namespace tsunami_lab {
  namespace setups {
    class ShockShock1d;
  }
}

class tsunami_lab::setups::ShockShock1d : public Setup
{
private:
  t_real m_heightLeft = 0;
  t_real m_heightRight = 0;
  t_real m_momentumLeft = 0;
  t_real m_locationShock = 0;
public:
  ShockShock1d( t_real i_heightLeft,
                t_real i_heightRight,
                t_real i_momentumLeft,
                t_real i_locationShock);
  ~ShockShock1d();

  t_real getHeight( t_real i_x, t_real i_y ) const;
  t_real getMomentumX( t_real i_x, t_real i_y ) const;
  t_real getMomentumY( t_real i_x, t_real i_y ) const;
};


#endif // SHOCKSHOCK1D_H