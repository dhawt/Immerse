/****************************************************************\
 
 iMMERSE Engine
 (C) 1999 No Tears Shed Software
 All rights reserved
 
 Filename: IMR_Collide.hpp
 Description: Simple ellipsoid-space based collision detection library
 
\****************************************************************/
#include "IMR_Collide.hpp"

IMR_3DPoint DbgInfo[4];

/***************************************************************************\
  Returns true if the specified sphere intersects with the bounding sphere
  of the specified poly.
\***************************************************************************/
int IMR_Collide_InRange(IMR_3DPoint &Pnt1, float RadiusSquared, IMR_Polygon &Poly)
{
float dX, dY, dZ, DistSquared;
IMR_3DPoint Hit, Delta;

Delta = Poly.Centroid - Hit;
dX = Delta.X;
dY = Delta.Y; 
dZ = Delta.Z;
DistSquared = (dX * dX) + (dY * dY) + (dZ * dZ);
if (DistSquared < RadiusSquared)
    return 1;

return 0;
 }

/***************************************************************************\
  Checks for collisions on the specified model.  Passed position should be
  in world ellipsoid space, will be converted to model and ellipsoid space 
  internally.
  Quadrilaterals will be split into triangles and tested seperately.
  Updates collision info structure according to collision status.
  
  Code originally by Telemachos of Peroxide and adapted by DH
  Returns flag stating actions taken.
\***************************************************************************/
int IMR_Collide_CheckModCollision(IMR_Model *Mdl, IMR_3DPoint WorldPos, IMR_Attitude WorldAtd, IMR_CollideInfo &Info)
{
int A, B, C;
IMR_3DPoint p1,p2,p3;
IMR_3DPoint pNormal;
IMR_3DPoint pOrigin;
IMR_3DPoint v1, v2;

// Make sure we have a model:
if (!Mdl)
    {
    IMR_LogMsg(__LINE__, __FILE__, "No model passed.");
    return IMR_COLLIDE_NOCOLLISION;
     }

// From info:
IMR_3DPoint source = Info.SourcePoint;
IMR_3DPoint velocity = Info.Velocity;
IMR_3DPoint eRadius; eRadius.X = eRadius.Z = Info.invRh; eRadius.Y = Info.invRv;

// Setup intersection data:
IMR_3DPoint sIPoint;    // sphere intersection point
IMR_3DPoint pIPoint;    // plane intersection point  
IMR_3DPoint polyIPoint; // polygon intersection point

// Make a normalized velocity vector and transform our other vectors:
IMR_3DPoint normalizedVelocity = velocity;
normalizedVelocity.Make_Unit();

// Calculate length of velocity vector:
float distanceToTravel = velocity.Mag();
float distToPlaneIntersection;
float distToEllipsoidIntersection;
  
// Loop through all faces in the model:
int isQuad = 0;
int index;

// Get transformed coords. for the model:
IMR_Matrix Transform;
Transform.Rotate(WorldAtd.X, WorldAtd.Y, WorldAtd.Z);
for (index = 0; index < Mdl->Num_Vertices; index ++)
    {
    Mdl->Vertices[index].LocalToActive();
    Mdl->Vertices[index].Transform(Transform);
    Mdl->Vertices[index] += WorldPos;           // Leave in active buffer...
     }

// Now check for collisions on each poly:
for (int i = 0; i < Mdl->Num_Polygons; i ++)
    {
    index = i;

    // Handle quads:
    if (Mdl->Polygons[i].Num_Verts == 4 && isQuad == 0) // First loop through this quad, do first half
        {
        isQuad = 1;
        A = Mdl->Polygons[i].Vtx_Index[0];
        B = Mdl->Polygons[i].Vtx_Index[1];
        C = Mdl->Polygons[i].Vtx_Index[3];
        i --;   // Do this poly again on the next iteration
         }
    else if (Mdl->Polygons[i].Num_Verts == 4 && isQuad == 1) // Second loop through this quad, do second half
        {
        isQuad = 0;
        A = Mdl->Polygons[i].Vtx_Index[1];
        B = Mdl->Polygons[i].Vtx_Index[2];
        C = Mdl->Polygons[i].Vtx_Index[3];
         }
    else                                        // Just a triangle... no problem        
        {
        A = Mdl->Polygons[i].Vtx_Index[0];
        B = Mdl->Polygons[i].Vtx_Index[1];
        C = Mdl->Polygons[i].Vtx_Index[2];
         }

    //////
    // Hack alert!
    //    Normal vertices in model shouldn't be used as regular vertices for polygons,
    //    assuming the models are properly constructed.
    //////

    // Get the data for the triangle:
    p1 = Mdl->Vertices[A] * eRadius;
    p2 = Mdl->Vertices[B] * eRadius;
    p3 = Mdl->Vertices[C] * eRadius;
    
    // Get normal to plane containing polygon:
    pNormal = *Mdl->Polygons[index].Normal - *Mdl->Polygons[index].Vtx_List[0];
    pNormal.Make_Unit();
    //pNormal = (pNormal * eRadius) - *Mdl->Polygons[index].Vtx_List[0];

    pOrigin = p1;
    v1 = p2 - p1;
    v2 = p3 - p1;

    // You might not need this if you KNOW all your triangles are valid
    if (!(v1.IsZero() || v2.IsZero()))
        {
        // Ignore backfaces:
        if (pNormal.Dot_Product(normalizedVelocity) < 1.0f)
            { 
            // Calculate sphere intersection point:
            sIPoint = source - pNormal;

            // Classify point to determine if ellipsoid spans the plane:
            int pClass = IMR_Collide_ClassifyPoint(sIPoint, pOrigin, pNormal);

            // Find the plane intersection point:
            if (pClass == IMR_COLLIDE_PLANE_BACKSIDE) // Plane is embedded in ellipsoid
                {
                // Find plane intersection point by shooting a ray from the
                // sphere intersection point along the planes normal:
                distToPlaneIntersection = IMR_Collide_IntersectRayPlane(sIPoint, pNormal, pOrigin, pNormal);

                // Calculate plane intersection point:
                pIPoint.X = sIPoint.X + distToPlaneIntersection * pNormal.X;
                pIPoint.Y = sIPoint.Y + distToPlaneIntersection * pNormal.Y;
                pIPoint.Z = sIPoint.Z + distToPlaneIntersection * pNormal.Z;
                 }
            else
                {
                // Shoot ray along the velocity vector:
                distToPlaneIntersection = IMR_Collide_IntersectRayPlane(sIPoint, normalizedVelocity, pOrigin, pNormal);

                // Calculate plane intersection point:
                pIPoint.X = sIPoint.X + distToPlaneIntersection * normalizedVelocity.X;
                pIPoint.Y = sIPoint.Y + distToPlaneIntersection * normalizedVelocity.Y;
                pIPoint.Z = sIPoint.Z + distToPlaneIntersection * normalizedVelocity.Z;
                 }

            // Find polygon intersection point. By default we assume its equal to the 
            // plane intersection point:
            polyIPoint = pIPoint;
            distToEllipsoidIntersection = distToPlaneIntersection;
            if (!IMR_Collide_CheckPointInTriangle(pIPoint, p1, p2, p3))   // If not in triangle...
                {
                polyIPoint = IMR_Collide_ClosestPointOnTriangle(p1, p2, p3, pIPoint);
                IMR_3DPoint Negated = normalizedVelocity;
                Negated.X = -Negated.X;
                Negated.Y = -Negated.Y;
                Negated.Z = -Negated.Z;
                distToEllipsoidIntersection = IMR_Collide_IntersectRaySphere(polyIPoint, Negated, source, 1.0f);

                // Calculate true sphere intersection point:
                if (distToEllipsoidIntersection > 0)
                    {
                    sIPoint.X = polyIPoint.X + (distToEllipsoidIntersection * Negated.X);
                    sIPoint.Y = polyIPoint.Y + (distToEllipsoidIntersection * Negated.Y);
                    sIPoint.Z = polyIPoint.Z + (distToEllipsoidIntersection * Negated.Z);
                     }
                 }

            DbgInfo[0] = polyIPoint;
            
            // Here we do the error checking to see if we got ourself stuck last frame:
            if (IMR_Collide_CheckPointInSphere(polyIPoint, source, 1.0f))
                Info.stuck = 1;

            // Ok, now we might update the collision data if we hit something:
            if ((distToEllipsoidIntersection > 0) && (distToEllipsoidIntersection <= distanceToTravel))
                { 
                if (!Info.foundCollision || (distToEllipsoidIntersection < Info.nearestDistance))
                    {
                    // If this is the first hit or the closest so far, save the information:
                    Info.nearestDistance = distToEllipsoidIntersection;
                    Info.nearestIntersectionPoint = sIPoint;
                    Info.nearestPolygonIntersectionPoint = polyIPoint;
                    Info.foundCollision = 1;
                     }
                 } 
             } // If not backface
         } // If a valid plane        
     } // For all faces     

// And return our action flag:
if (Info.foundCollision) return IMR_COLLIDE_COLLIDING;
return IMR_COLLIDE_NOCOLLISION;
 }

