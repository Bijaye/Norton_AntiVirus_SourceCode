// Copyright 1998 Symantec, SABU
//************************************************************************
//
// $Header:   S:/NAVEX/VCS/a97scn.cpv   1.3   27 Oct 1998 15:17:54   DCHI  $
//
// Description:
//  Contains Access 95/97 macro scan function code.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/a97scn.cpv  $
// 
//    Rev 1.3   27 Oct 1998 15:17:54   DCHI
// Setup scan type value before call to WDApplyVirusSignatures()
// necessary in the case where the file does not have macros/modules.
// 
//    Rev 1.2   01 Sep 1998 11:41:52   AOONWAL
// Updated from SNDBOT after the corruption in USSM-SARC\VOL1
// 
//    Rev 1.2   28 Aug 1998 17:41:08   DCHI
// Fixed hit bit initialization.  Was incorrectly A2.  Changed to A97.
// 
//    Rev 1.1   11 May 1998 18:15:20   DCHI
// Changes for Access 2.0 scanning and Access datafile conversion.
// 
//    Rev 1.0   17 Apr 1998 13:42:50   DCHI
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

#include "a97scn.h"

#include "wdscan15.h"
#include "offcsc15.h"

#include "mcrsgutl.h"

BYTE gabyA97_VBA_MODULE[0x18] =
{
    '_', 0, 'V', 0, 'B', 0, 'A', 0, '_', 0,
    'M', 0, 'O', 0, 'D', 0, 'U', 0, 'L', 0, 'E', 0, 0, 0
};

//********************************************************************
//
// Function:
//  BOOL A97ScanVBAModuleInit()
//
// Parameters:
//  lpstObj                 Ptr to object access structure
//  lpdwModuleOffset        Ptr to DWORD for module begin offset
//
// Description:
//  Searches for the VBA module stream within the object by
//  searching for the object within the item having a name
//  of _VBA_MODULE.
//
// Returns:
//  TRUE                    Always
//
//********************************************************************

