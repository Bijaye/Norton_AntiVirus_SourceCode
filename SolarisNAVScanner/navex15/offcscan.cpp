//************************************************************************
//
// $Header:   S:/NAVEX/VCS/offcscan.cpv   1.1   07 Apr 1997 18:46:10   DCHI  $
//
// Description:
//      Contains Microsoft Office macro scan function code.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/offcscan.cpv  $
// 
//    Rev 1.1   07 Apr 1997 18:46:10   DCHI
// Added code to speed up excel scanning by being more selective.
// 
//    Rev 1.0   14 Mar 1997 16:34:16   DCHI
// Initial revision.
// 
//************************************************************************

#include "platform.h"
#include "callback.h"

#include "ctsn.h"
#include "navex.h"

#include "storobj.h"
#include "wdencdoc.h"
#include "worddoc.h"

#include "endutils.h"

#include "wdscan.h"
#include "wd7scan.h"
#include "vba5scan.h"

#include "offcscan.h"

#include "xl5scan.h"

//********************************************************************
//
// Function:
//  int OLEOpenCB()
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

int OLEOpenCB
(
    LPOLE_DIR_ENTRY     lpstEntry,  // Ptr to the entry
    DWORD               dwIndex,    // The entry's index in the directory
    LPVOID              lpvCookie
)
{
    int                 i;
    WORD                w;
    LPMSOFFICE_SCAN     lpstMSOfficeScan = (LPMSOFFICE_SCAN)lpvCookie;

    if (lpstEntry->byMSE == STGTY_STREAM)
    {
        // Check for "WordDocument"

        if (WStrNCmp(lpstEntry->uszName,
                     (LPWORD)gpuszWordDocument,
                     MAX_OLE_NAME_LEN) == 0)
        {
            lpstMSOfficeScan->wType = OLE_SCAN_WD7;
            lpstMSOfficeScan->u.stWD7.wWordDocumentEntry = (WORD)dwIndex;
            return(OLE_OPEN_CB_STATUS_RETURN);
        }

        // Check for hexadecimal named stream

        for (i=0;i<MAX_OLE_NAME_LEN;i++)
        {
            w = ConvertEndianShort(lpstEntry->uszName[i]);
            if (w == 0)
                break;

            if (!('0' <= w && w <= '9' || 'a' <= w && w <= 'f'))
                return(OLE_OPEN_CB_STATUS_CONTINUE);
        }

        if (i > 4)
        {
            // Found a hexadecimally named stream

            lpstMSOfficeScan->wType = OLE_SCAN_XL5;
            lpstMSOfficeScan->u.stXL5.wHexNameEntry = (WORD)dwIndex;
            return(OLE_OPEN_CB_STATUS_RETURN);
        }
    }
    else
    if (lpstEntry->byMSE == STGTY_STORAGE)
    {
        // Check for "Macros"

        if (WStrNCmp(lpstEntry->uszName,
                     (LPWORD)gauszMacros,
                     MAX_OLE_NAME_LEN) == 0)
        {
            lpstMSOfficeScan->wType = OLE_SCAN_WD8;
            lpstMSOfficeScan->u.stVBA5.wType = VBA5_WD8;
            lpstMSOfficeScan->u.stVBA5.u.stWD8.wMacrosEntry =
                (WORD)dwIndex;
            lpstMSOfficeScan->u.stVBA5.u.stWD8.wMacrosChildEntry =
                (WORD)ConvertEndianLong(lpstEntry->dwSIDChild);
            return(OLE_OPEN_CB_STATUS_RETURN);
        }

        // Check for "VBA_PROJECT_CUR"

        if (WStrNCmp(lpstEntry->uszName,
                     (LPWORD)gausz_VBA_PROJECT_CUR,
                     MAX_OLE_NAME_LEN) == 0)
        {
            lpstMSOfficeScan->wType = OLE_SCAN_XL97;
            lpstMSOfficeScan->u.stVBA5.wType = VBA5_XL97;
            lpstMSOfficeScan->u.stVBA5.u.stXL97.w_VBA_PROJECT_CUREntry =
                (WORD)dwIndex;
            lpstMSOfficeScan->u.stVBA5.u.stXL97.w_VBA_PROJECT_CURChildEntry =
                (WORD)ConvertEndianLong(lpstEntry->dwSIDChild);
            return(OLE_OPEN_CB_STATUS_RETURN);
        }
    }

    return(OLE_OPEN_CB_STATUS_CONTINUE);
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
    LPCALLBACKREV1          lpstCallBack,       // File op callbacks
    LPOLE_FILE              lpstOLEFile,        // OLE file structure
    LPBYTE                  lpbyWorkBuffer,     // Work buffer >= 1400 bytes
    LPMSOFFICE_SCAN         lpstMSOfficeScan,   // Info about infection streams
    LPLPWD_VIRUS_SIG_INFO   lplpstVirusSigInfo  // Virus info storage on hit
)
{
    WORD                wMagicNum;
    LPBYTE              lpbyStreamBATCache;

    lpbyStreamBATCache = lpbyWorkBuffer;
    lpbyWorkBuffer += 512;

	// Verify that it is a Word file and that it has macros

	wMagicNum = 0;
    while (OLEOpenStreamCB(lpstCallBack,
                           lpstOLEFile->hFile,
                           lpstOLEFile,
                           OLEOpenCB,
                           (LPVOID)lpstMSOfficeScan,
                           lpbyStreamBATCache,
                           &wMagicNum) == OLE_OK)
    {
        switch (lpstMSOfficeScan->wType)
        {
            case OLE_SCAN_WD7:
            {
                if (WD7Scan(lpstCallBack,
                            lpstOLEFile,
                            lpbyWorkBuffer,
                            lplpstVirusSigInfo) == EXTSTATUS_VIRUS_FOUND)
                {
                    return(EXTSTATUS_VIRUS_FOUND);
                }
                break;
            }

            case OLE_SCAN_WD8:
            {
                if (WD8Scan(lpstCallBack,
                            lpstOLEFile,
                            lpbyStreamBATCache,
                            &lpstMSOfficeScan->u.stVBA5,
                            lpbyWorkBuffer,
                            lplpstVirusSigInfo) == EXTSTATUS_VIRUS_FOUND)
                {
                    return(EXTSTATUS_VIRUS_FOUND);
                }
                break;
            }

            case OLE_SCAN_XL97:
                if (XL97Scan(lpstCallBack,
                             lpstOLEFile,
                             lpbyStreamBATCache,
                             &lpstMSOfficeScan->u.stVBA5,
                             lpbyWorkBuffer,
                             lplpstVirusSigInfo) == EXTSTATUS_VIRUS_FOUND)
                {
                    return(EXTSTATUS_VIRUS_FOUND);
                }
                break;

            case OLE_SCAN_XL5:
                if (XL5Scan(lpstCallBack,
                            lpstOLEFile,
                            lpbyWorkBuffer,
                            &lpstMSOfficeScan->u.stXL5.wVID) ==
                    EXTSTATUS_VIRUS_FOUND)
                {
                    return(EXTSTATUS_VIRUS_FOUND);
                }
                break;

            default:
                break;
        }
    }

	// No virus

    return (EXTSTATUS_OK);
}


