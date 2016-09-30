//************************************************************************
//
// $Header:   S:/NAVEX/VCS/OFFCSC15.CPv   1.10   15 Dec 1998 12:12:38   DCHI  $
//
// Description:
//      Contains Microsoft Office macro scan function code.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/OFFCSC15.CPv  $
// 
//    Rev 1.10   15 Dec 1998 12:12:38   DCHI
// Added gnPP97ScanEnabled global.
// 
//    Rev 1.9   11 Nov 1998 11:24:46   DCHI
// Perform exhaustive known virus search before calling heuristics.
// 
//    Rev 1.8   10 Jun 1998 13:33:52   DCHI
// Changed MuckedUp() to return(FALSE) always.
// 
//    Rev 1.7   11 May 1998 17:57:40   DCHI
// Changes for Access 2.0 scanning and Access datafile conversion.
// 
//    Rev 1.6   15 Apr 1998 17:24:12   DCHI
// Modifications for new XL95 and XL5 engines.
// 
//    Rev 1.5   29 Jan 1998 19:04:28   DCHI
// Added support for Excel formula virus scan and repair.
// 
//    Rev 1.4   26 Nov 1997 18:35:12   DCHI
// Added #ifdef'd out PowerPoint scanning code.
// 
//    Rev 1.3   24 Nov 1997 18:26:12   DCHI
// Change McFlea function to check for a TDT <= 2.
// 
//    Rev 1.2   13 Nov 1997 14:00:10   DCHI
// Oops, last change plus check for WD8.
// 
//    Rev 1.1   13 Nov 1997 12:10:36   DCHI
// Added call to McFleaWD8MuckedUp() before returning virus found in WD8 case.
// 
//    Rev 1.0   09 Jul 1997 16:17:42   DCHI
// Initial revision.
// 
//************************************************************************

#include "gdefines.h"

#include "platform.h"
#include "callback.h"

#include "ctsn.h"
#include "navex.h"

#include "wd7api.h"
#include "o97api.h"
#include "xl5api.h"
#include "acsapi.h"

#include "olestrnm.h"
#include "wdscan15.h"
#include "offcsc15.h"
#include "o97scn15.h"
#include "wd7scn15.h"
#include "xl5scn.h"
#include "xl4scn.h"
#include "macrovid.h"

#ifdef PP_SCAN

#include "ppscan.h"

// Assume PP97 scanning is disabled

int gnPP97ScanEnabled = 0;

#endif // #ifdef PP_SCAN

//********************************************************************
//
// Function:
//  int MicrosoftOfficeScanCB()
//
// Description:
//  Looks for the following:
//      WordDocument stream -
//          Sets wScanType to OLE_SCAN_WD7
//          Sets the following fields of stWD7:
//              wWordDocumentEntry
//      Hexadecimally named stream -
//          Sets wScanType to OLE_SCAN_XL5
//          Sets the following fields of stXL5:
//              wHexNameEntry
//      Macros substorage -
//          Sets wScanType to OLE_SCAN_WD8
//          Sets the following fields of stWD8:
//              wMacrosEntry
//              wMacrosChildEntry
//      _VBA_PROJECT_CUR substorage -
//          Sets wScanType to OLE_SCAN_XL97
//          Sets the following fields of stXL97:
//              w_VBA_PROJECT_CUREntry
//              w_VBA_PROJECT_CURChildEntry
//
// Returns:
//  OLE_OPEN_CB_STATUS_RETURN       If one of the above was found
//  OLE_OPEN_CB_STATUS_CONTINUE     If none of the above were found
//
//********************************************************************

