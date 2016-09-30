//************************************************************************
//
// $Header:   S:/NAVEX/VCS/wdavsg15.cpv   1.3   11 May 1998 18:17:50   DCHI  $
//
// Description:
//  Virus signature application module.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/wdavsg15.cpv  $
// 
//    Rev 1.3   11 May 1998 18:17:50   DCHI
// Changes for menu repair and
// for Access 2.0 scanning and Access datafile conversion.
// 
//    Rev 1.2   15 Apr 1998 17:24:38   DCHI
// Modifications for new XL95 and XL5 engines.
// 
//    Rev 1.1   10 Jul 1997 17:48:20   DDREW
// Turn on NLM repairs for NAVEX15
// 
//    Rev 1.0   09 Jul 1997 16:19:46   DCHI
// Initial revision.
// 
//************************************************************************

#include "wd7api.h"
#include "o97api.h"
#include "xl5api.h"
#include "acsapi.h"
#include "wdscan15.h"
#include "wdsigcmd.h"
#include "wdapvsig.h"
#include "wdsigutl.h"

//*************************************************************************
//
// WD_STATUS WDApplyVirusSig()
//
// Parameters:
//  lpabySig        Signature
//  lpstSigSet      Sig set
//  lpstScan        The scan structure
//  lpbyStack       Ptr to >= 256 byte work buffer
//  lpwID           Ptr to WORD to store index in UseOtherRepair
//                      and CustomRepair functions.  Can be NULL
//                      for signatures guaranteed not to have
//                      these functions.
//
// Description:
//  Applies the given virus signature.
//
// Returns:
//  WD_STATUS_ERROR     On error in the signature
//  WD_STATUS_SIG_HIT   If there is a hit on the signature
//  WD_STATUS_OK        If there is no hit on the signature
//
//*************************************************************************

