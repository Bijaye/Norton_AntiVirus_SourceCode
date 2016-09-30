// Copyright 1996 Symantec, Peter Norton Product Group
//***************************************************************************
//
// $Header:   S:/NAVAPSVC/VCS/appacket.h_v   1.1   18 Apr 1997 13:59:14   DSACKIN  $
//
// Description:
//      This file declares the APPACKET.CPP exports.
//
// Contains:
//      DecodeNAVAPUserInfoSubPacket()
//
// See Also:
//      appacket.cpp
//
//***************************************************************************
// $Log:   S:/NAVAPSVC/VCS/appacket.h_v  $
// 
//    Rev 1.1   18 Apr 1997 13:59:14   DSACKIN
// Ported from Tesla
// 
//    Rev 1.0   13 Mar 1997 15:12:16   RFULLER
// Initial revision
// 
//    Rev 1.0   08 Nov 1996 12:16:14   DKEEFER
// Initial revision
// 
//    Rev 1.0   16 May 1996 10:28:32   RSTANEV
// Initial revision.
//
//***************************************************************************

#ifndef __cplusplus
#error Need a C++ compiler!
#endif  // __cplusplus

#ifndef _APPACKET_H_
#define _APPACKET_H_

BOOL DecodeNAVAPUserInfoSubPacket (
    PNAVAP_USERINFOSUBPACKET prPacket,
    PTSTR                    pszUser,
    PDWORD                   pcchUser,
    PTSTR                    pszDomain,
    PDWORD                   pcchDomain
    );

#endif  // _APPACKET_H_
