/****************************************************************\
 
 iMMERSE Engine
 (C) 1999 No Tears Shed Software
 All rights reserved
 
 Filename: IMR_RendCore.hpp
 Description: Renderer module - interfaces with Direct3D for 
              rasterization and blits.
 
\****************************************************************/
#include "IMR_RendCore.hpp"

/***************************************************************************\
  Initializes the renderer.  
  Returns IMR_OK if successful, otherwise an error.
\***************************************************************************/
int IMR_Renderer::Init(int MaxTextures, int MaxLightmaps)
{
// Make sure we've shutdown:
Shutdown();

// Now make sure the math tables have been buit:
IMR_BuildTables();

// Allocate space for our textures and lightmaps:
Textures.Init(MaxTextures);
Lightmaps.Init(MaxLightmaps);

// Return ok:
return IMR_OK;
 }

/***************************************************************************\
  Shuts down the renderer.  
  Returns IMR_OK if successful, otherwise an error.
\***************************************************************************/
int IMR_Renderer::Shutdown(void)
{
// Destroy buffers:
if (Target.Surface) DirectX.Destroy_Surface(Target.Surface);
Target.Surface = NULL;

// Shutdown DirectX:
DirectX.Shutdown();

// Shut down our textures:
Textures.Reset();

// Return ok:
return IMR_OK;
 }

/***************************************************************************\
  Initializes a rasterization batch.
  Returns IMR_OK if successful, otherwise an error.
\***************************************************************************/
int IMR_Renderer::Begin_Raster_Batch(IMR_Camera &Cam)
{
int err;

// Save a pointer to the camera and rend buffer:
Camera = &Cam;

// Start the scene:
Target_ClearBuffers();          // <--- Needed???
Get_DeviceInterface()->BeginScene();

Get_DeviceInterface()->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
Get_DeviceInterface()->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
Get_DeviceInterface()->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
if (DrawOpts.FilteringEnabled)
    {
    Get_DeviceInterface()->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTFN_LINEAR);
    Get_DeviceInterface()->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTFG_LINEAR);
     }
else
    {
    Get_DeviceInterface()->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTFN_POINT);
    Get_DeviceInterface()->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTFG_POINT);
     }

// Setup mipmapping:
if (DrawOpts.MipMappingEnabled)
    Get_DeviceInterface()->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTFP_LINEAR);
else
    Get_DeviceInterface()->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTFP_NONE);

// Setup stuff:
Flags.InRasterBatch = 1;
Flags.DrawMode = IMR_RENDERER_MODE_INIT;
PolysDrawn = 0;
LastTexture = CurrTexture = NULL;

// And return ok:
return IMR_OK;
 }

/***************************************************************************\
  Draws the specified poly with an unlit texture.
  Returns IMR_OK.
\***************************************************************************/
int IMR_Renderer::Draw_Textured_Polygon(IMR_Polygon &Poly)
{
struct
    {
    float x, y, z;
    float w;
    float u, v;
     } Verts[5];
if (!Camera)
    {
    IMR_LogMsg(__LINE__, __FILE__, "No camera installed!");
    return IMRERR_GENERIC;
     };
float ZNormalize = 1 / Camera->Lens_Get_Far();

// Make sure we are in a raster batch:
if (!Flags.InRasterBatch) 
    {
    IMR_LogMsg(__LINE__, __FILE__, "IMR_Renderer: Not in raster batch!");
    return IMRERR_NONFATAL_NOTINBATCH;
     }

// Search for the texture for this material if it hasn't been done yet:
if (!Poly.Material.Get_Texture().HasHost())
    {
    IMR_TexRef TexRef;
    TexRef = Texture_GetRef(Poly.Material.Get_TextureName());
    if (!TexRef.Host.Ptr)
        {
        IMR_LogMsg(__LINE__, __FILE__, "IMR_Renderer: Couldn't find texture %s!", Poly.Material.Get_TextureName());
        return IMR_OK;
         }
    Poly.Material.Set_Texture(TexRef);
     }

// Setup the device (if we have just changed states):
if (Flags.DrawMode != IMR_RENDERER_MODE_TEXTURED)
    {
    Get_DeviceInterface()->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    Get_DeviceInterface()->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT);
    Get_DeviceInterface()->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
    Get_DeviceInterface()->SetRenderState(D3DRENDERSTATE_ZENABLE, D3DZB_TRUE);
    Get_DeviceInterface()->SetRenderState(D3DRENDERSTATE_SHADEMODE, D3DSHADE_GOURAUD);
    Get_DeviceInterface()->SetRenderState(D3DRENDERSTATE_FILLMODE, D3DFILL_SOLID);
    Flags.DrawMode = IMR_RENDERER_MODE_TEXTURED;
     }

// Set texture:
IMR_Texture *Tex = (IMR_Texture *)Poly.Material.Get_Texture().Host.Ptr;
CurrTexture = Tex->Get_TextureInterface();
if (LastTexture != CurrTexture)
    Get_DeviceInterface()->SetTexture(0, CurrTexture);
LastTexture = CurrTexture;

if (Poly.Num_Verts_Proj == 3)
    {
    Verts[0].x = Poly.Vtx_Projected[0].pX;
    Verts[0].y = Poly.Vtx_Projected[0].pY;
    Verts[0].z = Poly.Vtx_Projected[0].pZ * ZNormalize;
    Verts[0].w = Poly.Vtx_Projected[0].pW;
    Verts[0].u = Poly.Vtx_Projected[0].pU;
    Verts[0].v = Poly.Vtx_Projected[0].pV;
    Verts[1].x = Poly.Vtx_Projected[1].pX;
    Verts[1].y = Poly.Vtx_Projected[1].pY;
    Verts[1].z = Poly.Vtx_Projected[1].pZ * ZNormalize;
    Verts[1].w = Poly.Vtx_Projected[1].pW;
    Verts[1].u = Poly.Vtx_Projected[1].pU;
    Verts[1].v = Poly.Vtx_Projected[1].pV;
    Verts[2].x = Poly.Vtx_Projected[2].pX;
    Verts[2].y = Poly.Vtx_Projected[2].pY;
    Verts[2].z = Poly.Vtx_Projected[2].Z * ZNormalize;
    Verts[2].w = Poly.Vtx_Projected[2].pW;
    Verts[2].u = Poly.Vtx_Projected[2].pU;
    Verts[2].v = Poly.Vtx_Projected[2].pV;
    Get_DeviceInterface()->DrawPrimitive(D3DPT_TRIANGLELIST,  
                                         D3DFVF_XYZRHW |
                                         D3DFVF_TEX1,
                                         Verts, 3, 
                                         D3DDP_DONOTCLIP | 
                                         D3DDP_DONOTLIGHT | 
                                         D3DDP_DONOTUPDATEEXTENTS);
     }
