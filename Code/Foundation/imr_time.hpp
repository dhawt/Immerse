/***************************************************************************\

 iMMERSE Engine
 (C) 1999 No Tears Shed Software
 All rights reserved
 
 Filename: IMR_Time.hpp
 Description: Header
  
\***************************************************************************/
#ifndef __IMR_TIME__HPP
#define __IMR_TIME__HPP

// Include stuff:
#include <time.h>
#include <windows.h>

// Globals:
extern float IMR_Time_ClockToMs;
extern int IMR_Time_FrameStartTime, IMR_Time_FrameTime;

// Timebase methods:
void IMR_Time_Reset(void);
int IMR_Time_StartTimer(void);
int IMR_Time_EndTimer(void);
inline int IMR_Time_GetClock(void) { return clock() * IMR_Time_ClockToMs; };
inline int IMR_Time_GetFrameTime(void) { return IMR_Time_FrameTime; };
inline float IMR_Time_GetNormalizedFrameTime(void) { return .001f * (float)IMR_Time_FrameTime; };

#endif
