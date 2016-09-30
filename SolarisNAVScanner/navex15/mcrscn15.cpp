//************************************************************************
//
// $Header:   S:/NAVEX/VCS/MCRSCN15.CPv   1.11   09 Dec 1998 17:44:58   DCHI  $
//
// Description:
//      Contains WordDocument macro scan function code.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/MCRSCN15.CPv  $
// 
//    Rev 1.11   09 Dec 1998 17:44:58   DCHI
// Added #include "gdefines.h"
// 
//    Rev 1.10   08 Dec 1998 12:57:04   DCHI
// Changes for MSX.
// 
//    Rev 1.9   04 Aug 1998 13:52:30   DCHI
// Changes for Excel heuristics.
// 
//    Rev 1.8   10 Jun 1998 13:20:46   DCHI
// Added W7H2
// 
//    Rev 1.7   11 May 1998 17:57:28   DCHI
// Changes for Access 2.0 scanning and Access datafile conversion.
// 
//    Rev 1.6   15 Apr 1998 17:23:46   DCHI
// Modifications for new XL95 and XL5 engines.
// 
//    Rev 1.5   29 Jan 1998 19:04:28   DCHI
// Added support for Excel formula virus scan and repair.
// 
//    Rev 1.4   05 Nov 1997 12:03:34   DCHI
// Added macro heuristics enablement checking for SYM_DOSX and SYM_WIN.
// 
//    Rev 1.3   07 Aug 1997 18:24:20   DCHI
// Implemented gnMacroHeuristicLevel.
// 
//    Rev 1.2   21 Jul 1997 14:22:46   DCHI
// Added additional condition to check for NAVEX15 before conditionally
// disabling macro heuristics under WIN16 and DOSX.
// 
//    Rev 1.1   16 Jul 1997 17:30:54   DCHI
// Added check for version >= 3 for enabling heuristics on WIN and DX.
// 
//    Rev 1.0   09 Jul 1997 16:20:30   DCHI
// Initial revision.
// 
//************************************************************************

#include "gdefines.h"

#include "macrscan.h"

#include "wd7api.h"
#include "o97api.h"
#include "xl5api.h"
#include "acsapi.h"
#include "wdscan15.h"
#include "offcsc15.h"
#include "o97scn15.h"
#include "mcrsgutl.h"
#include "mcrheusc.h"
#include "macrovid.h"
#include "nvx15inf.h"

#ifdef MSX
#include "msx.h"
#endif // #ifdef MSX

// Assume macro heuristics are enabled

int gnMacroHeuristicLevel = 1;

//********************************************************************
//
// Function:
//	EXTSTATUS EXTScanFileMacro()
//
// Description:
//	Scans a file for the presence of Word Macro viruses.
//
//  The function first creates the root structure for the OLE file,
//  allocates memory for opening one stream, and allocates memory
//  for the signature hit arrays.
//
//  The function then performs a scan of the file by calling
//  MicrosoftOfficeScan().
//
//  Lastly, the function frees the hit array memory, the stream
//  structure, and the root structure.
//
// Returns:
//	EXTSTATUS_OK			If no virus was found
//	EXTSTATUS_VIRUS_FOUND   If a virus was found
//
//	If a virus was found, the function sets *lpwVID to the VID
//	of the virus.
//
//********************************************************************

EXTSTATUS EXTScanFileMacro
(
    LPCALLBACKREV1      lpstCallBack,       // File op callbacks
    HFILE               hFile,              // Handle to file to scan
    LPBYTE              lpbyWorkBuffer,     // Work buffer >= 2048 bytes
    LPWORD              lpwVID              // Virus ID storage on hit
)
{
    MSOFFICE_SCAN_T     stMSOfficeScan;
    LPWD_VIRUS_SIG_INFO lpstVirusSigInfo;
    EXTSTATUS           extStatus;

#ifdef MACROHEU

    BOOL                bUseHeuristics = TRUE;

#if !defined(NAVEX15) && (defined(SYM_WIN16) || defined(SYM_DOSX))

    // Check the version number passed in as the first byte
    //  of the work buffer

    if (lpbyWorkBuffer[0] < 3)
        bUseHeuristics = FALSE;

#if defined(SYM_DOSX) || defined(SYM_WIN16)
    else
    {
        // See if we have loaded up heuristic's enabled flag yet

        if (gbCheckedMacroHeurEnableState == FALSE)
            GetMacroHeurEnableState(lpstCallBack);
    }
#endif // #if defined(SYM_DOSX) || defined(SYM_WIN16)

#endif

#endif // #ifdef MACROHEU

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

        return(EXTSTATUS_MEM_ERROR);
    }


    /////////////////////////////////////////////////////////////
    // Scan the file
    /////////////////////////////////////////////////////////////

    stMSOfficeScan.dwLongScanCount = 0;
    extStatus = MicrosoftOfficeScan(&stMSOfficeScan,
                                    lpbyWorkBuffer,
                                    &lpstVirusSigInfo);

    // Free scan and hit memory

    WDFreeScanStruct(lpstCallBack,stMSOfficeScan.lpstScan);
    FreeMacroSigHitMem(lpstCallBack,&stMSOfficeScan);

    if (extStatus == EXTSTATUS_VIRUS_FOUND)
    {
        // Virus found

        if (lpstVirusSigInfo->wID == VID_MACRO_HEURISTICS ||
            lpstVirusSigInfo->wID == VID_W7H2 ||
            lpstVirusSigInfo->wID == VID_XH)
        {
//////////////////////////////////////////////////////////////////////
#ifdef MACROHEU
            if (bUseHeuristics == TRUE && gnMacroHeuristicLevel == 1)
                extStatus = MSOfficeHeuristicScan(&stMSOfficeScan,
                                                  &lpstVirusSigInfo->wID);
            else
                extStatus = EXTSTATUS_OK;

            if (extStatus == EXTSTATUS_VIRUS_FOUND)
                *lpwVID = lpstVirusSigInfo->wID;
            else
#endif // #ifdef MACROHEU
//////////////////////////////////////////////////////////////////////
            if (stMSOfficeScan.bMVPApproved == FALSE)
            {
                *lpwVID = gstMVPVirusSigInfo.wID;
                extStatus = EXTSTATUS_VIRUS_FOUND;
            }
            else
            {
                // Ignore

                extStatus = EXTSTATUS_OK;
            }
        }
        else
        {
            *lpwVID = lpstVirusSigInfo->wID;
        }
    }


    /////////////////////////////////////////////////////////////
    // Free the memory
    /////////////////////////////////////////////////////////////

    SSFreeStreamStruct(stMSOfficeScan.lpstStream);
    SSDestroyRoot(stMSOfficeScan.lpstRoot);

#ifdef MSX
    if (extStatus == EXTSTATUS_OK)
    {
        if (stMSOfficeScan.dwLongScanCount > 2)
            MSXAddFile(lpstCallBack,hFile);
    }
#endif // #ifdef MSX

    return(extStatus);
}


