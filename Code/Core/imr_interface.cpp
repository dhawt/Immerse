/****************************************************************\
 
 iMMERSE Engine
 (C) 1999 No Tears Shed Software
 All rights reserved
 
 Filename: IMR_Interface.hpp
 Description: Immerse engine main interface module
 
\****************************************************************/
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

// Initialize the renderer:
err = Renderer.Init(256, InitData.GeometryInfo.MaxPolys);
if (IMR_ISNOTOK(err)) return err;

// Setup our default rendering info:
SetDrawOpt_MipMap(0);
SetDrawOpt_Filtering(1);

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
Renderer.Shutdown();

// If we have a thread handle, close it:
if (DrawThreadHandle)
    {
    CloseHandle(DrawThreadHandle);
    DrawThreadHandle = NULL;
     }

// Reset everything:
Flags.ClassInitialized = 0;
Flags.ScreenInitialized = 0;
Flags.WindowInitialized = 0;

// And return ok:
return IMR_OK;
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
    IMR_LogMsg(__LINE__, __FILE__, "IMR_Interface::Begin_Frame(): Still drawing!");
    return IMRERR_NOTREADY;
     }

// Make sure we are completely initialized:
if (!Flags.ScreenInitialized || !Flags.WindowInitialized)
    {
    IMR_LogMsg(__LINE__, __FILE__, "IMR_Interface::Begin_Frame(): Screen or target not initialized!");
    return IMRERR_NOTREADY;
     }

// Make sure we aren't in a frame:
if (Flags.InFrame)
    {
    #ifdef IMR_DEBUG
        IMR_LogMsg(__LINE__, __FILE__, "IMR_Interface::Begin_Frame(): Already in frame!");
    #endif
    return IMRERR_NONFATAL_INFRAME;
     }

// Setup the pipeline:
err = Pipeline.SetupFrame(Cam, Renderer); if (err != IMR_OK) return err;

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
    IMR_LogMsg(__LINE__, __FILE__, "IMR_Interface::Add_Object(): Still drawing!");
    return IMRERR_NOTREADY;
     }

// Make sure we are in a frame:
if (!Flags.InFrame)
    {
    #ifdef IMR_DEBUG
        IMR_LogMsg(__LINE__, __FILE__, "IMR_Interface::Add_Object(): Not in frame!");
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
    IMR_LogMsg(__LINE__, __FILE__, "IMR_Interface::Add_Model(): Still drawing!");
    return IMRERR_NOTREADY;
     }

// Make sure we are in a frame:
if (!Flags.InFrame)
    {
    #ifdef IMR_DEBUG
        IMR_LogMsg(__LINE__, __FILE__, "IMR_Interface::Add_Model(): Not in frame!");
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
    IMR_LogMsg(__LINE__, __FILE__, "IMR_Interface::Draw_Frame(): Still drawing!");
    return IMRERR_NOTREADY;
     }

// Make sure we are completely initialized:
if (!Flags.ScreenInitialized || !Flags.WindowInitialized)
    {
    IMR_LogMsg(__LINE__, __FILE__, "IMR_Interface::Begin_Frame(): Screen or buffer not initialized!");
    return IMRERR_NOTREADY;
     }

// Make sure we are in a frame:
if (!Flags.InFrame)
    {
    #ifdef IMR_DEBUG
        IMR_LogMsg(__LINE__, __FILE__, "IMR_Interface::Draw_Frame(): Not in frame!");
    #endif
    return IMRERR_NONFATAL_NOTINFRAME;
     }

// Perform pipeline operations:
err = Pipeline.Illuminate(); if (err != IMR_OK) return err;
err = Pipeline.Transform(); if (err != IMR_OK) return err;
err = Pipeline.Cull(); if (err != IMR_OK) return err;
err = Pipeline.ClipAndProject(); if (err != IMR_OK) return err;

// Now draw the frame.  If asynchronous drawing is enabled, spawn a thread:
if (Flags.DrawAsynchronous)
    {
    DrawThreadHandle = CreateThread(NULL, 0, &Pipeline.Async_DrawFrame, (void *)&Pipeline, 0, &DrawThreadID);
     }
else
    err = Pipeline.DrawFrame(); if (err != IMR_OK) return err;

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
    IMR_LogMsg(__LINE__, __FILE__, "IMR_Interface::End_Frame(): Still drawing!");
    return IMRERR_NOTREADY;
     }

// Make sure we are completely initialized:
if (!Flags.ScreenInitialized || !Flags.WindowInitialized)
    {
    IMR_LogMsg(__LINE__, __FILE__, "IMR_Interface::End_Frame(): Screen or buffer not initialized!");
    return IMRERR_NODATA;
     }

