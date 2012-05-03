/***************************************************************************\
  File: IMR_Material.hpp
  Description: Header.
  Author: Daniel Hawthorn
  Modified:
  
\***************************************************************************/
#ifndef __IMR_MATERIAL__HPP
#define __IMR_MATERIAL__HPP

// Include stuff:
#include <stdlib.h>
#include <string.h>
#include "IMR_DirectX.hpp"
#include "IMR_Err.hpp"
#include "IMR_FixedPnt.hpp"
#include "IMR_Texture.hpp"

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
    protected:
      char Name[9],                         // Name of this material
            TextureName[9];                 // Name of texture for this material
      IMR_Texture *Texture;                 // Texture to map onto the poly
      LPDIRECTDRAWSURFACE4 LightmapSurface; // DirectDraw lightmap surface
      LPDIRECT3DTEXTURE2   Lightmap;        // Direct3D Texture interface
      IMR_DirectXInterface *DirectX;        // Pointer to DirectX interface
      
      struct
          {
          unsigned int Shiny:1;
          unsigned int Reflective:1;
          unsigned int Transparent:1;
           } Flags;
      FIXEDPNT Kd, Ks;                        // Diffuse and specular reflection coefficients

    public:
 
      // Constructor and destructor:
      IMR_Material()
          { 
          Texture = NULL; 
          Flags.Shiny = IMR_OFF;
          Flags.Reflective = IMR_OFF;
          Flags.Transparent = IMR_OFF;
          Kd = Ks = FIXED_ONE;
          Lightmap = NULL;
          DirectX = NULL;
           };
      ~IMR_Material() { Shutdown(); };
      void Shutdown(void)
          { 
          if (Lightmap) Lightmap->Release(); 
          Lightmap = NULL;
          if (LightmapSurface) LightmapSurface->Release();
          LightmapSurface = NULL;
          DirectX = NULL;
           };

      // Name methods:
      void Set_Name(char *NewName);
      inline char *Get_Name(void) { return Name; };
      inline int Is(char *CmpName) { return !stricmp(Name, CmpName); };
      void Set_TextureName(char *NewName);
      inline char *Get_TextureName(void) { return TextureName; };

      // Texture methods:
      int Set_Texture(IMR_Texture *Tex) { Texture = Tex; return IMR_OK; };
      IMR_Texture *Get_Texture(void) { return Texture; };
      LPDIRECT3DTEXTURE2 Get_TextureInterface(void) 
          {
          if (Texture) 
              return Texture->Get_TextureInterface(); 
          else 
              return NULL; 
           };

      // Lightmap methods:
      int Init_Lightmap(IMR_DirectXInterface *DX);
      LPDIRECT3DTEXTURE2 Get_LightmapInterface(void) { return Lightmap; };
      void *Lock_Lightmap(void)
          {
          if (DirectX && LightmapSurface)
              return DirectX->Lock_Surface(LightmapSurface);
          else 
              return NULL;
           };
      void Unlock_Lightmap(void)
          {
          if (DirectX && LightmapSurface) 
              DirectX->Unlock_Surface(LightmapSurface);
           };
      
      // Appearance methods:
      inline void Set_DiffuseCoef(float K) { Kd = FloatToFixed(K); };
      inline void Set_SpecularCoef(float K) { Ks = FloatToFixed(K); };
      inline float Get_DiffuseCoef(void) { return FixedToFloat(Kd); };
      inline float Get_SpecularCoef(void) { return FixedToFloat(Ks); };
      inline void Set_Shiny(int Val) { Flags.Shiny = Val ? IMR_ON:IMR_OFF; };
      inline void Set_Reflective(int Val) { Flags.Reflective = Val ? IMR_ON:IMR_OFF; };
      inline void Set_Transparent(int Val) { Flags.Transparent = Val ? IMR_ON:IMR_OFF; };
      inline int Get_Shiny(void) { return Flags.Shiny; };
      inline int Get_Reflective(void) { return Flags.Reflective; };
      inline int Get_Transparent(void) { return Flags.Transparent; };
     };

#endif
