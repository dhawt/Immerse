/****************************************************************\
 
 iMMERSE Engine
 (C) 1999 No Tears Shed Software
 All rights reserved
 
 Filename: IMR_Geom_Light.hpp
 Description: Header
 
\****************************************************************/
#ifndef __IMR_GEOM_LIGHT__HPP
#define __IMR_GEOM_LIGHT__HPP

// Include headers:
#include "IMR_Geom_Prim.hpp"
#include "IMR_Geom_Poly.hpp"
#include "IMR_Matrix.hpp"
#include "IMR_Collide.hpp"

// Types, constants, and macros:
#define IMR_LIGHT_AMBIENT     1
#define IMR_LIGHT_CELESTIAL   2
#define IMR_LIGHT_POINT       3
#define IMR_LIGHT_SPOT        4    // Currently unsupported

// Light class:
class IMR_Light
    {
    protected:
      int ID, Type;
      float ColorR, ColorG, ColorB;     // R, G, and B color (0.0 - 1.0)
      
      IMR_3DPoint Position,             // Rlative pos (in point and spot lights)
                  WorldPos,             // World pos
                  CameraPos,            // Camera pos (for lens flare)
                  Direction,            // Only used in spot and celestial lights
                  WorldDirection;       // Global direction (for spots)
      IMR_PrjPoint Pos_Projected;       // Projected position (for lens flare)
      float Range;
      float RangeSquared;
      int Umbra, Penumbra;              // Spotlight umbra and penumbra

    public:
      IMR_Light() { Type = IMR_LIGHT_AMBIENT; };
      
      // Id methods:
      inline void Set_ID(int id) { ID = id; };
      inline int Get_ID(void) { return ID; };
      inline int Is(int id) { return id == ID; };
      
      // Type methods:
      void Set_Type(int T) { Type = T; };
      int Get_Type(void) { return Type; };
      
      // Range and color methods:
      void Set_Color(float R, float G, float B) { ColorR = R; ColorG = G; ColorB = B; };
      void Set_Range(float R) { Range = R;  RangeSquared = R * R; };
      float Get_Range(void) { return Range; };
      
      // Spotlight methods:
      void Set_Umbra(int Ang) { Ang %= IMR_DEGREECOUNT; Umbra = Ang / 2; };
      void Set_Penumbra(int Ang) { Ang %= IMR_DEGREECOUNT; Penumbra = Ang / 2; };
      
      // Position and direction methods:
      IMR_3DPoint &Get_Position(void) { return Position; };
      void Set_Position(IMR_3DPoint &Pos) { Position = Pos; };
      void Set_Direction(int AngX, int AngY);
      void Set_Direction(IMR_3DPoint &Target);
      IMR_3DPoint &Get_Direction(void) { return Direction; };
      IMR_3DPoint &Get_WorldPos(void) { return WorldPos; };
      IMR_3DPoint &Get_WorldDirection(void) { return WorldDirection; };
      
      // Miscellaneous methods:
      void IlluminatePolyList(IMR_Polygon *PList, int Num_Polys);
      inline void operator = (IMR_Light &L);
      
     };

inline void IMR_Light::operator = (IMR_Light &L)
{
ID = L.ID;
Type = L.Type;
Position = L.Position;
Direction = L.Direction;
WorldPos = L.WorldPos;
CameraPos = L.CameraPos;
WorldDirection = L.WorldDirection;
Range = L.Range;
RangeSquared = L.RangeSquared;
ColorR = L.ColorR;
ColorG = L.ColorG;
ColorB = L.ColorB;
Umbra = L.Umbra;
Penumbra = L.Penumbra;
 }

#endif

