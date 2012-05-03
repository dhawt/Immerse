/***************************************************************************\
  File: IMR_Err.cpp
  Description: Contains functions dealing with error messages.
  Author: Daniel Hawthorn
  Modified:
  
\***************************************************************************/
#include "IMR_Err.hpp"

// Globals:
struct
    {
    unsigned int ErrorQueued:1;
    char *ErrorMsg;
     } IMR_ErrorInfo;

/***************************************************************************\
  Sets the error message.  If an error has already occured and has not been 
  retrieved, it will be lost.
\***************************************************************************/
void IMR_SetErrorText(const char *Msg, ...)
{
va_list arglist;

// Make syre we have a message:
if (!Msg) return;

// If we already have a message queued, delete it:
if (IMR_ErrorInfo.ErrorQueued) delete IMR_ErrorInfo.ErrorMsg;

// Allocate space for the new message and copy the message into it:
if (!(IMR_ErrorInfo.ErrorMsg = new char[strlen(Msg)])) return;
va_start(arglist, Msg);
vsprintf(IMR_ErrorInfo.ErrorMsg, Msg, arglist);
va_end(arglist);

// Flag it:
IMR_ErrorInfo.ErrorQueued = 1;
 }

/***************************************************************************\
  Sets the error message.  If an error has already occured and has not been 
  retrieved, it will be lost.
\***************************************************************************/
char *IMR_GetErrorText(void)
{
if (IMR_ErrorInfo.ErrorQueued)
    {
    IMR_ErrorInfo.ErrorQueued = 0;
    return IMR_ErrorInfo.ErrorMsg;
     }
return NULL;
 }
