//************************************************************************
//
// $Header:   S:/NAVEX/VCS/navexshr.h_v   1.385   15 Dec 1998 12:10:40   DCHI  $
//
// Description:
//      Contains NAVEX code shared among all modules.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/navexshr.h_v  $
// 
//    Rev 1.385   15 Dec 1998 12:10:40   DCHI
// Added GetTempFile() prototype.
// 
//    Rev 1.384   18 Nov 1998 21:04:12   relniti
// change 288 data again
// 
//    Rev 1.383   18 Nov 1998 20:55:18   relniti
// reverted previous try
// 
//    Rev 1.382   18 Nov 1998 20:48:32   relniti
// change sector per track value of 2.88MB information to 0x12 from 0x24
// to fix generic boot repair problem.
// 
//    Rev 1.381   10 Mar 1998 15:43:08   JWILBER
// Added extern to prototypes for RepairFileMoveBytes and
// SearchFileForString.
//
//    Rev 1.380   10 Mar 1998 15:21:44   JWILBER
// Move prototypes here for RepairFileMoveBytes() and
// SearchFileForString() functions here from NAVEXRF.CPP.
//
//    Rev 1.379   26 Jan 1998 13:56:20   JWILBER
// Added #defines for FileGetDateTime and FileSetDateTime callbacks
// in NAVEX.
//
//    Rev 1.378   28 Jul 1997 12:55:12   AOONWAL
// Modified during AUG97 update
//
//    Rev 1.377   17 Jul 1997 18:09:04   JWILBER
// Added prototype for EXTScanOneHalf().
//
//    Rev 1.376   20 Jun 1997 12:13:20   DCHI
// Added undef of AnsiNext for SYM_VXD due to platform.h change.
//
//    Rev 1.375   03 Apr 1997 10:44:26   DCHI
// Removed VIDs and IDs and placed in navexvid.h and macrovid.h.
//
//    Rev 1.374   03 Apr 1997 09:32:08   RELNITI
// ADD VID_RAPIV2 0x4f9d & VID_JOHNNYF 0X4f9e
//
//    Rev 1.373   03 Apr 1997 08:25:28   RELNITI
// ADD VID_NPADAK 0x4f9c
//
//    Rev 1.372   02 Apr 1997 19:22:58   AOONWAL
// Removed ID_TWNO. TWNO.5248 is now using ID_YESMILE
//
//    Rev 1.371   02 Apr 1997 14:11:00   ECHIEN
// Added VID_PAYCHECKA 0x4f9B
//
//    Rev 1.370   02 Apr 1997 13:16:14   ECHIEN
// Added VID_NPADAJ 0x04f9A
//
//    Rev 1.369   02 Apr 1997 13:02:14   DANG
// Added WM.Swlabs.B for CI#11353
//
//    Rev 1.368   02 Apr 1997 12:08:24   RELNITI
// ADD VID_SHOWOFFW 0x4f99
//
//    Rev 1.367   02 Apr 1997 11:42:42   CFORMUL
// Added VID_SHOWOFFV
//
//    Rev 1.366   02 Apr 1997 09:43:36   RELNITI
// ADD VID_RAPIU2, VID_BANDUNGQ, VID_ERASERJ, VID_ATOMJ
//
//    Rev 1.365   01 Apr 1997 21:08:08   RELNITI
// ADD VID_COLORSAE 0x4f92
//
//    Rev 1.364   01 Apr 1997 17:17:48   AOONWAL
// Added #define ID_TWNO for TWNO.5248 file repair
//
//    Rev 1.363   01 Apr 1997 16:44:42   ECHIEN
// Added VID_BANDUNGREM 0x04f91
//
//    Rev 1.362   01 Apr 1997 11:44:22   ECHIEN
// Added JohnnyE 0x04f90
//
//    Rev 1.361   01 Apr 1997 09:52:28   ECHIEN
// Added VID_COLORSAD 0x04f8f
//
//    Rev 1.360   28 Mar 1997 12:32:02   CFORMUL
// Added VID_COLORSAC
//
//    Rev 1.359   28 Mar 1997 11:59:46   AOONWAL
// Added define for VID_HELPERB
//
//    Rev 1.358   28 Mar 1997 11:23:10   ECHIEN
// changed showoff.t => 4f8c and showoff.u => 0x4f8d
//
//    Rev 1.357   28 Mar 1997 11:16:42   ECHIEN
// Added VID_SHOWOFFT 0x04f8b and VID_SHOWOFFU 0x04f8c
//
//    Rev 1.356   27 Mar 1997 23:12:14   RELNITI
// RENAME 0x4f6d: VID_CONCEPTAB => VID_CONCEPTAC
//
//    Rev 1.355   27 Mar 1997 23:01:44   RELNITI
// ADD VID_NpadAF (0x4f7f reused)
//     VID_ColorsAA, AB,
//     VID_ConceptAB, AD,
//     VID_WazzuBI, BJ, BL, BM, BN, BO (0x4f81-0x4f8a)
//   all based on VMACRO CRC
// RENAME 0x4f6e: VID_WAZZUBI => VID_WAZZUBK
//
//    Rev 1.354   27 Mar 1997 20:41:52   RELNITI
// ADD VID_SHOWOFFS 4f7e (re-use)
//
//    Rev 1.353   27 Mar 1997 20:17:40   ECHIEN
// Commented out Colors.AA  using a modified ColorsE instead
//
//    Rev 1.352   27 Mar 1997 19:32:14   ECHIEN
// Commented out COlorsAB it will use ColorsB Remnant instead
//
//    Rev 1.351   27 Mar 1997 18:39:00   CFORMUL
// Added VID_NPADAI
//
//    Rev 1.350   27 Mar 1997 18:20:00   ECHIEN
// Added VID_COLORSAB 0x4f7f
//
//    Rev 1.349   27 Mar 1997 17:13:48   ECHIEN
// Added VID_COLORSAA 0x4f7e
//
//    Rev 1.348   27 Mar 1997 12:55:50   RELNITI
// ADD VID_ERASERE
//
//    Rev 1.347   26 Mar 1997 14:39:22   AOONWAL
// Added VID_CONCEPTAF
//
//    Rev 1.346   26 Mar 1997 12:21:54   DANG
// Added Rapi.T - T2 for CI#11296
//
//    Rev 1.345   26 Mar 1997 11:29:44   RELNITI
// ADD VID_CONCEPTAE, 4F78
//
//    Rev 1.344   26 Mar 1997 11:01:04   ECHIEN
// Added VID_MUCKD 0x4f77
//
//    Rev 1.343   25 Mar 1997 21:38:12   RELNITI
// ADD VID_CE, 4f76
//
//    Rev 1.342   25 Mar 1997 17:45:38   DANG
// Added Johhny.D for CI#11293
//
//    Rev 1.341   25 Mar 1997 17:30:58   CFORMUL
// Added VID_NPADAH
//
//    Rev 1.340   25 Mar 1997 16:20:50   DANG
// Added NPad.AG for CI#11299
//
//    Rev 1.339   25 Mar 1997 15:17:42   ECHIEN
// Added define for VID 0x4f72 WM.COLORSZ
//
//    Rev 1.338   25 Mar 1997 11:53:28   AOONWAL
// Added VID_COLORSY
//
//    Rev 1.337   25 Mar 1997 10:59:34   AOONWAL
// Modified during APRIL 97 build
//
//    Rev APRIL97 24 Mar 1997 18:08:18   CFORMUL
// Added VID_RAPIS2
//
//    Rev APRIL97 20 Mar 1997 17:38:20   DANG
// Added Npad.AE for CI#11251
//
//    Rev APRIL97 20 Mar 1997 16:04:50   DANG
// Added Wazzu.BI for CI#11215
//
//    Rev APRIL97 20 Mar 1997 15:18:28   AOONWAL
// Added VID_CONCEPTAB
//
//    Rev APRIL97 20 Mar 1997 14:01:52   RELNITI
// ADD    0x4f6c: VID_IRISHE
// MODIFY 0x4f56: VID_IRISHE => VID_IRISHF
//        0x4f63: VID_IRISHF => VID_IRISHG
//        0x4f67: VID_NPADAA => VID_NPADAC
//
//    Rev APRIL97 20 Mar 1997 13:56:40   CFORMUL
// Changed NPADAC to NPADAD
//
//    Rev APRIL97 20 Mar 1997 12:36:34   CFORMUL
// changed VID_NPADAC to the correct VID, doh!
//
//    Rev APRIL97 20 Mar 1997 12:26:30   CFORMUL
// Added VID_NAPDAC
//
//    Rev APRIL97 20 Mar 1997 12:24:24   DANG
// Added WM.NJ-WMDLK1.E for CI#11247
//
//    Rev APRIL97 19 Mar 1997 20:55:14   JWILBER
// Added #define for VID_MVDK, 4f69.
//
//    Rev APRIL97 19 Mar 1997 18:57:00   AOONWAL
// Added VID_RAPIR 0x4f68
//
//    Rev APRIL97 19 Mar 1997 17:18:58   JWILBER
// Changed VID_NPADO for 4f67 to VID_NPADAA.
//
//    Rev APRIL97 19 Mar 1997 17:05:18   JWILBER
// Enabled repair for WM.NPAD.O, 4f67.
//
//    Rev APRIL97 19 Mar 1997 11:49:40   RELNITI
// ADD VID_COLORSX (4f66)
//
//    Rev APRIL97 18 Mar 1997 18:27:40   AOONWAL
// Changed VID_CONCEPTAB to VID_SURABAYAA
//
//    Rev APRIL97 18 Mar 1997 16:48:02   CFORMUL
// Added BandungP
//
//    Rev APRIL97 18 Mar 1997 01:40:22   AOONWAL
// Added VID_CONCEPTAB 0x4f64
//
//    Rev APRIL97 17 Mar 1997 21:36:08   DANG
// Added Irish.F for CI#11206
//
//    Rev 1.336   17 Mar 1997 15:52:48   DANG
// Added Hybrid.C for CI#11205
//
//    Rev 1.335   14 Mar 1997 20:07:06   RELNITI
// MODIFY 0x1e9a: VID_WAZZUAV => VID_WAZZUBG
//        0x1e9d: VID_WAZZUAW => VID_WAZZUBH
// ADD VID_WAZZUAV 0x4f5f
//     VID_WAZZUAW 0x4f60
//     VID_WAZZUBF 0x4f61
//
//    Rev 1.334   14 Mar 1997 18:33:28   RELNITI
// ADD VID_COLORSW (4f5e)
// RENAME 1EB0 VID_COLORSI => VID_MSWA to reflect name-changing
//
//    Rev 1.333   14 Mar 1997 13:52:32   AOONWAL
// Added VID_COLORSH
//
//    Rev 1.332   14 Mar 1997 10:01:06   RELNITI
// ADD VID_RAPIQ1, VID_RAPIQ2
//
//    Rev 1.331   13 Mar 1997 21:59:18   RELNITI
// ADD VID_NPADN 4f59
//     VID_NPADAB 4f5a
//
//    Rev 1.330   13 Mar 1997 21:16:42   RELNITI
// MODIFY 0x1e1b: VID_NPADL => VID_NPADQ
//        0x1eae: VID_NPADN => VID_NPADL
//        0x1e7f: VID_NPADQ => VID_NPADP
//        0x1e19: VID_NPADM => VID_NPADO
//
//    Rev 1.329   13 Mar 1997 17:17:54   RELNITI
// ADD VID_COLORSV 0x4f41
//
//    Rev 1.328   13 Mar 1997 14:52:24   AOONWAL
// Added VID_SHOWOFFL
//
//    Rev 1.327   13 Mar 1997 10:06:00   AOONWAL
// Added VID_SMILEYA
//
//    Rev 1.326   12 Mar 1997 16:38:06   JWILBER
// Added VID_IRISHE, 4f56.
//
//    Rev 1.325   12 Mar 1997 15:41:26   AOONWAL
// Modified VID defines for 0x1f54 and 0x1f55
//
//    Rev 1.324   12 Mar 1997 14:02:50   AOONWAL
// Added vid define for 0x4f54 and 0x4f55
//
//    Rev 1.323   11 Mar 1997 20:35:02   RELNITI
// CHANGE 0x4f52 => VID_COLORSS
// REMOVE 0x4f41
//
//    Rev 1.322   11 Mar 1997 19:29:24   RELNITI
// ADD VID_COLORSBREMN
//
//    Rev 1.321   11 Mar 1997 15:40:34   RELNITI
// ADD VID_COLORSU, VID_COLORSV
//
//    Rev 1.320   11 Mar 1997 14:45:30   CFORMUL
// Added VID_ERASERB, VID_ERASERC, VID_ERASERD
//
//    Rev 1.319   11 Mar 1997 10:39:14   RELNITI
// ADD VID_WAZZUBE, VID_WAZZUAF, VID_PHARDERAD, VID_FIREA
// CHANGE 0x4f36: VID_FINDZS => VID_LOOKD
//
//    Rev 1.318   10 Mar 1997 20:00:28   RELNITI
// ADD VID_COLORSP, VID_COLORSQ, VID_COLORSR, VID_JOHNNYC,
//     VID_SHOWOFFH, VID_SHOWOFFI
//
//    Rev 1.317   10 Mar 1997 18:04:44   JWILBER
// Add #define for WM.Bandung.O.
//
//    Rev 1.316   10 Mar 1997 14:03:44   JWILBER
// Added VID_COLORST.
//
//    Rev 1.315   07 Mar 1997 16:54:16   RELNITI
// ADD VID_COLORSS
//
//    Rev 1.314   07 Mar 1997 12:02:34   DANG
// Added ShowOff.J CI#11087
//
//    Rev 1.313   06 Mar 1997 19:39:20   RELNITI
// CHANGE 0x4f31: VID_NPADY => VID_NPADZ
//
// ADD VID_ERASERA, VID_MINIMALD, VID_BABYA, VID_APPDERB,
//  VID_MINDA, VID_SHOWOFFG, VID_NPADY, VID_APPDERC
//
//    Rev 1.312   06 Mar 1997 17:40:06   DANG
// Added Irish.D.  CI#11084
//
//    Rev 1.311   06 Mar 1997 14:37:18   RELNITI
// ADD VID_FINDZS
//
//    Rev 1.310   06 Mar 1997 13:58:18   RELNITI
// ADD VID_WEATHERD, VID_THEATREC, VID_WHY
//
//    Rev 1.309   06 Mar 1997 12:24:34   CFORMUL
// Added VID_ROBOCOPA
//
//    Rev 1.308   06 Mar 1997 12:21:48   AOONWAL
// Added VID_TWOLINESC
//
//    Rev 1.307   05 Mar 1997 19:47:22   RELNITI
// ADD VID_NPADY
//
//    Rev 1.306   05 Mar 1997 19:07:42   DANG
// Added Rapi.P.  CI#11072
//
//    Rev 1.305   05 Mar 1997 16:43:20   DANG
// Added Rapi.O after finding sample. CI#11055
//
//    Rev 1.304   05 Mar 1997 16:37:22   RELNITI
// ADD VID_COLORSO
//
//    Rev 1.303   05 Mar 1997 16:19:14   CFORMUL
// Added VID_LEGEND
//
//    Rev 1.302   05 Mar 1997 15:44:40   DANG
// Added Rapi.O1 and Rapi.O2.  CI#11055  Did not have Rapi.O
//
//    Rev 1.301   05 Mar 1997 14:43:30   CFORMUL
// Added VID_LOOKA
//
//    Rev 1.300   05 Mar 1997 13:11:08   AOONWAL
// Added vid_conceptr
//
//    Rev 1.299   04 Mar 1997 15:15:24   CFORMUL
// Added VID_THEATREB
//
//    Rev 1.298   04 Mar 1997 14:35:34   RELNITI
// ADD VID_SNICKERSA, VID_SWLABS3, VID_VH1, VID_VH1A, VID_PHARDERAC
//
//    Rev 1.297   04 Mar 1997 11:45:32   RELNITI
// ADD VID_CAP_DAMAGED
//
//    Rev 1.296   28 Feb 1997 17:32:40   CFORMUL
// Added VID_WEATHERA
//
//    Rev 1.295   28 Feb 1997 15:58:26   RELNITI
// ADD  VID_WAZZUAY, VID_WAZZUAZ, VID_WAZZUBA, VID_NPADU
//  VID_NUCLEARG, VID_NUCLEARH, VID_DIVINAD,
//  VID_ALIENC, VID_NICEDAYC,VID_NO_FA,VID_SHOWOFFF,VID_CLOCKF
//
//    Rev 1.294   28 Feb 1997 11:47:30   RELNITI
// MODIFY VID_RAPIR2 => VID_RAPIM2
//
//    Rev 1.293   27 Feb 1997 18:26:42   RELNITI
// COMMENTED OUT VID_NUCLEARC 0x1fec
// UNCOMMENTED   VID_NUCLEARC 0x1e63
// for duplicates virscan entry
//
//    Rev 1.292   27 Feb 1997 18:10:04   JWILBER
// Changed VID_RAPIM to VID_RAPIN.  Add VID_RAPIN1 and VID_RAPIN2.
//
//    Rev 1.291   27 Feb 1997 17:58:38   AOONWAL
// Update source after the SAM build
//
//    Rev 1.289   27 Feb 1997 15:24:48   RELNITI
// ADD VID_BANDUNGN
//
//    Rev 1.288   27 Feb 1997 10:58:48   AOONWAL
// Changed WM.Wazzu.AY to WM.Wazzu.BB
//
//    Rev 1.287   26 Feb 1997 22:15:20   JWILBER
// Added VID_MUCKC, 4f10.
//
//    Rev 1.286   26 Feb 1997 18:37:16   RELNITI
// ADD VID_ANTICONCEPTA1, VID_CLOCKE, VID_CONCEPTAA, VID_DOGGIEB,
//     VID_MUCKA, VID_MUCKB
//
//    Rev 1.285   26 Feb 1997 18:23:06   CFORMUL
// Updated source after sharefun build
//
//    Rev 1.283   26 Feb 1997 16:05:40   JWILBER
// Added #define for WM.Rapi.M, 4f09.
//
//    Rev 1.282   26 Feb 1997 15:35:34   AOONWAL
// Added VID_WAZZUAY
//
//    Rev 1.281   26 Feb 1997 14:11:32   CFORMUL
// Added VID_BANDUNGM
//
//    Rev 1.280   25 Feb 1997 19:08:32   RELNITI
// CHANGE VID_WAZZUAY => VID_WAZZUN
// ADD VID_WAZZUAX
//
//    Rev 1.279   25 Feb 1997 18:09:18   JWILBER
// Fixed 4fxx VIDs again.
//
//    Rev 1.278   25 Feb 1997 17:55:30   JWILBER
// Fixed VID_DZTB to not conflict w/ other VIDs.
//
//    Rev 1.277   25 Feb 1997 16:58:28   AOONWAL
// Added VID_DZTB
//
//    Rev 1.276   25 Feb 1997 16:52:52   JWILBER
// Added permanent VIDs for Damaged Concept, and Concept Remnant,
// since the temporary ones got removed before all platforms were
// built.
//
//    Rev 1.275   25 Feb 1997 16:45:10   RELNITI
// CHANGE VID_RAPII => VID_RAPIK
//        VID_RAPIK,K1,K2 => VID_RAPII, I1, I2
// ADD VID_RAPIL, L1, L2
//
//    Rev 1.274   25 Feb 1997 16:38:20   CFORMUL
// Added VID_WAZZUU
//
//    Rev 1.273   25 Feb 1997 15:08:50   JWILBER
// Entered temporary VIDs 4f00/1.  Checked in only because *.h files
// have to be checked in to be built.  Reuse these VIDs.  No entry for
// these was checked into def4f.
//
//    Rev 1.272   25 Feb 1997 13:46:04   RELNITI
// ADD VID_COLORSN, VID_WAZZUAY
//
//    Rev 1.271   25 Feb 1997 12:08:16   AOONWAL
// Modified during MAR97 build
//
//    Rev MAR97   25 Feb 1997 10:12:36   RELNITI
// ADD VID_NPADX
//
//    Rev MAR97   24 Feb 1997 18:03:38   AOONWAL
// Added VID_CAPB  1efc
//
//    Rev MAR97   24 Feb 1997 14:43:32   CFORMUL
// Added VID_SHAREFUNA
//
//    Rev MAR97   24 Feb 1997 13:20:24   CFORMUL
// Added VID_NOPE
//
//    Rev MAR97   24 Feb 1997 13:12:38   DANG
// Added ID_WEED11 for Weed v1.1 repair
// Virus name is Weed.4080
//
//    Rev MAR97   21 Feb 1997 18:25:14   RELNITI
// MODIFY 1e9c: VID_WAZZUAR => VID_WAZZUM
// ADD    1ef5: VID_WAZZUAR
// MODIFY 1e9a: VID_WAZZUAQ => VID_WAZZUAV
// ADD    1ef6: VID_WAZZUAQ
// MODIFY 1e9d: VID_WAZZUAS => VID_WAZZUAW
// ADD    1ef7: VID_WAZZUAS
//
//    Rev MAR97   21 Feb 1997 18:08:42   JWILBER
// Changed VID_WAZZUQ to VID_WAZZUAU.
// Changed VID_WAZZUQQ back to VID_WAZZUQ.
//
//    Rev MAR97   21 Feb 1997 16:40:14   RELNITI
// ADD VID_BANDUNGL, VID_COLORSM
//
//    Rev MAR97   21 Feb 1997 14:34:16   RELNITI
// CHANGE 1ee3: VID_NPADU => VID_NPADV
// ADD 1ef2: VID_NPADW
//
//    Rev MAR97   21 Feb 1997 11:49:56   AOONWAL
// Change BoxB to all upper case BOXB
//
//    Rev MAR97   21 Feb 1997 11:29:10   RELNITI
// ADD VID_KERRANG, VID_NPADR, VID_NPADS
//
//    Rev MAR97   21 Feb 1997 11:01:46   AOONWAL
// Added vid_BOXB
//
//    Rev MAR97   20 Feb 1997 18:56:06   RELNITI
// ADD VID_RAPIK, VID_RAPIK1, VID_RAPIK2 (1eeb-d)
//
//    Rev MAR97   20 Feb 1997 15:32:18   JWILBER
// Added #define for Wazzu.AT, 1eea.
//
//    Rev MAR97   20 Feb 1997 13:44:16   JWILBER
// Changed VID_SOCIAL to VID_CAPA, because of name change.
//
//    Rev MAR97   20 Feb 1997 12:33:38   RELNITI
// ADD VID_GABLE 1ee9
//
//    Rev MAR97   20 Feb 1997 12:26:22   CFORMUL
// Changed VID_NPADO to VID_NPADT
//         VID_NPADP to VID_NPADU
//
//    Rev MAR97   19 Feb 1997 22:19:04   RELNITI
// ADD VID_NOMVIRA (1ee7), VID_NOMVIRB (1ee8)
//
//    Rev MAR97   19 Feb 1997 21:30:46   RELNITI
// ADD VID_HYBRIDB (1ee4), VID_KILLPROTA (1ee5), VID_SHOWOFFE (1ee6)
//
//    Rev MAR97   19 Feb 1997 20:45:06   RELNITI
// ADD VID_COLORSJ2 (1ea9), VID_COLORSK (1ee2)
//
//    Rev MAR97   19 Feb 1997 20:42:40   CFORMUL
// Added VID_NPADP
//
//    Rev MAR97   19 Feb 1997 17:18:08   CFORMUL
// Added VID_NPADO
//
//    Rev MAR97   19 Feb 1997 13:42:42   CFORMUL
// Added VID_RAPIJ1
//
//    Rev MAR97   18 Feb 1997 21:37:12   CFORMUL
// Added VID_BANDUNGK
//
//    Rev MAR97   18 Feb 1997 19:55:58   AOONWAL
// Added WM.TWNO.K:Tw
//
//    Rev MAR97   18 Feb 1997 15:13:50   DANG
// Pulled Wazzu.AT.  For some reason, Wazzu.Q's CRC did not show up
// when I searched for, it.  But scanning, it was detected as Wazzu.Q
// Same CRC.  Oh well.
//
//    Rev MAR97   18 Feb 1997 13:50:10   DANG
// Added Wazzu.AT CI10809
//
//    Rev MAR97   18 Feb 1997 13:13:16   CFORMUL
// Changed VID_COLORSJ from 1e9d to 1e9e
//
//    Rev MAR97   18 Feb 1997 13:02:44   CFORMUL
// Added VID_COLORSJ
//
//    Rev MAR97   14 Feb 1997 19:41:34   AOONWAL
// Added VID_WAZZUAS
//
//    Rev MAR97   14 Feb 1997 15:07:08   AOONWAL
// Added VID_WAZZUAR 1e9c
//
//    Rev MAR97   14 Feb 1997 14:42:42   AOONWAL
// Added VID_WAZZUAP 1e9b
//
//    Rev MAR97   14 Feb 1997 13:28:52   AOONWAL
// Changed VID_WAZZUAP to VID_WAZZUAQ
//
//    Rev MAR97   14 Feb 1997 13:14:58   AOONWAL
// Added vid_wazzuap  1e9a
//
//    Rev 1.270   13 Feb 1997 04:43:02   AOONWAL
// Updating newvir from defwork
//
//    Rev 1.259   11 Feb 1997 22:25:20   RELNITI
// FIXING typo: VID_DMVC, VID_NUCLEARF
//
//    Rev 1.258   11 Feb 1997 21:31:14   CFORMUL
// Added VID_XLDMV for 97 version of DMV
//
//    Rev 1.257   11 Feb 1997 21:24:08   RELNITI
// should've been VID_COLORSI2 to avoid duplicate
//
//    Rev 1.256   11 Feb 1997 21:20:24   RELNITI
// ADD VID_CLOCKC, VID_CLOCKD, VID_COLORSI, VID_DANIELC, VID_DMWC,
//     VID_FRIDAYA, VID_IRISHC, VID_JOHNNYB, VID_MDMAF, VID_NUCLEAR,
//     VID_SHOWOFFD, VID_TWNOE, VID_TWNOI, VID_XENIXOSB,
//
//    Rev 1.255   11 Feb 1997 20:25:06   AOONWAL
// Added VID_WAZZUAO
//
//    Rev 1.254   11 Feb 1997 20:14:04   RELNITI
// Ooops... misnumbered VID_BADBOYB
//
//    Rev 1.253   11 Feb 1997 19:11:42   CFORMUL
// Added VID_WAZZUAG
//
//    Rev 1.252   11 Feb 1997 18:41:46   RELNITI
// ADD  VID_RAPIR2          0x1e85
//      VID_BADBOYB         0x1e86
//      VID_BANDUNGH        0x1e87
//      VID_BANDUNGI        0x1e88
//      VID_BANDUNGJ        0x1e89
//
//    Rev 1.251   10 Feb 1997 13:48:02   CFORMUL
// Added VID_RAPIJ
//
//    Rev 1.250   07 Feb 1997 16:10:52   CFORMUL
// Added defines for Rapi.I and Run.B
//
//    Rev 1.249   06 Feb 1997 15:46:36   AOONWAL
// Added VID_NPADMM
//
//    Rev 1.248   05 Feb 1997 21:24:40   JWILBER
// Replaced #define of VID_UNKNOWNMACRO w/ VID_SOCIAL, 1ead.
//
//    Rev 1.247   04 Feb 1997 21:19:56   JWILBER
// Copied over from NewVir.
//
//    Rev 1.256   04 Feb 1997 19:03:52   RELNITI
// REMOVE DUPLICATES: VID_WAZZUU & VID_UNKNOWNMACRO
//
//    Rev 1.255   04 Feb 1997 18:27:54   AOONWAL
// Added VID_NPADQ
//
//    Rev 1.254   04 Feb 1997 12:56:46   AOONWAL
// Added VID for WM.TWNO.G:TW
//
//    Rev 1.253   03 Feb 1997 21:21:32   CFORMUL
// Added define for WM.GoodNight
//
//    Rev 1.252   03 Feb 1997 14:26:48   JWILBER
// Added #define for WM.NPAD.N, 1eae.
//
//    Rev 1.251   01 Feb 1997 14:31:52   RELNITI
// ADD VID_NJ_WMDLK_VARIANT
//
//    Rev 1.250   01 Feb 1997 12:21:10   RELNITI
// ADD VID_NJ_WMVCK_VARIANT 0x1edd
//
//    Rev 1.249   31 Jan 1997 21:30:22   RELNITI
// FEB '97 BLITZ:
// ADD VID_COOLIO2, VID_MISTERX2
//
//    Rev 1.248   31 Jan 1997 16:55:34   RELNITI
// FEB'97 BLITZ: ADD VID_RAPIA, VID_RAPIA1, VID_RAPIC, VID_RAPIC1
//
//    Rev 1.247   31 Jan 1997 14:55:28   AOONWAL
// Added VID_RUN
//
//    Rev 1.246   30 Jan 1997 18:29:00   RELNITI
// ADD VID_WAZZUC
//
//    Rev 1.245   30 Jan 1997 16:18:52   RELNITI
// ADD VID_CONCEPTZ
//
//    Rev 1.244   30 Jan 1997 15:16:34   RELNITI
// CORRECTING bad VIDs NJ- => NJ_
//
//    Rev 1.243   30 Jan 1997 14:56:36   RELNITI
// CORRECTION on duplicate VIDs due to typo, possible bad old samples, or
// DOT infections on:
// VID_CONCEPTC (0x1e4a) => should have been VID_BANDUNGC
// VID_NUCLEARC (0x1e63) => bad old sample?
// VID_ATOMB    (0x1e64) => bad old sample?
// VID_TWISTER  (0x1e66) => DOT infection, no need for 2nd VID
// VID_WAZZUQ   (0x1e6b) => bad old sample (for sure) => VID_WAZZUQQ
//                          left the old one as VID_WAZZUQ
// VID_BANDUNGB (0x1e71) => bad old sammple?
//
//    Rev 1.242   30 Jan 1997 14:21:18   RELNITI
// DEFINE:
// VID_NJ-WMDLK1AGEN, VID_CONCEPTG, VID_PHARDERA, VID_CONCEPTP
// VID_CONCEPTM, VID_CONCEPTC, VID_CONCEPTMDRP, VID_NJ-WMDLK1AVIR
// VID_NJ-WMDLK1BGEN, VID_COUNTTENA, VID_DANIELA, VID_DMVB
// VID_FURY, VID_NUCLEARD, VID_NUCLEARL, VID_HELLGATE
// VID_IMPOSTERB, VID_ITALIAN, VID_MDMAB, VID_NJ-WMDLK1CGEN
// VID_NJ-WMDLK1DGEN, VID_NJ-WMDLK1BVIR, VID_NJ-WMDLK1EGEN
// VID_CONCEPTQ, VID_CONCEPTF1, VID_CONCEPTF5, VID_RAPIB
// VID_RAPIB1, VID_RAPIB2, VID_NPADB, VID_NUCLEARC, VID_ATOMB
// VID_ATOMD, VID_TWISTER, VID_WAZZUAC, VID_WAZZUD, VID_WAZZUG
// VID_WAZZUK, VID_WAZZUQ, VID_WAZZUR, VID_WAZZUS, VID_WAZZUT
// VID_WAZZUW, VID_NJ-WMVCK3GEN, VID_BANDUNGB, VID_COLORSD
// VID_COLORSE, VID_COLORSF, VID_COLORSG
//
//    Rev 1.241   30 Jan 1997 00:08:06   CFORMUL
// Added ID_QUARTZ
//
//    Rev 1.240   29 Jan 1997 19:00:46   RELNITI
// ADD VID_BANDUNGG & VID_CONCEPTY
//
//    Rev 1.239   29 Jan 1997 14:18:18   AOONWAL
// Changed VID_TWNOE to VID_TWNOH
//
//    Rev 1.238   29 Jan 1997 14:08:56   DANG
// Added Rapi.H2
//
//    Rev 1.237   29 Jan 1997 12:20:52   AOONWAL
// Added VID for 0x1e42
//
//    Rev 1.236   28 Jan 1997 18:15:54   JWILBER
// Added #defines for macro viruses: Bandung.A   1eaf
//                                   Atom.C      1eba
//                                   Wazzu.I     1ebb
//
//    Rev 1.235   28 Jan 1997 16:10:34   AOONWAL
// Modified during feb97 build
//
//    Rev FEB97   28 Jan 1997 10:58:14   RELNITI
// ADD VID_MINIMALB, VID_RATSB, VID_RATSC, VID_TWNOG
//
//    Rev FEB97   27 Jan 1997 18:46:32   RELNITI
// CHANGE VID_NPADK (0x1e19) to VID_NPADM
// ADD VID_CLOCKBDE 0x1e3c
//     VID_NPADK    0x1e3d
//
//    Rev FEB97   27 Jan 1997 18:11:26   RELNITI
// ADD VID_ATOMH, VID_CEEFOUR, VID_CONCEPTW,
// VID_CONCEPTX, VID_DZTA, VID_IRISHB, VID_KOMPUA, VID_NICEDAYB,
// VID_RATSA, VID_SHOWOFFB, VID_SHOWOFFC, VID_SMILEYBDE,
// VID_WAZZUAJ, VID_WAZZUAK, VID_WAZZUAL, VID_WAZZUAM, VID_WAZZUAN
// (0x1e2b-0x1e3b respectively)
//
//    Rev FEB97   24 Jan 1997 17:56:12   RELNITI
// ADD VID_EPIDEMIC, VID_NIKITA, VID_NJMDLK1, VID_NOPD2, VID_SLABS1, VID_RAPIG
//
//    Rev FEB97   23 Jan 1997 15:46:08   RELNITI
// ADD VID_NFA, VID_BOOMB, VID_NOPD, VID_TWNOF for WM
//     NF.A, Boom.B:De, NOP.D:De and TWNO.F:Tw
//
//    Rev FEB97   23 Jan 1997 12:29:28   RELNITI
// ADD VID_BANDUNGE
//
//    Rev FEB97   22 Jan 1997 13:17:26   CFORMUL
// Added define for Concept.V
//
//    Rev FEB97   21 Jan 1997 13:51:46   CFORMUL
// Added define for Wazzu.AI
//
//    Rev FEB97   20 Jan 1997 17:37:42   RELNITI
// ADD VID_WAZZUAH (0x1e1c) and VID_DARKA (0x1e1d) for
//     WM.Wazzu.AH and WM.Dark.A
//
//    Rev FEB97   20 Jan 1997 17:01:40   RELNITI
// ADD VID_NFB (0x1e0c) for WM.NF.B (notes: reusing deleted VID 0x1e0c)
// Re-arrange the VID list by moving WM variant detection into a group right
// after VID's for Automated Macro output.
//
//    Rev 1.234   17 Jan 1997 18:25:44   AOONWAL
// Minor syntax changes to define names
//
//    Rev 1.233   17 Jan 1997 18:07:10   AOONWAL
// Resolved duplicate VID_CONCEPTJ
//
//    Rev 1.232   17 Jan 1997 17:58:46   AOONWAL
// Modified define names to avoid duplication
//
//    Rev 1.231   17 Jan 1997 15:47:24   AOONWAL
// Added define for Known and Unknown variant detection to support
// new Macro Engine. These modification were initially made to DEFWORK
//
//    Rev 1.230   14 Jan 1997 14:24:10   RELNITI
// Add VID_NPADL 0x1e1b for WM.NPad.L from Cust Folder #10543
//
//    Rev 1.229   09 Jan 1997 16:41:14   CFORMUL
// Added define for XM.Delta
//
//    Rev 1.228   08 Jan 1997 20:21:30   DANG
// Pulled NPad.K since NPad.J detects it.  New engine will have new def
//
//    Rev 1.227   08 Jan 1997 18:06:26   DANG
// renamed Bandung.G to NPad.K
//
//    Rev 1.226   08 Jan 1997 16:34:14   DANG
// Added def 1ED1.  Bandung.G .  Unknown detection by F4VM as of now.
//
//    Rev 1.225   08 Jan 1997 15:25:48   JWILBER
// Added defines for Tentrickle.
//
//    Rev 1.224   03 Jan 1997 18:10:28   AOONWAL
// Added VID for WM.Npad.J
//
//    Rev 1.223   03 Jan 1997 12:20:00   RELNITI
// Add VID 0x1e16 for WM.Wazzu.X
//
//    Rev 1.222   31 Dec 1996 15:33:04   CFORMUL
// Added ID for Weed.3263 repair
//
//    Rev 1.221   26 Dec 1996 15:02:16   AOONWAL
// Modified during JAN97 build
//
//    Rev 1.221   19 Dec 1996 20:01:38   CFORMUL
// Added macro define for WM.Npad.H
//
//    Rev 1.220   13 Dec 1996 17:04:34   AOONWAL
// Added VID for WM.Appder
//
//    Rev 1.219   12 Dec 1996 12:56:08   CFORMUL
// Added macro define for WM.Wazzu.Y
//
//    Rev 1.218   10 Dec 1996 15:45:04   CFORMUL
// Added define for WM.MDMA.C
//
//    Rev 1.217   09 Dec 1996 18:17:42   CFORMUL
// added define for WM.Nuclear.E
//
//    Rev 1.216   06 Dec 1996 11:56:32   RELNITI
// Removing VID_JOHNNY1 (0x1e0c) => WM.Johnny needs only 1 def for both
// normal.dot and doc infections.
//
//    Rev 1.215   05 Dec 1996 18:54:06   CFORMUL
// Added define for XM.Sofa
//
//    Rev 1.214   05 Dec 1996 17:38:42   AOONWAL
// Added define for Windows Tpvo repair
//
//    Rev 1.213   05 Dec 1996 15:59:36   CRENERT
// Removed NiceDay.
//
//    Rev 1.212   05 Dec 1996 15:57:04   CRENERT
// Added NiceDay.
//
//    Rev 1.211   05 Dec 1996 15:01:20   CRENERT
// Removed Bosnia.
//
//    Rev 1.210   05 Dec 1996 15:01:00   JWILBER
// Added define for WM.Divina.C, 1e0e.
//
//    Rev 1.209   05 Dec 1996 12:28:30   CFORMUL
// added ID for WM.Npad.E
//
//    Rev 1.208   05 Dec 1996 11:28:20   RELNITI
// changing WM.Johnny VID to 0x1e0b and 0x1e0c since Formy used 0x1e09
// for XM.Larouxb and Abid used 0x1e0a.
//
//    Rev 1.207   04 Dec 1996 20:07:26   CFORMUL
// Added define for XM.Laroux.B
//
//    Rev 1.206   04 Dec 1996 19:38:52   AOONWAL
// Added entry for WinTvpo.3783
//
//    Rev 1.205   04 Dec 1996 17:41:54   RELNITI
// Added VID for WM.Johnny
//
//    Rev 1.204   04 Dec 1996 13:00:04   JWILBER
// Added repair ID for Batman II.
//
//    Rev 1.203   03 Dec 1996 17:59:30   CFORMUL
// I'm going to do a NAVEX repair after all for Hoot.4676
//
//    Rev 1.202   02 Dec 1996 19:07:52   CFORMUL
// Removed Define for Hooter... I'm going to do a PAM instead.
//
//    Rev 1.201   02 Dec 1996 18:56:22   AOONWAL
// Added define for 0x1e08
//
//    Rev 1.200   02 Dec 1996 16:57:32   AOONWAL
// Changed define ID # from 0x1e06 to 0x1e07
//
//    Rev 1.199   02 Dec 1996 16:06:10   AOONWAL
// Added define for 0x1e06
//
//    Rev 1.198   02 Dec 1996 15:54:56   CFORMUL
// Added define for Hooter.4676 repair
//
//    Rev 1.197   02 Dec 1996 13:44:44   AOONWAL
// DEC96 update
//
//    Rev 1.201   27 Nov 1996 20:52:28   JWILBER
// Added repair ID for FourEleven.2144, 1e03.
//
//    Rev 1.200   27 Nov 1996 17:18:36   JWILBER
// Added define for FourEleven.2144, 1e03.
//
//    Rev 1.199   27 Nov 1996 11:53:30   CFORMUL
// Added define for HLLT.4416 repair
//
//    Rev 1.198   25 Nov 1996 19:38:08   AOONWAL
// Added define label for "WM.Alien.B"
//
//    Rev 1.197   22 Nov 1996 15:27:20   JWILBER
// Added define for Twno.D, 1fff.
//
//    Rev 1.196   19 Nov 1996 12:57:50   CFORMUL
// Added define for Npad.C
//
//    Rev 1.195   15 Nov 1996 23:07:06   JWILBER
// Added define for WM.AntiConcept.
//
//    Rev 1.194   14 Nov 1996 16:57:36   JWILBER
// Added define for VID of Stryx detection of Normal.dot infections.
//
//    Rev 1.193   13 Nov 1996 15:02:50   CFORMUL
// Added define for WM.Lunch.B
//
//    Rev 1.192   13 Nov 1996 01:52:52   JWILBER
// Added defines for VIDs: LookC, WeatherB, Theatre, Twno.C & B.
//
//    Rev 1.191   12 Nov 1996 12:23:30   JWILBER
// Added defines for Stryx and Outlaw macros.
//
//    Rev 1.190   07 Nov 1996 17:25:00   JWILBER
// Added defines Nuclear.A.Damaaged and Npad.
//
//    Rev 1.189   05 Nov 1996 15:59:32   JWILBER
// Added define for VID_BIGDADDY, 1fea.
//
//    Rev 1.188   05 Nov 1996 12:01:08   JWILBER
// Added define for VID_HELPER, 1fe8.
//
//    Rev 1.187   04 Nov 1996 19:57:28   JWILBER
// Removed define for MacWipe, 1fe8 - it's Concept.J!
//
//    Rev 1.186   04 Nov 1996 18:17:02   JWILBER
// Added define for WM.MacWipe, 1fe8.
//
//    Rev 1.185   04 Nov 1996 18:02:28   CFORMUL
// Added define for WM.Switches
//
//    Rev 1.184   04 Nov 1996 12:19:06   CFORMUL
// Added define for WM.Rapi
//
//    Rev 1.183   31 Oct 1996 16:58:24   AOONWAL
// Added define for Karnivali.1971
//
//    Rev 1.182   31 Oct 1996 16:54:28   CFORMUL
// Added define for WM.Alien
//
//    Rev 1.181   31 Oct 1996 15:07:54   CFORMUL
// Added define for WM.Colors.C
//
//    Rev 1.180   31 Oct 1996 10:55:28   AOONWAL
// Added define for Karnivali
//
//    Rev 1.179   30 Oct 1996 16:54:02   CFORMUL
// Added define for WM.Concept.K
//
//    Rev 1.178   29 Oct 1996 14:26:18   CFORMUL
// added define for WM.Wazzu.F
//
//    Rev 1.177   29 Oct 1996 12:56:32   AOONWAL
// Modified during NOV96 freeze
//
//    Rev 1.180   25 Oct 1996 12:16:06   CFORMUL
// Added define for WM.Wazzu.E
//
//    Rev 1.179   24 Oct 1996 14:48:54   CFORMUL
// Added define for WM.Divina.B
//
//    Rev 1.178   24 Oct 1996 13:38:12   CFORMUL
// Added define for WM.Wazzu.D
//
//    Rev 1.177   21 Oct 1996 15:00:28   CFORMUL
// Added macro for WM.Jakarta.b
//
//    Rev 1.176   18 Oct 1996 17:53:22   AOONWAL
// Added define for "WM.Concept.J"
//
//    Rev 1.175   18 Oct 1996 17:12:48   DCHI
// Fixed NTK build problem due to AnsiNext.
//
//    Rev 1.174   17 Oct 1996 16:18:08   AOONWAL
// Added define for "WM.Taiwan1"
//
//    Rev 1.173   16 Oct 1996 14:19:46   AOONWAL
// Added define for "WM.Concept.I"
//
//    Rev 1.172   16 Oct 1996 13:26:36   JWILBER
// Added VID #defines for WM.NOP.B      1fd3/4
//                        WM.Reflex     1fd5/6
//                        WM.Satanic    1fdb
//
//    Rev 1.171   16 Oct 1996 12:56:16   CFORMUL
// Added defines for WM.Tedious and WM.Concept.b
//
//    Rev 1.170   16 Oct 1996 12:08:16   AOONWAL
// Added define for "WM.COncept.E (3)" and "WM.COncept.E (4)"
//
//    Rev 1.169   11 Oct 1996 16:14:10   DCHI
// Added definition of AnsiNext for SYM_NTK.
//
//    Rev 1.168   11 Oct 1996 15:19:14   AOONWAL
// Added define for "WM.Bandung.b" Macro virus
//
//    Rev 1.167   10 Oct 1996 15:44:36   CFORMUL
// Added macros for Freddy_Krueger
//
//    Rev 1.166   09 Oct 1996 17:37:36   JWILBER
// Added define for WM.Wazzu.B, 1fcf.
//
//    Rev 1.165   07 Oct 1996 12:49:16   CFORMUL
// Added define for WM.Clock
//
//    Rev 1.164   02 Oct 1996 14:35:40   AOONWAL
// Added define for "WM.Terbura (2)"
//
//    Rev 1.163   02 Oct 1996 14:08:44   AOONWAL
// Added define for "WM.Terbaru"
//
//    Rev 1.162   27 Sep 1996 11:54:36   CFORMUL
// Added defines for abunch of macro viruses
//
//    Rev 1.161   26 Sep 1996 15:50:00   CFORMUL
// Added VID define for Y-Key.2346 repair
//
//    Rev 1.160   25 Sep 1996 15:49:34   CFORMUL
// Added define for Spooky macro virus
//
//    Rev 1.159   25 Sep 1996 15:23:34   CFORMUL
// Added define for Saver macro virus
//
//    Rev 1.158   20 Sep 1996 18:40:32   DCHI
// Added definition of AnsiNext, AnsiPrev from old platform.h.
// Was apparently removed for whatever reason.
//
//    Rev 1.157   20 Sep 1996 16:57:54   AOONWAL
// Added define for WAZZU3
//
//    Rev 1.156   20 Sep 1996 16:03:42   CFORMUL
// Added define for EasyMan Macro virus
//
//    Rev 1.155   20 Sep 1996 15:16:36   CFORMUL
// Added define for Alliance Macro
//
//    Rev 1.154   13 Sep 1996 14:48:46   JWILBER
// Added #define for VID_INDONESIA, 0x1fbe.
//
//    Rev 1.153   12 Sep 1996 22:15:34   AOONWAL
// Added define for Edwin
//
//    Rev 1.152   12 Sep 1996 21:47:28   CFORMUL
// Had wrong name for Vacsina macro.. fixed it
//
//    Rev 1.151   12 Sep 1996 21:40:54   CFORMUL
// Added ID defines for Vacsina.TP-04
//
//    Rev 1.150   10 Sep 1996 13:58:44   DANG
// moved Bogus VIDs to proper location
//
//    Rev 1.149   10 Sep 1996 13:15:10   DANG
// fixed deinfe type
//
//    Rev 1.148   10 Sep 1996 13:12:40   DANG
// Added Bogus macro virus definition
//
//    Rev 1.147   09 Sep 1996 12:41:14   CFORMUL
// Added new define for Xenixos for Mac infections
//
//    Rev 1.146   04 Sep 1996 13:57:42   SCOURSE
// Added entry for Fishfood
//
//    Rev 1.145   28 Aug 1996 19:41:44   CFORMUL
// Added defines for BuroNeu
//
//    Rev 1.144   28 Aug 1996 16:10:22   DCHI
// Added PACK1 and PACK definitions.
//
//    Rev 1.143   28 Aug 1996 15:58:06   DCHI
// Changed ISEXEHDR() macro to use AVDEREF_WORD() for dereferencing WORDs.
//
//    Rev 1.142   27 Aug 1996 18:07:22   CFORMUL
// add ConceptH defines
//
//    Rev 1.141   26 Aug 1996 18:27:38   CFORMUL
// Added define for BOSCO macro virus
//
//    Rev 1.140   19 Aug 1996 18:24:42   JWILBER
// Added #define for VID_JAKARTA.
//
//    Rev 1.139   16 Aug 1996 12:14:24   CFORMUL
// Added defines for variant of wazzu
//
//    Rev 1.138   15 Aug 1996 20:48:20   AOONWAL
// added define for Microsloth virus
//
//    Rev 1.137   15 Aug 1996 18:54:24   AOONWAL
// added define for TELEFONICA
//
//    Rev 1.136   08 Aug 1996 11:00:32   CRENERT
// Referenced 2f41 for One_Half detection/repair.
//
//    Rev 1.135   07 Aug 1996 17:48:48   JWILBER
// Added ID_OXANA for Oxana repair.
//
//    Rev 1.134   30 Jul 1996 20:57:20   CFORMUL
// Added Defines for VICE
//
//    Rev 1.133   29 Jul 1996 17:20:36   SCOURSE
// Added entry for Divina
//
//    Rev 1.132   26 Jul 1996 18:35:30   LNGUYEN
// Removed - in Color-B. def. 1fab
//
//    Rev 1.131   26 Jul 1996 18:17:16   LNGUYEN
// Renamed Benice 1fab. to Color-b
//
//    Rev 1.130   26 Jul 1996 14:08:02   CRENERT
// Returned to previous build state.
//
//    Rev 1.128   26 Jul 1996 10:47:52   LNGUYEN
// Register New ID. for BeNice.
// navexshr.h -> S:\NAVEX\VCS\navexshr.h_v
// Enter description of changes.
//
//    Rev 1.127   25 Jul 1996 18:43:18   JWILBER
// Added #define for Friendly, VID 1faa.
//
//    Rev 1.126   25 Jul 1996 18:13:42   DCHI
// Added VID_LAROUX for first Excel macro virus.
//
//    Rev 1.125   25 Jul 1996 18:08:26   RAY
// Added Generic Repair of DMF floppies.
//
//    Rev 1.124   25 Jul 1996 13:31:46   CFORMUL
// Added new defines for Hare variants
//
//    Rev 1.123   24 Jul 1996 10:46:30   LNGUYEN
// Added new def. VID. for Phantom.
//
//    Rev 1.122   23 Jul 1996 19:29:28   LNGUYEN
// Added new Vid for MadDog 1fa5.
//
//    Rev 1.121   23 Jul 1996 19:24:36   AOONWAL
// Added Nuclear.B
//
//    Rev 1.120   23 Jul 1996 18:45:12   CFORMUL
// Added define for Polite macro
//
//    Rev 1.119   23 Jul 1996 16:23:38   LNGUYEN
// Regressed changes to July 12 th.
//
//    Rev 1.111   12 Jul 1996 13:36:02   JWILBER
// Added #define for VID_ATOM, 1fa4.
//
//    Rev 1.110   11 Jul 1996 16:57:48   CFORMUL
// Had to fix up define for Hare again
//
//    Rev 1.109   11 Jul 1996 16:33:22   CFORMUL
// Modified the defines for Hare
//
//    Rev 1.108   11 Jul 1996 13:22:52   JWILBER
// Added #defines for NOP macro viruses, 1f9c/a1.
//
//    Rev 1.107   11 Jul 1996 12:37:16   CFORMUL
// Removed some comments
//
//    Rev 1.106   11 Jul 1996 12:35:54   CFORMUL
// Re-added defines for Hare cause someone commented them out!
//
//    Rev 1.105   11 Jul 1996 11:44:42   AOONWAL
// Added Parasite (2)
//
//    Rev 1.104   11 Jul 1996 10:28:02   LNGUYEN
// Renamed for MDMA_DMV MDMADMV
//
//    Rev 1.103   10 Jul 1996 20:20:14   CFORMUL
// Added another define for Hare
//
//    Rev 1.102   10 Jul 1996 18:30:44   LNGUYEN
// Added Vid 1f9d
//
//    Rev 1.101   10 Jul 1996 17:23:28   CFORMUL
// Added ID_HARE for navex file detection
//
//    Rev 1.100   10 Jul 1996 17:06:34   AOONWAL
// Added define for Parasite
//
//    Rev 1.99   09 Jul 1996 15:31:22   SCOURSE
// Added define for Moloch
//
//    Rev 1.98   08 Jul 1996 17:31:44   CFORMUL
// Added ID for Hare.7610.A repair
//
//    Rev 1.97   08 Jul 1996 16:42:46   CFORMUL
// Added Defines for HAre.7610.A
//
//    Rev 1.96   03 Jul 1996 02:06:10   CFORMUL
// Added Repair ID for HAre.7610.B
//
//    Rev 1.95   03 Jul 1996 00:00:36   CFORMUL
// Changed VID for Hare
//
//    Rev 1.94   02 Jul 1996 23:53:42   CFORMUL
// added defines for HARE.7610.B
//
//    Rev 1.93   02 Jul 1996 19:40:38   JWILBER
// Added #define for ID_TANPRO, for Tanpro repair to be called.
//
//    Rev 1.92   28 Jun 1996 15:21:00   CNACHEN
// Added VID's and ID's for Cruncher.
//
//    Rev 1.91   28 Jun 1996 14:12:04   CFORMUL
// Changed VID's for Xenixos and Boom
//
//    Rev 1.90   28 Jun 1996 13:44:06   CFORMUL
// Added macro define for Boom
//
//    Rev 1.89   27 Jun 1996 20:49:40   CFORMUL
// Added Macro define for XENIXOS
//
//    Rev 1.88   26 Jun 1996 15:36:50   CNACHEN
// Changed VID_RDA to 0x1F91.
//
//    Rev 1.87   26 Jun 1996 14:47:56   CNACHEN
// Added ID_RDA and VID_RDA.
//
//    Rev 1.86   25 Jun 1996 17:01:18   JWILBER
// Added #define for VID_DANISHBOOT because it needs a NAVEX detection.
//
//    Rev 1.85   24 Jun 1996 21:23:08   JWILBER
// Added #defines for Predator.2448 file repair, and Danish Boot repair.
//
//    Rev 1.84   12 Jun 1996 19:24:30   JWILBER
// Changed VID_WINNEWTENT to 1f8d, from 1f8c.
//
//    Rev 1.83   12 Jun 1996 18:44:22   JWILBER
// Added defines for detection and repair of Tentacle II, 1f8c.
//
//    Rev 1.82   11 Jun 1996 16:27:54   SCOURSE
// Removed define for WinHuge
//
//    Rev 1.81   10 Jun 1996 17:32:02   SCOURSE
// Added entry for WinHuge
//
//    Rev 1.80   10 Jun 1996 17:24:14   SCOURSE
// Removed entry for Winfector
//
//    Rev 1.79   10 Jun 1996 16:59:56   SCOURSE
// Added entry for Winfector(.10634)
//
//    Rev 1.78   15 May 1996 00:28:16   JWILBER
// Added ID_TENT for Tentacle repair, and added prototype for TruncateFile.
//
//    Rev 1.77   13 May 1996 16:51:02   JWILBER
// Moved CLOSEOUT() macro here from NAVEXRF.CPP.
//
//    Rev 1.76   09 May 1996 17:01:54   JWILBER
// Added VID_INFEZIONE #define.
//
//    Rev 1.75   07 May 1996 16:50:46   JALLEE
// Renamed VID_IRISH1 and VID_IRISH2 TO VID_IRISH0 and VID_IRISH1
//
//    Rev 1.74   07 May 1996 16:31:58   JALLEE
// Added VID_IRISH1 and VID_IRISH2
//
//    Rev 1.73   07 May 1996 11:56:26   JALLEE
// Added ID_BURGLAR for Burglar.1150 repair.
//
//    Rev 1.72   29 Apr 1996 19:46:48   JWILBER
// Minor mods to Wazzu virus #define.
//
//    Rev 1.71   29 Apr 1996 15:56:36   JWILBER
// Added #define for Werewolf virus.
//
//    Rev 1.70   22 Apr 1996 12:24:34   JWILBER
// Added VID_WAZZU for Wazzu macro scanning.
//
//    Rev 1.69   22 Apr 1996 11:54:20   JWILBER
// Removed #define for ID_SCREENSHAKE, since the repair is being removed.
//
//    Rev 1.68   18 Apr 1996 18:21:16   SCOURSE
// Added entry for Gen1 Tentacle sample
//
//    Rev 1.67   18 Apr 1996 17:10:20   SCOURSE
// Modified define
//
//    Rev 1.66   18 Apr 1996 17:06:56   SCOURSE
//
//    Rev 1.63   18 Apr 1996 16:18:12   SCOURSE
//
//    Rev 1.62   18 Apr 1996 14:30:16   JWILBER
// Added #define for DiamondRockSteady virus.
//
//    Rev 1.61   15 Apr 1996 20:44:08   RSTANEV
// TCHAR support.
//
//    Rev 1.60   15 Apr 1996 18:24:50   RSTANEV
// Defining SEEK_??? for SYM_NTK platform.
//
//    Rev 1.59   15 Apr 1996 16:58:24   JWILBER
// Fixed up macros slightly, for readability.
//
//    Rev 1.58   15 Apr 1996 11:33:36   JWILBER
// Added ID_SCRNSHAKE for ScreenShake5 repair.
//
//    Rev 1.57   14 Apr 1996 20:50:54   JWILBER
// Added handy macros that save lots of typing, and a prototype for the
// GetEntryPtOffset() function.
//
//    Rev 1.56   11 Apr 1996 11:44:36   JWILBER
// Added #defines for error returns, and InfectionBuffer offsets/lengths.
//
//    Rev 1.55   10 Apr 1996 21:43:06   JWILBER
// Added VID for Cordobes, 1f7a, since Navex detection is needed.
//
//    Rev 1.54   10 Apr 1996 21:00:38   JWILBER
// Added repair ID for Cordobes virus.
//
//    Rev 1.53   10 Apr 1996 12:17:30   JWILBER
// Changed #defines to reflect removal of French Concept DOC file
// detection.
//
//    Rev 1.52   09 Apr 1996 13:19:32   JALLEE
// Added VID_IMP0 and VID_IMP1 for Imposter macro virus detect and repair.
//
//    Rev 1.51   08 Apr 1996 18:01:40   SCOURSE
// Added entry for Tentacle
//
//    Rev 1.50   08 Apr 1996 17:48:16   JWILBER
// Changed #define for VID_CONCEPTF to VID_CONCEPTF0.
// Added #define for VID_CONCEPTF1.
//
//    Rev 1.49   08 Apr 1996 16:10:42   JWILBER
// Added VID_CONCEPTF, for French Concept.
//
//    Rev 1.48   04 Apr 1996 15:01:56   DCHI
// Added ID_JAVA for Java scan requests.
//
//    Rev 1.47   02 Apr 1996 11:17:22   JALLEE
// Added ID_VMEM for Vmem repair.
//
//    Rev 1.46   28 Mar 1996 19:48:42   JWILBER
// Added ID_ROMA for 1f73, Roma.PG Boot Repair.
//
//    Rev 1.45   28 Mar 1996 12:33:02   JALLEE
// Added VID_CONCEPT2 for French concept variant.
//
//    Rev 1.44   26 Mar 1996 16:40:58   SCOURSE
// Added entry for WinLamer
//
//    Rev 1.43   26 Mar 1996 16:00:08   SCOURSE
// Added define for WinSurf
//
//    Rev 1.42   26 Mar 1996 15:32:58   SCOURSE
// Changed VID for WinTiny
//
//    Rev 1.41   26 Mar 1996 14:44:48   SCOURSE
// Added stuff for Wintiny
//
//    Rev 1.40   19 Mar 1996 14:31:00   JALLEE
// Added VID_GRIPE for gripe.2040 detection.
//
//    Rev 1.39   19 Mar 1996 13:56:52   JALLEE
// Added ID_GRIPE for gripe.2040 file detection.
//
//    Rev 1.38   11 Mar 1996 20:07:24   JWILBER
// Added ID_JACKAL_F, for Jackal file repair.
//
//    Rev 1.37   08 Mar 1996 20:53:02   JWILBER
// Added #defines for Vagabond file detection/repair: VID_VAGABOND, and
// ID_VAGABOND.
//
//    Rev 1.36   06 Mar 1996 10:57:02   JWILBER
// Added stuff to handle partition tables.
//
//    Rev 1.35   05 Mar 1996 14:58:30   JALLEE
// Added ID_MANZON for Manzon repair
//
//    Rev 1.34   29 Feb 1996 16:51:36   JWILBER
// Added constant ID_GODZIL for Godzilla.890 file fix.
//
//    Rev 1.33   28 Feb 1996 12:54:44   JALLEE
// Added constant ID_ETP for esto te pasa mbr repair.
//
//    Rev 1.32   15 Feb 1996 16:03:34   SCOURSE
// Added VID_'s for Twitch, Fear, and Cyberiot
//
//    Rev 1.31   15 Feb 1996 12:23:20   JALLEE
// Added VID_HOT0 and VID_HOT1 for the Hot macro virus.
//
//    Rev 1.30   13 Feb 1996 17:56:58   DCHI
// Added ONEHALF ID and VID.
//
//    Rev 1.29   13 Feb 1996 17:09:52   JSULTON
// Added define VID_WINVIK for Winvik vid 0x1f65
//
//    Rev 1.28   12 Feb 1996 20:02:04   JSULTON
// Added define for Winvir virus.
//
//    Rev 1.27   09 Feb 1996 21:53:58   JWILBER
// Added constants for Unknown.1329 file fix.
//
//    Rev 1.26   08 Feb 1996 10:32:02   DCHI
// Added VID_BOZA and ID_BOZA defines.
//
//    Rev 1.25   06 Feb 1996 16:39:34   DCHI
// Added VID_CONCEPT0 and VID_CONCEPT1, removed VID_CONCEPT.
//
//    Rev 1.24   06 Feb 1996 10:17:16   JALLEE
// Added function id for calling scan of Uruguay 4
//
//    Rev 1.23   06 Feb 1996 10:07:38   JALLEE
// Added scanning constant for Uruguay 4
//
//    Rev 1.22   05 Feb 1996 17:22:42   DCHI
// Added constants for Windows executable scanning.
//
//    Rev 1.21   01 Feb 1996 14:20:14   JALLEE
// Added fucntion id for Cruel boot repair
//
//    Rev 1.20   01 Feb 1996 13:45:28   JWILBER
// Added constants for Ether 1f3e, Predator 1f58, and offsets into the MBR.
//
//    Rev 1.19   29 Jan 1996 10:17:24   JALLEE
// Added ID_YESMILE for Yesmile.5504 repair
//
//    Rev 1.18   19 Jan 1996 14:46:04   JALLEE
// Regressed to rev 1.15 plus ID_LR for little red as implemented in rev 1.17
//
//    Rev 1.15   10 Jan 1996 13:16:30   JALLEE
// Changed vids of NAVEX defs so all Navex defs are in def1f.src
//
//    Rev 1.14   03 Jan 1996 17:11:06   DCHI
// Prototyped RepairOpen and RepairClose functions in here.
//
//    Rev 1.13   12 Dec 1995 16:21:48   JALLEE
// Added constants for detection and repair of individual virues.
// FormatC, Bad_Head, CPW, Die Hard, Zarma, Crazy Boot, Da'Boys.
//
//    Rev 1.12   16 Nov 1995 10:15:54   JALLEE
// Changed Emmie repair function id
//
//    Rev 1.11   15 Nov 1995 12:12:00   JALLEE
// Added file scan function id for Nightfall2
//
//    Rev 1.10   14 Nov 1995 13:51:24   JALLEE
// changed ID_SMEG from 0x0101 to 0x4d53 to allow use of same def in
// virscan.dat for 3.0.8 and 3.0.7 NAV.
//
//    Rev 1.9   10 Nov 1995 12:18:00   JALLEE
// Changed the VID's for Colors, Concept, DMV and Nuclear.
//
//    Rev 1.8   10 Nov 1995 11:15:02   JALLEE
// Added individual VID defines for 4 Macro viruses,
// Colors, Concept, DMV, Nuclear.
//
//    Rev 1.7   10 Nov 1995 10:47:54   JALLEE
// Changed ID_SMEG from 0x0001 to 0x0101 to allow compatibility in virscan.dat
// between NAV 3.0.7 and NAV 3.0.8.
//
//    Rev 1.6   09 Nov 1995 16:54:56   DCHI
// Moved FileOpen mode #define's outside for all platforms.
//
//    Rev 1.5   09 Nov 1995 14:37:22   JALLEE
// Added FileOpen #defines for NLM only
//
//    Rev 1.4   09 Nov 1995 14:25:38   JALLEE
// Moved #defines for scan function ID's to here from navexsf.cpp
// Added #defines for N30 struct aliases used by repair functions.
//
//    Rev 1.3   01 Nov 1995 10:50:22   DCHI
// Now contains prototypes and constants rather than code
// which has been moved to navexshr.cpp.
//
//    Rev 1.2   16 Oct 1995 13:36:48   CNACHEN
// fixed W32 problem with setting wVariable = -1.
//
//    Rev 1.1   16 Oct 1995 10:00:14   JALLEE
// ScanString function added.
//
//    Rev 1.0   13 Oct 1995 13:06:00   DCHI
// Initial revision.
//
//************************************************************************

