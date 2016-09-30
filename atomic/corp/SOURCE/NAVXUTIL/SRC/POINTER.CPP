// Copyright 1997 Symantec Corporation, Peter Norton Group
//***************************************************************************
//
// $Header:   S:/NAVXUTIL/VCS/pointer.cpv   1.1   27 Jun 1997 12:03:42   JBRENNA  $
//
// Description:
//   JBRENNA: unknown what this does ... I just added this header comment so
//   that VCS would track the revision comments here.
//
// Contains:
//
//***************************************************************************
// $Log:   S:/NAVXUTIL/VCS/pointer.cpv  $
// 
//    Rev 1.1   27 Jun 1997 12:03:42   JBRENNA
// Added comment header.
// 
//***************************************************************************

#include "platform.h"
#ifdef SYM_WIN
#include "navutil.h"

#define MK_FP(seg, offset) (void __far *)(((unsigned long)seg << 16) \
    + (unsigned long)(unsigned)offset)

LPVOID SYM_EXPORT WINAPI NAV_GETPTR (HWND hwnd)
{
    LPVOID lpVoid = NULL;

    lpVoid = MK_FP((UINT)GetProp((hwnd),DLGREC_SELECTOROF),(UINT)GetProp((hwnd),DLGREC_OFFSETOF));

    return (lpVoid);
}

LPVOID SYM_EXPORT WINAPI NAV_ALLOCPTR (HWND hWnd, DWORD dwSize)
{
    LPVOID  lpDlgRec = NULL;

    if ((lpDlgRec = MemAllocPtr (GHND, dwSize)) != 0)
        {
        SetProp (hWnd, DLGREC_SELECTOROF, (HANDLE)HIWORD(lpDlgRec));
        SetProp (hWnd, DLGREC_OFFSETOF, (HANDLE)LOWORD(lpDlgRec));
        }

    return (lpDlgRec);
}

VOID SYM_EXPORT WINAPI NAV_FREEPTR (HWND hWnd)
{
    LPVOID lpVoid = NULL;

    lpVoid = NAV_GETPTR (hWnd);

    if (lpVoid)
        MemFreePtr (lpVoid);

    RemoveProp (hWnd, DLGREC_SELECTOROF);
    RemoveProp (hWnd, DLGREC_OFFSETOF);
}


VOID SYM_EXPORT WINAPI LP_SET(HWND hwnd, LPVOID lpVoid)
{
   SetProp((hwnd), (LPCSTR)NAV_SELECTOROF, (HANDLE)HIWORD((lpVoid)));
   SetProp((hwnd), (LPCSTR)NAV_OFFSETOF, (HANDLE)LOWORD((lpVoid)));
}

LPVOID SYM_EXPORT WINAPI LP_GET (HWND hwnd)
{
    LPVOID lpVoid = NULL;

    lpVoid = (LPVOID)MK_FP ((UINT)GetProp((hwnd), (LPCSTR)NAV_SELECTOROF),
                     (UINT)GetProp((hwnd), (LPCSTR)NAV_OFFSETOF));

    return (lpVoid);
}


VOID SYM_EXPORT WINAPI LP_FREE (HWND hwnd)
{
    RemoveProp ((hwnd), (LPCSTR)NAV_SELECTOROF);
    RemoveProp ((hwnd), (LPCSTR)NAV_OFFSETOF);
}


VOID SYM_EXPORT WINAPI WORD_SET(HWND hwnd, WORD wValue)
{
   SetProp (hwnd, (LPCSTR)NAV_WORD, (HANDLE)wValue);
}

WORD SYM_EXPORT WINAPI WORD_GET (HWND hwnd)
{
    auto    WORD    wValue;

    wValue = (WORD)GetProp (hwnd, (LPCSTR)NAV_WORD);

    return (wValue);
}

VOID SYM_EXPORT WINAPI WORD_FREE (HWND hwnd)
{
    RemoveProp (hwnd, (LPCSTR)NAV_WORD);
}

#endif

