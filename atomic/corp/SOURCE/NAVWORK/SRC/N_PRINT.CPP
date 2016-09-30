// Copyright 1994 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/NAVWORK/VCS/n_print.cpv   1.2   26 Jun 1998 15:13:04   jtaylor  $
//
// Description:
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/NAVWORK/VCS/n_print.cpv  $
// 
//    Rev 1.2   26 Jun 1998 15:13:04   jtaylor
// Dissabled Quaranine items if Quarantine is not enabled.
//
//    Rev 1.1   18 May 1998 23:01:56   tcashin
// Make the printed scan summary match the new scan summary dialog box.
//
//    Rev 1.0   06 Feb 1997 21:02:40   RFULLER
// Initial revision
//
//    Rev 1.3   03 Feb 1997 10:52:10   JBRENNA
// BuildResultInfo(), do not output MBR or BR information when on a NEC
// machine. We are not scanning these on NEC.
//
//    Rev 1.2   07 Aug 1996 15:49:02   JBRENNA
// LuigiPlus Port:
//   DALLEE: Bullet proof routines to get filestatus status and help strings.
//     Returns "" instead of trying to load string ID 0.
//
//
//    Rev 1.1   04 Apr 1996 16:50:02   YVANG
// Ported from Kirin.
//
//    Rev 1.1   03 Apr 1996 14:53:12   YVANG
// DBCS enable.
//
//    Rev 1.0   30 Jan 1996 15:55:36   BARRY
// Initial revision.
//
//    Rev 1.0   30 Jan 1996 14:50:40   BARRY
// Initial revision.
//
//    Rev 1.10   14 Jan 1996 16:50:12   DALLEE
// Changed #ifdef NAVWNT to NAVSCANW.
//
//    Rev 1.9   06 Nov 1995 13:51:40   DALLEE
// For NAVWNT: removed memory and inoculation text/buttons from dialogs.
//
//    Rev 1.8   09 May 1995 10:30:42   KEITH
// Correct some nouns
//
//************************************************************************

#include "platform.h"
#include "hstring.h"
#include "xapi.h"
#include "windowsx.h"
#include "symcfg.h"

#include "cmbstr.h"
#include "nunavdlg.h"
#include "nunavstb.h"
#include "navutil.h"
#include "options.h"

#ifdef SYM_WIN16
    #include "syminteg.h"
#endif

#ifndef SYM_DOS
    #include "n32behav.h" // Personality DLL
#endif

#include "stub.h"
#include "nonav.h"
#include "dbcs.h"

extern HINSTANCE hInstance;

#define SIZE_PRINT_BUFFER  50000
#define CRLF "\r\n"
#define COMMENT_WIDTH   70

HPSTR LOCAL ReallocIfRequired (
    HGLOBAL FAR *hMem,
    DWORD   dwBytesUsed,
    LPDWORD dwBufferSize,
    HPSTR   hpStr
    );

UINT LOCAL SetTab (
    UINT    uCharInLine,
    HPSTR   hpMem,
    UINT    uTab
    );

VOID WINAPI BuildRestOfVirusInfo (
    HWND    hDlg,
    HPSTR   hpMem,
    DWORD   i,
    BOOL    bMacType
    );

UINT WINAPI BuildPrintHeader (
    HWND    hDlg,
    LPSTR   lpMem
    );

// ************************************************************************ //
// ****        (C) Copyright Symantec Corporation 19xx - 1993          **** //
// ************************************************************************ //
// ****                                                                **** //
// ****  Procedure:                                                    **** //
// ****                                                                **** //
// ****  Author: Martin Fallenstedt          Date: January 8, 1993     **** //
// ****                                                                **** //
// ****  Functionality:                                                **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Input:                                                        **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Output:                                                       **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Update History:                                               **** //
// ****                                                                **** //
// ****                                                                **** //
// ************************************************************************ //

HGLOBAL SYM_EXPORT WINAPI BuildNavVirusInfo (HWND hDlg, BOOL bFileName)
{
    auto    HGLOBAL  hMem;
    auto    HPSTR    lpMem;
    auto    DWORD    i = 0;
    auto    UINT     uLength;
    auto    UINT     uCharInLine;


    if ((hMem = MemAlloc(GHND, SIZE_PRINT_BUFFER)) != NULL)
        {
        lpMem = (HPSTR)MemLock (hMem);

        i = BuildPrintHeader(hDlg, lpMem);

        if (bFileName == TRUE)
            {
            // **************************** //
            // ****     File Name      **** //
            // **************************** //
            uCharInLine = 0;
            uLength = GetWindowText (GetDlgItem(hDlg, IDI_VI_FNAMETXT),
                                    &lpMem[i], (UINT)(SIZE_PRINT_BUFFER - i));

            i += uLength;
            uCharInLine += uLength;

            i += SetTab(uCharInLine, &lpMem[i], 15);

            i += GetWindowText (GetDlgItem(hDlg, IDI_VI_FNAME), &lpMem[i],
                                (UINT)(SIZE_PRINT_BUFFER - i));

            STRCPY (&lpMem[i], CRLF);
            i += sizeof(CRLF)-1;

            // **************************** //
            // ****     File Status    **** //
            // **************************** //

            uCharInLine = 0;
            uLength = GetWindowText (GetDlgItem(hDlg, IDI_VI_STATUSTXT),
                                    &lpMem[i], (UINT)(SIZE_PRINT_BUFFER - i));
            i += uLength;
            uCharInLine += uLength;

            i += SetTab(uCharInLine, &lpMem[i], 15);

            i += GetWindowText (GetDlgItem(hDlg, IDI_VI_STATUS), &lpMem[i],
                                (UINT)(SIZE_PRINT_BUFFER - i));

            STRCPY (&lpMem[i], CRLF);
            i += sizeof(CRLF)-1;
            }

        BuildRestOfVirusInfo(hDlg, lpMem, i, FALSE);

        MemUnlock (hMem);
        }

    return (hMem);
}


