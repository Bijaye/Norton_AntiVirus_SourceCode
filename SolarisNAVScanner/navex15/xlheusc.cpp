// Copyright 1998 Symantec, SABU
//************************************************************************
//
// $Header:   S:/NAVEX/VCS/XLHEUSC.CPv   1.2   09 Dec 1998 17:45:16   DCHI  $
//
// Description:
//  Contains top-level Excel 5.0/95/97 heuristic macro scan function code.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/XLHEUSC.CPv  $
// 
//    Rev 1.2   09 Dec 1998 17:45:16   DCHI
// Added #include "gdefines.h"
// 
//    Rev 1.1   09 Sep 1998 12:45:36   DCHI
// Changes for password-protected Excel 97 document repair.
// 
//    Rev 1.0   04 Aug 1998 10:57:12   DCHI
// Initial revision.
// 
//************************************************************************

#include "gdefines.h"

#ifdef MACROHEU

#include "platform.h"

#include "ctsn.h"
#include "callback.h"
#include "navex.h"

#include "wd7api.h"
#include "o97api.h"
#include "xl5api.h"
#include "acsapi.h"
#include "olestrnm.h"

#include "wdscan15.h"
#include "offcsc15.h"

#include "xlheusc.h"
#include "excelheu.h"

#include "xl5rpr.h"
#include "xl97rpr.h"

//********************************************************************
//
// Function:
//  BOOL XL5HHasViralCopy()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  The function determines whether the module has a viral
//  Copy call.
//
// Returns:
//  TRUE                If a viral copy is found
//  FALSE               Otherwise
//
//********************************************************************

BOOL XL5HHasViralCopy
(
    LPXL5H              lpstEnv
)
{
    int                 nCopyIter;
    BOOL                bFound;

    // Do we need to scan for copies?

    if (XL5HModuleNeedsCopyScan(lpstEnv,
                                lpstEnv->wCopyModuleStreamIdx,
                                &bFound) == FALSE ||
        bFound == FALSE)
        return(FALSE);

    // Initialize for access

    if (lpstEnv->nType == XL5H_TYPE_95)
    {
        if (XL5ModAccInit(&lpstEnv->stMod.st95,
                          lpstEnv->lpstCopyStream,
                          lpstEnv->lpabyBuf,
                          lpstEnv->nBufSize) == FALSE)
            return(FALSE);
    }
    else
    {
        if (O97ModAccInit(&lpstEnv->stMod.st97,
                          lpstEnv->lpstCopyStream,
                          lpstEnv->lpabyBuf,
                          lpstEnv->nBufSize) == FALSE)
            return(FALSE);

        lpstEnv->bLittleEndian = lpstEnv->stMod.st97.bLittleEndian;
    }

    lpstEnv->lpstModStream = lpstEnv->lpstCopyStream;

    lpstEnv->stCopyState.wFuncBeginLine = 0;
    lpstEnv->stCopyState.wNextScanLine = 0;
    lpstEnv->stCopyState.nAssignHead = 0;
    lpstEnv->stCopyState.nAssignCount = 0;
    lpstEnv->stCopyState.nWithHead = 0;
    lpstEnv->stCopyState.nWithCount = 0;

    // Check all the copy calls

    for (nCopyIter=0;nCopyIter<16;nCopyIter++)
    {
        // Find the next copy call

        if (XL5HModuleFindCopy(lpstEnv,
                               &bFound) == FALSE ||
            bFound == FALSE)
            return(FALSE);

        // Resolve the call

        if (XL5HCopyCallIsViral(lpstEnv,
                                lpstEnv->stCopyState.
                                    wNextScanLine - 1) != FALSE)
            return(TRUE);
    }

    return(FALSE);
}


//********************************************************************
//
// Function:
//  BOOL XL5HHasViralAddName()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  The function determines whether the module has a viral
//  Add/Name set.
//
// Returns:
//  TRUE                If a viral Add/Name is found
//  FALSE               Otherwise
//
//********************************************************************

