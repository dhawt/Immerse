/****************************************************************\
 
 iMMERSE Engine
 (C) 1999 No Tears Shed Software
 All rights reserved
 
 Filename: IMR_Pipeline.hpp
 Description: Graphics pipeline.
 
\****************************************************************/
#include "IMR_Pipeline.hpp"

/***************************************************************************\
  Initializes memory for the pipeline.
  Returns IMR_OK if succesfull, otherwise returns an error.
\***************************************************************************/
int IMR_Pipeline::Init(int MaxVerts, int MaxPolys, int MaxLights)
{
// Init memory:
Vertices = new IMR_3DPoint[MaxVerts];
Polygons = new IMR_Polygon[MaxPolys];
DrawPolyList =(IMR_Polygon **)malloc(sizeof(IMR_Polygon *) * MaxPolys);

// Set maximum number of lights:
if (MaxLights > IMR_PIPE_MAX_LIGHTS)
    Max_Lights = IMR_PIPE_MAX_LIGHTS;
else
    Max_Lights = MaxLights;

// Check if we couldn't allocate the memory:
if (!Vertices) Max_Vertices = 0;
else Max_Vertices = MaxVerts;
if (!Polygons || !DrawPolyList) Max_Polygons = 0;
else Max_Polygons = MaxPolys;

// Return an error if we couldn't allocate memory:
if (!Vertices || !Polygons || !DrawPolyList)
    {
    IMR_LogMsg(__LINE__, __FILE__, "IMR_Pipeline::Init(): Out of memory! (%d,%d,%d)", MaxVerts, MaxPolys, MaxLights);
    return IMRERR_OUTOFMEM;
     }

// Reset everything:
Flags.ShouldQuit = 0;
Flags.IsDrawing = 0;

// Now return ok:
return IMR_OK;
 }

/***************************************************************************\
  Resets the pipeline.  Frees all associated memory,
\***************************************************************************/
void IMR_Pipeline::Reset(void)
{
// Signal that we want to stop drawing:
Flags.ShouldQuit = 1;

// Wait for the drawing thread to finish:
while (Flags.IsDrawing)
    {
    // Wait...
     };

// Free memory:
delete [] Vertices;
delete [] Polygons;
delete [] DrawPolyList;

// Reset stuff:
Max_Vertices = Max_Polygons = 0;
Flags.ShouldQuit = 0;
Flags.IsDrawing = 0;
 }

/***************************************************************************\
  Adds the specified model to the list in the specified position at the
  specified attitude.
  Notes: Protected member function.
  Returns: IMR_OK if successful, otherwise an error.
\***************************************************************************/
int IMR_Pipeline::Add_Model(IMR_Model &Mdl, IMR_3DPoint &Pos, IMR_Attitude &Atd)
{
IMR_Matrix Transform;

// Setup the transformation matrix:
Transform.Rotate(Atd.X, Atd.Y, Atd.Z);

// And add the model:
return Add_Model(Mdl, Pos, Transform);
 }

