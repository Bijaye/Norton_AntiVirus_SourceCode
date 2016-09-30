//************************************************************************
//
// $Header:   S:/NAVEX/VCS/XLRPRSHT.CPv   1.1   11 May 1998 17:49:32   DCHI  $
//
// Description:
//  Contains Excel sheet repair function code.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/XLRPRSHT.CPv  $
// 
//    Rev 1.1   11 May 1998 17:49:32   DCHI
// Changes for Access 2.0 scanning and Access datafile conversion.
// 
//    Rev 1.0   15 Apr 1998 16:46:14   DCHI
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

#include "mcrsgutl.h"

#include "xlrprsht.h"

//********************************************************************
//
// Function:
//  EXTSTATUS XLRepairSheets()
//
// Description:
//  Applies the signature set to the sheets in the given workbook
//  stream and erases any sheets that meet the criteria.
//
// Returns:
//  EXTSTATUS_OK            If repair was successful
//  EXTSTATUS_NO_REPAIR     If repair was unsuccessful
//
//********************************************************************

EXTSTATUS XLRepairSheets
(
    LPMSOFFICE_SCAN         lpstOffcScan,
    LPSS_STREAM             lpstStream,
    LPWD_SIG_SET            lpstSigSet,
    LPWD_VIRUS_SIG_INFO     lpstVirusSigInfo
)
{
    LPWD_SCAN               lpstScan;
    DWORD                   dwOffset;
    DWORD                   dwMaxOffset;
    XL5ENCKEY_T             stKey;
    XL_REC_BOUNDSHEET_T     stBoundSheet;
    XL_REC_HDR_T            stRec;
    DWORD                   dwByteCount;
    BOOL                    bDelete;
    EXTSTATUS               extStatus;
    WORD                    wSheetIndex;
    int                     nNameLen;
    int                     i;

    extStatus = EXTSTATUS_OK;

    lpstScan = lpstOffcScan->lpstScan;

    // Assign hit memory

    if (AssignMacroSigHitMem(lpstOffcScan,
                             lpstScan) == FALSE)
    {
        return(EXTSTATUS_MEM_ERROR);
    }

    // Initialize other scan structure fields

    lpstScan->lpstStream = lpstStream;

    // Get the key for the stream

    if (lpstOffcScan->wType == OLE_SCAN_XL5 ||
        lpstOffcScan->wType == OLE_SCAN_XL4_95)
    {
        if (XL5FindKey(lpstStream,
                       &stKey) == FALSE)
            return(EXTSTATUS_NO_REPAIR);
    }
    else
        stKey.bEncrypted = FALSE;

    // Iterate through the global workbook records, looking
    //  for boundsheet records

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

            // First, get the name

            if (stBoundSheet.byNameLen > XL_REC_BOUNDSHEET_MAX_NAME_LEN)
                nNameLen = XL_REC_BOUNDSHEET_MAX_NAME_LEN;
            else
                nNameLen = stBoundSheet.byNameLen;

            if (lpstOffcScan->wType == OLE_SCAN_XL5 ||
                lpstOffcScan->wType == OLE_SCAN_XL4_95)
            {
                if (XL5EncryptedRead(lpstStream,
                                     dwOffset,
                                     stRec,
                                     &stKey,
                                     sizeof(XL_REC_BOUNDSHEET_T),
                                     lpstScan->abyName,
                                     nNameLen) == FALSE)
                    return(FALSE);
            }
            else
            {
                if (XL97ReadUnicodeStr(lpstStream,
                                       dwOffset + sizeof(XL_REC_HDR_T) +
                                           sizeof(XL_REC_BOUNDSHEET_T),
                                       TRUE,
                                       lpstScan->abyName,
                                       (WORD)nNameLen,
                                       NULL) == FALSE)
                    return(FALSE);
            }

            // Zero-terminate the name and uppercase it

            lpstScan->abyName[nNameLen] = 0;
            for (i=0;i<nNameLen;i++)
                lpstScan->abyName[i] = SSToUpper(lpstScan->abyName[i]);

            // Initialize hit memory

            WDInitHitBitArrays(lpstSigSet,lpstScan);

            // Scan the name

            if (WDApplyNameSigs(lpstSigSet,
                                lpstScan) != WD_STATUS_OK)
            {
                // Error applying name signatures

                extStatus = EXTSTATUS_NO_REPAIR;
                break;
            }

            // Determine whether to delete this sheet

            if (WDApplyRepair(lpstSigSet,
                              lpstScan,
                              &bDelete,
                              lpstVirusSigInfo -
                                  lpstSigSet->lpastVirusSigInfo,
                              0) == WD_STATUS_ERROR)
            {
                // Error applying repair signature

                extStatus = EXTSTATUS_NO_REPAIR;
                break;
            }

            // Delete this sheet?

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

            ++wSheetIndex;
        }

        dwOffset += (DWORD)sizeof(XL_REC_HDR_T) + stRec.wLen;
    }

    return (extStatus);
}

#endif  // #ifndef SYM_NLM



