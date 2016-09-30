// Copyright 1996 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/INCLUDE/VCS/avobject.h_v   1.2   20 Nov 1996 15:24:08   AOONWAL  $
//
// Description:
//  AVAPI 2.0 object structure definitions.
//
// Contains:
//
// See Also:
//
//************************************************************************
// $Log:   S:/INCLUDE/VCS/avobject.h_v  $
// 
//    Rev 1.2   20 Nov 1996 15:24:08   AOONWAL
// No change.
// 
//    Rev 1.1   22 Oct 1996 12:06:08   AOONWAL
// No change.
// 
//    Rev 1.0   04 Jun 1996 12:57:12   DCHI
// Initial revision.
// 
//************************************************************************

#ifndef _AVOBJECT_H

#define _AVOBJECT_H

#include "avcb.h"

// Memory object

typedef struct tagMEMORYOBJECT
{
    LPVOID                      lpvInfo;
    LPMEMORYCALLBACKS           lpCallBacks;
} MEMORYOBJECT_T, FAR *LPMEMORYOBJECT;

// Linear object

typedef struct tagLINEAROBJECT
{
    LPVOID                      lpvInfo;
    LPLINEARCALLBACKS           lpCallBacks;
} LINEAROBJECT, FAR * LPLINEAROBJECT;

// File object

typedef struct tagFILEOBJECT
{
    struct tagFILEOBJECT FAR   *lpParent;
    LPVOID                      lpvInfo;
    LPFILECALLBACKS             lpCallBacks;
    LPTSTR                      lpszName;       // our communication back

} FILEOBJECT_T, FAR *LPFILEOBJECT;

#endif  // _AVOBJECT_H

