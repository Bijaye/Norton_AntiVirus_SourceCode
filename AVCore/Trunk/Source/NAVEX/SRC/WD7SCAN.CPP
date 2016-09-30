//************************************************************************
//
// $Header:   S:/NAVEX/VCS/wd7scan.cpv   1.1   14 Mar 1997 16:34:54   DCHI  $
//
// Description:
//      Contains Word 6.0/7.0 macro scan function code.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/wd7scan.cpv  $
// 
//    Rev 1.1   14 Mar 1997 16:34:54   DCHI
// Added support for Office 97 repair.
// 
//    Rev 1.0   13 Feb 1997 13:37:06   DCHI
// Initial revision.
// 
//************************************************************************

#include "storobj.h"
#include "wdencdoc.h"
#include "worddoc.h"

#include "ctsn.h"
#include "navex.h"

#include "endutils.h"

#include "wddecsig.h"
#include "wdscan.h"
#include "wd7scan.h"
#include "wd7sigs.h"

//********************************************************************
//
// Function:
//  EXTSTATUS WD7Scan()
//
// Description:
//  Scans a file for the presence of Word 6.0/7.0 Macro viruses.
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
    LPCALLBACKREV1          lpstCallBack,       // File op callbacks
    LPOLE_FILE              lpstOLEFile,        // WordDocument stream
    LPBYTE                  lpbyWorkBuffer,     // Work buffer >= 870 bytes
    LPLPWD_VIRUS_SIG_INFO   lplpstVirusSigInfo  // Virus info storage on hit
)
{
    WORD                wReturn = EXTSTATUS_OK;
    MACRO_TABLE_INFO_T  stMacroTableInfo;
    LPWD_SCAN           lpstScan;
    WDENCKEY_T          stKey;

    BYTE abyNameSigInterMacroHit[WD7_NAME_SIG_INTER_MACRO_HIT_ARRAY_SIZE];
    BYTE abyNameSigIntraMacroHit[WD7_NAME_SIG_INTRA_MACRO_HIT_ARRAY_SIZE];
    BYTE abyNamedCRCSigHit[WD7_NAMED_CRC_SIG_HIT_ARRAY_SIZE];
    BYTE abyMacroSigHit[WD7_MACRO_SIG_HIT_ARRAY_SIZE];
    BYTE abyCRCHit[WD7_CRC_HIT_ARRAY_SIZE];

    lpstScan = (LPWD_SCAN)lpbyWorkBuffer;

	// Verify that it is a Word file and that it has macros

    if (WordDocFindFirstKey(lpstCallBack,
                            lpstOLEFile,
                            lpbyWorkBuffer,
                            &stKey,
                            &gstRevKeyLocker) != WORDDOC_OK)
    {
        // Failure to find key or determining whether it
        //  was encrypted

        return(wReturn);
    }

    while (1)
    {
        if (WordDocContainsMacros(lpstCallBack,
                                  lpstOLEFile,
                                  &stKey,
                                  lpbyWorkBuffer,
                                  &stMacroTableInfo) == 0)
            break;

        lpstScan->lpabyNameSigInterMacroHit = abyNameSigInterMacroHit;
        lpstScan->lpabyNameSigIntraMacroHit = abyNameSigIntraMacroHit;
        lpstScan->lpabyNamedCRCSigHit = abyNamedCRCSigHit;
        lpstScan->lpabyMacroSigHit = abyMacroSigHit;
        lpstScan->lpabyCRCHit = abyCRCHit;

        lpstScan->lpstCallBack = lpstCallBack;
        lpstScan->lpstOLEStream = lpstOLEFile;
        lpstScan->wScanType = WD_SCAN_TYPE_WD7;
        lpstScan->uScan.stWD7.lpstMacroTableInfo = &stMacroTableInfo;
        lpstScan->uScan.stWD7.lpstKey = &stKey;
        if (WDScanDoc(&gstWD7SigSet,
                      lpstScan,
                      lplpstVirusSigInfo) == WD_STATUS_VIRUS_FOUND)
        {
            return(EXTSTATUS_VIRUS_FOUND);
        }

        if (stKey.bEncrypted == FALSE)
            break;

        if (WordDocFindNextKey(lpstCallBack,
                               lpstOLEFile,
                               lpbyWorkBuffer,
                               &stKey,
                               &gstRevKeyLocker) != WORDDOC_OK)
            break;
	}

	// No virus

	return (wReturn);
}