BOOL XL5HHasViralAddName
(
    LPXL5H              lpstEnv
)
{
    int                 nCopyIter;
    BOOL                bFound;

    // Do we need to scan for copies?

    if (XL5HModuleNeedsAddNameScan(lpstEnv,
                                   lpstEnv->wCopyModuleStreamIdx,
                                   &bFound) == FALSE ||
        bFound == FALSE)
        return(FALSE);

    // Initialize for access

    if (lpstEnv->nType == XL5H_TYPE_95)
    {
        if (XL5ModAccInit(&lpstEnv->stMod.st95,
                          lpstEnv->lpstCopyStream,
                          lpstEnv->lpabyBuf,
                          lpstEnv->nBufSize) == FALSE)
            return(FALSE);
    }
    else
    {
        if (O97ModAccInit(&lpstEnv->stMod.st97,
                          lpstEnv->lpstCopyStream,
                          lpstEnv->lpabyBuf,
                          lpstEnv->nBufSize) == FALSE)
            return(FALSE);

        lpstEnv->bLittleEndian = lpstEnv->stMod.st97.bLittleEndian;
    }

    lpstEnv->lpstModStream = lpstEnv->lpstCopyStream;

    // Check for a viral add

    if (XL5HHasAWBModulesAdd(lpstEnv) == FALSE)
        return(FALSE);

    lpstEnv->stCopyState.wFuncBeginLine = 0;
    lpstEnv->stCopyState.wNextScanLine = 0;
    lpstEnv->stCopyState.nAssignHead = 0;
    lpstEnv->stCopyState.nAssignCount = 0;
    lpstEnv->stCopyState.nWithHead = 0;
    lpstEnv->stCopyState.nWithCount = 0;

    // Check all the Name assignments

    for (nCopyIter=0;nCopyIter<16;nCopyIter++)
    {
        // Find the next copy call

        if (XL5HModuleFindNameAssign(lpstEnv,
                                     &bFound) == FALSE ||
            bFound == FALSE)
            return(FALSE);

        // Resolve the call

        if (XL5HNameAssignmentIsViral(lpstEnv,
                                      lpstEnv->stCopyState.
                                          wNextScanLine - 1) != FALSE)
            return(TRUE);
    }

    return(FALSE);
}


//********************************************************************
//
// Function:
//  BOOL XL5HHasViralAddInsertFile()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  The function determines whether the module has a viral
//  Add/InsertFile set.
//
// Returns:
//  TRUE                If a viral Add/InsertFile is found
//  FALSE               Otherwise
//
//********************************************************************

BOOL XL5HHasViralAddInsertFile
(
    LPXL5H              lpstEnv
)
{
    int                 nCopyIter;
    BOOL                bFound;

    // Do we need to scan for copies?

    if (XL5HModuleNeedsAddInsertScan(lpstEnv,
                                     lpstEnv->wCopyModuleStreamIdx,
                                     &bFound) == FALSE ||
        bFound == FALSE)
        return(FALSE);

    // Initialize for access

    if (lpstEnv->nType == XL5H_TYPE_95)
    {
        if (XL5ModAccInit(&lpstEnv->stMod.st95,
                          lpstEnv->lpstCopyStream,
                          lpstEnv->lpabyBuf,
                          lpstEnv->nBufSize) == FALSE)
            return(FALSE);
    }
    else
    {
        if (O97ModAccInit(&lpstEnv->stMod.st97,
                          lpstEnv->lpstCopyStream,
                          lpstEnv->lpabyBuf,
                          lpstEnv->nBufSize) == FALSE)
            return(FALSE);

        lpstEnv->bLittleEndian = lpstEnv->stMod.st97.bLittleEndian;
    }

    lpstEnv->lpstModStream = lpstEnv->lpstCopyStream;

    // Check for a viral add

    if (XL5HHasAWBModulesAdd(lpstEnv) == FALSE)
        return(FALSE);

    lpstEnv->stCopyState.wFuncBeginLine = 0;
    lpstEnv->stCopyState.wNextScanLine = 0;
    lpstEnv->stCopyState.nAssignHead = 0;
    lpstEnv->stCopyState.nAssignCount = 0;
    lpstEnv->stCopyState.nWithHead = 0;
    lpstEnv->stCopyState.nWithCount = 0;

    // Check all the InsertFile calls

    for (nCopyIter=0;nCopyIter<16;nCopyIter++)
    {
        // Find the next InsertFile call

        if (XL5HModuleFindInsertFile(lpstEnv,
                                     &bFound) == FALSE ||
            bFound == FALSE)
            return(FALSE);

        // Resolve the call

        if (XL5HInsertFileCallIsViral(lpstEnv,
                                      lpstEnv->stCopyState.
                                          wNextScanLine - 1) != FALSE)
            return(TRUE);
    }

    return(FALSE);
}


