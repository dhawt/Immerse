/****************************************************************\
 
 iMMERSE Engine
 (C) 1999 No Tears Shed Software
 All rights reserved
 
 Filename: IMR_GM_Interface.hpp
 Description: Header
 
\****************************************************************/
#ifndef __IMR_GM_INTERFACE__HPP
#define __IMR_GM_INTERFACE__HPP

// Include all the headers:
#include "IMR_GM_Figure.hpp"
#include "IMR_GM_CameraOp.hpp"
#include "..\Core\IMR_Interface.hpp"
#include "..\Core\IMR_Geometry.hpp"
#include "..\Core\IMR_Camera.hpp"
#include "..\Core\IMR_Geom_Light.hpp"
#include "..\Core\IMR_Resource.hpp"
#include "..\Foundation\IMR_List.hpp"

// Constants and macros:
#define IMR_MAX_GLBTEX        64
#define IMR_MAX_LOCTEX        64
#define IMR_MAX_GLBMOD        64
#define IMR_MAX_LOCMOD        64
#define IMR_MAX_GLBOBJ        64
#define IMR_MAX_LOCOBJ        64
#define IMR_MAX_GLBFIG        64
#define IMR_MAX_LOCFIG        64
#define IMR_MAX_LIGHTS        32
#define IMR_MAX_CAMERAS       8
#define IMR_LIST_LOCAL        1
#define IMR_LIST_GLOBAL       2

// Geometry manager interface class (derived from Immerse interface):
class IMR_GM_Interface : public IMR_Interface
    {
    protected:
      // Lists:
      IMR_IDList<IMR_Light>       Lights;
      IMR_NamedList<IMR_Camera>   Cameras;
      IMR_NamedList<IMR_Model>    GlobalModels, LocalModels;
      IMR_NamedList<IMR_Object>   GlobalObjects, LocalObjects;
      IMR_NamedList<IMR_Figure>   GlobalFigures, LocalFigures;
      
      // The parent geometry:
      IMR_Object                  World;
      
      // Geometry scale, expressed in terms of units per meter:
      int WorldScale;
      
      // Flags:
      struct
          {
          unsigned int ClassInitialized:1;
           } Flags;
      
    public:
      IMR_GM_Interface()
          {
          Flags.ClassInitialized = 0;
          Lights.Init(0);
          GlobalModels.Init(0);
          LocalModels.Init(0);
          GlobalObjects.Init(0);
          LocalObjects.Init(0);
          GlobalFigures.Init(0);
          LocalFigures.Init(0);
          Lights.Init(0);
          Cameras.Init(0);
          WorldScale = 100;     // Default to 100 units per meter
           };
      ~IMR_GM_Interface() { Shutdown(); };
      
      // Init and de-init methods:
      int Init(IMR_InterfaceInitData &InitData);
      int Shutdown(void);
      
      // Setup methods:
      void Set_WorldScale(int Val) { if (Val > 0) WorldScale = Val; };

      // Camera methods (and sub-classes):
      IMR_Camera *Add_Camera(char *Name) { return Cameras.Add_Item(Name); };
      IMR_Camera *Get_Camera(char *Name) { return Cameras.Get_Item(Name, NULL); };
      void Delete_Camera(char *Name) { Cameras.Delete_Item(Name); };
      IMR_CameraOp I_CameraOp;
      void Setup_CameraOp(IMR_CameraOp Op) { I_CameraOp = Op; };
      
      // Prep methods:
      int Prep_Geometries(void);

      // Geometry methods:
      int Attach_Object(char *Child, char *Parent);
      IMR_Object *Lop_Object(char *Name);
      inline IMR_Object *Seek_Object(char *Name) { if (!Name) return &World; else return World.Get_Child(Name); };
      inline IMR_Object *Detach_Object(char *Name) { return Lop_Object(Name); };
      
      // List access methods:
      int Add_Light(int ID);
      int Add_Model(int List, char *Name);
      int Add_Object(int List, char *Name);
      int Add_Figure(int List, char *Name);
      IMR_Light *Get_Light(int ID);
      IMR_Model *Get_Model(char *Name);
      IMR_Object *Get_Object(char *Name);
      IMR_Figure *Get_Figure(char *Name);
      void Clear_Lights(void) { Lights.Reset(); Lights.Init(IMR_MAX_LIGHTS); };
      void Clear_Global_Models(void) { GlobalModels.Reset(); GlobalModels.Init(IMR_MAX_GLBMOD); };
      void Clear_Local_Models(void) { LocalModels.Reset(); LocalModels.Init(IMR_MAX_LOCMOD); };
      void Clear_Global_Objects(void) { GlobalObjects.Reset(); GlobalObjects.Init(IMR_MAX_GLBOBJ); };
      void Clear_Local_Objects(void) { LocalObjects.Reset(); LocalObjects.Init(IMR_MAX_LOCOBJ); };
      void Clear_Global_Figures(void) { GlobalFigures.Reset(); GlobalFigures.Init(IMR_MAX_LOCFIG); };
      void Clear_Local_Figures(void) { LocalFigures.Reset(); LocalFigures.Init(IMR_MAX_LOCFIG); };
      void Clear_Global_Lists()
          {
          Clear_Global_Models(); 
          Clear_Global_Objects(); 
          Clear_Global_Figures();
           };
      void Clear_Local_Lists()
          {
          Clear_Local_Models();
          Clear_Local_Objects();
          Clear_Local_Figures();
           }
      void Clear_Lists(void)
          {
          Clear_Global_Lists();
          Clear_Local_Lists();
           };

      // New frame methods:
      int Begin_Frame(char *CamName);
      int Begin_Frame(IMR_Camera *ExternalCamera);
      int Add_Geometries(void);
     };

#endif

