/****************************************************************\
 
 iMMERSE Engine
 (C) 1999 No Tears Shed Software
 All rights reserved
 
 Filename: IMR_Geom_Object.hpp
 Description: Header
 
\****************************************************************/
#ifndef __IMR_GEOM_OBJECT__HPP
#define __IMR_GEOM_OBJECT__HPP

// Include stuff:
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>
#include "IMR_Geom_Light.hpp"
#include "IMR_Geom_Model.hpp"
#include "IMR_Geom_Poly.hpp"
#include "IMR_Geom_Prim.hpp"
#include "IMR_Matrix.hpp"
#include "IMR_Collide.hpp"
#include "..\CallStatus\IMR_Log.hpp"
#include "..\Foundation\IMR_List.hpp"
#include "..\Foundation\IMR_Time.hpp"

// Constants and macros:
#define IMR_OBJECT_MAXLIGHTS    64
#define IMR_OBJECT_MAXCHILDREN  32
#ifndef IMR_ANIMATION_DONE
    #define IMR_ANIMATION_DONE      0
#endif
#ifndef IMR_ANIMATION_ACTIVE
    #define IMR_ANIMATION_ACTIVE    1
#endif
#ifndef IMR_MOTION_DONE
    #define IMR_MOTION_DONE         0
#endif
#ifndef IMR_MOTION_ACTIVE
    #define IMR_MOTION_ACTIVE       1
#endif

// Object class:
class IMR_Object
    {
    protected:
      // Miscellaneous stuff:
      char Name[9];
      char ModelName[9];

      // List stuff:
      int Num_Lights, Num_Children;
      
      // Attached stuff (All local positions are relative to the object):
      IMR_Light  *AttachedLights[IMR_OBJECT_MAXLIGHTS];
      IMR_Model  *AttachedModel;
      IMR_Object *Children[IMR_OBJECT_MAXCHILDREN];
      
      // Position and orientation (relative to parent and global):
      IMR_3DPoint  RPos, GPos, StartPos, StartAtd;
      IMR_Attitude RAtd, GAtd;
      IMR_Matrix   RotMtrx;

      // Animation control stuff:
      IMR_3DPoint  PosVect, DestPos, AtdVect;
      IMR_Attitude DestAtd;
      int Animation_Time, Animation_Length, Animation_Status;
      
      // Motion control stuff:
      float Motion_TravelSpeed,
            Motion_TurnRate;
      IMR_3DPoint  Motion_TravelVect, Motion_TurnVect;
      IMR_Attitude Motion_DestAtd;
      int Motion_Time, Motion_Status;
      
//// BIG HACK ZONE

      int Collidable;

//// END BIG HACK ZONE
      
      // Pointer to parent object:
      IMR_Object *Parent;
      
      // Protected member functions:
      IMR_Light *Get_Light(int ID, int *index);
      IMR_Object *Get_Child(char *Name, int *index);
      
    public:
      
      // Constructor and destructor methods:
      IMR_Object() { Reset(); };
      ~IMR_Object() { Reset(); };
      
      // Init and de-init methods:
      void Reset(void);
      
      // Name methods:
      void Set_Name(char *NewName);
      inline char *Get_Name(void) { return Name; };
      inline int Is(char *CmpName) { return !stricmp(Name, CmpName); };
      
      // Setup methods:
      int Setup(IMR_NamedList<IMR_Model> *GlbMod, IMR_NamedList<IMR_Model> *LocMod);
      
      // Position and orientation methods:
      void UpdateCoords(void);
      void Set_RelativePos(IMR_3DPoint &Pos) { RPos = Pos; UpdateCoords(); };
      void Inc_RelativePos(IMR_3DPoint &Pos) { RPos += Pos; UpdateCoords(); };
      void Set_RelativeAtd(IMR_Attitude &Atd) { RAtd = Atd; UpdateCoords(); };
      void Inc_RelativeAtd(IMR_Attitude &Atd) { RAtd += Atd; UpdateCoords(); };
      inline IMR_3DPoint Get_RelativePos(void) { return RPos; };
      inline IMR_Attitude Get_RelativeAtd(void) { return RAtd; };
      inline IMR_3DPoint Get_GlobalPos(void) { return GPos; };
      inline IMR_Attitude Get_GlobalAtd(void) { return GAtd; };
      inline IMR_Matrix Get_RotMatrix(void) { return RotMtrx; };
      
      // Methods accessing parent:
      inline IMR_Object *Get_Parent(void) { return Parent; };
      
      // Attached light methods:
      inline int Get_Num_Lights(void) { return Num_Lights; };
      void Attach_Light(IMR_Light *Lit);
      void Detach_Light(int ID);
      inline IMR_Light *Get_Light(int index) 
          { 
          if (index >= 0 && index < Num_Lights) 
              return AttachedLights[index]; 
          return NULL; 
           };
      
      // Attached model methods:
      void Set_ModelName(char *MName);
      char *Get_ModelName(void) { return ModelName; };
      inline int Attach_Model(IMR_Model *Mdl) 
          {
          if (Mdl) 
              {
              AttachedModel = Mdl; 
              return IMR_OK;
               }
          IMR_LogMsg(__LINE__, __FILE__, "IMR_Object::Attach_Model(): NULL Model specified!");
          return IMRERR_NODATA;
           };
      inline void Detach_Model(void) { AttachedModel = NULL; };
      inline IMR_Model *Get_Model(void) { return AttachedModel; };
      int MergeToModel(IMR_Model *Mdl, IMR_3DPoint Offset);
            
      // Child object methods:
      int Attach_Child(IMR_Object *Child);
      IMR_Object *Detach_Child(char *Name);
      inline void Clear_Children(void) { for (int index = 0; index < Num_Children; index ++) Children[index] = NULL; };
      int Get_Num_Children(void) { return Num_Children; };
      inline IMR_Object *Get_Child(int index)  
          {
          if (index >= 0 && index < Num_Children) 
              return Children[index];
          return NULL;
           };
      inline IMR_Object *Get_Child(char *Name)  
          {
          IMR_Object *Temp;
          if (Is(Name)) return this;
          for (int index = 0; index < Num_Children; index ++)
              {
              Temp = Children[index]->Get_Child(Name);
              if (Temp != NULL) return Temp;
               }
          return NULL;
           };
      inline IMR_Object *Get_Local_Child(char *Name)  
          {
          for (int index = 0; index < Num_Children; index ++)
              if (Children[index] != NULL && Children[index]->Is(Name)) 
                  return Children[index];
          return NULL;
           };
     
      // Animation control methods:
      void Animation_Jump(IMR_3DPoint &Pos, IMR_Attitude &Ang);
      void Animation_Init(IMR_3DPoint &Pos, IMR_Attitude &Ang, int L);
      int Animation_Step(void);
      int Animation_Get_Status(void) const { return Animation_Status; };
      
      // Motion control methods:
      void Motion_Travel(IMR_3DPoint &Delta, float s);
      int Motion_Travel_CheckCollide(IMR_3DPoint &Delta, float s, IMR_Object *Environ, IMR_CollideInfo &CInfo);
      void Motion_Turn(IMR_Attitude &Delta, float r);
      
      // Collision detection methods:
      IMR_3DPoint CheckCollide(IMR_CollideInfo &CInfo, IMR_3DPoint position, IMR_3DPoint velocity);

//// ANOTHER BIG HACK ZONE

      void inline Set_Collidable(void) { Collidable = 1; };
      void inline Set_NonCollidable(void) { Collidable = 0; };

//// END ANOTHER BIG HACK ZONE

     };

#endif

