/***************************************************************************\
   RPGDemo testing module
\***************************************************************************/
// Pre-compile windows.h:
#include <windows.h>

// Setup iMMERSE debugging:
#define IMR_DEBUG

#include "Demo.HPP"

#define STATE_WALKING  0
#define STATE_STANDING 1

#define MODE_CLOSEBEHIND 1
#define MODE_FARBEHIND   2
#define MODE_ABOVE       3
#define MODE_BELOW       4

// Miscellaneous:
HANDLE ThisInstance;
BOOL Running = TRUE, Active = TRUE;
HWND GameWindow;
BigFont MainFont;
//Palette Palette;
int CameraMode = MODE_CLOSEBEHIND;

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
    
    err = Immerse.Texture_LoadFromPCX("Bricks.pcx", "Bricks"); if (IMR_ISNOTOK(err)) Quit("Error!");
    err = Immerse.Texture_LoadFromPCX("Concrete.pcx", "Concrete"); if (IMR_ISNOTOK(err)) Quit("Error!");
    err = Immerse.Texture_LoadFromPCX("Chrome.pcx", "Chrome"); if (IMR_ISNOTOK(err)) Quit("Error!");
    err = Immerse.Texture_LoadFromPCX("White.pcx", "White"); if (IMR_ISNOTOK(err)) Quit("Error!");
    err = Immerse.Texture_LoadFromPCX("Steel.pcx", "Steel"); if (IMR_ISNOTOK(err)) Quit("Error!");
    err = Immerse.Texture_LoadFromPCX("Wood.pcx", "Wood"); if (IMR_ISNOTOK(err)) Quit("Error!");
    err = Immerse.Texture_LoadFromPCX("Sky_Fr.pcx", "Sky_Fr"); if (IMR_ISNOTOK(err)) Quit("Error!");
    err = Immerse.Texture_LoadFromPCX("Sky_Bk.pcx", "Sky_Bk"); if (IMR_ISNOTOK(err)) Quit("Error!");
    err = Immerse.Texture_LoadFromPCX("Sky_Lt.pcx", "Sky_Lt"); if (IMR_ISNOTOK(err)) Quit("Error!");
    err = Immerse.Texture_LoadFromPCX("Sky_Rt.pcx", "Sky_Rt"); if (IMR_ISNOTOK(err)) Quit("Error!");
    err = Immerse.Texture_LoadFromPCX("Sky_Tp.pcx", "Sky_Tp"); if (IMR_ISNOTOK(err)) Quit("Error!");