BOOL A97ScanVBAModuleInit
(
    LPA97_OBJ_ACCESS    lpstObj,
    LPDWORD             lpdwModuleOffset
)
{
    DWORD               dwOffset;
    DWORD               dwByteCount;
    DWORD               dwNumBytes;
    int                 i;
    int                 nTimeOut;
    BYTE                abyBuf[sizeof(gabyA97_VBA_MODULE)];

    /////////////////////////////////////////////////////////////
    // Search for the VBA module

    dwOffset = 8;
    nTimeOut = 0;
    while (dwOffset < lpstObj->dwSize && nTimeOut++ < 1024)
    {
        // Get the length of the name

        if (A97ObjAccessRead(lpstObj,
                             dwOffset,
                             abyBuf,
                             sizeof(BYTE),
                             &dwByteCount) == FALSE ||
            dwByteCount != sizeof(BYTE))
            return(FALSE);

        ++dwOffset;

        // Does the name length match?

        dwNumBytes = abyBuf[0];
        if (dwNumBytes == sizeof(gabyA97_VBA_MODULE))
        {
            // Try it

            if (A97ObjAccessRead(lpstObj,
                                 dwOffset,
                                 abyBuf,
                                 dwNumBytes,
                                 &dwByteCount) == FALSE ||
                dwByteCount != dwNumBytes)
                return(FALSE);

            // Compare the name

            for (i=0;i<sizeof(gabyA97_VBA_MODULE);i++)
                if (abyBuf[i] != gabyA97_VBA_MODULE[i])
                    break;

            if (i == sizeof(gabyA97_VBA_MODULE))
            {
                dwOffset += dwNumBytes;
                break;
            }
        }

        dwOffset += dwNumBytes;

        // Get the length of the body

        if (A97ObjAccessRead(lpstObj,
                             dwOffset,
                             &dwNumBytes,
                             sizeof(DWORD),
                             &dwByteCount) == FALSE ||
            dwByteCount != sizeof(DWORD))
            return(FALSE);

        dwNumBytes = DWENDIAN(dwNumBytes);

        dwOffset += sizeof(DWORD) + dwNumBytes;
    }

    if (dwOffset >= lpstObj->dwSize || nTimeOut >= 1024)
        return(FALSE);

    // Get the length of the body

    if (A97ObjAccessRead(lpstObj,
                         dwOffset,
                         &dwNumBytes,
                         sizeof(DWORD),
                         &dwByteCount) == FALSE ||
        dwByteCount != sizeof(DWORD))
        return(FALSE);

    dwNumBytes = DWENDIAN(dwNumBytes);
    dwOffset += sizeof(DWORD);

    // Set the length to end right after the end of the module

    lpstObj->dwSize = dwOffset + dwNumBytes;

    *lpdwModuleOffset = dwOffset;

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL A97ScanVBAModuleEnumCB()
//
// Parameters:
//  lpstA97                 Ptr to A97_T structure
//  lpvVBAEnumCookie        Ptr to LPWD_SCAN structure
//  lpabypsszModuleName     Module name
//  dwSize                  Size of module
//  dwPage                  First page of module
//  nIndex                  First item of module
//
// Description:
//  Scans a module for the presence of Access 97 viruses.
//
//  If the module body can not be scanned, its size is set
//  to zero and only the name is scanned.
//
// Returns:
//  TRUE                    Always
//
//********************************************************************

BOOL A97ScanVBAModuleEnumCB
(
    LPA97               lpstA97,
    LPVOID              lpvVBAEnumCookie,
    LPBYTE              lpabypsszModuleName,
    DWORD               dwSize,
    DWORD               dwPage,
    int                 nIndex
)
{
    LPWD_SCAN           lpstScan;
    LPA97_OBJ_ACCESS    lpstObj;
    int                 i;
    DWORD               dwModuleOffset;

    lpstScan = (LPWD_SCAN)lpvVBAEnumCookie;

    lpstScan->wMacroCount++;

    // Copy the name

    for (i=1;i<=lpabypsszModuleName[0];i++)
        lpstScan->abyName[i-1] = SSToUpper(lpabypsszModuleName[i]);

    // Zero-terminate the name

    lpstScan->abyName[lpabypsszModuleName[0]] = 0;

    // Initialize for access to the object

    lpstObj = &lpstScan->uScan.lpstA97->stObj;
    if (A97ObjAccessInit(lpstA97,
                         dwSize,
                         dwPage,
                         nIndex,
                         lpstObj) == FALSE)
    {
        lpstScan->uScan.lpstA97->stObj.dwSize = 0;
        lpstScan->wScanType = WD_SCAN_TYPE_A97_RAW;
    }
    else
    {
        if (A97ScanVBAModuleInit(lpstObj,
                                 &dwModuleOffset) == FALSE ||
            A97LZNTModuleInit(lpstObj,
                              dwModuleOffset,
                              &lpstScan->uScan.lpstA97->stLZNT) == FALSE)
        {
            // Just scan the whole item

            lpstScan->wScanType = WD_SCAN_TYPE_A97_RAW;
        }
        else
        {
            lpstScan->wScanType = WD_SCAN_TYPE_A97_VBMOD;
        }
    }

    // Scan the macro module

    WDScanMacro(&gstA97SigSet,lpstScan);

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL A97ScanMacroEnumCB()
//
// Parameters:
//  lpstA97                 Ptr to A97_T structure
//  lpvMacroEnumCookie      Ptr to LPWD_SCAN structure
//  lpabypsszModuleName     Module name
//  dwSize                  Size of module
//  dwPage                  First page of module
//  nIndex                  First item of module
//
// Description:
//  Scans a module for the presence of Access 97 viruses.
//
//  If the macro body can not be scanned, its size is set
//  to zero and only the name is scanned.
//
// Returns:
//  TRUE                    Always
//
//********************************************************************

BOOL A97ScanMacroEnumCB
(
    LPA97               lpstA97,
    LPVOID              lpvMacroEnumCookie,
    LPBYTE              lpabypsszMacroName,
    DWORD               dwSize,
    DWORD               dwPage,
    int                 nIndex
)
{
    LPWD_SCAN           lpstScan;
    int                 i;
    BYTE                by0;
    WORD                wSize;
    DWORD               dwByteCount;

    lpstScan = (LPWD_SCAN)lpvMacroEnumCookie;

    lpstScan->wMacroCount++;

    // Copy the name

    for (i=1;i<=lpabypsszMacroName[0];i++)
        lpstScan->abyName[i-1] = SSToUpper(lpabypsszMacroName[i]);

    // Zero-terminate the name

    lpstScan->abyName[lpabypsszMacroName[0]] = 0;

    // Initialize for access to the object
    //  and search for the macro beginning

    if (A97ObjAccessInit(lpstA97,
                         dwSize,
                         dwPage,
                         nIndex,
                         &lpstScan->uScan.lpstA97->stObj) == FALSE ||
        A97ObjAccessRead(&lpstScan->uScan.lpstA97->stObj,
                         0,
                         &by0,
                         sizeof(BYTE),
                         &dwByteCount) == FALSE ||
        dwByteCount != sizeof(BYTE) ||
        A97ObjAccessRead(&lpstScan->uScan.lpstA97->stObj,
                         A97MACRO_HDR_ARG_COUNT_OFFSET +
                             ((by0 != 0xFF) ? 4 : 0),
                         &wSize,
                         sizeof(WORD),
                         &dwByteCount) == FALSE ||
        dwByteCount != sizeof(WORD))
    {
        // Set size to zero

        lpstScan->uScan.lpstA97->dwStartOffset = dwSize;
    }
    else
    {
        wSize = WENDIAN(wSize);

        lpstScan->uScan.lpstA97->dwStartOffset =
            A97MACRO_HDR_ARG_COUNT_OFFSET + ((by0 != 0xFF) ? 4 : 0) +
            sizeof(WORD) + wSize;

        if (lpstScan->uScan.lpstA97->dwStartOffset > dwSize)
            return(TRUE);
    }

    // Set the scan type

    lpstScan->wScanType = WD_SCAN_TYPE_A97_MACRO;

    // Scan the macro

    WDScanMacro(&gstA97SigSet,lpstScan);

    return(TRUE);
}


//********************************************************************
//
// Function:
//  EXTSTATUS EXTScanFileAccess97()
//
// Description:
//  Scans a file for the presence of Access 95/97 viruses.
//
// Returns:
//	EXTSTATUS_OK			If no virus was found
//	EXTSTATUS_VIRUS_FOUND   If a virus was found
//
//	If a virus was found, the function sets *lpwVID to the VID
//	of the virus.
//
//********************************************************************

EXTSTATUS EXTScanFileAccess97
(
    LPCALLBACKREV1      lpstCallBack,       // File op callbacks
    HFILE               hFile,              // Handle to file to scan
    LPBYTE              lpbyWorkBuffer,     // Work buffer >= 2048 bytes
    LPWORD              lpwVID              // Virus ID storage on hit
)
{
    EXTSTATUS           extStatus = EXTSTATUS_OK;
    LPA97_SCAN_INFO     lpstScan;
    MSOFFICE_SCAN_T     stMSOfficeScan;
    LPWD_VIRUS_SIG_INFO lpstVirusSigInfo;

    (void)lpbyWorkBuffer;

    if (ACSMemoryAlloc(lpstCallBack,
                       sizeof(A97_SCAN_INFO_T),
                       (LPVOID FAR *)&lpstScan) == FALSE)
        return(EXTSTATUS_OK);

    lpstScan->stA97.lpvRootCookie = lpstCallBack;
    lpstScan->stA97.lpvFile = (LPVOID)hFile;

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

    stMSOfficeScan.wType = OLE_SCAN_A97;

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

    WDInitHitBitArrays(&gstA97SigSet,stMSOfficeScan.lpstScan);

    // Initialize macro count

    stMSOfficeScan.lpstScan->wMacroCount = 0;

    // Disable full set

    stMSOfficeScan.lpstScan->wFlags = 0;

    // Initialize Access pointers

    stMSOfficeScan.lpstScan->uScan.lpstA97 = lpstScan;

    // Scan the 95 modules

    A97EnumVBAModules(&lpstScan->stA97,
                      TRUE,
                      A97ScanVBAModuleEnumCB,
                      stMSOfficeScan.lpstScan);

    // Scan the 97 modules

    A97EnumVBAModules(&lpstScan->stA97,
                      FALSE,
                      A97ScanVBAModuleEnumCB,
                      stMSOfficeScan.lpstScan);

    // Scan the macros

    A97EnumMacros(&lpstScan->stA97,
                  A97ScanMacroEnumCB,
                  stMSOfficeScan.lpstScan);

    /////////////////////////////////////////////////////////////
    // Do virus check
    /////////////////////////////////////////////////////////////

    // Apply virus signatures

    // The following assignment is necessary for NLM progress
    //  callback to use the right root cookie (e.g., callback ptr)

    stMSOfficeScan.lpstScan->wScanType = WD_SCAN_TYPE_A97_MACRO;
    if (WDApplyVirusSignatures(&gstA97SigSet,
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