if (Poly.Num_Verts_Proj == 4)
    {
    Verts[0].x = Poly.Vtx_Projected[0].pX;
    Verts[0].y = Poly.Vtx_Projected[0].pY;
    Verts[0].z = Poly.Vtx_Projected[0].pZ * ZNormalize;
    Verts[0].w = Poly.Vtx_Projected[0].pW;
    Verts[0].u = Poly.Vtx_Projected[0].pU;
    Verts[0].v = Poly.Vtx_Projected[0].pV;
    Verts[1].x = Poly.Vtx_Projected[3].pX;
    Verts[1].y = Poly.Vtx_Projected[3].pY;
    Verts[1].z = Poly.Vtx_Projected[3].pZ * ZNormalize;
    Verts[1].w = Poly.Vtx_Projected[3].pW;
    Verts[1].u = Poly.Vtx_Projected[3].pU;
    Verts[1].v = Poly.Vtx_Projected[3].pV;
    Verts[2].x = Poly.Vtx_Projected[1].pX;
    Verts[2].y = Poly.Vtx_Projected[1].pY;
    Verts[2].z = Poly.Vtx_Projected[1].pZ * ZNormalize;
    Verts[2].w = Poly.Vtx_Projected[1].pW;
    Verts[2].u = Poly.Vtx_Projected[1].pU;
    Verts[2].v = Poly.Vtx_Projected[1].pV;
    Verts[3].x = Poly.Vtx_Projected[2].pX;
    Verts[3].y = Poly.Vtx_Projected[2].pY;
    Verts[3].z = Poly.Vtx_Projected[2].pZ * ZNormalize;
    Verts[3].w = Poly.Vtx_Projected[2].pW;
    Verts[3].u = Poly.Vtx_Projected[2].pU;
    Verts[3].v = Poly.Vtx_Projected[2].pV;
    Get_DeviceInterface()->DrawPrimitive(D3DPT_TRIANGLESTRIP,  
                                         D3DFVF_XYZRHW | 
                                         D3DFVF_TEX1,
                                         Verts, 4, 
                                         D3DDP_DONOTCLIP | 
                                         D3DDP_DONOTLIGHT | 
                                         D3DDP_DONOTUPDATEEXTENTS);
     }
if (Poly.Num_Verts_Proj == 5)
    {
    Verts[0].x = Poly.Vtx_Projected[0].pX;
    Verts[0].y = Poly.Vtx_Projected[0].pY;
    Verts[0].z = Poly.Vtx_Projected[0].pZ * ZNormalize;
    Verts[0].w = Poly.Vtx_Projected[0].pW;
    Verts[0].u = Poly.Vtx_Projected[0].pU;
    Verts[0].v = Poly.Vtx_Projected[0].pV;
    Verts[1].x = Poly.Vtx_Projected[1].pX;
    Verts[1].y = Poly.Vtx_Projected[1].pY;
    Verts[1].z = Poly.Vtx_Projected[1].pZ * ZNormalize;
    Verts[1].w = Poly.Vtx_Projected[1].pW;
    Verts[1].u = Poly.Vtx_Projected[1].pU;
    Verts[1].v = Poly.Vtx_Projected[1].pV;
    Verts[2].x = Poly.Vtx_Projected[4].pX;
    Verts[2].y = Poly.Vtx_Projected[4].pY;
    Verts[2].z = Poly.Vtx_Projected[4].pZ * ZNormalize;
    Verts[2].w = Poly.Vtx_Projected[4].pW;
    Verts[2].u = Poly.Vtx_Projected[4].pU;
    Verts[2].v = Poly.Vtx_Projected[4].pV;
    Verts[3].x = Poly.Vtx_Projected[2].pX;
    Verts[3].y = Poly.Vtx_Projected[2].pY;
    Verts[3].z = Poly.Vtx_Projected[2].pZ * ZNormalize;
    Verts[3].w = Poly.Vtx_Projected[2].pW;
    Verts[3].u = Poly.Vtx_Projected[2].pU;
    Verts[3].v = Poly.Vtx_Projected[2].pV;
    Verts[4].x = Poly.Vtx_Projected[3].pX;
    Verts[4].y = Poly.Vtx_Projected[3].pY;
    Verts[4].z = Poly.Vtx_Projected[3].pZ * ZNormalize;
    Verts[4].w = Poly.Vtx_Projected[3].pW;
    Verts[4].u = Poly.Vtx_Projected[3].pU;
    Verts[4].v = Poly.Vtx_Projected[3].pV;
    Get_DeviceInterface()->DrawPrimitive(D3DPT_TRIANGLESTRIP,
                                         D3DFVF_XYZRHW |
                                         D3DFVF_TEX1,
                                         Verts, 5, 
                                         D3DDP_DONOTCLIP | 
                                         D3DDP_DONOTLIGHT | 
                                         D3DDP_DONOTUPDATEEXTENTS);
     }

// One more poly has been drawn:
++ PolysDrawn;

// Return ok:
return IMR_OK;
 }

