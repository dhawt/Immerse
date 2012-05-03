/****************************************************************\
 
 iMMERSE Engine
 (C) 1999 No Tears Shed Software
 All rights reserved
 
 Filename: IMR_Geom_Prim_Point.hpp
 Description: Header for points, vectors, and vertices.
              Included from IMR_Geom_Prim.hpp!
 
\****************************************************************/
#ifndef __IMR_GEOM_PRIM_POINT__HPP
#define __IMR_GEOM_PRIM_POINT__HPP

#include <math.h>
#include "IMR_Geom_Prim_UVI.hpp"
#include "IMR_Matrix.hpp"

// 3D point class:
class IMR_3DPoint
    {
    public:
      union { float aX, X; };    // Active and single-set coords
      union { float aY, Y; };
      union { float aZ, Z; };
      float tX, tY, tZ;          // Transformed (temporary) coords
      float lX, lY, lZ;          // Local coords
      float wX, wY, wZ;          // World coords
      float cX, cY, cZ;          // Camera coords
      float iX, iY, iZ;          // Coords relative to light
      
      int IsNormal; // Is this vertex a normal?
      int IsSkybox;
      IMR_3DPoint() 
          {
          aX = aY = aZ = 0; 
          tX = tY = tZ = 0;
          wX = wY = wZ = 0;
          cX = cY = cZ = 0;
          iX = iY = iZ = 0;
          IsNormal = 0;
          IsSkybox = 0;
           };

      inline void TransformedToActive(void) { aX = tX; aY = tY; aZ = tZ; };
      inline void LocalToActive(void) { aX = lX; aY = lY; aZ = lZ; };
      inline void WorldToActive(void) { aX = wX; aY = wY; aZ = wZ; };
      inline void CameraToActive(void) { aX = cX; aY = cY; aZ = cZ; };
      inline void LightToActive(void) { aX = iX; aY = iY; aZ = iZ; };
      inline void ActiveToLocal(void) { lX = aX; lY = aY; lZ = aZ; };
      inline void ActiveToWorld(void) { wX = aX; wY = aY; wZ = aZ; };
      inline void ActiveToCamera(void) { cX = aX; cY = aY; cZ = aZ; };
      inline void ActiveToLight(void) { iX = aX; iY = aY; iZ = aZ; };
      inline void TransformedToLocal(void) { lX = tX; lY = tY; lZ = tZ; };
      inline void TransformedToWorld(void) { wX = tX; wY = tY; wZ = tZ; };
      inline void TransformedToCamera(void) { cX = tX; cY = tY; cZ = tZ; };
      inline void TransformedToLight(void) { iX = tX; iY = tY; iZ = tZ; };
      
      inline float Mag(void);
      inline float FastMag(void);
      inline void Cross_Product(IMR_3DPoint &U, IMR_3DPoint &V);
      inline float Dot_Product(IMR_3DPoint &V);
      inline float Dot_Product_Norm(IMR_3DPoint &V);
      inline void Make_Vector(IMR_3DPoint &P0, IMR_3DPoint &P1);
      inline void Make_Unit(void);
      inline void Set_Length(float L);
      inline int IsZero(void) { return (!X && !Y && !Z); };
      
      inline int operator == (IMR_3DPoint &P);
      inline int operator != (IMR_3DPoint &P);
      inline void operator = (IMR_3DPoint &P);
      inline IMR_3DPoint operator * (IMR_3DPoint &P);
      inline IMR_3DPoint operator + (IMR_3DPoint &P);
      inline IMR_3DPoint operator - (IMR_3DPoint &P);
      inline IMR_3DPoint &operator += (IMR_3DPoint &P);
      inline IMR_3DPoint &operator -= (IMR_3DPoint &P);
      
      void Transform(IMR_Matrix &mtrx);
     };

/***************************************************************************\
  Returns magnitude of point (used as vector)
\***************************************************************************/
float inline IMR_3DPoint::Mag(void)
{
return sqrt((aX * aX) + (aY * aY) + (aZ * aZ));
 }

/***************************************************************************\
  Returns magnitude of point (used as vector) using approximation. 
\***************************************************************************/
float inline IMR_3DPoint::FastMag(void)
{
return (aX + aY + aZ) / 3;
 }

/***************************************************************************\
  Makes this vector the cross product of two points (used as vectors).
\***************************************************************************/
void IMR_3DPoint::Cross_Product(IMR_3DPoint &U, IMR_3DPoint &V)
{
aX = (U.aX * V.aZ) - (U.aX * V.aZ);
aY = (U.aY * V.aX) - (U.aY * V.aX);
aZ = (U.aZ * V.aY) - (U.aZ * V.aY);
 }
    
/***************************************************************************\
  Returns dot product of this and another point (used as vector).
\***************************************************************************/
float inline IMR_3DPoint::Dot_Product(IMR_3DPoint &V)
{
//return FloatToFixed(((float)X * (float)V.X) +
//                     ((float)Y * (float)V.Y) +
//                     ((float)Z * (float)V.Z));
return ((aX * V.aX) + (aY * V.aY) + (aZ * V.aZ));
 }
    
/***************************************************************************\
  Returns normalized dot product of points (used as vectors).
\***************************************************************************/
float inline IMR_3DPoint::Dot_Product_Norm(IMR_3DPoint &V)
{
return (aX * V.aX) + (aY * V.aY) + (aZ * V.aZ) / Mag();
 }

