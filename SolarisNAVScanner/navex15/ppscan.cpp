//************************************************************************
//
// $Header:   S:/NAVEX/VCS/PPSCAN.CPv   1.2   15 Dec 1998 12:11:56   DCHI  $
//
// Description:
//      Contains PowerPoint macro scan function code.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/PPSCAN.CPv  $
// 
//    Rev 1.2   15 Dec 1998 12:11:56   DCHI
// #ifdefd out repair and changed to use shared GetTempFile().
// 
//    Rev 1.1   11 May 1998 17:53:34   DCHI
// Changes for Access 2.0 scanning and Access datafile conversion.
// 
//    Rev 1.0   26 Nov 1997 18:37:30   DCHI
// Initial revision.
// 
//************************************************************************

#include "gdefines.h"

#ifdef PP_SCAN

#include "platform.h"
#include "callback.h"
#include "ctsn.h"
#include "navex.h"
#include "navexshr.h"

#include "wd7api.h"
#include "o97api.h"
#include "xl5api.h"
#include "acsapi.h"
#include "ppapi.h"
#include "df.h"

#include "wdscan15.h"
#include "wdrepair.h"
#include "offcsc15.h"
#include "o97scn15.h"
#include "mcrsgutl.h"
#include "ppscan.h"

#if 0
//********************************************************************
//
// Function:
//  BOOL PPRepairModule()
//
// Description:
//  Repairs a module of a PowerPoint virus.  If the module should
//  be deleted, then *lpbDelete is set to TRUE before returning.
//
// Returns:
//  TRUE        If the repair was successful
//  FALSE       If the repair was unsuccessful
//
//********************************************************************

BOOL PPRepairModule
(
    LPMSOFFICE_SCAN     lpstOffcScan,
    WORD                wModuleCount,       // Count of modules
    LPBYTE              abyModuleName,      // Zero terminated module name
    LPBOOL              lpbDelete,          // Ptr to BOOL to request delete
    LPWD_VIRUS_SIG_INFO lpstVirusSigInfo    // Virus sig info
)
{
    int                 i;
    LPWD_SCAN           lpstScan;

    lpstScan = lpstOffcScan->lpstScan;

    /////////////////////////////////////////////////////////////
    // Initialize the scan structure
    /////////////////////////////////////////////////////////////

    // Assign hit memory

    if (AssignMacroSigHitMem(lpstOffcScan,
                             lpstScan) == FALSE)
    {
        return(FALSE);
    }

    lpstScan->wMacroCount = wModuleCount;

    lpstScan->lpstStream = lpstOffcScan->lpstStream;
    lpstScan->wScanType = WD_SCAN_TYPE_VBA5;

    // Copy the name

    i = 0;
    while (1)
    {
        lpstScan->abyName[i] = abyModuleName[i];
        if (abyModuleName[i++] == 0)
            break;
    }

    WDInitHitBitArrays(&gstXL97SigSet,lpstOffcScan->lpstScan);

    /////////////////////////////////////////////////////////////
    // Scan and apply repair to module
    /////////////////////////////////////////////////////////////

    // Scan module

    VBA5ScanModule(&gstXL97SigSet,lpstScan);

    // Apply repair signature

    *lpbDelete = FALSE;
/*
    if (MVPCheck(MVP_XL97,
                 lpstScan->abyName,
                 lpstScan->dwCRC) == FALSE &&
        O97LZNTStreamAttributeOnly(lpstLZNT) == FALSE)
    {
        // Found a non-approved macro

        *lpbDelete = TRUE;
    }
    else
    if (lpstVirusSigInfo->wID != VID_MVP)
    {
*/
        // Apply repair signature

        if (WDApplyRepair(&gstXL97SigSet,
                          lpstScan,
                          lpbDelete,
                          lpstVirusSigInfo -
                              gstXL97SigSet.lpastVirusSigInfo,
                          0) ==
            WD_STATUS_ERROR)
        {
            // Error applying repair signature

            return(FALSE);
        }
//    }

    return(TRUE);
}


