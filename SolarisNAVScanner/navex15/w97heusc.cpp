//************************************************************************
//
// $Header:   S:/NAVEX/VCS/W97HEUSC.CPv   1.6   14 Dec 1998 18:52:58   DCHI  $
//
// Description:
//  Contains top-level Word 97 heuristic emulation logic.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/W97HEUSC.CPv  $
// 
//    Rev 1.6   14 Dec 1998 18:52:58   DCHI
// Changes for ThisDocument repair fix.
// 
//    Rev 1.5   09 Dec 1998 17:45:18   DCHI
// Added #include "gdefines.h"
// 
//    Rev 1.4   12 Oct 1998 13:42:42   DCHI
// - Added call to CopyIsEmpty() before perfomring global template
//   emulation if not doing repair.
// - Limited emulation to maximum of 1024 modules.
// 
//    Rev 1.3   11 May 1998 17:57:42   DCHI
// Changes for Access 2.0 scanning and Access datafile conversion.
// 
//    Rev 1.2   15 Apr 1998 17:24:24   DCHI
// Modifications for new XL95 and XL5 engines.
// 
//    Rev 1.1   10 Mar 1998 13:41:10   DCHI
// Modified to use new O97 CRC.
// 
//    Rev 1.0   15 Oct 1997 13:20:40   DCHI
// Initial revision.
// 
//************************************************************************

#include "gdefines.h"

#ifdef MACROHEU

#include "platform.h"
#include "ctsn.h"
#include "navex.h"

#include "wd7api.h"
#include "o97api.h"
#include "xl5api.h"
#include "acsapi.h"

#include "wdscan15.h"
#include "offcsc15.h"
#include "o97scn15.h"

#include "olestrnm.h"
#include "o97env.h"
#include "w97str.h"

#include "copy.h"

#include "mcrxclud.h"

//********************************************************************
//
// Function:
//  EXTSTATUS Word97HeuristicRepair()
//
// Parameters:
//  lpstOffcScan            Ptr to office scan structure
//  lpstSibs                Sibling enumeration structure
//  lpstEnv                 Ptr to environment structure
//  lpstLZNT                Ptr to allocated LZNT structure
//  lpabyName               Ptr to 256 byte buffer for names
//
// Description:
//  Repairs a file of Word 8.0 Macro viruses.
//
// Returns:
//	EXTSTATUS_OK			If no virus was found
//  EXTSTATUS_VIRUS_FOUND   If a virus was found
//  EXTSTATUS_NO_REPAIR     If the virus could not be repaired
//
//********************************************************************

