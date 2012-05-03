/***************************************************************************\
  File: IMR_Geom_Model.cpp
  Description: Model geometry module.
  Author: Daniel Hawthorn
  Modified:
  
\***************************************************************************/
#include "IMR_Geom_Model.hpp"

/***************************************************************************\
  Allocates memory for the vertex and polygon lists.
  **** ALLOCATES ROOM FOR NORMALS! ****
  Returns IMR_OK if successful, otherwise an error.
\***************************************************************************/
int IMR_Model::Init(int NumV, int NumP)
{
Reset();
if (!(Vertices = new IMR_3DPoint[NumV + NumP]))
    {
    IMR_SetErrorText("IMR_Model::Init(): Out of memory! (NumV %d)", sizeof(IMR_3DPoint) * NumV);
    return IMRERR_OUTOFMEM;
     }
if (!(Polygons = new IMR_Polygon[NumP]))
    {
    IMR_SetErrorText("IMR_Model::Init(): Out of memory! (NumP %d)", sizeof(IMR_Polygon) * NumP);
    return IMRERR_OUTOFMEM;
     }
return IMR_OK;
 }

/***************************************************************************\
  Frees all memory associated with this model and resets everything.
\***************************************************************************/
void IMR_Model::Reset(void)
{
for (int poly = 0; poly < Num_Polygons; poly ++) Polygons[poly].Material.Shutdown();
Num_Vertices = Num_Polygons = 0;
delete [] Vertices;
delete [] Polygons;
 }

/***************************************************************************\
  Sets the name of the model.
\***************************************************************************/
void IMR_Model::Set_Name(char *NewName)
{
if (strlen(NewName) > 8)
    {
    memcpy((void *)Name, (void *)NewName, 8);
    Name[8] = '/0';
     }
else
    strcpy(Name, NewName);
 }

/***************************************************************************\
  Sets up the model for use.  Fills in all pointers, calculates normals, 
  and so on.  Also searches the lists (if they are not NULL) for the 
  textures for the polys.
  Returns IMR_OK if successful, otherwise an error.
\***************************************************************************/
int IMR_Model::Setup(IMR_NamedList<IMR_Texture> *GlbTex, IMR_NamedList<IMR_Texture> *LocTex)
{
int poly, vtx, err;
IMR_Texture *Temp;

// Loop through all the polygons:
for (poly = 0; poly < Num_Polygons; poly ++)
    {
    // Fill in all the vertex pointers and set active coords:
    for (vtx = 0; vtx < Polygons[poly].Num_Verts; vtx ++)
        {
        Polygons[poly].Vtx_List[vtx] = &Vertices[Polygons[poly].Vtx_Index[vtx]];
        Polygons[poly].Vtx_List[vtx]->LocalToActive();
         }
    
    // Fill in the normal pointer:
    Polygons[poly].Normal = &Vertices[Polygons[poly].Normal_Index];
    Polygons[poly].Normal->IsNormal = 1;
    
    // Calculate the normal, radius, centroid, and recommended lighting model:
    Polygons[poly].Find_Normal();
    Polygons[poly].Find_Radius();
         
    // Search the local list (if there is one) for the texture:
    if (LocTex)
        Temp = LocTex->Get_Item(Polygons[poly].Material.Get_TextureName(), NULL);
    
    // If we haven't found the texture already, search the global list:
    if (!Temp && GlbTex)
        Temp = GlbTex->Get_Item(Polygons[poly].Material.Get_TextureName(), NULL);
    
    // Set the texture for the material:
    Polygons[poly].Material.Set_Texture(Temp);
    
    // Init the lightmap:
    err = Polygons[poly].Material.Init_Lightmap(DX); if (IMR_ISNOTOK(err)) return err;
     }

// And return ok:
return IMR_OK;
 }

/***************************************************************************\
  Shifts the model by the specified ammounts.
  Returns IMR_OK if successful, otherwise an error.
\***************************************************************************/
int IMR_Model::Shift_Pos(int X, int Y, int Z)
{
FIXEDPNT sX = IntToFixed(X),
         sY = IntToFixed(Y),
         sZ = IntToFixed(Z);

// Loop through all the vertices and shift the local coord:
for (int vtx = 0; vtx < Num_Vertices; vtx ++)
    {
    Vertices[vtx].lX += sX;
    Vertices[vtx].lY += sY;
    Vertices[vtx].lZ += sZ;
     }

// And return ok:
return IMR_OK;
 }

