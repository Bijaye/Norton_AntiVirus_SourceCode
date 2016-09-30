// Copyright 1996 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/AVENGE/VCS/getbulk.cpv   1.0   20 May 1997 13:18:12   CNACHEN  $
//
// Description:
//  Functions for loading virus names/info records many at a time for fast
//  virus list loading.
//
// Contains:
//
// See Also:
//
//************************************************************************
// $Log:   S:/AVENGE/VCS/getbulk.cpv  $
// 
//    Rev 1.0   20 May 1997 13:18:12   CNACHEN
// Initial revision.
// 
//************************************************************************


#include "avenge.h"

ENGSTATUS EngInitBulkLoad
(
    HGENG               hGEng,
    LPBULK_LOAD_INFO    lpstBLI
)
{
    if (EngInitNameLoad(hGEng,
                        NULL,
                        &lpstBLI->stNameInfo) != ENGSTATUS_OK)
    {
        return(ENGSTATUS_ERROR);
    }

    if (EngInitInfRecLoad(hGEng,
                          lpstBLI->stNameInfo.lpvFileInfo,
                          &lpstBLI->stInfRecInfo) != ENGSTATUS_OK)
    {
        return(ENGSTATUS_ERROR);
    }

    return(ENGSTATUS_OK);
}

ENGSTATUS EngGetNextN30
(
    HGENG                   hGEng,
    LPBULK_LOAD_INFO        lpstBLI,
    LPN30                   lpstN30,
    LPBOOL                  lpbDone
)
{
    VIRUS_INFO_T            stVirusInfo;
    N30                     stTempN30 = {0};

    if (EngGetNextName(hGEng,
                       &lpstBLI->stNameInfo,
                       stTempN30.lpVName) != ENGSTATUS_OK)
    {
        return(ENGSTATUS_ERROR);
    }

    if (EngGetNextInfRec(hGEng,
                         &lpstBLI->stInfRecInfo,
                         &stVirusInfo,
                         lpbDone) != ENGSTATUS_OK)
    {
        return(ENGSTATUS_ERROR);
    }

    // first fill in virus ID

    stTempN30.wVirID = stVirusInfo.wVirusID;

    // copy over the virus size

    stTempN30.wInfSize = stVirusInfo.wInfSize;

    // next fill in the def type

    if (stVirusInfo.wFlags & AVENGE_TYPE_CRC)
        stTempN30.wType = CRCtype;
    if (stVirusInfo.wFlags & AVENGE_TYPE_ALG)
        stTempN30.wType = ALGtype;
    if (stVirusInfo.wFlags & AVENGE_TYPE_BOO)
        stTempN30.wType = BOOtype;
    if (stVirusInfo.wFlags & AVENGE_TYPE_MAC)
        stTempN30.wType = MACtype;

    // the next two items set up support for Macro, Agent and Windows
    // virus types, which are encoded in the two high bits of the wControl2
    // field of the N30 structure.  (They are contained in the flags word of
    // the avenge info structure.)

    if (stVirusInfo.wFlags & AVENGE_TYPE_EXTENSION_LOW)
        stTempN30.wControl2 |= N30_CTRL2_BIT_FLAG2I;

    if (stVirusInfo.wFlags & AVENGE_TYPE_EXTENSION_HIGH)
        stTempN30.wControl2 |= N30_CTRL2_BIT_FLAG2J;

    // now set up our info flags (only the low word is from N30)

    stTempN30.wInfo = (WORD)(stVirusInfo.dwInfo & 0xFFFFU);

    *lpstN30 = stTempN30;

    return(ENGSTATUS_OK);
}


ENGSTATUS EngCloseBulkLoad
(
    HGENG               hGEng,
    LPBULK_LOAD_INFO    lpstBLI
)
{
    if (EngCloseNameLoad(hGEng,&lpstBLI->stNameInfo) != ENGSTATUS_OK)
    {
        return(ENGSTATUS_ERROR);
    }

    if (EngCloseInfRecLoad(hGEng,
                           &lpstBLI->stInfRecInfo) != ENGSTATUS_OK)
    {
        return(ENGSTATUS_ERROR);
    }

    return(ENGSTATUS_OK);
}