// ************************************************************************ //
// ****        (C) Copyright Symantec Corporation 19xx - 1993          **** //
// ************************************************************************ //
// ****                                                                **** //
// ****  Procedure:                                                    **** //
// ****                                                                **** //
// ****  Author: Martin Fallenstedt          Date: January 8, 1993     **** //
// ****                                                                **** //
// ****  Functionality:                                                **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Input:                                                        **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Output:                                                       **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Update History:                                               **** //
// ****                                                                **** //
// ****                                                                **** //
// ************************************************************************ //

HGLOBAL SYM_EXPORT WINAPI BuildResultInfo (HWND hDlg, LPNONAV lpNav)
{
    auto    LPINFECTEDFILE  lpInfected;
    auto    LPINFARR        lpInfArr;
    auto    HGLOBAL         hMem;
    auto    HPSTR           lpMem;
    auto    HPSTR           hpStr;
    auto    DWORD           dwBufferSize = SIZE_PRINT_BUFFER;
    auto    DWORD           dwBytesUsed = 0;
    auto    UINT            i;
    auto    UINT            uCharInLine;
    auto    UINT            uLength;
    auto    char            szZipPath   [ SYM_MAX_PATH+1 ];
    auto    char            szFileName  [ SYM_MAX_PATH+1 ];
    auto    char            szWorkBuff  [ 512 ];
    auto    LPSTR           lpWorkPtr;
    auto    CString         strMBR1;
    auto    CString         strMBR2;
    auto    BOOL            bFoundString = FALSE;
    auto    CString         strTempBuf;


    if ((hMem = MemAlloc(GHND, SIZE_PRINT_BUFFER)) != 0)
        {
        hpStr = lpMem = (HPSTR)MemLock (hMem);

        hpStr += BuildPrintHeader(hDlg, hpStr);

        // **************************** //
        // ****      Summary       **** //
        // **************************** //

        uCharInLine = 0;
        uLength = GetWindowText (GetDlgItem(hDlg, ID_SS_SCANSUMTXT), szZipPath,
                                 sizeof(szZipPath));

        hmemcpy (hpStr, szZipPath, uLength);

        hpStr       += uLength;
        uCharInLine += uLength;

        hpStr += (DWORD)SetTab(uCharInLine, hpStr, 20);

        uLength = GetWindowText (GetDlgItem(hDlg, ID_SS_SCANSUM), szZipPath,
                                 sizeof(szZipPath));

        hmemcpy (hpStr, szZipPath, uLength);
        hpStr += uLength;

        hmemcpy (hpStr, CRLF, sizeof(CRLF));
        hpStr += sizeof(CRLF)-1;

        // **************************** //
        // ****    Items Scanned   **** //
        // **************************** //

        uCharInLine = 0;
        uLength = GetWindowText (GetDlgItem(hDlg, ID_SS_ITSCANTXT), szZipPath,
                                 sizeof(szZipPath));

        hmemcpy (hpStr, szZipPath, uLength);
        hpStr       += uLength;
        uCharInLine += uLength;

        hpStr += SetTab(uCharInLine, hpStr, 20);

        uLength = GetWindowText (GetDlgItem(hDlg, ID_SS_ITSCAN), szZipPath,
                                 sizeof(szZipPath));

        hmemcpy (hpStr, szZipPath, uLength);
        hpStr += uLength;

        hmemcpy (hpStr, CRLF, sizeof(CRLF));
        hpStr += sizeof(CRLF)-1;

        // ***************************************** //
        // ****    Memory Scanned - Not on NT   **** //
        // ***************************************** //

        if (SystemGetWindowsType() != SYM_SYSTEM_WIN_NT)
        {
            uCharInLine = 0;
            uLength = GetWindowText (GetDlgItem(hDlg, ID_SS_MEMTXT), szZipPath,
                                     sizeof(szZipPath));

            hmemcpy (hpStr, szZipPath, uLength);
            hpStr       += uLength;
            uCharInLine += uLength;

            hpStr += SetTab(uCharInLine, hpStr, 20);

            uLength = GetWindowText (GetDlgItem(hDlg, ID_SS_MEMSCAN), szZipPath,
                                     sizeof(szZipPath));

            hmemcpy (hpStr, szZipPath, uLength);
            hpStr += uLength;

            hmemcpy (hpStr, CRLF, sizeof(CRLF));
            hpStr += sizeof(CRLF)-1;
        }

        // ************************* //
        // ****     File Type   **** //
        // ************************* //

        uCharInLine = 0;
        uLength = GetWindowText (GetDlgItem(hDlg, ID_SS_SCANTYPTXT), szZipPath,
                                sizeof(szZipPath));

        hmemcpy (hpStr, szZipPath, uLength);
        hpStr       += uLength;
        uCharInLine += uLength;

        hpStr += SetTab(uCharInLine, hpStr, 20);

        uLength = GetWindowText (GetDlgItem(hDlg, ID_SS_SCANTYP), szZipPath,
                                  sizeof(szZipPath));

        hmemcpy (hpStr, szZipPath, uLength);
        hpStr += uLength;

        hmemcpy (hpStr, CRLF, sizeof(CRLF));
        hpStr += sizeof(CRLF)-1;

        // ************************* //
        // ****   Inoculation   **** //
        // ************************* //
/*
        if (SystemGetWindowsType() != SYM_SYSTEM_WIN_NT)
        {
            uCharInLine = 0;
            uLength = GetWindowText (GetDlgItem(hDlg, ID_SS_INOCTXT), szZipPath,
                                    sizeof(szZipPath));


            hmemcpy (hpStr, szZipPath, uLength);
            hpStr       += uLength;
            uCharInLine += uLength;

            hpStr += SetTab(uCharInLine, hpStr, 20);

            uLength = GetWindowText (GetDlgItem(hDlg, ID_SS_INOC), szZipPath,
                                    sizeof(szZipPath));

            hmemcpy (hpStr, szZipPath, uLength);
            hpStr += uLength;

            hmemcpy (hpStr, CRLF, sizeof(CRLF));
            hpStr += sizeof(CRLF)-1;
        }
*/
        // ************************** //
        // ****  Other Settings  **** //
        // ************************** //

        uCharInLine = 0;
        uLength = GetWindowText (GetDlgItem(hDlg, ID_SS_OTHERTXT), szZipPath,
                                 sizeof(szZipPath));

        hmemcpy (hpStr, szZipPath, uLength);
        hpStr       += uLength;
        uCharInLine += uLength;

        hpStr += SetTab(uCharInLine, hpStr, 20);

        uLength = GetWindowText (GetDlgItem(hDlg, ID_SS_OTHER), szZipPath,
                                sizeof(szZipPath));

        hmemcpy (hpStr, szZipPath, uLength);
        hpStr += uLength;

        hmemcpy (hpStr, CRLF, sizeof(CRLF));
        hpStr += sizeof(CRLF)-1;

        // ********************* //
        // ****  Scan Time  ****
        // ********************* //

        uCharInLine = 0;
        uLength = GetWindowText (GetDlgItem(hDlg, ID_SS_SCANTIMTXT), szZipPath,
                                 sizeof(szZipPath));

        hmemcpy (hpStr, szZipPath, uLength);
        hpStr       += uLength;
        uCharInLine += uLength;

        hpStr += SetTab(uCharInLine, hpStr, 20);

        uLength = GetWindowText (GetDlgItem(hDlg, ID_SS_SCANTIM), szZipPath,
                                sizeof(szZipPath));

        hmemcpy (hpStr, szZipPath, uLength);
        hpStr += uLength;
        hmemcpy (hpStr, CRLF, sizeof(CRLF));
        hpStr += sizeof(CRLF)-1;
        hmemcpy (hpStr, CRLF, sizeof(CRLF));
        hpStr += sizeof(CRLF)-1;

        // ********************************************** //
        // ****  Above spread sheet type cells text  **** //
        // ********************************************** //

        // ****  Get the MBR string first since it wraps within
        // ****  its column.

        uCharInLine = 0;

        uLength = SetTab(uCharInLine, hpStr, 33);
        hpStr       += uLength;
        uCharInLine += uLength;

        uLength = GetWindowText (GetDlgItem(hDlg, ID_SS_MBRECTXT), szZipPath,
                                sizeof(szZipPath));

        // **** Build the first heading line

        strMBR1 = szZipPath;
        strMBR2 = szZipPath;

        for (i=0; i<strMBR1.GetLength();i++)
        {
            if (strMBR1.GetAt(i) == ' ')
            {
                if (bFoundString == TRUE)
                    break;
            }
            else
            {
                if (bFoundString == FALSE)
                    bFoundString = TRUE;
            }
        }

        strTempBuf = strMBR1.Left(i);
        strMBR2 = strMBR1.Right(strMBR1.GetLength() - i);

        lpWorkPtr = strTempBuf.GetBuffer(strTempBuf.GetLength()+1);
        memcpy(szZipPath, lpWorkPtr, strTempBuf.GetLength()+1);

        uLength = strTempBuf.GetLength();

        hmemcpy (hpStr, szZipPath, uLength);
        hpStr       += uLength;
        uCharInLine += uLength;

        hmemcpy (hpStr, CRLF, sizeof(CRLF));
        hpStr += sizeof(CRLF)-1;

        // **** Now the build the second heading line.

        uCharInLine = 0;
        uLength = GetWindowText (GetDlgItem(hDlg, ID_SS_ITEMTXT), szZipPath,
                                sizeof(szZipPath));

        hmemcpy (hpStr, szZipPath, uLength);
        hpStr       += uLength;
        uCharInLine += uLength;

        uLength = SetTab(uCharInLine, hpStr, 23);

        hpStr       += uLength;
        uCharInLine += uLength;

        uLength = GetWindowText (GetDlgItem(hDlg, ID_SS_FILESTXT), szZipPath,
                                sizeof(szZipPath));

        hmemcpy (hpStr, szZipPath, uLength);
        hpStr       += uLength;
        uCharInLine += uLength;

        uLength = SetTab(uCharInLine, hpStr, 30);

        hpStr       += uLength;
        uCharInLine += uLength;

        uLength = strMBR2.GetLength();
        memset(szZipPath, 0, sizeof(szZipPath));
        lpWorkPtr = strMBR2.GetBuffer(uLength);
        memcpy(szZipPath, lpWorkPtr, uLength+1);

        hmemcpy (hpStr, szZipPath, uLength);
        hpStr       += uLength;
        uCharInLine += uLength;

        uLength = SetTab(uCharInLine, hpStr, 45);

        hpStr       += uLength;
        uCharInLine += uLength;

        uLength = GetWindowText (GetDlgItem(hDlg, ID_SS_BRTXT), szZipPath,
                                sizeof(szZipPath));

        hmemcpy (hpStr, szZipPath, uLength);
        hpStr += uLength;

        // ******************************** //
        // ****  Draw line under text  **** //
        // ******************************** //

        hmemcpy (hpStr, CRLF, sizeof(CRLF));
        hpStr += sizeof(CRLF)-1;

        for (i = 0; i < COMMENT_WIDTH; i++)
            *hpStr++ = '-';

        hmemcpy (hpStr, CRLF, sizeof(CRLF));
        hpStr += sizeof(CRLF)-1;
        hmemcpy (hpStr, CRLF, sizeof(CRLF));
        hpStr += sizeof(CRLF)-1;


        // *************************************************** //
        // ****  Scanned Items Status Line                **** //
        // *************************************************** //

        uCharInLine = 0;
        uLength = GetWindowText (GetDlgItem(hDlg, ID_SS_SCANNEDTXT), szZipPath,
                                 sizeof(szZipPath));

        hmemcpy (hpStr, szZipPath, uLength);
        hpStr       += uLength;
        uCharInLine += uLength;

        uLength = SetTab(uCharInLine, hpStr, 25);

        hpStr       += uLength;
        uCharInLine += uLength;

        uLength = GetWindowText (GetDlgItem(hDlg, ID_SS_FILSCAN), szZipPath,
                                sizeof(szZipPath));

        hmemcpy (hpStr, szZipPath, uLength);
        hpStr       += uLength;
        uCharInLine += uLength;

        uLength = SetTab(uCharInLine, hpStr, 37);

        hpStr       += uLength;
        uCharInLine += uLength;

        uLength = GetWindowText (GetDlgItem(hDlg, ID_SS_MBSCAN), szZipPath,
                                sizeof(szZipPath));

        hmemcpy (hpStr, szZipPath, uLength);
        hpStr       += uLength;
        uCharInLine += uLength;

        uLength = SetTab(uCharInLine, hpStr, 50);

        hpStr       += uLength;
        uCharInLine += uLength;

        uLength = GetWindowText (GetDlgItem(hDlg, ID_SS_BRSCAN), szZipPath,
                                    sizeof(szZipPath));

        hmemcpy (hpStr, szZipPath, uLength);
        hpStr += uLength;

        hmemcpy (hpStr, CRLF, sizeof(CRLF));
        hpStr += sizeof(CRLF)-1;

        // ******************************************** //
        // ****  Infected Items Status Line        **** //
        // ******************************************** //

        uCharInLine = 0;
        uLength = GetWindowText (GetDlgItem(hDlg, ID_SS_INFTXT), szZipPath,
                                 sizeof(szZipPath));

        hmemcpy (hpStr, szZipPath, uLength);
        hpStr       += uLength;
        uCharInLine += uLength;

        uLength = SetTab(uCharInLine, hpStr, 25);

        hpStr       += uLength;
        uCharInLine += uLength;

        uLength = GetWindowText (GetDlgItem(hDlg, ID_SS_FILINF), szZipPath,
                                sizeof(szZipPath));

        hmemcpy (hpStr, szZipPath, uLength);
        hpStr       += uLength;
        uCharInLine += uLength;

        uLength = SetTab(uCharInLine, hpStr, 37);

        hpStr       += uLength;
        uCharInLine += uLength;

        uLength = GetWindowText (GetDlgItem(hDlg, ID_SS_MBINF), szZipPath,
                                sizeof(szZipPath));

        hmemcpy (hpStr, szZipPath, uLength);
        hpStr       += uLength;
        uCharInLine += uLength;

        uLength = SetTab(uCharInLine, hpStr, 50);

        hpStr       += uLength;
        uCharInLine += uLength;

        uLength = GetWindowText (GetDlgItem(hDlg, ID_SS_BRINF), szZipPath,
                                sizeof(szZipPath));

        hmemcpy (hpStr, szZipPath, uLength);
        hpStr += uLength;

        hmemcpy (hpStr, CRLF, sizeof(CRLF));
        hpStr += sizeof(CRLF)-1;


        // ************************************** //
        // ****  Repaired Items Status Line  **** //
        // ************************************** //

        uCharInLine = 0;
        uLength = GetWindowText (GetDlgItem(hDlg, ID_SS_REPAIRTXT), szZipPath,
                                sizeof(szZipPath));

        hmemcpy (hpStr, szZipPath, uLength);

        hpStr       += uLength;
        uCharInLine += uLength;

        uLength = SetTab(uCharInLine, hpStr, 25);

        hpStr       += uLength;
        uCharInLine += uLength;

        uLength = GetWindowText (GetDlgItem(hDlg, ID_SS_FILCLN), szZipPath,
                                sizeof(szZipPath));

        hmemcpy (hpStr, szZipPath, uLength);
        hpStr       += uLength;
        uCharInLine += uLength;

        uLength = SetTab(uCharInLine, hpStr, 37);

        hpStr       += uLength;
        uCharInLine += uLength;

        uLength = GetWindowText (GetDlgItem(hDlg, ID_SS_MBCLN), szZipPath,
                                 sizeof(szZipPath));

        hmemcpy (hpStr, szZipPath, uLength);
        hpStr       += uLength;
        uCharInLine += uLength;

        uLength = SetTab(uCharInLine, hpStr, 50);

        hpStr       += uLength;
        uCharInLine += uLength;

        uLength = GetWindowText (GetDlgItem(hDlg, ID_SS_BRCLN), szZipPath,
                                sizeof(szZipPath));

        hmemcpy (hpStr, szZipPath, uLength);
        hpStr += uLength;

        hmemcpy (hpStr, CRLF, sizeof(CRLF));
        hpStr += sizeof(CRLF)-1;


        // ***************************************** //
        // ****  Quarantined Items Status Line  **** //
        // ***************************************** //
        // Only print the Quarantine statistics if Quarantine is enabled.
#ifndef SYM_DOS
        if( NAV_IS_QUARANTINE() )
#endif
            {
            uCharInLine = 0;
            uLength = GetWindowText (GetDlgItem(hDlg, ID_SS_QUARTXT), szZipPath,
                                    sizeof(szZipPath));

            hmemcpy (hpStr, szZipPath, uLength);

            hpStr       += uLength;
            uCharInLine += uLength;

            uLength = SetTab(uCharInLine, hpStr, 25);

            hpStr       += uLength;
            uCharInLine += uLength;

            uLength = GetWindowText (GetDlgItem(hDlg, ID_SS_FILQUAR), szZipPath,
                                    sizeof(szZipPath));

            hmemcpy (hpStr, szZipPath, uLength);
            hpStr += uLength;

            hmemcpy (hpStr, CRLF, sizeof(CRLF));
            hpStr += sizeof(CRLF)-1;

            }

        // ***************************************** //
        // ****  Deleted Items Status Line      **** //
        // ***************************************** //

        uCharInLine = 0;
        uLength = GetWindowText (GetDlgItem(hDlg, ID_SS_DELETETXT), szZipPath,
                                sizeof(szZipPath));

        hmemcpy (hpStr, szZipPath, uLength);

        hpStr       += uLength;
        uCharInLine += uLength;

        uLength = SetTab(uCharInLine, hpStr, 25);

        hpStr       += uLength;
        uCharInLine += uLength;

        uLength = GetWindowText (GetDlgItem(hDlg, ID_SS_FILDEL), szZipPath,
                                sizeof(szZipPath));

        hmemcpy (hpStr, szZipPath, uLength);
        hpStr += uLength;
        hmemcpy (hpStr, CRLF, sizeof(CRLF));
        hpStr += sizeof(CRLF)-1;
        hmemcpy (hpStr, CRLF, sizeof(CRLF));
        hpStr += sizeof(CRLF)-1;
                                        // ------------------------------------
                                        // We are assuming that upto this point
                                        // hpStr has NOT been incremented past
                                        // the segment boundary so I feel safe
                                        // doing pointer arythmetic to find out
                                        // number of bytes used.
                                        // ------------------------------------

        dwBytesUsed = (hpStr - lpMem);
                                        // ------------------------------------
                                        // Now comes the sweet part. If we have
                                        // found viruses, all the infections
                                        // should be appended to the end of
                                        // this result info
                                        // ------------------------------------
        if (lpNav->lpInfArr)
            {
            lpInfArr = lpNav->lpInfArr;

            hmemcpy (hpStr, CRLF, sizeof(CRLF));
            hpStr       += sizeof(CRLF)-1;
            dwBytesUsed += sizeof(CRLF)-1;

            do
                {
                if (lpInfected = (LPINFECTEDFILE)MemLock(lpInfArr->hInf))
                    {
                    for (i =  uCharInLine = 0; i < lpInfArr->uNumUsed;
                         i++, uCharInLine = 0)
                        {
                                        // ---------------------------------
                                        // We must have at least 1000 bytes
                                        // free before this line is processed.
                                        // If required reallocate this buffer
                                        // with SIZE_PRINT_BUFFER bytes.
                                        // ---------------------------------

                        hpStr = ReallocIfRequired(&hMem, dwBytesUsed,
                                                  &dwBufferSize, hpStr);

                        if ( !hMem && !hpStr)
                            break;

                                        // ---------------------------------
                                        // First take care of the file name
                                        // ---------------------------------

                        uLength = String_Get (IDS_FILENAME, szZipPath,
                                              sizeof(szZipPath));

                        hmemcpy (hpStr, szZipPath, uLength);

                        hpStr       += uLength;
                        uCharInLine  = uLength;
                        dwBytesUsed += uLength;

                        uLength = SetTab(uCharInLine, hpStr, 20);

                        hpStr       += uLength;
                        uCharInLine += uLength;
                        dwBytesUsed += uLength;

                        if (lpInfected[i].wStatus == FILESTATUS_ZIPINFECTED)
                            {
                            ParseZipFileName (lpInfected[i].szFullPath,
                                              szZipPath, szFileName);

                            uLength = SPRINTF (szWorkBuff, String_Ptr(IDS_PRINTZIPFILE),
                                         (LPSTR)szFileName, (LPSTR)szZipPath);

                            hmemcpy (hpStr, szWorkBuff, uLength);

                            hpStr += uLength;
                            dwBytesUsed += uLength;
                            }
                        else
                            {
                            hmemcpy (hpStr, lpInfected[i].szFullPath,
                                    STRLEN(lpInfected[i].szFullPath));

                            hpStr += STRLEN (lpInfected[i].szFullPath);
                            dwBytesUsed += STRLEN (lpInfected[i].szFullPath);
                            }

                        hmemcpy (hpStr, CRLF, sizeof(CRLF));
                        hpStr += sizeof(CRLF)-1;
                        dwBytesUsed += sizeof(CRLF)-1;

                                        // ---------------------------------
                                        // Now take care of the virus name
                                        // ---------------------------------

                        uLength = String_Get (IDS_VIRUSNAME, szZipPath,
                                             sizeof(szZipPath));

                        hmemcpy (hpStr, szZipPath, uLength);

                        hpStr       += uLength;
                        uCharInLine  = uLength;
                        dwBytesUsed += uLength;

                        uLength = SetTab(uCharInLine, hpStr, 20);

                        hpStr       += uLength;
                        uCharInLine += uLength;
                        dwBytesUsed += uLength;

                        hmemcpy (hpStr, lpInfected[i].Notes.lpVirName,
                                STRLEN(lpInfected[i].Notes.lpVirName));

                        uLength = STRLEN (lpInfected[i].Notes.lpVirName);

                        hpStr       += uLength;
                        dwBytesUsed += uLength;
                        hmemcpy (hpStr, CRLF, sizeof(CRLF));
                        hpStr += sizeof(CRLF)-1;
                        dwBytesUsed += sizeof(CRLF)-1;

                                        // ---------------------------------
                                        // And finally indicate what the
                                        // status of this file is.
                                        // ---------------------------------

                        uLength = String_Get (IDS_STATUS, szZipPath,
                                              sizeof(szZipPath));

                        hmemcpy (hpStr, szZipPath, uLength);
                        hpStr       += uLength;
                        uCharInLine  = uLength;
                        dwBytesUsed += uLength;

                        uLength = SetTab(uCharInLine, hpStr, 20);

                        hpStr       += uLength;
                        uCharInLine += uLength;
                        dwBytesUsed += uLength;

                        uLength = FileStatusString (lpInfected[i].wStatus,
                                                    szZipPath);

                        hmemcpy (hpStr, szZipPath, uLength);
                        hpStr += uLength;
                        dwBytesUsed += uLength;

                        hmemcpy (hpStr, CRLF, sizeof(CRLF));
                        hpStr += sizeof(CRLF)-1;
                        dwBytesUsed += sizeof(CRLF)-1;

                        hmemcpy (hpStr, CRLF, sizeof(CRLF));
                        hpStr += sizeof(CRLF)-1;
                        dwBytesUsed += sizeof(CRLF)-1;
                        }

                    MemUnlock (lpInfArr->hInf);
                    }
                else
                    break;

                lpInfArr = (LPINFARR)lpInfArr->lpNext;
                }
            while (lpInfArr && hMem && hpStr);

            }

        if ( hMem )
            MemUnlock (hMem);
        }

     return (hMem);
}
                                        // ---------------------------------
                                        // We must have at least 1000 bytes
                                        // free before this line is processed.
                                        // If required reallocate this buffer
                                        // with SIZE_PRINT_BUFFER bytes.
                                        // ---------------------------------