#ifndef _NAVEXSHR_H

#define _NAVEXSHR_H

// make sure to define our VIDs

#include "navexvid.h"

#if defined(SYM_NLM) || defined(SYM_NTK)

#define SEEK_SET    0
#define SEEK_CUR    1
#define SEEK_END    2

#endif

#define READ        0
#define WRITE       1
#define READ_WRITE  2

// #defines for FileGetDateTime and FileSetDateTime callbacks in NAVEX

#define TIME_LASTWRITE  0
#define TIME_LASTACCESS 4
#define TIME_CREATION   6

// Error Stuff

#define WERROR          0xffff          // WORD error indicator
#define DWERROR         0xffffffff      // DWORD error indicator
#define UERROR          ((UINT) -1)     // UINT error indicator

// Partition Table Stuff

#define PART_TAB_OFF        0x1be   // Offset of partition table
#define MAX_PART_ENTS       4       // Maximum partition table entries
#define CHECK_VAL_OFF       0x1fe   // Offset of 0xaa55 for validity checking
                                    // Also used in boot sectors
#define GOODSEC             0xaa55  // Good boot and MBRs end with this

typedef struct {
    BYTE    status;                 // Bootable (0x80) or not (0x00)
    BYTE    st_head;                // Starting head
    WORD    st_cylsec;              // Starting cylinder/sector value
    BYTE    type;                   // Partition type (FAT12, FAT16, etc.)
    BYTE    en_head;                // Ending head
    WORD    en_cylsec;              // Ending cylinder/sector value
    DWORD   st_abssec;              // Starting absolute sector
    DWORD   sectors;                // Number of sectors in partition
    }   PART_ENT;

