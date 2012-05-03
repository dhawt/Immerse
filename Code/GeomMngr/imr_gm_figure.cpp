/****************************************************************\
 
 iMMERSE Engine
 (C) 1999 No Tears Shed Software
 All rights reserved
 
 Filename: IMR_GM_Figure.hpp
 Description: Figure module
 
\****************************************************************/
#include "IMR_GM_Figure.hpp"

/***************************************************************************\
**
**  Skeleton Segment stuff
**
\***************************************************************************/

/***************************************************************************\
  Sets the name of the skeleton segment.
\***************************************************************************/
void IMR_SkelSegment::Set_Name(char *NewName)
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
  Sets the name of the object for skeleton segment.
\***************************************************************************/
void IMR_SkelSegment::Set_ObjectName(char *Name)
{
if (strlen(Name) > 8)
    {
    memcpy((void *)ObjName, (void *)Name, 8);
    ObjName[8] = '/0';
     }
else
    strcpy(ObjName, Name);
 }


/***************************************************************************\
**
**  Key skeleton stuff
**
\***************************************************************************/

/***************************************************************************\
  Sets the name of the skeleton.
\***************************************************************************/
void IMR_Skeleton::Set_Name(char *NewName)
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
  Searches the specified lists for the objects to attach to each segment.
\***************************************************************************/
int IMR_Skeleton::Setup(IMR_NamedList<IMR_Object> *GlbObj, 
                        IMR_NamedList<IMR_Object> *LocObj)
{
IMR_SkelSegment *CurrSeg = Segs;
char *ObjName;
IMR_Object *Tmp;

// Loop while we have a segment:
while (CurrSeg)
    {
    // Get the name of the object for the segment:
    ObjName = CurrSeg->Get_ObjectName();
    
    // Search the local list (if there is one) for the object:
    if (LocObj)
        Tmp = LocObj->Get_Item(ObjName, NULL);

    // If we haven't found the object already, search the global list:
    if (!Tmp && GlbObj)
        Tmp = GlbObj->Get_Item(ObjName, NULL);
    
    // Set the object:
    CurrSeg->Set_Object(Tmp);
    
    // Set next segment:
    CurrSeg = CurrSeg->Get_Next();
     }

// And return ok:
return IMR_OK;
 }

/***************************************************************************\
  Adds a new segment into the skeleton.
  Returns IMR_OK if successful, otherwise an error.
\***************************************************************************/
int IMR_Skeleton::Add_Segment(char *Name)
{
IMR_SkelSegment *NewSeg;

// Create a new segment:
NewSeg = new IMR_SkelSegment;

// If we're out of memory, return an error:
if (!NewSeg)
    {
    IMR_LogMsg(__LINE__, __FILE__, "IMR_Skeleton::Add_Segment(): Out of memory in segment %s!", Name);
    return IMRERR_OUTOFMEM;
     }

// Now make this segment the head:
NewSeg->Set_Next(Segs);
Segs = NewSeg;

// And give it a name:
NewSeg->Set_Name(Name);

// One more segment:
++ Num_Segs;

// And return ok:
return IMR_OK;
 }

/***************************************************************************\
  Returns a pointer to the specified segment if found, otherwise NULL.
\***************************************************************************/
IMR_SkelSegment *IMR_Skeleton::Get_Segment(char *Name)
{
IMR_SkelSegment *Curr = Segs;

// Loop while we have a segment:
while (Curr)
    {
    if (Curr->Is(Name))
        return Curr;
    Curr = Curr->Get_Next();
     }

// No segment by that name, so return null:
return NULL;
 }

/***************************************************************************\
  Deletes the specified segment.
\***************************************************************************/
void IMR_Skeleton::Delete_Segment(char *Name)
{
IMR_SkelSegment *Curr = Segs,
                *Prev = NULL;

// Find the specified segment:
while (Curr && !Curr->Is(Name))
    {
    Prev = Curr;
    Curr = Curr->Get_Next();
     }

// If we couldn't find the segment, return:
if (!Curr) return;

// Unlink the segment:
if (Prev)
    Prev->Set_Next(Curr->Get_Next());
else
    Segs = Curr->Get_Next();

// And delete the node:
delete Curr;

// One less seg:
++ Num_Segs;
 }

/***************************************************************************\
  Deletes all the segments in the list.
\***************************************************************************/
void IMR_Skeleton::Wipe_Segments(void)
{
IMR_SkelSegment *Curr = Segs, *Next;

// Loop and delete while we have a segment:
while (Curr)
    {
    Next = Curr->Get_Next();
    delete Curr;
    Curr = Next;
     }

// And set segs to null:
Segs = NULL;
Num_Segs = 0;
 }

