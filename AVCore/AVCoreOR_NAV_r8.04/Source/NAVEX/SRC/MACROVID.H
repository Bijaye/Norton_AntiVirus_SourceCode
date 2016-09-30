//************************************************************************
//
// $Header:   S:/NAVEX/VCS/macrovid.h_v   1.106   15 May 1997 16:34:40   ECHIEN  $
//
// Description:
//      Contains NAVEX macro VIDs.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/macrovid.h_v  $
// 
//    Rev 1.106   15 May 1997 16:34:40   ECHIEN
// Added VID_APPDERE 0x4ec0 and VID_NPADBB 0x4ebf
// 
//    Rev 1.105   15 May 1997 16:03:24   AOONWAL
// Added VID_DMVE 0x4ebe
// 
//    Rev 1.104   14 May 1997 22:55:48   RELNITI
// MODIFY 0x4f43: VID_BANDUNGO => VID_BANDUNGM
//        0x4f07: VID_BANDUNGM => VID_BANDUNGAF
// ADD 0x4eBC: VID_BANDUNGO
// 
//    Rev 1.103   14 May 1997 21:11:56   JWILBER
// Uncommented out #define for NiceDay.G, 4ebb.
//
//    Rev 1.102   14 May 1997 19:29:08   RELNITI
// ADD VID_CONCEPTAP 4EBA
//  COMMENTED OUT VID_NICEDAYG 4EBB
//
//    Rev 1.101   14 May 1997 18:56:22   AOONWAL
// Added VID_JOHNNYK  0x4eb9 and changed VID_NICEDAYG to 0x4eba
//
//    Rev 1.100   14 May 1997 18:18:24   JWILBER
// Added #define for VID_NICEDAYG, 4eb9.
//
//    Rev 1.99   14 May 1997 14:37:26   JWILBER
// Added #define for Divina.F, 4eb8.
//
//    Rev 1.98   14 May 1997 14:35:14   ECHIEN
// Added VID_)COLORSAT 0x4eb6 VID_TWOLINESH 0x4eb5
//
//    Rev 1.97   14 May 1997 14:32:32   CFORMUL
// Added Colors.AS
//
//    Rev 1.96   14 May 1997 13:24:12   AOONWAL
// Added define VID_MDMAO 0x4eb4
//
//    Rev 1.95   14 May 1997 12:48:18   ECHIEN
// Added VID_SHOWOFFAT 0x4eb3
//
//    Rev 1.94   14 May 1997 10:13:18   RELNITI
// ADD VID_BANDUNGAE reusing 4ea8
//     VID_SHOWOFFAS 4eb2
//
//    Rev 1.93   13 May 1997 18:52:32   RELNITI
// ADD VID 4ead-4eb1 for WM.Rapi.AF1, AF2, AG, AG1, AG2
//
//    Rev 1.92   13 May 1997 17:33:42   AOONWAL
// Removed syntax error
//
//    Rev 1.91   13 May 1997 17:15:06   AOONWAL
// Added vid define for WM.Npad.BA
//
//    Rev 1.90   13 May 1997 13:01:58   AOONWAL
// Added vid_define for VID_DZTC
//
//    Rev 1.89   13 May 1997 11:43:52   ECHIEN
// Removed VID_NIGHTSHADEA 0x4ea8  Sig already done.
//
//    Rev 1.88   13 May 1997 11:36:54   ECHIEN
// Added VID_FRENZYA 0x4ea7 VID_NIGHTSHADEA 0x4ea8 VID_SPARKLEA 0x4ea9
//
//    Rev 1.87   13 May 1997 10:59:56   RELNITI
// ADD VID_WAZZUCB 0x4eaa
//
//    Rev 1.86   13 May 1997 09:19:30   RELNITI
// ADD VID 4ea1-4ea6
//  for WM.Minimal.O, Alien.D, E, Bandung.AC, Colors.AR, Irish.L
//
//    Rev 1.85   12 May 1997 19:02:26   AOONWAL
// Added vid define for WM.ShowOff.AR
//
//    Rev 1.84   12 May 1997 18:08:32   AOONWAL
// Added VID define for WM.Bandung.AD
//
//    Rev 1.83   12 May 1997 13:29:58   DANG
// Added Concept.AO for CI#11792
//
//    Rev 1.82   12 May 1997 08:23:16   RELNITI
// MODIFY 4e32: Npad.AU => Npad.AV
// ADD VID for WM.Npad.AU, AY, AZ, Concept.AJ, ShareFun.B, Minimal.N,
//        Wazzu, BU, BV, BW, BX, BY, BZ, CA, MDMA.N, Cap.L, Rapi.AE1, AE2,
//        Bandung.AA, AB, ShowOff.AM, AQ
//     4e89-4e9d all based on VMACRO Posting
//
//    Rev 1.81   09 May 1997 18:54:54   RELNITI
// ADD 2 dummy VID to hold Word Macro Virus Trash for FullSet:
//  VID_WM_TRASH1 0x0013 & VID_WM_TRASH2 0x0014
//
//    Rev 1.80   09 May 1997 16:18:56   AOONWAL
// Added VID_APPDERD 0x4e88
//
//    Rev 1.79   08 May 1997 17:00:02   DANG
// Added Concept.AN for CI#11781
//
//    Rev 1.78   08 May 1997 15:46:04   DANG
// Added NOP.K for CI#11781
//
//    Rev 1.77   07 May 1997 22:04:28   JWILBER
// Added VID 4e85 for WM.Concept.AM.
//
//    Rev 1.76   07 May 1997 20:17:00   RELNITI
// ADD 0x4e83 VID_NUCLEARM, 0x4e84 VID_TWOLINESG
//
//    Rev 1.75   07 May 1997 09:52:10   AOONWAL
// Added VID_NICEDAYF 0x4e82
//
//    Rev 1.74   07 May 1997 09:18:06   RELNITI
// ADD 4e81 VID_JOHNNYJ
//
//    Rev 1.73   06 May 1997 16:52:48   CFORMUL
// Added VID_BANDINGZ
//
//    Rev 1.72   06 May 1997 11:44:42   CFORMUL
// Added VID_LAROUXC
//
//    Rev 1.71   06 May 1997 11:01:06   ECHIEN
// Modified VID_NUCLEARL 0x1e53 -> VID_CONCEPTL
// Modified VID_NUCLEARM 0x4e7d -> VID_NUCLEARL
//
//    Rev 1.70   06 May 1997 09:52:18   RELNITI
// ADD VID_NJWMDKL1A 4e7e
//
//    Rev 1.69   05 May 1997 14:44:58   ECHIEN
// Modified VID_NUCLEARL -> VID_NUCLEARM 0x4e7d
//
//    Rev 1.68   05 May 1997 14:38:42   ECHIEN
// Added VID_NUCLEARL 0x4e7d
//
//    Rev 1.67   05 May 1997 13:17:10   AOONWAL
// Added VID_NPADAA 4e7c
//
//    Rev 1.66   05 May 1997 12:45:04   ECHIEN
// Added VID_CONCEPTAL 0x4e7b
//
//    Rev 1.65   05 May 1997 12:24:18   AOONWAL
// Added VID_RAPIAD2 for 0x4e7a and renamed old VID_RAPIAD2 to VID_RAPI.AZ2
//
//    Rev 1.64   04 May 1997 14:11:04   ECHIEN
// Added VID_SPIRALA 0x4e79
//
//    Rev 1.63   04 May 1997 13:44:12   ECHIEN
// Added VID_GOGGLESA 0x4e77 and VID_RANDOMA 0x4e78
//
//    Rev 1.62   03 May 1997 12:21:18   ECHIEN
// Modified VID_DIVINIAE -> VID_DIVINAE
//
//    Rev 1.61   03 May 1997 12:15:36   ECHIEN
// Added VID_PHARDERAB 0x4e75 and VID_DIVINIAE 0x4e76
//
//    Rev 1.60   03 May 1997 10:55:26   ECHIEN
// Correctly added TalonH,H1,H2,C,B
//
//    Rev 1.59   03 May 1997 10:53:30   ECHIEN
// Added VID_TALONH 0x4e70 VID_TALONH1 0x4e71 VID_TALONH2 0x4e72
//       VID_TALONC 0x4e73 VID_TALONB 0x4e74
//
//    Rev 1.58   03 May 1997 09:04:30   ECHIEN
// Added:  Wm.Badboy.A  0x4e6d
//         Wm.Talon.A   0x4e6e
//         Wm.Dub.A     0x4e6f
//
//    Rev 1.57   02 May 1997 13:03:36   AOONWAL
// Added vid_irishk 4e6c
//
//    Rev 1.56   02 May 1997 13:00:52   RELNITI
// ADD 4e5c-4e6b: VID_JOHNNYH, ERASERP, LUNCHC, NOPJ, HIDERA,
//     TESTAREA, IMPOSTERC, CONCEPTAK, DAVEA, ATOMI, NOPI, COUNTTEND,
//     CEEFOURB, GOLDFISHB, CVCK1B, NUCLEARJ
//
//    Rev 1.55   02 May 1997 10:29:00   ECHIEN
// added VID_SHUFFLEA 0x4e56
//
//    Rev 1.54   02 May 1997 10:22:20   RELNITI
// ADD 4e57-5b: VID_COLORSAM, WAZZUBD, HELPERC, D, E
//
//    Rev 1.53   01 May 1997 18:39:44   DANG
// Added Goodnight.B for CI#11707
//
//    Rev 1.52   01 May 1997 17:11:18   ECHIEN
// Added VID_BANDUNGY 0x4e544
//
//    Rev 1.51   01 May 1997 16:34:18   RELNITI
// MODIFY 4e29: VID_SHOWOFFAJ => AN
//        4e35: VID_SHOWOFFAO => AP
// ADD VID_SHOWOFFAJ, AK, AL, AO: 4e50-53
//
//    Rev 1.50   01 May 1997 14:08:18   ECHIEN
// Added VID_NPADAX 0x4e4f
//
//    Rev 1.49   01 May 1997 13:05:56   CFORMUL
// Added COLORSAQ
//
//    Rev 1.48   01 May 1997 10:40:28   RELNITI
// ADD 4e4c-d VID_WAZZUBT, TOTEN
//
//    Rev 1.47   01 May 1997 10:00:54   RELNITI
// RENAME 4e2e: VID_RAPIZ2 => VID_RAPIAB2
//
//    Rev 1.46   01 May 1997 09:09:16   RELNITI
// RENAME 4e45: VID_MDMAN => VID_MDMAM
// ADD 4e46-4e4b: VID_NOPH, RAPIX, RAPIY, RAPIY1, SIMPLEA, SIMPLEB
//
//    Rev 1.45   30 Apr 1997 21:17:30   DANG
// Added MDMA.N for CI#11688
//
//    Rev 1.44   30 Apr 1997 17:17:42   AOONWAL
// Added VID for WM.COLORS.AP
//
//    Rev 1.43   30 Apr 1997 15:09:24   AOONWAL
// Added VID for WM.NiceDay.E
//
//    Rev 1.42   30 Apr 1997 13:56:54   CFORMUL
// Added IDs for CAP.K and Rapi.AD2
//
//    Rev 1.41   30 Apr 1997 12:02:04   RELNITI
// ADD VID_DATEB, HIAC, NPADAP, CAPG, CAPH, CAPI
//
//    Rev 1.40   29 Apr 1997 20:43:36   JWILBER
// Added VID 0x4e3a for Twolines.F.
//
//    Rev 1.39   29 Apr 1997 15:29:10   AOONWAL
// Added VID for WM.RAPIAC?
//
//    Rev 1.38   29 Apr 1997 14:04:04   DANG
// Added Cap.J for CI#11679
//
//    Rev 1.37   29 Apr 1997 13:32:18   DANG
// Added Showoff.AO for CI#11679
//
//    Rev 1.36   28 Apr 1997 13:29:42   DANG
// Added TwoLines.E for CI#11656
//
//    Rev 1.35   28 Apr 1997 12:13:24   CFORMUL
// Added VID_RAPIAA2
//
//    Rev 1.34   25 Apr 1997 17:42:14   DANG
// Added NPad.AU for CI#11646
//
//    Rev 1.33   25 Apr 1997 17:12:54   AOONWAL
// Modified during MAY97 update
//
//    Rev MAY97  24 Apr 1997 17:18:44   ECHIEN
// Changed VID_RAPIZ -> RAPIZ2 0x4e2e
//
//    Rev MAY97  24 Apr 1997 16:59:08   ECHIEN
// Added VID_RAPIZ 0x4e2e
//
//    Rev MAY97  23 Apr 1997 17:38:22   CFORMUL
// Added VID_IRISHJ
//
//    Rev MAY97  23 Apr 1997 15:09:40   ECHIEN
// Added VID_SHOWOFFAJ 0x4e29
//
//    Rev MAY97  23 Apr 1997 13:34:14   ECHIEN
// Added VID_NJWMDLK1G 0x4e28
//
//    Rev MAY97  23 Apr 1997 09:50:50   RELNITI
// RENAME 4e19: VID_NPADAO => AQ
//        4e1a: VID_NPADAP => AR
// ADD VID_NPADAO reusing 4e22
//     VID_WAZZUBS 4e27
//
//    Rev MAY97  22 Apr 1997 22:53:08   JWILBER
// Marked VID_CONCEPTAJ as re-usable.
//
//    Rev MAY97  22 Apr 1997 21:28:34   RELNITI
// ADD VID_COLORSAO, VID_SHOWOFFAI
//
//    Rev MAY97  22 Apr 1997 20:55:50   JWILBER
// Changed VID_COLORSAM to VID_COLORSAN.
//
//    Rev MAY97  22 Apr 1997 20:54:20   JWILBER
// Fixed typos on VID_COLORSAM and VID_CONCEPTAJ.
//
//    Rev MAY97  22 Apr 1997 18:09:08   RELNITI
// ADD VID_NUCLEARK, VID_TWOLINESD
//
//    Rev MAY97  22 Apr 1997 17:14:52   JWILBER
// Added VIDs 4e21 for WM.Colors.AM, and 4e22 for WM.Concept.AJ.
//
//    Rev MAY97  22 Apr 1997 15:57:08   ECHIEN
// Added VID_NPADAT 0x4e20
//
//    Rev MAY97  22 Apr 1997 14:11:48   ECHIEN
// Added VID_JOHNNYI 0x4e1f
//
//    Rev MAY97  21 Apr 1997 16:50:52   AOONWAL
// Added VID 4e1e
//
//    Rev MAY97  21 Apr 1997 13:03:36   DANG
// Added Colors.AL for CI#11549
//
//    Rev MAY97  21 Apr 1997 12:54:50   ECHIEN
// Added Showoff.AH 0x4e1c
//
//    Rev MAY97  21 Apr 1997 12:52:48   RELNITI
// ADD 4E1B: VID_BANDUNGX
//
//    Rev 1.32   18 Apr 1997 18:00:14   AOONWAL
// Modified during DEV1 malfunction
//
//    Rev ABID   16 Apr 1997 18:34:10   RELNITI
// ADD VID_NPADAO, VID_NPADAP
//
//    Rev ABID   16 Apr 1997 15:12:46   DANG
// Added WM.Cap.F for CI#11536.  It's a Japanese document with this sample.
//
//    Rev ABID   16 Apr 1997 13:39:22   CFORMUL
// Added VID_SHOWOFFAG
//
//    Rev ABID   16 Apr 1997 11:35:10   RELNITI
// DUPLICATE VID: VID_MDMAI => VID_MDMAII
//
//    Rev ABID   16 Apr 1997 11:28:42   RELNITI
// RENAME 4ffc: VID_SHOWOFFAC=>SHOWOFFAE
//        4ffd: VID_WAZZUBQ=>WAZZUBR
//        4fff: VID_COLORSAJ=>COLORSAK
// ADD VID_SHOWOFFAC, CVCK1, DRUGSA, BANDUNGT, COLORSAJ, CONCEPTAI,
//         IRISHH, MDMAI, SATANICB, WAZZUBQ, OVALA, CVCK1A, CAPE,
//         BANDUNGV, BANDUNGW, ERASERO, MDMAJ, MDMAK, NO_FB, SHOWOFFAF
//
//    Rev ABID   15 Apr 1997 17:26:48   CFORMUL
// Changed Bandung.T to Bandung.U
//
//    Rev ABID   15 Apr 1997 15:54:32   CFORMUL
// Added VID_BANDUNGT
//
//    Rev ABID   14 Apr 1997 12:52:52   CFORMUL
// Added VID_IRISHI
//
//    Rev ABID   14 Apr 1997 09:14:04   RELNITI
// ADD 4e00: VID_FULL_SET
//
//    Rev 1.31   11 Apr 1997 18:15:32   ECHIEN
// Added VID_COLORSAJ 0x4fff
//
//    Rev 1.30   11 Apr 1997 16:51:46   ECHIEN
// Added VID_SHOWOFFAD 0x4ffe
//
//    Rev 1.29   11 Apr 1997 14:52:50   ECHIEN
// Added VID_WAZZUBQ 0x4ffd
//
//    Rev 1.28   11 Apr 1997 13:53:22   ECHIEN
// Added VID_SHOWOFFAC 0x4ffc
//
//    Rev 1.27   10 Apr 1997 19:40:16   RELNITI
// CHANGE VID_JOHNNYG: 0x4ff8 => 0x4ffb
// ADD VID_ATOMG, VID_WAZZUBC, VID_BADBOYC (0x4ff8 - 0x4ffa)
//
//    Rev 1.26   10 Apr 1997 19:35:16   JWILBER
// Added VID for Johnny.G, 4ff8.
//
//    Rev 1.25   10 Apr 1997 18:14:20   RELNITI
// ADD VID_TALONE 0x4ff7
//
//    Rev 1.24   10 Apr 1997 16:02:04   RELNITI
// ADD VID_FUZZYA, VID_CONCEPTAG, VID_MDMAH, VID_LEMONB, VID_NPADAN
//     VID_TALONF, VID_TALONG, VID_MINIMALM, VID_DARKD
//
//    Rev 1.23   09 Apr 1997 19:34:44   JWILBER
// Added VID_CAPD, 4fed.
//
//    Rev 1.22   09 Apr 1997 17:15:24   ECHIEN
// Modified VID_RAPIW -> VID_RAPIW2
//
//    Rev 1.21   09 Apr 1997 16:51:22   ECHIEN
// Added VID_RAPIW 0x4fec
//
//    Rev 1.20   09 Apr 1997 15:59:06   AOONWAL
// Added define for WM.Showoff.AB
//
//    Rev 1.19   09 Apr 1997 15:07:24   ECHIEN
// Added Showoff.AA 0x4fea
//
//    Rev 1.18   09 Apr 1997 11:00:42   ECHIEN
// Added VID_BANDUNGS 0x4fe9
//
//    Rev 1.17   09 Apr 1997 09:50:48   RELNITI
// ADD VID_MessengerA, VID_DarkB, VID_HunterA, VID_HunterB
//  VID_MercyA, VID_SamA, VID_FridayB, VID_BertikA
//  VID_HilightA, VID_Mota, VID_NJ-WMDLK1FVIR, VID_SwlabsA, VID_TerrorA
//  VID_ShowOffZ, VID_ColorsAI, VID_EraserM, VID_EraserN, VID_NopG
//  VID_WallpaperA, VID_TalonD, VID_DarkC, VID_NuclearI
//  VID_MinimalK, VID_MinimalL,VID_CapC, VID_AndryA
//  (0x4fcf-4fd8)
//
//    Rev 1.16   08 Apr 1997 11:04:54   RELNITI
// ADD VID_COLORSAH, VID_CONCEPTAH, VID_BANDUNGR
//
//    Rev 1.15   07 Apr 1997 20:18:48   RELNITI
// RENAME 0x4fc9: VID_MDMAG => VID_MDMAI
// ADD 0x4fca: VID_MDMAD
//     0x4fcb: VID_MDMAG
//
//    Rev 1.14   07 Apr 1997 17:30:38   AOONWAL
// Added define for MDMAG
//
//    Rev 1.13   07 Apr 1997 11:27:02   RELNITI
// ADD VID_SHOWOFFY, VID_NPADAM
//
//    Rev 1.12   04 Apr 1997 18:13:44   RELNITI
// ADD VID_SETMD, LAZYA, HAGGISA, LEMONA, PHARDERAE, NOPF
//  (0x4fc1-c6)
//
//    Rev 1.11   04 Apr 1997 17:16:12   RELNITI
// ADD VID_DMVD, VID_DOGGIEC, VID_DOGGIED (4fbe-4fc0)
//
//    Rev 1.10   04 Apr 1997 16:51:44   ECHIEN
// Added VID_ARMADILLOA 0x4fbc and VID_SWORDA 0x4fbd
//
//    Rev 1.9   04 Apr 1997 16:40:12   RELNITI
// ADD VID_MINIMALE, F, G, H, I, J (0x4fb6-bb)
//
//    Rev 1.8   04 Apr 1997 16:22:32   ECHIEN
// Added VID_TEMPLEA 0x4fb5
//
//    Rev 1.7   04 Apr 1997 12:35:10   RELNITI
// ADD VID_COLORSAG 0x4fb4
//
//    Rev 1.6   04 Apr 1997 11:07:16   RELNITI
// ADD VID_NPADAL, VID_ERASERF,G,H,I,K,L
//
//    Rev 1.5   04 Apr 1997 09:50:10   RELNITI
// ADD VID_CLOCKG, VID_CLOCKH
//
//    Rev 1.4   04 Apr 1997 08:54:46   RELNITI
// ADD VID_SHOWOFFK,M,N,O,P,Q,R (0x4fa3-9)
//     VID_COLORSAF 0x4faa
//
//    Rev 1.3   03 Apr 1997 20:40:10   RELNITI
// ADD VID_WAZZUBP 0x4fa1
//     VID_SHOWOFFV 0x4fa2
// RENAME 0x4f97: VID_SHOWOFFV => W
//        0x4f99: VID_SHOWOFFW => X
//
//    Rev 1.2   03 Apr 1997 17:44:22   ECHIEN
// Added VID_MTFA 0x4fa0
//
//    Rev 1.1   03 Apr 1997 12:18:32   CFORMUL
// Added VID_NICEDAYD
//
//    Rev 1.0   03 Apr 1997 10:45:02   DCHI
// Initial revision.
//
//************************************************************************

