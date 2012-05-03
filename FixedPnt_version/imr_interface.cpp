/***************************************************************************\
  File: IMR_Interface.cpp
  Description: iMERRSE engine interface module.
  Author: Daniel Hawthorn
  Modified:
  
\***************************************************************************/
#include "IMR_Interface.hpp"

/***************************************************************************\
  Initializes the interface using the specified init structure.

  Returns IMR_OK if successful, otherwise an error.
\***************************************************************************/
int IMR_Interface::Init(IMR_InterfaceInitData &InitData)
{
int err;

// Shutdown...
Shutdown();

// Setup our effects:
GlobalSettings.Effects.LightingEnabled = IMR_ON;
GlobalSettings.Effects.ShadowsEnabled = IMR_OFF;

// Setup our rendering info:
GlobalSettings.Renderer.MipMappingEnabled = IMR_OFF;
GlobalSettings.Renderer.FilteringEnabled = IMR_OFF;
GlobalSettings.Renderer.FilledPolysEnabled = IMR_ON;
GlobalSettings.Renderer.TexturesEnabled = IMR_ON;

// Initialize the pipeline.  Return an error if it failed:
err = Pipeline.Init(InitData.GeometryInfo.MaxVerts, 
                    InitData.GeometryInfo.MaxPolys, 
                    InitData.GeometryInfo.MaxLights);
if (IMR_ISNOTOK(err)) return err;

// Flag that this class has been initialized:
Flags.ClassInitialized = 1;

// And return ok:
return IMR_OK;
 }

/***************************************************************************\
  Shuts down the interface.  
  Returns IMR_OK if successful, otherwise an error.
\***************************************************************************/
int IMR_Interface::Shutdown(void)
{
if (!Flags.ClassInitialized)
    return IMR_OK;

// Shut down everything:
RendCore.Shutdown();
RendBuffer.Shutdown();

// If we have a thread handle, close it:
if (DrawThreadHandle)
    {
    CloseHandle(DrawThreadHandle);
    DrawThreadHandle = NULL;
     }

// Reset everything:
Flags.ClassInitialized = 0;
Flags.ScreenInitialized = 0;
Flags.TargetInitialized = 0;

// And return ok:
return IMR_OK;
 }

/***************************************************************************\
  Adds the specified texture to the specified list. 
  Returns IMR_OK if successful, otherwise an error.
\***************************************************************************/
int IMR_Interface::Add_Texture(int List, char *Name)
{
int err;
IMR_Texture *Temp;

// Default to local list:
if (List == IMR_LIST_GLOBAL)
    Temp = GlobalTextures.Add_Item(Name);
else
    Temp = LocalTextures.Add_Item(Name);

// If we couldn't add the item, return an error:
if (!Temp)
    {
    IMR_SetErrorText("IMR_Interface::Add_Texture(): Couldn't add texture!");
    return IMRERR_TOMANY;
     }

// And return ok:
return IMR_OK;
 }

/***************************************************************************\
  Returns a pointer to the specified texture.
  Checks the local list first.  If not found there, checks the global list.
  If the texture doesn't exist in either list, returns null.
\***************************************************************************/
IMR_Texture *IMR_Interface::Get_Texture(char *Name)
{
IMR_Texture *Temp;

// Check the local list:
Temp = LocalTextures.Get_Item(Name, NULL);

// Didn't find it there?  Check the global list:
if (!Temp)
    Temp = GlobalTextures.Get_Item(Name, NULL);

// And return a pointer to the texture (which, if you recall, is NULL if it 
// couldn't be found):
return Temp;
 }

/***************************************************************************\
  Sets up the frame for use with the specified camera.
  Returns IMR_OK if successful, otherwise an error.
\***************************************************************************/
int IMR_Interface::Begin_Frame(IMR_Camera &Cam)
{
int err;

// If we are in asynchronous mode, make sure we are done drawing:
if (Flags.DrawAsynchronous && Pipeline.Async_IsDrawing())
    {
    IMR_SetErrorText("IMR_Interface::Begin_Frame(): Still drawing!");
    return IMRERR_NOTREADY;
     }

// Make sure we are completely initialized:
if (!Flags.ScreenInitialized || !Flags.TargetInitialized)
    {
    IMR_SetErrorText("IMR_Interface::Begin_Frame(): Screen or buffer not initialized!");
    return IMRERR_NOTREADY;
     }

// Make sure we aren't in a frame:
if (Flags.InFrame)
    {
    #ifdef IMR_DEBUG
        IMR_SetErrorText("IMR_Interface::Begin_Frame(): Already in frame!");
    #endif
    return IMRERR_NONFATAL_INFRAME;
     }

// Setup the pipeline:
err = Pipeline.SetupFrame(Cam, RendBuffer); if (err != IMR_OK) return err;

// And flag that we're ready to go:
Flags.InFrame = 1;

// And return ok:
return IMR_OK;
 }

