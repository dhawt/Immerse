/****************************************************************\
 
 iMMERSE Engine
 (C) 1999 No Tears Shed Software
 All rights reserved
 
 Filename: IMR_DirectX__Err.hpp
 Description: Header
 
\****************************************************************/
#ifndef __IMR_DIRECTX_ERR__HPP
#define __IMR_DIRECTX_ERR__HPP

// Include stuff:
#include <string.h>
#define INITGIUD
#include <ddraw.h>
#include <d3d.h>

// Prototypes:
char *IMR_MsgFromDXErr(int err);

#endif
