/****************************************************************\
 
 iMMERSE Engine
 (C) 1999 No Tears Shed Software
 All rights reserved
 
 Filename: IMR_RendCore.hpp
 Description: Header
 
\****************************************************************/
#ifndef __IMR_RENDCORE__HPP
#define __IMR_RENDCORE__HPP

// Include headers:
#include <string.h>
#include <malloc.h>
#include <math.h>
#include <stdlib.h>
#include "..\..\Core\IMR_Table.hpp"
#include "..\..\Core\IMR_Geometry.hpp"
#include "..\..\Core\IMR_Camera.hpp"
#include "..\..\Core\IMR_TexRef.hpp"
#include "..\..\Core\IMR_Resource.hpp"
#include "..\..\Foundation\IMR_List.hpp"
#include "..\..\CallStatus\IMR_Log.hpp"
#include "..\..\CallStatus\IMR_RetVals.hpp"
#include "IMR_Texture.hpp"
#include "IMR_DirectX.hpp"

// Macros:
#define IMR_RENDERER_MODE_INIT          0
#define IMR_RENDERER_MODE_TEXTURED      3
#define IMR_RENDERER_MODE_TEXTUREDLIT   4
#define IMR_RENDERER_BATCHMAXVERTS      8192

// Renderer class:
class IMR_Renderer
    {
    protected:

      // Interfaces:
      IMR_DirectXInterface DirectX;
      
      // Renderer target info:
      struct
          {
          // Target surface:
          LPDIRECTDRAWSURFACE4 Surface;
          char *Data, *BackData;
          int Length;
          
          // Target window info:
          struct 
              {
              int Width, Height,
                  MinX, MaxX,
                  MinY, MaxY,
                  XCenter, YCenter;
               } Window;
          
          // Target screen info:
          struct
              {
              int Width, 
                  Height,
                  BPP;
              HWND AppWindow;
                } Screen;

           } Target;

      // Camera info:
      IMR_Camera *Camera;   // Pointer to camera in use
      
      // Textures and lightmaps:
      IMR_NamedList<IMR_Texture> Textures; 
      IMR_IDList<IMR_Lightmap> Lightmaps;
      LPDIRECT3DTEXTURE2 LastTexture, CurrTexture;

      // Draw option flags:
      struct
          {
          unsigned int MipMappingEnabled:1;    // Flags if mipmapping should be performed
          unsigned int FilteringEnabled:1;     // Flags if mag- and minification is on
           } DrawOpts;

      // Flags:
      struct 
          {
          unsigned int InRasterBatch:1;
          unsigned int BufferLocked:1;
          unsigned int BackLocked:1; 
          unsigned int ScreenInitialized:1;
          unsigned int WindowInitialized:1;
          int DrawMode;
           } Flags;

      // Debug stuff:
      int PolysDrawn;

      // Protected methods:
      inline LPDIRECT3DDEVICE3 Get_DeviceInterface(void) { return DirectX.Get_DeviceInterface(); };

    public:
      IMR_Renderer()
          {
          LastTexture = CurrTexture = NULL;
          Flags.InRasterBatch = 0;
          Flags.BufferLocked = 0;
          Flags.BackLocked = 0;
          Flags.DrawMode = IMR_RENDERER_MODE_INIT;
          DrawOpts.MipMappingEnabled = 1;
          DrawOpts.FilteringEnabled = 0;
          Camera = NULL;
          Target.Surface = NULL;
          Target.Data = Target.BackData = NULL;
          Target.Length = 0;
          Target.Window.Width = Target.Window.Height = 0;
          Target.Window.MinX = Target.Window.MaxX = Target.Window.MinY = Target.Window.MaxY = 0;
          Target.Window.XCenter = Target.Window.YCenter = 0;
           };
      ~IMR_Renderer() { Shutdown(); };
      
      // Init and shutdown methods:
      int Init(int MaxTextures, int MaxLightmaps);
      int Shutdown(void);

      // Target access methods:
      int Target_Lock(void);
      int Target_LockBack(void);
      void *Target_GetBackData(void) { return Target.BackData; };
      void Target_Unlock(void);
      void Target_UnlockBack(void);
      void Target_ClearBuffers(void) { if (Target.Surface) DirectX.Prepare_Frame(Target.Surface); };
      int Target_Blit(void);
      int Target_Flip(void);

      // Screen and window methods:
      int Set_Screen(int W, int H, HWND Wnd);
      int Set_Window(int x0, int y0, int x1, int y1);
      inline int Get_WindowXCenter(void) { return Target.Window.XCenter; };
      inline int Get_WindowYCenter(void) { return Target.Window.YCenter; };
      inline int Get_WindowWidth(void) { return Target.Window.Width; };
      inline int Get_WindowHeight(void) { return Target.Window.Height; };
      inline int Get_ScreenPitch(void) { return DirectX.Get_PrimarySurfacePitch(); };
      
      // Draw options:
      void DrawOpts_MipMap(int Flag) { DrawOpts.MipMappingEnabled = Flag ? 1:0; };
      void DrawOpts_Filtering(int Flag) { DrawOpts.FilteringEnabled = Flag ? 1:0; };
      int DrawOpts_GetMipMap(void) { return DrawOpts.MipMappingEnabled; };
      int DrawOpts_GetFiltering(void) { return DrawOpts.MipMappingEnabled; };
      
      // Raster batch methods:
      int Begin_Raster_Batch(IMR_Camera &Cam);
      int Draw_Textured_Polygon(IMR_Polygon &Poly);
      int Draw_TexturedLit_Polygon(IMR_Polygon &Poly);
      int Draw_PolyBatch(IMR_Polygon **PolyList, int NumPolygons);
      int End_Raster_Batch(void);
      
      // Texture methods:
      int Texture_LoadFromPCX(char *Filename, char *TexName);
      int Texture_Load(char *TexName);
      int Texture_CreateDummy(char *TexName);
      IMR_TexRef Texture_Add(char *Name);
      int Texture_Gen(IMR_TexRef Ref);
      IMR_TexRef Texture_GetRef(char *Name);
      IMR_TexRef Texture_GetData(char *Name);
      int Texture_ReturnData(IMR_TexRef Ref);
       
      // Lightmap methods:
      IMR_TexRef Lightmap_Add(int ID);
      int Lightmap_Gen(IMR_TexRef ref);
      IMR_TexRef Lightmap_GetRef(int ID);
      IMR_TexRef Lightmap_GetData(int ID);
      int Lightmap_ReturnData(IMR_TexRef Ref);
      
      // Debug methods:
      int Get_Polys_Drawn(void) { return PolysDrawn; };
     };

#endif
