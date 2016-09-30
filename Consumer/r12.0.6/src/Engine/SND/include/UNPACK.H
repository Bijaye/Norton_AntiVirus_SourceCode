/////////////////////////////////////////////////////////////////////
//
// UNPACK.H
//
// Including this header file turns off structure packing.
//
/////////////////////////////////////////////////////////////////////


#if defined(SYM_WIN32)      || \
    defined(SYM_NTK)        || \
    defined(SYM_VXD)        || \
    defined(SYM_400)

#include "poppack.h"


#elif defined(__MACINTOSH__)|| \
      defined(SYM_AIX)

#pragma options align=reset


#else

#pragma pack()

#endif