// ***** Create the environ model *****

    // Create block models for environ:
    err = Immerse.Add_Model(IMR_LIST_LOCAL, "Light"); if (IMR_ISNOTOK(err)) Quit("Error!");
    err = Immerse.Get_Model("Light")->Make_Rectangle(20, 20, 20, "Light"); if (IMR_ISNOTOK(err)) Quit("Error!");
    err = Immerse.Get_Model("Light")->Paint("White");
    err = Immerse.Get_Model("Light")->Set_PolyFlag_LightSource(1);
    err = Immerse.Add_Model(IMR_LIST_LOCAL, "Support"); if (IMR_ISNOTOK(err)) Quit("Error!");
    err = Immerse.Get_Model("Support")->Make_Rectangle(4, 4, 46, "Support"); if (IMR_ISNOTOK(err)) Quit("Error!");
    err = Immerse.Get_Model("Support")->Paint_Unpegged("Concrete", 256, 256);
    err = Immerse.Add_Model(IMR_LIST_LOCAL, "Dock"); if (IMR_ISNOTOK(err)) Quit("Error!");
    err = Immerse.Get_Model("Dock")->Make_Rectangle(300, 96, 150, "Dock"); if (IMR_ISNOTOK(err)) Quit("Error!");
    err = Immerse.Get_Model("Dock")->Paint_Unpegged("Concrete", 256, 256);
    err = Immerse.Add_Model(IMR_LIST_LOCAL, "Step"); if (IMR_ISNOTOK(err)) Quit("Error!");
    err = Immerse.Get_Model("Step")->Make_Rectangle(90, 32, 32, "Step"); if (IMR_ISNOTOK(err)) Quit("Error!");
    err = Immerse.Get_Model("Step")->Paint_Unpegged("Concrete", 256, 256);
    err = Immerse.Add_Model(IMR_LIST_LOCAL, "Wall"); if (IMR_ISNOTOK(err)) Quit("Error!");
    err = Immerse.Get_Model("Wall")->Make_Wall(600, 480, 100, "Wall"); if (IMR_ISNOTOK(err)) Quit("Error!");
    err = Immerse.Get_Model("Wall")->Paint("Bricks");
    err = Immerse.Add_Model(IMR_LIST_LOCAL, "FrntWall"); if (IMR_ISNOTOK(err)) Quit("Error!");
    err = Immerse.Get_Model("FrntWall")->Make_Wall(600, 384, 100, "FrntWall"); if (IMR_ISNOTOK(err)) Quit("Error!");
    err = Immerse.Get_Model("FrntWall")->Paint("Bricks");
    Immerse.Get_Model("FrntWall")->Shift_Pos(0, 48, 0);
    err = Immerse.Add_Model(IMR_LIST_LOCAL, "Floor"); if (IMR_ISNOTOK(err)) Quit("Error!");
    err = Immerse.Get_Model("Floor")->Make_Wall(600, 1050, 100, "Floor"); if (IMR_ISNOTOK(err)) Quit("Error!");
    Immerse.Get_Model("Floor")->Shift_Pos(0, -75, 0);
    err = Immerse.Get_Model("Floor")->Paint("Concrete");    
    err = Immerse.Add_Model(IMR_LIST_LOCAL, "Door"); if (IMR_ISNOTOK(err)) Quit("Error!");
    err = Immerse.Get_Model("Door")->Make_Rectangle(75, 105, 16, "Door"); if (IMR_ISNOTOK(err)) Quit("Error!");
    err = Immerse.Get_Model("Door")->Paint_Unpegged("Steel", 75, 75);
    err = Immerse.Add_Model(IMR_LIST_LOCAL, "Crate"); if (IMR_ISNOTOK(err)) Quit("Error!");
    err = Immerse.Get_Model("Crate")->Make_Cube(30, "Crate"); if (IMR_ISNOTOK(err)) Quit("Error!");
    err = Immerse.Get_Model("Crate")->Paint("Wood");

    // Create the objects:
    err = Immerse.Add_Object(IMR_LIST_LOCAL, "Support"); if (IMR_ISNOTOK(err)) Quit("Error!");
    Pos.X = 0; Pos.Y = 175; Pos.Z = -23;
    Atd.X = 0; Atd.Y = 0; Atd.Z = 0;
    Immerse.Get_Object("Support")->Set_RelativePos(Pos);
    Immerse.Get_Object("Support")->Set_RelativeAtd(Atd);
    Immerse.Get_Object("Support")->Set_ModelName("Support");
    err = Immerse.Add_Object(IMR_LIST_LOCAL, "Bulb"); if (IMR_ISNOTOK(err)) Quit("Error!");
    Pos.X = 0; Pos.Y = -12; Pos.Z = -23;
    Atd.X = 0; Atd.Y = 0; Atd.Z = 0;
    Immerse.Get_Object("Bulb")->Set_RelativePos(Pos);
    Immerse.Get_Object("Bulb")->Set_RelativeAtd(Atd);
    Immerse.Get_Object("Bulb")->Set_ModelName("Light");
    err = Immerse.Add_Object(IMR_LIST_LOCAL, "Door1"); if (IMR_ISNOTOK(err)) Quit("Error!");
    Pos.X = 0; Pos.Y = 11; Pos.Z = -8;
    Atd.X = 0; Atd.Y = 0; Atd.Z = 0;
    Immerse.Get_Object("Door1")->Set_RelativePos(Pos);
    Immerse.Get_Object("Door1")->Set_RelativeAtd(Atd);
    Immerse.Get_Object("Door1")->Set_ModelName("Door");
    err = Immerse.Add_Object(IMR_LIST_LOCAL, "Door2"); if (IMR_ISNOTOK(err)) Quit("Error!");
    Pos.X = 0; Pos.Y = -103; Pos.Z = 0;
    Atd.X = 0; Atd.Y = 0; Atd.Z = 0;
    Immerse.Get_Object("Door2")->Set_RelativePos(Pos);
    Immerse.Get_Object("Door2")->Set_RelativeAtd(Atd);
    Immerse.Get_Object("Door2")->Set_ModelName("Door");
    err = Immerse.Add_Object(IMR_LIST_LOCAL, "Crate1"); if (IMR_ISNOTOK(err)) Quit("Error!");
    Pos.X = -170; Pos.Y = 126; Pos.Z = 525;
    Atd.X = 0; Atd.Y = 128; Atd.Z = 0;
    Immerse.Get_Object("Crate1")->Set_RelativePos(Pos);
    Immerse.Get_Object("Crate1")->Set_RelativeAtd(Atd);
    Immerse.Get_Object("Crate1")->Set_ModelName("Crate");
    err = Immerse.Add_Object(IMR_LIST_LOCAL, "Crate2"); if (IMR_ISNOTOK(err)) Quit("Error!");
    Pos.X = -235; Pos.Y = 30; Pos.Z = -450;
    Atd.X = 0; Atd.Y = 64; Atd.Z = 0;
    Immerse.Get_Object("Crate2")->Set_RelativePos(Pos);
    Immerse.Get_Object("Crate2")->Set_RelativeAtd(Atd);
    Immerse.Get_Object("Crate2")->Set_ModelName("Crate");
    err = Immerse.Add_Object(IMR_LIST_LOCAL, "Crate3"); if (IMR_ISNOTOK(err)) Quit("Error!");
    Pos.X = -150; Pos.Y = 30; Pos.Z = -500;
    Atd.X = 0; Atd.Y = 10; Atd.Z = 0;
    Immerse.Get_Object("Crate3")->Set_RelativePos(Pos);
    Immerse.Get_Object("Crate3")->Set_RelativeAtd(Atd);
    Immerse.Get_Object("Crate3")->Set_ModelName("Crate");
    err = Immerse.Add_Object(IMR_LIST_LOCAL, "Floor"); if (IMR_ISNOTOK(err)) Quit("Error!");
    Pos.X = 0; Pos.Y = 0; Pos.Z = 0;
    Atd.X = 256; Atd.Y = 0; Atd.Z = 0;
    Immerse.Get_Object("Floor")->Set_RelativePos(Pos);
    Immerse.Get_Object("Floor")->Set_RelativeAtd(Atd);
    Immerse.Get_Object("Floor")->Set_ModelName("Floor");
    err = Immerse.Add_Object(IMR_LIST_LOCAL, "FrntWall"); if (IMR_ISNOTOK(err)) Quit("Error!");
    Pos.X = 0; Pos.Y = 240; Pos.Z = 600;
    Atd.X = 0; Atd.Y = 0; Atd.Z = 0;
    Immerse.Get_Object("FrntWall")->Set_RelativePos(Pos);
    Immerse.Get_Object("FrntWall")->Set_RelativeAtd(Atd);
    Immerse.Get_Object("FrntWall")->Set_ModelName("FrntWall");
    err = Immerse.Add_Object(IMR_LIST_LOCAL, "RearWall"); if (IMR_ISNOTOK(err)) Quit("Error!");
    Pos.X = 0; Pos.Y = 240; Pos.Z = -600;
    Atd.X = 0; Atd.Y = 512; Atd.Z = 0;
    Immerse.Get_Object("RearWall")->Set_RelativePos(Pos);
    Immerse.Get_Object("RearWall")->Set_RelativeAtd(Atd);
    Immerse.Get_Object("RearWall")->Set_ModelName("Wall");
    err = Immerse.Add_Object(IMR_LIST_LOCAL, "LftWall1"); if (IMR_ISNOTOK(err)) Quit("Error!");
    Pos.X = -300; Pos.Y = 240; Pos.Z = 300;
    Atd.X = 0; Atd.Y = 768; Atd.Z = 0;
    Immerse.Get_Object("LftWall1")->Set_RelativePos(Pos);
    Immerse.Get_Object("LftWall1")->Set_RelativeAtd(Atd);
    Immerse.Get_Object("LftWall1")->Set_ModelName("Wall");
    err = Immerse.Add_Object(IMR_LIST_LOCAL, "LftWall2"); if (IMR_ISNOTOK(err)) Quit("Error!");
    Pos.X = -300; Pos.Y = 240; Pos.Z = -300;
    Atd.X = 0; Atd.Y = 768; Atd.Z = 0;
    Immerse.Get_Object("LftWall2")->Set_RelativePos(Pos);
    Immerse.Get_Object("LftWall2")->Set_RelativeAtd(Atd);
    Immerse.Get_Object("LftWall2")->Set_ModelName("Wall");
    err = Immerse.Add_Object(IMR_LIST_LOCAL, "RitWall1"); if (IMR_ISNOTOK(err)) Quit("Error!");
    Pos.X = 300; Pos.Y = 240; Pos.Z = 300;
    Atd.X = 0; Atd.Y = 256; Atd.Z = 0;
    Immerse.Get_Object("RitWall1")->Set_RelativePos(Pos);
    Immerse.Get_Object("RitWall1")->Set_RelativeAtd(Atd);
    Immerse.Get_Object("RitWall1")->Set_ModelName("Wall");
    err = Immerse.Add_Object(IMR_LIST_LOCAL, "RitWall2"); if (IMR_ISNOTOK(err)) Quit("Error!");
    Pos.X = 300; Pos.Y = 240; Pos.Z = -300;
    Atd.X = 0; Atd.Y = 256; Atd.Z = 0;
    Immerse.Get_Object("RitWall2")->Set_RelativePos(Pos);
    Immerse.Get_Object("RitWall2")->Set_RelativeAtd(Atd);
    Immerse.Get_Object("RitWall2")->Set_ModelName("Wall");
    err = Immerse.Add_Object(IMR_LIST_LOCAL, "Dock1"); if (IMR_ISNOTOK(err)) Quit("Error!");
    Pos.X = -150; Pos.Y = 48; Pos.Z = 525;
    Atd.X = 0; Atd.Y = 0; Atd.Z = 0;
    Immerse.Get_Object("Dock1")->Set_RelativePos(Pos);
    Immerse.Get_Object("Dock1")->Set_RelativeAtd(Atd);
    Immerse.Get_Object("Dock1")->Set_ModelName("Dock");
    err = Immerse.Add_Object(IMR_LIST_LOCAL, "Dock2"); if (IMR_ISNOTOK(err)) Quit("Error!");
    Pos.X = 150; Pos.Y = 48; Pos.Z = 525;
    Atd.X = 0; Atd.Y = 0; Atd.Z = 0;
    Immerse.Get_Object("Dock2")->Set_RelativePos(Pos);
    Immerse.Get_Object("Dock2")->Set_RelativeAtd(Atd);
    Immerse.Get_Object("Dock2")->Set_ModelName("Dock");
    err = Immerse.Add_Object(IMR_LIST_LOCAL, "Step1"); if (IMR_ISNOTOK(err)) Quit("Error!");
    Pos.X = 255; Pos.Y = 16; Pos.Z = 402;
    Atd.X = 0; Atd.Y = 0; Atd.Z = 0;
    Immerse.Get_Object("Step1")->Set_RelativePos(Pos);
    Immerse.Get_Object("Step1")->Set_RelativeAtd(Atd);
    Immerse.Get_Object("Step1")->Set_ModelName("Step");
    err = Immerse.Add_Object(IMR_LIST_LOCAL, "Step2"); if (IMR_ISNOTOK(err)) Quit("Error!");
    Pos.X = 255; Pos.Y = 16; Pos.Z = 434;
    Atd.X = 0; Atd.Y = 0; Atd.Z = 0;
    Immerse.Get_Object("Step2")->Set_RelativePos(Pos);
    Immerse.Get_Object("Step2")->Set_RelativeAtd(Atd);
    Immerse.Get_Object("Step2")->Set_ModelName("Step");
    err = Immerse.Add_Object(IMR_LIST_LOCAL, "Step3"); if (IMR_ISNOTOK(err)) Quit("Error!");
    Pos.X = 255; Pos.Y = 48; Pos.Z = 434;
    Atd.X = 0; Atd.Y = 0; Atd.Z = 0;
    Immerse.Get_Object("Step3")->Set_RelativePos(Pos);
    Immerse.Get_Object("Step3")->Set_RelativeAtd(Atd);
    Immerse.Get_Object("Step3")->Set_ModelName("Step");

    // Build the environ object heiriarchies:
    err = Immerse.Add_Object(IMR_LIST_LOCAL, "Environ"); if (IMR_ISNOTOK(err)) Quit("Error!");
    Pos.X = 0; Pos.Y = 0; Pos.Z = 0;
    Atd.X = 0; Atd.Y = 0; Atd.Z = 0;
    Immerse.Get_Object("Environ")->Set_RelativePos(Pos);
    Immerse.Get_Object("Environ")->Set_RelativeAtd(Atd);
    Immerse.Attach_Object("Floor", "Environ");
    Immerse.Attach_Object("FrntWall", "Environ");
    Immerse.Attach_Object("RearWall", "Environ");
    Immerse.Attach_Object("LftWall1", "Environ");
    Immerse.Attach_Object("LftWall2", "Environ");
    Immerse.Attach_Object("RitWall1", "Environ");
    Immerse.Attach_Object("RitWall2", "Environ");
    Immerse.Attach_Object("Crate1", "Environ");
    Immerse.Attach_Object("Crate2", "Environ");
    Immerse.Attach_Object("Crate3", "Environ");
    Immerse.Attach_Object("Door1", "FrntWall");
    Immerse.Attach_Object("Door2", "Door1");
    Immerse.Attach_Object("Support", "FrntWall");
    Immerse.Attach_Object("Bulb", "Support");
    Immerse.Attach_Object("Dock1", "Environ");
    Immerse.Attach_Object("Dock2", "Environ");
    Immerse.Attach_Object("Step1", "Environ");
    Immerse.Attach_Object("Step2", "Environ");
    Immerse.Attach_Object("Step3", "Environ");
    err = Immerse.Prep_Geometries(); if (IMR_ISNOTOK(err)) Quit("Error!");

    // Combine the environ into one model:
    IMR_Object *Env = Immerse.Get_Object("Environ");
    if (!Env) Quit("No env!");
    Pos.X = 0; Pos.Y = 0; Pos.Z = 0;
    Atd.X = 0; Atd.Y = 0; Atd.Z = 0;
    Env->Set_RelativePos(Pos);
    Env->Set_RelativeAtd(Atd);
    err = Immerse.Add_Model(IMR_LIST_GLOBAL, "Environ"); if (IMR_ISNOTOK(err)) Quit("Error!");
    Env->MergeToModel(Immerse.Get_Model("Environ"), Pos);
    
    // Clear the old objects:
    Immerse.Clear_Local_Lists();

    // Create a new environ object:
    Immerse.Add_Object(IMR_LIST_GLOBAL, "Environ"); if (IMR_ISNOTOK(err)) Quit("Error!");
    Env = Immerse.Get_Object("Environ");
    if (!Env) Quit("No environ!");
    Env->Set_ModelName("Environ");
    Env->Set_Collidable();

