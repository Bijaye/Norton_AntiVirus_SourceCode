//***********************************************************************
//
// $Header:   S:/NAVEX/VCS/MCRRPR15.CPv   1.23   09 Dec 1998 17:45:12   DCHI  $
//
// Description:
//      Contains WordDocument macro repair function code.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/MCRRPR15.CPv  $
// 
//    Rev 1.23   09 Dec 1998 17:45:12   DCHI
// Added #include "gdefines.h"
// 
//    Rev 1.22   12 Oct 1998 13:41:26   DCHI
// Added hiding of XL97 XLSTART.
// 
//    Rev 1.21   10 Sep 1998 13:05:04   DCHI
// Added #ifdef around heuristic header file includes and Mac ifdefs
// around RepairFileClose() calls.
// 
//    Rev 1.20   09 Sep 1998 12:45:24   DCHI
// Changes for password-protected Excel 97 document repair.
// 
//    Rev 1.19   17 Aug 1998 12:01:34   DCHI
// Added RepairFileClose() calls during error returns in WordMacroVirusRepair().
// 
//    Rev 1.18   04 Aug 1998 13:52:28   DCHI
// Changes for Excel heuristics.
// 
//    Rev 1.17   10 Jun 1998 13:20:46   DCHI
// Added W7H2
// 
//    Rev 1.16   11 May 1998 18:17:48   DCHI
// Changes for menu repair and
// for Access 2.0 scanning and Access datafile conversion.
// 
//    Rev 1.15   15 Apr 1998 17:23:42   DCHI
// Modifications for new XL95 and XL5 engines.
// 
//    Rev 1.14   29 Jan 1998 19:04:26   DCHI
// Added support for Excel formula virus scan and repair.
// 
//    Rev 1.13   03 Dec 1997 11:53:00   RELNITI
// REMOVE VID_CAP_CORRUPTED from the CASE list
// 
//    Rev 1.12   26 Nov 1997 18:34:54   DCHI
// Added #ifdef'd out PowerPoint scanning code.
// 
//    Rev 1.11   03 Nov 1997 16:20:06   RELNITI
// ADDING more VID_CAP*
// 
//    Rev 1.10   16 Oct 1997 11:20:54   DCHI
// Moved variable declarations to top of function in ShouldDoQuickRepair().
// 
//    Rev 1.9   15 Oct 1997 18:15:44   DCHI
// Added Word 97 heuristics.
// 
//    Rev 1.8   15 Oct 1997 13:01:14   JWILBER
// Changed TCHAR to TBYTE to avoid compiler warning w/ definition
// of name for gszJapaneseNormaldot[].
//
//    Rev 1.7   10 Oct 1997 17:50:24   DCHI
// Added code to reset customizations in normal.dot and Japanese equivalent
// if VID is VID_CAP*.
//
//    Rev 1.6   16 Sep 1997 14:57:34   DCHI
// Added logic for setting TDT size to zero.
//
//    Rev 1.5   03 Sep 1997 17:03:02   DCHI
// Moved uAttr out of #ifndef in WordMacroVirusRepair().
//
//    Rev 1.4   02 Sep 1997 14:58:26   DCHI
// Added freeing of macro scan struct in WD7MacroVirusRepair().
//
//    Rev 1.3   21 Jul 1997 14:23:58   DCHI
// Added additional condition to check for NAVEX15 before conditionally
// disabling macro heuristics under WIN16 and DOSX.
//
//    Rev 1.2   16 Jul 1997 17:31:24   DCHI
// Added check for version >= 3 for enabling heuristics on WIN and DX.
//
//    Rev 1.1   10 Jul 1997 17:45:28   DDREW
// Turn on NLM repairs for NAVEX15
//
//    Rev 1.0   09 Jul 1997 16:20:28   DCHI
// Initial revision.
//
//************************************************************************

#include "gdefines.h"

#if defined(NAVEX15) || !defined(SYM_NLM)

#include "wd7api.h"
#include "o97api.h"
#include "xl5api.h"
#include "acsapi.h"

#include "macrorep.h"

#include "wddecsig.h"
#include "wdscan15.h"
#include "wdrepair.h"

#include "offcsc15.h"
#include "o97scn15.h"
#include "o97rpr15.h"
#include "xl97rpr.h"
#include "xl5rpr.h"
#include "xl4rpr.h"
#include "mcrsgutl.h"

#ifdef PP_SCAN
#include "ppscan.h"
#endif // #ifdef PP_SCAN

#ifdef MACROHEU
#include "mcrheusc.h"
#include "wd7heusc.h"
#include "w97heusc.h"
#include "xlheusc.h"
#endif // #ifdef MACROHEU

#include "macrovid.h"

#include "navexshr.h"

#ifndef SYM_WIN
BOOL gbInWindows = TRUE;
#else
extern BOOL gbInWindows;
#endif

LPTSTR FuncAnsiNext(LPTSTR lp)
{
    if (*lp != '\0')
        return(lp + 1);

    return(lp);
}

// local prototypes

#ifdef __MACINTOSH__
Boolean WordDocChangeCreator( HFILE hFile );
#endif

WORD TStringCmp2
(
    LPTSTR  lptstr0,    // String 0
    LPTSTR  lptstr1,    // String 1
    LPTSTR  lptstr2,    // String 2
    WORD    wLen        // Number of bytes to compare
);

WORD EndsInDOTOrNORMAL
(
    LPTSTR  lpszFileName        // Name of infected file
);


//********************************************************************
//
// Function:
//  WORD TStringCmp2()
//
// Description:
//  Determines whether the characters of one string are equal
//  to any of the corresponding characters in the other two TStrings.
//
// Returns:
//  0           If the string matches up to wLen characters
//  1           If the string does not match up to wLen characters
//
//********************************************************************

WORD TStringCmp2
(
    LPTSTR  lptstr0,    // String 0
    LPTSTR  lptstr1,    // String 1
    LPTSTR  lptstr2,    // String 2
    WORD    wLen        // Number of bytes to compare
)
{
    while (wLen-- != 0)
    {
        if (*lptstr0 != *lptstr1 && *lptstr0 != *lptstr2)
            return 1;

        if (gbInWindows == FALSE)
        {
            lptstr0 = FuncAnsiNext(lptstr0);
            lptstr1 = FuncAnsiNext(lptstr1);
            lptstr2 = FuncAnsiNext(lptstr2);
        }
        else
        {
            lptstr0 = AnsiNext(lptstr0);
            lptstr1 = AnsiNext(lptstr1);
            lptstr2 = AnsiNext(lptstr2);
        }
    }

    return 0;
}


//********************************************************************
//
// Function:
//  BOOL TStringIEq()
//
// Description:
//  Determines whether the two strings are equal with case
//  insensitivity.
//
// Returns:
//  TRUE        If the strings are equal
//  FALSE       If the strings are not equal
//
//********************************************************************

