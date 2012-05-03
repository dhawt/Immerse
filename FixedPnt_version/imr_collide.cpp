/***************************************************************************\
  File: IMR_Collide.cpp
  Description: Collision module.
  Author: Daniel Hawthorn
  Modified:
  
\***************************************************************************/
#include "IMR_Collide.hpp"

/***************************************************************************\
  Returns true if a line drawn between the specified points pierces the
  specified Poly.  Sorta accurate - but somtimes gives false readings due
  to using the radius.
\***************************************************************************/
int IMR_Collide_HitsPoly(IMR_3DPoint &Pnt1, IMR_3DPoint &Pnt2, IMR_Polygon &Poly)
{
float nX, nY, nZ, Offset, T,
      dX, dY, dZ, DistSquared,
      Sign1, Sign2;
IMR_3DPoint Hit, Delta;

//
// -- First check if the two points are on opposite sides of the plane
// -- of the Poly:
//

// Find normal to Poly:
nX = Poly.Normal->X - Poly.Vtx_List[0]->X;
nY = Poly.Normal->Y - Poly.Vtx_List[0]->Y;
nZ = Poly.Normal->Z - Poly.Vtx_List[0]->Z;

// Find offset:
Offset = -((nX * Poly.Vtx_List[0]->X) +
           (nY * Poly.Vtx_List[0]->Y) +
           (nZ * Poly.Vtx_List[0]->Z));

// Find the signs of the two points:
Sign1 = (nX * Pnt1.X) + (nY * Pnt1.Y) + (nZ * Pnt1.Z) + Offset;
Sign2 = (nX * Pnt2.X) + (nY * Pnt2.Y) + (nZ * Pnt2.Z) + Offset;

// If the signs are the same, the line doesn't pierce the Poly:
if ((Sign1 >= 0 && Sign2 >= 0) ||
    (Sign1 <= 0 && Sign2 <= 0)) return 0;

//
// -- Now find the point that the line pierces the Poly:
//
Delta = Pnt2 - Pnt1;
dX = Delta.X;
dY = Delta.Y;
dZ = Delta.Z;
if (nX && dX)
    {
    Hit.X = (-Offset - (nY * Pnt1.Y) - (nZ * Pnt1.Z)) / nX;
    T = (Hit.X - Pnt2.X) / dX;
    Hit.Y = Pnt2.Y + (dY * T);
    Hit.Z = Pnt2.Z + (dZ * T);
     }
else if (nY && dY)
    {
    Hit.Y = (-Offset - (nX * Pnt1.X) - (nZ * Pnt1.Z)) / nY;
    T = (Hit.Y - Pnt2.Y) / dY;
    Hit.X = Pnt2.X + (dX * T);
    Hit.Z = Pnt2.Z + (dZ * T);
     }
else if (nZ && dZ)
    {
    Hit.Z = (-Offset - (nX * Pnt1.X) - (nY * Pnt1.Y)) / nZ;
    T = (Hit.Z - Pnt2.Z) / dZ;
    Hit.X = Pnt2.X + (dX * T);
    Hit.Y = Pnt2.Y + (dY * T);
     }

//
// -- If the point is within the radius of the Poly, then a collision has occured.
//    (probably...)
//
Delta = Poly.Centroid - Hit;
dX = Delta.X;
dY = Delta.Y; 
dZ = Delta.Z;
DistSquared = (dX * dX) + (dY * dY) + (dZ * dZ);
if (DistSquared < Poly.RadiusSquared)
    return 1;

// The point is outside the Poly, so no collision has occured:
return 0;
 }
