/****************************************************************\
 
 iMMERSE Engine
 (C) 1999 No Tears Shed Software
 All rights reserved
 
 Filename: IMR_DirectX.hpp
 Description: DirectX wrapper class code module
 
\****************************************************************/
#define INITGUID
#include <windows.h>
#include <ddraw.h>
#include <d3d.h>
#include "IMR_DirectX.HPP"

// Globals:
static struct
    {
    int Width, 
        Height, 
        BPP;
     } Display;
static struct
    {
    GUID Guid;
    char Description[32], 
         Name[32];
    D3DDEVICEDESC HWDeviceDesc, 
                  SWDeviceDesc;
     } Direct3DDeviceInfo;

/***************************************************************************\
  Renderer device enumeration callback function.
\***************************************************************************/
static HRESULT CALLBACK IMR_DirectXInterface::DeviceCallback(GUID *DeviceGuid, 
                                                             char *DeviceDescription, 
                                                             char *DeviceName,
                                                             LPD3DDEVICEDESC HWDeviceDesc,
                                                             LPD3DDEVICEDESC SWDeviceDesc,
                                                             void *UserParm)
{
BOOL IsHardware;
LPD3DDEVICEDESC DeviceDesc;

// Check if the device is hardware:
IsHardware = (HWDeviceDesc->dcmColorModel != 0); // Will be non-zero for hardware devices
DeviceDesc = (IsHardware ? HWDeviceDesc : SWDeviceDesc);

// Discard the device if it can't do our bit-depth:
if (Display.BPP == 8)
    if (!(DeviceDesc->dwDeviceRenderBitDepth & DDBD_8))
        return D3DENUMRET_OK;
if (Display.BPP == 16)
    if (!(DeviceDesc->dwDeviceRenderBitDepth & DDBD_16))
        return D3DENUMRET_OK;
if (Display.BPP == 24)
    if (!(DeviceDesc->dwDeviceRenderBitDepth & DDBD_24))
        return D3DENUMRET_OK;
if (Display.BPP == 32)
    if (!(DeviceDesc->dwDeviceRenderBitDepth & DDBD_32))
        return D3DENUMRET_OK;

// Make sure the device supports Gouraud-shaded triangles:
if (DeviceDesc->dcmColorModel == D3DCOLOR_MONO)
    {
    if (!(DeviceDesc->dpcTriCaps.dwShadeCaps & D3DPSHADECAPS_COLORGOURAUDMONO))
        return D3DENUMRET_OK;
     }
if (DeviceDesc->dcmColorModel == D3DCOLOR_RGB)
    {
    if (!(DeviceDesc->dpcTriCaps.dwShadeCaps & D3DPSHADECAPS_COLORGOURAUDRGB))
        return D3DENUMRET_OK;
     }

// If this is a software RGB and we have already found a device, skip this one:
if (!IsHardware && *(int *)UserParm && (DeviceDesc->dcmColorModel == D3DCOLOR_RGB))
    return D3DENUMRET_OK;

// This device'll work.  Keep it:
*(int *)UserParm = TRUE;
memcpy((void *)&Direct3DDeviceInfo.Guid, DeviceGuid, sizeof(GUID));
strcpy(Direct3DDeviceInfo.Description, DeviceDescription);
strcpy(Direct3DDeviceInfo.Name, DeviceName);
memcpy((void *)&Direct3DDeviceInfo.HWDeviceDesc, HWDeviceDesc, sizeof(D3DDEVICEDESC));
memcpy((void *)&Direct3DDeviceInfo.SWDeviceDesc, SWDeviceDesc, sizeof(D3DDEVICEDESC));

// We have what we need, so stop enumerating:
return D3DENUMRET_CANCEL;
 }                                                  

/***************************************************************************\
  Depth-buffer enumeration callback function.
\***************************************************************************/
static HRESULT WINAPI IMR_DirectXInterface::ZBufferCallback(DDPIXELFORMAT *Format, void *Found)
{
// Make sure this depth-buffer is a Z-buffer:
if (Format->dwFlags == DDPF_ZBUFFER)
    {
    // Only accept 16-bpp z-buffers:
    if (Format->dwZBufferBitDepth == 16)
        {
        // Copy the format:
        memcpy(Found, Format, sizeof(DDPIXELFORMAT)); 

        // Return with D3DENUMRET_CANCEL to end enumeration:
        return D3DENUMRET_CANCEL;
         } 
     }

// Continue enumerating:
return D3DENUMRET_OK;
 }

