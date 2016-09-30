//*********************************************************************
// Copyright 1993 Symantec, Peter Norton Group
//
// File:  symalogi.h    Internal declarations for SYMALOG project
//
//
//Kedit:EditV SetF FExt CPP
//
// See Also: SYMALOG.H
//
// Future Enhancements:
//*********************************************************************
// $Header:   S:/navalog/VCS/actilogi.h_v   1.0   06 Feb 1997 20:55:06   RFULLER  $
// 04/20/93 WRL is William R. Lee II (BLEE)
//            - Moved to Quake from NIRVANA:ACTILOG
//            - Renamed from ACTILOG.H to SYMALOG.H
// 05/12/93 WRL Changed _ACTILOG_H to _ACTILOGI_H
//            - Deleted unused SIZE_DATE and SIZE_TIME
//*********************************************************************
#ifndef _ACTILOGI_H
#define _ACTILOGI_H
#include <stdio.h>                      // L_tmpnam tmpnam()
#include "platform.h"                   // Symantec global stuff
#include "options.h"
#include "actilog.h"                    // Project specific external dcls

#define ERR_NOTHERE  0xFFFF
#define ERR_MEMALLOC      1
#define ERR_NOT_VALID     2
#define ERR_FILEEXIST     3             // Log file cannot be found
#define ERR_NOT_DATE      4
#define ERR_DIALOG        5
#define ERR_ALOGINIT      6             // error in symALogInit()

//**************************************************************************
// This PRIVATE typedef is used solely by the ACTILOG.DLL It is not meant to
// be used by users to pass information.
//**************************************************************************
typedef struct {
   BOOL        bReceivedPaint;
   LPACTILOG   lpAct;                   // Activity Log block
   HALOG       hALog;                   // symALog init block
   TCHAR       TempFile[SYM_MAX_PATH+1]; // Temp file name
   BOOL        bEndBuild;
} PRIVATE, FAR * LPRIVATE;

//*********************************************************************
// - Global variables
//*********************************************************************
EXTERN HINSTANCE hDLLInst;              // This DLL's instance handle

/**********************************************************************
** - External Prototypes and macros
**********************************************************************/
#define WinGetInstance(hWnd) ((HINSTANCE)GetWindowWord((hWnd),GWW_HINSTANCE))

#ifdef __cplusplus
extern "C" {
#endif

int SYM_EXPORT WINAPI FilterActivityLog(
   HWND     hParent,                    // Calling dude
   UINT FAR *aFilterDesc,               // Array of String IDs to
                                        //    filter descriptions
   LPRIVATE lpShowStruct);              // Show structure information

BOOL SYM_EXPORT WINAPI ClearLog(HWND hWindow); // Mommy window

VOID ErrorHandler(HWND hDlg,          // Mommy window
                  UINT uStatus);      // Status to cry about

#if defined( __cplusplus )
}
#endif  // #if defined( __cplusplus )

#endif