int MicrosoftOfficeScanCB
(
    LPSS_DIR_ENTRY      lpstEntry,  // Ptr to the entry
    DWORD               dwIndex,    // The entry's index in the directory
    LPVOID              lpvCookie
)
{
    LPMSOFFICE_SCAN     lpstMSOfficeScan = (LPMSOFFICE_SCAN)lpvCookie;

    if (lpstEntry->byMSE == STGTY_STREAM)
    {
        // Check for "WordDocument"

        if (SSWStrNCmp(lpstEntry->uszName,
                       (LPWORD)gabywszWordDocument,
                       SS_MAX_NAME_LEN) == 0)
        {
            lpstMSOfficeScan->wType = OLE_SCAN_WD7;
            lpstMSOfficeScan->u.stWD7.dwWordDocumentEntry = dwIndex;
            return(SS_ENUM_CB_STATUS_OPEN);
        }

        // Check for "Book"

        if (SSWStrNCmp(lpstEntry->uszName,
                       (LPWORD)gabywszBook,
                       SS_MAX_NAME_LEN) == 0)
        {
            lpstMSOfficeScan->wType = OLE_SCAN_XL4_95;
            lpstMSOfficeScan->u.stXL4.dwBookEntry = dwIndex;
            return(SS_ENUM_CB_STATUS_OPEN);
        }

        // Check for "Workbook"

        if (SSWStrNCmp(lpstEntry->uszName,
                       (LPWORD)gabywszWorkbook,
                       SS_MAX_NAME_LEN) == 0)
        {
            lpstMSOfficeScan->wType = OLE_SCAN_XL4_97;
            lpstMSOfficeScan->u.stXL4.dwBookEntry = dwIndex;
            return(SS_ENUM_CB_STATUS_OPEN);
        }

#ifdef PP_SCAN
        // Check for "PowerPoint Document"

        if (gnPP97ScanEnabled != 0 &&
            SSWStrNCmp(lpstEntry->uszName,
                       (LPWORD)gabywszPowerPointDocument,
                       SS_MAX_NAME_LEN) == 0)
        {
            lpstMSOfficeScan->wType = OLE_SCAN_PP;
            lpstMSOfficeScan->u.stPP.dwPowerPointDocumentEntry = dwIndex;
            return(SS_ENUM_CB_STATUS_OPEN);
        }
#endif // #ifdef PP_SCAN
    }
    else
    if (lpstEntry->byMSE == STGTY_STORAGE)
    {
        // Check for "_VBA_PROJECT"

        if (SSWStrNCmp(lpstEntry->uszName,
                       (LPWORD)gabywsz_VBA_PROJECT,
                       SS_MAX_NAME_LEN) == 0)
        {
            lpstMSOfficeScan->wType = OLE_SCAN_XL5;
            lpstMSOfficeScan->u.stXL5.dw_VBA_PROJECTEntry = dwIndex;
            lpstMSOfficeScan->u.stXL5.dw_VBA_PROJECTChildEntry =
                DWENDIAN(lpstEntry->dwSIDChild);
            return(SS_ENUM_CB_STATUS_RETURN);
        }

        // Check for "Macros"

        if (SSWStrNCmp(lpstEntry->uszName,
                       (LPWORD)gabywszMacros,
                       SS_MAX_NAME_LEN) == 0)
        {
            lpstMSOfficeScan->wType = OLE_SCAN_WD8;

            lpstMSOfficeScan->u.stO97.wType = VBA5_WD8;
            lpstMSOfficeScan->u.stO97.u.stWD8.dwMacrosEntry =
                dwIndex;
            lpstMSOfficeScan->u.stO97.u.stWD8.dwMacrosChildEntry =
                DWENDIAN(lpstEntry->dwSIDChild);

            return(SS_ENUM_CB_STATUS_RETURN);
        }

        // Check for "VBA_PROJECT_CUR"

        if (SSWStrNCmp(lpstEntry->uszName,
                       (LPWORD)gabywsz_VBA_PROJECT_CUR,
                       SS_MAX_NAME_LEN) == 0)
        {
            lpstMSOfficeScan->wType = OLE_SCAN_XL97;

            lpstMSOfficeScan->u.stO97.wType = VBA5_XL97;
            lpstMSOfficeScan->u.stO97.u.stXL97.dw_VBA_PROJECT_CUREntry =
                dwIndex;
            lpstMSOfficeScan->u.stO97.u.stXL97.dw_VBA_PROJECT_CURChildEntry =
                DWENDIAN(lpstEntry->dwSIDChild);

            return(SS_ENUM_CB_STATUS_RETURN);
        }
    }

    return(SS_ENUM_CB_STATUS_CONTINUE);
}


//********************************************************************
//
// Function:
//  EXTSTATUS MicrosoftOfficeScan()
//
// Description:
//  Scans a file for the presence of Microsoft Office macro viruses.
//
//  When the function returns, the following stream indices are known
//  if a virus is detected:
//      WD7:    wWordDocumentEntry
//      WD8:    wMacrosEntry
//              wMacrosChildEntry
//              wVBAEntry
//              wVBAChildEntry
//      XL97:   w_VBA_PROJECT_CUREntry
//              w_VBA_PROJECT_CURChildEntry
//              wVBAEntry
//              wVBAChildEntry
//      XL5:    wHexNameEntry
//
// Returns:
//	EXTSTATUS_OK			If no virus was found
//	EXTSTATUS_VIRUS_FOUND   If a virus was found
//
//	If a virus was found, the function sets *lpwVID to the VID
//  of the virus.
//
//********************************************************************

