/***************************************************************************\
  File: IMR_RendBuffer.cpp
  Description: Renderer buffer module.
  Author: Daniel Hawthorn
  Modified:
  
\***************************************************************************/
#include "IMR_RendBuffer.hpp"

/***************************************************************************\
  Initializes the buffer.

  Returns IMR_OK if successful, otherwise an error.
\***************************************************************************/
int IMR_RendBuffer::Set_Screen(int W, int H, HWND Wnd, IMR_DirectXInterface *DX)
{
IMR_DirectXInterface_InitData DXInitData;
int err;

// Make sure we have a DirectX interface:
if (!DX) 
    {
    IMR_SetErrorText("IMR_RendBuffer::Init(): Null DirectX interface specified!");
    return IMRERR_NODATA;
     }            
DirectX = DX;
    
// Shut down DirectX:
DirectX->Shutdown();

// Setup DirectX:
DXInitData.AppWnd = Wnd;
DXInitData.Display.Width = W;
DXInitData.Display.Height = H;
DXInitData.Display.BPP = 16;
err = DirectX->Init(DXInitData);
if (IMR_ISNOTOK(err)) return err;

// Set vars:
ScreenWidth = W;
ScreenHeight = H;

// And return OK:
return IMR_OK;
 }

/***************************************************************************\
  Sets up the render target surface.
  Returns IMR_OK if successful, otherwise an error.
\***************************************************************************/
int IMR_RendBuffer::Set_RenderTarget(int W, int H, IMR_DirectXInterface *DX)
{
// Make sure we have a DirectX interface:
if (!DX) 
    {
    IMR_SetErrorText("IMR_RendBuffer::Init(): Null DirectX interface specified!");
    return IMRERR_NODATA;
     }            
    
// Setup buffer info:
Width = W;
Height = H;
MinX = 0; MaxX = Width - 1;
MinY = 0; MaxY = Height - 1;
XCenter = Width / 2;
YCenter = Height / 2;
Length = Width * Height * 2;    // 2-byte pixels...

// Destroy the double-buffer:
if (BufferSurface)
    DirectX->Destroy_Surface(BufferSurface);
BufferSurface = NULL;

// Create a new double-buffer surface:
BufferSurface = DirectX->Create_3DSurface(Width, Height, 16);
if (!BufferSurface) return IMRERR_DIRECTX;

// (Re)initialize Direct3D:
int err = DirectX->InitDirect3D(BufferSurface);
if (IMR_ISNOTOK(err)) return err;

// And return ok:
return IMR_OK;
 }

/***************************************************************************\
  Shuts down the buffer.  
\***************************************************************************/
void IMR_RendBuffer::Shutdown(void)
{
// Destroy the double-buffer:
if (BufferSurface)
    DirectX->Destroy_Surface(BufferSurface);
BufferSurface = NULL;

// Shutdown DirectX:
if (DirectX) 
    DirectX->Shutdown();

// Reset everything:
Width = Height = Length = 0;
 }

/***************************************************************************\
  Locks the double-buffer surface to allow writes.
  Returns IMR_OK if successful, otherwise an error.
\***************************************************************************/
int IMR_RendBuffer::Lock(void)
{
// Make sure we have a DirectX interface:
if (!DirectX)
    {
    IMR_SetErrorText("IMR_Rendbuffer::Lock(): No DirectX interface!");
    return IMRERR_GENERIC;
     }

// If we are already locked, unlock ourselves:
if (Flags.BufferLocked) Unlock();

// Lock the surface:
Data = (char *)DirectX->Lock_Surface(BufferSurface);

// Did it work?
if (!Data)
    {
    IMR_SetErrorText("IMR_Rendbuffer::Lock(): Lock failed!");
    return IMRERR_NODATA;
     }

// Set locked flag:
Flags.BufferLocked = 1;

// Return ok:
return IMR_OK;
 }

/***************************************************************************\
  Unlocks the double-buffer surface.
  Returns IMR_OK if successful, otherwise an error.
\***************************************************************************/
void IMR_RendBuffer::Unlock(void)
{
// Make sure we have a DirectX interface:
if (!DirectX) return;

// If we aren't locked, go away:
if (!Flags.BufferLocked)
    return;

// Unlock the surface:
DirectX->Unlock_Surface(BufferSurface);
Data = NULL;

// Set locked flag:
Flags.BufferLocked = 0;
 }

/***************************************************************************\
  Locks the back surface to allow writes.
  Returns IMR_OK if successful, otherwise an error.
\***************************************************************************/
int IMR_RendBuffer::LockBack(void)
{
// Make sure we have a DirectX interface:
if (!DirectX)
    {
    IMR_SetErrorText("IMR_Rendbuffer::LockBack(): No DirectX interface!");
    return IMRERR_GENERIC;
     }

// If we are already locked, unlock ourselves:
if (Flags.BackLocked) Unlock();

// Lock the surface:
BackData = (char *)DirectX->Lock_Surface(DirectX->Get_BackSurface());

// Did it work?
if (!BackData)
    {
    IMR_SetErrorText("IMR_Rendbuffer::LockBack(): Lock failed!");
    return IMRERR_NODATA;
     }

// Set locked flag:
Flags.BackLocked = 1;

// Return ok:
return IMR_OK;
 }

/***************************************************************************\
  Unlocks the Back surface.
  Returns IMR_OK if successful, otherwise an error.
\***************************************************************************/
void IMR_RendBuffer::UnlockBack(void)
{
// Make sure we have a DirectX interface:
if (!DirectX) return;

// If we aren't locked, go away:
if (!Flags.BackLocked)
    return;

// Unlock the surface:
DirectX->Unlock_Surface(DirectX->Get_BackSurface());
BackData = NULL;

// Set locked flag:
Flags.BackLocked = 0;
 }

/***************************************************************************\
  Clears all the buffers.
\***************************************************************************/
void IMR_RendBuffer::Clear_Buffers(void)
{
if (DirectX && BufferSurface)
    DirectX->Prepare_Frame(BufferSurface);
 }

/***************************************************************************\
  Blits the double-buffer surface to the screen.
\***************************************************************************/
int IMR_RendBuffer::Blit(void)
{
// Make sure everythings ok for a blit:
if (Flags.BufferLocked || !DirectX || !BufferSurface) 
    {
    IMR_SetErrorText("IMR_RendBuffer::Blit(): State not correct!");
    return IMRERR_NODATA;
     }

// And blit:
DirectX->BlitToBack(BufferSurface);

return IMR_OK;
 }

/***************************************************************************\
  Flips the Back and back buffer surfaces.
\***************************************************************************/
int IMR_RendBuffer::Flip(void)
{
// Make sure everythings ok for a blit:
if (Flags.BackLocked || !DirectX) 
    {
    IMR_SetErrorText("IMR_RendBuffer::Flip(): State not correct!");
    return IMRERR_NODATA;
     }

// And flip:
DirectX->FlipBackBuffer();

return IMR_OK;
 }