#define PART_ENT_SIZE   sizeof(PART_ENT)    // Size of partition table entry

// Boot Sector Offsets
// These are offsets into the boot sector for important bits of data

#define OEM_NAME_OFF        0x03    // OEM Name
#define BYTESPERSEC_OFF     0x0b    // Bytes per Sector
#define SECTORSPERUNIT_OFF  0x0d    // Sectors per Allocation Unit
#define RESERVEDSECTORS_OFF 0x0e    // Reserved Sectors
#define NUM_FATS_OFF        0x10    // Number of FATs
#define NUMROOTDIRENTS_OFF  0x11    // Number of Root Directory Entries
#define OLDLOGVOLSEC_OFF    0x13    // Sectors in Logical Volume (DOS < 4.0)
#define MEDIA_BYTE_OFF      0x15    // Media Descriptor Byte
#define NUMSECSPERFAT_OFF   0x16    // Number of Sectors per FAT
#define SECSPERTRACK_OFF    0x18    // Number of Sectors per Track
#define NUMHEADS_OFF        0x1a    // Number of Heads
#define NUMHIDSECS_OFF      0x1c    // Number of Hidden Sectors
#define LOGVOLSECS_OFF      0x20    // Sectors in Logical Volume (DOS 4.0+)
#define PHYSDRVNUM_OFF      0x24    // Physical Drive Number
#define EXBOOTSIGREC_OFF    0x26    // Extended Boot Signature Record
#define VOLUME_ID_OFF       0x27    // Volume ID
#define VOLUME_LAB_OFF      0x2b    // Volume Label