/***************************************************************************\
  Texture format enumeration callback.
\***************************************************************************/
static HRESULT WINAPI IMR_DirectXInterface::TextureFormatCallback(DDPIXELFORMAT *Format, void *Found)
{
// Check if the device can handle 8-bit palettized textures:
if (Format->dwFlags & DDPF_PALETTEINDEXED8)
    {
    // Flag that we found an 8-bit format:
    *(int *)Found |= 0x1;
    
    // If we have found a 24-bit format as well, return with D3DENUMRET_CANCEL 
    // to end enumeration:
    if (*(int *)Found & 0x2) return D3DENUMRET_CANCEL;
     }

// Check if the device can handle 24-bit textures:
if ((Format->dwFlags & DDPF_RGB) && (Format->dwRGBBitCount == 24))
    {
    // Flag that we found a 24-bit format:
    *(int *)Found |= 0x2;
    
    // If we have found an 8-bit format as well, return with D3DENUMRET_CANCEL 
    // to end enumeration:
    if (*(int *)Found & 0x1) return D3DENUMRET_CANCEL;
     }

// Continue enumerating:
return D3DENUMRET_OK;
 }

/***************************************************************************\
  Initializes the DirectX system.
  Returns IMR_OK if successful, otherwise an error.
\***************************************************************************/
int IMR_DirectXInterface::Init(IMR_DirectXInterface_InitData &InitData)
{
DDSURFACEDESC2 DDSurfaceDesc;
DDSCAPS        DDSCaps;
DDSCAPS2       DDSCaps2;
int err;

// Shutdown the interface:
Shutdown();

// Setup the display:
AppWnd = InitData.AppWnd;
Display.Width = InitData.Display.Width;
Display.Height = InitData.Display.Height;
Display.BPP = InitData.Display.BPP;

// Create the DirectDraw interface:
err = DirectDrawCreate(NULL, &DirectDraw1, NULL);
if (err != DD_OK)
    {
    IMR_LogMsg(__LINE__, __FILE__, "DirectX says: %s", IMR_MsgFromDXErr(err));
    Shutdown(); return IMRERR_DIRECTX;
     }
err = DirectDraw1->QueryInterface(IID_IDirectDraw4, (void **)&DirectDraw);
if (err != DD_OK)
    {
    IMR_LogMsg(__LINE__, __FILE__, "DirectX says: %s", IMR_MsgFromDXErr(err));
    Shutdown(); return IMRERR_DIRECTX;
     }

// Set exclusive mode:
err = DirectDraw->SetCooperativeLevel(AppWnd, DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN);
if (err != DD_OK)
    {
    IMR_LogMsg(__LINE__, __FILE__, "DirectX says: %s", IMR_MsgFromDXErr(err));
    Shutdown(); return IMRERR_DIRECTX;
     }

// Set the display mode:
err = DirectDraw->SetDisplayMode(Display.Width, Display.Height, Display.BPP, 0, 0);
if (err != DD_OK)
    {
    IMR_LogMsg(__LINE__, __FILE__, "DirectX says: Display mode unsupported!");
    Shutdown(); return IMRERR_DIRECTX;
     }

// Create the primary surface:
ZeroMemory((void *)&DDSurfaceDesc, sizeof(DDSurfaceDesc));
DDSurfaceDesc.dwSize = sizeof(DDSurfaceDesc);
DDSurfaceDesc.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
DDSurfaceDesc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_COMPLEX | DDSCAPS_FLIP; 
DDSurfaceDesc.dwBackBufferCount = 1;
err = DirectDraw->CreateSurface(&DDSurfaceDesc, &DDPrimarySurface, NULL);
if (err != DD_OK)
    {
    IMR_LogMsg(__LINE__, __FILE__, "DirectX says: %s", IMR_MsgFromDXErr(err));
    Shutdown(); return IMRERR_DIRECTX;
     }

// Get a pointer to the back buffer:
ZeroMemory((void *)&DDSCaps2, sizeof(DDSCAPS2));
DDSCaps2.dwCaps = DDSCAPS_BACKBUFFER; 
err = DDPrimarySurface->GetAttachedSurface(&DDSCaps2, &DDBackSurface); 
if (err != DD_OK)
    {
    IMR_LogMsg(__LINE__, __FILE__, "DirectX says: %s", IMR_MsgFromDXErr(err));
    Shutdown(); return IMRERR_DIRECTX;
     }
 
// And flag that DirectDraw is running:
Flags.DirectDrawActive = 1;

// And return OK:
return IMR_OK;
 }

