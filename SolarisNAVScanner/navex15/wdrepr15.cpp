//************************************************************************
//
// $Header:   S:/NAVEX/VCS/wdrepr15.cpv   1.3   11 May 1998 18:17:50   DCHI  $
//
// Description:
//  Top-level repair module.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/wdrepr15.cpv  $
// 
//    Rev 1.3   11 May 1998 18:17:50   DCHI
// Changes for menu repair and
// for Access 2.0 scanning and Access datafile conversion.
// 
//    Rev 1.2   15 Apr 1998 17:24:40   DCHI
// Modifications for new XL95 and XL5 engines.
// 
//    Rev 1.1   10 Jul 1997 17:49:06   DDREW
// Turn on NLM repairs for NAVEX15
// 
//    Rev 1.0   09 Jul 1997 16:25:26   DCHI
// Initial revision.
// 
//************************************************************************

#if defined(NAVEX15) || !defined(SYM_NLM)

#include "wd7api.h"
#include "o97api.h"
#include "xl5api.h"
#include "acsapi.h"
#include "wdscan15.h"
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
                    case VNIB1_MACRO_COUNT_EQ:
                    case VNIB1_MACRO_COUNT_LT:
                    case VNIB1_MACRO_COUNT_GT:
                    case VNIB1_CUSTOM_REPAIR_BYTE:
                        ++nDataStreamLen;
                        break;

                    case VNIB1_CUSTOM_REPAIR_WORD:
                        nDataStreamLen += 2;
                        break;

                    case VNIB1_FULL_SET_REPAIR:
                    case VNIB1_FULL_SET:
                    case VNIB1_HEURISTIC_REPAIR:
                        break;

                    case VNIB1_MENU_REPAIR:
                        nDataStreamLen += 1 +
                            2 * lpabySig[1 + (nControlStreamLen >> 1) + nDataStreamLen];
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
    BOOL                bDelete;
    WD7_TDT_INFO_T      stTDTInfo;
    BOOL                bActive;

    //////////////////////////////////////////////////////////////////
    // Get the number of active macros
    //////////////////////////////////////////////////////////////////

    if (WD7InitTDTInfo(lpstScan->lpstStream,
                       lpstScan->uScan.stWD7.lpstMacroScan->lpstKey,
                       &stTDTInfo) != WD7_STATUS_OK)
    {
        // Error getting TDT info

        return(WD_STATUS_ERROR);
    }

    if (WD7CountActiveMacros(&stTDTInfo,
                             &lpstScan->wMacroCount) != WD7_STATUS_OK)
    {
        // Error counting number of active macros

        return(WD_STATUS_ERROR);
    }


    //////////////////////////////////////////////////////////////////
    // Iterate through macros
    //////////////////////////////////////////////////////////////////

    for (wMacroIdx=0;wMacroIdx<stTDTInfo.wNumMCDs;wMacroIdx++)
    {
        // Initialize global hit bit arrays

        WDInitHitBitArrays(lpstSigSet,lpstScan);

        // Get the macro info

        if (WD7GetMacroInfoAtIndex(&stTDTInfo,
                                   wMacroIdx,
                                   lpstScan->abyName,
                                   &lpstScan->uScan.stWD7.dwMacroOffset,
                                   &lpstScan->uScan.stWD7.dwMacroSize,
                                   &lpstScan->uScan.stWD7.byMacroEncryptByte,
                                   &bActive) == WD7_STATUS_ERROR ||
            bActive == FALSE)
        {
            // Error getting this macro or not an active macro,
            //  go to the next one

            continue;
        }

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
            if (WD7DeactivateMacroAtIndex(&stTDTInfo,
                                          wMacroIdx) != WD7_STATUS_OK)
            {
                // Error deleting the macro

                return(WD_STATUS_ERROR);
            }
        }
    }

    return(WD_STATUS_OK);
}


//*************************************************************************
//
// BOOL WDIsHeuristicRepair()
//
// Parameters:
//  lpstVirusSigInfo    Ptr to virus signature info
//
// Description:
//  Determines whether the repair signature calls HeuristicRepair.
//
// Returns:
//  int                 Length of signature
//
//*************************************************************************

