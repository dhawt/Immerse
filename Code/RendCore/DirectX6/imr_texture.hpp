/****************************************************************\
 
 iMMERSE Engine
 (C) 1999 No Tears Shed Software
 All rights reserved
 
 Filename: IMR_Texture.hpp
 Description: Header - used internally by IMR_Renderer
 
\****************************************************************/
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
#include "IMR_DirectX.hpp"
#include "..\..\CallStatus\IMR_Log.hpp"

// Texture class:
class IMR_Texture
    {
    protected:
      // Texture name:
      char Name[9];
      
      // Size and format info:
      int Width, Height, Size;
      enum { Null, Texture, Lightmap } Type;
      int IsLocked;

      // DirectX stuff:
      LPDIRECTDRAWSURFACE4 DDSurface;       // Surface interface
      LPDIRECT3DTEXTURE2   D3DTexture;      // Texture interface
      void *Data;

    public:
       // Constructor and destructor:
      IMR_Texture()
          {
          IsLocked = 0;
          Data = NULL;
          Type = Null;
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
      int Create(IMR_DirectXInterface &, int, int, void *);
      int Create_Lightmap(IMR_DirectXInterface &, int, int);
      
      // Data methods:
      void *LockData(IMR_DirectXInterface &DX);
      void UnlockData(IMR_DirectXInterface &DX);
      inline int DataIsLocked(void) { return IsLocked; };
      inline void *GetData(void) { if (DataIsLocked()) return Data; return NULL; };
      
      // DirectX access methods:
      inline LPDIRECT3DTEXTURE2 Get_TextureInterface(void) { return D3DTexture; };
      inline LPDIRECTDRAWSURFACE4 Get_Surface(void) { return DDSurface; };
     };

// Lightmap class:
class IMR_Lightmap:public IMR_Texture
    {
    protected:
      int ID;
      
    public:
      void Set_ID(int id) { ID = id; };
      inline int Get_ID(void) { return ID; };
      inline int Is(int i) { return (ID == i); };
     };

#endif
