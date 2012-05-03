/****************************************************************\ 
 iMMERSE Engine
 (C) 1999 No Tears Shed Software
 All rights reserved
 
 Filename: IMR_Interface.hpp
 Description: Header
 
\****************************************************************/
#ifndef __IMR_INTERFACE__HPP
#define __IMR_INTERFACE__HPP

// Include all the headers:
#include <windows.h>
#include "IMR_Table.hpp"
#include "IMR_Geometry.hpp"
#include "IMR_Camera.hpp"
#include "IMR_Palette.hpp"
#include "IMR_TexRef.hpp"
#include "IMR_Pipeline.hpp"
#include "IMR_Resource.hpp"
#include "..\RendCore\DirectX6\IMR_Renderer.hpp"
#include "..\Foundation\IMR_List.hpp"
#include "..\Foundation\IMR_Time.hpp"
#include "..\CallStatus\IMR_Log.hpp"

// Constants and macros:
#define IMR_MAX_GLBTEX        64
#define IMR_MAX_LOCTEX        64
#define IMR_LIST_LOCAL        1
#define IMR_LIST_GLOBAL       2
#define IMR_ZOOM320200        200
#define IMR_ZOOM320240        200
#define IMR_ZOOM640480        400
#define IMR_STDNEAR           32
#define IMR_STDFAR            32000
#define IMR_CALCZOOM(xs, ys)  ((xs / ys) * xs / 2.13)

// Engine init structure (outdated but still important):
struct IMR_InterfaceInitData
    {
    struct
        {
        int MaxVerts,
            MaxPolys,
            MaxLights;
         } GeometryInfo;
    struct
        {
        HWND AppWnd;
         } OS_Win9x;
     };

// Interface class:
class IMR_Interface
    {
    protected:
      // Interfaces:
      IMR_Renderer                Renderer;             // Our renderer...
      IMR_Pipeline                Pipeline;             // The pipeline interface
      
      // Flags:
      struct
          {
          unsigned int ClassInitialized:1;
          unsigned int ScreenInitialized:1;
          unsigned int WindowInitialized:1;
          unsigned int DrawAsynchronous:1;
          unsigned int InFrame:1;
           } Flags;
      
      // Handle for draw thread:
      HANDLE DrawThreadHandle;
      unsigned long DrawThreadID;
      
    public:
      IMR_Interface() 
          {
          Flags.ClassInitialized = 0;
          Flags.ScreenInitialized = 0;
          Flags.WindowInitialized = 0;
          Flags.DrawAsynchronous = 0;
          Flags.InFrame = 0;
          DrawThreadHandle = NULL;
           };
      ~IMR_Interface() { Shutdown(); };
      
      // Init and de-init methods:
      int Init(IMR_InterfaceInitData &InitData);
      int Shutdown(void);
      
      // Resource manager:
      int LoadRIF(char *RIFName) { return IMR_Resources.LoadResources(RIFName); };

      // Frame rendering and external draw methods:
      int Async_IsDrawing(void) { return Pipeline.Async_IsDrawing(); };
      int Begin_Frame(IMR_Camera &Cam);
      int Add_Model(IMR_Model &Mod, IMR_3DPoint &Pos, IMR_Attitude &Atd);
      int Add_Object(IMR_Object &Obj);
      int Draw_Frame(void);
      int End_Frame(void);
      int Blit_Frame(void);
      void *Start_External_Draw(void) { Renderer.Target_LockBack(); return (void *)Renderer.Target_GetBackData(); };
      int Get_ScreenPitch(void) { return Renderer.Get_ScreenPitch(); };
      void End_External_Draw(void) { Renderer.Target_UnlockBack(); };
      int Flip_Buffers(void);

      // Settings methods:
      void Set_AsyncEnable(int val) { Flags.DrawAsynchronous = val ? 1:0; };
      int Set_Screen(int W, int H, HWND hWnd);
      int Set_Window(int x0, int y0, int x1, int y1);
      
      // Settings methods:
      void SetDrawOpt_MipMap(int Flag) { Renderer.DrawOpts_MipMap(Flag); };
      void SetDrawOpt_Filtering(int Flag) { Renderer.DrawOpts_Filtering(Flag); };
      int GetDrawOpt_MipMap(void) { return Renderer.DrawOpts_GetMipMap(); };
      int GetDrawOpt_Filtering(void) { return Renderer.DrawOpts_GetFiltering(); };

      // Texture methods:
      int Texture_LoadFromPCX(char *Filename, char *TexName) { return Renderer.Texture_LoadFromPCX(Filename, TexName); };
      int Texture_LoadFromRDF(char *Filename, char *TexName) { return Renderer.Texture_LoadFromPCX(Filename, TexName); };

      // Debug methods:
      IMR_Renderer *Get_Renderer_Interface(void) { return &Renderer; };
      int Get_Geom_Polys(void) { return Pipeline.Get_Geom_Polys(); };
      int Get_Polys_Drawn(void) { return Renderer.Get_Polys_Drawn(); };
      int Get_Polys_Culled(void) { return Pipeline.Get_Polys_Culled(); };
     };

#endif

