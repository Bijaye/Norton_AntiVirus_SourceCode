///////////////////////////////////////////////////////////////////////////////
// Copyright 1996 Symantec Corp. / Peter Norton Product Group
///////////////////////////////////////////////////////////////////////////////
//************************************************************************
//
// File:
//      navdemo.h
//
// Description:
//      Header file for the main implementation file of NAV Demo.
//
/////////////////////////////////////////////////////////////////////////////


#ifndef __NAVDEMO_H__
#define __NAVDEMO_H__


                                        // define MAX_PATH for Win16
#ifndef MAX_PATH
#define MAX_PATH 144
#endif


                                        // struct used to pass options for
                                        // scanning system areas
typedef struct tag_SYSAREA_OPTS
{
    BOOL bOneDrive;                     // FALSE if multiple drives are
                                        // being scanned

    WORD wDrive;                        // what drive or drives to scan
}  SYSAREA_OPTS, FAR* LPSYSAREA_OPTS;


                                        // this is the ID of the main
                                        // window's output listbox
#define IDL_OUTPUT_LISTBOX 100

#ifndef EXPORT                          // define a portable EXPORT so
#ifdef  _WIN32                          // it can be used by the memory
#define EXPORT                          // scanning callback in _WIN16
#else
#define EXPORT __export
#endif
#endif

int PASCAL WinMain ( HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpszCmdLine,
                     int       nCmdShow );

#ifdef __cplusplus
extern "C" {
#endif

LRESULT WINAPI MainWndProc ( HWND   hWnd,
                             UINT   uMessage,
                             WPARAM wParam,
                             LPARAM lParam   );
#ifdef __cplusplus
}
#endif


BOOL NavDemoWM_CREATE ( HWND              hWnd,
                        CREATESTRUCT FAR *lpCreate );

void NavDemoWM_DESTROY ( HWND hWnd );

void NavDemoWM_COMMAND ( HWND        hWnd,
                         int         nID,
                         HWND        hWndCtl,
                         UINT        uNotify );

BOOL NavDemoSelectFile ( HWND  hWnd,
                         LPSTR szFile,
                         LPCSTR szFilters );

void NavDemoScanFile ( HWND  hWnd,
                       LPSTR szFile );

void NavDemoHandleFileInfection ( LPSTR szFile );

void NavDemoScanMem ( BOOL bScanUpperMem );

BOOL NavDemoSelectDrive ( LPSYSAREA_OPTS prOpts );

void NavDemoScanSysArea ( LPSYSAREA_OPTS prOpts );

void NavDemoHandleSysAreaInfection ( void );

void NavDemoSelfTest ( HWND hWnd, LPSTR szFile );

void MessageOutput ( LPCSTR szMessage );

BOOL WINAPI EXPORT NavDemoMemScanCallback ( DWORD  dwNumDone,
                                            DWORD  dwNumTotal,
                                            LPVOID lpUserData);


#endif  // ifndef __NAVDEMO_H__
