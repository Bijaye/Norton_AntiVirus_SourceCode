// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
//************************************************************************
//
// $Header:   S:/NAVEX/VCS/navexvid.h_v   1.117   14 Jan 1999 15:18:22   DKESSNE  $
//
// Description:
//      Contains NAVEX non-macro VIDs.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/navexvid.h_v  $
// 
//    Rev 1.117   14 Jan 1999 15:18:22   DKESSNE
// added ID_W95MEMORIAL 0x25a4
// 
//    Rev 1.116   24 Dec 1998 22:45:58   DCHI
// Added #define for ID_REMXCORRUPT.
// 
//    Rev 1.115   23 Dec 1998 18:02:56   relniti
// ID_REMOTEEXPLORE 0x1d90
// 
//    Rev 1.114   15 Dec 1998 17:02:36   CFORMUL
// Added ID_TERMITE
// 
//    Rev 1.113   10 Dec 1998 16:02:48   DKESSNE
// added ID_K32 0x25d1
// 
//    Rev 1.112   09 Dec 1998 14:22:14   DKESSNE
// added VID_K32 0x25d1
// 
//    Rev 1.111   08 Dec 1998 19:10:14   ECHIEN
// Added ID_TMCB and VID_TMCB
// 
//    Rev 1.110   02 Dec 1998 13:48:40   DKESSNE
// added ID_LEXOTRAN and VID_LEXOTRAN, both 0x25c5
// 
//    Rev 1.109   30 Nov 1998 14:51:16   DKESSNE
// added ID_ZOMBIEZCME and VID_ZOMBIEZCME, both 0x25c4
// 
//    Rev 1.108   23 Nov 1998 19:23:24   DKESSNE
// added VID_PARVO and ID_PARVO, both 0x25c3
// 
//    Rev 1.107   19 Nov 1998 15:22:34   DKESSNE
// added VID_LUDYEL 0x25c1 and VID_LUDHILL 0x25c2
// 
//    Rev 1.106   19 Nov 1998 12:22:04   DKESSNE
// added VID_SHAITAN 0x25c0
// 
//    Rev 1.105   18 Nov 1998 22:45:08   relniti
// id_144floppy back to 0000
// 
//    Rev 1.104   18 Nov 1998 21:52:36   relniti
// blah
// 
//    Rev 1.103   18 Nov 1998 21:30:26   relniti
// ADD ID_144FLOPPY
// 
//    Rev 1.102   18 Nov 1998 18:03:12   CFORMUL
// Pulled daboysf define
// 
//    Rev 1.101   18 Nov 1998 16:27:16   CFORMUL
// Added ID_DABOYSF
// 
//    Rev 1.100   17 Nov 1998 14:30:18   DKESSNE
// added ID_PADANIA and VID_PADANIA, both 0x25be
// 
//    Rev 1.99   12 Nov 1998 14:17:56   DKESSNE
// added ID_HPS 0x6160 for W95.HPS 
// 
//    Rev 1.98   21 Oct 1998 17:17:18   DKESSNE
// added VID_LIBERTINE 0x25bb
// 
//    Rev 1.97   20 Oct 1998 17:51:44   DKESSNE
// added VID_EVIL 0x25b8
// 
//    Rev 1.96   14 Oct 1998 17:08:34   DKESSNE
// added VID_IKX for W95.IKX 0x25ab
// 
//    Rev 1.95   14 Oct 1998 12:37:28   DKESSNE
// removed VID_CABANAS, added ID_CABANAS for repair
// 
//    Rev 1.94   12 Oct 1998 16:05:46   DKESSNE
// added VID_CEZAR and ID_CEZAR, both 0x25a9
// 
//    Rev 1.93   10 Oct 1998 14:54:40   DKESSNE
// changed my VID_MEMORIAL to VID_W95MEMORIAL
// 
//    Rev 1.92   10 Oct 1998 13:42:26   DKESSNE
// added #define VID_MEMORIAL 0x25a4
// 
//    Rev 1.91   09 Oct 1998 12:51:56   DKESSNE
// added VID_OLIVIA 0x25a3 
// 
//    Rev 1.90   08 Oct 1998 17:57:58   CFORMUL
// Changed ID_WIDOW
// 
//    Rev 1.89   08 Oct 1998 17:39:30   CFORMUL
// Added VID_WIDOW
// 
//    Rev 1.88   08 Oct 1998 17:31:44   CFORMUL
// Added ID_WIDOW
// 
//    Rev 1.87   08 Oct 1998 16:37:20   DKESSNE
// added VID_VFW 0x259e and VID_VECNA 0x259f
// 
//    Rev 1.86   08 Oct 1998 11:03:24   DKESSNE
// added #define VID_HARRYB 0x2599
// 
//    Rev 1.85   02 Oct 1998 15:35:44   DKESSNE
// added ID_LIGHTNING and VID_LIGHTNING 0x2545
// 
//    Rev 1.84   01 Oct 1998 14:07:02   DKESSNE
// added ID_VANITAS3712 and VID_VANITAS3712, both 0x2544
// 
//    Rev 1.83   30 Sep 1998 15:28:40   DKESSNE
// added ID_INCA 0x258b
// 
//    Rev 1.82   24 Sep 1998 12:15:48   cng
// added 0x7587 Demon3b.5350
// 
//    Rev 1.81   15 Sep 1998 13:09:42   JWILBER
// Changed VID for RedTeam Kernel to 1ad7.
//
//    Rev 1.80   14 Sep 1998 20:11:14   JWILBER
// Added #defines for Red Team Kernel, VID 61d2.
//
//    Rev 1.79   08 Sep 1998 18:21:08   AOONWAL
// #define ID_CREED 0x5652
//
//    Rev 1.78   08 Sep 1998 12:14:04   JWILBER
// Added #define ID_REDTEAM for RedTeam repair, VID 6130, CI 4213.
//
//    Rev 1.77   01 Sep 1998 12:36:40   DKESSNE
// added VID_MARKJ and ID_MARKJ
//
//    Rev 1.76   25 Aug 1998 23:09:52   ECHIEN
// Added ID_SAILOR, VID_SAILORCOM, VID_SAILOREXE
//
//    Rev 1.75   25 Aug 1998 12:27:06   CNACHEN
// added w95.inca detection, commented out with #ifdef SARC_ZOO_DARRENK
//
//    Rev 1.74   24 Aug 1998 15:34:02   DKESSNE
// added VID_SSR19834 and ID_SSR19834
//
//    Rev 1.73   20 Aug 1998 18:00:58   DKESSNE
// removed VID_CEREBRUS
//
//    Rev 1.72   20 Aug 1998 14:16:38   DKESSNE
// added VID_PUNCH 0x257f and VID_CEREBRUS 0x2580
//
//    Rev 1.71   19 Aug 1998 17:45:44   jwilber
// Added #defines for Apparition.W95.B, and Anxiety.E.
//
//    Rev 1.70   19 Aug 1998 12:46:16   CNACHEN
// Updated windows engine to scan last section of PE files, added
// W95.Cabanas.E
//
//    Rev 1.69   18 Aug 1998 13:10:10   DKESSNE
// added VID_ZOMBIE  0x2570
//
//    Rev 1.68   17 Aug 1998 21:55:04   jwilber
// Added VIDs for:
// Apparition.C 61a6
// Apparition.D 61a7
// Apparition.A 61a8
// Skim.1454    61a9
//
//    Rev 1.67   17 Aug 1998 17:05:52   DKESSNE
// added VID_HARRY 0x256f
//
//    Rev 1.66   17 Aug 1998 11:26:26   DKESSNE
// added VID_DARKSIDE and VID_SGWW
//
//    Rev 1.65   12 Aug 1998 20:02:56   jwilber
// Added ID_MARBURGB, and VID_MARBURGB, both 0x61a1.
//
//    Rev 1.64   29 Jul 1998 18:29:04   AOONWAL
// changed 1c09 to 1ca9
//
//    Rev 1.63   29 Jul 1998 14:07:52   AOONWAL
// Added #define ID_W95HLL480099  0x1c09
//
//    Rev 1.62   23 Jul 1998 09:48:54   DKESSNE
// added VID_MARBURG and ID_MARBURG, both 0x2566
//
//    Rev 1.61   26 Jun 1998 12:43:10   DKESSNE
// renamed VID_PECIH to VID_CIH, added ID_CIH 0x617e
//
//    Rev 1.60   25 Jun 1998 19:49:14   jwilber
// Added #define VID_PECIH for a Win32 virus.
//
//    Rev 1.59   03 Jun 1998 09:06:32   AOONWAL
// Added #define ID_POJER 0x2f74
//
//    Rev 1.58   29 May 1998 16:21:24   JWILBER
// Added VID for Jacky.1107, 616b.
//
//    Rev 1.57   29 May 1998 12:56:46   JWILBER
// Changed VIDs for Homer NE viruses, and added VIDs for PE Zoo
// viruses.
//
//    Rev 1.56   28 May 1998 15:41:50   JWILBER
// Added VID_HOMER and VID_HOMERPAY for Zoo Defs.
//
//    Rev 1.55   18 May 1998 17:56:10   DKESSNE
// VID_HFS -> VID_HPS
//
//    Rev 1.54   18 May 1998 17:01:50   DKESSNE
// added VID_HFS  0x6160
//
//    Rev 1.53   11 May 1998 17:59:28   DCHI
// Added ID_ACCESS2 for Access 2.0 NAVEX call entry.
//
//    Rev 1.52   21 Apr 1998 17:37:16   DKESSNE
// added VID_LOREZ  0x2515
//
//    Rev 1.51   18 Apr 1998 14:31:34   JWILBER
// Added VID_SEX98 for VID 6149, CI 1746.
//
//    Rev 1.50   17 Apr 1998 11:14:24   DCHI
// Added ID_ACCESS97 for Access 95/97 NAVEX call entry.
//
//    Rev 1.49   16 Apr 1998 18:06:20   DKESSNE
// removed VID_DAMNPOPPY
//
//    Rev 1.48   16 Apr 1998 18:00:04   DKESSNE
// added VID_DAMNPOPPY  0x2515
//
//    Rev 1.47   25 Mar 1998 23:03:40   JWILBER
// Added a #define for VID_ANXIETYPOPPYIII, VID 6133, CI 1575.
//
//    Rev 1.46   24 Mar 1998 17:08:36   JWILBER
// Added #define for VID_REDTEAM, VID 6130.
//
//    Rev 1.45   24 Mar 1998 15:45:44   DKESSNE
// added ID_TPVO3464  0x250c
//
//    Rev 1.44   13 Mar 1998 03:56:08   CFORMUL
// Added defines for Cryptor.4161 detections
//
//    Rev 1.43   12 Mar 1998 20:12:48   RELNITI
// ADD ID_KLUNKY
//
//    Rev 1.42   12 Mar 1998 14:57:50   RELNITI
// ADD VID_KLUNKY 6128
//
//    Rev 1.41   10 Mar 1998 22:33:30   AOONWAL
// Added define for ID_TMC 0x1ca2
//
//    Rev 1.40   10 Mar 1998 15:23:14   JWILBER
// Added repair id ID_APPAR for Apparition.89021 - CI 1462.
//
//    Rev 1.39   10 Feb 1998 18:33:24   CFORMUL
// Added defines for Inivisible detection
//
//    Rev 1.38   10 Feb 1998 14:01:40   CFORMUL
// Changed ID for Wolleh repair
//
//    Rev 1.37   05 Feb 1998 16:35:28   DKESSNE
// Added #defines for Wolleh detection and repair
//
//    Rev 1.36   02 Feb 1998 16:42:04   CFORMUL
// ADDED ID_UN
//
//    Rev 1.35   28 Jan 1998 17:17:48   JWILBER
// Reconciled from DEFWORK - commented out ID_ANXIETYPOPPYII, since
// it uses the same repair as ID_POPPY, and added #define for
// Raid.5831 repair ID - CI 1041.
//
//    Rev 1.34   19 Jan 1998 11:04:26   CFORMUL
// Added defines for Cryptor.2582
//
//    Rev 1.33   15 Jan 1998 18:24:34   JWILBER
// Added defines for Windows viruses:
// VKit.650  VID 60ef
// VKit.1241     60f0
// Winvik.B      60f1
// Winvir.961    60f2
//
//    Rev 1.32   10 Jan 1998 01:41:30   CFORMUL
// Added VID_CRYPTOR2169
//
//    Rev 1.31   10 Jan 1998 01:34:14   CFORMUL
// Added ID for Cryptor.2169 scan functions
//
//    Rev 1.30   08 Jan 1998 13:23:58   CFORMUL
// Added defines for Mad.5054 detection
//
//    Rev 1.29   29 Dec 1997 17:58:34   JWILBER
// Modified during Jan98 build.
//
//    Rev JAN98  17 Dec 1997 17:37:40   ECHIEN
// Remove ID_LILITH and ID_LILITHMBR
//
//    Rev JAN98  17 Dec 1997 17:05:24   ECHIEN
// Added ID_LILITH 0x2447 and ID_LILITHMBR
//
//    Rev JAN98  17 Dec 1997 13:13:50   ECHIEN
// Added Lilith to navexsb and navexsp 0x2447
//
//    Rev 1.28   01 Dec 1997 20:01:10   JWILBER
// Added #defines VID_VICOTIN and VID_SKIM for Vicotin.1168 and
// Skim.1455, VIDs 608d/e.
// Removed #defines for DuBug, since they aren't needed.
//
//    Rev 1.27   01 Dec 1997 19:48:30   RELNITI
// ADD ID_ANXIETYPOPPYII 5032 & VID_ANXIETYPOPPYII 6092
//
//    Rev 1.26   26 Nov 1997 04:17:24   JWILBER
// Added ID for DuBug repair, ID_DUBUG, 0x4455.
//
//    Rev 1.25   03 Nov 1997 20:50:44   JWILBER
// Added #define and repair ID for Poppy.1052, VID 6071, for CI 417.
//
//    Rev 1.24   30 Oct 1997 10:40:14   JWILBER
// Added VID_POPPY for Poppy.1052, VID 6071 for CI 417.  This is
// a Win95 infector.
//
//    Rev 1.23   27 Oct 1997 16:01:20   AOONWAL
// Added define for WPCB.3207
//
//    Rev 1.22   20 Oct 1997 21:15:22   aoonwal
// Added define ID_SWISS 0x08db
//
//    Rev 1.21   15 Oct 1997 19:26:02   CFORMUL
// Changed VID_CHAN to 1d68
//
//    Rev 1.20   01 Oct 1997 22:52:28   JWILBER
// Added ID_R11 for R11 virus, VID 4cf3.
//
//    Rev 1.19   05 Sep 1997 20:40:32   RELNITI
// ADD ID_DIGI3547 & VID_DIGI3547
//
//    Rev 1.18   05 Sep 1997 20:27:06   CNACHEN
// Added bloodhound vids
//
//    Rev 1.17   05 Sep 1997 12:29:04   CNACHEN
// Added bloodhound VIDs
//
//    Rev 1.16   26 Aug 1997 10:38:36   JWILBER
// Modified during Sep 97 build.
//
//    Rev 1.15   21 Aug 1997 14:12:10   RELNITI
// ADD VID_CHAN 0x1d69
//
//    Rev 1.14   13 Aug 1997 16:32:32   DANG
// Added ID_INT12 for INT12 boot repair
//
//    Rev 1.13   07 Aug 1997 15:43:16   CFORMUL
// Added VID_GINGER and ID_GINGER
//
//    Rev 1.12   06 Aug 1997 12:11:10   CFORMUL
// Added VID_MIDSUMMER
//
//    Rev 1.11   28 Jul 1997 18:17:46   AOONWAL
// Added VID_RUSTYBUG5850
//
//    Rev 1.10   28 Jul 1997 12:56:06   AOONWAL
// Modified during AUG97 update
//
//    Rev AUG97   23 Jul 1997 13:34:14   DANG
// Added repair for DeltaX (4C07)
//
//    Rev 1.9   19 Jun 1997 14:26:48   CFORMUL
// Added ID_MONKEY
//
//    Rev 1.8   16 Jun 1997 12:14:46   RELNITI
// MODIFY 1e9f: VID_TWNOK => VID_BOXA
//
//    Rev 1.7   11 Jun 1997 17:20:18   JWILBER
// Added #define for Password Stealing Trojan, 4d1f.
//
//    Rev 1.6   15 May 1997 15:41:26   JWILBER
// Added #define for AoTaun Trojan, VID_WINAOTAUN, 4ebd.
//
//    Rev 1.5   06 May 1997 13:35:56   DCHI
// Removed many macro vid definitions.
//
//    Rev 1.4   06 May 1997 12:04:26   CFORMUL
// Added VID_LAROUXC
//
//    Rev 1.3   28 Apr 1997 17:27:38   AOONWAL
// Added VID_DEFINE for SAIGON boot repair
//
//    Rev 1.2   23 Apr 1997 19:38:16   AOONWAL
// ADD repair ID for WEED.5850
//
//    Rev 1.1   04 Apr 1997 15:57:04   RELNITI
// RENAME 0x4f97: VID_SHOWOFFV => VID_SHOWOFFW
//        0x4f99: VID_SHOWOFFW => VID_SHOWOFFX
//
//    Rev 1.0   03 Apr 1997 10:45:00   DCHI
// Initial revision.
//
//************************************************************************

