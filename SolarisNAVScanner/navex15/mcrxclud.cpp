// Copyright 1997 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/NAVEX/VCS/MCRXCLUD.CPv   1.2   09 Dec 1998 17:45:14   DCHI  $
//
// Description:
//  This source file contains routines for checking to see whether
//  a macro should be excluded from heuristic emulation.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/MCRXCLUD.CPv  $
// 
//    Rev 1.2   09 Dec 1998 17:45:14   DCHI
// Added #include "gdefines.h"
// 
//    Rev 1.1   04 Aug 1998 13:52:30   DCHI
// Changes for Excel heuristics.
// 
//    Rev 1.0   15 Oct 1997 13:20:44   DCHI
// Initial revision.
// 
//************************************************************************

#include "gdefines.h"

#ifdef MACROHEU

#include "platform.h"
#include "mcrxclud.h"

#ifndef MACRODAT

extern DWORD    gdwWD7ExclusionSigCount;
extern LPDWORD  gpadwWD7ExclusionCRCs;
extern LPDWORD  gpadwWD7ExclusionCRCByteCounts;

extern DWORD    gdwXL95ExclusionSigCount;
extern LPDWORD  gpadwXL95ExclusionCRCs;
extern LPDWORD  gpadwXL95ExclusionCRCByteCounts;

extern DWORD    gdwWD8ExclusionSigCount;
extern LPDWORD  gpadwWD8ExclusionCRCs;
extern LPDWORD  gpadwWD8ExclusionCRCByteCounts;

extern DWORD    gdwXL97ExclusionSigCount;
extern LPDWORD  gpadwXL97ExclusionCRCs;
extern LPDWORD  gpadwXL97ExclusionCRCByteCounts;

MACRO_EXCLUSION_SET_T gastMacroExclusionSets[NUM_MACRO_EXCLUSION_SETS] =
{
    {
        0,
        gdwWD7ExclusionSigCount,
        gpadwWD7ExclusionCRCs,
        gpadwWD7ExclusionCRCByteCounts
    },
    {
        0,
        gdwWD8ExclusionSigCount,
        gpadwWD8ExclusionCRCs,
        gpadwWD8ExclusionCRCByteCounts
    },
    {
        0,
        gdwXL95ExclusionSigCount,
        gpadwXL95ExclusionCRCs,
        gpadwXL95ExclusionCRCByteCounts
    },
    {
        0,
        gdwXL97ExclusionSigCount,
        gpadwXL97ExclusionCRCs,
        gpadwXL97ExclusionCRCByteCounts
    }
};

#endif // #ifndef MACRODAT

//*************************************************************************
//
// BOOL MacroIsExcluded()
//
// Parameters:
//  nExcludeType    The exclusion category to check
//  dwCRC           The CRC of the macro/module
//  dwCRCByteCount  The number of bytes used to compute the CRC
//
// Description:
//  Verifies the category and then performs a binary search through
//  the desired exclusion signature set for the given CRC.  If there
//  is a CRC match, the search proceeds for a matching byte count
//  using a linear backward, then forward search.
//
// Returns:
//  TRUE            If the CRC and CRC byte count are in the exclusion set
//  FALSE           If the category is unknown or there was no matching
//                      CRC and CRC byte count in the exclusion set
//
//*************************************************************************

BOOL MacroIsExcluded
(
    int             nExcludeType,
    DWORD           dwCRC,
    DWORD           dwCRCByteCount
)
{
    long            lCount, lLow, lMid, lHigh, l;
    LPDWORD         lpadwCRCs;
    LPDWORD         lpadwCRCByteCounts;

    /////////////////////////////////////////////////////////////
    // Verify the type and get the exclusion data

    lCount = gastMacroExclusionSets[nExcludeType].dwCount;
    lpadwCRCs = gastMacroExclusionSets[nExcludeType].lpadwCRCs;
    lpadwCRCByteCounts =
        gastMacroExclusionSets[nExcludeType].lpadwCRCByteCounts;

    /////////////////////////////////////////////////////////////
    // Search for the CRC using a binary search

    lLow = 0;
    lHigh = lCount - 1;
    while (lLow <= lHigh)
    {
        lMid = (lLow + lHigh) / 2;

        if (dwCRC < lpadwCRCs[lMid])
        {
            // In lower half

            lHigh = lMid - 1;
        }
        else
        if (dwCRC > lpadwCRCs[lMid])
        {
            // In upper half

            lLow = lMid + 1;
        }
        else
        {
            // Found a match

            break;
        }
    }

    if (lLow > lHigh)
    {
        // Couldn't find the CRC

        return(FALSE);
    }

    /////////////////////////////////////////////////////////////
    // Search for a byte count match using a backward, then
    //  forward binary search

    // Search backwards

    l = lMid;
    do
    {
        if (lpadwCRCByteCounts[l] == dwCRCByteCount)
        {
            // Found an exclusion match

            return(TRUE);
        }

        --l;
    }
    while (l >= 0 && lpadwCRCs[l] == dwCRC);

    // Search forwards

    l = lMid + 1;
    while (l < lCount && lpadwCRCs[l] == dwCRC)
    {
        if (lpadwCRCByteCounts[l] == dwCRCByteCount)
        {
            // Found an exclusion match

            return(TRUE);
        }

        ++l;
    }

    // No exclusion match

    return(FALSE);
}

#endif // #ifdef MACROHEU

