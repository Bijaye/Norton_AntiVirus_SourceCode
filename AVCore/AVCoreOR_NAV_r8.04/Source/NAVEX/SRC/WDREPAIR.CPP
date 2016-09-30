//************************************************************************
//
// $Header:   S:/NAVEX/VCS/wdrepair.cpv   1.10   13 May 1997 14:48:50   DCHI  $
//
// Description:
//  Top-level repair module.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/wdrepair.cpv  $
// 
//    Rev 1.10   13 May 1997 14:48:50   DCHI
// Corrected problem with MVP repair where crashing would result because
// MVP is a special case that doesn't have a repair signature.
// 
//    Rev 1.9   12 May 1997 16:40:34   DCHI
// Modifications so that MVP repair is always applied regardless of which
// virus was detected.
// 
//    Rev 1.8   10 Apr 1997 16:51:48   DCHI
// Fixed FullSet checking.
// 
//    Rev 1.7   09 Apr 1997 11:39:44   DCHI
// Corrected IsFullSet() function to verify that the sig info structure
// has a valid pointer before dereferencing for checking.
// 
//    Rev 1.6   08 Apr 1997 12:40:52   DCHI
// Added support for FullSet(), FullSetRepair, Or()/Not(), MacroCount(), etc.
// 
//    Rev 1.5   07 Apr 1997 18:11:16   DCHI
// Added MVP support.
// 
//    Rev 1.4   14 Mar 1997 16:35:08   DCHI
// Added support for Office 97 repair.
// 
//    Rev 1.3   13 Feb 1997 13:34:46   DCHI
// Modifications to support VBA 5 scanning.
// 
//    Rev 1.2   28 Jan 1997 15:43:18   AOONWAL
// Modified during FEB97 build
// 
//    Rev FEB97 23 Jan 1997 11:50:56   DCHI
// Fixed global variable usage problem.
// 
//    Rev 1.0   17 Jan 1997 11:23:46   DCHI
// Initial revision.
// 
//************************************************************************

#ifndef SYM_NLM

#include "storobj.h"
#include "wdencdoc.h"
#include "worddoc.h"
#include "macrodel.h"
#include "wdscan.h"
#include "wdsigcmd.h"
#include "wdapsig.h"
#include "wdapvsig.h"
#include "wdsigutl.h"
#include "wdrepair.h"
#include "mvp.h"

//*************************************************************************
//
// int WDGetSigLen()
//
// Parameters:
//  lpabySig            Ptr to signature
//
// Description:
//  Calculates the length of the given signature.
//
// Returns:
//  int                 Length of signature
//
//*************************************************************************

int WDGetSigLen
(
    LPBYTE              lpabySig
)
{
    int                 nCommand;
    int                 nNibble;
    int                 nControlStreamLen;
    int                 nControlStreamIndex;
    LPBYTE              lpabyControlStream;
    int                 nDataStreamLen;

    nControlStreamLen = lpabySig[0];
    lpabyControlStream = lpabySig + 1;

    // Multiply by two to get number of control nibbles

    nControlStreamLen *= 2;

    nDataStreamLen = 0;
    nControlStreamIndex = 0;
    while (nControlStreamIndex < nControlStreamLen)
    {
        nCommand = WDGetControlStreamNibble(lpabyControlStream,
                                            &nControlStreamIndex);

        switch (nCommand)
        {
            case VNIB0_NAME_SIG_BYTE:
            case VNIB0_NAMED_CRC_SIG_BYTE:
            case VNIB0_MACRO_SIG_BYTE:
            case VNIB0_CRC_BYTE:
            case VNIB0_USE_OTHER_REPAIR_BYTE:
                ++nDataStreamLen;
                break;

            case VNIB0_NAME_SIG_WORD:
            case VNIB0_NAMED_CRC_SIG_WORD:
            case VNIB0_MACRO_SIG_WORD:
            case VNIB0_CRC_WORD:
            case VNIB0_USE_OTHER_REPAIR_WORD:
                nDataStreamLen += 2;
                break;

            case VNIB0_AND:
            case VNIB0_OR:
            case VNIB0_NOT:
            case VNIB0_USE_SIGNATURE:
                break;

            case VNIB0_MISC:
            {
                nNibble = WDGetControlStreamNibble(lpabyControlStream,
                                                   &nControlStreamIndex);

                switch (nNibble)
                {
                    case VNIB1_CUSTOM_REPAIR_BYTE:
                        ++nDataStreamLen;
                        break;

                    case VNIB1_CUSTOM_REPAIR_WORD:
                        nDataStreamLen += 2;
                        break;

                    default:
                        // This should never happen
                        break;
                }

                break;
            }

            case VNIB0_END:
                // NOP
                break;

            default:
                // This should never happen
                break;
        }
    }

    return(1 + (nControlStreamLen >> 1) + nDataStreamLen);
}


