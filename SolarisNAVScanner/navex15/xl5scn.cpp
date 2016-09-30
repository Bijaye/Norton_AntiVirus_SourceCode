//************************************************************************
//
// $Header:   S:/NAVEX/VCS/XL5SCN.CPv   1.4   09 Dec 1998 17:45:04   DCHI  $
//
// Description:
//  Contains Excel macro scan function code.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/XL5SCN.CPv  $
// 
//    Rev 1.4   09 Dec 1998 17:45:04   DCHI
// Added #include "gdefines.h"
// 
//    Rev 1.3   08 Dec 1998 12:57:10   DCHI
// Changes for MSX.
// 
//    Rev 1.2   04 Aug 1998 13:52:26   DCHI
// Changes for Excel heuristics.
// 
//    Rev 1.1   11 May 1998 17:49:50   DCHI
// Changes for Access 2.0 scanning and Access datafile conversion.
// 
//    Rev 1.0   15 Apr 1998 16:46:20   DCHI
// Initial revision.
// 
//************************************************************************

#include "gdefines.h"

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
#include "offcsc15.h"

#include "xl5scn.h"

#include "mcrsgutl.h"

//********************************************************************
//
// Function:
//  EXTSTATUS XL5Scan()
//
// Description:
//  Scans a file for the presence of Excel 5.0/95 viruses.
//
//  The function iterates through and scans each of the
//  hexadecimally named streams in the _VBA_PROJECT storage.
//
//  If the scan indicates viral code in the streams, then
//  a verification is made that the associated Book stream
//  also contains an OBPROJ (Visual Basic Property) record
//  before returning a virus found status.
//
// Returns:
//	EXTSTATUS_OK			If no virus was found
//	EXTSTATUS_VIRUS_FOUND   If a virus was found
//
//********************************************************************

EXTSTATUS XL5Scan
(
    LPMSOFFICE_SCAN         lpstOffcScan,
    LPLPWD_VIRUS_SIG_INFO   lplpstVirusSigInfo
)
{
    LPWD_SCAN               lpstScan;
    LPSS_ENUM_SIBS          lpstSibs;
    LPSS_STREAM             lpstDirStream;
    BYTE                    abypsStreamName[SS_MAX_NAME_LEN+2];
    int                     i, nLen;

    lpstScan = lpstOffcScan->lpstScan;

    // Allocate a stream structure for the dir stream

    if (SSAllocStreamStruct(lpstOffcScan->lpstRoot,
                            &lpstDirStream,
                            SS_STREAM_FLAG_DEF_BAT_CACHE) != SS_STATUS_OK)
    {
        return(EXTSTATUS_MEM_ERROR);
    }

    // Allocate a sibiling enumeration structure

    if (SSAllocEnumSibsStruct(lpstOffcScan->lpstRoot,
                              &lpstSibs,
                              SS_DEF_MAX_WAITING_SIBS) != SS_STATUS_OK)
    {
        SSFreeStreamStruct(lpstDirStream);
        return(EXTSTATUS_MEM_ERROR);
    }

    // Assign hit memory

    if (AssignMacroSigHitMem(lpstOffcScan,
                             lpstScan) == FALSE)
    {
        SSFreeStreamStruct(lpstDirStream);
        SSFreeEnumSibsStruct(lpstOffcScan->lpstRoot,lpstSibs);
        return(EXTSTATUS_MEM_ERROR);
    }

    // Initialize hit memory

    WDInitHitBitArrays(&gstXL95SigSet,lpstScan);

    // Disable full set

    lpstScan->wFlags = 0;

    // Get the dir stream

    SSInitEnumSibsStruct(lpstSibs,
                         lpstOffcScan->u.stXL5.dw_VBA_PROJECTChildEntry);

    if (SSEnumSiblingEntriesCB(lpstOffcScan->lpstRoot,
                               SSOpenStreamByNameCB,
                               gabywszdir,
                               lpstSibs,
                               lpstDirStream) != SS_STATUS_OK)
    {
        SSFreeStreamStruct(lpstDirStream);
        SSFreeEnumSibsStruct(lpstOffcScan->lpstRoot,lpstSibs);
        return(EXTSTATUS_MEM_ERROR);
    }

    // Initialize for dir access

    if (XL5DirInfoInit(lpstDirStream,
                       &lpstScan->uScan.stXL5.stDirInfo) == FALSE)
    {
        SSFreeStreamStruct(lpstDirStream);
        SSFreeEnumSibsStruct(lpstOffcScan->lpstRoot,lpstSibs);
        return(EXTSTATUS_MEM_ERROR);
    }

    // Scan the dir stream

    lpstScan->wScanType = WD_SCAN_TYPE_XL5_DIR;
    lpstScan->abyName[0] = 'd';
    lpstScan->abyName[1] = 'i';
    lpstScan->abyName[2] = 'r';
    lpstScan->abyName[3] = 0;
    lpstScan->lpstStream = lpstDirStream;

    WDScanMacro(&gstXL95SigSet,
                lpstScan);

    // Initialize other scan structure fields

    lpstScan->lpstStream = lpstOffcScan->lpstStream;
    lpstScan->wScanType = WD_SCAN_TYPE_XL5;

    // Initialize macro count

    lpstScan->wMacroCount = 0;

    // Start with the VBA child entry

    SSInitEnumSibsStruct(lpstSibs,
                         lpstOffcScan->u.stXL5.dw_VBA_PROJECTChildEntry);

    while (SSEnumSiblingEntriesCB(lpstOffcScan->lpstRoot,
                                  XL5OpenHexNamedStreamCB,
                                  abypsStreamName + 1,
                                  lpstSibs,
                                  lpstOffcScan->lpstStream) == SS_STATUS_OK)
    {
        // Check for circular links

        if (lpstScan->wMacroCount &&
            SSStreamID(lpstOffcScan->lpstStream) ==
            lpstOffcScan->u.stXL5.dw_VBA_PROJECTChildEntry)
            break;

        if (lpstScan->wMacroCount++ > 16384)
            break;

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

        // Scan the module

        WDScanMacro(&gstXL95SigSet,
                    lpstScan);
    }

    // We can free the sibling enumeration structure
    //  and the dir stream structure

    SSFreeStreamStruct(lpstDirStream);
    SSFreeEnumSibsStruct(lpstOffcScan->lpstRoot,lpstSibs);

    // Update the long scan count

    lpstOffcScan->dwLongScanCount += lpstScan->wMacroCount;


    /////////////////////////////////////////////////////////////
    // Do virus check
    /////////////////////////////////////////////////////////////

    // Apply virus signatures

    if (WDApplyVirusSignatures(&gstXL95SigSet,
                               lpstScan,
                               lplpstVirusSigInfo) == WD_STATUS_VIRUS_FOUND)
    {
        /////////////////////////////////////////////////////////
        // Now make sure that there is a VB project

        // Get the associated Book stream

        if (XL5GetVBABookStream(lpstOffcScan->lpstRoot,
                                lpstOffcScan->u.stXL5.
                                    dw_VBA_PROJECTEntry,
                                lpstOffcScan->lpstStream) != FALSE)
        {
            // Make sure there is a VB project record

            if (XLFindFirstGlobalRec(lpstOffcScan->lpstStream,
                                     eXLREC_OBPROJ,
                                     NULL,
                                     NULL) != FALSE)
            {
                return(EXTSTATUS_VIRUS_FOUND);
            }
        }
    }

    // No virus

    return (EXTSTATUS_OK);
}



