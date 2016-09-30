//************************************************************************
//
// $Header:   S:/NAVEX/VCS/O97scn15.cpv   1.9   15 Dec 1998 12:11:38   DCHI  $
//
// Description:
//      Contains Office 97 macro scan function code.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/O97scn15.cpv  $
// 
//    Rev 1.9   15 Dec 1998 12:11:38   DCHI
// Added PowerPoint scan case.
// 
//    Rev 1.8   09 Dec 1998 17:45:34   DCHI
// Added #include "gdefines.h"
// 
//    Rev 1.7   08 Dec 1998 12:57:08   DCHI
// Changes for MSX.
// 
//    Rev 1.6   12 Oct 1998 13:41:42   DCHI
// Limited scan to at most 16K modules.
// 
//    Rev 1.5   04 Aug 1998 13:52:32   DCHI
// Changes for Excel heuristics.
// 
//    Rev 1.4   11 May 1998 17:53:32   DCHI
// Changes for Access 2.0 scanning and Access datafile conversion.
// 
//    Rev 1.3   15 Apr 1998 17:24:10   DCHI
// Modifications for new XL95 and XL5 engines.
// 
//    Rev 1.2   10 Mar 1998 13:41:08   DCHI
// Modified to use new O97 CRC.
// 
//    Rev 1.1   26 Nov 1997 18:35:10   DCHI
// Added #ifdef'd out PowerPoint scanning code.
// 
//    Rev 1.0   09 Jul 1997 16:18:04   DCHI
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

#include "mcrsgutl.h"

#include "olestrnm.h"

#include "mvp.h"

//********************************************************************
//
// Function:
//  EXTSTATUS VBA5Scan()
//
// Parameters:
//  lpstOffcScan            Ptr to office scan structure
//  lpbyWorkBuffer          Work buffer >= 870 bytes
//  lplpstVirusSigInfo      Virus info storage on hit
//  lpbMVPApproved          Ptr to BOOL for MVP status
//
// Description:
//  Scans a file for the presence of Word 8.0 Macro viruses.
//
// Returns:
//	EXTSTATUS_OK			If no virus was found
//  EXTSTATUS_VIRUS_FOUND   If a virus was found
//
//	If a virus was found, the function sets *lpwVID to the VID
//	of the virus.
//
//********************************************************************

