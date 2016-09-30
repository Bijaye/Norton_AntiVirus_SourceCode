//************************************************************************
//
// $Header:   S:/NAVEX/VCS/config.h_v   1.86   14 Jan 1999 15:53:44   DKESSNE  $
//
// Description:
//  Defines constants for selective inclusion of customer issues in NAVEX
//  code.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/config.h_v  $
// 
//    Rev 1.86   14 Jan 1999 15:53:44   DKESSNE
// added SARC_CUST_11108
// 
//    Rev 1.85   24 Dec 1998 05:54:38   relniti
// REMOTEEXPLORER
// 
//    Rev 1.84   17 Dec 1998 15:09:20   relniti
// REMOVING #ifdef DKZOO
// 
//    Rev 1.83   09 Dec 1998 14:23:28   DKESSNE
// added SARC_CUST_9029
// 
//    Rev 1.82   09 Dec 1998 13:11:58   MMAGEE
// removed #define for SARC_SECURECOMPUTING
// removed #define for SARC_CUST_7557
// removed #define for SARC_CUST_7390
// removed #define for SARC_CUST_7982
// removed #define for SARC_CUST_7839
// 
//    Rev 1.81   08 Dec 1998 19:11:02   ECHIEN
// Added ifdef for ECZOO
// 
//    Rev 1.80   25 Nov 1998 15:25:28   relniti
// FP 7839 on a NAVEXSF function for Neuroquilla
// 
//    Rev 1.79   23 Nov 1998 19:24:22   DKESSNE
// added SARC_CUST_7982
// 
//    Rev 1.78   19 Nov 1998 13:55:50   relniti
// SARC_SECURECOMPUTING
// 
//    Rev 1.77   18 Nov 1998 15:41:32   DKESSNE
// added SARC_CUST_7390
// 
//    Rev 1.76   17 Nov 1998 14:23:32   DKESSNE
// added SARC_CUST_7557 for W95.Padania
// 
//    Rev 1.75   17 Nov 1998 10:00:14   AOONWAL
// removed #ifdef SARC_CUST_6689
// 
//    Rev 1.74   12 Nov 1998 14:15:08   DKESSNE
// added #define SARC_CUST_6689 for W95.HPS detection/repair
// 
//    Rev 1.73   11 Nov 1998 11:39:50   MMAGEE
// removed #defines for SARC_CST_5449, SARC_CST_5935, SARC_CST_5650
// 
//    Rev 1.72   21 Oct 1998 12:34:48   AOONWAL
// Added #define SARC_CST_5449
// 
//    Rev 1.71   20 Oct 1998 15:53:10   DKESSNE
// added SARC_CST_5935
// 
//    Rev 1.70   14 Oct 1998 12:39:58   DKESSNE
// added SARC_CST_5650
// 
//    Rev 1.69   14 Oct 1998 12:27:26   MMAGEE
// removed #define for SARC_CST_4577 (vid 258b, W95.Inca)
// removed #define for REDTEAMKRNL (vid 1ad7)
// removed #define for SARC_CST_4989 (vid 258b, W95.Inca)
// removed #define for SARC_CST_5401 (vid 7b70, Widowmaker.5747)
// 
//    Rev 1.68   09 Oct 1998 17:19:22   CFORMUL
// Added define for CUST 5401
// 
//    Rev 1.67   01 Oct 1998 14:14:44   DKESSNE
// added #define SARC_DKZOO for navex zoo defs
// 
//    Rev 1.66   30 Sep 1998 15:35:04   DKESSNE
// added #ifdef SARC_CST_4989 for Inca repair
// 
//    Rev 1.65   23 Sep 1998 16:25:22   MMAGEE
// reverting to rev 1.62 to restore red team kernel and W95.Inca ifdefs
// 
//    Rev 1.62   17 Sep 1998 17:05:50   JWILBER
// Added #define for CI 4577.
//
//    Rev 1.61   14 Sep 1998 20:04:16   JWILBER
// Added #define for REDTEAMKRNL, for RedTeam Krnl386 detection.
//
//    Rev 1.60   10 Sep 1998 15:06:16   AOONWAL
// removed #define SARC_CUST_4321 for Maryl offcourse
//
//    Rev 1.59   10 Sep 1998 14:59:14   MMAGEE
// removed #define for SARC_CUST_4213
//
//    Rev 1.58   09 Sep 1998 15:06:14   MMAGEE
// removed #define for SARC_CUST_MARKJ
//
//    Rev 1.57   08 Sep 1998 20:28:34   AOONWAL
// Added #define SARC_CUST_4321
//
//    Rev 1.56   08 Sep 1998 12:25:32   JWILBER
// Added #define for SARC_CUST_4213, for RedTeam Repair, CI 4213.
//
//    Rev 1.55   01 Sep 1998 12:31:34   DKESSNE
// added #define SARC_CUST_MARKJ
//
//    Rev 1.54   27 Aug 1998 14:09:42   JWILBER
// Removed SARC_ZOO #ifdefs.
//
//    Rev 1.53   27 Aug 1998 13:37:14   ECHIEN
// Removed SARC_ZOO_SAILOR
//
//    Rev 1.52   25 Aug 1998 22:53:30   ECHIEN
// Added SARC_ZOO_SAILOR
//
//    Rev 1.51   17 Aug 1998 21:54:30   jwilber
// Added #ifdef for SARC_ZOO_JJW, for my Zoo defs.
//
//    Rev 1.50   17 Aug 1998 11:41:56   DKESSNE
// moved SARC_ZOO_DARRENK out of #define ZOODEF block
//
//    Rev 1.49   17 Aug 1998 10:48:56   DKESSNE
// added #define SARC_ZOO_DARRENK
//
//    Rev 1.48   13 Aug 1998 19:00:18   PDEBATS
// Removed #define SARC_CST_MARBURGB VID 0x61a1
//
//    Rev 1.47   13 Aug 1998 09:44:08   jwilber
// Added #define for Marburg.B stuff.
//
//    Rev 1.46   12 Aug 1998 18:41:08   MMAGEE
// removed #define for SARC_VID_7346
//
//    Rev 1.45   03 Aug 1998 13:39:58   MMAGEE
// removed #define for sarc_marburg;  vid 0x2566; ci 2291
//
//    Rev 1.44   03 Aug 1998 12:22:28   PDEBATS
// No change.
//
//    Rev 1.43   03 Aug 1998 12:21:06   PDEBATS
// No change.
//
//    Rev 1.42   03 Aug 1998 12:19:52   PDEBATS
// Removed sarc_cst_2242 cihV1.2 define, also for navexsf.cpp and nepeshr.cpp
//
//    Rev 1.41   03 Aug 1998 12:18:54   PDEBATS
// No change.
//
//    Rev 1.40   03 Aug 1998 12:09:08   PDEBATS
// Removed sarc_cst_2071 crazy boot ; Boot repair passed
//
//    Rev 1.39   03 Aug 1998 11:51:08   PDEBATS
// Removed define statements for sarc_2461 and sarc_cst_pecih
//
//    Rev 1.38   30 Jul 1998 22:59:46   AOONWAL
// Added #define  SARC_2461
//
//    Rev 1.37   27 Jul 1998 11:43:38   DKESSNE
// added #define SARC_MARBURG for w95.Marburg
//
//    Rev 1.36   22 Jul 1998 18:50:26   jwilber
// Added SARC_VID_7346 for Raid/Krile repair mods - VID 7346, of course.
//
//    Rev 1.35   30 Jun 1998 13:09:04   DKESSNE
// added #define SARC_CST_2242
//
//    Rev 1.34   25 Jun 1998 19:27:20   jwilber
// Added #define for PE_CIH issue.
//
//    Rev 1.33   24 Jun 1998 17:42:06   RELNITI
// ADD SARC_CST_2071 for Crazy Boot Repair problem #2071
//
//    Rev 1.32   09 Jun 1998 21:48:42   JWILBER
// Added #define SARC_ZOO_WIN1 for compiling Windows zoo defs.
// Added #ifdef ZOODEF to conditionally compile zoo defs.
//
//    Rev 1.31   05 Jun 1998 08:55:30   PDEBATS
// Removed sarc_2042 define statement (navex repair code was in navexrf.cpp)
//
//    Rev 1.30   03 Jun 1998 20:06:56   PDEBATS
// Defined SARC_CST_2042 so navexrf.cpp has true condition for ifdef cst2042
//
//    Rev 1.29   05 May 1998 12:14:58   PDEBATS
// Removed define statement for cst1880 Trainer Trojan vid 6152
//
//    Rev 1.28   04 May 1998 16:59:44   JWILBER
// Added #define for CI 1880, VID 6152, Trainer Trojan.
//
//    Rev 1.27   27 Apr 1998 16:00:10   PDEBATS
// Removed cst1746 define for vid 6149 "sex 98"
//
//    Rev 1.26   18 Apr 1998 14:39:30   JWILBER
// Added #define for CI 1746 to include Sex98, VID 6149.
//
//    Rev 1.25   16 Apr 1998 15:38:56   PDEBATS
// passed cst1745 pwsteal
//
//    Rev 1.24   15 Apr 1998 20:05:52   JWILBER
// Added SARC_CST_1745
//
//    Rev 1.23   03 Apr 1998 15:02:58   FBARAJA
// Removed #define SARC_CST_1575
//
//    Rev 1.22   01 Apr 1998 12:55:38   PDEBATS
// Removed define statement for cst1603
//
//    Rev 1.21   26 Mar 1998 16:02:26   JWILBER
// Added #define for CI 1603.
//
//    Rev 1.20   26 Mar 1998 11:44:24   FBARAJA
// Removed #define SARC_CST_REDTEAM
//
//    Rev 1.19   25 Mar 1998 23:03:20   JWILBER
// Added #define for CI 1575.
//
//    Rev 1.18   25 Mar 1998 20:57:30   FBARAJA
// Remove #ifdef SARC_CST_1608
//
//    Rev 1.17   25 Mar 1998 20:46:10   FBARAJA
// Removed #ifdef SARC_CST_1582
//
//    Rev 1.16   25 Mar 1998 18:47:04   JWILBER
// Added #define for CI 1608.
//
//    Rev 1.15   24 Mar 1998 18:02:38   DKESSNE
// added #define SARC_CST_1582
//
//    Rev 1.14   24 Mar 1998 15:33:14   JWILBER
// Added #define for SARC_CST_REDTEAM.
//
//    Rev 1.13   19 Mar 1998 16:40:04   FBARAJA
// Removed #define SARC_CST_1490
//
//    Rev 1.12   19 Mar 2000 11:09:14   hjaland
// Added 1490 for further testing.
//
//    Rev 1.11   18 Mar 2000 11:11:50   hjaland
// Removed #ifdef SARC_CST_1490
//
//    Rev 1.10   17 Mar 1998 17:03:40   PDEBATS
// Removed defines for cst1508 and cst1525
//
//    Rev 1.9   16 Mar 1998 19:48:00   hjaland
// Removed #ifdef SARC_CST_1502
//
//    Rev 1.8   14 Mar 1998 19:21:24   JWILBER
// Added #define for CI 1525, AOL Trojan 1525, VID 612a.
//
//    Rev 1.7   13 Mar 1998 12:16:50   JWILBER
// Added 1508.
//
//    Rev 1.6   12 Mar 1998 20:08:04   RELNITI
// add 1490
//
//    Rev 1.5   11 Mar 1998 20:51:54   FBARAJA
// Removed #ifdef SARC_CST_1462 (HJ)
//
//    Rev 1.4   11 Mar 1998 19:43:00   JWILBER
// Added comments back - added a CR-LF at the end of the file
// to fix things.
//
//    Rev 1.3   11 Mar 1998 19:34:42   JWILBER
// Removed comments which may be messing up the #defines.
//
//    Rev 1.2   11 Mar 1998 19:22:30   JWILBER
// Added #define for CI 1502.
//
//    Rev 1.1   10 Mar 1998 14:38:38   JWILBER
// Added #define for CI 1482.
//
//    Rev 1.0   10 Mar 1998 14:20:34   JWILBER
// Initial revision.
//
//************************************************************************

#ifndef CONFIG_H

#define CONFIG_H

// Defines for Customer Issues go here
#define SARC_REMOTEEXPLORER
#define SARC_ECZOO          // for NAVEX zoo defs -- Darren K.
#define SARC_CUST_11108

#ifdef ZOODEF

// Defines for Zoo def stuff go here

#endif // ZOODEF

#endif // CONFIG_H
