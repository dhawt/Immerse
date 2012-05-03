/****************************************************************\
 
  iMMERSE Engine
  (C) 1999 No Tears Shed Software
  All rights reserved
     
  Filename: IMR_Log.cpp
  Description: Logfile class code module 
 
\****************************************************************/
#include "IMR_Log.hpp"

// Globals:
bool IMR_LogEnabled;
char *IMR_LogFileName;
int IMR_LogFile;
struct
    {
    bool LogQueued;
    int LogLine;
    char *LogModule, *LogMsg;
     } IMR_LastLogMsg;

/****************************************************************\
  Initializes logging.
  Notes: Pass NULL file to disable logging.
\****************************************************************/
int IMR_StartLogging(char *LogFile)
{
if (LogFile) 
    {
    // Setup flags:
    IMR_LogEnabled = TRUE;
    IMR_LogFileName = LogFile;
    
    // Attempt to open the file:
    IMR_LogFile = open(LogFile, O_RDWR | O_TEXT | O_TRUNC | O_CREAT, S_IRUSR | S_IWUSR);

    // If we can't, disable logging:
    if (IMR_LogFile == -1)
        {
        IMR_LogEnabled = FALSE;
        IMR_LogFileName = NULL;
        return IMRERR_BADFILE;
         }
    
    // Write a message to the log file:
    IMR_LogLine("--- Begin Logfile %s ---\n\n", LogFile);
     }
else
    IMR_LogEnabled = FALSE;

return IMR_OK;
 }

/****************************************************************\
  Stops all logging.
\****************************************************************/
int IMR_EndLogging(void)
{
// Check if logging is enabled:
if (IMR_LogEnabled) 
    {
    // Write a final message:
    IMR_LogLine("\n\n--- End Logfile %s ---", IMR_LogFileName);

    // Close the log file:
    close(IMR_LogFile); 
     }

// Setup flags:
IMR_LogEnabled = FALSE;
IMR_LogFileName = NULL;

// And return ok:
return IMR_OK;
 }

/****************************************************************\
  Writes a line to the log file.
\****************************************************************/
void IMR_LogLine(char *Msg, ...)
{
char Buffer[151];
va_list arglist;

// Make sure logging is enabled:
if (IMR_LogEnabled)
    {
    if (strlen(Msg) > 150) Msg[150] = 0;
    va_start(arglist, Msg);
    vsprintf(Buffer, Msg, arglist);
    va_end(arglist);
    write(IMR_LogFile, Buffer, strlen(Buffer));
     }
 }

/****************************************************************\
  Writes a log to the file.
\****************************************************************/
void IMR_LogMsg(int Line, char *Module, char *Msg, ...)
{
// Make sure we have a message and logging is enabled:
if (!Msg || !IMR_LogEnabled) return;

va_list arglist;

// If we already have a message queued, delete it:
if (IMR_LastLogMsg.LogMsg) { delete IMR_LastLogMsg.LogMsg; IMR_LastLogMsg.LogMsg = NULL; };
if (IMR_LastLogMsg.LogModule) { delete IMR_LastLogMsg.LogModule; IMR_LastLogMsg.LogModule = NULL; };

// Allocate space for the new file and copy it into it:
if (!(IMR_LastLogMsg.LogModule = new char[strlen(Module) + 1])) return;
strcpy(IMR_LastLogMsg.LogModule, Module);

// Allocate space for the new message and copy the message into it:
if (!(IMR_LastLogMsg.LogMsg = new char[strlen(Msg) + 80])) return;
va_start(arglist, Msg);
vsprintf(IMR_LastLogMsg.LogMsg, Msg, arglist);
va_end(arglist);

// Flag it:
IMR_LastLogMsg.LogQueued = 1;

// Now write the entry to the file:
IMR_LogLine("%s (%d): %s\n", Module, Line, IMR_LastLogMsg.LogMsg);
 }

/****************************************************************\
  Returns the most recent log entry.
  Buffer must have enough memory allocated to store stuff!
\****************************************************************/
void IMR_GetLastLog(int &Line, char *ModuleBuffer, char *MsgBuffer)
{
Line = IMR_LastLogMsg.LogLine;
if (ModuleBuffer) strcpy(ModuleBuffer, IMR_LastLogMsg.LogModule);
if (MsgBuffer) strcpy(MsgBuffer, IMR_LastLogMsg.LogMsg);
 }