// Make sure we are in a frame:
if (!Flags.InFrame)
    {
    #ifdef IMR_DEBUG
        IMR_LogMsg(__LINE__, __FILE__, "IMR_Interface::End_Frame(): Not in frame!");
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
    IMR_LogMsg(__LINE__, __FILE__, "IMR_Interface::Blit_Frame(): Still drawing!");
    return IMRERR_NOTREADY;
     }

// Make sure we are completely initialized:
if (!Flags.ScreenInitialized || !Flags.WindowInitialized)
    {
    IMR_LogMsg(__LINE__, __FILE__, "IMR_Interface::End_Frame(): Screen or buffer not initialized!");
    return IMRERR_NOTREADY;
     }

// Make sure we are not in a frame:
if (Flags.InFrame)
    {
    #ifdef IMR_DEBUG
        IMR_LogMsg(__LINE__, __FILE__, "IMR_Interface::Blit_Frame(): Currently in frame!");
    #endif
    return IMRERR_NONFATAL_INFRAME;
     }

// Blit the buffer to the screen:
err = Renderer.Target_Blit();
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
    IMR_LogMsg(__LINE__, __FILE__, "IMR_Interface::Flip_Buffers(): Screen not initialized!");
    return IMRERR_NOTREADY;
     }

// Make sure we are not in a frame:
if (Flags.InFrame)
    {
    #ifdef IMR_DEBUG
        IMR_LogMsg(__LINE__, __FILE__, "IMR_Interface::Flip_Buffers(): In frame!");
    #endif
    return IMRERR_NONFATAL_INFRAME;
     }

// Flip the buffers and return what we get:
return Renderer.Target_Flip();
 }

/***************************************************************************\
  Sets the screen resolution.
  Returns IMR_OK if successful, otherwise an error.
\***************************************************************************/
int IMR_Interface::Set_Screen(int W, int H, HWND hWnd)
{
// If we are in asynchronous mode, make sure we are done drawing:
if (Flags.DrawAsynchronous && Pipeline.Async_IsDrawing())
    {
    IMR_LogMsg(__LINE__, __FILE__, "IMR_Interface::Set_Screen(): Still drawing!");
    return IMRERR_NOTREADY;
     }

int err = Renderer.Set_Screen(W, H, hWnd);
if (IMR_ISNOTOK(err)) return err;
Flags.ScreenInitialized = 1;
return IMR_OK;
 }

/***************************************************************************\
  Sets the render window.
  Should be reset when the screen resolution changes.
  Returns IMR_OK if successful, otherwise an error.
\***************************************************************************/
int IMR_Interface::Set_Window(int x0, int y0, int x1, int y1)
{
int err;

// If we are in asynchronous mode, make sure we are done drawing:
if (Flags.DrawAsynchronous && Pipeline.Async_IsDrawing())
    {
    IMR_LogMsg(__LINE__, __FILE__, "IMR_Interface::Set_RenderTarget(): Still drawing!");
    return IMRERR_NOTREADY;
     }

err = Renderer.Set_Window(x0, y0, x1, y1);
if (IMR_ISNOTOK(err)) return err;
Flags.WindowInitialized = 1;
return IMR_OK;
 }

/***************************************************************************\
  Loads the specified PCX file into the specified texture.
  Returns IMR_OK if successful, an error otherwise.
\***************************************************************************/
/*
int IMR_Interface::Texture_LoadFromPCX(char *Filename, char *TexName)
{
char Byte, HeaderByte, ColorByte, Buffer[10], *Data;
IMR_PaletteEntry Pal[256];
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
Ref = Renderer.Texture_Add(TexName);
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
if (IMR_ISNOTOK(err = Renderer.Texture_Gen(Ref)))
    {
    IMR_LogMsg(__LINE__, __FILE__, "Texture generation failed for texture %s!", TexName);
    return err;
     }

// Now get a refrence with the texture data:
Ref = Renderer.Texture_GetData(TexName);

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
while (Py < Height)
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
            short Temp = 0;
            Temp |= (short)(Pal[ColorByte].R >> 4) << 8;
            Temp |= (short)(Pal[ColorByte].G >> 4) << 4;
            Temp |= (short)(Pal[ColorByte].B >> 4);
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
        short Temp = 0;//(short)Data[Pos];
        Temp |= (short)(Pal[ColorByte].R >> 4) << 8;
        Temp |= (short)(Pal[ColorByte].G >> 4) << 4;
        Temp |= (short)(Pal[ColorByte].B >> 4);
        Data[Pos ++] = Temp & 0x00ff;
        Data[Pos ++] = Temp >> 8;
         }
     }

// Return the texture data:
Renderer.Texture_ReturnData(Ref);

// Close the file:
IMR_Resources.Get_RDFManager()->CloseRDF(PcxFile);

// And return ok:
return IMR_OK;
 }
*/