/***************************************************************************\
  Adds the specified model to the list and transforms the vertices using
  the specified matrix.
  Notes: Protected member function.
  Returns: IMR_OK if successful, otherwise an error.
\***************************************************************************/
int IMR_Pipeline::Add_Model(IMR_Model &Mdl, IMR_3DPoint &Pos, IMR_Matrix &Transform)
{
int FirstVtx, polyindx;
int vtx, poly, index, tmp;

// Save an index to the first vertex from this model in the list:
FirstVtx = Num_Vertices;

// Now make space for more vertices:
tmp = Num_Vertices + Mdl.Num_Vertices;

// If we have don't have enough room for the model, just don't add it:
if (tmp > Max_Vertices)
    {
    IMR_LogMsg(__LINE__, __FILE__, "IMR_Pipeline::Add_Model(): (NONFATAL) Vertex list space overflow!");
    return IMRERR_NONFATAL_SPACEOVERFLOW;
     }
else 
    Num_Vertices = tmp;

// Do a quick hack to see if the model is a skybox...
int isSkybox = Mdl.Polygons[0].Flags.Skybox;

// Transform all the vertices in the model and add them to our list:
for (vtx = 0, index = FirstVtx; vtx < Mdl.Num_Vertices; vtx ++, index ++)
    {
    Vertices[index] = Mdl.Vertices[vtx];
    Vertices[index].LocalToActive();
    Vertices[index].Transform(Transform);
    if (!isSkybox) Vertices[index] += Pos;      // Don't transform if this is a skybox
    Vertices[index].ActiveToWorld();
     }

// Now add all the polys to our list:
for (poly = 0; poly < Mdl.Num_Polygons; poly ++)
    {
    // One more poly in the list:
    ++ Num_Polygons;
    
    // If we have too many, bail:
    if (Num_Polygons > Max_Polygons)
        {
        Num_Polygons = Max_Polygons;
        return IMR_OK; // IMRERR_NONFATAL_SPACEOVERFLOW
         }
    
    // Copy the poly:
    polyindx = Num_Polygons - 1;
    Polygons[polyindx] = Mdl.Polygons[poly];
    
    // Setup lighting and vertices:
    for (vtx = 0; vtx < Polygons[polyindx].Num_Verts; vtx ++)
        {
        Polygons[polyindx].Vtx_List[vtx] = &Vertices[Mdl.Polygons[poly].Vtx_Index[vtx] + FirstVtx];
        Polygons[polyindx].UVI_Info[vtx].R = 0.0;
        Polygons[polyindx].UVI_Info[vtx].G = 0.0;
        Polygons[polyindx].UVI_Info[vtx].B = 0.0;
         }

    // Setup flags and normals:
    Polygons[polyindx].Normal_Index =  Mdl.Polygons[poly].Normal_Index + FirstVtx;
    Polygons[polyindx].Normal =  &Vertices[Polygons[polyindx].Normal_Index];
    Polygons[polyindx].Flags.Visible = 1;
    Polygons[polyindx].Flags.Culled = 0;
     }

// And return ok:
return IMR_OK;
 }

/***************************************************************************\
  Adds the specified object to the list.
  Notes: Protected member function.
  Returns: True if successful, false otherwise.
\***************************************************************************/
int IMR_Pipeline::Add_Object(IMR_Object &Obj)
{
int index;
IMR_Light *TmpLight;
IMR_Model *TmpModel;
IMR_Object *TmpChild;

// Add the lights to the list:
for (index = 0; index < Obj.Get_Num_Lights(); index ++)
    {
    // Get a pointer to the light and make sure it exists:
    if (!(TmpLight = Obj.Get_Light(index))) break;
    
    // One more light in the list:
    ++ Num_Lights;
    
    // If we have too many, stop adding lights:
    if (Num_Lights > Max_Lights)
        {
        Num_Lights = Max_Lights;
        break;
         }
    
    // Add the light to the list:
    Lights[Num_Lights - 1] = TmpLight;
    
    // Now find the world pos and direction of the light:
    if (TmpLight->Get_Type() == IMR_LIGHT_POINT || 
        TmpLight->Get_Type() == IMR_LIGHT_SPOT)
        {       
        TmpLight->Get_WorldPos() = TmpLight->Get_Position() + Obj.Get_GlobalPos();
         }
    if (TmpLight->Get_Type() == IMR_LIGHT_CELESTIAL ||
        TmpLight->Get_Type() == IMR_LIGHT_SPOT)
        {
        TmpLight->Get_WorldDirection() = TmpLight->Get_Direction();
        TmpLight->Get_WorldDirection().Transform(Obj.Get_RotMatrix());
         }
     }

// Now add the model to the list (if there is one):
if (TmpModel = Obj.Get_Model())
    Add_Model(*TmpModel, Obj.Get_GlobalPos(), Obj.Get_RotMatrix());

// Now add all the children objects to the list:
for (index = 0; index < Obj.Get_Num_Children(); index ++)
    {
    if (TmpChild = Obj.Get_Child(index)) Add_Object(*TmpChild);
     };

// And return ok:
return IMR_OK;
 }

/***************************************************************************\
  Sets up the pipeline for the next frame.
  Returns IMR_OK.
\***************************************************************************/
int IMR_Pipeline::SetupFrame(IMR_Camera &Cam, IMR_Renderer &Rend)
{
// Setup the camera and rend buffer:
CurrCamera = &Cam;
CurrRenderer = &Rend;

// Reset the lists:
Num_Vertices = Num_Polygons = Num_Lights = 0;

// Reset debug info:
#ifdef IMR_DEBUG
    PolysCulled = 0;
#endif

// And return ok:
return IMR_OK;
 }

