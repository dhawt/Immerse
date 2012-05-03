/****************************************************************\
 
 iMMERSE Engine
 (C) 1999 No Tears Shed Software
 All rights reserved
 
 Filename: IMR_GM_Interface.hpp
 Description: GeometryManager iMMERSE interface inherited from 
              IMR_Interface class.
 
\****************************************************************/
#include "IMR_GM_Interface.hpp"

/***************************************************************************\
  Initializes the interface.
  Returns IMR_OK if successful, otherwise an error.
\***************************************************************************/
int IMR_GM_Interface::Init(IMR_InterfaceInitData &InitData)
{
int err;

// Init the engine interface:
err = IMR_Interface::Init(InitData); if (IMR_ISNOTOK(err)) return err;

// Init geometry lists:
GlobalModels.Init(IMR_MAX_GLBMOD);
LocalModels.Init(IMR_MAX_LOCMOD);
GlobalObjects.Init(IMR_MAX_GLBOBJ);
LocalObjects.Init(IMR_MAX_LOCOBJ);
GlobalFigures.Init(IMR_MAX_GLBFIG);
LocalFigures.Init(IMR_MAX_LOCFIG);
Lights.Init(IMR_MAX_LIGHTS);
Cameras.Init(IMR_MAX_CAMERAS);
World.Set_Name("Root");

// Flag that we have been initialized:
Flags.ClassInitialized = 1;

// And return ok:
return IMR_OK;
 }

/***************************************************************************\
  Shuts down the interface.  
  Returns IMR_OK if successful, otherwise an error.
\***************************************************************************/
int IMR_GM_Interface::Shutdown(void)
{
int err;

// Shut down the engine interface:
err = IMR_Interface::Shutdown(); if (IMR_ISNOTOK(err)) return err;

// Reset everything:
Flags.ClassInitialized = 0;

// And return ok:
return IMR_OK;
 }

/***************************************************************************\
  Sets up all the geometries.
  Returns IMR_OK if successful, otherwise an error.
\***************************************************************************/
int IMR_GM_Interface::Prep_Geometries(void)
{
int err, mdl, obj, fig;

// Make sure the class has been initialized:
if (!Flags.ClassInitialized) 
    {
    IMR_LogMsg(__LINE__, __FILE__, "IMR_GM_Interface::Prep_Geometries(): Not initialized!");
    return IMRERR_NOTREADY;
     }

// Setup all the local models:
for (mdl = 0; mdl < LocalModels.Get_Num_Items(); mdl ++)
    {
    err = LocalModels[mdl].Setup();
    if (IMR_ISNOTOK(err)) return err;
     }

// Setup all the global models:
for (mdl = 0; mdl < GlobalModels.Get_Num_Items(); mdl ++)
    {
    err = GlobalModels[mdl].Setup();
    if (IMR_ISNOTOK(err)) return err;
     }

// Setup the local objects:
for (obj = 0; obj < LocalObjects.Get_Num_Items(); obj ++)
    {
    err = LocalObjects[obj].Setup(&GlobalModels, &LocalModels);
    if (IMR_ISNOTOK(err)) return err;
     }
    
// Setup the global objects:
for (obj = 0; obj < GlobalObjects.Get_Num_Items(); obj ++)
    {
    err = GlobalObjects[obj].Setup(&GlobalModels, &LocalModels);
    if (IMR_ISNOTOK(err)) return err;
     }

// Setup local figures:
for (fig = 0; fig < LocalFigures.Get_Num_Items(); fig ++)
    {
    err = LocalFigures[fig].Setup(&GlobalObjects, &LocalObjects);
    if (IMR_ISNOTOK(err)) return err;
     }

// Setup global figures:
for (fig = 0; fig < GlobalFigures.Get_Num_Items(); fig ++)
    {
    err = GlobalFigures[fig].Setup(&GlobalObjects, &LocalObjects);
    if (IMR_ISNOTOK(err)) return err;
     }

// Now go through all the objects and update their coordinates:
World.UpdateCoords();

// And return ok:
return IMR_OK;
 }