/***************************************************************************\
  Makes the model a cube with the specified radius and gives it the 
  specified name.
  Note: Does not setup the model, so a call to Setup() with textures and
        such should be made right after calling this!
  Returns IMR_OK if successful, otherwise an error.
\***************************************************************************/
int IMR_Model::Make_Cube(int Radius, char *Name)
{
FIXEDPNT FRad = IntToFixed(Radius);
int err, poly;

// Setup everything:
err = Init(14, 6); if (IMR_ISNOTOK(err)) return err;

// Set the name of the model:
Set_Name(Name);

// Setup stuff:
Num_Vertices = 14;
Num_Polygons = 6;

// Setup the vertices:
Vertices[0].lX = -FRad;        // Back square
Vertices[0].lY =  FRad;
Vertices[0].lZ =  FRad;
Vertices[1].lX =  FRad;
Vertices[1].lY =  FRad;
Vertices[1].lZ =  FRad;
Vertices[2].lX =  FRad;
Vertices[2].lY = -FRad;
Vertices[2].lZ =  FRad;
Vertices[3].lX = -FRad;
Vertices[3].lY = -FRad;
Vertices[3].lZ =  FRad;
Vertices[4].lX = -FRad;        // Front square
Vertices[4].lY =  FRad;
Vertices[4].lZ = -FRad;
Vertices[5].lX =  FRad;
Vertices[5].lY =  FRad;
Vertices[5].lZ = -FRad;
Vertices[6].lX =  FRad;
Vertices[6].lY = -FRad;
Vertices[6].lZ = -FRad;
Vertices[7].lX = -FRad;
Vertices[7].lY = -FRad;
Vertices[7].lZ = -FRad;

// Now setup the polys:
Polygons[0].Num_Verts = 4;      // Back face
Polygons[0].Vtx_Index[0] = 1;
Polygons[0].Vtx_Index[1] = 0;
Polygons[0].Vtx_Index[2] = 3;
Polygons[0].Vtx_Index[3] = 2;
Polygons[1].Num_Verts = 4;      // Front face
Polygons[1].Vtx_Index[0] = 4;
Polygons[1].Vtx_Index[1] = 5;
Polygons[1].Vtx_Index[2] = 6;
Polygons[1].Vtx_Index[3] = 7;
Polygons[2].Num_Verts = 4;      // Left face
Polygons[2].Vtx_Index[0] = 0;
Polygons[2].Vtx_Index[1] = 4;
Polygons[2].Vtx_Index[2] = 7;
Polygons[2].Vtx_Index[3] = 3;
Polygons[3].Num_Verts = 4;      // Right face
Polygons[3].Vtx_Index[0] = 5;
Polygons[3].Vtx_Index[1] = 1;
Polygons[3].Vtx_Index[2] = 2;
Polygons[3].Vtx_Index[3] = 6;
Polygons[4].Num_Verts = 4;      // Top face
Polygons[4].Vtx_Index[0] = 0;
Polygons[4].Vtx_Index[1] = 1;
Polygons[4].Vtx_Index[2] = 5;
Polygons[4].Vtx_Index[3] = 4;
Polygons[5].Num_Verts = 4;      // Bottom face
Polygons[5].Vtx_Index[0] = 7;
Polygons[5].Vtx_Index[1] = 6;
Polygons[5].Vtx_Index[2] = 2;
Polygons[5].Vtx_Index[3] = 3;

// Setup normal indices:
for (poly = 0; poly < 6; poly ++)
    {
    Polygons[poly].Flags.TwoSided = 0;
    Polygons[poly].Flags.Transparent = 0;
    Polygons[poly].Normal_Index = 8 + poly;
    Polygons[poly].Normal = &Vertices[Polygons[poly].Normal_Index];
     }

// And return ok:
return IMR_OK;
 }