BOOL TStringIEq
(
    LPTSTR  lptstr0,    // String 0
    LPTSTR  lptstr1     // String 1
)
{
    TCHAR   tch0, tch1;

    while (*lptstr0 || *lptstr1)
    {
        tch0 = *lptstr0;
        tch1 = *lptstr1;

        // Lower case if necessary

        if (tch0 <= 'Z' && tch0 >= 'A')
            tch0 = tch0 + ('a' - 'A');

        if (tch1 <= 'Z' && tch1 >= 'A')
            tch1 = tch1 + ('a' - 'A');

        // Compare

        if (tch0 != tch1)
            return(FALSE);

        // Next character

        ++lptstr0;
        ++lptstr1;
    }

    // Strings are equal

    return(TRUE);
}


//********************************************************************
//
// Function:
//  WORD EndsInDOTOrNORMAL()
//
// Description:
//  Determines whether the filename ends in .DOT or NORMAL (for Mac).
//
// Returns:
//  0           If the file name does not end in ".DOT" or "NORMAL"
//  1           If the file name does end in ".DOT" or "NORMAL"
//
//********************************************************************

TCHAR gtchDOT[] = { '.', 'D', 'O', 'T' };
TCHAR gtchLowerDOT[] = { '.', 'd', 'o', 't' };
TCHAR gtchNORMAL[] = { 'N', 'O', 'R', 'M', 'A', 'L' };
TCHAR gtchLowerNORMAL[] = { 'n', 'o', 'r', 'm', 'a', 'l' };

WORD EndsInDOTOrNORMAL
(
    LPTSTR  lpszFileName        // Name of infected file
)
{
    LPTSTR  lptstrNORMAL;
    LPTSTR  lptstrDOT;
    LPTSTR  lptstrCur;
    int     i;

    if (lpszFileName == NULL || *lpszFileName == '\0')
        return 0;

    // Initial candidate is at beginning

    lptstrNORMAL = lptstrDOT = lptstrCur = lpszFileName;

    // See if there are at least five characters in file name for .DOT

    for (i=0;i<4;i++)
    {
        if (gbInWindows == FALSE)
            lptstrCur = FuncAnsiNext(lptstrCur);
        else
            lptstrCur = AnsiNext(lptstrCur);
        if (*lptstrCur == '\0')
        {
            // File name less than five characters

            return 0;
        }
    }

    if (gbInWindows == FALSE)
    {
        lptstrDOT = FuncAnsiNext(lptstrDOT);
        lptstrCur = FuncAnsiNext(lptstrCur);
    }
    else
    {
        lptstrDOT = AnsiNext(lptstrDOT);
        lptstrCur = AnsiNext(lptstrCur);
    }

    if (*lptstrCur == '\0')
    {
        if (TStringCmp2(lptstrDOT,gtchDOT,gtchLowerDOT,
            sizeof(gtchDOT)/sizeof(TCHAR)) == 0)
            return 1;

        return 0;
    }

    // Now go to the end

    if (gbInWindows == FALSE)
    {
        lptstrDOT = FuncAnsiNext(lptstrDOT);
        lptstrCur = FuncAnsiNext(lptstrCur);

        while (*lptstrCur != '\0')
        {
            lptstrNORMAL = FuncAnsiNext(lptstrNORMAL);
            lptstrDOT = FuncAnsiNext(lptstrDOT);
            lptstrCur = FuncAnsiNext(lptstrCur);
        }
    }
    else
    {
        lptstrDOT = AnsiNext(lptstrDOT);
        lptstrCur = AnsiNext(lptstrCur);

        while (*lptstrCur != '\0')
        {
            lptstrNORMAL = AnsiNext(lptstrNORMAL);
            lptstrDOT = AnsiNext(lptstrDOT);
            lptstrCur = AnsiNext(lptstrCur);
        }
    }

    if (TStringCmp2(lptstrDOT,gtchDOT,gtchLowerDOT,
        sizeof(gtchDOT)/sizeof(TCHAR)) == 0)
        return 1;

    if (TStringCmp2(lptstrNORMAL,gtchNORMAL,gtchLowerNORMAL,
        sizeof(gtchNORMAL)/sizeof(TCHAR)) == 0)
        return 1;

    return 0;
}

#ifdef __MACINTOSH__
//********************************************************************
//
// Function:
//  WordDocChangeCreator
//
// Description:
//  If file creator & type are a word template, change it to a document.
//
// Returns:
//  false       Couldn't get file info or unable to change type
//  true        file was not a template or successfully changed
//
//********************************************************************

Boolean WordDocChangeCreator( HFILE hFile )
{

    FCBPBRec    FCBInfo;
    Str31       fileName;
    FInfo       fndrInfo;
    Boolean     fileOK = true;
 
    // get file info for this file
    // first get FCB info for the open file, then get the finder info

    FCBInfo.ioFCBIndx = 0;  // get info for this file
    FCBInfo.ioRefNum = hFile;
    FCBInfo.ioNamePtr = fileName;
    if (PBGetFCBInfoSync( &FCBInfo ) == noErr)
        {

        if (HGetFInfo( FCBInfo.ioFCBVRefNum, FCBInfo.ioFCBParID, fileName, &fndrInfo) == noErr)
            {
            // if file is Word template
            if ((fndrInfo.fdCreator == 'MSWD') &&
                (fndrInfo.fdType == 'WTBN'))
                {
                // change to Word document
                fndrInfo.fdType = 'W6BN';
 
                if (HSetFInfo( FCBInfo.ioFCBVRefNum, FCBInfo.ioFCBParID, fileName, &fndrInfo) != noErr)
                    fileOK = false;
                }
            }
        else // can't get info; return file not fixed
            fileOK = false;
        }
    else // can't get FCB info; return file not fixed
        fileOK = false;
 
    return fileOK;
}
#endif

//*************************************************************************
//
// WD_STATUS WDDoQuickRepair()
//
// Parameters:
//  lpstScan                Ptr to scan structure
//
// Description:
//  Deletes all macros and then sets the TDT size to zero.
//
// Returns:
//  WD_STATUS_ERROR         If repair failed
//  WD_STATUS_OK            If repair succeeded
//
//*************************************************************************

WD_STATUS WDDoQuickRepair
(
    LPWD_SCAN           lpstScan
)
{
    WORD                wMacroIdx;
    WD7_TDT_INFO_T      stTDTInfo;
    BOOL                bActive;

    // Initialize TDT structure

    if (WD7InitTDTInfo(lpstScan->lpstStream,
                       lpstScan->uScan.stWD7.lpstMacroScan->lpstKey,
                       &stTDTInfo) != WD7_STATUS_OK)
    {
        // Error getting TDT info

        return(WD_STATUS_ERROR);
    }

    // Delete all the macros

    for (wMacroIdx=0;wMacroIdx<stTDTInfo.wNumMCDs;wMacroIdx++)
    {
        // Get the macro info

        if (WD7GetMacroInfoAtIndex(&stTDTInfo,
                                   wMacroIdx,
                                   lpstScan->abyName,
                                   &lpstScan->uScan.stWD7.dwMacroOffset,
                                   &lpstScan->uScan.stWD7.dwMacroSize,
                                   &lpstScan->uScan.stWD7.byMacroEncryptByte,
                                   &bActive) == WD7_STATUS_ERROR ||
            bActive == FALSE)
        {
            // Error getting this macro or not an active macro,
            //  go to the next one

            continue;
        }


        if (WD7DeactivateMacroAtIndex(&stTDTInfo,
                                      wMacroIdx) != WD7_STATUS_OK)
        {
            // Error deleting the macro

            return(WD_STATUS_ERROR);
        }
    }

    // Set the TDT size to zero

    if (WD7SetTDTSize(lpstScan->lpstStream,
                      lpstScan->uScan.stWD7.lpstMacroScan->lpstKey,
                      0) != WD7_STATUS_OK)
    {
        // Error deleting the macro

        return(WD_STATUS_ERROR);
    }

    return(WD_STATUS_OK);
}