/***************************************************************************\
  Releases all DirectDraw objects.
\***************************************************************************/
void IMR_DirectXInterface::Shutdown(void)
{
// Shutdown Direct3D:
if (Direct3DViewport)
    {
    Direct3DViewport->Release();
    Direct3DViewport = NULL;
     }
if (Direct3DDevice)
    {
    Direct3DDevice->Release();
    Direct3DDevice = NULL;
     }
if (Direct3D)
    {
    Direct3D->Release();
    Direct3D= NULL;
     }
Flags.Direct3DActive = 0;

// Shutdown DirectDraw:
if (DirectDraw)
    {
    if (DDPrimarySurface)
        {
        DDPrimarySurface->Release();
        DDPrimarySurface = NULL;
        DDBackSurface = NULL;
         }
    DirectDraw->RestoreDisplayMode();
    DirectDraw->Release();
    DirectDraw = NULL;
     }
if (DirectDraw1)     
    {
    DirectDraw1->Release();
    DirectDraw1 = NULL;
     }
Flags.DirectDrawActive = 0;
 }

/***************************************************************************\
  Restores the specified surface.
\***************************************************************************/
void IMR_DirectXInterface::Restore(LPDIRECTDRAWSURFACE4 Surface)
{
int err;

// If the surface hasn't been initialized, return:
if (!Surface) return;

// Restore the surface:
err = Surface->Restore();
if (err == DDERR_WRONGMODE)
    {
    DirectDraw->SetDisplayMode(Display.Width, Display.Height, Display.BPP, 0, 0);
    err = Surface->Restore();
     }
 }

/***************************************************************************\
  Creates a offscreen surface.
  Returns a pointer to the surface, null otherwise.
\***************************************************************************/
LPDIRECTDRAWSURFACE4 IMR_DirectXInterface::Create_Surface(int Width, int Height, int BPP)
{
DDSURFACEDESC2 DDSurfaceDesc;
DDSCAPS        DDSCaps;
LPDIRECTDRAWSURFACE4 Surface;
int err;

// Make sure DirectDraw is active:
if (!Flags.DirectDrawActive)
    return NULL;

// Fill in surface descriptor:
ZeroMemory((void *)&DDSurfaceDesc, sizeof(DDSurfaceDesc));
DDSurfaceDesc.dwSize = sizeof(DDSurfaceDesc);
DDSurfaceDesc.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
DDSurfaceDesc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
DDSurfaceDesc.dwWidth = Width;
DDSurfaceDesc.dwHeight = Height;

// Create the surface:
err = DirectDraw->CreateSurface(&DDSurfaceDesc, &Surface, NULL);
if (err != DD_OK)
    {
    IMR_LogMsg(__LINE__, __FILE__, "DirectX says: %s", IMR_MsgFromDXErr(err));
    return NULL;
     }

// And return a pointer to the surface:
return Surface;
 }

/***************************************************************************\
  Creates an 8bpp palette-indexed texture.  A pointer to the surface palette
  is passed.
  Returns a pointer to the surface, null otherwise.
\***************************************************************************/
LPDIRECTDRAWSURFACE4 IMR_DirectXInterface::Create_8BitTexture(int Width, int Height, void *Palette)
{
DDSURFACEDESC2 DDSurfaceDesc;
DDSCAPS        DDSCaps;
LPDIRECTDRAWSURFACE4 Surface;
int err;

// Make sure DirectDraw is active:
if (!Flags.DirectDrawActive)
    return NULL;

// Fill in surface descriptor:
ZeroMemory((void *)&DDSurfaceDesc, sizeof(DDSurfaceDesc));
ZeroMemory((void *)&DDSurfaceDesc.ddpfPixelFormat, sizeof(DDPIXELFORMAT));
DDSurfaceDesc.dwSize = sizeof(DDSurfaceDesc);
DDSurfaceDesc.dwFlags = DDSD_CAPS | DDSD_MIPMAPCOUNT | 
                        DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT; 
DDSurfaceDesc.ddsCaps.dwCaps = DDSCAPS_TEXTURE | DDSCAPS_COMPLEX | DDSCAPS_MIPMAP;
DDSurfaceDesc.dwMipMapCount = 4;
DDSurfaceDesc.dwWidth = Width;
DDSurfaceDesc.dwHeight = Height;
DDSurfaceDesc.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
DDSurfaceDesc.ddpfPixelFormat.dwFlags = DDPF_RGB | DDPF_PALETTEINDEXED8;
DDSurfaceDesc.ddpfPixelFormat.dwRGBBitCount = 8;

// Create the surface:
err = DirectDraw->CreateSurface(&DDSurfaceDesc, &Surface, NULL);
if (err != DD_OK)
    {
    IMR_LogMsg(__LINE__, __FILE__, "DirectX says: %s", IMR_MsgFromDXErr(err));
    return NULL;
     }

// Now set it's palette:
Set_Surface_Palette(Surface, Palette);

// And return a pointer to the surface:
return Surface;
 }