/***************************************************************************\
  Attaches two objects.  A NULL parent means the root object.
  Returns IMR_OK if successful, otherwise an error.
\***************************************************************************/
int IMR_GM_Interface::Attach_Object(char *Child, char *Parent)
{
IMR_Object *ParentObj, *ChildObj;

// Make sure the class has been initialized:
if (!Flags.ClassInitialized) 
    {
    IMR_LogMsg(__LINE__, __FILE__, "IMR_GM_Interface::Attach_Object(): Not initialized!");
    return IMRERR_NOTREADY;
     }

// Make sure we have an object:
if (!Child) 
    {
    IMR_LogMsg(__LINE__, __FILE__, "IMR_GM_Interface::Attach_Object(): NULL child specified!");
    return IMRERR_NODATA;
     }

// Make sure we have a parent:
ParentObj = Get_Object(Parent);
if (!ParentObj)
    {
    IMR_LogMsg(__LINE__, __FILE__, "IMR_GM_Interface::Attach_Object(): Parent %s not found!", Parent);
    return IMRERR_NONFATAL_NOTFOUND;
     }

// If the child has already been attached to somthing, lop it off.  Otherwise, look for it
// in the lists:
//if (Get_Object(Child))
//    ChildObj = Lop_Object(Child);
//else
    ChildObj = Get_Object(Child);   // Just attach it for now...

// Make sure we got an object as a result of that debacle:
if (!ChildObj)
    {
    IMR_LogMsg(__LINE__, __FILE__, "IMR_GM_Interface::Attach_Object(): Child %s not found!", Child);
    return IMRERR_NONFATAL_NOTFOUND;
     }

// Now attempt to attach the object to the parent:
int err = ParentObj->Attach_Child(ChildObj); if (IMR_ISNOTOK(err)) return err;

// And return ok:
return IMR_OK;
 }

/***************************************************************************\
  Lops the specified object and all it's children off from the geometry.
  Returns a pointer to the lopped object if successful, otherwise NULL.
\***************************************************************************/
IMR_Object *IMR_GM_Interface::Lop_Object(char *Name)
{
IMR_Object *Obj, *Parent;

// Make sure the class has been initialized:
if (!Flags.ClassInitialized) 
    return NULL;

// Look for the specified object.  If we can't find it, return null:
if (!(Obj = Seek_Object(Name))) return NULL;

// Now get a pointer to it's parent.  If the object doesn't have a parent, 
// we can't lop it, so return null:
if (!(Parent = Obj->Get_Parent())) return NULL;

// Now detach the object from it's parent and return a pointer to the object:
return Parent->Detach_Child(Name);
 }

/***************************************************************************\
  Creates a light with the specified id.
  Returns IMR_OK if successful, otherwise an error.
\***************************************************************************/
int IMR_GM_Interface::Add_Light(int ID)
{
int err;
IMR_Light *Temp;

// Make sure the class has been initialized:
if (!Flags.ClassInitialized) 
    {
    IMR_LogMsg(__LINE__, __FILE__, "IMR_GM_Interface::Add_Light(): Not initialized!");
    return IMRERR_NOTREADY;
     }

Temp = Lights.Add_Item(ID);

// If we couldn't add the item, return an error:
if (!Temp)
    {
    IMR_LogMsg(__LINE__, __FILE__, "IMR_GM_Interface::Add_Light(): Couldn't add light %d!", ID);
    return IMRERR_TOMANY;
     }

// And return ok:
return IMR_OK;
 }

/***************************************************************************\
  Creates a model in the specified list with the specified name.
  Returns IMR_OK if successful, otherwise an error.
\***************************************************************************/
int IMR_GM_Interface::Add_Model(int List, char *Name)
{
int err;
IMR_Model *Temp;

// Make sure the class has been initialized:
if (!Flags.ClassInitialized) 
    {
    IMR_LogMsg(__LINE__, __FILE__, "IMR_GM_Interface::Add_Model(): Not initialized!");
    return IMRERR_NOTREADY;
     }

// Default to local list:
if (List == IMR_LIST_GLOBAL)
    Temp = GlobalModels.Add_Item(Name);
else
    Temp = LocalModels.Add_Item(Name);

// If we couldn't add the item, return an error:
if (!Temp)
    {
    IMR_LogMsg(__LINE__, __FILE__, "IMR_GM_Interface::Add_Model(): Couldn't add model %s!", Name);
    return IMRERR_TOMANY;
     }

// And return ok:
return IMR_OK;
 }

/***************************************************************************\
  Returns a pointer to the specified light.
  If the light doesn't exist, returns null.
\***************************************************************************/
IMR_Light *IMR_GM_Interface::Get_Light(int ID)
{
IMR_Light *Temp;

// Make sure the class has been initialized:
if (!Flags.ClassInitialized) 
    return NULL;

// Check the list:
Temp = Lights.Get_Item(ID, NULL);

// And return a pointer to the light (which, if you recall, is NULL if it 
// couldn't be found):
return Temp;
 }

/***************************************************************************\
  Returns a pointer to the specified model.
  Checks the local list first.  If not found there, checks the global list.
  If the model doesn't exist in either list, returns null.
\***************************************************************************/
IMR_Model *IMR_GM_Interface::Get_Model(char *Name)
{
IMR_Model *Temp;

// Make sure the class has been initialized:
if (!Flags.ClassInitialized) 
    return NULL;

// Check the local list:
Temp = LocalModels.Get_Item(Name, NULL);

// Didn't find it there?  Check the global list:
if (!Temp)
    Temp = GlobalModels.Get_Item(Name, NULL);

// And return a pointer to the model (which, if you recall, is NULL if it 
// couldn't be found):
return Temp;
 }