//*************************************************************************
//
// WD_STATUS WDApplyRepair()
//
// Parameters:
//  lpstSigSet              Signature set to apply
//  lpstScan                Ptr to scan structure
//  lpbDelete               Ptr to BOOL, set to TRUE if the
//                              macro should be deleted
//  wID                     Index of virus sig info for repair
//
// Description:
//  Applies the repair signature for the given virus.
//
// Returns:
//  WD_STATUS_REPAIRED      If a modification was made
//  WD_STATUS_ERROR         If repair failed
//  WD_STATUS_OK            If repair succeeded
//
//*************************************************************************

WD_STATUS WDApplyRepair
(
    LPWD_SIG_SET        lpstSigSet,
    LPWD_SCAN           lpstScan,
    LPBOOL              lpbDelete,
    WORD                wID,
    int                 nDepth
)
{
    LPBYTE              lpabySig;
    LPBYTE              lpabyRepairSig;
    BOOL                bDelete;
    WORD                wRepairID;
    WORD                wdStatus;

    if (nDepth > 8)
    {
        // Exceeded maximum recursive depth

        return(WD_STATUS_ERROR);
    }

    lpabySig = lpstSigSet->lpastVirusSigInfo[wID].lpabySig;

    // Jump to the first repair signature

    lpabyRepairSig = lpabySig + WDGetSigLen(lpabySig);

    // Iterate through repair signatures until the macro is deleted
    //  or all repair signatures have been exhausted.

    *lpbDelete = FALSE;
    while (*lpabyRepairSig != 0)
    {
        wdStatus = WDApplyVirusSig(lpabyRepairSig,
                                   lpstSigSet,
                                   lpstScan,
                                   lpstScan->abyRunBuf,
                                   &wRepairID);

        bDelete = FALSE;
        switch (wdStatus)
        {
            case WD_STATUS_SIG_HIT:
                bDelete = TRUE;
                break;

            case WD_STATUS_REPAIR_USE_SIG:
                if (WDApplyUseSigRepair(lpabySig,
                                        lpstSigSet,
                                        lpstScan) == WD_STATUS_SIG_HIT)
                    bDelete = TRUE;

                break;

            case WD_STATUS_REPAIR_USE_OTHER:
                // Recursively call

                wdStatus = WDApplyRepair(lpstSigSet,
                                         lpstScan,
                                         &bDelete,
                                         wRepairID,
                                         nDepth + 1);

                if (wdStatus == WD_STATUS_REPAIRED)
                {
                    *lpbDelete = bDelete;
                    return(WD_STATUS_REPAIRED);
                }

                if (wdStatus == WD_STATUS_ERROR)
                    return(WD_STATUS_ERROR);

                break;

            case WD_STATUS_REPAIR_CUSTOM:
                wdStatus = lpstSigSet->lpapfCustomRepair[wRepairID]
                    (lpstSigSet,lpstScan,&bDelete,wID);

                if (wdStatus == WD_STATUS_REPAIRED)
                {
                    *lpbDelete = bDelete;
                    return(WD_STATUS_REPAIRED);
                }

                if (wdStatus == WD_STATUS_ERROR)
                    return(WD_STATUS_ERROR);

                break;

            default:
                // Nothing to do

                break;
        }

        if (bDelete == TRUE)
        {
            // Delete the macro

            *lpbDelete = TRUE;
            return(WD_STATUS_OK);
        }

        lpabyRepairSig += WDGetSigLen(lpabyRepairSig);
    }

    return(WD_STATUS_OK);
}


//*************************************************************************
//
// WD_STATUS WDRepairDoc()
//
// Parameters:
//  lpstSigSet              Signature set to apply
//  lpstScan                Ptr to scan structure
//  lpstVirusSigInfo        Ptr to virus sig info for repair
//
// Description:
//  Assumes lpstScan->lpstCallBack and lpstScan->lpstOLEStream are set.
//
//  1. For each macro:
//      a. Initialize global hit bit arrays
//      b. Apply all repair sigs of the virus at the given index
//      c. Delete the macro if necessary
//
// Returns:
//  WD_STATUS_ERROR         If repair failed
//  WD_STATUS_OK            If repair succeeded
//
//*************************************************************************