/***************************************************************************\
  Adds the specified object and it's children to the list.
  Returns IMR_OK if successful, otherwise an error.
\***************************************************************************/
int IMR_Interface::Add_Object(IMR_Object &Obj)
{
// If we are in asynchronous mode, make sure we are done drawing:
if (Flags.DrawAsynchronous && Pipeline.Async_IsDrawing())
    {
    IMR_SetErrorText("IMR_Interface::Add_Object(): Still drawing!");
    return IMRERR_NOTREADY;
     }

// Make sure we are in a frame:
if (!Flags.InFrame)
    {
    #ifdef IMR_DEBUG
        IMR_SetErrorText("IMR_Interface::Add_Object(): Not in frame!");
    #endif
    return IMRERR_NONFATAL_NOTINFRAME;
     }

// Add the object to the pipeline:
return Pipeline.Add_Object(Obj);
 }

/***************************************************************************\
  Adds the specified model to the list.
  Returns IMR_OK if successful, otherwise an error.
\***************************************************************************/
int IMR_Interface::Add_Model(IMR_Model &Mod, IMR_3DPoint &Pos, IMR_Attitude &Atd)
{
// If we are in asynchronous mode, make sure we are done drawing:
if (Flags.DrawAsynchronous && Pipeline.Async_IsDrawing())
    {
    IMR_SetErrorText("IMR_Interface::Add_Model(): Still drawing!");
    return IMRERR_NOTREADY;
     }

// Make sure we are in a frame:
if (!Flags.InFrame)
    {
    #ifdef IMR_DEBUG
        IMR_SetErrorText("IMR_Interface::Add_Model(): Not in frame!");
    #endif
    return IMRERR_NONFATAL_NOTINFRAME;
     }

// Add the model the the pipeline and return what we get:
return Pipeline.Add_Model(Mod, Pos, Atd);
 }

/***************************************************************************\
  Draws the frame in the buffer.
  Returns IMR_OK if successful, otherwise an error.
\***************************************************************************/
int IMR_Interface::Draw_Frame(void)
{
int err;

// If we are in asynchronous mode, make sure we are done drawing:
if (Flags.DrawAsynchronous && Pipeline.Async_IsDrawing())
    {
    IMR_SetErrorText("IMR_Interface::Draw_Frame(): Still drawing!");
    return IMRERR_NOTREADY;
     }

// Make sure we are completely initialized:
if (!Flags.ScreenInitialized || !Flags.TargetInitialized)
    {
    IMR_SetErrorText("IMR_Interface::Begin_Frame(): Screen or buffer not initialized!");
    return IMRERR_NOTREADY;
     }

// Make sure we are in a frame:
if (!Flags.InFrame)
    {
    #ifdef IMR_DEBUG
        IMR_SetErrorText("IMR_Interface::Draw_Frame(): Not in frame!");
    #endif
    return IMRERR_NONFATAL_NOTINFRAME;
     }

// Perform pipeline operations:
CurrentSettings = &GlobalSettings;
err = Pipeline.Illuminate(); if (err != IMR_OK) return err;
err = Pipeline.Transform(); if (err != IMR_OK) return err;
err = Pipeline.Cull(); if (err != IMR_OK) return err;
err = Pipeline.ClipAndProject(); if (err != IMR_OK) return err;

// Now draw the frame.  If asynchronous drawing is enabled, spawn a thread:
if (Flags.DrawAsynchronous)
    {
    Pipeline.Async_SetRenderer(RendCore);
    DrawThreadHandle = CreateThread(NULL, 0, &IMR_Pipeline_Async_DrawFrame, (void *)&Pipeline, 0, &DrawThreadID);
     }    
else
    err = Pipeline.DrawFrame(RendCore); if (err != IMR_OK) return err;

// And return ok:
return IMR_OK;
 }

/***************************************************************************\
  Ends the frame and cleans up stuff.
  Returns IMR_OK if successful, otherwise an error.
\***************************************************************************/
int IMR_Interface::End_Frame(void)
{
int err;

// If we are in asynchronous mode, make sure we are done drawing:
if (Flags.DrawAsynchronous && Pipeline.Async_IsDrawing())
    {
    IMR_SetErrorText("IMR_Interface::End_Frame(): Still drawing!");
    return IMRERR_NOTREADY;
     }

// Make sure we are completely initialized:
if (!Flags.ScreenInitialized || !Flags.TargetInitialized)
    {
    IMR_SetErrorText("IMR_Interface::End_Frame(): Screen or buffer not initialized!");
    return IMRERR_NODATA;
     }

// Make sure we are in a frame:
if (!Flags.InFrame)
    {
    #ifdef IMR_DEBUG
        IMR_SetErrorText("IMR_Interface::End_Frame(): Not in frame!");
    #endif
    return IMRERR_NONFATAL_NOTINFRAME;
     }

// If we are in asynchronous mode, destroy the thread handle:
CloseHandle(DrawThreadHandle);
DrawThreadHandle = NULL;

// Flag that we are done with the frame:
Flags.InFrame = 0;

// And return ok:
return IMR_OK;
 }

