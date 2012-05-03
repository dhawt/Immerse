/***************************************************************************\
  File: IMR_Geom_Object.cpp
  Description: Object geometry module.
  Author: Daniel Hawthorn
  Modified:
  
\***************************************************************************/
#include "IMR_Geom_Object.hpp"

/***************************************************************************\
  Updates the global positioning and rotation of this and each child object.
\***************************************************************************/
void IMR_Object::UpdateCoords(void)
{
// First calculate the initial rotation matrix:
if (Parent)
    {
    GAtd = Parent->GAtd + RAtd;
    RotMtrx.Rotate(GAtd.X, GAtd.Y, GAtd.Z);
     }
else 
    {
    RotMtrx.Rotate(RAtd.X, RAtd.Y, RAtd.Z);
    GAtd = RAtd;
     }

// Now rotate the relative position and find the global pos:
if (Parent)
    {
    GPos = RPos;
    GPos.Transform(Parent->Get_RotMatrix());
    GPos += Parent->GPos;
     }
else
    GPos = RPos;

// Now loop through and update all the kiddies:
for (int index = 0; index < Num_Children; index ++)
    Children[index]->UpdateCoords();
 }

/***************************************************************************\
  Frees all memory associated with this model and resets everything.
  Note: If the object contains the only pointers to the child objects, 
        access to them will be lost.
\***************************************************************************/
void IMR_Object::Reset(void)
{
Name[0] = 0; ModelName[0] = 0;
Num_Lights = Num_Children = 0;
Animation_Frame = Animation_Status = 0;
AttachedModel = NULL;
Parent = NULL;
for (int i = 0; i < IMR_OBJECT_MAXCHILDREN; i ++)
    Children[i] = NULL;
RotMtrx.Identity();
 }

/***************************************************************************\
  Sets the name of the object.
\***************************************************************************/
void IMR_Object::Set_Name(char *NewName)
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
  Searches the specified lists for the model to attach to the object.
\***************************************************************************/
int IMR_Object::Setup(IMR_NamedList<IMR_Model> *GlbMod, 
                       IMR_NamedList<IMR_Model> *LocMod)
{
// Search the local list (if there is one) for the model:
if (LocMod)
    AttachedModel = LocMod->Get_Item(ModelName, NULL);

// If we haven't found the model already, search the global list:
if (!AttachedModel && GlbMod)
    AttachedModel = GlbMod->Get_Item(ModelName, NULL);

return IMR_OK;
 }

/***************************************************************************\
  Attaches the specified light to the object.  
\***************************************************************************/
void IMR_Object::Attach_Light(IMR_Light *Lit)
{
// One more light:
++ Num_Lights;

// If we have too many, return:
if (Num_Lights > IMR_OBJECT_MAXLIGHTS) { Num_Lights = IMR_OBJECT_MAXLIGHTS; return; }

// Add the pointer to the light to the end of the list:
AttachedLights[Num_Lights - 1] = Lit;
 }

/***************************************************************************\
  Detaches the light with the specified ID from the object (if it exists).
\***************************************************************************/
void IMR_Object::Detach_Light(int ID)
{
int ItemIndex, Index;

// Get an index to the item:
if (!Get_Light(ID, &ItemIndex)) return;

// Shift all the items over the item we are deleting:
for (Index = ItemIndex; Index < Num_Lights - 1; Index ++)
    AttachedLights[Index] = AttachedLights[Index + 1];

// One less item:
-- Num_Lights;
 }

/***************************************************************************\
  Returns a pointer to the light with the specified ID.
\***************************************************************************/
IMR_Light *IMR_Object::Get_Light(int ID, int *index)
{
// Search each item in the list:
for (int item = 0; item < Num_Lights; item ++)
    if (AttachedLights[item]->Is(ID))
        {
        if (index) *index = item;
        return AttachedLights[item];
         }

// No match was found, so return null:
return NULL;
 }

/***************************************************************************\
  Attaches the specified child to the object.
  Returns IMR_OK if successful.
\***************************************************************************/
int IMR_Object::Attach_Child(IMR_Object *Obj)
{
// Make sure we have an object:
if (!Obj)
    {
    IMR_SetErrorText("IMR_Object::Attach_Child(): NULL object specified!");
    return IMRERR_NODATA;
     }

// One more child:
++ Num_Children;

// If we have too many, return:
if (Num_Children > IMR_OBJECT_MAXCHILDREN) 
    {
    Num_Children = IMR_OBJECT_MAXCHILDREN; 
    IMR_SetErrorText("IMR_Object::Attach_Child(): To many children in object %s! (%d)", Name, IMR_OBJECT_MAXCHILDREN);
    return IMRERR_TOMANY;
     }

// Add the pointer to the child to the end of the list:
Children[Num_Children - 1] = Obj;

// And add a pointer to ourselves (it's parent):
Children[Num_Children - 1]->Parent = this;

// And return ok:
return IMR_OK;
 }

/***************************************************************************\
  Detaches the child with the specified name from the object.
  Returns a pointer to the detached child if successful, otherwise null.
\***************************************************************************/
IMR_Object *IMR_Object::Detach_Child(char *Name)
{
IMR_Object *Temp;
int ItemIndex;

// Get an index to the child:
if (!Get_Child(Name, &ItemIndex)) return NULL;

// Save a pointer to the child:
Temp = Children[ItemIndex];

// Shift all the items over the item we are deleting:
memmove((void *)&Children[ItemIndex], 
        (void *)&Children[ItemIndex + 1], 
        sizeof(IMR_Object) * (Num_Children - ItemIndex - 1));

// One less child:
-- Num_Children;

// And return a pointer to the child:
return Temp;
 }

