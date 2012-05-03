/***************************************************************************\
  File: IMR_Palette.cpp
  Description: Palette module.
  Author: Daniel Hawthorn
  Modified:
  
\***************************************************************************/
#include "IMR_Palette.hpp"

/***************************************************************************\
  Sets the palette and builds the colormapping tables.
\***************************************************************************/
int IMR_PaletteInfo::SetPalette(void *Pal)
{
// Make sure a palette was specified:
if (!Pal)
    {
    IMR_SetErrorText("IMR_PaletteInfo::SetPalette(): No palette specified!");
    return IMRERR_NOPALETTE;
     }

// Reset everything:
Reset();

// Copy the palette data into our local palette:
memcpy((void *)Palette, Pal, 768);

// Allocate memory for the tables:
if (!(LightTable = new char [8192]) ||
    !(FogTable = new char [8192]))
    {
    IMR_SetErrorText("IMR_PaletteInfo::SetPalette(): Out of memory! (Tabl)");
    return IMRERR_OUTOFMEM;
     }

// Build the tables:
CreateLightTable();
CreateFogTable();    

// And return ok:
return IMR_OK;
 }

/***************************************************************************\
 Creates the lighting fadetable.
\***************************************************************************/
void IMR_PaletteInfo::CreateLightTable(void)
{
struct { float R, G, B; } Start, Mid, End, Curr, Vect;
IMR_PaletteEntry Err;
int Color, Pos;

// Make sure the memory for the table has been allocated:
if (!LightTable) return;

// Setup start and end of fades:
Start.R = 63;
Start.G = 63;
Start.B = 63;
End.R = 0;
End.G = 0;
End.B = 0;

// Loop through each color:
for (Color = 0; Color < 256; Color ++)
    {
    Mid.R = (float)Palette[Color].R;
    Mid.G = (float)Palette[Color].G;
    Mid.B = (float)Palette[Color].B;
    Vect.R = (Mid.R - Start.R) / 9;
    Vect.G = (Mid.G - Start.G) / 9;
    Vect.B = (Mid.B - Start.B) / 9;
    Curr.R = Start.R;
    Curr.G = Start.G;
    Curr.B = Start.B;
    for (Pos = 0; Pos < 9; Pos ++)
        {
        LightTable[(Pos << 8) + Color] = (char)FindPalIndex(Curr.R, Curr.G, Curr.B);
        Curr.R += Vect.R;
        Curr.G += Vect.G;
        Curr.B += Vect.B;
         }
    Vect.R = (End.R - Mid.R) / 24;
    Vect.G = (End.G - Mid.G) / 24;
    Vect.B = (End.B - Mid.B) / 24;
    Curr.R = Mid.R;
    Curr.G = Mid.G;
    Curr.B = Mid.B;
    for (Pos = 0; Pos < 24; Pos ++)
        {
        LightTable[(Pos << 8) + Color] = (char)FindPalIndex(Curr.R, Curr.G, Curr.B);
        Curr.R += Vect.R;
        Curr.G += Vect.G;
        Curr.B += Vect.B;
         }
     }
 }

/***************************************************************************\
 Creates the fog fadetable.
\***************************************************************************/
void IMR_PaletteInfo::CreateFogTable(void)
{
struct { float R, G, B; } Curr, End, Vect;
IMR_PaletteEntry Err;
int Color, Pos;

// Make sure the memory for the table has been allocated:
if (!FogTable) return;

// Setup start and end of the fade:
End.R = IMR_PAL_FOGR;
End.G = IMR_PAL_FOGG;
End.B = IMR_PAL_FOGB;

// Loop through each color:
for (Color = 0; Color < 256; Color ++)
    {
    Curr.R = (float)Palette[Color].R;
    Curr.G = (float)Palette[Color].G;
    Curr.B = (float)Palette[Color].B;
    Vect.R = (End.R - Curr.R) / 32;
    Vect.G = (End.G - Curr.G) / 32;
    Vect.B = (End.B - Curr.B) / 32;
    for (Pos = 0; Pos < 32; Pos ++)
        {
        LightTable[(Pos << 8) + Color] = (char)FindPalIndex(Curr.R, Curr.G, Curr.B);
        Curr.R += Vect.R;
        Curr.G += Vect.G;
        Curr.B += Vect.B;
         }
     }
 }

/***************************************************************************\
  Finds a color in the palette that matches the specified RGB.
  Return the index to the color.
\***************************************************************************/
int IMR_PaletteInfo::FindPalIndex(int R, int G, int B)
{
int BestMatch = 0, BestErr = 256, CurrErr;
int Rd, Gd, Bd;

// Loop through all the colors (skipping 255 because that's reserved for transparent):
for (int Color = 0; Color < 255; Color ++)
    {
    Rd = abs(Palette[Color].R - R);
    Gd = abs(Palette[Color].G - G);
    Bd = abs(Palette[Color].B - B);
    CurrErr = Rd + Gd + Bd;
    if (CurrErr < BestErr)
        {
        BestErr = CurrErr; 
        BestMatch = Color;
         }
     }

// And return the best match:
return BestMatch;
 }

/***************************************************************************\
  Creates a map from one palette to the other.
  Map is a pointer to an array of 256 ints.
  Returns IMR_OK.
\***************************************************************************/
int IMR_PaletteInfo::CreateMap(IMR_PaletteInfo &OldPal, int *Map)
{
// Just loop through each of the colors in the old palette and find the best
// match in the new palette:
for (int co = 0; co < 255; co ++)
    Map[co] = FindPalIndex(OldPal.Palette[co].R, 
                           OldPal.Palette[co].G, 
                           OldPal.Palette[co].B);

// And return ok:
return IMR_OK;
 }