// ... or you can use this structure at offset 0x0b for floppy BPBs
typedef struct tagFLOPPYBPB
{
    WORD        wBytesPerSector;        // Bytes per sector
    BYTE        bySectorsPerCluster;    // Sectors per cluster
    WORD        wReservedSectors;       // First sector of Fat
    BYTE        byNumberOfFats;         // FAT Copies
    WORD        wMaxRootEntries;        // Maximum entries in Root dir
    WORD        wTotalSectors;          // Total number of sectors
    BYTE        byMediaDescriptor;      // Media descriptor byte
    WORD        wSectorsPerFat;         // Sectors per Fat, 0 = 32 bit FAT
    WORD        wSectorsPerTrack;       // Sectors per Track
    BYTE        bySides;                // Number of sides
} FLOPPYBPB, FAR * LPFLOPPYBPB;

// --------------------------------------
// Drive Types used by NavexGetFloppyType
// --------------------------------------

#define DISK_FLOPPY             0x0010  // Floppy flag
#define DISK_FIXED              0x1000  // HD flag
#define DISK_UNKNOWN            0x8000  // Unknown Flag (0x8010: unknown floppy)

#define DISK_FLOPPY_360K        0x0011  // 360k Floppy / 40 x 9 x 2
#define DISK_FLOPPY_720K        0x0012  // 720k Floppy / 80 x 9 x 2
#define DISK_FLOPPY_12M         0x0013  // 1.2M Floppy / 80 x 15 x 2
#define DISK_FLOPPY_144M        0x0014  // 1.44M Floppy / 80 x 18 x 2
#define DISK_FLOPPY_288M        0x0015  // 2.88M Floppy / 80 x 36 x 2
#define DISK_FLOPPY_160K        0x0016  // 160K Floppy / 40 x 8 x 1
#define DISK_FLOPPY_180K        0x0017  // 180K Floppy / 40 x 9 x 1
#define DISK_FLOPPY_320K        0x0018  // 320K Floppy / 40 x 8 x 2
#define DISK_FLOPPY_640K        0x0019  // 640K Floppy / 80 x 8 x 2
#define DISK_FLOPPY_12M_2       0x001A  // 1.2M Floppy / 80 x 8 x 2 (1k/sector)
#define DISK_FLOPPY_1KDMF       0x001B  // 1.68M Floppy / 80 x 21 x 2
#define DISK_FLOPPY_2KDMF       0x001C  // 1.68M Floppy / 80 x 21 x 2

