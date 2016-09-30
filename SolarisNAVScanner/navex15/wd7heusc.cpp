//************************************************************************
//
// $Header:   S:/NAVEX/VCS/WD7HEUSC.CPv   1.5   09 Dec 1998 17:45:14   DCHI  $
//
// Description:
//      Contains Word 6.0/7.0 macro scan function code.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/WD7HEUSC.CPv  $
// 
//    Rev 1.5   09 Dec 1998 17:45:14   DCHI
// Added #include "gdefines.h"
// 
//    Rev 1.4   11 May 1998 17:57:42   DCHI
// Changes for Access 2.0 scanning and Access datafile conversion.
// 
//    Rev 1.3   15 Apr 1998 17:24:28   DCHI
// Modifications for new XL95 and XL5 engines.
// 
//    Rev 1.2   15 Oct 1997 18:15:42   DCHI
// Added Word 97 heuristics.
// 
//    Rev 1.1   06 Aug 1997 15:44:26   DCHI
// Added call to destroy copy structure on copy init failure.
// 
//    Rev 1.0   09 Jul 1997 16:24:06   DCHI
// Initial revision.
// 
//************************************************************************

#include "gdefines.h"

#ifdef MACROHEU

#include "platform.h"
#include "ctsn.h"
#include "navex.h"

#include "wd7api.h"
#include "o97api.h"
#include "xl5api.h"
#include "acsapi.h"

#include "wdscan15.h"
#include "offcsc15.h"

#include "wddecsig.h"

#include "wd7env.h"

#include "copy.h"

#include "mcrxclud.h"

//********************************************************************
//
// Function:
//  EXTSTATUS WD7DoHeuristicScanRepair()
//
// Parameters:
//  lpstStream              Ptr to WordDocument stream
//  lpstKey                 Ptr to key
//  bRepair                 TRUE if repair should be applied
//
// Description:
//  Performs a heuristic emulation for macro virus detection.
//
// Returns:
//	EXTSTATUS_OK			If no virus was found
//  EXTSTATUS_VIRUS_FOUND   If a virus was found
//  EXTSTATUS_NO_REPAIR     If the virus could not be repaired
//
//********************************************************************

#define WD7_MAX_XCL_ASIZE   4
#define WD7_MAX_CACHED_XCL  (WD7_MAX_XCL_ASIZE * 8)