#ifndef _NAVEXVID_H_

#define _NAVEXVID_H_

// VID for excluding FPs in Navex using the trojan scanner
#define GOOD_EXCLUDE_VID                0x4A11

// Function ID's
// Do not use 0x00 as either hi or lo order byte.
// This is to allow backwards compatibiltiy in the virscan.dat with
// ver <= 3.0.7

#define ID_CORDOBES     0x4443          // Detects Cordobes (and repairs)
#define ID_SMEG         0x4d53          // Detects Smeg
#define ID_NATAS        0x414E          // Detects Natas
//#define ID_TPEBOSNIA    0x4254          // Detects TPE.Bosnia
#define ID_MACRO        0x6477          // Detects Word Macro Family
#define ID_MSO          0x736D          // Detects EDITDATA.MSO
#define ID_ACCESS2      0xA2A2          // Detects Access 2.0
#define ID_ACCESS97     0x6361          // Detects Access 95/97
#define ID_NIGHTFALL    0x716e          // Detects Nightfall and Neuroquila
#define ID_NIGHTFALL2   0x6e6e          // Second Nightfall/Neuroquila call
#define ID_NIGHTFALL3   0x6e6f          // Nightfall.5815 enhanced checking.
#define ID_ONEHALF      0x4F48          // One Half
#define ID_URUG4        0x4755          // Uruguay 4
#define ID_VAGABOND     0x4756          // Vagabond
#define ID_GRIPE        0x5247          // Gripe.2040
#define ID_RDA          0x4452          // RDA viruses
#define ID_CRUNCHER     0x5243          // Cruncher viruses "CR"
#define ID_HARE         0x4b48          // Hare.7610
#define ID_VICE         0x4356          // VICE
#define ID_FREDDY       0x4B46          // Freddy_Krueger
#define ID_TENTRK       0x5454          // Tentrickle det (& rep later)
#define ID_MAD5054      0x5053          // Mad.5054
#define ID_MONKEY       0x4d4d          // Stoned.Empire.Monkey
#define ID_DIGI3547     0x3547          // Digi.3547 / Digital.3547
#define ID_CRYPTOR      0x2169          // Cryptor.2169
#define ID_CRYPTOR2582  0x2582          // Cryptor.2582
#define ID_CRYPTOR4161  0x4161          // Cryptor.4161
#define ID_SSR19834     0x258a          // SSR19834 VID 258a
#define ID_VANITAS3712  0x2544          // Vanitas.3712 VID 2544
#define ID_LIGHTNING    0x2545          // Lightning.4251 VID 2545
#define ID_NUTCRACKER   0x8023          // Nutcracker.Family VID 8023
#define ID_NUTC2890     0x8027          // Nutcracker.Family size 2890 VID 8027
#define ID_NUTC7008     0x8029          // Nutcracker.Family size 7008 VID 8029
#define ID_CEZAR        0x25a9          // Cezar.4567 VID 25a9
#define ID_SAILOR       0x7b66          // Sailor.Pluto.3672 VID 7b66 and 7b67
#define ID_DEMON3B5350  0x7587          // Demon3b.5350 VID 7587
#define ID_WIDOW        0xf2b1          // Widowmaker.5747
#define ID_ZOMBIEZCME   0x25c4          // Zombie.ZCME.8192 VID 25c4
#define ID_LEXOTRAN     0x25c5          // Lexotran VID 25c5
#define ID_TMCB         0x245B          // TMC.B 0x245B
#define ID_SKCOM        0x2a46          // W95.SK (com) 0x2a46