/***************************************************************************\
  Draws the specified poly with a lit texture.
  Returns IMR_OK.
\***************************************************************************/
int IMR_Renderer::Draw_TexturedLit_Polygon(IMR_Polygon &Poly)
{
struct
    {
    float x, y, z;
    float w;
    long Color;
    float u, v;
     } Verts[5];
if (!Camera)
    {
    IMR_LogMsg(__LINE__, __FILE__, "No camera installed!");
    return IMRERR_GENERIC;
     };
float ZNormalize = 1 / Camera->Lens_Get_Far();

// Make sure we are in a raster batch:
if (!Flags.InRasterBatch) 
    {
    IMR_LogMsg(__LINE__, __FILE__, "IMR_Renderer::Draw_Wireframe_Polygon(): Not in raster batch!");
    return IMRERR_NONFATAL_NOTINBATCH;
     }

// Search for the texture for this material if it hasn't been done yet:
if (!Poly.Material.Get_Texture().HasHost())
    {
    IMR_TexRef TexRef;
    TexRef = Texture_GetRef(Poly.Material.Get_TextureName());
    if (!TexRef.Host.Ptr)
        {
        IMR_LogMsg(__LINE__, __FILE__, "IMR_Renderer: Couldn't find texture %s!", Poly.Material.Get_TextureName());
        return IMR_OK;
         }
    Poly.Material.Set_Texture(TexRef);
     }

// Setup the device (if we have just changed states):
if (Flags.DrawMode != IMR_RENDERER_MODE_TEXTUREDLIT)
    {
    Get_DeviceInterface()->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    Get_DeviceInterface()->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
    Get_DeviceInterface()->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
    Get_DeviceInterface()->SetRenderState(D3DRENDERSTATE_ZENABLE, D3DZB_TRUE);
    Get_DeviceInterface()->SetRenderState(D3DRENDERSTATE_SHADEMODE, D3DSHADE_GOURAUD);
    Get_DeviceInterface()->SetRenderState(D3DRENDERSTATE_FILLMODE, D3DFILL_SOLID);
    Flags.DrawMode = IMR_RENDERER_MODE_TEXTUREDLIT;
     }

// Set texture:
IMR_Texture *Tex = (IMR_Texture *)Poly.Material.Get_Texture().Host.Ptr;
CurrTexture = Tex->Get_TextureInterface();
if (LastTexture != CurrTexture)
    Get_DeviceInterface()->SetTexture(0, CurrTexture);
LastTexture = CurrTexture;

// Check how we should handle ZBuffering:
// !!!! MaxZ is handled in projection !!!!
if (Poly.Flags.MinZ || Poly.Flags.MaxZ) Get_DeviceInterface()->SetRenderState(D3DRENDERSTATE_ZENABLE, D3DZB_FALSE);
else Get_DeviceInterface()->SetRenderState(D3DRENDERSTATE_ZENABLE, D3DZB_TRUE);

if (Poly.Num_Verts_Proj == 3)
    {
    Verts[0].x = Poly.Vtx_Projected[0].pX;
    Verts[0].y = Poly.Vtx_Projected[0].pY;
    Verts[0].z = Poly.Vtx_Projected[0].pZ * ZNormalize;
    Verts[0].w = Poly.Vtx_Projected[0].pW;
    Verts[0].Color = 0xff000000 | (Poly.Vtx_Projected[0].pR << 16) |
                                  (Poly.Vtx_Projected[0].pG << 8) |
                                  (Poly.Vtx_Projected[0].pB);
    Verts[0].u = Poly.Vtx_Projected[0].pU;
    Verts[0].v = Poly.Vtx_Projected[0].pV;
    Verts[1].x = Poly.Vtx_Projected[1].pX;
    Verts[1].y = Poly.Vtx_Projected[1].pY;
    Verts[1].z = Poly.Vtx_Projected[1].pZ * ZNormalize;
    Verts[1].w = Poly.Vtx_Projected[1].pW;
    Verts[1].Color = 0xff000000 | (Poly.Vtx_Projected[1].pR << 16) |
                                  (Poly.Vtx_Projected[1].pG << 8) |
                                  (Poly.Vtx_Projected[1].pB);
    Verts[1].u = Poly.Vtx_Projected[1].pU;
    Verts[1].v = Poly.Vtx_Projected[1].pV;
    Verts[2].x = Poly.Vtx_Projected[2].pX;
    Verts[2].y = Poly.Vtx_Projected[2].pY;
    Verts[2].z = Poly.Vtx_Projected[2].Z * ZNormalize;
    Verts[2].w = Poly.Vtx_Projected[2].pW;
    Verts[2].Color = 0xff000000 | (Poly.Vtx_Projected[2].pR << 16) |
                                  (Poly.Vtx_Projected[2].pG << 8) |
                                  (Poly.Vtx_Projected[2].pB);
    Verts[2].u = Poly.Vtx_Projected[2].pU;
    Verts[2].v = Poly.Vtx_Projected[2].pV;
    Get_DeviceInterface()->DrawPrimitive(D3DPT_TRIANGLELIST,
                                         D3DFVF_XYZRHW |
                                         D3DFVF_TEX1 | 
                                         D3DFVF_DIFFUSE,
                                         Verts, 3, 
                                         D3DDP_DONOTCLIP | 
                                         D3DDP_DONOTLIGHT | 
                                         D3DDP_DONOTUPDATEEXTENTS);
     }
if (Poly.Num_Verts_Proj == 4)
    {
    Verts[0].x = Poly.Vtx_Projected[0].pX;
    Verts[0].y = Poly.Vtx_Projected[0].pY;
    Verts[0].z = Poly.Vtx_Projected[0].pZ * ZNormalize;
    Verts[0].w = Poly.Vtx_Projected[0].pW;
    Verts[0].Color = 0xff000000 | (Poly.Vtx_Projected[0].pR << 16) |
                                  (Poly.Vtx_Projected[0].pG << 8) |
                                  (Poly.Vtx_Projected[0].pB);
    Verts[0].u = Poly.Vtx_Projected[0].pU;
    Verts[0].v = Poly.Vtx_Projected[0].pV;
    Verts[1].x = Poly.Vtx_Projected[3].pX;
    Verts[1].y = Poly.Vtx_Projected[3].pY;
    Verts[1].z = Poly.Vtx_Projected[3].pZ * ZNormalize;
    Verts[1].w = Poly.Vtx_Projected[3].pW;
    Verts[1].Color = 0xff000000 | (Poly.Vtx_Projected[3].pR << 16) |
                                  (Poly.Vtx_Projected[3].pG << 8) |
                                  (Poly.Vtx_Projected[3].pB);
    Verts[1].u = Poly.Vtx_Projected[3].pU;
    Verts[1].v = Poly.Vtx_Projected[3].pV;
    Verts[2].x = Poly.Vtx_Projected[1].pX;
    Verts[2].y = Poly.Vtx_Projected[1].pY;
    Verts[2].z = Poly.Vtx_Projected[1].pZ * ZNormalize;
    Verts[2].w = Poly.Vtx_Projected[1].pW;
    Verts[2].Color = 0xff000000 | (Poly.Vtx_Projected[1].pR << 16) |
                                  (Poly.Vtx_Projected[1].pG << 8) |
                                  (Poly.Vtx_Projected[1].pB);
    Verts[2].u = Poly.Vtx_Projected[1].pU;
    Verts[2].v = Poly.Vtx_Projected[1].pV;
    Verts[3].x = Poly.Vtx_Projected[2].pX;
    Verts[3].y = Poly.Vtx_Projected[2].pY;
    Verts[3].z = Poly.Vtx_Projected[2].pZ * ZNormalize;
    Verts[3].w = Poly.Vtx_Projected[2].pW;
    Verts[3].Color = 0xff000000 | (Poly.Vtx_Projected[2].pR << 16) |
                                  (Poly.Vtx_Projected[2].pG << 8) |
                                  (Poly.Vtx_Projected[2].pB);
    Verts[3].u = Poly.Vtx_Projected[2].pU;
    Verts[3].v = Poly.Vtx_Projected[2].pV;
    Get_DeviceInterface()->DrawPrimitive(D3DPT_TRIANGLESTRIP,  
                                         D3DFVF_XYZRHW |
                                         D3DFVF_TEX1 | 
                                         D3DFVF_DIFFUSE,
                                         Verts, 4, 
                                         D3DDP_DONOTCLIP | 
                                         D3DDP_DONOTLIGHT | 
                                         D3DDP_DONOTUPDATEEXTENTS);
     }
if (Poly.Num_Verts_Proj == 5)
    {
    Verts[0].x = Poly.Vtx_Projected[0].pX;
    Verts[0].y = Poly.Vtx_Projected[0].pY;
    Verts[0].z = Poly.Vtx_Projected[0].pZ * ZNormalize;
    Verts[0].w = Poly.Vtx_Projected[0].pW;
    Verts[0].Color = 0xff000000 | (Poly.Vtx_Projected[0].pR << 16) |
                                  (Poly.Vtx_Projected[0].pG << 8) |
                                  (Poly.Vtx_Projected[0].pB);
    Verts[0].u = Poly.Vtx_Projected[0].pU;
    Verts[0].v = Poly.Vtx_Projected[0].pV;
    Verts[1].x = Poly.Vtx_Projected[1].pX;
    Verts[1].y = Poly.Vtx_Projected[1].pY;
    Verts[1].z = Poly.Vtx_Projected[1].pZ * ZNormalize;
    Verts[1].w = Poly.Vtx_Projected[1].pW;
    Verts[1].Color = 0xff000000 | (Poly.Vtx_Projected[1].pR << 16) |
                                  (Poly.Vtx_Projected[1].pG << 8) |
                                  (Poly.Vtx_Projected[1].pB);
    Verts[1].u = Poly.Vtx_Projected[1].pU;
    Verts[1].v = Poly.Vtx_Projected[1].pV;
    Verts[2].x = Poly.Vtx_Projected[4].pX;
    Verts[2].y = Poly.Vtx_Projected[4].pY;
    Verts[2].z = Poly.Vtx_Projected[4].pZ * ZNormalize;
    Verts[2].w = Poly.Vtx_Projected[4].pW;
    Verts[2].Color = 0xff000000 | (Poly.Vtx_Projected[4].pR << 16) |
                                  (Poly.Vtx_Projected[4].pG << 8) |
                                  (Poly.Vtx_Projected[4].pB);
    Verts[2].u = Poly.Vtx_Projected[4].pU;
    Verts[2].v = Poly.Vtx_Projected[4].pV;
    Verts[3].x = Poly.Vtx_Projected[2].pX;
    Verts[3].y = Poly.Vtx_Projected[2].pY;
    Verts[3].z = Poly.Vtx_Projected[2].pZ * ZNormalize;
    Verts[3].w = Poly.Vtx_Projected[2].pW;
    Verts[3].Color = 0xff000000 | (Poly.Vtx_Projected[2].pR << 16) |
                                  (Poly.Vtx_Projected[2].pG << 8) |
                                  (Poly.Vtx_Projected[2].pB);
    Verts[3].u = Poly.Vtx_Projected[2].pU;
    Verts[3].v = Poly.Vtx_Projected[2].pV;
    Verts[4].x = Poly.Vtx_Projected[3].pX;
    Verts[4].y = Poly.Vtx_Projected[3].pY;
    Verts[4].z = Poly.Vtx_Projected[3].pZ * ZNormalize;
    Verts[4].w = Poly.Vtx_Projected[3].pW;
    Verts[4].Color = 0xff000000 | (Poly.Vtx_Projected[3].pR << 16) |
                                  (Poly.Vtx_Projected[3].pG << 8) |
                                  (Poly.Vtx_Projected[3].pB);
    Verts[4].u = Poly.Vtx_Projected[3].pU;
    Verts[4].v = Poly.Vtx_Projected[3].pV;
    Get_DeviceInterface()->DrawPrimitive(D3DPT_TRIANGLESTRIP,  
                                         D3DFVF_XYZRHW |
                                         D3DFVF_TEX1 | 
                                         D3DFVF_DIFFUSE,
                                         Verts, 5, 
                                         D3DDP_DONOTCLIP | 
                                         D3DDP_DONOTLIGHT | 
                                         D3DDP_DONOTUPDATEEXTENTS);
     }

// One more poly has been drawn:
++ PolysDrawn;

// Return ok:
return IMR_OK;
 }