HPSTR LOCAL ReallocIfRequired (HGLOBAL FAR *hMem, DWORD dwBytesUsed,
                               DWORD FAR * dwBufferSize, HPSTR hpStr)
{
    auto    HGLOBAL hTemp;

    if ((dwBytesUsed + 1024) > *dwBufferSize)
        {
        *dwBufferSize += SIZE_PRINT_BUFFER;

        MemUnlock (*hMem);

        hTemp = MemReAlloc(*hMem, *dwBufferSize, GHND);

        if (!hTemp)
            {
            hpStr = NULL;
            }
        else
            {
            *hMem = hTemp;
            hpStr = (HPSTR)MemLock (hTemp);

            hpStr += dwBytesUsed;
            }
        }

    return ( hpStr );
}


UINT SYM_EXPORT WINAPI FileStatusString (WORD wFileStatus, LPSTR lpMem)
{
    extern FILESTATUSSTRUCT FileStatus[];

    STRCPY ( lpMem,
             FileStatus[wFileStatus].uFileStatusString ?
             String_Ptr(FileStatus[wFileStatus].uFileStatusString) :
             "" );

    return (STRLEN(lpMem));
}


UINT LOCAL SetTab (UINT uCharInLine, HPSTR hpMem, UINT uTab)
{
   UINT uChars = 0;

   uTab --;
   if (uCharInLine < uTab)
      {
      for (uChars = 0; uChars < (uTab - uCharInLine); uChars++)
         *hpMem++ = ' ';
      }

   return (uChars);
}

