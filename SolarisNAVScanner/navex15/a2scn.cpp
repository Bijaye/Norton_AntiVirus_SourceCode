// Copyright 1998 Symantec, SABU
//************************************************************************
//
// $Header:   S:/NAVEX/VCS/a2scn.cpv   1.1   27 Oct 1998 15:26:36   DCHI  $
//
// Description:
//  Contains Access 2.0 macro scan function code.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/a2scn.cpv  $
// 
//    Rev 1.1   27 Oct 1998 15:26:36   DCHI
// Setup scan type value before call to WDApplyVirusSignatures()
// necessary in the case where the file does not have macros/modules.
// 
//    Rev 1.0   11 May 1998 17:45:28   DCHI
// Initial revision.
// 
//************************************************************************

#include "platform.h"

#include "callback.h"
#include "ctsn.h"
#include "navex.h"

#include "wd7api.h"
#include "o97api.h"
#include "xl5api.h"
#include "acsapi.h"

#include "a2scn.h"

#include "wdscan15.h"
#include "offcsc15.h"

#include "mcrsgutl.h"

//********************************************************************
//
// Function:
//  BOOL A2ScanVBModuleEnumCB()
//
// Parameters:
//  lpstA2                  Ptr to A2_T structure
//  lpvObjectEnumCookie     Ptr to LPWD_SCAN structure
//  lpabypsszModuleName     Module name
//  dwSize                  Size of module
//  dwPage                  First page of module
//  nIndex                  First item of module
//
// Description:
//  Scans a module for the presence of Access 2.0 viruses.
//
//  If the module body can not be scanned, its size is set
//  to zero and only the name is scanned.
//
// Returns:
//  TRUE                    Always
//
//********************************************************************

BOOL A2ScanVBModuleEnumCB
(
    LPA2                lpstA2,
    LPVOID              lpvObjectEnumCookie,
    LPBYTE              lpabypsszModuleName,
    DWORD               dwSize,
    DWORD               dwPage,
    int                 nIndex
)
{
    LPWD_SCAN           lpstScan;
    int                 i;

    lpstScan = (LPWD_SCAN)lpvObjectEnumCookie;

    lpstScan->wMacroCount++;

    // Copy the name

    for (i=1;i<=lpabypsszModuleName[0];i++)
        lpstScan->abyName[i-1] = SSToUpper(lpabypsszModuleName[i]);

    // Zero-terminate the name

    lpstScan->abyName[lpabypsszModuleName[0]] = 0;

    // Initialize for access to the object

    if (A2ObjAccessInit(lpstA2,
                        dwSize,
                        dwPage,
                        nIndex,
                        &lpstScan->uScan.lpstA2->stObj) == FALSE)
        lpstScan->uScan.lpstA2->stObj.dwSize = 0;

    // Scan the macro module

    WDScanMacro(&gstA2SigSet,lpstScan);

    return(TRUE);
}


//********************************************************************
//
// Function:
//  EXTSTATUS EXTScanFileAccess2()
//
// Description:
//  Scans a file for the presence of Access 2.0 viruses.
//
// Returns:
//	EXTSTATUS_OK			If no virus was found
//	EXTSTATUS_VIRUS_FOUND   If a virus was found
//
//	If a virus was found, the function sets *lpwVID to the VID
//	of the virus.
//
//********************************************************************

EXTSTATUS EXTScanFileAccess2
(
    LPCALLBACKREV1      lpstCallBack,       // File op callbacks
    HFILE               hFile,              // Handle to file to scan
    LPBYTE              lpbyWorkBuffer,     // Work buffer >= 2048 bytes
    LPWORD              lpwVID              // Virus ID storage on hit
)
{
    EXTSTATUS           extStatus = EXTSTATUS_OK;
    LPA2_SCAN_INFO      lpstScan;
    MSOFFICE_SCAN_T     stMSOfficeScan;
    LPWD_VIRUS_SIG_INFO lpstVirusSigInfo;

    (void)lpbyWorkBuffer;

    if (ACSMemoryAlloc(lpstCallBack,
                       sizeof(A2_SCAN_INFO_T),
                       (LPVOID FAR *)&lpstScan) == FALSE)
        return(EXTSTATUS_OK);

    lpstScan->stA2.lpvRootCookie = lpstCallBack;
    lpstScan->stA2.lpvFile = (LPVOID)hFile;

    // Allocate hit memory

    if (AllocMacroSigHitMem(lpstCallBack,
                            &stMSOfficeScan) == FALSE)
    {
        // Release allocated memory

        ACSMemoryFree(lpstCallBack,lpstScan);

        // Failed to allocate hit memory

        return(EXTSTATUS_MEM_ERROR);
    }

    // Allocate scan memory

    if (WDAllocScanStruct(lpstCallBack,
                          &stMSOfficeScan.lpstScan,
                          DEF_WD_RUN_BUF_SIZE,
                          DEF_WD_EXTRA_BUF_SIZE) != WD_STATUS_OK)
    {
        // Release allocated memory

        FreeMacroSigHitMem(lpstCallBack,&stMSOfficeScan);
        ACSMemoryFree(lpstCallBack,lpstScan);

        // Failed to create stream structure

        return(EXTSTATUS_MEM_ERROR);
    }

    // Set type

    stMSOfficeScan.wType = OLE_SCAN_A2;

    // Assign hit memory

    if (AssignMacroSigHitMem(&stMSOfficeScan,
                             stMSOfficeScan.lpstScan) == FALSE)
    {
        WDFreeScanStruct(lpstCallBack,stMSOfficeScan.lpstScan);
        FreeMacroSigHitMem(lpstCallBack,&stMSOfficeScan);
        ACSMemoryFree(lpstCallBack,lpstScan);
        return(EXTSTATUS_MEM_ERROR);
    }

    // Initialize hit memory

    WDInitHitBitArrays(&gstA2SigSet,stMSOfficeScan.lpstScan);

    // Initialize other scan structure fields

    stMSOfficeScan.lpstScan->wScanType = WD_SCAN_TYPE_A2;

    // Initialize macro count

    stMSOfficeScan.lpstScan->wMacroCount = 0;

    // Disable full set

    stMSOfficeScan.lpstScan->wFlags = 0;

    // Initialize Access pointers

    stMSOfficeScan.lpstScan->uScan.lpstA2 = lpstScan;

    // Scan the modules

    A2EnumVBModules(&lpstScan->stA2,
                    A2ScanVBModuleEnumCB,
                    stMSOfficeScan.lpstScan);

    // Scan the macros

    WDScanA2Macros(&gstA2SigSet,
                   stMSOfficeScan.lpstScan);

    /////////////////////////////////////////////////////////////
    // Do virus check
    /////////////////////////////////////////////////////////////

    // Apply virus signatures

    // The following assignment is necessary for NLM progress
    //  callback to use the right root cookie (e.g., callback ptr)

    stMSOfficeScan.lpstScan->wScanType = WD_SCAN_TYPE_A2;
    if (WDApplyVirusSignatures(&gstA2SigSet,
                               stMSOfficeScan.lpstScan,
                               &lpstVirusSigInfo) == WD_STATUS_VIRUS_FOUND)
    {
        extStatus = EXTSTATUS_VIRUS_FOUND;
        *lpwVID = lpstVirusSigInfo->wID;
    }

    WDFreeScanStruct(lpstCallBack,stMSOfficeScan.lpstScan);
    FreeMacroSigHitMem(lpstCallBack,&stMSOfficeScan);
    ACSMemoryFree(lpstCallBack,lpstScan);

    return(extStatus);
}

