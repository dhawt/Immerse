/***************************************************************************\
  File: IMR_DirectX_Err.cpp
  Description: Code for converting DirectX error codes to string messages.
  Author: Daniel Hawthorn
  Modified:
  
\***************************************************************************/
#include "IMR_DirectX_Err.hpp"

/***************************************************************************\
  Returns an error message from the specified DirectX error code.
  If there is no message available, returns null.
\***************************************************************************/
char *IMR_MsgFromDXErr(int err)
{
switch(err)
    {
    case DD_OK: return "DD_OK";
    case DDERR_ALREADYINITIALIZED: return "DDERR_ALREADYINITIALIZED";
    case DDERR_BLTFASTCANTCLIP: return "DDERR_BLTFASTCANTCLIP";
    case DDERR_CANNOTATTACHSURFACE: return "DDERR_CANNOTATTACHSURFACE";
    case DDERR_CANNOTDETACHSURFACE : return "DDERR_CANNOTDETACHSURFACE";
    case DDERR_CANTDUPLICATE: return "DDERR_CANTDUPLICATE";
    case DDERR_CANTLOCKSURFACE: return "DDERR_CANTLOCKSURFACE";
    case DDERR_CLIPPERISUSINGHWND: return "DDERR_CLIPPERISUSINGHWND";
    case DDERR_COLORKEYNOTSET: return "DDERR_COLORKEYNOTSET";
    case DDERR_CURRENTLYNOTAVAIL: return "DDERR_CURRENTLYNOTAVAIL";
    case DDERR_DEVICEDOESNTOWNSURFACE: return "DDERR_DEVICEDOESNTOWNSURFACE";
    case DDERR_EXCEPTION: return "DDERR_EXCEPTION";
    case DDERR_GENERIC: return "DDERR_GENERIC";
    case DDERR_EXCLUSIVEMODEALREADYSET: return "DDERR_EXCLUSIVEMODEALREADYSET";
    case DDERR_HWNDALREADYSET: return "DDERR_HWNDALREADYSET";
    case DDERR_HWNDSUBCLASSED: return "DDERR_HWNDSUBCLASSED";
    case DDERR_INVALIDOBJECT: return "DDERR_INVALIDOBJECT";
    case DDERR_INVALIDPARAMS: return "DDERR_INVALIDPARAMS";
    case DDERR_OUTOFMEMORY: return "DDERR_OUTOFMEMORY";
    case DDERR_SURFACELOST: return "DDERR_SURFACELOST";
    case DDERR_SURFACEBUSY: return "DDERR_SURFACEBUSY";
    case DDERR_SURFACEALREADYATTACHED: return "DDERR_SURFACEALREADYATTACHED";
    case DDERR_INVALIDSURFACETYPE: return "DDERR_INVALIDSURFACETYPE";
    case DDERR_INVALIDPIXELFORMAT: return "DDERR_INVALIDPIXELFORMAT";
    case DDERR_INVALIDCAPS: return "DDERR_INVALIDCAPS";
    case DDERR_UNSUPPORTED: return "DDERR_UNSUPPORTED";
    case DDERR_WASSTILLDRAWING: return "DDERR_WASSTILLDRAWING";
    //case : return " ";
    default: return NULL;
     }

// And return null:
return NULL;
 }