// ************************************************************************ //
// ****        (C) Copyright Symantec Corporation 19xx - 1993          **** //
// ************************************************************************ //
// ****                                                                **** //
// ****  Procedure:                                                    **** //
// ****                                                                **** //
// ****  Author: Martin Fallenstedt          Date: January 8, 1993     **** //
// ****                                                                **** //
// ****  Functionality:                                                **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Input:                                                        **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Output:                                                       **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Update History:                                               **** //
// ****                                                                **** //
// ****                                                                **** //
// ************************************************************************ //

UINT WINAPI BuildPrintHeader (HWND hDlg, LPSTR lpMem)
{
    UINT        i;
    UINT        uCopy;


    // ************************************ //
    // ***      Get Window Header       *** //
    // ************************************ //

    i = LoadString (hInstance, IDS_STARS, lpMem, SIZE_WORK);
    STRCPY (lpMem + i, CRLF);
    i += STRLEN (CRLF);

    uCopy = (COMMENT_WIDTH - GetWindowTextLength(hDlg)) / 2;
    i += SetTab(0, lpMem + i, uCopy);

    i += GetWindowText (hDlg, lpMem + i, SIZE_WORK);
    STRCPY (lpMem + i, CRLF);
    i += STRLEN (CRLF);
    i += LoadString (hInstance, IDS_STARS, lpMem + i, SIZE_WORK);

    STRCPY (lpMem + i, CRLF);
    i += STRLEN (CRLF);
    STRCPY (lpMem + i, CRLF);
    i += STRLEN (CRLF);
    STRCPY (lpMem + i, CRLF);
    i += STRLEN (CRLF);

    return(i);
}