#ifndef _MACROVID_H_

#define _MACROVID_H_

// Function ID's
// Do not use 0x00 as either hi or lo order byte.
// This is to allow backwards compatibiltiy in the virscan.dat with
// ver <= 3.0.7

#define ID_CORDOBES     0x4443          // Detects Cordobes (and repairs)
#define ID_SMEG         0x4d53          // Detects Smeg
#define ID_NATAS        0x414E          // Detects Natas
//#define ID_TPEBOSNIA    0x4254          // Detects TPE.Bosnia
#define ID_MACRO        0x6477          // Detects Word Macro Family
#define ID_NIGHTFALL    0x716e          // Detects Nightfall and Neuroquila
#define ID_NIGHTFALL2   0x6e6e          // Second Nightfall/Neuroquila call
#define ID_NIGHTFALL3   0x6e6f          // Nightfall.5815 enhanced checking.
#define ID_ONEHALF      0x4F48          // One Half
#define ID_URUG4        0x4755          // Uruguay 4
#define ID_VAGABOND     0x4756          // Vagabond
#define ID_GRIPE        0x5247          // Gripe.2040
#define ID_RDA          0x4452          // RDA viruses
#define ID_CRUNCHER     0x5243          // Cruncher viruses "CR"
#define ID_HARE			0x4b48			// Hare.7610
#define ID_VICE			0x4356			// VICE
#define ID_FREDDY       0x4B46          // Freddy_Krueger
#define ID_TENTRK       0x5454          // Tentrickle det (& rep later)