#define ID_WINDOWS      0x5a42          // WI - windows

#define ID_JAVA         0x564A          // JV - Java

#define ID_LOVEFWA      0x7009          // VBS.LoveLetter.FW.A

#if defined(NAVEX_QA)                   // QA ScanFile Test ID
#define ID_QATESTDEF    0x4141
#endif                                  // NAVEX_QA

// Repair ID's
// Exec_Code control flag plus these values in the bookmark field
// determine which repair function to invoke.

// File Viruses

#define ID_BATMAN2      0x4232          // Batman II
#define ID_BURGLAR      0x7562          // Burglar.1150
#define ID_CPW          0x4350          // CPW
#define ID_DELTAX       0x4C07          // DeltaX repair
#define ID_DIAMOND      0x5244          // DiamondRockSteady
#define ID_DH           0x4844          // Die Hard
#define ID_EMMIE        0x4d45          // Emmie repair, 3 strains.
#define ID_FAIRZ        0x7a66          // Fairz
#define ID_FOUR11       0x040b          // FourEleven.2144
#define ID_GENRAT       0x4547          // Genrat.785
#define ID_GINGER       0x4F5A          // Ginger
#define ID_GODZIL       0x5a47          // Godzilla.890
#define ID_HLLT4416     0x4416          // HLLT.4416
#define ID_HOOT4676     0x1e05          // Hoot.4676
#define ID_I4060        0x6161          // Infector.4060
#define ID_JACKAL_F     0x464a          // Jackal (file)
#define ID_JACKAL       0x494a          // Jackal (boot)
#define ID_KARN         0x4B41          // Karnivali.1971
#define ID_LR           0x524c          // Little Red.1465
#define ID_MANUEL       0x4d61          // Manuel.2209
#define ID_MANZON       0x7a6d          // Manzon.1445
#define ID_OXANA        0x584f          // Oxana.1654/1670/1671
#define ID_PRED24       0x4450          // Predator.2448
#define ID_QUARTZ       0x1baa          // Quartz
#define ID_QUICKY       0x6262          // Quicky.1376
#define ID_R11          0x4552          // R11
#define ID_RAID         0x5652          // RV for Raid Virus (Raid.5831)
#define ID_RUSTYBUG5850 0x1C8D          // RustyBug.5850
#define ID_TANPRO       0x5054          // Tanpro.524
#define ID_VACSINA1     0x06aa          // Vacsina.TP-04
#define ID_VACSINA2     0x06ab          // Vacsina.TP-04 (x)
#define ID_VMEM         0x4d56          // Vmem and Vmem (x) repair
#define ID_UNKNOWN      0x4b55          // Unknown.1329
#define ID_YESMILE      0x4159          // Yesmile.5504
#define ID_WEED         0x3263          // Weed.3263
#define ID_WEED11       0x4080          // Weed v1.1 (Weed.4080)
#define ID_WEED5850     0x4E2A          // Weed v2   (Weed.5850)
#define ID_WEREWOLF     0x5757          // Werewolf
#define ID_YKEY         0x1b99          // Y-Key.2346
#define ID_ZARMA        0x617a          // Zarma
#define ID_WPCB         0x1565          // WPCB.3207
#define ID_TMC          0x1ca2          // TMC_Level-69
#define ID_POJER        0x2f74          // Pojer.4028
#define ID_IROK         0x484d          // Irok.Trojan.Worm
#define ID_IROK7840     0x2a5b          // Irok.7840
#define ID_W95HLL480099 0x1ca9          // W95.HLL.480099
#define ID_CREED        0x5652          // Creed.5209
#define ID_TERMITE      0x6515          // Termite
#define ID_TERMITE6585  0x74aa          // Termite.6585
#define ID_TERMITE5000B 0x25f5          // Termite.5000.B
#define ID_TERMITE7800  0x74ad          // Termite.7800
#define ID_TERMITE7800B 0x74b2          // Termite.7800.B

