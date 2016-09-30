// Copyright 1996 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/navwork/VCS/n_scanvw.cpv   1.0   06 Feb 1997 21:02:32   RFULLER  $
//
// Description:
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/navwork/VCS/n_scanvw.cpv  $
// 
//    Rev 1.0   06 Feb 1997 21:02:32   RFULLER
// Initial revision
// 
//    Rev 1.3   24 Jun 1996 21:37:04   RHAYES
// Add.'l OEM<->Ansi mod.'s (new and from Luigi).
// 
//    Rev 1.2   22 May 1996 17:47:54   RHAYES
// Round # 2 of Dave Allee's OEM<->Ansi mod.'s in Luigi.
// n_scanvw.cpp -> S:\NAVWORK\VCS\n_scanvw.cpv
// Enter description of changes.
// 
//    Rev 1.1   22 Feb 1996 14:08:14   YVANG
// Renamed 'nstdhelp.h' to 'stdhelp.h' to use Quake's StdHelp merged with the
// NAV version.
// 
//    Rev 1.0   30 Jan 1996 15:55:56   BARRY
// Initial revision.
// 
//    Rev 1.0   30 Jan 1996 14:50:58   BARRY
// Initial revision.
// 
//    Rev 1.0   21 Jan 1996 12:51:44   GWESTER
// Initial revision.
// 
//************************************************************************

#include "platform.h"
#include "commdlg.h"
#include "xapi.h"
#include "ngui.h"
#include "ctl3d.h"

#include "file.h"
#include "cmbstr.h"
#include "symcfg.h"
#include "stdhelp.h"

#include "navwork.h"
#include "navutil.h"
#include "options.h"
#include "exclude.h"
#include "nunavdlg.h"
#include "nunavstb.h"
#include "navw_hlp.h"
#include "navalog.h"

#include "stub.h"
#include "virscan.h"
#include "nonav.h"
#include "nunavdlg.h"
#include "nunavstb.h"

extern   HINSTANCE   hInstance;
VOID LOCAL ViewerDoFileError (LPNONAV lpNav);

// ************************************************************************ //

VOID SYM_EXPORT WINAPI ViewerDecideWhatToDo (LPNONAV lpNav)
{
    auto    NOTIFYSTRUCT        Notify;

    MEMSET (&Notify, '\0', sizeof(Notify));
    lpNav->Infected.wStatus = FILESTATUS_INFECTED;

    if (lpNav->Infected.Notes.wVirID == (WORD)-1)
        {                                // --------------------------
        ViewerDoFileError (lpNav);       // Internal DOS Error occured
        }                                // --------------------------
    else if (lpNav->Infected.Notes.wVirID != 0)
        {
        lpNav->Stats.Files.uInfected++;

        ScanTime(lpNav->hMommyWindow, FALSE);
        VirusFound (lpNav);
        ScanTime(lpNav->hMommyWindow, TRUE);
        }
}

// -------------------------------------------------------------------------- //


VOID SYM_EXPORT WINAPI ViewerDecideWhatToDoWithZip (LPNONAV lpNav)
{
    auto    NOTIFYSTRUCT    Notify;
    auto    char            szFileName [ SYM_MAX_PATH+1 ];

    MEMSET (&Notify, '\0', sizeof(Notify));

                                        // ----------------------------
                                        // Construct display version of
                                        // zip filename.
                                        // And fill in lpNav->Infected
                                        // ----------------------------
    STRCPY (lpNav->Infected.szFullOemPath, lpNav->szTheDirectory);

    NameReturnFile (lpNav->Zip.lpZipFileName, szFileName);
    NameAppendFile (lpNav->Infected.szFullOemPath, szFileName);

    ZipNameAppendChildToParent ( lpNav->Infected.szFullOemPath,
                                 lpNav->Zip.szZipChildFile,
                                 sizeof ( lpNav->Infected.szFullOemPath ) );

#ifdef SYM_WIN32
    NavOemToAnsi(lpNav->Infected.szFullOemPath, lpNav->Infected.szFullPath);
#endif
    lpNav->Infected.wStatus = FILESTATUS_ZIPINFECTED;


    if (lpNav->Infected.Notes.wVirID == (WORD)-1)
        {                                // --------------------------
        ViewerDoFileError (lpNav);       // Internal DOS Error occured
        }                                // --------------------------
    else if (lpNav->Infected.Notes.wVirID != 0)
        {
        lpNav->Stats.Files.uInfected++;

        ScanTime(lpNav->hMommyWindow, FALSE);
        VirusFound (lpNav);
        ScanTime(lpNav->hMommyWindow, TRUE);
        }
}

VOID LOCAL ViewerDoFileError (LPNONAV lpNav)
{
    auto    NOTIFYSTRUCT   Notify   ;

    MEMSET (&Notify, '\0', sizeof(Notify));

    STRCPY(lpNav->Infected.Notes.lpVirName, String_Ptr(IDS_DOSERROR));
    lpNav->Infected.wStatus = FILESTATUS_ERROR;

    ScanTime(lpNav->hMommyWindow, FALSE);
    VirusFound (lpNav);
    ScanTime(lpNav->hMommyWindow, TRUE);
}