EXTSTATUS WD7DoHeuristicScanRepair
(
    LPSS_STREAM             lpstStream,
    LPWD7ENCKEY             lpstKey,
    BOOL                    bRepair
)
{
    EXTSTATUS               extStatus = EXTSTATUS_OK;

    LPENV                   lpstEnv;
    WORD                    wMacroIdx;
    BYTE                    abyMacroName[256];
    DWORD                   dwMacroOffset;
    DWORD                   dwMacroSize;
    BYTE                    byMacroEncryptByte;
    BOOL                    bActive;
    DWORD                   dwCRC;
    BYTE                    abyExcludeHit[WD7_MAX_XCL_ASIZE];

    // Create heuristic scanning environment

    lpstEnv = WD7EnvCreate(lpstStream->lpstRoot->lpvRootCookie);

    if (lpstEnv == NULL)
        return(EXTSTATUS_MEM_ERROR);

    // Initialize TDT structure

    if (WD7InitTDTInfo(lpstStream,
                       lpstKey,
                       &lpstEnv->stTDTInfo) != WD7_STATUS_OK)
    {
        // Error getting TDT info

        WD7EnvDestroy(lpstEnv);
        return(EXTSTATUS_FILE_ERROR);
    }

    if (WD7CountActiveMacros(&lpstEnv->stTDTInfo,
                             &lpstEnv->wMacroCount) != WD7_STATUS_OK)
    {
        // Error counting number of active macros

        WD7EnvDestroy(lpstEnv);
        return(EXTSTATUS_FILE_ERROR);
    }

    //////////////////////////////////////////////////////////////////
    // Iterate through macros
    //////////////////////////////////////////////////////////////////

    lpstEnv->bNormal = FALSE;

    lpstEnv->lpvContextData =
        CopyCreate(lpstStream->lpstRoot->lpvRootCookie);

    if (lpstEnv->lpvContextData == NULL)
    {
        WD7EnvDestroy(lpstEnv);
        return(EXTSTATUS_MEM_ERROR);
    }

    if (CopyInit((LPCOPY)lpstEnv->lpvContextData) == FALSE)
    {
        CopyDestroy(lpstStream->lpstRoot->lpvRootCookie,
                    (LPCOPY)lpstEnv->lpvContextData);

        WD7EnvDestroy(lpstEnv);
        return(EXTSTATUS_MEM_ERROR);
    }

    // Initialize exclusion array to no hits

    for (wMacroIdx=0;wMacroIdx<WD7_MAX_XCL_ASIZE;wMacroIdx++)
        abyExcludeHit[wMacroIdx] = 0;

    //printf("****Executing as a local document...\n");

    for (wMacroIdx=0;
         wMacroIdx<lpstEnv->stTDTInfo.wNumMCDs;
         wMacroIdx++)
    {
            // Get the macro info

        if (WD7GetMacroInfoAtIndex(&lpstEnv->stTDTInfo,
                                   wMacroIdx,
                                   abyMacroName,
                                   &dwMacroOffset,
                                   &dwMacroSize,
                                   &byMacroEncryptByte,
                                   &bActive) == WD7_STATUS_ERROR ||
            bActive == FALSE)
        {
            // Error getting this macro or not an active macro,
            //  go to the next one

            continue;
        }

        // First get a CRC of the macro and check to see whether
        //  to exclude the macro from emulation

        if (WD7MacroBinaryCRC(lpstStream,
                              lpstKey,
                              dwMacroOffset,
                              dwMacroSize,
                              byMacroEncryptByte,
                              &dwCRC) == WD7_STATUS_OK)
        {
            if (MacroIsExcluded(MACRO_EXCLUDE_TYPE_WD7,
                                dwCRC,
                                dwMacroSize) == TRUE)
            {
                // Exclude this macro from emulation

                if (wMacroIdx < WD7_MAX_CACHED_XCL)
                    abyExcludeHit[wMacroIdx >> 3] |=
                        1 << (wMacroIdx & 7);

                continue;
            }
        }

        // Initialize the environment for the macro

        if (WD7EnvInit(lpstEnv,
                       lpstStream,
                       lpstKey) == FALSE)
        {
            CopyDestroy(lpstStream->lpstRoot->lpvRootCookie,
                        (LPCOPY)lpstEnv->lpvContextData);
            WD7EnvDestroy(lpstEnv);
            return(EXTSTATUS_MEM_ERROR);
        }

        // Scan the macro

        //printf("Executing %s...",abyMacroName);
        if (WD7Execute(lpstEnv,
                       wMacroIdx,
                       dwMacroOffset,
                       dwMacroSize,
                       byMacroEncryptByte) == FALSE)
        {
            //printf("Error executing at index %u\n",
            //    wMacroIdx);
            break;
        }
    }

    //printf("****Executing as the global template...\n");

    lpstEnv->bNormal = TRUE;

    for (wMacroIdx=0;
         wMacroIdx<lpstEnv->stTDTInfo.wNumMCDs;
         wMacroIdx++)
    {
        // Get the macro info

        if (WD7GetMacroInfoAtIndex(&lpstEnv->stTDTInfo,
                                   wMacroIdx,
                                   abyMacroName,
                                   &dwMacroOffset,
                                   &dwMacroSize,
                                   &byMacroEncryptByte,
                                   &bActive) == WD7_STATUS_ERROR ||
            bActive == FALSE)
        {
            // Error getting this macro or not an active macro,
            //  go to the next one

            continue;
        }

        // Check for emulation exclusion

        if (wMacroIdx < WD7_MAX_CACHED_XCL)
        {
            if (abyExcludeHit[wMacroIdx >> 3] & (1 << (wMacroIdx & 7)))
            {
                // Macro has been excluded from emulation

                continue;
            }
        }
        else
        {
            // Get a CRC of the macro and check to see whether
            //  to exclude the macro from emulation

            if (WD7MacroBinaryCRC(lpstStream,
                                  lpstKey,
                                  dwMacroOffset,
                                  dwMacroSize,
                                  byMacroEncryptByte,
                                  &dwCRC) == WD7_STATUS_OK)
            {
                if (MacroIsExcluded(MACRO_EXCLUDE_TYPE_WD7,
                                    dwCRC,
                                    dwMacroSize) == TRUE)
                {
                    // Exclude this macro from emulation

                    continue;
                }
            }
        }

        // Initialize the environment for the macro

        if (WD7EnvInit(lpstEnv,
                       lpstStream,
                       lpstKey) == FALSE)
        {
            CopyDestroy(lpstStream->lpstRoot->lpvRootCookie,
                        (LPCOPY)lpstEnv->lpvContextData);
            WD7EnvDestroy(lpstEnv);
            return(EXTSTATUS_MEM_ERROR);
        }

        // Scan the macro

        //printf("Executing %s...",abyMacroName);
        if (WD7Execute(lpstEnv,
                       wMacroIdx,
                       dwMacroOffset,
                       dwMacroSize,
                       byMacroEncryptByte) == FALSE)
        {
            //printf("Error executing at index %u\n",
            //    wMacroIdx);
            break;
        }
    }

/*
    CopyPrint((LPCOPY)lpstEnv->lpvContextData);

    if (CopyIsViral((LPCOPY)lpstEnv->lpvContextData) == FALSE)
        printf("-------Heuristically *NOT* viral!\n");
    else
        printf("-------Heuristically *IS* viral!\n");
*/
    if (bRepair == TRUE)
    {
        // Iterate for repair

        for (wMacroIdx=0;
             wMacroIdx<lpstEnv->stTDTInfo.wNumMCDs;
             wMacroIdx++)
        {
            // Get the macro's info

            if (WD7GetMacroInfoAtIndex(&lpstEnv->stTDTInfo,
                                       wMacroIdx,
                                       abyMacroName,
                                       &dwMacroOffset,
                                       &dwMacroSize,
                                       &byMacroEncryptByte,
                                       &bActive) == WD7_STATUS_ERROR ||
                bActive == FALSE)
            {
                // Error getting this macro or not an active macro,
                //  go to the next one

                continue;
            }

            // Get the UL name

            if (WD7GetULMacroNameAtIndex(&lpstEnv->stTDTInfo,
                                         wMacroIdx,
                                         abyMacroName) == WD7_STATUS_ERROR)
            {
                extStatus = EXTSTATUS_NO_REPAIR;
                break;
            }

            // See if we need to delete this macro

            if (CopyIsPartOfViralSet((LPCOPY)lpstEnv->lpvContextData,
                                     abyMacroName) == TRUE)
            {
                if (WD7DeactivateMacroAtIndex(&lpstEnv->stTDTInfo,
                                              wMacroIdx) != WD7_STATUS_OK)
                {
                    // Error deleting the macro

                    extStatus = EXTSTATUS_NO_REPAIR;
                    break;
                }
            }
        }
    }
    else
    {
        if (CopyIsViral((LPCOPY)lpstEnv->lpvContextData) != FALSE)
            extStatus = EXTSTATUS_VIRUS_FOUND;
    }

    CopyDestroy(lpstStream->lpstRoot->lpvRootCookie,
                (LPCOPY)lpstEnv->lpvContextData);
    WD7EnvDestroy(lpstEnv);

    return(extStatus);
}