//********************************************************************
//
// Function:
//  BOOL ShouldDoQuickRepair()
//
// Parameters:
//  wVID                    VID
//  lpszFileName            File name
//
// Description:
//  Repairs the first WordDocument stream found that is infected
//  with the virus identified by VID.
//
// Returns:
//  TRUE                    If quick repair should be done
//  FALSE                   If quick repair should not be done
//
//********************************************************************

#if defined(SYM_NTK)

// Assume compiling under little endian machine

TCHAR gszJapaneseNormalDot[] = { 0x5795, 0x808F, '.', 'd', 'o', 't' };

#else

// Assume single byte Japanese normal.dot filename

TCHAR gszJapaneseNormalDot[] =
    { (TCHAR)0x95, (TCHAR)0x57, (TCHAR)0x8F, (TCHAR)0x80, '.', 'd', 'o', 't' };

#endif

LPTSTR galpszQuickRepairFileNames[] =
{
    _T("normal.dot"),
    (LPTSTR)gszJapaneseNormalDot
};

#define NUM_QUICK_NAMES (sizeof(galpszQuickRepairFileNames)/sizeof(LPTSTR))

BOOL ShouldDoQuickRepair
(
    WORD        wVID,
    LPTSTR      lpszFileName
)
{
    int         i, nLastColon, nLastBackSlash, nFileName;

    switch (wVID)
    {
        case VID_CAPA:
        case VID_CAPB:
        case VID_CAPC:
        case VID_CAPD:
        case VID_CAPE:
        case VID_CAPF:
        case VID_CAPG:
        case VID_CAPH:
        case VID_CAPI:
        case VID_CAPJ:
        case VID_CAPK:
        case VID_CAPL:
        case VID_CAPM:
        case VID_CAPN:
        case VID_CAPO:
        case VID_CAPP:
        case VID_CAPQ:
        case VID_CAPR:
        case VID_CAPS:
        case VID_CAPT:
        case VID_CAPU:
        case VID_CAPV:
        case VID_CAPW:
        case VID_CAPX:
        case VID_CAPY:
        case VID_CAPZ:
        case VID_CAPAA:
        case VID_CAPAB:
        case VID_CAPAC:
        case VID_CAPAD:
        case VID_CAPAE:
        case VID_CAPAF:
        case VID_CAPAH:
        case VID_CAPAI:
        case VID_CAPAJ:
        case VID_CAPAK:
        case VID_CAPAL:
        case VID_CAPAM:
        case VID_CAPAN:
        case VID_CAPAO:
        case VID_CAPAQ:
        case VID_CAPAR:
        case VID_CAPAS:
        case VID_CAPAT:
        case VID_CAPAU:
        case VID_CAPAV:
        case VID_CAPAW:
        case VID_CAPAX:
        case VID_CAPAY:
        case VID_CAPAZ:
        case VID_CAPBA:
        case VID_CAPBB:
        case VID_CAPBC:
        case VID_CAPBD:
        case VID_CAP_FAMILY:
        case VID_CAP_DAMAGED:
            // Now check the file name

            // Find the start of the name by
            //  Finding the last colon, the last backslash

            i = 0;
            nLastColon = nLastBackSlash = 0;
            while (lpszFileName[i])
            {
                if (lpszFileName[i] == ':')
                    nLastColon = i;
                else
#if defined(SYM_UNIX)
                if (lpszFileName[i] == '/')
#else
                if (lpszFileName[i] == '\\')
#endif
                    nLastBackSlash = i;

                ++i;
            }

            // Determine where the name starts
            //  If there was no colon and no

            if (nLastBackSlash > nLastColon)
            {
                if (nLastBackSlash == 0)
                    nFileName = 0;
                else
                    nFileName = nLastBackSlash + 1;
            }
            else
            {
                if (nLastColon == 0)
                    nFileName = 0;
                else
                    nFileName = nLastColon + 1;
            }

            // Determine whether it is a global template name

            for (i=0;i<NUM_QUICK_NAMES;i++)
            {
                if (TStringIEq(galpszQuickRepairFileNames[i],
                               lpszFileName + nFileName) == TRUE)
                {
                    // Found a match

                    return(TRUE);
                }
            }
            break;

        default:
            break;
    }

    return (FALSE);
}

// Decryption enhanced function below

//********************************************************************
//
// Function:
//  EXTSTATUS WD7MacroVirusRepair()
//
// Parameters:
//  lpstStream              WordDocument stream
//  lpszFileName            File name
//  hFile                   Handle to file
//  bTryHeuristicRepair     TRUE if heuristic repair is enabled
//  lpstVirusSigInfo        Info of virus to repair
//  lpbyWorkBuffer          Work buffer >= 1400 bytes
//
// Description:
//  Repairs the first WordDocument stream found that is infected
//  with the virus identified by VID.
//
// Returns:
//  EXTSTATUS_OK            If the repair was successful
//  EXTSTATUS_NO_REPAIR     If the repair was unsuccessful
//
//********************************************************************

