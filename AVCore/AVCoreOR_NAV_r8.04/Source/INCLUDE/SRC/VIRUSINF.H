// Copyright 1994 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:\source\include\vcs\virusinf.h_v   1.6   01 Aug 1994 12:23:32   DALLEE  $
//
// Description:
//      Header defines for VIRSCAN.INF
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:\source\include\vcs\virusinf.h_v  $
// 
//    Rev 1.6   01 Aug 1994 12:23:32   DALLEE
// Moved SIZE_COPYRIGHT #define here from VIRUSDBF.H
//************************************************************************

#ifndef _VIRUSINF_H
#define _VIRUSINF_H


#define VIRINF_NAV30_VERSION   0
#define VIRINF_NAV40_VERSION   1

#define SIZE_COPYRIGHT  128

typedef struct inf_header 
    {
    short           _i_version;         /* Version = 0 for now */
    unsigned short  _i_highest_vid;
    long            _i_no_info;
    short           _i_multiplier;      /* default = 4 */
    short           _i_size;            /* default = 2 */
    char            _i_reserved[52];    /* For a total of 64 */
    } INF_HEADER;

#endif  // _VIRUSINF_H