/***************************************************************************\
  Draws the specified poly list with a lit texture.
  Returns IMR_OK.
\***************************************************************************/
struct Vert
    {
    float x, y, z;
    float w;
    long Color;
    float u, v;
     };
Vert D3DVerts[IMR_RENDERER_BATCHMAXVERTS];
int logged = 0;
int IMR_Renderer::Draw_PolyBatch(IMR_Polygon **PList, int NumPolys)
{
float ZNormalize = 1 / Camera->Lens_Get_Far();
int NumVerts = 0, CurrVert, UseVert, MaxVertsHit = 0;

// First do some checking:
if (!PList || NumPolys <= 0)
    {
    IMR_LogMsg(__LINE__, __FILE__, "Degenerate polygon list passed!");
    return IMRERR_NODATA;
     };
if (!Camera)
    {
    IMR_LogMsg(__LINE__, __FILE__, "No camera installed!");
    return IMRERR_GENERIC;
     };
if (!Flags.InRasterBatch) 
    {
    IMR_LogMsg(__LINE__, __FILE__, "Not in raster batch!");
    return IMRERR_NONFATAL_NOTINBATCH;
     }

// Setup the device (if we have just changed states):
if (Flags.DrawMode != IMR_RENDERER_MODE_TEXTUREDLIT)
    {
    Get_DeviceInterface()->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    Get_DeviceInterface()->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
    Get_DeviceInterface()->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
    Get_DeviceInterface()->SetRenderState(D3DRENDERSTATE_ZENABLE, D3DZB_TRUE);
    Get_DeviceInterface()->SetRenderState(D3DRENDERSTATE_SHADEMODE, D3DSHADE_GOURAUD);
    Get_DeviceInterface()->SetRenderState(D3DRENDERSTATE_FILLMODE, D3DFILL_SOLID);
    Flags.DrawMode = IMR_RENDERER_MODE_TEXTUREDLIT;
     }

// Now do render loop:
LastTexture = CurrTexture = NULL;
for (int CurrPoly = 0; CurrPoly < NumPolys || MaxVertsHit; CurrPoly ++)
    {
    // Search for the texture for this material if it hasn't been done yet:
    if (!PList[CurrPoly]->Material.Get_Texture().HasHost())
        {
        IMR_TexRef TexRef;
        TexRef = Texture_GetRef(PList[CurrPoly]->Material.Get_TextureName());
        if (!TexRef.Host.Ptr)
            {
            IMR_LogMsg(__LINE__, __FILE__, "IMR_Renderer: Couldn't find texture %s!", PList[CurrPoly]->Material.Get_TextureName());
            return IMR_OK;
             }
        PList[CurrPoly]->Material.Set_Texture(TexRef);
         }

    // Get the texture of this polygon:
    IMR_Texture *Tex = (IMR_Texture *)PList[CurrPoly]->Material.Get_Texture().Host.Ptr;
    LastTexture = CurrTexture;
    CurrTexture = Tex->Get_TextureInterface();
    
    // Check how we should handle ZBuffering:
    // !!!! MaxZ is handled in projection !!!!
    if (PList[CurrPoly]->Flags.MinZ) Get_DeviceInterface()->SetRenderState(D3DRENDERSTATE_ZENABLE, D3DZB_FALSE);
    else Get_DeviceInterface()->SetRenderState(D3DRENDERSTATE_ZENABLE, D3DZB_TRUE);
    
    // If this texture is the same as the previous (or the previous is null),
    // add this polygon to the list:
    if (CurrTexture == LastTexture)
        {
        // One more poly has been drawn:
        ++ PolysDrawn;

        if (PList[CurrPoly]->Num_Verts_Proj == 3)
            {
            // Convert to Direct3D vertex:
            for (CurrVert = 0; CurrVert < 3; CurrVert++)
                {
                D3DVerts[NumVerts].x = PList[CurrPoly]->Vtx_Projected[CurrVert].pX;
                D3DVerts[NumVerts].y = PList[CurrPoly]->Vtx_Projected[CurrVert].pY;
                D3DVerts[NumVerts].z = PList[CurrPoly]->Vtx_Projected[CurrVert].pZ * ZNormalize;
                D3DVerts[NumVerts].w = PList[CurrPoly]->Vtx_Projected[CurrVert].pW;
                D3DVerts[NumVerts].Color = 0xff000000 | (PList[CurrPoly]->Vtx_Projected[CurrVert].pR << 16) |
                                                        (PList[CurrPoly]->Vtx_Projected[CurrVert].pG << 8) |
                                                        (PList[CurrPoly]->Vtx_Projected[CurrVert].pB);
                D3DVerts[NumVerts].u = PList[CurrPoly]->Vtx_Projected[CurrVert].pU;
                D3DVerts[NumVerts].v = PList[CurrPoly]->Vtx_Projected[CurrVert].pV;
                
                // One more vertex:
                NumVerts ++;
                
                // If we are out of vertices, flag:
                if (NumVerts > IMR_RENDERER_BATCHMAXVERTS) 
                    {
                    #ifdef IMR_DEBUG
                        IMR_LogMsg(__LINE__, __FILE__, "BackMaxVerts hit!");
                    #endif 
                    MaxVertsHit = 1; break; 
                     };
                 }
             }
        if (PList[CurrPoly]->Num_Verts_Proj == 4)
            {
            // Convert to Direct3D vertex:
            for (CurrVert = 0; CurrVert < 6; CurrVert++)
                {
                if (CurrVert == 0) UseVert = 0;
                if (CurrVert == 1) UseVert = 1;
                if (CurrVert == 2) UseVert = 3;
                if (CurrVert == 3) UseVert = 1;
                if (CurrVert == 4) UseVert = 2;
                if (CurrVert == 5) UseVert = 3;
                D3DVerts[NumVerts].x = PList[CurrPoly]->Vtx_Projected[UseVert].pX;
                D3DVerts[NumVerts].y = PList[CurrPoly]->Vtx_Projected[UseVert].pY;
                D3DVerts[NumVerts].z = PList[CurrPoly]->Vtx_Projected[UseVert].pZ * ZNormalize;
                D3DVerts[NumVerts].w = PList[CurrPoly]->Vtx_Projected[UseVert].pW;
                D3DVerts[NumVerts].Color = 0xff000000 | (PList[CurrPoly]->Vtx_Projected[UseVert].pR << 16) |
                                                        (PList[CurrPoly]->Vtx_Projected[UseVert].pG << 8) |
                                                        (PList[CurrPoly]->Vtx_Projected[UseVert].pB);
                D3DVerts[NumVerts].u = PList[CurrPoly]->Vtx_Projected[UseVert].pU;
                D3DVerts[NumVerts].v = PList[CurrPoly]->Vtx_Projected[UseVert].pV;
                NumVerts ++;
                if (NumVerts > IMR_RENDERER_BATCHMAXVERTS) 
                    {
                    #ifdef IMR_DEBUG
                        IMR_LogMsg(__LINE__, __FILE__, "BackMaxVerts hit!");
                    #endif 
                    MaxVertsHit = 1; break; 
                     };
                 }
             }
        if (PList[CurrPoly]->Num_Verts_Proj == 5)
            {
            // Convert to Direct3D vertex:
            for (CurrVert = 0; CurrVert < 9; CurrVert++)
                {
                if (CurrVert == 0) UseVert = 0;
                if (CurrVert == 1) UseVert = 1;
                if (CurrVert == 2) UseVert = 4;
                if (CurrVert == 3) UseVert = 1;
                if (CurrVert == 4) UseVert = 2;
                if (CurrVert == 5) UseVert = 4;
                if (CurrVert == 6) UseVert = 2;
                if (CurrVert == 7) UseVert = 3;
                if (CurrVert == 8) UseVert = 4;
                D3DVerts[NumVerts].x = PList[CurrPoly]->Vtx_Projected[UseVert].pX;
                D3DVerts[NumVerts].y = PList[CurrPoly]->Vtx_Projected[UseVert].pY;
                D3DVerts[NumVerts].z = PList[CurrPoly]->Vtx_Projected[UseVert].pZ * ZNormalize;
                D3DVerts[NumVerts].w = PList[CurrPoly]->Vtx_Projected[UseVert].pW;
                D3DVerts[NumVerts].Color = 0xff000000 | (PList[CurrPoly]->Vtx_Projected[UseVert].pR << 16) |
                                                        (PList[CurrPoly]->Vtx_Projected[UseVert].pG << 8) |
                                                        (PList[CurrPoly]->Vtx_Projected[UseVert].pB);
                D3DVerts[NumVerts].u = PList[CurrPoly]->Vtx_Projected[UseVert].pU;
                D3DVerts[NumVerts].v = PList[CurrPoly]->Vtx_Projected[UseVert].pV;
                NumVerts ++;
                if (NumVerts > IMR_RENDERER_BATCHMAXVERTS) 
                    {
                    #ifdef IMR_DEBUG
                        IMR_LogMsg(__LINE__, __FILE__, "BackMaxVerts hit!");
                    #endif 
                    MaxVertsHit = 1; break; 
                     };
                 }
             }
         }
    // Otherwise, draw all the polygons to this point and reset the list:
    else
        {
        // Set the current texture:
        Get_DeviceInterface()->SetTexture(0, LastTexture);

        // Draw the polys:
        Get_DeviceInterface()->DrawPrimitive(D3DPT_TRIANGLELIST,
                                             D3DFVF_XYZRHW |
                                             D3DFVF_TEX1 | 
                                             D3DFVF_DIFFUSE,
                                             D3DVerts, NumVerts, 
                                             D3DDP_DONOTCLIP | 
                                             D3DDP_DONOTLIGHT | 
                                             D3DDP_DONOTUPDATEEXTENTS);

        // Now set our poly index back 1 (since the current poly was never added):
        CurrPoly --; 
        NumVerts = 0;
         }
     }

// Return ok:
return IMR_OK;
 }

