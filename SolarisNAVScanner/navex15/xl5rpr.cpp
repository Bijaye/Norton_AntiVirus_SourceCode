//***********************************************************************
//
// $Header:   S:/NAVEX/VCS/XL5RPR.CPv   1.5   09 Dec 1998 17:45:40   DCHI  $
//
// Description:
//  Contains function for Excel 5.0/95 macro virus repair.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/XL5RPR.CPv  $
// 
//    Rev 1.5   09 Dec 1998 17:45:40   DCHI
// Added #include "gdefines.h"
// 
//    Rev 1.4   08 Dec 1998 13:00:56   DCHI
// Changes for MSX.
// 
//    Rev 1.3   09 Sep 1998 12:45:28   DCHI
// Changes for password-protected Excel 97 document repair.
// 
//    Rev 1.2   08 Jul 1998 15:47:04   DCHI
// Added checks for encrypted [companion] Workbook streams.
// 
//    Rev 1.1   11 May 1998 17:49:48   DCHI
// Changes for Access 2.0 scanning and Access datafile conversion.
// 
//    Rev 1.0   15 Apr 1998 16:46:20   DCHI
// Initial revision.
// 
//************************************************************************

#include "gdefines.h"

#if defined(NAVEX15) || !defined(SYM_NLM)

#include "platform.h"

#ifdef WIN16        // VIRSCAN QUAKE 2
#include "cts4.h"
#else
#include "ctsn.h"
#include "callback.h"
#endif

#include "navex.h"

#include "wd7api.h"
#include "o97api.h"
#include "xl5api.h"
#include "acsapi.h"
#include "olestrnm.h"

#include "wdscan15.h"
#include "wdrepair.h"
#include "offcsc15.h"

#include "mcrsgutl.h"

#include "xl5rpr.h"
#include "xlrprsht.h"

#include "nvx15inf.h"

extern int gnXL97EncryptedRepair;

//********************************************************************
//
// Function:
//  BOOL XL5EncryptedRepairCheck()
//
// Parameters:
//  lpstOffcScan        Ptr to MSOFFICE_SCAN_T structure
//
// Description:
//  Determines whether or not to proceed with repair based on
//  the encrypted state of a companion Workbook stream if present.
//
// Returns:
//  TRUE                If repair should proceed
//  FALSE               If repair should not proceed
//
//********************************************************************

BOOL XL5EncryptedRepairCheck
(
    LPMSOFFICE_SCAN     lpstOffcScan
)
{
    BOOL                bResult;

#if !defined(NAVEX15) && (defined(SYM_WIN16) || defined(SYM_DOSX))

    // See if we have loaded up XL97 encrypted repair enabled flag yet

    if (gbCheckedXL97EncRepEnableState == FALSE)
        GetXL97EncRepEnableState((LPCALLBACKREV1)lpstOffcScan->lpstRoot->
                                     lpvRootCookie);

#endif // #if defined(SYM_DOSX) || defined(SYM_WIN16)

    // First determine whether there is
    //  an encrypted companion Workbook stream.
    //  If there is, don't repair if we are not doing
    //  XL97 encrypted repair.

    if (XL97EntryHasEncryptedWBSib(lpstOffcScan->u.stXL5.
                                       dw_VBA_PROJECTEntry,
                                   lpstOffcScan->lpstStream,
                                   &bResult) == FALSE)
        return(FALSE);

    // If the document is encrypted and we are not doing
    //  XL97 encrypted repair, return NO_REPAIR

    if (bResult != FALSE && gnXL97EncryptedRepair == 0)
        return(FALSE);

    // The document is either not encrypted or
    //  it is and we are doing XL97 encrypted repair

    return(TRUE);
}


//********************************************************************
//
// Function:
//  EXTSTATUS XL5Repair()
//
// Description:
//  Repairs a file of the given Excel 5.0/95 virus.
//
// Returns:
//  EXTSTATUS_OK            If repair was successful
//  EXTSTATUS_NO_REPAIR     If repair was unsuccessful
//
//********************************************************************