/***************************************************************************\
  Creates a 24bpp texture.
  Returns a pointer to the surface, null otherwise.
\***************************************************************************/
LPDIRECTDRAWSURFACE4 IMR_DirectXInterface::Create_24BitTexture(int Width, int Height)
{
DDSURFACEDESC2 DDSurfaceDesc;
DDSCAPS        DDSCaps;
LPDIRECTDRAWSURFACE4 Surface;
int err;

// Make sure DirectDraw is active:
if (!Flags.DirectDrawActive)
    return NULL;

// Fill in surface descriptor:
ZeroMemory((void *)&DDSurfaceDesc, sizeof(DDSurfaceDesc));
ZeroMemory((void *)&DDSurfaceDesc.ddpfPixelFormat, sizeof(DDPIXELFORMAT));
DDSurfaceDesc.dwSize = sizeof(DDSurfaceDesc);
DDSurfaceDesc.dwFlags = DDSD_CAPS | DDSD_MIPMAPCOUNT | 
                        DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT; 
DDSurfaceDesc.ddsCaps.dwCaps = DDSCAPS_TEXTURE | DDSCAPS_COMPLEX | DDSCAPS_MIPMAP;
DDSurfaceDesc.dwMipMapCount = 4;
DDSurfaceDesc.dwWidth = Width;
DDSurfaceDesc.dwHeight = Height;
DDSurfaceDesc.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
DDSurfaceDesc.ddpfPixelFormat.dwFlags = DDPF_RGB;
DDSurfaceDesc.ddpfPixelFormat.dwRGBBitCount = 24;
DDSurfaceDesc.ddpfPixelFormat.dwRBitMask = 0x00FF0000;
DDSurfaceDesc.ddpfPixelFormat.dwGBitMask = 0x0000FF00;
DDSurfaceDesc.ddpfPixelFormat.dwBBitMask = 0x000000FF;

// Create the surface:
err = DirectDraw->CreateSurface(&DDSurfaceDesc, &Surface, NULL);
if (err != DD_OK)
    {
    IMR_LogMsg(__LINE__, __FILE__, "DirectX says: %s", IMR_MsgFromDXErr(err));
    return NULL;
     }

// And return a pointer to the surface:
return Surface;
 }

/***************************************************************************\
  Creates a surface for 3d rendering.
  Returns a pointer to the surface, null otherwise.
\***************************************************************************/
LPDIRECTDRAWSURFACE4 IMR_DirectXInterface::Create_3DSurface(int Width, int Height, int BPP)
{
DDSURFACEDESC2 DDSurfaceDesc;
DDSCAPS        DDSCaps;
LPDIRECTDRAWSURFACE4 Surface;
int err;

// Make sure DirectDraw is active:
if (!Flags.DirectDrawActive)
    return NULL;

// Fill in surface descriptor:
ZeroMemory((void *)&DDSurfaceDesc, sizeof(DDSurfaceDesc));
DDSurfaceDesc.dwSize = sizeof(DDSurfaceDesc);
DDSurfaceDesc.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
DDSurfaceDesc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_3DDEVICE;
DDSurfaceDesc.dwWidth = Width;
DDSurfaceDesc.dwHeight = Height;

// Create the surface:
err = DirectDraw->CreateSurface(&DDSurfaceDesc, &Surface, NULL);
if (err != DD_OK)
    {
    IMR_LogMsg(__LINE__, __FILE__, "DirectX says: %s", IMR_MsgFromDXErr(err));
    return NULL;
     }

// And return a pointer to the surface:
return Surface;
 }