/***************************************************************************\
**
**  Keyed figure stuff
**
\***************************************************************************/

/***************************************************************************\
  Sets the name of the figure.
\***************************************************************************/
void IMR_Figure::Set_Name(char *NewName)
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
  Sets up the figure.
\***************************************************************************/
int IMR_Figure::Setup(IMR_NamedList<IMR_Object> *GlbObj, 
                      IMR_NamedList<IMR_Object> *LocObj)
{
IMR_Skeleton *CurrKey = Keys;
IMR_Object *Tmp;

// Setup our root object (if it hasn't been already):
if (!RootObj)
    {
    if (LocObj) Tmp = LocObj->Get_Item(RootObjName, NULL);
    if (!Tmp && GlbObj)
        Tmp = GlbObj->Get_Item(RootObjName, NULL);
    Set_RootObj(Tmp);
     }
     
// Loop while we have a key skeleton:
while (CurrKey)
    {
    // Setup the key:
    CurrKey->Setup(GlbObj, LocObj);
    
    // Set next key:
    CurrKey = CurrKey->Get_Next();
     }

// And return ok:
return IMR_OK;
 }

/***************************************************************************\
  Adds a new key into the figure.
  Returns IMR_OK if successful, otherwise an error.
\***************************************************************************/
int IMR_Figure::Add_Key(char *Name)
{
IMR_Skeleton *NewKey;

// Create a new key:
NewKey = new IMR_Skeleton;

// If we're out of memory, return an error:
if (!NewKey)
    {
    IMR_LogMsg(__LINE__, __FILE__, "IMR_Figure::Add_Key(): Out of memory in skeleton %s!", Name);
    return IMRERR_OUTOFMEM;
     }

// Now make this segment the head:
NewKey->Set_Next(Keys);
Keys = NewKey;

// And give it a name:
NewKey->Set_Name(Name);

// One more key:
++ Num_Keys;

// And return ok:
return IMR_OK;
 }

/***************************************************************************\
  Returns a pointer to the specified key if found, otherwise NULL.
\***************************************************************************/
IMR_Skeleton *IMR_Figure::Get_Key(char *Name)
{
IMR_Skeleton *Curr = Keys;

// Loop while we have a key:
while (Curr)
    {
    if (Curr->Is(Name))
        return Curr;
    Curr = Curr->Get_Next();
     }

// No key by that name, so return null:
return NULL;
 }

/***************************************************************************\
  Deletes the specified key.
\***************************************************************************/
void IMR_Figure::Delete_Key(char *Name)
{
IMR_Skeleton *Curr = Keys, *Prev = NULL;

// Find the specified key:
while (Curr && !Curr->Is(Name))
    {
    Prev = Curr;
    Curr = Curr->Get_Next();
     }

// If we couldn't find the key, return:
if (!Curr) return;

// Unlink the key:
if (Prev)
    Prev->Set_Next(Curr->Get_Next());
else
    Keys = Curr->Get_Next();

// And delete the node:
delete Curr;

// One less key:
++ Num_Keys;
 }

/***************************************************************************\
  Deletes all the keys in the list.
\***************************************************************************/
void IMR_Figure::Wipe_Keys(void)
{
IMR_Skeleton *Curr = Keys, *Next;

// Loop and delete while we have a key:
while (Curr)
    {
    Next = Curr->Get_Next();
    delete Curr;
    Curr = Next;
     }

// And set keys to null:
Keys = NULL;
Num_Keys = 0;
 }

/***************************************************************************\
  "Jumps" the figure to the specified relative position and attitude.
\***************************************************************************/
void IMR_Figure::Animation_Jump(char *KeyName)
{
IMR_Skeleton *Key;
IMR_SkelSegment *Seg;
IMR_Object *Obj;

// Find the specified key:
if (!(Key = Get_Key(KeyName))) return;

// Now loop through all the segments in the key:
Seg = Key->GetFirstSegment();
while (Seg)
    {
    // Get the object:
    Obj = Seg->Get_Object();
    
    // Make sure there is an object:
    if (Obj) Obj->Animation_Jump(Seg->Get_Pos(), Seg->Get_Atd());
    
    // Get the next segment:
    Seg = Seg->Get_Next();
     }
 }

/***************************************************************************\
  Initializes an animation to the specified relative position and attitude
  over the specified number of frames.
\***************************************************************************/
void IMR_Figure::Animation_Init(char *KeyName, int Length)
{
IMR_Skeleton *Key;
IMR_SkelSegment *Seg;
IMR_Object *Obj;

// Find the specified key:
if (!(Key = Get_Key(KeyName))) return;

// Now loop through all the segments in the key:
Seg = Key->GetFirstSegment();
while (Seg)
    {
    // Get the object:
    Obj = Seg->Get_Object();
    
    // Make sure there is an object:
    if (Obj) 
        Obj->Animation_Init(Seg->Get_Pos(), Seg->Get_Atd(), Length);
   
    // Get the next segment:
    Seg = Seg->Get_Next();
     }

// Now setup stuff:
Animation_Status = IMR_ANIMATION_ACTIVE;
Animation_Time = 0;
Animation_Length = Length;
Animation_Key = Key;
 }