/***************************************************************************\
  Adds the specified object and all it's children and lights to the 
  pipeline lists.  
\***************************************************************************/
int IMR_Pipeline::Add_Geometries(IMR_Object &Obj)
{
return Add_Object(Obj);
 }

/***************************************************************************\
  Cycles through each polygon in the list and lights it.
\***************************************************************************/
int IMR_Pipeline::Illuminate(void)
{
int index;

// Calculate the centroids for each poly:
for (index = 0; index < Num_Polygons; index ++)
    Polygons[index].Find_Centroid();

// Loop through each light and illuminate the polygon list:
for (index = 0; index < Num_Lights; index ++)
    Lights[index]->IlluminatePolyList(Polygons, Num_Polygons);

// Return ok:
return IMR_OK;
 }

/***************************************************************************\
  Performs world->camera pos transformations.
  Returns IMR_OK.
\***************************************************************************/
int IMR_Pipeline::Transform(void)
{
IMR_Matrix Rot;
IMR_Attitude Atd = CurrCamera->Get_Atd();

Atd.X = -Atd.X;
Atd.Y = -Atd.Y;
Atd.Z = -Atd.Z;
Atd.Fix_Ang();
Rot.ZYXRotate(Atd.X, Atd.Y, Atd.Z);
for (int index = 0; index < Num_Vertices; index ++)
    {
    Vertices[index].WorldToActive();
    if (!Vertices[index].IsSkybox) Vertices[index] -= CurrCamera->Get_Pos();
    Vertices[index].Transform(Rot);
    Vertices[index].ActiveToCamera();
     }
return IMR_OK;
 }

