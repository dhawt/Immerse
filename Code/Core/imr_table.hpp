/****************************************************************\
 
 iMMERSE Engine
 (C) 1999 No Tears Shed Software
 All rights reserved
 
 Filename: IMR_Table.hpp
 Description: Header for all tables
 Notes: DegreeCount must be a power of 2!
 
\****************************************************************/
#ifndef __IMR_TABLE__HPP
#define __IMR_TABLE__HPP

// Include headers:
#include <math.h>

// Constants and macros:
#define IMR_DEGREECOUNT     1024
#define IMR_DEGREEMOD       1024
#define IMR_DEGREEAND       1023
#define IMR_HALFDEGREECOUNT 512

// Globals:
extern float SinTable[IMR_DEGREECOUNT + 1], CosTable[IMR_DEGREECOUNT + 1];

// Prototypes:
void IMR_BuildTables(void);

#endif