// ***** Create the person and figure *****

    // Create models:
    err = Immerse.Add_Model(IMR_LIST_GLOBAL, "Head"); if (IMR_ISNOTOK(err)) Quit("Error!");
    err = Immerse.Get_Model("Head")->Make_Pyramid(23, 23, 23, "Head"); if (IMR_ISNOTOK(err)) Quit("Error!");
    err = Immerse.Get_Model("Head")->Paint("Sky_Fr");
    err = Immerse.Add_Model(IMR_LIST_GLOBAL, "Torso"); if (IMR_ISNOTOK(err)) Quit("Error!");
    err = Immerse.Get_Model("Torso")->Make_Rectangle(32, 68, 23, "Torso"); if (IMR_ISNOTOK(err)) Quit("Error!");
    err = Immerse.Get_Model("Torso")->Paint("Chrome");
    err = Immerse.Add_Model(IMR_LIST_GLOBAL, "Arm"); if (IMR_ISNOTOK(err)) Quit("Error!");
    err = Immerse.Get_Model("Arm")->Make_Rectangle(8, 57, 8, "Arm"); if (IMR_ISNOTOK(err)) Quit("Error!");
    err = Immerse.Get_Model("Arm")->Paint("Chrome");
    Immerse.Get_Model("Arm")->Shift_Pos(0, -29, 0);
    err = Immerse.Add_Model(IMR_LIST_GLOBAL, "Leg"); if (IMR_ISNOTOK(err)) Quit("Error!");
    err = Immerse.Get_Model("Leg")->Make_Rectangle(14, 92, 14, "Leg"); if (IMR_ISNOTOK(err)) Quit("Error!");
    err = Immerse.Get_Model("Leg")->Paint("Chrome");
    Immerse.Get_Model("Leg")->Shift_Pos(0, -46, 0);

    err = Immerse.Add_Object(IMR_LIST_GLOBAL, "Head"); if (IMR_ISNOTOK(err)) Quit("Error!");
    Immerse.Get_Object("Head")->Set_ModelName("Head");
    err = Immerse.Add_Object(IMR_LIST_GLOBAL, "Torso"); if (IMR_ISNOTOK(err)) Quit("Error!");
    Immerse.Get_Object("Torso")->Set_ModelName("Torso");
    err = Immerse.Add_Object(IMR_LIST_GLOBAL, "LArm"); if (IMR_ISNOTOK(err)) Quit("Error!");
    Immerse.Get_Object("LArm")->Set_ModelName("Arm");
    err = Immerse.Add_Object(IMR_LIST_GLOBAL, "RArm"); if (IMR_ISNOTOK(err)) Quit("Error!");
    Immerse.Get_Object("RArm")->Set_ModelName("Arm");
    err = Immerse.Add_Object(IMR_LIST_GLOBAL, "LLeg"); if (IMR_ISNOTOK(err)) Quit("Error!");
    Immerse.Get_Object("LLeg")->Set_ModelName("Leg");
    err = Immerse.Add_Object(IMR_LIST_GLOBAL, "RLeg"); if (IMR_ISNOTOK(err)) Quit("Error!");
    Immerse.Get_Object("RLeg")->Set_ModelName("Leg");
    err = Immerse.Add_Object(IMR_LIST_GLOBAL, "Person"); if (IMR_ISNOTOK(err)) Quit("Error!");
    Immerse.Get_Object("Person")->Set_ModelName("");

    // Create figure:
    err = Immerse.Add_Figure(IMR_LIST_GLOBAL, "Person"); if (IMR_ISNOTOK(err)) Quit("Error!");
    err = Immerse.Get_Figure("Person")->Add_Key("Still"); if (IMR_ISNOTOK(err)) Quit("Error!");
    Pos.X = 0; Pos.Y = 46; Pos.Z = 0;
    Atd.X = 256; Atd.Y = 512; Atd.Z = 0;
    err = Immerse.Get_Figure("Person")->Get_Key("Still")->Add_Segment("Head"); if (IMR_ISNOTOK(err)) Quit("Error!");
    Immerse.Get_Figure("Person")->Get_Key("Still")->Get_Segment("Head")->Set_ObjectName("Head");
    Immerse.Get_Figure("Person")->Get_Key("Still")->Get_Segment("Head")->Set_Pos(Pos);
    Immerse.Get_Figure("Person")->Get_Key("Still")->Get_Segment("Head")->Set_Atd(Atd);
    Pos.X = 0; Pos.Y = 0; Pos.Z = 0;
    Atd.X = 0; Atd.Y = 0; Atd.Z = 0;
    err = Immerse.Get_Figure("Person")->Get_Key("Still")->Add_Segment("Torso"); if (IMR_ISNOTOK(err)) Quit("Error!");
    Immerse.Get_Figure("Person")->Get_Key("Still")->Get_Segment("Torso")->Set_ObjectName("Torso");
    Immerse.Get_Figure("Person")->Get_Key("Still")->Get_Segment("Torso")->Set_Pos(Pos);
    Immerse.Get_Figure("Person")->Get_Key("Still")->Get_Segment("Torso")->Set_Atd(Atd);
    Pos.X = -22; Pos.Y = 34; Pos.Z = 0;
    Atd.X = 0; Atd.Y = 0; Atd.Z = 0;
    err = Immerse.Get_Figure("Person")->Get_Key("Still")->Add_Segment("LeftArm"); if (IMR_ISNOTOK(err)) Quit("Error!");
    Immerse.Get_Figure("Person")->Get_Key("Still")->Get_Segment("LeftArm")->Set_ObjectName("LArm");
    Immerse.Get_Figure("Person")->Get_Key("Still")->Get_Segment("LeftArm")->Set_Pos(Pos);
    Immerse.Get_Figure("Person")->Get_Key("Still")->Get_Segment("LeftArm")->Set_Atd(Atd);
    Pos.X = 22; Pos.Y = 34; Pos.Z = 0;
    Atd.X = 0; Atd.Y = 0; Atd.Z = 0;
    err = Immerse.Get_Figure("Person")->Get_Key("Still")->Add_Segment("RightArm"); if (IMR_ISNOTOK(err)) Quit("Error!");
    Immerse.Get_Figure("Person")->Get_Key("Still")->Get_Segment("RightArm")->Set_ObjectName("RArm");
    Immerse.Get_Figure("Person")->Get_Key("Still")->Get_Segment("RightArm")->Set_Pos(Pos);
    Immerse.Get_Figure("Person")->Get_Key("Still")->Get_Segment("RightArm")->Set_Atd(Atd);
    Pos.X = -10; Pos.Y = -34; Pos.Z = 0;
    Atd.X = 0; Atd.Y = 0; Atd.Z = 0;
    err = Immerse.Get_Figure("Person")->Get_Key("Still")->Add_Segment("LeftLeg"); if (IMR_ISNOTOK(err)) Quit("Error!");
    Immerse.Get_Figure("Person")->Get_Key("Still")->Get_Segment("LeftLeg")->Set_ObjectName("LLeg");
    Immerse.Get_Figure("Person")->Get_Key("Still")->Get_Segment("LeftLeg")->Set_Pos(Pos);
    Immerse.Get_Figure("Person")->Get_Key("Still")->Get_Segment("LeftLeg")->Set_Atd(Atd);
    Pos.X = 10; Pos.Y = -34; Pos.Z = 0;
    Atd.X = 0; Atd.Y = 0; Atd.Z = 0;
    err = Immerse.Get_Figure("Person")->Get_Key("Still")->Add_Segment("RightLeg"); if (IMR_ISNOTOK(err)) Quit("Error!");
    Immerse.Get_Figure("Person")->Get_Key("Still")->Get_Segment("RightLeg")->Set_ObjectName("RLeg");
    Immerse.Get_Figure("Person")->Get_Key("Still")->Get_Segment("RightLeg")->Set_Pos(Pos);
    Immerse.Get_Figure("Person")->Get_Key("Still")->Get_Segment("RightLeg")->Set_Atd(Atd);

    err = Immerse.Get_Figure("Person")->Add_Key("Walk0"); if (IMR_ISNOTOK(err)) Quit("Error!");
    Pos.X = 0; Pos.Y = 46; Pos.Z = 0;
    Atd.X = 256; Atd.Y = 528; Atd.Z = 0;
    err = Immerse.Get_Figure("Person")->Get_Key("Walk0")->Add_Segment("Head"); if (IMR_ISNOTOK(err)) Quit("Error!");
    Immerse.Get_Figure("Person")->Get_Key("Walk0")->Get_Segment("Head")->Set_ObjectName("Head");
    Immerse.Get_Figure("Person")->Get_Key("Walk0")->Get_Segment("Head")->Set_Pos(Pos);
    Immerse.Get_Figure("Person")->Get_Key("Walk0")->Get_Segment("Head")->Set_Atd(Atd);
    Pos.X = 0; Pos.Y = 0; Pos.Z = 0;
    Atd.X = 0; Atd.Y = 0; Atd.Z = 0;
    err = Immerse.Get_Figure("Person")->Get_Key("Walk0")->Add_Segment("Torso"); if (IMR_ISNOTOK(err)) Quit("Error!");
    Immerse.Get_Figure("Person")->Get_Key("Walk0")->Get_Segment("Torso")->Set_ObjectName("Torso");
    Immerse.Get_Figure("Person")->Get_Key("Walk0")->Get_Segment("Torso")->Set_Pos(Pos);
    Immerse.Get_Figure("Person")->Get_Key("Walk0")->Get_Segment("Torso")->Set_Atd(Atd);
    Pos.X = -22.0; Pos.Y = 34.0; Pos.Z = 0;
    Atd.X = 32; Atd.Y = 0; Atd.Z = 0;
    err = Immerse.Get_Figure("Person")->Get_Key("Walk0")->Add_Segment("LeftArm"); if (IMR_ISNOTOK(err)) Quit("Error!");
    Immerse.Get_Figure("Person")->Get_Key("Walk0")->Get_Segment("LeftArm")->Set_ObjectName("LArm");
    Immerse.Get_Figure("Person")->Get_Key("Walk0")->Get_Segment("LeftArm")->Set_Pos(Pos);
    Immerse.Get_Figure("Person")->Get_Key("Walk0")->Get_Segment("LeftArm")->Set_Atd(Atd);
    Pos.X = 22; Pos.Y = 34; Pos.Z = 0;
    Atd.X = -32; Atd.Y = 0; Atd.Z = 0;
    err = Immerse.Get_Figure("Person")->Get_Key("Walk0")->Add_Segment("RightArm"); if (IMR_ISNOTOK(err)) Quit("Error!");
    Immerse.Get_Figure("Person")->Get_Key("Walk0")->Get_Segment("RightArm")->Set_ObjectName("RArm");
    Immerse.Get_Figure("Person")->Get_Key("Walk0")->Get_Segment("RightArm")->Set_Pos(Pos);
    Immerse.Get_Figure("Person")->Get_Key("Walk0")->Get_Segment("RightArm")->Set_Atd(Atd);
    Pos.X = -10; Pos.Y = -34; Pos.Z = 0;
    Atd.X = -32; Atd.Y = 0; Atd.Z = 0;
    err = Immerse.Get_Figure("Person")->Get_Key("Walk0")->Add_Segment("LeftLeg"); if (IMR_ISNOTOK(err)) Quit("Error!");
    Immerse.Get_Figure("Person")->Get_Key("Walk0")->Get_Segment("LeftLeg")->Set_ObjectName("LLeg");
    Immerse.Get_Figure("Person")->Get_Key("Walk0")->Get_Segment("LeftLeg")->Set_Pos(Pos);
    Immerse.Get_Figure("Person")->Get_Key("Walk0")->Get_Segment("LeftLeg")->Set_Atd(Atd);
    Pos.X = 10; Pos.Y = -34; Pos.Z = 0;
    Atd.X = 32; Atd.Y = 0; Atd.Z = 0;
    err = Immerse.Get_Figure("Person")->Get_Key("Walk0")->Add_Segment("RightLeg"); if (IMR_ISNOTOK(err)) Quit("Error!");
    Immerse.Get_Figure("Person")->Get_Key("Walk0")->Get_Segment("RightLeg")->Set_ObjectName("RLeg");
    Immerse.Get_Figure("Person")->Get_Key("Walk0")->Get_Segment("RightLeg")->Set_Pos(Pos);
    Immerse.Get_Figure("Person")->Get_Key("Walk0")->Get_Segment("RightLeg")->Set_Atd(Atd);

    err = Immerse.Get_Figure("Person")->Add_Key("Walk1"); if (IMR_ISNOTOK(err)) Quit("Error!");
    Pos.X = 0; Pos.Y = 46; Pos.Z = 0;
    Atd.X = 256; Atd.Y = 496; Atd.Z = 0;
    err = Immerse.Get_Figure("Person")->Get_Key("Walk1")->Add_Segment("Head"); if (IMR_ISNOTOK(err)) Quit("Error!");
    Immerse.Get_Figure("Person")->Get_Key("Walk1")->Get_Segment("Head")->Set_ObjectName("Head");
    Immerse.Get_Figure("Person")->Get_Key("Walk1")->Get_Segment("Head")->Set_Pos(Pos);
    Immerse.Get_Figure("Person")->Get_Key("Walk1")->Get_Segment("Head")->Set_Atd(Atd);
    Pos.X = 0; Pos.Y = 0; Pos.Z = 0;
    Atd.X = 0; Atd.Y = 0; Atd.Z = 0;
    err = Immerse.Get_Figure("Person")->Get_Key("Walk1")->Add_Segment("Torso"); if (IMR_ISNOTOK(err)) Quit("Error!");
    Immerse.Get_Figure("Person")->Get_Key("Walk1")->Get_Segment("Torso")->Set_ObjectName("Torso");
    Immerse.Get_Figure("Person")->Get_Key("Walk1")->Get_Segment("Torso")->Set_Pos(Pos);
    Immerse.Get_Figure("Person")->Get_Key("Walk1")->Get_Segment("Torso")->Set_Atd(Atd);
    Pos.X = -22.0; Pos.Y = 34; Pos.Z = 0;
    Atd.X = -32; Atd.Y = 0; Atd.Z = 0;
    err = Immerse.Get_Figure("Person")->Get_Key("Walk1")->Add_Segment("LeftArm"); if (IMR_ISNOTOK(err)) Quit("Error!");
    Immerse.Get_Figure("Person")->Get_Key("Walk1")->Get_Segment("LeftArm")->Set_ObjectName("LArm");
    Immerse.Get_Figure("Person")->Get_Key("Walk1")->Get_Segment("LeftArm")->Set_Pos(Pos);
    Immerse.Get_Figure("Person")->Get_Key("Walk1")->Get_Segment("LeftArm")->Set_Atd(Atd);
    Pos.X = 22; Pos.Y = 34; Pos.Z = 0;
    Atd.X = 32; Atd.Y = 0; Atd.Z = 0;
    err = Immerse.Get_Figure("Person")->Get_Key("Walk1")->Add_Segment("RightArm"); if (IMR_ISNOTOK(err)) Quit("Error!");
    Immerse.Get_Figure("Person")->Get_Key("Walk1")->Get_Segment("RightArm")->Set_ObjectName("RArm");
    Immerse.Get_Figure("Person")->Get_Key("Walk1")->Get_Segment("RightArm")->Set_Pos(Pos);
    Immerse.Get_Figure("Person")->Get_Key("Walk1")->Get_Segment("RightArm")->Set_Atd(Atd);
    Pos.X = -10; Pos.Y = -34; Pos.Z = 0;
    Atd.X = 32; Atd.Y = 0; Atd.Z = 0;
    err = Immerse.Get_Figure("Person")->Get_Key("Walk1")->Add_Segment("LeftLeg"); if (IMR_ISNOTOK(err)) Quit("Error!");
    Immerse.Get_Figure("Person")->Get_Key("Walk1")->Get_Segment("LeftLeg")->Set_ObjectName("LLeg");
    Immerse.Get_Figure("Person")->Get_Key("Walk1")->Get_Segment("LeftLeg")->Set_Pos(Pos);
    Immerse.Get_Figure("Person")->Get_Key("Walk1")->Get_Segment("LeftLeg")->Set_Atd(Atd);
    Pos.X = 10; Pos.Y = -34; Pos.Z = 0;
    Atd.X = -32; Atd.Y = 0; Atd.Z = 0;
    err = Immerse.Get_Figure("Person")->Get_Key("Walk1")->Add_Segment("RightLeg"); if (IMR_ISNOTOK(err)) Quit("Error!");
    Immerse.Get_Figure("Person")->Get_Key("Walk1")->Get_Segment("RightLeg")->Set_ObjectName("RLeg");
    Immerse.Get_Figure("Person")->Get_Key("Walk1")->Get_Segment("RightLeg")->Set_Pos(Pos);
    Immerse.Get_Figure("Person")->Get_Key("Walk1")->Get_Segment("RightLeg")->Set_Atd(Atd);

