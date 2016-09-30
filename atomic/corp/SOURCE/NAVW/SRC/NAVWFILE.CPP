// Copyright 1992-1997 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/NAVW/VCS/navwfile.cpv   1.5   01 Apr 1998 15:19:16   DALLEE  $
//
// Description:
//      This file contians those routines necessary for handling the
// selection of file(s) for the main dialog window of the Norton AntiVirus
// for windows.
//
// Contains (as exports):
//      ScanFileHookProc(),
//
// See Also:
//      Navwmcmd.cpp
//************************************************************************
// $Log:   S:/NAVW/VCS/navwfile.cpv  $
// 
//    Rev 1.5   01 Apr 1998 15:19:16   DALLEE
// Turned off optimizations around wonky code.
// Stuff in here has buffers, pointers to the buffer, and pointers to the
// pointers. All being modified in the same function.
// MSVC was getting confused.
// 
//    Rev 1.4   27 Jul 1997 14:26:50   BGERHAR
// Added Scan:File support for Compressed Files
// 
//    Rev 1.3   15 Jul 1997 14:12:24   DBUCHES
// Added Ctl3d for DEC Alpha
// 
//    Rev 1.2   09 Jun 1997 16:54:46   DBUCHES
// Ifdef'd out Ctl3d stuff for non-intel platfomrs.
// 
//    Rev 1.1   06 Mar 1997 17:20:38   DALLEE
// Removed network and password stub references.
// Related to HYDRA.15 port: no longer required.
// 
//    Rev 1.0   06 Feb 1997 21:01:32   RFULLER
// Initial revision
// 
//    Rev 1.7   18 Jul 1996 11:16:50   TIVANOV
// fix the file type filter if there are no program extension entries
//************************************************************************

#include "platform.h"                   // Platform independence
#include "dlgs.h"
#include "xapi.h"
#include "ngui.h"                       // Norton look / feel
#include "ctl3d.h"                      // Microsoft 3D look
#include "commdlg.h"                    // Windows 3.1 Common dialog
#include "disk.h"                       // For drive types
#include "ndrvlist.h"                   // For Norton drive list control
#include "msgdisp.h"                    // For Quake Message Display API

#ifdef SYM_WIN16
    #include "syminteg.h"               // For network routines
#endif

#include "nonav.h"
#include "stdhelp.h"
#include "navw.h"                       // NAV Windows Clinic stuff
#include "navw_hlp.h"                   // Help ids
#include "navwres.h"                    // NAV resource identifiers
#include "navwstb.h"                    // NAV string table entries


//************************************************************************
// Local definitions
//************************************************************************

// New file extensions added for 5.02 and this was only defined as 256
// which caused buffer overflows galore.  I'm not sure how big this needs
// to be now, but this should hold us for a while.
#define FILTER_LIST_SIZE        1024

//************************************************************************
// Local Prototypes
//************************************************************************

VOID LOCAL NavScanFileFilterListPrograms ( // Adds in "Program Files" filter
    LPTSTR       lpszzFilterList,        // [in/out] buffer for filter list
    LPTSTR FAR  *lplpszzFilterListPos);  // [in/out] where to start/stop

VOID LOCAL NavScanFileFilterListZips (   // Adds in "Compressed Files" filter
    LPTSTR       lpszzFilterList,        // [in/out] buffer for filter list
    LPTSTR FAR  *lplpszzFilterListPos);  // [in/out] where to start/stop

VOID LOCAL NavScanFileFilterListAll (   // Adds in "All Files" filter
    LPTSTR       lpszzFilterList,        // [in/out] buffer for filter list
    LPTSTR FAR  *lplpszzFilterListPos);  // [in/out] where to start/stop


/* **************************************************************************

@Name:
BOOL WINAPI NavMainWndScanFile (        // Scans file specified by user
    HWND        hWnd,                   // [in] Main dialog window handle
    LPNAVMAIN   lpNav,                  // [in] pointer to nav structure
    LPTSTR       lpFilePath              // [out] full path to the file
    );

@Description:
This routine prompts the user via a standard browse dialog for the file
he wishes to scan.  If the user enters in a valid file specification,
Norton AntiVirus for Windows should scan the file specified.

NOTE:  Since multiple files may be selected, the lpstrFile member of
OFName will point to a buffer containing the path to the current
directory and all filenames in the selection.  The first filename
is separated from the path by a space.  Each subsequent filename
is seperated by one space from the preceding filename.  Some of the
selected filenames may be preceded by relative paths; for example,
the buffer could contain something like this:
     "c:\files file1.txt file2.txt ..\bin\file3.txt"

@Parameters:
$hWnd$            [in] Window handle of main dialog window
$lpNavScanSpec$   [in/out] Specifies what to scan

@Returns:
      TRUE            Time to begin scan of file(s)
      FALSE           Nothing to scan

************************************************************************** */