EXTSTATUS VBA5Scan
(
    LPMSOFFICE_SCAN         lpstOffcScan,
    LPLPWD_VIRUS_SIG_INFO   lplpstVirusSigInfo,
    LPBOOL                  lpbMVPApproved
)
{
    LPWD_SCAN               lpstScan;
    LPSS_ENUM_SIBS          lpstSibs;
    LPWD_SIG_SET            lpstSigSet;
    DWORD                   dwFirstStreamID;
    int                     i;
    int                     nMVPType;

    lpstScan = lpstOffcScan->lpstScan;

    // Determine whether we are scanning WD8 or XL97

    switch (lpstOffcScan->wType)
    {
        case OLE_SCAN_WD8:
            lpstSigSet = &gstWD8SigSet;
            nMVPType = MVP_WD8;
            break;

        case OLE_SCAN_XL97:
            lpstSigSet = &gstXL97SigSet;
            nMVPType = MVP_XL97;
            break;

#ifdef PP_SCAN
        case OLE_SCAN_PP:
            // Need to fix this when we get the chance
            lpstSigSet = &gstP97SigSet;
            nMVPType = -1;
            break;
#endif // #ifdef PP_SCAN

        default:
            // This should never happen
            return(EXTSTATUS_OK);
    }


    /////////////////////////////////////////////////////////////
    // Allocate memory and initialize
    /////////////////////////////////////////////////////////////

    // Allocate LZNT structure

    if (LZNTAllocStruct(lpstOffcScan->lpstRoot->lpvRootCookie,
                        &lpstScan->uScan.stVBA5.lpstLZNT) != TRUE)
    {
        return(FALSE);
    }

    // Allocate a sibiling enumeration structure

    if (SSAllocEnumSibsStruct(lpstOffcScan->lpstRoot,
                              &lpstSibs,
                              SS_DEF_MAX_WAITING_SIBS) != SS_STATUS_OK)
    {
        LZNTFreeStruct(lpstOffcScan->lpstRoot->lpvRootCookie,
                       lpstScan->uScan.stVBA5.lpstLZNT);

        return(FALSE);
    }

    // Assign hit memory

    if (AssignMacroSigHitMem(lpstOffcScan,
                             lpstScan) == FALSE)
    {
        LZNTFreeStruct(lpstOffcScan->lpstRoot->lpvRootCookie,
                       lpstScan->uScan.stVBA5.lpstLZNT);

        SSFreeEnumSibsStruct(lpstOffcScan->lpstRoot,lpstSibs);
        return(EXTSTATUS_MEM_ERROR);
    }

    // Initialize hit memory

    WDInitHitBitArrays(lpstSigSet,lpstScan);

    // Initialize other scan structure fields

    lpstScan->lpstStream = lpstOffcScan->lpstStream;
    lpstScan->wScanType = WD_SCAN_TYPE_VBA5;

    // Initialize macro count

    lpstScan->wMacroCount = 0;

    // Assume a full set

    lpstScan->wFlags = WD_SCAN_FLAG_FULL_SET;


    /////////////////////////////////////////////////////////////
    // Iterate through the children of the VBA storage
    /////////////////////////////////////////////////////////////

    // Find the <VBA> storage

    if (O97OpenVBAStorage(lpstOffcScan->lpstRoot,
                          &lpstOffcScan->u.stO97) == FALSE)
    {
        // No VBA storage!

        LZNTFreeStruct(lpstOffcScan->lpstRoot->lpvRootCookie,
                       lpstScan->uScan.stVBA5.lpstLZNT);

        SSFreeEnumSibsStruct(lpstOffcScan->lpstRoot,lpstSibs);
        return(EXTSTATUS_OK);
    }

    // Start with the VBA child entry

    SSInitEnumSibsStruct(lpstSibs,
                         lpstOffcScan->u.stO97.dwVBAChildEntry);

    dwFirstStreamID = 0;
    for (i=0;i<16384;i++)
    {
        if (SSEnumSiblingEntriesCB(lpstOffcScan->lpstRoot,
                                   O97OpenVBA5ModuleCB,
                                   lpstScan->abyName,
                                   lpstSibs,
                                   lpstOffcScan->lpstStream) != SS_STATUS_OK)
            break;
    
        // Check for circular link

        if (dwFirstStreamID == 0)
            dwFirstStreamID = SSStreamID(lpstOffcScan->lpstStream);
        else
        if (dwFirstStreamID == SSStreamID(lpstOffcScan->lpstStream))
            break;

        VBA5ScanModule(lpstSigSet,lpstScan);

        if (O97LZNTStreamAttributeOnly(lpstScan->uScan.stVBA5.lpstLZNT) ==
            FALSE)
        {
            if (MVPCheck(nMVPType,
                         lpstScan->abyName,
                         lpstScan->dwMVPCRC) == FALSE)
            {
                // Found a non-approved macro

                *lpbMVPApproved = FALSE;
            }

            // The stream contains non-attribute lines

            // Determine full set status

            if ((lpstScan->wFlags & WD_SCAN_FLAG_MACRO_IS_FULL_SET) == 0)
            {
                // Found a macro that was not part of the full set

                lpstScan->wFlags &= ~WD_SCAN_FLAG_FULL_SET;
            }

            lpstScan->wMacroCount++;
        }
    }

    // We can free the sibling enumeration and LZNT structures now

    LZNTFreeStruct(lpstOffcScan->lpstRoot->lpvRootCookie,
                   lpstScan->uScan.stVBA5.lpstLZNT);

    SSFreeEnumSibsStruct(lpstOffcScan->lpstRoot,lpstSibs);

    // Update the long scan count

    lpstOffcScan->dwLongScanCount += lpstScan->wMacroCount;


    /////////////////////////////////////////////////////////////
    // Do virus, full set, and MVP checking now
    /////////////////////////////////////////////////////////////

    // If macro count is zero, don't assume full set

    if (lpstScan->wMacroCount == 0)
        lpstScan->wFlags &= ~WD_SCAN_FLAG_FULL_SET;

    // Apply virus signatures

    if (WDApplyVirusSignatures(lpstSigSet,
                               lpstScan,
                               lplpstVirusSigInfo) == WD_STATUS_VIRUS_FOUND)
    {
        DWORD   dwID;

        switch (lpstOffcScan->wType)
        {
            case OLE_SCAN_WD8:
                if (SSGetNamedSiblingID(lpstOffcScan->lpstRoot,
                                        lpstOffcScan->u.stO97.u.stWD8.
                                            dwMacrosEntry,
                                        gabywszWordDocument,
                                        &dwID) == FALSE || dwID == 0)
                    return(EXTSTATUS_OK);

                break;

            case OLE_SCAN_XL97:
                if (SSGetNamedSiblingID(lpstOffcScan->lpstRoot,
                                        lpstOffcScan->u.stO97.u.stXL97.
                                            dw_VBA_PROJECT_CUREntry,
                                        gabywszWorkbook,
                                        &dwID) == FALSE || dwID == 0)
                    return(EXTSTATUS_OK);

                break;

            default:
                break;
        }

        return(EXTSTATUS_VIRUS_FOUND);
    }

    // No virus

    return (EXTSTATUS_OK);
}



