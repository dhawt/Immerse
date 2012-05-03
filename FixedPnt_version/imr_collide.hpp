/***************************************************************************\
  File: IMR_Collide.hpp
  Description: Function set that detects collisions.
  Author: Daniel Hawthorn
  Modified:
  
\***************************************************************************/
#ifndef __IMR_COLLIDE__HPP
#define __IMR_COLLIDE__HPP

// Include headers:
#include <math.h>
#include "IMR_Geom_Prim.hpp"
#include "IMR_Geom_Poly.hpp"

// Prototypes:
int IMR_Collide_HitsPoly(IMR_3DPoint &Pnt1, IMR_3DPoint &Pnt2, IMR_Polygon &Poly);
//IMR_3DPoint IMR_Collide_GetHitPoint(IMR_3DPoint &Pnt1, IMR_3DPoint &Pnt2, IMR_Polygon &Poly);

#endif
