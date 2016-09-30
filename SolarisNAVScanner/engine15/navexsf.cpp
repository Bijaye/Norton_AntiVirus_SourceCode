//************************************************************************
//
// $Header:   S:/ENGINE15/VCS/NAVEXSF.CPv   1.19   20 Nov 1998 17:02:06   rpulint  $
//
// Description:
//      Contains NAVEX EXTScanFile code.
//
//************************************************************************
// $Log:   S:/ENGINE15/VCS/NAVEXSF.CPv  $
// 
//    Rev 1.19   20 Nov 1998 17:02:06   rpulint
// WATCOM Changes: moved extern declaration to theglobal section from
// within functions
// 
//    Rev 1.18   06 Nov 1998 17:15:54   rpulint
// Added Changes for OS/2
// 
//    Rev 1.17   14 Oct 1998 12:00:48   MKEATIN
// Carey's RelinquishControl calls for NLM.
// 
//    Rev 1.16   11 Aug 1998 17:33:30   CNACHEN
// updated to support ibm string scanning at level 2 heuristics.
// 
//    Rev 1.15   12 Jun 1998 19:59:02   CNACHEN
// Added "infestmode" NAVEX15.INF option to disable infestation mode:
// 
// usage:
// 
// [NAVW32]
// infestmode=0                    ; off
// infestmode=1                    ; on (default)
// 
// 
//    Rev 1.14   12 Jun 1998 13:57:36   CNACHEN
// Added infestation check and code.
// 
//    Rev 1.13   05 Jun 1998 15:16:56   CNACHEN
// Modified to perform IBM string scan.
// 
//    Rev 1.12   26 Jan 1998 16:54:34   DDREW
// For NLM we don't malloc for each file any more
// 
//    Rev 1.11   07 Nov 1997 13:48:58   CNACHEN
// Added check for NLM for heur level.
// 
//    Rev 1.10   02 Sep 1997 14:44:28   CNACHEN
// Fixed for NAVEX.NLM
// 
//    Rev 1.9   23 Aug 1997 11:18:32   STRILLI
// Added checks for USE_HEUR so that we use heuristics on the right platforms.
// 
//    Rev 1.8   18 Jul 1997 16:09:48   CNACHEN
// Added for NAVENG.
// 
//    Rev 1.7   09 Jul 1997 16:59:14   CNACHEN
// Fixed extension checking bug.  If we add ??? to our scanning extenion list
// in NAVEX.INI, we failed to scan files like FOO.XX, where the extension is
// less than 3 characters.
// 
//    Rev 1.6   01 Jul 1997 14:37:30   CNACHEN
// Fixed compiler internal error because of re-extern of global.
// 
//    Rev 1.5   01 Jul 1997 14:07:04   CNACHEN
// Fixed double extern bug.
// 
//    Rev 1.4   01 Jul 1997 13:50:22   CNACHEN
// Added support for heavy hitters.
// 
//    Rev 1.3   11 Jun 1997 17:34:58   CNACHEN
// Added extension list support...
// 
//    Rev 1.2   04 Jun 1997 12:02:52   CNACHEN
// Added bloodhound support.
// 
//    Rev 1.1   15 May 1997 18:32:10   jsulton
// 
//    Rev 1.0   15 May 1997 15:00:52   jsulton
// Initial revision.
//************************************************************************

#include "stdio.h"
#include "stdlib.h"
#include "ctype.h"

#include "platform.h"

#ifdef WIN16        // VIRSCAN QUAKE 2
#include "cts4.h"
#else
//#include "ctsn.h"	// NEW_UNIX
#include "n30type.h"	// NEW_UNIX
#include "callback.h"
#endif

#include "navex.h"

#ifdef SYM_DOSX
#include "stdlib.h"
#endif

#ifdef SYM_OS2
#include "ctype.h"
#endif

// Declare shared routines

#include "navexshr.h"

#ifdef SYM_DOSX
#undef SYM_DOS
#endif

#if defined(SYM_DOS)

WORD wDS, wES;                      // save loader's DS,ES

#endif

#include "heurapi.h"
#include "navheur.h"

#ifdef SYM_OS2
    extern NAVEX_HEUR_T     gstHeur;
#endif	

#include "avendian.h"               // IBM string scanner
#include "strscan.h"

