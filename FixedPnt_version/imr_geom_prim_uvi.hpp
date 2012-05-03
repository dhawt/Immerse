/***************************************************************************\
  File: IMR_Geom_Prim_UVI.hpp
  Description: Header for UV texture coords and I intensities.
  Author: Daniel Hawthorn
  Modified:
  
\***************************************************************************/
#ifndef __IMR_GEOM_PRIM_UVI__HPP
#define __IMR_GEOM_PRIM_UVI__HPP

// Texture coordinate and intensity class:
class IMR_UVIInfo
    {
    public:
      float U, V;
      int I, R, G, B;
      IMR_UVIInfo() { U = V = 0.0; I = R = G = B = 0; };
     };

#endif

