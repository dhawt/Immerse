/***************************************************************************\
   Collision detecting testing module
\***************************************************************************/
// Pre-compile windows.h:
#include <windows.h>

// Setup iMMERSE debugging:
#define IMR_DEBUG

#include "Demo.HPP"

// Miscellaneous:
HANDLE ThisInstance;
BOOL Running = TRUE, Active = TRUE;
HWND GameWindow;
BigFont MainFont;
//Palette Palette;

// Windows interfaces:
WinKeyboardSys KB;

// iMMERSE interface:
//IMR_Interface Immerse;
IMR_GM_Interface Immerse;

/***************************************************************************\
  Performes cleanup stuff.
\***************************************************************************/
void Cleanup(void)
{
Immerse.Shutdown();
IMR_EndLogging();
ShowWindow(GameWindow, SW_SHOWMAXIMIZED);
 }
 
/***************************************************************************\
  Shuts down everything.
\***************************************************************************/
void Quit(char *String)
{
int error = 0;   // Error flag for DOS
va_list arglist;

// Shutdown everything:
Cleanup();

// Show error message:
if (String && *String)
    {
    MessageBox(GetActiveWindow(), String, "Error!", MB_OK | MB_ICONSTOP);
    error = 1;
     }

exit(error);
 }

/***************************************************************************\
  Initializes all windows app stuff.
  Returns false if an error occures.
\***************************************************************************/
void InitWindowApp(void)
{
WNDCLASS WndClass;

// Setup window:
WndClass.cbClsExtra = 0;
WndClass.cbWndExtra = 0;
WndClass.hbrBackground = GetStockObject(BLACK_BRUSH);
WndClass.hCursor = NULL;
WndClass.hIcon = LoadIcon(ThisInstance, NULL);
WndClass.hInstance = ThisInstance;
WndClass.lpfnWndProc = WndProc;
WndClass.lpszMenuName = NULL;
WndClass.lpszClassName = "Immerse";
WndClass.style = NULL;
if (!RegisterClass(&WndClass)) Quit("Register class failed!");

// Create the window and display it:
GameWindow = CreateWindowEx(0,
                            "Immerse",
                            "iMMERSE Testing App",
                            WS_POPUP | WS_SYSMENU,
                            0, 0,
                            GetSystemMetrics(SM_CXSCREEN),
                            GetSystemMetrics(SM_CYSCREEN),
                            NULL,
                            NULL,
                            ThisInstance,
                            NULL);
ShowWindow(GameWindow, SW_SHOWMAXIMIZED);
 }