WD_STATUS WDRepairDoc
(
    LPWD_SIG_SET        lpstSigSet,
    LPWD_SCAN           lpstScan,
    LPWD_VIRUS_SIG_INFO lpstVirusSigInfo
)
{
    WORD                wMacroIdx;
    MACRO_INFO_T        stMacroInfo;
    WD_GET_MACRO_INFO_T stGetInfo;
    BOOL                bDelete;

    // Get the number of active macros

    if (WordDocCountActiveMacros(lpstScan->lpstCallBack,
                                 lpstScan->lpstOLEStream,
                                 lpstScan->uScan.stWD7.lpstKey,
                                 lpstScan->uScan.stWD7.lpstMacroTableInfo,
                                 &lpstScan->wMacroCount,
                                 lpstScan->abyRunBuf) != WORDDOC_OK)
    {
        return(WD_STATUS_ERROR);
    }

    //////////////////////////////////////////////////////////////////
    // Iterate through macros
    //////////////////////////////////////////////////////////////////

    lpstScan->uScan.stWD7.lpstMacroInfo = &stMacroInfo;
    stGetInfo.bUseIndex = TRUE;
    stGetInfo.wNextNameIndex = 0;
    for (wMacroIdx=0;
         wMacroIdx<lpstScan->uScan.stWD7.lpstMacroTableInfo->wNumMacros;
         wMacroIdx++)
    {
        // Initialize global hit bit arrays

        WDInitHitBitArrays(lpstSigSet,lpstScan);

        // Get the macro info

        if (WordDocGetMacroInfoAtIndex(lpstScan->lpstCallBack,
                                       &stGetInfo,
                                       lpstScan->lpstOLEStream,
                                       lpstScan->uScan.stWD7.lpstKey,
                                       lpstScan->uScan.stWD7.
                                           lpstMacroTableInfo,
                                       &stMacroInfo,
                                       lpstScan->abyRunBuf,
                                       (LPSTR)lpstScan->abyName) != WORDDOC_OK)
        {
            // Error getting this macro, go to the next one

            continue;
        }

        stGetInfo.bUseIndex = FALSE;

        // Scan the macro

        if (WDScanMacro(lpstSigSet,
                        lpstScan) != WD_STATUS_OK)
        {
            // Error scanning macro, go to the next one

            continue;
        }

        // Default is don't delete

        bDelete = FALSE;

        if (MVPCheck(MVP_WD7,
                     lpstScan->abyName,
                     lpstScan->dwCRC) == FALSE)
        {
            // Non-approved macro

            bDelete = TRUE;
        }
        else
        if (lpstVirusSigInfo->wID != VID_MVP)
        {
            if (WDApplyRepair(lpstSigSet,
                              lpstScan,
                              &bDelete,
                              lpstVirusSigInfo -
                                  lpstSigSet->lpastVirusSigInfo,
                              0) == WD_STATUS_ERROR)
            {
                // Error repairing

                return(WD_STATUS_ERROR);
            }
        }

        // Delete the macro if necessary

        if (bDelete == TRUE)
        {
            if (WordDocDeleteMacro(lpstScan->lpstCallBack,
                                   lpstScan->lpstOLEStream,
                                   lpstScan->uScan.stWD7.lpstKey,
                                   lpstScan->uScan.stWD7.lpstMacroTableInfo,
                                   lpstScan->uScan.stWD7.lpstMacroInfo,
                                   lpstScan->abyRunBuf) != WORDDOC_OK)
                return(WD_STATUS_ERROR);
        }
    }

    return(WD_STATUS_OK);
}


//*************************************************************************
//
// BOOL WDIsFullSetRepair()
//
// Parameters:
//  lpstVirusSigInfo        Ptr to virus sig info to check
//
// Description:
//  Determines whether the repair of the given virus sig info begins
//  with a FullSetRepair.
//
// Returns:
//  TRUE        If the repair begins with a FullSetRepair
//  WD_STATUS_REPAIRED      If a modification was made
//  WD_STATUS_ERROR         If repair failed
//  WD_STATUS_OK            If repair succeeded
//
//*************************************************************************

BOOL WDIsFullSetRepair
(
    LPWD_VIRUS_SIG_INFO lpstVirusSigInfo
)
{
    LPBYTE              lpabySig;
    LPBYTE              lpabyRepairSig;

    lpabySig = lpstVirusSigInfo->lpabySig;

    if (lpabySig == NULL)
    {
        // No repair signature, must be special

        return(FALSE);
    }

    // Jump to the first repair signature

    lpabyRepairSig = lpabySig + WDGetSigLen(lpabySig);

    // Check first repair literal

    if (lpabyRepairSig[0] > 0 &&
        lpabyRepairSig[1] ==
        ((VNIB1_FULL_SET_REPAIR << 4) | VNIB0_MISC))
    {
        // It is a FullSetRepair

        return(TRUE);
    }

    return(FALSE);
}


