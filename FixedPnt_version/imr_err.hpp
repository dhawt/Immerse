/***************************************************************************\
  File: IMR_Err.hpp
  Description: Header
  Author: Daniel Hawthorn
  Modified:
  
\***************************************************************************/
#ifndef __IMR_ERR__HPP
#define __IMR_ERR__HPP

// Include stuff:
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include "IMR_RetVals.hpp"

// Prototypes:
void IMR_SetErrorText(const char *Msg, ...);
char *IMR_GetErrorText(void);

#endif