//********************************************************************
//
// Function:
//  EXTSTATUS PPVBARepair()
//
// Parameters:
//  lpstOffcScan            Ptr to office scan structure
//  lpbyWorkBuffer          Work buffer
//  lpstVirusSigInfo        Virus signature
//  lpbEmpty                Ptr to BOOL to store empty result
//
// Description:
//  Repairs a VBA file of PowerPoint macro viruses.
//
//  If after repair, the macros storage is empty, then
//  *lpbEmpty is set to TRUE.
//
// Returns:
//  EXTSTATUS_OK            If the repair was successful
//  EXTSTATUS_NO_REPAIR     If the repair was unsuccessful
//
//********************************************************************

EXTSTATUS PPVBARepair
(
    LPMSOFFICE_SCAN     lpstOffcScan,
    LPBYTE              lpbyWorkBuffer,
    LPWD_VIRUS_SIG_INFO lpstVirusSigInfo,
    LPBOOL              lpbEmpty
)
{
    BYTE                abyModuleName[SS_MAX_NAME_LEN+1];
    WORD                wModuleCount;

    BOOL                bDelete;
    BOOL                bMore;
    LPSS_ENUM_SIBS      lpstSibs;
    LPWD_SCAN           lpstScan;

    lpstScan = lpstOffcScan->lpstScan;

    if (O97VBAFindAssociatedStreams(lpstOffcScan->lpstRoot,
                                    &lpstOffcScan->u.stO97) == FALSE)
    {
        // Could not find associated streams

        return(EXTSTATUS_NO_REPAIR);
    }

    // Allocate LZNT structure

    if (LZNTAllocStruct(lpstOffcScan->lpstRoot->lpvRootCookie,
                        &lpstScan->uScan.stVBA5.lpstLZNT) != TRUE)
    {
        return(EXTSTATUS_NO_REPAIR);
    }

    // Count number of non-empty modules

    if (O97CountNonEmptyModules(lpstOffcScan->lpstRoot,
                                lpstOffcScan->lpstStream,
                                lpstScan->uScan.stVBA5.lpstLZNT,
                                lpstOffcScan->u.stO97.dwVBAChildEntry,
                                &wModuleCount) == FALSE)
    {
        // Failed to get module count

        LZNTFreeStruct(lpstOffcScan->lpstRoot->lpvRootCookie,
                       lpstScan->uScan.stVBA5.lpstLZNT);

        return(EXTSTATUS_NO_REPAIR);
    }

    // Allocate a sibling enumeration structure

    if (SSAllocEnumSibsStruct(lpstOffcScan->lpstRoot,
                              &lpstSibs,
                              SS_DEF_MAX_WAITING_SIBS) != SS_STATUS_OK)
    {
        LZNTFreeStruct(lpstOffcScan->lpstRoot->lpvRootCookie,
                       lpstScan->uScan.stVBA5.lpstLZNT);

        return(EXTSTATUS_NO_REPAIR);
    }

    // Iterate through the modules to delete

    bMore = TRUE;
    while (bMore)
    {
        SSInitEnumSibsStruct(lpstSibs,
                             lpstOffcScan->u.stO97.dwVBAChildEntry);

        while (1)
        {
            if (SSEnumSiblingEntriesCB(lpstOffcScan->lpstRoot,
                                       O97OpenVBA5ModuleCB,
                                       abyModuleName,
                                       lpstSibs,
                                       lpstOffcScan->lpstStream) ==
                SS_STATUS_OK)
            {
                // Scan module

                if (PPRepairModule(lpstOffcScan,
                                   wModuleCount,
                                   abyModuleName,
                                   &bDelete,
                                   lpstVirusSigInfo) == FALSE)
                {
                    // Error repairing this module

                    LZNTFreeStruct(lpstOffcScan->lpstRoot->lpvRootCookie,
                                   lpstScan->uScan.stVBA5.lpstLZNT);

                    SSFreeEnumSibsStruct(lpstOffcScan->lpstRoot,
                                         lpstSibs);
                    return(EXTSTATUS_NO_REPAIR);
                }

                if (bDelete == TRUE)
                {
                    if (O97DeleteModule(lpstOffcScan->lpstRoot,
                                        lpstOffcScan->lpstStream,
                                        lpstScan->uScan.stVBA5.lpstLZNT,
                                        abyModuleName,
                                        &lpstOffcScan->u.stO97,
                                        lpbyWorkBuffer) == FALSE)
                    {
                        // Error deleting the module

                        LZNTFreeStruct(lpstOffcScan->lpstRoot->lpvRootCookie,
                                       lpstScan->uScan.stVBA5.lpstLZNT);

                        SSFreeEnumSibsStruct(lpstOffcScan->lpstRoot,
                                             lpstSibs);
                        return(EXTSTATUS_NO_REPAIR);
                    }

                    if (SSGetChildOfStorage(lpstOffcScan->lpstRoot,
                                            lpstOffcScan->u.stO97.dwVBAEntry,
                                            &lpstOffcScan->u.stO97.dwVBAChildEntry) !=
                        SS_STATUS_OK)
                    {
                        // Error getting the child of the VBA storage

                        LZNTFreeStruct(lpstOffcScan->lpstRoot->lpvRootCookie,
                                       lpstScan->uScan.stVBA5.lpstLZNT);

                        SSFreeEnumSibsStruct(lpstOffcScan->lpstRoot,
                                             lpstSibs);
                        return(EXTSTATUS_NO_REPAIR);
                    }

                    // Need to resynchronize

                    // Resynchronization assumes that repairs without
                    //  deletions will not do a blind repair.

                    break;
                }
            }
            else
            {
                // No more candidate modules

                bMore = FALSE;
                break;
            }
        }
    }

    SSFreeEnumSibsStruct(lpstOffcScan->lpstRoot,lpstSibs);

    // Delete __SRP_ streams and increment second ushort
    //  of _VBA_PROJECT stream

    if (O97UpdateForRecompilation(lpstOffcScan->lpstRoot,
                                  lpstOffcScan->lpstStream,
                                  &lpstOffcScan->u.stO97) == FALSE)
    {
        // Error updating for recompilation

        LZNTFreeStruct(lpstOffcScan->lpstRoot->lpvRootCookie,
                       lpstScan->uScan.stVBA5.lpstLZNT);

        return(EXTSTATUS_NO_REPAIR);
    }

    // Determine whether to get rid of the macros storage

    if (O97VBACheckMacrosDeletable(lpstOffcScan->lpstRoot,
                                   lpstOffcScan->lpstStream,
                                   &lpstOffcScan->u.stO97) == TRUE)
    {
        // Get rid of macros storage

        *lpbEmpty = TRUE;
    }
    else
    {
        // Don't get rid of macros storage

        *lpbEmpty = FALSE;
    }

    LZNTFreeStruct(lpstOffcScan->lpstRoot->lpvRootCookie,
                   lpstScan->uScan.stVBA5.lpstLZNT);

    return(EXTSTATUS_OK);
}