WD_STATUS WDApplyVirusSig
(
    LPBYTE              lpabySig,
    LPWD_SIG_SET        lpstSigSet,
    LPWD_SCAN           lpstScan,
    LPBYTE              lpbyStack,
    LPWORD              lpwID
)
{
    int                 nCommand;
    int                 nNibble;
    int                 nControlStreamLen;
    int                 nControlStreamIndex;
    LPBYTE              lpabyControlStream;
    LPBYTE              lpabyDataStream;
    WORD                wID;
    int                 nStackTopIndex;

    (void)lpstSigSet;

    nControlStreamLen = lpabySig[0];
    lpabyControlStream = lpabySig + 1;

    // The data stream of a virus signature does not have a length byte

    lpabyDataStream = lpabyControlStream + nControlStreamLen;

    // Multiply by two to get number of control nibbles

    nControlStreamLen *= 2;

    nStackTopIndex = -1;
    nControlStreamIndex = 0;
    while (nControlStreamIndex < nControlStreamLen)
    {
        nCommand = WDGetControlStreamNibble(lpabyControlStream,
                                            &nControlStreamIndex);

        switch (nCommand)
        {
            case VNIB0_NAME_SIG_BYTE:
                wID = WDGetDataStreamBYTE(&lpabyDataStream);
                if ((lpstScan->lpabyNameSigInterMacroHit[wID >> 3] &
                     (1 << (wID & 0x07))) == 0)
                {
                    lpbyStack[++nStackTopIndex] = 0;
                }
                else
                {
                    lpbyStack[++nStackTopIndex] = 1;
                }
                break;

            case VNIB0_NAME_SIG_WORD:
                wID = WDGetDataStreamWORD(&lpabyDataStream);
                if ((lpstScan->lpabyNameSigInterMacroHit[wID >> 3] &
                     (1 << (wID & 0x07))) == 0)
                {
                    lpbyStack[++nStackTopIndex] = 0;
                }
                else
                {
                    lpbyStack[++nStackTopIndex] = 1;
                }
                break;

            case VNIB0_NAMED_CRC_SIG_BYTE:
                wID = WDGetDataStreamBYTE(&lpabyDataStream);
                if ((lpstScan->lpabyNamedCRCSigHit[wID >> 3] &
                     (1 << (wID & 0x07))) == 0)
                {
                    lpbyStack[++nStackTopIndex] = 0;
                }
                else
                {
                    lpbyStack[++nStackTopIndex] = 1;
                }
                break;

            case VNIB0_NAMED_CRC_SIG_WORD:
                wID = WDGetDataStreamWORD(&lpabyDataStream);
                if ((lpstScan->lpabyNamedCRCSigHit[wID >> 3] &
                     (1 << (wID & 0x07))) == 0)
                {
                    lpbyStack[++nStackTopIndex] = 0;
                }
                else
                {
                    lpbyStack[++nStackTopIndex] = 1;
                }
                break;

            case VNIB0_MACRO_SIG_BYTE:
                wID = WDGetDataStreamBYTE(&lpabyDataStream);
                if ((lpstScan->lpabyMacroSigHit[wID >> 3] &
                     (1 << (wID & 0x07))) == 0)
                {
                    lpbyStack[++nStackTopIndex] = 0;
                }
                else
                {
                    lpbyStack[++nStackTopIndex] = 1;
                }
                break;

            case VNIB0_MACRO_SIG_WORD:
                wID = WDGetDataStreamWORD(&lpabyDataStream);
                if ((lpstScan->lpabyMacroSigHit[wID >> 3] &
                     (1 << (wID & 0x07))) == 0)
                {
                    lpbyStack[++nStackTopIndex] = 0;
                }
                else
                {
                    lpbyStack[++nStackTopIndex] = 1;
                }
                break;

            case VNIB0_CRC_BYTE:
                wID = WDGetDataStreamBYTE(&lpabyDataStream);
                if ((lpstScan->lpabyCRCHit[wID >> 3] &
                     (1 << (wID & 0x07))) == 0)
                {
                    lpbyStack[++nStackTopIndex] = 0;
                }
                else
                {
                    lpbyStack[++nStackTopIndex] = 1;
                }
                break;

            case VNIB0_CRC_WORD:
                wID = WDGetDataStreamWORD(&lpabyDataStream);
                if ((lpstScan->lpabyCRCHit[wID >> 3] &
                     (1 << (wID & 0x07))) == 0)
                {
                    lpbyStack[++nStackTopIndex] = 0;
                }
                else
                {
                    lpbyStack[++nStackTopIndex] = 1;
                }
                break;

            case VNIB0_AND:
                --nStackTopIndex;
                lpbyStack[nStackTopIndex] &= lpbyStack[nStackTopIndex + 1];
                break;

            case VNIB0_OR:
                --nStackTopIndex;
                lpbyStack[nStackTopIndex] |= lpbyStack[nStackTopIndex + 1];
                break;

            case VNIB0_NOT:
                lpbyStack[nStackTopIndex] ^= 1;
                break;

            case VNIB0_USE_SIGNATURE:
                if (nControlStreamIndex == 1 ||
                    (nStackTopIndex == 0 && *lpbyStack != 0))
                {
                    return(WD_STATUS_REPAIR_USE_SIG);
                }
                else
                {
                    // No repair

                    return(WD_STATUS_OK);
                }

            case VNIB0_USE_OTHER_REPAIR_BYTE:
                *lpwID = WDGetDataStreamBYTE(&lpabyDataStream);
                if (nControlStreamIndex == 1 ||
                    (nStackTopIndex == 0 && *lpbyStack != 0))
                {
                    return(WD_STATUS_REPAIR_USE_OTHER);
                }
                else
                {
                    // No repair

                    return(WD_STATUS_OK);
                }

            case VNIB0_USE_OTHER_REPAIR_WORD:
                *lpwID = WDGetDataStreamWORD(&lpabyDataStream);
                if (nControlStreamIndex == 1 ||
                    (nStackTopIndex == 0 && *lpbyStack != 0))
                {
                    return(WD_STATUS_REPAIR_USE_OTHER);
                }
                else
                {
                    // No repair

                    return(WD_STATUS_OK);
                }

            case VNIB0_MISC:
            {
                nNibble = WDGetControlStreamNibble(lpabyControlStream,
                                                   &nControlStreamIndex);

                switch (nNibble)
                {
                    case VNIB1_CUSTOM_REPAIR_BYTE:
                        *lpwID = WDGetDataStreamBYTE(&lpabyDataStream);
                        if (nControlStreamIndex == 1 ||
                            (nStackTopIndex == 0 && *lpbyStack != 0))
                        {
                            return(WD_STATUS_REPAIR_CUSTOM);
                        }
                        else
                        {
                            // No repair

                            return(WD_STATUS_OK);
                        }

                    case VNIB1_CUSTOM_REPAIR_WORD:
                        *lpwID = WDGetDataStreamWORD(&lpabyDataStream);
                        if (nControlStreamIndex == 1 ||
                            (nStackTopIndex == 0 && *lpbyStack != 0))
                        {
                            return(WD_STATUS_REPAIR_CUSTOM);
                        }
                        else
                        {
                            // No repair

                            return(WD_STATUS_OK);
                        }

                    case VNIB1_FULL_SET_REPAIR:
                    case VNIB1_HEURISTIC_REPAIR:
                        // Heuristic and full set repair should have
                        //  been taken care of outside

                        return(WD_STATUS_OK);

                    case VNIB1_MACRO_COUNT_EQ:
                        if (lpstScan->wMacroCount ==
                            WDGetDataStreamBYTE(&lpabyDataStream))
                        {
                            lpbyStack[++nStackTopIndex] = 1;
                        }
                        else
                        {
                            lpbyStack[++nStackTopIndex] = 0;
                        }
                        break;

                    case VNIB1_MACRO_COUNT_LT:
                        if (lpstScan->wMacroCount <
                            WDGetDataStreamBYTE(&lpabyDataStream))
                        {
                            lpbyStack[++nStackTopIndex] = 1;
                        }
                        else
                        {
                            lpbyStack[++nStackTopIndex] = 0;
                        }
                        break;

                    case VNIB1_MACRO_COUNT_GT:
                        if (lpstScan->wMacroCount >
                            WDGetDataStreamBYTE(&lpabyDataStream))
                        {
                            lpbyStack[++nStackTopIndex] = 1;
                        }
                        else
                        {
                            lpbyStack[++nStackTopIndex] = 0;
                        }
                        break;

                    case VNIB1_FULL_SET:
                        if (lpstScan->wFlags & WD_SCAN_FLAG_FULL_SET)
                        {
                            lpbyStack[++nStackTopIndex] = 1;
                        }
                        else
                        {
                            lpbyStack[++nStackTopIndex] = 0;
                        }
                        break;

                    case VNIB1_MENU_REPAIR:
                        return(WD_STATUS_MENU_REPAIR);

                    default:
                        // This should never happen
                        return(WD_STATUS_ERROR);
                }
            }

            case VNIB0_END:
                // NOP
                break;

            default:
                // This should never happen
                return(WD_STATUS_ERROR);
        }
    }

    if (nStackTopIndex == 0 && *lpbyStack != 0)
        return(WD_STATUS_SIG_HIT);

    return(WD_STATUS_OK);
}


