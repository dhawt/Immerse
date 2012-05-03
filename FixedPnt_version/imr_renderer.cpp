/***************************************************************************\
  File: IMR_Renderer.cpp
  Description: Renderer module.
  Author: Daniel Hawthorn
  Modified:
  
\***************************************************************************/
#include "IMR_Renderer.hpp"

/***************************************************************************\
  Initializes the renderer.  
  Returns IMR_OK if successful, otherwise an error.
\***************************************************************************/
int IMR_Renderer::Init(void)
{
// Now make sure the math tables have been buit:
IMR_BuildTables();

// Flag that this class has been initialized:
Flags.ClassInitialized = 1;

// Return ok:
return IMR_OK;
 }

/***************************************************************************\
  Shuts down the renderer.  
  Returns IMR_OK if successful, otherwise an error.
\***************************************************************************/
int IMR_Renderer::Shutdown(void)
{
// Flag that this class has not been initialized:
Flags.ClassInitialized = 0;

// Return ok:
return IMR_OK;
 }

/***************************************************************************\
  Initializes a rasterization batch.
  Returns IMR_OK if successful, otherwise an error.
\***************************************************************************/
int IMR_Renderer::Begin_Raster_Batch(IMR_Camera &Cam, IMR_RendBuffer &Buff)
{
int err;

// Make sure we have settings:
if (!CurrentSettings)
    {
    IMR_SetErrorText("IMR_Renderer::Begin_Raster_Batch(): No settings available!");
    return IMRERR_NODATA;
     }

// Save a pointer to the camera and rend buffer:
Camera = &Cam;
Buffer = &Buff;

// Start the scene:
Buffer->Clear_Buffers();
Buffer->Get_DeviceInterface()->BeginScene();

Buffer->Get_DeviceInterface()->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
Buffer->Get_DeviceInterface()->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
Buffer->Get_DeviceInterface()->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
if (CurrentSettings->Renderer.FilteringEnabled)
    {
    Buffer->Get_DeviceInterface()->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTFN_LINEAR);
    Buffer->Get_DeviceInterface()->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTFG_LINEAR);
     }
else
    {
    Buffer->Get_DeviceInterface()->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTFN_POINT);
    Buffer->Get_DeviceInterface()->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTFG_POINT);
     }

// Setup mipmapping:
if (CurrentSettings->Renderer.MipMappingEnabled)
    Buffer->Get_DeviceInterface()->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTFP_LINEAR);
else
    Buffer->Get_DeviceInterface()->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTFP_NONE);

// Setup stuff:
Flags.InRasterBatch = 1;
Flags.DrawMode = IMR_RENDERER_MODE_INIT;
PolysDrawn = 0;

// And return ok:
return IMR_OK;
 }

/***************************************************************************\
  Draws the specified poly in wireframe.  Does not require a texture.
  Returns IMR_OK.
\***************************************************************************/
int IMR_Renderer::Draw_Wireframe_Polygon(IMR_Polygon &Poly)
{
struct
    {
    float x, y, z;
    float w;
    long Color;
     } Verts[5];
float ZNormalize = 1 / FixedToFloat(Buffer->Get_Far());

// Make sure we are in a raster batch:
if (!Flags.InRasterBatch) 
    {
    #ifdef IMR_DEBUG
        IMR_SetErrorText("IMR_Renderer::Draw_Wireframe_Polygon(): Not in raster batch!");
    #endif
    return IMRERR_NONFATAL_NOTINBATCH;
     }

// Setup the device (if we have just changed states):
if (Flags.DrawMode != IMR_RENDERER_MODE_WIREFRAME)
    {
    Buffer->Get_DeviceInterface()->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    Buffer->Get_DeviceInterface()->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT);
    Buffer->Get_DeviceInterface()->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_DISABLE);
    Buffer->Get_DeviceInterface()->SetRenderState(D3DRENDERSTATE_ZENABLE, D3DZB_FALSE);
    Buffer->Get_DeviceInterface()->SetRenderState(D3DRENDERSTATE_SHADEMODE, D3DSHADE_FLAT);
    Buffer->Get_DeviceInterface()->SetRenderState(D3DRENDERSTATE_FILLMODE, D3DFILL_WIREFRAME);
    Flags.DrawMode = IMR_RENDERER_MODE_WIREFRAME;
     }

