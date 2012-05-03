/***************************************************************************\
  File: IMR_Geom_Prim_Ang.hpp
  Description: Header for angles and attitudes.
  Author: Daniel Hawthorn
  Modified:
  
\***************************************************************************/
#ifndef __IMR_GEOM_PRIM_ANG__HPP
#define __IMR_GEOM_PRIM_ANG__HPP

#include "IMR_Table.hpp"

// Attitude class:
class IMR_Attitude
    {
    public:
      int X, Y, Z;
      IMR_Attitude() { X = Y = Z = 0; };
      inline void Fix_Ang(void);
      inline int operator == (IMR_Attitude &A);
      inline int operator != (IMR_Attitude &A);
      inline void operator = (IMR_Attitude &A);
      inline IMR_Attitude operator + (IMR_Attitude &A);
      inline IMR_Attitude operator - (IMR_Attitude &A);
      inline IMR_Attitude &operator += (IMR_Attitude &A);
      inline IMR_Attitude &operator -= (IMR_Attitude &A);
     };

/***************************************************************************\
  Corrects the attutude so it is within the engine boundries.
\***************************************************************************/
inline void IMR_Attitude::Fix_Ang(void)
{
X &= IMR_DEGREEAND;
Y &= IMR_DEGREEAND;
Z &= IMR_DEGREEAND;
 };

/***************************************************************************\
  Overloaded operators.
\***************************************************************************/
inline int IMR_Attitude::operator == (IMR_Attitude &A)
{
int RValue = 0;
if (A.X == X && A.Y == Y && A.Z == Z) RValue = 1;
return RValue;
 }

inline int IMR_Attitude::operator != (IMR_Attitude &A)
{
int RValue = 0;
if ((A.X != X) || (A.Y != Y) || (A.Z != Z)) RValue = 1;
return RValue;
 }

inline void IMR_Attitude::operator = (IMR_Attitude &A)
{
A.Fix_Ang();
X = A.X;
Y = A.Y;
Z = A.Z;
 }

inline IMR_Attitude IMR_Attitude::operator + (IMR_Attitude &A)
{
IMR_Attitude Temp;
Temp.X = X + A.X;
Temp.Y = Y + A.Y;
Temp.Z = Z + A.Z;
Temp.Fix_Ang();
return Temp;
 }

inline IMR_Attitude IMR_Attitude::operator - (IMR_Attitude &A)
{
IMR_Attitude Temp;
Temp.X = X - A.X;
Temp.Y = Y - A.Y;
Temp.Z = Z - A.Z;
Temp.Fix_Ang();
return Temp;
 }

inline IMR_Attitude &IMR_Attitude::operator += (IMR_Attitude &A)
{
X += A.X;
Y += A.Y;
Z += A.Z;
Fix_Ang();
return *this;
 }

inline IMR_Attitude &IMR_Attitude::operator -= (IMR_Attitude &A)
{
X -= A.X;
Y -= A.Y;
Z -= A.Z;
Fix_Ang();
return *this;
 }

#endif

