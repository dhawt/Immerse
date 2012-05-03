/****************************************************************\
 
 iMMERSE Engine
 (C) 1999 No Tears Shed Software
 All rights reserved
 
 Filename: IMR_Geom_Object.hpp
 Description: Object geometry module
 
\****************************************************************/
#include "IMR_Geom_Object.hpp"
#include "IMR_Collide.hpp"

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
  Frees all memory associated with this object and resets everything.
  Note: If the object contains the only pointers to the child objects, 
        access to them will be lost.
\***************************************************************************/
void IMR_Object::Reset(void)
{
Name[0] = 0; ModelName[0] = 0;
Num_Lights = Num_Children = 0;
Animation_Time = Animation_Length = Animation_Status = 0;
AttachedModel = NULL;
Parent = NULL;
for (int i = 0; i < IMR_OBJECT_MAXCHILDREN; i ++)
    Children[i] = NULL;
RotMtrx.Identity();

/// HACKHACKHACK
Collidable = 0;
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
  Merges this and all children into one equivalent model.
  Notes: Offset should be the offset position to merge the models to 
         in the dest model.
  Returns IMR_OK if successful.
\***************************************************************************/
int IMR_Object::MergeToModel(IMR_Model *Mdl, IMR_3DPoint Offset)
{
// Verify we have a real model passed to us:
if (!Mdl)
    {
    IMR_LogMsg(__LINE__, __FILE__, "NULL model passed!");
    return IMRERR_GENERIC;
     }

// Adjust the offset to the position of this object (in model space!):
Offset += RPos;

// Merge this object's model with the passed model (if there is one):
if (AttachedModel)
    if (Mdl->CombineModel(AttachedModel, Offset, GAtd) != IMR_OK)
        return IMRERR_GENERIC;

// Loop through each child and merge them:
int Status = IMR_OK;
for (int item = 0; item < Num_Children; item ++)
    if (Children[item]) Status |= Children[item]->MergeToModel(Mdl, Offset);

// Check status and return:
if (Status == IMR_OK) return IMR_OK;
return IMRERR_GENERIC;
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
    IMR_LogMsg(__LINE__, __FILE__, "IMR_Object::Attach_Child(): NULL object specified!");
    return IMRERR_NODATA;
     }

// One more child:
++ Num_Children;

// If we have too many, return:
if (Num_Children > IMR_OBJECT_MAXCHILDREN) 
    {
    Num_Children = IMR_OBJECT_MAXCHILDREN; 
    IMR_LogMsg(__LINE__, __FILE__, "IMR_Object::Attach_Child(): To many children in object %s! (%d)", Name, IMR_OBJECT_MAXCHILDREN);
    return IMRERR_TOMANY;
     }

// Add the pointer to the child to the end of the list:
Children[Num_Children - 1] = Obj;

// And add a pointer to ourselves (it's parent):
Children[Num_Children - 1]->Parent = this;

// And update our childs coords:
Obj->UpdateCoords();

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
        sizeof(IMR_Object *) * (Num_Children - ItemIndex - 1));

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
void IMR_Object::Set_ModelName(char *MName)
{
if (strlen(Name) > 8)
    {
    memcpy((void *)ModelName, (void *)MName, 8);
    ModelName[8] = '/0';
     }
else
    strcpy(ModelName, MName);
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
  --Modified 3/9/00 for nonlinear interpolation--
\***************************************************************************/
void IMR_Object::Animation_Init(IMR_3DPoint &Pos, IMR_Attitude &Atd, int L)
{
float XDelta, YDelta, ZDelta;

// Copy current position:
StartPos = RPos;
StartAtd.X = (float)RAtd.X;
StartAtd.Y = (float)RAtd.Y;
StartAtd.Z = (float)RAtd.Z;

// Set destination position and attitude:
DestPos = Pos;
DestAtd = Atd;
if (L <= 0) return;

// Set position change vector:
if (RPos != Pos)
    {
    PosVect.X = Pos.X - RPos.X;
    PosVect.Y = Pos.Y - RPos.Y;
    PosVect.Z = Pos.Z - RPos.Z;
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
        AtdVect.X = (float)XDelta;
         }
    else AtdVect.X = 0;
            
    if (YDelta)
        {
        if (YDelta > IMR_HALFDEGREECOUNT || YDelta < -IMR_HALFDEGREECOUNT)
            YDelta = -((IMR_DEGREECOUNT - Atd.Y) + RAtd.Y);
        if (YDelta < -IMR_HALFDEGREECOUNT)
            YDelta = (IMR_DEGREECOUNT - RAtd.Y) + Atd.Y;
        AtdVect.Y = (float)YDelta;
         }
    else AtdVect.Y = 0;
                 
    if (ZDelta)
        {
        if (ZDelta > IMR_HALFDEGREECOUNT || ZDelta < -IMR_HALFDEGREECOUNT)
            ZDelta = -((IMR_DEGREECOUNT - Atd.Z) + RAtd.Z);
        if (ZDelta < -IMR_HALFDEGREECOUNT)
            ZDelta = (IMR_DEGREECOUNT - RAtd.Z) + Atd.Z;
        AtdVect.Z = (float)ZDelta;
         }
    else AtdVect.Z = 0;
     }
else
    AtdVect.X = AtdVect.Y = AtdVect.Z = 0;
 
// Reset counters:
Animation_Status = IMR_ANIMATION_ACTIVE;
Animation_Time = 0;
Animation_Length = L;
 }