/***************************************************************************\
  Returns the distance to plane in world units, -1 if no intersection.
  Expects normalized directinoal vectors.
\***************************************************************************/
float IMR_Collide_IntersectRayPlane(IMR_3DPoint rOrigin, IMR_3DPoint rVector, IMR_3DPoint pOrigin, IMR_3DPoint pNormal)
{
float d = -(pNormal.Dot_Product(pOrigin));
float numer = pNormal.Dot_Product(rOrigin) + d;
float denom = pNormal.Dot_Product(rVector);
if (denom == 0)  // normal is orthogonal to vector, cant intersect
    return -1.0f;
return -(numer / denom);      
 }

/***************************************************************************\
  Returns the distance to sphere in world units, -1 if no intersection.
  Expects normalized directinoal vectors.
\***************************************************************************/
float IMR_Collide_IntersectRaySphere(IMR_3DPoint rO, IMR_3DPoint rV, IMR_3DPoint sO, float sR)
{
IMR_3DPoint Q = sO - rO;
   
float c = Q.Mag();
float v = Q.Dot_Product(rV);
float d = (sR * sR) - (c * c) - (v * v);

// If there was no intersection, return -1:
if (d < 0.0) return (-1.0f);

// Return the distance to the [first] intersecting point:
return (v - sqrt(d));
}

