//***********************************************************************
//
// $Header:   S:/NAVEX/VCS/macrorep.cpv   1.19   19 Jun 1997 15:26:14   DCHI  $
//
// Description:
//      Contains WordDocument macro repair function code.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/macrorep.cpv  $
// 
//    Rev 1.19   19 Jun 1997 15:26:14   DCHI
// Added check of gbInWindows to switch between FuncAnsiNext() and AnsiNext().
// 
//    Rev 1.18   27 May 1997 16:51:04   DCHI
// Modifications to support dynamic allocation of hit memory and new
// Office 97 CRC.
// 
//    Rev 1.17   08 May 1997 15:36:18   DCHI
// Added office 97 conversion of template to document and added criteria
// for absence of AutoText before converting to document for both Word versions.
// 
//    Rev 1.16   09 Apr 1997 18:34:04   DCHI
// Fixed to prevent invalid pointer reference after OfficeScan of Excel 5 docs.
// 
//    Rev 1.15   08 Apr 1997 12:40:36   DCHI
// Added support for FullSet(), FullSetRepair, Or()/Not(), MacroCount(), etc.
// 
//    Rev 1.14   14 Mar 1997 16:34:00   DCHI
// Added support for Office 97 repair.
// 
//    Rev 1.13   13 Feb 1997 13:30:38   DCHI
// Modifications to support VBA 5 scanning.
// 
//    Rev 1.12   23 Jan 1997 11:12:16   DCHI
// Fixed global variable usage problem.
// 
//    Rev 1.11   17 Jan 1997 11:01:42   DCHI
// Modifications supporting new macro engine.
// 
//    Rev 1.10   26 Dec 1996 15:22:54   AOONWAL
// No change.
// 
//    Rev 1.9   02 Dec 1996 14:00:46   AOONWAL
// No change.
// 
//    Rev 1.8   29 Oct 1996 12:59:34   AOONWAL
// No change.
// 
//    Rev 1.7   28 Aug 1996 16:14:42   DCHI
// Use AVDEREF_WORD() for dereferencing WORD values.  Addition of parameters
// for BAT cache support.
// 
//    Rev 1.6   28 May 1996 12:53:52   DCHI
// Integrated a few SAM changes.
// 
//    Rev 1.5   09 May 1996 15:52:52   DCHI
// Removed #ifdef __MACINTOSH__ for filename comparison functions.
// 
//    Rev 1.4   06 May 1996 17:20:02   DCHI
// Modified repair to convert template back to document when appropriate.
// 
//    Rev 1.3   03 May 1996 13:26:56   DCHI
// Endian-enabled the code for Mac compatibility.
// 
//    Rev 1.2   15 Apr 1996 20:32:50   RSTANEV
// TCHAR support.
//
//    Rev 1.1   30 Jan 1996 15:43:30   DCHI
// Added #ifndef SYM_NLM and #endif pairs to ifdef out functions unused
// on NLM platform.
//
//    Rev 1.0   03 Jan 1996 17:14:54   DCHI
// Initial revision.
//
//************************************************************************

#ifndef SYM_NLM

#if defined(SYM_DOSX) || !defined(SYM_DOS)

#include "storobj.h"
#include "wdencdoc.h"
#include "worddoc.h"
#include "macrodel.h"
#include "macrorep.h"

#include "endutils.h"

#include "wddecsig.h"
#include "wdscan.h"
#include "wdrepair.h"
#include "wd7sigs.h"

#include "vba5scan.h"
#include "offcscan.h"
#include "vba5rep.h"

#include "mcrhitmm.h"

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

#else

#include "storobj.h"
#include "worddoc.h"
#include "macrodel.h"

#include "macrorep.h"
#include "macrscan.h"

#include "mcrscndf.h"
#include "mcrrepdf.h"

#include "navexshr.h"

#include "endutils.h"

