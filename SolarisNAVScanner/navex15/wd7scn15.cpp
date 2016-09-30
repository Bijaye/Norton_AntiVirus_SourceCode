//************************************************************************
//
// $Header:   S:/NAVEX/VCS/wd7scn15.cpv   1.4   09 Dec 1998 17:45:34   DCHI  $
//
// Description:
//      Contains Word 6.0/7.0 macro scan function code.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/wd7scn15.cpv  $
// 
//    Rev 1.4   09 Dec 1998 17:45:34   DCHI
// Added #include "gdefines.h"
// 
//    Rev 1.3   08 Dec 1998 12:57:26   DCHI
// Changes for MSX.
// 
//    Rev 1.2   11 May 1998 18:15:36   DCHI
// Changes for Access 2.0 scanning and Access datafile conversion.
// 
//    Rev 1.1   15 Apr 1998 17:24:30   DCHI
// Modifications for new XL95 and XL5 engines.
// 
//    Rev 1.0   09 Jul 1997 16:18:32   DCHI
// Initial revision.
// 
//************************************************************************

#include "gdefines.h"

#include "platform.h"
#include "ctsn.h"
#include "navex.h"

#include "wd7api.h"
#include "o97api.h"
#include "xl5api.h"
#include "acsapi.h"

#include "wdscan15.h"
#include "offcsc15.h"
#include "o97scn15.h"
#include "wd7scn15.h"
#include "mcrsgutl.h"

#include "wddecsig.h"

//********************************************************************
//
// Function:
//  EXTSTATUS WD7Scan()
//
// Parameters:
//  lpstOffcScan            Ptr to office scan structure
//  lplpstVirusSigInfo      Ptr to ptr for virus sig info
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

EXTSTATUS WD7Scan
(
    LPMSOFFICE_SCAN         lpstOffcScan,
    LPLPWD_VIRUS_SIG_INFO   lplpstVirusSigInfo
)
{
    EXTSTATUS               extStatus = EXTSTATUS_OK;
    WD7ENCKEY_T             stKey;
    LPWD_SCAN               lpstScan;
    BOOL                    bMVPApproved;

    lpstScan = lpstOffcScan->lpstScan;

    /////////////////////////////////////////////////////////////
    // Initialize for Word 6.0/7.0/95 scanning
    /////////////////////////////////////////////////////////////

    // Allocate a macro scan structure

    if (WD7AllocMacroScanStruct(lpstOffcScan->lpstRoot->lpvRootCookie,
                                &lpstScan->uScan.
                                    stWD7.lpstMacroScan,
                                0,
                                0) != WD7_STATUS_OK)
    {
        return(EXTSTATUS_MEM_ERROR);
    }

    // Assign hit memory

    if (AssignMacroSigHitMem(lpstOffcScan,
                             lpstScan) == FALSE)
    {
        WD7FreeMacroScanStruct(lpstOffcScan->lpstRoot->lpvRootCookie,
                               lpstScan->uScan.
                                   stWD7.lpstMacroScan);

        return(EXTSTATUS_MEM_ERROR);
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

        return(EXTSTATUS_OK);
    }


    /////////////////////////////////////////////////////////////
    // Iterate on scanning as long as a key is found
    /////////////////////////////////////////////////////////////

    while (1)
    {
        bMVPApproved = TRUE;
        if (WDScanDoc(&gstWD7SigSet,
                      lpstScan,
                      lplpstVirusSigInfo,
                      &bMVPApproved) == WD_STATUS_VIRUS_FOUND)
        {
            extStatus = EXTSTATUS_VIRUS_FOUND;
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

    lpstOffcScan->bMVPApproved = bMVPApproved;

    // Free the scan structure

    WD7FreeMacroScanStruct(lpstOffcScan->lpstRoot->lpvRootCookie,
                           lpstScan->uScan.stWD7.lpstMacroScan);

    // Update the long scan count

    lpstOffcScan->dwLongScanCount += lpstScan->wMacroCount;

    return (extStatus);
}

