/***************************************************************************\
  File: IMR_Global.hpp
  Description: Header
  Author: Daniel Hawthorn
  Modified:
  
\***************************************************************************/
#ifndef __IMR_GLOBAL__HPP
#define __IMR_GLOBAL__HPP

#include "Windows.h"
#include "IMR_FixedPnt.hpp"

// Constants and macros:
#undef IMR_ON
#define IMR_ON 1
#undef IMR_OFF
#define IMR_OFF 0

// Settings interface (more a structure, actually):
typedef struct
    {
    // Screen info:
    struct
        {
        int Width,                           // Screen resolution
            Height;
        HWND hWnd;
         } Screen;
    
    // Target buffer info:
    struct
        {
        int Width,                           // Target surface dimensions
            Height;
         } Target;
    
    // Lens info:
    struct
        {
        FIXEDPNT Zoom, 
                 Near, 
                 Far;
         } Lens;
        
    // Effects info:
    struct
        {
        unsigned int LightingEnabled:1;      // Flags if lighting should be calculated
        unsigned int ShadowsEnabled:1;       // Flags if shadows should be calculated
         } Effects;
   
    // Renderer info:
    struct
        {
        unsigned int MipMappingEnabled:1;    // Flags if mipmapping should be performed
        unsigned int FilteringEnabled:1;     // Flags if mag- and minification is on
        unsigned int FilledPolysEnabled:1;   // Flags if polygons should be rendered filled
        unsigned int TexturesEnabled:1;      // Flags if polygons should be texturemapped
         } Renderer;
   
    IMR_SettingsInterface() 
        {
        Screen.Width = Screen.Height = 0;
        Screen.hWnd = NULL;
        Target.Width = Target.Height = 0;
        Lens.Zoom = Lens.Near = Lens.Far = 0;
        Effects.LightingEnabled = IMR_ON;
        Effects.ShadowsEnabled = IMR_OFF;
        Renderer.MipMappingEnabled = IMR_ON;
        Renderer.FilteringEnabled = IMR_OFF;
        Renderer.FilledPolysEnabled = IMR_ON;
        Renderer.TexturesEnabled = IMR_ON;
         }
     } IMR_Settings;

// Externals:
extern IMR_Settings *CurrentSettings;  // Global settings used by all modules - updated 
                                       // depending on which main interface is being 
                                       // used at the moment.

#endif
