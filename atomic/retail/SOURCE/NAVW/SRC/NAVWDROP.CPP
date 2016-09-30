// Copyright 1995-1997 Symantec Corporation
//***************************************************************************
// 
// $Header:   S:/NAVW/VCS/navwdrop.cpv   1.2   07 Apr 1997 18:49:54   DALLEE  $ 
// 
// Description:                                                         
// 
// Contains:                                                            
// 
// See Also:                                                            
// 
//***************************************************************************
// $Log:   S:/NAVW/VCS/navwdrop.cpv  $ 
// 
//    Rev 1.2   07 Apr 1997 18:49:54   DALLEE
// Added call to BuildScanListIndex() to create new format input for
// ScanStart().
// 
//    Rev 1.1   06 Mar 1997 17:20:30   DALLEE
// Removed network and password stub references.
// Related to HYDRA.15 port: no longer required.
// 
//    Rev 1.0   06 Feb 1997 21:01:30   RFULLER
// Initial revision
// 
//    Rev 1.3   28 Jun 1996 13:03:14   MKEATIN
// Ported code from LuigiPlus
// 
//    Rev 1.2   24 Jun 1996 19:42:06   RHAYES
// Simple OEM mod.'s from Luigi.
// 
//    Rev 1.1   16 Apr 1996 15:05:14   YVANG
// Ported from Kirin.
// 
//    Rev 1.1   28 Mar 1996 14:18:52   jworden
// DBCS Enable
// 
//    Rev 1.0   30 Jan 1996 15:55:02   BARRY
// Initial revision.
// 
//    Rev 1.0   30 Jan 1996 14:50:06   BARRY
// Initial revision.
// 
//    Rev 1.19   11 Jul 1995 10:38:26   BARRY
// Commented additional paragraph being added to dwActualNumBytes (my change
// checked in by MARKL).
// 
//    Rev 1.18   11 Jul 1995 10:32:00   MARKL
// Fixed defect #33595.  We now make sure that we are made the foreground
// window after files have been dropped on us.
// 
//    Rev 1.17   06 Jul 1995 10:34:20   RHAYES
// Assures that no spec.'s are sent to ScanStart() that exceed the dreaded 260
// char. max. Also performed general cleanup.
// 
//    Rev 1.16   25 Jun 1995 09:41:52   KEITH
// Just adding/fixing the file header.
//***************************************************************************

#include "platform.h"
#include "xapi.h"
#include "file.h"
#include <shellapi.h>

#include "nonav.h"
#include "navw.h"
#include "nonav.h"
#include "navutil.h"

#include "navwstb.h"                    // NAV string table entries

//************************************************************************
// External data
//************************************************************************

extern          HINSTANCE           ghInst;


#ifndef NAVNETW

/* **************************************************************************
@Name:
VOID WINAPI DragDrop (                  // Handle Windows drag/drop message
    HWND        hWnd,                   // [in] window that received drop
    HDROP       hDrop,                  // [in] drop handle
    LPNAVMAIN   lpNavMain               // [in] our pointer handle
    );

@Description:
This function is called when NAV receives a WM_DROPFILES message from
Windows. This function queries Windows for all the dropped objects and
if they are valid they are scanned.

@Parameters:
$hWnd$      Handle to main NAV window that received drop message.
$hDrop$     handle to drop object to query.
$lpNavMain$ NAV structure pointer.

@Include: stdhelp.h
************************************************************************** */

