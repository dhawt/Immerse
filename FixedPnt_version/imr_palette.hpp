/***************************************************************************\
  File: IMR_Palette.hpp
  Description: Header for the palette stuff.
  Author: Daniel Hawthorn
  Modified:
  
\***************************************************************************/
#ifndef __IMR_PALETTE__HPP
#define __IMR_PALETTE__HPP

// Include headers:
#include <stdlib.h>
#include <math.h>
#include "IMR_Err.hpp"
#include "IMR_RetVals.hpp"

// Types:
typedef struct { char R, G, B; } IMR_PaletteEntry;

// Constants and macros:
#define IMR_PAL_FOGR            49
#define IMR_PAL_FOGG            49
#define IMR_PAL_FOGB            54
#define IMR_PAL_COLORDEPTH       63

// Palette information class:
class IMR_PaletteInfo
    {
    protected:
      char *LightTable,
            *FogTable;
      IMR_PaletteEntry Palette[256];
      
      void CreateLightTable(void);
      void CreateFogTable(void);

    public:
      IMR_PaletteInfo() { LightTable = FogTable = 0; };
      ~IMR_PaletteInfo() { Reset(); };
      inline void Reset(void) { delete [] LightTable; delete [] FogTable; };

      int SetPalette(void *Palette);
      void *GetPalette(void) { return Palette; };
      IMR_PaletteEntry *GetData(void) { return Palette; };
      int FindPalIndex(int R, int G, int B);
      int CreateMap(IMR_PaletteInfo &OldPal, int *Map);
      inline int GetFogmappedColor();
      inline int GetLightmappedColor();
     };

#endif
