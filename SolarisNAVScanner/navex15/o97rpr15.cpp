//************************************************************************
//
// $Header:   S:/NAVEX/VCS/o97rpr15.cpv   1.10   14 Dec 1998 18:52:28   DCHI  $
//
// Description:
//      Contains Word 8.0/Excel 97 macro repair function code.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/o97rpr15.cpv  $
// 
//    Rev 1.10   14 Dec 1998 18:52:28   DCHI
// Changes for ThisDocument repair fix.
// 
//    Rev 1.9   09 Sep 1998 12:45:32   DCHI
// Changes for password-protected Excel 97 document repair.
// 
//    Rev 1.8   08 Jul 1998 15:46:58   DCHI
// Added checks for encrypted [companion] Workbook streams.
// 
//    Rev 1.7   11 May 1998 17:53:30   DCHI
// Changes for Access 2.0 scanning and Access datafile conversion.
// 
//    Rev 1.6   15 Apr 1998 17:24:08   DCHI
// Modifications for new XL95 and XL5 engines.
// 
//    Rev 1.5   02 Apr 1998 10:54:16   DCHI
// Use O97XLWorkbookVBtoVHWS() instead of O97XLWorkbookVeryHideVBSheets().
// 
//    Rev 1.4   10 Mar 1998 13:41:04   DCHI
// Modified to use new O97 CRC.
// 
//    Rev 1.3   26 Sep 1997 12:41:04   DCHI
// Modified XL97 repair to very hide all VB module sheets.
// 
//    Rev 1.2   13 Aug 1997 16:22:16   DCHI
// Corrected use of incorrect return values on error, etc.
// 
//    Rev 1.1   10 Jul 1997 17:45:48   DDREW
// Turn on NLM repairs for NAVEX15
// 
//    Rev 1.0   09 Jul 1997 16:19:14   DCHI
// Initial revision.
// 
//************************************************************************

#if defined(NAVEX15) || !defined(SYM_NLM)

#include "platform.h"
#include "ctsn.h"
#include "navex.h"

#include "wd7api.h"
#include "o97api.h"
#include "xl5api.h"
#include "acsapi.h"

#include "wdscan15.h"
#include "wdrepair.h"

#include "offcsc15.h"
#include "o97scn15.h"
#include "o97rpr15.h"

#include "mcrsgutl.h"

#include "mvp.h"

//********************************************************************
//
// Function:
//  BOOL WD8RepairModule()
//
// Description:
//  Repairs a module of a Word 8.0 virus.  If the module should
//  be deleted, then *lpbDelete is set to TRUE before returning.
//
// Returns:
//  TRUE        If the repair was successful
//  FALSE       If the repair was unsuccessful
//
//********************************************************************

BOOL WD8RepairModule
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
    LPLZNT              lpstLZNT;

    lpstScan = lpstOffcScan->lpstScan;
    lpstLZNT = lpstScan->uScan.stVBA5.lpstLZNT;

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

    WDInitHitBitArrays(&gstWD8SigSet,lpstScan);

    /////////////////////////////////////////////////////////////
    // Scan and apply repair to module
    /////////////////////////////////////////////////////////////

    // Scan module

    VBA5ScanModule(&gstWD8SigSet,lpstScan);

    // Apply repair signature

    *lpbDelete = FALSE;

    if (MVPCheck(MVP_WD8,
                 lpstScan->abyName,
                 lpstScan->dwMVPCRC) == FALSE &&
        O97LZNTStreamAttributeOnly(lpstLZNT) == FALSE)
    {
        // Found a non-approved macro

        *lpbDelete = TRUE;
    }
    else
    if (lpstVirusSigInfo->wID != VID_MVP)
    {
        // Apply repair signature

        if (WDApplyRepair(&gstWD8SigSet,
                          lpstScan,
                          lpbDelete,
                          lpstVirusSigInfo -
                              gstWD8SigSet.lpastVirusSigInfo,
                          0) ==
            WD_STATUS_ERROR)
        {
            // Error applying repair signature

            return(FALSE);
        }
    }

    return(TRUE);
}