EXTSTATUS WD7MacroVirusRepair
(
    LPMSOFFICE_SCAN     lpstOffcScan,
    LPTSTR              lpszFileName,
    HFILE               hFile,
#ifdef MACROHEU
    BOOL                bTryHeuristicRepair,
#endif // #ifdef MACROHEU
    LPWD_VIRUS_SIG_INFO lpstVirusSigInfo,
    LPBYTE              lpbyWorkBuffer
)
{
    EXTSTATUS           extStatus = EXTSTATUS_OK;
    WD7ENCKEY_T         stKey;
    WORD                wVID;
    LPWD_SCAN           lpstScan;
    BOOL                bHasAutoText;
    BOOL                bMVPApproved;

    WD7_TDT_INFO_T      stTDTInfo;

    lpstScan = lpstOffcScan->lpstScan;

    /////////////////////////////////////////////////////////////
    // Initialize for Word 6.0/7.0/95 scanning
    /////////////////////////////////////////////////////////////

    // Allocate a macro scan structure

    if (WD7AllocMacroScanStruct(lpstOffcScan->lpstRoot->lpvRootCookie,
                                &lpstScan->uScan.stWD7.lpstMacroScan,
                                0,
                                0) != WD7_STATUS_OK)
    {
        return(EXTSTATUS_NO_REPAIR);
    }

    // Assign hit memory

    if (AssignMacroSigHitMem(lpstOffcScan,
                             lpstScan) == FALSE)
    {
        WD7FreeMacroScanStruct(lpstOffcScan->lpstRoot->lpvRootCookie,
                               lpstScan->uScan.stWD7.lpstMacroScan);

        return(EXTSTATUS_NO_REPAIR);
    }

    // Initialize other scan structure fields

    lpstScan->lpstStream = lpstOffcScan->lpstStream;
    lpstScan->wScanType = WD_SCAN_TYPE_WD7;
    lpstScan->uScan.stWD7.lpstMacroScan->lpstKey = &stKey;

	// Verify that it is a Word file and that it has macros

    if (WD7FindFirstKey(lpstOffcScan->lpstStream,
                        &stKey,
                        &gstRevKeyLocker) != WD7_STATUS_OK)
    {
        // Failure to find key or determining whether it
        //  was encrypted

        WD7FreeMacroScanStruct(lpstOffcScan->lpstRoot->lpvRootCookie,
                               lpstScan->uScan.stWD7.lpstMacroScan);

        return(EXTSTATUS_NO_REPAIR);
    }


    /////////////////////////////////////////////////////////////
    // The stream passed in must be the WordDocument stream
    //  containing an infection with the given VID.
    /////////////////////////////////////////////////////////////

    // Find the key

    wVID = lpstVirusSigInfo->wID;
    while (1)
    {
        bMVPApproved = TRUE;
        if (WDScanDoc(&gstWD7SigSet,
                      lpstScan,
                      &lpstVirusSigInfo,
                      &bMVPApproved) == WD_STATUS_VIRUS_FOUND &&
            lpstVirusSigInfo->wID == wVID ||
            bMVPApproved == FALSE)
        {
            if (bMVPApproved == FALSE)
                lpstVirusSigInfo = &gstMVPVirusSigInfo;

            break;
        }

        lpstVirusSigInfo = NULL;

        if (stKey.bEncrypted == FALSE)
        {
            // If not encrypted, then just scan once

            break;
        }

        // Try to find another key

        if (WD7FindNextKey(lpstOffcScan->lpstStream,
                           &stKey,
                           &gstRevKeyLocker) != WD7_STATUS_OK)
            break;
	}

    if (lpstVirusSigInfo != NULL &&
        ShouldDoQuickRepair(wVID,lpszFileName) == TRUE)
    {
        if (WDDoQuickRepair(lpstScan) != WD_STATUS_OK)
            extStatus = EXTSTATUS_NO_REPAIR;
    }
    else
    if (lpstVirusSigInfo != NULL)
    {
        // Found the stream with the given virus

        // Repair it

        if (WDIsFullSetRepair(lpstVirusSigInfo) == TRUE)
        {
            // Do full set repair

            if (WDDoFullSetRepair(&gstWD7SigSet,
                                  lpstScan) != WD_STATUS_OK)
                extStatus = EXTSTATUS_NO_REPAIR;
        }

//////////////////////////////////////////////////////////////////////
#ifdef MACROHEU

        if (bTryHeuristicRepair == TRUE &&
            WDIsHeuristicRepair(lpstVirusSigInfo) == TRUE)
        {
            // Do heuristic repair

            if (WD7DoHeuristicScanRepair(lpstOffcScan->lpstStream,
                                         &stKey,
                                         TRUE) != EXTSTATUS_OK)
            {
                extStatus = EXTSTATUS_NO_REPAIR;
            }
        }

#endif // #ifdef MACROHEU
//////////////////////////////////////////////////////////////////////

        if (extStatus == EXTSTATUS_OK)
        {
            if (WDRepairDoc(&gstWD7SigSet,
                            lpstScan,
                            lpstVirusSigInfo) != WD_STATUS_OK)
                extStatus = EXTSTATUS_NO_REPAIR;
        }

        if (extStatus == EXTSTATUS_OK)
        {
            // Repair successful

            // Get the number of active macros

            if (WD7InitTDTInfo(lpstOffcScan->lpstStream,
                               &stKey,
                               &stTDTInfo) != WD7_STATUS_OK ||
                WD7CountActiveMacros(&stTDTInfo,
                                     &lpstScan->wMacroCount) != WD7_STATUS_OK)
            {
                // Error getting TDT info or macro count

                extStatus = EXTSTATUS_NO_REPAIR;
            }

            // Call to do menu repair

            if (extStatus == EXTSTATUS_OK)
            {
                if (WD7DoMenuRepair(&gstWD7SigSet,
                                    lpstScan,
                                    lpstOffcScan->lpstStream,
                                    &stKey,
                                    &stTDTInfo,
                                    lpstVirusSigInfo,
                                    0) == FALSE)
                {
                    extStatus = EXTSTATUS_NO_REPAIR;
                }
            }

            // Determine whether or not to set the TDT size
            //  to zero
            //
            // Set the TDT size to zero if all of the following hold:
            //  1. The document has no active macros
            //  2. The document has no non-macro information

            if (extStatus == EXTSTATUS_OK &&
                lpstScan->wMacroCount == 0 &&
                WD7TDTHasNonMacroInfo(&stTDTInfo) == FALSE)
            {
                if (WD7SetTDTSize(lpstOffcScan->lpstStream,
                                  &stKey,
                                  0) != WD7_STATUS_OK)
                    extStatus = EXTSTATUS_NO_REPAIR;
            }

            // Determine whether to leave as template or
            //  convert back to a document
            //
            // Leave as a template if any of the following are true:
            //  - After repair, the document still has active macros
            //  - The document has non-macro information in the
            //      template data table
            //  - The last four characters of the file name are ".DOT"
            //  - The last six characters of the file name are "NORMAL"
            //
            // Otherwise, if none of the above conditions are true,
            //  then convert back to a document.

            if (extStatus == EXTSTATUS_OK &&
                lpstScan->wMacroCount == 0 &&
                EndsInDOTOrNORMAL(lpszFileName) == 0 &&
                WD7TDTHasNonMacroInfo(&stTDTInfo) == FALSE &&
                WD7HasAutoText(lpstOffcScan->lpstStream,
                               &stKey,
                               &bHasAutoText) == TRUE &&
                bHasAutoText == FALSE)
            {
                if (WD7ChangeToDocument(lpstOffcScan->lpstStream,
                                        &stKey) != WD7_STATUS_OK)
                    extStatus = EXTSTATUS_NO_REPAIR;
#ifdef __MACINTOSH__
                else
                if (!WordDocChangeCreator( hFile ))
                    extStatus = EXTSTATUS_NO_REPAIR;
#endif
            }
        }
    }
    else
    {
        // This should never happen

        extStatus = EXTSTATUS_NO_REPAIR;
    }

    // Free the scan structure

    WD7FreeMacroScanStruct(lpstOffcScan->lpstRoot->lpvRootCookie,
                           lpstScan->uScan.stWD7.lpstMacroScan);

    return(extStatus);
}


#ifdef MACROHEU

//********************************************************************
//
// Function:
//  EXTSTATUS WD7HeuristicMacroVirusRepair()
//
// Parameters:
//  lpstStream              WordDocument stream
//  lpszFileName            File name
//  hFile                   Handle to file
//
// Description:
//  Repairs the first WordDocument stream found that is infected
//  with the virus identified by VID.
//
// Returns:
//  EXTSTATUS_OK            If the repair was successful
//  EXTSTATUS_NO_REPAIR     If the repair was unsuccessful
//
//********************************************************************