/***************************************************************************\
  Ends a rasterization batch.
  Returns IMR_OK if successful, otherwise an error.
\***************************************************************************/
int IMR_Renderer::End_Raster_Batch(void)
{
// Make sure we are already in a raster batch:
if (!Flags.InRasterBatch) 
    {
    IMR_LogMsg(__LINE__, __FILE__, "IMR_Renderer::End_Raster_Batch(): Not in raster batch!");
    return IMRERR_NONFATAL_NOTINBATCH;
     }

// Setup flags:
Flags.InRasterBatch = 0;

// End the scene:
Get_DeviceInterface()->EndScene();
                                              
// Nullify all pointers:
Camera = NULL;
Target.Data = NULL;

// And return ok:
return IMR_OK;
 }

/***************************************************************************\
  Adds a new texture.
  Returns IMR_OK if successful, otherwise an error.
\***************************************************************************/
IMR_TexRef IMR_Renderer::Texture_Add(char *Name)
{
IMR_TexRef NewTex;
NewTex.Host.Ptr = (void *)Textures.Add_Item(Name);
if (!NewTex.Host.Ptr)
    {
    IMR_LogMsg(__LINE__, __FILE__, "Map_Add failed!");
    return NewTex;
     };
NewTex.Host.Flag1 = 0;  // Not locked

return NewTex;
 }

/***************************************************************************\
  Generates the specified texture using parameters contained in the refrence
  Returns IMR_OK if successful, otherwise an error.
\***************************************************************************/
int IMR_Renderer::Texture_Gen(IMR_TexRef Ref)
{
IMR_Texture *Tex;
if (!(Tex = (IMR_Texture *)Ref.Host.Ptr)) 
    {
    IMR_LogMsg(__LINE__, __FILE__, "NULL refrence passed (no host pointer)!");
    return IMRERR_NODATA;
     };
//if (!Ref.Palette) 
//    {
//    IMR_LogMsg(__LINE__, __FILE__, "Refrence passed without palette!");
//    return IMRERR_NODATA;
//     };
if (Ref.Host.Flag1) 
    {
    IMR_LogMsg(__LINE__, __FILE__, "Refrence passed to locked surface!");
    return IMRERR_NOTREADY;
     };

// Now create the texture:
Ref.Type = IMR_REFTYPE_TEXTURE;
Tex->Reset();
int err = Tex->Create(DirectX, Ref.Width, Ref.Height, Ref.Palette);

// And return:
return err;
 }