//********************************************************************
//
// Function:
//  EXTSTATUS WD8MacroVirusRepair()
//
// Description:
//  Repairs a file of Word 8.0 macro viruses.
//
// Returns:
//  EXTSTATUS_OK            If the repair was successful
//  EXTSTATUS_NO_REPAIR     If the repair was unsuccessful
//
//********************************************************************

EXTSTATUS WD8MacroVirusRepair
(
    LPMSOFFICE_SCAN     lpstOffcScan,
    LPBYTE              lpbyWorkBuffer,
    LPWD_VIRUS_SIG_INFO lpstVirusSigInfo
)
{
    BYTE                abyModuleName[SS_MAX_NAME_LEN+1];
    WORD                wModuleCount;

    BOOL                bDelete;
    BOOL                bMore;
    LPSS_ENUM_SIBS      lpstSibs;
    LPWD_SCAN           lpstScan;
    BOOL                bThisDocumentDeleted;
    BOOL                bIsThisDocument;

    lpstScan = lpstOffcScan->lpstScan;

    if (O97WDFindAssociatedStreams(lpstOffcScan->lpstRoot,
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

    // Iterate through the modules to delete

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

    bMore = TRUE;
    bThisDocumentDeleted = FALSE;
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

                if (WD8RepairModule(lpstOffcScan,
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
                    bIsThisDocument =
                        O97MemICmp(abyModuleName,
                                   (LPBYTE)gabyszThisDocument,
                                   sizeof(gabyszThisDocument));

                    // Delete the module if it is not "ThisDocument"
                    //  or it is "ThisDocument" and it has not
                    //  yet been deleted.

                    if (bIsThisDocument == FALSE ||
                        bThisDocumentDeleted == FALSE)
                    {
                        if (O97WDDeleteModule(lpstOffcScan->lpstRoot,
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
                    }

                    // Resync only if it is not "ThisDocument"

                    if (bIsThisDocument == FALSE)
                    {
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
                    else
                    {
                        // Indicate that this document has been deleted

                        bThisDocumentDeleted = TRUE;
                    }
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

    // Determine whether to get rid of the Macros storage

    if (O97WDCheckMacrosDeletable(lpstOffcScan->lpstRoot,
                                  lpstOffcScan->lpstStream,
                                  lpstScan->uScan.stVBA5.lpstLZNT,
                                  &lpstOffcScan->u.stO97,
                                  lpbyWorkBuffer) == TRUE)
    {
        // Delete the Macros storage

        if (SSUnlinkEntry(lpstOffcScan->lpstRoot,
                          lpstOffcScan->u.stO97.u.stWD8.
                              dwParentStorageEntry,
                          lpstOffcScan->u.stO97.u.stWD8.
                              dwMacrosEntry) != SS_STATUS_OK)
        {
            // Failed to delete Macros storage

            LZNTFreeStruct(lpstOffcScan->lpstRoot->lpvRootCookie,
                           lpstScan->uScan.stVBA5.lpstLZNT);

            return(EXTSTATUS_NO_REPAIR);
        }
    }

    LZNTFreeStruct(lpstOffcScan->lpstRoot->lpvRootCookie,
                   lpstScan->uScan.stVBA5.lpstLZNT);

    return(EXTSTATUS_OK);
}


//********************************************************************
//
// Function:
//  BOOL VBA5IsFullSet()
//
// Parameters:
//  lpstSigSet          Signature set to apply
//  lpstScan            Ptr to scan structure
//  wVBAChildEntry      Child entry of VBA storage
//  lpstOpenSib         OLE open sib structure
//
// Description:
//  Determines whether all macros are part of the full set.
//
// Returns:
//  TRUE        If all non-empty modules belong to the full set
//  FALSE       If at least one non-empty module does not belong
//              to the full set
//
//********************************************************************

BOOL VBA5IsFullSet
(
    LPSS_ROOT           lpstRoot,
    LPSS_STREAM         lpstStream,
    LPWD_SIG_SET        lpstSigSet,         // Signature set to apply
    LPWD_SCAN           lpstScan,           // Ptr to scan structure
    DWORD               dwVBAChildEntry,    // Child of VBA storage
    LPSS_ENUM_SIBS      lpstEnumSibs
)
{
    //////////////////////////////////////////////////////////////////
    // Iterate through modules to determine full set status
    //////////////////////////////////////////////////////////////////

    SSInitEnumSibsStruct(lpstEnumSibs,
                         dwVBAChildEntry);

    while (SSEnumSiblingEntriesCB(lpstRoot,
                                  O97OpenVBA5ModuleCB,
                                  lpstScan->abyName,
                                  lpstEnumSibs,
                                  lpstStream) == SS_STATUS_OK)
    {
        WDInitHitBitArrays(lpstSigSet,lpstScan);

        // Scan module

        VBA5ScanModule(lpstSigSet,lpstScan);

        if (O97LZNTStreamAttributeOnly(lpstScan->uScan.stVBA5.lpstLZNT) ==
            TRUE)
        {
            // Empty modules are ignored when checking for membership
            //  in the full set

            continue;
        }

        if ((lpstScan->wFlags & WD_SCAN_FLAG_MACRO_IS_FULL_SET) == 0)
        {
            // Found a macro that was not part of the full set,
            //  so just return

            return(FALSE);
        }
    }

    // At this point, all the modules have been verified to be part
    //  of the full set

    return(TRUE);
}


//********************************************************************
//
// Function:
//  EXTSTATUS WD8DoFullSetRepair()
//
// Description:
//  Performs a full set repair on a Word 97 document.
//
// Returns:
//  EXTSTATUS_OK            If the repair was successful
//  EXTSTATUS_NO_REPAIR     If the repair was unsuccessful
//
//********************************************************************

EXTSTATUS WD8DoFullSetRepair
(
    LPMSOFFICE_SCAN     lpstOffcScan,
    LPBYTE              lpbyWorkBuffer
)
{
    BOOL                bMore;
    LPWD_SCAN           lpstScan;
    LPSS_ENUM_SIBS      lpstSibs;
    BOOL                bThisDocumentDeleted;
    BOOL                bIsThisDocument;

    lpstScan = lpstOffcScan->lpstScan;

    // Allocate LZNT structure

    if (LZNTAllocStruct(lpstOffcScan->lpstRoot->lpvRootCookie,
                        &lpstScan->uScan.stVBA5.lpstLZNT) != TRUE)
    {
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

    // Assign hit memory

    if (AssignMacroSigHitMem(lpstOffcScan,
                             lpstScan) == FALSE)
    {
        LZNTFreeStruct(lpstOffcScan->lpstRoot->lpvRootCookie,
                       lpstScan->uScan.stVBA5.lpstLZNT);

        SSFreeEnumSibsStruct(lpstOffcScan->lpstRoot,lpstSibs);
        return(EXTSTATUS_NO_REPAIR);
    }


    /////////////////////////////////////////////////////////////
    // Initialize the scan structure
    /////////////////////////////////////////////////////////////

    lpstScan->lpstStream = lpstOffcScan->lpstStream;
    lpstScan->wScanType = WD_SCAN_TYPE_VBA5;

    // Check to see if it is a full set

    if (VBA5IsFullSet(lpstOffcScan->lpstRoot,
                      lpstOffcScan->lpstStream,
                      &gstWD8SigSet,
                      lpstScan,
                      lpstOffcScan->u.stO97.dwVBAChildEntry,
                      lpstSibs) == FALSE)
    {
        // The set of modules is not a full set

        LZNTFreeStruct(lpstOffcScan->lpstRoot->lpvRootCookie,
                       lpstScan->uScan.stVBA5.lpstLZNT);

        SSFreeEnumSibsStruct(lpstOffcScan->lpstRoot,lpstSibs);
        return(EXTSTATUS_OK);
    }

    // At this point, all the modules have been verified to be part
    //  of the full set, so delete them all

    if (O97WDFindAssociatedStreams(lpstOffcScan->lpstRoot,
                                   &lpstOffcScan->u.stO97) == FALSE)
    {
        // Could not find associated streams

        LZNTFreeStruct(lpstOffcScan->lpstRoot->lpvRootCookie,
                       lpstScan->uScan.stVBA5.lpstLZNT);

        SSFreeEnumSibsStruct(lpstOffcScan->lpstRoot,lpstSibs);
        return(EXTSTATUS_NO_REPAIR);
    }

    // Iterate through the modules to delete

    bMore = TRUE;
    bThisDocumentDeleted = FALSE;
    while (bMore)
    {
        SSInitEnumSibsStruct(lpstSibs,
                             lpstOffcScan->u.stO97.dwVBAChildEntry);

        while (1)
        {
            if (SSEnumSiblingEntriesCB(lpstOffcScan->lpstRoot,
                                       O97OpenVBA5ModuleCB,
                                       lpstScan->abyName,
                                       lpstSibs,
                                       lpstOffcScan->lpstStream) ==
                SS_STATUS_OK)
            {
                bIsThisDocument =
                    O97MemICmp(lpstScan->abyName,
                               (LPBYTE)gabyszThisDocument,
                               sizeof(gabyszThisDocument));

                // Delete the module if it is not "ThisDocument"
                //  or it is "ThisDocument" and it has not
                //  yet been deleted.

                if (bIsThisDocument == FALSE ||
                    bThisDocumentDeleted == FALSE)
                {
                    if (O97WDDeleteModule(lpstOffcScan->lpstRoot,
                                          lpstOffcScan->lpstStream,
                                          lpstScan->uScan.stVBA5.lpstLZNT,
                                          lpstScan->abyName,
                                          &lpstOffcScan->u.stO97,
                                          lpbyWorkBuffer) == FALSE)
                    {
                        // Error deleting the module

                        LZNTFreeStruct(lpstOffcScan->lpstRoot->lpvRootCookie,
                                       lpstScan->uScan.stVBA5.lpstLZNT);

                        SSFreeEnumSibsStruct(lpstOffcScan->lpstRoot,lpstSibs);
                        return(EXTSTATUS_NO_REPAIR);
                    }
                }


                // Resync only if it is not "ThisDocument"

                if (bIsThisDocument == FALSE)
                {
                    if (SSGetChildOfStorage(lpstOffcScan->lpstRoot,
                                             lpstOffcScan->u.stO97.dwVBAEntry,
                                             &lpstOffcScan->u.stO97.dwVBAChildEntry) !=
                        SS_STATUS_OK)
                    {
                        // Error getting the child of the VBA storage

                        LZNTFreeStruct(lpstOffcScan->lpstRoot->lpvRootCookie,
                                       lpstScan->uScan.stVBA5.lpstLZNT);

                        SSFreeEnumSibsStruct(lpstOffcScan->lpstRoot,lpstSibs);
                        return(EXTSTATUS_NO_REPAIR);
                    }

                    // Need to resynchronize

                    // Resynchronization assumes that repairs without
                    //  deletions will not do a blind repair.

                    break;
                }
                else
                {
                    // Indicate that this document has been deleted

                    bThisDocumentDeleted = TRUE;
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

    LZNTFreeStruct(lpstOffcScan->lpstRoot->lpvRootCookie,
                   lpstScan->uScan.stVBA5.lpstLZNT);

    SSFreeEnumSibsStruct(lpstOffcScan->lpstRoot,lpstSibs);
    return(EXTSTATUS_OK);
}

#endif  // #ifndef SYM_NLM