EXTSTATUS WD7HeuristicMacroVirusRepair
(
    LPMSOFFICE_SCAN     lpstOffcScan,
    LPTSTR              lpszFileName,
    HFILE               hFile
)
{
    EXTSTATUS           extStatus = EXTSTATUS_OK;
    WD7ENCKEY_T         stKey;
    BOOL                bFound;
    BOOL                bHasAutoText;
    WORD                wMacroCount;
    WD7_TDT_INFO_T      stTDTInfo;

    /////////////////////////////////////////////////////////////
    // Initialize for Word 6.0/7.0/95 scanning
    /////////////////////////////////////////////////////////////

	// Verify that it is a Word file and that it has macros

    if (WD7FindFirstKey(lpstOffcScan->lpstStream,
                        &stKey,
                        &gstRevKeyLocker) != WD7_STATUS_OK)
    {
        // Failure to find key or determining whether it
        //  was encrypted

        return(EXTSTATUS_NO_REPAIR);
    }


    /////////////////////////////////////////////////////////////
    // The stream passed in must be the WordDocument stream
    //  containing an infection with the given VID.
    /////////////////////////////////////////////////////////////

    // Find the key

    bFound = FALSE;
    while (1)
    {
        if (WD7DoHeuristicScanRepair(lpstOffcScan->lpstStream,
                                     &stKey,
                                     FALSE) == WD_STATUS_VIRUS_FOUND)
        {
            bFound = TRUE;
            break;
        }

        if (stKey.bEncrypted == FALSE)
        {
            // If not encrypted, then just scan once

            break;
        }

        // Try to find another key

        if (WD7FindNextKey(lpstOffcScan->lpstStream,
                           &stKey,
                           &gstRevKeyLocker) != WD7_STATUS_OK)
            break;
	}

    if (bFound != FALSE)
    {
        // Found the stream with the virus

        // Repair it

        if (WD7DoHeuristicScanRepair(lpstOffcScan->lpstStream,
                                     &stKey,
                                     TRUE) != EXTSTATUS_OK)
        {
            extStatus = EXTSTATUS_NO_REPAIR;
        }

        if (extStatus == EXTSTATUS_OK)
        {
            // Repair successful

            // Get the number of active macros

            if (WD7InitTDTInfo(lpstOffcScan->lpstStream,
                               &stKey,
                               &stTDTInfo) != WD7_STATUS_OK ||
                WD7CountActiveMacros(&stTDTInfo,
                                     &wMacroCount) != WD7_STATUS_OK)
            {
                // Error getting TDT info or macro count

                extStatus = EXTSTATUS_NO_REPAIR;
            }

            // Determine whether to leave as template or
            //  convert back to a document
            //
            // Leave as a template if any of the following are true:
            //  - After repair, the document still has active macros
            //  - The document has non-macro information in the
            //      template data table
            //  - The last four characters of the file name are ".DOT"
            //  - The last six characters of the file name are "NORMAL"
            //
            // Otherwise, if none of the above conditions are true,
            //  then convert back to a document.

            if (extStatus == EXTSTATUS_OK &&
                wMacroCount == 0 &&
                EndsInDOTOrNORMAL(lpszFileName) == 0 &&
                WD7TDTHasNonMacroInfo(&stTDTInfo) == FALSE &&
                WD7HasAutoText(lpstOffcScan->lpstStream,
                               &stKey,
                               &bHasAutoText) == TRUE &&
                bHasAutoText == FALSE)
            {
                if (WD7ChangeToDocument(lpstOffcScan->lpstStream,
                                        &stKey) != WD7_STATUS_OK)
                    extStatus = EXTSTATUS_NO_REPAIR;
#ifdef __MACINTOSH__
                else
                if (!WordDocChangeCreator( hFile ))
                    extStatus = EXTSTATUS_NO_REPAIR;
#endif
            }
        }
    }
    else
    {
        // This should never happen

        extStatus = EXTSTATUS_NO_REPAIR;
    }

    return(extStatus);
}

#endif // #ifdef MACROHEU


//********************************************************************
//
// Function:
//  EXTSTATUS W97MacroVirusRepair()
//
// Parameters:
//  lpstOffcScan            Ptr to office scan structure
//  bTryHeuristicRepair     TRUE if heuristic repair is enabled
//  lpstVirusSigInfo        Info of virus to repair
//  lpbyWorkBuffer          Work buffer >= 1400 bytes
//
// Description:
//  Repairs Word 97 document of the given virus.
//
// Returns:
//  EXTSTATUS_OK            If the repair was successful
//  EXTSTATUS_NO_REPAIR     If the repair was unsuccessful
//
//********************************************************************

EXTSTATUS W97MacroVirusRepair
(
    LPMSOFFICE_SCAN     lpstOffcScan,
#ifdef MACROHEU
    BOOL                bTryHeuristicRepair,
#endif // #ifdef MACROHEU
    LPWD_VIRUS_SIG_INFO lpstVirusSigInfo,
    LPBYTE              lpbyWorkBuffer
)
{
    EXTSTATUS           extStatus = EXTSTATUS_OK;

    if (WDIsFullSetRepair(lpstVirusSigInfo) == TRUE)
    {
        // Do full set repair

        if (WD8DoFullSetRepair(lpstOffcScan,
                               lpbyWorkBuffer) != WD_STATUS_OK)
            extStatus = EXTSTATUS_NO_REPAIR;
    }

//////////////////////////////////////////////////////////////////////
#ifdef MACROHEU

    if (bTryHeuristicRepair == TRUE &&
        WDIsHeuristicRepair(lpstVirusSigInfo) == TRUE)
    {
        // Do heuristic repair

        if (Word97HeuristicScan(lpstOffcScan,
                                TRUE) != EXTSTATUS_OK)
        {
            extStatus = EXTSTATUS_NO_REPAIR;
        }
    }

#endif // #ifdef MACROHEU
//////////////////////////////////////////////////////////////////////

    if (extStatus == EXTSTATUS_OK)
    {
        extStatus = WD8MacroVirusRepair(lpstOffcScan,
                                        lpbyWorkBuffer,
                                        lpstVirusSigInfo);
    }

    return(extStatus);
}


//********************************************************************
//
// Function:
//  BOOL WD8TemplateToDocumentStep()
//
// Parameters:
//  lpstOffcScan        Ptr to office scan structure
//  lpszFileName        File name of repaired file
//
// Description:
//  Opens the WordDocument stream, checks to see whether it has
//  any AutoText entries or ends in DOT or NORMAL.  If either
//  condition is true, the document is left as is.  Otherwise,
//  the template bit is reset to zero to indicate a document.
//
// Returns:
//  TRUE                On success
//  FALSE               On failure
//
//********************************************************************