/***************************************************************************\
  Changes to the next frame in an animation.
  --Modified 3/6/00 for nonlinear interpolation--
\***************************************************************************/
int IMR_Object::Animation_Step(void)
{
float q;

// Make sure an animation is in progress:
if (Animation_Status == IMR_ANIMATION_DONE) return IMR_ANIMATION_DONE;

// Increment counter and calculate interpolant
Animation_Time += IMR_Time_GetFrameTime(); 
q = (float)Animation_Time / (float)Animation_Length;

// Update attitude and position:
RPos.X = StartPos.X + (PosVect.X * q);
RPos.Y = StartPos.Y + (PosVect.Y * q);
RPos.Z = StartPos.Z + (PosVect.Z * q);
if (PosVect.X != 0.0)
    if (PosVect.X > 0.0) { if (RPos.X > DestPos.X) RPos.X = DestPos.X; } else { if (RPos.X < DestPos.X) RPos.X = DestPos.X; };
if (PosVect.Y != 0.0)
    if (PosVect.Y > 0.0) { if (RPos.Y > DestPos.Y) RPos.Y = DestPos.Y; } else { if (RPos.Y < DestPos.Y) RPos.Y = DestPos.Y; };
if (PosVect.Z != 0.0)
    if (PosVect.Z > 0.0) { if (RPos.Z > DestPos.Z) RPos.Z = DestPos.Z; } else { if (RPos.Z < DestPos.Z) RPos.Z = DestPos.Z; };
RAtd.X = int(StartAtd.X + (AtdVect.X * q));
RAtd.Y = int(StartAtd.Y + (AtdVect.Y * q));
RAtd.Z = int(StartAtd.Z + (AtdVect.Z * q));
RAtd.Fix_Ang();
/*  --removed 3.28.00, seems it caused some problems and isn't really needed ---
if (AtdVect.X != 0.0)    
    if (AtdVect.X > 0.0) { if (RAtd.X > DestAtd.X) RAtd.X = DestAtd.X; } else { if (RAtd.X < DestAtd.X) RAtd.X = DestAtd.X; };
if (AtdVect.Y != 0.0)
    if (AtdVect.Y > 0.0) { if (RAtd.Y > DestAtd.Y) RAtd.Y = DestAtd.Y; } else { if (RAtd.Y < DestAtd.Y) RAtd.Y = DestAtd.Y; };
if (AtdVect.Z != 0.0)
    if (AtdVect.Z > 0.0) { if (RAtd.Z > DestAtd.Z) RAtd.Z = DestAtd.Z; } else { if (RAtd.Z < DestAtd.Z) RAtd.Z = DestAtd.Z; };
*/
UpdateCoords();
 
// Tell the system our status:
if (Animation_Time >= Animation_Length) 
    {
    // Set status:
    Animation_Status = IMR_ANIMATION_DONE;
    
    // Set positions and attitudes exactly:
    RPos = DestPos;
    RAtd = DestAtd;
    UpdateCoords();
    PosVect.X = PosVect.Y = PosVect.Z = 0.0;
    AtdVect.X = AtdVect.Y = AtdVect.Z = 0.0;

    // Return status:
    return IMR_ANIMATION_DONE;
     }
else 
    {
    Animation_Status = IMR_ANIMATION_ACTIVE;
    return IMR_ANIMATION_ACTIVE;
     }
 }

/***************************************************************************\
  Moves the object by the delta based on the specified rate.
  Does not do any collision detection.
\***************************************************************************/
void IMR_Object::Motion_Travel(IMR_3DPoint &Delta, float s)
{
float q = s * IMR_Time_GetNormalizedFrameTime();
RPos.X += (Delta.X * q);
RPos.Y += (Delta.Y * q);
RPos.Z += (Delta.Z * q);
UpdateCoords();
 }

/***************************************************************************\
  Moves the object by the delta based on the specified rate, if possible
  based on collision detection.  Performs collision responce to walls,
  ceilings, steps, floors, and drops if desired.
\***************************************************************************/
int IMR_Object::Motion_Travel_CheckCollide(IMR_3DPoint &Delta, float s, IMR_Object *Environ, IMR_CollideInfo &CInfo)
{
float q = s * IMR_Time_GetNormalizedFrameTime();
IMR_3DPoint NewPos;
IMR_3DPoint EllipsoidSource, EllipsoidVelocity;
int ActionFlags;

// Make sure we have an object to check:
if (!Environ)
    {
    IMR_LogMsg(__LINE__, __FILE__, "Null object passed as environ!");
    return IMRERR_NODATA;
     };

// Convert to ellipsoid space:
EllipsoidSource.X = GPos.X * CInfo.invRh;
EllipsoidSource.Y = GPos.Y * CInfo.invRv;
EllipsoidSource.Z = GPos.Z * CInfo.invRh;
EllipsoidVelocity.X = Delta.X * CInfo.invRh * q;
EllipsoidVelocity.Y = Delta.Y * CInfo.invRv * q;
EllipsoidVelocity.Z = Delta.Z * CInfo.invRh * q;

// Move and check for collisions:
NewPos = Environ->CheckCollide(CInfo, EllipsoidSource, EllipsoidVelocity);

// Convert new position to spheroid space:
NewPos.X *= CInfo.Rh;
NewPos.Y *= CInfo.Rv;
NewPos.Z *= CInfo.Rh;

// Set our new pos:
if (Parent)
    RPos = NewPos - Parent->GPos;
else
    RPos = NewPos;

// Update our coords:
UpdateCoords();

// And return ok:
return IMR_OK;
 }