#ifdef __WATCOMC__
    extern SEARCH_INFO_T    gstIBMFile;     // IBM string scanning information
    extern BOOL             gbDisableInfestationMode;
#endif

// Prototypes for local functions


#if defined(NAVEX_QA)                  // QA ScanFile function Prototypes

EXTSTATUS EXTQAScanFilePre (
                            LPCALLBACKREV1   lpCallBack,
                            LPTSTR           lpszFileName,
                            HFILE            hFile,
                            LPBYTE           lpbyInfectionBuffer,
                            LPBYTE           lpbyWorkBuffer,
                            WORD             wVersionNumber,
                            LPWORD           lpwVID
                           );


EXTSTATUS EXTQAScanFileTest (
                             LPCALLBACKREV1 lpCallBack,
                             LPTSTR         lpszFileName,
                             HFILE          hFile,
                             LPBYTE         lpbyInfectionBuffer,
                             LPBYTE         lpbyWorkBuffer,
                             WORD           wVersionNumber,
                             LPWORD         lpwVID
                            );

EXTSTATUS EXTQAScanFilePost (
                             LPCALLBACKREV1   lpCallBack,
                             LPTSTR           lpszFileName,
                             HFILE            hFile,
                             LPBYTE           lpbyInfectionBuffer,
                             LPBYTE           lpbyWorkBuffer,
                             WORD             wVersionNumber,
                             LPWORD           lpwVID,
                             WORD             wResult
                            );

#endif                                  // NAVEX_QA

// EXTExcludeFile arguments:
//
// lpCallBack           : pointer to callback structure for FileOpen, etc.
// lpszFileName         : file name of infected host.
// hFile                : file handle into current file to scan
// lpbyInfectionBuffer  : 4K fstart buffers containing TOF, ENTRY, etc.
// lpwExclude			: TRUE to exclude, FALSE otherwise
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK                  if OK
//                        EXTSTATUS_DISK_READ_ERROR     if disk read error
//                        EXTSTATUS_MEM_ERROR           if memory error
//
//
//
//EXTSTATUS EXTExcludeFile(LPCALLBACKREV1 lpCallBack,
//                         LPTSTR         lpszFileName,
//                         HFILE          hFile,
//                         LPBYTE         lpbyInfectionBuffer,
//                         LPWORD         lpwExclude)
//{
//
//    // no exclusions
//
//
//    return(EXTSTATUS_OK);
//}

// EXTScanFile arguments:
//
// lpCallBack           : pointer to callback structure for FileOpen, etc.
// lpszFileName         : pointer to the name of the file
// hFile                : file handle into current file to scan
// lpbyInfectionBuffer  : 4K fstart buffers containing TOF, ENTRY, etc.
//                         +0   256 fstart bytes of target program.
//                         +256 64 bytes from TOF of target program.
//                         +512 256 unused
//                         +768 2 Number of external defs called.
//                         +770 2 ID of first external called.
//                         +772 ? ID of additional externals called.
// lpbyWorkBuffer       : 2-4K buffer for temporary operations (STACK OK also)
// wVersionNumber       : Engine version number so detections are not performed
//                        by an external DLL if the engine has been updated
//                        to provide the detection itself.
// lpwVID               : virus ID # of the virus if found
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK                  if no virus found
//                        EXTSTATUS_FILE_ERROR          if file error occured
//                        EXTSTATUS_VIRUS_FOUND         if virus was found
//                        EXTSTATUS_MEM_ERROR           if memory alloc. error

#if defined(SYM_WIN32) || defined(SYM_NTK) || defined(SYM_VXD)
#include "pshpack1.h"
#else
#pragma PACK1
#endif

typedef struct tagNAVEX_CALL
{
    WORD    wCount;
    WORD    wID[256];
} NAVEX_CALL_T, FAR *LPNAVEX_CALL;

#if defined(SYM_WIN32) || defined(SYM_NTK) || defined(SYM_VXD)
#include "poppack.h"
#else
#pragma PACK
#endif

#ifdef BUILD_HEAVY_HITTER

        extern DWORD        gdwLastIterations;

#endif

BOOL NeedsToBeScanned
(
    LPTSTR  lpszFileName        // Name of infected file
);