/***************************************************************************\
  Changes to the next frame in an animation.
\***************************************************************************/
int IMR_Figure::Animation_Step(void)
{
IMR_Skeleton *Key;
IMR_SkelSegment *Seg;
IMR_Object *Obj;

// Make sure an animation is in progress:
if (Animation_Status == IMR_ANIMATION_DONE) return IMR_ANIMATION_DONE;

// Find the specified key:
Key = Animation_Key;

// Now loop through all the segments in the key:
Seg = Key->GetFirstSegment();
while (Seg)
    {
    // Get the object:
    Obj = Seg->Get_Object();
    
    // Make sure there is an object:
    if (Obj) Obj->Animation_Step();
    
    // Get the next segment:
    Seg = Seg->Get_Next();
     }

// Increment counter:
Animation_Time += IMR_Time_GetFrameTime(); 

// Tell the user our status:
if (Animation_Time >= Animation_Length) 
    {
    Animation_Status = IMR_ANIMATION_DONE;
    return IMR_ANIMATION_DONE;
     }
else 
    {
    Animation_Status = IMR_ANIMATION_ACTIVE;
    return IMR_ANIMATION_ACTIVE;
     }
 }

/***************************************************************************\
  Writes the figure to the specified RDF.
  Returns OK if succesfull.
\***************************************************************************/
/*
int IMR_Figure::Write_RDF(char *Filename)
{
IMR_Segment *Current;
IMR_ObjectKF_Joint *Curr_Joint;
int Current_Seg, Current_KF;
int RDF, Temp;

// Open the file:
RDF = open(Filename, O_RDWR | O_BINARY | O_CREAT | O_TRUNC, 
           S_IWUSR | S_IRUSR);

// If we can't, return an error:
if (RDF == -1) 
    { 
    ErrorCall("Could not open %s! (Object::Write_RDF())", Filename);
    return ERR_BADFILE; 
     }

// Write id string:
write(RDF, "OBJ", 3);

// Write name of object:
write(RDF, Name, 8);

// Write number of segs:
write(RDF, &Num_Segs, 2);

// Write number of keyframes:
write(RDF, &Num_Keyframes, 2);

// Loop through each seg and write it to the file:
for (Current_Seg = 0; Current_Seg < Num_Segs; ++ Current_Seg)
    {
    // Set the current segment to this seg:
    Current = Seg_List[Current_Seg];
    
    // Write the name of the segment:        
    write(RDF, Current->Name, 8);

    // Write the name of the model attached to this segment:
    write(RDF, Current->Model_Name, 8);

    // Write the pos:
    Temp = Current->Position.X >> 16; write(RDF, &Temp, 2);
    Temp = Current->Position.Y >> 16; write(RDF, &Temp, 2);
    Temp = Current->Position.Z >> 16; write(RDF, &Temp, 2);
    
    // Write the angle:
    write(RDF, &Current->Angle.X, 2);
    write(RDF, &Current->Angle.Y, 2);
    write(RDF, &Current->Angle.Z, 2);
     }

// Write all the keyframes:
for (Current_KF = 0; Current_KF < Num_Keyframes; Current_KF ++)
    {
    // Write the name of the keyframe:
    write(RDF, Keyframes[Current_KF].Name, 8);

    // Write the number of joints in the keyframe:
    write(RDF, &Keyframes[Current_KF].Num_Joints, 2);

    // Write all the joints:
    Curr_Joint = Keyframes[Current_KF].JointList;
    while (Curr_Joint)
        {
        // Write the segment for the joint:
        write(RDF, Curr_Joint->Seg_Name, 8);

        // Write the pos:
        Temp = Curr_Joint->Position.X >> 16; write(RDF, &Temp, 2);
        Temp = Curr_Joint->Position.Y >> 16; write(RDF, &Temp, 2);
        Temp = Curr_Joint->Position.Z >> 16; write(RDF, &Temp, 2);
        
        // Write the angle:
        write(RDF, &Curr_Joint->Angle.X, 2);
        write(RDF, &Curr_Joint->Angle.Y, 2);
        write(RDF, &Curr_Joint->Angle.Z, 2);

        // Set next joint: 
        Curr_Joint = Curr_Joint->Next;
         }
     }

// Close the file:
close(RDF); 

// And return OK:
return OK;
 }
*/
