/***************************************************************************\
  File: IMR_Pipeling.hpp
  Description: Header.  Also includes headers for other pipeline modules.
  Author: Daniel Hawthorn
  Modified:
  
\***************************************************************************/
#ifndef __IMR_PIPELINE__HPP
#define __IMR_PIPELINE__HPP

#include <stdlib.h>
#include <windows.h>
#include "IMR_Renderer.hpp"
#include "IMR_Err.hpp"
#include "IMR_Geometry.hpp"
#include "IMR_Matrix.hpp"
#include "IMR_Camera.hpp"
#include "IMR_List.hpp"
#include "IMR_RendBuffer.hpp"

#define IMR_PIPE_MAX_LIGHTS 64

// Thread function:
DWORD WINAPI IMR_Pipeline_Async_DrawFrame(LPVOID); // Accepts pointer to pipeline interface

// Pipeline class:
class IMR_Pipeline
    {
    // Get all buddy-buddy with our thread:
    friend DWORD WINAPI IMR_Pipeline_Async_DrawFrame(LPVOID);
    
    protected:
      // Counters:
      int Num_Vertices, Max_Vertices,
           Num_Polygons, Max_Polygons,
           Num_Lights, Max_Lights;
      int PolysCulled;
      
      // Interfaces used for the current frame:
      IMR_Camera *CurrCamera;
      IMR_RendBuffer *CurrBuffer;
      IMR_Renderer *AsyncRenderer;

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
    
    public:
      IMR_Pipeline() 
          {
          CurrCamera = NULL;
          CurrBuffer = NULL;
          AsyncRenderer = NULL;
          Num_Vertices = Num_Polygons = Num_Lights = 0; 
          Max_Vertices = Max_Polygons = Max_Lights = 0;
          Vertices = NULL;
          Polygons = NULL;
          Flags.IsDrawing = 0;
          Flags.ShouldQuit = 0;
           };
      
      // Initialization and shutdown methods:
      int Init(int MaxVerts, int MaxPolys, int MaxLights);
      void Reset(void);
      
      // Pipeline methods:
      int SetupFrame(IMR_Camera &Cam, IMR_RendBuffer &Buff);
      int Add_Geometries(IMR_Object &Obj);
      int Add_Model(IMR_Model &Mdl, IMR_3DPoint &Pos, IMR_Attitude &Rot);
      int Add_Model(IMR_Model &Mdl, IMR_3DPoint &Pos, IMR_Matrix &Transform);
      int Add_Object(IMR_Object &Obj);
      int Illuminate(void);
      int Transform(void);
      int Cull(void);
      int ClipAndProject(void);
      int DrawFrame(IMR_Renderer &Rnd);
      
      // Asynchroneous draw methods:
      int Async_IsDrawing(void) { return Flags.IsDrawing; };
      void Async_SetRenderer(IMR_Renderer &Rnd) { AsyncRenderer = &Rnd; };
            
      // Debug methods:
      #ifdef IMR_DEBUG
          int Get_Geom_Polys(void) { return Num_Polygons; };
          int Get_Polys_Culled(void) { return PolysCulled; };
          IMR_3DPoint *Get_Vertices(int *Amt) { if (Amt) *Amt = Num_Vertices; return Vertices; };
          IMR_Polygon *Get_Polygons(int *Amt) { if (Amt) *Amt = Num_Polygons; return Polygons; };
      #endif
     };

#endif

