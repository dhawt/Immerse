/****************************************************************\
 
 iMMERSE Engine
 (C) 1999 No Tears Shed Software
 All rights reserved
 
 Filename: IMR_Geom_Light.hpp
 Description: Light geometry module.  Contains all lighting code.
 
\****************************************************************/
#include "IMR_Geom_Light.hpp"

/***************************************************************************\
  Sets the direction of the light given the specified angle. 
  Note: The light, by default, points down the positive Z axis.
\***************************************************************************/
void IMR_Light::Set_Direction(int AngX, int AngY)
{
IMR_Matrix Rot;

// Make sure the angle is ok:
AngX %= IMR_DEGREECOUNT;
AngY %= IMR_DEGREECOUNT;

// Create a vector using the angle:
Rot.Rotate(AngX, AngY, 0);
Direction.X = Direction.Y = 0.0;
Direction.Z = -1.0;
Direction.Transform(Rot);
 }

/***************************************************************************\
  Sets the direction of light to point to the specified 3DPoint. 
  Note: Make sure the position of the light has been defined before calling
        this method.
\***************************************************************************/
void IMR_Light::Set_Direction(IMR_3DPoint &Target)
{
// Create a vector to the target from the position of the light:
Direction = Target - Position;

// Normalize the vector:
Direction.Make_Unit();
 }

