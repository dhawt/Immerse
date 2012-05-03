/****************************************************************\
 
 iMMERSE Engine
 (C) 1999 No Tears Shed Software
 All rights reserved
 
 Filename: IMR_DirectX.hpp
 Description: Header for DirectX encapsulation
 
\****************************************************************/
#ifndef __IMR_DIRECTX__HPP
#define __IMR_DIRECTX__HPP

// Include headers:
#include <string.h>
#include <malloc.h>
#include <math.h>
#include <stdlib.h>
#include <windows.h>
#include <ddraw.h>
#include <d3d.h>
#include "IMR_DirectX_Err.hpp"
#include "..\..\CallStatus\IMR_Log.hpp"

// DirectX interface init structure:
typedef struct
    {
    HWND AppWnd;
    struct
        {
        int Width,
            Height,
            BPP;
         } Display;
     } IMR_DirectXInterface_InitData;

// Windows graphics class:
class IMR_DirectXInterface
    {
    protected:
      
      // DirectDraw COM interfaces:
      LPDIRECTDRAW4        DirectDraw;
      LPDIRECTDRAW         DirectDraw1;
      
      // Direct3D COM interfaces:
      LPDIRECT3D3          Direct3D;
      LPDIRECT3DDEVICE3    Direct3DDevice;
      LPDIRECT3DVIEWPORT3  Direct3DViewport;
      
      // Surface COM interfaces:
      LPDIRECTDRAWSURFACE4  DDPrimarySurface;
      LPDIRECTDRAWSURFACE4  DDBackSurface;
      LPDIRECTDRAWSURFACE4  DDZBuffer;
      
      // Windows info:
      HWND AppWnd;

      // Flags:
      struct
          {
          unsigned int DirectDrawActive:1;
          unsigned int Direct3DActive:1;
           } Flags;

    public:
      IMR_DirectXInterface()
          {
          DirectDraw = NULL;
          DDPrimarySurface = NULL;
          DDBackSurface = NULL;
          Direct3D = NULL;
          Direct3DDevice = NULL;
          Flags.DirectDrawActive = 0;
          Flags.Direct3DActive = 0;
           };
      ~IMR_DirectXInterface() { Shutdown(); };

      // Init and shutdown methods:
      int Init(IMR_DirectXInterface_InitData &InitData);
      void Shutdown(void);
      
      // Ummm... sorry...
      LPDIRECTDRAWSURFACE4 Get_BackSurface(void) const { return DDBackSurface; };
      
      // Surface methods:
      void Restore(LPDIRECTDRAWSURFACE4 Surface);
      void Destroy_Surface(LPDIRECTDRAWSURFACE4 Surface) const { Surface->Release(); };
      LPDIRECTDRAWSURFACE4 Create_Surface(int Width, int Height, int BPP);
      LPDIRECTDRAWSURFACE4 Create_8BitTexture(int Width, int Height, void *Palette);
      LPDIRECTDRAWSURFACE4 Create_16BitTexture(int Width, int Height);
      LPDIRECTDRAWSURFACE4 Create_24BitTexture(int Width, int Height);
      LPDIRECTDRAWSURFACE4 Create_3DSurface(int Width, int Height, int BPP);
      LPDIRECTDRAWSURFACE4 Create_Lightmap(int Width, int Height);
      void *Lock_Surface(LPDIRECTDRAWSURFACE4 Surface);  // NULL for primary
      void Unlock_Surface(LPDIRECTDRAWSURFACE4 Surface); // NULL for primary
      int Set_Surface_Palette(LPDIRECTDRAWSURFACE4 Surface, void *Palette);
      void Clear_Surface(LPDIRECTDRAWSURFACE4 Surface);
      
      // Blit methods:
      int FlipBackBuffer(void);
      int BlitToPrimary(LPDIRECTDRAWSURFACE4 Source);
      int BlitToBack(LPDIRECTDRAWSURFACE4 Source);
      int Blit(LPDIRECTDRAWSURFACE4 Source, LPDIRECTDRAWSURFACE4 Target);

      // Direct3D methods:
      int InitDirect3D(LPDIRECTDRAWSURFACE4 Target);
      void Prepare_Frame(LPDIRECTDRAWSURFACE4 Target);
      
      // Access methods:
      inline LPDIRECT3DDEVICE3 Get_DeviceInterface(void) const { return Direct3DDevice; };
      int Get_PrimarySurfacePitch(void);

      // Callbacks:
      static HRESULT WINAPI ZBufferCallback(DDPIXELFORMAT *Format, void *Found);
      static HRESULT WINAPI TextureFormatCallback(DDPIXELFORMAT *Format, void *Found);
      static HRESULT CALLBACK DeviceCallback(GUID *DeviceGuid, char *DeviceDescription, char *DeviceName, LPD3DDEVICEDESC HWDeviceDesc, LPD3DDEVICEDESC SWDeviceDesc, void *UserParm);
            
     };

#endif
