/****************************************************************\
 
 iMMERSE Engine
 (C) 1999 No Tears Shed Software
 All rights reserved
 
 Filename: IMR_RDFIo.hpp
 Description: RDF i/o encapsulation class
 
\****************************************************************/
#include "IMR_RDFIo.hpp"

/***************************************************************************\
  Returns an index to the specified file in the directory.
  Protected member function.
\***************************************************************************/
int RDFILE::GetFile(char *Name)
{
// Loop through all the files:
for (int index = 0; index < Directory.Num_Files; index ++)
    if (!stricmp(Directory.Names[index], Name))
        return index;

// Not found, so return -1:
return -1;
 }

/***************************************************************************\
  Opens the specified file from the specified RDF.
  Note: A NULL RDF is considered a file from disk
  Returns: RDF_OK if successful, RDF_FAILED otherwise.
\***************************************************************************/
int RDFILE::Open(char *Name, RDFILE *S)
{
char sln;

// Are we opening a file from another RDF or from disk?
if (!S)
    {
    // Try to open the file from disk:
    int fd = open(Name, O_RDONLY | O_BINARY);

    // If we couldn't, return an error:
    if (fd == -1) { return RDF_FAILED; };
    
    // Setup everything:
    Offset = 0;
    Size = filelength(fd);
    End = Size;
    
    // Make sure the file is long enough for a type code:
    if (Size < 4) 
        {
        Close();
        return RDF_FAILED;
         }
    
    // Read the type code:
    read(fd, Type, 4);
    
    // Is this file a rdf?  If so, set it up:
    if (!stricmp(Type, "RDTF")) goto SetupDirectory;
    
    // And return ok:
    return RDF_OK;
     }

// Otherwise, we're opening from another RDF:
else
    {
    // Get an index to the file in the source rdf:
    int index = S->GetFile(Name);
    
    // If there is no file by that name, return an error:
    if (index == -1) return RDF_FAILED;
    
    // Now setup our data with the info from that file:
    fd = S->fd;
    Offset = S->Directory.Start[index];
    Size = S->Directory.Size[index];
    End = Offset + Size;
    
    // Make sure the file is long enough for a type code:
    if (Size < 4) 
        {
        Close();
        return RDF_FAILED;
         }
    
    // Read the type code:
    read(fd, Type, 4);
    
    // Is this file a rdf?  If so, set it up:
    if (!stricmp(Type, "RDTF")) goto SetupDirectory;
    
    // And return ok:
    return RDF_OK;
     }

// Setup the directory if the file is an RDF:
SetupDirectory:

// Read the number of files in the directory:
Directory.Num_Files = 0; read(fd, &Directory.Num_Files, 2);

// Now read all the directory entries:
for (int index = 0; index < Directory.Num_Files; index ++)
    {
    // Read the length of the filename:
    read(fd, &sln, 1);
    
    // Read the filename:
    delete [] Directory.Names[index];
    if (!(Directory.Names[index] = new char [sln + 2]))
        {
        Close();
        return RDF_FAILED;
         }
    read(fd, Directory.Names, sln);
    Directory.Names[sln + 1] = '\0';
    
    // Read the start and size of file data:
    read(fd, &Directory.Start[index], 4);
    read(fd, &Directory.Size[index], 4);
     }

// And return ok:
return RDF_OK;
 }

/***************************************************************************\
  Closes the file.
\***************************************************************************/
int RDFILE::Close(void)
{
// Reset everything:
close(fd);
Offset = Size = End = 0;
Directory.Num_Files = 0;
return RDF_OK;
 }

/***************************************************************************\
  Reads a float from the file.
\***************************************************************************/
int RDFILE::GetFloat(float &D)
{
if (read(fd, &D, sizeof(float)) < sizeof(float))
    return RDF_FAILED;
return RDF_OK;
 }

/***************************************************************************\
  Reads a long (4-byte) integer from the file.
\***************************************************************************/
int RDFILE::GetLong(long &D)
{
if (read(fd, &D, 4) < 4)
    return RDF_FAILED;
return RDF_OK;
 }

/***************************************************************************\
  Reads a short (2-byte) interger from the file and initializes the
  dest to null.
\***************************************************************************/
int RDFILE::GetShort(int &D)
{
D = 0;
if (read(fd, &D, 2) < 2)
    return RDF_FAILED;
return RDF_OK;
 }

/***************************************************************************\
  Reads a short (2-byte) interger from the file.
\***************************************************************************/
int RDFILE::GetShort(short &D)
{
if (read(fd, &D, 2) < 2)
    return RDF_FAILED;
return RDF_OK;
 }

/***************************************************************************\
  Reads a byte from the file.  Initializes the dest to null.
\***************************************************************************/
int RDFILE::GetByte(int &D)
{
D = 0;
if (read(fd, &D, 1) < 1)
    return RDF_FAILED;
return RDF_OK;
 }

/***************************************************************************\
  Reads a byte from the file.
\***************************************************************************/
int RDFILE::GetByte(char &D)
{
if (read(fd, &D, 1) < 1)
    return RDF_FAILED;
return RDF_OK;
 }

/***************************************************************************\
  Reads a string from the file.  The string consists of a 1 byte header 
  that contains the number of characters in the string.  A null character
  is appended to the end of the string once it has been read.
  Care must be taken that the buffer has enough space for any string.
\***************************************************************************/
int RDFILE::GetString(char *Str)
{
char StrLen = 0;
if (read(fd, &StrLen, 1) < 1) return RDF_FAILED;
if (read(fd, Str, StrLen) < StrLen) return RDF_FAILED;
Str[StrLen + 1] = '\0';
return RDF_OK;
 }

