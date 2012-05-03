/****************************************************************\
 
 iMMERSE Engine
 (C) 1999 No Tears Shed Software
 All rights reserved
 
 Filename: IMR_Geom_Poly.hpp
 Description: Header
 
\****************************************************************/
#ifndef __IMR_GEOM_POLY__HPP
#define __IMR_GEOM_POLY__HPP

#include <math.h>
#include <string.h>
#include "IMR_Geom_Prim.hpp"
#include "IMR_Material.hpp"

// Defines:
#define IMR_MAXPOLYVERTS 6
#define SQRT_ONEHALF 0.707106f

// Polygon:
class IMR_Polygon
    {
    public:
      int Num_Verts,                          // Number of vertices in polygon before projection
          Num_Verts_Proj;                      // Number of vertices in polygon after projection
      
      int Vtx_Index[IMR_MAXPOLYVERTS];        // Indices into a vertex list
      IMR_3DPoint *Vtx_List[IMR_MAXPOLYVERTS]; // Pointers to the vertices
      IMR_UVIInfo UVI_Info[IMR_MAXPOLYVERTS];  // Uvi info for each vertex
      IMR_PrjPoint Vtx_Projected[IMR_MAXPOLYVERTS]; // Projected vertices
      
      int Normal_Index;                       // Index for normal vertex
      IMR_3DPoint *Normal;                     // Pointer to normal vertex
      
      IMR_Material Material;                  // Material for poly

      struct 
          {
          unsigned int Culled:1;       // Flags if poly has been culled
          unsigned int Visible:1;      // Flags if poly is visible
          unsigned int TwoSided:1;     // Flags if poly is two-sided 
          unsigned int Pegged:1;       // Flags if poly is pegged
          unsigned int LightSource:1;  // Flags if poly is a light source (rendered at max. inten.)
          unsigned int Transparent:1;  // Flags if poly is transparent
          unsigned int MaxZ:1;         // Flags if poly should be rendered with max Z (i.e. skybox)
          unsigned int MinZ:1;         // Flags if poly should be rendered with min Z (i.e. overlay)
          unsigned int Skybox:1;       // Flags if poly shouldn't be translated (but will be transformed)
           } Flags;
      
      // Collision detection stuff:
      float Radius, RadiusSquared;     // The radius of the poly
      IMR_3DPoint Centroid;

      IMR_Polygon()
          {
          Vtx_Index[0] = Vtx_Index[1] = Vtx_Index[2] = Vtx_Index[3] = 0;
          Num_Verts = 0; Num_Verts_Proj = 0;
          Flags.Culled = 0;
          Flags.Visible = 1;
          Flags.TwoSided = 0;
          Flags.Transparent = 0;
          Flags.LightSource = 0;
           };
      ~IMR_Polygon() { Material.Shutdown(); };
      inline void operator = (IMR_Polygon &P);

      // Initialization methods:
      void Find_Centroid(void);
      void Find_Normal(void);
      void Find_Radius(void);
      
      // Flag access methods:
      inline void Set_TwoSided(int Val) { Flags.TwoSided = Val ? 1 : 0; };
      inline void Set_Pegged(int Val) { Flags.Pegged = Val ? 1 : 0; };
      inline void Set_LightSource(int Val) { Flags.LightSource = Val ? 1 : 0; };
      inline void Set_Transparent(int Val) { Flags.Transparent = Val ? 1 : 0; };
      inline int Get_TwoSided(void) { return Flags.TwoSided; };
      inline int Get_Pegged(void) { return Flags.Pegged; };
      inline int Get_LightSource(void) { return Flags.LightSource; };
      inline int Get_Transparent(void) { return Flags.Transparent; };
     };

inline void IMR_Polygon::operator = (IMR_Polygon &P)
{
int vtx;
Num_Verts = P.Num_Verts;
Num_Verts_Proj = P.Num_Verts_Proj;
for (vtx = 0; vtx < 6; vtx ++)
    {
    Vtx_Index[vtx] = P.Vtx_Index[vtx];
    UVI_Info[vtx] = P.UVI_Info[vtx];
     }
Normal_Index = P.Normal_Index;
Material = P.Material;
Flags.TwoSided = P.Flags.TwoSided;
Flags.Pegged = P.Flags.Pegged;
Flags.Transparent = P.Flags.Transparent;
Flags.LightSource = P.Flags.LightSource;
Flags.MaxZ = P.Flags.MaxZ;
Flags.MinZ = P.Flags.MinZ;
Flags.Skybox = P.Flags.Skybox;
Radius = P.Radius;
RadiusSquared = P.RadiusSquared;
 }    

#endif

