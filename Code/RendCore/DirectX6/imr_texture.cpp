/****************************************************************\
 
 iMMERSE Engine
 (C) 1999 No Tears Shed Software
 All rights reserved
 
 Filename: IMR_Texture.hpp
 Description: Texture primitive methods
 
\****************************************************************/
#include "IMR_Texture.hpp"

/***************************************************************************\
  Sets the name of the texture.
\***************************************************************************/
void IMR_Texture::Set_Name(char *NewName)
{
if (strlen(NewName) > 8)
    {
    memcpy((void *)Name, (void *)NewName, 8);
    Name[8] = '/0';
     }
else
    strcpy(Name, NewName);
 }

/***************************************************************************\
  Creates a texture with the specified dimentions.
  Returns IMR_OK if successful, an error otherwise.
\***************************************************************************/
int IMR_Texture::Create(IMR_DirectXInterface &DX, int W, int H, void *Pal)
{
// Reset the texture:
Reset();

// Create the surface:
//DDSurface = DX.Create_8BitTexture(W, H, Pal);
DDSurface = DX.Create_16BitTexture(W, H);
if (!DDSurface) return IMRERR_DIRECTX;
DDSurface->QueryInterface(IID_IDirect3DTexture2, (void**)&D3DTexture);
if (!D3DTexture)
    {
    IMR_LogMsg(__LINE__, __FILE__, "QueryInterface failed!");
    return IMRERR_GENERIC;
     }
Width = W;
Height = H;
Size = W * H * 2;// * 3;  // 24 bpp = 3 bytes
Type = Texture;

// And return ok:
return IMR_OK;
 }

/***************************************************************************\
  Creates a lightmap with the specified dimentions.
  Returns IMR_OK if successful, an error otherwise.
\***************************************************************************/
int IMR_Texture::Create_Lightmap(IMR_DirectXInterface &DX, int W, int H)
{
// Reset the texture:
Reset();

// Create the surface:
DDSurface = DX.Create_Lightmap(W, H);
if (!DDSurface) return IMRERR_DIRECTX;
DDSurface->QueryInterface(IID_IDirect3DTexture2, (void**)&D3DTexture);
if (!D3DTexture)
    {
    IMR_LogMsg(__LINE__, __FILE__, "QueryInterface failed!");
    return IMRERR_GENERIC;
     }
Width = W;
Height = H;
Size = W * H;
Type = Lightmap; 

// And return ok:
return IMR_OK;
 }
 
/***************************************************************************\
  Locks the surface data.
  Returns IMR_OK if successful, an error otherwise.
\***************************************************************************/
void *IMR_Texture::LockData(IMR_DirectXInterface &DX)
{
void *dt = DX.Lock_Surface(DDSurface);
if (!dt)
    {
    IMR_LogMsg(__LINE__, __FILE__, "Lock failed!");
    return NULL;
     }
IsLocked = 1;
Data = dt;
return Data;
 }

/***************************************************************************\
  Unlocks the surface data.
  Returns IMR_OK if successful, an error otherwise.
\***************************************************************************/
void IMR_Texture::UnlockData(IMR_DirectXInterface &DX)
{
DX.Unlock_Surface(DDSurface);
IsLocked = 0;
 };