/***************************************************************************\
  Returns a pointer to the child with the specified name.
\***************************************************************************/
IMR_Object *IMR_Object::Get_Child(char *Name, int *index)
{
// Search each item in the list:
for (int item = 0; item < Num_Children; item ++)
    if (Children[item]->Is(Name))
        {
        if (index) *index = item;
        return Children[item];
         }

// No match was found, so return null:
return NULL;
 }

/***************************************************************************\
  Sets the name of the model for this object.
\***************************************************************************/
void IMR_Object::Set_ModelName(char *Name)
{
if (strlen(Name) > 8)
    {
    memcpy((void *)ModelName, (void *)Name, 8);
    ModelName[8] = '/0';
     }
else
    strcpy(ModelName, Name);
 }

/***************************************************************************\
  "Jumps" the object to the specified relative position and attitude.
\***************************************************************************/
void IMR_Object::Animation_Jump(IMR_3DPoint &Pos, IMR_Attitude &Atd)
{
RPos = Pos;
RAtd = Atd;
UpdateCoords();
 }

/***************************************************************************\
  Initializes an animation to the specified relative position and attitude
  over the specified number of frames.
\***************************************************************************/
void IMR_Object::Animation_Init(IMR_3DPoint &Pos, IMR_Attitude &Atd, int Length)
{
FIXEDPNT XDelta, YDelta, ZDelta;

// Set destination position and attitude:
DestPos = Pos;
DestAtd = Atd;
if (!Length) return;

// Set position change vector:
if (RPos != Pos)
    {
    XDelta = Pos.X - RPos.X;
    YDelta = Pos.Y - RPos.Y;
    ZDelta = Pos.Z - RPos.Z;

    if (XDelta) PosVect.X = XDelta / Length; 
    else PosVect.X = 0;

    if (YDelta) PosVect.Y = YDelta / Length;
    else PosVect.Y = 0;

    if (ZDelta) PosVect.Z = ZDelta / Length;
    else PosVect.Z = 0;
     }
else
    PosVect.X = PosVect.Y = PosVect.Z = 0;

// Set attitude change vector:
if (RAtd != Atd)
    {
    XDelta = Atd.X - RAtd.X;
    YDelta = Atd.Y - RAtd.Y;
    ZDelta = Atd.Z - RAtd.Z;

    if (XDelta)
        {
        if (XDelta > IMR_HALFDEGREECOUNT || XDelta < -IMR_HALFDEGREECOUNT) 
            XDelta = -((IMR_DEGREECOUNT - Atd.X) + RAtd.X);
        if (XDelta < -IMR_HALFDEGREECOUNT) 
            XDelta = (IMR_DEGREECOUNT - RAtd.X) + Atd.X;
        AtdVect.X = XDelta / Length;
         }
    else AtdVect.X = 0;
            
    if (YDelta)
        {
        if (YDelta > IMR_HALFDEGREECOUNT || YDelta < -IMR_HALFDEGREECOUNT)
            YDelta = -((IMR_DEGREECOUNT - Atd.Y) + RAtd.Y);
        if (YDelta < -IMR_HALFDEGREECOUNT)
            YDelta = (IMR_DEGREECOUNT - RAtd.Y) + Atd.Y;
        AtdVect.Y = YDelta / Length;
         }
    else AtdVect.Y = 0;
                 
    if (ZDelta)
        {
        if (ZDelta > IMR_HALFDEGREECOUNT || ZDelta < -IMR_HALFDEGREECOUNT)
            ZDelta = -((IMR_DEGREECOUNT - Atd.Z) + RAtd.Z);
        if (ZDelta < -IMR_HALFDEGREECOUNT)
            ZDelta = (IMR_DEGREECOUNT - RAtd.Z) + Atd.Z;
        AtdVect.Z = ZDelta / Length;
         }
    else AtdVect.Z = 0;
     }
else
    AtdVect.X = AtdVect.Y = AtdVect.Z;
 
// Reset counters:
Animation_Status = IMR_ANIMATION_ACTIVE;
Animation_Frame = Length;
 }

/***************************************************************************\
  Changes to the next frame in an animation.
\***************************************************************************/
int IMR_Object::Animation_Step(void)
{
// Make sure an animation is in progress:
if (Animation_Status == IMR_ANIMATION_DONE) return IMR_ANIMATION_DONE;

// Increment attitude and position:
RPos += PosVect; RPos.TransformedToActive();
RAtd += AtdVect;
UpdateCoords();
 
// Decrement counter:
-- Animation_Frame; 

// Tell the user our status:
if (!Animation_Frame) 
    {
    // Set status:
    Animation_Status = IMR_ANIMATION_DONE;
    
    // Set positions and attitudes exactly:
    RPos = DestPos;
    RAtd = DestAtd;
    UpdateCoords();
    PosVect.X = PosVect.Y = PosVect.Z = 0;
    AtdVect.X = AtdVect.Y = AtdVect.Z = 0;

    // Return status:
    return IMR_ANIMATION_DONE;
     }
else 
    {
    Animation_Status = IMR_ANIMATION_ACTIVE;
    return IMR_ANIMATION_ACTIVE;
     }
 }
