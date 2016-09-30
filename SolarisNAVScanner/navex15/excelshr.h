//************************************************************************
//
// $Header:   S:/NAVEX/VCS/excelshr.h_v   1.3   09 Jul 1997 16:46:50   DCHI  $
//
// Description:
//      Header file for source file containing function to open
//      a hex-named stream.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/excelshr.h_v  $
// 
//    Rev 1.3   09 Jul 1997 16:46:50   DCHI
// Change to MCRENG15.
// 
//    Rev 1.2   21 Nov 1996 17:14:28   AOONWAL
// No change.
//
//    Rev 1.1   29 Oct 1996 12:59:48   AOONWAL
// No change.
// 
//    Rev 1.0   05 Aug 1996 10:39:46   DCHI
// Initial revision.
//
//************************************************************************

#ifndef _EXCELSHR_H_

#define _EXCELSHR_H_

typedef struct
{
	WORD		wVID;
	WORD		wSigLen;
    LPWORD      lpwSig;
    WORD        wFirstSigWord;
} EXCELSIG_T;

#define EXCEL_SIG_WILD_CARD_VALUE   0xFFFF

extern EXCELSIG_T gExcelSigs[];

extern WORD gwNumExcelSigs;

#endif // _EXCELSHR_H_

