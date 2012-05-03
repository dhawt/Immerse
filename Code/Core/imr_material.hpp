/****************************************************************\
 
 iMMERSE Engine
 (C) 1999 No Tears Shed Software
 All rights reserved
 
 Filename: IMR_Material.hpp
 Description: Header
 
\****************************************************************/
#ifndef __IMR_MATERIAL__HPP
#define __IMR_MATERIAL__HPP

// Include stuff:
#include <stdlib.h>
#include <string.h>
#include "IMR_TexRef.hpp"
#include "..\CallStatus\IMR_Log.hpp"

// Constants and macros:
#ifndef IMR_ON
    #define IMR_ON  1
#endif
#ifndef IMR_OFF
    #define IMR_OFF 0
#endif
#define IMR_MIPMAPLEVEL_ONE   0
#define IMR_MIPMAPLEVEL_TWO   1
#define IMR_MIPMAPLEVEL_THREE 2
#define IMR_MIPMAPLEVEL_FOUR  3

// Material class:
class IMR_Material
    {
    private:
      static int Count;                    // Internal counter (used for id)

    protected:
      char Name[9],                        // Name of this material
           TextureName[9];                  // Name of texture for this material
      int LightmapID;                      // Id for lightmap
      IMR_TexRef Texture,                   // Texture to map onto the poly
                 LightMap;                  // Lightmap for poly
      
      struct
          {
          unsigned int Shiny:1;
          unsigned int Reflective:1;
          unsigned int Transparent:1;
           } Flags;

    public:
      // Constructor and destructor:
      IMR_Material()
          { 
          Flags.Shiny = IMR_OFF;
          Flags.Reflective = IMR_OFF;
          Flags.Transparent = IMR_OFF;
           };
      ~IMR_Material() { Shutdown(); };
      void Shutdown(void)
          { 
           };

      // Name methods:
      void Set_Name(char *NewName);
      inline char *Get_Name(void) { return Name; };
      inline int Is(char *CmpName) { return !stricmp(Name, CmpName); };
      void Set_TextureName(char *NewName);
      inline char *Get_TextureName(void) { return TextureName; };

      // Texture methods:
      int Set_Texture(IMR_TexRef Tex) { Texture = Tex; return IMR_OK; };
      IMR_TexRef Get_Texture(void) { return Texture; };

      // Appearance methods:
      inline void Set_Shiny(int Val) { Flags.Shiny = Val ? IMR_ON:IMR_OFF; };
      inline void Set_Reflective(int Val) { Flags.Reflective = Val ? IMR_ON:IMR_OFF; };
      inline void Set_Transparent(int Val) { Flags.Transparent = Val ? IMR_ON:IMR_OFF; };
      inline int Get_Shiny(void) { return Flags.Shiny; };
      inline int Get_Reflective(void) { return Flags.Reflective; };
      inline int Get_Transparent(void) { return Flags.Transparent; };
     };

#endif