//********************************************************************
//
// Function:
//  BOOL PPVBAReintegrate()
//
// Parameters:
//  lpstCallBack        Ptr to callback structure
//  lpstStream          Ptr to PowerPoint Document stream
//  hVBAFile            Handle to VBA file to reintegrate
//  bEmpty              Whether the VBA file has no more macros
//
// Description:
//  Reintegrates a VBA file into a PowerPoint Document.
//  If bEmpty is TRUE, then the second DWORD of the VBAInfoAtom
//  record is set to zero.
//
// Returns:
//  TRUE                If the reintegration was successful
//  FALSE               If the reintegration was unsuccessful
//
//********************************************************************

BOOL PPVBAReintegrate
(
    LPCALLBACKREV1  lpstCallBack,
    LPSS_STREAM     lpstStream,
    HFILE           hVBAFile,
    BOOL            bEmpty
)
{
    HFILE           hCompressedFile;
    TCHAR           szCompressedFileName[SYM_MAX_PATH];
    DWORD           dwFileLen;
    DWORD           dwBytesLeft;
    UINT            uNumBytes;
    DWORD           dwBytesWritten;
    DWORD           dwOffset;
    DWORD           dwLength;
    BYTE            abyBuf[512];
    LPDF            lpstDF;
    BOOL            bResult = TRUE;

    // Get the length of the VBA file

    dwFileLen = lpstCallBack->FileSeek(hVBAFile,
                                       0,
                                       SEEK_END);

    if (dwFileLen == (DWORD)-1)
        return(FALSE);

    // Seek back to the beginning

    if (lpstCallBack->FileSeek(hVBAFile,
                               0,
                               SEEK_SET) != 0)
        return(FALSE);

    /////////////////////////////////////////////////////////////
    // Compress stream to a temporary file

    // Create a temporary file for the compressed stream

    hCompressedFile = lpstCallBack->
        GetTempFile(szCompressedFileName,NULL);

    if (hCompressedFile == (HFILE)-1)
        return(FALSE);

    // Allocate an inflation structure

    lpstDF = DFAllocDF(lpstCallBack);
    if (lpstDF == NULL)
        bResult = FALSE;

    if (bResult != FALSE)
    {
        // Initialize for deflation

        if (DFInit(lpstDF,
                   (LPVOID)hCompressedFile,
                   dwFileLen) == FALSE)
            bResult = FALSE;
    }

    if (bResult != FALSE)
    {
        // Deflate

        if (DFDeflate(lpstDF,
                      (LPVOID)hVBAFile) == FALSE)
            bResult = FALSE;
    }

    if (lpstDF != NULL)
        DFFreeDF(lpstDF);

    /////////////////////////////////////////////////////////////
    // Reintegrate file

    if (bResult != FALSE)
    {
        // Find the most recent ExOleObjStg record

        if (PPGetOLEStgOffset(lpstStream,
                              0,
                              &dwOffset,
                              &dwLength) == FALSE)
            bResult = FALSE;
    }

    if (bResult != FALSE)
    {
        // Get the length of the compressed file

        dwFileLen = lpstCallBack->FileSeek(hCompressedFile,
                                           0,
                                           SEEK_END);

        if (dwFileLen == (DWORD)-1)
            bResult = FALSE;

        // Seek back to beginning

        if (lpstCallBack->FileSeek(hCompressedFile,
                                   0,
                                   SEEK_SET) != 0)
            bResult = FALSE;
    }

    // Is the new compressed stream too large?

    if (dwFileLen > dwLength)
        bResult = FALSE;

    if (bResult != FALSE)
    {
        uNumBytes = sizeof(abyBuf);

        dwBytesLeft = dwFileLen;
        while (dwBytesLeft != 0)
        {
            if (dwBytesLeft < uNumBytes)
                uNumBytes = (UINT)dwBytesLeft;

            // Read from the source

            if (lpstCallBack->FileRead(hCompressedFile,
                                       abyBuf,
                                       uNumBytes) != uNumBytes)
            {
                bResult = FALSE;
                break;
            }

            // Write to destination

            if (SSSeekWrite(lpstStream,
                            dwOffset,
                            abyBuf,
                            uNumBytes,
                            &dwBytesWritten) != SS_STATUS_OK ||
                dwBytesWritten != uNumBytes)
            {
                bResult = FALSE;
                break;
            }

            dwBytesLeft -= uNumBytes;
            dwOffset += uNumBytes;
        }

        // Fill rest with zeros

        for (dwBytesLeft=0;dwBytesLeft<sizeof(abyBuf);dwBytesLeft++)
            abyBuf[dwBytesLeft] = 0;

        dwBytesLeft = dwLength - dwFileLen;
        uNumBytes = sizeof(abyBuf);
        while (dwBytesLeft != 0)
        {
            if (dwBytesLeft < uNumBytes)
                uNumBytes = (UINT)dwBytesLeft;

            if (SSSeekWrite(lpstStream,
                            dwOffset,
                            abyBuf,
                            uNumBytes,
                            &dwBytesWritten) != SS_STATUS_OK ||
                dwBytesWritten != uNumBytes)
            {
                bResult = FALSE;
                break;
            }

            dwBytesLeft -= uNumBytes;
            dwOffset += uNumBytes;
        }
    }

    lpstCallBack->FileClose(hCompressedFile);
    lpstCallBack->FileDelete(szCompressedFileName);

    if (bResult == FALSE)
        return(FALSE);

    /////////////////////////////////////////////////////////////
    // Mark no macros if macros storage is empty

    if (bEmpty == TRUE)
    {
        PP_REC_VBAInfoAtom_T    stVBAInfo;

        // Get VBAInfoAtom offset

        if (PPGetVBAInfoAtomOffset(lpstStream,
                                   0,
                                   &dwOffset,
                                   &dwLength) == FALSE)
            return(FALSE);

        // Read VBAInfoAtom

        if (SSSeekRead(lpstStream,
                       dwOffset,
                       &stVBAInfo,
                       sizeof(PP_REC_VBAInfoAtom_T),
                       &dwBytesWritten) != SS_STATUS_OK ||
            dwBytesWritten != sizeof(PP_REC_VBAInfoAtom_T))
            return(FALSE);

        // Set flags to zero

        stVBAInfo.dwFlags = 0;

        // Rewrite VBAInfoAtom

        if (SSSeekWrite(lpstStream,
                        dwOffset,
                        &stVBAInfo,
                        sizeof(PP_REC_VBAInfoAtom_T),
                        &dwBytesWritten) != SS_STATUS_OK ||
            dwBytesWritten != sizeof(PP_REC_VBAInfoAtom_T))
            return(FALSE);
    }

    return(TRUE);
}
#endif