#define ID_WINDOWS      0x5a42          // WI - windows

#define ID_JAVA         0x564A          // JV - Java

#if defined(NAVEX_QA)                   // QA ScanFile Test ID
#define ID_QATESTDEF    0x4141
#endif                                  // NAVEX_QA

// Repair ID's
// Exec_Code control flag plus these values in the bookmark field
// determine which repair function to invoke.

#define ID_BATMAN2      0x4232          // Batman II
#define ID_BURGLAR      0x7562          // Burglar.1150
#define ID_CPW          0x4350          // CPW
#define ID_DIAMOND      0x5244          // DiamondRockSteady
#define ID_DH           0x4844          // Die Hard
#define ID_EMMIE        0x4d45          // Emmie repair, 3 strains.
#define ID_FAIRZ        0x7a66          // Fairz
#define ID_FOUR11       0x040b          // FourEleven.2144
#define ID_GENRAT       0x4547          // Genrat.785
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
#define ID_TANPRO       0x5054          // Tanpro.524
#define	ID_VACSINA1		0x06aa			// Vacsina.TP-04
#define	ID_VACSINA2		0x06ab			// Vacsina.TP-04 (x)
#define ID_VMEM         0x4d56          // Vmem and Vmem (x) repair
#define ID_UNKNOWN      0x4b55          // Unknown.1329
#define ID_YESMILE      0x4159          // Yesmile.5504
#define ID_WEED         0x3263          // Weed.3263
#define ID_WEREWOLF     0x5757          // Werewolf
#define ID_YKEY         0x1b99          // Y-Key.2346
#define ID_ZARMA        0x617a          // Zarma
#define ID_WEED11       0x4080          // Weed v1.1 (Weed.4080)

