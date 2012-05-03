/****************************************************************\
 
 iMMERSE Engine
 (C) 1999 No Tears Shed Software
 All rights reserved
 
 Filename: IMR_Resource.hpp
 Description: Header

\****************************************************************/
#ifndef IMR_RESOURCE_HPP_INCLUDED
#define IMR_RESOURCE_HPP_INCLUDED

#include <sys\types.h>
#include <sys\stat.h>
#include <fcntl.h>
#include <io.h>
#include <malloc.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include "..\CallStatus\IMR_Log.hpp"
#include "..\Core\IMR_RDFMngr.hpp"

// Resource manager class:
class IMR_ResourceManager
    {
    protected:
      // RDF manager:
      IMR_RDFManager I_RDFManager;
      
    public:
      int LoadResources(char *RIFName);
      inline IMR_RDFManager *Get_RDFManager() { return &I_RDFManager; };

     };

// Globals:
extern IMR_ResourceManager IMR_Resources;

#endif
