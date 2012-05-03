/****************************************************************\

 iMMERSE Engine
 (C) 1999 No Tears Shed Software
 All rights reserved
 
 Filename: IMR_RDFMngr.hpp
 Description: Header
 
\****************************************************************/
#ifndef IMR_RDFMNGR_HPP_INCLUDED
#define IMR_RDFMNGR_HPP_INCLUDED

#include <sys\types.h>
#include <sys\stat.h>
#include <fcntl.h>
#include <io.h>
#include <malloc.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include "..\CallStatus\IMR_Log.hpp"

// RDF resource type:
typedef int IMR_RDFResourceDesc;

// Constants:
const IMR_MaxRDFListEntries = 1024;

// RDF list entry class:
class IMR_RDFListEnt
    {
    friend class IMR_RDFManager;
    protected:
      char Type[4],
           File[81],
           Resource[33];
    public:
      IMR_RDFListEnt() { Type[3] = File[80] = Resource[32] = 0; Type[0] = File[0] = Resource[0] = 0;};
      
      inline void Set_Type(char *t) { if (t) memcpy(Type, t, 3); };
      inline void Set_File(char *f) { if (f) strcpy(File, f); };
      inline void Set_Resource(char *r) { if (r) memcpy(Resource, r, 32); };
      inline char *Get_File(void) { return File; };
      inline int IsType(char *t) { return !stricmp(t, Type); };
      inline int IsFile(char *f);
      inline int IsResource(char *r) { return !stricmp(r, Resource); };
     };

// RDF Manager class:
class IMR_RDFManager
    {
    protected:
      // Directory info:
      int NumDirEnts;
      IMR_RDFListEnt *Directory[IMR_MaxRDFListEntries];
      
      // Protected members:
      int AddDirEnt(void);
      
    public:
      IMR_RDFManager() { NumDirEnts = 0; };
      ~IMR_RDFManager() { Finalize(); };
      
      // Shutdown method:
      void Finalize(void);
      
      // RDF file methods:
      int AddRDF(char *FileName);
      IMR_RDFResourceDesc FindRDF(char *rdfTyp, char *rdfNam, char *rdfRes);
      char *GetRDFLocation(char *rdfTyp, char *rdfNam, char *rdfRes);
      int OpenRDF(IMR_RDFResourceDesc RD, int &fd);
      void CloseRDF(int fd) { close(fd); };
     };

#endif