EXTSTATUS MicrosoftOfficeScan
(
    LPMSOFFICE_SCAN         lpstMSOfficeScan,   // Info about infection streams
    LPBYTE                  lpbyWorkBuffer,     // Work buffer >= 1400 bytes
    LPLPWD_VIRUS_SIG_INFO   lplpstVirusSigInfo  // Virus info storage on hit
)
{
    DWORD                   dwDirID;
    EXTSTATUS               extStatus;
    LPWD_VIRUS_SIG_INFO     lpstHeuVirusSigInfo;

    (void)lpbyWorkBuffer;

    lpstMSOfficeScan->bMVPApproved = TRUE;
    extStatus = EXTSTATUS_OK;
    lpstHeuVirusSigInfo = NULL;

    dwDirID = 0;
    while (SSEnumDirEntriesCB(lpstMSOfficeScan->lpstRoot,
                              MicrosoftOfficeScanCB,
                              lpstMSOfficeScan,
                              &dwDirID,
                              lpstMSOfficeScan->lpstStream) == SS_STATUS_OK)
    {
        switch (lpstMSOfficeScan->wType)
        {
            case OLE_SCAN_WD7:
            {
                if (WD7Scan(lpstMSOfficeScan,
                            lplpstVirusSigInfo) == EXTSTATUS_VIRUS_FOUND)
                    extStatus = EXTSTATUS_VIRUS_FOUND;

                if (lpstMSOfficeScan->bMVPApproved == FALSE)
                {
                    *lplpstVirusSigInfo = &gstMVPVirusSigInfo;
                    return(EXTSTATUS_VIRUS_FOUND);
                }
                break;
            }

            case OLE_SCAN_WD8:
            case OLE_SCAN_XL97:
            {
                if (VBA5Scan(lpstMSOfficeScan,
                             lplpstVirusSigInfo,
                             &lpstMSOfficeScan->bMVPApproved) == EXTSTATUS_VIRUS_FOUND)
                    extStatus = EXTSTATUS_VIRUS_FOUND;

                if (lpstMSOfficeScan->bMVPApproved == FALSE)
                {
                    *lplpstVirusSigInfo = &gstMVPVirusSigInfo;
                    return(EXTSTATUS_VIRUS_FOUND);
                }
                break;
            }

            case OLE_SCAN_XL4_95:
            case OLE_SCAN_XL4_97:
                if (XL4Scan(lpstMSOfficeScan,
                            lplpstVirusSigInfo) == EXTSTATUS_VIRUS_FOUND)
                    extStatus = EXTSTATUS_VIRUS_FOUND;

                break;

            case OLE_SCAN_XL5:
                if (XL5Scan(lpstMSOfficeScan,
                            lplpstVirusSigInfo) == EXTSTATUS_VIRUS_FOUND)
                    extStatus = EXTSTATUS_VIRUS_FOUND;

                break;

#ifdef PP_SCAN
            case OLE_SCAN_PP:
                if (PPScan(lpstMSOfficeScan,
                           FALSE,
                           lpbyWorkBuffer,
                           lplpstVirusSigInfo,
                           &lpstMSOfficeScan->bMVPApproved) == EXTSTATUS_VIRUS_FOUND)
                {
                    return(EXTSTATUS_VIRUS_FOUND);
                }
                break;
#endif // #ifdef PP_SCAN

            default:
                break;
        }

        if (extStatus == EXTSTATUS_VIRUS_FOUND)
        {
            // If a heuristic signature triggered,
            //  then don't return until we are sure
            //  there are no known viruses

            switch ((*(lplpstVirusSigInfo))->wID)
            {
                case VID_MACRO_HEURISTICS:
                case VID_W7H2:
                case VID_XH:
                    lpstHeuVirusSigInfo = *lplpstVirusSigInfo;
                    break;

                default:
                    return(EXTSTATUS_VIRUS_FOUND);
            }

            extStatus = EXTSTATUS_OK;
        }
    }

    // Did a heuristic signature trigger?

    if (lpstHeuVirusSigInfo != NULL)
    {
        *lplpstVirusSigInfo = lpstHeuVirusSigInfo;
        return(EXTSTATUS_VIRUS_FOUND);
    }

    // No virus

    return (EXTSTATUS_OK);
}