#pragma optimize( "", off )
BOOL WINAPI NavMainWndScanFile (        // Scans file specified by user
    HWND        hWnd,                   // [in] Main dialog window handle
    LPNAVMAIN   lpNav,                  // [in] pointer to nav structure
    LPTSTR      lpFilePath,             // [out] full path to the file
    UINT        uFilePathLength         // [in] length of above
    )
{
    OPENFILENAME OFName;                // Common dialog
    BYTE         szzFilterSpec[FILTER_LIST_SIZE]; // Filter list spec
    BYTE         szTitle[40];           // Dialog Title
    BOOL         bRet = FALSE;          // Return code
    extern       HINSTANCE      ghInst;
                                        // -----------------------------------
                                        // Build filter list specification
                                        // for the common dialog box.
                                        // -----------------------------------

    NavScanFileFilterList ( lpNav->Scan.bScanAll, (LPTSTR)szzFilterSpec );

                                        // -----------------------------------
                                        // Load our title from the string
                                        // table.
                                        // -----------------------------------
    LoadString (ghInst,
                IDS_COMMDLG_TITLE_SCAN_FILE,
                (LPTSTR)szTitle,
                sizeof (szTitle));

    *lpFilePath = EOS;
                                        // -----------------------------------
                                        // Set up the OFName structure such
                                        // that common dialog may prompt for
                                        // the files to scan.
                                        // -----------------------------------
    OFName.lStructSize       = sizeof (OFName);
    OFName.hwndOwner         = hWnd;
    OFName.hInstance         = ghInst;
    OFName.lpstrFilter       = (LPTSTR)szzFilterSpec;
    OFName.lpstrCustomFilter = (LPTSTR)NULL;
    OFName.nMaxCustFilter    = 0;
    OFName.nFilterIndex      = 0;
    OFName.lpstrFile         = lpFilePath;
    OFName.nMaxFile          = uFilePathLength;
    OFName.lpstrFileTitle    = (LPTSTR)NULL;
    OFName.nMaxFileTitle     = 0;
    OFName.lpstrInitialDir   = NULL;
    OFName.lpstrTitle        = (LPTSTR)szTitle;
    OFName.lpfnHook          = NULL;

#ifdef SYM_WIN32
                                        // NT doesn't allow explorer flag
                                        // with hook.
                                        // Hook doesn't catch CDN_HELP so
                                        // I removed it.  -DALLEE 09/22/95
    OFName.Flags             = OFN_FILEMUSTEXIST
                             | OFN_PATHMUSTEXIST
                             | OFN_HIDEREADONLY
                             | OFN_EXPLORER
                             | OFN_LONGNAMES;
#else
    OFName.Flags             = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST |
                                OFN_HIDEREADONLY | OFN_SHOWHELP ;
#endif

    OFName.lpstrDefExt       = (LPTSTR)szzFilterSpec;
    OFName.lCustData         = NULL;
    OFName.lpTemplateName    = (LPTSTR)NULL;

    if (GetOpenFileName (&OFName))
        {
                                        // ----------------------------------
                                        // Drive letter should be upper case.
                                        // ----------------------------------
        *lpFilePath = CharToUpper ( *lpFilePath );

        bRet = TRUE;
        }

    return ( bRet );
}
#pragma optimize( "", on )

/* **************************************************************************

@Name:
VOID LOCAL NavScanFileFilterList (      // Builds filter list for scan dlg
    LPNAVMAIN   lpNav,                  // Options is contained in here
    LPTSTR       lpszzFilterList         // [out] buffer to receive filter lst
    );

@Description:
This routine builds a valid filter list for the Scan File dialog of the
main window of the Norton AntiVirus for Windows.

NOTE 1:  When building the filter list for the common dialog box, we must
take into account how gNavOptions.szzProgramExts is stored and how
OFName.lpstrFilter expects the list to appear.  gNavOptions.szzProgramExts
stores the list of valid program files as:
   "COM|EXE|OV?|SYS|DRV|"    (note that '|' is an EOS)
OFName.lpstrFilter expects the following for the given example:
   "Program Files|*.COM;*.EXE;*.OV?;*.SYS;*.DRV|All Files (*.*)|*.*|"

NOTE 2:  If gNavOptions.bProgramFilesOnly is TRUE, then make "Program
Files" the first filter; if FALSE, make "All Files" the first filter.

@Parameters:
$lpszzFilterList$     [out] buffer to receive filter lst

@Returns:   Nothing

************************************************************************** */

