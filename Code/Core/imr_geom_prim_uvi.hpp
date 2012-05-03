/****************************************************************\
 
 iMMERSE Engine
 (C) 1999 No Tears Shed Software
 All rights reserved
 
 Filename: IMR_Geom_Prim_UVI.hpp
 Description: Header for textel coordinates and colors.
 
\****************************************************************/
#ifndef __IMR_GEOM_PRIM_UVI__HPP
#define __IMR_GEOM_PRIM_UVI__HPP

// Texture coordinate and intensity+color class:
class IMR_UVIInfo
    {
    public:
      float U, V;
      float I, R, G, B;
      IMR_UVIInfo() { U = V = I = R = G = B = 0; };
     };

#endif

