/****************************************************************\
 
 iMMERSE Engine
 (C) 1999 No Tears Shed Software
 All rights reserved
 
 Filename: IMR_GM_Figure.hpp
 Description: Header for Geometry Manager figures
 
\****************************************************************/
#ifndef __IMR_GM_FIGURE__HPP
#define __IMR_GM_FIGURE__HPP

// Include all the headers:
#include "..\Foundation\IMR_List.hpp"
#include "..\Foundation\IMR_Time.hpp"
#include "..\Core\IMR_Geometry.hpp"
#include "..\CallStatus\IMR_Log.hpp"

// Constants and macros:
#define IMR_SKELETON_MAXKEYS 32

// Skeleton segment:
class IMR_SkelSegment
    {
    protected:
      // Name stuff:
      char Name[9];                                 // Name of segment, i.e. "LeftFoot"
      
      // Object stuff:
      char ObjName[9];                              // Name of object, i.e. "LFootObj"
      IMR_Object *Obj;
      IMR_3DPoint Pos;                              // Position and attitude of object
      IMR_Attitude Atd;
      
      // Pointer to next segment in list:
      IMR_SkelSegment *Next;
      
    public:
      IMR_SkelSegment() { Name[0] = 0; Next = NULL; };
      ~IMR_SkelSegment() { };
      
      // Name methods:
      void Set_Name(char *NewName);
      inline char *Get_Name(void) { return Name; };
      inline int Is(char *CmpName) { return !stricmp(Name, CmpName); };
      
      // List methods:
      void Set_Next(IMR_SkelSegment *n) { Next = n; };
      IMR_SkelSegment *Get_Next(void) const { return Next; };
      
      // Object methods:
      void Set_ObjectName(char *Name);
      char *Get_ObjectName(void) { return ObjName; };
      void Set_Object(IMR_Object *o) { Obj = o; };
      IMR_Object *Get_Object(void) { return Obj; };
      
      // Position and attitude methods:
      IMR_3DPoint &Get_Pos(void) { return Pos; };
      void Set_Pos(IMR_3DPoint &P) { Pos = P; };
      IMR_Attitude &Get_Atd(void) { return Atd; };
      void Set_Atd(IMR_Attitude &A) { Atd = A; };
     };

// Skeleton:
class IMR_Skeleton
    {
    protected:
      // Name stuff:
      char Name[9];                    // Skeleton name - i.e. "Chair"
      
      // Segment stuff:
      int Num_Segs;
      IMR_SkelSegment *Segs;            // Pointer to first segment in linked list
            
      // Pointer to next skeleton in list:
      IMR_Skeleton *Next;
      
    public:
      IMR_Skeleton() { Next = NULL; Segs = NULL; Name[0] = 0; };
      ~IMR_Skeleton() { Wipe_Segments(); };
      
      // Name methods:
      void Set_Name(char *NewName);
      inline char *Get_Name(void) { return Name; };
      inline int Is(char *CmpName) { return !stricmp(Name, CmpName); };

      // Setup methods:
      int Setup(IMR_NamedList<IMR_Object> *GlbObj, IMR_NamedList<IMR_Object> *LocObj);
      
      // Segment methods:
      IMR_SkelSegment *GetFirstSegment(void) { return Segs; };
      int Add_Segment(char *Name);
      IMR_SkelSegment *Get_Segment(char *Name);
      void Delete_Segment(char *Name);
      void Wipe_Segments(void);
      int Get_Num_Segs(void) const { return Num_Segs; };

      // List methods:
      void Set_Next(IMR_Skeleton *n) { Next = n; };
      IMR_Skeleton *Get_Next(void) const { return Next; };
      
      // Object methods:
      inline IMR_Object *Get_RootObj(void) { if (Segs) return Segs->Get_Object(); else return NULL; };
     };

// Figure:
class IMR_Figure
    {
    protected:
      // Name stuff:
      char Name[9];
      
      // Skeletons:
      int Num_Keys;
      IMR_Skeleton *Keys;                   // Key skeletons - aka keyframes or keys
      char RootObjName[9];                  // Name of root object
      IMR_Object *RootObj;                  // Root object (all objects in figure
                                            // should be children)
    
      // Animation stuff:
      int Animation_Time, Animation_Status, Animation_Length;
      IMR_Skeleton *Animation_Key;
    
    public:
      IMR_Figure()
          {
          Name[0] = '/0';
          RootObjName[0] = '/0';
          Num_Keys = 0; Keys = NULL;
          Animation_Time = Animation_Length = 0; Animation_Status = IMR_ANIMATION_DONE;
           };
      ~IMR_Figure() { Wipe_Keys(); };

      // Name methods:
      void Set_Name(char *NewName);
      inline char *Get_Name(void) { return Name; };
      inline int Is(char *CmpName) { return !stricmp(Name, CmpName); };
      
      // Setup methods:
      int Setup(IMR_NamedList<IMR_Object> *GlbObj, IMR_NamedList<IMR_Object> *LocObj);
      
      // Key skeleton methods:
      int Add_Key(char *Name);
      IMR_Skeleton *Get_Key(char *Name);
      void Delete_Key(char *Name);
      void Wipe_Keys(void);
      int Get_Num_Keys(void) const { return Num_Keys; };
      
      // Animation methods:
      void Animation_Jump(char *KeyName);
      void Animation_Init(char *KeyName, int Length);
      int Animation_Step(void);
      int Animation_Get_Status(void) { return Animation_Status; };
      
      // File IO methods:
      int Write_RDF(char *Filename);

      // Object methods: (returns object attached to first segment of first skeleton,
      // so be careful this is root!)
      void Set_RootObj(char *NewName)
          {
          if (strlen(NewName) > 8)
              { memcpy((void *)RootObjName, (void *)NewName, 8); RootObjName[8] = '/0'; } 
          else
              strcpy(RootObjName, NewName); 
           };
      void Set_RootObj(IMR_Object *Obj) { RootObj = Obj; IMR_LogMsg(__LINE__, __FILE__, "Rootobj for %s set to %s", Name, Obj->Get_Name()); };
      inline IMR_Object *Get_RootObj(void) { return RootObj; }; //if (Keys) return Keys->Get_RootObj(); else return NULL; };
     };
      
#endif
