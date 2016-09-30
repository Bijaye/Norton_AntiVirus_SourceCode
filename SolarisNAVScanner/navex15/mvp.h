//************************************************************************
//
// $Header:   S:/NAVEX/VCS/mvp.h_v   1.4   17 Sep 1997 14:45:56   DDREW  $
//
// Description:
//      Contains Macro Virus Protection (MVP) functions.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/mvp.h_v  $
// 
//    Rev 1.4   17 Sep 1997 14:45:56   DDREW
// Enabled MVP for SYM_NLM
// 
//    Rev 1.3   16 Jul 1997 18:44:28   DCHI
// Re-enabled MVP for DEC Alpha autoprotect.
// 
//    Rev 1.2   04 Jun 1997 14:09:52   DCHI
// Temporarily disabled MVP for DEC ALPHA autoprotect.
// 
//    Rev 1.1   18 Apr 1997 19:04:02   AOONWAL
// Modified during DEV1 malfunction
// 
//    Rev ABID  16 Apr 1997 19:45:24   DCHI
// Changes so that mvpdef.dat is only loaded from NAV directory.
// 
//    Rev 1.0   19 Mar 1997 17:32:36   DCHI
// Initial revision.
// 
//************************************************************************

#ifndef _MVP_H_

#define _MVP_H_

#define VID_MVP         0x1556

#define MVP_WD7         0
#define MVP_WD8         1
#define MVP_XL97        2

#if defined(SYM_WIN) || defined(SYM_VXD) || defined(SYM_NTK) || defined(SYM_NLM)
#define MVP_ENABLED
#endif

#if defined(MVP_ENABLED)
BOOL MVPLoadData
(
    LPVOID              lpvExtra
);

void MVPUnloadData
(
    void
);

BOOL MVPCheck
(
    int                 nType,
    LPBYTE              lpabyName,
    DWORD               dwCRC
);
#else
#define MVPLoadData(a)  (TRUE)
#define MVPUnloadData()
#define MVPCheck(a,b,c) (TRUE)
#endif

#endif // #ifndef _MVP_H_