VOID WINAPI DragDrop (                  // Handle Windows drag/drop message
    HWND        hWnd,                   // [in] window that received drop
    HDROP       hDrop,                  // [in] drop handle
    LPNAVMAIN   lpNavMain               // [in] our pointer handle
    )
{
    extern  NAVOPTIONS     navOptions;
    auto    LPTSTR          lpScanItem, lpScanList = NULL;
    auto    HGLOBAL        hScanList;
    auto    LPSCANSTRUCT   lpScan = NULL;
    auto    UINT           uNumFiles, uIndex;
    auto    DWORD          dwNumBytes, dwActualNumBytes;
    auto    UINT           uObjectLen;
    auto    BOOL           bScanOk = FALSE;
    auto    UINT           uBogusAttr;

#define LOCAL_MAX_STRING              256
    auto    TCHAR          szBuffer1[LOCAL_MAX_STRING],
                           szBuffer2[LOCAL_MAX_STRING],
                           szCaption[LOCAL_MAX_STRING];

                                        // Make buffer extra to allow for 
                                        // expansion via 
                                        // IfSpecialFileGetTrueFile()
    auto    TCHAR           szTemp [SYM_MAX_PATH * 2];

                                        // Ignore request if we are already
                                        // in the process of scanning
    if ( !lpNavMain->bScanInProgress )
        {
        uNumFiles = DragQueryFile (hDrop, (UINT)-1, NULL, 0);
                                        // -----------------------------------
                                        // We no longer calculate the total
                                        // of bytes since each file may expand
                                        // via IfSpecialFileGetTrueFile().
                                        // Therefore, we allocate the max.
                                        // size for each file and adjust the
                                        // allocation after all valid files
                                        // have been copied and the true size
                                        // is known.
                                        // -----------------------------------
        dwNumBytes = uNumFiles * ( SYM_MAX_PATH + 1);
        if ( ( lpScanList = (LPTSTR) MemAllocPtr( GHND, dwNumBytes ) ) &&
             ( lpScan = (LPSCANSTRUCT) MemAllocPtr( GHND, sizeof(SCANSTRUCT) ) ) )
            {
                                        // -----------------------------------
                                        // Now make a pass to copy all valid
                                        // file spec.'s to our global memory
                                        // buffer. Add in a paragraph to the
                                        // actual # bytes to assure that our
                                        // buffer contains extra NULL's 
                                        // if/when we realloc so that
                                        // GetBytesToScan() finds the end of
                                        // the list correctly.
                                        // -----------------------------------
            for ( dwActualNumBytes = 16L, uIndex = 0, lpScanItem = lpScanList;
                  uIndex < uNumFiles; uIndex++ )
                {
                                        // -----------------------------------
                                        // Make sure the object length is not
                                        // too large (before possible 
                                        // expansion).
                                        // -----------------------------------
                szTemp[0] = '\0';
                if ( ( DragQueryFile ( hDrop, uIndex, NULL, 0 ) )
                     < SYM_MAX_PATH )
                    {
                                        // Get the current object in temp. buf.
                    DragQueryFile( hDrop, uIndex, szTemp, sizeof(szTemp) );
                    SYM_ASSERT ( szTemp[0] );

                                        // -----------------------------------
                                        // Make sure we can actually scan this
                                        // file
                                        // -----------------------------------

                    if (NOERR != FileGetAttr (szTemp, &uBogusAttr))
                        {
                        if (GetCustomVersion () == VERSION_NETSCAPE)
                            LoadString ( ghInst, IDS_NAVW_INTERNET, szCaption, sizeof (szCaption));
                        else
                            LoadString ( ghInst, IDS_NAVW, szCaption, sizeof (szCaption));
                        LoadString (ghInst, IDS_GARBAGEFILENAME, szBuffer1, sizeof (szBuffer1));
                        SPRINTF (szBuffer2, szBuffer1, (LPSTR)szTemp);
                        MessageBox (hWnd, szBuffer2, szCaption, MB_OK);
                        }
                    else
                        {
                                            // -----------------------------------
                                            // Expand the file if *.Lnk/*.Pif.
                                            // Append a '\' if name contains only
                                            // a drive spec.
                                            // -----------------------------------
                        IfSpecialFileGetTrueFile ( szTemp );
                        SYM_ASSERT ( szTemp[0] );
                        if ( ( uObjectLen == 2 ) && ( NameHasDriveSpec( szTemp ) ) )
                            STRCAT( szTemp, "\\" );
                        uObjectLen = STRLEN ( szTemp );
                        SYM_ASSERT ( uObjectLen );

                                            // -----------------------------------
                                            // Copy the final file spec. to our
                                            // list if it's less than the max.
                                            // allowable. Adjust our list ptr. &
                                            // buffer size accordingly (including
                                            // NULL terminators).
                                            // -----------------------------------
                        if ( uObjectLen < SYM_MAX_PATH )
                            {
                            STRCPY ( lpScanItem, szTemp );
                            lpScanItem += ( uObjectLen + 1 );
                            dwActualNumBytes += ( uObjectLen + 1);
                            }
                        }
                    }
                }
                                        // -----------------------------------
                                        // Adjust the size of the allocated
                                        // file name buffer down if needed.
                                        // -----------------------------------
            if ( dwActualNumBytes < dwNumBytes )
                {
                lpScanList = (LPTSTR) MemReAllocPtr( lpScanList,
                                                    dwActualNumBytes, GHND );
                SYM_ASSERT( lpScanList );
                }
                                        // -----------------------------------
                                        // If no valid objects were found then
                                        // skip scanning
                                        // -----------------------------------
            if ( *lpScanList )
                {
                                        // -----------------------------------
                                        // hScanList is freed up in ScanStart
                                        // once the function is done with the
                                        // memory block
                                        // -----------------------------------
                SetForegroundWindow (hWnd);
                UpdateWindow (hWnd);

                                        // Memory will be released and freed at
                                        // the end of the scan.
                                        // Buffer must remain locked since new
                                        // lpScanItems has pointers into
                                        // buffer.
                hScanList = MemPtrHandle( lpScanList );
                //MemUnlock( hScanList = MemPtrHandle( lpScanList ) );

                SYM_ASSERT( hScanList );
                lpScan->hDlg           = hWnd;
                lpScan->wCallWhenDone  = 0;
                lpScan->hScanList      = hScanList;
                lpScan->bScanSubs      = TRUE;
                lpScan->lpNavOptions   = &navOptions;

                lpScan->lpScanItems    = BuildScanListIndex( lpScanList,
                                                             lpScan->bScanSubs );

                if ( NULL != lpScan->lpScanItems )
                    {
                    lpNavMain->hScanMain   = ScanStart (lpScan);
                    bScanOk = TRUE;
                    }
                }
            }
                                        // Perform memory cleanup if operation
                                        // failed for whatever reason.
        if ( !bScanOk )
            {
            if( lpScanList )
                MemFreePtr ( lpScanList );
            if ( lpScan )
                MemFreePtr ( lpScan );
            }
        }   // if ( !lpNavMain->bScanInProgress )
}

#endif

