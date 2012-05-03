/****************************************************************\
 
 iMMERSE Engine
 (C) 1999 No Tears Shed Software
 All rights reserved
 
 Filename: IMR_Material.hpp
 Description: Material class code module
 
\****************************************************************/
#include "IMR_Material.hpp"

/***************************************************************************\
  Sets the name of the material.
\***************************************************************************/
void IMR_Material::Set_Name(char *NewName)
{
if (strlen(NewName) > 8)
    {
    memcpy((void *)TextureName, (void *)NewName, 8);
    Name[8] = '/0';
     }
else
    strcpy(Name, NewName);
 }

/***************************************************************************\
  Sets the name of the texture for this material.
\***************************************************************************/
void IMR_Material::Set_TextureName(char *NewName)
{
if (strlen(NewName) > 8)
    {
    memcpy((void *)TextureName, (void *)NewName, 8);
    Name[8] = '/0';
     }
else
    strcpy(TextureName, NewName);
 }