// ***** Create the skybox model and object *****
    // Create the model:
    err = Immerse.Add_Model(IMR_LIST_LOCAL, "Skybox"); if (IMR_ISNOTOK(err)) Quit("Error!");
    err = Immerse.Get_Model("Skybox")->Make_Skybox("Skybox"); if (IMR_ISNOTOK(err)) Quit("Error!");
    err = Immerse.Get_Model("Skybox")->Paint_Skybox("Sky_Fr", "Sky_Rt", "Sky_Bk", "Sky_Lt", "Sky_Tp", NULL);

    // Create the object:
    err = Immerse.Add_Object(IMR_LIST_GLOBAL, "Skybox"); if (IMR_ISNOTOK(err)) Quit("Error!");
    Immerse.Get_Object("Skybox")->Set_ModelName("Skybox");

// ***** Assemble the world *****

    // Attach the objects:
    Immerse.Attach_Object("Skybox", NULL);
    Immerse.Attach_Object("Environ", NULL);
    Immerse.Attach_Object("Person", NULL);
    Immerse.Attach_Object("Torso", "Person");
    Immerse.Attach_Object("LArm", "Torso");
    Immerse.Attach_Object("RArm", "Torso");
    Immerse.Attach_Object("LLeg", "Torso");
    Immerse.Attach_Object("RLeg", "Torso");
    Immerse.Attach_Object("Head", "Torso");

    // Create the lights:
    Immerse.Add_Light(0);       // Ambient light
    Immerse.Get_Light(0)->Set_Type(IMR_LIGHT_AMBIENT);
    Immerse.Get_Light(0)->Set_Color(0.3f, 0.3f, 0.3f);
    Immerse.Get_Object(NULL)->Attach_Light(Immerse.Get_Light(0));

    Immerse.Add_Light(1);       // Person light
    Immerse.Get_Light(1)->Set_Type(IMR_LIGHT_POINT);
    Immerse.Get_Light(1)->Set_Color(0.0, 0.0f, 1.0f);
    Pos.X = 0; Pos.Y = 0; Pos.Z = 0;
    Immerse.Get_Light(1)->Set_Position(Pos);
    Immerse.Get_Light(1)->Set_Range(300);
    Immerse.Get_Object("Head")->Attach_Light(Immerse.Get_Light(1));

    Immerse.Add_Light(2);       // Loading dock overhead light
    Immerse.Get_Light(2)->Set_Type(IMR_LIGHT_POINT);
    Immerse.Get_Light(2)->Set_Color(1.0f, 1.0f, 0.703125f);
    Pos.X = 0; Pos.Y = 403; Pos.Z = 554;
    Immerse.Get_Light(2)->Set_Position(Pos);
    Immerse.Get_Light(2)->Set_Range(800);
    //Immerse.Get_Light(2)->Set_Direction(256, 0);
    //Immerse.Get_Light(2)->Set_Umbra(128);
    //Immerse.Get_Light(2)->Set_Penumbra(170);
    Immerse.Get_Object("Environ")->Attach_Light(Immerse.Get_Light(2));

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
    CameraMan.Vcn_SetTargetObj(Immerse.Get_Object("Torso"));
    CameraMan.Vcn_SetParam(IMR_CAMERAOP_VCN_MODE, IMR_CAMERAOP_VCN_MODE_CHASE);
    CameraMan.Vcn_SetParam(IMR_CAMERAOP_VCN_FOLLOWTIME, 250);
    Pos.X = 0; Pos.Y = 0; Pos.Z = -300; CameraMan.Vcn_SetPnt1(Pos);
    Atd.X = 64; Atd.Y = 0; Atd.Z = 0; CameraMan.Vcn_SetAtd1(Atd);

