////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

               
//***********************************************************************
//                                                                
// $Header:   S:/include/VCS/insert.h_v   1.0   06 Feb 1997 20:52:00   RFULLER  $
//                                                                
// Description:                                                   
//      Definitions and prototypes for INSERT functionality.
//                                                                
// Contains (as exports):                                                      
//                                                                
// See Also:                                                      
//***********************************************************************
// $Log:   S:/include/VCS/insert.h_v  $
// 
//    Rev 1.0   06 Feb 1997 20:52:00   RFULLER
// Initial revision
// 
//    Rev 1.0   30 Jan 1996 15:47:02   BARRY
// Initial revision.
// 
//    Rev 1.0   30 Jan 1996 10:11:24   BARRY
// Initial revision.
// 
//    Rev 1.2   01 Feb 1995 19:04:54   DALLEE
// Added extern "C" around prototypes.
// 
//    Rev 1.1   31 Jan 1995 18:32:30   DALLEE
// Removed InsertOVL().
// Use InsertSmallOVL() to create 5K TSR.  (Really the same function as
//   InsertOVL() but removed unused lpDataBuffer and now accepts NAVOPTIONS
//   to use when building TSR -- options are copied locally, then fudged so
//   5K size is built.)
// 
//    Rev 1.0   31 Oct 1994 12:48:44   MFALLEN
// Initial revision.
// 
//    Rev 1.7   10 Sep 1993 03:32:18   BARRY
// Updated prototype and buffer sizes
// 
//    Rev 1.6   24 Aug 1993 20:03:54   MFALLEN
// Since Carlton was twinsting my arm I had to increase the 512 buffer to 1024
// 
//    Rev 1.5   13 Aug 1993 01:46:08   BARRY
// Remove special code for RTLINK
// 
//    Rev 1.4   13 Aug 1993 01:44:56   BARRY
// Changed buffer sizes, put LPVOID in UpdateTSR prototype (they used to be LPSTR)
// 
//    Rev 1.3   12 Aug 1993 19:48:10   MFALLEN
// Slight modification to InsertOVL() proto.
// 
//    Rev 1.2   12 Aug 1993 18:45:10   CGUC
// Changed InsertOVL( to include buffer sizes.
// 
//    Rev 1.1   03 Aug 1993 05:10:58   CGUC
// Changed both buffers to 65000 bytes to create TSR with.
// 
//    Rev 1.0   07 Jun 1993 19:23:26   BRAD
// Initial revision.
//***********************************************************************

#ifndef INCL_INSERT
#define INCL_INSERT

                                        // DATABUFFER is currently UNUSED!
#define  SIZE_DATABUFFER ((UINT)35*(UINT)1024)       
                                        // Max size the code or data can get
                                        // 9/10/93 INSERT.ASM documents at 35K
#define  SIZE_CODEBUFFER ((UINT)35*(UINT)1024)       
#define  SIZE_HDRBUFFER  1024           // Size of EXE header 

#ifdef __cplusplus
extern "C" {
#endif

int WINAPI InsertSmallOVL (LPNAVOPTIONS lpNavOptions,
                           LPVOID lpCodeBuffer, WORD wCodeBufferSize,
                           LPVOID lpHdrBuffer, WORD wHdrBufferSize);

#ifdef __cplusplus
}
#endif

#endif
