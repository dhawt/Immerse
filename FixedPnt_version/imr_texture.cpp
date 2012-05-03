/***************************************************************************\
  File: IMR_Texture.cpp
  Description: Texture primitive module.
  Author: Daniel Hawthorn
  Modified:
  
\***************************************************************************/
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
  Loads the texture from the specified PCX file and gives it the specified
  name.
  Returns IMR_OK if successful, an error otherwise.
\***************************************************************************/
int IMR_Texture::Load_PCX(IMR_DirectXInterface &DX, char *Filename, char *TextureName)
{
char Byte, CurrErr, HeaderByte, ColorByte, Buffer[10];
IMR_PaletteEntry Pal[256];
int index, err, Pos, 
    PcxFile, 
    BestErr, BestMatch, 
    TransparentRemap, 
    Px, Py, RunLength;
char *Data;

// Reset the texture:
Reset();

// Open the file:
PcxFile = open(Filename, O_RDONLY | O_BINARY);

// Make sure the file is ok:
if (!PcxFile)
    {
    IMR_SetErrorText("IMR_Texture::Load_PCX(): File %s not found!", Filename);
    return IMRERR_BADFILE;
     }

// Eat some stuff:
read(PcxFile, Buffer, 8);

// Find the width and the height of the image:
Width = 0; read(PcxFile, &Width, 2);
Height = 0; read(PcxFile, &Height, 2);
Width ++; 
Height ++;
Size = Width * Height;

// Go to the palette data:
lseek(PcxFile, filelength(PcxFile) - 768, SEEK_SET);

// Read and set the color palette:
read(PcxFile, (void *)Pal, 768);
Palette.SetPalette((void *)Pal);

// Create the texture:
DDSurface = DX.Create_8BitTexture(Width, Height, (void *)Pal);
if (!DDSurface) return IMRERR_DIRECTX;
DDSurface->QueryInterface(IID_IDirect3DTexture2, (void**)&D3DTexture);
if (!D3DTexture)
    {
    IMR_SetErrorText("IMR_Texture::Load_PCX(): QueryInterface failed!");
    return IMRERR_GENERIC;
     }

// Remap color 255:
BestErr = 256;
BestMatch = 0;
for (index = 0; index < 255; index ++)
    {
    // Calculate error delta:
    CurrErr = (Pal[255].R - Pal[index].R) +
              (Pal[255].G - Pal[index].G) +
              (Pal[255].B - Pal[index].B);
    
    // If the current error delta is less than the previous best's, 
    // make this the best match:
    if (CurrErr < BestErr)
        {
        BestErr = CurrErr;
        BestMatch = index;
         }
     }

// Set the transparent remap color:     
TransparentRemap = BestMatch;

// Go to the image data:
lseek(PcxFile, 128, SEEK_SET);

// Reset position:
Px = Py = 0;
Pos = 0;

// Lock the texture:
Data = (char *)DX.Lock_Surface(DDSurface);
if (!Data)
    {
    IMR_SetErrorText("IMR_Texture::Load_PCX(): Lock failed!");
    return IMRERR_NODATA;
     }

// Read image:
while (Py < Height)
    {
    // Read header/color byte:
    read(PcxFile, &HeaderByte, 1);
    
    // Check if we have encountered a run:
    if (HeaderByte >= 193)
        {   
        // Read color byte:
        read(PcxFile, &ColorByte, 1);
            
        // If the color is transparent, remap it:
        if (ColorByte == 255)
            ColorByte = TransparentRemap;
       
        // Calculate run length:
        RunLength = HeaderByte - 193;
       
        // Write the run:
        for (index = 0; index < RunLength + 1; index ++)
            {
            // Increment x position:
            Px ++;
            if (Px >= (Width - 1)) { Py ++; Px = 0; }
            
            // Set the pixel:
            Data[Pos ++] = ColorByte;
             }
         }
    
    // If we have just a single pixel, write it:
    else
        {
        // Set the color:
        ColorByte = HeaderByte;

        // If the color is transparent, remap it:
        if (ColorByte == 255)
            ColorByte = TransparentRemap;

        // Increment x position:
        Px ++;
        if (Px >= Width) { Py ++; Px = 0; }
        
        // Set the pixel:
        Data[Pos ++] = ColorByte;
         }       
     }

// Unlock the surface:
DX.Unlock_Surface(DDSurface);

// Close the file:
close(PcxFile);

// Set the name of the texture:
Set_Name(TextureName);

// And return ok:
return IMR_OK;
 }
