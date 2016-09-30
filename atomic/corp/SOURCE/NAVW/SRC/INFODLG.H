// Copyright 1998 Symantec Corporation.
//*************************************************************************
//
// $Header:   S:/NAVW/VCS/infodlg.h_v   1.0   07 Jun 1998 15:38:44   tcashin  $
//
// Description:
//      Routines for the Info button.
//
// Contains:
//
//*************************************************************************
// $Log:   S:/NAVW/VCS/infodlg.h_v  $
// 
//    Rev 1.0   07 Jun 1998 15:38:44   tcashin
// Initial revision.
// 
//*************************************************************************

#ifndef __INFODLG_H
#define __INFODLG_H

#define INFOVIRUSDEFS       0x00000001
#define INFOQUARANTINE      0x00000002

typedef struct tagInfo {
    DWORD       dwInfoType;
    LPNAVMAIN   lpNav;
} INFOSTRUCT, *LPINFOSTRUCT;


VOID SYM_EXPORT WINAPI InfoDlg (HWND hDlg, LPINFOSTRUCT lpInfo);
BOOL CALLBACK InfoDlgProc (HWND hDlg, UINT wMessage, WPARAM wParam, LPARAM lParam);

#endif // __INFODLG_H
