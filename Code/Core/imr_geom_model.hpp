/****************************************************************\
 
 iMMERSE Engine
 (C) 1999 No Tears Shed Software
 All rights reserved
 
 Filename: IMR_Geom_Model.hpp
 Description: Header
 
\****************************************************************/
#ifndef __IMR_GEOM_MODEL__HPP
#define __IMR_GEOM_MODEL__HPP

// Include stuff:
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include "IMR_Geom_Poly.hpp"
#include "IMR_Geom_Prim.hpp"
#include "IMR_TexRef.hpp"
#include "IMR_Table.hpp"
#include "..\Foundation\IMR_List.hpp"
#include "..\CallStatus\IMR_Log.hpp"
#include "..\CallStatus\IMR_RetVals.hpp"

// Model class:
class IMR_Model
    {
    protected:
      char Name[9];
      int Morph_Status,
           Morph_Progress,
           Morph_Length;
    public:
      int Num_Vertices,
           Num_Polygons;
      IMR_3DPoint *Vertices;
      IMR_Polygon *Polygons;
      IMR_Model() 
          {
          Name[8] = 0;
          Num_Vertices = Num_Polygons = 0;
          Vertices = (IMR_3DPoint *)NULL;
          Polygons = (IMR_Polygon *)NULL;
           };
      ~IMR_Model() { Reset(); };
      
      // Init and de-init methods:
      int Init(int NumV, int NumP);
      void Reset(void);
      
      // Name methods:
      void Set_Name(char *NewName);
      inline char *Get_Name(void) { return Name; };
      inline int Is(char *CmpName) { return !stricmp(Name, CmpName); };
      
      // Setup methods:
      int Setup(void);
      
      // Shape generation methods:
      int Shift_Pos(float X, float Y, float Z);
      int Make_Cube(float Radius, char *Name);
      int Make_Rectangle(float Width, float Height, float Depth, char *Name);
      int Make_Pyramid(float Width, float Height, float Depth, char *Name);
      int Make_Wall(float Width, float Height, float PanelSize, char *Name);
      int Make_Cylinder(float Height, float Radius, char *Name);
      
      // Paint methods:
      int Paint(char *Name);
      int Paint_Unpegged(char *Name, float Width, float Height);
      int Set_PolyFlag_LightSource(int State);

      // CSG:
      int CombineModel(IMR_Model *Mdl, IMR_3DPoint Pos, IMR_Attitude Atd);
      
      // Skybox:
      int Make_Skybox(char *Name);
      int Paint_Skybox(char *FrName, char *RName, char *BkName, char *LName, char *TName, char *BName);
       };

#endif