//********************************************************************
//
// Function:
//  BOOL XL5HFindViralCopy()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  The function determines whether the module has a viral
//  Copy, Add/Name, or Add/InsertFile set.
//
// Returns:
//  TRUE                If a viral set is found
//  FALSE               Otherwise
//
//********************************************************************

BOOL XL5HFindViralCopy
(
    LPXL5H              lpstEnv
)
{
    int                 nCopyIter;
    LPFNSS_ENUM_CB      lpfnEnumCB;

    if (lpstEnv->nType == XL5H_TYPE_95)
        lpfnEnumCB = XL5OpenHexNamedStreamCB;
    else
        lpfnEnumCB = O97OpenVBA5ModuleCB;

    // Start with the VBA child entry

    SSInitEnumSibsStruct(lpstEnv->lpstCopySibs,
                         lpstEnv->dw_VBA_PROJECTChildEntry);

    lpstEnv->wCopyModuleStreamIdx = 0;
    lpstEnv->lpstModStream = lpstEnv->lpstCopyStream;
    nCopyIter = 0;
    while (nCopyIter++ < 256)
    {
        if (SSEnumSiblingEntriesCB(lpstEnv->lpstRoot,
                                   lpfnEnumCB,
                                   lpstEnv->abypsCopyStreamName + 1,
                                   lpstEnv->lpstCopySibs,
                                   lpstEnv->lpstCopyStream) != SS_STATUS_OK)
            break;

        // Check for circular links

        if (nCopyIter > 1 &&
            SSStreamID(lpstEnv->lpstCopyStream) ==
            lpstEnv->dw_VBA_PROJECTChildEntry)
            break;

        // Look for a viral copy
        // Look for a viral Add/Name
        // Look for a viral Add/InsertFile

        if (XL5HHasViralCopy(lpstEnv) != FALSE ||
            XL5HHasViralAddName(lpstEnv) != FALSE ||
            XL5HHasViralAddInsertFile(lpstEnv) != FALSE)
        {
            int         nLen;
            int         i;

            // Set up the stream name's length byte

            nLen = 0;
            while (lpstEnv->abypsCopyStreamName[nLen + 1])
                ++nLen;

            lpstEnv->abypsCopyStreamName[0] = (BYTE)nLen;

            if (lpstEnv->nType == XL5H_TYPE_95)
            {
                // Get the actual name of the module

                if (XL5DirInfoModuleName(&lpstEnv->stDirInfo,
                                         lpstEnv->abypsCopyStreamName,
                                         lpstEnv->abypsCopySheetName) != FALSE)
                {
                    // Lower case the module name and place
                    //  it in lpstScan->abypsCopySheetName

                    nLen = lpstEnv->abypsCopySheetName[0];
                    for (i=1;i<=nLen;i++)
                        lpstEnv->abypsCopySheetName[i] =
                            SSToLower(lpstEnv->abypsCopySheetName[i]);
                }
            }
            else
            {
                for (i=0;i<=nLen;i++)
                    lpstEnv->abypsCopySheetName[i] =
                        SSToLower(lpstEnv->abypsCopyStreamName[i]);

                lpstEnv->abypsCopySheetName[nLen+1] = 0;
            }

            return(TRUE);
        }

        // Try the next one

        lpstEnv->wCopyModuleStreamIdx++;
    }

    return(FALSE);
}


//********************************************************************
//
// Function:
//  BOOL XL5HeuristicRepair()
//
// Parameters:
//  lpstOffcScan        Ptr to office scan structure
//  lpstEnv             Ptr to environment structure
//
// Description:
//  The function removes the modules specified in the fields
//  lpstEnv->abypsSaveSheetName[] and lpstEnv->abypsCopySheetName[]
//  and erases the streams specified in the fields
//  lpstEnv->abypsSaveStreamName[] and lpstEnv->abypsCopyStreamName[].
//
//  The function uses lpstOffcScan->lpstStream as the working stream.
//  The function uses lpstEnv->lpstSaveSibs as the working sibling
//  structure.
//
// Returns:
//  TRUE                On success
//  FALSE               On error
//
//********************************************************************