EXTSTATUS XL5Repair
(
    LPMSOFFICE_SCAN         lpstOffcScan,
    LPBYTE                  lpbyWorkBuffer,
    LPWD_VIRUS_SIG_INFO     lpstVirusSigInfo
)
{
    LPWD_SCAN               lpstScan;
    LPSS_ENUM_SIBS          lpstSibs;
    LPSS_STREAM             lpstDirStream;
    LPSS_STREAM             lpstBookStream;
    BYTE                    abypsStreamName[SS_MAX_NAME_LEN+2];
    int                     i, nLen;
    BOOL                    bDelete;
    XL5ENCKEY_T             stKey;
    EXTSTATUS               extStatus = EXTSTATUS_OK;

    // Decide whether to proceed based on encryption state

    if (XL5EncryptedRepairCheck(lpstOffcScan) == FALSE)
        return(EXTSTATUS_NO_REPAIR);

    lpstScan = lpstOffcScan->lpstScan;

    // Allocate a stream structure for the Book stream

    if (SSAllocStreamStruct(lpstOffcScan->lpstRoot,
                            &lpstBookStream,
                            SS_STREAM_FLAG_DEF_BAT_CACHE) != SS_STATUS_OK)
        return(EXTSTATUS_NO_REPAIR);

    // Get the associated Book stream

    if (XL5GetVBABookStream(lpstOffcScan->lpstRoot,
                            lpstOffcScan->u.stXL5.
                                dw_VBA_PROJECTEntry,
                            lpstBookStream) == FALSE)
    {
        SSFreeStreamStruct(lpstBookStream);
        return(EXTSTATUS_NO_REPAIR);
    }

    // Get the key

    if (XL5FindKey(lpstBookStream,&stKey) == FALSE)
    {
        SSFreeStreamStruct(lpstBookStream);
        return(EXTSTATUS_NO_REPAIR);
    }

    // Allocate a stream structure for the dir stream

    if (SSAllocStreamStruct(lpstOffcScan->lpstRoot,
                            &lpstDirStream,
                            SS_STREAM_FLAG_DEF_BAT_CACHE) != SS_STATUS_OK)
    {
        SSFreeStreamStruct(lpstBookStream);
        return(EXTSTATUS_NO_REPAIR);
    }

    // Allocate a sibiling enumeration structure

    if (SSAllocEnumSibsStruct(lpstOffcScan->lpstRoot,
                              &lpstSibs,
                              SS_DEF_MAX_WAITING_SIBS) != SS_STATUS_OK)
    {
        SSFreeStreamStruct(lpstDirStream);
        return(EXTSTATUS_NO_REPAIR);
    }

    // Assign hit memory

    if (AssignMacroSigHitMem(lpstOffcScan,
                             lpstScan) == FALSE)
    {
        SSFreeStreamStruct(lpstBookStream);
        SSFreeStreamStruct(lpstDirStream);
        SSFreeEnumSibsStruct(lpstOffcScan->lpstRoot,lpstSibs);
        return(EXTSTATUS_NO_REPAIR);
    }

    // Initialize other scan structure fields

    lpstScan->lpstStream = lpstOffcScan->lpstStream;
    lpstScan->wScanType = WD_SCAN_TYPE_XL5;

    // Get the dir stream

    SSInitEnumSibsStruct(lpstSibs,
                         lpstOffcScan->u.stXL5.dw_VBA_PROJECTChildEntry);

    if (SSEnumSiblingEntriesCB(lpstOffcScan->lpstRoot,
                               SSOpenStreamByNameCB,
                               gabywszdir,
                               lpstSibs,
                               lpstDirStream) != SS_STATUS_OK)
    {
        SSFreeStreamStruct(lpstBookStream);
        SSFreeStreamStruct(lpstDirStream);
        SSFreeEnumSibsStruct(lpstOffcScan->lpstRoot,lpstSibs);
        return(EXTSTATUS_NO_REPAIR);
    }

    // Initialize for dir access

    if (XL5DirInfoInit(lpstDirStream,
                       &lpstScan->uScan.stXL5.stDirInfo) == FALSE)
    {
        SSFreeStreamStruct(lpstBookStream);
        SSFreeStreamStruct(lpstDirStream);
        SSFreeEnumSibsStruct(lpstOffcScan->lpstRoot,lpstSibs);
        return(EXTSTATUS_NO_REPAIR);
    }

    // Start with the VBA child entry

    SSInitEnumSibsStruct(lpstSibs,
                         lpstOffcScan->u.stXL5.dw_VBA_PROJECTChildEntry);

    while (SSEnumSiblingEntriesCB(lpstOffcScan->lpstRoot,
                                  XL5OpenHexNamedStreamCB,
                                  abypsStreamName + 1,
                                  lpstSibs,
                                  lpstOffcScan->lpstStream) == SS_STATUS_OK)
    {
        // Set up the stream name's length byte

        abypsStreamName[0] = 0;
        while (abypsStreamName[abypsStreamName[0] + 1])
            abypsStreamName[0]++;

        // Get the actual name of the module

        if (XL5DirInfoModuleName(&lpstScan->uScan.stXL5.stDirInfo,
                                 abypsStreamName,
                                 lpstScan->abyName) == FALSE)
        {
            // Try the next one

            continue;
        }

        // Upper case the module name and place it in lpstScan->abyName

        nLen = lpstScan->abyName[0];
        for (i=0;i<nLen;i++)
            lpstScan->abyName[i] = SSToUpper(lpstScan->abyName[i+1]);

        lpstScan->abyName[i] = 0;

        if (XL5GetMacroLineInfo(lpstOffcScan->lpstStream,
                                &lpstScan->uScan.
                                    stXL5.dwTableOffset,
                                &lpstScan->uScan.
                                    stXL5.dwTableSize,
                                &lpstScan->uScan.
                                    stXL5.dwMacroOffset,
                                &lpstScan->uScan.
                                    stXL5.dwMacroSize) == FALSE)
        {
            // Try the next one

            continue;
        }

        // Initialize hit memory

        WDInitHitBitArrays(&gstXL95SigSet,lpstScan);

        // Scan the module

        if (WDScanMacro(&gstXL95SigSet,
                        lpstScan) == WD_STATUS_OK)
        {
            if (WDApplyRepair(&gstXL95SigSet,
                              lpstScan,
                              &bDelete,
                              lpstVirusSigInfo -
                                  gstXL95SigSet.lpastVirusSigInfo,
                              0) == WD_STATUS_ERROR)
            {
                // Error applying repair signature

                extStatus = EXTSTATUS_NO_REPAIR;
                break;
            }

            // Delete this macro?

            if (bDelete == TRUE)
            {
                DWORD       dwSheetIndex;
                DWORD       dwBoundSheetOffset;

                // Create the Pascal name

                lpbyWorkBuffer[0] = nLen;
                for (i=0;i<nLen;i++)
                    lpbyWorkBuffer[i+1] = lpstScan->abyName[i];

                // Get the BOUNDSHEET info for the sheet

                dwSheetIndex = 0;
                if (XL5FindBoundSheet(lpstBookStream,
                                      &stKey,
                                      lpbyWorkBuffer,
                                      0xFF,
                                      &dwSheetIndex,
                                      &dwBoundSheetOffset,
                                      NULL) == FALSE)
                {
                    extStatus = EXTSTATUS_NO_REPAIR;
                    break;
                }

                // Erase the sheet information

                if (XL5EraseSheet(lpstBookStream,
                                  &stKey,
                                  dwBoundSheetOffset,
                                  (WORD)dwSheetIndex) == FALSE)
                {
                    extStatus = EXTSTATUS_NO_REPAIR;
                    break;
                }

                // Delete the macro contents

                if (XL5BlankOutVBModuleStream(lpstOffcScan->
                                                  lpstStream) == FALSE)
                {
                    extStatus = EXTSTATUS_NO_REPAIR;
                    break;
                }
            }
        }
    }

    if (extStatus == EXTSTATUS_OK)
    {
        // Repair any sheets

        if (XLRepairSheets(lpstOffcScan,
                           lpstBookStream,
                           &gstXL95SigSet,
                           lpstVirusSigInfo) != EXTSTATUS_OK)
            extStatus = EXTSTATUS_NO_REPAIR;
    }

    // We can free the sibling enumeration structure
    //  and the dir stream structure

    SSFreeStreamStruct(lpstBookStream);
    SSFreeStreamStruct(lpstDirStream);
    SSFreeEnumSibsStruct(lpstOffcScan->lpstRoot,lpstSibs);

    return(extStatus);
}

#endif  // #ifndef SYM_NLM