/***************************************************************************\
  Main. Duh.
\***************************************************************************/
int WINAPI WinMain(HANDLE hInstance, HANDLE hPrevInstance,
                    LPSTR lpszCmdParam, int nCmdShow)
{
int index, done = 0, err, ClearNeeded = 2, FrameDisplayed = 1;
char *Data, Buffer[80];
MSG Message;
IMR_3DPoint Pos;
IMR_Attitude Atd;
// --- Debugging stuff:
IMR_Pipeline *Pipe;
IMR_3DPoint *Verts;
IMR_Polygon *Polys;
int vindex, pindex, NumVerts, NumPolys;
int TimeDiff, StartTime, EndTime;
float fps;

// Initialize window:
ThisInstance = hInstance;
InitWindowApp();

// Start logging:
if (IMR_ISNOTOK(IMR_StartLogging("C:\\Windows\\Desktop\\Immerse execution log.txt"))) Quit("Logging failed!");

// Setup Immerse:
IMR_InterfaceInitData IMRInitData;
IMRInitData.GeometryInfo.MaxVerts = 1600;
IMRInitData.GeometryInfo.MaxPolys = 400;
IMRInitData.GeometryInfo.MaxLights = 10;
if (IMR_ISNOTOK(Immerse.Init(IMRInitData))) Quit("Error!");
if (IMR_ISNOTOK(Immerse.Set_Screen(640, 480, GameWindow))) Quit("Error!");
if (IMR_ISNOTOK(Immerse.Set_Window(0, 0, 639, 479))) Quit("Error!");
if (IMR_ISNOTOK(Immerse.Set_AsyncEnable(1))) Quit("Error!");
Immerse.Set_WorldScale(100);            // Set scale to 100 units/meter, i.e. 1 unit = 1 cm

// Load our resources:
Immerse.LoadRIF("Data\\Resources.rif");

// ***** Add the textures *****
    
    err = Immerse.Texture_LoadFromPCX("Concrete.pcx", "Concrete"); if (IMR_ISNOTOK(err)) Quit("Error!");
    err = Immerse.Texture_LoadFromPCX("Wood.pcx", "Wood"); if (IMR_ISNOTOK(err)) Quit("Error!");
    err = Immerse.Texture_LoadFromPCX("White.pcx", "White"); if (IMR_ISNOTOK(err)) Quit("Error!");

// ***** Create the geometries *****

    err = Immerse.Add_Model(IMR_LIST_LOCAL, "Tracer"); if (IMR_ISNOTOK(err)) Quit("Error!");
    err = Immerse.Get_Model("Tracer")->Make_Cube(5, "Tracer"); if (IMR_ISNOTOK(err)) Quit("Error!");
    err = Immerse.Get_Model("Tracer")->Paint("White");
    err = Immerse.Add_Object(IMR_LIST_GLOBAL, "Tracer1"); if (IMR_ISNOTOK(err)) Quit("Error!");
    Immerse.Get_Object("Tracer1")->Set_ModelName("Tracer");
    Pos.X = 0; Pos.Y = 0; Pos.Z = 0;
    Atd.X = 0; Atd.Y = 0; Atd.Z = 0;
    Immerse.Get_Object("Tracer1")->Set_RelativePos(Pos);
    Immerse.Get_Object("Tracer1")->Set_RelativeAtd(Atd);
    err = Immerse.Add_Object(IMR_LIST_GLOBAL, "Tracer2"); if (IMR_ISNOTOK(err)) Quit("Error!");
    Immerse.Get_Object("Tracer2")->Set_ModelName("Tracer");
    Pos.X = 0; Pos.Y = 0; Pos.Z = 0;
    Atd.X = 0; Atd.Y = 0; Atd.Z = 0;
    Immerse.Get_Object("Tracer2")->Set_RelativePos(Pos);
    Immerse.Get_Object("Tracer2")->Set_RelativeAtd(Atd);

    err = Immerse.Add_Model(IMR_LIST_LOCAL, "Panel"); if (IMR_ISNOTOK(err)) Quit("Error!");
    err = Immerse.Get_Model("Panel")->Make_Wall(100, 100, 100, "Panel"); if (IMR_ISNOTOK(err)) Quit("Error!");
    err = Immerse.Get_Model("Panel")->Paint("Wood");
    err = Immerse.Add_Object(IMR_LIST_GLOBAL, "TestPnl1"); if (IMR_ISNOTOK(err)) Quit("Error!");
    Pos.X = 0; Pos.Y = 0; Pos.Z = 300;
    Atd.X = 128; Atd.Y = 0; Atd.Z = 0;
    Immerse.Get_Object("TestPnl1")->Set_ModelName("Panel");
    Immerse.Get_Object("TestPnl1")->Set_RelativePos(Pos);
    Immerse.Get_Object("TestPnl1")->Set_RelativeAtd(Atd);
    Immerse.Get_Object("TestPnl1")->Set_Collidable();
    err = Immerse.Add_Object(IMR_LIST_GLOBAL, "TestPnl2"); if (IMR_ISNOTOK(err)) Quit("Error!");
    Pos.X = 50; Pos.Y = 0; Pos.Z = 250;
    Atd.X = 128; Atd.Y = 256; Atd.Z = 0;
    Immerse.Get_Object("TestPnl2")->Set_ModelName("Panel");
    Immerse.Get_Object("TestPnl2")->Set_RelativePos(Pos);
    Immerse.Get_Object("TestPnl2")->Set_RelativeAtd(Atd);
    Immerse.Get_Object("TestPnl2")->Set_Collidable();

    // Create models:
    err = Immerse.Add_Model(IMR_LIST_GLOBAL, "Head"); if (IMR_ISNOTOK(err)) Quit("Error!");
    err = Immerse.Get_Model("Head")->Make_Pyramid(23, 23, 23, "Head"); if (IMR_ISNOTOK(err)) Quit("Error!");
    err = Immerse.Get_Model("Head")->Paint("Concrete");

    // Setup the person:
    err = Immerse.Add_Object(IMR_LIST_GLOBAL, "Person"); if (IMR_ISNOTOK(err)) Quit("Error!");
    Pos.X = 0; Pos.Y = 0; Pos.Z = 0;
    Atd.X = 0; Atd.Y = 0; Atd.Z = 0;
    Immerse.Get_Object("Person")->Set_ModelName("Head");
    Immerse.Get_Object("Person")->Set_RelativePos(Pos);
    Immerse.Get_Object("Person")->Set_RelativeAtd(Atd);

    // Attach the objects:
    Immerse.Attach_Object("Person", NULL);
    Immerse.Attach_Object("Tracer1", NULL);
    Immerse.Attach_Object("Tracer2", NULL);
    Immerse.Attach_Object("TestPnl1", NULL);
    //Immerse.Attach_Object("TestPnl2", NULL);

    // Create the lights:
    Immerse.Add_Light(0);       // Ambient light
    Immerse.Get_Light(0)->Set_Type(IMR_LIGHT_AMBIENT);
    Immerse.Get_Light(0)->Set_Color(0.5f, 0.7f, 0.7f);
    Immerse.Get_Object(NULL)->Attach_Light(Immerse.Get_Light(0));

    Immerse.Add_Light(1);       // Person light
    Immerse.Get_Light(1)->Set_Type(IMR_LIGHT_POINT);
    Immerse.Get_Light(1)->Set_Color(1.0f, 0.1f, 0.8f);
    Pos.X = 0; Pos.Y = 0; Pos.Z = 0;
    Immerse.Get_Light(1)->Set_Position(Pos);
    Immerse.Get_Light(1)->Set_Range(600);
    Immerse.Get_Object("Person")->Attach_Light(Immerse.Get_Light(1));

    // Now prep everything:
    err = Immerse.Prep_Geometries(); if (IMR_ISNOTOK(err)) Quit("Error!");

    // Create our camera:
    err = Immerse.Add_Object(IMR_LIST_GLOBAL, "Tripod"); if (IMR_ISNOTOK(err)) Quit("Error!");
    if (!Immerse.Add_Camera("CndidCam")) Quit("Camera creation failed!");
    Pos.X = 0; Pos.Y = 0; Pos.Z = -300;
    Atd.X = 0; Atd.Y = 0; Atd.Z = 0;
    Immerse.Get_Camera("CndidCam")->Obj_Attach(Immerse.Get_Object("Tripod"));
    Immerse.Get_Camera("CndidCam")->Set_Pos(Pos);
    Immerse.Get_Camera("CndidCam")->Set_Atd(Atd);
    Immerse.Get_Camera("CndidCam")->Lens_Set_Near(100);
    Immerse.Get_Camera("CndidCam")->Lens_Set_Far(2000);
    Immerse.Get_Camera("CndidCam")->Lens_Set_Zoom(IMR_ZOOM640480);
    IMR_CameraOp CameraMan;
    CameraMan.Cam_Set(Immerse.Get_Camera("CndidCam"));
    CameraMan.Vcn_SetTargetObj(Immerse.Get_Object("Person"));
    CameraMan.Vcn_SetParam(IMR_CAMERAOP_VCN_MODE, IMR_CAMERAOP_VCN_MODE_CHASE);
    CameraMan.Vcn_SetParam(IMR_CAMERAOP_VCN_FOLLOWTIME, 250);
    Pos.X = 0; Pos.Y = 0; Pos.Z = -300; CameraMan.Vcn_SetPnt1(Pos);
    Atd.X = 64; Atd.Y = 0; Atd.Z = 0; CameraMan.Vcn_SetAtd1(Atd);

// *****
IMR_3DPoint MotionVect; MotionVect.X = MotionVect.Y = MotionVect.Z = 0;
IMR_Attitude Heading; Heading.X = Heading.Z = 0; Heading.Y = 0; 
IMR_Matrix Mat;
int Frame = 0;
int Speed;

// Load our font:
char *FontFName = IMR_Resources.Get_RDFManager()->GetRDFLocation("BFN", "BFN_T8x8.rdf", NULL);
if (!FontFName) Quit("Can't find fontfile in resources!");
if (!MainFont.Load_RDF(FontFName)) Quit("MainFont.Load_RDF() failed!");

// Reset the frame timer:
IMR_Time_Reset();

// Setup our collision info:
IMR_CollideInfo CInfo;
//CInfo.Setup_Ellipsoid(200, 100);
CInfo.Setup_Ellipsoid(50, 50);

// Loop until we're done:
Running = 1;
while (Running)
    {
    // Make sure the app is active:
    if (Active)
        {
        Pos.X = DbgInfo[0].X * 50;
        Pos.Y = DbgInfo[0].Y;
        Pos.Z = DbgInfo[0].Z * 50;
        Immerse.Get_Object("Tracer1")->Set_RelativePos(Pos);
        Pos.X = DbgInfo[1].X * 50;
        Pos.Y = DbgInfo[1].Y;
        Pos.Z = DbgInfo[1].Z * 50;
        Immerse.Get_Object("Tracer2")->Set_RelativePos(Pos);
        
        // Handle keyboard input:
        if (KB.Get_State(KEY_ESC)) Running = 0;
        if (KB.Get_State(KEY_LEFT))
            {
            Heading.Y = -1;
            Heading.Fix_Ang();
            Immerse.Get_Object("Person")->Motion_Turn(Heading, 300);
             }
        if (KB.Get_State(KEY_RIGHT))
            {
            Heading.Y = 1;
            Heading.Fix_Ang();
            Immerse.Get_Object("Person")->Motion_Turn(Heading, 300);
             }
        if (KB.Get_State(KEY_UP))
            {
            MotionVect.X = 0;
            MotionVect.Y = 0;
            MotionVect.Z = 1.0f;
            Heading = Immerse.Get_Object("Person")->Get_GlobalAtd();
            Mat.Rotate(Heading.X, Heading.Y, Heading.Z);
            MotionVect.Transform(Mat);
            Immerse.Get_Object("Person")->Motion_Travel_CheckCollide(MotionVect, 300, Immerse.Get_Object("TestPnl1"), CInfo);
             }

        if (KB.Get_State(KEY_A))
            {
            Atd = Immerse.Get_Object("TestPnl1")->Get_RelativeAtd();
            Atd.X += 2; 
            Immerse.Get_Object("TestPnl1")->Set_RelativeAtd(Atd);
             }
        if (KB.Get_State(KEY_S))
            {
            Atd = Immerse.Get_Object("TestPnl1")->Get_RelativeAtd();
            Atd.X -= 2; 
            Immerse.Get_Object("TestPnl1")->Set_RelativeAtd(Atd);
             }
        
        // Control the camera:
        Pos.X = 0; Pos.Y = 0; Pos.Z = -300; CameraMan.Vcn_SetPnt1(Pos);
        Atd.X = 64; Atd.Y = 0; Atd.Z = 0; CameraMan.Vcn_SetAtd1(Atd);
        CameraMan.Vcn_UpdateCam(NULL);

        // **** Do the frame ****

        // Get fps measurement and sync time:
        TimeDiff = IMR_Time_EndTimer(); // 0 the first frame
        if (TimeDiff) 
            fps = 1000.0f / (float)TimeDiff;          // So we don't get devide-by-zero
        IMR_Time_StartTimer();          // And start timing again...

        // Begin the frame, add the geometries, and draw:
        if (IMR_ISNOTOK(Immerse.Begin_Frame("CndidCam"))) Quit("Error!");
        if (IMR_ISNOTOK(Immerse.Add_Geometries())) Quit("Error!");
        if (IMR_ISNOTOK(Immerse.Draw_Frame())) Quit("Error!");
        if (IMR_ISNOTOK(Immerse.End_Frame())) Quit("Error!");

        // Clear the back buffer:
        if (!(Data = (char *)Immerse.Start_External_Draw())) Quit("ExternalDraw failed!");
        if (!ClearNeeded)
            memset(Data, 0, 30 * Immerse.Get_ScreenPitch());      // All we need is the text area
        else
            {
            memset(Data, 0, 30 * Immerse.Get_ScreenPitch());     // If we changed modes, clear the whole screen
            ClearNeeded --;
             }
        Immerse.End_External_Draw();
    
        // Blit the frame to the back buffer:
        if (Immerse.Blit_Frame() != IMR_OK) Quit("Error!");
    
        // Begin external drawing:
        if (!(Data = (char *)Immerse.Start_External_Draw())) Quit("ExternalDraw failed!");
        
        // Draw some text:
        MainFont.Set_16BitBuffer(Data, 640, 480, Immerse.Get_ScreenPitch());
        MainFont.Print(2, 2, "FPS: %f PPS: %.1f", fps, (float)Immerse.Get_Polys_Drawn() * fps);
        MainFont.Print(2, 12, "Geometry info:");
        MainFont.Print(22, 22, "%d polys in environment", Immerse.Get_Geom_Polys());
        MainFont.Print(22, 32, "%d polys drawn", Immerse.Get_Polys_Drawn());
        MainFont.Print(22, 42, "%d polys culled", Immerse.Get_Polys_Culled());

        // And finish drawing:
        Immerse.End_External_Draw();

        // And flip the buffer:
        if (IMR_ISNOTOK(Immerse.Flip_Buffers())) Quit("Error!");
         }
         
    // Get messages:
    if (PeekMessage(&Message, GameWindow, 0, 0, PM_NOREMOVE) == TRUE)
        {
        GetMessage(&Message, GameWindow, 0, 0);
        TranslateMessage(&Message);
        DispatchMessage(&Message);
         }
     }

// Cleanup and quit:
Cleanup();
return Message.wParam;
 }

/***************************************************************************\
  Handles windows messages.
\***************************************************************************/
LRESULT CALLBACK WndProc(HWND hWnd, unsigned int iMessage, 
                         unsigned int wParam, LONG lParam)
{
switch(iMessage)
    {
    case WM_COMMAND:
        {
        switch(LOWORD(wParam))
            {
             }
        break;
         }
    case WM_DESTROY: 
        Running = FALSE; 
        break;
    case WM_ACTIVATEAPP:
        // togglepausestate();
        Active = wParam;
        if (Active) SetCursor(NULL);
        if (!Active) SetCursor(LoadCursor(NULL, IDC_ARROW));
        break;
    case WM_SETCURSOR:
        if (Active) SetCursor(NULL);
        if (!Active) SetCursor(LoadCursor(NULL, IDC_ARROW));
        break;
    default: 
        return DefWindowProc(hWnd, iMessage, wParam, lParam);
     }
return 0;
 }

