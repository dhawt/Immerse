/****************************************************************\
 
 iMMERSE Engine
 (C) 1999 No Tears Shed Software
 All rights reserved
 
 Filename: IMR_Geom_Prim_Point.hpp
 Description: Point primitive geometry module
 
\****************************************************************/
#include "IMR_Geom_Prim_Point.hpp"

/***************************************************************************\
  Transforms a point using the specified matrix.
\***************************************************************************/
void IMR_3DPoint::Transform(IMR_Matrix &mtrx)
{
tX = (aX * mtrx.Mtrx[0][0]) + 
     (aY * mtrx.Mtrx[1][0]) + 
     (aZ * mtrx.Mtrx[2][0]) + mtrx.Mtrx[3][0];
tY = (aX * mtrx.Mtrx[0][1]) + 
     (aY * mtrx.Mtrx[1][1]) + 
     (aZ * mtrx.Mtrx[2][1]) + mtrx.Mtrx[3][1];
tZ = (aX * mtrx.Mtrx[0][2]) + 
     (aY * mtrx.Mtrx[1][2]) + 
     (aZ * mtrx.Mtrx[2][2]) + mtrx.Mtrx[3][2];
aX = tX; aY = tY; aZ = tZ;
 }

