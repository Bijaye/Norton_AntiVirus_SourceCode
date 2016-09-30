// unixmem.cpp
// Copyright 1999 by Symantec Corporation.  All rights reserved.
// A Unix implementation of the Mem functions in the Symantec 
// core library (SYMKRNL).


#ifdef SYM_UNIX  // whole file


#include <stdlib.h>
#include "platform.h"


LPVOID MemAllocPtr(UINT uFlags, DWORD dwSize)
{
    if ((uFlags & GMEM_ZEROINIT) != 0)
        return calloc(1, dwSize);
    else
        return malloc(dwSize);
}


HGLOBAL MemFreePtr(LPVOID lpMem)
{
    free(lpMem);
    return (HGLOBAL) lpMem;
}


#endif // SYM_UNIX