EXTSTATUS FAR WINAPI NLOADDS EXTScanFile(LPCALLBACKREV1 lpCallBack,
                                         LPTSTR          lpszFileName,
                                         HFILE          hFile,
                                         LPBYTE         lpbyInfectionBuffer,
                                         LPBYTE         lpbyWorkBuffer,
                                         WORD           wVersionNumber,
                                         LPWORD         lpwVID)
{

#if defined(SYM_DOS)

    _asm
        {
        mov cs:[wDS], ds
        mov cs:[wES], es
        mov ax, cs
        mov ds, ax
        mov es, ax
        }

#endif
/*
    WORD                    wResult = EXTSTATUS_OK;
    LPNAVEX_CALL            lpNAVEXCall =
                                (LPNAVEX_CALL)(lpbyInfectionBuffer + 768);
*/
#ifndef SYM_OS2
    extern NAVEX_HEUR_T     gstHeur;
#endif	
//    extern NAVEX_HEUR_T     gstHeur;

#ifndef __WATCOMC__
    extern SEARCH_INFO_T    gstIBMFile;     // IBM string scanning information
#endif
    LPBYTE                  lpbyScanBuffer; // decrypted buffer

#ifdef SYM_NLM
    char                    szOptionValue[128];
    BOOL                    bImplemented;
#endif

    static DWORD            dwInfectionCount = 0;
    static DWORD            dwCleanCount = 0;
    static BOOL             bInfestationDetected = FALSE;
    int                     nHeurLevel;
#ifndef __WATCOMC__
    extern BOOL             gbDisableInfestationMode;
#endif

#ifdef SYM_NLM
        // relinquish control on NLM
        ThreadSwitch();
#endif


#if defined(NAVEX_QA)                  // QA ScanFile Entry Point function

    EXTQAScanFilePre (lpCallBack,
                      lpszFileName,
                      hFile,
                      lpbyInfectionBuffer,
                      lpbyWorkBuffer,
                      wVersionNumber,
                      lpwVID);

#endif                                  // NAVEX_QA

    (void)lpszFileName;
    (void)wVersionNumber;

    // use heuristics!

#if defined(USE_HEUR)

    if (gstHeur.bEnabled == TRUE && NeedsToBeScanned(lpszFileName))
    {
        PAMLHANDLE                      hLocal;
        BOOL                            bVirusFound;
        WORD                            wVirusID;
        PAMSTATUS                       pamStatus;

        // scan that file

        bVirusFound = FALSE;            // nothing yet!

#ifdef SYM_NLM

        // on the NLM we need to check for status change of heur level before
        // every scan...

        // this only works if we're single threaded!

        lpCallBack->GetConfigInfo("heur",
                                  (LPVOID)szOptionValue,
                                  sizeof(szOptionValue)-1,
                                  &bImplemented,
                                  NULL);

        if (bImplemented == TRUE)
        {
            if (szOptionValue[0] >= '0' && szOptionValue[0] <= '3')
                gstHeur.nHeurLevel = szOptionValue[0] - '0';
            else
                gstHeur.nHeurLevel = 2;
        }
#endif  // #ifdef SYM_NLM

        if (gstHeur.nHeurLevel != 0)
        {
#ifndef SYM_NLM
            if (PAMLocalInit(gstHeur.hGHeur,&hLocal) != PAMSTATUS_OK)
            {
                return(EXTSTATUS_MEM_ERROR);
            }
#else
            // for NLM, just use the previously initialized local info

            hLocal = gstHeur.hLHeur;
#endif

            memset(hLocal->byVirusBuffers,0,MAX_ASSUMED_VIRUS_SIZE);

            nHeurLevel = gstHeur.nHeurLevel;

            if (gbDisableInfestationMode == FALSE &&
                bInfestationDetected == TRUE)
                nHeurLevel = 3;

            pamStatus = PAMScanFileH(hLocal,
                                     hFile,
                                     nHeurLevel,
                                     &bVirusFound,
                                     &wVirusID);

            //////////////////////////////////////////////////////////////////
            // now check for infestation so we know to raise the BH level!
            //////////////////////////////////////////////////////////////////

            if (pamStatus == PAMSTATUS_OK)
            {
                if (bVirusFound == TRUE)
                {
                    dwCleanCount = 0;
                    dwInfectionCount++;
                    if (dwInfectionCount >= 4)
                    {
                        bInfestationDetected = TRUE;
                    }
                }
                else
                {
                    dwCleanCount++;
                    if (dwCleanCount >= 25)
                    {
                        dwInfectionCount = 0;
                        bInfestationDetected = FALSE;
                    }
                }
            }

            //////////////////////////////////////////////////////////////////
            // infestation check done!
            //////////////////////////////////////////////////////////////////

            // SCAN with IBM STRING SCANNER!!!!

            if (nHeurLevel >= 2)        // we're going to use IBM-lite mode
                                        // for level 2, IBM-regular for level 3
            {
                WORD                    wEncSize;

                if (hLocal->stBM.
                     m_byBehaviorArray[BF6_FOUND_SELF_MOD_CODE] == TRUE)
                {
                    lpbyScanBuffer = hLocal->byVirusBuffers;
                }
                else
                {
                    lpbyScanBuffer = NULL;
                }

                // change below from MAX_ASSUMED_VIRUS_SIZE to actual size
                // used by bloodhound. this will lower the buffer size to scan
                // under heuristic level 2.

                if (nHeurLevel == 2)
                    wEncSize = gstHeur.hGHeur->config_info[nHeurLevel-1].wAssumedVirusSize;
                else
                    wEncSize = MAX_ASSUMED_VIRUS_SIZE;

                if (IBMStringScanFile(lpCallBack,
                                      &gstIBMFile,
                                      hFile,
                                      lpbyScanBuffer,
                                      wEncSize,
                                      nHeurLevel == 2 /*cheezy=true*/) ==
                     STRING_SCAN_FOUND_STRING)
                {
                    wVirusID = IBM_STRING_FILE_VID;
                    pamStatus = PAMSTATUS_OK;
                    bVirusFound = TRUE;
                }
            }

            // IBM string scanner is done!

            // kill the instance

#ifndef SYM_NLM
            PAMLocalClose(hLocal);
#endif

#ifdef SYM_NLM
        // relinquish control on NLM
        ThreadSwitch();
#endif

            if (pamStatus == PAMSTATUS_OK && bVirusFound == TRUE)
            {
                *lpwVID = wVirusID;

                return(EXTSTATUS_VIRUS_FOUND);
            }
        }

        // had to be an error or clean file, return OK anyway
    }

#endif // #if defined(USE_HEUR)

#ifdef SYM_NLM
        // relinquish control on NLM
        ThreadSwitch();
#endif

    return(EXTSTATUS_OK);
}


