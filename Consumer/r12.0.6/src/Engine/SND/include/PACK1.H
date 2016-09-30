/////////////////////////////////////////////////////////////////////
//
// PACK1.H
//
// Including this header file turns on structure packing.
//
/////////////////////////////////////////////////////////////////////


#if defined(SYM_WIN32)      || \
    defined(SYM_NTK)        || \
    defined(SYM_VXD)        || \
    defined(SYM_400)

#include "pshpack1.h"


#elif defined(__MACINTOSH__)|| \
      defined(SYM_AIX)

#pragma options align=packed


#else

#pragma pack(1)

#endif


