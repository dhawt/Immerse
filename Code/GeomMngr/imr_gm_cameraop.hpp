/****************************************************************\
 
 iMMERSE Engine
 (C) 1999 No Tears Shed Software
 All rights reserved
 
 Filename: IMR_GM_CameraOp.hpp
 Description: Header for CameraOperator class
 
\****************************************************************/
#ifndef __IMR_GM_CAMERAOP__HPP
#define __IMR_GM_CAMERAOP__HPP

// Include all the headers:
#include "..\Foundation\IMR_List.hpp"
#include "..\Foundation\IMR_Time.hpp"
#include "..\CallStatus\IMR_Log.hpp"
#include "..\Core\IMR_Geometry.hpp"
#include "..\Core\IMR_Camera.hpp"

#define IMR_CAMERAOP_VCN_MODE                   0x01
    #define IMR_CAMERAOP_VCN_MODE_FIXED             0x00
    #define IMR_CAMERAOP_VCN_MODE_CHASE             0x01
    #define IMR_CAMERAOP_VCN_MODE_FREE              0x02
    #define IMR_CAMERAOP_VCN_MODE_SERVL             0x03
    #define IMR_CAMERAOP_VCN_MODE_TRACK             0x04
#define IMR_CAMERAOP_VCN_DISTANCE               0x02
#define IMR_CAMERAOP_VCN_FOLLOWTIME             0x03
#define IMR_CAMERAOP_VCN_CONSTRAINT             0x04
    #define IMR_CAMERAOP_VCN_CONSTRAINT_NOPAN       0x01
    #define IMR_CAMERAOP_VCN_CONSTRAINT_RELATION    0x02
    #define IMR_CAMERAOP_VCN_CONSTRAINT_NOBLOCK     0x04

// CameraOperator class:
class IMR_CameraOp
    {
    /*
    
     CameraOp notes:
     
     *Camera is the camera for which this CameraOp operates
     *TargetObj is the object for which this camera follows or is attached to
     Pnt1,2 and Atd1,2 are the end positions for a track camera.
     When the camera is in chase mode these are positioning offsets.
   
    */

    protected:
      // Various pointers:
      IMR_Camera *Camera;
      IMR_Object *TargetObj;
      
      // Animation control:
      IMR_3DPoint  Pnt1, Pnt2;
      IMR_Attitude Atd1, Atd2;
      
      // Flags:
      struct
          {
          int Mode,
              Distance,
              FollowTime,
              Constraints;
           } VcnFlags;
      
    public:
    
      // Constructor and destructor methods:
      IMR_CameraOp() { Reset(); };
      ~IMR_CameraOp() { Reset(); };

      // Operator overloads:
      inline void operator=(IMR_CameraOp &Op)
          {
          Camera = Op.Camera;
          TargetObj = Op.TargetObj;
          Pnt1 = Op.Pnt1;
          Pnt2 = Op.Pnt2;
          Atd1 = Op.Atd1;
          Atd2 = Op.Atd2;
          VcnFlags = Op.VcnFlags;
           };
      
      // Init and deinit:
      void Reset();
      
      // Camera access methods:
      inline void Cam_Set(IMR_Camera *Cam) { Camera = Cam; };
      inline IMR_Camera *Cam_Get(void) { return Camera; };
      
      // Viewcontrol methods (ViewCoNtrol):
      inline void Vcn_SetTargetObj(IMR_Object *Obj) { TargetObj = Obj; };
      inline IMR_Object *Vcn_GetTargetObj(void) { return TargetObj; };
      inline void Vcn_SetPnt1(IMR_3DPoint &P) { Pnt1 = P; };
      inline void Vcn_SetPnt2(IMR_3DPoint &P) { Pnt2 = P; };
      inline void Vcn_SetAtd1(IMR_Attitude &A) { Atd1 = A; };
      inline void Vcn_SetAtd2(IMR_Attitude &A) { Atd2 = A; };
      inline IMR_3DPoint Vcn_GetPnt1(void) { return Pnt1; };
      inline IMR_3DPoint Vcn_GetPnt2(void) { return Pnt2; };
      inline IMR_Attitude Vcn_GetAtd1(void) { return Atd1; };
      inline IMR_Attitude Vcn_GetAtd2(void) { return Atd2; };
      inline void Vcn_ClearConstraints(void) { VcnFlags.Constraints = 0; };
      int Vcn_SetParam(int Param, int Value);
      int Vcn_UpdateCam(IMR_Object *Geom);
     };

#endif