#define ID_BOZA         0x7762          // WB for Windows Boza
#define ID_TENT         0x7774          // WT for Windows Tentacle
#define ID_TENT2        0x7432          // T2 for Windows Tentacle II
#define ID_TPVO         0x5470          // Tp for Windows Tpvo

#define ID_CRUEL        0x5243          // Boot Repair for Cruel
#define ID_DABOYS       0x4244          // Boot Repair for Da'Boys
#define ID_ETHER        0x5445          // Boot Repair for Ether
#define ID_ROMA         0x4d52          // Boot Repair for Roma.PG
#define ID_EDWIN        0x4445          // Boot Repair for Edwin
#define ID_KARNIVALI    0x414B          // Boot Repair for Karnivali

#define ID_BH           0x414a          // MBR Repair for Bad_Head
#define ID_CB           0x4243          // MBR Repair for Crazy Boot
#define ID_DANEBOOT     0x4e44          // MBR Repair for Danish Boot
#define ID_FRANK        0x3052          // MBR Repair for Frankenstein
#define ID_ETP          0x7065          // MBR Repair for Esto te pasa
#define ID_NEURO        0x726e          // MBR Repair for Neuro
#define ID_PREDT        0x5250          // MBR Repair for Predator
#define ID_URKEL        0x5255          // MBR Repair for Urkel


// VID's returned by scan functions