/***************************************************************************\
  Creates a texref for the specified texture.
  Returns IMR_OK if successful, otherwise an error.
\***************************************************************************/
IMR_TexRef IMR_Renderer::Texture_GetRef(char *Name)
{
IMR_Texture *Tex;
IMR_TexRef Ref;

// Find the texture:
if (!(Tex = Textures.Get_Item(Name, NULL)))
    {
    // We couldn't find it?  Try and load it from our resources:
    if (IMR_ISNOTOK(Texture_Load(Name)))
        {
        Texture_CreateDummy(Name);      // Create a dummy
        IMR_LogMsg(__LINE__, __FILE__, "Texture %s not found!", Name);
        Ref.Host.Ptr = NULL;
        return Ref;
         }

    // Now get it again:
    if (!(Tex = Textures.Get_Item(Name, NULL)))
        {
        Texture_CreateDummy(Name);      // Create a dummy
        IMR_LogMsg(__LINE__, __FILE__, "Texture %s not found!", Name);
        Ref.Host.Ptr = NULL;
        return Ref;
         }
     }

// Now create a texref:
Ref.Host.Ptr = Tex;
Ref.Host.Flag1 = Tex->DataIsLocked();
Ref.Width = Tex->Get_Width();
Ref.Height = Tex->Get_Height();
Ref.Type = IMR_REFTYPE_TEXTURE;
Ref.Data = Tex->GetData();

// And return:
return Ref;
 }

/***************************************************************************\
  Creates a texref for the specified texture and gives access to it's data.
  Returns IMR_OK if successful, otherwise an error.
\***************************************************************************/
IMR_TexRef IMR_Renderer::Texture_GetData(char *Name)
{
IMR_Texture *Tex;
IMR_TexRef Ref;

// Find the texture:
if (!(Tex = Textures.Get_Item(Name, NULL))) 
    {
    IMR_LogMsg(__LINE__, __FILE__, "Texture %s not found!", Name);
    Ref.Host.Ptr = NULL;
    Ref.Data = NULL;
    return Ref;
     };

// Check if we need to lock the texture:
if (!Tex->DataIsLocked())
    Ref.Data = Tex->LockData(DirectX);
else
    Ref.Data = Tex->GetData();

// Now create a texref:
Ref.Host.Ptr = Tex;
Ref.Host.Flag1 = Tex->DataIsLocked();
Ref.Width = Tex->Get_Width();
Ref.Height = Tex->Get_Height();
Ref.Type = IMR_REFTYPE_TEXTURE;

// And return:
return Ref;
 }

/***************************************************************************\
  Returns access to texture surface data to the renderer.
  Returns IMR_OK if successful, otherwise an error.
\***************************************************************************/
int IMR_Renderer::Texture_ReturnData(IMR_TexRef Ref)
{
// Unlock the data:
IMR_Texture *Tex = (IMR_Texture *)Ref.Host.Ptr;
Tex->UnlockData(DirectX);

// And return ok:
return IMR_OK;
 }

/***************************************************************************\
  Adds a new lightmap
  Returns IMR_OK if successful, otherwise an error.
\***************************************************************************/
IMR_TexRef IMR_Renderer::Lightmap_Add(int ID)
{
IMR_TexRef NewTex;
NewTex.Host.Ptr = Lightmaps.Add_Item(ID);
if (!NewTex.Host.Ptr)
    {
    IMR_LogMsg(__LINE__, __FILE__, "Map_Add failed!");
    return NewTex;
     };
NewTex.Host.Flag1 = 0;  // Not locked

return NewTex;
 }

/***************************************************************************\
  Generates the specified texture using parameters contained in the refrence
  Returns IMR_OK if successful, otherwise an error.
\***************************************************************************/
int IMR_Renderer::Lightmap_Gen(IMR_TexRef Ref)
{
IMR_Lightmap *Tex;
if (!(Tex = (IMR_Lightmap *)Ref.Host.Ptr)) 
    {
    IMR_LogMsg(__LINE__, __FILE__, "NULL refrence passed (no host pointer)!");
    return IMRERR_NODATA;
     };
if (Ref.Host.Flag1) 
    {
    IMR_LogMsg(__LINE__, __FILE__, "Refrence passed to locked surface!");
    return IMRERR_NOTREADY;
     };

// Now create the lightmap:
Ref.Type = IMR_REFTYPE_LIGHTMAP;
Tex->Reset();
int err = Tex->Create_Lightmap(DirectX, Ref.Width, Ref.Height);

// And return:
return err;
 }

/***************************************************************************\
  Creates a texref for the specified lightmap.
  Returns IMR_OK if successful, otherwise an error.
\***************************************************************************/
IMR_TexRef IMR_Renderer::Lightmap_GetRef(int ID)
{
IMR_Lightmap *Tex;
IMR_TexRef Ref;

// Find the texture:
if (!(Tex = Lightmaps.Get_Item(ID, NULL))) 
    {
    IMR_LogMsg(__LINE__, __FILE__, "Lightmap %d not found!", ID);
    Ref.Host.Ptr = NULL;    
    return Ref;
     };

// Now create a texref:
Ref.Host.Ptr = Tex;
Ref.Host.Flag1 = Tex->DataIsLocked();
Ref.Width = Tex->Get_Width();
Ref.Height = Tex->Get_Height();
Ref.Type = IMR_REFTYPE_LIGHTMAP;
Ref.Data = NULL;

// And return:
return Ref;
 }

/***************************************************************************\
  Creates a texref for the specified lightmap and gives access to it's data.
  Returns IMR_OK if successful, otherwise an error.
\***************************************************************************/
IMR_TexRef IMR_Renderer::Lightmap_GetData(int ID)
{
IMR_Lightmap *Tex;
IMR_TexRef Ref;

// Find the lightmap:
if (!(Tex = Lightmaps.Get_Item(ID, NULL))) 
    {
    IMR_LogMsg(__LINE__, __FILE__, "Lightmap %d not found!", ID);
    Ref.Host.Ptr = NULL;
    Ref.Data = NULL;
    return Ref;
     };

// Check if we need to lock the lightmap:
if (!Tex->DataIsLocked())
    Ref.Data = Tex->LockData(DirectX);
else
    Ref.Data = Tex->GetData();

// Now create a texref:
Ref.Host.Ptr = Tex;
Ref.Host.Flag1 = Tex->DataIsLocked();
Ref.Width = Tex->Get_Width();
Ref.Height = Tex->Get_Height();
Ref.Type = IMR_REFTYPE_LIGHTMAP;

// And return:
return Ref;
 }

/***************************************************************************\
  Returns access to lightmap surface data to the renderer.
  Returns IMR_OK if successful, otherwise an error.
\***************************************************************************/
int IMR_Renderer::Lightmap_ReturnData(IMR_TexRef Ref)
{
// Unlock the data:
IMR_Texture *Tex = (IMR_Texture *)Ref.Host.Ptr;
Tex->UnlockData(DirectX);

// And return ok:
return IMR_OK;
 }

/***************************************************************************\


    Protected methods


\***************************************************************************/


/***************************************************************************\
  Initializes the screen.
  Returns IMR_OK if successful, otherwise an error.
\***************************************************************************/
int IMR_Renderer::Set_Screen(int W, int H, HWND Wnd)
{
IMR_DirectXInterface_InitData DXInitData;
int err;

// Shut down DirectX:
DirectX.Shutdown();

// Setup DirectX:
DXInitData.AppWnd = Wnd;
DXInitData.Display.Width = W;
DXInitData.Display.Height = H;
DXInitData.Display.BPP = 16;
err = DirectX.Init(DXInitData);
if (IMR_ISNOTOK(err)) return err;

// Set vars:
Target.Screen.Width = W;
Target.Screen.Height = H;
Target.Screen.AppWindow = Wnd;

// And set our flag:
Flags.ScreenInitialized = 1;

// And return OK:
return IMR_OK;
 }

