/////////////////////////////////////////////////////////////////////////////
// Copyright 1996 Symantec Corp. / Peter Norton Product Group
/////////////////////////////////////////////////////////////////////////////
//************************************************************************
//
// File:
//      navdwnd.h
//
// Description:
//      Header file for the window-related functions and dialog box procs.
//
/////////////////////////////////////////////////////////////////////////////


#ifndef __NAVDWND_H__
#define __NAVDWND_H__

// define MAX_PATH for Win16
#ifndef MAX_PATH
#define MAX_PATH 144
#endif


void NavDemoCreateOutputListbox ( HWND hWnd );

void NavDemoResizeOutputListbox ( void );

BOOL CALLBACK NavDemoHandleInfectionDlgProc ( HWND   hWndDlg,
                                              UINT   uMsg,
                                              WPARAM wParam,
                                              LPARAM lParam );

BOOL CALLBACK NavDemoVirusInfoDlgProc ( HWND   hWndDlg,
                                        UINT   uMsg,
                                        WPARAM wParam,
                                        LPARAM lParam );


BOOL CALLBACK NavDemoMemScanProgressDlgProc ( HWND   hWndDlg,
                                              UINT   uMsg,
                                              WPARAM wParam,
                                              LPARAM lParam );

BOOL CALLBACK NavDemoSysAreaPromptDlgProc ( HWND   hWndDlg,
                                            UINT   uMsg,
                                            WPARAM wParam,
                                            LPARAM lParam );

#endif  // ifndef __NAVDWND_H__
