/***************************************************************************\
  File: IMR_RDFIo.hpp
  Description: Header
  Author: Daniel Hawthorn
  Modified:
  
\***************************************************************************/
#ifndef __IMR_RDFIO__HPP
#define __IMR_RDFIO__HPP

// Include headers:
#include <sys\types.h>
#include <sys\stat.h>
#include <fcntl.h>
#include <io.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <math.h>
#include <stdlib.h>

// Constants and macros:
#define RDF_MAXFILES   256
#define RDF_FAILED     -1
#define RDF_EOF        0
#define RDF_OK         1
#define RDF_SEEK_SET   SEEK_SET
#define RDF_SEEK_CUR   SEEK_CUR
#define RDF_SEEK_END   SEEK_END

// RDF file class:
class RDFILE
    {
    protected:
      int fd;
      int Offset,       // Offset into file this file starts
           Size,         // Size (in bytes) of file
           End;          // Last byte belonging to this file
      char Type[5];     // Four-character code
      struct
          {
          int Num_Files;
          char *Names[RDF_MAXFILES];
          long Start[RDF_MAXFILES];
          long Size[RDF_MAXFILES];
           } Directory;

      // Access methods:
      int GetFile(char *Name);
      
    public:
      RDFILE()
          {
          Directory.Num_Files = 0;
          Offset = 0;
          Size = 0;
          End = 0;
          Type[4] = 0; 
           };
      ~RDFILE()
          {
          Close();
           };

      // Access methods:
      int Open(char *Name, RDFILE *S);
      int Close(void);
      
      // Seek and size methods:
      int Seek(int offset, int method) { lseek(fd, offset, method); return RDF_OK; };
      int GetPos(void) { return tell(fd) - Offset; };
      int GetSize(void) { return Size; };
      
      // Data methods:
      int GetFloat(float &D);
      int GetLong(long &D);
      int GetShort(int &D);
      int GetShort(short &D);
      int GetByte(int &D);
      int GetByte(char &D);
      int GetString(char *Str);
      
      // Ptr methods:
      RDFILE *GetPtr(void) { return this; };
     };

#endif