WORD FindStreamWithVirus
(
    LPCALLBACKREV1      lpCallBack,         // File op callbacks
    HFILE               hFile,              // Handle to file
    LPOLE_FILE_T        pOLEFile,           // OLE file info
    LPMACRO_TABLE_INFO  pMacroTableInfo,    // Macro table info
    LPMACRO_INFO_T      pMacroInfo,         // Macro info
    LPBYTE          lpbyStreamBATCache,     // Non NULL if cache, 512 bytes
    LPBYTE              lpbyWorkBuffer,     // Work buffer >= 512 bytes
    LPWORD_MACRO_SIG_T  pwmsSig,            // Virus signature
    LPWORD              pwMagicNum          // Magic number
);

WORD WordMacroVirusRepairHelp
(
    LPCALLBACKREV1      lpCallBack,         // File op callbacks
    LPTSTR              lpszFileName,       // Name of infected file
#ifdef __MACINTOSH__
    HFILE               hFile,              // Handle to file to repair
#endif
    LPWORD_MACRO_SIG_T  pwmsSig,            // Virus signature
    LPMACRO_REPAIR_T    pRepairInfo,        // Virus repair info
    LPBYTE              lpbyWorkBuffer      // Work buffer >= 1024 bytes
);

//********************************************************************
//
// Function:
//  WORD FindStreamWithVirus()
//
// Description:
//  Finds the first stream infected with a given virus.
//
// Returns:
//  1   If a stream is found with the virus
//  0   If no stream with the virus was found
//
//********************************************************************

WORD FindStreamWithVirus
(
    LPCALLBACKREV1      lpCallBack,         // File op callbacks
    HFILE               hFile,              // Handle to file
    LPOLE_FILE_T        pOLEFile,           // OLE file info
    LPMACRO_TABLE_INFO  pMacroTableInfo,    // Macro table info
    LPMACRO_INFO_T      pMacroInfo,         // Macro info
    LPBYTE          lpbyStreamBATCache,     // Non NULL if cache, 512 bytes
    LPBYTE              lpbyWorkBuffer,     // Work buffer >= 512 bytes
    LPWORD_MACRO_SIG_T  pwmsSig,            // Virus signature
    LPWORD              pwMagicNum          // Magic number
)
{
    LPBYTE lpbySig;
    WORD j, wSigOffset, wSigLen;

    // Verify that it is a Word file and that it has macros

    while (WordDocOpen(lpCallBack,
                       hFile,
                       pOLEFile,
                       lpbyStreamBATCache,
                       pwMagicNum) != 0)
    {
        if (WordDocContainsMacros(lpCallBack,pOLEFile,lpbyWorkBuffer,
            pMacroTableInfo) == 0)
            continue;

        while (pwmsSig->pszMacroName != NULL)
        {
            if (WordDocGetMacroInfo(lpCallBack,
                pOLEFile,
                pMacroTableInfo,
                pMacroInfo,
                lpbyWorkBuffer,
                (LPSTR)(pwmsSig->pszMacroName)) != WORDDOC_OK)
                break;

            // Macro by that name exists
            //  Now verify all signatures

            lpbySig = pwmsSig->pbySig;
            wSigOffset = AVDEREF_WORD((LPBYTE)lpbySig);
            while (wSigOffset != (WORD)(0xFFFF))
            {
                // Skip pass signature offset

                lpbySig += 2;

                wSigLen = (WORD)(*lpbySig++);

                //  Seek to it and read

                if (OLESeekRead(lpCallBack,pOLEFile,
                    pMacroInfo->dwOffset + (DWORD)wSigOffset,
                    lpbyWorkBuffer,wSigLen) != wSigLen)
                {
                    // Couldn't read that many bytes, so fail

                    break;
                }

                // Compare

                for (j=0;j<wSigLen;j++)
                {
                    if (lpbyWorkBuffer[j] !=
                        (pMacroInfo->byEncrypt ^ (*lpbySig++)))
                        break;
                }

                if (j != wSigLen)
                {
                    // No match

                    break;
                }

                // Next signature offset in this macro

                wSigOffset = AVDEREF_WORD((LPBYTE)lpbySig);
            }

            if (wSigOffset != (WORD)(0xFFFF))
            {
                // Doesn't match this macro definition

                break;
            }

            ++pwmsSig;
        }

        if (pwmsSig->pszMacroName == NULL)
        {
            // Found a match

            return 1;
        }
    }

    // No stream with indicated virus found

    return 0;
}

#endif

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


