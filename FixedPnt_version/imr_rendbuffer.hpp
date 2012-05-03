/***************************************************************************\
  File: IMR_RendBuffer.hpp
  Description: Header
  Author: Daniel Hawthorn
  Modified:
  
\***************************************************************************/
#ifndef __IMR_RENDBUFFER__HPP
#define __IMR_RENDBUFFER__HPP

// Include headers:
#include "IMR_DirectX.hpp"
#include "IMR_FixedPnt.hpp"
#include "IMR_Err.hpp"
#include "IMR_Global.hpp"

// Renderer buffer class:
class IMR_RendBuffer
    {
    protected:
     
      // DirectX stuff:
      IMR_DirectXInterface *DirectX;
      LPDIRECTDRAWSURFACE4 BufferSurface;
      char *Data, *BackData;                     // Pointer to surface data (when locked)
      
      // Data members:
      int Width, Height,
           MinX, MaxX,
           MinY, MaxY,
           XCenter, YCenter,
           Length;

      // Screen members:
      int ScreenWidth, ScreenHeight;

      // Lens info:
      struct
          {
          FIXEDPNT Zoom,
                   Near,
                   Far;
           } Lens;
      
      // Flags:
      struct
          {
          unsigned int BufferLocked:1;
          unsigned int BackLocked:1; 
           } Flags;

    public:
      
      IMR_RendBuffer()
          { 
          Width = Height = 0;
          MinX = MaxX = MinY = MaxY = 0;
          XCenter = YCenter = 0;
          Length = 0;
          DirectX = NULL;
          BufferSurface = NULL;
          Data = BackData = NULL;
          Flags.BufferLocked = 0;
          Flags.BackLocked = 0;
           };
      ~IMR_RendBuffer() { Shutdown(); };      

      // Init and shutdown methods:
      int Set_Screen(int W, int H, HWND Wnd, IMR_DirectXInterface *DX);
      int Set_RenderTarget(int W, int H, IMR_DirectXInterface *DX);
      void Shutdown(void);
      
      // DirectX methods:
      LPDIRECT3DDEVICE3 Get_DeviceInterface(void)
          {
          if (DirectX) 
              return DirectX->Get_DeviceInterface(); 
          else 
              return NULL; 
           };
      
      // Lock methods:
      int Lock(void);
      void Unlock(void);
      int LockBack(void);
      void UnlockBack(void);
      
      // Display methods:
      void Clear_Buffers(void);
      int Blit(void);
      int Flip(void);
            
      // Data methods:
      char *Get_Data(void) { if (Flags.BufferLocked) return Data; else return NULL; };
      char *Get_BackData(void) { if (Flags.BackLocked) return BackData; else return NULL; };
      char operator [] (int k) { if (Flags.BufferLocked) return Data[k]; else return NULL; };

      // Size methods:
      int Get_Width(void) { return Width; };
      int Get_Height(void) { return Height; };
      int Get_XCenter(void) { return XCenter; };
      int Get_YCenter(void) { return YCenter; };
      int Get_MinX(void) { return MinX; };
      int Get_MaxX(void) { return MaxX; };
      int Get_MinY(void) { return MinY; };
      int Get_MaxY(void) { return MaxY; };
      int Get_ScreenWidth(void) { return ScreenWidth; };
      int Get_ScreenHeight(void) { return ScreenHeight; };

      // Lens methods:
      inline FIXEDPNT Get_Zoom(void) { return Lens.Zoom; };
      inline FIXEDPNT Get_Near(void) { return Lens.Near; };
      inline FIXEDPNT Get_Far(void) { return Lens.Far; };
      inline void Set_Zoom(int Zm) { Lens.Zoom = IntToFixed(Zm); };
      inline void Set_Near(int Nr) { Lens.Near = IntToFixed(Nr); };
      inline void Set_Far(int Fr) { Lens.Far = IntToFixed(Fr); };
      
     };

#endif