/***************************************************************************\
  Makes the model a rectangle with the specified dimensions.
  Note: Does not setup the model, so a call to Setup() with textures and
        such should be made right after calling this!
  Returns IMR_OK if successful, otherwise an error.
\***************************************************************************/
int IMR_Model::Make_Rectangle(int Width, int Height, int Depth, char *Name)
{
int err, poly;
FIXEDPNT XDim = IntToFixed(Width / 2);
FIXEDPNT YDim = IntToFixed(Height / 2);
FIXEDPNT ZDim = IntToFixed(Depth / 2);

// Setup everything:
err = Init(14, 6); if (IMR_ISNOTOK(err)) return err;

// Set the name of the model:
Set_Name(Name);

// Setup stuff:
Num_Vertices = 14;
Num_Polygons = 6;

// Setup the vertices:
Vertices[0].lX = -XDim;        // Back square
Vertices[0].lY =  YDim;
Vertices[0].lZ =  ZDim;
Vertices[1].lX =  XDim;
Vertices[1].lY =  YDim;
Vertices[1].lZ =  ZDim;
Vertices[2].lX =  XDim;
Vertices[2].lY = -YDim;
Vertices[2].lZ =  ZDim;
Vertices[3].lX = -XDim;
Vertices[3].lY = -YDim;
Vertices[3].lZ =  ZDim;
Vertices[4].lX = -XDim;        // Front square
Vertices[4].lY =  YDim;
Vertices[4].lZ = -ZDim;
Vertices[5].lX =  XDim;
Vertices[5].lY =  YDim;
Vertices[5].lZ = -ZDim;
Vertices[6].lX =  XDim;
Vertices[6].lY = -YDim;
Vertices[6].lZ = -ZDim;
Vertices[7].lX = -XDim;
Vertices[7].lY = -YDim;
Vertices[7].lZ = -ZDim;

// Now setup the polys:
Polygons[0].Num_Verts = 4;      // Back face
Polygons[0].Vtx_Index[0] = 1;
Polygons[0].Vtx_Index[1] = 0;
Polygons[0].Vtx_Index[2] = 3;
Polygons[0].Vtx_Index[3] = 2;
Polygons[1].Num_Verts = 4;      // Front face
Polygons[1].Vtx_Index[0] = 4;
Polygons[1].Vtx_Index[1] = 5;
Polygons[1].Vtx_Index[2] = 6;
Polygons[1].Vtx_Index[3] = 7;
Polygons[2].Num_Verts = 4;      // Left face
Polygons[2].Vtx_Index[0] = 0;
Polygons[2].Vtx_Index[1] = 4;
Polygons[2].Vtx_Index[2] = 7;
Polygons[2].Vtx_Index[3] = 3;
Polygons[3].Num_Verts = 4;      // Right face
Polygons[3].Vtx_Index[0] = 5;
Polygons[3].Vtx_Index[1] = 1;
Polygons[3].Vtx_Index[2] = 2;
Polygons[3].Vtx_Index[3] = 6;
Polygons[4].Num_Verts = 4;      // Top face
Polygons[4].Vtx_Index[0] = 0;
Polygons[4].Vtx_Index[1] = 1;
Polygons[4].Vtx_Index[2] = 5;
Polygons[4].Vtx_Index[3] = 4;
Polygons[5].Num_Verts = 4;      // Bottom face
Polygons[5].Vtx_Index[0] = 7;
Polygons[5].Vtx_Index[1] = 6;
Polygons[5].Vtx_Index[2] = 2;
Polygons[5].Vtx_Index[3] = 3;

// Setup normal indices:
for (poly = 0; poly < 6; poly ++)
    {
    Polygons[poly].Flags.TwoSided = 0;
    Polygons[poly].Flags.Transparent = 0;
    Polygons[poly].Normal_Index = 8 + poly;
    Polygons[poly].Normal = &Vertices[Polygons[poly].Normal_Index];
     }

// And return ok:
return IMR_OK;
 }