#if defined(SYM_DOSX) || !defined(SYM_DOS)

// Decryption enhanced function below

//********************************************************************
//
// Function:
//  EXTSTATUS WD7MacroVirusRepair()
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
    LPCALLBACKREV1      lpstCallBack,       // File op callbacks
    LPOLE_FILE          lpstOLEFile,        // OLE for infected stream
    LPTSTR              lpszFileName,       // Infected file's name
    HFILE               hFile,              // Handle to file to repair
    LPWD_VIRUS_SIG_INFO lpstVirusSigInfo,   // Info of virus to repair
    LPBYTE              lpbyWorkBuffer      // Work buffer >= 1400 bytes
)
{
    EXTSTATUS           extStatus = EXTSTATUS_OK;
    MACRO_TABLE_INFO_T  stMacroTableInfo;
    WORD                wMagicNum;
    LPBYTE              lpbyStreamBATCache;
    LPWD_SCAN           lpstScan;
    WDENCKEY_T          stKey;
    WORD                wVID;
    BOOL                bHasAutoText;

#if !defined(ALLOC_MACR_SIG_HIT_MEM)
    BYTE abyNameSigInterMacroHit[WD7_NAME_SIG_INTER_MACRO_HIT_ARRAY_SIZE];
    BYTE abyNameSigIntraMacroHit[WD7_NAME_SIG_INTRA_MACRO_HIT_ARRAY_SIZE];
    BYTE abyNamedCRCSigHit[WD7_NAMED_CRC_SIG_HIT_ARRAY_SIZE];
    BYTE abyMacroSigHit[WD7_MACRO_SIG_HIT_ARRAY_SIZE];
    BYTE abyCRCHit[WD7_CRC_HIT_ARRAY_SIZE];
#else
    LPBYTE              lpbyHitMem;
#endif

    lpbyStreamBATCache = NULL;

    lpstScan = (LPWD_SCAN)lpbyWorkBuffer;
    lpbyWorkBuffer += sizeof(WD_SCAN_T);

#if !defined(ALLOC_MACR_SIG_HIT_MEM)
    lpstScan->lpabyNameSigInterMacroHit = abyNameSigInterMacroHit;
    lpstScan->lpabyNameSigIntraMacroHit = abyNameSigIntraMacroHit;
    lpstScan->lpabyNamedCRCSigHit = abyNamedCRCSigHit;
    lpstScan->lpabyMacroSigHit = abyMacroSigHit;
    lpstScan->lpabyCRCHit = abyCRCHit;
#else
    if (AllocMacroSigHitMem(lpstCallBack,
                            &lpbyHitMem,
                            HIT_MEM_WD7) == FALSE)
        return(EXTSTATUS_MEM_ERROR);

    AssignMacroSigHitMem(lpbyHitMem,lpstScan);
#endif

    // Find the first WordDocument stream containing an infection
    //  with the given VID

    wVID = lpstVirusSigInfo->wID;
    lpstVirusSigInfo = NULL;
    wMagicNum = 0;
    while (WordDocOpen(lpstCallBack,
                       hFile,
                       lpstOLEFile,
                       lpbyStreamBATCache,
                       &wMagicNum) != 0)
    {
        if (WordDocFindFirstKey(lpstCallBack,
                                lpstOLEFile,
                                lpbyWorkBuffer,
                                &stKey,
                                &gstRevKeyLocker) != WORDDOC_OK)
        {
            // Failure to find key or determining whether it
            //  was encrypted

            continue;
        }

        while (1)
        {
            if (WordDocContainsMacros(lpstCallBack,
                                      lpstOLEFile,
                                      &stKey,
                                      lpbyWorkBuffer,
                                      &stMacroTableInfo) == 0)
                break;

            // Need to do this here because lpstScan is the same as
            //  lpbyWorkBuffer

            lpstScan->lpstCallBack = lpstCallBack;
            lpstScan->lpstOLEStream = lpstOLEFile;
            lpstScan->wScanType = WD_SCAN_TYPE_WD7;
            lpstScan->uScan.stWD7.lpstMacroTableInfo = &stMacroTableInfo;
            lpstScan->uScan.stWD7.lpstKey = &stKey;

            if (WDScanDoc(&gstWD7SigSet,
                          lpstScan,
                          &lpstVirusSigInfo) == WD_STATUS_VIRUS_FOUND)
            {
                if (lpstVirusSigInfo->wID == wVID)
                    break;
            }

            lpstVirusSigInfo = NULL;

            if (stKey.bEncrypted == FALSE)
                break;

            if (WordDocFindNextKey(lpstCallBack,
                                   lpstOLEFile,
                                   lpbyWorkBuffer,
                                   &stKey,
                                   &gstRevKeyLocker) != WORDDOC_OK)
                break;
        }

        if (lpstVirusSigInfo != NULL)
            break;
    }

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

        if (extStatus == EXTSTATUS_OK)
        {
            if (WDRepairDoc(&gstWD7SigSet,
                            lpstScan,
                            lpstVirusSigInfo) != WD_STATUS_OK)
                extStatus = EXTSTATUS_NO_REPAIR;
        }

        if (extStatus != EXTSTATUS_NO_REPAIR)
        {
            // Repair successful

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

            if (WordDocHasActiveMacros(lpstCallBack,
                                       lpstOLEFile,
                                       &stKey,
                                       &stMacroTableInfo,
                                       lpbyWorkBuffer) == 0 &&
                EndsInDOTOrNORMAL(lpszFileName) == 0 &&
                WordDocHasNonMacroTemplateInfo(&stMacroTableInfo) == 0 &&
                WordDocHasAutoText(lpstCallBack,
                                   lpstOLEFile,
                                   &stKey,
                                   &bHasAutoText) == TRUE &&
                bHasAutoText == FALSE)
            {
                if (WordDocChangeToDocument(lpstCallBack,
                                            lpstOLEFile,
                                            &stKey,
                                            lpbyWorkBuffer) != WORDDOC_OK)
                    extStatus = EXTSTATUS_NO_REPAIR;
#ifdef __MACINTOSH__
                else
                if (!WordDocChangeCreator( hFile ))
                    extStatus = EXTSTATUS_NO_REPAIR;
#endif                
            }
        }
        FreeMacroSigHitMem(lpstCallBack,lpbyHitMem);
        return(extStatus);
    }

    FreeMacroSigHitMem(lpstCallBack,lpbyHitMem);
    return(EXTSTATUS_NO_REPAIR);
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
    LPCALLBACKREV1      lpstCallBack,       // File op callbacks
    LPOLE_FILE          lpstOLEFile,        // OLE for infected stream
    LPTSTR              lpszFileName,
    HFILE               hFile,              // Handle to OLE file
    LPMSOFFICE_SCAN     lpstOffcScan
)
{
    BOOL                bHasAutoText;

    // Open the WordDocument stream

    if (OLEOpenStreamEntryNum(lpstCallBack,
                              hFile,
                              lpstOLEFile,
                              lpstOffcScan->u.stVBA5.u.stWD8.
                                  wWordDocumentEntry,
                              NULL) != OLE_OK)
    {
        return(FALSE);
    }

    // Determine whether the document has AutoText

    if (WD8HasAutoText(lpstCallBack,
                       lpstOLEFile,
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
        return WD8ChangeToDocument(lpstCallBack,
                                   lpstOLEFile);
    }

    return(TRUE);
}



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
    UINT                uAttr;
#endif
    LPOLE_FILE          lpstOLEFile;
    MSOFFICE_SCAN_T     stMSOfficeScan;
    LPWD_VIRUS_SIG_INFO lpstVirusSigInfo;

    lpstOLEFile = (LPOLE_FILE)lpbyWorkBuffer;
    lpbyWorkBuffer += sizeof(OLE_FILE_T);

#ifndef __MACINTOSH__
    if (RepairFileOpen(lpstCallBack,lpszFileName,&hFile,&uAttr) != 0)
        return(EXTSTATUS_NO_REPAIR);
#endif

    lpstOLEFile->hFile = hFile;
    if (MicrosoftOfficeScan(lpstCallBack,
                            lpstOLEFile,
                            lpbyWorkBuffer,
                            &stMSOfficeScan,
                            &lpstVirusSigInfo) == EXTSTATUS_VIRUS_FOUND &&
        stMSOfficeScan.wType != OLE_SCAN_XL5 &&
        lpstVirusSigInfo->wID == wVID)
    {
        switch (stMSOfficeScan.wType)
        {
            case OLE_SCAN_WD7:
                extStatus = WD7MacroVirusRepair(lpstCallBack,
                                                lpstOLEFile,
                                                lpszFileName,
                                                hFile,
                                                lpstVirusSigInfo,
                                                lpbyWorkBuffer);
                break;

            case OLE_SCAN_WD8:
                if (WDIsFullSetRepair(lpstVirusSigInfo) == TRUE)
                {
                    // Do full set repair

                    if (WD8DoFullSetRepair(lpstCallBack,
                                           lpstOLEFile,
                                           &stMSOfficeScan.u.stVBA5,
                                           lpbyWorkBuffer) != WD_STATUS_OK)
                        extStatus = EXTSTATUS_NO_REPAIR;
                }

                if (extStatus == EXTSTATUS_OK)
                {
                    extStatus = WD8MacroVirusRepair(lpstCallBack,
                                                    lpstOLEFile,
                                                    &stMSOfficeScan.u.stVBA5,
                                                    lpstVirusSigInfo,
                                                    lpbyWorkBuffer);
                }

                if (extStatus == EXTSTATUS_OK)
                {
                    if (WD8TemplateToDocumentStep(lpstCallBack,
                                                  lpstOLEFile,
                                                  lpszFileName,
                                                  hFile,
                                                  &stMSOfficeScan) ==
                        FALSE)
                    {
                        extStatus = EXTSTATUS_NO_REPAIR;
                    }
                }

                break;

            case OLE_SCAN_XL97:
                if (WDIsFullSetRepair(lpstVirusSigInfo) == TRUE)
                {
                    // Do full set repair

                    if (XL97DoFullSetRepair(lpstCallBack,
                                            lpstOLEFile,
                                            &stMSOfficeScan.u.stVBA5,
                                            lpbyWorkBuffer) != WD_STATUS_OK)
                        extStatus = EXTSTATUS_NO_REPAIR;
                }

                if (extStatus == EXTSTATUS_OK)
                {
                    extStatus = XL97MacroVirusRepair(lpstCallBack,
                                                     lpstOLEFile,
                                                     &stMSOfficeScan.u.stVBA5,
                                                     lpstVirusSigInfo,
                                                     lpbyWorkBuffer);
                }

                break;

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

#ifndef __MACINTOSH__
    RepairFileClose(lpstCallBack,lpszFileName,hFile,uAttr);
#endif

    return(extStatus);
}

// Decryption enhanced function above

#else

// Plain DOS below

//********************************************************************
//
// Function:
//	WORD WordMacroVirusRepairHelp()
//
// Description:
//	Finds the first stream infected with the indicated virus
//	and then attempts a repair on the stream.
//
// Returns:
//	0			On failure
//	VID_???		On success, the VID of the virus repaired is returned.
//
//********************************************************************

WORD WordMacroVirusRepairHelp
(
	LPCALLBACKREV1 		lpCallBack,			// File op callbacks
    LPTSTR              lpszFileName,       // Name of infected file
#ifdef __MACINTOSH__
    HFILE               hFile,              // Handle to file to repair
#endif
    LPWORD_MACRO_SIG_T	pwmsSig,			// Virus signature
	LPMACRO_REPAIR_T	pRepairInfo,		// Virus repair info
    LPBYTE              lpbyWorkBuffer      // Work buffer >= 1024 bytes
)
{
#ifndef __MACINTOSH__
    HFILE hFile;
    UINT uAttr;
#endif
	OLE_FILE_T OLEFile;
	MACRO_TABLE_INFO_T macroTableInfo;
	MACRO_INFO_T macroInfo;
	WORD wMagicNum;
	BYTE LPF * LPF *ppszMacroNames;
    LPBYTE  lpbyStreamBATCache;

    lpbyStreamBATCache = lpbyWorkBuffer;
    lpbyWorkBuffer += 512;

#ifndef __MACINTOSH__
    if (RepairFileOpen(lpCallBack,lpszFileName,&hFile,&uAttr) != 0)
		return 0;
#endif

	// Verify that it is a Word file and that it has macros

	// Find the first document containing the virus

	wMagicNum = 0;
	if (FindStreamWithVirus(lpCallBack,
    	hFile,
		&OLEFile,
		&macroTableInfo,
		&macroInfo,
        lpbyStreamBATCache,
		lpbyWorkBuffer,
    	pwmsSig,
		&wMagicNum) != 0)
	{
        WORD wReturnValue = pRepairInfo->wVID;

		ppszMacroNames = pRepairInfo->ppszMacroNames;

		// Delete all the macros of the virus

		for (;*ppszMacroNames != NULL;++ppszMacroNames)
		{
			if (WordDocGetMacroInfo(lpCallBack,
				&OLEFile,
				&macroTableInfo,
				&macroInfo,
				lpbyWorkBuffer,
				(LPSTR)(*ppszMacroNames)) != WORDDOC_OK)
				continue;

			WordDocDeleteMacro(lpCallBack,
				&OLEFile,
				&macroTableInfo,
				&macroInfo,
				lpbyWorkBuffer);
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

        if (WordDocHasActiveMacros(lpCallBack,
            &OLEFile,
            &macroTableInfo,
            lpbyWorkBuffer) == 0 &&
            EndsInDOTOrNORMAL(lpszFileName) == 0 &&
            WordDocHasNonMacroTemplateInfo(&macroTableInfo) == 0)
        {
            if (WordDocChangeToDocument(lpCallBack,
                &OLEFile,lpbyWorkBuffer) != WORDDOC_OK)
                wReturnValue = 0;
#ifdef __MACINTOSH__
            else if (!WordDocChangeCreator( hFile ))
                	wReturnValue = 0;
      	    
#endif                
                
        }

#ifndef __MACINTOSH__
        RepairFileClose(lpCallBack,lpszFileName,hFile,uAttr);
#endif

        return wReturnValue;
	}

#ifndef __MACINTOSH__
    RepairFileClose(lpCallBack,lpszFileName,hFile,uAttr);
#endif

	return 0;
}


//********************************************************************
//
// Function:
//	WORD WordMacroVirusRepair()
//
// Description:
//	Repairs the first WordDocument stream found that is infected
//	with the virus identified by VID.
//
// Returns:
//	EXTSTATUS_OK			If the repair was successful
//	EXTSTATUS_NO_REPAIR		If the repair was unsuccessful
//
//********************************************************************

EXTSTATUS WordMacroVirusRepair
(
	LPCALLBACKREV1 	lpCallBack,				// File op callbacks
	WORD 			wVID,					// ID of virus to repair
    LPTSTR          lpszFileName,           // Infected file's name
#ifdef __MACINTOSH__
    HFILE           hFile,                  // Handle to file to repair
#endif
    LPBYTE          lpbyWorkBuffer          // Work buffer >= 1024 bytes
)
{
	int repairIndex, sigIndex;

	for (sigIndex=0;sigIndex<gwNumMacroScanDefs;sigIndex++)
	{
		if (wVID == gpWordVirusSigs[sigIndex].wVID)
			break;
	}

	if (sigIndex == gwNumMacroScanDefs)
	{
		// Invalid ID passed in?

		return EXTSTATUS_NO_REPAIR;
	}

	for (repairIndex=0;repairIndex<gwNumMacroRepairDefs;repairIndex++)
	{
		if (wVID == gMacroVirusRepairInfo[repairIndex].wVID)
			break;
	}

	if (repairIndex == gwNumMacroRepairDefs)
		return EXTSTATUS_NO_REPAIR;

	if (WordMacroVirusRepairHelp(lpCallBack,
		lpszFileName,
#ifdef __MACINTOSH__
        hFile,
#endif
        gpWordVirusSigs[sigIndex].pwmsSig,
		gMacroVirusRepairInfo+repairIndex,
		lpbyWorkBuffer) == wVID)
		return EXTSTATUS_OK;

	return EXTSTATUS_NO_REPAIR;
}

// Plain DOS above

#endif

#endif  // #ifndef SYM_NLM