// ----------------------------------------------------------------
// BPB used by NavexGetFloppyType() and EXTRepairBootDMFGeneric()
// ----------------------------------------------------------------

                                        // -------------
                                        // 360k Floppies
                                        // -------------
#define BPB_360K    { 0x00, 0x02, 0x02, 0x01, 0x00, 0x02, 0x70, 0x00, \
                      0xD0, 0x02, 0xFD, 0x02, 0x00, 0x09, 0x00, 0x02 }

                                        // -------------
                                        // 720k Floppies
                                        // -------------
#define BPB_720K    { 0x00, 0x02, 0x02, 0x01, 0x00, 0x02, 0x70, 0x00, \
                      0xA0, 0x05, 0xF9, 0x03, 0x00, 0x09, 0x00, 0x02 }

                                        // -------------
                                        // 1.2M Floppies
                                        // -------------
#define BPB_12M     { 0x00, 0x02, 0x01, 0x01, 0x00, 0x02, 0xE0, 0x00, \
                      0x60, 0x09, 0xF9, 0x07, 0x00, 0x0F, 0x00, 0x02 }

                                        // --------------
                                        // 1.44M Floppies
                                        // --------------
#define BPB_144M    { 0x00, 0x02, 0x01, 0x01, 0x00, 0x02, 0xE0, 0x00, \
                      0x40, 0x0B, 0xF0, 0x09, 0x00, 0x12, 0x00, 0x02 }

                                        // --------------
                                        // 2.88M Floppies
                                        // --------------
