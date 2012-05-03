/***************************************************************************\
  File: IMR_Interface.hpp
  Description: Main iMMERSE interface class.
  Author: Daniel Hawthorn
  Modified:
  
\***************************************************************************/
#ifndef __IMR_INTERFACE__HPP
#define __IMR_INTERFACE__HPP

// Include all the headers:
#include <windows.h>
#include "IMR_DirectX.hpp"
#include "IMR_List.hpp"
#include "IMR_Table.hpp"
#include "IMR_Geometry.hpp"
#include "IMR_Camera.hpp"
#include "IMR_Palette.hpp"
#include "IMR_Texture.hpp"
#include "IMR_Pipeline.hpp"
#include "IMR_RendBuffer.hpp"
#include "IMR_Global.hpp"

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
     };

// Interface class:
class IMR_Interface
    {
    protected:
      // Texture cache:
      IMR_NamedList<IMR_Texture>  GlobalTextures, LocalTextures;
      
      // Interfaces:
      IMR_RendBuffer              RendBuffer;           // The buffer we render everything to
      IMR_Renderer                RendCore;             // The core renderer interface
      IMR_Pipeline                Pipeline;             // The pipeline interface
      
      // DirectX interface:
      IMR_DirectXInterface        DirectX;
      
      // Flags:
      struct
          {
          unsigned int ClassInitialized:1;
          unsigned int ScreenInitialized:1;
          unsigned int TargetInitialized:1;
          unsigned int DrawAsynchronous:1;
          unsigned int InFrame:1;
           } Flags;
      
      // Handle for draw thread:
      HANDLE DrawThreadHandle;
      unsigned long DrawThreadID;
      
      // Settings:
      IMR_Settings                GlobalSettings;
      
    public:
      IMR_Interface() 
          {
          Flags.ClassInitialized = 0;
          Flags.ScreenInitialized = 0;
          Flags.TargetInitialized = 0;
          Flags.DrawAsynchronous = 0;
          Flags.InFrame = 0;
          GlobalTextures.Init(IMR_MAX_GLBTEX);
          LocalTextures.Init(IMR_MAX_LOCTEX);
          DrawThreadHandle = NULL;
           };
      ~IMR_Interface() { Shutdown(); };
      
      // Init and de-init methods:
      int Init(IMR_InterfaceInitData &InitData);
      int Shutdown(void);
      
      // DirectX access methods:
      inline IMR_DirectXInterface *Get_DirectX(void) { return &DirectX; };
      
      // Texture cache access methods:
      int Add_Texture(int List, char *Name);
      IMR_Texture *Get_Texture(char *Name);
      void Clear_Global_Textures(void) { GlobalTextures.Reset(); GlobalTextures.Init(IMR_MAX_GLBTEX); };
      void Clear_Local_Textures(void) { LocalTextures.Reset(); LocalTextures.Init(IMR_MAX_LOCTEX); };
      
      // Frame rendering and external draw methods:
      int Async_IsDrawing(void) { return Pipeline.Async_IsDrawing(); };
      int Begin_Frame(IMR_Camera &Cam);
      int Add_Model(IMR_Model &Mod, IMR_3DPoint &Pos, IMR_Attitude &Atd);
      int Add_Object(IMR_Object &Obj);
      int Draw_Frame(void);
      int End_Frame(void);
      int Blit_Frame(void);
      void *Start_External_Draw(void) { RendBuffer.LockBack(); return (void *)RendBuffer.Get_BackData(); };
      void End_External_Draw(void) { RendBuffer.UnlockBack(); };
      int Flip_Buffers(void);

      // Settings methods:
      void Set_AsyncEnable(int val) { Flags.DrawAsynchronous = val ? 1:0; };
      int Set_Screen(int Width, int Height, HWND hWnd);
      void Get_Screen(int *Width, int *Height, HWND *hWnd)
          {
          if (Width) *Width = GlobalSettings.Screen.Width;
          if (Height) *Height = GlobalSettings.Screen.Height;
          if (hWnd) *hWnd = GlobalSettings.Screen.hWnd;
           }
      int Set_RenderTarget(int Width, int Height);
      void Get_RenderTarget(int *Width, int *Height)
          {
          if (Width) *Width = GlobalSettings.Target.Width;
          if (Height) *Height = GlobalSettings.Target.Height;
           }
      int Set_Lens(int Zm, int Nr, int Fr);
      int Get_Zoom(void) { return RendBuffer.Get_Zoom(); };
      int Get_Near(void) { return RendBuffer.Get_Near(); };
      int Get_Far(void) { return RendBuffer.Get_Far(); };
      
      // Settings methods:
      void Set_LightingEnable(int I) { GlobalSettings.Effects.LightingEnabled = I ? IMR_ON : IMR_OFF; };
      int Get_LightingEnable(void) { return GlobalSettings.Effects.LightingEnabled; };
      void Set_ShadowEnable(int I) { GlobalSettings.Effects.ShadowsEnabled = I ? IMR_ON : IMR_OFF; };
      int Get_ShadowEnable(void) { return GlobalSettings.Effects.ShadowsEnabled; };
      void Set_FilterEnable(int I) { GlobalSettings.Renderer.FilteringEnabled = I ? IMR_ON : IMR_OFF; };
      int Get_FilterEnable(void) { return GlobalSettings.Renderer.FilteringEnabled; };
      void Set_MipMapEnable(int I) { GlobalSettings.Renderer.MipMappingEnabled = I ? IMR_ON : IMR_OFF; };
      int Get_MipMapEnable(void) { return GlobalSettings.Renderer.MipMappingEnabled; };
      void Set_FilledPolysEnable(int I) { GlobalSettings.Renderer.FilledPolysEnabled = I ? IMR_ON : IMR_OFF; };
      int Get_FilledPolysEnable(void) { return GlobalSettings.Renderer.FilledPolysEnabled; };
      void Set_TexturesEnable(int I) { GlobalSettings.Renderer.TexturesEnabled = I ? IMR_ON : IMR_OFF; };
      int Get_TexturesEnable(void) { return GlobalSettings.Renderer.TexturesEnabled; };

      // Debug methods (IMR_DEBUG must be defined):
      #ifdef IMR_DEBUG
          IMR_RendBuffer *Get_RendBuffer_Interface(void) { return &RendBuffer; };
          IMR_Pipeline *Get_Pipeline_Interface(void) { return &Pipeline; };
          IMR_Renderer *Get_Renderer_Interface(void) { return &RendCore; };
          int Get_Geom_Polys(void) { return Pipeline.Get_Geom_Polys(); };
          int Get_Polys_Drawn(void) { return RendCore.Get_Polys_Drawn(); };
          int Get_Polys_Culled(void) { return Pipeline.Get_Polys_Culled(); };
      #endif
     };

#endif