// Windows Viruses

#define ID_BOZA         0x7762          // WB for Windows Boza
#define ID_POPPY        0x5050          // PP for Windows Poppy
//#define ID_ANXIETYPOPPYII 0x5032  // Not needed - same repair as Poppy
#define ID_TENT         0x7774          // WT for Windows Tentacle
#define ID_TENT2        0x7432          // T2 for Windows Tentacle II
#define ID_TPVO         0x5470          // Tp for Windows Tpvo
#define ID_APPAR        0x4150          // Ap for Apparation
#define ID_KLUNKY       0x6128          // VID for Klunky
#define ID_CIH          0x617e          // VID for CIH
#define ID_INCA         0x258b          // VID for Inca
#define ID_MARBURG      0x2566          // VID for Marburg.A
#define ID_MARBURGB     0x61a1          // VID for Marburg.B
#define ID_MARKJ        0x258d          // VID for MarkJ
#define ID_CABANAS      0x2572          // VID for Cabanas
#define ID_REDTEAM      0x5452          // RT for RedTeam
#define ID_REDTEAMKRNL  0x544b          // RK for RedTeamKrnl
#define ID_HPS          0x6160          // VID for HPS
#define ID_PADANIA      0x25be          // VID for Padania
#define ID_PARVO        0x25c3          // VID for Parvo
#define ID_K32          0x25d1          // VID for K32
#define ID_REMOTEEXPLORE 0x1d90         // VID for W32.RemoteExplore
#define ID_REMXCORRUPT  0x1d94          // VID for W32.RemoteExp.Corrupt
#define ID_W95MEMORIAL  0x25a4          // VID for W95.Memorial
#define ID_KENSTON      0x25dc          // VID for W95.Kenston
#define ID_SEXY         0x2807          // VID for W95.Sexy
#define ID_LOREZ        0x2515          // VID for W95.Lorez
#define ID_UWAGA        0x280a          // VID for W95.Uwaga
#define ID_APATHY       0x280b          // VID for W32.Apathy
#define ID_MAYA         0x280c          // VID for W32.Maya
#define ID_JADIS        0x280d          // VID for W95.Lud.Jadis
#define ID_HERETIC      0x2810          // VID for W32.Heretic
#define ID_REDEMPTION   0x2816          // VID for W32.Redemption
#define ID_REGSWAP      0x2817          // VID for W95.Regswap
#define ID_VB           0x2818          // VID for W95.VB
#define ID_TWINNY       0x2819          // VID for W95.Twinny
#define ID_YABRAN       0x282a          // VID for W95.Yabran
#define ID_WINZERG      0x282b          // VID for W95.Zerg
#define ID_VOODOO       0x2837          // VID for W95.Voodoo
#define ID_POWERFUL     0x2838          // VID for W95.Powerful
#define ID_LEVI         0x283b          // VID for W95.Levi
#define ID_TENTACLE     0x283c          // VID for W95.Tentacle
#define ID_APPARITION   0x283e          // VID for W95.Apparition
#define ID_GIRI         0x283f          // VID for W95.Giri
#define ID_LUD_JEZ      0x2840          // VID for W95.Lud.Jez
#define ID_HIGHWAY      0x2841          // VID for W95.Highway
#define ID_OBSOLETE     0x2845          // VID for W95.Obsolete
#define ID_NIKO         0x2846          // VID for W95.Niko
#define ID_SAVIOR       0x2847          // VID for W95.Savior
#define ID_RUFF         0x284b          // VID for W95.Ruff
#define ID_ENUMIACS     0x284c          // VID for W95.Enumiacs
#define ID_BABYLONIA_HELP 0x25f8        // VID for W95.Babylonia (HLP)
#define ID_HELP         0x2d04          // ID for HLP file scanning

