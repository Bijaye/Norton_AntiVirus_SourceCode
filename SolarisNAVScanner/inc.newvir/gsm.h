//************************************************************************
//
// $Header:   S:/INCLUDE/VCS/GSM.H_v   1.0   30 Jun 1997 16:17:34   DCHI  $
//
// Description:
//      Header file for local memory allocation functions.
//
//************************************************************************
// $Log:   S:/INCLUDE/VCS/GSM.H_v  $
// 
//    Rev 1.0   30 Jun 1997 16:17:34   DCHI
// Initial revision.
// 
//************************************************************************

#ifndef _GSM_H_

#define _GSM_H_

typedef struct tagGSM_HDR
{
    struct tagGSM_HDR FAR *     lpstNextBlk;
    DWORD                       dwSizeHdrUnits;
} GSM_HDR_T, FAR *LPGSM_HDR;

/////////////////////////////////////////////////////////////////
// Global shared memory functions
/////////////////////////////////////////////////////////////////

LPGSM_HDR GSMCreate
(
    LPVOID          lpvRootCookie,
    DWORD           dwSizeBytes
);

BOOL GSMDestroy
(
    LPVOID          lpvRootCookie,
    LPGSM_HDR       lpstHdr
);

void GSMInit
(
    LPGSM_HDR       lpstHdr,
    DWORD           dwSizeBytes
);

LPVOID GSMAlloc
(
    LPGSM_HDR       lpstHdr,
    DWORD           dwSizeBytes
);

BOOL GSMFree
(
    LPGSM_HDR       lpstHdr,
    LPVOID          lpvMemoryBlock
);

#endif // #ifndef _GSM_H_