//#define VID_TPEBOSNIA       0x1f01
#define VID_FORMATC         0x1f02
#define VID_COLORS          0x1f03
#define VID_COLORSA         0x1f03
#define VID_CONCEPT0        0x1f04
#define VID_CONCEPTA        0x1f04
#define VID_CONCEPT1        0x1f5e
#define VID_DMV             0x1f05
#define VID_DMVA            0x1f05
#define VID_NUCLEAR         0x1f06
#define VID_NUCLEARA        0x1f06
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
#define VID_CONCEPTF        0x1f79
#define VID_CONCEPTBB       0x1f79
#define VID_CORDOBES        0x1f7a
#define VID_WAZZU           0x1f7e
#define VID_WAZZUA          0x1f7e
#define VID_WAZZU2          0x1fb2
#define VID_WAZZUE          0x1fb2
#define VID_WAZZU3          0x1fc3
#define VID_WAZZUL          0x1fc3
#define VID_WAZZU4          0x1fe0
#define VID_WAZZUJ          0x1fe0
#define VID_WAZZU5          0x1fe2
#define VID_WAZZUO          0x1fe2
#define VID_WAZZU6          0x1fe3
#define VID_WAZZUU          0x1fe3
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
#define VID_HARE_BOOT	    0x1f99
#define VID_MOLOCH          0x1f9a
#define VID_PARASITE1       0x1f9b
#define VID_CONCEPTFF       0x1f9b
#define VID_NOP1            0x1f9c
#define VID_NOPA            0x1f9c
#define VID_MDMADMV         0X1f9d
#define VID_MDMAA           0X1f9d  //unused?
#define VID_PARASITE0       0x1f9e
#define VID_HARE            0x1f9f
#define	VID_HARE7610		0x2fe7
#define	VID_HARE7750		0x2ed9
#define	VID_HARE7786		0x2fe6
#define VID_NOP2            0x1fa1
#define VID_ATOM            0x1fa4
#define VID_ATOMA           0x1fa4
#define VID_MADDOG          0x1fa5
#define VID_MADDOGA         0x1fa5
#define VID_NUCLEARB        0x1fa6
#define VID_POLITE			0x1fa7
#define VID_PHANTOM         0x1fa8
#define VID_FRIENDLY        0x1faa
#define VID_COLOR_B         0x1fab
#define VID_COLORSB         0x1fab
#define VID_DIVINA          0x1fac
#define VID_DIVINAA         0x1fac
#define VID_VICE            0x1fad
#define VID_TELEFONICA      0x1fb0
#define VID_TELE            0x1fb0
#define VID_MICROSLOTH      0x1fb1
#define VID_WAZZUH          0x1fb1
#define VID_JAKARTA         0x1fb3
#define VID_NPADA           0x1fb3
#define VID_BOSCO           0x1fb4
#define VID_WAZZUF          0x1fb4
#define VID_CONCEPTH0       0x1fb5
#define VID_CONCEPTH        0x1fb5
#define VID_CONCEPTH1		0x1fb6
#define VID_BURONEU0        0x1fb7
#define VID_BUERO           0x1fb7
#define VID_BURONEU1		0x1fb8
#define VID_FISHFOOD        0x1fb9
#define VID_GOLDFISH        0x1fb9
#define VID_XENIXOS1		0x1fba
#define VID_BOGUS0          0x1fbb
#define VID_HASSLE          0x1fbb
#define VID_BOGUS1          0x1fbc
#define VID_INDONESIA       0x1fbe
#define VID_BANDUNGD        0x1fbe
#define VID_ALLIANCE        0x1fbf
#define VID_EASYMAN         0x1fc0
#define VID_EASY            0x1fc0
#define VID_SAVER           0x1fc1
#define VID_SPOOKY          0x1fc2
#define VID_ATOMB           0x1fc4
#define VID_BIRTHDAY        0x1fc5
#define VID_DOGGIE          0x1fc6
#define VID_KILLDLL         0x1fc7
#define VID_CONCEPTC        0x1fc8
#define VID_CONCEPTD        0x1fc9
#define VID_CONCEPTE        0x1fca
#define VID_CONCEPTHCR      0x1fcb
#define VID_CONCEPTN        0x1fcb
#define VID_TERBARU0        0x1fcc
#define VID_SHOWOFF         0x1fcc
#define VID_TERBARU1        0x1fcd
#define VID_CLOCK           0x1fce
#define VID_WAZZUB          0x1fcf
#define VID_FREDDY          0x1fd0
#define VID_BANDUNG         0x1fd2
#define VID_BANDUNGB        0x1fd2
#define VID_NOPB0           0x1fd3
#define VID_NOPB            0x1fd3
#define VID_NOPB1           0x1fd4
#define VID_REFLEX0         0x1fd5
#define VID_REFLEX          0x1fd5
#define VID_REFLEX1         0x1fd6
#define VID_PARASITE3       0x1fd7
#define VID_PARASITE4       0x1fd8
#define VID_TEDIOUS         0x1fd9
#define VID_CONCEPTB        0x1fda
#define VID_CONCEPTKK       0x1fda
#define VID_SATANIC         0x1fdb
#define VID_CONCEPTI        0x1fdC
#define VID_TAIWAN1         0x1fdd
#define VID_TWNOA           0x1fdd
#define VID_CONCEPTJ        0x1fde
#define VID_CONCEPTJJ       0x1fd7
#define VID_JAKARTAB        0x1fdf
#define VID_DIVINAB         0x1fe1
#define VID_CONCEPTK        0x1fe4
#define VID_LUNCHA          0x1fe4
#define VID_COLORSC         0x1fe5
#define VID_ALIEN           0x1fe6
#define VID_RAPI            0x1fe7
#define VID_HELPER          0x1fe8
#define VID_SWITCHES        0x1fe9
#define VID_BIGDADDY        0x1fea
#define VID_GANGSTERZ       0x1fea
#define VID_NUKEDAM         0x1fec
// #define VID_NUCLEARC        0x1fec moved to 1e63
#define VID_NPAD            0x1fed
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
#define VID_NPADC           0x1ffe
#define VID_TWNOD           0x1fff
#define VID_ALIENB          0x1e00
#define VID_FOUR11          0x1e03
#define VID_CONCEPTT        0x1e07
#define VID_NICEDAY         0x1e07
#define VID_SUTRA           0x1e08
#define VID_CONCEPTS        0x1e08
#define VID_JOHNNY0         0x1e0b
#define VID_NFB             0x1e0c
#define VID_NPADE           0x1e0d
#define VID_DIVINAC         0x1e0e
#define VID_NUCLEARE        0x1e10
#define VID_MDMAC           0x1e11
#define VID_WAZZUY          0x1e12
#define VID_APPDER          0x1e13
#define VID_NPADH           0x1e14
#define VID_WAZZUX          0x1e16
#define VID_NPADJ           0x1e17
#define VID_NPADI           0x1e18
#define VID_NPADO           0x1e19
#define VID_NPADQ           0x1e1b
#define VID_WAZZUAH         0x1e1c
#define VID_DARKA           0x1e1d
#define VID_WAZZUAI         0x1e1e
#define VID_CONCEPTV        0x1e1f
#define VID_BANDUNGE        0x1e20
#define VID_NFA             0x1e21
#define VID_BOOMB           0x1e22
#define VID_NOPD            0x1e23
#define VID_TWNOF           0x1e24
#define VID_EPIDEMIC        0x1e25
#define VID_NIKITA          0x1e26
#define VID_NJWMDLK1D       0x1e27
#define VID_NOPD2           0x1e28
#define VID_SWLABS1         0x1e29
#define VID_RAPIG           0x1e2a
#define VID_ATOMH           0x1e2b
#define VID_CEEFOUR         0x1e2c
#define VID_CONCEPTW        0x1e2d
#define VID_CONCEPTX        0x1e2e
#define VID_DZTA            0x1e2f
#define VID_IRISHB          0x1e30
#define VID_KOMPUA          0x1e31
#define VID_NICEDAYB        0x1e32
#define VID_RATSA           0x1e33
#define VID_SHOWOFFB        0x1e34
#define VID_SHOWOFFC        0x1e35
#define VID_SMILEYBDE       0x1e36
#define VID_WAZZUAJ         0x1e37
#define VID_WAZZUAK         0x1e38
#define VID_WAZZUAL         0x1e39
#define VID_WAZZUAM         0x1e3a
#define VID_WAZZUAN         0x1e3b
#define VID_CLOCKBDE        0x1e3c
#define VID_NPADK           0x1e3d
#define VID_MINIMALB        0x1e3e
#define VID_RATSB           0x1e3f
#define VID_RATSC           0x1e40
#define VID_TWNOG           0x1e41
#define VID_TWNOH           0x1e42
#define VID_BANDUNGG        0x1e43
#define VID_CONCEPTY        0x1e44
#define VID_NJ_WMDLK1AGEN   0x1e45
#define VID_CONCEPTG        0x1e46
#define VID_PHARDERA        0x1e47
#define VID_CONCEPTP        0x1e48
#define VID_CONCEPTM        0x1e49
#define VID_BANDUNGC        0x1e4a
#define VID_CONCEPTMDRP     0x1e4b
#define VID_NJ_WMDLK1AVIR   0x1e4c
#define VID_NJ_WMDLK1BGEN   0x1e4d
#define VID_COUNTTENA       0x1e4e
#define VID_DANIELA         0x1e4f
#define VID_DMVB            0x1e50
#define VID_FURY            0x1e51
#define VID_NUCLEARD        0x1e52
#define VID_CONCEPTL        0x1e53
#define VID_HELLGATE        0x1e54
#define VID_IMPOSTERB       0x1e55
#define VID_ITALIAN         0x1e56
#define VID_MDMAB           0x1e57
#define VID_NJ_WMDLK1CGEN   0x1e58
#define VID_NJ_WMDLK1DGEN   0x1e59
#define VID_NJ_WMDLK1BVIR   0x1e5a
#define VID_NJ_WMDLK1EGEN   0x1e5b
#define VID_CONCEPTQ        0x1e5c
#define VID_CONCEPTF1       0x1e5d
#define VID_CONCEPTF5       0x1e5e
#define VID_RAPIB           0x1e5f
#define VID_RAPIB1          0x1e60
#define VID_RAPIB2          0x1e61
#define VID_NPADB           0x1e62
#define VID_NUCLEARC        0x1e63
//#define VID_ATOMB           0x1e64  removed duplicate VID / bad old sample?
#define VID_ATOMD           0x1e65
#define VID_COOLIO2         0x1e66
#define VID_WAZZUAC         0x1e67
#define VID_WAZZUD          0x1e68
#define VID_WAZZUG          0x1e69
#define VID_WAZZUK          0x1e6a
#define VID_WAZZUQ          0x1e6b    //this one is the real Wazzu.Q
#define VID_WAZZUR          0x1e6c
#define VID_WAZZUS          0x1e6d
#define VID_WAZZUT          0x1e6e
#define VID_WAZZUW          0x1e6f
#define VID_NJ_WMVCK3GEN    0x1e70
#define VID_MISTERX2        0x1e71
#define VID_COLORSD         0x1e72
#define VID_COLORSE         0x1e73
#define VID_COLORSF         0x1e74
#define VID_COLORSG         0x1e75
#define VID_CONCEPTZ        0x1e76
#define VID_WAZZUC          0x1e77
#define VID_RUN             0x1e78
#define VID_RAPIA           0x1e79
#define VID_RAPIA1          0x1e7a
#define VID_RAPIC           0x1e7b
#define VID_RAPIC1          0x1e7c
#define VID_GOODNIGHT       0x1e7d
#define VID_TWNOGTW         0x1e7e
#define VID_NPADP           0x1e7f
#define VID_NPADMM          0x1e80
#define VID_RUNB            0x1e81
#define VID_RAPIK           0x1e82
#define VID_RAPIJ           0x1e83
#define VID_WAZZUAG         0x1e84
#define VID_RAPIM2          0x1e85  //renamed 022897 RE
#define VID_WAZZUAO         0x1e86
#define VID_BANDUNGH        0x1e87
#define VID_BANDUNGI        0x1e88
#define VID_BANDUNGJ        0x1e89
#define VID_BADBOYB         0x1e8a
#define VID_CLOCKC          0x1e8b
#define VID_CLOCKD          0x1e8c
#define VID_COLORSI2        0x1e8d
#define VID_DANIELC         0x1e8e
#define VID_DMVC            0x1e8f
#define VID_FRIDAYA         0x1e90
#define VID_IRISHC          0x1e91
#define VID_JOHNNYB         0x1e92
#define VID_MDMAF           0x1e93
#define VID_NUCLEARF        0x1e94
#define VID_SHOWOFFD        0x1e95
#define VID_TWNOE           0x1e96
#define VID_TWNOI           0x1e97
#define VID_XENIXOSB        0x1e98
#define VID_WAZZUP          0x1e99
#define VID_WAZZUBG         0x1e9a
#define VID_WAZZUAP         0x1e9b
#define VID_WAZZUM          0x1e9c
#define VID_WAZZUBH         0x1e9d
#define VID_COLORSJ         0x1e9e
#define VID_TWNOK           0x1e9f
#define VID_BANDUNGK        0x1edf
#define VID_RAPIJ1          0x1ee0
#define VID_NPADT           0x1ee1
#define VID_COLORSK         0x1ee2
#define VID_NPADV           0x1ee3
#define VID_HYBRIDB         0x1ee4
#define VID_KILLPROTA       0x1ee5
#define VID_SHOWOFFE        0x1ee6
#define VID_NOMVIRA         0x1ee7
#define VID_NOMVIRB         0x1ee8
#define VID_GABLE           0x1ee9
#define VID_WAZZUAT         0x1eea
#define VID_RAPII           0x1eeb
#define VID_RAPII1          0x1eec
#define VID_RAPII2          0x1eed
#define VID_KERRANG         0x1eee
#define VID_NPADR           0x1eef
#define VID_NPADS           0x1ef0
#define VID_BOXB            0x1ef1
#define VID_NPADW           0x1ef2
#define VID_BANDUNGL        0x1ef3
#define VID_COLORSM         0x1ef4
#define VID_WAZZUAR	        0x1ef5
#define VID_WAZZUAQ	        0x1ef6
#define VID_WAZZUAS	        0x1ef7
#define VID_NOPE            0x1efa
#define VID_SHAREFUNA       0x1efb