/***************************************************************************\
  Creates a 16x16 24bpp texture with a ramp palette of the specified color 
  for use with lightmapping.

  Data must have at least 768 bytes allocated, otherwise program may cause
  general protection faults...

  Returns a pointer to the surface, null otherwise.
\***************************************************************************/
LPDIRECTDRAWSURFACE4 IMR_DirectXInterface::Create_Lightmap(void)
{
DDSURFACEDESC2 DDSurfaceDesc;
DDSCAPS        DDSCaps;
LPDIRECTDRAWSURFACE4 Surface;
int err;

// Make sure DirectDraw is active:
if (!Flags.DirectDrawActive)
    return NULL;

// Fill in the initial surface descriptor:
ZeroMemory((void *)&DDSurfaceDesc, sizeof(DDSurfaceDesc));
ZeroMemory((void *)&DDSurfaceDesc.ddpfPixelFormat, sizeof(DDPIXELFORMAT));
DDSurfaceDesc.dwSize = sizeof(DDSurfaceDesc);
DDSurfaceDesc.dwFlags = DDSD_HEIGHT | DDSD_WIDTH | DDSD_PITCH |
                        DDSD_CAPS | DDSD_PIXELFORMAT;
DDSurfaceDesc.ddsCaps.dwCaps = DDSCAPS_TEXTURE;
DDSurfaceDesc.ddsCaps.dwCaps2 = DDSCAPS2_HINTDYNAMIC;
DDSurfaceDesc.dwWidth = 16;
DDSurfaceDesc.dwHeight = 16;
DDSurfaceDesc.lPitch = 16 * 3;
DDSurfaceDesc.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
DDSurfaceDesc.ddpfPixelFormat.dwFlags = DDPF_RGB;
DDSurfaceDesc.ddpfPixelFormat.dwRGBBitCount = 24;
DDSurfaceDesc.ddpfPixelFormat.dwRBitMask = 0x00FF0000;
DDSurfaceDesc.ddpfPixelFormat.dwGBitMask = 0x0000FF00;
DDSurfaceDesc.ddpfPixelFormat.dwBBitMask = 0x000000FF;

// Create the surface:
err = DirectDraw->CreateSurface(&DDSurfaceDesc, &Surface, NULL);
if (err != DD_OK)
    {
    IMR_LogMsg(__LINE__, __FILE__, "DirectX says: %s", IMR_MsgFromDXErr(err));
    return NULL;
     }

// And return a pointer to the surface:
return Surface;
 }

/***************************************************************************\
  Locks the specified surface to allow drawing.  Returns a pointer to the
  surface memory if a lock was obtained, otherwise returns null.
  
  -- PITCH IS CURRENTLY IGNORED - MAY CAUSE PROBLEMS --
\***************************************************************************/
void *IMR_DirectXInterface::Lock_Surface(LPDIRECTDRAWSURFACE4 Surface)
{
DDSURFACEDESC2 DDSurfaceDesc;
HRESULT ddflag;

// If a null surface was specified, use the primary:
if (!Surface) Surface = DDPrimarySurface;

// Lock the surface.  If we loose it, restore it and try again:
DDSurfaceDesc.dwSize = sizeof(DDSurfaceDesc);
ddflag = DDERR_SURFACELOST;
while (ddflag == DDERR_SURFACELOST)
    {
    ddflag = Surface->Lock(NULL, &DDSurfaceDesc, DDLOCK_WAIT, NULL);
    if (ddflag == DDERR_SURFACELOST)
        Restore(Surface);
    }
if (ddflag != DD_OK)
    return NULL;
    
// And return a pointer to the surface memory:
return DDSurfaceDesc.lpSurface;
 }

/***************************************************************************\
  Unlocks the specified surface.  
\***************************************************************************/
void IMR_DirectXInterface::Unlock_Surface(LPDIRECTDRAWSURFACE4 Surface)
{
if (!Surface) Surface = DDPrimarySurface;
Surface->Unlock(NULL);
}