//********************************************************************
//
// Function:
//  EXTSTATUS WD7HeuristicScan()
//
// Parameters:
//  lpstOffcScan            Ptr to office scan structure
//  lpwVID                  Ptr to WORD for VID
//
// Description:
//  Scans a stream for the presence of Word 6.0/7.0/95 Macro viruses.
//
// Returns:
//	EXTSTATUS_OK			If no virus was found
//  EXTSTATUS_VIRUS_FOUND   If a virus was found
//
//	If a virus was found, the function sets *lpwVID to the VID
//	of the virus.
//
//********************************************************************

EXTSTATUS WD7HeuristicScan
(
    LPMSOFFICE_SCAN         lpstOffcScan
)
{
    EXTSTATUS               extStatus = EXTSTATUS_OK;
    WD7ENCKEY_T             stKey;

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

        return(EXTSTATUS_OK);
    }


    /////////////////////////////////////////////////////////////
    // Iterate on scanning as long as a key is found
    /////////////////////////////////////////////////////////////

    while (1)
    {
        // Call off to heuristic scan

        extStatus = WD7DoHeuristicScanRepair(lpstOffcScan->lpstStream,
                                             &stKey,
                                             FALSE);

        if (extStatus != EXTSTATUS_OK)
            break;

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

    return (extStatus);
}

#endif // #ifdef MACROHEU

