// Copyright 1995 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/EMULATOR/VCS/cover.cpv   1.8   29 Jul 1996 11:13:48   jmillar  $
//
// Description:
//
//  Contains cover functions for memory allocation.
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/EMULATOR/VCS/cover.cpv  $
// 
//    Rev 1.8   29 Jul 1996 11:13:48   jmillar
// Add the next comment - brief crashed during chkin
// Don't zero memory in allocate - Carey is doing this above, and it dogs
// the performance.
// 
//    Rev 1.7   29 Jul 1996 11:12:36   jmillar
//
//  Don't zero memory in allocate - Carey is doing this above, and it dogs
//  the performance.
// 
//    Rev 1.6   17 May 1996 09:59:30   CNACHEN
// Changed MemoryAlloc/MemoryFree to PMemoryAlloc/PMemoryFree to avoid
// collisions with NTK #includes.
// 
//    Rev 1.5   14 Dec 1995 13:02:42   CNACHEN
// Added support for control relinquishing under NLM...
// 
//    Rev 1.4   19 Oct 1995 18:23:46   CNACHEN
// Initial revision... with comment header :)
// 
//************************************************************************

#include "platform.h"

#include "pamapi.h"

LPVOID PMemoryAlloc(DWORD uSize)
{
    return(MemAllocPtr(GMEM_MOVEABLE,uSize));
}

void PMemoryFree(LPVOID lpBuffer)
{
    MemFreePtr(lpBuffer);
}

#ifdef SYM_NLM

void PAMRelinquishControl(void)
{
    void RelinquishControl();
    // call NLM relinquish control function here!

    RelinquishControl();
}

#endif
