/***************************************************************************\
  File: IMR_FixedPnt.hpp
  Description: Everything you need to use for the FIXEDPNT data type.
               Definitions, typedefs, and inline functions.  No cpp module!
  Author: Daniel Hawthorn
  Modified:
  
\***************************************************************************/
#ifndef __IMR_FIXEDPNT__HPP
#define __IMR_FIXEDPNT__HPP

// Include libraries:
#include <math.h>

// Types, constants, and macros:
typedef long FIXEDPNT;
#define FIXED_ONE            65536
#define FIXED_MAXOVERONE     1

inline FIXEDPNT FixedMult(FIXEDPNT x, FIXEDPNT y)
{
__int64 tx = x, ty = y;
return (tx * ty) >> 16;
 }
inline FIXEDPNT FixedDev(FIXEDPNT x, FIXEDPNT y)
{
__int64 tx = x, ty = y;
return ((tx << 16) / ty);
 }
inline int FixedToInt(FIXEDPNT x) { return (int)x >> 16; }
inline float FixedToFloat(FIXEDPNT x) { return (float)x / 65536; }
inline FIXEDPNT IntToFixed(int x) { return (FIXEDPNT)(x << 16); }
inline FIXEDPNT FloatToFixed(float x) { return (FIXEDPNT)(x * 65536); }


#endif