BOOL WD8TemplateToDocumentStep
(
    LPMSOFFICE_SCAN     lpstOffcScan,
    LPTSTR              lpszFileName
)
{
    BOOL                bHasAutoText;

    // Open the WordDocument stream

    if (SSOpenStreamAtIndex(lpstOffcScan->lpstStream,
                            lpstOffcScan->u.stO97.u.stWD8.
                                dwWordDocumentEntry) != SS_STATUS_OK)
    {
        return(FALSE);
    }

    // Determine whether the document has AutoText

    if (O97WDHasAutoText(lpstOffcScan->lpstStream,
                         &bHasAutoText) == FALSE)
    {
        // Failed to determine AutoText status

        return(FALSE);
    }

    // If the document has no AutoText and does not end in either
    //  DOT or NORMAL, then change it to a document.

    if (bHasAutoText == FALSE &&
        EndsInDOTOrNORMAL(lpszFileName) == 0)
    {
        return O97WDChangeToDocument(lpstOffcScan->lpstStream);
    }

    return(TRUE);
}


#ifdef __MACINTOSH__

// prototypes
Boolean ExcelMoveStartupCopy( HFILE fileRef );
Boolean EqualSubstr( uchar *str1, uchar *str2, uchar len );

//*************************************************************************
//
// Function:
//  WORD ExcelMoveStartupCopy()
//
// Description:
//  Tries to move any backup copy of infected file out of Excel Startup folder
//
// Returns:
//  false       On failure
//  true        On success or no backup found moveable
//
//*************************************************************************

Boolean ExcelMoveStartupCopy( HFILE fileRef )
{

	FCBPBRec	FCBInfo;
	Str31		repFileName;
	Boolean 	fileOK = true;
	uchar		startupFolderName[] = "\pExcel Startup Folder"; //make resource for int'l?
	
	// get file info for repaired file

	FCBInfo.ioFCBIndx = 0;  // info for this file
	FCBInfo.ioRefNum = fileRef;
	FCBInfo.ioNamePtr = repFileName;
	
	if (PBGetFCBInfoSync( &FCBInfo ) == noErr)
		{
		OSErr		err;
		CInfoPBRec	cpb;
		Str31		name;
		long		dirID = FCBInfo.ioFCBParID;
		short		vRef = FCBInfo.ioFCBVRefNum;
		short		repNameLen = *repFileName;
	
		// get the directory name
		name[0] = 0;
		cpb.hFileInfo.ioCompletion = nil;
		cpb.dirInfo.ioNamePtr = name;
		cpb.dirInfo.ioVRefNum = vRef;
		cpb.dirInfo.ioDrDirID =  dirID;
		cpb.hFileInfo.ioFDirIndex = -1;  // get info on dir
		err = PBGetCatInfoSync( &cpb );
	
		if (err == noErr)
			{
			// if directory name starts with "Excel Startup Folder"
			if (*name >= *startupFolderName && EqualSubstr( name+1, startupFolderName+1, *startupFolderName ))
				{
				uchar	ellipsis = (FontScript() == smJapanese) ? 0xFF : 'É';
				short 	dirIndex = 1;
	
				// walk the directory
				do
					{
					name[0] = 0;
					cpb.hFileInfo.ioCompletion = nil;
					cpb.hFileInfo.ioNamePtr = name;
					cpb.hFileInfo.ioVRefNum = vRef;
					cpb.hFileInfo.ioFDirIndex = dirIndex;
					cpb.hFileInfo.ioDirID = dirID;
	
					err = PBGetCatInfoSync( &cpb );
	
					if ( err == noErr )
						{
						// if not a directory
						if ( (cpb.hFileInfo.ioFlAttrib & ioDirMask) == 0 )
							{
							short 	nameLen = *name;
							Boolean	needMove = false;
	
							// if end of file name is same as repaired file's, move it
							if ( (repNameLen < nameLen) &&
									EqualSubstr( repFileName+1, name + nameLen - repNameLen + 1, repNameLen ))
								needMove = true;
	
							// also move if the file name was ellipsized (copy name too long)
							//  and it's not the same as the repaired one
							else if ( (nameLen == 31) && (name[nameLen] == ellipsis)
									 && ( (repNameLen != 31) || !EqualSubstr( repFileName+1, name+1, 31 ) ))
								needMove = true;
	
							if (needMove)
								{
								OSErr 		moveErr;
								CMovePBRec	pb;

								// attempt to move file to root dir of this drive
	
								pb.ioCompletion = nil;
								pb.ioNamePtr = name;
								pb.ioVRefNum = vRef;
								pb.ioDirID = dirID;
								pb.ioNewDirID = fsRtDirID;
								pb.ioNewName = nil;
	
								moveErr = PBCatMoveSync( &pb );
	
								// flag any failure but continue attempt to move others
								if (moveErr != noErr)
									fileOK = false;
								else
									// decrement index so we don't skip next file (move decrements # files in dir)
									dirIndex--;
								}
							}
	
						dirIndex++;
						}
	
					} while ( err == noErr );
	
				}
			}
		else // can't get dir name
			fileOK = false;
		}
	else
		fileOK = false;

	return fileOK;
}

// compare two memory blocks for a specified length
Boolean EqualSubstr( uchar *str1, uchar *str2, uchar len )
{
	short index;
	
	for ( index = 0; index < len; index++ )
		if (str1[index] != str2[index])
			break;
	
	return (index >= len);
}

#else

//*************************************************************************
//
// Function:
//  WORD ExcelTStringNCmpI()
//
// Description:
//  Compares the two strings up to N characters with case insensitivity.
//
// Returns:
//  0       If both strings are the same up to N characters
//  1       if the strings differ
//
//*************************************************************************

WORD ExcelTStringNCmpI
(
    LPTSTR          lptstr0,
    LPTSTR          lptstr1,
    int             n
)
{
    TCHAR           tch0, tch1;

    tch0 = *lptstr0;
    tch1 = *lptstr1;
    while (tch0 && tch1 && n > 0)
    {
        // Convert to upper case

        if ('a' <= tch0 && tch0 <= 'z')
            tch0 = (tch0 - 'a') + 'A';

        if ('a' <= tch1 && tch1 <= 'z')
            tch1 = (tch1 - 'a') + 'A';

        if (tch0 != tch1)
            return 1;

        tch0 = *++lptstr0;
        tch1 = *++lptstr1;
        --n;
    }

    if (n == 0)
    {
        // Equal

        return 0;
    }

    // Unequal

    return 1;
}


//*************************************************************************
//
// Function:
//  void ExcelTryHideName()
//
// Description:
//  Tries a candidate name to hide.
//
// Returns:
//  Nothing.
//
//*************************************************************************

void ExcelTryHideName
(
    LPCALLBACKREV1  lpCallBack,         // File op callbacks
    LPTSTR          lptstrFileName,     // Infected file's name
    LPTSTR          lptstrHideName,     // Candidate name buffer
    int             nStrLen             // Length of name
)
{
    if (ExcelTStringNCmpI(lptstrFileName,
                          lptstrHideName,
                          nStrLen) == 0)
    {
        // Strings are equal, can't use it

        return;
    }

    // Try changing attributes to hidden

    lpCallBack->FileSetAttr(lptstrHideName,2);

    return;
}