/***************************************************************************\
  Turns the object by the delta based on the specified rate.
\***************************************************************************/
void IMR_Object::Motion_Turn(IMR_Attitude &Delta, float r)
{
int q = (int)(r * IMR_Time_GetNormalizedFrameTime());
RAtd.X += (Delta.X * q);
RAtd.Y += (Delta.Y * q);
RAtd.Z += (Delta.Z * q);
UpdateCoords();
 }

/***************************************************************************\
  Checks for and handles a collision on this object and it's children.
  Position passed in collision info structure should be in global coords.
  Returns position after collision responce.
\***************************************************************************/
IMR_3DPoint IMR_Object::CheckCollide(IMR_CollideInfo &CInfo, IMR_3DPoint position, IMR_3DPoint velocity)
{
IMR_3DPoint Pos;
 
// Check if we need to move at all:
if (velocity.Mag() < IMR_COLLIDE_EPSILON) return position;

// Find destination point:
IMR_3DPoint destinationPoint = position + velocity;
 
// reset the collision package we send to the mesh 
CInfo.Velocity = velocity;
CInfo.SourcePoint = position;
CInfo.foundCollision = FALSE;
CInfo.stuck = FALSE;
CInfo.nearestDistance = -1;      

/*
// Loop through the children and check for collisions on each:
IMR_Object *Obj;
for (int child = 0; child < Get_Num_Children(); child ++)
    if (Obj = Get_Child(child))
        {
        CInfo.foundCollision = FALSE;
        CInfo.stuck = FALSE;
        CInfo.nearestDistance = -1;      
        Obj->CheckCollide(CInfo, CInfo.SourcePoint, CInfo.Velocity);
         }
*/

// Check for a collision with this model (if it exists):
IMR_Model *Mdl;
if ((Mdl = Get_Model()) && Collidable) IMR_Collide_CheckModCollision(Mdl, GPos, GAtd, CInfo);

// Check return value here, and possibly call recursively:
if (!CInfo.foundCollision)
    { 
    // If no collision move very close to the desired destination:
    float l = velocity.Mag();
    IMR_3DPoint V = velocity; 
    V.Set_Length(l - IMR_COLLIDE_EPSILON);
        
    // Update the last safe position for future error recovery:
    CInfo.lastSafePosition = position;      

    // Return the final position:
    return position + V;
     }
else // There was a collision...
    { 
    // If we are stuck, we just back up to last safe position:
    if (CInfo.stuck) return CInfo.lastSafePosition;
      
    // OK, first task is to move close to where we hit something:
    IMR_3DPoint newSourcePoint;
                   
    // Only update if we are not already very close:
    if (CInfo.nearestDistance >= IMR_COLLIDE_EPSILON)
        {
        IMR_3DPoint V = velocity;
        V.Set_Length(CInfo.nearestDistance - IMR_COLLIDE_EPSILON);
        newSourcePoint = CInfo.SourcePoint + V;
         }
    else
        newSourcePoint = CInfo.SourcePoint;

    // Now we must calculate the sliding plane:
    IMR_3DPoint slidePlaneOrigin = CInfo.nearestPolygonIntersectionPoint;
    IMR_3DPoint slidePlaneNormal = newSourcePoint - CInfo.nearestPolygonIntersectionPoint;
  
    // We now project the destination point onto the sliding plane:
    double l = IMR_Collide_IntersectRayPlane(destinationPoint, slidePlaneNormal, slidePlaneOrigin, slidePlaneNormal); 
  
    // We can now calculate a new destination point on the sliding plane:
    IMR_3DPoint newDestinationPoint;
    newDestinationPoint.X = destinationPoint.X + l * slidePlaneNormal.X;
    newDestinationPoint.Y = destinationPoint.Y + l * slidePlaneNormal.Y;
    newDestinationPoint.Z = destinationPoint.Z + l * slidePlaneNormal.Z;
   
    // Generate the slide vector, which will become our new velocity vector
    // for the next iteration:
    IMR_3DPoint newVelocityVector = newDestinationPoint - CInfo.nearestPolygonIntersectionPoint;
        
    // now we recursively call the function with the new position and velocity 
    CInfo.lastSafePosition = position;
    return CheckCollide(CInfo, newSourcePoint, newVelocityVector);
     }
return position;
 }