/***************************************************************************\
  Blits the frame to the back buffer.
  Returns IMR_OK if successful, otherwise an error.
\***************************************************************************/
int IMR_Interface::Blit_Frame(void)
{
int err;

// If we are in asynchronous mode, make sure we are done drawing:
if (Flags.DrawAsynchronous && Pipeline.Async_IsDrawing())
    {
    IMR_SetErrorText("IMR_Interface::Blit_Frame(): Still drawing!");
    return IMRERR_NOTREADY;
     }

// Make sure we are completely initialized:
if (!Flags.ScreenInitialized || !Flags.TargetInitialized)
    {
    IMR_SetErrorText("IMR_Interface::End_Frame(): Screen or buffer not initialized!");
    return IMRERR_NOTREADY;
     }

// Make sure we are not in a frame:
if (Flags.InFrame)
    {
    #ifdef IMR_DEBUG
        IMR_SetErrorText("IMR_Interface::Blit_Frame(): Currently in frame!");
    #endif
    return IMRERR_NONFATAL_INFRAME;
     }

// Blit the buffer to the screen:
CurrentSettings = &GlobalSettings;
err = RendBuffer.Blit();
if (IMR_ISNOTOK(err)) return err;

// And return ok:
return IMR_OK;
 }

/***************************************************************************\
  Flips the primary surface and the back buffer.
  Returns IMR_OK if successful, otherwise an error.
\***************************************************************************/
int IMR_Interface::Flip_Buffers(void)
{
int err;

// Make sure we are completely initialized:
if (!Flags.ScreenInitialized)
    {
    IMR_SetErrorText("IMR_Interface::Flip_Buffers(): Screen not initialized!");
    return IMRERR_NOTREADY;
     }

// Make sure we are not in a frame:
if (Flags.InFrame)
    {
    #ifdef IMR_DEBUG
        IMR_SetErrorText("IMR_Interface::Flip_Buffers(): In frame!");
    #endif
    return IMRERR_NONFATAL_INFRAME;
     }

// Flip the buffers and return what we get:
return RendBuffer.Flip();
 }

/***************************************************************************\
  Sets the screen resolution.
  Returns IMR_OK if successful, otherwise an error.
\***************************************************************************/
int IMR_Interface::Set_Screen(int Width, int Height, HWND hWnd)
{
// If we are in asynchronous mode, make sure we are done drawing:
if (Flags.DrawAsynchronous && Pipeline.Async_IsDrawing())
    {
    IMR_SetErrorText("IMR_Interface::Set_Screen(): Still drawing!");
    return IMRERR_NOTREADY;
     }

if (Width <= 0 || Height <= 0 || !hWnd)
    {
    IMR_SetErrorText("IMR_Interface::Set_Screen(): Bad buffer specified!");
    return IMRERR_BADBUFFER;
     }
GlobalSettings.Screen.Width = Width;
GlobalSettings.Screen.Height = Height;
GlobalSettings.Screen.hWnd = hWnd;
int err = RendBuffer.Set_Screen(Width, Height, hWnd, &DirectX);
if (IMR_ISNOTOK(err)) return err;
Flags.ScreenInitialized = 1;
return IMR_OK;
 }

/***************************************************************************\
  Sets the render target resolution.
  Should be reset when the screen resolution changes.
  Returns IMR_OK if successful, otherwise an error.
\***************************************************************************/
int IMR_Interface::Set_RenderTarget(int Width, int Height)
{
int err;

// If we are in asynchronous mode, make sure we are done drawing:
if (Flags.DrawAsynchronous && Pipeline.Async_IsDrawing())
    {
    IMR_SetErrorText("IMR_Interface::Set_RenderTarget(): Still drawing!");
    return IMRERR_NOTREADY;
     }

if (Width <= 0 || Height <= 0)
    {
    IMR_SetErrorText("IMR_Interface::Set_RenderTarget(): Bad buffer specified!");
    return IMRERR_BADBUFFER;
     }
GlobalSettings.Target.Width = Width;
GlobalSettings.Target.Height = Height;
if (IMR_ISNOTOK(err = RendCore.Init())) return err;
err = RendBuffer.Set_RenderTarget(Width, Height, &DirectX);
if (IMR_ISNOTOK(err)) return err;
Flags.TargetInitialized = 1;
return IMR_OK;
 }

/***************************************************************************\
  Sets the lens.
  Returns IMR_OK if successful, otherwise an error.
\***************************************************************************/
int IMR_Interface::Set_Lens(int Zm, int Nr, int Fr)
{
// If we are in asynchronous mode, make sure we are done drawing:
if (Flags.DrawAsynchronous && Pipeline.Async_IsDrawing())
    {
    IMR_SetErrorText("IMR_Interface::Set_Lens(): Still drawing!");
    return IMRERR_NOTREADY;
     }

if (Zm)
    {
    GlobalSettings.Lens.Zoom = Zm;
    RendBuffer.Set_Zoom(Zm);
     }
if (Nr)
    {
    GlobalSettings.Lens.Near = Nr;
    RendBuffer.Set_Near(Nr);
     }
if (Fr)
    {
    GlobalSettings.Lens.Far = Fr;
    RendBuffer.Set_Far(Fr);
     }
return IMR_OK;
 }