#define BPB_288M    { 0x00, 0x02, 0x02, 0x01, 0x00, 0x02, 0xF0, 0x00, \
                      0x80, 0x16, 0xF0, 0x09, 0x00, 0x12, 0x00, 0x02 }

                                        // -------------
                                        // 160k Floppies
                                        // -------------
#define BPB_160K    { 0x00, 0x02, 0x01, 0x01, 0x00, 0x02, 0x40, 0x00, \
                      0x40, 0x01, 0xFE, 0x01, 0x00, 0x08, 0x00, 0x01 }

                                        // -------------
                                        // 180k Floppies
                                        // -------------
#define BPB_180K    { 0x00, 0x02, 0x01, 0x01, 0x00, 0x02, 0x40, 0x00, \
                      0x68, 0x01, 0xFC, 0x02, 0x00, 0x09, 0x00, 0x01 }

                                        // -------------
                                        // 320k Floppies
                                        // -------------
#define BPB_320K    { 0x00, 0x02, 0x02, 0x01, 0x00, 0x02, 0x70, 0x00, \
                      0x80, 0x02, 0xFF, 0x01, 0x00, 0x08, 0x00, 0x02 }

                                        // -------------
                                        // 640k Floppies
                                        // -------------
#define BPB_640K    { 0x00, 0x02, 0x02, 0x01, 0x00, 0x02, 0x70, 0x00, \
                      0x00, 0x05, 0xFB, 0x02, 0x00, 0x08, 0x00, 0x02 }

                                        // --------------------------
                                        // 1.2M Floppies (1k sectors)
                                        // --------------------------