/***************************************************************************\
  Sets the palette for the specified surface.
  Palette is a pointer to 256 3-byte (RGB) palette entries.  
\***************************************************************************/
int IMR_DirectXInterface::Set_Surface_Palette(LPDIRECTDRAWSURFACE4 Surface, void *Palette)
{
PALETTEENTRY SurfacePal[256];
LPDIRECTDRAWPALETTE DDPalette;
char *Pal = (char *)Palette;
int ColorIndex, err;

// If NULL was specified for Surface, use the primary surface:
if (!Surface) Surface = DDPrimarySurface;
if (!DDPrimarySurface)
    {
    IMR_LogMsg(__LINE__, __FILE__, "No primary surface!");
    return IMRERR_NODATA;
     }

// Convert the palette:
for (int color = 0; color < 256; color ++)
    {
    ColorIndex = color * 3;
    SurfacePal[color].peRed   = Pal[ColorIndex];
    SurfacePal[color].peGreen = Pal[ColorIndex + 1];
    SurfacePal[color].peBlue  = Pal[ColorIndex + 2];
    SurfacePal[color].peFlags = PC_NOCOLLAPSE;
     }

// Make sure the DirectDraw object and the surfaces have been initialized:
if (DirectDraw && Surface)
    {
    // Create the palette.  If we can't, return false:
    err = DirectDraw->CreatePalette(DDPCAPS_8BIT | DDPCAPS_ALLOW256, SurfacePal, &DDPalette, NULL);
    if (err != DD_OK)
        {
        IMR_LogMsg(__LINE__, __FILE__, "DirectX says: %s", IMR_MsgFromDXErr(err));
        return IMRERR_DIRECTX;
         }
  
    // Set the palette for the surface:
    Surface->SetPalette(DDPalette);
     }
else
    {
    IMR_LogMsg(__LINE__, __FILE__, "No surface specified or not initialized!");
    return IMRERR_NODATA;
     }

// Return OK:
return IMR_OK;
 }

/***************************************************************************\
  Clears the specified surface.
\***************************************************************************/
void IMR_DirectXInterface::Clear_Surface(LPDIRECTDRAWSURFACE4 Surface)
{
HRESULT ddflag;
RECT Area;
DDSURFACEDESC2 DDSurfaceDesc;
char *Mem;

// Make sure the surface exists:
if (!Surface) return;

// Lock the surface:
DDSurfaceDesc.dwSize = sizeof(DDSurfaceDesc);
ddflag = Surface->Lock(&Area, &DDSurfaceDesc, DDLOCK_WAIT, NULL);
if (ddflag == DDERR_SURFACELOST)
    {
    Restore(Surface);
    ddflag = Surface->Lock(NULL, &DDSurfaceDesc, DDLOCK_WAIT, NULL);
     }
if (ddflag != DD_OK) return;
Mem = (char *)DDSurfaceDesc.lpSurface;

// Clear the surface:
if (Mem) memset(Mem, 0, DDSurfaceDesc.dwSize);

// And unlock the surface:
Surface->Unlock(NULL);
 }

/***************************************************************************\
 Flips the primary and the back buffer surfaces.
\***************************************************************************/
int IMR_DirectXInterface::FlipBackBuffer(void)
{
int err;
if (DDPrimarySurface)
    {
    err = DDPrimarySurface->Flip(NULL, DDFLIP_WAIT);
    if (err != DD_OK)
        {
        IMR_LogMsg(__LINE__, __FILE__, "DirectX says: %s", IMR_MsgFromDXErr(err));
        return IMRERR_DIRECTX;
         }
     }
return IMR_OK;
 }

/***************************************************************************\
  Blits the specified surface to the primary surface.
  (Visible immediately)
\***************************************************************************/
int IMR_DirectXInterface::BlitToPrimary(LPDIRECTDRAWSURFACE4 Source)
{
return Blit(Source, DDPrimarySurface);
 }

/***************************************************************************\
  Blits the specified surface to the back buffer surface.
  (Must be flipped to be visible)
\***************************************************************************/
int IMR_DirectXInterface::BlitToBack(LPDIRECTDRAWSURFACE4 Source)
{
return Blit(Source, DDBackSurface);
 }