/***************************************************************************\
  Makes the model a pyramid with the specified dimensions.
  Note: Does not setup the model, so a call to Setup() with textures and
        such should be made right after calling this!
  Returns IMR_OK if successful, otherwise an error.
\***************************************************************************/
int IMR_Model::Make_Pyramid(int Width, int Height, int Depth, char *Name)
{
int err, poly;
FIXEDPNT XDim = IntToFixed(Width / 2);
FIXEDPNT YDim = IntToFixed(Height / 2);
FIXEDPNT ZDim = IntToFixed(Depth / 2);

// Setup everything:
err = Init(10, 5); if (IMR_ISNOTOK(err)) return err;

// Set the name of the model:
Set_Name(Name);

// Setup stuff:
Num_Vertices = 10;
Num_Polygons = 5;

// Setup the vertices:
Vertices[0].lX = -XDim;        // Base
Vertices[0].lY = -YDim;
Vertices[0].lZ =  ZDim;
Vertices[1].lX =  XDim;
Vertices[1].lY = -YDim;
Vertices[1].lZ =  ZDim;
Vertices[2].lX =  XDim;
Vertices[2].lY = -YDim;
Vertices[2].lZ = -ZDim;
Vertices[3].lX = -XDim;
Vertices[3].lY = -YDim;
Vertices[3].lZ = -ZDim;
Vertices[4].lX =  0;           // Tip
Vertices[4].lY =  YDim;
Vertices[4].lZ =  0;

// Now setup the polys:
Polygons[0].Num_Verts = 4;      // Base
Polygons[0].Vtx_Index[0] = 3;
Polygons[0].Vtx_Index[1] = 2;
Polygons[0].Vtx_Index[2] = 1;
Polygons[0].Vtx_Index[3] = 0;
Polygons[1].Num_Verts = 3;      // Back face
Polygons[1].Vtx_Index[0] = 4;
Polygons[1].Vtx_Index[1] = 0;
Polygons[1].Vtx_Index[2] = 1;
Polygons[2].Num_Verts = 3;      // Front face
Polygons[2].Vtx_Index[0] = 4;
Polygons[2].Vtx_Index[1] = 2;
Polygons[2].Vtx_Index[2] = 3;
Polygons[3].Num_Verts = 3;      // Left face
Polygons[3].Vtx_Index[0] = 4;
Polygons[3].Vtx_Index[1] = 3;
Polygons[3].Vtx_Index[2] = 0;
Polygons[4].Num_Verts = 3;      // Right face
Polygons[4].Vtx_Index[0] = 4;
Polygons[4].Vtx_Index[1] = 1;
Polygons[4].Vtx_Index[2] = 2;

// Setup normal indices:
for (poly = 0; poly < 5; poly ++)
    {
    Polygons[poly].Flags.TwoSided = 0;
    Polygons[poly].Flags.Transparent = 0;
    Polygons[poly].Normal_Index = 5 + poly;
    Polygons[poly].Normal = &Vertices[Polygons[poly].Normal_Index];
     }

// And return ok:
return IMR_OK;
 }

/***************************************************************************\
  Makes the model a wall with the specified dimensions parallel to the X 
  axis.
  Note: Does not setup the model, so a call to Setup() with textures and
        such should be made right after calling this!
  Returns IMR_OK if successful, otherwise an error.
\***************************************************************************/
int IMR_Model::Make_Wall(int Width, int Height, int PSize, char *Name)
{
int err, poly;
FIXEDPNT PanelSize = IntToFixed(PSize);
int NumXSteps, NumYSteps;
FIXEDPNT XStep, YStep,
         XDim = IntToFixed(Width),
         YDim = IntToFixed(Height),
         XOff = -IntToFixed(Width / 2),
         YOff = IntToFixed(Height / 2);

// Calculate step values:
NumXSteps = (XDim / PanelSize);
NumYSteps = (YDim / PanelSize);
XStep = XDim / NumXSteps;
YStep = YDim / NumYSteps;

// Calculate needed ammounts:
int NeededVerts = (NumXSteps + 1) * (NumYSteps + 1);
int NeededPolys = NumXSteps * NumYSteps;

// Setup everything:
err = Init(NeededVerts + NeededPolys, NeededPolys); if (IMR_ISNOTOK(err)) return err;

// Set the name of the model:
Set_Name(Name);

// Setup stuff:
Num_Vertices = NeededVerts + NeededPolys;
Num_Polygons = NeededPolys;

// Setup the vertices:
int Vtx = 0;
for (FIXEDPNT Y = YOff; Y >= YOff - YDim; Y -= YStep)
    for (FIXEDPNT X = XOff; X <= XOff + XDim; X += XStep)
        {
        Vertices[Vtx].lX = X;
        Vertices[Vtx].lY = Y;
        Vertices[Vtx].lZ = 0;
        Vtx ++; 
         }

// Now setup the polys:
int Poly = 0;
for (FIXEDPNT Row = 0; Row < NumYSteps; Row ++)
    for (FIXEDPNT Col = 0; Col < NumXSteps; Col ++)
        {
        Polygons[Poly].Num_Verts = 4;
        Polygons[Poly].Vtx_Index[0] = ((Row) * (NumXSteps + 1)) + (Col);
        Polygons[Poly].Vtx_Index[1] = ((Row) * (NumXSteps + 1)) + (Col + 1);
        Polygons[Poly].Vtx_Index[2] = ((Row + 1) * (NumXSteps + 1)) + (Col + 1);
        Polygons[Poly].Vtx_Index[3] = ((Row + 1) * (NumXSteps + 1)) + (Col);
        Polygons[Poly].Normal_Index = NeededVerts + Poly;
        Polygons[Poly].Normal = &Vertices[Polygons[Poly].Normal_Index];
        Poly ++;
         }

// And return ok:
return IMR_OK;
 }