// Boot Viruses

#define ID_CRUEL        0x5243          // Boot Repair for Cruel
#define ID_DABOYS       0x4244          // Boot Repair for Da'Boys
#define ID_ETHER        0x5445          // Boot Repair for Ether
#define ID_ROMA         0x4d52          // Boot Repair for Roma.PG
#define ID_EDWIN        0x4445          // Boot Repair for Edwin
#define ID_KARNIVALI    0x414B          // Boot Repair for Karnivali
#define ID_SAIGON       0x5341          // Boot Repair for Saigon
#define ID_INT12        0x1561          // Boot Repair for Int12
#define ID_SWISS        0x08db          // Boot Repair for Swiss Boot
#define ID_WOLLEH       0x0561          // Boot Repair for Wolleh
#define ID_144FLOPPY    0x0000          // Boot Repair for 144Floppy

// MBR Viruses

#define ID_BH           0x414a          // MBR Repair for Bad_Head
#define ID_CB           0x4243          // MBR Repair for Crazy Boot
#define ID_DANEBOOT     0x4e44          // MBR Repair for Danish Boot
#define ID_FRANK        0x3052          // MBR Repair for Frankenstein
#define ID_ETP          0x7065          // MBR Repair for Esto te pasa
#define ID_NEURO        0x726e          // MBR Repair for Neuro
#define ID_PREDT        0x5250          // MBR Repair for Predator
#define ID_TPVO3464     0x250c          // MBR Repair for TPVO.3464 0x250c
#define ID_URKEL        0x5255          // MBR Repair for Urkel
#define ID_UN           0x4E55          // MBR Repair for UN
#define ID_IMPLANT      0x2592          // MBR Repair for Implant.MP.6128 (b)
#define ID_EEKBOOT      0x2c01          // MBR and BS Repair for Eek (b)
#define ID_WYX          0x2c1b          // MBR and BS Repair for Wyx (b)

