/***************************************************************************\
  File: IMR_Err.hpp
  Description: All return codes used for the Immerse engine.
  Author: Daniel Hawthorn
  Modified:
  
\***************************************************************************/
#ifndef __IMR_RETVALS__HPP
#define __IMR_RETVALS__HPP


// Everythings a ok:
#define IMR_OK                          1

// Non-fatal errors:
#define IMRERR_NONFATAL_NOTFOUND        2
#define IMRERR_NONFATAL_SPACEOVERFLOW   3
#define IMRERR_NONFATAL_NOTINFRAME      4
#define IMRERR_NONFATAL_INFRAME         5
#define IMRERR_NONFATAL_NOTINBATCH      6

// Fatal errors:
#define IMRERR_GENERIC           0
#define IMRERR_INCOMPLETE       -2
#define IMRERR_TOMANY           -3
#define IMRERR_UNUNIQUE         -4
#define IMRERR_BADBUFFER        -5
#define IMRERR_OUTOFMEM         -6
#define IMRERR_BADWIDTH         -7
#define IMRERR_NOPALETTE        -8
#define IMRERR_NODATA           -9
#define IMRERR_NOTREADY         -10
#define IMRERR_DIRECTX          -11
#define IMRERR_NOTLOCKED        -12
#define IMRERR_LOCKED           -13
#define IMRERR_BADFILE          -14

// Macros:
#define IMR_ISFATALERR(x)    (x < IMR_OK)
#define IMR_ISNONFATALERR(x) (x > IMR_OK)
#define IMR_ISOK(x)          (x == IMR_OK)
#define IMR_ISNOTOK(x)       (x != IMR_OK)

#endif