BOOL XL5HeuristicRepair
(
    LPMSOFFICE_SCAN     lpstOffcScan,
    LPXL5H              lpstEnv
)
{
    LPSS_STREAM         lpstStream;
    XL5ENCKEY_T         stKey;
    BOOL                bSaveAndCopyEq;
    int                 i;
    DWORD               dwSheetIndex;
    DWORD               dwBoundSheetOffset;

    lpstStream = lpstOffcScan->lpstStream;

    // Decide whether to proceed based on encryption state

    if (XL5EncryptedRepairCheck(lpstOffcScan) == FALSE)
        return(FALSE);

    // Determine whether both the save and copy streams are equal

    bSaveAndCopyEq = TRUE;
    if (lpstEnv->abypsSaveStreamName[0] ==
        lpstEnv->abypsCopyStreamName[0])
    {
        for (i=1;i<=lpstEnv->abypsSaveStreamName[0];i++)
        {
            if (lpstEnv->abypsSaveStreamName[i] !=
                lpstEnv->abypsCopyStreamName[i])
            {
                bSaveAndCopyEq = FALSE;
                break;
            }
        }
    }
    else
        bSaveAndCopyEq = FALSE;

    // Get the associated Book stream

    if (XL5GetVBABookStream(lpstOffcScan->lpstRoot,
                            lpstOffcScan->u.stXL5.
                                dw_VBA_PROJECTEntry,
                            lpstStream) == FALSE)
        return(FALSE);

    // Get the key

    if (XL5FindKey(lpstStream,&stKey) == FALSE)
        return(FALSE);

    ////////////////////////////////////////////////////////
    // Delete the Save and Copy sheets and streams

    // Get the BOUNDSHEET info for the Save sheet

    dwSheetIndex = 0;
    if (XL5FindBoundSheet(lpstStream,
                          &stKey,
                          lpstEnv->abypsSaveSheetName,
                          0xFF,
                          &dwSheetIndex,
                          &dwBoundSheetOffset,
                          NULL) == FALSE)
        return(FALSE);

    // Erase the Save sheet information

    if (XL5EraseSheet(lpstStream,
                      &stKey,
                      dwBoundSheetOffset,
                      (WORD)dwSheetIndex) == FALSE)
        return(FALSE);

    // Blank out the Save stream

    if (XL5BlankOutVBModuleStream(lpstEnv->lpstSaveStream) == FALSE)
        return(FALSE);

    // If the copy sheet is different, delete it also

    if (bSaveAndCopyEq == FALSE)
    {
        // Get the BOUNDSHEET info for the Copy sheet

        dwSheetIndex = 0;
        if (XL5FindBoundSheet(lpstStream,
                              &stKey,
                              lpstEnv->abypsCopySheetName,
                              0xFF,
                              &dwSheetIndex,
                              &dwBoundSheetOffset,
                              NULL) == FALSE)
            return(FALSE);

        // Erase the Copy sheet information

        if (XL5EraseSheet(lpstStream,
                          &stKey,
                          dwBoundSheetOffset,
                          (WORD)dwSheetIndex) == FALSE)
            return(FALSE);

        // Blank out the Copy stream

        if (XL5BlankOutVBModuleStream(lpstEnv->lpstCopyStream) == FALSE)
            return(FALSE);
    }

    return(TRUE);
}


//********************************************************************
//
// Function:
//  EXTSTATUS XL5HeuristicScan()
//
// Parameters:
//  lpstOffcScan        Ptr to office scan structure
//  bRepair             TRUE if repair should also be done
//
// Description:
//  Heuristically scans a file for the presence
//  of Excel 5.0/95 viruses.
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
//  EXTSTATUS_VIRUS_FOUND   If a virus was found
//
//********************************************************************