/***************************************************************************\
  Culls invisible polygons. (Uses camera coords)
  Returns IMR_OK.
\***************************************************************************/
int IMR_Pipeline::Cull(void)
{
float Comp[5], DotProduct;
float FOV_Width;
IMR_3DPoint View, Normal_Trans;

PolysCulled = Num_Polygons;

// Loop through all the polygons in the list:
for (int poly = 0; poly < Num_Polygons; ++ poly)
    {
    // Reset the culled flag:
    Polygons[poly].Flags.Culled = 0;

    // Check against the near clip plane:
    if (!Polygons[poly].Flags.Skybox)
        {
        Polygons[poly].Flags.Visible = 0;
        if (Polygons[poly].Vtx_List[0]->cZ > CurrCamera->Lens_Get_Near()) Polygons[poly].Flags.Visible = 1;
        if (Polygons[poly].Vtx_List[1]->cZ > CurrCamera->Lens_Get_Near()) Polygons[poly].Flags.Visible = 1;
        if (Polygons[poly].Vtx_List[2]->cZ > CurrCamera->Lens_Get_Near()) Polygons[poly].Flags.Visible = 1;
        if (Polygons[poly].Num_Verts == 4)
            if (Polygons[poly].Vtx_List[3]->cZ > CurrCamera->Lens_Get_Near()) Polygons[poly].Flags.Visible = 1;
        if (!Polygons[poly].Flags.Visible) { Polygons[poly].Flags.Culled = 1; continue; }
         }
              
    // Check against the far clip plane:
    if (!Polygons[poly].Flags.Skybox)
        {
        Polygons[poly].Flags.Visible = 0;
        if (Polygons[poly].Vtx_List[0]->cZ < CurrCamera->Lens_Get_Far()) Polygons[poly].Flags.Visible = 1;
        if (Polygons[poly].Vtx_List[1]->cZ < CurrCamera->Lens_Get_Far()) Polygons[poly].Flags.Visible = 1;
        if (Polygons[poly].Vtx_List[2]->cZ < CurrCamera->Lens_Get_Far()) Polygons[poly].Flags.Visible = 1;
        if (Polygons[poly].Num_Verts == 4)
            if (Polygons[poly].Vtx_List[3]->cZ < CurrCamera->Lens_Get_Far()) Polygons[poly].Flags.Visible = 1;
        if (!Polygons[poly].Flags.Visible) { Polygons[poly].Flags.Culled = 1; continue; }
         }
         
    // Check the dot-product (if the poly is not two-sided):
    if (!Polygons[poly].Flags.TwoSided && !Polygons[poly].Flags.Skybox)
        {
        Polygons[poly].Normal->CameraToActive();
        Polygons[poly].Vtx_List[0]->CameraToActive();
        Normal_Trans = *Polygons[poly].Normal - *Polygons[poly].Vtx_List[0];
        DotProduct = Normal_Trans.Dot_Product(*Polygons[poly].Vtx_List[0]);
        if (DotProduct > 0)
            {
            Polygons[poly].Flags.Culled = 1;
            continue;
             }
         }
    
    // Calculate max values:
    FOV_Width = CurrRenderer->Get_WindowWidth() / CurrCamera->Lens_Get_Zoom();
    Comp[0] = Polygons[poly].Vtx_List[0]->cZ * FOV_Width;
    Comp[1] = Polygons[poly].Vtx_List[1]->cZ * FOV_Width;
    Comp[2] = Polygons[poly].Vtx_List[2]->cZ * FOV_Width;
    if (Polygons[poly].Num_Verts == 4)
        Comp[3] = Polygons[poly].Vtx_List[3]->cZ * FOV_Width;

    // Check against the X clip planes:
    if (!Polygons[poly].Flags.Skybox)
        {
        Polygons[poly].Flags.Visible = 0;
        if ((Polygons[poly].Vtx_List[0]->cX > -Comp[0]) && 
            (Polygons[poly].Vtx_List[0]->cX < Comp[0])) Polygons[poly].Flags.Visible = 1;
        if ((Polygons[poly].Vtx_List[1]->cX > -Comp[1]) && 
            (Polygons[poly].Vtx_List[1]->cX < Comp[1])) Polygons[poly].Flags.Visible = 1;
        if ((Polygons[poly].Vtx_List[2]->cX > -Comp[2]) && 
            (Polygons[poly].Vtx_List[2]->cX < Comp[2])) Polygons[poly].Flags.Visible = 1;
        if (Polygons[poly].Num_Verts == 4)
            if ((Polygons[poly].Vtx_List[3]->cX > -Comp[3]) && 
                (Polygons[poly].Vtx_List[3]->cX < Comp[3])) Polygons[poly].Flags.Visible = 1;
        if (!Polygons[poly].Flags.Visible) { Polygons[poly].Flags.Culled = 1; continue; }
         }
         
    // Check against the Y clip planes:
    if (!Polygons[poly].Flags.Skybox)
        {
        Polygons[poly].Flags.Visible = 0;
        if ((Polygons[poly].Vtx_List[0]->cY > -Comp[0]) && 
            (Polygons[poly].Vtx_List[0]->cY < Comp[0])) Polygons[poly].Flags.Visible = 1;
        if ((Polygons[poly].Vtx_List[1]->cY > -Comp[1]) && 
            (Polygons[poly].Vtx_List[1]->cY < Comp[1])) Polygons[poly].Flags.Visible = 1;
        if ((Polygons[poly].Vtx_List[2]->cY > -Comp[2]) && 
            (Polygons[poly].Vtx_List[2]->cY < Comp[2])) Polygons[poly].Flags.Visible = 1;
        if (Polygons[poly].Num_Verts == 4)
            if ((Polygons[poly].Vtx_List[3]->cY > -Comp[3]) && 
                (Polygons[poly].Vtx_List[3]->cY < Comp[3])) Polygons[poly].Flags.Visible = 1;
        if (!Polygons[poly].Flags.Visible) { Polygons[poly].Flags.Culled = 1; continue; }
         }
         
    PolysCulled --;
     }

// Return ok:
return IMR_OK;
 }