if (Poly.Num_Verts_Proj == 3)
    {
    Verts[0].x = Poly.Vtx_Projected[0].pX;
    Verts[0].y = Poly.Vtx_Projected[0].pY;
    Verts[0].z = Poly.Vtx_Projected[0].pZ * ZNormalize;
    Verts[0].w = 1 / Poly.Vtx_Projected[0].pW;
    Verts[0].Color = 0xffffffff;
    Verts[1].x = Poly.Vtx_Projected[1].pX;
    Verts[1].y = Poly.Vtx_Projected[1].pY;
    Verts[1].z = Poly.Vtx_Projected[1].pZ * ZNormalize;
    Verts[1].w = 1 / Poly.Vtx_Projected[1].pW;
    Verts[1].Color = 0xffffffff;
    Verts[2].x = Poly.Vtx_Projected[2].pX;
    Verts[2].y = Poly.Vtx_Projected[2].pY;
    Verts[2].z = FixedToFloat(Poly.Vtx_Projected[2].Z) * ZNormalize;
    Verts[2].w = 1 / FixedToFloat(Poly.Vtx_Projected[2].pW);
    Verts[2].Color = 0xffffffff;
    Buffer->Get_DeviceInterface()->DrawPrimitive(D3DPT_TRIANGLELIST,  
                                                 D3DFVF_XYZRHW |
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
    Verts[0].w = 1 / Poly.Vtx_Projected[0].pW;
    Verts[0].Color = 0xffffffff;
    Verts[1].x = Poly.Vtx_Projected[3].pX;
    Verts[1].y = Poly.Vtx_Projected[3].pY;
    Verts[1].z = Poly.Vtx_Projected[3].pZ * ZNormalize;
    Verts[1].w = 1 / Poly.Vtx_Projected[3].pW;
    Verts[1].Color = 0xffffffff;
    Verts[2].x = Poly.Vtx_Projected[1].pX;
    Verts[2].y = Poly.Vtx_Projected[1].pY;
    Verts[2].z = Poly.Vtx_Projected[1].pZ * ZNormalize;
    Verts[2].w = 1 / Poly.Vtx_Projected[1].pW;
    Verts[2].Color = 0xffffffff;
    Verts[3].x = Poly.Vtx_Projected[2].pX;
    Verts[3].y = Poly.Vtx_Projected[2].pY;
    Verts[3].z = Poly.Vtx_Projected[2].pZ * ZNormalize;
    Verts[3].w = 1 / Poly.Vtx_Projected[2].pW;
    Verts[3].Color = 0xffffffff;
    Buffer->Get_DeviceInterface()->DrawPrimitive(D3DPT_TRIANGLESTRIP,  
                                                 D3DFVF_XYZRHW | 
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
    Verts[0].w = 1 / Poly.Vtx_Projected[0].pW;
    Verts[0].Color = 0xffffffff;
    Verts[1].x = Poly.Vtx_Projected[1].pX;
    Verts[1].y = Poly.Vtx_Projected[1].pY;
    Verts[1].z = Poly.Vtx_Projected[1].pZ * ZNormalize;
    Verts[1].w = 1 / Poly.Vtx_Projected[1].pW;
    Verts[1].Color = 0xffffffff;
    Verts[2].x = Poly.Vtx_Projected[4].pX;
    Verts[2].y = Poly.Vtx_Projected[4].pY;
    Verts[2].z = Poly.Vtx_Projected[4].pZ * ZNormalize;
    Verts[2].w = 1 / Poly.Vtx_Projected[4].pW;
    Verts[2].Color = 0xffffffff;
    Verts[3].x = Poly.Vtx_Projected[2].pX;
    Verts[3].y = Poly.Vtx_Projected[2].pY;
    Verts[3].z = Poly.Vtx_Projected[2].pZ * ZNormalize;
    Verts[3].w = 1 / Poly.Vtx_Projected[2].pW;
    Verts[3].Color = 0xffffffff;
    Verts[4].x = Poly.Vtx_Projected[3].pX;
    Verts[4].y = Poly.Vtx_Projected[3].pY;
    Verts[4].z = Poly.Vtx_Projected[3].pZ * ZNormalize;
    Verts[4].w = 1 / Poly.Vtx_Projected[3].pW;
    Verts[4].Color = 0xffffffff;
    Buffer->Get_DeviceInterface()->DrawPrimitive(D3DPT_TRIANGLESTRIP,
                                                 D3DFVF_XYZRHW |
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
  Draws the specified poly with only lighting color.
  Returns IMR_OK.
\***************************************************************************/
int IMR_Renderer::Draw_Filled_Polygon(IMR_Polygon &Poly)
{
struct
    {
    float x, y, z;
    float w;
    long Color;
     } Verts[5];
float ZNormalize = 1 / FixedToFloat(Buffer->Get_Far());

// Make sure we are in a raster batch:
if (!Flags.InRasterBatch) 
    {
    #ifdef IMR_DEBUG
        IMR_SetErrorText("IMR_Renderer::Draw_Wireframe_Polygon(): Not in raster batch!");
    #endif
    return IMRERR_NONFATAL_NOTINBATCH;
     }

// Setup the device (if we have just changed states):
if (Flags.DrawMode != IMR_RENDERER_MODE_FILLED)
    {
    Buffer->Get_DeviceInterface()->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
    Buffer->Get_DeviceInterface()->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT);
    Buffer->Get_DeviceInterface()->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
    Buffer->Get_DeviceInterface()->SetRenderState(D3DRENDERSTATE_ZENABLE, D3DZB_TRUE);
    Buffer->Get_DeviceInterface()->SetRenderState(D3DRENDERSTATE_SHADEMODE, D3DSHADE_GOURAUD);
    Buffer->Get_DeviceInterface()->SetRenderState(D3DRENDERSTATE_FILLMODE, D3DFILL_SOLID);
    Flags.DrawMode = IMR_RENDERER_MODE_FILLED;
     }

if (Poly.Num_Verts_Proj == 3)
    {
    Verts[0].x = Poly.Vtx_Projected[0].pX;
    Verts[0].y = Poly.Vtx_Projected[0].pY;
    Verts[0].z = Poly.Vtx_Projected[0].pZ * ZNormalize;
    Verts[0].w = 1 / Poly.Vtx_Projected[0].pW;
    Verts[0].Color = 0xff000000 | (Poly.Vtx_Projected[0].pR << 16) |
                                  (Poly.Vtx_Projected[0].pG << 8) |
                                  (Poly.Vtx_Projected[0].pB);
    Verts[1].x = Poly.Vtx_Projected[1].pX;
    Verts[1].y = Poly.Vtx_Projected[1].pY;
    Verts[1].z = Poly.Vtx_Projected[1].pZ * ZNormalize;
    Verts[1].w = 1 / Poly.Vtx_Projected[1].pW;
    Verts[1].Color = 0xff000000 | (Poly.Vtx_Projected[1].pR << 16) |
                                  (Poly.Vtx_Projected[1].pG << 8) |
                                  (Poly.Vtx_Projected[1].pB);
    Verts[2].x = Poly.Vtx_Projected[2].pX;
    Verts[2].y = Poly.Vtx_Projected[2].pY;
    Verts[2].z = FixedToFloat(Poly.Vtx_Projected[2].Z) * ZNormalize;
    Verts[2].w = 1 / FixedToFloat(Poly.Vtx_Projected[2].pW);
    Verts[2].Color = 0xff000000 | (Poly.Vtx_Projected[2].pR << 16) |
                                  (Poly.Vtx_Projected[2].pG << 8) |
                                  (Poly.Vtx_Projected[2].pB);
    Buffer->Get_DeviceInterface()->DrawPrimitive(D3DPT_TRIANGLELIST,  
                                                 D3DFVF_XYZRHW |
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
    Verts[0].w = 1 / Poly.Vtx_Projected[0].pW;
    Verts[0].Color = 0xff000000 | (Poly.Vtx_Projected[0].pR << 16) |
                                  (Poly.Vtx_Projected[0].pG << 8) |
                                  (Poly.Vtx_Projected[0].pB);
    Verts[1].x = Poly.Vtx_Projected[3].pX;
    Verts[1].y = Poly.Vtx_Projected[3].pY;
    Verts[1].z = Poly.Vtx_Projected[3].pZ * ZNormalize;
    Verts[1].w = 1 / Poly.Vtx_Projected[3].pW;
    Verts[1].Color = 0xff000000 | (Poly.Vtx_Projected[3].pR << 16) |
                                  (Poly.Vtx_Projected[3].pG << 8) |
                                  (Poly.Vtx_Projected[3].pB);
    Verts[2].x = Poly.Vtx_Projected[1].pX;
    Verts[2].y = Poly.Vtx_Projected[1].pY;
    Verts[2].z = Poly.Vtx_Projected[1].pZ * ZNormalize;
    Verts[2].w = 1 / Poly.Vtx_Projected[1].pW;
    Verts[2].Color = 0xff000000 | (Poly.Vtx_Projected[1].pR << 16) |
                                  (Poly.Vtx_Projected[1].pG << 8) |
                                  (Poly.Vtx_Projected[1].pB);
    Verts[3].x = Poly.Vtx_Projected[2].pX;
    Verts[3].y = Poly.Vtx_Projected[2].pY;
    Verts[3].z = Poly.Vtx_Projected[2].pZ * ZNormalize;
    Verts[3].w = 1 / Poly.Vtx_Projected[2].pW;
    Verts[3].Color = 0xff000000 | (Poly.Vtx_Projected[2].pR << 16) |
                                  (Poly.Vtx_Projected[2].pG << 8) |
                                  (Poly.Vtx_Projected[2].pB);
    Buffer->Get_DeviceInterface()->DrawPrimitive(D3DPT_TRIANGLESTRIP,  
                                                 D3DFVF_XYZRHW | 
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
    Verts[0].w = 1 / Poly.Vtx_Projected[0].pW;
    Verts[0].Color = 0xff000000 | (Poly.Vtx_Projected[0].pR << 16) |
                                  (Poly.Vtx_Projected[0].pG << 8) |
                                  (Poly.Vtx_Projected[0].pB);
    Verts[1].x = Poly.Vtx_Projected[1].pX;
    Verts[1].y = Poly.Vtx_Projected[1].pY;
    Verts[1].z = Poly.Vtx_Projected[1].pZ * ZNormalize;
    Verts[1].w = 1 / Poly.Vtx_Projected[1].pW;
    Verts[1].Color = 0xff000000 | (Poly.Vtx_Projected[1].pR << 16) |
                                  (Poly.Vtx_Projected[1].pG << 8) |
                                  (Poly.Vtx_Projected[1].pB);
    Verts[2].x = Poly.Vtx_Projected[4].pX;
    Verts[2].y = Poly.Vtx_Projected[4].pY;
    Verts[2].z = Poly.Vtx_Projected[4].pZ * ZNormalize;
    Verts[2].w = 1 / Poly.Vtx_Projected[4].pW;
    Verts[2].Color = 0xff000000 | (Poly.Vtx_Projected[4].pR << 16) |
                                  (Poly.Vtx_Projected[4].pG << 8) |
                                  (Poly.Vtx_Projected[4].pB);
    Verts[3].x = Poly.Vtx_Projected[2].pX;
    Verts[3].y = Poly.Vtx_Projected[2].pY;
    Verts[3].z = Poly.Vtx_Projected[2].pZ * ZNormalize;
    Verts[3].w = 1 / Poly.Vtx_Projected[2].pW;
    Verts[3].Color = 0xff000000 | (Poly.Vtx_Projected[2].pR << 16) |
                                  (Poly.Vtx_Projected[2].pG << 8) |
                                  (Poly.Vtx_Projected[2].pB);
    Verts[4].x = Poly.Vtx_Projected[3].pX;
    Verts[4].y = Poly.Vtx_Projected[3].pY;
    Verts[4].z = Poly.Vtx_Projected[3].pZ * ZNormalize;
    Verts[4].w = 1 / Poly.Vtx_Projected[3].pW;
    Verts[4].Color = 0xff000000 | (Poly.Vtx_Projected[3].pR << 16) |
                                  (Poly.Vtx_Projected[3].pG << 8) |
                                  (Poly.Vtx_Projected[3].pB);
    Buffer->Get_DeviceInterface()->DrawPrimitive(D3DPT_TRIANGLESTRIP,
                                                 D3DFVF_XYZRHW |
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
float ZNormalize = 1 / FixedToFloat(Buffer->Get_Far());

// Make sure we are in a raster batch and the polygon has a texture:
if (!Flags.InRasterBatch) 
    {
    #ifdef IMR_DEBUG
        IMR_SetErrorText("IMR_Renderer::Draw_Wireframe_Polygon(): Not in raster batch!");
    #endif
    return IMRERR_NONFATAL_NOTINBATCH;
     }
if (!Poly.Material.Get_Texture())
    return IMR_OK;

// Setup the device (if we have just changed states):
if (Flags.DrawMode != IMR_RENDERER_MODE_TEXTURED)
    {
    Buffer->Get_DeviceInterface()->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    Buffer->Get_DeviceInterface()->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT);
    Buffer->Get_DeviceInterface()->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
    Buffer->Get_DeviceInterface()->SetRenderState(D3DRENDERSTATE_ZENABLE, D3DZB_TRUE);
    Buffer->Get_DeviceInterface()->SetRenderState(D3DRENDERSTATE_SHADEMODE, D3DSHADE_GOURAUD);
    Buffer->Get_DeviceInterface()->SetRenderState(D3DRENDERSTATE_FILLMODE, D3DFILL_SOLID);
    Flags.DrawMode = IMR_RENDERER_MODE_TEXTURED;
     }

if (Poly.Num_Verts_Proj == 3)
    {
    Verts[0].x = Poly.Vtx_Projected[0].pX;
    Verts[0].y = Poly.Vtx_Projected[0].pY;
    Verts[0].z = Poly.Vtx_Projected[0].pZ * ZNormalize;
    Verts[0].w = 1 / Poly.Vtx_Projected[0].pW;
    Verts[0].u = Poly.Vtx_Projected[0].pU;
    Verts[0].v = Poly.Vtx_Projected[0].pV;
    Verts[1].x = Poly.Vtx_Projected[1].pX;
    Verts[1].y = Poly.Vtx_Projected[1].pY;
    Verts[1].z = Poly.Vtx_Projected[1].pZ * ZNormalize;
    Verts[1].w = 1 / Poly.Vtx_Projected[1].pW;
    Verts[1].u = Poly.Vtx_Projected[1].pU;
    Verts[1].v = Poly.Vtx_Projected[1].pV;
    Verts[2].x = Poly.Vtx_Projected[2].pX;
    Verts[2].y = Poly.Vtx_Projected[2].pY;
    Verts[2].z = FixedToFloat(Poly.Vtx_Projected[2].Z) * ZNormalize;
    Verts[2].w = 1 / FixedToFloat(Poly.Vtx_Projected[2].pW);
    Verts[2].u = Poly.Vtx_Projected[2].pU;
    Verts[2].v = Poly.Vtx_Projected[2].pV;
    Buffer->Get_DeviceInterface()->SetTexture(0, Poly.Material.Get_TextureInterface());
    Buffer->Get_DeviceInterface()->DrawPrimitive(D3DPT_TRIANGLELIST,  
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
    Verts[0].w = 1 / Poly.Vtx_Projected[0].pW;
    Verts[0].u = Poly.Vtx_Projected[0].pU;
    Verts[0].v = Poly.Vtx_Projected[0].pV;
    Verts[1].x = Poly.Vtx_Projected[3].pX;
    Verts[1].y = Poly.Vtx_Projected[3].pY;
    Verts[1].z = Poly.Vtx_Projected[3].pZ * ZNormalize;
    Verts[1].w = 1 / Poly.Vtx_Projected[3].pW;
    Verts[1].u = Poly.Vtx_Projected[3].pU;
    Verts[1].v = Poly.Vtx_Projected[3].pV;
    Verts[2].x = Poly.Vtx_Projected[1].pX;
    Verts[2].y = Poly.Vtx_Projected[1].pY;
    Verts[2].z = Poly.Vtx_Projected[1].pZ * ZNormalize;
    Verts[2].w = 1 / Poly.Vtx_Projected[1].pW;
    Verts[2].u = Poly.Vtx_Projected[1].pU;
    Verts[2].v = Poly.Vtx_Projected[1].pV;
    Verts[3].x = Poly.Vtx_Projected[2].pX;
    Verts[3].y = Poly.Vtx_Projected[2].pY;
    Verts[3].z = Poly.Vtx_Projected[2].pZ * ZNormalize;
    Verts[3].w = 1 / Poly.Vtx_Projected[2].pW;
    Verts[3].u = Poly.Vtx_Projected[2].pU;
    Verts[3].v = Poly.Vtx_Projected[2].pV;
    Buffer->Get_DeviceInterface()->SetTexture(0, Poly.Material.Get_TextureInterface());
    Buffer->Get_DeviceInterface()->DrawPrimitive(D3DPT_TRIANGLESTRIP,  
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
    Verts[0].w = 1 / Poly.Vtx_Projected[0].pW;
    Verts[0].u = Poly.Vtx_Projected[0].pU;
    Verts[0].v = Poly.Vtx_Projected[0].pV;
    Verts[1].x = Poly.Vtx_Projected[1].pX;
    Verts[1].y = Poly.Vtx_Projected[1].pY;
    Verts[1].z = Poly.Vtx_Projected[1].pZ * ZNormalize;
    Verts[1].w = 1 / Poly.Vtx_Projected[1].pW;
    Verts[1].u = Poly.Vtx_Projected[1].pU;
    Verts[1].v = Poly.Vtx_Projected[1].pV;
    Verts[2].x = Poly.Vtx_Projected[4].pX;
    Verts[2].y = Poly.Vtx_Projected[4].pY;
    Verts[2].z = Poly.Vtx_Projected[4].pZ * ZNormalize;
    Verts[2].w = 1 / Poly.Vtx_Projected[4].pW;
    Verts[2].u = Poly.Vtx_Projected[4].pU;
    Verts[2].v = Poly.Vtx_Projected[4].pV;
    Verts[3].x = Poly.Vtx_Projected[2].pX;
    Verts[3].y = Poly.Vtx_Projected[2].pY;
    Verts[3].z = Poly.Vtx_Projected[2].pZ * ZNormalize;
    Verts[3].w = 1 / Poly.Vtx_Projected[2].pW;
    Verts[3].u = Poly.Vtx_Projected[2].pU;
    Verts[3].v = Poly.Vtx_Projected[2].pV;
    Verts[4].x = Poly.Vtx_Projected[3].pX;
    Verts[4].y = Poly.Vtx_Projected[3].pY;
    Verts[4].z = Poly.Vtx_Projected[3].pZ * ZNormalize;
    Verts[4].w = 1 / Poly.Vtx_Projected[3].pW;
    Verts[4].u = Poly.Vtx_Projected[3].pU;
    Verts[4].v = Poly.Vtx_Projected[3].pV;
    Buffer->Get_DeviceInterface()->SetTexture(0, Poly.Material.Get_TextureInterface());
    Buffer->Get_DeviceInterface()->DrawPrimitive(D3DPT_TRIANGLESTRIP,
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
float ZNormalize = 1 / FixedToFloat(Buffer->Get_Far());

// Make sure we are in a raster batch and the polygon has a texture:
if (!Flags.InRasterBatch) 
    {
    #ifdef IMR_DEBUG
        IMR_SetErrorText("IMR_Renderer::Draw_Wireframe_Polygon(): Not in raster batch!");
    #endif
    return IMRERR_NONFATAL_NOTINBATCH;
     }
if (!Poly.Material.Get_Texture())
    return IMR_OK;

// Setup the device (if we have just changed states):
if (Flags.DrawMode != IMR_RENDERER_MODE_TEXTUREDLIT)
    {
    Buffer->Get_DeviceInterface()->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    Buffer->Get_DeviceInterface()->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
    Buffer->Get_DeviceInterface()->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
    Buffer->Get_DeviceInterface()->SetRenderState(D3DRENDERSTATE_ZENABLE, D3DZB_TRUE);
    Buffer->Get_DeviceInterface()->SetRenderState(D3DRENDERSTATE_SHADEMODE, D3DSHADE_GOURAUD);
    Buffer->Get_DeviceInterface()->SetRenderState(D3DRENDERSTATE_FILLMODE, D3DFILL_SOLID);
    Flags.DrawMode = IMR_RENDERER_MODE_TEXTUREDLIT;
     }

if (Poly.Num_Verts_Proj == 3)
    {
    Verts[0].x = Poly.Vtx_Projected[0].pX;
    Verts[0].y = Poly.Vtx_Projected[0].pY;
    Verts[0].z = Poly.Vtx_Projected[0].pZ * ZNormalize;
    Verts[0].w = 1 / Poly.Vtx_Projected[0].pW;
    Verts[0].Color = 0xff000000 | (Poly.Vtx_Projected[0].pR << 16) |
                                  (Poly.Vtx_Projected[0].pG << 8) |
                                  (Poly.Vtx_Projected[0].pB);
    Verts[0].u = Poly.Vtx_Projected[0].pU;
    Verts[0].v = Poly.Vtx_Projected[0].pV;
    Verts[1].x = Poly.Vtx_Projected[1].pX;
    Verts[1].y = Poly.Vtx_Projected[1].pY;
    Verts[1].z = Poly.Vtx_Projected[1].pZ * ZNormalize;
    Verts[1].w = 1 / Poly.Vtx_Projected[1].pW;
    Verts[1].Color = 0xff000000 | (Poly.Vtx_Projected[1].pR << 16) |
                                  (Poly.Vtx_Projected[1].pG << 8) |
                                  (Poly.Vtx_Projected[1].pB);
    Verts[1].u = Poly.Vtx_Projected[1].pU;
    Verts[1].v = Poly.Vtx_Projected[1].pV;
    Verts[2].x = Poly.Vtx_Projected[2].pX;
    Verts[2].y = Poly.Vtx_Projected[2].pY;
    Verts[2].z = FixedToFloat(Poly.Vtx_Projected[2].Z) * ZNormalize;
    Verts[2].w = 1 / FixedToFloat(Poly.Vtx_Projected[2].pW);
    Verts[2].Color = 0xff000000 | (Poly.Vtx_Projected[2].pR << 16) |
                                  (Poly.Vtx_Projected[2].pG << 8) |
                                  (Poly.Vtx_Projected[2].pB);
    Verts[2].u = Poly.Vtx_Projected[2].pU;
    Verts[2].v = Poly.Vtx_Projected[2].pV;
    Buffer->Get_DeviceInterface()->SetTexture(0, Poly.Material.Get_TextureInterface());
    Buffer->Get_DeviceInterface()->DrawPrimitive(D3DPT_TRIANGLELIST,  
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
    Verts[0].w = 1 / Poly.Vtx_Projected[0].pW;
    Verts[0].Color = 0xff000000 | (Poly.Vtx_Projected[0].pR << 16) |
                                  (Poly.Vtx_Projected[0].pG << 8) |
                                  (Poly.Vtx_Projected[0].pB);
    Verts[0].u = Poly.Vtx_Projected[0].pU;
    Verts[0].v = Poly.Vtx_Projected[0].pV;
    Verts[1].x = Poly.Vtx_Projected[3].pX;
    Verts[1].y = Poly.Vtx_Projected[3].pY;
    Verts[1].z = Poly.Vtx_Projected[3].pZ * ZNormalize;
    Verts[1].w = 1 / Poly.Vtx_Projected[3].pW;
    Verts[1].Color = 0xff000000 | (Poly.Vtx_Projected[3].pR << 16) |
                                  (Poly.Vtx_Projected[3].pG << 8) |
                                  (Poly.Vtx_Projected[3].pB);
    Verts[1].u = Poly.Vtx_Projected[3].pU;
    Verts[1].v = Poly.Vtx_Projected[3].pV;
    Verts[2].x = Poly.Vtx_Projected[1].pX;
    Verts[2].y = Poly.Vtx_Projected[1].pY;
    Verts[2].z = Poly.Vtx_Projected[1].pZ * ZNormalize;
    Verts[2].w = 1 / Poly.Vtx_Projected[1].pW;
    Verts[2].Color = 0xff000000 | (Poly.Vtx_Projected[1].pR << 16) |
                                  (Poly.Vtx_Projected[1].pG << 8) |
                                  (Poly.Vtx_Projected[1].pB);
    Verts[2].u = Poly.Vtx_Projected[1].pU;
    Verts[2].v = Poly.Vtx_Projected[1].pV;
    Verts[3].x = Poly.Vtx_Projected[2].pX;
    Verts[3].y = Poly.Vtx_Projected[2].pY;
    Verts[3].z = Poly.Vtx_Projected[2].pZ * ZNormalize;
    Verts[3].w = 1 / Poly.Vtx_Projected[2].pW;
    Verts[3].Color = 0xff000000 | (Poly.Vtx_Projected[2].pR << 16) |
                                  (Poly.Vtx_Projected[2].pG << 8) |
                                  (Poly.Vtx_Projected[2].pB);
    Verts[3].u = Poly.Vtx_Projected[2].pU;
    Verts[3].v = Poly.Vtx_Projected[2].pV;
    Buffer->Get_DeviceInterface()->SetTexture(0, Poly.Material.Get_TextureInterface());
    Buffer->Get_DeviceInterface()->DrawPrimitive(D3DPT_TRIANGLESTRIP,  
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
    Verts[0].w = 1 / Poly.Vtx_Projected[0].pW;
    Verts[0].Color = 0xff000000 | (Poly.Vtx_Projected[0].pR << 16) |
                                  (Poly.Vtx_Projected[0].pG << 8) |
                                  (Poly.Vtx_Projected[0].pB);
    Verts[0].u = Poly.Vtx_Projected[0].pU;
    Verts[0].v = Poly.Vtx_Projected[0].pV;
    Verts[1].x = Poly.Vtx_Projected[1].pX;
    Verts[1].y = Poly.Vtx_Projected[1].pY;
    Verts[1].z = Poly.Vtx_Projected[1].pZ * ZNormalize;
    Verts[1].w = 1 / Poly.Vtx_Projected[1].pW;
    Verts[1].Color = 0xff000000 | (Poly.Vtx_Projected[1].pR << 16) |
                                  (Poly.Vtx_Projected[1].pG << 8) |
                                  (Poly.Vtx_Projected[1].pB);
    Verts[1].u = Poly.Vtx_Projected[1].pU;
    Verts[1].v = Poly.Vtx_Projected[1].pV;
    Verts[2].x = Poly.Vtx_Projected[4].pX;
    Verts[2].y = Poly.Vtx_Projected[4].pY;
    Verts[2].z = Poly.Vtx_Projected[4].pZ * ZNormalize;
    Verts[2].w = 1 / Poly.Vtx_Projected[4].pW;
    Verts[2].Color = 0xff000000 | (Poly.Vtx_Projected[4].pR << 16) |
                                  (Poly.Vtx_Projected[4].pG << 8) |
                                  (Poly.Vtx_Projected[4].pB);
    Verts[2].u = Poly.Vtx_Projected[4].pU;
    Verts[2].v = Poly.Vtx_Projected[4].pV;
    Verts[3].x = Poly.Vtx_Projected[2].pX;
    Verts[3].y = Poly.Vtx_Projected[2].pY;
    Verts[3].z = Poly.Vtx_Projected[2].pZ * ZNormalize;
    Verts[3].w = 1 / Poly.Vtx_Projected[2].pW;
    Verts[3].Color = 0xff000000 | (Poly.Vtx_Projected[2].pR << 16) |
                                  (Poly.Vtx_Projected[2].pG << 8) |
                                  (Poly.Vtx_Projected[2].pB);
    Verts[3].u = Poly.Vtx_Projected[2].pU;
    Verts[3].v = Poly.Vtx_Projected[2].pV;
    Verts[4].x = Poly.Vtx_Projected[3].pX;
    Verts[4].y = Poly.Vtx_Projected[3].pY;
    Verts[4].z = Poly.Vtx_Projected[3].pZ * ZNormalize;
    Verts[4].w = 1 / Poly.Vtx_Projected[3].pW;
    Verts[4].Color = 0xff000000 | (Poly.Vtx_Projected[3].pR << 16) |
                                  (Poly.Vtx_Projected[3].pG << 8) |
                                  (Poly.Vtx_Projected[3].pB);
    Verts[4].u = Poly.Vtx_Projected[3].pU;
    Verts[4].v = Poly.Vtx_Projected[3].pV;
    Buffer->Get_DeviceInterface()->SetTexture(0, Poly.Material.Get_TextureInterface());
    Buffer->Get_DeviceInterface()->DrawPrimitive(D3DPT_TRIANGLESTRIP,
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
  Ends a rasterization batch.
  Returns IMR_OK if successful, otherwise an error.
\***************************************************************************/
int IMR_Renderer::End_Raster_Batch(void)
{
// Make sure we are already in a raster batch:
if (!Flags.InRasterBatch) 
    {
    #ifdef IMR_DEBUG
        IMR_SetErrorText("IMR_Renderer::End_Raster_Batch(): Not in raster batch!");
    #endif
    return IMRERR_NONFATAL_NOTINBATCH;
     }

// Setup flags:
Flags.InRasterBatch = 0;

// End the scene:
Buffer->Get_DeviceInterface()->EndScene();
                                              
// Nullify all pointers:
Camera = NULL;
Buffer = NULL;
Data = NULL;

// And return ok:
return IMR_OK;
 }