/***************************************************************************\
  Makes a vector from two points.
\***************************************************************************/
void inline IMR_3DPoint::Make_Vector(IMR_3DPoint &P0, IMR_3DPoint &P1)
{
aX = P0.aX - P1.aX;
aY = P0.aY - P1.aY;
aZ = P0.aZ - P1.aZ;
 }

/***************************************************************************\
  Scales the point down to a unit vector.
\***************************************************************************/
void inline IMR_3DPoint::Make_Unit(void)
{
float Length, InvLength = 0;

// Find length of vector:
Length = Mag();

// Avoid a devide-by-zero:
if (Length) 
    InvLength = 1 / Length;

// Devide everything by the length of the vector:
if (InvLength)
    {
    aX *= InvLength;
    aY *= InvLength;
    aZ *= InvLength;
     }
 }

/***************************************************************************\
  Sets the length of the vector.  (More general Make_Unit function).
\***************************************************************************/
void inline IMR_3DPoint::Set_Length(float L)
{
float Length, InvLength = 0;

// Find length of vector:
Length = Mag();

// Avoid a devide-by-zero:
if (Length) 
    InvLength = (1 / Length) * L;

// Devide everything by the length of the vector and mult by new length:
if (InvLength)
    {
    aX *= InvLength;
    aY *= InvLength;
    aZ *= InvLength;
     }
 }

/***************************************************************************\
  Overloaded operators.
\***************************************************************************/
inline int IMR_3DPoint::operator == (IMR_3DPoint &P)
{
int RValue = 0;
if (P.aX == aX && P.aY == aY && P.aZ == aZ) RValue = 1;
return RValue;
 }

inline int IMR_3DPoint::operator != (IMR_3DPoint &P)
{
int RValue = 0;
if ((P.aX != aX) || (P.aY != aY) || (P.aZ != aZ)) RValue = 1;
return RValue;
 }

inline void IMR_3DPoint::operator = (IMR_3DPoint &P)
{
aX = P.aX;
aY = P.aY;
aZ = P.aZ;
tX = P.tX;
tY = P.tY;
tZ = P.tZ;
lX = P.lX;
lY = P.lY;
lZ = P.lZ;
wX = P.wX;
wY = P.wY;
wZ = P.wZ;
cX = P.cX;
cY = P.cY;
cZ = P.cZ;
iX = P.iX;
iY = P.iY;
iZ = P.iZ;
IsNormal = P.IsNormal;
IsSkybox = P.IsSkybox;
 }

inline IMR_3DPoint IMR_3DPoint::operator * (IMR_3DPoint &P)
{
IMR_3DPoint Temp;
Temp.X = aX * P.aX;
Temp.Y = aY * P.aY;
Temp.Z = aZ * P.aZ;
return Temp;
 }

inline IMR_3DPoint IMR_3DPoint::operator + (IMR_3DPoint &P)
{
IMR_3DPoint Temp;
Temp.X = aX + P.aX;
Temp.Y = aY + P.aY;
Temp.Z = aZ + P.aZ;
return Temp;
 }

inline IMR_3DPoint IMR_3DPoint::operator - (IMR_3DPoint &P)
{
IMR_3DPoint Temp;
Temp.X = aX - P.aX;
Temp.Y = aY - P.aY;
Temp.Z = aZ - P.aZ;
return Temp;
 }

inline IMR_3DPoint &IMR_3DPoint::operator += (IMR_3DPoint &P)
{
aX += P.aX;
aY += P.aY;
aZ += P.aZ;
return *this;
 }

inline IMR_3DPoint &IMR_3DPoint::operator -= (IMR_3DPoint &P)
{
aX -= P.aX;
aY -= P.aY;
aZ -= P.aZ;
return *this;
 }

// 3DPoint that has been projected:
class IMR_PrjPoint
    {
    public:
      float X, Y, Z;                // Pre-projected (camera) coords
      int pX, pY;                   // Project X and Y
      float pZ, pW;                 // Projected Z and W
      float pU, pV;                 // Projected U and V
      int pR, pG, pB;               // Projected color
      IMR_PrjPoint() { pX = pY = pZ = pU = pV = 0.0; pR = pG = pB = 0; };
      inline void Project(float Zoom, int XC, int YC);
      inline void operator = (IMR_3DPoint &P);
      inline void operator = (IMR_UVIInfo &U);
      inline void operator = (IMR_PrjPoint &P);
    };

// Projects the point:
inline void IMR_PrjPoint::Project(float Zoom, int XC, int YC)
{
if (Z == 0.0f) Z = 0.0000001f;
pW = 1 / Z;
float Mult = pW * Zoom;
pX = XC + (X * Mult);
pY = YC - (Y * Mult);
pZ = Z;
 }

// Overloaded assignment operator:
inline void IMR_PrjPoint::operator = (IMR_3DPoint &P)
{
X = P.cX;
Y = P.cY;
Z = P.cZ;
 }

// Overloaded assignment operator:
inline void IMR_PrjPoint::operator = (IMR_UVIInfo &U)
{
pU = U.U;
pV = U.V;
pR = int(U.R * 255.0f);
pG = int(U.G * 255.0f);
pB = int(U.B * 255.0f);
 }

// Overloaded assignment operator:
inline void IMR_PrjPoint::operator = (IMR_PrjPoint &P)
{
X = P.X;
Y = P.Y;
Z = P.Z;
pX = P.pX;
pY = P.pY;
pZ = P.pZ;
pU = P.pU;
pV = P.pV;
pR = P.pR;
pG = P.pG;
pB = P.pB;
 }

#endif

