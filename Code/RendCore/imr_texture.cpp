/****************************************************************\
 
 iMMERSE Engine
 (C) 1999 No Tears Shed Software
 All rights reserved
 
 Filename: IMR_Texture.hpp
 Description: Texture primitive methods
 
\****************************************************************/
#include "IMR_Texture.hpp"

/***************************************************************************\
  Sets the name of the texture.
\***************************************************************************/
void IMR_Texture::Set_Name(char *NewName)
{
if (strlen(NewName) > 8)
    {
    memcpy((void *)Name, (void *)NewName, 8);
    Name[8] = '/0';
     }
else
    strcpy(Name, NewName);
 }
