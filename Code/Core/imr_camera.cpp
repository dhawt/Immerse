/****************************************************************\
 
 iMMERSE Engine
 (C) 1999 No Tears Shed Software
 All rights reserved
 
 Filename: IMR_Camera.hpp
 Description: Camera class code module
 
\****************************************************************/
#include "IMR_Camera.hpp"

/***************************************************************************\
  Sets the name of the camera.
\***************************************************************************/
void IMR_Camera::Set_Name(char *NewName)
{
if (strlen(NewName) > 8)
    {
    memcpy((void *)Name, (void *)NewName, 8);
    Name[8] = '/0';
     }
else
    strcpy(Name, NewName);
 }