BOOL WDIsHeuristicRepair
(
    LPWD_VIRUS_SIG_INFO lpstVirusSigInfo
)
{
    int                 nCommand;
    int                 nNibble;
    int                 nControlStreamLen;
    int                 nControlStreamIndex;
    LPBYTE              lpabyControlStream;
    int                 nDataStreamLen;

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
    while (*lpabyRepairSig != 0)
    {
        nControlStreamLen = lpabyRepairSig[0];
        lpabyControlStream = lpabyRepairSig + 1;

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
                        case VNIB1_MACRO_COUNT_EQ:
                        case VNIB1_MACRO_COUNT_LT:
                        case VNIB1_MACRO_COUNT_GT:
                        case VNIB1_CUSTOM_REPAIR_BYTE:
                            ++nDataStreamLen;
                            break;

                        case VNIB1_CUSTOM_REPAIR_WORD:
                            nDataStreamLen += 2;
                            break;

                        case VNIB1_FULL_SET_REPAIR:
                        case VNIB1_FULL_SET:
                            break;

                        case VNIB1_HEURISTIC_REPAIR:
                            return(TRUE);

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

        lpabyRepairSig +=
            (1 + (nControlStreamLen >> 1) + nDataStreamLen);
    }

    return(FALSE);
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
    WD7_TDT_INFO_T      stTDTInfo;
    BOOL                bActive;

    // Initialize TDT structure

    if (WD7InitTDTInfo(lpstScan->lpstStream,
                       lpstScan->uScan.stWD7.lpstMacroScan->lpstKey,
                       &stTDTInfo) != WD7_STATUS_OK)
    {
        // Error getting TDT info

        return(WD_STATUS_ERROR);
    }

    if (WD7CountActiveMacros(&stTDTInfo,
                             &lpstScan->wMacroCount) != WD7_STATUS_OK)
    {
        // Error counting number of active macros

        return(WD_STATUS_ERROR);
    }


    //////////////////////////////////////////////////////////////////
    // Iterate through macros to determine full set status
    //////////////////////////////////////////////////////////////////

    for (wMacroIdx=0;wMacroIdx<stTDTInfo.wNumMCDs;wMacroIdx++)
    {
        // Initialize global hit bit arrays

        WDInitHitBitArrays(lpstSigSet,lpstScan);

        // Get the macro info

        if (WD7GetMacroInfoAtIndex(&stTDTInfo,
                                   wMacroIdx,
                                   lpstScan->abyName,
                                   &lpstScan->uScan.stWD7.dwMacroOffset,
                                   &lpstScan->uScan.stWD7.dwMacroSize,
                                   &lpstScan->uScan.stWD7.byMacroEncryptByte,
                                   &bActive) == WD7_STATUS_ERROR ||
            bActive == FALSE)
        {
            // Error getting this macro or not an active macro,
            //  go to the next one

            continue;
        }

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

    for (wMacroIdx=0;wMacroIdx<stTDTInfo.wNumMCDs;wMacroIdx++)
    {
        // Initialize global hit bit arrays

        WDInitHitBitArrays(lpstSigSet,lpstScan);

        // Get the macro info

        if (WD7GetMacroInfoAtIndex(&stTDTInfo,
                                   wMacroIdx,
                                   lpstScan->abyName,
                                   &lpstScan->uScan.stWD7.dwMacroOffset,
                                   &lpstScan->uScan.stWD7.dwMacroSize,
                                   &lpstScan->uScan.stWD7.byMacroEncryptByte,
                                   &bActive) == WD7_STATUS_ERROR ||
            bActive == FALSE)
        {
            // Error getting this macro or not an active macro,
            //  go to the next one

            continue;
        }


        if (WD7DeactivateMacroAtIndex(&stTDTInfo,
                                      wMacroIdx) != WD7_STATUS_OK)
        {
            // Error deleting the macro

            return(WD_STATUS_ERROR);
        }
    }

    return(WD_STATUS_OK);
}


//*************************************************************************
//
// BOOL WD7ApplyMenuRepair()
//
// Parameters:
//  lpstSigSet              Ptr to the WD7 signature set
//  lpstStream              Ptr to WordDocument stream
//  lpstKey                 Ptr to encryption key
//  lpstTDTInfo             Ptr to TDT information structure
//  lpabySig                Ptr to the menu repair clause
//
// Description:
//  This function iterates through all the menu repair IDs of the given
//  menu repair clause.  Each menu repair ID is interpreted as an offset
//  into the menu repair signature array.  At that position in the array
//  is a menu repair signature where the first byte of the signature
//  is a flag byte that specifies the presence of paramaters that follow
//  it.  The meaning of the bits in the flag byte are as listed in the
//  #defines immediately below this comment block.  The ordering of the
//  paramaters that follow the flag byte are in the same order as the
//  #defines.  If a bit is set then the paramater is present, otherwise
//  the paramater is absent and assumed to be a wildcard.  These paramaters
//  are passed WD7API function WD7DeleteDelta which attempts to match
//  the paramaters against the menu customizations of the document.  Any
//  matches are deleted.
//
// Returns:
//  TRUE                    If repair succeeded
//  FALSE                   If repair failed
//
//*************************************************************************

#define MR_INSTRUCTION          0x01
#define MR_MENU_POSITION        0x02
#define MR_MENU_NAME_INDEX      0x04
#define MR_FUNCTION             0x08
#define MR_MACRO_NAME_INDEX     0x10
#define MR_MENU_ITEM_POSITION   0x20
#define MR_MENU_NAME_A          0x40
#define MR_MENU_NAME_B          0x80

BOOL WD7ApplyMenuRepair
(
    LPWD_SIG_SET        lpstSigSet,
    LPSS_STREAM         lpstStream,
    LPWD7ENCKEY         lpstKey,
    LPWD7_TDT_INFO      lpstTDTInfo,
    LPBYTE              lpabySig
)
{
    LPBYTE              lpabyDataStream;
    LPBYTE              lpabyMenuRepair;
    BYTE                byFlags;
    DWORD               dwMenuRepairID;
    MENU_INFO_T         stInfo;
    int                 nCount;
    int                 i;
    WD7_STATUS          wd7Status;

    // Advance to the data stream

    lpabyDataStream = lpabySig + 1 + lpabySig[0];

    // Get the number of menu repair IDs
    // The first byte of the data stream is the number of repair IDs

    nCount = WDGetDataStreamBYTE(&lpabyDataStream);

    while (nCount != 0)
    {
        // The menu repair ID is the offset into the menu repair sig array

        dwMenuRepairID = WDGetDataStreamWORD(&lpabyDataStream);
        lpabyMenuRepair = lpstSigSet->lpabyMenuRepairSigs + dwMenuRepairID;

        // Get the menu repair flag which tells us which parameters are
        // specified.  Values that are not specified are wildcards.  The
        // following statement also advances lpabyMenuRepair to the first
        // parameter

        byFlags = *lpabyMenuRepair++;

        // Parse through the data stream and assign each parameter
        // to the appropiate variable in the Menu info structure, stInfo

        if (byFlags & MR_INSTRUCTION)
            stInfo.dwInst = WDGetDataStreamDWORD(&lpabyMenuRepair);
        else
            stInfo.dwInst = WD7_DELTA_WILD;

        if (byFlags & MR_MENU_POSITION)
            stInfo.dwMenu = WDGetDataStreamDWORD(&lpabyMenuRepair);
        else
            stInfo.dwMenu = WD7_DELTA_WILD;

        if (byFlags & MR_MENU_NAME_INDEX)
            stInfo.dwMenuItem = WDGetDataStreamDWORD(&lpabyMenuRepair);
        else
            stInfo.dwMenuItem = WD7_DELTA_WILD;

        if (byFlags & MR_FUNCTION)
            stInfo.dwFunction = WDGetDataStreamDWORD(&lpabyMenuRepair);
        else
            stInfo.dwFunction = WD7_DELTA_WILD;

        if (byFlags & MR_MACRO_NAME_INDEX)
            stInfo.dwMacroIndex = WDGetDataStreamDWORD(&lpabyMenuRepair);
        else
            stInfo.dwMacroIndex = WD7_DELTA_WILD;

        if (byFlags & MR_MENU_ITEM_POSITION)
            stInfo.dwMenuItemPos = WDGetDataStreamDWORD(&lpabyMenuRepair);
        else
            stInfo.dwMenuItemPos = WD7_DELTA_WILD;

        if (byFlags & MR_MENU_NAME_A)
        {
            stInfo.lpbyMenuNameA = lpabyMenuRepair++;
            for (i=1;i<=stInfo.lpbyMenuNameA[0];i++)
            {
                // This decrypts a character of the menu name string
                // and increments lpabyMenuRepair past that character
                --(*lpabyMenuRepair++);
            }
        }
        else
            stInfo.lpbyMenuNameA = NULL;

        if (byFlags & MR_MENU_NAME_B)
        {
            stInfo.lpbyMenuNameB = lpabyMenuRepair++;
            for (i=1;i<=stInfo.lpbyMenuNameB[0];i++)
            {
                // This decrypts a character of the menu name string
                // and increments lpabyMenuRepair past that character
                --(*lpabyMenuRepair++);
            }
        }
        else
            stInfo.lpbyMenuNameB = NULL;

        // Call the function to delete the specified menu by stInfo

        wd7Status = WD7DeleteDelta(lpstStream,
                                   lpstKey,
                                   lpstTDTInfo,
                                   &stInfo);

        /////////////////////////////////////////////////////////
        // Reencrypt the strings

        if (stInfo.lpbyMenuNameA != NULL)
        {
            for (i=1;i<=stInfo.lpbyMenuNameA[0];i++)
            {
                // This reencrypts a character of the menu name string
                ++(stInfo.lpbyMenuNameA[i]);
            }
        }

        if (stInfo.lpbyMenuNameB != NULL)
        {
            for (i=1;i<=stInfo.lpbyMenuNameB[0];i++)
            {
                // This reencrypts a character of the menu name string
                ++(stInfo.lpbyMenuNameB[i]);
            }
        }

        if (wd7Status != WD7_STATUS_OK)
            return(FALSE);

        nCount--;
    } // while nCount != 0

    return(TRUE);
}


//*************************************************************************
//
// BOOL WD7DoMenuRepair()
//
// Parameters:
//  lpstSigSet              Ptr to WD7 signature set
//  lpstScan                Ptr to the scan structure
//  lpstStream              Ptr to WordDocument stream
//  lpstKey                 Ptr to encryption key
//  lpstTDTInfo             Ptr to TDT information structure
//  lpstVirusSigInfo        Ptr to virus signature information
//  nDepth                  Current recursive depth
//
// Description:
//  This function iterates through all repair clauses of the given
//  virus signature and applies all menu repair clauses of the signature
//  as well as signatures referenced using UseOtherRepair().
//
// Returns:
//  TRUE                    If repair succeeded
//  FALSE                   If repair failed
//
//*************************************************************************

BOOL WD7DoMenuRepair
(
    LPWD_SIG_SET        lpstSigSet,
    LPWD_SCAN           lpstScan,
    LPSS_STREAM         lpstStream,
    LPWD7ENCKEY         lpstKey,
    LPWD7_TDT_INFO      lpstTDTInfo,
    LPWD_VIRUS_SIG_INFO lpstVirusSigInfo,
    int                 nDepth
)
{
    LPBYTE              lpabySig;
    LPBYTE              lpabyRepairSig;
    WORD                wRepairID;
    WORD                wdStatus;

    if (nDepth > 8)
    {
        // Exceeded maximum recursive depth

        return(FALSE);
    }

    lpabySig = lpstVirusSigInfo->lpabySig;

    // Jump to the first repair signature

    lpabyRepairSig = lpabySig + WDGetSigLen(lpabySig);

    // Iterate through all menu repairs

    while (*lpabyRepairSig != 0)
    {
        wdStatus = WDApplyVirusSig(lpabyRepairSig,
                                   lpstSigSet,
                                   lpstScan,
				   lpstScan->abyRunBuf,
				   &wRepairID);

	switch (wdStatus)
	{
	    case WD_STATUS_MENU_REPAIR:
		if (WD7ApplyMenuRepair(lpstSigSet,
				       lpstStream,
				       lpstKey,
				       lpstTDTInfo,
				       lpabyRepairSig) == FALSE)
                    return(FALSE);
                break;

            case WD_STATUS_REPAIR_USE_OTHER:
                // Recursively call
                if (WD7DoMenuRepair(lpstSigSet,
                                    lpstScan,
                                    lpstStream,
                                    lpstKey,
                                    lpstTDTInfo,
                                    lpstSigSet->lpastVirusSigInfo + wRepairID,
                                    nDepth + 1) == FALSE)
                    return(FALSE);
                break;

            default:
                // Nothing to do
                break;
        }

        lpabyRepairSig += WDGetSigLen(lpabyRepairSig);
    }

    return(TRUE);
}


#endif  // #ifndef SYM_NLM