#if defined(NAVEX15) || !defined(SYM_NLM)

//*************************************************************************
//
// WD_STATUS WDApplyUseSigRepair()
//
// Parameters:
//  lpabySig        Signature
//  lpstSigSet      Sig set
//  lpstScan                The scan structure
//
// Description:
//  Returns WD_STATUS_SIG_HIT if at least one literal of the sig is a hit
//
// Returns:
//  WD_STATUS_ERROR     On error in the signature
//  WD_STATUS_SIG_HIT   If there is a hit on the signature
//  WD_STATUS_OK        If there is no hit on the signature
//
//*************************************************************************


WD_STATUS WDApplyUseSigRepair
(
    LPBYTE              lpabySig,
    LPWD_SIG_SET        lpstSigSet,
    LPWD_SCAN           lpstScan
)
{
    int                 nCommand;
    int                 nControlStreamLen;
    int                 nControlStreamIndex;
    LPBYTE              lpabyControlStream;
    LPBYTE              lpabyDataStream;
    WORD                wID;

    (void)lpstSigSet;

    nControlStreamLen = lpabySig[0];
    lpabyControlStream = lpabySig + 1;

    // The data stream of a virus signature does not have a length byte

    lpabyDataStream = lpabyControlStream + nControlStreamLen;

    // Multiply by two to get number of control nibbles

    nControlStreamLen *= 2;

    nControlStreamIndex = 0;
    while (nControlStreamIndex < nControlStreamLen)
    {
        nCommand = WDGetControlStreamNibble(lpabyControlStream,
                                            &nControlStreamIndex);

        switch (nCommand)
        {
            case VNIB0_NAME_SIG_BYTE:
                wID = WDGetDataStreamBYTE(&lpabyDataStream);
                if ((lpstScan->lpabyNameSigInterMacroHit[wID >> 3] &
                     (1 << (wID & 0x07))) != 0)
                {
                    return(WD_STATUS_SIG_HIT);
                }
                break;

            case VNIB0_NAME_SIG_WORD:
                wID = WDGetDataStreamWORD(&lpabyDataStream);
                if ((lpstScan->lpabyNameSigInterMacroHit[wID >> 3] &
                     (1 << (wID & 0x07))) != 0)
                {
                    return(WD_STATUS_SIG_HIT);
                }
                break;

            case VNIB0_NAMED_CRC_SIG_BYTE:
                wID = WDGetDataStreamBYTE(&lpabyDataStream);
                if ((lpstScan->lpabyNamedCRCSigHit[wID >> 3] &
                     (1 << (wID & 0x07))) != 0)
                {
                    return(WD_STATUS_SIG_HIT);
                }
                break;

            case VNIB0_NAMED_CRC_SIG_WORD:
                wID = WDGetDataStreamWORD(&lpabyDataStream);
                if ((lpstScan->lpabyNamedCRCSigHit[wID >> 3] &
                     (1 << (wID & 0x07))) != 0)
                {
                    return(WD_STATUS_SIG_HIT);
                }
                break;

            case VNIB0_MACRO_SIG_BYTE:
                wID = WDGetDataStreamBYTE(&lpabyDataStream);
                if ((lpstScan->lpabyMacroSigHit[wID >> 3] &
                     (1 << (wID & 0x07))) != 0)
                {
                    return(WD_STATUS_SIG_HIT);
                }
                break;

            case VNIB0_MACRO_SIG_WORD:
                wID = WDGetDataStreamWORD(&lpabyDataStream);
                if ((lpstScan->lpabyMacroSigHit[wID >> 3] &
                     (1 << (wID & 0x07))) != 0)
                {
                    return(WD_STATUS_SIG_HIT);
                }
                break;

            case VNIB0_CRC_BYTE:
                wID = WDGetDataStreamBYTE(&lpabyDataStream);
                if ((lpstScan->lpabyCRCHit[wID >> 3] &
                     (1 << (wID & 0x07))) != 0)
                {
                    return(WD_STATUS_SIG_HIT);
                }
                break;

            case VNIB0_CRC_WORD:
                wID = WDGetDataStreamWORD(&lpabyDataStream);
                if ((lpstScan->lpabyCRCHit[wID >> 3] &
                     (1 << (wID & 0x07))) != 0)
                {
                    return(WD_STATUS_SIG_HIT);
                }
                break;

            case VNIB0_AND:
                break;

            case VNIB0_OR:
                break;

            case VNIB0_NOT:
                break;

            case VNIB0_END:
                // NOP
                break;

            default:
                // This should never happen
                return(WD_STATUS_ERROR);
        }
    }

    return(WD_STATUS_OK);
}

#endif  // #ifndef SYM_NLM