#pragma optimize( "", off )
VOID WINAPI NavScanFileFilterList (     // Builds filter list for scan dlg
    BOOL        bScanAll,               // [in] 'All files' should go first.
    LPTSTR      lpszzFilterList         // [out] buffer to receive filter lst
    )
{
    LPTSTR lplpszzFilterListPos;

                                        // -----------------------------------
                                        // Build filter list specification
                                        // for the common dialog box.
                                        // -----------------------------------

    lplpszzFilterListPos = lpszzFilterList;
    *lplpszzFilterListPos = EOS;

    if ( bScanAll == FALSE )
        {
                                        // -----------------------------------
                                        // Put "Program Files" before
                                        // "All Files".
                                        // -----------------------------------
        NavScanFileFilterListPrograms ( lpszzFilterList,
                                       &lplpszzFilterListPos
                                       );

#ifndef _ALPHA_                         // Don't support ZIP on DEC yet.
        NavScanFileFilterListZips ( lpszzFilterList,
                                    &lplpszzFilterListPos
                                    );
#endif

        NavScanFileFilterListAll ( lpszzFilterList,
                                  &lplpszzFilterListPos
                                  );
        }
    else
        {
                                        // -----------------------------------
                                        // Put "All Files" before
                                        // "Program Files".
                                        // -----------------------------------
        NavScanFileFilterListAll ( lpszzFilterList,
                                   &lplpszzFilterListPos
                                   );

        NavScanFileFilterListPrograms ( lpszzFilterList,
                                       &lplpszzFilterListPos
                                       );

#ifndef _ALPHA_                         // Don't support ZIP on DEC yet.
        NavScanFileFilterListZips ( lpszzFilterList,
                                    &lplpszzFilterListPos
                                    );
#endif

        }

    *lplpszzFilterListPos = EOS;
}
#pragma optimize( "", on )

/* **************************************************************************

@Name:
VOID LOCAL NavScanFileFilterListPrograms ( // Adds in "Program Files" filter
    LPTSTR       lpszzFilterList,        // [in/out] buffer for filter list
    LPTSTR FAR  *lplpszzFilterListPos    // [in/out] where to start/stop
    )

@Description:
This routine adds to the current filter list the "Program Files" filter
list.

NOTE 1:  When building the filter list for the common dialog box, we must
take into account how gNavOptions.szzProgramExts is stored and how
OFName.lpstrFilter expects the list to appear.  gNavOptions.szzProgramExts
stores the list of valid program files as:
   "COM|EXE|OV?|SYS|DRV|"    (note that '|' is an EOS)
OFName.lpstrFilter expects the following for the given example:
   "Program Files|*.COM;*.EXE;*.OV?;*.SYS;*.DRV|All Files (*.*)|*.*|"

@Parameters:
$lpszzFilterList$       [in/out] References the very beginning of the
                        current filter list.
$lplpszzFilterListPos$  [in/out] Tells routine where to begin adding
                        filter list entry and then informs caller
                        where next entry may be added later.

@Returns:   Nothing

************************************************************************** */

#pragma optimize( "", off )
VOID LOCAL NavScanFileFilterListPrograms ( // Adds in "Program Files" filter
    LPTSTR       lpszzFilterList,        // [in/out] buffer for filter list
    LPTSTR FAR  *lplpszzFilterListPos    // [in/out] where to start/stop
    )
{
#define PROGRAM_EXT_SIZE        (sizeof("*.XXX;")-1)

    extern  HINSTANCE   ghInst;
    extern  HWND        ghWndMain;
    extern  NAVMAIN     NavMain;

    static  LPTSTR       lpExt[] =
        {
        "COM",
        "EXE",
        "OV?",
        "SYS",
        "DLL",
        "386",
        "VXD",
        NULL
        };

	HWND		hWnd = ghWndMain;
	LPNAVOPTIONS	lpNav = &navOptions;

                                            //check for empty program extension list
    if (lpNav->general.szExt[0])
    {
        lpNav = lpNav;
                                            // --------------------------------
                                            // First place the "Program Files"
                                            // prompt in the list as the first
                                            // entry.
                                            // --------------------------------

        LoadString ( ghInst, IDS_COMMDLG_SCANFILE_PROGRAMS,
                    (LPTSTR)*lplpszzFilterListPos, FILTER_LIST_SIZE );

                                            // --------------------------------
                                            // Now build a list a valid filters
                                            // for program files based on what
                                            // is in the gNavOptions record.
                                            // --------------------------------

        *lplpszzFilterListPos += (STRLEN (*lplpszzFilterListPos) + 1);
        *(*lplpszzFilterListPos) = EOS;

#if 0
        for ( UINT uIndex = 0; lpExt[uIndex]; uIndex++ )
            {
            STRCAT (*lplpszzFilterListPos, "*.");
            STRCAT (*lplpszzFilterListPos, lpExt[uIndex] );

            if ( lpExt[uIndex+1] )
                STRCAT (*lplpszzFilterListPos, ";");
            }

        *lplpszzFilterListPos += ( STRLEN (*lplpszzFilterListPos) + 1 );
#endif

        //Picking up the extenstions from the options.
        //
        // Opitons->General.szExt contains a list of extensions 3 chars
        // seperated by space
        //
        for (WORD wPos = 0;                         // start from 0th pos
                lpNav->general.szExt[wPos];         // not EOS
                wPos += SIZE_EXTENSION + 1)         // Skip the current ext
        {
            STRCAT (*lplpszzFilterListPos, "*.");
            // Add the 3 char extension
            STRNCAT (*lplpszzFilterListPos,
                        &lpNav->general.szExt[wPos],
                        SIZE_EXTENSION);
            *(lplpszzFilterListPos + SIZE_EXTENSION) = EOS;

            // If we have next extension, then add a ';'
            if (lpNav->general.szExt[wPos + SIZE_EXTENSION])
                STRCAT (*lplpszzFilterListPos, ";");
            else
                break;  //break if this is the end of the file extentions
        }
        *lplpszzFilterListPos += (STRLEN (*lplpszzFilterListPos) + 1);
    }
}
#pragma optimize( "", on )

