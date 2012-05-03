/***************************************************************************\
  File: IMR_Renderer.hpp
  Description: Header
  Author: Daniel Hawthorn
  Modified:
  
\***************************************************************************/
#ifndef __IMR_RENDERER__HPP
#define __IMR_RENDERER__HPP

// Include headers:
#include <string.h>
#include <malloc.h>
#include <math.h>
#include <stdlib.h>
#include "IMR_Table.hpp"
#include "IMR_FixedPnt.hpp"
#include "IMR_Err.hpp"
#include "IMR_RetVals.hpp"
#include "IMR_Geometry.hpp"
#include "IMR_Camera.hpp"
#include "IMR_RendBuffer.hpp"
#include "IMR_Global.hpp"

// Macros:
#define IMR_RENDERER_MODE_INIT          0
#define IMR_RENDERER_MODE_WIREFRAME     1
#define IMR_RENDERER_MODE_FILLED        2
#define IMR_RENDERER_MODE_TEXTURED      3
#define IMR_RENDERER_MODE_TEXTUREDLIT   4

// Renderer class:
class IMR_Renderer
    {
    protected:
      // Flags:
      struct 
          {
          unsigned int ClassInitialized:1;
          unsigned int InRasterBatch:1;
          int DrawMode;
           } Flags;

      // Debug stuff:
      int PolysDrawn;
      
      // Renderer stuff:
      IMR_Camera *Camera;
      IMR_RendBuffer *Buffer;
      char *Data;
      
    public:
      IMR_Renderer()
          {
          Flags.ClassInitialized = 0;
          Flags.InRasterBatch = 0;
          Flags.DrawMode = IMR_RENDERER_MODE_INIT;
          Camera = NULL;
          Buffer = NULL;
           };
      ~IMR_Renderer() { Shutdown(); };
      
      // Init and shutdown methods:
      int Init(void);
      int Shutdown(void);
      
      // Raster batch methods:
      int Begin_Raster_Batch(IMR_Camera &Cam, IMR_RendBuffer &Buff);
      int Draw_Wireframe_Polygon(IMR_Polygon &Poly);
      int Draw_Filled_Polygon(IMR_Polygon &Poly);
      int Draw_Textured_Polygon(IMR_Polygon &Poly);
      int Draw_TexturedLit_Polygon(IMR_Polygon &Poly);
      int End_Raster_Batch(void);
      
      // Debug methods:
      int Get_Polys_Drawn(void) { return PolysDrawn; };
     };

#endif
