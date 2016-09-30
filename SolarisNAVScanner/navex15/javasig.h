//************************************************************************
//
// $Header:   S:/NAVEX/VCS/javasig.h_v   1.2   21 Nov 1996 17:12:54   AOONWAL  $
//
// Description:
//      Contains definition of structure for Java virus
//      definitions.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/javasig.h_v  $
// 
//    Rev 1.2   21 Nov 1996 17:12:54   AOONWAL
// No change.
// 
//    Rev 1.1   29 Oct 1996 13:00:08   AOONWAL
// No change.
// 
//    Rev 1.0   04 Apr 1996 14:54:28   DCHI
// Initial revision.
// 
//************************************************************************

#ifndef _JAVASIGH_

#define _JAVASIGH_

#include "platform.h"

#define SCAN_BUFFER_SIZE        	512
#define SCAN_OVERLAP_SIZE       	16
#define WILD_CARD_VALUE         	0xFFFF

// This must be updated to reflect the maximum length
// of all named methods in the definitions

#define MAX_JAVA_METHOD_NAME_LEN    32

#define JAVA_SCAN_NAMED_METHOD    0x0000001ul
#define JAVA_SCAN_ALL_METHODS     0x0000002ul

typedef struct
{
	WORD		wVID;
	DWORD		dwFlags;
    WORD		wMethodNameLen;
    PBYTE		pbyMethodName;
	WORD		wSigLen;
    PWORD       pwSig;
    WORD        wFirstSigWord;
	DWORD		dwDistToSearch;
} JAVASIG_T;

extern JAVASIG_T    gJavaSigs[];
extern WORD         gwNumJavaSigs;

#define MAX_JAVA_SIGS	32

#endif


