/****************************************************************\
 
 iMMERSE Engine
 (C) 1999 No Tears Shed Software
 All rights reserved
 
 Filename: IMR_Camera.hpp
 Description: Header
 Revisions: 3/27/00 - Added object attachment to supercede the
                      original motion control functionality.
 
\****************************************************************/
#ifndef __IMR_CAMERA__HPP
#define __IMR_CAMERA__HPP

// Include headers:
#include <string.h>
#include <malloc.h>
#include <math.h>
#include <stdlib.h>
#include "IMR_Geometry.hpp"
#include "IMR_Table.hpp"
#include "..\CallStatus\IMR_Log.hpp"
#include "..\CallStatus\IMR_RetVals.hpp"

// Camera interface class:
class IMR_Camera
    {
    protected:
      // Miscellaneous
      char Name[9];
      
      // Position and orientation data:
      IMR_3DPoint Pos;
      IMR_Attitude Atd;
      
      // Lens info:
      struct { float Zoom, Near, Far; } Lens;
      
      // Object attachment:
      IMR_Object *Obj;
      
    public:
      IMR_Camera() { Name[0] = '\0'; Obj = NULL; };
      ~IMR_Camera() { };
      
      // Name methods:
      void Set_Name(char *NewName);
      inline char *Get_Name(void) { return Name; };
      inline int Is(char *CmpName) { return !stricmp(Name, CmpName); };

      // Lens methods:
      inline void Lens_Set_Zoom(float Z) { Lens.Zoom = Z; };
      inline void Lens_Set_Near(float N) { Lens.Near = N; };
      inline void Lens_Set_Far(float F) { Lens.Far = F; };
      inline float Lens_Get_Zoom(void) { return Lens.Zoom; };
      inline float Lens_Get_Near(void) { return Lens.Near; };
      inline float Lens_Get_Far(void) { return Lens.Far; };
      
      // Position methods:
      inline void Set_Pos(IMR_3DPoint &Tmp) { if (Obj) Obj->Set_RelativePos(Tmp); };
      inline IMR_3DPoint Get_Pos(void) { IMR_3DPoint Pos; if (Obj) Pos = Obj->Get_RelativePos(); return Pos; };
      inline void Set_Atd(IMR_Attitude &Tmp) { if (Obj) Obj->Set_RelativeAtd(Tmp); };
      inline IMR_Attitude Get_Atd(void) { IMR_Attitude Atd; if (Obj) Atd = Obj->Get_RelativeAtd(); return Atd; };

      // Object attachment:
      inline void Obj_Attach(IMR_Object *O) { Obj = O; };
      inline IMR_Object *Obj_GetAttached(void) { return Obj; };
     };

#endif