/***************************************************************************\
  Sets up the render window.
  Returns IMR_OK if successful, otherwise an error.
\***************************************************************************/
int IMR_Renderer::Set_Window(int x0, int y0, int x1, int y1)
{
// Make sure our screen has been initialized:
if (!Flags.ScreenInitialized)
    {
    IMR_LogMsg(__LINE__, __FILE__, "Screen must be initialized before window!");
    return IMRERR_GENERIC;
     };

// Make sure our window is within the screen:
if (x0 < 0 || y0 < 0 || 
    x1 >= Target.Screen.Width || y1 >= Target.Screen.Width)
    {
    IMR_LogMsg(__LINE__, __FILE__, "Window must be totally inside of screen!");
    return IMRERR_BADWIDTH;
     };

// Setup buffer info:
Target.Window.Width = (x1 - x0);// + 1;
Target.Window.Height = (y1 - y0);// + 1;
Target.Window.MinX = x0;
Target.Window.MinY = y0;
Target.Window.MaxX = x1;
Target.Window.MaxY = y1;
Target.Window.XCenter = Target.Window.Width / 2;    // Relative center...
Target.Window.YCenter = Target.Window.Height / 2;
Target.Length = Target.Window.Width * Target.Window.Height * 2;    // 2-byte pixels...

// Destroy the double-buffer:
if (Target.Surface)
    DirectX.Destroy_Surface(Target.Surface);
Target.Surface = NULL;

// Create a new double-buffer surface:
Target.Surface = DirectX.Create_3DSurface(Target.Window.Width, Target.Window.Height, 16);
if (!Target.Surface) return IMRERR_DIRECTX;

// (Re)initialize Direct3D:
int err = DirectX.InitDirect3D(Target.Surface);
if (IMR_ISNOTOK(err)) return err;

// And return ok:
return IMR_OK;
 }

/***************************************************************************\
  Locks the double-buffer surface to allow writes.
  Returns IMR_OK if successful, otherwise an error.
\***************************************************************************/
int IMR_Renderer::Target_Lock(void)
{
if (Flags.BufferLocked) Target_Unlock();
Target.Data = (char *)DirectX.Lock_Surface(Target.Surface);
if (!Target.Data)
    {
    IMR_LogMsg(__LINE__, __FILE__, "Lock failed!");
    return IMRERR_NODATA;
     }
Flags.BufferLocked = 1;
return IMR_OK;
 }

/***************************************************************************\
  Unlocks the double-buffer surface.
\***************************************************************************/
void IMR_Renderer::Target_Unlock(void)
{
if (!Flags.BufferLocked) return;
DirectX.Unlock_Surface(Target.Surface);
Target.Data = NULL;
Flags.BufferLocked = 0;
 }

/***************************************************************************\
  Locks the back surface to allow writes.
  Returns IMR_OK if successful, otherwise an error.
\***************************************************************************/
int IMR_Renderer::Target_LockBack(void)
{
// If we are already locked, unlock ourselves:
if (Flags.BackLocked) Target_UnlockBack();
Target.BackData = (char *)DirectX.Lock_Surface(DirectX.Get_BackSurface());
if (!Target.BackData)
    {
    IMR_LogMsg(__LINE__, __FILE__, "Lock failed!");
    return IMRERR_NODATA;
     }
Flags.BackLocked = 1;
return IMR_OK;
 }

/***************************************************************************\
  Unlocks the back surface.
\***************************************************************************/
void IMR_Renderer::Target_UnlockBack(void)
{
if (!Flags.BackLocked) return;
DirectX.Unlock_Surface(DirectX.Get_BackSurface());
Target.BackData = NULL;
Flags.BackLocked = 0;
 }

/***************************************************************************\
  Blits the double-buffer surface to the screen.
\***************************************************************************/
int IMR_Renderer::Target_Blit(void)
{
if (Flags.BufferLocked || !Target.Surface) 
    {
    IMR_LogMsg(__LINE__, __FILE__, "Can't blit! (State not correct)");
    return IMRERR_GENERIC;
     }
DirectX.BlitToBack(Target.Surface);
return IMR_OK;
 }

/***************************************************************************\
  Flips the double- and back buffer surfaces.
\***************************************************************************/
int IMR_Renderer::Target_Flip(void)
{
if (Flags.BackLocked) 
    {
    IMR_LogMsg(__LINE__, __FILE__, "Can't flip! (State not correct)");
    return IMRERR_GENERIC;
     }
DirectX.FlipBackBuffer();
return IMR_OK;
 }

/***************************************************************************\
  Loads the specified PCX file into the specified texture.
  Returns IMR_OK if successful, an error otherwise.
\***************************************************************************/
int IMR_Renderer::Texture_LoadFromPCX(char *Filename, char *TexName)
{
char Byte, HeaderByte, ColorByte, Buffer[10], *Data;
char Pal[768];
int index, err, Pos, Width, Height,
    PcxFile, 
    Px, Py, RunLength;
IMR_TexRef Ref;
IMR_RDFResourceDesc ResourceDesc;

// Look for the file in our resources:
ResourceDesc = IMR_Resources.Get_RDFManager()->FindRDF(NULL, Filename, NULL);
if (ResourceDesc == -1)
    {
    IMR_LogMsg(__LINE__, __FILE__, "File %s not found in resource directory!", Filename);
    return IMRERR_BADFILE;
     }

// Open the file:
err = IMR_Resources.Get_RDFManager()->OpenRDF(ResourceDesc, PcxFile);
if (IMR_ISNOTOK(err))
    {
    IMR_LogMsg(__LINE__, __FILE__, "Can't load texture %s!", Filename);
    return IMRERR_BADFILE;
     }

// Eat some stuff:
read(PcxFile, Buffer, 8);

// Find the width and the height of the image:
Width = 0; read(PcxFile, &Width, 2);
Height = 0; read(PcxFile, &Height, 2);

// Make sure we have a valid texture:
if (Width <= 0 || Height <= 0)
    {
    IMR_LogMsg(__LINE__, __FILE__, "Image size is <= 0 in file %s!", Filename);
    return IMRERR_BADFILE;
     }

// Add the texture:
Ref = Texture_Add(TexName);
if (!Ref.HasHost())
    {
    IMR_LogMsg(__LINE__, __FILE__, "Add texture %s failed!", TexName);
    return IMRERR_GENERIC;
     }

// Go to the palette data:
lseek(PcxFile, filelength(PcxFile) - 768, SEEK_SET);

// Read the color palette:
read(PcxFile, (void *)Pal, 768);

// Generate our texture:
Ref.Width = Width + 1;
Ref.Height = Height + 1;
Ref.Set_Palette(Pal);
Ref.Type = IMR_REFTYPE_TEXTURE;
if (IMR_ISNOTOK(err = Texture_Gen(Ref)))
    {
    IMR_LogMsg(__LINE__, __FILE__, "Texture generation failed for texture %s!", TexName);
    return err;
     }

// Now get a refrence with the texture data:
Ref = Texture_GetData(TexName);

// Go to the image data:
lseek(PcxFile, 128, SEEK_SET);

// Reset position:
Px = Py = 0;
Pos = 0;

// Make sure we have surface data:
if (!Ref.Get_Data())
    {
    IMR_LogMsg(__LINE__, __FILE__, "No data for texture %s!", TexName);
    return IMRERR_NODATA;
     }
Data = (char *)Ref.Get_Data();

// Read image:
while (Py < (Height + 3))
    {
    // Read header/color byte:
    read(PcxFile, &HeaderByte, 1);
    
    // Check if we have encountered a run:
    if (HeaderByte >= 193)
        {
        // Read color byte:
        read(PcxFile, &ColorByte, 1);

        // Calculate run length:
        RunLength = HeaderByte - 193;
       
        // Write the run:
        for (index = 0; index < RunLength + 1; index ++)
            {
            // Increment x position:
            Px ++;
            if (Px > (Width - 1)) { Py ++; Px = 0; }
            
            // Set the pixel:
            //Data[Pos ++] = ColorByte;
            //short Temp = 0;
            //Temp |= (short)(Pal[(ColorByte * 3)] >> 4) << 8;
            //Temp |= (short)(Pal[(ColorByte * 3) + 1] >> 4) << 4;
            //Temp |= (short)(Pal[(ColorByte * 3) + 2] >> 4);
            //Data[Pos ++] = Temp & 0x00ff;
            //Data[Pos ++] = Temp >> 8;
            short Temp = 0;
            Temp |= (short)(Pal[(ColorByte * 3)] >> 3) << 10;
            Temp |= (short)(Pal[(ColorByte * 3) + 1] >> 3) << 5;
            Temp |= (short)(Pal[(ColorByte * 3) + 2] >> 3);
            Data[Pos ++] = Temp & 0x00ff;
            Data[Pos ++] = Temp >> 8;
             }
         }
    
    // If we have just a single pixel, write it:
    else
        {
        // Set the color:
        ColorByte = HeaderByte;

        // Increment x position:
        Px ++;
        if (Px > Width) { Py ++; Px = 0; }
        
        // Set the pixel:
        //Data[Pos ++] = ColorByte;
        //short Temp = 0;//(short)Data[Pos];
        //Temp |= (short)(Pal[(ColorByte * 3)] >> 4) << 8;
        //Temp |= (short)(Pal[(ColorByte * 3) + 1] >> 4) << 4;
        //Temp |= (short)(Pal[(ColorByte * 3) + 2] >> 4);
        //Data[Pos ++] = Temp & 0x00ff;
        //Data[Pos ++] = Temp >> 8;
            short Temp = 0;
            Temp |= (short)(Pal[(ColorByte * 3)] >> 3) << 10;
            Temp |= (short)(Pal[(ColorByte * 3) + 1] >> 3) << 5;
            Temp |= (short)(Pal[(ColorByte * 3) + 2] >> 3);
            Data[Pos ++] = Temp & 0x00ff;
            Data[Pos ++] = Temp >> 8;
         }
     }

// Return the texture data:
Texture_ReturnData(Ref);

// Close the file:
IMR_Resources.Get_RDFManager()->CloseRDF(PcxFile);

// And return ok:
return IMR_OK;
 }

