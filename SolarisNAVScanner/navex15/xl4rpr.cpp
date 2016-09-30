//************************************************************************
//
// $Header:   S:/NAVEX/VCS/XL4RPR.CPv   1.1   11 May 1998 17:49:46   DCHI  $
//
// Description:
//  Contains Excel 4.0 macro repair function code.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/XL4RPR.CPv  $
// 
//    Rev 1.1   11 May 1998 17:49:46   DCHI
// Changes for Access 2.0 scanning and Access datafile conversion.
// 
//    Rev 1.0   15 Apr 1998 16:46:16   DCHI
// Initial revision.
// 
//************************************************************************

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
#include "xlrec.h"
#include "olestrnm.h"

#include "wdscan15.h"
#include "wdrepair.h"
#include "offcsc15.h"

#include "xl4scn.h"
#include "mcrsgutl.h"

#include "xl4rpr.h"
#include "xlrprsht.h"

//********************************************************************
//
// Function:
//  EXTSTATUS XL4Repair()
//
// Description:
//  Repairs a file of Excel 4.0 viruses.
//
// Returns:
//  EXTSTATUS_OK            If repair was successful
//  EXTSTATUS_NO_REPAIR     If repair was unsuccessful
//
//********************************************************************

EXTSTATUS XL4Repair
(
    LPMSOFFICE_SCAN         lpstOffcScan,
    LPBYTE                  lpbyWorkBuffer,
    LPWD_VIRUS_SIG_INFO     lpstVirusSigInfo
)
{
    LPWD_SCAN               lpstScan;
    DWORD                   dwOffset;
    DWORD                   dwMaxOffset;
    XL5ENCKEY_T             stKey;
    XL_REC_BOUNDSHEET_T     stBoundSheet;
    LPSS_STREAM             lpstStream;
    XL_REC_HDR_T            stRec;
    DWORD                   dwByteCount;
    BOOL                    bDelete;
    EXTSTATUS               extStatus;
    WORD                    wSheetIndex;

    (void)lpbyWorkBuffer;

    extStatus = EXTSTATUS_OK;

    lpstScan = lpstOffcScan->lpstScan;
    lpstStream = lpstOffcScan->lpstStream;

    // Assign hit memory

    if (AssignMacroSigHitMem(lpstOffcScan,
                             lpstScan) == FALSE)
    {
        return(EXTSTATUS_MEM_ERROR);
    }

    // Initialize other scan structure fields

    lpstScan->lpstStream = lpstStream;

    // Get the key for the stream

    if (lpstOffcScan->wType == OLE_SCAN_XL4_95)
    {
        lpstScan->wScanType = WD_SCAN_TYPE_XL4_95;
        if (XL5FindKey(lpstStream,
                       &stKey) == FALSE)
            return(EXTSTATUS_MEM_ERROR);

        lpstScan->uScan.stXL4.lpstKey = &stKey;
    }
    else
    {
        lpstScan->wScanType = WD_SCAN_TYPE_XL4_97;
        stKey.bEncrypted = FALSE;
        lpstScan->uScan.stXL4.lpstKey = NULL;
    }

    // Iterate through the global workbook records, looking
    //  for boundsheet records that are Excel 4.0 sheets

    dwOffset = 0;
    dwMaxOffset = SSStreamLen(lpstStream);
    wSheetIndex = 0;
    while (dwOffset < dwMaxOffset)
    {
        if (SSSeekRead(lpstStream,
                       dwOffset,
                       &stRec,
                       sizeof(XL_REC_HDR_T),
                       &dwByteCount) != SS_STATUS_OK ||
            dwByteCount != sizeof(XL_REC_HDR_T))
            return(WD_STATUS_ERROR);

        stRec.wType = WENDIAN(stRec.wType);
        stRec.wLen = WENDIAN(stRec.wLen);

        if (stRec.wType == eXLREC_EOF)
            break;

        if (stRec.wType == eXLREC_BOUNDSHEET)
        {
            // Read the BOUNDSHEET record

            if (XL5EncryptedRead(lpstStream,
                                 dwOffset,
                                 stRec,
                                 &stKey,
                                 0,
                                 (LPBYTE)&stBoundSheet,
                                 sizeof(XL_REC_BOUNDSHEET_T)) == FALSE)
            {
                extStatus = EXTSTATUS_NO_REPAIR;
                break;
            }

            // Is it an Excel 4.0 sheet?

            if ((stBoundSheet.bySheetType & XL_REC_BOUNDSHEET_TYPE_MASK) ==
                XL_REC_BOUNDSHEET_40_MACRO)
            {
                // Initialize hit memory

                WDInitHitBitArrays(&gstXL4SigSet,lpstScan);

                // Scan the sheet

                if (XL4ScanSheet(lpstOffcScan->wType,
                                 lpstScan,
                                 dwOffset,
                                 stRec,
                                 &stBoundSheet) == FALSE)
                {
                    extStatus = EXTSTATUS_NO_REPAIR;
                    break;
                }

                // Determine whether to delete this sheet

                if (WDApplyRepair(&gstXL4SigSet,
                                  lpstScan,
                                  &bDelete,
                                  lpstVirusSigInfo -
                                      gstXL4SigSet.lpastVirusSigInfo,
                                  0) == WD_STATUS_ERROR)
                {
                    // Error applying repair signature

                    extStatus = EXTSTATUS_NO_REPAIR;
                    break;
                }

                // Delete this macro?

                if (bDelete == TRUE)
                {
                    // Erase the sheet information

                    if (lpstOffcScan->wType == OLE_SCAN_XL5 ||
                        lpstOffcScan->wType == OLE_SCAN_XL4_95)
                    {
                        if (XL5EraseSheet(lpstStream,
                                          &stKey,
                                          dwOffset,
                                          wSheetIndex) == FALSE)
                        {
                            extStatus = EXTSTATUS_NO_REPAIR;
                            break;
                        }
                    }
                    else
                    {
                        if (XL97EraseSheet(lpstStream,
                                           dwOffset,
                                           wSheetIndex) == FALSE)
                        {
                            extStatus = EXTSTATUS_NO_REPAIR;
                            break;
                        }
                    }
                }
            }

            ++wSheetIndex;
        }

        dwOffset += (DWORD)sizeof(XL_REC_HDR_T) + stRec.wLen;
    }

    if (extStatus == EXTSTATUS_OK)
    {
        // Repair any sheets

        if (XLRepairSheets(lpstOffcScan,
                           lpstStream,
                           &gstXL4SigSet,
                           lpstVirusSigInfo) != EXTSTATUS_OK)
            extStatus = EXTSTATUS_NO_REPAIR;
    }

    return (extStatus);
}

#endif  // #ifndef SYM_NLM



