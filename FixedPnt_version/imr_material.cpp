/***************************************************************************\
  File: IMR_Material.cpp
  Description: Material module.
  Author: Daniel Hawthorn
  Modified:
  
\***************************************************************************/
#include "IMR_Material.hpp"

/***************************************************************************\
  Sets the name of the material.
\***************************************************************************/
void IMR_Material::Set_Name(char *NewName)
{
if (strlen(NewName) > 8)
    {
    memcpy((void *)TextureName, (void *)NewName, 8);
    Name[8] = '/0';
     }
else
    strcpy(Name, NewName);
 }

/***************************************************************************\
  Sets the name of the texture for this material.
\***************************************************************************/
void IMR_Material::Set_TextureName(char *NewName)
{
if (strlen(NewName) > 8)
    {
    memcpy((void *)TextureName, (void *)NewName, 8);
    Name[8] = '/0';
     }
else
    strcpy(TextureName, NewName);
 }

/***************************************************************************\
  Initializes the lightmap for this polygon.  (i.e. creates the DirectDraw
  surface)
  Returns IMR_OK if succesful, otherwise an error.
\***************************************************************************/
int IMR_Material::Init_Lightmap(IMR_DirectXInterface *DX)
{
// Make sure we have a directX interface:
if (!DX) 
    {
    IMR_SetErrorText("IMR_Material::Init_Lightmap(): NULL DirectX interface specified!");
    return IMRERR_NODATA;
     }

// Save a pointer to the interface:
DirectX = DX;

// Create the surface:
if (!(LightmapSurface = DirectX->Create_Lightmap()))
    return IMRERR_DIRECTX;

// Create a texture interface:
LightmapSurface->QueryInterface(IID_IDirect3DTexture2, (void**)&Lightmap);
if (!Lightmap)
    {
    IMR_SetErrorText("IMR_Material::Init_Lightmap(): QueryInterface failed!");
    return IMRERR_GENERIC;
     }

// And return ok:
return IMR_OK;
 }