//*************************************************************************
//
// Function:
//  WORD ExcelXLStartBackup()
//
// Description:
//  Tries to hide the NAV backup of the specified file if the
//  directory it is in is XLSTART
//
// Returns:
//  0       On failure
//  1       On success or no backup found hideable
//
//*************************************************************************

TCHAR gtchXLSTART[] = { 'X', 'L', 'S', 'T', 'A', 'R', 'T' };

WORD ExcelHideXLStartBackup
(
    LPCALLBACKREV1  lpCallBack,         // File op callbacks
    LPTSTR          lpszFileName,       // Infected file's name
    LPBYTE          lpbyWorkBuffer      // Work buffer >= 2048 bytes
)
{
    int             nStrLen;
    int             i;
    LPTSTR          lptstrCur;
    LPTSTR          lptstrEnd;
    LPBYTE          lpby;

    if (lpszFileName == NULL || *lpszFileName == 0)
    {
        // Don't know what to hide

        return 0;
    }

    //////////////////////////////////////////////////////////////////
    // Look for XLStart
    //////////////////////////////////////////////////////////////////

    lptstrCur = lpszFileName;
    while (*lptstrCur != 0)
    {
        if (ExcelTStringNCmpI(lptstrCur,gtchXLSTART,7) == 0)
            break;

        ++lptstrCur;
    }

    if (*lptstrCur == 0)
    {
        // Not XLSTART

        return 1;
    }

    //////////////////////////////////////////////////////////////////
    // Find the location of the extension and set up buffer
    //  with filename to hide
    //////////////////////////////////////////////////////////////////

    // Find the length of the string

    nStrLen = 0;
    lptstrCur = lpszFileName;
    while (lptstrCur[nStrLen] != 0)
        ++nStrLen;

    // Don't do it if the extension is not a three character
    //  ASCII extension or if the length is greater than 1024 chars

    if (nStrLen < 4)
        return 1;

    lptstrEnd = lpszFileName + nStrLen - 4;

    if (lptstrEnd[0] != '.' ||
        lptstrEnd[1] < 1 || lptstrEnd[1] > 127 ||
        lptstrEnd[2] < 1 || lptstrEnd[2] > 127 ||
        lptstrEnd[3] < 1 || lptstrEnd[3] > 127 ||
        ((nStrLen + 1) * sizeof(TCHAR)) > 1024)
        return 1;

    // Make a copy of the filename

    lpby = (LPBYTE)lpszFileName;
    for (i=0;i<=(int)(nStrLen*sizeof(TCHAR));i++)
        lpbyWorkBuffer[i] = lpby[i];

    // Point to the character after the period

    lptstrEnd = (LPTSTR)(lpbyWorkBuffer +
        ((LPBYTE)lptstrEnd - (LPBYTE)lpszFileName)) + 1;

    //////////////////////////////////////////////////////////////////
    // Try to hide all VIR and VI{0-9} extensions
    //////////////////////////////////////////////////////////////////

    // Initialize with .VIR and try it

    lptstrEnd[0] = 'V';
    lptstrEnd[1] = 'I';
    lptstrEnd[2] = 'R';

    ExcelTryHideName(lpCallBack,
                     lpszFileName,
                     (LPTSTR)lpbyWorkBuffer,
                     nStrLen);

    // Try .VI0 through .VI9

    for (i=0;i<10;i++)
    {
        lptstrEnd[2] = '0' + i;

        ExcelTryHideName(lpCallBack,
                         lpszFileName,
                         (LPTSTR)lpbyWorkBuffer,
                         nStrLen);
    }

    return 1;
}

#endif  //__MACINTOSH__


//********************************************************************
//
// Function:
//  WORD WordMacroVirusRepair()
//
// Description:
//  Repairs the first WordDocument stream found that is infected
//  with the virus identified by VID.
//
// Returns:
//  EXTSTATUS_OK            If the repair was successful
//  EXTSTATUS_NO_REPAIR     If the repair was unsuccessful
//
//********************************************************************

