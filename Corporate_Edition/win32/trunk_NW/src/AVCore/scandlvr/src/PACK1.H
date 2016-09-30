// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
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


