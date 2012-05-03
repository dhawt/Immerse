/***************************************************************************\
  File: IMR_DirectX_Err.hpp
  Description: Header
  Author: Daniel Hawthorn
  Modified:
  
\***************************************************************************/
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