EXTSTATUS WordMacroVirusRepair
(
    LPCALLBACKREV1  lpstCallBack,           // File op callbacks
    WORD            wVID,                   // ID of virus to repair
    LPTSTR          lpszFileName,           // Infected file's name
#ifdef __MACINTOSH__
    HFILE           hFile,                  // Handle to file to repair
#endif
    LPBYTE          lpbyWorkBuffer          // Work buffer >= 2048 bytes
)
{
    EXTSTATUS           extStatus = EXTSTATUS_OK;
#ifndef __MACINTOSH__
    HFILE               hFile;
#endif
    UINT                uAttr;
    MSOFFICE_SCAN_T     stMSOfficeScan;
    LPWD_VIRUS_SIG_INFO lpstVirusSigInfo;
    BOOL                bFound;

#ifdef MACROHEU
    BOOL                bTryHeuristicRepair = TRUE;

#if !defined(NAVEX15) && (defined(SYM_WIN16) || defined(SYM_DOSX))

    // Check the version number passed in as the first byte
    //  of the work buffer

    if (lpbyWorkBuffer[0] < 3)
        bTryHeuristicRepair = FALSE;

#endif

#endif // #ifdef MACROHEU

#ifndef __MACINTOSH__
    if (RepairFileOpen(lpstCallBack,lpszFileName,&hFile,&uAttr) != 0)
        return(EXTSTATUS_NO_REPAIR);
#endif

    /////////////////////////////////////////////////////////////
    // Allocate root, stream, hit memory, and scan memory
    /////////////////////////////////////////////////////////////

    // Create root structure

    if (SSCreateRoot(&stMSOfficeScan.lpstRoot,
                     lpstCallBack,
                     (LPVOID)hFile,
                     SS_ROOT_FLAG_DIR_CACHE_DEF |
                      SS_ROOT_FLAG_FAT_CACHE_DEF) != SS_STATUS_OK)
    {
        // Failed to create root structure

#ifndef __MACINTOSH__
        RepairFileClose(lpstCallBack,lpszFileName,hFile,uAttr);
#endif
        return(EXTSTATUS_MEM_ERROR);
    }

    // Create stream structure

    if (SSAllocStreamStruct(stMSOfficeScan.lpstRoot,
                            &stMSOfficeScan.lpstStream,
                            SS_STREAM_FLAG_DEF_BAT_CACHE) != SS_STATUS_OK)
    {
        // First destroy the root structure

        SSDestroyRoot(stMSOfficeScan.lpstRoot);

        // Failed to create stream structure

#ifndef __MACINTOSH__
        RepairFileClose(lpstCallBack,lpszFileName,hFile,uAttr);
#endif
        return(EXTSTATUS_MEM_ERROR);
    }

    // Allocate hit memory

    if (AllocMacroSigHitMem(lpstCallBack,
                            &stMSOfficeScan) == FALSE)
    {
        // Release allocated memory

        SSFreeStreamStruct(stMSOfficeScan.lpstStream);
        SSDestroyRoot(stMSOfficeScan.lpstRoot);

        // Failed to create stream structure

#ifndef __MACINTOSH__
        RepairFileClose(lpstCallBack,lpszFileName,hFile,uAttr);
#endif
        return(EXTSTATUS_MEM_ERROR);
    }

    // Allocate scan memory

    if (WDAllocScanStruct(lpstCallBack,
                          &stMSOfficeScan.lpstScan,
                          DEF_WD_RUN_BUF_SIZE,
                          DEF_WD_EXTRA_BUF_SIZE) != WD_STATUS_OK)
    {
        // Release allocated memory

        FreeMacroSigHitMem(lpstCallBack,&stMSOfficeScan);
        SSFreeStreamStruct(stMSOfficeScan.lpstStream);
        SSDestroyRoot(stMSOfficeScan.lpstRoot);

        // Failed to create stream structure

#ifndef __MACINTOSH__
        RepairFileClose(lpstCallBack,lpszFileName,hFile,uAttr);
#endif
        return(EXTSTATUS_MEM_ERROR);
    }


    /////////////////////////////////////////////////////////////
    // Find out what to repair
    /////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
#ifdef MACROHEU

    if ((wVID == VID_MACRO_HEURISTICS || wVID == VID_XH) &&
        MSOfficeHeuristicScan(&stMSOfficeScan,&wVID) == EXTSTATUS_VIRUS_FOUND)
    {
        // Found a virus heuristically

        bFound = TRUE;
    }
    else

#endif // #ifdef MACROHEU
//////////////////////////////////////////////////////////////////////

    if (MicrosoftOfficeScan(&stMSOfficeScan,
                            lpbyWorkBuffer,
                            &lpstVirusSigInfo) == EXTSTATUS_VIRUS_FOUND &&
        lpstVirusSigInfo->wID == wVID)
    {
        // Found a virus by the normal means

        bFound = TRUE;
    }
    else
    {
        // Couldn't find the reported virus

        bFound = FALSE;
    }

    if (bFound == TRUE)
    {
        switch (stMSOfficeScan.wType)
        {
            case OLE_SCAN_WD7:
//////////////////////////////////////////////////////////////////////
#ifdef MACROHEU

                if (wVID == VID_MACRO_HEURISTICS)
                {
                    extStatus =
                        WD7HeuristicMacroVirusRepair(&stMSOfficeScan,
                                                     lpszFileName,
                                                     hFile);
                }
                else

#endif // #ifdef MACROHEU
//////////////////////////////////////////////////////////////////////

                {
                    extStatus = WD7MacroVirusRepair(&stMSOfficeScan,
                                                    lpszFileName,
                                                    hFile,
#ifdef MACROHEU
                                                    bTryHeuristicRepair,
#endif // #ifdef MACROHEU
                                                    lpstVirusSigInfo,
                                                    lpbyWorkBuffer);
                }
                break;

            case OLE_SCAN_WD8:
//////////////////////////////////////////////////////////////////////
#ifdef MACROHEU

                if (wVID == VID_MACRO_HEURISTICS)
                {
                    extStatus =
                        Word97HeuristicScan(&stMSOfficeScan,
                                            TRUE);
                }
                else

#endif // #ifdef MACROHEU
//////////////////////////////////////////////////////////////////////

                {
                    extStatus = W97MacroVirusRepair(&stMSOfficeScan,
#ifdef MACROHEU
                                                    bTryHeuristicRepair,
#endif // #ifdef MACROHEU
                                                    lpstVirusSigInfo,
                                                    lpbyWorkBuffer);
                }

                if (extStatus == EXTSTATUS_OK)
                {
                    if (WD8TemplateToDocumentStep(&stMSOfficeScan,
                                                  lpszFileName) ==
                        FALSE)
                    {
                        extStatus = EXTSTATUS_NO_REPAIR;
                    }
                }

                break;

            case OLE_SCAN_XL97:
//////////////////////////////////////////////////////////////////////
#ifdef MACROHEU
                if (wVID == VID_XH)
                {
                    extStatus = XL97HeuristicScan(&stMSOfficeScan,
                                                  TRUE);
                }
                else
#endif // #ifdef MACROHEU
//////////////////////////////////////////////////////////////////////
                {
                    if (WDIsFullSetRepair(lpstVirusSigInfo) == TRUE)
                    {
                        // Do full set repair

                        if (XL97DoFullSetRepair(&stMSOfficeScan,
                                                lpbyWorkBuffer) != WD_STATUS_OK)
                            extStatus = EXTSTATUS_NO_REPAIR;
                    }

                    if (extStatus == EXTSTATUS_OK)
                    {
                        extStatus = XL97MacroVirusRepair(&stMSOfficeScan,
                                                         lpbyWorkBuffer,
                                                         lpstVirusSigInfo);
                    }
                }
                break;

            case OLE_SCAN_XL5:
//////////////////////////////////////////////////////////////////////
#ifdef MACROHEU
                if (wVID == VID_XH)
                {
                    extStatus = XL5HeuristicScan(&stMSOfficeScan,
                                                 TRUE);
                }
                else
#endif // #ifdef MACROHEU
//////////////////////////////////////////////////////////////////////
                {
                    extStatus = XL5Repair(&stMSOfficeScan,
                                          lpbyWorkBuffer,
                                          lpstVirusSigInfo);
                }
                break;

            case OLE_SCAN_XL4_95:
            case OLE_SCAN_XL4_97:
                extStatus = XL4Repair(&stMSOfficeScan,
                                      lpbyWorkBuffer,
                                      lpstVirusSigInfo);
                break;

#ifdef PP_SCAN
            case OLE_SCAN_PP:
                extStatus = PPScan(&stMSOfficeScan,
                                   TRUE,
                                   lpbyWorkBuffer,
                                   &lpstVirusSigInfo,
                                   &stMSOfficeScan.bMVPApproved);
                break;
#endif // #ifdef PP_SCAN

            default:
                extStatus = EXTSTATUS_NO_REPAIR;
                break;
        }
    }
    else
    {
        // Could not find an infected document

        extStatus = EXTSTATUS_NO_REPAIR;
    }

    /////////////////////////////////////////////////////////////
    // Free the memory
    /////////////////////////////////////////////////////////////

    WDFreeScanStruct(lpstCallBack,stMSOfficeScan.lpstScan);
    FreeMacroSigHitMem(lpstCallBack,&stMSOfficeScan);
    SSFreeStreamStruct(stMSOfficeScan.lpstStream);
    SSDestroyRoot(stMSOfficeScan.lpstRoot);

#ifndef __MACINTOSH__
    RepairFileClose(lpstCallBack,lpszFileName,hFile,uAttr);
#endif

    /////////////////////////////////////////////////////////////
    // Delete the backup file if necessary
    /////////////////////////////////////////////////////////////

    if (stMSOfficeScan.wType == OLE_SCAN_XL5 ||
        stMSOfficeScan.wType == OLE_SCAN_XL97)
    {
        // Delete the backup file

#ifndef __MACINTOSH__
        if (ExcelHideXLStartBackup(lpstCallBack,
                                   lpszFileName,
                                   lpbyWorkBuffer) == 0)
#else
        if (!ExcelMoveStartupCopy( hFile ))
#endif
        {
            // Failed to hide the backup

            return(EXTSTATUS_NO_REPAIR);
        }
    }

    return(extStatus);
}

#endif  // #ifndef SYM_NLM