#define VID_TENTRK          0x1ed0
#define VID_RAPIH2          0x1EDC


// Automated macro defs
#define VID_WAZZUAU         0x1ea0
#define VID_ABC             0x1ea1
#define VID_WEATHERC        0x1ea2
#define VID_WAZZUAB         0x1ea3
#define VID_RAPIE2          0x1ea4
#define VID_NJWMVCKB        0x1ea5
#define VID_COOLIO          0x1ea6
#define VID_MISTERX         0x1ea7
#define VID_HYBRID          0x1ea8
#define VID_COLORSJ2        0x1ea9
#define VID_COUNTTENB       0x1eaa
#define VID_NPADF           0x1eab
#define VID_NPADG           0x1eac
#define VID_CAPA            0x1ead
#define VID_NPADL           0x1eae
#define VID_BANDUNGA        0x1eaf
#define VID_MSWA            0x1eb0
#define VID_TWISTER         0x1eb1
#define VID_WAZZUAA         0x1eb2
#define VID_MADDOGB         0x1eb3
#define VID_ATOMF           0x1eb4
#define VID_OLYMPICA        0x1eb5
#define VID_OLYMPICB        0x1eb6
#define VID_RAPID           0x1eb7
#define VID_RAPID1          0x1eb8
#define VID_RAPID2          0x1eb9
#define VID_ATOMC           0x1eba
#define VID_WAZZUI          0x1ebb
#define VID_MVDKA           0x1ebc
#define VID_MVDKB           0x1ebd
#define VID_WAZZUZ          0x1ebe
#define VID_DANIELB         0x1ebf
#define VID_ATOME           0x1ec0
#define VID_GOODBYEDR       0x1ec1
#define VID_GOODBYE         0x1ec2
#define VID_MAGNUM          0x1ec3
#define VID_TARGETB         0x1ec4
#define VID_NIKI            0x1ec5
#define VID_MDMAE           0x1ec6
#define VID_BANDUNGF        0x1ec7
#define VID_MINIMAL         0x1ec8
#define VID_TARGET          0x1ec9
#define VID_RAPIF           0x1eca
#define VID_RAPIF1          0x1ecb
#define VID_RAPIF2          0x1ecc
#define VID_CHAOS           0x1ecd
#define VID_WAZZUAD         0x1ece
#define VID_WAZZUAE         0x1ecf
#define VID_CAPB            0x1efc
#define VID_NPADX           0x1efd
#define VID_COLORSN         0x1efe
#define VID_WAZZUN          0x1eff

