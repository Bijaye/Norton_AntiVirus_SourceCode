// Copyright 1995 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/HEUR/VCS/ba.cpv   1.1   04 Jun 1997 16:35:40   CNACHEN  $
//
// Description:
//
//  Contains code for the bloodhound behavior analyzer (boy is it simple!)
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/HEUR/VCS/ba.cpv  $
// 
//    Rev 1.1   04 Jun 1997 16:35:40   CNACHEN
// Fixed bug where we return FALSE (error) when we decide not to emulate any
// more.
// 
//    Rev 1.0   14 May 1997 17:23:10   CNACHEN
// Initial revision.
// 
//************************************************************************


#include "avtypes.h"
#include "heurapi.h"
#include "heurint.h"


BOOL BADetectVirus
(
    WORD                    wHeurStreamLen,
    LPBYTE                  lpbyHeurCode,
    LPBEHAVE_MONITOR        pstBehavior,
    LPBOOL                  lpbFoundVirus,
    LPWORD                  lpwVirusID
)
{
    // now run the interpereter to find if we have a virus

    *lpbFoundVirus = FALSE;
    *lpwVirusID = 0;

    // perform some checks before we do analysis

    if (BMStopEmulating(pstBehavior) == TRUE)
        return(TRUE);

    if (InterpretStream(wHeurStreamLen,
                        lpbyHeurCode,
                        pstBehavior->m_byBehaviorArray,
                        lpbFoundVirus,
                        lpwVirusID) == FALSE)
        return(FALSE);

    return(TRUE);
}