/***************************************************************************\
  Makes the model a cylinder with the specified dimensions.
  Note: Does not setup the model, so a call to Setup() with textures and
        such should be made right after calling this!
  Returns IMR_OK if successful, otherwise an error.
\***************************************************************************/
int IMR_Model::Make_Cylinder(int Height, int Radius, char *Name)
{
int err, poly, Vtx, Poly, Ang;
FIXEDPNT YDim = IntToFixed(Height / 2);

// Make sure we have tables:
IMR_BuildTables();

// Setup everything:
err = Init(26, 14); if (IMR_ISNOTOK(err)) return err;

// Set the name of the model:
Set_Name(Name);

// Setup stuff:
Num_Vertices = 12;
Num_Polygons = 14;

// Setup the vertices:
for (Vtx = 0, Ang = 0; Vtx < 6; Vtx ++, Ang += 170)
    {
    Vertices[Vtx].lX = -Radius * SinTable[Ang];
    Vertices[Vtx].lY = YDim;
    Vertices[Vtx].lZ = Radius * CosTable[Ang];
    Vertices[Vtx + 6].lX = -Radius * SinTable[Ang];
    Vertices[Vtx + 6].lY = -YDim;
    Vertices[Vtx + 6].lZ = Radius * CosTable[Ang];
     }

// Now setup the polys:
for (Poly = 0; Poly < 6; Poly ++)
    {
    Polygons[Poly].Num_Verts = 4;
    Vtx = Poly + 1; if (Vtx > 5) Vtx = 0;
    Polygons[Poly].Vtx_Index[0] = Poly;
    Polygons[Poly].Vtx_Index[1] = Vtx;
    Polygons[Poly].Vtx_Index[2] = Vtx + 6;
    Polygons[Poly].Vtx_Index[3] = Poly + 6;
     }    
Polygons[6].Num_Verts = 3;
Polygons[6].Vtx_Index[0] = 5;
Polygons[6].Vtx_Index[1] = 4;
Polygons[6].Vtx_Index[2] = 0;
Polygons[7].Num_Verts = 3;
Polygons[7].Vtx_Index[0] = 4;
Polygons[7].Vtx_Index[1] = 3;
Polygons[7].Vtx_Index[2] = 0;
Polygons[8].Num_Verts = 3;
Polygons[8].Vtx_Index[0] = 3;
Polygons[8].Vtx_Index[1] = 2;
Polygons[8].Vtx_Index[2] = 0;
Polygons[9].Num_Verts = 3;
Polygons[9].Vtx_Index[0] = 2;
Polygons[9].Vtx_Index[1] = 1;
Polygons[9].Vtx_Index[2] = 0;
Polygons[10].Num_Verts = 3;
Polygons[10].Vtx_Index[0] = 10;
Polygons[10].Vtx_Index[1] = 11;
Polygons[10].Vtx_Index[2] = 6;
Polygons[11].Num_Verts = 3;
Polygons[11].Vtx_Index[0] = 9;
Polygons[11].Vtx_Index[1] = 10;
Polygons[11].Vtx_Index[2] = 6;
Polygons[12].Num_Verts = 3;
Polygons[12].Vtx_Index[0] = 8;
Polygons[12].Vtx_Index[1] = 9;
Polygons[12].Vtx_Index[2] = 6;
Polygons[13].Num_Verts = 3;
Polygons[13].Vtx_Index[0] = 7;
Polygons[13].Vtx_Index[1] = 8;
Polygons[13].Vtx_Index[2] = 6;
    
// Setup normal indices:
for (poly = 0; poly < Num_Polygons; poly ++)
    {
    Polygons[poly].Flags.TwoSided = 0;
    Polygons[poly].Flags.Transparent = 0;
    Polygons[poly].Normal_Index = Num_Vertices + poly;
    Polygons[poly].Normal = &Vertices[Polygons[poly].Normal_Index];
     }

// And return ok:
return IMR_OK;
 }