/* **************************************************************************

@Name:
VOID LOCAL NavScanFileFilterListZips (   // Adds in "Compressed Files" filter
    LPTSTR       lpszzFilterList,        // [in/out] buffer for filter list
    LPTSTR FAR  *lplpszzFilterListPos    // [in/out] where to start/stop
    )

@Description:
This routine adds to the current filter list the "Compressed Files" filter list.

@Parameters:
$lpszzFilterList$       [in/out] References the very beginning of the
                        current filter list.
$lplpszzFilterListPos$  [in/out] Tells routine where to begin adding
                        filter list entry and then informs caller
                        where next entry may be added later.

@Returns:   Nothing

************************************************************************** */

#pragma optimize( "", off )
VOID LOCAL NavScanFileFilterListZips (   // Adds in "All Files" filter
    LPTSTR       lpszzFilterList,        // [in/out] buffer for filter list
    LPTSTR FAR  *lplpszzFilterListPos    // [in/out] where to start/stop
    )
{
    extern       HINSTANCE      ghInst;

                                        // -----------------------------------
                                        // Parameter not used. Compiler cryes.
                                        // Make compiler shut up.
                                        // -----------------------------------
    if (lpszzFilterList)
       ;

    LoadString (ghInst,
                IDS_COMMDLG_SCANFILE_ZIPFILES,
                *lplpszzFilterListPos,
                FILTER_LIST_SIZE);

    while (*(*lplpszzFilterListPos) != EOS)
        {
        if (*(*lplpszzFilterListPos) == _T('|'))
            {
            *(*lplpszzFilterListPos) = EOS;
            }
        *lplpszzFilterListPos =  AnsiNext (*lplpszzFilterListPos);
		if (*(*lplpszzFilterListPos) == EOS)
			(*lplpszzFilterListPos) ++;	// Skip \0
        }

} // NavScanFileFilterListZips()
#pragma optimize( "", on )

/* **************************************************************************

@Name:
VOID LOCAL NavScanFileFilterListAll (   // Adds in "All Files" filter
    LPTSTR       lpszzFilterList,        // [in/out] buffer for filter list
    LPTSTR FAR  *lplpszzFilterListPos    // [in/out] where to start/stop
    )

@Description:
This routine adds to the current filter list the "All Files" filter list.

@Parameters:
$lpszzFilterList$       [in/out] References the very beginning of the
                        current filter list.
$lplpszzFilterListPos$  [in/out] Tells routine where to begin adding
                        filter list entry and then informs caller
                        where next entry may be added later.

@Returns:   Nothing

************************************************************************** */

#pragma optimize( "", off )
VOID LOCAL NavScanFileFilterListAll (   // Adds in "All Files" filter
    LPTSTR       lpszzFilterList,        // [in/out] buffer for filter list
    LPTSTR FAR  *lplpszzFilterListPos    // [in/out] where to start/stop
    )
{
    extern       HINSTANCE      ghInst;

                                        // -----------------------------------
                                        // Parameter not used. Compiler cryes.
                                        // Make compiler shut up.
                                        // -----------------------------------
    if (lpszzFilterList)
       ;

    LoadString (ghInst,
                IDS_COMMDLG_SCANFILE_ALLFILES,
                *lplpszzFilterListPos,
                FILTER_LIST_SIZE);

    while (*(*lplpszzFilterListPos) != EOS)
        {
        if (*(*lplpszzFilterListPos) == _T('|'))
            {
            *(*lplpszzFilterListPos) = EOS;
            }
        *lplpszzFilterListPos =  AnsiNext (*lplpszzFilterListPos);
		if (*(*lplpszzFilterListPos) == EOS)
			(*lplpszzFilterListPos) ++;	// Skip \0
        }

} // NavScanFileFilterListAll()
#pragma optimize( "", on )

