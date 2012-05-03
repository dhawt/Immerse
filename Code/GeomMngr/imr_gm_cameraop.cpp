/****************************************************************\
 
 iMMERSE Engine
 (C) 1999 No Tears Shed Software
 All rights reserved
 
 Filename: IMR_GM_CameraOp.hpp
 Description: CameraOperator code module.
 
\****************************************************************/
#include "IMR_GM_CameraOp.hpp"

/****************************************************************\
  Resets the cameraop.
\****************************************************************/
void IMR_CameraOp::Reset(void)
{
// Reset all pointers:
Camera = NULL;
TargetObj = NULL;

// Reset animation:
Pnt1.X = Pnt1.Y = Pnt1.Z = Pnt2.X = Pnt2.Y = Pnt2.Z = 0.0f;
Atd1.X = Atd1.Y = Atd1.Z = Atd2.X = Atd2.Y = Atd2.Z = 0;

// Reset flags:
VcnFlags.Distance = VcnFlags.FollowTime = VcnFlags.Constraints = VcnFlags.Mode = 0; 
 }

/****************************************************************\
  Sets the specified viewcontrol parameter to the specified 
  value.
\****************************************************************/
int IMR_CameraOp::Vcn_SetParam(int Param, int Value)
{
if (Param == IMR_CAMERAOP_VCN_MODE) { VcnFlags.Mode = Value; return IMR_OK; }
if (Param == IMR_CAMERAOP_VCN_DISTANCE) { VcnFlags.Distance = Value; return IMR_OK; }
if (Param == IMR_CAMERAOP_VCN_FOLLOWTIME) { VcnFlags.FollowTime = Value; return IMR_OK; }
if (Param == IMR_CAMERAOP_VCN_CONSTRAINT) { VcnFlags.Constraints |= Value; return IMR_OK; }

// Return an error message if we didn't get a real parameter value:
IMR_LogMsg(__LINE__, __FILE__, "Unknown viewcontrol parameter specified! (%d)", Param);
return IMRERR_GENERIC;
 }

/****************************************************************\
  Updates the camera position based on viewcontrol parameters
  and geometry info.
  Notes: Geom is geometry info for view obstruction checking.  
         Pass NULL object and it'll just be ignored.
\****************************************************************/
int IMR_CameraOp::Vcn_UpdateCam(IMR_Object *Geom)
{
// Check what mode our camera is in and position it accordingly:
if (VcnFlags.Mode == IMR_CAMERAOP_VCN_MODE_FIXED)
    {
    // Don't need to do anything, since the camera should already be attached
    // to the object.
    return IMR_OK;
     }
if (VcnFlags.Mode == IMR_CAMERAOP_VCN_MODE_CHASE)
    {
    if (!Camera || !Camera->Obj_GetAttached() || !TargetObj) return IMR_OK;
    IMR_Matrix Mat;

    // Check for obstructions here...

    Atd2 = Atd1 + TargetObj->Get_GlobalAtd();
    Mat.Rotate(Atd2.X, Atd2.Y, Atd2.Z);
    Pnt2 = Pnt1;
    Pnt2.Transform(Mat);
    Pnt2 += TargetObj->Get_GlobalPos();
    Camera->Obj_GetAttached()->Animation_Init(Pnt2, Atd2, VcnFlags.FollowTime);
    Camera->Obj_GetAttached()->Animation_Step();
    //Camera->Obj_GetAttached()->Set_RelativePos(Pnt2);
    //Camera->Obj_GetAttached()->Set_RelativeAtd(Atd2);

    return IMR_OK;
     }

return IMR_OK;
 }