EXTSTATUS XL5HeuristicScan
(
    LPMSOFFICE_SCAN     lpstOffcScan,
    BOOL                bRepair
)
{
    EXTSTATUS           extStatus;
    LPXL5H              lpstEnv;
    int                 i, nLen;
    int                 nSaveIter;
    BOOL                bFound;

    // Allocate an XL5H structure

    lpstEnv = XL5HAlloc(lpstOffcScan->lpstRoot,
                        lpstOffcScan->lpstStream);

    if (lpstEnv == NULL)
        return(EXTSTATUS_MEM_ERROR);

    // Initialize environment

    XL5HInit(lpstEnv);
    lpstEnv->nType = XL5H_TYPE_95;

    // Get the dir stream

    SSInitEnumSibsStruct(lpstEnv->lpstSaveSibs,
                         lpstOffcScan->u.stXL5.dw_VBA_PROJECTChildEntry);

    if (SSEnumSiblingEntriesCB(lpstOffcScan->lpstRoot,
                               SSOpenStreamByNameCB,
                               gabywszdir,
                               lpstEnv->lpstSaveSibs,
                               lpstEnv->lpstDirStream) != SS_STATUS_OK)
    {
        XL5HFree(lpstEnv);
        return(EXTSTATUS_MEM_ERROR);
    }

    // Initialize for dir access

    if (XL5DirInfoInit(lpstEnv->lpstDirStream,
                       &lpstEnv->stDirInfo) == FALSE)
    {
        XL5HFree(lpstEnv);
        return(EXTSTATUS_MEM_ERROR);
    }

    // Scan the dir stream

    if (XL5HGetAppStrID(lpstEnv) == FALSE)
    {
        XL5HFree(lpstEnv);
        return(EXTSTATUS_MEM_ERROR);
    }

    // Initialize environment

    lpstEnv->dw_VBA_PROJECTChildEntry =
        lpstOffcScan->u.stXL5.dw_VBA_PROJECTChildEntry;
    lpstEnv->wModuleStreamIdx = 0;
    lpstEnv->bLittleEndian = TRUE;

    // Start with the VBA child entry

    SSInitEnumSibsStruct(lpstEnv->lpstSaveSibs,
                         lpstOffcScan->u.stXL5.dw_VBA_PROJECTChildEntry);

    extStatus = EXTSTATUS_OK;
    nSaveIter = 0;
    while (nSaveIter++ < 1024)
    {
        if (SSEnumSiblingEntriesCB(lpstOffcScan->lpstRoot,
                                   XL5OpenHexNamedStreamCB,
                                   lpstEnv->abypsSaveStreamName + 1,
                                   lpstEnv->lpstSaveSibs,
                                   lpstEnv->lpstSaveStream) != SS_STATUS_OK)
            break;

        // Check for circular links

        if (nSaveIter > 1 &&
            SSStreamID(lpstEnv->lpstSaveStream) ==
            lpstEnv->dw_VBA_PROJECTChildEntry)
            break;

        lpstEnv->lpstModStream = lpstEnv->lpstSaveStream;
        if (XL5HModuleNeedsSaveScan(lpstEnv,
                                    lpstEnv->wModuleStreamIdx,
                                    &bFound) == FALSE ||
            bFound == FALSE)
        {
            // Try the next one

            lpstEnv->wModuleStreamIdx++;
            continue;
        }

        // Set up the stream name's length byte

        nLen = 0;
        while (lpstEnv->abypsSaveStreamName[nLen + 1])
            ++nLen;

        lpstEnv->abypsSaveStreamName[0] = (BYTE)nLen;

        // Get the actual name of the module

        if (XL5DirInfoModuleName(&lpstEnv->stDirInfo,
                                 lpstEnv->abypsSaveStreamName,
                                 lpstEnv->abypsSaveSheetName) == FALSE)
        {
            // Try the next one

            lpstEnv->wModuleStreamIdx++;
            continue;
        }

        // Lower case the module name and place
        //  it in lpstScan->abypsSaveSheetName

        nLen = lpstEnv->abypsSaveSheetName[0];
        for (i=1;i<=nLen;i++)
            lpstEnv->abypsSaveSheetName[i] =
                SSToLower(lpstEnv->abypsSaveSheetName[i]);

        lpstEnv->stSaveState.wFuncBeginLine = 0;
        lpstEnv->stSaveState.wNextScanLine = 0;
        lpstEnv->stSaveState.nAssignHead = 0;
        lpstEnv->stSaveState.nAssignCount = 0;
        lpstEnv->stSaveState.nWithHead = 0;
        lpstEnv->stSaveState.nWithCount = 0;

        while (nSaveIter++ < 1024)
        {
            if (XL5ModAccInit(&lpstEnv->stMod.st95,
                              lpstEnv->lpstSaveStream,
                              lpstEnv->lpabyBuf,
                              lpstEnv->nBufSize) == FALSE)
            {
                // Try the next one

                break;
            }

            lpstEnv->lpstModStream = lpstEnv->lpstSaveStream;
            if (XL5HModuleFindSave(lpstEnv,
                                   &bFound) == FALSE)
            {
                // Try the next one

                break;
            }

            // Scan the module

            if (bFound == FALSE)
                break;

            // Resolve the call

            if (XL5HResolveSaveCallLine(lpstEnv,
                                        lpstEnv->stSaveState.wNextScanLine - 1) == FALSE)
                continue;

            // Now find a corresponding copy

            if (XL5HFindViralCopy(lpstEnv) != FALSE)
            {
                extStatus = EXTSTATUS_VIRUS_FOUND;
                break;
            }
        }

        if (extStatus == EXTSTATUS_VIRUS_FOUND)
            break;

        lpstEnv->wModuleStreamIdx++;
    }

    if (bRepair == FALSE)
    {
        /////////////////////////////////////////////////////////////
        // Do virus check
        /////////////////////////////////////////////////////////////

        if (extStatus == EXTSTATUS_VIRUS_FOUND)
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
                                         NULL) == FALSE)
                    extStatus = EXTSTATUS_OK;
            }
            else
                extStatus = EXTSTATUS_OK;
        }
    }
    else
    {
        // Do repair

        if (XL5HeuristicRepair(lpstOffcScan,
                               lpstEnv) == FALSE)
            extStatus = EXTSTATUS_NO_REPAIR;
        else
            extStatus = EXTSTATUS_OK;
    }

    // Free the heuristic environment structure

    XL5HFree(lpstEnv);

    // No virus

    return(extStatus);
}