#define VID_RAPIL           0x4f00
#define VID_RAPIL1          0x4f01
#define VID_RAPIL2          0x4f02
#define VID_DZTB            0x4f03
#define VID_CONDAM          0x4f04
#define VID_CONREM          0x4f05
#define VID_WAZZUAX	        0x4f06
#define VID_BANDUNGAF       0x4f07
#define VID_WAZZUBB         0x4f08
#define VID_RAPIN           0x4f09
#define VID_ANTICONCEPTA1   0x4f0a
#define VID_CLOCKE	        0x4f0b
#define VID_CONCEPTAA	    0x4f0c
#define VID_DOGGIEB	        0x4f0d
#define VID_MUCKA	        0x4f0e
#define VID_MUCKB	        0x4f0f
#define VID_MUCKC           0x4f10
#define VID_BANDUNGN        0x4f11
#define VID_RAPIN1          0x4f12
#define VID_RAPIN2          0x4f13
#define VID_WAZZUAY	        0x4f14
#define VID_WAZZUAZ	        0x4f15
#define VID_WAZZUBA         0x4f16
#define VID_NPADU	        0x4f17
#define VID_NUCLEARG        0x4f18
#define VID_NUCLEARH        0x4f19
#define VID_DIVINAD	        0x4f1a
#define VID_ALIENC	        0x4f1b
#define VID_NICEDAYC        0x4f1c
#define VID_NO_FA           0x4f1d
#define VID_SHOWOFFF        0x4f1e
#define VID_CLOCKF          0x4f1f
#define VID_WEATHERA        0x4f20
#define VID_CAP_DAMAGED     0x4f21
#define VID_SNICKERSA       0x4f22
#define VID_SWLABS3         0x4f23
#define VID_VH1             0x4f24
#define VID_VH1A            0x4f25
#define VID_PHARDERAC       0x4f26
#define VID_THEATREB        0x4f27
#define VID_CONCEPTR        0x4f28
#define VID_LOOKA           0x4f29
#define VID_RAPIO           0x4F2E
#define VID_RAPIO1          0x4F2A
#define VID_RAPIO2          0x4F2B
#define VID_COLORSO         0x4f2d
#define VID_RAPIP           0x4F30
#define VID_NPADZ           0x4f31
#define VID_TWOLINESC       0x4f32
#define VID_WEATHERD	    0x4f33
#define VID_THEATREC        0x4f34
#define VID_WHY             0x4f35
#define VID_LOOKD           0x4f36
#define VID_IRISHD          0x4F37
#define VID_ERASERA         0x4f38
#define VID_MINIMALD        0x4f39
#define VID_BABYA           0x4f3a
#define VID_APPDERB         0x4f3b
#define VID_MINDA           0x4f3c
#define VID_SHOWOFFG        0x4f3d
#define VID_NPADY           0x4f3e
#define VID_APPDERC         0x4f3f
#define VID_SHOWOFFJ        0x4F40
#define VID_COLORSV         0x4f41
#define VID_COLORST         0x4f42
#define VID_BANDUNGM        0x4f43
#define VID_COLORSP         0x4f44
#define VID_COLORSQ         0x4f45
#define VID_COLORSR         0x4f46
#define VID_JOHNNYC         0x4f47
#define VID_SHOWOFFH        0x4f48
#define VID_SHOWOFFI        0x4f49
#define VID_WAZZUBE         0x4f4a
#define VID_WAZZUAF         0x4f4b
#define VID_PHARDERAD       0x4f4c
#define VID_FIREA           0x4f4d
#define VID_ERASERB         0x4f4e
#define VID_ERASERC         0x4f4f
#define VID_ERASERD         0x4f50
#define VID_COLORSU         0x4f51
#define VID_COLORSS         0x4f52
#define VID_COLORSBREMN     0x4f53
#define VID_NIGHTSHADEA     0x4F54
#define VID_MINIMALC        0x4F55
#define VID_IRISHF          0x4f56
#define VID_SMILEYA         0x4f57
#define VID_SHOWOFFL        0x4f58
#define VID_NPADN           0x4f59
#define VID_NPADAB          0x4f5a
#define VID_RAPIQ1          0x4f5b
#define VID_RAPIQ2          0x4f5c
#define VID_COLORSH         0x4f5d
#define VID_COLORSW         0x4f5e
#define VID_WAZZUAV         0x4f5f
#define VID_WAZZUAW         0x4f60
#define VID_WAZZUBF         0x4f61
#define VID_HYBRIDC         0x4F62
#define VID_IRISHG          0x4F63
#define VID_SURABAYAA       0x4F64
#define VID_BANDUNGP        0x4f65
#define VID_COLORSX         0x4f66
#define VID_NPADAC          0x4f67
#define VID_RAPIR           0x4f68
#define VID_MVDK            0x4f69
#define VID_NJWMDLK1E       0x4F6A
#define VID_NPADAD          0x4f6B
#define VID_IRISHE          0x4f6c
#define VID_CONCEPTAC       0x4f6d
#define VID_WAZZUBK         0x4F6E
#define VID_NPADAE          0x4F6F
#define VID_RAPIS2          0x4f70
#define VID_COLORSY         0x4f71
#define VID_COLORSZ         0x4f72
#define VID_NPADAG          0x4F73
#define VID_NPADAH          0x4f74
#define VID_JOHNNYD         0x4F75
#define VID_CE              0x4f76
#define VID_MUCKD           0x4f77
#define VID_CONCEPTAE       0x4f78
#define VID_RAPIT           0x4F79
#define VID_RAPIT1          0x4F7A
#define VID_RAPIT2          0x4F7B
#define VID_CONCEPTAF       0x4F7C
#define VID_ERASERE         0x4f7d
#define VID_SHOWOFFS        0x4f7e  //re-use
#define VID_NPADAF          0x4f7f  //re-use
#define VID_NPADAI          0x4f80
#define VID_COLORSAA        0x4f81
#define VID_COLORSAB        0x4f82
#define VID_CONCEPTAB       0x4f83
#define VID_CONCEPTAD       0x4f84
#define VID_WAZZUBI         0x4f85
#define VID_WAZZUBJ         0x4f86
#define VID_WAZZUBL         0x4f87
#define VID_WAZZUBM         0x4f88
#define VID_WAZZUBN         0x4f89
#define VID_WAZZUBO         0x4f8a
#define VID_HELPERB         0x4f8b
#define VID_SHOWOFFT        0x4f8c
#define VID_SHOWOFFU        0x4f8d
#define VID_COLORSAC        0x4f8e
#define VID_COLORSAD        0x4f8f
#define VID_JOHNNYE         0x4f90
#define VID_BANDUNGREM      0x4f91
#define VID_COLORSAE        0x4f92
#define VID_RAPIU2          0x4f93
#define VID_BANDUNGQ        0x4f94
#define VID_ERASERJ         0x4f95
#define VID_ATOMJ           0x4f96
#define VID_SHOWOFFW        0x4f97
#define VID_SWLABSB         0x4F98
#define VID_SHOWOFFX        0x4f99
#define VID_NPADAJ          0x4f9A
#define VID_PAYCHECKA       0x4f9B
#define VID_NPADAK          0x4f9c
#define VID_RAPIV2          0x4f9d
#define VID_JOHNNYF         0x4f9e
#define VID_NICEDAYD        0x4f9f
#define VID_MTFA            0x4fa0
#define VID_WAZZUBP         0x4fa1
#define VID_SHOWOFFV        0x4fa2
#define VID_SHOWOFFK        0x4fa3
#define VID_SHOWOFFM        0x4fa4
#define VID_SHOWOFFN        0x4fa5
#define VID_SHOWOFFO        0x4fa6
#define VID_SHOWOFFP        0x4fa7
#define VID_SHOWOFFQ        0x4fa8
#define VID_SHOWOFFR        0x4fa9
#define VID_COLORSAF        0x4faa
#define VID_CLOCKG          0x4fab
#define VID_CLOCKH          0x4fac
#define VID_NPADAL          0x4fad
#define VID_ERASERF         0x4fae
#define VID_ERASERG         0x4faf
#define VID_ERASERH         0x4fb0
#define VID_ERASERI         0x4fb1
#define VID_ERASERK         0x4fb2
#define VID_ERASERL         0x4fb3
#define VID_COLORSAG        0x4fb4
#define VID_TEMPLEA         0x4fb5
#define VID_MINIMALE        0x4fb6
#define VID_MINIMALF        0x4fb7
#define VID_MINIMALG        0x4fb8
#define VID_MINIMALH        0x4fb9
#define VID_MINIMALI        0x4fba
#define VID_MINIMALJ        0x4fbb
#define VID_ARMADILLOA      0x4fbc
#define VID_SWORDA          0x4fbd
#define VID_DMVD            0x4fbe
#define VID_DOGGIEC         0x4fbf
#define VID_DOGGIED         0x4fc0
#define VID_SETMD           0x4fc1
#define VID_LAZYA           0x4fc2
#define VID_HAGGISA         0x4fc3
#define VID_LEMONA          0x4fc4
#define VID_PHARDERAE       0x4fc5
#define VID_NOPF            0x4fc6
#define VID_SHOWOFFY        0x4fc7
#define VID_NPADAM          0x4fc8
#define VID_MDMAII          0x4fc9
#define VID_MDMAD           0x4fca
#define VID_MDMAG           0x4fcb
#define VID_COLORSAH        0x4fcc
#define VID_CONCEPTAH       0x4fcd
#define VID_BANDUNGR        0x4fce
#define VID_MESSENGERA      0x4fcf
#define VID_DARKB           0x4fd0
#define VID_HUNTERA         0x4fd1
#define VID_HUNTERB         0x4fd2
#define VID_MERCYA          0x4fd3
#define VID_SAMA            0x4fd4
#define VID_FRIDAYB         0x4fd5
#define VID_BERTIKA         0x4fd6
#define VID_HILIGHTA        0x4fd7
#define VID_MOTA            0x4fd8
#define VID_NJ_WMDLK1FVIR   0x4fd9
#define VID_SWLABSA         0x4fda
#define VID_TERRORA         0x4fdb
#define VID_SHOWOFFZ        0x4fdc
#define VID_COLORSAI        0x4fdd
#define VID_ERASERM         0x4fde
#define VID_ERASERN         0x4fdf
#define VID_NOPG            0x4fe0
#define VID_WALLPAPERA      0x4fe1
#define VID_TALOND          0x4fe2
#define VID_DARKC           0x4fe3
#define VID_NUCLEARI        0x4fe4
#define VID_MINIMALK        0x4fe5
#define VID_MINIMALL        0x4fe6
#define VID_CAPC            0x4fe7
#define VID_ANDRYA          0x4fe8
#define VID_BANDUNGS        0x4fe9
#define VID_SHOWOFFAA       0x4fea
#define VID_SHOWOFFAB       0x4feb
#define VID_RAPIW2          0x4fec
#define VID_CAPD            0x4fed
#define VID_FUZZYA          0x4fee
#define VID_CONCEPTAG       0x4fef
#define VID_MDMAH           0x4ff0
#define VID_LEMONB          0x4ff1
#define VID_NPADAN          0x4ff2
#define VID_TALONF          0x4ff3
#define VID_TALONG          0x4ff4
#define VID_MINIMALM        0x4ff5
#define VID_DARKD           0x4ff6
#define VID_TALONE          0x4ff7
#define VID_ATOMG           0x4ff8
#define VID_WAZZUBC         0x4ff9
#define VID_BADBOYC         0x4ffa
#define VID_JOHNNYG         0x4ffb
#define VID_SHOWOFFAE       0x4ffc
#define VID_WAZZUBR         0x4ffd
#define VID_SHOWOFFAD       0x4ffe
#define VID_COLORSAK        0x4fff
#define VID_FULL_SET        0x4E00
#define VID_IRISHI          0x4E01
#define VID_BANDUNGU        0x4E02
#define VID_SHOWOFFAC       0x4E03
#define VID_CVCK1           0x4E04
#define VID_DRUGSA          0x4E05
#define VID_BANDUNGT        0x4E06
#define VID_COLORSAJ        0x4E07
#define VID_CONCEPTAI       0x4E08
#define VID_IRISHH          0x4E09
#define VID_MDMAI           0x4E0A
#define VID_SATANICB        0x4E0B
#define VID_WAZZUBQ         0x4E0C
#define VID_OVALA           0x4E0D
#define VID_CVCK1A          0x4E0E
#define VID_CAPE            0x4E0F
#define VID_BANDUNGV        0x4E10
#define VID_BANDUNGW        0x4E11
#define VID_ERASERO         0x4E12
#define VID_MDMAJ           0x4E13
#define VID_MDMAK           0x4E14
#define VID_NO_FB           0x4E15
#define VID_SHOWOFFAF       0x4E16
#define VID_SHOWOFFAG       0x4E17
#define VID_CAPF            0x4E18
#define VID_NPADAQ          0x4e19
#define VID_NPADAR          0x4e1a
#define VID_BANDUNGX        0x4e1b
#define VID_SHOWOFFAH       0x4e1c
#define VID_COLORSAL        0x4E1D
#define VID_NPADAS          0x4E1E
#define VID_JOHNNYI         0x4E1F
#define VID_NPADAT          0x4E20
#define VID_COLORSAN        0x4e21
#define VID_NPADAO          0x4e22
#define VID_NUCLEARK        0x4e23
#define VID_TWOLINESD       0x4e24
#define VID_COLORSAO        0x4e25
#define VID_SHOWOFFAI       0x4e26
#define VID_WAZZUBS         0x4e27
#define VID_NJWMDLK1G       0x4e28
#define VID_SHOWOFFAN       0x4e29
#define VID_IRISHJ          0x4e2d
#define VID_RAPIAB2         0x4e2e
#define VID_NPADAV          0x4E32
#define VID_RAPIAA2         0x4E33
#define VID_TWOLINESE       0x4E34
#define VID_SHOWOFFAP       0x4E35
#define VID_CAPJ            0x4E36
#define VID_RAPIAC          0x4E37
#define VID_RAPIAC1         0x4E38
#define VID_RAPIAC2         0x4E39
#define VID_TWOLINESF       0x4e3a
#define VID_DATEB           0x4e3b
#define VID_HIAC            0x4e3c
#define VID_NPADAP          0x4e3d
#define VID_CAPG            0x4e3e
#define VID_CAPH            0x4e3f
#define VID_CAPI            0x4e40
#define VID_CAPK            0x4e41
#define VID_RAPIAZ2         0x4e42
#define VID_NICEDAYE        0x4e43
#define VID_COLORSAP        0x4e44
#define VID_MDMAM           0x4E45
#define VID_NOPH            0x4e46
#define VID_RAPIX           0x4e47
#define VID_RAPIY           0x4e48
#define VID_RAPIY1          0x4e49
#define VID_SIMPLEA         0x4e4a
#define VID_SIMPLEB         0x4e4b
#define VID_WAZZUBT         0x4e4c
#define VID_TOTEN           0x4e4d
#define VID_COLORSAQ        0x4e4e
#define VID_NPADAX          0x4e4f
#define VID_SHOWOFFAJ       0x4e50
#define VID_SHOWOFFAK       0x4e51
#define VID_SHOWOFFAL       0x4e52
#define VID_SHOWOFFAO       0x4e53
#define VID_BANDUNGY        0x4e54
#define VID_GOODNIGHTB      0x4E55
#define VID_SHUFFLEA        0x4e56
#define VID_COLORSAM        0x4e57
#define VID_WAZZUBD         0x4e58
#define VID_HELPERC         0x4e59
#define VID_HELPERD         0x4e5a
#define VID_HELPERE         0x4e5b
#define VID_JOHNNYH         0x4e5c
#define VID_ERASERP         0x4e5d
#define VID_LUNCHC          0x4e5e
#define VID_NOPJ            0x4e5f
#define VID_HIDERA          0x4e60
#define VID_TESTAREA        0x4e61
#define VID_IMPOSTERC       0x4e62
#define VID_CONCEPTAK       0x4e63
#define VID_DAVEA           0x4e64
#define VID_ATOMI           0x4e65
#define VID_NOPI            0x4e66
#define VID_COUNTTEND       0x4e67
#define VID_CEEFOURB        0x4e68
#define VID_GOLDFISHB       0x4e69
#define VID_CVCK1B          0x4e6a
#define VID_NUCLEARJ        0x4e6b
#define VID_IRISHK          0x4e6c
#define VID_BADBOYA         0x4e6d
#define VID_TALONA          0x4e6e
#define VID_DUBA            0x4e6f
#define VID_TALONH          0x4e70
#define VID_TALONH1         0x4e71
#define VID_TALONH2         0x4e72
#define VID_TALONB          0x4e73
#define VID_TALONC          0x4e74
#define VID_PHARDERAB       0x4e75
#define VID_DIVINAE         0x4e76
#define VID_GOGGLESA        0x4e77
#define VID_RANDOMA         0x4e78
#define VID_SPIRALA         0x4e79
#define VID_RAPIAD2         0x4e7a
#define VID_CONCEPTAL       0x4e7b
#define VID_NPADAA          0x4e7c
#define VID_NUCLEARL        0x4e7d
#define VID_NJWMDLK1A       0x4e7e
#define VID_BANDUNGZ        0x4e80
#define VID_JOHNNYJ         0x4e81
#define VID_NICEDAYF        0x4e82
#define VID_NUCLEARM        0x4e83
#define VID_TWOLINESG       0x4e84
#define VID_CONCEPTAM       0x4e85
#define VID_NOPK            0x4E86
#define VID_CONCEPTAN       0x4E87
#define VID_APPDERD         0x4E88
#define VID_NPADAU          0x4e89
#define VID_NPADAY          0x4e8a
#define VID_NPADAZ          0x4e8b
#define VID_CONCEPTAJ       0x4e8c
#define VID_SHAREFUNB       0x4e8d
#define VID_MINIMALN        0x4e8e
#define VID_WAZZUBU         0x4e8f
#define VID_WAZZUBV         0x4e90
#define VID_WAZZUBW         0x4e91
#define VID_WAZZUBX         0x4e92
#define VID_WAZZUBY         0x4e93
#define VID_WAZZUBZ         0x4e94
#define VID_WAZZUCA         0x4e95
#define VID_MDMAN           0x4e96
#define VID_CAPL            0x4e97
#define VID_RAPIAE1         0x4e98
#define VID_RAPIAE2         0x4e99
#define VID_BANDUNGAA       0x4e9a
#define VID_BANDUNGAB       0x4e9b
#define VID_SHOWOFFAM       0x4e9c
#define VID_SHOWOFFAQ       0x4e9d
#define VID_CONCEPTAO       0x4E9E
#define VID_BANDUNGAD       0x4e9f
#define VID_SHOWOFFAR       0x4ea0
#define VID_MINIMALO        0x4ea1
#define VID_ALIEND          0x4ea2
#define VID_ALIENE          0x4ea3
#define VID_BANDUNGAC       0x4ea4
#define VID_COLORSAR        0x4ea5
#define VID_IRISHL          0x4ea6
#define VID_FRENZYA         0x4ea7
#define VID_BANDUNGAE       0x4ea8
#define VID_SPARKLEA        0x4ea9
#define VID_WAZZUCB         0x4eaa
#define VID_DZTC            0x4eab
#define VID_NPADBA          0x4eac
#define VID_RAPIAF1         0x4ead
#define VID_RAPIAF2         0x4eae
#define VID_RAPIAG          0x4eaf
#define VID_RAPIAG1         0x4eb0
#define VID_RAPIAG2         0x4eb1
#define VID_SHOWOFFAS       0x4eb2
#define VID_SHOWOFFAT       0x4eb3
#define VID_MDMAO           0x4eb4
#define VID_TWOLINESH       0x4eb5
#define VID_COLORSAT        0x4eb6
#define VID_COLORSAS        0x4eb7
#define VID_DIVINAF         0x4eb8
#define VID_JOHNNYK         0x4eb9
#define VID_CONCEPTAP       0x4eBA
#define VID_NICEDAYG        0x4ebb
#define VID_BANDUNGO        0x4eBC
#define VID_DMVE            0x4ebe
#define VID_NPADBB          0x4ebf
#define VID_APPDERE         0x4ec0