#define BPB_12M_2   { 0x00, 0x04, 0x01, 0x01, 0x00, 0x02, 0xC0, 0x00, \
                      0xD0, 0x04, 0xFE, 0x02, 0x00, 0x08, 0x00, 0x02 }

                                        // --------------------------------
                                        // 1.68M DMF Floppies (1k clusters)
                                        // --------------------------------
#define BPB_1KDMF   { 0x00, 0x02, 0x02, 0x01, 0x00, 0x02, 0x10, 0x00, \
                      0x20, 0x0D, 0xF0, 0x05, 0x00, 0x15, 0x00, 0x02 }

                                        // --------------------------------
                                        // 1.68M DMF Floppies (2k clusters)
                                        // --------------------------------
#define BPB_2KDMF   { 0x00, 0x02, 0x04, 0x01, 0x00, 0x02, 0x10, 0x00, \
                      0x20, 0x0D, 0xF0, 0x03, 0x00, 0x15, 0x00, 0x02 }


#define BPB_INDEX_360K      0x00
#define BPB_INDEX_720K      0x01
#define BPB_INDEX_12M       0x02
#define BPB_INDEX_144M      0x03
#define BPB_INDEX_288M      0x04
#define BPB_INDEX_160K      0x05
#define BPB_INDEX_180K      0x06
#define BPB_INDEX_320K      0x07
#define BPB_INDEX_640K      0x08
#define BPB_INDEX_12M_2     0x09
#define BPB_INDEX_1KDMF     0x0A
#define BPB_INDEX_2KDMF     0x0B

#define NUM_BPBs            0x0C

#define sBPB                0x10
                                        // -----------------------------
#define BPB_MATCH_THRESHOLD     7       // out of 10 fields causes match
                                        // -----------------------------


// -------------------------------------
// Miscellaneous defines for Boot Repair
// -------------------------------------

                                        // --------------
                                        // JMP 0040 & NOP
                                        // --------------
