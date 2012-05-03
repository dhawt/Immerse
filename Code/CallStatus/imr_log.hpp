/****************************************************************\
 
 iMMERSE Engine
 (C) 1999 No Tears Shed Software
 All rights reserved
 
 Filename: IMR_Log.hpp
 Description: Header
 
\****************************************************************/
#ifndef IMR_LOG_HPP_INCLUDED
#define IMR_LOG_HPP_INCLUDED

#include <sys\types.h>
#include <sys\stat.h>
#include <fcntl.h>
#include <io.h>
#include <malloc.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include "..\CallStatus\IMR_RetVals.hpp"

// Prototypes:
int IMR_StartLogging(char *LogFile);
int IMR_EndLogging(void);
void IMR_LogLine(char *Msg, ...);
void IMR_LogMsg(int Line, char *Module, char *Msg, ...);
void IMR_GetLastLog(int &Line, char *ModuleBuffer, char *MsgBuffer);

#endif