//********************************************************************
//
// Function:
//  WORD NeedsToBeScanned()
//
// Description:
//  Determines whether the filename ends in .COM or .EXE for the time being.
//
// Returns:
//  FALSE       If the file should NOT be scanned
//  TRUE        If the file should be scanned
//
//********************************************************************

#if defined(USE_HEUR)

BOOL NeedsToBeScanned
(
    LPTSTR  lpszFileName        // Name of infected file
)
{
    LPTSTR                      lptstrDOT;
    LPTSTR                      lptstrCur;
    int                         i, j;
    LPEXT_NODE                  lpstExt;
    extern NAVEX_HEUR_T         gstHeur;

    if (lpszFileName == NULL || *lpszFileName == '\0')
        return (FALSE);

    lptstrDOT = NULL;
    lptstrCur = lpszFileName;

    // find last . in filename

    while (*lptstrCur != 0)
    {
        lptstrCur = AnsiNext(lptstrCur);
        if (*lptstrCur == '.')
        {
            lptstrDOT = lptstrCur;
        }
    }

    if (lptstrDOT == NULL)
        return(FALSE);

    // advance over the period

    lptstrDOT = AnsiNext(lptstrDOT);

    // now compare the extensions

    lpstExt = gstHeur.lpstExtList;

    while (lpstExt != NULL)
    {
        for (i=0;lpstExt->szExt[i];i++)
        {
            if (!lptstrDOT[i])
            {
                for (j=i;lpstExt->szExt[j];j++)
                    if (lpstExt->szExt[j] != '?')
                        break;

                if (lpstExt->szExt[j] == 0)
                    return(TRUE);

                break;
            }

            if (lpstExt->szExt[i] != toupper(lptstrDOT[i]) &&
                lpstExt->szExt[i] != '?')
                break;
        }

        if (lpstExt->szExt[i] == 0)
        {
            return(TRUE);
        }

        lpstExt = lpstExt->lpstNext;
    }



    return (FALSE);
}

#endif // #if defined(USE_HEUR)
