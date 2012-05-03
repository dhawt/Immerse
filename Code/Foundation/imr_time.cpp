/****************************************************************\
 
 iMMERSE Engine
 (C) 1999 No Tears Shed Software
 All rights reserved
 
 Filename: IMR_Time.cpp
 Description: Timebase for all animation control.
 
\****************************************************************/
#include "IMR_Time.hpp"

// File globals:
float IMR_Time_ClockToMs = 1000 / CLOCKS_PER_SEC;
int IMR_Time_FrameStartTime, IMR_Time_FrameTime;

/****************************************************************\
  Resets frame time counter; 
\****************************************************************/
void IMR_Time_Reset(void)
{
IMR_Time_FrameStartTime = -1;
 }

/****************************************************************\
  Resets the frametime counter.
\****************************************************************/
int IMR_Time_StartTimer(void)
{
IMR_Time_FrameStartTime = IMR_Time_GetClock();
return IMR_Time_FrameStartTime;
 }

/****************************************************************\
  Returns the number of milliseconds elapsed since
  StartTimer() was called.  If StartTimer() has not yet been 
  called, returns 0.
  Note: Doesn't bother to take into account counter rollover.
\****************************************************************/
int IMR_Time_EndTimer(void)
{
if (IMR_Time_FrameStartTime == -1) 
    {
    IMR_Time_FrameTime = 0;
    return 0;
     }
IMR_Time_FrameTime = IMR_Time_GetClock() - IMR_Time_FrameStartTime;
return IMR_Time_FrameTime;
 }
