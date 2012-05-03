/****************************************************************\
 
 iMMERSE Engine
 (C) 1999 No Tears Shed Software
 All rights reserved
 
 Filename: IMR_RetVals.hpp
 Description: All status codes used in the Immerse engine
 
\****************************************************************/
#ifndef __IMR_RETVALS__HPP
#define __IMR_RETVALS__HPP

// True and false:
#ifndef TRUE
    #define TRUE  1
#endif
#ifndef FALSE
    #define FALSE 0
#endif

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
#define IMRERR_CANTUSETARGET    -15
#define IMRERR_BADTARGETWIDTH   -16
#define IMRERR_BADTARGETHEIGHT  -17
#define IMRERR_BADTARGETFORMAT  -18
#define IMRERR_BADID            -19

// Macros:
#define IMR_ISFATALERR(x)    (x < IMR_OK)
#define IMR_ISNONFATALERR(x) (x > IMR_OK)
#define IMR_ISOK(x)          (x == IMR_OK)
#define IMR_ISNOTOK(x)       (x != IMR_OK)

#endif