//********************************************************************
//
// Function:
//  BOOL XL97HeuristicRepair()
//
// Parameters:
//  lpstOffcScan        Ptr to office scan structure
//  lpstEnv             Ptr to environment structure
//
// Description:
//  The function removes the modules specified in the fields
//  lpstEnv->abypsSaveSheetName[] and lpstEnv->abypsCopySheetName[]
//  and erases the streams specified in the fields
//  lpstEnv->abypsSaveStreamName[] and lpstEnv->abypsCopyStreamName[].
//
//  The function uses lpstOffcScan->lpstStream as the working stream.
//  The function uses lpstEnv->lpstSaveSibs as the working sibling
//  structure.
//
// Returns:
//  TRUE                On success
//  FALSE               On error
//
//********************************************************************

BOOL XL97HeuristicRepair
(
    LPMSOFFICE_SCAN     lpstOffcScan,
    LPXL5H              lpstEnv
)
{
    BOOL                bEncrypted;
    BOOL                bSaveAndCopyEq;
    LPLZNT              lpstLZNT;
    int                 i;

    // Check to see whether we should continue with repair
    //  based on the encrypted state of the document and
    //  whether we are doing XL97 encrypted repair

    if (XL97EncryptedRepairCheck(lpstOffcScan,
                                 &bEncrypted) == FALSE)
    {
        // Do not proceed with repair

        return(FALSE);
    }

    // Determine whether both the save and copy streams are equal

    bSaveAndCopyEq = TRUE;
    if (lpstEnv->abypsSaveStreamName[0] ==
        lpstEnv->abypsCopyStreamName[0])
    {
        for (i=1;i<=lpstEnv->abypsSaveStreamName[0];i++)
        {
            if (lpstEnv->abypsSaveStreamName[i] !=
                lpstEnv->abypsCopyStreamName[i])
            {
                bSaveAndCopyEq = FALSE;
                break;
            }
        }
    }
    else
        bSaveAndCopyEq = FALSE;

    // Allocate LZNT structure

    if (LZNTAllocStruct(lpstOffcScan->lpstRoot->lpvRootCookie,
                        &lpstLZNT) != TRUE)
        return(FALSE);

    ////////////////////////////////////////////////////////
    // Remove the Save module

    if (bEncrypted == FALSE)
    {
        if (O97XLDeleteModule(lpstOffcScan->lpstRoot,
                              lpstEnv->lpstSaveStream,
                              lpstLZNT,
                              lpstEnv->abypsSaveStreamName + 1,
                              &lpstOffcScan->u.stO97,
                              lpstEnv->lpabyBuf) == FALSE)
        {
            // Error deleting the module

            LZNTFreeStruct(lpstOffcScan->lpstRoot->lpvRootCookie,
                           lpstLZNT);

            return(FALSE);
        }
    }
    else
    {
        // Just replace the module with a blank stream

        if (XL97ReplaceVBASrcWithMsg(lpstEnv->lpstSaveStream,
                                     lpstLZNT) == FALSE)
        {
            // Error replacing stream

            LZNTFreeStruct(lpstOffcScan->lpstRoot->lpvRootCookie,
                           lpstLZNT);

            return(FALSE);
        }
    }

    ////////////////////////////////////////////////////////
    // Remove the Copy module

    if (bSaveAndCopyEq == FALSE)
    {
        if (bEncrypted == FALSE)
        {
            // Resynchronize

            if (SSGetChildOfStorage(lpstOffcScan->lpstRoot,
                                    lpstOffcScan->u.stO97.dwVBAEntry,
                                    &lpstOffcScan->u.stO97.dwVBAChildEntry) !=
                SS_STATUS_OK)
            {
                // Error getting the child of the VBA storage

                LZNTFreeStruct(lpstOffcScan->lpstRoot->lpvRootCookie,
                               lpstLZNT);

                return(FALSE);
            }

            if (O97XLDeleteModule(lpstOffcScan->lpstRoot,
                                  lpstEnv->lpstCopyStream,
                                  lpstLZNT,
                                  lpstEnv->abypsCopyStreamName + 1,
                                  &lpstOffcScan->u.stO97,
                                  lpstEnv->lpabyBuf) == FALSE)
            {
                // Error deleting the module

                LZNTFreeStruct(lpstOffcScan->lpstRoot->lpvRootCookie,
                               lpstLZNT);

                return(FALSE);
            }
        }
        else
        {
            // Just replace the module with a blank stream

            if (XL97ReplaceVBASrcWithMsg(lpstEnv->lpstCopyStream,
                                         lpstLZNT) == FALSE)
            {
                // Error replacing stream

                LZNTFreeStruct(lpstOffcScan->lpstRoot->lpvRootCookie,
                               lpstLZNT);

                return(FALSE);
            }
        }
    }

    ////////////////////////////////////////////////////////
    // Update companion streams

    if (XL97RepairEpilogue(lpstOffcScan,
                           lpstLZNT,
                           bEncrypted) == FALSE)
    {
        // Error in epilogue

        LZNTFreeStruct(lpstOffcScan->lpstRoot->lpvRootCookie,
                       lpstLZNT);

        return(FALSE);
    }

    LZNTFreeStruct(lpstOffcScan->lpstRoot->lpvRootCookie,
                   lpstLZNT);

    return(TRUE);
}


