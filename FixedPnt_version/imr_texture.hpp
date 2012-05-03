/***************************************************************************\
  File: IMR_Texture.hpp
  Description: Header.
  Author: Daniel Hawthorn
  Modified:
  
\***************************************************************************/
#ifndef __IMR_TEXTURE__HPP
#define __IMR_TEXTURE__HPP

// Include stuff:
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <conio.h>
#include <io.h>
#include <sys\types.h>
#include <sys\stat.h>
#include <fcntl.h>
#include "IMR_Err.hpp"
#include "IMR_Palette.hpp"
#include "IMR_DirectX.hpp"
#include "IMR_FixedPnt.hpp"

// Texture class:
class IMR_Texture
    {
    protected:
      // Texture name:
      char Name[9];
      
      // Size and format info:
      int Width, Height, Size, BPP;

      // Texture data and palette info:
      IMR_PaletteInfo Palette;

      // DirectX stuff:
      LPDIRECTDRAWSURFACE4 DDSurface;       // Surface interface
      LPDIRECT3DTEXTURE2   D3DTexture;      // Texture interface

    public:
       // Constructor and destructor:
      IMR_Texture()
          { 
          Width = Height = Size = 0;
          D3DTexture = NULL;
          DDSurface = NULL;
           };
      ~IMR_Texture() { Reset(); };
        
      // Init and shutdown methods:
      inline int Init(void) { return IMR_OK; };
      inline int Reset(void) 
          {
          Width = Height = Size = 0;
          if (D3DTexture) D3DTexture->Release();
          D3DTexture = NULL;
          if (DDSurface) DDSurface->Release();
          DDSurface = NULL;
          return IMR_OK;
           }
      
      // Name methods:
      void Set_Name(char *NewName);
      inline char *Get_Name(void) { return Name; };
      inline int Is(char *CmpName) { return !stricmp(Name, CmpName); };
      
      // Size methods:
      inline int Get_Width(void) { return Width; };
      inline int Get_Height(void) { return Height; };
      
      // DirectX access methods:
      inline LPDIRECT3DTEXTURE2 Get_TextureInterface(void) { return D3DTexture; };
      inline LPDIRECTDRAWSURFACE4 Get_Surface(void) { return DDSurface; };
      
      // Loading methods:
      int Load_PCX(IMR_DirectXInterface &DX, char *Filename, char *TextureName);
     };

#endif
