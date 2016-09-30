//************************************************************************
//
// $Header:   S:/NAVEX/VCS/remotex.h_v   1.0   24 Dec 1998 23:02:54   DCHI  $
//
// Description:
//  Contains prototypes for W32.RemoteExplorer repair
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/remotex.h_v  $
// 
//    Rev 1.0   24 Dec 1998 23:02:54   DCHI
// Initial revision.
// 
//************************************************************************

#ifndef _REMOTEX_H_

#define _REMOTEX_H_

typedef struct tagREMOTEXCB
{
    HFILE               hFile;
    DWORD               dwSrcBaseOffset;
    DWORD               dwSrcCurOffset;
    DWORD               dwDstBaseOffset;
    DWORD               dwDstCurOffset;
} REMOTEXCB_T, FAR *LPREMOTEXCB;

BOOL RemoteXRestoreHost
(
    LPCALLBACKREV1      lpstCallBack,
    HFILE               hFile,
    DWORD               dwHostOffset,
    DWORD               dwHostSize,
    LPBYTE              lpabyWorkBuffer,
    DWORD               dwWorkBufSize
);

BOOL RemoteXRestoreNonEXEHost
(
    LPCALLBACKREV1      lpstCallBack,
    HFILE               hFile,
    LPBYTE              lpabyWorkBuffer,
    DWORD               dwWorkBufSize
);

#endif // #ifndef _REMOTEX_H_