/***************************************************************************\
  Illuminate each polygon in the list.
\***************************************************************************/
void IMR_Light::IlluminatePolyList(IMR_Polygon *PList, int Num_Polys)
{
int poly, vtx, PolyVisible, p,
    PolyVisable, InRange[4];
float DeltaX, DeltaY, DeltaZ, Delta[4],
      LightDot, DotMag, ViewDot,
      Intensity, Attenuation, ConeIntensity,
      nX, nY, nZ,
      dX, dY, dZ,
      Distance,
      Cr, Cg, Cb;
IMR_3DPoint LDelta, Normal;

// Make sure we have a list:
if (!PList)
    {
    IMR_LogMsg(__LINE__, __FILE__, "NULL list passed!");
    return;
     }

// Loop through each polygon in the list:
for (poly = 0; poly < Num_Polys; poly ++)
    {
    // If this poly is a lightsource (or we are not doing any lighting), 
    // set at max intensity and move on:
    if (PList[poly].Flags.LightSource)
        {
        for (vtx = 0; vtx < PList[poly].Num_Verts; vtx ++)
            {
            PList[poly].UVI_Info[vtx].R = 1.0;
            PList[poly].UVI_Info[vtx].G = 1.0;
            PList[poly].UVI_Info[vtx].B = 1.0;
             }
        
        // Next poly:
        continue;
         }

    // Is this an ambient light?
    if (Type == IMR_LIGHT_AMBIENT)
        {
        // Loop through each vertex in the poly:
        for (vtx = 0; vtx < PList[poly].Num_Verts; vtx ++)
            {
            // More light:
            PList[poly].UVI_Info[vtx].R += ColorR;
            PList[poly].UVI_Info[vtx].G += ColorG;
            PList[poly].UVI_Info[vtx].B += ColorB;

            // Keep in range of 0-1:
            if (PList[poly].UVI_Info[vtx].R > 1.0) PList[poly].UVI_Info[vtx].R = 1.0;
            if (PList[poly].UVI_Info[vtx].G > 1.0) PList[poly].UVI_Info[vtx].G = 1.0;
            if (PList[poly].UVI_Info[vtx].B > 1.0) PList[poly].UVI_Info[vtx].B = 1.0;
             }

        // Next poly:
        continue;
         }

    // Is this a celestial light?
    if (Type == IMR_LIGHT_CELESTIAL)
        {
        // Find normal to poly:
        nX = PList[poly].Normal->wX - PList[poly].Vtx_List[0]->wX;
        nY = PList[poly].Normal->wY - PList[poly].Vtx_List[0]->wY;
        nZ = PList[poly].Normal->wZ - PList[poly].Vtx_List[0]->wZ;

        // Calculate dot product between poly normal and lightsource direction vector:
        LightDot = (nX * Direction.X) + (nY * Direction.Y) + (nZ * Direction.Z);
        
        // If no light is reflecting off the poly, go to the next poly:
        if (LightDot <= 0.0) continue;

        // Find level of light:
        Cr = LightDot * ColorR;
        Cg = LightDot * ColorG;
        Cb = LightDot * ColorB;
            
        // And light each vertex:
        for (vtx = 0; vtx < PList[poly].Num_Verts; vtx ++)
            {
            PList[poly].UVI_Info[vtx].R += Cr;
            PList[poly].UVI_Info[vtx].G += Cg;
            PList[poly].UVI_Info[vtx].B += Cb;

            // Keep in range of 0-1:
            if (PList[poly].UVI_Info[vtx].R > 1.0) PList[poly].UVI_Info[vtx].R = 1.0;
            if (PList[poly].UVI_Info[vtx].G > 1.0) PList[poly].UVI_Info[vtx].G = 1.0;
            if (PList[poly].UVI_Info[vtx].B > 1.0) PList[poly].UVI_Info[vtx].B = 1.0;
             }
         }

    // Is this a point light?
    if (Type == IMR_LIGHT_POINT)
        {
        // Find normal to poly:
        nX = PList[poly].Normal->wX - PList[poly].Vtx_List[0]->wX;
        nY = PList[poly].Normal->wY - PList[poly].Vtx_List[0]->wY;
        nZ = PList[poly].Normal->wZ - PList[poly].Vtx_List[0]->wZ;
        
        // Backface cull the poly:
        dX = WorldPos.X - PList[poly].Vtx_List[0]->wX;
        dY = WorldPos.Y - PList[poly].Vtx_List[0]->wY;
        dZ = WorldPos.Z - PList[poly].Vtx_List[0]->wZ;
        LightDot = (nX * dX) + (nY * dY) + (nZ * dZ);
        if (LightDot <= 0.0) continue;
        
        // Now loop through each vertex in the poly and see if it is within the
        // range of the light:
        PolyVisible = 0;
        for (vtx = 0; vtx < PList[poly].Num_Verts; vtx ++)
            {
            // Calculate squared distance from the light to this vertex:
            DeltaX = WorldPos.X - PList[poly].Vtx_List[vtx]->wX;
            DeltaY = WorldPos.Y - PList[poly].Vtx_List[vtx]->wY;
            DeltaZ = WorldPos.Z - PList[poly].Vtx_List[vtx]->wZ;
            Delta[vtx] = (DeltaX * DeltaX) + 
                         (DeltaY * DeltaY) + 
                         (DeltaZ * DeltaZ);

            // If the vertex is within range of the light (and is not in shadow), 
            // set the flag to visible:
            if (Delta[vtx] < RangeSquared)
                {
                /*   --- no shadows ---
                if (CurrentSettings->Effects.ShadowsEnabled)
                    {
                    for (p = 0; p < Num_Polys; p ++)
                        {
                        if (IMR_Collide_HitsPoly(WorldPos, *PList[poly].Vtx_List[vtx], PList[p]) &&
                            !PList[p].Flags.LightSource && 
                            p != poly)
                            {
                            InRange[vtx] = 0;
                            break;
                             }
                        else 
                            {
                            InRange[vtx] = 1;
                            PolyVisible = 1;
                             }
                         }
                     }
                else
                */
                    {
                    PolyVisible = 1;
                    InRange[vtx] = 1;
                     }
                 }
            else 
                InRange[vtx] = 0;
             }
        
        // If the poly isn't within the range of the light, go to the next poly:
        if (!PolyVisible) continue;

        // Now loop through each vertex in the poly and light it:
        for (vtx = 0; vtx < PList[poly].Num_Verts; vtx ++)
            {
            // If the vertex isn't in range, go to the next:
            if (!InRange[vtx]) continue;
            
            // Calculate the square root of the delta:
            Distance = sqrt(Delta[vtx]);

            // Calculate intensity:
            if (Range)
                Intensity = (Range - Distance) / Range;
            else 
                Intensity = 0.0;
            if (Intensity < 0.0) Intensity = -Intensity;
            
            // Calculate the dot product for this vertex:
            if (vtx != 0)       // We already have the dot product if this is vertex 0
                {
                dX = WorldPos.X - PList[poly].Vtx_List[vtx]->wX;
                dY = WorldPos.Y - PList[poly].Vtx_List[vtx]->wY;
                dZ = WorldPos.Z - PList[poly].Vtx_List[vtx]->wZ;
                LightDot = (nX * dX) + (nY * dY) + (nZ * dZ);
                LightDot /= sqrt((dX * dX) + (dY * dY) + (dZ * dZ));
                 }
            else                // Otherwise just normalize the one we have
                LightDot /= sqrt((dX * dX) + (dY * dY) + (dZ * dZ));
            
            // Now calculate color components:
            Attenuation = Intensity * LightDot;
            Cr = Attenuation * ColorR;
            Cg = Attenuation * ColorG;
            Cb = Attenuation * ColorB;
            
            // Add to the vertex:
            PList[poly].UVI_Info[vtx].R += Cr;
            PList[poly].UVI_Info[vtx].G += Cg;
            PList[poly].UVI_Info[vtx].B += Cb;
            if (Attenuation > 1.0)
                {
                PList[poly].UVI_Info[vtx].R = 1.0;
                PList[poly].UVI_Info[vtx].G = 1.0;
                PList[poly].UVI_Info[vtx].B = 1.0;
                 }           
                
            // Keep in range of 0-1:
            if (PList[poly].UVI_Info[vtx].R > 1.0) PList[poly].UVI_Info[vtx].R = 1.0;
            if (PList[poly].UVI_Info[vtx].G > 1.0) PList[poly].UVI_Info[vtx].G = 1.0;
            if (PList[poly].UVI_Info[vtx].B > 1.0) PList[poly].UVI_Info[vtx].B = 1.0;
             }

        // Next poly: 
        continue;
         }
     }
 }