//********************************************************************
//
// Function:
//  BOOL PPExtractVBA()
//
// Parameters:
//  lpstCallBack        Ptr to callback structure
//  lpstStream          Ptr to PowerPoint Document stream
//  lphVBAFile          Ptr to HFILE for extracted VBA store
//  lpszVBAFileName     Ptr to array for file name of VBA store
//
// Description:
//  Extracts and decompresses the compressed VBA store within
//  the given PowerPoint Document stream if the store exists.
//
//  If the function returns TRUE, then *lphVBAFile is set to the
//  handle of the file containing the decompressed VBA store
//  and lpszVBAFileName contains the name of the file.
//
// Returns:
//  TRUE                If a VBA store was successfully extracted
//  FALSE               On error or no VBA store
//
//********************************************************************

BOOL PPExtractVBA
(
    LPCALLBACKREV1  lpstCallBack,
    LPSS_STREAM     lpstStream,
    HFILE FAR *     lphVBAFile,
    LPTSTR          lpszVBAFileName
)
{
    DWORD           dwOffset;
    DWORD           dwLength;
    DWORD           dwBytesLeft;
    DWORD           dwBytesToRead;
    DWORD           dwBytesRead;
    HFILE           hCompressedFile;
    TCHAR           szCompressedFileName[SYM_MAX_PATH];
    BYTE            abyBuf[512];
    LPIF            lpstIF;
    BOOL            bResult = TRUE;

    // Search the PowerPoint Document stream
    //  for an ExOleObjStg record

    if (PPFindRecord(lpstStream,
                     ePPREC_ExOleObjStg,
                     0,
                     SSStreamLen(lpstStream),
                     NULL,
                     NULL) == FALSE)
    {
        // Error or no such record found

        return(FALSE);
    }

    // Find the most recent ExOleObjStg record

    if (PPGetOLEStgOffset(lpstStream,
                          0,
                          &dwOffset,
                          &dwLength) == FALSE)
    {
        // Error or no such record found

        return(FALSE);
    }

    /////////////////////////////////////////////////////////////
    // Extract compressed stream to a temporary file

    // Create a temporary file for the extracted compressed stream

    hCompressedFile =
        GetTempFile(lpstCallBack,szCompressedFileName);

    if (hCompressedFile == (HFILE)-1)
        return(FALSE);

    dwBytesLeft = dwLength;
    dwBytesToRead = sizeof(abyBuf);
    while (dwBytesLeft != 0)
    {
        if (dwBytesLeft < dwBytesToRead)
            dwBytesToRead = dwBytesLeft;

        // Read the compressed stream

        if (SSSeekRead(lpstStream,
                       dwOffset,
                       abyBuf,
                       dwBytesToRead,
                       &dwBytesRead) != SS_STATUS_OK ||
            dwBytesRead != dwBytesToRead)
        {
            bResult = FALSE;
            break;
        }

        // Write it out

        if (lpstCallBack->FileWrite(hCompressedFile,
                                    abyBuf,
                                    dwBytesToRead) != dwBytesToRead)
        {
            bResult = FALSE;
            break;
        }

        dwOffset += dwBytesToRead;
        dwBytesLeft -= dwBytesToRead;
    }

    if (bResult != FALSE)
    {
        if (lpstCallBack->FileSeek(hCompressedFile,0,SEEK_SET) != 0)
            bResult = FALSE;
    }

    /////////////////////////////////////////////////////////////
    // Deflate compressed file

    // Create a temporary file for the decompressed stream

    if (bResult != FALSE)
    {
        *lphVBAFile =
            GetTempFile(lpstCallBack,lpszVBAFileName);

        if (*lphVBAFile == (HFILE)-1)
            bResult = FALSE;
    }
    else
        *lphVBAFile = (HFILE)-1;

    // Allocate an inflation structure

    if (bResult != FALSE)
    {
        lpstIF = IFAllocIF(lpstCallBack);
        if (lpstIF == NULL)
            bResult = FALSE;
    }
    else
        lpstIF = NULL;

    // Initialize the inflation structure

    if (bResult != FALSE)
    {
        if (IFInit(lpstIF,
                   (LPVOID)hCompressedFile,
                   0,
                   dwLength) == FALSE)
            bResult = FALSE;
    }

    // Inflate the compressed VBA stream

    if (bResult != FALSE)
    {
        if (IFInflate(lpstIF,
                      (LPVOID)*lphVBAFile) == FALSE)
            bResult = FALSE;
    }

    // Delete the result if there was an error

    if (bResult == FALSE && *lphVBAFile != (HFILE)-1)
    {
        lpstCallBack->FileClose(*lphVBAFile);
        lpstCallBack->FileDelete(lpszVBAFileName);
    }

    if (lpstIF != NULL)
        IFFreeIF(lpstIF);

    lpstCallBack->FileClose(hCompressedFile);
    lpstCallBack->FileDelete(szCompressedFileName);

    return(bResult);
}