/***************************************************************************\
  Blits one surface to another.
\**************************************************************************/
int IMR_DirectXInterface::Blit(LPDIRECTDRAWSURFACE4 Source, LPDIRECTDRAWSURFACE4 Target)
{
RECT Dest;
HRESULT ddrval;
DDSURFACEDESC2 SourceDesc, TargetDesc;
int BltX, BltY;

// Make sure DirectDraw is active:
if (!Flags.DirectDrawActive)
    {
    IMR_LogMsg(__LINE__, __FILE__, "Interface not initialized!");
    return IMRERR_NOTREADY;
     }

// Make sure we have surfaces:
if (!Source || !Target)
    {
    IMR_LogMsg(__LINE__, __FILE__, "NUKK surface(s) specified!");
    return IMRERR_NODATA;
     }

// Get a surface descriptor for the surfaces:
TargetDesc.dwSize = sizeof(TargetDesc);
Target->GetSurfaceDesc(&TargetDesc);
SourceDesc.dwSize = sizeof(SourceDesc);
Source->GetSurfaceDesc(&SourceDesc);

// Now find the blit coords:
BltX = (TargetDesc.dwWidth / 2) - (SourceDesc.dwWidth / 2);
BltY = (TargetDesc.dwHeight / 2) - (SourceDesc.dwHeight / 2);

// Blit the surface:
ddrval = Target->BltFast(BltX, BltY, Source, NULL, DDBLTFAST_NOCOLORKEY | DDBLTFAST_WAIT);

// If we lost the surface, return failure:
if (ddrval == DDERR_SURFACELOST)
    {
    Restore(Source);
    Restore(Target);
    IMR_LogMsg(__LINE__, __FILE__, "Lost surface!");
    return IMRERR_DIRECTX;
     }

// Return ok:
return IMR_OK;
 }

