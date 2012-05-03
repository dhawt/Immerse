/****************************************************************\

 iMMERSE Engine
 (C) 1999 No Tears Shed Software
 All rights reserved
 
 Filename: IMR_Resource.hpp
 Description: RDFManager wrapper class.
 
\****************************************************************/
#include "IMR_Resource.hpp"

// Globals:
IMR_ResourceManager IMR_Resources;

/****************************************************************\
  Loads the specified resource index file (RIF).
\****************************************************************/
int IMR_ResourceManager::LoadResources(char *RIFName)
{
char Buffer[120], Filename[80];
FILE *fd;

if (!(fd = fopen(RIFName, "r"))) 
    { 
    IMR_LogMsg(__LINE__, __FILE__, "Failed to open file %s!", RIFName);
    return IMRERR_BADFILE;
     }

// Main loop:
while (fgets(Buffer, 120, fd))
    {
    sscanf(Buffer, " %s", Filename);
    I_RDFManager.AddRDF(Filename);    
     }

// Close our files:
fclose(fd);

// And return ok:
return IMR_OK;
 }

