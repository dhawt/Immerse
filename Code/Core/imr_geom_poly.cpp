/****************************************************************\
 
 iMMERSE Engine
 (C) 1999 No Tears Shed Software
 All rights reserved
 
 Filename: IMR_Geom_Poly.hpp
 Description: Polygon geometry module.
 
\****************************************************************/
#include "IMR_Geom_Poly.hpp"

/***************************************************************************\
  Calculates the centroid of the poly and stores it in the specified point.
  Uses the active coords, so make sure they are set correctly before use.
\***************************************************************************/
void IMR_Polygon::Find_Centroid(void)
{
if (Num_Verts == 4)
    {
    Centroid.X = (Vtx_List[0]->aX + Vtx_List[1]->aX + Vtx_List[2]->aX + Vtx_List[3]->aX) * 0.25;
    Centroid.Y = (Vtx_List[0]->aY + Vtx_List[1]->aY + Vtx_List[2]->aY + Vtx_List[3]->aY) * 0.25;
    Centroid.Z = (Vtx_List[0]->aZ + Vtx_List[1]->aZ + Vtx_List[2]->aZ + Vtx_List[3]->aZ) * 0.25;
     }
if (Num_Verts == 3)
    {
    Centroid.X = (Vtx_List[0]->aX + Vtx_List[1]->aX + Vtx_List[2]->aX) * 0.3333;
    Centroid.Y = (Vtx_List[0]->aY + Vtx_List[1]->aY + Vtx_List[2]->aY) * 0.3333;
    Centroid.Z = (Vtx_List[0]->aZ + Vtx_List[1]->aZ + Vtx_List[2]->aZ) * 0.3333;
     }
if (Num_Verts != 3 && Num_Verts != 4)
    {
    Centroid.X = Centroid.Y = Centroid.Z = 0;
    for (int vtx = 0; vtx < Num_Verts; vtx ++)
        {
        Centroid.X += Vtx_List[vtx]->aX;
        Centroid.Y += Vtx_List[vtx]->aY;
        Centroid.Z += Vtx_List[vtx]->aZ;
         }
    float iNV = 1 / float(Num_Verts);
    Centroid.X *= iNV;
    Centroid.Y *= iNV;
    Centroid.Z *= iNV;
     }
 }

/***************************************************************************\
  Computes the normal to the poly and stores it in its normal vertex.
\***************************************************************************/
void IMR_Polygon::Find_Normal(void)
{
float x0, y0, z0, x1, y1, z1, x2, y2, z2, nx, ny, nz, length;

// Make sure the pointers are ok:
if (!Vtx_List[0] || !Vtx_List[1] || !Vtx_List[2] || !Normal)
    return;

// Convert vertices to floating point:
x0 = Vtx_List[0]->lX;
y0 = Vtx_List[0]->lY;
z0 = Vtx_List[0]->lZ;
x1 = Vtx_List[1]->lX;
y1 = Vtx_List[1]->lY;
z1 = Vtx_List[1]->lZ;
x2 = Vtx_List[2]->lX;
y2 = Vtx_List[2]->lY;
z2 = Vtx_List[2]->lZ;

// Find the cross product to the two vectors:
nx = (y0 * (z1 - z2)) +
     (y1 * (z2 - z0)) +
     (y2 * (z0 - z1));
ny = (z0 * (x1 - x2)) +
     (z1 * (x2 - x0)) +
     (z2 * (x0 - x1));
nz = (x0 * (y1 - y2)) +
     (x1 * (y2 - y0)) +
     (x2 * (y0 - y1));

// Make the normal a unit vector:
length = 1 / sqrt((nx * nx) + (ny * ny) + (nz * nz));
nx *= length;
ny *= length;
nz *= length;

// Translate the normal relative to the polygon and setup the list:
Normal->lX = nx + x0;
Normal->lY = ny + y0;
Normal->lZ = nz + z0;

// And flag the normal vertex as a normal:
Normal->IsNormal = 1;
 }

/***************************************************************************\
  Computes the radius of the poly.
  Uses active coords.
\***************************************************************************/
void IMR_Polygon::Find_Radius(void)
{
IMR_3DPoint TempVtx[IMR_MAXPOLYVERTS];
float Distance[IMR_MAXPOLYVERTS], X, Y, Z;
int vtx;

// Copy vertices into temporary array:
for (vtx = 0; vtx < Num_Verts; vtx ++)
    TempVtx[vtx] = *Vtx_List[vtx];

// Find the centroid of the poly:
Find_Centroid();

// Translate polygon to its center:
for (vtx = 0; vtx < Num_Verts; vtx ++)
    TempVtx[vtx] -= Centroid;

// Find the distance to each vertex:
for (vtx = 0; vtx < Num_Verts; vtx ++)
    {
    X = TempVtx[vtx].aX;
    Y = TempVtx[vtx].aY;
    Z = TempVtx[vtx].aZ;
    Distance[vtx] = (X * X) + (Y * Y) + (Z * Z);
     }

// Find the maximum distance:
RadiusSquared = Distance[0];
for (vtx = 0; vtx < Num_Verts; vtx ++)
    if (Distance[vtx] > RadiusSquared) 
        {
        RadiusSquared = Distance[vtx];
        Radius = sqrt(Distance[vtx]);
         }
 }

