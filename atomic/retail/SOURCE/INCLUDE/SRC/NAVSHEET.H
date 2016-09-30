/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/INCLUDE/VCS/NAVSHEET.H_v   1.3   11 Jul 1997 16:55:12   DBUCHES  $
/////////////////////////////////////////////////////////////////////////////
//
// NAVSHEET.H - This file contains the declarations for all functions exported
//		from N32OPTS.DLL
//
//
//
//
//
//
//
//
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/INCLUDE/VCS/NAVSHEET.H_v  $
// 
//    Rev 1.3   11 Jul 1997 16:55:12   DBUCHES
// Added bIsNT parameter to NAVOptionsSheetEx()
// 
//    Rev 1.2   27 Jun 1997 13:01:10   DBUCHES
// Added iActivePage paremeter to NAVOptionsSheet* functions.
// 
//    Rev 1.1   27 May 1997 12:29:58   DALLEE
// export "C" around NavOptionsSheet/Ex() exports to remove C++ name mangling.
// 
//    Rev 1.0   14 May 1997 14:59:12   DBUCHES
// Initial revision.
/////////////////////////////////////////////////////////////////////////////
#ifndef NAVSHEET_H_
#define NAVSHEET_H_

#include "platform.h"
#include "options.h"
#include "exclude.h"

#ifdef BUILDING_DLL
#define DECLSPEC_NAVSHEET __declspec(dllexport)
#else
#define DECLSPEC_NAVSHEET __declspec(dllimport)
#endif



///////////////////////////////////////////////////////////////////////////////////////
// Page IDs
///////////////////////////////////////////////////////////////////////////////////////
#define N32OPTS_SCANNER_PAGE        0
#define N32OPTS_AP_PAGE             1
#define N32OPTS_STARTUP_PAGE        2
#define N32OPTS_ALERT_PAGE          3
#define N32OPTS_LOG_PAGE            4
#define N32OPTS_EXCLUDE_PAGE        5
#define N32OPTS_GENERAL_PAGE        6
#define N32OPTS_INOC_PAGE           7
#define N32OPTS_PASSWORD_PAGE       8

///////////////////////////////////////////////////////////////////////////////////////
// Maximum number of pages allowed.
///////////////////////////////////////////////////////////////////////////////////////
#define N32OPTS_MAXPAGES            32


///////////////////////////////////////////////////////////////////////////////////////
// Exported functions
///////////////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
extern "C" {
#endif

int DECLSPEC_NAVSHEET PASCAL NAVOptionsSheet(HWND hwnd, LPNAVOPTIONS pNavOpts, int iActivePage );
int DECLSPEC_NAVSHEET PASCAL NAVOptionsSheetEx(HWND hwnd, LPNAVOPTIONS pNavOpts, int *iPanels, int iCount, int iActivePage, BOOL bIsNT );

#ifdef __cplusplus
}
#endif

#endif
