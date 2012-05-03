/***************************************************************************\
  File: IMR_Geom_Prim_Point.cpp
  Description: Point primitive geometry module.
  Author: Daniel Hawthorn
  Modified:
  
\***************************************************************************/
#include "IMR_Geom_Prim_Point.hpp"

/***************************************************************************\
  Transforms a point using the specified matrix.
\***************************************************************************/
void IMR_3DPoint::Transform(IMR_Matrix &mtrx)
{
tX = FixedMult(aX, mtrx.Mtrx[0][0]) + 
     FixedMult(aY, mtrx.Mtrx[1][0]) + 
     FixedMult(aZ, mtrx.Mtrx[2][0]) + mtrx.Mtrx[3][0];
tY = FixedMult(aX, mtrx.Mtrx[0][1]) + 
     FixedMult(aY, mtrx.Mtrx[1][1]) + 
     FixedMult(aZ, mtrx.Mtrx[2][1]) + mtrx.Mtrx[3][1];
tZ = FixedMult(aX, mtrx.Mtrx[0][2]) + 
     FixedMult(aY, mtrx.Mtrx[1][2]) + 
     FixedMult(aZ, mtrx.Mtrx[2][2]) + mtrx.Mtrx[3][2];
aX = tX; aY = tY; aZ = tZ;
 }