// *****
IMR_3DPoint MotionVect; MotionVect.X = MotionVect.Y = MotionVect.Z = 0;
IMR_Attitude Heading; Heading.X = Heading.Z = 0; Heading.Y = 0; 
Immerse.Get_Object("Person")->Set_RelativeAtd(Heading);
IMR_Matrix Mat;
int Frame = 0;
int PrevState = STATE_STANDING;
int CurrState = STATE_STANDING;
int Speed;
Immerse.Get_Figure("Person")->Animation_Jump("Still");
Pos.X = 0; Pos.Y = 500; Pos.Z = 100;
Immerse.Get_Object("Person")->Set_RelativePos(Pos);

// Load our font:
char *FontFName = IMR_Resources.Get_RDFManager()->GetRDFLocation("BFN", "BFN_T8x8.rdf", NULL);
if (!FontFName) Quit("Can't find fontfile in resources!");
if (!MainFont.Load_RDF(FontFName)) Quit("MainFont.Load_RDF() failed!");

// Reset the frame timer:
IMR_Time_Reset();

// Setup our collision info:
IMR_CollideInfo CInfo;
CInfo.Setup_Ellipsoid(120, 25);

// Loop until we're done:
Running = 1;
while (Running)
    {
    // Make sure the app is active:
    if (Active)
        {
        // Handle keyboard input:
        if (KB.Get_State(KEY_ESC)) Running = 0;
        if (KB.Get_State(KEY_LEFT_SHIFT) || KB.Get_State(KEY_RIGHT_SHIFT)) Speed = 600; else Speed = 300;
        if (KB.Get_State(KEY_F1)) CameraMode = MODE_CLOSEBEHIND;
        if (KB.Get_State(KEY_F2)) CameraMode = MODE_FARBEHIND;
        if (KB.Get_State(KEY_F3)) CameraMode = MODE_ABOVE;
        if (KB.Get_State(KEY_F4)) CameraMode = MODE_BELOW;
        if (KB.Get_State(KEY_LEFT))
            {
            if (KB.Get_State(KEY_LEFT_SHIFT) || KB.Get_State(KEY_RIGHT_SHIFT))
                Heading.Y = -2;
            else
                Heading.Y = -1;
            Heading.Fix_Ang();
            Immerse.Get_Object("Person")->Motion_Turn(Heading, Speed);
             }
        if (KB.Get_State(KEY_RIGHT))
            {
            if (KB.Get_State(KEY_LEFT_SHIFT) || KB.Get_State(KEY_RIGHT_SHIFT))
                Heading.Y = 2;
            else
                Heading.Y = 1;
            Heading.Fix_Ang();
            Immerse.Get_Object("Person")->Motion_Turn(Heading, Speed);
             }
        if (KB.Get_State(KEY_UP))
            {
            MotionVect.X = 0;
            MotionVect.Y = 0;
            if (KB.Get_State(KEY_LEFT_SHIFT) || KB.Get_State(KEY_RIGHT_SHIFT))
                MotionVect.Z = 2.0;
            else
                MotionVect.Z = 1.0;
            Heading = Immerse.Get_Object("Person")->Get_GlobalAtd();
            Mat.Rotate(Heading.X, Heading.Y, Heading.Z);
            MotionVect.Transform(Mat);
            //Immerse.Get_Object("Person")->Motion_Travel(MotionVect, Speed);
            Immerse.Get_Object("Person")->Motion_Travel_CheckCollide(MotionVect, Speed, Immerse.Get_Object("Environ"), CInfo);
            CurrState = STATE_WALKING;
             }
        if (!KB.Get_State(KEY_UP))
            CurrState = STATE_STANDING;

        // Do gravity:
        MotionVect.X = 0;
        MotionVect.Y = -1;
        MotionVect.Z = 0;
        Immerse.Get_Object("Person")->Motion_Travel_CheckCollide(MotionVect, 300, Immerse.Get_Object("Environ"), CInfo);

        // Target resolution:
        /*
        if (KB.Get_State(KEY_1))
            {
            if (IMR_ISNOTOK(Immerse.Set_Window(0, 0, 639, 479))) Quit("Error!");
            Immerse.Get_Camera("CndidCam")->Lens_Set_Near(32);
            Immerse.Get_Camera("CndidCam")->Lens_Set_Far(1500);
            Immerse.Get_Camera("CndidCam")->Lens_Set_Zoom(IMR_ZOOM640480);
            ClearNeeded = 1;
             }
        if (KB.Get_State(KEY_2))
            {
            if (IMR_ISNOTOK(Immerse.Set_Window(160, 120, 480, 360))) Quit("Error!");
            Immerse.Get_Camera("CndidCam")->Lens_Set_Near(32);
            Immerse.Get_Camera("CndidCam")->Lens_Set_Far(1500);
            Immerse.Get_Camera("CndidCam")->Lens_Set_Zoom(IMR_ZOOM320240);
            ClearNeeded = 1;
             }
        if (KB.Get_State(KEY_3))
            {
            if (IMR_ISNOTOK(Immerse.Set_Window(240, 100, 400, 220))) Quit("Error!");
            Immerse.Get_Camera("CndidCam")->Lens_Set_Near(32);
            Immerse.Get_Camera("CndidCam")->Lens_Set_Far(1500);
            Immerse.Get_Camera("CndidCam")->Lens_Set_Zoom(IMR_CALCZOOM(160, 120));
            ClearNeeded = 1;
             }
        */

        // Various toggles:
        if (KB.Get_State(KEY_F))
            {
            if (Immerse.GetDrawOpt_Filtering())
                Immerse.SetDrawOpt_Filtering(0);
            else
                Immerse.SetDrawOpt_Filtering(1);
             }
        if (KB.Get_State(KEY_M))
            {
            if (Immerse.GetDrawOpt_MipMap())
                Immerse.SetDrawOpt_MipMap(0);
            else
                Immerse.SetDrawOpt_MipMap(1);
             }

        // Animate the guy:
        if (CurrState != PrevState)
            {
            PrevState = CurrState;
            
            if (CurrState == STATE_WALKING)
                {
                Frame = 0;
                Immerse.Get_Figure("Person")->Animation_Init("Walk1", 150);
                 }
            if (CurrState == STATE_STANDING)
                Immerse.Get_Figure("Person")->Animation_Init("Still", 250);
             }
        if (Immerse.Get_Figure("Person")->Animation_Step() == IMR_ANIMATION_DONE)
            {
            if (CurrState == STATE_WALKING)
                {
                Frame = ++Frame % 2;
                if (Frame == 0) Immerse.Get_Figure("Person")->Animation_Init("Walk1", 250);
                if (Frame == 1) Immerse.Get_Figure("Person")->Animation_Init("Walk0", 250);
                 }
             }
        
        // Control the camera:
        if (CameraMode == MODE_CLOSEBEHIND)
            {
            Pos.X = 0; Pos.Y = 0; Pos.Z = -300; CameraMan.Vcn_SetPnt1(Pos);
            Atd.X = 64; Atd.Y = 0; Atd.Z = 0; CameraMan.Vcn_SetAtd1(Atd);
             }
        if (CameraMode == MODE_FARBEHIND)
            {
            Pos.X = 0; Pos.Y = 0; Pos.Z = -600; CameraMan.Vcn_SetPnt1(Pos);
            Atd.X = 64; Atd.Y = 0; Atd.Z = 0; CameraMan.Vcn_SetAtd1(Atd);
             }
        if (CameraMode == MODE_ABOVE)
            {
            Pos.X = 0; Pos.Y = 0; Pos.Z = -300; CameraMan.Vcn_SetPnt1(Pos);
            Atd.X = 192; Atd.Y = 0; Atd.Z = 0; CameraMan.Vcn_SetAtd1(Atd);
             }
        if (CameraMode == MODE_BELOW)
            {
            Pos.X = 0; Pos.Y = 0; Pos.Z = -200; CameraMan.Vcn_SetPnt1(Pos);
            Atd.X = 960; Atd.Y = 0; Atd.Z = 0; CameraMan.Vcn_SetAtd1(Atd);
             }
        CameraMan.Vcn_UpdateCam(NULL);

        // Do fake collision detection:
        //Pos = Immerse.Get_Object("Person")->Get_RelativePos();
        //if (Pos.X < -270) Pos.X = -270;
        //if (Pos.X > 270) Pos.X = 270;
        //if (Pos.Z < -570) Pos.Z = -570;
        //if (Pos.Z > 570) Pos.Z = 570;
        
        //Pos.Y = 108;
        //if (Pos.Z > 450) Pos.Y = 204;   // Dock
        //if (Pos.Z > 418 && 
        //    Pos.Z < 451 &&
        //    Pos.X > 210) Pos.Y = 172;   // Top step
        //if (Pos.Z > 386 && 
        //    Pos.Z < 418 &&
        //    Pos.X > 210) Pos.Y = 140;   // Bottom step
        //Immerse.Get_Object("Person")->Set_RelativePos(Pos);
        
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

