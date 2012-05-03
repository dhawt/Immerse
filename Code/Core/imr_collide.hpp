/****************************************************************\
 
 iMMERSE Engine
 (C) 1999 No Tears Shed Software
 All rights reserved
 
 Filename: IMR_Collide.hpp
 Description: Header
 
\****************************************************************/
#ifndef __IMR_COLLIDE__HPP
#define __IMR_COLLIDE__HPP

// Include headers:
#include <math.h>
#include "IMR_Geom_Prim.hpp"
#include "IMR_Geom_Model.hpp"
#include "IMR_Matrix.hpp"

extern IMR_3DPoint DbgInfo[4];

// Constants:
#define IMR_COLLIDE_EPSILON     0.05f
#define IMR_COLLIDE_NOCOLLISION 0x01
#define IMR_COLLIDE_COLLIDING   0x02
#define IMR_COLLIDE_PLANE_BACKSIDE 0x000001
#define IMR_COLLIDE_PLANE_FRONT    0x000002
#define IMR_COLLIDE_ON_PLANE       0x000004

// CollideInfo class:
class IMR_CollideInfo
    {
    public:
      // Constraints:
      float Rv, RvSqrd,     // Vertical radius (for collisions with floor and ceiling)
             Rh, RhSqrd,    // Horizontal radius (for collisions with walls)
             invRv, invRh;  // Inverse radius (1/r)
      
      // Info for original motion:
      IMR_3DPoint Velocity;
      IMR_3DPoint SourcePoint;

      // Info for collision response:
      bool foundCollision;
      float nearestDistance;                        // Dist to hit point on sphere
      IMR_3DPoint nearestIntersectionPoint;         // Hitpoint on sphere
      IMR_3DPoint nearestPolygonIntersectionPoint;  // Hitpoint on polygon
      
      // Error handling:
      IMR_3DPoint lastSafePosition;
      bool stuck; 

      // Functions:
      inline void Setup_Ellipsoid(float rv, float rh)
          {
          Rv = rv;  RvSqrd = rv * rv;
          Rh = rh;  RhSqrd = rh * rh;
          if (Rv) invRv = 1 / Rv; else invRv = 0;
          if (Rh) invRh = 1 / Rh; else invRh = 0;         
           };
     };

// Prototypes:
int IMR_Collide_CheckModCollision(IMR_Model *Mdl, IMR_3DPoint ModPos, IMR_Attitude ModAtd, IMR_CollideInfo &Info);
int IMR_Collide_InRange(IMR_3DPoint &Pnt1, float RadiusSquared, IMR_Polygon &Poly);
float IMR_Collide_IntersectRayPlane(IMR_3DPoint rOrigin, IMR_3DPoint rVector, IMR_3DPoint pOrigin, IMR_3DPoint pNormal); 
float IMR_Collide_IntersectRaySphere(IMR_3DPoint rO, IMR_3DPoint rV, IMR_3DPoint sO, float sR);
IMR_3DPoint IMR_Collide_ClosestPointOnLine(IMR_3DPoint &a, IMR_3DPoint &b, IMR_3DPoint &p);
IMR_3DPoint IMR_Collide_ClosestPointOnTriangle(IMR_3DPoint a, IMR_3DPoint b, IMR_3DPoint c, IMR_3DPoint p);
bool IMR_Collide_CheckPointInTriangle(IMR_3DPoint point, IMR_3DPoint a, IMR_3DPoint b, IMR_3DPoint c);
bool IMR_Collide_CheckPointInSphere(IMR_3DPoint point, IMR_3DPoint sO, float sR);
IMR_3DPoint IMR_Collide_TangentPlaneNormalOfEllipsoid(IMR_3DPoint point, IMR_3DPoint eO, IMR_3DPoint eR);
int IMR_Collide_ClassifyPoint(IMR_3DPoint point, IMR_3DPoint pO, IMR_3DPoint pN);

#endif
