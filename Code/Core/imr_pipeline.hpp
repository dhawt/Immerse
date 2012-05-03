/****************************************************************\
 
 iMMERSE Engine
 (C) 1999 No Tears Shed Software
 All rights reserved
 
 Filename: IMR_Pipeline.hpp
 Description: Header
 
\****************************************************************/
#ifndef __IMR_PIPELINE__HPP
#define __IMR_PIPELINE__HPP

#include <stdlib.h>
#include <windows.h>
#include "IMR_Geometry.hpp"
#include "IMR_Matrix.hpp"
#include "IMR_Camera.hpp"
#include "..\RendCore\DirectX6\IMR_Renderer.hpp"
#include "..\CallStatus\IMR_Log.hpp"
#include "..\Foundation\IMR_List.hpp"

#define IMR_PIPE_MAX_LIGHTS 64

// Pipeline class:
class IMR_Pipeline
    {
    protected:
      // Counters:
      int Num_Vertices, Max_Vertices,
           Num_Polygons, Max_Polygons,
           Num_Lights, Max_Lights;
      int PolysCulled;
      
      // Interfaces used for the current frame:
      IMR_Camera *CurrCamera;
      IMR_Renderer *CurrRenderer;

      // Flags:
      struct 
          {
          unsigned int IsDrawing:1;
          unsigned int ShouldQuit:1;
           } Flags;
      
      // Lists:
      IMR_3DPoint                *Vertices;
      IMR_Polygon                *Polygons;
      IMR_Light                  *Lights[IMR_PIPE_MAX_LIGHTS];
    
      // Temporary storage:
      IMR_Polygon **DrawPolyList;
    
    public:
      IMR_Pipeline() 
          {
          CurrCamera = NULL;
          CurrRenderer = NULL;
          DrawPolyList = NULL;
          Num_Vertices = Num_Polygons = Num_Lights = 0; 
          Max_Vertices = Max_Polygons = Max_Lights = 0;
          Vertices = NULL;
          Polygons = NULL;
          Flags.IsDrawing = 0;
          Flags.ShouldQuit = 0;
           };
      
      // Our asynchronous draw thread:
      static DWORD WINAPI Async_DrawFrame(LPVOID);

      // Initialization and shutdown methods:
      int Init(int MaxVerts, int MaxPolys, int MaxLights);
      void Reset(void);
      
      // Pipeline methods:
      int SetupFrame(IMR_Camera &, IMR_Renderer &);
      int Add_Geometries(IMR_Object &Obj);
      int Add_Model(IMR_Model &Mdl, IMR_3DPoint &Pos, IMR_Attitude &Rot);
      int Add_Model(IMR_Model &Mdl, IMR_3DPoint &Pos, IMR_Matrix &Transform);
      int Add_Object(IMR_Object &Obj);
      int Illuminate(void);
      int Transform(void);
      int Cull(void);
      int ClipAndProject(void);
      int DrawFrame(void);
      
      // Asynchroneous draw methods:
      int Async_IsDrawing(void) { return Flags.IsDrawing; };
            
      // Debug methods:
      int Get_Geom_Polys(void) { return Num_Polygons; };
      int Get_Polys_Culled(void) { return PolysCulled; };
      IMR_3DPoint *Get_Vertices(int *Amt) { if (Amt) *Amt = Num_Vertices; return Vertices; };
      IMR_Polygon *Get_Polygons(int *Amt) { if (Amt) *Amt = Num_Polygons; return Polygons; };
     };

#endif