/***************************************************************************\
  Creates an object with the specified name in the specified list.
  Returns IMR_OK if successful, otherwise an error.
\***************************************************************************/
int IMR_GM_Interface::Add_Object(int List, char *Name)
{
int err;
IMR_Object *Temp;

// Make sure the class has been initialized:
if (!Flags.ClassInitialized) 
    {
    IMR_LogMsg(__LINE__, __FILE__, "IMR_GM_Interface::Add_Object(): Not initialized!");
    return IMRERR_NOTREADY;
     }

// Default to local list:
if (List == IMR_LIST_GLOBAL)
    Temp = GlobalObjects.Add_Item(Name);
else
    Temp = LocalObjects.Add_Item(Name);

// If we couldn't add the item, return an error:
if (!Temp)
    {
    IMR_LogMsg(__LINE__, __FILE__, "IMR_GM_Interface::Add_Object(): Couldn't add object %s!", Name);
    return IMRERR_TOMANY;
     }

// And return ok:
return IMR_OK;
 }

/***************************************************************************\
  Returns a pointer to the specified object.
  Checks the local list first.  If not found there, checks the global list.
  Specifying a NULL name will return the root object.
  If the object doesn't exist in either list, returns null.
\***************************************************************************/
IMR_Object *IMR_GM_Interface::Get_Object(char *Name)
{
IMR_Object *Temp;

// Make sure the class has been initialized:
if (!Flags.ClassInitialized) 
    return NULL;

// Do we want the root object?  Return a pointer to it:
if (!Name) return &World;

// Check the local list:
Temp = LocalObjects.Get_Item(Name, NULL);

// Didn't find it there?  Check the global list:
if (!Temp)
    Temp = GlobalObjects.Get_Item(Name, NULL);

// And return a pointer to the object (which, if you recall, is NULL if it 
// couldn't be found):
return Temp;
 }

/***************************************************************************\
  Creates a figure with the specified name in the specified list.
  Returns IMR_OK if successful, otherwise an error.
\***************************************************************************/
int IMR_GM_Interface::Add_Figure(int List, char *Name)
{
int err;
IMR_Figure *Temp;

// Make sure the class has been initialized:
if (!Flags.ClassInitialized) 
    {
    IMR_LogMsg(__LINE__, __FILE__, "IMR_GM_Interface::Add_Figure(): Not initialized!");
    return IMRERR_NOTREADY;
     }

// Default to local list:
if (List == IMR_LIST_GLOBAL)
    Temp = GlobalFigures.Add_Item(Name);
else
    Temp = LocalFigures.Add_Item(Name);

// If we couldn't add the item, return an error:
if (!Temp)
    {
    IMR_LogMsg(__LINE__, __FILE__, "IMR_GM_Interface::Add_Figure(): Couldn't add figure %s!", Name);
    return IMRERR_TOMANY;
     }

// And return ok:
return IMR_OK;
 }

/***************************************************************************\
  Returns a pointer to the specified figure.
  Checks the local list first.  If not found there, checks the global list.
  If the figure doesn't exist in either list, returns null.
\***************************************************************************/
IMR_Figure *IMR_GM_Interface::Get_Figure(char *Name)
{
IMR_Figure *Temp;

// Make sure the class has been initialized:
if (!Flags.ClassInitialized) 
    return NULL;

// Check the local list:
Temp = LocalFigures.Get_Item(Name, NULL);

// Didn't find it there?  Check the global list:
if (!Temp)
    Temp = GlobalFigures.Get_Item(Name, NULL);

// And return a pointer to the figure
return Temp;
 }

/***************************************************************************\
  Searches for the specified camera and sets up the frame.
  Returns IMR_OK if successful, otherwise an error.
\***************************************************************************/
int IMR_GM_Interface::Begin_Frame(char *CamName)
{
IMR_Camera *Cam;

// Search for the camera.  If it doesn't exist, return an error:
if (!(Cam = Get_Camera(CamName)))
    {
    IMR_LogMsg(__LINE__, __FILE__, "IMR_GM_Interface::Begin_Frame(): Camera %s not found!", CamName);
    return IMRERR_NODATA;
     }

// And return what we get from our engine init:
return IMR_Interface::Begin_Frame(*Cam);
 }

/***************************************************************************\
  Sets up the frame using the specified external camera.
  Returns IMR_OK if successful, otherwise an error.
\***************************************************************************/
int IMR_GM_Interface::Begin_Frame(IMR_Camera *ExternalCamera)
{
// Check for a null pointer:
if (!ExternalCamera)
    {
    IMR_LogMsg(__LINE__, __FILE__, "IMR_GM_Interface::Begin_Frame(): NULL Camera passed!");
    return IMRERR_NODATA;
     }

// And return what we get from our engine init:
return IMR_Interface::Begin_Frame(*ExternalCamera);
 }

/***************************************************************************\
  Adds the geometries to the polygon list.
  Returns IMR_OK if successful, otherwise an error.
\***************************************************************************/
int IMR_GM_Interface::Add_Geometries(void)
{
// Return what we get from our engine add:
return IMR_Interface::Add_Object(World);
 }