/***************************************************************************\
  Loads the specified RDF texture file
  Returns IMR_OK if successful, an error otherwise.
\***************************************************************************/
int IMR_Renderer::Texture_Load(char *TexName)
{
char R, G, B, Buffer[35], *Data;
int Width, Height, px, py, fd, Pos, err;
short pel, Temp;
IMR_TexRef Ref;
IMR_RDFResourceDesc ResourceDesc;

// Look for the file in our resources:
ResourceDesc = IMR_Resources.Get_RDFManager()->FindRDF("Tex", NULL, TexName);
if (ResourceDesc == -1)
    {
    IMR_LogMsg(__LINE__, __FILE__, "Texture %s not found in resource directory!", TexName);
    return IMRERR_BADFILE;
     }

// Open the file:
err = IMR_Resources.Get_RDFManager()->OpenRDF(ResourceDesc, fd);
if (IMR_ISNOTOK(err))
    {
    IMR_LogMsg(__LINE__, __FILE__, "Can't load texture %s!", TexName);
    return IMRERR_BADFILE;
     }

// Eat ID and name:
read(fd, Buffer, 35);

// Find the width and the height of the image:
Width = 0; read(fd, &Width, 2);
Height = 0; read(fd, &Height, 2);

// Make sure we have a valid texture:
if (Width <= 0 || Height <= 0)
    {
    IMR_LogMsg(__LINE__, __FILE__, "Image size is <= 0 in texture %s! (%d, %d)", TexName, Width, Height);
    return IMRERR_BADFILE;
     }

// Add the texture:
Ref = Texture_Add(TexName);
if (!Ref.HasHost())
    {
    IMR_LogMsg(__LINE__, __FILE__, "Add texture %s failed!", TexName);
    return IMRERR_GENERIC;
     }

// Generate our texture:
Ref.Width = Width;
Ref.Height = Height;
Ref.Type = IMR_REFTYPE_TEXTURE;
if (IMR_ISNOTOK(err = Texture_Gen(Ref)))
    {
    IMR_LogMsg(__LINE__, __FILE__, "Texture generation failed for texture %s!", TexName);
    return err;
     }

// Now get a refrence with the texture data:
Ref = Texture_GetData(TexName);

// Make sure we have surface data:
if (!Ref.Get_Data())
    {
    IMR_LogMsg(__LINE__, __FILE__, "No data for texture %s!", TexName);
    return IMRERR_NODATA;
     }
Data = (char *)Ref.Get_Data();

// Read image:
Pos = 0;
for (py = 0; py <= Height; py ++)
    {
    for (px = 0; px <= Width; px ++)
        {
        // Read pixel:
        read(fd, &pel, 2);
        
        // Get componenets:
        R = (pel & 0x7c00) >> 10;
        G = (pel & 0x3e0) >> 5;
        B = (pel & 0x1f);

        // Convert to 4:4:4:
        R >>= 1;
        G >>= 1;
        B >>= 1;
        
        // And save the new pixel
        Temp = 0;
        Temp |= R << 8;
        Temp |= G << 4;
        Temp |= B;
        Temp = pel;
        Data[Pos ++] = Temp & 0x00ff;
        Data[Pos ++] = Temp >> 8;
         }
     }

// Return the texture data:
Texture_ReturnData(Ref);

// Close the file:
IMR_Resources.Get_RDFManager()->CloseRDF(fd);

// And return ok:
return IMR_OK;
 }

/***************************************************************************\
  Creates a dummy texture to replace textures that are unavailable.
  Returns IMR_OK if successful, an error otherwise.
\***************************************************************************/
int IMR_Renderer::Texture_CreateDummy(char *TexName)
{
char *Data;
int err;
IMR_TexRef Ref;

// Add the texture:
Ref = Texture_Add(TexName);
if (!Ref.HasHost())
    {
    IMR_LogMsg(__LINE__, __FILE__, "Add texture %s failed!", TexName);
    return IMRERR_GENERIC;
     }

// Generate our texture:
Ref.Width = 1;
Ref.Height = 1;
Ref.Type = IMR_REFTYPE_TEXTURE;
if (IMR_ISNOTOK(err = Texture_Gen(Ref)))
    {
    IMR_LogMsg(__LINE__, __FILE__, "Texture generation failed for texture %s!", TexName);
    return err;
     }

// Now get a refrence with the texture data:
Ref = Texture_GetData(TexName);

// Make sure we have surface data:
if (!Ref.Get_Data())
    {
    IMR_LogMsg(__LINE__, __FILE__, "No data for texture %s!", TexName);
    return IMRERR_NODATA;
     }
Data = (char *)Ref.Get_Data();

// Make the texture black:
Data[0] = 0x00;
Data[1] = 0x00;

// Return the texture data:
Texture_ReturnData(Ref);

// And return ok:
return IMR_OK;
 }
