/***************************************************************************\
  File: IMR_Camera.hpp
  Description: Header
  Author: Daniel Hawthorn
  Modified:
  
\***************************************************************************/
#ifndef __IMR_CAMERA__HPP
#define __IMR_CAMERA__HPP

// Include headers:
#include <string.h>
#include <malloc.h>
#include <math.h>
#include <stdlib.h>
#include "IMR_Geometry.hpp"
#include "IMR_Err.hpp"
#include "IMR_RetVals.hpp"
#include "IMR_Table.hpp"

// Constants and macros:
#ifndef IMR_MOTION_DONE
    #define IMR_MOTION_DONE 0
#endif
#ifndef IMR_MOTION_ACTIVE
    #define IMR_MOTION_ACTIVE 1
#endif

// Camera interface class:
class IMR_Camera
    {
    protected:
      // Miscellaneous
      char Name[9];
      
      // Position and orientation data:
      IMR_3DPoint Pos;
      IMR_Attitude Atd;
      
      // Motion control:
      int Motion_Length, Motion_Frame, Motion_Status;
      IMR_3DPoint DestPos, PosVect;
      IMR_Attitude DestAtd, AtdVect;
      
    public:
      IMR_Camera() { Name[0] = '\0'; Motion_Status = IMR_MOTION_DONE; };
      ~IMR_Camera() { };
      
      // Name methods:
      void Set_Name(char *NewName);
      inline char *Get_Name(void) { return Name; };
      inline int Is(char *CmpName) { return !stricmp(Name, CmpName); };
      
      // Position methods:
      inline void Set_Pos(IMR_3DPoint &Tmp) { Pos = Tmp; };
      inline IMR_3DPoint &Get_Pos(void) { return Pos; };
      inline void Set_Atd(IMR_Attitude &Tmp) { Atd = Tmp; };
      inline IMR_Attitude &Get_Atd(void) { return Atd; };

      // Motion methods:
      void Motion_Init(IMR_3DPoint &P, IMR_Attitude &A, int Length);
      int Motion_Step(void);
      inline int Motion_Get_Status(void) const { return Motion_Status; };
      inline IMR_3DPoint &Get_PosVect(void) { return PosVect; };
     };

#endif
