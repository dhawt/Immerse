/****************************************************************\
 
 iMMERSE Engine
 (C) 1999 No Tears Shed Software
 All rights reserved
 
 Filename: IMR_TexRef.hpp
 Description: Header
 
\****************************************************************/
#ifndef __IMR_TEXREF__HPP
#define __IMR_TEXREF__HPP

// Constants:
#define IMR_REFTYPE_TEXTURE 0
#define IMR_REFTYPE_LIGHTMAP 1

// Texture class:
class IMR_TexRef
    {
    friend class IMR_Renderer;
    protected:
      // Data:
      void *Data;
      void *Palette;
      
      // Host info:
      struct
          {
          void *Ptr;
          bool Flag1,
               Flag2,
               Flag3,
               Flag4;
           } Host;
      
    public:
      // Constructor and destructor:
      IMR_TexRef() { Data = Palette = Host.Ptr = NULL; };
      ~IMR_TexRef() { };
      
      // Size and type info:
      int Width, Height;
      int Type;
      
      // Data access methods:
      void *Get_Data(void) { return Data; };
      inline int HasHost(void) { return (Host.Ptr != NULL); };
      inline void Set_Palette(void *P) { Palette = P; };
     };

#endif