#define JMP_AND_NOP { 0xEB, 0x3E, 0x90 }
#define sJMP_AND_NOP 0x03

                                        // -------------------
                                        // OEM ID = "SYMANTEC"
                                        // -------------------
#define OEM_ID { "SYMANTEC" }
#define sOEM_ID 0x08

                                        // ----------------------------
                                        // Volume Label = "GENERICBOOT"
                                        // ----------------------------
#define VOLUME_LABEL { "GENERICBOOT" }
#define sVOLUME_LABEL 0x0B



#define NON_BOOTABLE_CODE { 0xFA, 0x33, 0xC0, 0x8E, 0xD0, 0xBC, 0x00, 0x7C, \
                            0x8E, 0xD8, 0x8E, 0xC0, 0xFB, 0xBE, 0x80, 0x7C, \
                            0xE8, 0x06, 0x00, 0x30, 0xE4, 0xCD, 0x16, 0xCD, \
                            0x19, 0xAC, 0x08, 0xC0, 0x74, 0x09, 0xB4, 0x0E, \
                            0xBB, 0x07, 0x00, 0xCD, 0x10, 0xEB, 0xF2, 0xC3 }

#define sNON_BOOTABLE_CODE 0x28

                                        // ----------------------------------
                                        // 0140 FA            CLI
                                        // 0141 33C0          XOR     AX,AX
                                        // 0143 8ED0          MOV     SS,AX
                                        // 0145 BC007C        MOV     SP,7C00
                                        // 0148 8ED8          MOV     DS,AX
                                        // 014A 8EC0          MOV     ES,AX
                                        // 014C FB            STI
                                        // 014D BE807C        MOV     SI,7C80
                                        // 0150 E80600        CALL    0159
                                        // 0153 30E4          XOR     AH,AH
                                        // 0155 CD16          INT     16
                                        // 0157 CD19          INT     19
                                        // 0159 AC            LODSB
                                        // 015A 08C0          OR      AL,AL
                                        // 015C 7409          JZ      0167
                                        // 015E B40E          MOV     AH,0E
                                        // 0160 BB0700        MOV     BX,0007
                                        // 0163 CD10          INT     10
                                        // 0165 EBF2          JMP     0159
                                        // 0167 C3            RET
                                        // ----------------------------------


#define NON_BOOTABLE_TEXT { "\0x13\0x10This is not a bootable diskette.\0x13\0x10Remove it and press a key to restart...\0x13\0x10" }
#define sNON_BOOTABLE_TEXT 0x4D

#define SECTOR_SIZE     512


// Infection Buffer Stuff

#define FSTART_OFF      0               // Offset of FSTART buffer
#define FSTART_LEN      256             // Length of FSTART buffer
#define HEADER_OFF      256             // Offset of file header
#define HEADER_LEN      64              // Length of file header
#define INFBUF_OFF      512             // Offset of infection buffer

// Aliases used by repair for N30 struct members and values.

#ifndef REP_EXEC_CODE

#define REP_EXEC_CODE   20
#define EXEC_CODE       0x20
#define wHardType       wInfSize
#define wHardLoc1       wHostJmp

#endif

// Prototypes for shared functions

// EXTScanDanish arguments:
//
// lpcallback           : Pointer to callback structure
// scanbuffer           : Buffer containing sector to scan
// lpwVID               : Pointer used to return VID of Danish Boot, if found
//
// Returns:
//
//  WORD                : EXTSTATUS_OK                  if no virus found
//                        EXTSTATUS_VIRUS_FOUND         if virus was found
//
//  This is called from EXTScanPartDanish and EXTScanBootDanish, and used to
// check for the Danish Boot virus (of course, you say).  It's the same
// algorithm in both cases, so I put it here to save space, and keep it
// maintainable.

WORD EXTScanDanish(LPCALLBACKREV1 lpcallback,
                    LPBYTE scanbuffer,
                    LPWORD lpwVID);

// EXTScanOneHalf arguments:
//
// lpcallback           : Pointer to callback structure
// scanbuffer           : Buffer containing sector to scan
// lpwVID               : Pointer used to return VID of Danish Boot, if found
//
// Returns:
//
//  WORD                : EXTSTATUS_OK                  if no virus found
//                        EXTSTATUS_VIRUS_FOUND         if virus was found
//
//  This is called from EXTScanPartOneHalf and EXTScanBootOneHalf, and used to
// check for the OneHalf Boot virus or any of the Shin variants.  It's the
// same algorithm in both cases, so I put it here to save space, and keep it
// maintainable.  The function is in DANISH.CPP.

WORD EXTScanOneHalf(LPCALLBACKREV1 lpcallback,  // In DANISH.CPP!!!
                    LPBYTE scanbuffer,
                    LPWORD lpwVID);

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

// RepairFileMoveBytes arguments:
//
// lpCallBack           : Callback for file ops
// hFile                : Handle for file w/ bytes to move
// lpbyWorkBuffer       : Buffer to use for moving bytes
// dwBegRead            : File offset to begin reading
// dwBegWrite           : File offset to begin writing
// dwNumBytes           : Number of bytes to move

extern WORD RepairFileMoveBytes (LPCALLBACKREV1 lpCallBack,
                                    HFILE       hFile,
                                    LPBYTE      lpbyWorkBuffer,
                                    DWORD       dwBegRead,
                                    DWORD       dwBegWrite,
                                    DWORD       dwNumBytes);

// RepairFileMoveBytes arguments:
//
// lpCallBack           : Callback for file ops
// hFile                : Handle for file w/ bytes to move
// lpbyWorkBuffer       : Buffer to use for moving bytes
// dwStartPos           : File offset to begin searching
// dwSrchLen            : Number of bytes to search for string
// lpbySrchBuf          : Buffer containing string to search for
// wBufLen              : Length of string to search for

extern DWORD SearchFileForString(LPCALLBACKREV1 lpCallBack,
                                    HFILE       hFile,
                                    LPBYTE      lpbyWorkBuffer,
                                    DWORD       dwStartPos,
                                    DWORD       dwSrchLen,
                                    LPBYTE      lpbySrchBuf,
                                    WORD        wBufLen);

// ****************************
//
// File Repair shared functions
//
// ****************************

// RepairFileOpen
//
// lpCallBack           : pointer to callback structure for FileOpen, etc.
// lpszFileName         : Name of the file
// lphFile              : Pointer to the file handle
// lpuAttr              : Pointer to file attribute
//
// Returns:
//
//     WORD             : 0            on success
//                      : non-zero     on failure

WORD RepairFileOpen (LPCALLBACKREV1 lpCallBack,
                     LPTSTR lpszFileName,
                     HFILE FAR *lphFile,
                     UINT FAR *lpuAttr);

// RepairFileClose
//
// lpCallBack           : pointer to callback structure for FileOpen, etc.
// lpszFileName         : Name of the file
// hFile                : File handle
// uAttr                : File attribute to set
//
// Returns:
//
//     WORD             : 0            on success
//                      : non-zero     on failure

WORD RepairFileClose (LPCALLBACKREV1 lpCallBack,
                     LPTSTR lpszFileName,
                     HFILE hFile,
                     UINT  uAttr);

// RepairFileTruncate
//
// lpCallBack           : pointer to callback structure for FileOpen, etc.
// hFile                : File handle
// lOffset              : Number of bytes to remove from EOF.
//                        Should be a positive number.
//
// Returns:
//
//     WORD             : 0            on success
//                      : non-zero     on failure

WORD RepairFileTruncate (LPCALLBACKREV1 lpCallBack,
                         HFILE          hFile,
                         long           lOffset);

// GetEntryPtOffset
//
// lpCallBack           : pointer to callback structure for FileSeek, etc.
// hFile                : File Handle of opened file to get entry point
// lpbyBuff             : Buffer to use for reading file header - should hold
//                        at least 0x18 bytes.
// Returns:
//      DWORD           : File offset of program entry point.  This is
//                        assumed to be 0 if the file doesn't begin w/ MZ, ZM,
//                        or 0xe9.      on success
//                      : DWERROR       on failure

DWORD GetEntryPtOffset (LPBYTE          lpbyBuff);

// ----------------
// Floppy functions
// ----------------

BOOL NavexGetFloppyType
(
    LPBYTE              lpbySector,     // [in] Boot sector to anaylze
    LPWORD              lpwDiskType     // [out] Disk type
);


BOOL NavexRepairBootGeneric
(
    LPBYTE  lpbyNewBootSector,          // [in/out] The generic boot sector
    WORD    wDiskType                   // [in] Disk type
);


// ************
//
// Handy Macros
//
// ************

// These save a lot of typing in most circumstances

#define FILEOPEN()  RepairFileOpen(lpCallBack, lpszFileName, &hFile, &uAttr)
#define FILECLOSE() RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr)
#define FILEREAD(b, s)  lpCallBack->FileRead(hFile, b, s)
#define FILEWRITE(b, s) lpCallBack->FileWrite(hFile, b, s)
#define FILESEEK(p) lpCallBack->FileSeek(hFile, p, SEEK_SET)
#define FILESIZE()  lpCallBack->FileSize(hFile)
#define FILETRUNCATE(t) RepairFileTruncate(lpCallBack, hFile, t)
#define SEEK_TOF()  lpCallBack->FileSeek(hFile, 0, SEEK_SET)

// Very handy for error returns when file is already open

#define CLOSEOUT(rtarg)     { FILECLOSE(); return(rtarg); }

// EXE Header Stuff

#define EXEHDRSIG   0x4d5a
#define RDHEXESIG   0x5a4d

#define ISEXEHDR(b) ((EXEHDRSIG == AVDEREF_WORD(b)) || \
                                            (RDHEXESIG == AVDEREF_WORD(b)))

// Pack stuff

#ifdef __MACINTOSH__
    #define PACK1       options align=packed
    #define PACK        options align=reset
#else
    #define PACK1       pack(1)
    #define PACK        pack()
#endif

// Special fix for NTK and VXD

#if defined(SYM_NTK) || defined(SYM_VXD)

#if defined(AnsiNext)
#undef AnsiNext
#endif

#if defined(AnsiPrev)
#undef AnsiPrev
#endif

#endif // #if defined(SYM_NTK) || defined(SYM_VXD)

// AnsiNext stuff - from old version of platform.h

#if !defined(SYM_WIN) || defined(SYM_NTK)       // ##DBCS - don't redefine

#if !defined(AnsiNext)
#   define AnsiNext(lp)      ((*((LPTSTR)lp)!='\0') ? (((LPTSTR)lp)+1) : ((LPTSTR)lp))
#   define AnsiPrev(lpS,lpC) (((LPTSTR)lpC>(LPTSTR)lpS) ? (((LPTSTR)lpC)-1) : ((LPTSTR)lpS))
#endif

#endif

HFILE GetTempFile
(
    LPCALLBACKREV1      lpstCallBack,
    LPTSTR              lpszFileName
);

#endif  // _NAVEXSHR_H
