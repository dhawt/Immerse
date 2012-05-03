/***************************************************************************\
  File: IMR_Camera.cpp
  Description: Camera module.
  Author: Daniel Hawthorn
  Modified:
  
\***************************************************************************/
#include "IMR_Camera.hpp"

/***************************************************************************\
  Sets the name of the camera.
\***************************************************************************/
void IMR_Camera::Set_Name(char *NewName)
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
  Initializes a motion to the specified position and attitude over the 
  specified number of frames.
\***************************************************************************/
void IMR_Camera::Motion_Init(IMR_3DPoint &P, IMR_Attitude &A, int Length)
{
float XDelta, YDelta, ZDelta;

// Set destination position and attitude:
DestPos = P;
DestAtd = A;
if (!Length) return;

// Set position change vector:
if (Pos != DestPos)
    {
    XDelta = DestPos.X - Pos.X;
    YDelta = DestPos.Y - Pos.Y;
    ZDelta = DestPos.Z - Pos.Z;

    if (Length) PosVect.X = XDelta / Length; 
    else PosVect.X = 0;

    if (Length) PosVect.Y = YDelta / Length;
    else PosVect.Y = 0;

    if (Length) PosVect.Z = ZDelta / Length;
    else PosVect.Z = 0;
     }
else
    PosVect.X = PosVect.Y = PosVect.Z = 0;

// Set attitude change vector:
if (Atd != DestAtd)
    {
    XDelta = DestAtd.X - Atd.X;
    YDelta = DestAtd.Y - Atd.Y;
    ZDelta = DestAtd.Z - Atd.Z;

    if (XDelta)
        {
        if (XDelta > IMR_HALFDEGREECOUNT || XDelta < -IMR_HALFDEGREECOUNT) 
            XDelta = -((IMR_DEGREECOUNT - DestAtd.X) + Atd.X);
        if (XDelta < -IMR_HALFDEGREECOUNT) 
            XDelta = (IMR_DEGREECOUNT - Atd.X) + DestAtd.X;
        AtdVect.X = XDelta / Length;
         }
    else AtdVect.X = 0;
            
    if (YDelta)
        {
        if (YDelta > IMR_HALFDEGREECOUNT || YDelta < -IMR_HALFDEGREECOUNT)
            YDelta = -((IMR_DEGREECOUNT - DestAtd.Y) + Atd.Y);
        if (YDelta < -IMR_HALFDEGREECOUNT)
            YDelta = (IMR_DEGREECOUNT - Atd.Y) + DestAtd.Y;
        AtdVect.Y = YDelta / Length;
         }
    else AtdVect.Y = 0;
                 
    if (ZDelta)
        {
        if (ZDelta > IMR_HALFDEGREECOUNT || ZDelta < -IMR_HALFDEGREECOUNT)
            ZDelta = -((IMR_DEGREECOUNT - DestAtd.Z) + Atd.Z);
        if (ZDelta < -IMR_HALFDEGREECOUNT)
            ZDelta = (IMR_DEGREECOUNT - Atd.Z) + DestAtd.Z;
        AtdVect.Z = ZDelta / Length;
         }
    else AtdVect.Z = 0;
     }
else
    AtdVect.X = AtdVect.Y = AtdVect.Z;
 
// Reset counters:
Motion_Status = IMR_MOTION_ACTIVE;
Motion_Frame = Length;
 }

/***************************************************************************\
  Changes to the next frame in a motion.
  Returns the status of the motion.
\***************************************************************************/
int IMR_Camera::Motion_Step(void)
{
// Make sure a motion is in progress:
if (Motion_Status == IMR_MOTION_DONE) return IMR_MOTION_DONE;

// Increment attitude and position:
Pos += PosVect;
Atd += AtdVect;

// Decrement counter:
-- Motion_Frame; 

// Tell the user our status:
if (!Motion_Frame)
    {
    // Set status:
    Motion_Status = IMR_MOTION_DONE;
    
    // Set positions and attitudes exactly:
    Pos = DestPos;
    Atd = DestAtd;
    PosVect.X = PosVect.Y = PosVect.Z = 0;
    AtdVect.X = AtdVect.Y = AtdVect.Z = 0;

    // Return status:
    return IMR_MOTION_DONE;
     }
else 
    {
    Motion_Status = IMR_MOTION_ACTIVE;
    return IMR_MOTION_ACTIVE;
     }
 }