EXTSTATUS Word97HeuristicRepair
(
    LPMSOFFICE_SCAN         lpstOffcScan,
    LPSS_ENUM_SIBS          lpstSibs,
    LPO97_ENV               lpstEnv,
    LPLZNT                  lpstLZNT,
    LPBYTE                  lpabyName
)
{
    EXTSTATUS               extStatus = EXTSTATUS_OK;

    int                     i;
    BYTE                    abyWorkBuffer[1024];
    BOOL                    bMore = TRUE;
    BOOL                    bThisDocumentDeleted;
    BOOL                    bIsThisDocument;

    // First find the associated streams

    if (O97WDFindAssociatedStreams(lpstOffcScan->lpstRoot,
                                   &lpstOffcScan->u.stO97) == FALSE)
        return(EXTSTATUS_NO_REPAIR);

    bThisDocumentDeleted = FALSE;
    while (bMore)
    {
        SSInitEnumSibsStruct(lpstSibs,
                             lpstOffcScan->u.stO97.dwVBAChildEntry);

        while (1)
        {
            if (SSEnumSiblingEntriesCB(lpstOffcScan->lpstRoot,
                                       O97OpenVBA5ModuleCB,
                                       lpabyName,
                                       lpstSibs,
                                       lpstOffcScan->lpstStream) ==
                SS_STATUS_OK)
            {
                // Copy the name as a Pascal string because
                //  CopyIsPartOfViralSet requires this

                for (i=0;i<255;i++)
                {
                    if (lpabyName[i] == 0)
                        break;

                    abyWorkBuffer[i+1] = lpabyName[i];
                }

                abyWorkBuffer[0] = i;

                // Now determine whether it is part of the viral set

                if (CopyIsPartOfViralSet((LPCOPY)lpstEnv->lpvContextData,
                                         abyWorkBuffer) == TRUE)
                {
                    bIsThisDocument =
                        O97MemICmp(lpabyName,
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
                                              lpstLZNT,
                                              lpabyName,
                                              &lpstOffcScan->u.stO97,
                                              abyWorkBuffer) == FALSE)
                        {
                            // Error deleting the module

                            extStatus = EXTSTATUS_NO_REPAIR;
                            bMore = FALSE;
                            break;
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

                            extStatus = EXTSTATUS_NO_REPAIR;
                            bMore = FALSE;
                            break;
                        }

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

    // Delete __SRP_ streams and increment second ushort
    //  of _VBA_PROJECT stream

    if (O97UpdateForRecompilation(lpstOffcScan->lpstRoot,
                                  lpstOffcScan->lpstStream,
                                  &lpstOffcScan->u.stO97) == FALSE)
    {
        // Error updating for recompilation

        extStatus = EXTSTATUS_NO_REPAIR;
    }

    // Determine whether to get rid of the Macros storage

    if (O97WDCheckMacrosDeletable(lpstOffcScan->lpstRoot,
                                  lpstOffcScan->lpstStream,
                                  lpstLZNT,
                                  &lpstOffcScan->u.stO97,
                                  abyWorkBuffer) == TRUE)
    {
        // Delete the Macros storage

        if (SSUnlinkEntry(lpstOffcScan->lpstRoot,
                          lpstOffcScan->u.stO97.u.stWD8.
                              dwParentStorageEntry,
                          lpstOffcScan->u.stO97.u.stWD8.
                              dwMacrosEntry) != SS_STATUS_OK)
        {
            // Failed to delete Macros storage

            extStatus = EXTSTATUS_NO_REPAIR;
        }
    }

    return(extStatus);
}


//********************************************************************
//
// Function:
//  EXTSTATUS Word97HeuristicScan()
//
// Parameters:
//  lpstOffcScan            Ptr to office scan structure
//  bRepair                 TRUE if repair should be applied
//
// Description:
//  Scans a file for the presence of Word 8.0 Macro viruses
//  and optionally repairs.
//
// Returns:
//	EXTSTATUS_OK			If no virus was found
//  EXTSTATUS_VIRUS_FOUND   If a virus was found
//  EXTSTATUS_NO_REPAIR     If the virus could not be repaired
//
//********************************************************************

extern O97_OBJECT_FUNC_T gstWord97ObjectFunc;

#define WD8_MAX_XCL_ASIZE   4
#define WD8_MAX_CACHED_XCL  (WD8_MAX_XCL_ASIZE * 8)

EXTSTATUS Word97HeuristicScan
(
    LPMSOFFICE_SCAN         lpstOffcScan,
    BOOL                    bRepair
)
{
    EXTSTATUS               extStatus = EXTSTATUS_OK;

    LPSS_ENUM_SIBS          lpstSibs;

    BYTE                    abyName[256];
    DWORD                   dwModuleIndex;
    LPO97_MODULE_ENV        lpstModEnv;
    LPO97_ENV               lpstEnv;
    O97_IP_T                stIP;
    LPLZNT                  lpstLZNT;
    DWORD                   dwCRC;
    DWORD                   dwCRCByteCount;
    BYTE                    abyExcludeHit[WD8_MAX_XCL_ASIZE];

    // Determine whether we are scanning WD8 or XL97

    /////////////////////////////////////////////////////////////
    // Allocate memory and initialize
    /////////////////////////////////////////////////////////////

    // Allocate a sibling enumeration structure

    if (SSAllocEnumSibsStruct(lpstOffcScan->lpstRoot,
                              &lpstSibs,
                              SS_DEF_MAX_WAITING_SIBS) != SS_STATUS_OK)
        return(FALSE);

    // Allocate the environment structure

    lpstEnv = O97EnvCreate(lpstOffcScan->lpstRoot->lpvRootCookie,
                           NUM_W97_STR);

    if (lpstEnv == NULL)
    {
        SSFreeEnumSibsStruct(lpstOffcScan->lpstRoot,lpstSibs);
        return(EXTSTATUS_OK);
    }

    /////////////////////////////////////////////////////////////
    // Iterate through the children of the VBA storage
    /////////////////////////////////////////////////////////////

    // Find the <VBA> storage

    if (O97OpenVBAStorage(lpstOffcScan->lpstRoot,
                          &lpstOffcScan->u.stO97) == FALSE)
    {
        // No VBA storage!

        O97EnvDestroy(lpstOffcScan->lpstRoot->lpvRootCookie,lpstEnv);
        SSFreeEnumSibsStruct(lpstOffcScan->lpstRoot,lpstSibs);
        return(EXTSTATUS_OK);
    }

    if (O97EnvInit(lpstEnv,
                   lpstOffcScan->lpstRoot,
                   lpstOffcScan->u.stO97.dwVBAChildEntry,
                   &gstWord97ObjectFunc,
                   galpbyW97_STR) == FALSE)
    {
        O97EnvDestroy(lpstOffcScan->lpstRoot->lpvRootCookie,lpstEnv);
        SSFreeEnumSibsStruct(lpstOffcScan->lpstRoot,lpstSibs);
        return(EXTSTATUS_OK);
    }

    // Create a module environment structure

    lpstModEnv = O97ModuleEnvCreate(lpstEnv);
    if (lpstModEnv == NULL)
    {
        O97EnvDestroy(lpstOffcScan->lpstRoot->lpvRootCookie,lpstEnv);
        SSFreeEnumSibsStruct(lpstOffcScan->lpstRoot,lpstSibs);
        return(EXTSTATUS_OK);
    }

    //////////////////////////////////////////////////////////////////
    // Create and initialize copy log structure

    lpstEnv->lpvContextData =
        CopyCreate(lpstOffcScan->lpstRoot->lpvRootCookie);

    if (lpstEnv->lpvContextData == NULL)
    {
        // Error creating copy structure

        O97ModuleEnvDestroy(lpstEnv,lpstModEnv);
        O97EnvDestroy(lpstOffcScan->lpstRoot->lpvRootCookie,lpstEnv);
        SSFreeEnumSibsStruct(lpstOffcScan->lpstRoot,lpstSibs);
        return(EXTSTATUS_OK);
    }

    if (CopyInit((LPCOPY)lpstEnv->lpvContextData) == FALSE)
    {
        // Error initializing copy structure

        CopyDestroy(lpstOffcScan->lpstRoot->lpvRootCookie,
                    (LPCOPY)lpstEnv->lpvContextData);

        O97ModuleEnvDestroy(lpstEnv,lpstModEnv);
        O97EnvDestroy(lpstOffcScan->lpstRoot->lpvRootCookie,lpstEnv);
        SSFreeEnumSibsStruct(lpstOffcScan->lpstRoot,lpstSibs);
        return(EXTSTATUS_OK);
    }

    //////////////////////////////////////////////////////////////////
    // Allocate LZNT structure

    if (LZNTAllocStruct(lpstOffcScan->lpstRoot->lpvRootCookie,
                        &lpstLZNT) == FALSE)
    {
        // Error allocating LZNT structure

        CopyDestroy(lpstOffcScan->lpstRoot->lpvRootCookie,
                    (LPCOPY)lpstEnv->lpvContextData);

        O97ModuleEnvDestroy(lpstEnv,lpstModEnv);
        O97EnvDestroy(lpstOffcScan->lpstRoot->lpvRootCookie,lpstEnv);
        SSFreeEnumSibsStruct(lpstOffcScan->lpstRoot,lpstSibs);
        return(EXTSTATUS_OK);
    }

    //////////////////////////////////////////////////////////////////
    // Execute as a local document

    lpstEnv->bNormal = FALSE;

    // Start with the VBA child entry

    SSInitEnumSibsStruct(lpstSibs,
                         lpstOffcScan->u.stO97.dwVBAChildEntry);

    // Initialize exclusion array to no hits

    for (dwModuleIndex=0;dwModuleIndex<WD8_MAX_XCL_ASIZE;dwModuleIndex++)
        abyExcludeHit[dwModuleIndex] = 0;

    for (dwModuleIndex=0;dwModuleIndex<1024;dwModuleIndex++)
    {
        if (SSEnumSiblingEntriesCB(lpstOffcScan->lpstRoot,
                                   O97OpenVBA5ModuleCB,
                                   abyName,
                                   lpstSibs,
                                   lpstModEnv->lpstStream) != SS_STATUS_OK)
            break;

        //printf("Module name: %s\n",abyName);

        // First get a CRC of the module and check to see whether
        //  to exclude the macro from emulation

        if (O97ModuleSourceCRC2(lpstModEnv->lpstStream,
                                lpstLZNT,
                                &dwCRC,
                                &dwCRCByteCount) == TRUE)
        {
            if (MacroIsExcluded(MACRO_EXCLUDE_TYPE_WD8,
                                dwCRC,
                                dwCRCByteCount) == TRUE)
            {
                // Exclude this macro from emulation

                if (dwModuleIndex < WD8_MAX_CACHED_XCL)
                    abyExcludeHit[dwModuleIndex >> 3] |=
                        1 << (dwModuleIndex & 7);

                continue;
            }
        }

        // Initialize the module environment

        if (O97ModuleEnvInit(lpstModEnv,
                             dwModuleIndex,
                             SSStreamID(lpstModEnv->lpstStream)) == FALSE)
        {
            //printf("Failed to init module %lu access\n",dwModuleIndex);
            continue;
        }

        // Iterate through the subroutines

        stIP.wLine = stIP.wOffset = 0xFFFF;
        while (1)
        {
            if (O97ModuleEnvFindNextSub(lpstModEnv,
                                        &stIP) == FALSE)
            {
                //printf("Error getting IP\n");
                break;
            }

            if (stIP.wLine == 0xFFFF && stIP.wOffset == 0xFFFF)
                break;

            //printf("Sub at line %u offset %u\n",stIP.wLine,stIP.wOffset);

            if (O97Execute(lpstEnv,
                           lpstModEnv,
                           &stIP) == FALSE)
            {
                //printf("Error executing\n");
                break;
            }

            // Skip past the sub

            stIP.wOffset += 6;
        }

        //printf("Module %lu\n",dwModuleIndex);
    }

    //////////////////////////////////////////////////////////////////
    // Execute as the global template

    if (CopyIsEmpty((LPCOPY)lpstEnv->lpvContextData) == FALSE ||
        bRepair == TRUE)
    {
        lpstEnv->bNormal = TRUE;

        // Start with the VBA child entry

        SSInitEnumSibsStruct(lpstSibs,
                             lpstOffcScan->u.stO97.dwVBAChildEntry);

        for (dwModuleIndex=0;dwModuleIndex<1024;dwModuleIndex++)
        {
            if (SSEnumSiblingEntriesCB(lpstOffcScan->lpstRoot,
                                       O97OpenVBA5ModuleCB,
                                       abyName,
                                       lpstSibs,
                                       lpstModEnv->lpstStream) != SS_STATUS_OK)
                break;

            //printf("Module name: %s\n",abyName);

            // Check for emulation exclusion

            if (dwModuleIndex < WD8_MAX_CACHED_XCL)
            {
                if (abyExcludeHit[dwModuleIndex >> 3] & (1 << (dwModuleIndex & 7)))
                {
                    // Macro has been excluded from emulation

                    continue;
                }
            }
            else
            {
                // Get a CRC of the macro and check to see whether
                //  to exclude the macro from emulation

                if (O97ModuleSourceCRC2(lpstModEnv->lpstStream,
                                        lpstLZNT,
                                        &dwCRC,
                                        &dwCRCByteCount) == TRUE)
                {
                    if (MacroIsExcluded(MACRO_EXCLUDE_TYPE_WD8,
                                        dwCRC,
                                        dwCRCByteCount) == TRUE)
                    {
                        // Exclude this macro from emulation

                        continue;
                    }
                }
            }

            // Initialize the module environment

            if (O97ModuleEnvInit(lpstModEnv,
                                 dwModuleIndex,
                                 SSStreamID(lpstModEnv->lpstStream)) == FALSE)
            {
                //printf("Failed to init module %lu access\n",dwModuleIndex);
                continue;
            }

            // Iterate through the subroutines

            stIP.wLine = stIP.wOffset = 0xFFFF;
            while (1)
            {
                if (O97ModuleEnvFindNextSub(lpstModEnv,
                                            &stIP) == FALSE)
                {
                    //printf("Error getting IP\n");
                    break;
                }

                if (stIP.wLine == 0xFFFF && stIP.wOffset == 0xFFFF)
                    break;

                //printf("Sub at line %u offset %u\n",stIP.wLine,stIP.wOffset);

                if (O97Execute(lpstEnv,
                               lpstModEnv,
                               &stIP) == FALSE)
                {
                    //printf("Error executing\n");
                    break;
                }

                // Skip past the sub

                stIP.wOffset += 6;
            }

            //printf("Module %lu\n",dwModuleIndex);
        }
    }

//    CopyPrint((LPCOPY)lpstEnv->lpvContextData);
/*
    if (CopyIsViral((LPCOPY)lpstEnv->lpvContextData) == FALSE)
        printf("-------Heuristically *NOT* viral!\n");
    else
        printf("-------Heuristically *IS* viral!\n");
*/
    if (bRepair == TRUE)
    {
        extStatus = Word97HeuristicRepair(lpstOffcScan,
                                          lpstSibs,
                                          lpstEnv,
                                          lpstLZNT,
                                          abyName);
    }
    else
    {
        if (CopyIsViral((LPCOPY)lpstEnv->lpvContextData) != FALSE)
            extStatus = EXTSTATUS_VIRUS_FOUND;
    }

    LZNTFreeStruct(lpstOffcScan->lpstRoot->lpvRootCookie,
                   lpstLZNT);

    CopyDestroy(lpstOffcScan->lpstRoot->lpvRootCookie,
                (LPCOPY)lpstEnv->lpvContextData);

    O97ModuleEnvDestroy(lpstEnv,lpstModEnv);
    O97EnvDestroy(lpstOffcScan->lpstRoot->lpvRootCookie,lpstEnv);
    SSFreeEnumSibsStruct(lpstOffcScan->lpstRoot,lpstSibs);

    return(extStatus);
}

#endif // #ifdef MACROHEU