VOID WINAPI BuildRestOfVirusInfo (
    HWND    hDlg,
    HPSTR   hpMem,
    DWORD   i,
    BOOL    bMacType )
{
    auto    UINT        uCopy;
    auto    UINT        uCharInLine;
    auto    UINT        uLength;
    auto    UINT        uTextLength;
    auto    HGLOBAL     hComMem;
    auto    LPSTR       lpComment;
    auto    char        lpWork[SIZE_WORK+1];
    auto    UINT        j;

    extern  WORD        gwForPrintHack;

    // ******************* //
    // LINE 1:  Virus Name //
    // ******************* //

    uCharInLine = 0;
    uLength = GetWindowText (GetDlgItem(hDlg, IDI_VI_VNAMETXT), &hpMem[i],
                             SIZE_WORK);
    i           += uLength;
    uCharInLine += uLength;

    i += SetTab(uCharInLine, &hpMem[i], 15);

    i += GetWindowText (GetDlgItem(hDlg, IDI_VI_VNAME), &hpMem[i],
                        SIZE_WORK);

    STRCPY (&hpMem[i], CRLF);
    i += STRLEN (CRLF);

    // ******************** //
    // LINE 2:  Virus Alias//
    // ******************** //

    uCharInLine = 0;
    uLength = GetWindowText (GetDlgItem(hDlg, IDI_VI_ALIASTXT), &hpMem[i],
                             SIZE_WORK);
    i           += uLength;
    uCharInLine += uLength;

    i += SetTab(uCharInLine, &hpMem[i], 15);

    i += GetWindowText (GetDlgItem(hDlg, IDI_VI_ALIAS), &hpMem[i],
                        SIZE_WORK);

    STRCPY (&hpMem[i], CRLF);
    i += STRLEN (CRLF);

    // ********************************* //
    // LINE 3:  Type of files it infects //
    // ********************************* //

    uCharInLine = 0;
    uLength = GetWindowText (GetDlgItem(hDlg, IDI_VI_INFECTSTXT), &hpMem[i],
                        SIZE_WORK);

    i           += uLength;
    uCharInLine += uLength;

    i += SetTab(uCharInLine, &hpMem[i], 15);

    i += GetWindowText (GetDlgItem(hDlg, IDI_VI_INFECTS), &hpMem[i],
                        SIZE_WORK);

    STRCPY (&hpMem[i], CRLF);
    i += STRLEN (CRLF);

    // ******************* //
    // LINE 4:  Likelihood //
    // ******************* //

    uCharInLine = 0;
    uLength = GetWindowText (GetDlgItem(hDlg, IDI_VI_LIKETXT), &hpMem[i],
                        SIZE_WORK);

    i           += uLength;
    uCharInLine += uLength;

    i += SetTab(uCharInLine, &hpMem[i], 15);

    i += GetWindowText (GetDlgItem(hDlg, IDI_VI_LIKE), &hpMem[i],
                        SIZE_WORK);

    STRCPY (&hpMem[i], CRLF);
    i += STRLEN (CRLF);

    // ********************* //
    // LINE 5:  Virus Length //
    // ********************* //

    uCharInLine = 0;
    uLength = GetWindowText (GetDlgItem(hDlg, IDI_VI_LENGTHTXT), &hpMem[i],
                        SIZE_WORK);

    i           += uLength;
    uCharInLine += uLength;

    i += SetTab(uCharInLine, &hpMem[i], 15);

    i += GetWindowText (GetDlgItem(hDlg, IDI_VI_LENGTH), &hpMem[i],
                        SIZE_WORK);

    STRCPY (&hpMem[i], CRLF);
    i += STRLEN (CRLF);

    // *************************** //
    // LINE 6:  Type of virus text //
    // *************************** //

    STRCPY (&hpMem[i], CRLF);
    i += STRLEN (CRLF);

    i += GetWindowText (GetDlgItem(hDlg, IDI_VI_TYPEGRP), &hpMem[i],
                        SIZE_WORK);

    STRCPY (&hpMem[i], CRLF);
    i += STRLEN (CRLF);
    STRCPY (&hpMem[i], CRLF);
    i += STRLEN (CRLF);

    // ***************************************** //
    // LINE 7:  Memory resident and trigger line //
    // ***************************************** //

                                        // ----------------------------------
                                        // ----------------------------------
    if (bMacType == FALSE)
        {
        uCharInLine = 0;
        uLength = GetWindowText (GetDlgItem(hDlg, IDI_VI_MEMRESTXT), &hpMem[i],
                                SIZE_WORK);

        i           += uLength;
        uCharInLine += uLength;

        uLength = SetTab(uCharInLine, &hpMem[i], 20);

        i           += uLength;
        uCharInLine += uLength;

        uLength = LoadString (
                            hInstance,
                            ( gwForPrintHack & bitINFO_RESIDENT )
                            ? IDS_YES : IDS_NO,
                            &hpMem[i],
                            SIZE_WORK
                            );

        i           += uLength;
        uCharInLine += uLength;

        uLength = SetTab(uCharInLine, &hpMem[i], 27);

        i           += uLength;
        uCharInLine += uLength;

        // ************************************************************ //

        uLength = GetWindowText (GetDlgItem(hDlg, IDI_VI_OVERWRTXT), &hpMem[i],
                            SIZE_WORK);

        i           += uLength;
        uCharInLine += uLength;

        uLength = SetTab(uCharInLine, &hpMem[i], 43);

        i           += uLength;
        uCharInLine += uLength;

        uLength = LoadString (hInstance,
                            ( gwForPrintHack & bitINFO_TRIGGERED )
                            ? IDS_YES : IDS_NO,
                            &hpMem[i],
                            SIZE_WORK
                            );

        i           += uLength;
        uCharInLine += uLength;

        uLength = SetTab(uCharInLine, &hpMem[i], 51);

        i           += uLength;
        uCharInLine += uLength;

        STRCPY (&hpMem[i], CRLF);
        i += STRLEN (CRLF);

        // ***************************************** //
        // LINE 8:  Size stealth and encrypting line //
        // ***************************************** //

        uCharInLine = 0;
        uLength = GetWindowText (GetDlgItem(hDlg, IDI_VI_STEALTHTXT), &hpMem[i],
                                 SIZE_WORK);

        i           += uLength;
        uCharInLine += uLength;

        uLength = SetTab(uCharInLine, &hpMem[i], 20);

        i           += uLength;
        uCharInLine += uLength;

        uLength = LoadString (hInstance,
                            ( gwForPrintHack & bitINFO_SIZESTLTH )
                            ? IDS_YES : IDS_NO,
                            &hpMem[i],
                            SIZE_WORK
                            );

        i           += uLength;
        uCharInLine += uLength;

        uLength = SetTab(uCharInLine, &hpMem[i], 27);

        i           += uLength;
        uCharInLine += uLength;

        // ************************************************************ //
    
        uLength = GetWindowText (GetDlgItem(hDlg, IDI_VI_COMPTXT), &hpMem[i],
                             SIZE_WORK);
        i           += uLength;
        uCharInLine += uLength;

        uLength = SetTab(uCharInLine, &hpMem[i], 43);

        i           += uLength;
        uCharInLine += uLength;

        i += LoadString (hInstance,
                            ( gwForPrintHack & bitINFO_ENCRYPT )
                            ? IDS_YES : IDS_NO,
                            &hpMem[i],
                            SIZE_WORK
                            );

        STRCPY (&hpMem[i], CRLF);
        i += STRLEN (CRLF);

        // ****************************************** //
        // LINE 9:  Full stealth and polymorphic line //
        // ****************************************** //

        uCharInLine = 0;
        uLength = GetWindowText (GetDlgItem(hDlg, IDI_VI_SLFENCRTXT), &hpMem[i],
                                 SIZE_WORK);
        i           += uLength;
        uCharInLine += uLength;

        uLength = SetTab(uCharInLine, &hpMem[i], 20);

        i           += uLength;
        uCharInLine += uLength;

        uLength = LoadString (hInstance,
                            ( gwForPrintHack & bitINFO_FULLSTLTH )
                            ? IDS_YES : IDS_NO,
                            &hpMem[i],
                            SIZE_WORK
                            );

        i           += uLength;
        uCharInLine += uLength;

        uLength = SetTab(uCharInLine, &hpMem[i], 27);

        i           += uLength;
        uCharInLine += uLength;

        // ************************************************************ //

        uLength = GetWindowText (GetDlgItem(hDlg, IDI_VI_PARASTXT), &hpMem[i],
                            SIZE_WORK);

        i           += uLength;
        uCharInLine += uLength;

        i += SetTab(uCharInLine, &hpMem[i], 43);

        i += LoadString (hInstance,
                            ( gwForPrintHack & bitINFO_POLYMORPH )
                            ? IDS_YES : IDS_NO,
                            &hpMem[i],
                            SIZE_WORK
                            );
        }

    STRCPY (&hpMem[i], CRLF);
    i += STRLEN (CRLF);

    // *********************** //
    // LINE 10:  Comments line //
    // *********************** //

    STRCPY (&hpMem[i], CRLF);
    i += STRLEN (CRLF);

    i += GetWindowText (GetDlgItem(hDlg, IDI_VI_COMMGRP), &hpMem[i],
                        SIZE_WORK);

    STRCPY (&hpMem[i], CRLF);
    i += STRLEN (CRLF);

    // ************************************************** //
    // We chop the comment up into bite sizes if required //
    // ************************************************** //

    uTextLength = GetWindowTextLength(GetDlgItem(hDlg, IDI_VI_COMMENT));

    if (uTextLength > COMMENT_WIDTH-1)
       {
       uTextLength--;               //&? Last byte may be a lead byte
       if ((hComMem = MemAlloc(GHND, uTextLength + 10)) != 0)
          {
          lpComment = (LPSTR)MemLock (hComMem);

          GetWindowText (GetDlgItem(hDlg, IDI_VI_COMMENT), lpComment,
                        uTextLength + 10);

          while (uTextLength)
             {
             MEMSET (lpWork, EOS, sizeof(lpWork));

             uCopy = ((int)(uTextLength - COMMENT_WIDTH) > 0 ? COMMENT_WIDTH :
                      uTextLength);

             MEMCPY (lpWork, lpComment, uCopy);

                                    // Truncate last byte if it is a lead byte
                                    // and process the byte at next
                                    // iteration.
             if (DBCSIsLeadByte (lpWork[uCopy - 1]))
                 {
                 lpWork[uCopy - 1] = EOS;
                 uCopy -= 1;
                 }

             if (uCopy != uTextLength)
                {
                auto LPSTR lpStr;

                lpStr = SYMstrrchr ((LPCSTR)lpWork, (int)' ');
                if (NULL != lpStr)
                    j = lpStr - lpWork;
                else
                    j = 0;

                uCopy -= (STRLEN(lpWork) - j);

                }

             if (lpWork[0] == ' ')
                {
                MEMCPY (&hpMem[i], &lpWork[1], (uCopy-1));
                i += (uCopy-1);
                }
             else
                {
                MEMCPY (&hpMem[i], lpWork, uCopy);
                i += uCopy;
                }

             uTextLength -= uCopy;
             lpComment   += uCopy;
             STRCPY (&hpMem[i], CRLF);
             i += STRLEN (CRLF);
             }

          MemUnlock (hComMem);
          MemFree(hComMem);
          }
       }
    else
       {
       i += GetWindowText (GetDlgItem(hDlg, IDI_VI_COMMENT), &hpMem[i],
                           SIZE_WORK);
       }

    STRCPY (&hpMem[i], CRLF);
    i += STRLEN (CRLF);
    STRCPY (&hpMem[i], CRLF);
    i += STRLEN (CRLF);
}