// VID's returned by scan functions

//#define VID_TPEBOSNIA       0x1f01
#define VID_SAILORCOM       0x7b67
#define VID_SAILOREXE       0x7b66
#define VID_MAD5054         0x791d
#define VID_CRYPTOR2169     0x792e
#define VID_CRYPTOR2582     0x7cd1
#define VID_CRYPTOR4161     0x7986
#define VID_MIDSUMMER       0x1bcd
#define VID_GINGER          0x1bce
#define VID_NIGHTFALL       0x1f07
#define VID_NIGHTFALLB      0x1f14
#define VID_NEUROQUILA      0x1f15
#define VID_NEURO_BOOT      0x1f26
#define VID_SMEG            0x1f27
#define VID_NATAS           0x1f2a
#define VID_PREDT_BOOT      0x1f58
#define VID_URUG4           0x1f5d
#define VID_ONEHALF         0x2f41
#define VID_UNKNOWN         0x1f5f
#define VID_HOT0            0x1f67
#define VID_HOT             0x1f67
#define VID_HOT1            0x1f68
#define VID_IMP0            0x1f7c
#define VID_IMPOSTERA       0x1f7c
#define VID_IMP1            0x1f7d
#define VID_TWITCH          0x1f66
#define VID_CYBERIOT        0x1f63
#define VID_PH33R           0x1f62
#define VID_VAGABOND        0x1f6c
#define VID_GRIPE           0x1f6e
#define VID_CORDOBES        0x1f7a
#define VID_IRISH0          0x1f88
#define VID_IRISH           0x1f88
#define VID_IRISH1          0x1f89
#define VID_INFEZIONE       0x1f8a
#define VID_DATE            0x1f8a
#define VID_DANISHBOOT      0x1f8f
#define VID_RDA             0x1f91
#define VID_XENIXOS         0x1f93
#define VID_BOOM            0x1f94
#define VID_CRUNCHER_COM    0x1f95
#define VID_CRUNCHER_EXE    0x1f96
#define VID_HARE_BOOT       0x1f99
#define VID_MOLOCH          0x1f9a
#define VID_PARASITE1       0x1f9b
#define VID_NOP1            0x1f9c
#define VID_NOPA            0x1f9c
#define VID_MDMADMV         0X1f9d
#define VID_MDMAA           0X1f9d
#define VID_PARASITE0       0x1f9e
#define VID_HARE            0x1f9f
#define VID_HARE7610        0x2fe7
#define VID_HARE7750        0x2ed9
#define VID_HARE7786        0x2fe6
#define VID_NOP2            0x1fa1
#define VID_ATOM            0x1fa4
#define VID_ATOMA           0x1fa4
#define VID_MADDOG          0x1fa5
#define VID_MADDOGA         0x1fa5
#define VID_POLITE          0x1fa7
#define VID_PHANTOM         0x1fa8
#define VID_FRIENDLY        0x1faa
#define VID_COLOR_B         0x1fab
#define VID_DIVINA          0x1fac
#define VID_DIVINAA         0x1fac
#define VID_VICE            0x1fad
#define VID_TELEFONICA      0x1fb0
#define VID_TELE            0x1fb0
#define VID_MICROSLOTH      0x1fb1
#define VID_JAKARTA         0x1fb3
#define VID_BOSCO           0x1fb4
#define VID_BURONEU0        0x1fb7
#define VID_BUERO           0x1fb7
#define VID_BURONEU1        0x1fb8
#define VID_FISHFOOD        0x1fb9
#define VID_GOLDFISH        0x1fb9
#define VID_XENIXOS1        0x1fba
#define VID_BOGUS0          0x1fbb
#define VID_HASSLE          0x1fbb
#define VID_BOGUS1          0x1fbc
#define VID_INDONESIA       0x1fbe
#define VID_ALLIANCE        0x1fbf
#define VID_EASYMAN         0x1fc0
#define VID_EASY            0x1fc0
#define VID_SAVER           0x1fc1
#define VID_SPOOKY          0x1fc2
#define VID_ATOMB           0x1fc4
#define VID_BIRTHDAY        0x1fc5
#define VID_DOGGIE          0x1fc6
#define VID_KILLDLL         0x1fc7
#define VID_TERBARU0        0x1fcc
#define VID_SHOWOFF         0x1fcc
#define VID_TERBARU1        0x1fcd
#define VID_CLOCK           0x1fce
#define VID_FREDDY          0x1fd0
#define VID_NOPB0           0x1fd3
#define VID_NOPB            0x1fd3
#define VID_NOPB1           0x1fd4
#define VID_REFLEX0         0x1fd5
#define VID_REFLEX          0x1fd5
#define VID_REFLEX1         0x1fd6
#define VID_PARASITE3       0x1fd7
#define VID_PARASITE4       0x1fd8
#define VID_TEDIOUS         0x1fd9
#define VID_SATANIC         0x1fdb
#define VID_TAIWAN1         0x1fdd
#define VID_TWNOA           0x1fdd
#define VID_JAKARTAB        0x1fdf
#define VID_DIVINAB         0x1fe1
#define VID_LUNCHA          0x1fe4
#define VID_ALIEN           0x1fe6
#define VID_HELPER          0x1fe8
#define VID_SWITCHES        0x1fe9
#define VID_BIGDADDY        0x1fea
#define VID_GANGSTERZ       0x1fea
#define VID_NUKEDAM         0x1fec
#define VID_STRYX           0x1fee
#define VID_OUTLAW1A        0x1fef
#define VID_OUTLAWA         0x1fef
#define VID_OUTLAW2         0x1ff0
#define VID_OUTLAW3A        0x1ff1
#define VID_OUTLAW1B        0x1ff2
#define VID_OUTLAWB         0x1ff2
#define VID_OUTLAW3B        0x1ff3
#define VID_LOOKC           0x1ff4
#define VID_WEATHERB        0x1ff5
#define VID_THEATRE         0x1ff6
#define VID_TWNOC1          0x1ff7
#define VID_TWNOC           0x1ff7
#define VID_TWNOC2          0x1ff8
#define VID_TWNOB1          0x1ff9
#define VID_TWNOB           0x1ff9
#define VID_TWNOB2          0x1ffa
#define VID_LUNCHB          0x1ffb
#define VID_STRYX2          0x1ffc
#define VID_ANTICON         0x1ffd
#define VID_TWNOD           0x1fff
#define VID_ALIENB          0x1e00
#define VID_FOUR11          0x1e03
#define VID_NICEDAY         0x1e07
#define VID_SUTRA           0x1e08
#define VID_JOHNNY0         0x1e0b
#define VID_NFB             0x1e0c
#define VID_DIVINAC         0x1e0e
#define VID_MDMAC           0x1e11
#define VID_APPDER          0x1e13
#define VID_DARKA           0x1e1d
#define VID_NFA             0x1e21
#define VID_BOOMB           0x1e22
#define VID_NOPD            0x1e23
#define VID_TWNOF           0x1e24
#define VID_EPIDEMIC        0x1e25
#define VID_NIKITA          0x1e26
#define VID_NJWMDLK1D       0x1e27
#define VID_NOPD2           0x1e28
#define VID_SWLABS1         0x1e29
#define VID_ATOMH           0x1e2b
#define VID_CEEFOUR         0x1e2c
#define VID_DZTA            0x1e2f
#define VID_IRISHB          0x1e30
#define VID_KOMPUA          0x1e31
#define VID_NICEDAYB        0x1e32
#define VID_RATSA           0x1e33
#define VID_SHOWOFFB        0x1e34
#define VID_SHOWOFFC        0x1e35
#define VID_SMILEYBDE       0x1e36
#define VID_CLOCKBDE        0x1e3c
#define VID_MINIMALB        0x1e3e
#define VID_RATSB           0x1e3f
#define VID_RATSC           0x1e40
#define VID_TWNOG           0x1e41
#define VID_TWNOH           0x1e42
#define VID_NJ_WMDLK1AGEN   0x1e45
#define VID_PHARDERA        0x1e47
#define VID_NJ_WMDLK1AVIR   0x1e4c
#define VID_NJ_WMDLK1BGEN   0x1e4d
#define VID_COUNTTENA       0x1e4e
#define VID_DANIELA         0x1e4f
#define VID_FURY            0x1e51
#define VID_HELLGATE        0x1e54
#define VID_IMPOSTERB       0x1e55
#define VID_ITALIAN         0x1e56
#define VID_MDMAB           0x1e57
#define VID_NJ_WMDLK1CGEN   0x1e58
#define VID_NJ_WMDLK1DGEN   0x1e59
#define VID_NJ_WMDLK1BVIR   0x1e5a
#define VID_NJ_WMDLK1EGEN   0x1e5b
//#define VID_ATOMB           0x1e64  removed duplicate VID / bad old sample?
#define VID_ATOMD           0x1e65
#define VID_COOLIO2         0x1e66
#define VID_NJ_WMVCK3GEN    0x1e70
#define VID_MISTERX2        0x1e71
#define VID_RUN             0x1e78
#define VID_GOODNIGHT       0x1e7d
#define VID_TWNOGTW         0x1e7e
#define VID_RUNB            0x1e81
#define VID_BADBOYB         0x1e8a
#define VID_CLOCKC          0x1e8b
#define VID_CLOCKD          0x1e8c
#define VID_DANIELC         0x1e8e
#define VID_FRIDAYA         0x1e90
#define VID_IRISHC          0x1e91
#define VID_JOHNNYB         0x1e92
#define VID_MDMAF           0x1e93
#define VID_SHOWOFFD        0x1e95
#define VID_TWNOE           0x1e96
#define VID_TWNOI           0x1e97
#define VID_XENIXOSB        0x1e98
#define VID_BOXA            0x1e9f
#define VID_HYBRIDB         0x1ee4
#define VID_KILLPROTA       0x1ee5
#define VID_SHOWOFFE        0x1ee6
#define VID_NOMVIRA         0x1ee7
#define VID_NOMVIRB         0x1ee8
#define VID_GABLE           0x1ee9
#define VID_KERRANG         0x1eee
#define VID_BOXB            0x1ef1
#define VID_NOPE            0x1efa
#define VID_SHAREFUNA       0x1efb