/***************************************************************************\
  Clips and projects the polygons in the list.
  Returns IMR_OK.
\***************************************************************************/
int IMR_Pipeline::ClipAndProject(void)
{
int poly, vtx, StartVtx, EndVtx;
float DeltaR, DeltaG, DeltaB,
      DeltaU, DeltaV, DeltaNear, 
      DeltaX, DeltaY, DeltaZ, T;

// Loop through each poly:
for (poly = 0; poly < Num_Polygons; poly ++)
    {
    // If this poly has been culled, go on to the next:
    if (Polygons[poly].Flags.Culled) continue;
    
    // Reset number of projected vertices:
    Polygons[poly].Num_Verts_Proj = 0;
    
    // Init pointer to last vertex in poly:
    StartVtx = Polygons[poly].Num_Verts - 1;
    
    // Loop through all the edges in the panel and clip them using the S&H algorithm:
    for (EndVtx = 0; EndVtx < Polygons[poly].Num_Verts; EndVtx ++)
        {
        // Check if the edge starts inside the clipping frustrum (or it's a skybox):
        if (Polygons[poly].Vtx_List[StartVtx]->cZ >= CurrCamera->Lens_Get_Near())
            {
            // If edge is entirely inside clipping frustrum, output unchanged vertex:
            if (Polygons[poly].Vtx_List[EndVtx]->cZ >= CurrCamera->Lens_Get_Near())
                {
                Polygons[poly].Vtx_Projected[Polygons[poly].Num_Verts_Proj] = *Polygons[poly].Vtx_List[EndVtx];
                Polygons[poly].Vtx_Projected[Polygons[poly].Num_Verts_Proj] = Polygons[poly].UVI_Info[EndVtx];
                
                // One more vertex:
                ++ Polygons[poly].Num_Verts_Proj;
                 }

            // If the start of the edge is outside the clipping frustrum, create a vertex
            // where it crosses the near clipping plane:
            else
                {
                // Find deltas:
                DeltaNear = CurrCamera->Lens_Get_Near() - Polygons[poly].Vtx_List[StartVtx]->cZ;
                DeltaX = Polygons[poly].Vtx_List[EndVtx]->cX - Polygons[poly].Vtx_List[StartVtx]->cX;
                DeltaY = Polygons[poly].Vtx_List[EndVtx]->cY - Polygons[poly].Vtx_List[StartVtx]->cY;
                DeltaZ = Polygons[poly].Vtx_List[EndVtx]->cZ - Polygons[poly].Vtx_List[StartVtx]->cZ;
                DeltaU = Polygons[poly].UVI_Info[EndVtx].U - Polygons[poly].UVI_Info[StartVtx].U;
                DeltaV = Polygons[poly].UVI_Info[EndVtx].V - Polygons[poly].UVI_Info[StartVtx].V;
                DeltaR = Polygons[poly].UVI_Info[EndVtx].R - Polygons[poly].UVI_Info[StartVtx].R;
                DeltaG = Polygons[poly].UVI_Info[EndVtx].G - Polygons[poly].UVI_Info[StartVtx].G;
                DeltaB = Polygons[poly].UVI_Info[EndVtx].B - Polygons[poly].UVI_Info[StartVtx].B;
               
                // Find parametric form of the edge:
                if (DeltaZ)
                    T = DeltaNear / DeltaZ;
                else 
                    T = 1;
                
                // Create clipped clipped vertex:
                Polygons[poly].Vtx_Projected[Polygons[poly].Num_Verts_Proj].X = Polygons[poly].Vtx_List[StartVtx]->cX + (DeltaX * T);
                Polygons[poly].Vtx_Projected[Polygons[poly].Num_Verts_Proj].Y = Polygons[poly].Vtx_List[StartVtx]->cY + (DeltaY * T);
                Polygons[poly].Vtx_Projected[Polygons[poly].Num_Verts_Proj].Z = CurrCamera->Lens_Get_Near();
                Polygons[poly].Vtx_Projected[Polygons[poly].Num_Verts_Proj].pU = Polygons[poly].UVI_Info[StartVtx].U + (DeltaU * T);
                Polygons[poly].Vtx_Projected[Polygons[poly].Num_Verts_Proj].pV = Polygons[poly].UVI_Info[StartVtx].V + (DeltaV * T);
                Polygons[poly].Vtx_Projected[Polygons[poly].Num_Verts_Proj].pR = (Polygons[poly].UVI_Info[StartVtx].R + (DeltaR * T)) * 255;
                Polygons[poly].Vtx_Projected[Polygons[poly].Num_Verts_Proj].pG = (Polygons[poly].UVI_Info[StartVtx].G + (DeltaG * T)) * 255;
                Polygons[poly].Vtx_Projected[Polygons[poly].Num_Verts_Proj].pB = (Polygons[poly].UVI_Info[StartVtx].B + (DeltaB * T)) * 255;

                // One more vertex:
                ++ Polygons[poly].Num_Verts_Proj;
                 }
             }

        // The edge starts outside the clipping frustrum:
        else
            {
            // If the edge is entering the clipping frustrum, output a clipped vertex as well
            // as the end vertex:
            if (Polygons[poly].Vtx_List[EndVtx]->cZ >= CurrCamera->Lens_Get_Near())
                {
                // Find deltas:
                DeltaNear = CurrCamera->Lens_Get_Near() - Polygons[poly].Vtx_List[StartVtx]->cZ;
                DeltaX = Polygons[poly].Vtx_List[EndVtx]->cX - Polygons[poly].Vtx_List[StartVtx]->cX;
                DeltaY = Polygons[poly].Vtx_List[EndVtx]->cY - Polygons[poly].Vtx_List[StartVtx]->cY;
                DeltaZ = Polygons[poly].Vtx_List[EndVtx]->cZ - Polygons[poly].Vtx_List[StartVtx]->cZ;
                DeltaU = Polygons[poly].UVI_Info[EndVtx].U - Polygons[poly].UVI_Info[StartVtx].U;
                DeltaV = Polygons[poly].UVI_Info[EndVtx].V - Polygons[poly].UVI_Info[StartVtx].V;
                DeltaR = Polygons[poly].UVI_Info[EndVtx].R - Polygons[poly].UVI_Info[StartVtx].R;
                DeltaG = Polygons[poly].UVI_Info[EndVtx].G - Polygons[poly].UVI_Info[StartVtx].G;
                DeltaB = Polygons[poly].UVI_Info[EndVtx].B - Polygons[poly].UVI_Info[StartVtx].B;
               
                // Find parametric form of the edge:
                if (DeltaZ)
                    T = DeltaNear / DeltaZ;
                else 
                    T = 1;
                
                // Create clipped clipped vertex:
                Polygons[poly].Vtx_Projected[Polygons[poly].Num_Verts_Proj].X = Polygons[poly].Vtx_List[StartVtx]->cX + (DeltaX * T);
                Polygons[poly].Vtx_Projected[Polygons[poly].Num_Verts_Proj].Y = Polygons[poly].Vtx_List[StartVtx]->cY + (DeltaY * T);
                Polygons[poly].Vtx_Projected[Polygons[poly].Num_Verts_Proj].Z = CurrCamera->Lens_Get_Near();
                Polygons[poly].Vtx_Projected[Polygons[poly].Num_Verts_Proj].pU = Polygons[poly].UVI_Info[StartVtx].U + (DeltaU * T);
                Polygons[poly].Vtx_Projected[Polygons[poly].Num_Verts_Proj].pV = Polygons[poly].UVI_Info[StartVtx].V + (DeltaV * T);
                Polygons[poly].Vtx_Projected[Polygons[poly].Num_Verts_Proj].pR = (Polygons[poly].UVI_Info[StartVtx].R + (DeltaR * T)) * 255;
                Polygons[poly].Vtx_Projected[Polygons[poly].Num_Verts_Proj].pG = (Polygons[poly].UVI_Info[StartVtx].G + (DeltaG * T)) * 255;
                Polygons[poly].Vtx_Projected[Polygons[poly].Num_Verts_Proj].pB = (Polygons[poly].UVI_Info[StartVtx].B + (DeltaB * T)) * 255;

                if (DeltaNear)
                    {
                    // One more vertex:
                    ++ Polygons[poly].Num_Verts_Proj;
                
                    // Now add the end vertex unchanged:
                    Polygons[poly].Vtx_Projected[Polygons[poly].Num_Verts_Proj] = *Polygons[poly].Vtx_List[EndVtx];
                    Polygons[poly].Vtx_Projected[Polygons[poly].Num_Verts_Proj] = Polygons[poly].UVI_Info[EndVtx];
                
                    // One more vertex:
                    ++ Polygons[poly].Num_Verts_Proj;
                     }
                 }
             }
        
        // Project the vertices:
        for (vtx = 0; vtx < Polygons[poly].Num_Verts_Proj; vtx ++)
            {
            Polygons[poly].Vtx_Projected[vtx].Project(CurrCamera->Lens_Get_Zoom(),
                                                      CurrRenderer->Get_WindowXCenter(),
                                                      CurrRenderer->Get_WindowYCenter());
            if (Polygons[poly].Flags.Skybox)
                {
                //Polygons[poly].Vtx_Projected[vtx].Z = CurrCamera->Lens_Get_Far() - 100;
                //Polygons[poly].Vtx_Projected[vtx].pW = 1 - Polygons[poly].Vtx_Projected[vtx].Z;
                Polygons[poly].Vtx_Projected[vtx].pR = 255;
                Polygons[poly].Vtx_Projected[vtx].pG = 255;
                Polygons[poly].Vtx_Projected[vtx].pB = 255;
                 }
             };

        // Set next start vertex:
        StartVtx = EndVtx;
         }
     }

// Return ok:
return IMR_OK;
 }

