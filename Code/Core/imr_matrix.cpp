/****************************************************************\
 
 iMMERSE Engine
 (C) 1999 No Tears Shed Software
 All rights reserved
 
 Filename: IMR_Matrix.hpp
 Description: Matrix math module
 
\****************************************************************/
#include "IMR_Matrix.hpp"

/***************************************************************************\
  Makes Matrix a rotation matrix
\***************************************************************************/
void IMR_Matrix::Rotate(int Ang_X, int Ang_Y, int Ang_Z)
{
IMR_Matrix Rotate_X, Rotate_Y, Rotate_Z, Temp;
int product = 0;

// Correct angles
if (Ang_X < 0) Ang_X = 0 - Ang_X; 
if (Ang_Y < 0) Ang_Y = 0 - Ang_Y; 
if (Ang_Z < 0) Ang_Z = 0 - Ang_Z; 
if (Ang_X > IMR_DEGREECOUNT) Ang_X &= IMR_DEGREECOUNT - 1;
if (Ang_Y > IMR_DEGREECOUNT) Ang_Y &= IMR_DEGREECOUNT - 1;
if (Ang_Z > IMR_DEGREECOUNT) Ang_Z &= IMR_DEGREECOUNT - 1;

// Init this matrix
Identity();

// Create X rotation matrix (if needed):
if (Ang_X)
    {
    product |= 1;
    Rotate_X.Identity();
    Rotate_X.Mtrx[1][1] = CosTable[Ang_X];
    Rotate_X.Mtrx[1][2] = SinTable[Ang_X];
    Rotate_X.Mtrx[2][1] = -SinTable[Ang_X];
    Rotate_X.Mtrx[2][2] = CosTable[Ang_X];
     }
    
// Create Y rotation matrix (if needed):
if (Ang_Y)
    {
    product |= 2;
    Rotate_Y.Identity();
    Rotate_Y.Mtrx[0][0] = CosTable[Ang_Y];
    Rotate_Y.Mtrx[0][2] = -SinTable[Ang_Y];
    Rotate_Y.Mtrx[2][0] = SinTable[Ang_Y];
    Rotate_Y.Mtrx[2][2] = CosTable[Ang_Y];
     }

// Create Z rotation matrix (if needed):
if (Ang_Z)
    {
    product |= 4;
    Rotate_Z.Identity();
    Rotate_Z.Mtrx[0][0] = CosTable[Ang_Z];
    Rotate_Z.Mtrx[0][1] = SinTable[Ang_Z];
    Rotate_Z.Mtrx[1][0] = -SinTable[Ang_Z];
    Rotate_Z.Mtrx[1][1] = CosTable[Ang_Z];
     }
     
// Merge all the rotation matrices:
if (product == 1)                   // Only X rotation
    Merge_Matrix(Rotate_X.Mtrx);
if (product == 2)                   // Only Y rotation
    Merge_Matrix(Rotate_Y.Mtrx);
if (product == 3)                   // X and Y rotation
    Merge_Matrices(Rotate_X.Mtrx, Rotate_Y.Mtrx);
if (product == 4)                   // Only Z rotation
    Merge_Matrix(Rotate_Z.Mtrx);
if (product == 5)                   // X and Z rotation
    Merge_Matrices(Rotate_X.Mtrx, Rotate_Z.Mtrx);
if (product == 6)                   // Y and Z rotation
    Merge_Matrices(Rotate_Y.Mtrx, Rotate_Z.Mtrx);
if (product == 7)                   // X, Y, and Z rotation
    {
    Temp.Merge_Matrices(Rotate_X.Mtrx, Rotate_Y.Mtrx);
    Merge_Matrices(Temp.Mtrx, Rotate_Z.Mtrx);
     }
 }

/***************************************************************************\
  Makes Matrix a translation matrix.
\***************************************************************************/
void IMR_Matrix::Translate(float Pos_X, float Pos_Y, float Pos_Z)
{
// Set matrix to identity:
//Identity();

// Fill in translation stuff:
Mtrx[3][0] = Pos_X; Mtrx[3][1] = Pos_Y; Mtrx[3][2] = Pos_Z;
 }

/***************************************************************************\
  Makes Matrix a rotation matrix.
\***************************************************************************/
void IMR_Matrix::ZYXRotate(int Ang_X, int Ang_Y, int Ang_Z)
{
IMR_Matrix Rotate_X, Rotate_Y, Rotate_Z, Temp;
int product = 0;

// Correct angles
if (Ang_X < 0) Ang_X = 0 - Ang_X; 
if (Ang_Y < 0) Ang_Y = 0 - Ang_Y; 
if (Ang_Z < 0) Ang_Z = 0 - Ang_Z; 
if (Ang_X > IMR_DEGREECOUNT) Ang_X &= IMR_DEGREECOUNT - 1;
if (Ang_Y > IMR_DEGREECOUNT) Ang_Y &= IMR_DEGREECOUNT - 1;
if (Ang_Z > IMR_DEGREECOUNT) Ang_Z &= IMR_DEGREECOUNT - 1;

// Init this matrix
Identity();

// Create X rotation matrix (if needed):
if (Ang_X)
    {
    product |= 1;
    Rotate_X.Identity();
    Rotate_X.Mtrx[1][1] = CosTable[Ang_X];
    Rotate_X.Mtrx[1][2] = SinTable[Ang_X];
    Rotate_X.Mtrx[2][1] = -SinTable[Ang_X];
    Rotate_X.Mtrx[2][2] = CosTable[Ang_X];
     }
    
// Create Y rotation matrix (if needed):
if (Ang_Y)
    {
    product |= 2;
    Rotate_Y.Identity();
    Rotate_Y.Mtrx[0][0] = CosTable[Ang_Y];
    Rotate_Y.Mtrx[0][2] = -SinTable[Ang_Y];
    Rotate_Y.Mtrx[2][0] = SinTable[Ang_Y];
    Rotate_Y.Mtrx[2][2] = CosTable[Ang_Y];
     }

// Create Z rotation matrix (if needed):
if (Ang_Z)
    {
    product |= 4;
    Rotate_Z.Identity();
    Rotate_Z.Mtrx[0][0] = CosTable[Ang_Z];
    Rotate_Z.Mtrx[0][1] = SinTable[Ang_Z];
    Rotate_Z.Mtrx[1][0] = -SinTable[Ang_Z];
    Rotate_Z.Mtrx[1][1] = CosTable[Ang_Z];
     }
     
// Merge all the rotation matrices:
if (product == 1)                   // Only X rotation
    Merge_Matrix(Rotate_X.Mtrx);
if (product == 2)                   // Only Y rotation
    Merge_Matrix(Rotate_Y.Mtrx);
if (product == 3)                   // X and Y rotation
    Merge_Matrices(Rotate_Y.Mtrx, Rotate_X.Mtrx);
if (product == 4)                   // Only Z rotation
    Merge_Matrix(Rotate_Z.Mtrx);
if (product == 5)                   // X and Z rotation
    Merge_Matrices(Rotate_Z.Mtrx, Rotate_X.Mtrx);
if (product == 6)                   // Y and Z rotation
    Merge_Matrices(Rotate_Z.Mtrx, Rotate_Y.Mtrx);
if (product == 7)                   // X, Y, and Z rotation
    {
    Temp.Merge_Matrices(Rotate_Z.Mtrx, Rotate_Y.Mtrx);
    Merge_Matrices(Temp.Mtrx, Rotate_X.Mtrx);
     }
 }