/*
    // Is this a spot light?
    if (Type == IMR_LIGHT_SPOT)
        {
        // Find normal to poly:
        Normal = *PList[poly].Normal - *PList[poly].Vtx_List[0];

        // Backface cull the poly:
        LDelta = WorldPos - *PList[poly].Vtx_List[0];
        LightDot = LDelta.Dot_Product(Normal);
        if (LightDot <= 0) continue;

        // Now loop through each vertex in the poly and see if it is within the
        // range of the light:
        PolyVisible = 0;
        for (vtx = 0; vtx < PList[poly].Num_Verts; vtx ++)
            {
            // Calculate squared distance from the light to this vertex:
            DeltaX = FixedToInt(WorldPos.X - PList[poly].Vtx_List[vtx]->X);
            DeltaY = FixedToInt(WorldPos.Y - PList[poly].Vtx_List[vtx]->Y);
            DeltaZ = FixedToInt(WorldPos.Z - PList[poly].Vtx_List[vtx]->Z);
            Delta[vtx] = (DeltaX * DeltaX) + 
                         (DeltaY * DeltaY) + 
                         (DeltaZ * DeltaZ);
                                              
            // If the vertex is within range of the light, set the flag to visible:
            if (Delta[vtx] < RangeSquared)
                {
                PolyVisible = 1;
                InRange[vtx] = 1;
                 }
            else 
                InRange[vtx] = 0;
             }
        
        // If the poly isn't within the range of the light, go to the next poly:
        if (!PolyVisible) continue;

        // Loop through each vertex in the poly:
        for (vtx = 0; vtx < PList[poly].Num_Verts; vtx ++)
            {
            // If the vertex isn't in range, go to the next:
            if (!InRange[vtx]) continue;

            // Create a vector pointing from the vertex to the light:
            LDelta = *PList[poly].Vtx_List[vtx] - WorldPos;
            LDelta.Make_Unit();
         
            // Calculate the dot product between the light direction and the delta:
            ViewDot = LDelta.Dot_Product(WorldDirection);
            
            // Skip this vertex if no light is hitting it:
            if (ViewDot <= CosTable[Penumbra]) continue;

            // Calculate the distance:
            Distance = sqrt((float)Delta[vtx]);

            // Check if the vertex is inside the umbra:
            if (ViewDot > CosTable[Umbra])
                {
                // Calculate attenuation:
                Attenuation = FloatToFixed((fRange - Distance) / fRange);
                
                // Calculate the dot product for this vertex:
                dX = WorldPos.X - PList[poly].Vtx_List[vtx]->X;
                dY = WorldPos.Y - PList[poly].Vtx_List[vtx]->Y;
                dZ = WorldPos.Z - PList[poly].Vtx_List[vtx]->Z;
                LightDot = FixedMult(nX, dX) + FixedMult(nY, dY) + FixedMult(nZ, dZ);
                LightDot /= (int)sqrt(FixedToInt(LightDot));
                LightDot = (LightDot + 65536) >> 1;
                
                // Calculate color components:
                Intensity = FixedMult(Attenuation, LightDot);
                //if (Intensity < 0) Intensity = -Intensity;
                Cr = FixedToInt(Intensity * ColorR);
                Cg = FixedToInt(Intensity * ColorG);
                Cb = FixedToInt(Intensity * ColorB);
                
                // Add to the vertex:
                PList[poly].UVI_Info[vtx].R += Cr;
                PList[poly].UVI_Info[vtx].G += Cg;
                PList[poly].UVI_Info[vtx].B += Cb;
                if (Intensity < 0)
                    {
                    PList[poly].UVI_Info[vtx].R = 255;
                    PList[poly].UVI_Info[vtx].G = 0;
                    PList[poly].UVI_Info[vtx].B = 0;
                     }           
                
                // And keep in range of 0-255:
                if (PList[poly].UVI_Info[vtx].R > 255) PList[poly].UVI_Info[vtx].R = 255;
                if (PList[poly].UVI_Info[vtx].G > 255) PList[poly].UVI_Info[vtx].G = 255;
                if (PList[poly].UVI_Info[vtx].B > 255) PList[poly].UVI_Info[vtx].B = 255;
                
                // Next vertex:
                continue;
                 }

            // Calculate intensity:
            Intensity = FloatToFixed((fRange - Distance) / fRange);
            
            // Calculate the dot product for this vertex:
            dX = WorldPos.X - PList[poly].Vtx_List[vtx]->X;
            dY = WorldPos.Y - PList[poly].Vtx_List[vtx]->Y;
            dZ = WorldPos.Z - PList[poly].Vtx_List[vtx]->Z;
            LightDot = FixedMult(nX, dX) + FixedMult(nY, dY) + FixedMult(nZ, dZ);
            LightDot /= (int)sqrt(FixedToInt(LightDot));
            LightDot = (LightDot + 65536) >> 1;

            // Calculate the cone intensity:
            ConeIntensity = FixedDev(ViewDot - CosTable[Penumbra], CosTable[Umbra] - CosTable[Penumbra]);
            if (ConeIntensity < 0) ConeIntensity = -ConeIntensity;
            
            // Calculate attenuation:
            Attenuation = FixedMult(FixedMult(Intensity, LightDot), ConeIntensity);
            
            // Calculate color components:
            Cr = FixedToInt(Attenuation * ColorR);
            Cg = FixedToInt(Attenuation * ColorG);
            Cb = FixedToInt(Attenuation * ColorB);
                
            // Add to the vertex:
            PList[poly].UVI_Info[vtx].R += Cr;
            PList[poly].UVI_Info[vtx].G += Cg;
            PList[poly].UVI_Info[vtx].B += Cb;
            if (Attenuation < 0)
                {
                PList[poly].UVI_Info[vtx].R = 255;
                PList[poly].UVI_Info[vtx].G = 0;
                PList[poly].UVI_Info[vtx].B = 0;
                 }           
                
            // And keep in range of 0-255:
            if (PList[poly].UVI_Info[vtx].R > 255) PList[poly].UVI_Info[vtx].R = 255;
            if (PList[poly].UVI_Info[vtx].G > 255) PList[poly].UVI_Info[vtx].G = 255;
            if (PList[poly].UVI_Info[vtx].B > 255) PList[poly].UVI_Info[vtx].B = 255;
             }
         }
*/