//Word Macro Virus Variant Detection
#define VID_NUCLEAR_VARIANT 0x1ed1
#define VID_NPAD_VARIANT    0x1ed2
#define VID_MDMA_VARIANT    0x1ed3
#define VID_ATOM_VARIANT    0x1ed4
#define VID_BANDUNG_VARIANT 0x1ed5
#define VID_COLORS_VARIANT  0x1ed6
#define VID_CONCEPT_VARIANT 0x1ed7
#define VID_WAZZU_VARIANT   0x1ed8
#define VID_OUTLAW_VARIANT  0x1ed9
#define VID_RAPI_VARIANT    0x1eda
#define VID_TWNO_VARIANT    0x1edb
#define VID_NJ_WMVCK_VARIANT 0x1edd
#define VID_NJ_WMDLK_VARIANT 0x1ede

//Word Macro Virus Trash Dummy Detection
#define VID_WM_TRASH1       0x0013
#define VID_WM_TRASH2       0x0014


// Excel macro viruses
#define VID_LAROUX          0x1fa9
#define VID_LAROUXB         0x1e0a
#define VID_LAROUXC         0x4e7f
#define VID_SOFA            0x1e0f
#define VID_DELTA           0x1e1a
#define VID_XLDMV           0x1548
#define VID_LEGEND          0x4f2c
#define VID_ROBOCOPA        0x4f2f


// Windows NE viruses

#define VID_WINVIR      0x1f64
#define VID_WINVIK      0x1f65
#define VID_WINTINY     0x1f70
#define VID_WINSURF     0x1f71
#define VID_WINLAMER    0x1f72
#define VID_WINTENTACL1 0x1f7b
#define VID_WINTENTACL2 0x1f80
#define VID_WINNEWTENT  0x1f8d
#define VID_WINTVPO     0x1e09

// Windows PE viruses

#define VID_BOZA        0x1f59

// Java viruses

#define VID_JAVATEST    0x1f77
#define VID_JAVATEST2   0x1f78

#endif // #ifndef _MACROVID_H_