/***************************************************************************\
  Draws everything in the frame.
  Returns IMR_OK if successful, otherwise returns an error.
\***************************************************************************/
int IMR_Pipeline::DrawFrame(void)
{
int err, poly, NumDrawPolys;

// Make sure we have a camera, rend buffer, and renderer:
if (!CurrCamera || !CurrRenderer) 
    {
    IMR_LogMsg(__LINE__, __FILE__, "IMR_Pipeline::Draw_Frame(): Missing interface!");
    return IMRERR_GENERIC;
     }

// Init the renderer for this batch of polys:
err = CurrRenderer->Begin_Raster_Batch(*CurrCamera); if (IMR_ISNOTOK(err)) return err;

// Now create a list of pointers to the polygons:
NumDrawPolys = 0;
for (poly = 0; poly < Num_Polygons; poly ++)
    if (!Polygons[poly].Flags.Culled)
        {
        DrawPolyList[NumDrawPolys] = &Polygons[poly];
        if (++ NumDrawPolys >= (Max_Polygons - 1)) break;
         }

// --- Sort list here ---

// And draw 'em all:
//err = CurrRenderer->Draw_PolyBatch(DrawPolyList, NumDrawPolys);
//if (IMR_ISNOTOK(err)) return err;

for (poly = 0; poly < Num_Polygons; poly ++)
    {
    if (!Polygons[poly].Flags.Culled)
        {
        // Draw the polygon:
        err = CurrRenderer->Draw_TexturedLit_Polygon(Polygons[poly]); 
        if (IMR_ISNOTOK(err)) return err;
         }
     }


// End this raster batch:
err = CurrRenderer->End_Raster_Batch(); if (IMR_ISNOTOK(err)) return err;

// And return ok:
return IMR_OK;
 }

