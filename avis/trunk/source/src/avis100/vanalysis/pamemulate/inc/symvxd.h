/* Copyright 1992,1995 Symantec Corporation                                  */
/************************************************************************
 *                                                                      *
 * $Header:   S:/INCLUDE/VCS/SYMVXD.H_v   1.3   03 Feb 1997 11:29:30   LCOHEN  $ *
 *                                                                      *
 * Description:                                                         *
 *      Contains VxD ids and other VxD values.                          *
 *                                                                      *
 *      NOTE: Keep in sync with SYMVXD.INC.                             *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * $Log:   S:/INCLUDE/VCS/SYMVXD.H_v  $ *
// 
//    Rev 1.3   03 Feb 1997 11:29:30   LCOHEN
// Added IDs for NANEvent VxD.
//
//    Rev 1.2   03 Feb 1997 11:14:34   LCOHEN
// Added IDs for NANEvent VxD.
//
//    Rev 1.1   27 Feb 1996 14:40:08   MBARNES
// Synchronized with symvxd.inc
//
//    Rev 1.3   27 Feb 1996 14:39:38   MBARNES
// Synchronized with symvxd.inc
//
//    Rev 1.2   16 Aug 1995 16:06:12   RSTANEV
// Added SEPDID_NAVAP_PERSISTENTDATA.
//
//    Rev 1.1   02 Feb 1995 17:37:24   RSTANEV
// Cleaned up.  Added SEPDID_NAVAP_OPENFILEDATA.
//
//    Rev 1.0   27 Jan 1995 17:40:00   BRAD
// Initial revision.
 ************************************************************************/


//;=============================================================================
//;==     S Y M A N T E C   B A S E   V x D   I D   ( 3 2   a v a i l a b l e)
//;=============================================================================
#define SYMVxD_Device_ID        (9376)      // (293 << 5)

//;=============================================================================
//;==     V x D   I D s
//;=============================================================================
#define VNSS_Device_ID          (SYMVxD_Device_ID+0)
#define VNDWD_Device_ID         (SYMVxD_Device_ID+1)
#define SYMEvent_Device_ID      (SYMVxD_Device_ID+2)
#define PVCACHE_Device_ID       (SYMVxD_Device_ID+3)
#define SVCACHE_Device_ID       (SYMVxD_Device_ID+4)
//#define Unused_Device_ID      (SYMVxD_Device_ID+5)
//#define Unused_Device_ID      (SYMVxD_Device_ID+6)
#define NAVAP_Device_ID         (SYMVxD_Device_ID+7)
#define SYMEvntT_Device_ID      (SYMVxD_Device_ID+8)
#define AWHost_Device_ID        (SYMVxD_Device_ID+9)
#define DLVXD_Device_ID         (SYMVxD_Device_ID+10)
#define NANEvent_Device_ID      (SYMVxD_Device_ID+11)

//;=============================================================================
//;==     P E R S I S T E N T   D A T A   I D s
//;=============================================================================
#define SEPDID_SYMKRNL_LASTERROR    (1)
#define SEPDID_NAVAP_OPENFILEDATA   (2)
#define SEPDID_NAVAP_PERSISTENTDATA (3)
