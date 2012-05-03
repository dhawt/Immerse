/****************************************************************\

 iMMERSE Engine
 (C) 1999 No Tears Shed Software
 All rights reserved
 
 Filename: IMR_RDFMangr.cpp
 Description: RDF manager
 
\****************************************************************/
#include "IMR_RDFMngr.hpp"

/****************************************************************\
  Returns true if the passed filename (without path) is the same 
  as the resource filename (without path).
  Returns the RDFResourceDesc if successful, otherwise -1.
\****************************************************************/
int IMR_RDFListEnt::IsFile(char *f)
{
int Len, FNameStartIndex;

// First find the start of the filename:
Len = strlen(File);
FNameStartIndex = Len - 1;
while (FNameStartIndex >= 0 && 
       File[FNameStartIndex] != '\\' && 
       File[FNameStartIndex] != ':')
       {
       FNameStartIndex --;
        };
if (FNameStartIndex != 0) FNameStartIndex ++;

// Now compare the two names:
return !stricmp(f, (char *)&File[FNameStartIndex]);
 }

/****************************************************************\
  Allocates space for a new RDF entry in the directory.
  Returns the index to the new space if successful, otherwise
  -1.
\****************************************************************/
int IMR_RDFManager::AddDirEnt(void)
{
// Make sure we have space for a new entry:
if (NumDirEnts >= IMR_MaxRDFListEntries)
    {
    IMR_LogMsg(__LINE__, __FILE__, "Max director entires exceeded! (%d)", IMR_MaxRDFListEntries);
    return IMRERR_GENERIC;
     }

// Allocate memory:
Directory[NumDirEnts ++] = new IMR_RDFListEnt;

// Make sure it worked:
if (NumDirEnts >= IMR_MaxRDFListEntries)
    {
    IMR_LogMsg(__LINE__, __FILE__, "Out of memory allocating %d bytes!", sizeof(IMR_RDFListEnt));
    return IMRERR_OUTOFMEM;
     }

// And return an index:
return NumDirEnts - 1;
 }

/****************************************************************\
  Finalizes the class.
\****************************************************************/
void IMR_RDFManager::Finalize(void)
{
for (int Index = 0; Index < NumDirEnts; Index ++)
    {
    if (Directory[Index]) delete [] Directory[Index]; 
    Directory[Index] = NULL;
     }
NumDirEnts = 0;
 }

/****************************************************************\
  Adds to the list of available RDFs.
  Returns ok if successful, otherwise an error code.
\****************************************************************/
int IMR_RDFManager::AddRDF(char *FileName)
{
int fd, Index;
char Buffer[80];

// Make sure the file exists:
if (!(fd = open(FileName, O_RDONLY | O_BINARY)))
    {
    IMR_LogMsg(__LINE__, __FILE__, "Can not open file %s!", FileName);
    return IMRERR_BADFILE;
     }

// Add a new entry to the directory:
Index = AddDirEnt();

// Make sure it worked:
if (Index == -1)
    {
    IMR_LogMsg(__LINE__, __FILE__, "AddDirEnt() failed!");
    return IMRERR_GENERIC;
     }

// Read info from the RDF:
Buffer[3] = 0; read(fd, Buffer, 3); Directory[Index]->Set_Type(Buffer);      // Read RDF type ID
Buffer[32] = 0; read(fd, Buffer, 32); Directory[Index]->Set_Resource(Buffer);// Read RDF resource name
Directory[Index]->Set_File(FileName);
//IMR_LogMsg(__LINE__, __FILE__, "Loaded %s,%s,%s", Directory[Index]->Type, Directory[Index]->Resource, Directory[Index]->File);

// Close the file:
close(fd);
return IMR_OK;
 }

/****************************************************************\
  Attempts to find the specified resource.
  Returns the RDFResourceDesc if successful, otherwise -1.
\****************************************************************/
IMR_RDFResourceDesc IMR_RDFManager::FindRDF(char *rdfTyp, char *rdfNam, char *rdfRes)
{
int IsType, IsFile, IsResource;

// First make sure we have any RDFs:
if (!NumDirEnts) 
    {
    IMR_LogMsg(__LINE__, __FILE__, "No RDFs loaded.");
    return -1;
     }

// Now loop through all the RDFs in the directory and find the first one that matches:
for (int Index = 0; Index < NumDirEnts; Index ++)
    {
    // Check if the specified parameters match the current file:
    if (rdfTyp) IsType = Directory[Index]->IsType(rdfTyp); else IsType = 1;    // Assume true...
    if (rdfNam) IsFile = Directory[Index]->IsFile(rdfNam); else IsFile = 1;    // Assume true...
    if (rdfRes) IsResource = Directory[Index]->IsResource(rdfRes); else IsResource = 1;    // Assume true...

    // Everything matches?  Return the descriptor (i.e. index):
    if (IsType && IsFile && IsResource)
        return Index;
     }

// Couldn't find the file, so return -1:
IMR_LogMsg(__LINE__, __FILE__, "RDF %s,%s,%s not found.", rdfTyp, rdfNam, rdfRes);
return -1;
 }

/****************************************************************\
  Finds the specified resource and returns the full path + fname
  Returns the filename if successful, otherwise NULL.
\****************************************************************/
char *IMR_RDFManager::GetRDFLocation(char *rdfTyp, char *rdfNam, char *rdfRes)
{
IMR_RDFResourceDesc Desc;

// Get the index for the specified file:
Desc = FindRDF(rdfTyp, rdfNam, rdfRes);

// If we couldn't find the specified resource, return NULL:
if (Desc == -1) return NULL;

// Now get the full path and filename for the file:
return Directory[Desc]->Get_File();
 }

/****************************************************************\
  Attempts to open the specified RDF.
  Returns ok if successful, otherwise an error code.
\****************************************************************/
int IMR_RDFManager::OpenRDF(IMR_RDFResourceDesc RD, int &fd)
{
// First make sure we have any RDFs:
if (!NumDirEnts) 
    {
    IMR_LogMsg(__LINE__, __FILE__, "No RDFs loaded.");
    return IMRERR_NODATA;
     }

// First make sure we have the specified file:
if (RD >= NumDirEnts || RD < 0) 
    {
    IMR_LogMsg(__LINE__, __FILE__, "Illegal RDFResourceDesc passed. (%d)", RD);
    return IMRERR_GENERIC;
     }

// Now open the file:
fd = open(Directory[RD]->Get_File(), O_RDONLY | O_BINARY);
if (!fd)
    {
    IMR_LogMsg(__LINE__, __FILE__, "Could not open file %s!", Directory[RD]->Get_File());
    return IMRERR_BADFILE;
     }

// And return ok:
return IMR_OK;
 }