#define VID_TENTRK          0x1ed0
#define VID_DIGI3547        0x4cA5

#define VID_CHAN            0x1d68
#define VID_CE              0x4f76
#define VID_EEK             0x2C01
#define VID_WYX             0x2c1b

#define VID_LILITH          0x2447
#define VID_TMCB            0x245B
#define VID_TMC             0x1ca2

#define VID_SSR19834        0x258a
#define VID_VANITAS3712     0x2544
#define VID_LIGHTNING       0x2545
#define VID_NUTCRACKER      0x8023
#define VID_NUTCBHD         0x8028
#define VID_NUTC2890        0x8027
#define VID_NUTC7008        0x8029
#define VID_CEZAR           0x25a9
#define VID_ZOMBIEZCME      0x25c4
#define VID_LEXOTRAN        0x25c5
#define VID_DEMON3B5350     0x7587
#define VID_SKCOM           0x2a46

#define VID_WOLLEH          0x6105
#define VID_INVIS2926       0x1bde
#define VID_INVIS3223       0x1bdf
#define VID_WIDOW           0x7b70

#define VID_LOVEFWA         0x7009

// Bloodhound boot vids

#define VID_BLOODHOUND_BOOT 0x4a07
#define VID_BLOODHOUND_PART 0x4a08

// Windows NE viruses

