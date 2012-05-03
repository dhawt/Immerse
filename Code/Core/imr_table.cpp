/****************************************************************\
 
 iMMERSE Engine
 (C) 1999 No Tears Shed Software
 All rights reserved
 
 Filename: IMR_Table.hpp
 Description: Lookup tables
 
\****************************************************************/
#include "IMR_Table.hpp"

// Globals:
int TablesBuilt = 0;
float SinTable[IMR_DEGREECOUNT + 1], CosTable[IMR_DEGREECOUNT + 1];

/***************************************************************************\
  Generates all the lookup tables if needed.
\***************************************************************************/
void IMR_BuildTables(void)
{
if (TablesBuilt) return;

// Build the trig tables:
float AngToRad = (3.1415926 * 2.0F) / IMR_DEGREECOUNT;
for (int Angle = 0; Angle <= IMR_DEGREECOUNT; ++ Angle)
    {
    float Degree = Angle;
    CosTable[Angle] = cos(AngToRad * Degree);
    SinTable[Angle] = sin(AngToRad * Degree);
     }

// Flag that the tables have been initialized:
TablesBuilt = 1;
 }
