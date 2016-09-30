//************************************************************************
//
// $Header:   S:/NAVEX/VCS/winsig.h_v   1.6   17 Nov 1998 14:06:20   DKESSNE  $
//
// Description:
//      Contains definitions and structures for Windows virus
//      definitions.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/winsig.h_v  $
// 
//    Rev 1.6   17 Nov 1998 14:06:20   DKESSNE
// removed flag WINSIG_SCAN_EP_AS_OFFSET (jw added this for CIH, but it was
// never used);  added flag WINSIG_SCAN_LAST_SEG_START
// 
//    Rev 1.5   14 Sep 1998 20:01:24   JWILBER
// Added #defines and an extern declaration for the RedTeam Kernel386
// detection.
//
//    Rev 1.4   25 Jun 1998 19:30:08   jwilber
// Added #define for WINSIG_SCAN_EP_AS_OFFSET.
//
//    Rev 1.3   21 Nov 1996 17:13:02   AOONWAL
// No change.
//
//    Rev 1.2   29 Oct 1996 13:00:10   AOONWAL
// No change.
//
//    Rev 1.1   12 Jun 1996 19:37:54   JWILBER
// Added bitfield defs for WIN_SIG_SCAN_FIRST_SEG and
// WIN_SIG_SCAN_LAST_SEG.
//
//    Rev 1.0   05 Feb 1996 17:23:24   DCHI
// Initial revision.
//
//************************************************************************

#ifndef _WINSIGH_

#define _WINSIGH_

#include "platform.h"

#define WINSIG_SCAN_MAIN_EP         0x0000001ul
#define WINSIG_SCAN_ALL_EP          0x0000002ul
#define WINSIG_SCAN_ALL_CODE        0x0000004ul
#define WINSIG_SCAN_ALL_DATA        0x0000008ul
#define WINSIG_SCAN_FIRST_SEG       0x0000010ul // Not currently implemented
#define WINSIG_SCAN_LAST_SEG        0x0000020ul
#define WINSIG_SCAN_LAST_SEG_START  0x0000040ul


// These values are #defines so we can access them without searching all sigs
#define REDTEAMSUBCON   3               // RedTeam entry sub const
#define REDTEAMSIGLEN   0x20            // Length of RedTeam sig

typedef struct
{
	WORD		wVID;
	DWORD		dwFlags;
	WORD		wEntrySubConst;
	WORD		wSigLen;
    PWORD       pwSig;
    WORD        wFirstSigWord;
	WORD		wDistToSearch;
} WINSIG_T;

extern WORD gpwWinRedTeamSig[];     // For special detection of RedTeam Kernel

extern WINSIG_T gNESigs[];
extern WORD gwNumNESigs;

extern WINSIG_T gPESigs[];
extern WORD gwNumPESigs;

#endif