//********************************************************************
//
// Function:
//  EXTSTATUS XL97HeuristicScan()
//
// Parameters:
//  lpstOffcScan        Ptr to office scan structure
//  bRepair             TRUE if repair should also be done
//
// Description:
//  Heuristically scans a file for the presence
//  of Excel 5.0/95 viruses in Excel 97 format.
//
//  The function iterates through and scans each of the
//  streams in the VBA storage.
//
// Returns:
//  EXTSTATUS_OK            If no virus was found
//  EXTSTATUS_VIRUS_FOUND   If a virus was found
//
//********************************************************************

EXTSTATUS XL97HeuristicScan
(
    LPMSOFFICE_SCAN     lpstOffcScan,
    BOOL                bRepair
)
{
    EXTSTATUS           extStatus;
    LPXL5H              lpstEnv;
    int                 nLen;
    int                 nSaveIter;
    BOOL                bFound;

    // Allocate an XL5H structure

    lpstEnv = XL5HAlloc(lpstOffcScan->lpstRoot,
                        lpstOffcScan->lpstStream);

    if (lpstEnv == NULL)
        return(EXTSTATUS_MEM_ERROR);

    // Initialize environment

    XL5HInit(lpstEnv);
    lpstEnv->nType = XL5H_TYPE_97;

    // Find the <VBA> storage

    if (O97OpenVBAStorage(lpstOffcScan->lpstRoot,
                          &lpstOffcScan->u.stO97) == FALSE)
    {
        XL5HFree(lpstEnv);
        return(EXTSTATUS_MEM_ERROR);
    }

    // Get the _VBA_PROJECT stream

    SSInitEnumSibsStruct(lpstEnv->lpstSaveSibs,
                         lpstOffcScan->u.stO97.dwVBAChildEntry);

    if (SSEnumSiblingEntriesCB(lpstOffcScan->lpstRoot,
                               SSOpenStreamByNameCB,
                               gabywsz_VBA_PROJECT,
                               lpstEnv->lpstSaveSibs,
                               lpstEnv->lpstDirStream) != SS_STATUS_OK)
    {
        XL5HFree(lpstEnv);
        return(EXTSTATUS_MEM_ERROR);
    }

    // Scan the dir stream

    if (XL5H97GetAppStrID(lpstEnv) == FALSE)
    {
        XL5HFree(lpstEnv);
        return(EXTSTATUS_MEM_ERROR);
    }

    // Initialize environment

    lpstEnv->dw_VBA_PROJECTChildEntry =
        lpstOffcScan->u.stO97.dwVBAChildEntry;
    lpstEnv->wModuleStreamIdx = 0;

    // Start with the VBA child entry

    SSInitEnumSibsStruct(lpstEnv->lpstSaveSibs,
                         lpstOffcScan->u.stO97.dwVBAChildEntry);

    extStatus = EXTSTATUS_OK;
    nSaveIter = 0;
    while (nSaveIter++ < 1024)
    {
        if (SSEnumSiblingEntriesCB(lpstOffcScan->lpstRoot,
                                   O97OpenVBA5ModuleCB,
                                   lpstEnv->abypsSaveStreamName + 1,
                                   lpstEnv->lpstSaveSibs,
                                   lpstEnv->lpstSaveStream) != SS_STATUS_OK)
            break;

        // Check for circular links

        if (nSaveIter > 1 &&
            SSStreamID(lpstEnv->lpstSaveStream) ==
            lpstEnv->dw_VBA_PROJECTChildEntry)
            break;

        lpstEnv->lpstModStream = lpstEnv->lpstSaveStream;

        if (XL5HModuleNeedsSaveScan(lpstEnv,
                                    lpstEnv->wModuleStreamIdx,
                                    &bFound) == FALSE ||
            bFound == FALSE)
        {
            // Try the next one

            lpstEnv->wModuleStreamIdx++;
            continue;
        }

        // Set up the stream name's length byte

        nLen = 0;
        while (lpstEnv->abypsSaveStreamName[nLen + 1])
        {
            ++nLen;
            lpstEnv->abypsSaveSheetName[nLen] =
                SSToLower(lpstEnv->abypsSaveStreamName[nLen]);
        }

        lpstEnv->abypsSaveSheetName[nLen+1] = 0;

        lpstEnv->abypsSaveStreamName[0] = (BYTE)nLen;
        lpstEnv->abypsSaveSheetName[0] = (BYTE)nLen;

        lpstEnv->stSaveState.wFuncBeginLine = 0;
        lpstEnv->stSaveState.wNextScanLine = 0;
        lpstEnv->stSaveState.nAssignHead = 0;
        lpstEnv->stSaveState.nAssignCount = 0;
        lpstEnv->stSaveState.nWithHead = 0;
        lpstEnv->stSaveState.nWithCount = 0;

        lpstEnv->wModuleSheetIdx = 0;

        while (nSaveIter++ < 1024)
        {
            if (O97ModAccInit(&lpstEnv->stMod.st97,
                              lpstEnv->lpstSaveStream,
                              lpstEnv->lpabyBuf,
                              lpstEnv->nBufSize) == FALSE)
            {
                // Try the next one

                break;
            }

            lpstEnv->lpstModStream = lpstEnv->lpstSaveStream;
            lpstEnv->bLittleEndian = lpstEnv->stMod.st97.bLittleEndian;

            if (XL5HModuleFindSave(lpstEnv,
                                   &bFound) == FALSE)
            {
                // Try the next one

                break;
            }

            // Scan the module

            if (bFound == FALSE)
                break;

            // Resolve the call

            if (XL5HResolveSaveCallLine(lpstEnv,
                                        lpstEnv->stSaveState.wNextScanLine - 1) == FALSE)
                continue;

            // Now find a corresponding copy

            if (XL5HFindViralCopy(lpstEnv) != FALSE)
            {
                extStatus = EXTSTATUS_VIRUS_FOUND;
                break;
            }
        }

        if (extStatus == EXTSTATUS_VIRUS_FOUND)
            break;

        lpstEnv->wModuleStreamIdx++;
    }

    if (bRepair != FALSE && extStatus == EXTSTATUS_VIRUS_FOUND)
    {
        // Do repair

        if (XL97HeuristicRepair(lpstOffcScan,
                                lpstEnv) == FALSE)
            extStatus = EXTSTATUS_NO_REPAIR;
        else
            extStatus = EXTSTATUS_OK;
    }

    // Free the heuristic environment structure

    XL5HFree(lpstEnv);

    return(extStatus);
}

#endif // #ifdef MACROHEU