#define VID_REDTEAMKRNL 0x1ad7
#define VID_WINVIR      0x1f64
#define VID_WINVIK      0x1f65
#define VID_WINTINY     0x1f70
#define VID_WINSURF     0x1f71
#define VID_WINLAMER    0x1f72
#define VID_WINTENTACL1 0x1f7b
#define VID_WINTENTACL2 0x1f80
#define VID_WINNEWTENT  0x1f8d
#define VID_WINTVPO     0x1e09
#define VID_WINAOTAUN   0x4ebd
#define VID_WINPWTROJ   0x4d1f
#define VID_VICODIN     0x608d
#define VID_SKIM        0x608e
#define VID_VKIT650     0x60ef
#define VID_VKIT1241    0x60f0
#define VID_WINVIKB     0x60f1
#define VID_WINVIR961   0x60f2
#define VID_REDTEAM     0x6130
#define VID_HOMER       0x6165
#define VID_HOMERPAY    0x6166
#define VID_APPARC      0x61a6
#define VID_APPARD      0x61a7
#define VID_APPARA      0x61a8
#define VID_SKIM2       0x61a9
#define VID_VFW         0x259e
#define VID_VECNA       0x259f

// Windows PE viruses

#define VID_BOZA            0x1f59
#define VID_BOZAD           0x1DB0
#define VID_POPPY           0x6071
#define VID_ANXIETYPOPPYII  0x6092
#define VID_KLUNKY          0x6128
#define VID_ANXIETYPOPPYIII 0x6133
#define VID_SEX98           0x6149
#define VID_LOREZ           0x2515
#define VID_HPS             0x6160
#define VID_SEMISOFT        0x6167
#define VID_MEMORIAL        0x6168
#define VID_YURN            0x6169
#define VID_JACKY           0x616b
#define VID_CIH             0x617e
#define VID_MARBURG         0x2566
#define VID_MARBURGB        0x61a1
#define VID_DARKSIDE        0x256d
#define VID_SGWW            0x256e
#define VID_HARRY           0x256f
#define VID_HARRYB          0x2599
#define VID_ZOMBIE          0x2570
#define VID_CABANAS_E       0x2572
#define VID_ANXIETYE        0x61ad
#define VID_APPAR95B        0x61ae
#define VID_PUNCH           0x257f
#define VID_INCA            0x258b
#define VID_MARKJ           0x258d
#define VID_OLIVIA          0x25a3
#define VID_W95MEMORIAL     0x25a4
#define VID_IKX             0x25ab
#define VID_EVIL            0x25b8
#define VID_LIBERTINE       0x25bb
#define VID_PADANIA         0x25be
#define VID_SHAITAN         0x25c0
#define VID_LUDYEL          0x25c1
#define VID_LUDHILL         0x25c2
#define VID_PARVO           0x25c3
#define VID_K32             0x25d1
#define VID_KENSTON         0x25dc
#define VID_IDYLLWILD       0x7476
#define VID_FONOBOOT        0x25f4
#define VID_SEXY            0x2807
#define VID_MAD             0x2602
#define VID_MURKY390        0x2603
#define VID_UWAGA           0x280a
#define VID_APATHY          0x280b
#define VID_MAYA            0x280c
#define VID_JADIS           0x280d
#define VID_HERETIC         0x2810
#define VID_REDEMPTION      0x2816
#define VID_REGSWAP         0x2817
#define VID_VB              0x2818
#define VID_TWINNY          0x2819
#define VID_YABRAN          0x282a
#define VID_WINZERG         0x282b
#define VID_VOODOO          0x2837
#define VID_POWERFUL        0x2838
#define VID_LEVI            0x283b
#define VID_TENTACLE        0x283c
#define VID_APPARITION      0x283e
#define VID_GIRI            0x283f
#define VID_LUD_JEZ         0x2840
#define VID_HIGHWAY         0x2841
#define VID_OBSOLETE        0x2845
#define VID_NIKO            0x2846
#define VID_SAVIOR          0x2847
#define VID_RUFF            0x284b
#define VID_ENUMIACS        0x284c
#define VID_BABYLONIA_HELP  0x25f8
#define VID_SK_HELP         0x25fd
#define VID_HLPDEMO         0x25fe
#define VID_PADA            0x2d05
#define VID_HLP_DREAM       0x700a
#define VID_HLP_PLUMA       0x2952

// Java viruses

#define VID_JAVATEST    0x1f77
#define VID_JAVATEST2   0x1f78

#endif // #ifndef _NAVEXVID_H_