/***************************************************************************\
  Initializes Direct3D.
\***************************************************************************/
int IMR_DirectXInterface::InitDirect3D(LPDIRECTDRAWSURFACE4 Target)
{
int err;
D3DVIEWPORT2   PortInitData;
DDPIXELFORMAT  ZBufferPixelFormat;  
DDSURFACEDESC2 ZBufferDesc, TargetDesc;
D3DDEVICEDESC  HALDesc, HELDesc;
int DeviceFound = FALSE, FormatFound = FALSE;

// Make sure DirectDraw is active:
if (!Flags.DirectDrawActive)
    {
    IMR_LogMsg(__LINE__, __FILE__, "DirectDraw not initialized!");
    return IMRERR_NOTREADY;
     }

// Make sure we have a target surface:
if (!Target)
    {
    IMR_LogMsg(__LINE__, __FILE__, "NULL target specified!");
    return IMRERR_NODATA;
     }

// Get target size info:
TargetDesc.dwSize = sizeof(TargetDesc);
Target->GetSurfaceDesc(&TargetDesc);

// Release all Direct3D interfaces:
if (Direct3DViewport)
    {
    Direct3DViewport->Release();
    Direct3DViewport = NULL;
     }
if (Direct3DDevice)
    {
    Direct3DDevice->Release();
    Direct3DDevice = NULL;
     }
if (Direct3D)
    {
    Direct3D->Release();
    Direct3D = NULL;
     }
Flags.Direct3DActive = 0;

// Now get a new Direct3D3 interface:
DirectDraw->QueryInterface(IID_IDirect3D3, (void **)&Direct3D);
if (err != DD_OK)
    {
    IMR_LogMsg(__LINE__, __FILE__, "DirectX says: %s", IMR_MsgFromDXErr(err));
    return IMRERR_DIRECTX;
     }

// Enumerate the devices and find one to use:
err = Direct3D->EnumDevices(DeviceCallback, &DeviceFound);
if (err != D3D_OK)
    {
    IMR_LogMsg(__LINE__, __FILE__, "DirectX says: %s", IMR_MsgFromDXErr(err));
    return IMRERR_DIRECTX;
     }
if (!DeviceFound) 
    { 
    IMR_LogMsg(__LINE__, __FILE__, "No usable rendering devices!");
    return IMRERR_DIRECTX;
     }

// Find a pixel format for our z-buffer and verify that it worked:
Direct3D->EnumZBufferFormats(Direct3DDeviceInfo.Guid, ZBufferCallback, (void *)&ZBufferPixelFormat);
if (ZBufferPixelFormat.dwSize != sizeof(DDPIXELFORMAT))
    {
    IMR_LogMsg(__LINE__, __FILE__, "Couldn't find acceptable Z-Buffer pixel format!");
    return IMRERR_DIRECTX;
     }

// Create a Z-Buffer:
ZeroMemory((void *)&ZBufferDesc, sizeof(ZBufferDesc));
ZBufferDesc.dwSize = sizeof(ZBufferDesc);
ZBufferDesc.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT | DDSD_PIXELFORMAT;
ZBufferDesc.ddsCaps.dwCaps = DDSCAPS_ZBUFFER;
ZBufferDesc.dwWidth = TargetDesc.dwWidth;
ZBufferDesc.dwHeight = TargetDesc.dwHeight;
memcpy(&ZBufferDesc.ddpfPixelFormat, &ZBufferPixelFormat, sizeof(DDPIXELFORMAT));
if (IsEqualIID(Direct3DDeviceInfo.Guid, IID_IDirect3DHALDevice))
    ZBufferDesc.ddsCaps.dwCaps |= DDSCAPS_VIDEOMEMORY;
else
    ZBufferDesc.ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY; 
err = DirectDraw->CreateSurface(&ZBufferDesc, &DDZBuffer, NULL);
if (err != DD_OK)
    {
    IMR_LogMsg(__LINE__, __FILE__, "DirectX says: %s", IMR_MsgFromDXErr(err));
    return IMRERR_DIRECTX;
     }

// Now attach the Z-buffer to the target surface:
err = Target->AddAttachedSurface(DDZBuffer);
if (err != DD_OK)
    {
    IMR_LogMsg(__LINE__, __FILE__, "DirectX says: %s", IMR_MsgFromDXErr(err));
    return IMRERR_DIRECTX;
     }

// Now create the device:
err = Direct3D->CreateDevice(Direct3DDeviceInfo.Guid, Target, &Direct3DDevice, NULL);
if (err != D3D_OK)
    {
    IMR_LogMsg(__LINE__, __FILE__, "DirectX says: %s", IMR_MsgFromDXErr(err));
    return IMRERR_DIRECTX;
     }
 
// Setup the device:
Direct3DDevice->SetRenderState(D3DRENDERSTATE_ZENABLE, D3DZB_TRUE);
Direct3DDevice->SetRenderState(D3DRENDERSTATE_SHADEMODE, D3DSHADE_GOURAUD);
Direct3DDevice->SetRenderState(D3DRENDERSTATE_CULLMODE, D3DCULL_NONE);
Direct3DDevice->SetLightState(D3DLIGHTSTATE_MATERIAL, NULL);

// Make sure this device can handle our textures:
Direct3DDevice->EnumTextureFormats(TextureFormatCallback, (void *)&FormatFound);
if (!FormatFound)
    {
    IMR_LogMsg(__LINE__, __FILE__, "No acceptable texture formats found!");
    return IMRERR_DIRECTX;
     }

// Now create a viewport init structure:
ZeroMemory(&PortInitData, sizeof(D3DVIEWPORT2));
PortInitData.dwSize = sizeof(D3DVIEWPORT2);
PortInitData.dwX = 0;
PortInitData.dwY = 0;
PortInitData.dwWidth = TargetDesc.dwWidth;
PortInitData.dwHeight = TargetDesc.dwHeight;
PortInitData.dvClipX = 0;
PortInitData.dvClipY = 0;
PortInitData.dvClipWidth = TargetDesc.dwWidth;
PortInitData.dvClipHeight = TargetDesc.dwHeight;
PortInitData.dvMinZ = 0.0f;
PortInitData.dvMaxZ = 1.0f;

// Create the viewport:
err = Direct3D->CreateViewport(&Direct3DViewport, NULL);
if (err != D3D_OK)
    {
    IMR_LogMsg(__LINE__, __FILE__, "DirectX says: %s", IMR_MsgFromDXErr(err));
    return IMRERR_DIRECTX;
     }

// Associate the viewport with the device:
Direct3DDevice->AddViewport(Direct3DViewport); 

// Set the parameters for the new viewport:
Direct3DViewport->SetViewport2(&PortInitData);

// Set the current viewport for the device:
Direct3DDevice->SetCurrentViewport(Direct3DViewport);

// Flag that Direct3D is up and running:
Flags.Direct3DActive = 1;

// Whew!  That's done, so return success:
return IMR_OK;
 }

/***************************************************************************\
  Prepares all surfaces for rendering.
\***************************************************************************/
void IMR_DirectXInterface::Prepare_Frame(LPDIRECTDRAWSURFACE4 Target)
{
D3DRECT ClearRect;
DDSURFACEDESC2 TargetDesc;

// Make sure we have a target surface and a viewport interface:
if (!Direct3DViewport || !Target)
    return;

// Get target size info:
TargetDesc.dwSize = sizeof(TargetDesc);
Target->GetSurfaceDesc(&TargetDesc);

// Setup the area to clear:
ClearRect.x1 = 0;
ClearRect.y1 = 0;
ClearRect.x2 = TargetDesc.dwWidth;
ClearRect.y2 = TargetDesc.dwHeight;

// Clear the viewport:
Direct3DViewport->Clear2(1, &ClearRect, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, (float)1.0, NULL);
 }