//********************************************************************
//
// Function:
//  EXTSTATUS PPScan()
//
// Parameters:
//  lpstOffcScan            Ptr to office scan structure
//  bRepair                 TRUE if the file should be repaired
//  lpbyWorkBuffer          Work buffer
//  lplpstVirusSigInfo      Virus info storage on hit
//  lpbMVPApproved          Ptr to BOOL for MVP status
//
// Description:
//  Scans a file for the presence of PowerPoint Macro viruses.
//
//  lpstOffcScan->u.stPP.dwPowerPointDocumentEntry should be
//  valid before entering the function.
//
//  The function searches through the PowerPoint stream looking
//  for a ExOleObjStg record.  If none is found, the function
//  returns, indicating no virus.
//
//  Otherwise, the function finds the valid ExOleObjStg and scans
//  it for viruses.
//
// Returns:
//	EXTSTATUS_OK			If no virus was found
//  EXTSTATUS_VIRUS_FOUND   If a virus was found
//
//********************************************************************

EXTSTATUS PPScan
(
    LPMSOFFICE_SCAN         lpstOffcScan,
    BOOL                    bRepair,
    LPBYTE                  lpbyWorkBuffer,
    LPLPWD_VIRUS_SIG_INFO   lplpstVirusSigInfo,
    LPBOOL                  lpbMVPApproved
)
{
    HFILE                   hVBAFile;
    TCHAR                   szVBAFileName[SYM_MAX_PATH];
    LPSS_ROOT               lpstOldRoot;
    LPSS_STREAM             lpstOldStream;
    EXTSTATUS               extStatus = EXTSTATUS_OK;
    LPCALLBACKREV1          lpstCallBack;
//    BOOL                    bEmpty;

    (void)lplpstVirusSigInfo;
    (void)lpbMVPApproved;

    lpstCallBack = (LPCALLBACKREV1)lpstOffcScan->lpstRoot->lpvRootCookie;

    lpstOldRoot = lpstOffcScan->lpstRoot;
    lpstOldStream = lpstOffcScan->lpstStream;

    if (PPExtractVBA(lpstCallBack,
                     lpstOffcScan->lpstStream,
                     &hVBAFile,
                     szVBAFileName) == FALSE)
    {
        // Error or no VBA stream

        return(EXTSTATUS_OK);
    }

    // Create root structure

    if (SSCreateRoot(&lpstOffcScan->lpstRoot,
                     lpstCallBack,
                     (LPVOID)hVBAFile,
                     SS_ROOT_FLAG_DIR_CACHE_DEF |
                      SS_ROOT_FLAG_FAT_CACHE_DEF) != SS_STATUS_OK)
    {
        lpstOffcScan->lpstRoot = NULL;
        extStatus = EXTSTATUS_MEM_ERROR;
    }

    // Create stream structure

    if (extStatus == EXTSTATUS_OK)
    {
        if (SSAllocStreamStruct(lpstOffcScan->lpstRoot,
                                &lpstOffcScan->lpstStream,
                                SS_STREAM_FLAG_DEF_BAT_CACHE) != SS_STATUS_OK)
        {
            lpstOffcScan->lpstStream = NULL;
            extStatus = EXTSTATUS_MEM_ERROR;
        }
    }

    // Get the root child entry

    if (extStatus == EXTSTATUS_OK)
    {
        if (SSGetChildOfStorage(lpstOffcScan->lpstRoot,
                                0,
                                &lpstOffcScan->u.stO97.u.stGeneric.
                                    dwVBAParentChildEntry) != SS_STATUS_OK)
            extStatus = EXTSTATUS_MEM_ERROR;
        else
        {
            // For now, use GENERIC for PP

            lpstOffcScan->u.stO97.wType = VBA5_GENERIC;
            lpstOffcScan->u.stO97.u.stGeneric.dwVBAParentEntry = 0;
        }
    }

    if (extStatus == EXTSTATUS_OK)
    {
        extStatus = VBA5Scan(lpstOffcScan,
                             lplpstVirusSigInfo,
                             lpbMVPApproved);
    }

#if 0
    if (bRepair == TRUE && extStatus == EXTSTATUS_VIRUS_FOUND)
    {
        extStatus = PPVBARepair(lpstOffcScan,
                                lpbyWorkBuffer,
                                *lplpstVirusSigInfo,
                                &bEmpty);
    }
#endif

    if (lpstOffcScan->lpstStream != NULL)
        SSFreeStreamStruct(lpstOffcScan->lpstStream);

    if (lpstOffcScan->lpstRoot != NULL)
        SSDestroyRoot(lpstOffcScan->lpstRoot);

    // Restore original root and stream

    lpstOffcScan->lpstRoot = lpstOldRoot;
    lpstOffcScan->lpstStream = lpstOldStream;

    // Reintegrate repair result if necessary
#if 0
    if (bRepair == TRUE && extStatus == EXTSTATUS_OK)
    {
        if (PPVBAReintegrate(lpstCallBack,
                             lpstOffcScan->lpstStream,
                             hVBAFile,
                             bEmpty) == FALSE)
            extStatus = EXTSTATUS_NO_REPAIR;
    }
#endif

    lpstCallBack->FileClose(hVBAFile);
    lpstCallBack->FileDelete(szVBAFileName);

    return(extStatus);
}

#endif // #ifdef PP_SCAN