/***************************************************************************\
  Draws the frame asynchroneously.
\***************************************************************************/
static DWORD WINAPI IMR_Pipeline::Async_DrawFrame(LPVOID Arg)
{
int err, poly, NumDrawPolys;
IMR_Pipeline *Pipe = (IMR_Pipeline *)Arg;

// Make sure we have the neccesary interfaces:
if (!Pipe || !Pipe->CurrRenderer || !Pipe->CurrCamera) 
    {
    IMR_LogMsg(__LINE__, __FILE__, "IMR_Pipeline::Async_DrawFrame(): Missing interface!");
    return IMRERR_GENERIC;
     }

// Set the flag that we are drawing:
Pipe->Flags.IsDrawing = 1;

// Init the renderer for this batch of polys:
err = Pipe->CurrRenderer->Begin_Raster_Batch(*Pipe->CurrCamera); if (IMR_ISNOTOK(err)) return err;

// Now create a list of pointers to the polygons:
NumDrawPolys = 0;
for (poly = 0; poly < Pipe->Num_Polygons; poly ++)
    if (!Pipe->Polygons[poly].Flags.Culled)
        {
        Pipe->DrawPolyList[NumDrawPolys] = &Pipe->Polygons[poly];
        if (++ NumDrawPolys >= (Pipe->Max_Polygons - 1)) break;
         }

// --- Sort list here ---

// And draw 'em all:
err = Pipe->CurrRenderer->Draw_PolyBatch(Pipe->DrawPolyList, NumDrawPolys);
if (IMR_ISNOTOK(err)) return err;

// Shutdown the renderer:
err = Pipe->CurrRenderer->End_Raster_Batch(); if (IMR_ISNOTOK(err)) return err;

// Set the flag that we are done drawing:
Pipe->Flags.IsDrawing = 0;

// And return ok:
return IMR_OK;
 }