/***************************************************************************\
  Applies the specified texture to all the polygons in the model.
  Sets default pegged texture coordinates.
  Note: Does not setup the model, so a call to Setup() should be made
        before use.
  Returns IMR_OK if successful, otherwise an error.
\***************************************************************************/
int IMR_Model::Paint(IMR_Texture *Texture)
{
int poly, vtx;

// Make sure we have a texture:
if (!Texture)
    {
    IMR_SetErrorText("IMR_Model::Paint(): No texture specified!");
    return IMRERR_NODATA;
     }

// Setup normal indices:
for (poly = 0; poly < Num_Polygons; poly ++)
    {
    // Set the texture for the poly:
    Polygons[poly].Material.Set_TextureName(Texture->Get_Name());
    
    // Now set the texture coords:
    Polygons[poly].UVI_Info[0].U = 0;
    Polygons[poly].UVI_Info[0].V = 0;
    Polygons[poly].UVI_Info[1].U = 1;
    Polygons[poly].UVI_Info[1].V = 0;
    Polygons[poly].UVI_Info[2].U = 1;
    Polygons[poly].UVI_Info[2].V = 1;
    if (Polygons[poly].Num_Verts == 4)
        {
        Polygons[poly].UVI_Info[3].U = 0;
        Polygons[poly].UVI_Info[3].V = 1;
         }
     }
        
// And return OK:
return IMR_OK;
 }

/***************************************************************************\
  Applies the specified texture to all the polygons in the model.
  Sets unpegged texture coordinates using the specified width and height,
  specified in world units.
  Note: Does not setup the model, so a call to Setup() should be made
        before use.
  Returns IMR_OK if successful, otherwise an error.
\***************************************************************************/
int IMR_Model::Paint_Unpegged(IMR_Texture *Texture, int Width, int Height)
{
int poly, vtx, start, end;
int DeltaX, DeltaY, DeltaZ, MagSquared;
float Distance[4];

// Make sure we have a texture:
if (!Texture)
    {
    IMR_SetErrorText("IMR_Model::Paint(): No texture specified!");
    return IMRERR_NODATA;
     }

// Setup normal indices:
for (poly = 0; poly < Num_Polygons; poly ++)
    {
    // Calculate distances from each vertex:
    start = 1;
    end = 0;
    for (vtx = 0; vtx < Polygons[poly].Num_Verts; vtx ++)
        {
        // Calculate distance:
        DeltaX = FixedToInt(Vertices[Polygons[poly].Vtx_Index[end]].lX - Vertices[Polygons[poly].Vtx_Index[start]].lX);
        DeltaY = FixedToInt(Vertices[Polygons[poly].Vtx_Index[end]].lY - Vertices[Polygons[poly].Vtx_Index[start]].lY);
        DeltaZ = FixedToInt(Vertices[Polygons[poly].Vtx_Index[end]].lZ - Vertices[Polygons[poly].Vtx_Index[start]].lZ);
        MagSquared = (DeltaX * DeltaX) + (DeltaY * DeltaY) + (DeltaZ * DeltaZ);
        Distance[vtx] = sqrt((float)MagSquared);

        // Set next span:
        ++ end;
        if (++ start == Polygons[poly].Num_Verts) start = 0;
         }
         
    // Set the texture for the poly:
    Polygons[poly].Material.Set_TextureName(Texture->Get_Name());
    
    // Now set the texture coords:
    Polygons[poly].Set_Pegged(1);
    Polygons[poly].UVI_Info[0].U = 0;
    Polygons[poly].UVI_Info[0].V = 0;
    Polygons[poly].UVI_Info[1].U = Distance[0] / (float)Width;
    Polygons[poly].UVI_Info[1].V = 0;
    Polygons[poly].UVI_Info[2].U = Distance[2] / (float)Width;
    Polygons[poly].UVI_Info[2].V = Distance[1] / (float)Height;
    if (Polygons[poly].Num_Verts == 4)
        {
        Polygons[poly].UVI_Info[3].U = 0;
        Polygons[poly].UVI_Info[3].V = Distance[3] / (float)Height;
         }
     }
        
// And return OK:
return IMR_OK;
 }        

/***************************************************************************\
  Sets the LightSource flag in all the polys in the model to the specified
  state.
  Returns IMR_OK if successful, otherwise an error.
\***************************************************************************/
int IMR_Model::Set_PolyFlag_LightSource(int State)
{
for (int poly = 0; poly < Num_Polygons; poly ++)
    Polygons[poly].Flags.LightSource = 1;
        
// And return OK:
return IMR_OK;
 }        
