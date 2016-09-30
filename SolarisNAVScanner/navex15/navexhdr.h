//************************************************************************
//
// $Header:   S:/NAVEX/VCS/navexhdr.h_v   1.4   21 Nov 1996 17:14:54   AOONWAL  $
//
// Description:
//      Contains NAVEX constants and function prototypes common to all
//      modules.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/navexhdr.h_v  $
// 
//    Rev 1.4   21 Nov 1996 17:14:54   AOONWAL
// No change.
// 
//    Rev 1.3   29 Oct 1996 12:59:54   AOONWAL
// No change.
// 
//    Rev 1.2   15 Apr 1996 18:25:46   RSTANEV
// Defining SEEK_??? for SYM_NTK platform.
//
//    Rev 1.1   18 Oct 1995 14:27:24   cnachen
// Defined SEEK_ constants.
//
//    Rev 1.0   16 Oct 1995 12:58:02   DCHI
// Initial revision.
//
//************************************************************************

#ifndef _NAVEXHDR_H

#define _NAVEXHDR_H

#if defined(SYM_NLM) || defined(SYM_NTK)

#define SEEK_SET    0
#define SEEK_CUR    1
#define SEEK_END    2

#endif
// ScanString arguments:
//
// lpbyBuffer1          : Buffer in which to search.
// cwBuffer1            : Count of bytes in Buffer1.
//                      : cwBuffer1 must be > cwBuffer2.
// lpbyBuffer2          : String to search for in Buffer1.
// cwBuffer2            : Count of bytes in Buffer2.
//                      : cwBuffer2 ust be >= 2 bytes.
//
// Returns:
//
//     WORD             : -1            no match
//                        n >= 0        offset of buffer2 in buffer1.

WORD ScanString (LPBYTE lpbyBuffer1,
                 WORD cwBuffer1,
                 LPBYTE lpbyBuffer2,
                 WORD cwBuffer2);



#endif  // _NAVEXHDR_H