//*************************************************************************
//
// WD_STATUS WDDoFullSetRepair()
//
// Parameters:
//  lpstSigSet              Signature set to apply
//  lpstScan                Ptr to scan structure
//
// Description:
//  The function first iterates through all macros to determine whether
//  all macros are part of the full set.
//
//  If all macros are part of the full set, then the function deletes
//  all the macros.
//
// Returns:
//  WD_STATUS_ERROR         If repair failed
//  WD_STATUS_OK            If repair succeeded
//
//*************************************************************************

WD_STATUS WDDoFullSetRepair
(
    LPWD_SIG_SET        lpstSigSet,
    LPWD_SCAN           lpstScan
)
{
    WORD                wMacroIdx;
    MACRO_INFO_T        stMacroInfo;
    WD_GET_MACRO_INFO_T stGetInfo;

    //////////////////////////////////////////////////////////////////
    // Iterate through macros to determine full set status
    //////////////////////////////////////////////////////////////////

    lpstScan->uScan.stWD7.lpstMacroInfo = &stMacroInfo;
    stGetInfo.bUseIndex = TRUE;
    stGetInfo.wNextNameIndex = 0;
    for (wMacroIdx=0;
         wMacroIdx<lpstScan->uScan.stWD7.lpstMacroTableInfo->wNumMacros;
         wMacroIdx++)
    {
        // Initialize global hit bit arrays

        WDInitHitBitArrays(lpstSigSet,lpstScan);

        // Get the macro info

        if (WordDocGetMacroInfoAtIndex(lpstScan->lpstCallBack,
                                       &stGetInfo,
                                       lpstScan->lpstOLEStream,
                                       lpstScan->uScan.stWD7.lpstKey,
                                       lpstScan->uScan.stWD7.
                                           lpstMacroTableInfo,
                                       &stMacroInfo,
                                       lpstScan->abyRunBuf,
                                       (LPSTR)lpstScan->abyName) != WORDDOC_OK)
        {
            // Error getting this macro, go to the next one

            continue;
        }

        stGetInfo.bUseIndex = FALSE;

        // Scan the macro

        if (WDScanMacro(lpstSigSet,
                        lpstScan) != WD_STATUS_OK)
        {
            // Error scanning macro, go to the next one

            continue;
        }

        // Is it part of the full set?

        if ((lpstScan->wFlags & WD_SCAN_FLAG_MACRO_IS_FULL_SET) == 0)
        {
            // Found a macro that was not part of the full set,
            //  so just return

            return(WD_STATUS_OK);
        }
    }

    //////////////////////////////////////////////////////////////////
    // At this point, all the macros have been verified to be part
    //  of the full set, so delete them all
    //////////////////////////////////////////////////////////////////

    lpstScan->uScan.stWD7.lpstMacroInfo = &stMacroInfo;
    stGetInfo.bUseIndex = TRUE;
    stGetInfo.wNextNameIndex = 0;
    for (wMacroIdx=0;
         wMacroIdx<lpstScan->uScan.stWD7.lpstMacroTableInfo->wNumMacros;
         wMacroIdx++)
    {
        // Get the macro info

        if (WordDocGetMacroInfoAtIndex(lpstScan->lpstCallBack,
                                       &stGetInfo,
                                       lpstScan->lpstOLEStream,
                                       lpstScan->uScan.stWD7.lpstKey,
                                       lpstScan->uScan.stWD7.
                                           lpstMacroTableInfo,
                                       &stMacroInfo,
                                       lpstScan->abyRunBuf,
                                       (LPSTR)lpstScan->abyName) != WORDDOC_OK)
        {
            // Error getting this macro, go to the next one

            continue;
        }

        stGetInfo.bUseIndex = FALSE;

        if (WordDocDeleteMacro(lpstScan->lpstCallBack,
                               lpstScan->lpstOLEStream,
                               lpstScan->uScan.stWD7.lpstKey,
                               lpstScan->uScan.stWD7.lpstMacroTableInfo,
                               lpstScan->uScan.stWD7.lpstMacroInfo,
                               lpstScan->abyRunBuf) != WORDDOC_OK)
        {
            // Error deleting the macro

            return(WD_STATUS_ERROR);
        }
    }

    return(WD_STATUS_OK);
}

#endif  // #ifndef SYM_NLM