/***************************************************************************\
  Returns true if the point is in the triangle, false otherwise.
  Expects clockwise vertex ordering.
\***************************************************************************/
bool IMR_Collide_CheckPointInTriangle(IMR_3DPoint point, IMR_3DPoint a, IMR_3DPoint b, IMR_3DPoint c)
{
double total_angles = 0.0f;

// Make the 3 vectors:
IMR_3DPoint v1 = point - a;
IMR_3DPoint v2 = point - b;
IMR_3DPoint v3 = point - c;
  
v1.Make_Unit();
v2.Make_Unit();
v3.Make_Unit();

total_angles += acos(v1.Dot_Product(v2));   
total_angles += acos(v2.Dot_Product(v3));
total_angles += acos(v3.Dot_Product(v1)); 

if (fabs((6.2848f) - total_angles) <= 0.005) return 1;
return 0;
 }

/***************************************************************************\
  Returns the closest point on the line segment to the input point.
\***************************************************************************/
IMR_3DPoint IMR_Collide_ClosestPointOnLine(IMR_3DPoint &a, IMR_3DPoint &b, IMR_3DPoint &p)
{
// Determine t (the length of the vector from ‘a’ to ‘p’):
IMR_3DPoint c = p - a;
IMR_3DPoint V = b - a; 
float d = V.Mag();
V.Make_Unit();  
float t = V.Dot_Product(c);

// Check to see if ‘t’ is beyond the extents of the line segment
if (t < 0.0f) return (a);
if (t > d) return (b);
  
// Return the point between ‘a’ and ‘b’
// Set length of V to t. V is normalized so this is easy:
V.X = V.X * t;
V.Y = V.Y * t;
V.Z = V.Z * t;
           
return (a + V);        
}

/***************************************************************************\
  Returns the closest point on the triangle to the input point.
\***************************************************************************/
IMR_3DPoint IMR_Collide_ClosestPointOnTriangle(IMR_3DPoint a, IMR_3DPoint b, IMR_3DPoint c, IMR_3DPoint p)
{
IMR_3DPoint Rab = IMR_Collide_ClosestPointOnLine(a, b, p);
IMR_3DPoint Rbc = IMR_Collide_ClosestPointOnLine(b, c, p);
IMR_3DPoint Rca = IMR_Collide_ClosestPointOnLine(c, a, p);
    
float dAB = (p - Rab).Mag();
float dBC = (p - Rbc).Mag();
float dCA = (p - Rca).Mag();
  
float min = dAB;
IMR_3DPoint result = Rab;

if (dBC < min) { min = dBC; result = Rbc; }
if (dCA < min) result = Rca;
  
return result;      
}

/***************************************************************************\
  Returns true if the point is contained in the specified sphere, false if 
  not.
\***************************************************************************/
bool IMR_Collide_CheckPointInSphere(IMR_3DPoint point, IMR_3DPoint sO, float sR)
{
float d = (point - sO).Mag();
if ( d <= sR) return 1;
return 0;  
 }

/***************************************************************************\
  Returns unit normal vector to the tangent plane of the ellipsoid in
  the point.
\***************************************************************************/
IMR_3DPoint IMR_Collide_TangentPlaneNormalOfEllipsoid(IMR_3DPoint point, IMR_3DPoint eO, IMR_3DPoint eR)
{
IMR_3DPoint p = point - eO;

float a2 = eR.X * eR.X;
float b2 = eR.Y * eR.Y;
float c2 = eR.Z * eR.Z;
 
IMR_3DPoint res;
if (a2) res.X = p.X / a2;
if (b2) res.Y = p.Y / b2;
if (c2) res.Z = p.Z / c2;

res.Make_Unit();
return (res);  
 }

/***************************************************************************\
  Returns one of 3 classification codes
\***************************************************************************/
int IMR_Collide_ClassifyPoint(IMR_3DPoint point, IMR_3DPoint pO, IMR_3DPoint pN)
{
IMR_3DPoint dir = pO - point;
float d = dir.Dot_Product(pN);
 
if (d < -0.001f) return IMR_COLLIDE_PLANE_FRONT;   
else if (d > 0.001f) return IMR_COLLIDE_PLANE_BACKSIDE;        
return IMR_COLLIDE_ON_PLANE;        
}


