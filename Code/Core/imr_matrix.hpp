/****************************************************************\
 
 iMMERSE Engine
 (C) 1999 No Tears Shed Software
 All rights reserved
 
 Filename: IMR_Matrix.hpp
 Description: Header
 
\****************************************************************/
#ifndef __IMR_MATRIX__HPP
#define __IMR_MATRIX__HPP

#include "IMR_Table.hpp"

// Data type used in matrix
typedef float mat[4][4];

// Matrix class
class IMR_Matrix 
    {
    public:
      mat Mtrx;
      Matrix() { };
      void Init(mat Mtrx);
      void inline Identity(void);
      void inline Merge_Matrix(mat Mtrx);
      void inline Merge_Matrices(mat a, mat b);
      void Rotate(int Ang_X, int Ang_Y, int Ang_Z);
      void ZYXRotate(int Ang_X, int Ang_Y, int Ang_Z);
      void Translate(float Pos_X, float Pos_Y, float Pos_Z);
     };

/***************************************************************************\
  Sets matrix to identity matrix.
\***************************************************************************/
void inline IMR_Matrix::Identity(void)
{
Mtrx[0][0] = Mtrx[1][0] = Mtrx[2][0] = Mtrx[3][0] = 0;
Mtrx[0][1] = Mtrx[1][1] = Mtrx[2][1] = Mtrx[3][1] = 0;
Mtrx[0][2] = Mtrx[1][2] = Mtrx[2][2] = Mtrx[3][2] = 0;
Mtrx[0][3] = Mtrx[1][3] = Mtrx[2][3] = Mtrx[3][3] = 0;
Mtrx[0][0] = Mtrx[1][1] = Mtrx[2][2] = Mtrx[3][3] = 1.0f;
 }

/***************************************************************************\
  Merges NewMatrix with Matrix.
  Author: John De Goes, modified by Daniel Hawthorn
\***************************************************************************/
void inline IMR_Matrix::Merge_Matrix(mat NewMatrix)
{
mat TempMatrix;
int i, j;    

for (i = 0; i < 4; i++)
    for (j = 0; j < 4; j++) 
        TempMatrix[i][j] = (Mtrx[i][0] * NewMatrix[0][j]) +
                           (Mtrx[i][1] * NewMatrix[1][j]) +
                           (Mtrx[i][2] * NewMatrix[2][j]) +
                           (Mtrx[i][3] * NewMatrix[3][j]);

for (i = 0; i < 4; i++)
    {
    Mtrx[i][0] = TempMatrix[i][0];
    Mtrx[i][1] = TempMatrix[i][1];
    Mtrx[i][2] = TempMatrix[i][2];
    Mtrx[i][3] = TempMatrix[i][3];
     }
 }

/***************************************************************************\
  Merges MtrxA with MtrxB and stores the result in Matrix.
  Author: John De Goes, modified by Daniel Hawthorn
\***************************************************************************/
void inline IMR_Matrix::Merge_Matrices(mat MtrxA, mat MtrxB)
{
int i, j;
Identity();
for (i = 0; i < 4; i++ )
    for (j = 0; j < 4; j++ )
        Mtrx[i][j] = (MtrxA[i][0] * MtrxB[0][j]) +
                     (MtrxA[i][1] * MtrxB[1][j]) +
                     (MtrxA[i][2] * MtrxB[2][j]) +
                     (MtrxA[i][3] * MtrxB[3][j]);
 }

#endif
