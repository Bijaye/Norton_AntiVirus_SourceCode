//************************************************************************
//
// $Header:   S:/NAVEX/VCS/navexrf.cpv   1.317   25 Apr 1997 17:23:00   AOONWAL  $
//
// Description:
//      Contains NAVEX EXTRepairFile code.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/navexrf.cpv  $
// 
//    Rev 1.317   25 Apr 1997 17:23:00   AOONWAL
// Modified during MAY97 update
// 
//    Rev MAY97   24 Apr 1997 17:47:32   AOONWAL
// Modified Weed.5850 function to work for other variant of Weed
// 
//    Rev MAY97   23 Apr 1997 16:51:20   RELNITI
// FIX an error in Weed.5860 repair
//
//    Rev MAY97   23 Apr 1997 19:34:12   AOONWAL
// ADD Weed.5860 repair to be included w/ this build
// 
//    Rev 1.315   03 Apr 1997 10:43:48   DCHI
// Removed necessity of adding case statement for each macro repair.
// 
//    Rev 1.314   03 Apr 1997 09:32:36   RELNITI
// ADD VID_RAPIV2, VID_JOHNNYF
// 
//    Rev 1.313   03 Apr 1997 08:25:10   RELNITI
// ADD VID_NPADAK
// 
//    Rev 1.312   02 Apr 1997 19:00:00   AOONWAL
// Modified Yesmile repair to work for TWNO.5248. This repair can now be used
// for any EXE PREPEND virus. Also added (LPN30 structure) parameter to all
// the file repair functions
// 
//    Rev 1.311   02 Apr 1997 14:12:16   ECHIEN
// Added case for PAYCHECKA
// 
//    Rev 1.310   02 Apr 1997 13:17:10   ECHIEN
// Added VID_NPADAJ 0x04f9A
// 
//    Rev 1.309   02 Apr 1997 13:02:52   DANG
// Added WM.Swlabs.B for CI#11353
// 
//    Rev 1.308   02 Apr 1997 12:11:06   RELNITI
// ADD VID_SHOWOFFW
// 
//    Rev 1.307   02 Apr 1997 11:45:02   CFORMUL
// Added case label for ShowOff.V repair
// 
//    Rev 1.306   02 Apr 1997 09:42:56   RELNITI
// ADD VID_RAPIU2, VID_BANDUNGQ, VID_ERASERJ, VID_ATOMJ
// 
//    Rev 1.305   01 Apr 1997 21:08:24   RELNITI
// ADD VID_COLORSAE 0x4f92
// 
//    Rev 1.304   01 Apr 1997 16:46:26   ECHIEN
// Added case for BANDUNGREM
// 
//    Rev 1.303   01 Apr 1997 11:45:14   ECHIEN
// Added case label VID_JOHNNYE
// 
//    Rev 1.302   01 Apr 1997 09:54:14   ECHIEN
// Added case VID_COLORSAD
// 
//    Rev 1.301   28 Mar 1997 12:32:38   CFORMUL
// Added case label for Colors.AC reapir
// 
// 
//    Rev 1.300   28 Mar 1997 12:03:24   AOONWAL
// Added case label VID_HELPERB
// 
//    Rev 1.299   28 Mar 1997 11:18:20   ECHIEN
// Added case for VID_SHOWOFFT and VID_SHOWOFFU  Cust# 11330
// 
//    Rev 1.298   27 Mar 1997 23:13:16   RELNITI
// RENAME duplicate VID_CONCEPTAB => VID_CONCEPTAC
// 
//    Rev 1.297   27 Mar 1997 23:04:08   RELNITI
// ADD VID_NpadAF
//     VID_ColorsAA, AB,
//     VID_ConceptAB, AD,
//     VID_WazzuBK, BJ, BL, BM, BN, BO
// 
// 
//    Rev 1.296   27 Mar 1997 20:42:56   RELNITI
// ADD VID_SHOWOFFS
// 
//    Rev 1.295   27 Mar 1997 20:15:58   ECHIEN
// Deleted case for Colors.AA  Colors.AA => Modified ColorsE
// 
//    Rev 1.294   27 Mar 1997 19:30:30   ECHIEN
// Removed case for Colors.AB it will use ColorsB remnant instead
// 
//    Rev 1.293   27 Mar 1997 18:39:42   CFORMUL
// Added case label for Npad.AI repair
// 
//    Rev 1.292   27 Mar 1997 18:21:42   ECHIEN
// Added case for VID_COLORSAB 0x4f7f
// 
//    Rev 1.291   27 Mar 1997 17:15:16   ECHIEN
// Added case for VID_COLORSAA 0x4f7e
// 
//    Rev 1.290   27 Mar 1997 12:55:38   RELNITI
// ADD VID_ERASERE
// 
//    Rev 1.289   26 Mar 1997 14:42:00   AOONWAL
// Added case label VID_CONCEPTAF
// 
//    Rev 1.288   26 Mar 1997 12:21:06   DANG
// Added Rapi.T - T2 for CI#11296
// 
//    Rev 1.287   26 Mar 1997 11:36:40   RELNITI
// ADD VID_CONCEPTAE
// 
//    Rev 1.286   26 Mar 1997 11:03:50   ECHIEN
// Added case for MUCKD 0x4f77  cust# 11310
// 
//    Rev 1.285   25 Mar 1997 17:46:32   DANG
// Added Johhny.D for CI#11293
// 
//    Rev 1.284   25 Mar 1997 17:31:52   CFORMUL
// Added case label for Npad.AH repair
// 
//    Rev 1.283   25 Mar 1997 16:20:12   DANG
// Added NPad.AG for CI#11299
// 
//    Rev 1.282   25 Mar 1997 15:18:12   ECHIEN
// Added case for WM.COLORSZ
// 
//    Rev 1.281   25 Mar 1997 11:54:44   AOONWAL
// Added case label VID_COLORSY
// 
//    Rev 1.280   25 Mar 1997 11:01:26   AOONWAL
// Modified during APRIL 97 build
// 
//    Rev APRIL97 24 Mar 1997 18:09:18   CFORMUL
// Added case label for Rapi.S2 repair
// 
// 
//    Rev APRIL97 20 Mar 1997 17:37:44   DANG
// Added Npad.AE for CI#11251
// 
//    Rev APRIL97 20 Mar 1997 16:04:10   DANG
// Added Wazzu.BI for CI#11215
// 
//    Rev APRIL97 20 Mar 1997 15:20:00   AOONWAL
// Added WM.Concept.AB
// 
//    Rev APRIL97 20 Mar 1997 14:03:26   RELNITI
// ADD VID_IRISHG
// MODIFY VID_NPADAA => VID_NPADAC
// 
//    Rev APRIL97 20 Mar 1997 13:57:34   CFORMUL
// Added case label for Npad.AD repair
// 
// 
//    Rev APRIL97 20 Mar 1997 12:23:30   DANG
// Added WM.NJ-WMDLK1.E for CI#11247
// 
//    Rev APRIL97 19 Mar 1997 21:20:52   JWILBER
// Enabled repair for MVDK-Generated samples.
//
//    Rev APRIL97 19 Mar 1997 18:58:04   AOONWAL
// Added case label for VID_RAPIR
//
//    Rev APRIL97 19 Mar 1997 17:56:20   JWILBER
// Enabled repair for Npad.AA, 4f67.
//
//    Rev APRIL97 19 Mar 1997 16:58:18   DCHI
// Added MVP support.
//
//    Rev APRIL97 19 Mar 1997 11:49:22   RELNITI
// ADD VID_COLORSX (4f66)
//
//    Rev APRIL97 18 Mar 1997 18:29:10   AOONWAL
// Changed case label VID_CONCEPTAB to VID_SURABAYAA
//
//    Rev APRIL97 18 Mar 1997 16:49:40   CFORMUL
// Added case label for BANDUNGP repair
//
//    Rev APRIL97 18 Mar 1997 01:41:38   AOONWAL
// Added CASE VID_CONCEPTAB
//
//    Rev APRIL97 17 Mar 1997 21:35:28   DANG
// Added Irish.F for CI#11206
//
//    Rev 1.279   17 Mar 1997 15:52:28   DANG
// Added Hybrid.C for CI#11205
//
//    Rev 1.278   14 Mar 1997 20:08:44   RELNITI
// ADD VID_WAZZUBF
//     VID_WAZZUBG
//     VID_WAZZUBH
//
//    Rev 1.277   14 Mar 1997 18:34:24   RELNITI
// ADD VID_COLORSW (4f5e)
// RENAME 1EB0 VID_COLORSI => VID_MSWA to reflect name-changing
//
//    Rev 1.276   14 Mar 1997 13:54:22   AOONWAL
// Added case VID_COLORSH
//
//    Rev 1.275   14 Mar 1997 13:06:28   DCHI
// Fixed problem with Office 97 repair not being called for Excel 97 docs.
//
//    Rev 1.274   14 Mar 1997 10:00:42   RELNITI
// ADD VID_RAPIQ1, VID_RAPIQ2
//
//    Rev 1.273   13 Mar 1997 21:58:58   RELNITI
// ADD VID_NPADN, VID_NPADAB
//
//    Rev 1.272   13 Mar 1997 21:18:06   RELNITI
// MODIFY VID_NPADN => VID_NPADP
//        VID_NPADM => VID_NPADO
//
//    Rev 1.271   13 Mar 1997 17:18:12   RELNITI
// ADD case VID_COLORSV
//
//    Rev 1.270   13 Mar 1997 14:53:04   AOONWAL
// Added case VID_SHOWOFFL
//
//    Rev 1.269   13 Mar 1997 10:08:32   AOONWAL
// Added case label for VID_SMILEYA
//
//    Rev 1.268   12 Mar 1997 16:40:08   JWILBER
// Added case statement to enable repair for Irish.E.
//
//    Rev 1.267   12 Mar 1997 15:43:08   AOONWAL
// Modified case labels for WM.NightShade.A and WM.Minimal.C
//
//    Rev 1.266   12 Mar 1997 14:12:08   AOONWAL
// Added
// case VID_NIGHTSHADE
// case VID_BONTCHEV
//
//    Rev 1.265   11 Mar 1997 20:34:44   RELNITI
// REMOVE case VID_COLORSV
//
//    Rev 1.264   11 Mar 1997 19:29:36   RELNITI
// ADD VID_COLORSBREMN
//
//    Rev 1.263   11 Mar 1997 15:40:48   RELNITI
// ADD VID_COLORSU, VID_COLORSV
//
//    Rev 1.262   11 Mar 1997 14:47:28   CFORMUL
// Added Case labels for Eraser.B, C, and D repairs
//
//    Rev 1.261   11 Mar 1997 10:40:00   RELNITI
// ADD VID_WAZZUBE, VID_WAZZUAF, VID_PHARDERAD, VID_FIREA
// CHANGE 0x4f36: VID_FINDZS => VID_LOOKD
//
//    Rev 1.260   10 Mar 1997 20:00:58   RELNITI
// ADD VID_COLORSP, VID_COLORSQ, VID_COLORSR, VID_JOHNNYC,
//     VID_SHOWOFFH, VID_SHOWOFFI
//
//    Rev 1.259   10 Mar 1997 18:06:00   JWILBER
// Added case label to enable repair for Bandung.O.
//
//    Rev 1.258   10 Mar 1997 14:01:36   JWILBER
// Added VID_COLORST to enable repair.
//
//    Rev 1.257   07 Mar 1997 16:54:26   RELNITI
// ADD VID_COLORSS
//
//    Rev 1.256   07 Mar 1997 12:01:56   DANG
// Added ShowOff.J CI#11087
//
//    Rev 1.255   06 Mar 1997 19:40:40   RELNITI
// ADD VID_ERASERA, VID_MINIMALD, VID_BABYA, VID_APPDERB,
//  VID_MINDA, VID_SHOWOFFG, VID_NPADZ, VID_APPDERC
//
//    Rev 1.254   06 Mar 1997 17:39:08   DANG
// Added Irish.D.  CI#11084
//
//    Rev 1.253   06 Mar 1997 14:37:28   RELNITI
// ADD VID_FINDZS
//
//    Rev 1.252   06 Mar 1997 13:58:36   RELNITI
// ADD VID_WEATHERD, VID_THEATREC, VID_WHY
//
//    Rev 1.251   06 Mar 1997 12:41:40   CFORMUL
// Added case label for XM.Robocop.A repair
//
//    Rev 1.250   06 Mar 1997 12:26:36   AOONWAL
// Added case VID_TWOLINESC
//
//    Rev 1.249   05 Mar 1997 19:47:08   RELNITI
// ADD VID_NPADY
//
//    Rev 1.248   05 Mar 1997 19:08:14   DANG
// Added Rapi.P.  CI#11072
//
//    Rev 1.247   05 Mar 1997 16:42:18   DANG
// Added Rapi.O after finding sample. CI#11055
//
//    Rev 1.246   05 Mar 1997 16:37:30   RELNITI
// ADD VID_COLORSO
//
//    Rev 1.245   05 Mar 1997 16:20:00   CFORMUL
// Added case label for XM.Legend repair
//
//
//    Rev 1.244   05 Mar 1997 15:43:44   DANG
// Added Rapi.O1 and Rapi.O2.  CI#11055  Did not have Rapi.O
//
//    Rev 1.243   05 Mar 1997 14:45:54   CFORMUL
// Added case label for Look.A:Tw repair
//
//    Rev 1.242   05 Mar 1997 13:12:38   AOONWAL
// Added case vid_conceptr
//
//    Rev 1.241   04 Mar 1997 15:19:38   CFORMUL
// Added case label for Theatre.B repair
//
//    Rev 1.240   04 Mar 1997 14:35:58   RELNITI
// ADD VID_SNICKERSA, VID_SWLABS3, VID_VH1, VID_VH1A, VID_PHARDERAC
//
//    Rev 1.239   04 Mar 1997 13:23:28   JWILBER
// Enabled repairs for WM.Rapi.N, N1, and N2, by adding case labels.
//
//    Rev 1.238   04 Mar 1997 11:45:54   RELNITI
// ADD VID_CAP_DAMAGED
//
//    Rev 1.237   28 Feb 1997 17:33:46   CFORMUL
// Added case label for Weather.A repair
//
//    Rev 1.236   28 Feb 1997 15:58:38   RELNITI
// ADD  VID_WAZZUAY, VID_WAZZUAZ, VID_WAZZUBA, VID_NPADU
//  VID_NUCLEARG, VID_NUCLEARH, VID_DIVINAD,
//  VID_ALIENC, VID_NICEDAYC,VID_NO_FA,VID_SHOWOFFF,VID_CLOCKF
//
//    Rev 1.235   28 Feb 1997 11:47:50   RELNITI
// MODIFY VID_RAPIR2 => VID_RAPIM2
//
//    Rev 1.234   27 Feb 1997 15:24:36   RELNITI
// ADD VID_BANDUNGN
//
//    Rev 1.233   27 Feb 1997 11:00:02   AOONWAL
// Changed WAZZUAY to WAZZUBB
//
//    Rev 1.232   26 Feb 1997 22:27:36   JWILBER
// Added case for VID_MUCKC.
//
//    Rev 1.231   26 Feb 1997 18:38:04   RELNITI
// ADD case VID_ANTICONCEPTA1, VID_CLOCKE, VID_CONCEPTAA, VID_DOGGIEB,
//     VID_MUCKA, VID_MUCKB
//
//    Rev 1.230   26 Feb 1997 18:24:14   CFORMUL
// Updated source after sharefun build
//
//    Rev 1.228   26 Feb 1997 15:36:44   AOONWAL
// Added case label VID_WAZZUAY
//
//    Rev 1.227   26 Feb 1997 14:12:46   CFORMUL
// Added case label for bandung.m repair
//
//    Rev 1.226   26 Feb 1997 12:57:30   JWILBER
// Added casts in the Weed repair to avoid type conversion warnings
// when we build.
//
//    Rev 1.225   25 Feb 1997 19:08:54   RELNITI
// CHANGE VID_WAZZUAY => VID_WAZZUN
// ADD VID_WAZZUAX
//
//    Rev 1.224   25 Feb 1997 17:32:10   JWILBER
// Added VIDs to enable repair of Concept.Damaged, and Concept.Remnant.
//
//    Rev 1.223   25 Feb 1997 17:05:56   AOONWAL
// Added case label VID_DZTB
//
//    Rev 1.222   25 Feb 1997 16:47:48   RELNITI
// CHANGE VID_RAPIK1, K2 => I1, I2
// ADD VID_RAPIL, L1, L2
//
//    Rev 1.221   25 Feb 1997 16:38:30   CFORMUL
// Added VID_WAZZUU
//
//    Rev 1.220   25 Feb 1997 13:52:34   AOONWAL
// Added case label for WM.Wazzu.Variant repair
//
//    Rev 1.219   25 Feb 1997 13:46:36   RELNITI
// ADD case VID_COLORSN, VID_WAZZUAY
//
//    Rev 1.218   25 Feb 1997 12:15:18   AOONWAL
// Modified during MAR97 build
//
//    Rev MAR97   25 Feb 1997 10:15:12   RELNITI
// ADD case VID_NPADX
//
//    Rev MAR97   24 Feb 1997 20:49:26   DANG
// Added repair for Weed v1.1  (WEED.4080)
//
//    Rev MAR97   24 Feb 1997 18:12:38   AOONWAL
// Added case label for WM.Cap.B
//
//    Rev MAR97   24 Feb 1997 14:44:18   CFORMUL
// Added case label for ShareFun.A
//
//    Rev MAR97   24 Feb 1997 13:27:16   CFORMUL
// Added case label for NOP.E
//
//    Rev MAR97   21 Feb 1997 18:32:58   RELNITI
// CHANGE VID_WAZZUQQ => VID_WAZZUAU
//
//    Rev MAR97   21 Feb 1997 18:05:48   RELNITI
// ADD case VID_WAZZUM, VID_WAZZUAV, VID_WAZZUAW
//
//    Rev MAR97   21 Feb 1997 16:40:34   RELNITI
// ADD case VID_BANDUNGL, VID_COLORSM
//
//    Rev MAR97   21 Feb 1997 14:35:46   RELNITI
// CHANGE VID_NPADU => VID_NPADV
// ADD case VID_NPADW
//
//    Rev MAR97   21 Feb 1997 11:30:30   RELNITI
// ADD case for VID_KERRANG, VID_NPADR, VID_NPADS
//
//    Rev MAR97   21 Feb 1997 11:03:56   AOONWAL
// Added case VID_BOXB
//
//    Rev MAR97   20 Feb 1997 18:57:06   RELNITI
// ADD case for VID_RAPIK, VID_RAPIK1, VID_RAPIK2
//
//    Rev MAR97   20 Feb 1997 15:42:46   JWILBER
// Added case VID_WAZZUAT.
//
//    Rev MAR97   20 Feb 1997 13:53:16   JWILBER
// Changed VID_SOCIAL to VID_CAPA in repair enabling, because of name
// change.
//
//    Rev MAR97   20 Feb 1997 12:34:20   RELNITI
// ADD case VID_GABLE
//
//    Rev MAR97   20 Feb 1997 12:28:16   CFORMUL
// Changed VID_NPADO to VID_NPADT
//         VID_NPADP to VID_NPADU
//
//    Rev MAR97   19 Feb 1997 22:19:58   RELNITI
// ADD case VID_NOMVIRA, VID_NOMVIRB
//
//    Rev MAR97   19 Feb 1997 21:32:18   RELNITI
// ADD case for VID_HYBRIDB, VID_KILLPROTA, VID_SHOWOFFE
//
//    Rev MAR97   19 Feb 1997 20:45:56   CFORMUL
// Added case label for Npad.P
//
//    Rev MAR97   19 Feb 1997 20:43:48   RELNITI
// ADD cases for VID_COLORSJ2, VID_COLORSK
//
//    Rev MAR97   19 Feb 1997 17:19:28   CFORMUL
// Added case label for Npad.O repair
//
//    Rev MAR97   19 Feb 1997 13:43:42   CFORMUL
// Added case label for Rapi.J1
//
//    Rev MAR97   18 Feb 1997 21:38:00   CFORMUL
// Added case label for Bandung.K
//
//    Rev MAR97   18 Feb 1997 19:57:20   AOONWAL
// Added case VID_TWNOK
//
//    Rev MAR97   18 Feb 1997 15:12:20   DANG
// Pulled Wazzu.AT.  For some reason, Wazzu.Q's CRC did not show up
// when I searched for, it.  But scanning, it was detected as Wazzu.Q
// Same CRC.  Oh well.
//
//    Rev MAR97   18 Feb 1997 13:50:38   DANG
// Added Wazzu.AT CI10809
//
//    Rev MAR97   18 Feb 1997 13:03:34   CFORMUL
// Added case label for Colors.J repair
//
//    Rev MAR97   14 Feb 1997 19:42:22   AOONWAL
// Added case label for VID_WAZZUAS
//
//    Rev MAR97   14 Feb 1997 17:20:58   JWILBER
// Finished detection for Manuel.2209.
//
//    Rev MAR97   14 Feb 1997 15:08:28   AOONWAL
// Added case label for WAZZUAR
//
//    Rev MAR97   14 Feb 1997 14:58:56   AOONWAL
// Added two variant of WAZZU
//
//    Rev MAR97   14 Feb 1997 14:53:36   CFORMUL
// Changed dwOrigLen to dwFileLen
//
//    Rev MAR97   14 Feb 1997 14:48:48   CFORMUL
// Added case label for Wazzu.p repair
//
//    Rev MAR97   14 Feb 1997 14:47:42   JWILBER
// Added repair for Manuel.2209.
//
//    Rev MAR97   13 Feb 1997 23:15:48   JWILBER
// Pulled prototypes for Manuel, until I can add the real functions
// tomorrow.
//
//    Rev 1.215   13 Feb 1997 05:15:24   AOONWAL
// Update newvir from defwork
//
//    Rev 1.214   13 Feb 1997 15:34:42   JWILBER
// Added prototypes for EXTRepairFileManuel, to be added later.
//
//    Rev 1.213   11 Feb 1997 20:01:16   AOONWAL
// Added case label for WAZZU.AO
//
//    Rev 1.212   11 Feb 1997 18:47:20   RELNITI
// ADD  case for VID_RAPIR2, VID_BADBOYB, VID_BANDUNGH, VID_BANDUNGI,
//      VID_BANDUNGJ
//
//    Rev 1.211   11 Feb 1997 17:02:54   CFORMUL
// Added case label for Wazzu.AG repair
//
//    Rev 1.210   10 Feb 1997 13:16:08   CFORMUL
// Added case label for Rapi.J repair
//
//    Rev 1.209   07 Feb 1997 15:02:06   CFORMUL
// Added case lables for Run.B and Rapi.I
//
//    Rev 1.208   06 Feb 1997 15:59:00   CFORMUL
// Removed VID_WAZZUAO because it's the same as WAZZUAI
//
//    Rev 1.207   06 Feb 1997 15:38:22   CFORMUL
// Added case label for WM.Wazzu.AO
//
//    Rev 1.206   06 Feb 1997 15:29:12   AOONWAL
// Added case label for VID_NPADMM
//
//    Rev 1.205   05 Feb 1997 19:46:20   JWILBER
// Enabled repair for WM.Social, 1ead.
//
//    Rev 1.204   04 Feb 1997 19:03:16   RELNITI
// REMOVE DUPLICATES: VID_WAZZUU, VID_UNKNOWNMACRO
//
//    Rev 1.203   04 Feb 1997 18:29:04   AOONWAL
// Added case label for VID_NAPDQ
//
//    Rev 1.202   04 Feb 1997 12:57:48   AOONWAL
// Added repair for WM.TWNO.G:TW
//
//    Rev 1.201   03 Feb 1997 21:23:38   CFORMUL
// Added Case label for WM.GoodNight repair
//
//    Rev 1.200   03 Feb 1997 15:03:08   JWILBER
// Enabled repair for WM.Npad.N, 1eae.
//
//    Rev 1.199   31 Jan 1997 21:31:44   RELNITI
// ADD case for VID_COOLIO2, VID_MISTERX2
//
//    Rev 1.198   31 Jan 1997 19:06:16   RELNITI
// ADD case for VID_RAPIA, VID_RAPIA1, VID_RAPIC, VID_RAPIC1
//
//    Rev 1.197   31 Jan 1997 14:57:02   AOONWAL
// Added "case VID_RUN"
//
//    Rev 1.196   30 Jan 1997 18:29:16   RELNITI
// ADD case VID_WAZZUC
//
//    Rev 1.195   30 Jan 1997 16:19:12   RELNITI
// ADD case VID_CONCEPTZ
//
//    Rev 1.194   30 Jan 1997 15:16:04   RELNITI
// CORRECTING bad VIDs NJ- => NJ_
//
//    Rev 1.193   30 Jan 1997 15:03:50   RELNITI
// CORRECTION on typos & duplicates:
// VID_CONCEPTC => should have been VID_BANDUNGC
// WAZZUQ => WAZZUQQ to distinguish from WAZZUQ
// removed: NUCLEARC, ATOMB, TWISTER, BANDUNGB
//
//    Rev 1.192   30 Jan 1997 14:25:38   RELNITI
// ADD case for
// VID_BANDUNGG, VID_CONCEPTY
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
//    Rev 1.191   30 Jan 1997 11:56:10   CFORMUL
// Added repair for Quartz
//
//    Rev 1.190   29 Jan 1997 18:15:40   JWILBER
// Enabled repairs for Atom.C, Bandung.A, and Wazzu.I macro viruses.
//
//    Rev 1.189   29 Jan 1997 14:19:14   AOONWAL
// Changed case label TWNOE to TWNOH
//
//    Rev 1.188   29 Jan 1997 14:09:28   DANG
// Added Rapi.H2
//
//    Rev 1.187   29 Jan 1997 12:22:04   AOONWAL
// Added case label for 0x1e42
//
//    Rev 1.186   28 Jan 1997 16:17:58   AOONWAL
// Modified during FEB97 build
//
//    Rev FEB97   28 Jan 1997 11:12:38   RELNITI
// No change.
//
//    Rev FEB97   28 Jan 1997 11:01:04   RELNITI
// ADD case for VID_MINIMALB, VID_RATSB, VID_RATSC, VID_TWNOG
//
//    Rev FEB97   27 Jan 1997 18:51:48   RELNITI
// ADD case for VID_NPADM (for the old WM.Npad.K, see notes on virscan entry)
//  & VID_CLOCKBDE
//
//    Rev FEB97   27 Jan 1997 18:15:18   RELNITI
//     navexrf.cpp -> S:\NAVEX\VCS\navexrf.cpv
// Enter description of changes.
// ADD case for VID_ATOMH, VID_CEEFOUR, VID_CONCEPTW, VID_CONCEPTX, VID_DZTA,
// VID_IRISHB, VID_KOMPUA, VID_NICEDAYB, VID_RATSA, VID_SHOWOFFB,
// VID_SHOWOFFC, VID_SMILEYBDE, VID_WAZZUAJ, VID_WAZZUAK, VID_WAZZUAL,
// VID_WAZZUAM, VID_WAZZUAN
//
//    Rev FEB97   24 Jan 1997 18:05:02   RELNITI
// ADD case for VID_EPIDEMIC, VID_NIKITA, VID_NJWMDLK1D, VID_NOPD2,
// VID_SWLABS1, & VID_RAPIG
//
//    Rev FEB97   23 Jan 1997 15:51:36   RELNITI
// ADD case for VID_NFA, VID_BOOMB, VID_NOPD, & VID_TWNOF
//
//    Rev FEB97   23 Jan 1997 12:26:38   RELNITI
// ADD case for VID_BANDUNGE
//
//    Rev FEB97   22 Jan 1997 13:18:20   CFORMUL
// Added case label for Concept.V repair
//
//    Rev FEB97   21 Jan 1997 17:05:30   AOONWAL
// Added case label for NPADL repair
//
//    Rev FEB97   21 Jan 1997 13:59:46   RELNITI
// ADD case for Wazzu.AH, Dark.A and NF.B .... thanks Formy!
//
//    Rev FEB97   21 Jan 1997 13:54:28   CFORMUL
// Added case label for Wazzu.AI repair
//
//    Rev 1.184   17 Jan 1997 19:36:52   AOONWAL
// Modified name changes to few VID defines to avoid name conflict
//
//    Rev 1.183   17 Jan 1997 17:29:44   AOONWAL
// Added case labels for Macro repairs. This work was initially done
// in DEFWORK
//
//    Rev 1.182   16 Jan 1997 19:07:34   JWILBER
// Added repair for Tentatrickle.
//
//    Rev 1.181   15 Jan 1997 15:28:46   CFORMUL
// Modified HLLT.4416 repair so it doesn't allocate too much memory.
//
//    Rev 1.180   09 Jan 1997 16:41:48   CFORMUL
// Added case label for XM.Delta repair
//
//    Rev 1.179   08 Jan 1997 20:22:20   DANG
// Pulled NPad.K since NPad.J detects it.  New engine will have new def
//
//    Rev 1.178   08 Jan 1997 18:08:08   DANG
// Renamed Bandung.G to NPad.K
//
//    Rev 1.177   08 Jan 1997 16:49:34   DANG
// Added 1ED1.  Bandung.G macro virus
//
//    Rev 1.176   03 Jan 1997 18:19:34   AOONWAL
// Added case label for Npad.J
//
//    Rev 1.175   03 Jan 1997 12:40:04   RELNITI
// Add case for WM.Wazzu.X, VID=0x1e16, Customer #10496
//
//    Rev 1.174   31 Dec 1996 18:02:58   CFORMUL
// Added repair for Weed.3263, ifdef'd out for dos
//
//    Rev 1.173   26 Dec 1996 15:19:38   AOONWAL
// Modified during JAN97 build
//
//    Rev JAN97   26 Dec 1996 15:13:34   AOONWAL
// Following virus have been ifdef'd out for DOS
// FOURELEVEN
// VAGABOND
// WEREWOLF
//
//    Rev JAN97   19 Dec 1996 20:02:46   CFORMUL
// Added case label for Npad.H repair
//
//    Rev 1.168   13 Dec 1996 17:34:02   AOONWAL
// Added case label for WM.Appder repair
//
//    Rev 1.167   12 Dec 1996 16:09:34   CRENERT
// Commented in Johnny.
//
//    Rev 1.166   12 Dec 1996 12:57:08   CFORMUL
// Added case label for WM.Wazzu.Y repair
//
//    Rev 1.165   10 Dec 1996 16:13:06   AOONWAL
// Added pointer to call WInTpvo repair
//
//    Rev 1.164   10 Dec 1996 15:45:58   CFORMUL
// Added case label for WM.MDMA.C repair
//
//    Rev 1.163   10 Dec 1996 13:02:14   CFORMUL
// added Case label for WM.Nuclear.E repair
//
//    Rev 1.162   06 Dec 1996 19:38:22   RELNITI
// removing WM.Johnny temporarily
//
//    Rev 1.161   06 Dec 1996 19:25:52   RELNITI
// adding case VID_JOHNNY0 (0x1e0b)
//
//    Rev 1.160   06 Dec 1996 15:25:04   CFORMUL
// Added case label for XM.Sofa repair
//
//    Rev 1.159   05 Dec 1996 15:19:50   JWILBER
// Enabled repair for WM.Divina.C, 1e0e.
//
//    Rev 1.158   05 Dec 1996 12:29:44   CFORMUL
// Added case label for WM.Npad.E repair
//
//    Rev 1.157   04 Dec 1996 20:23:12   CFORMUL
// Added case label for XM.Laroux.B repair
//
//    Rev 1.156   04 Dec 1996 15:57:34   CFORMUL
// Added repair for Hoot.4676, ifdef'd out for dos.
//
//    Rev 1.155   04 Dec 1996 12:59:44   JWILBER
// Added repair for Batman II, 1f00.
//
//    Rev 1.154   02 Dec 1996 19:11:26   AOONWAL
// Added case label for WM.Sutra
//
//    Rev 1.153   02 Dec 1996 18:01:40   CRENERT
// Removed extraneous local variables.
//
//    Rev 1.152   02 Dec 1996 17:33:26   CFORMUL
// Removed asm int 3 that was accidentally left in 411 repair
//
//    Rev 1.151   02 Dec 1996 16:30:52   AOONWAL
// Added case label for 0x1e06
//
//    Rev 1.150   02 Dec 1996 13:46:18   AOONWAL
// DEC96 update
//
//    Rev 1.155   27 Nov 1996 20:53:44   JWILBER
// Added repair for FourEleven.2144, 1e03.
//
//    Rev 1.154   27 Nov 1996 12:47:44   CFORMUL
// Added HLLT.4416 repair
//
//    Rev 1.153   25 Nov 1996 19:50:52   AOONWAL
// Added case label for "WM.Alien.B"
//
//    Rev 1.152   25 Nov 1996 18:38:06   CFORMUL
// Made changed to Burglar.1150 repair so that it does not
// rely on a seek EOF-?? to do repair.
//
//    Rev 1.151   22 Nov 1996 15:50:46   JWILBER
// Enabled repair for Twno.D, 1fff.
//
//    Rev 1.150   20 Nov 1996 11:43:14   AOONWAL
// No change.
//
//    Rev 1.149   19 Nov 1996 12:58:26   CFORMUL
// Added case label for Npad.C repair
//
//    Rev 1.148   19 Nov 1996 12:21:00   CRENERT
// More #if defs for DOS:
//     CPW, Fairz, Genrat, Jackal & Tanpro
//
//    Rev 1.147   15 Nov 1996 23:38:42   JWILBER
// Enabled repair for WM.AntiConcept.
//
//    Rev 1.146   14 Nov 1996 18:52:58   AOONWAL
// Added case label for WM.Twno.A:Tw repair
//
//    Rev 1.145   14 Nov 1996 16:58:50   JWILBER
// Enable repairs for Stryx:De in Normal.dot.
//
//    Rev 1.144   13 Nov 1996 15:04:22   CFORMUL
// Added case label for WM.Lunch.B repair
//
//    Rev 1.143   13 Nov 1996 02:24:20   JWILBER
// Enabled repairs for Look.C, Weather.B, Theatre, and Twno.C & B.
//
//    Rev 1.142   12 Nov 1996 13:38:16   JWILBER
// Enabled repair for WM.Stryx.
//
//    Rev 1.141   07 Nov 1996 20:09:56   JWILBER
// Commented out repairs for DiamondRockSteady (Diamond), Oxana,
// and Unknown because Navex got too big for DOS.  Actually,
// they're only #ifdef'd out for DOS.  They're still there for the
// other platforms.
//
//    Rev 1.140   07 Nov 1996 17:25:54   JWILBER
// Enabled repair for Nuclear.A.Damaged and Npad.
//
//    Rev 1.139   07 Nov 1996 14:11:38   JWILBER
// Added repair for Genrat.785, 1feb.
//
//    Rev 1.138   05 Nov 1996 16:17:36   JWILBER
// Enabled repair for WM.BigDaddy, 1fea.
//
//    Rev 1.137   05 Nov 1996 13:00:28   JWILBER
// Enabled repair for WM.Helper, 1fe8.
//
//    Rev 1.136   04 Nov 1996 18:03:26   CFORMUL
// Added case label for WM.Switches
//
//    Rev 1.135   04 Nov 1996 13:18:26   AOONWAL
// Change data type DWORD to INT
//
//    Rev 1.134   04 Nov 1996 12:25:06   CFORMUL
// Added case label for WM.Rapi
//
//    Rev 1.133   01 Nov 1996 18:26:32   AOONWAL
// Change data type DWORD from WORD in karnivali.1971 and y-key.2346 repair
//
//    Rev 1.132   01 Nov 1996 11:47:48   AOONWAL
// Added comments to Y-key.2346 and Karnivali.1971 repairs
//
//    Rev 1.131   31 Oct 1996 19:28:12   AOONWAL
// Added shared code to do "Y-key" and "Karnivali" EXE file repair.
//
//    Rev 1.130   31 Oct 1996 16:55:32   CFORMUL
// Added case label for WM.Alien repair
//
//    Rev 1.129   31 Oct 1996 15:09:26   CFORMUL
// Added case label for WM.Colors.C repair
//
//    Rev 1.128   30 Oct 1996 16:58:12   CFORMUL
// Added case label for WM.Concept.K repair
//
//    Rev 1.127   29 Oct 1996 14:27:08   CFORMUL
// Added case label for WM.Wazzu.F
//
//    Rev 1.126   29 Oct 1996 12:57:22   AOONWAL
// Modified during NOV96 freeze
//
//    Rev 1.129   25 Oct 1996 12:16:54   CFORMUL
// Added case label for WM.Wazzu.E repair
//
//    Rev 1.128   24 Oct 1996 14:51:08   CFORMUL
// Added case label for WM.Divina.B
//
//    Rev 1.127   24 Oct 1996 14:01:32   CFORMUL
// Added case label for WM.Wazzu.D repair
//
//    Rev 1.126   21 Oct 1996 17:01:24   CFORMUL
// Added case label for WM.Jakarta.b repair
//
//    Rev 1.125   18 Oct 1996 18:19:40   AOONWAL
// Added case label for "WM.Concept.J"
//
//    Rev 1.124   17 Oct 1996 17:57:06   AOONWAL
// Removed case label for "WM.Taiwan1" repair
//
//    Rev 1.123   17 Oct 1996 17:48:12   AOONWAL
// Added case label for "WM.Taiwan1"
//
//    Rev 1.122   16 Oct 1996 16:59:06   JWILBER
// Enabled repairs for these WinMacro viruses: NOP.B    1fd3/4
//                                             Reflex   1fd5/6
//                                             Satanic  1fdb
//
//    Rev 1.121   16 Oct 1996 14:45:28   AOONWAL
// Added case label for "WM.Concept.I"
//
//    Rev 1.120   16 Oct 1996 14:02:04   AOONWAL
// Added case labels for "WM.Concept.E (3)" and "WM.COncept.E (4)"
//
//    Rev 1.119   16 Oct 1996 13:14:12   CFORMUL
// Added case labels for WM.Concept.b and WM.Tedious
//
//    Rev 1.118   11 Oct 1996 15:28:28   AOONWAL
// Added case statement for "WM.Bandung.b"
//
//    Rev 1.117   09 Oct 1996 17:39:12   JWILBER
// Added case for repair of Wazzu.B, 1fcf.
//
//    Rev 1.116   07 Oct 1996 13:16:18   CFORMUL
// Added case label for WM.Clock repair
//
//    Rev 1.115   02 Oct 1996 15:45:18   AOONWAL
// Added case statement to repair "WM.Terbaru"
//
//    Rev 1.114   27 Sep 1996 14:24:42   CFORMUL
// Added case labels for repairs for 8 macro viruses
//
//    Rev 1.113   26 Sep 1996 17:25:28   CFORMUL
// Added repair for Y-Key.2346
//
//    Rev 1.112   25 Sep 1996 15:58:30   CFORMUL
// Added cases for 4 macro virus repairs
//
//    Rev 1.111   24 Sep 1996 18:05:54   AOONWAL
// Added Wazzu (3) to the list of repairs
//
//    Rev 1.110   17 Sep 1996 16:51:44   DCHI
// Added XENIXOS1 to list of macro repairs.
//
//    Rev 1.109   16 Sep 1996 18:06:40   DCHI
// Fixed a couple of mistakes in endianization of Quicky repair.
//
//    Rev 1.108   13 Sep 1996 15:58:32   JWILBER
// Added case to switch statement for repair of Indonesia Macro
// virus, 1fbe.
//
//    Rev 1.107   13 Sep 1996 13:32:16   CFORMUL
// Change in Vacsina repair - COMs are now restored to their original size
//
//    Rev 1.106   13 Sep 1996 12:32:26   CFORMUL
// Added repair for Vacsina.TP-04
// IFDEF'ed out for DOS
//
//    Rev 1.105   10 Sep 1996 14:07:36   DANG
// Added Bogus macro virus
//
//    Rev 1.104   06 Sep 1996 12:49:04   CRENERT
// Uncommented VMEM & Zarma prototypes and calls.
//
//    Rev 1.103   05 Sep 1996 14:57:04   SCOURSE
// Added entry for Fishfood
//
//    Rev 1.102   04 Sep 1996 13:05:18   DCHI
// Uncommented VMEM and Zarma and corrected parenthesization problems
// in calls to AVENDIAN macros.
//
//    Rev 1.101   04 Sep 1996 12:08:30   CRENERT
// Removed VMEM & Zarma -- "macro too large" for #ifdef.
//
//    Rev 1.100   04 Sep 1996 11:50:10   CRENERT
// Re-instated the 6 removed September '96 repairs for
// all platforms except DOS.
//
//    Rev 1.99   29 Aug 1996 17:40:08   DANG
// Fixe missing colon error
//
//    Rev 1.98   29 Aug 1996 16:08:00   DANG
// Added Concept.H
//
//    Rev 1.97   29 Aug 1996 15:21:38   CFORMUL
// Added case for BuroNeu repair
//
//    Rev 1.96   28 Aug 1996 17:27:28   CFORMUL
// Added case for Bosco macro repair
//
//    Rev 1.95   28 Aug 1996 16:32:26   DCHI
// Endianized everything, including code that was commented out.
//
//    Rev 1.94   26 Aug 1996 13:31:08   CRENERT
// Removed 6 repairs to stay within NAVEX memory bounds.
//
//    Rev 1.93   19 Aug 1996 18:24:10   JWILBER
// Added VID_JAKARTA for Macro repair switch statement.
//
//    Rev 1.92   16 Aug 1996 12:24:52   CFORMUL
// Added case for variant of wazzu
//
//    Rev 1.91   15 Aug 1996 21:03:02   AOONWAL
// Added case statement for Microsloth repair
//
//    Rev 1.90   15 Aug 1996 19:43:50   AOONWAL
// Added case statement for Telefonical repair
//
//    Rev 1.89   14 Aug 1996 21:59:24   MCANDELA
// uncommented oxana
//
//    Rev 1.88   08 Aug 1996 17:45:40   JWILBER
// Remove unused variable from Oxana repair.
//
//    Rev 1.87   07 Aug 1996 22:36:32   JWILBER
// Added repair for Oxana.1654/1670, 1fac.  This repair also works
// with Oxana.1671.
//
//    Rev 1.86   05 Aug 1996 10:45:58   DCHI
// Added Excel macro virus repair.
//
//    Rev 1.85   29 Jul 1996 17:56:48   SCOURSE
// Added entry for Divina
//
//    Rev 1.84   29 Jul 1996 14:29:18   LNGUYEN
// Added new repaire for Color_B
//
//    Rev 1.83   25 Jul 1996 19:29:18   JWILBER
// Added repair case for Friendly, 1faa.
//
//    Rev 1.82   24 Jul 1996 10:54:04   LNGUYEN
// navexrf.cpp -> S:\NAVEX\VCS\navexrf.cpv
// Enter description of changes.Added new Vid for Phantom
//
//    Rev 1.81   23 Jul 1996 20:38:42   AOONWAL
// Added case labels for "Nuclear.b"
//
//    Rev 1.80   23 Jul 1996 20:00:22   LNGUYEN
// No change.
//
//    Rev 1.79   23 Jul 1996 19:41:48   LNGUYEN
// Restored Vid. to MadDog.
//
//    Rev 1.78   23 Jul 1996 19:01:10   CFORMUL
// Added Case statement for Polite macro repair
//
//    Rev 1.77   23 Jul 1996 18:15:34   CFORMUL
// Made a mod to Emmie repair so it repairs 2823.A and 2823.B
//
//    Rev 1.76   12 Jul 1996 14:18:20   JWILBER
// Added repair for Atom Macro, 1fa4.
//
//    Rev 1.75   11 Jul 1996 16:46:20   LNGUYEN
// Added ID repair for MdmaDMV
//
//    Rev 1.74   11 Jul 1996 15:34:00   AOONWAL
// Added case statements to repair Parasite macro virus
//
//    Rev 1.73   11 Jul 1996 14:50:30   JWILBER
// Added macro repairs for NOP to switch/case statement.  NOP is
// VIDs 1f9c/a1.
//
//    Rev 1.72   02 Jul 1996 19:40:12   JWILBER
// Added repair for Tanpro.524, 1f97.
//
//    Rev 1.71   28 Jun 1996 14:03:46   CFORMUL
// Added case labels for Xenixos and Boom
//
//    Rev 1.70   24 Jun 1996 21:20:40   JWILBER
// Added file repair for Predator.2448, 1f8e.
//
//    Rev 1.69   17 Jun 1996 18:16:36   MCANDELA
// uncommented tent2, fairz, i4060
//
//    Rev 1.68   13 Jun 1996 21:16:38   JWILBER
// Added pointers to Tentacle II repair, VID 1f8d.
//
//    Rev 1.67   03 Jun 1996 18:37:06   CRENERT
// Fairz repair commented out, pending QA.
//
//    Rev 1.66   03 Jun 1996 18:07:32   CRENERT
// Repair added for Fairz.2340
//
//    Rev 1.65   20 May 1996 13:20:46   MCANDELA
//
//    Rev 1.64   15 May 1996 00:30:18   JWILBER
// Added stuff so Tentacle repair would be called.
//
//    Rev 1.63   13 May 1996 16:52:44   JWILBER
// Moved GetEntryPtOffset() to NAVEXSHR.CPP from here.
//
//    Rev 1.62   10 May 1996 18:15:18   JWILBER
// Added case for Infezione repair.
//
//    Rev 1.61   07 May 1996 17:12:08   JALLEE
// Added Irish macro repair.
//
//    Rev 1.60   07 May 1996 13:31:52   JALLEE
// Added repair for Burglar.1150
//
//    Rev 1.59   29 Apr 1996 19:49:28   JWILBER
// Added case for Wazzu Macro virus.
//
//    Rev 1.58   29 Apr 1996 16:30:36   JWILBER
// Changed repair for Jackal file infections to handle Jackal.3120 in
// addition to Jackal.3118.  Added repair for Werewolf.
//
//    Rev 1.57   22 Apr 1996 16:07:58   MCANDELA
// UNCOMMENTED CORDOBES 1F7A
//
//    Rev 1.56   22 Apr 1996 15:42:20   MCANDELA
// UNCOMMENTED VMEM
//
//    Rev 1.55   22 Apr 1996 15:24:16   MCANDELA
// uncommented diamondrocksteady 1f7f
//
//    Rev 1.54   22 Apr 1996 12:02:28   JWILBER
// Pulled file repair for ScreenShake5.
//
//    Rev 1.53   18 Apr 1996 18:01:06   JWILBER
// Added repair for DiamondRockSteady.
//
//    Rev 1.52   15 Apr 1996 20:56:58   RSTANEV
// TCHAR support.
//
//    Rev 1.51   15 Apr 1996 19:44:02   JWILBER
// Put pointer for ScreenSaver5 fix into untested area of jump table.
// Removed INT 3 from ScreenSaver5 fix, also.
//
//    Rev 1.50   15 Apr 1996 17:55:16   CNACHEN
// Typecasting strings to LPTSTR for callbacks..
//
//    Rev 1.49   15 Apr 1996 16:46:32   JWILBER
// Finished repair for ScreenShake5.
//
//    Rev 1.48   15 Apr 1996 12:53:12   JWILBER
// Added in-progress version of EXTRepairFileScrnShake().
//
//    Rev 1.47   14 Apr 1996 20:53:44   JWILBER
// Added EXTFileRepairCordobes(), and GetEntryPtOffset() functions.
// Cordobes file repair is disabled pending QA approval.
//
//    Rev 1.46   10 Apr 1996 12:13:38   JWILBER
// Changed switches due to changes in French Concept Repair/Detection.
//
//    Rev 1.45   09 Apr 1996 13:55:48   JALLEE
// Added Imposter macro virus
//
//    Rev 1.44   08 Apr 1996 18:38:12   JWILBER
// Added repairs for French Concept stuff.
//
//    Rev 1.43   02 Apr 1996 11:24:48   JALLEE
// Added Vmem file repair.
//
//    Rev 1.42   20 Mar 1996 14:59:22   JSULTON
// Uncommented Manzon.  Better late than never.
//
//    Rev 1.41   19 Mar 1996 20:51:46   JALLEE
// Folded in Manzon repair from rev 1.31
// Entered untested pending QA approval.
//
//    Rev 1.40   19 Mar 1996 14:24:34   JSULTON
// Added Quicky to jump table.  It is ready to go.
//
//    Rev 1.39   19 Mar 1996 11:06:20   JALLEE
// Changed (UINT) to (WORD) in Quicky repair
//
//    Rev 1.38   19 Mar 1996 10:59:34   JALLEE
// Changed uint in Quicky repair to (WORD), doh!
//
//    Rev 1.37   15 Mar 1996 20:18:54   JSULTON
// Added Vagabond to jump table.
//
//    Rev 1.36   13 Mar 1996 19:50:02   JSULTON
// Added ID_JACKAL_F to jump table.
//
//    Rev 1.35   12 Mar 1996 13:56:28   JWILBER
// Added file repair for Jackal, VID 1f6d.
//
//    Rev 1.34   11 Mar 1996 11:49:30   JWILBER
// Added repair for Vagabond, 1f6c.
//
//    Rev 1.33   07 Mar 1996 11:26:42   JWILBER
// Changed UINTs to WORDs, and LONGs to DWORDs in Godzilla file fix for
// greater inter-platform compatibility.  Variable names changed as needed
// (ie, lChop -> dwChop).  Some UINTs left because other function take
// them as arguments.
//
//    Rev 1.32   06 Mar 1996 11:56:30   JALLEE
// rget navexrf.cpp version 1.30 for mid month build.
// Version 1.31 contains the Manzon repair not desired for mid month.
//
//    Rev 1.30   05 Mar 1996 12:01:30   JSULTON
// Added Godzilla.890 to jump table and removed #ifdef from function.
//
//    Rev 1.29   05 Mar 1996 11:13:34   JSULTON
// Added Unknown.1329 to jump table and removed #ifdef from the function.
//
//    Rev 1.28   01 Mar 1996 18:20:18   JSULTON
// Added ID_CPW to jump table
//
//    Rev 1.27   01 Mar 1996 17:13:52   JWILBER
// Added macro CLOSEOUT(), that closes the currently open file and returns.
// This is used for error returns when the file to be fixed is still open.
// CLOSEOUT added to Unknown.1329 and Godzilla.890 file fixes where needed.
//
//    Rev 1.26   01 Mar 1996 16:33:02   JALLEE
// Fixed Fairz error checking.
//
//    Rev 1.25   29 Feb 1996 16:55:58   JWILBER
// Added EXTRepairFileGodz(), a file fix for Godzilla.890.
//
//    Rev 1.24   29 Feb 1996 12:47:38   JSULTON
// Removed #ifdef from Fairz.  Also changed ID_Fairz to ID_FAIRZ.  Repair is
// good.
//
//    Rev 1.23   28 Feb 1996 16:09:04   JSULTON
// Removed #ifdef from Die_Hard
//
//    Rev 1.22   15 Feb 1996 16:24:44   JALLEE
// Added repair for Hot macro virus.
//
//    Rev 1.21   15 Feb 1996 16:13:32   MKEATIN
// Added DX Support
//
//    Rev 1.20   15 Feb 1996 15:02:30   JWILBER
// Removed EXE repairs for Unknown.1329 fix.  If EXE is encountered for a
// fix now, it will bail w/o repairing it.
//
//    Rev 1.19   10 Feb 1996 18:25:38   JWILBER
// Finished, debugged, and tested by programmer - now ready for QA.
//
//    Rev 1.18   09 Feb 1996 21:52:26   JWILBER
// Began entering fix for Unknown.1329 - work in progress is commented out
// so file still compiles.
//
//    Rev 1.17   08 Feb 1996 12:05:14   JSULTON
// Removed #if def for Boza repair
//
//    Rev 1.16   08 Feb 1996 10:36:38   DCHI
// Added BOZA repair.
//
//    Rev 1.15   06 Feb 1996 16:53:50   DCHI
// Correction of CONCEPT def to account for NORMAL.DOT.
//
//    Rev 1.14   01 Feb 1996 14:01:36   JALLEE
// Yesmile.5504
//
//    Rev 1.13   30 Jan 1996 15:43:34   DCHI
// Added #ifndef SYM_NLM and #endif pairs to ifdef out functions unused
// on NLM platform.
//
//    Rev 1.12   18 Jan 1996 12:19:02   JALLEE
// Added Little Red.1465 repair
//
//    Rev 1.11   03 Jan 1996 17:13:50   DCHI
// Added Word Macro virus repair.
//
//    Rev 1.10   13 Dec 1995 12:18:06   JALLEE
// Cast literal return values to fix compile errors in w32 platform.
//
//    Rev 1.9   12 Dec 1995 18:42:12   JALLEE
// Voided unused params for NLM compiler.
// (Bug fix to below changes)
//
//    Rev 1.8   12 Dec 1995 17:53:26   JALLEE
// Clarified comments regarding the below changes.
//
//    Rev 1.7   12 Dec 1995 17:16:00   JALLEE
// Fixed Emmie repair to work on read only or system files.
// Added several more repairs, but disabled them in the source.
//
//    Rev 1.6   09 Nov 1995 17:09:18   DCHI
// Fixed NLM complaint about C++ style variable definitions in middle of code.
//
//    Rev 1.5   09 Nov 1995 14:27:04   JALLEE
// Added repair for Emmie virus by Formy.
//
//    Rev 1.4   01 Nov 1995 10:49:04   DCHI
// Removed include of navexhdr.h due to change in format of navexshr.h.
//
//    Rev 1.3   19 Oct 1995 14:20:54   DCHI
// Added LOADDS to external functions for WIN16.  Modified DOS export
// table structure to support tiny model modules.
//
//    Rev 1.2   18 Oct 1995 11:52:24   CNACHEN
// Voided unused parameters to compile correctly for NLM.
//
//    Rev 1.1   16 Oct 1995 13:03:32   DCHI
// Added include of navexrf.h.
//
//    Rev 1.0   13 Oct 1995 13:05:30   DCHI
// Initial revision.
//
//************************************************************************

#ifndef SYM_NLM

#include "platform.h"

#include "endutils.h"

#ifdef WIN16        // VIRSCAN QUAKE 2
#include "cts4.h"
#else
#include "ctsn.h"
#include "callback.h"
#endif

#include "navex.h"

// Declare shared routines

#include "navexshr.h"

#ifdef SYM_DOSX
#undef SYM_DOS
#endif

#if defined(SYM_DOS)

WORD wDS, wES;                      // save loader's DS,ES

#endif

#include "macrorep.h"

#include "neperep.h"

#include "excelrep.h"

#include "mvp.h"

// ******************************
//
// Prototypes for local functions
//
// ******************************

WORD EXTRepairFileQuartz(LPCALLBACKREV1 lpCallBack,
                         LPN30       lpsN30,
                         LPTSTR      lpszFileName,
                         LPBYTE      lpbyWorkBuf);

WORD EXTRepairFileTentatrick(LPCALLBACKREV1 lpCallBack,     // &&TT
                                LPN30       lpsN30,
                                LPTSTR      lpszFileName,
                                LPBYTE      lpbyWorkBuf);

WORD EXTRepairFileBatman2(LPCALLBACKREV1    lpCallBack,
                            LPN30           lpsN30,
                            LPTSTR          lpszFileName,
                            LPBYTE          lpbyWorkBuf);

WORD EXTRepairFileKarn(LPCALLBACKREV1 lpCallBack,
                       LPN30          lpsN30,
                       LPTSTR         lpszFileName,
                       LPBYTE         lpbyWorkBuf);

WORD EXTRepairFileYKey(LPCALLBACKREV1 lpCallBack,
                       LPN30          lpsN30,
                       LPTSTR         lpszFileName,
                       LPBYTE         lpbyWorkBuf);

//
// *** #if-def'd out due to lack of memory in SYM_DOS ***
//
#if defined(SYM_DOSX) || !defined(SYM_DOS)

WORD EXTRepairFileWeed(LPCALLBACKREV1 lpCallBack,
                       LPN30          lpsN30,
                       LPTSTR         lpszFileName,
                       LPBYTE         lpbyWorkBuf);

WORD EXTRepairFileWeed11(LPCALLBACKREV1 lpCallBack,
                         LPN30          lpsN30,
                         LPTSTR         lpszFileName,
                         LPBYTE         lpbyWorkBuf);

WORD EXTRepairFileWeed5850(LPCALLBACKREV1 lpCallBack,
                           LPN30          lpsN30,
                           LPTSTR         lpszFileName,
                           LPBYTE         lpbyWorkBuf);

WORD EXTRepairFileHoot4676(LPCALLBACKREV1 lpCallBack,
                           LPN30          lpsN30,
                           LPTSTR         lpszFileName,
                           LPBYTE         lpbyWorkBuf);


WORD EXTRepairFileHLLT4416(LPCALLBACKREV1 lpCallBack,
                           LPN30          lpsN30,
                           LPTSTR         lpszFileName,
                           LPBYTE         lpbyWorkBuf);
 
WORD EXTRepairFileVacsina1(LPCALLBACKREV1 lpCallBack,
                           LPN30          lpsN30,
                           LPTSTR         lpszFileName,
                           LPBYTE         lpbyWorkBuf);

WORD EXTRepairFileVacsina2(LPCALLBACKREV1 lpCallBack,
                           LPN30          lpsN30,
                           LPTSTR         lpszFileName,
                           LPBYTE         lpbyWorkBuf);
#endif

//
// *** #if-def'd out due to lack of memory in SYM_DOS ***
//
#if defined(SYM_DOSX) || !defined(SYM_DOS)
                                        //Repair for Emmie
WORD EXTRepairEmmie(LPCALLBACKREV1 lpCallBack,
                    LPN30          lpsN30,
                    LPTSTR         lpszFileName,
                    LPBYTE         WorkBuf);
#endif

WORD EXTRepairFileDH (LPCALLBACKREV1 lpCallBack,
                        LPN30          lpsN30,
                        LPTSTR         lpszFileName,
                        LPBYTE         lpbyWorkBuffer);


//WORD EXTRepairFileByway (LPCALLBACKREV1 lpCallBack,
//                          LPTSTR         lpszFileName,
//                          LPBYTE         lpbyWorkBuffer);

                                        // Repair for Cordobes
WORD EXTRepairFileCordobes (LPCALLBACKREV1  lpCallBack,
                            LPN30           lpsN30,
                            LPTSTR          lpszFileName,
                            LPBYTE          lpbyWorkBuffer);
//
// *** #if-def'd out due to lack of memory in SYM_DOS ***
//
#if defined(SYM_DOSX) || !defined(SYM_DOS)

                                        // Repair for DiamondRockSteady (x)
WORD EXTRepairFileDiamond (LPCALLBACKREV1  lpCallBack,
                            LPN30           lpsN30,
                            LPTSTR          lpszFileName,
                            LPBYTE          lpbyWorkBuffer);
#endif

//
// *** #if-def'd out due to lack of memory in SYM_DOS ***
//
#if defined(SYM_DOSX) || !defined(SYM_DOS)
                                        // Repair for CPW
WORD EXTRepairFileCPW (LPCALLBACKREV1 lpCallBack,
                        LPN30          lpsN30,
                        LPTSTR         lpszFileName,
                        LPBYTE         lpbyWorkBuffer);

#endif
                                        // Repair for Burglar.1150
WORD EXTRepairFileBurglar (LPCALLBACKREV1 lpCallBack,
                           LPN30          lpsN30,
                           LPTSTR         lpszFileName,
                           LPBYTE         lpbyWorkBuffer);

//
// *** #if-def'd out due to lack of memory in SYM_DOS ***
//
#if defined(SYM_DOSX) || !defined(SYM_DOS)
                                        // Repair for Fairz
WORD EXTRepairFileFairz (LPCALLBACKREV1 lpCallBack,
                            LPN30          lpsN30,
                            LPTSTR         lpszFileName,
                            LPBYTE         lpbyWorkBuffer);


WORD EXTRepairFileFour11 (LPCALLBACKREV1    lpCallBack,
                            LPN30           lpsN30,
                            LPTSTR          lpszFileName,
                            LPBYTE          lpbyWorkBuffer);
#endif

//
// *** #if-def'd out due to lack of memory in SYM_DOS ***
//
#if defined(SYM_DOSX) || !defined(SYM_DOS)

                                        // Repair for Genrat.785
WORD EXTRepairFileGenrat (LPCALLBACKREV1 lpCallBack,
                            LPN30          lpsN30,
                            LPTSTR         lpszFileName,
                            LPBYTE         lpbyWorkBuffer);
#endif
//
// *** #if-def'd out due to lack of memory in SYM_DOS ***
//
#if defined(SYM_DOSX) || !defined(SYM_DOS)

                                        // Repair for Godzilla.890
WORD EXTRepairFileGodz (LPCALLBACKREV1  lpCallBack,
                        LPN30           lpsN30,
                        LPTSTR          lpszFileName,
                        LPBYTE          lpbyWorkBuffer);
#endif
//
// *** #if-def'd out due to lack of memory in SYM_DOS ***
//
#if defined(SYM_DOSX) || !defined(SYM_DOS)

                                        // Repair for Infector.4060
WORD EXTRepairFileI4060 (LPCALLBACKREV1 lpCallBack,
                            LPN30          lpsN30,
                            LPTSTR         lpszFileName,
                            LPBYTE         lpbyWorkBuffer);
#endif

//
// *** #if-def'd out due to lack of memory in SYM_DOS ***
//
#if defined(SYM_DOSX) || !defined(SYM_DOS)

                                        // Repair for Jackal
WORD EXTRepairFileJackal (LPCALLBACKREV1 lpCallBack,
                            LPN30          lpsN30,
                            LPTSTR         lpszFileName,
                            LPBYTE         lpbyWorkBuffer);
#endif

                                        // Repair for Little Red.1465
                                        //            Little Red.1465 (x)
WORD EXTRepairFileLittleR (LPCALLBACKREV1 lpCallBack,
                            LPN30          lpsN30,
                            LPTSTR         lpszFileName,
                            LPBYTE         lpbyWorkBuffer);

//
// *** #if-def'd out due to lack of memory in SYM_DOS ***
//
// #if defined(SYM_DOSX) || !defined(SYM_DOS)
                                        // repair for Manuel.2209
WORD EXTRepairFileManuel(LPCALLBACKREV1 lpCallBack,
                            LPN30       lpsN30,
                            LPTSTR      lpszFileName,
                            LPBYTE      lpbyWorkBuffer);
//#endif

                                        // repair for manzon
WORD EXTRepairFileManzon  (LPCALLBACKREV1 lpCallBack,
                            LPN30          lpsN30,
                            LPTSTR         lpszFileName,
                            LPBYTE         lpbyWorkBuffer);
//
// *** #if-def'd out due to lack of memory in SYM_DOS ***
//
#if defined(SYM_DOSX) || !defined(SYM_DOS)

                                        // repair for Oxana.1654/1670/1671
WORD EXTRepairFileOxana (LPCALLBACKREV1 lpCallBack,
                            LPN30       lpsN30,
                            LPTSTR      lpszFileName,
                            LPBYTE      lpbyWorkBuffer);
#endif
                                        // repair for Predator.2448
WORD EXTRepairFilePredt (LPCALLBACKREV1 lpCallBack,
                            LPN30          lpsN30,
                            LPTSTR         lpszFileName,
                            LPBYTE         lpbyWorkBuffer);

                                        // Repair for Quicky
WORD EXTRepairFileQuicky (LPCALLBACKREV1 lpCallBack,
                            LPN30          lpsN30,
                            LPTSTR         lpszFileName,
                            LPBYTE         lpbyWorkBuffer);

//
// *** #if-def'd out due to lack of memory in SYM_DOS ***
//
#if defined(SYM_DOSX) || !defined(SYM_DOS)

                                        // Repair for Tanpro
WORD EXTRepairFileTanpro (LPCALLBACKREV1    lpCallBack,
                            LPN30           lpsN30,
                            LPTSTR          lpszFileName,
                            LPBYTE          lpbyWorkBuffer);
#endif
//
// *** #if-def'd out due to lack of memory in SYM_DOS ***
//
#if defined(SYM_DOSX) || !defined(SYM_DOS)
                                        // Repair for Unknown.1329

WORD EXTRepairFileUnknown (LPCALLBACKREV1 lpCallBack,
                            LPN30          lpsN30,
                            LPTSTR         lpszFileName,
                            LPBYTE         lpbyWorkBuffer);
                                        // Repair for Vagabond
WORD EXTRepairFileVaga (LPCALLBACKREV1 lpCallBack,
                         LPN30          lpsN30,
                         LPTSTR         lpszFileName,
                         LPBYTE         lpbyWorkBuffer);
#endif

//
// *** #if-def'd out due to lack of memory in SYM_DOS ***
//
#if defined(SYM_DOSX) || !defined(SYM_DOS)

                                        // Repair for Vmem
WORD EXTRepairFileVMem (LPCALLBACKREV1 lpCallBack,
                         LPN30          lpsN30,
                         LPTSTR         lpszFileName,
                         LPBYTE         lpbyWorkBuffer);

                                        // Repair for Werewolf
WORD EXTRepairFileWerewolf (LPCALLBACKREV1 lpCallBack,
                            LPN30           lpsN30,
                            LPTSTR          lpszFileName,
                            LPBYTE         lpbyWorkBuffer);
#endif

//
// *** #if-def'd out due to lack of memory in SYM_DOS ***
//
#if defined(SYM_DOSX) || !defined(SYM_DOS)

                                        // Repair for Yesmile.5504
WORD EXTRepairFileYesmile (LPCALLBACKREV1 lpCallBack,
                            LPN30           lpsN30,
                            LPTSTR          lpszFileName,
                            LPBYTE         lpbyWorkBuffer);

#endif

WORD RepairFileYKey_Karn (LPCALLBACKREV1 lpCallBack,
                        LPTSTR         lpszFileName,
                        LPBYTE         lpbyWorkBuffer,
                        int            offsetEOF);

//
// *** #if-def'd out due to lack of memory in SYM_DOS ***
//
#if defined(SYM_DOSX) || !defined(SYM_DOS)

                                        // Repair for Zarma
WORD EXTRepairFileZarma (LPCALLBACKREV1 lpCallBack,
                            LPN30       lpsN30,
                            LPTSTR      lpszFileName,
                            LPBYTE      lpbyWorkBuffer);
#endif

WORD RepairFileOpen (LPCALLBACKREV1     lpCallBack,
                        LPTSTR          lpszFileName,
                        HFILE FAR       *lphFile,
                        UINT FAR        *lpuAttr);

WORD RepairFileClose (LPCALLBACKREV1    lpCallBack,
                        LPTSTR          lpszFileName,
                        HFILE           hFile,
                        UINT            uAttr);

WORD RepairFileTruncate (LPCALLBACKREV1 lpCallBack,
                         HFILE          hFile,
                         long           lOffset);

WORD RepairFileMoveBytes (LPCALLBACKREV1    lpCallBack,
                            HFILE           hFile,
                            LPBYTE          lpbyWorkBuffer,
                            DWORD           dwBegRead,
                            DWORD           dwBegWrite,
                            DWORD           dwNumBytes);

// ***************************************************************************
//
// RepairJumpTable[]
//
//      Jump Table to Local File repair functions.
//      Used by EXTRepairFile to call individual repairs.
//      Note: 0x00 is an illegal value for either hi or lo byte of function id.
//            This is for backwards compatibility in the virscan.dat w/ 3.0.7.
//
//      Defwriter, add your function id and function pointer here.
//
// ***************************************************************************

struct tagJumpTableEntry
{
    WORD    wID;
    WORD    (*fnRepairFunction) (LPCALLBACKREV1 lpCallBack,
                                 LPN30           lpsN30,
                                 LPTSTR          lpszFileName,
                                 LPBYTE         WorkBuf);
} RepairJumpTable[] =
    {
//
// *** #if-def'd out due to lack of memory in SYM_DOS ***
//
#if defined(SYM_DOSX) || !defined(SYM_DOS)
        {ID_DIAMOND,        EXTRepairFileDiamond},  // &&D
        {ID_EMMIE,          EXTRepairEmmie},
        {ID_GODZIL,         EXTRepairFileGodz},     // &&G
        {ID_I4060,          EXTRepairFileI4060},
        {ID_OXANA,          EXTRepairFileOxana},    // &&O
        {ID_UNKNOWN,        EXTRepairFileUnknown},  // &&&
        {ID_VACSINA1,       EXTRepairFileVacsina1},
        {ID_VACSINA2,       EXTRepairFileVacsina2},
        {ID_VMEM,           EXTRepairFileVMem},
        {ID_YESMILE,        EXTRepairFileYesmile},
        {ID_JACKAL_F,       EXTRepairFileJackal},   // &&J
        {ID_FAIRZ,          EXTRepairFileFairz},
        {ID_GENRAT,         EXTRepairFileGenrat},   // &&GR
        {ID_TANPRO,         EXTRepairFileTanpro},   // &&T
        {ID_CPW,            EXTRepairFileCPW},
        {ID_ZARMA,          EXTRepairFileZarma},
        {ID_HLLT4416,       EXTRepairFileHLLT4416},
        {ID_HOOT4676,       EXTRepairFileHoot4676},
        {ID_VAGABOND,       EXTRepairFileVaga},     // &&V
        {ID_WEREWOLF,       EXTRepairFileWerewolf}, // &&W
        {ID_FOUR11,         EXTRepairFileFour11},
        {ID_WEED,           EXTRepairFileWeed},
        {ID_WEED11,         EXTRepairFileWeed11},
        {ID_WEED5850,       EXTRepairFileWeed5850},

#endif
        {ID_MANUEL,         EXTRepairFileManuel},   // &&M
        {ID_TENTRK,         EXTRepairFileTentatrick},   // &&TT
        {ID_BATMAN2,        EXTRepairFileBatman2},  // &&B2
        {ID_BOZA,           EXTRepairFileBoza},
        {ID_BURGLAR,        EXTRepairFileBurglar},
        {ID_CORDOBES,       EXTRepairFileCordobes}, // &&C
        {ID_DH,             EXTRepairFileDH},
        {ID_KARN,           EXTRepairFileKarn},
        {ID_LR,             EXTRepairFileLittleR},
        {ID_MANZON,         EXTRepairFileManzon},
        {ID_PRED24,         EXTRepairFilePredt},
        {ID_QUICKY,         EXTRepairFileQuicky},
        {ID_TENT,           EXTRepairFileTent},
        {ID_TENT2,          EXTRepairFileNewTent},
        {ID_YKEY,           EXTRepairFileYKey},
        {ID_TPVO,           EXTRepairFileTpvo},
        {ID_QUARTZ,         EXTRepairFileQuartz}
#if(0)
// These repairs are untested.
#endif
    };

// ***************************************************************************
//
// Native NAVEX function
//
// ***************************************************************************


// EXTRepairFile arguments:
//
// lpCallBack           : pointer to callback structure for FileOpen, etc.
// wVersionNumber       : version of NAV calling external code
// lpsN30               : pointer to N30 structure containing VIRSCAN.DAT
//                        information and virus stats
// lpszFileName         : name of the file to repair
// lpbyWorkBuffer       : temporary 2-4K work buffer.
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK          if OK
//                        EXTSTATUS_FILE_ERROR  if file error
//                        EXTSTATUS_MEM_ERROR   if memory error
//                        EXTSTATUS_NO_REPAIR   if unable to repair

EXTSTATUS FAR WINAPI NLOADDS EXTRepairFile(LPCALLBACKREV1 lpCallBack,
                                           WORD           wVersionNumber,
                                           LPN30          lpsN30,
                                           LPTSTR         lpszFileName,
                                           LPBYTE         lpbyWorkBuffer)
{

#if defined(SYM_DOS)

    _asm
        {
        mov cs:[wDS], ds
        mov cs:[wES], es
        mov ax, cs
        mov ds, ax
        mov es, ax
        }

#endif

    WORD        wReturn = EXTSTATUS_NO_REPAIR;
    WORD        i = 0;


    (void)lpCallBack;
    (void)wVersionNumber;
    (void)lpsN30;
    (void)lpszFileName;
    (void)lpbyWorkBuffer;


    if (lpsN30->wControl2 & EXEC_CODE)
    {
        while(i < (sizeof(RepairJumpTable) / sizeof(struct tagJumpTableEntry)))
        {
            if (lpsN30->wBookMark == RepairJumpTable[i].wID)
            {
                wReturn = RepairJumpTable[i].fnRepairFunction(lpCallBack,
                                                              lpsN30,
                                                              lpszFileName,
                                                              lpbyWorkBuffer);
                break;
            }
            i++;
        }
    }
    else
    {

#define N30_CTRL2_BIT_FLAG2I        0x4000
#define N30_CTRL2_BIT_FLAG2J        0x8000

        if ((lpsN30->wControl2 & N30_CTRL2_BIT_FLAG2I) &&
            !(lpsN30->wControl2 & N30_CTRL2_BIT_FLAG2J))
        {
            /////////////////////////////////////////////////////
            // Word 6.0/95/97
            // Excel 97
            // MVP
            /////////////////////////////////////////////////////

            wReturn = WordMacroVirusRepair(lpCallBack,
                                           lpsN30->wVirID,
                                           lpszFileName,
                                           lpbyWorkBuffer);

            /////////////////////////////////////////////////////
            // Excel 5.0/95 macro virus repair
            /////////////////////////////////////////////////////

            if (wReturn != EXTSTATUS_OK)
            {
                wReturn = ExcelMacroVirusRepair(lpCallBack,
                                                lpsN30->wVirID,
                                                lpszFileName,
                                                lpbyWorkBuffer);
            }
        }
    }

#if defined(SYM_DOS)

    _asm
        {
        mov ds, cs:[wDS]
        mov es, cs:[wES]
        }

#endif

    return ( wReturn );
}

// ***************************************************************************
//
// File repair functions
//
// ***************************************************************************

// EXTRepairFileByway arguments:
//
// lpCallBack           : pointer to callback structure for FileOpen, etc.
// lpszFileName         : name of the file to repair
// lpbyWorkBuffer       : temporary 2-4K work buffer.
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK          if OK
//                        EXTSTATUS_NO_REPAIR   if unable to repair

//WORD EXTRepairFileByway (LPCALLBACKREV1 lpCallBack,
//                         LPTSTR          lpszFileName,
//                         LPBYTE         lpbyWorkBuffer)
//{
//    return (EXTSTATUS_NO_REPAIR);
//}


// EXTRepairFileQuartz arguments:
//
// lpCallBack           : pointer to callback structure for FileOpen, etc.
// lpsN30               : pointer to N30 structure containing VIRSCAN.DAT
//                        information and virus stats
// lpszFileName         : name of the file to repair
// lpbyWorkBuffer       : temporary 2-4K work buffer.
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK          if OK
//                        EXTSTATUS_NO_REPAIR   if unable to repair
//

WORD EXTRepairFileQuartz (LPCALLBACKREV1 lpCallBack,
                          LPN30          lpsN30,
                          LPTSTR         lpszFileName,
                          LPBYTE         lpbyWorkBuffer)
{
    HFILE       hFile;
    UINT        uAttr;
    BYTE        bTOF;
    WORD        wComSize;
    DWORD       dwFileSeek, dwEntryPt;
    EXEHEADER   ExeHdr;


    if (0 != FILEOPEN())                // Open file
        return(EXTSTATUS_NO_REPAIR);

    if (1 != FILEREAD((LPBYTE) &bTOF, 1))
        CLOSEOUT(EXTSTATUS_FILE_ERROR);

    if (bTOF == 0xb8)       // It's a COM
    {
        if (1 != FILESEEK(1))
            CLOSEOUT(EXTSTATUS_FILE_ERROR);

        //Get the Entry Point offset
        if (2 != FILEREAD((LPBYTE) &wComSize, 2))
            CLOSEOUT(EXTSTATUS_FILE_ERROR);

        dwFileSeek = (DWORD) wComSize + 0xdf;

        //Seek to the orriginal bytes
        if (dwFileSeek != FILESEEK(dwFileSeek))
            CLOSEOUT(EXTSTATUS_FILE_ERROR);

        //Read them in
        if (5 != FILEREAD(lpbyWorkBuffer, 5))
            CLOSEOUT(EXTSTATUS_FILE_ERROR);

        SEEK_TOF();

        //Write them out
        if (5 != FILEWRITE(lpbyWorkBuffer, 5))
            CLOSEOUT(EXTSTATUS_FILE_ERROR);

        dwEntryPt = (DWORD) wComSize - 0x100;

    }
    else                        //It's an EXE
    {
        SEEK_TOF();
                                            // Get file header
        if (sEXEHEADER != FILEREAD((LPBYTE) &ExeHdr, sEXEHEADER))
            CLOSEOUT(EXTSTATUS_FILE_ERROR);

        dwEntryPt = GetEntryPtOffset((LPBYTE) &ExeHdr);
        dwFileSeek = 0x2e1 + dwEntryPt;

        //Seek to orig sp and read in
        if (dwFileSeek != FILESEEK(dwFileSeek))
            CLOSEOUT(EXTSTATUS_FILE_ERROR);

        if (2 != FILEREAD((LPBYTE) &ExeHdr.exInitSP, 2))
            CLOSEOUT(EXTSTATUS_FILE_ERROR);

        dwFileSeek += 0x153;

        //Seek to orig cs and read in
        if (dwFileSeek != FILESEEK(dwFileSeek))
            CLOSEOUT(EXTSTATUS_FILE_ERROR);

        if (2 != FILEREAD((LPBYTE) &ExeHdr.exInitCS, 2))
            CLOSEOUT(EXTSTATUS_FILE_ERROR);

        dwFileSeek += 0xb2;

        //Seek to Orig ss and read in
        if (dwFileSeek != FILESEEK(dwFileSeek))
            CLOSEOUT(EXTSTATUS_FILE_ERROR);

        if (2 != FILEREAD((LPBYTE) &ExeHdr.exInitSS, 2))
            CLOSEOUT(EXTSTATUS_FILE_ERROR);

        dwFileSeek += 0x13;

        //Seek to orig ip and read in
        if (dwFileSeek != FILESEEK(dwFileSeek))
            CLOSEOUT(EXTSTATUS_FILE_ERROR);

        if (2 != FILEREAD((LPBYTE) &ExeHdr.exInitIP, 2))
            CLOSEOUT(EXTSTATUS_FILE_ERROR);

        SEEK_TOF();

        //Write out the header
        if (sEXEHEADER != FILEWRITE((LPBYTE) &ExeHdr, sEXEHEADER))
            CLOSEOUT(EXTSTATUS_FILE_ERROR);
    }


    //Truncate the file
    if (dwEntryPt != FILESEEK(dwEntryPt))
        CLOSEOUT(EXTSTATUS_FILE_ERROR);

    if (-1 == FILEWRITE(NULL, 0))
        CLOSEOUT(EXTSTATUS_FILE_ERROR);

    CLOSEOUT(EXTSTATUS_OK);

}

//
// *** #if-def'd out due to lack of memory in SYM_DOS ***
//
#if defined(SYM_DOSX) || !defined(SYM_DOS)

// EXTRepairFileWeed arguments:
//
// lpCallBack           : pointer to callback structure for FileOpen, etc.
// lpsN30               : pointer to N30 structure containing VIRSCAN.DAT
//                        information and virus stats
// lpszFileName         : name of the file to repair
// lpbyWorkBuffer       : temporary 2-4K work buffer.
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK          if OK
//                        EXTSTATUS_NO_REPAIR   if unable to repair
//

WORD EXTRepairFileWeed (LPCALLBACKREV1 lpCallBack,
                        LPN30          lpsN30,
                        LPTSTR         lpszFileName,
                        LPBYTE         lpbyWorkBuffer)
{
    HFILE   hFile;
    UINT    uAttr;
    BYTE    byHbyte;
    int     i;

    // Open the infected file
    if (RepairFileOpen(lpCallBack, lpszFileName, &hFile, &uAttr))
    {
        return(EXTSTATUS_NO_REPAIR);
    }

    // Seek to beginning of the host code
    if ( -1 == lpCallBack->FileSeek(hFile, -3263, SEEK_END))
    {
        RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
        return(EXTSTATUS_NO_REPAIR);
    }

    // Read in the encrypted host
    if ( -1 == lpCallBack->FileRead(hFile, lpbyWorkBuffer, 3263))
    {
        RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
        return(EXTSTATUS_NO_REPAIR);
    }

    // Decrypt the rest of the code with a different key
    for (i = 0; i < 3263; i++)
    {
       byHbyte = lpbyWorkBuffer[i] >> 4;

       if ((byHbyte < 4) || ((byHbyte > 7) && (byHbyte < 0xC)))
           lpbyWorkBuffer[i] ^= 0x40;
       else
           lpbyWorkBuffer[i] ^= 0xc0;
    }


    // Seek to beginning of the file
    if ( -1 == lpCallBack->FileSeek(hFile, 0, SEEK_SET))
    {
        RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
        return(EXTSTATUS_NO_REPAIR);
    }

    // Write the bytes back to the top of the file
    if ( -1 == lpCallBack->FileWrite(hFile, lpbyWorkBuffer, 3263))
    {
        RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
        return(EXTSTATUS_NO_REPAIR);
    }

    // Truncate the file
    if (RepairFileTruncate(lpCallBack, hFile, 3263))
    {
        RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
        return(EXTSTATUS_NO_REPAIR);
    }

    if (RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr))
    {
        return(EXTSTATUS_NO_REPAIR);
    }

    return (EXTSTATUS_OK);

}


// EXTRepairFileWeed11 arguments:
//
// lpCallBack           : pointer to callback structure for FileOpen, etc.
// lpsN30               : pointer to N30 structure containing VIRSCAN.DAT
//                        information and virus stats
// lpszFileName         : name of the file to repair
// lpbyWorkBuffer       : temporary 2-4K work buffer.
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK          if OK
//                        EXTSTATUS_NO_REPAIR   if unable to repair
//

WORD EXTRepairFileWeed11 (LPCALLBACKREV1 lpCallBack,
                          LPN30          lpsN30,
                          LPTSTR         lpszFileName,
                          LPBYTE         lpbyWorkBuffer)
{
    HFILE   hFile;
    UINT    uAttr;
    WORD    wHostWord;
    int     i, counter;

    // Open the infected file
    if (RepairFileOpen(lpCallBack, lpszFileName, &hFile, &uAttr))
    {
        return(EXTSTATUS_NO_REPAIR);
    }

    // Seek to beginning of the host code
    if ( -1 == lpCallBack->FileSeek(hFile, -4080, SEEK_END))
    {
        RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
        return(EXTSTATUS_NO_REPAIR);
    }

    if ( -1 == lpCallBack->FileRead(hFile, lpbyWorkBuffer, 4080))
    {
        RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
        return(EXTSTATUS_NO_REPAIR);
    }

    counter = 0;  // This is the position of the current word being
                  // decrypted.
    // Decrypt the rest of the code with a different key
    for (i = 0; i < 4080; i+=2)
    {
       wHostWord =  lpbyWorkBuffer[i+1];
       wHostWord =  wHostWord << 8;
       wHostWord |= lpbyWorkBuffer[i];

       wHostWord = wHostWord + 0xFF81;
       wHostWord = wHostWord + 0xFF40;
       wHostWord = wHostWord - (counter++);

       lpbyWorkBuffer[i]  = (BYTE) (wHostWord & 0x00FF);
       wHostWord = wHostWord & 0x0000FF00;
       lpbyWorkBuffer[i+1]= (BYTE) (wHostWord >> 8);
    }

    // Seek to beginning of the file
    if ( -1 == lpCallBack->FileSeek(hFile, 0, SEEK_SET))
    {
        RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
        return(EXTSTATUS_NO_REPAIR);
    }

    // Write the bytes back to the top of the file
    if ( -1 == lpCallBack->FileWrite(hFile, lpbyWorkBuffer, 4080))
    {
        RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
        return(EXTSTATUS_NO_REPAIR);
    }


    // Truncate the file at the point where we grabbed the orginal
    // host code.
    if (RepairFileTruncate(lpCallBack, hFile, 4080))
    {
        RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
        return(EXTSTATUS_NO_REPAIR);
    }

    if (RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr))
    {
        return(EXTSTATUS_NO_REPAIR);
    }

    return (EXTSTATUS_OK);

}

// EXTRepairFileWeed5850 arguments:
//
// lpCallBack           : pointer to callback structure for FileOpen, etc.
// lpsN30               : pointer to N30 structure containing VIRSCAN.DAT
//                        information and virus stats
// lpszFileName         : name of the file to repair
// lpbyWorkBuffer       : temporary 2-4K work buffer.
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK          if OK
//                        EXTSTATUS_NO_REPAIR   if unable to repair
//

WORD EXTRepairFileWeed5850 (LPCALLBACKREV1 lpCallBack,
                            LPN30          lpsN30,
                            LPTSTR         lpszFileName,
                            LPBYTE         lpbyWorkBuffer)
{
   HFILE   hFile;
   UINT    uAttr;
   WORD    wHostWord,wInfectionSize = 0,wFirstKey = 0,wSecondKey = 0;
   int     i, counter, seek_pos, pass;

   wInfectionSize += lpsN30->wInfSize;
   wFirstKey += lpsN30->wTagSize;
   wSecondKey += lpsN30->wHardLoc1;


   // Open the infected file
   if (RepairFileOpen(lpCallBack, lpszFileName, &hFile, &uAttr))
   {
       return(EXTSTATUS_NO_REPAIR);
   }

   counter = 0;    // This is the position of the current word being

   // Will go three passes, since the infection size is greater than
   // the maximum buffer size.
   for ( pass=0; pass<3 ; pass++)
   {
      // This keeps track of where to read the host code.
      seek_pos = wInfectionSize - (pass*2048);
      if (seek_pos < 0)
         seek_pos += 2048;


      // Seek to beginning of the host code
      if ( -1 == lpCallBack->FileSeek(hFile, -seek_pos, SEEK_END))
      {
         RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
         return(EXTSTATUS_NO_REPAIR);
      }

      if ( -1 == lpCallBack->FileRead(hFile, lpbyWorkBuffer, 2048))
      {
         RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
         return(EXTSTATUS_NO_REPAIR);
      }


      // decrypted.
      // Decrypt the rest of the code with a different key
      for (i = 0; i < 2048; i+=2)
      {
         wHostWord =  lpbyWorkBuffer[i+1];
         wHostWord =  wHostWord << 8;
         wHostWord |= lpbyWorkBuffer[i];

         wHostWord = wHostWord + wFirstKey;
         wHostWord = wHostWord + wSecondKey;
         wHostWord = wHostWord - (counter++);

         lpbyWorkBuffer[i]  = (BYTE) (wHostWord & 0x00FF);
         wHostWord = wHostWord & 0x0000FF00;
         lpbyWorkBuffer[i+1]= (BYTE) (wHostWord >> 8);
      }

      // Seek to part of the file to write the current decoded bytes
      if ( -1 == lpCallBack->FileSeek(hFile, pass*2048, SEEK_SET))
      {
         RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
         return(EXTSTATUS_NO_REPAIR);
      }

      // Write the bytes back to the top of the file
      if (pass < 2)
         i = 2048;
      else
         i = 1754;

      if ( -1 == lpCallBack->FileWrite(hFile, lpbyWorkBuffer, i))
      {
         RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
         return(EXTSTATUS_NO_REPAIR);
      }
   }


      // Truncate the file at the point where we grabbed the orginal
      // host code.
      if (RepairFileTruncate(lpCallBack, hFile, wInfectionSize))
      {
         RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
         return(EXTSTATUS_NO_REPAIR);
      }

      if (RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr))
      {
         return(EXTSTATUS_NO_REPAIR);
      }

      return (EXTSTATUS_OK);
}


// EXTRepairFileHoot4676 arguments
//
// lpCallBack           : pointer to callback structure for FileOpen, etc.
// lpsN30               : pointer to N30 structure containing VIRSCAN.DAT
//                        information and virus stats
// lpszFileName         : name of the file to repair
// lpbyWorkBuffer       : temporary 2-4K work buffer.
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK          if OK
//                        EXTSTATUS_NO_REPAIR   if unable to repair
//

WORD EXTRepairFileHoot4676 (LPCALLBACKREV1 lpCallBack,
                            LPN30          lpsN30,
                            LPTSTR         lpszFileName,
                            LPBYTE         lpbyWorkBuffer)
{
    WORD    i, wBytesRead,
            ax, bx, cx, dx;
    DWORD   dwSeed = 0x54697473,
            dwResult;
    HFILE   hFile;
    UINT    uAttr;

    // Open the infected file
    if (RepairFileOpen(lpCallBack, lpszFileName, &hFile, &uAttr))
    {
        return(EXTSTATUS_NO_REPAIR);
    }

    do{
        // Seek to the next block of host code to decrypt
        if ( -1 == lpCallBack->FileSeek(hFile, 0x1244, SEEK_CUR))
        {
            RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
            return(EXTSTATUS_NO_REPAIR);
        }

        // Read in 2k of the encrypted host
        if ( -1 == (wBytesRead = (WORD) lpCallBack->FileRead(hFile, lpbyWorkBuffer, 0x800)))
        {
            RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
            return(EXTSTATUS_NO_REPAIR);
        }

        // This tests if we're alredy at EOF
        if (!wBytesRead)
            break;

        // This is a heinous loop that calculates the key and decrpyts
        // the block we just read in.  This code may not be portable to
        // big endian machines!!
        for (i = 0; i < wBytesRead; i++)
        {
            bx = (WORD) (dwSeed >> 16);
            cx = (WORD) (dwSeed & 0x0000ffff);
            dwResult = (DWORD) ((dwSeed & 0x0000ffff) * 0x8405);
            ax = (WORD) (dwResult & 0x0000ffff);
            dx = (WORD) (dwResult >> 16) + (cx * 0x808) +
                 bx + (bx << 2) + (bx << 10) + (bx << 15);
            dwSeed = (DWORD) ((((DWORD) dx << 16 ) + (DWORD) ax) + 1);

            // It's a simple byte xor, but getting the key is a pain!!!
            lpbyWorkBuffer[i] ^= (BYTE) (dwSeed >> 24);
        }

        // Seek backwards to the point were we want to write out the
        // now decrypted bytes
        if ( -1 == lpCallBack->FileSeek(hFile, -(long)(wBytesRead + 0x1244), SEEK_CUR))
        {
            RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
            return(EXTSTATUS_NO_REPAIR);
        }
 
        // Write 'em out
        if ( -1 == lpCallBack->FileWrite(hFile, lpbyWorkBuffer, wBytesRead))
        {
            RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
            return(EXTSTATUS_NO_REPAIR);
        }

    } while (wBytesRead == 0x800);


    if (RepairFileTruncate(lpCallBack, hFile, 0x1244))
    {
        RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
        return(EXTSTATUS_NO_REPAIR);
    }

    if (RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr))
    {
        return(EXTSTATUS_NO_REPAIR);
    }

    return (EXTSTATUS_OK);

}

// EXTRepairFileHLLT4416 arguments:
//
// lpCallBack           : pointer to callback structure for FileOpen, etc.
// lpsN30               : pointer to N30 structure containing VIRSCAN.DAT
//                        information and virus stats
// lpszFileName         : name of the file to repair
// lpbyWorkBuffer       : temporary 2-4K work buffer.
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK          if OK
//                        EXTSTATUS_NO_REPAIR   if unable to repair
//

WORD EXTRepairFileHLLT4416 (LPCALLBACKREV1 lpCallBack,
                            LPN30          lpsN30,
                            LPTSTR         lpszFileName,
                            LPBYTE         lpbyWorkBuffer)
{
    HFILE   hFile;
    UINT    uAttr, uBytesRead;
    LONG    lOffset = 0;
    int     i, iBytesLeft, iEncBytesLeft = 3176;


    if (RepairFileOpen(lpCallBack, lpszFileName, &hFile, &uAttr))
    {
        return(EXTSTATUS_NO_REPAIR);
    }

    for (iBytesLeft = 4416; iBytesLeft > 0; iBytesLeft -= (int) uBytesRead, lOffset += (LONG) uBytesRead)
    {
        if ((DWORD)-1 == lpCallBack->FileSeek(hFile, (LONG) -iBytesLeft, SEEK_END))
        {
            RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
            return(EXTSTATUS_NO_REPAIR);
        }

        uBytesRead = lpCallBack->FileRead(hFile, lpbyWorkBuffer, 2048);

        if (uBytesRead != 2048 && uBytesRead != (UINT) iBytesLeft)
        {
            RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
            return(EXTSTATUS_NO_REPAIR);
        }

        for (i = 0; i < (int) uBytesRead && iEncBytesLeft > 0; i++, iEncBytesLeft--)
            lpbyWorkBuffer[i] ^= 0xff;
 
        if ((DWORD)-1 == lpCallBack->FileSeek(hFile, lOffset, SEEK_SET))
        {
            RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
            return(EXTSTATUS_NO_REPAIR);
        }
 
        if ((UINT)-1 == lpCallBack->FileWrite(hFile, lpbyWorkBuffer, uBytesRead))
        {
            RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
            return(EXTSTATUS_NO_REPAIR);
        }
    }


    if (RepairFileTruncate(lpCallBack, hFile, 4416))
    {
        RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
        return(EXTSTATUS_NO_REPAIR);
    }


    if (RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr))
    {
        return(EXTSTATUS_NO_REPAIR);
    }

    return (EXTSTATUS_OK);

}

#endif

// EXTRepairFileKarn arguments:
//
// lpCallBack           : pointer to callback structure for FileOpen, etc.
// lpsN30               : pointer to N30 structure containing VIRSCAN.DAT
//                        information and virus stats
// lpszFileName         : name of the file to repair
// lpbyWorkBuffer       : temporary 2-4K work buffer.
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK          if OK
//                        EXTSTATUS_NO_REPAIR   if unable to repair
//
//  This repair calls RepairFileYKeyKarn  function to do actual repair by
//  passing an extra parameter, offset in bytes from end of file, where
//  EXE header information is available.


WORD EXTRepairFileKarn (LPCALLBACKREV1 lpCallBack,
                        LPN30          lpsN30,
                        LPTSTR         lpszFileName,
                        LPBYTE         lpbyWorkBuffer)
{
    int    k;

    k = -1210;           // offset from end of file

    if ((WORD)-1 == RepairFileYKey_Karn (lpCallBack,lpszFileName,lpbyWorkBuffer,k))

    {
        return (EXTSTATUS_NO_REPAIR);
    }

    return (EXTSTATUS_OK);

}

// EXTRepairFileYKey arguments:
//
// lpCallBack           : pointer to callback structure for FileOpen, etc.
// lpsN30               : pointer to N30 structure containing VIRSCAN.DAT
//                        information and virus stats
// lpszFileName         : name of the file to repair
// lpbyWorkBuffer       : temporary 2-4K work buffer.
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK          if OK
//                        EXTSTATUS_NO_REPAIR   if unable to repair
//
//
//  This repair calls RepairFileYKeyKarn  function to do actual repair by
//  passing an extra parameter, offset in bytes from end of file, where
//  EXE header information is available.


WORD EXTRepairFileYKey (LPCALLBACKREV1 lpCallBack,
                        LPN30          lpsN30,
                        LPTSTR         lpszFileName,
                        LPBYTE         lpbyWorkBuffer)
{
    int    k;

    k = -1439;           // offset from end of file

    if ((WORD)-1 == RepairFileYKey_Karn (lpCallBack,lpszFileName,lpbyWorkBuffer,k))

    {
        return (EXTSTATUS_NO_REPAIR);
    }

    return (EXTSTATUS_OK);

}

//
// *** #if-def'd out due to lack of memory in SYM_DOS ***
//
#if defined(SYM_DOSX) || !defined(SYM_DOS)

// EXTRepairFileVacsina1 arguments:
//
// lpCallBack           : pointer to callback structure for FileOpen, etc.
// lpsN30               : pointer to N30 structure containing VIRSCAN.DAT
//                        information and virus stats
// lpszFileName         : name of the file to repair
// lpbyWorkBuffer       : temporary 2-4K work buffer.
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK          if OK
//                        EXTSTATUS_NO_REPAIR   if unable to repair
//

WORD EXTRepairFileVacsina1 (LPCALLBACKREV1 lpCallBack,
                            LPN30          lpsN30,
                            LPTSTR         lpszFileName,
                            LPBYTE         lpbyWorkBuffer)
{
    DWORD   dwOrigEP;
    WORD    wOrigOffset,
            wVirOffset;
    LONG    lSeekLen;
    HFILE   hFile;
    UINT    uAttr;


    if (RepairFileOpen(lpCallBack, lpszFileName, &hFile, &uAttr))
    {
        return(EXTSTATUS_NO_REPAIR);
    }

    if ((DWORD)-1 == lpCallBack->FileSeek(hFile, -6, SEEK_END))
    {
        RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
        return(EXTSTATUS_NO_REPAIR);
    }


    if ((UINT) -1 == lpCallBack->FileRead(hFile, (LPBYTE)&wOrigOffset, 2))
    {
        RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
        return(EXTSTATUS_NO_REPAIR);
    }

    wOrigOffset = WENDIAN(wOrigOffset) - 0x103;
    wOrigOffset = WENDIAN(wOrigOffset);

    if ((DWORD)-1 == lpCallBack->FileSeek(hFile, 1, SEEK_SET))
    {
        RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
        return(EXTSTATUS_NO_REPAIR);
    }
 
    if ((UINT)-1 == lpCallBack->FileWrite(hFile, (LPBYTE)&wOrigOffset, 2))
    {
        RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
        return(EXTSTATUS_NO_REPAIR);
    }


    lSeekLen = (LONG) WENDIAN(wOrigOffset) + 3;

    if ((DWORD)-1 == lpCallBack->FileSeek(hFile, lSeekLen, SEEK_SET))
    {
        RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
        return(EXTSTATUS_NO_REPAIR);
    }

    if ((UINT)-1 == lpCallBack->FileRead(hFile, (LPBYTE)&dwOrigEP, 4))
    {
        RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
        return(EXTSTATUS_NO_REPAIR);
    }

    if (DWENDIAN(dwOrigEP) == 0x5b0000e8)
    {
        if ((DWORD)-1 == lpCallBack->FileSeek(hFile, 0x6c, SEEK_CUR))
        {
            RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
            return(EXTSTATUS_NO_REPAIR);
        }
    }
    else
    {
        if ((DWORD)-1 == lpCallBack->FileSeek(hFile, (LONG)-8, SEEK_END))
        {
            RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
            return(EXTSTATUS_NO_REPAIR);
        }

        if ((UINT) -1 == lpCallBack->FileRead(hFile, (LPBYTE)&wVirOffset, 2))
        {
            RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
            return(EXTSTATUS_NO_REPAIR);
        }

        lSeekLen = (LONG) WENDIAN(wVirOffset);

        if ((DWORD)-1 == lpCallBack->FileSeek(hFile, lSeekLen, SEEK_SET))
        {
            RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
            return(EXTSTATUS_NO_REPAIR);
        }
    }

    if ((UINT)-1 == lpCallBack->FileWrite(hFile, NULL, 0))
    {
         RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
         return(EXTSTATUS_NO_REPAIR);
    }

    if (RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr))
    {
        return(EXTSTATUS_NO_REPAIR);
    }

    return (EXTSTATUS_OK);

}

// EXTRepairFileVacsina2 arguments:
//
// lpCallBack           : pointer to callback structure for FileOpen, etc.
// lpsN30               : pointer to N30 structure containing VIRSCAN.DAT
//                        information and virus stats
// lpszFileName         : name of the file to repair
// lpbyWorkBuffer       : temporary 2-4K work buffer.
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK          if OK
//                        EXTSTATUS_NO_REPAIR   if unable to repair
//

WORD EXTRepairFileVacsina2 (LPCALLBACKREV1 lpCallBack,
                            LPN30          lpsN30,
                            LPTSTR         lpszFileName,
                            LPBYTE         lpbyWorkBuffer)
{
    BYTE    byBuffer[6], byHeader[4] = {'M','Z',0,0};
    WORD    wVirOffset,wImgSize;
    HFILE   hFile;
    UINT    uAttr;


    if (RepairFileOpen(lpCallBack, lpszFileName, &hFile, &uAttr))
    {
        return(EXTSTATUS_NO_REPAIR);
    }


    if ((UINT)-1 == lpCallBack->FileRead(hFile, (LPBYTE)byBuffer, 6))
    {
        RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
        return(EXTSTATUS_NO_REPAIR);
    }

    wVirOffset = AVDEREF_WORD(byBuffer + 1) - 0x11;
    wImgSize = (AVDEREF_WORD(byBuffer + 4) - 1) << 9;
    AVASSIGN_WORD((byHeader + 2), wVirOffset - wImgSize);
 
    if ((DWORD)-1 == lpCallBack->FileSeek(hFile, 0, SEEK_SET))
    {
        RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
        return(EXTSTATUS_NO_REPAIR);
    }

    if ((UINT)-1 == lpCallBack->FileWrite(hFile, (LPBYTE)byHeader, 4))
    {
        RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
        return(EXTSTATUS_NO_REPAIR);
    }

    if ((DWORD)-1 == lpCallBack->FileSeek(hFile, (LONG)wVirOffset, SEEK_SET))
    {
        RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
        return(EXTSTATUS_NO_REPAIR);
    }

    if ((UINT)-1 == lpCallBack->FileWrite(hFile, NULL, 0))
    {
         RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
         return(EXTSTATUS_NO_REPAIR);
    }

    if (RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr))
    {
        return(EXTSTATUS_NO_REPAIR);
    }

    return (EXTSTATUS_OK);
 
}

#endif

//
// *** #if-def'd out due to lack of memory in SYM_DOS ***
//
#if defined(SYM_DOSX) || !defined(SYM_DOS)

// EXTRepairFileI4060 arguments:
//
// lpCallBack           : pointer to callback structure for FileOpen, etc.
// lpsN30               : pointer to N30 structure containing VIRSCAN.DAT
//                        information and virus stats
// lpszFileName         : name of the file to repair
// lpbyWorkBuffer       : temporary 2-4K work buffer.
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK          if OK
//                        EXTSTATUS_NO_REPAIR   if unable to repair
//

WORD EXTRepairFileI4060 (LPCALLBACKREV1 lpCallBack,
                          LPN30           lpsN30,
                          LPTSTR          lpszFileName,
                          LPBYTE         lpbyWorkBuffer)
{


    HFILE   hFile;
    UINT    uAttr;
    LPBYTE  lpbyDecrByte;
    BYTE    byVirusBuffer[4096];
    WORD    i, wKey, wDecrByteOff = 0;

    (void)lpbyWorkBuffer;

                                            // Open file

    if (RepairFileOpen(lpCallBack, lpszFileName, &hFile, &uAttr))
    {
        return(EXTSTATUS_NO_REPAIR);
    }
                                        // Get repair info
                                        // Seek to EOF - 4060

    if ((DWORD)-1 == lpCallBack->FileSeek(hFile, -4060, SEEK_END))
    {
        RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
        return(EXTSTATUS_NO_REPAIR);
    }

                                        // Read 4060 bytes

    if ((UINT) -1 == lpCallBack->FileRead(hFile, (LPBYTE)byVirusBuffer, 4060))
    {
        RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
        return(EXTSTATUS_NO_REPAIR);
    }

                                        // Get initial encryption key
    for (i=0;i<26;i++)
    {
        if ((WORD) 0xbd0e == AVDEREF_WORD(byVirusBuffer + i))
        {
            wKey = AVDEREF_WORD(byVirusBuffer + i + 2);
            break;
        }
    }

    if (i == 26)
    {
        RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
        return(EXTSTATUS_NO_REPAIR);
    }

                                        // Set pointer to bytes in viral
                                        // body used to generate encryption key.

    lpbyDecrByte = byVirusBuffer + i + 4;

    for (i=0;i<4038;i++)
    {
        wDecrByteOff += 512;
        if (wDecrByteOff >= 4035)
            wDecrByteOff -= 4035;


        wKey ^= AVDEREF_WORD(lpbyDecrByte + wDecrByteOff);
    }

                                        // Decrypt viral body
    for (i=0;i<1938;i++)
    {
        // ((LPWORD)lpbyDecrByte)[28 + i] ^= wKey;
        lpbyDecrByte[2 * (28 + i)] ^= (wKey & 0xFF);
        lpbyDecrByte[2 * (28 + i) + 1] ^= ((wKey >> 8) & 0xFF);

                                        // rol wKey
        wKey = (wKey << 1) | ((wKey >> ((sizeof(wKey) * 8) - 1)) & 1);
    }

                                        // Decrypt repair bytes
    for (i=0;i<26;i++)
    {
        lpbyDecrByte[3964 + i] ^= lpbyDecrByte[3275];
    }

                                        // Seek TOF
    if ((DWORD)-1 == lpCallBack->FileSeek(hFile, 0, SEEK_SET))
    {
        RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
        return(EXTSTATUS_NO_REPAIR);
    }

                                        // EXE or COM ?
    if (0 != lpbyDecrByte[3946])
    {

                                        // Exe: Write 26 header bytes

        if ((UINT) -1 == lpCallBack->FileWrite(hFile, lpbyDecrByte + 3964, 26))
        {
            RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
            return(EXTSTATUS_NO_REPAIR);
        }
    }
    else
    {
                                        // COM repair

        if ((UINT) -1 == lpCallBack->FileWrite(hFile, lpbyDecrByte + 3964, 7))
        {
            RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
            return(EXTSTATUS_NO_REPAIR);
        }
    }
                                        // Truncate the file

    if (RepairFileTruncate(lpCallBack, hFile, 4060))
    {
        RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
        return(EXTSTATUS_NO_REPAIR);
    }

                                        // Close file

    if (RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr))
    {
        return(EXTSTATUS_NO_REPAIR);
    }
                                        // Return success
    return (EXTSTATUS_OK);
}
#endif


// EXTRepairFileLittleR arguments:
//
// lpCallBack           : pointer to callback structure for FileOpen, etc.
// lpsN30               : pointer to N30 structure containing VIRSCAN.DAT
//                        information and virus stats
// lpszFileName         : name of the file to repair
// lpbyWorkBuffer       : temporary 2-4K work buffer.
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK          if OK
//                        EXTSTATUS_NO_REPAIR   if unable to repair
//

WORD EXTRepairFileLittleR (LPCALLBACKREV1 lpCallBack,
                          LPN30           lpsN30,
                          LPTSTR          lpszFileName,
                          LPBYTE         lpbyWorkBuffer)
{


    HFILE   hFile;
    UINT    uAttr;
    BYTE    byDecryptKey, byIndex = 0;
    WORD    wPartPage, wPageCnt;
    DWORD   dwFileSize;
                                            // Open file

    if (RepairFileOpen(lpCallBack, lpszFileName, &hFile, &uAttr))
    {
        return(EXTSTATUS_NO_REPAIR);
    }

                                        // Seek TOF
    if ((DWORD)-1 == lpCallBack->FileSeek(hFile, 0, SEEK_SET))
    {
        RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
        return(EXTSTATUS_NO_REPAIR);
    }

                                        // Read 20h bytes
    if ((UINT) -1 == lpCallBack->FileRead(hFile, lpbyWorkBuffer + 0x100, 0x20))
    {
        RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
        return(EXTSTATUS_NO_REPAIR);
    }

                                        // Get repair info
                                        // Seek to EOF - 151

    if ((DWORD)-1 == lpCallBack->FileSeek(hFile, -151, SEEK_END))
    {
        RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
        return(EXTSTATUS_NO_REPAIR);
    }

                                        // Read 12 bytes

    if ((UINT) -1 == lpCallBack->FileRead(hFile, lpbyWorkBuffer, 12))
    {
        RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
        return(EXTSTATUS_NO_REPAIR);
    }

                                        // check for bookmark value 0xc3c1
    if ( AVDEREF_WORD(lpbyWorkBuffer) != 0xc3c1)
    {
        RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
        return(EXTSTATUS_NO_REPAIR);
    }

                                        // E9, MZ, ZM or bust
    if (lpbyWorkBuffer[0x100] == 0xe9)
    {

                                        // Com Repairs
        for (byDecryptKey= 5;byDecryptKey > 0;byDecryptKey--)
        {
            lpbyWorkBuffer[0x100 + byIndex] =
                lpbyWorkBuffer[2 + byIndex] ^ byDecryptKey;
            byIndex++;
        }
    }
    else if(lpbyWorkBuffer[0x100] == 0x4d ||
            lpbyWorkBuffer[0x100] == 0x5a)
    {
                                        // Exe Repairs
        for (byDecryptKey = 10;byDecryptKey > 0;byDecryptKey--)
        {
            lpbyWorkBuffer[0x10e + byIndex] =
                lpbyWorkBuffer[2 + byIndex] ^ byDecryptKey;
            byIndex++;
        }
                                        // Adjust image size in exe header

        if ((DWORD)-1 == (dwFileSize = lpCallBack->FileSeek(hFile, -1465, SEEK_END)))
        {
            RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
            return(EXTSTATUS_NO_REPAIR);
        }

        // (*(LPWORD)(lpbyWorkBuffer + 0x100 + 4)) = (WORD) (dwFileSize / 512);
        wPageCnt = (WORD) (dwFileSize / 512);
        wPartPage = (WORD) (dwFileSize % 512);
        if (0 != wPartPage)
            ++wPageCnt;

        AVASSIGN_WORD(lpbyWorkBuffer + 0x100 + 4,wPageCnt);
        AVASSIGN_WORD(lpbyWorkBuffer + 0x100 + 2,wPartPage);
    }
    else
    {
        RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
        return(EXTSTATUS_NO_REPAIR);
    }


                                          // Seek TOF
    if ((DWORD)-1 == lpCallBack->FileSeek(hFile, 0, SEEK_SET))
    {
        RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
        return(EXTSTATUS_NO_REPAIR);
    }

                                        // Write back repair header
    if ((UINT) -1 == lpCallBack->FileWrite(hFile, lpbyWorkBuffer+0x100, 0x20))
    {
        RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
        return(EXTSTATUS_NO_REPAIR);
    }

                                        // Truncate the file

    if (RepairFileTruncate(lpCallBack, hFile, 1465))
    {
        RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
        return(EXTSTATUS_NO_REPAIR);
    }

                                        // Close file

    if (RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr))
    {
        return(EXTSTATUS_NO_REPAIR);
    }
                                        // Return success
    return (EXTSTATUS_OK);
}

// EXTRepairFileQuicky arguments:
//
// lpCallBack           : pointer to callback structure for FileOpen, etc.
// lpsN30               : pointer to N30 structure containing VIRSCAN.DAT
//                        information and virus stats
// lpszFileName         : name of the file to repair
// lpbyWorkBuffer       : temporary 2-4K work buffer.
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK          if OK
//                        EXTSTATUS_NO_REPAIR   if unable to repair
//

WORD EXTRepairFileQuicky (LPCALLBACKREV1 lpCallBack,
                          LPN30           lpsN30,
                          LPTSTR          lpszFileName,
                          LPBYTE         lpbyWorkBuffer)
{
    HFILE   hFile;
    UINT    uAttr;
    WORD    wTemp;
    WORD    wNumPadBytes;
    DWORD   dwFileSize;
                                        // Open file

    if (RepairFileOpen(lpCallBack, lpszFileName, &hFile, &uAttr))
    {
        return(EXTSTATUS_NO_REPAIR);
    }
                                        // Get repair info
                                        // Seek to EOF - 1376

    if ((DWORD)-1 == (dwFileSize = lpCallBack->FileSeek(hFile, -1376, SEEK_END)))
    {
        RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
        return(EXTSTATUS_NO_REPAIR);
    }

                                        // Virus pads to paragraph boundaries.
    wNumPadBytes = (WORD)dwFileSize & 0x0f;


                                        // Read 1376 bytes

    if ((UINT) -1 == lpCallBack->FileRead(hFile, lpbyWorkBuffer, 1376))
    {
        RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
        return(EXTSTATUS_NO_REPAIR);
    }

                                        // Seek to TOF

    if ((DWORD)-1 == lpCallBack->FileSeek(hFile, 0, SEEK_SET))
    {
        RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
        return(EXTSTATUS_NO_REPAIR);
    }
                                        // Read 26 header bytes

    if ((UINT) -1 == lpCallBack->FileRead(hFile, lpbyWorkBuffer + 1376, 26))
    {
        RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
        return(EXTSTATUS_NO_REPAIR);
    }
                                        // IP
    // *(LPWORD)(lpbyWorkBuffer + 1376 + 20) =
    //     *(LPWORD)(lpbyWorkBuffer - wNumPadBytes + 29);
    AVASSIGN_WORD(lpbyWorkBuffer + 1376 + 20,
        AVDEREF_WORD(lpbyWorkBuffer - wNumPadBytes + 29));

                                        // SP
    // *(LPWORD)(lpbyWorkBuffer + 1376 + 16) =
    //     *(LPWORD)(lpbyWorkBuffer - wNumPadBytes + 33);
    AVASSIGN_WORD(lpbyWorkBuffer + 1376 + 16,
        AVDEREF_WORD(lpbyWorkBuffer - wNumPadBytes + 33));

                                        // CS

    // *(LPWORD)(lpbyWorkBuffer + 1376 + 22) +=
    //     *(LPWORD)(lpbyWorkBuffer - wNumPadBytes + 31);
    wTemp = AVDEREF_WORD(lpbyWorkBuffer + 1376 + 22);
    wTemp += AVDEREF_WORD(lpbyWorkBuffer - wNumPadBytes + 31);
    AVASSIGN_WORD(lpbyWorkBuffer + 1376 + 22,wTemp);

                                        // SS
    // *(LPWORD)(lpbyWorkBuffer + 1376 + 14) +=
    //     *(LPWORD)(lpbyWorkBuffer - wNumPadBytes + 35);
    wTemp = AVDEREF_WORD(lpbyWorkBuffer + 1376 + 14);
    wTemp += AVDEREF_WORD(lpbyWorkBuffer - wNumPadBytes + 35);
    AVASSIGN_WORD(lpbyWorkBuffer + 1376 + 14,wTemp);


                                        // Image size

    // *(LPWORD)(lpbyWorkBuffer + 1376 + 4) = (WORD) (dwFileSize / 512);
    // if (0 != (*(LPWORD)(lpbyWorkBuffer + 1376 + 2) = (WORD)(dwFileSize % 512)))
    // {
    //     (*(LPWORD)(lpbyWorkBuffer + 1376 + 4))++;
    // }

    wTemp = (WORD)(dwFileSize / 512);
    AVASSIGN_WORD(lpbyWorkBuffer + 1376 + 2,(WORD)(dwFileSize % 512));
    if (0 != (WORD)(dwFileSize % 512))
    {
        wTemp++;
    }
    AVASSIGN_WORD(lpbyWorkBuffer + 1376 + 4,wTemp);

                                        // Seek to TOF

    if ((DWORD)-1 == lpCallBack->FileSeek(hFile, 0, SEEK_SET))
    {
        RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
        return(EXTSTATUS_NO_REPAIR);
    }

                                        // Write 26 header bytes

    if ((UINT) -1 == lpCallBack->FileWrite(hFile, lpbyWorkBuffer + 1376, 26))
    {
        RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
        return(EXTSTATUS_NO_REPAIR);
    }

                                        // Truncate the file

    if (RepairFileTruncate(lpCallBack, hFile, 1376))
    {
        RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
        return(EXTSTATUS_NO_REPAIR);
    }

                                        // Close file

    if (RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr))
    {
        return(EXTSTATUS_NO_REPAIR);
    }
                                        // Return success
    return (EXTSTATUS_OK);
}
//
// *** #if-def'd out due to lack of memory in SYM_DOS ***
//
#if defined(SYM_DOSX) || !defined(SYM_DOS)

// EXTRepairFileCPW arguments:
//
// lpCallBack           : pointer to callback structure for FileOpen, etc.
// lpsN30               : pointer to N30 structure containing VIRSCAN.DAT
//                        information and virus stats
// lpszFileName         : name of the file to repair
// lpbyWorkBuffer       : temporary 2-4K work buffer.
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK          if OK
//                        EXTSTATUS_NO_REPAIR   if unable to repair
//

WORD EXTRepairFileCPW (LPCALLBACKREV1 lpCallBack,
                     LPN30           lpsN30,
                     LPTSTR          lpszFileName,
                     LPBYTE         lpbyWorkBuffer)
{
    HFILE   hFile;
    UINT    uAttr;
    WORD    i;
    BYTE    bKey;

                                        // Open file

    if (RepairFileOpen(lpCallBack, lpszFileName, &hFile, &uAttr))
    {
        return(EXTSTATUS_NO_REPAIR);
    }
                                        // Get repair info
                                        // Seek to EOF - 1527

    if ((DWORD)-1 == lpCallBack->FileSeek(hFile, -1527, SEEK_END))
    {
        RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
        return(EXTSTATUS_NO_REPAIR);
    }
                                        // Read 0x100 bytes

    if ((UINT) -1 == lpCallBack->FileRead(hFile, lpbyWorkBuffer + 0x100, 0x100))
    {
        RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
        return(EXTSTATUS_NO_REPAIR);
    }
                                        // Decrypt repair bytes for com and exe

    bKey = lpbyWorkBuffer[0x100 + 0x34];

    for (i=0x91;i<=0xb5;i++)
    {
        lpbyWorkBuffer[0x100 + i] = -lpbyWorkBuffer[0x100 + i];
        lpbyWorkBuffer[0x100 + i] += bKey;
    }
                                        // Seek to TOF

    if ((DWORD)-1 == lpCallBack->FileSeek(hFile, 0, SEEK_SET))
    {
        RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
        return(EXTSTATUS_NO_REPAIR);
    }
                                        // Read 24 bytes
                                        // Enough for either exe or com repair

    if ((UINT) -1 == lpCallBack->FileRead(hFile, lpbyWorkBuffer, 24))
    {
        RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
        return(EXTSTATUS_NO_REPAIR);
    }

                                        // Seek TOF

    if ((DWORD)-1 == lpCallBack->FileSeek(hFile, 0, SEEK_SET))
    {
        RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
        return(EXTSTATUS_NO_REPAIR);
    }

                                        // Is this an Exe or Com file?

    if (AVDEREF_WORD(lpbyWorkBuffer) == 0x5a4d ||
        AVDEREF_WORD(lpbyWorkBuffer) == 0x4d5a)
    {
        WORD    wPartPage, wPageCnt;

        // ((LPWORD)lpbyWorkBuffer)[1] += 9;
        //
        // if (((LPWORD)lpbyWorkBuffer)[1] >= 512)
        // {
        //     ((LPWORD)lpbyWorkBuffer)[1] %= 512;
        //     ((LPWORD)lpbyWorkBuffer)[2]++;
        // }
        // ((LPWORD)lpbyWorkBuffer)[2] -= 3;

                                        // Fix up image size for exe's

        wPartPage = AVDEREF_WORD(lpbyWorkBuffer + 2) + 9;
        wPageCnt = AVDEREF_WORD(lpbyWorkBuffer + 4);
        if (wPartPage >= 512)
        {
            wPartPage %= 512;
            ++wPageCnt;
        }
        wPageCnt -= 3;
        AVASSIGN_WORD(lpbyWorkBuffer+2,wPartPage);
        AVASSIGN_WORD(lpbyWorkBuffer+4,wPageCnt);

                                        // Repair the rest of exe header
        for (i=0;i<10;i++)
        {
            lpbyWorkBuffer[14 + i] = lpbyWorkBuffer[0x100 + 0x91 + i];
        }

                                        // Write the repair exe header to file

        if ((UINT) -1 == lpCallBack->FileWrite(hFile, lpbyWorkBuffer, 24))
        {
            RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
            return(EXTSTATUS_NO_REPAIR);
        }
    }
    else
    {
                                        // Com repair
        for (i=0;i<3;i++)
        {
            lpbyWorkBuffer[i] = lpbyWorkBuffer[0x100 + 0xb3 + i];
        }
                                        // Write the 3 repair bytes to file

        if ((UINT) -1 == lpCallBack->FileWrite(hFile, lpbyWorkBuffer, 3))
        {
            RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
            return(EXTSTATUS_NO_REPAIR);
        }
    }

                                        // Truncate the file

    if (RepairFileTruncate(lpCallBack, hFile, 1527))
    {
        RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
        return(EXTSTATUS_NO_REPAIR);
    }

                                        // Close file

    if (RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr))
    {
        return(EXTSTATUS_NO_REPAIR);
    }
                                        // Return success
    return (EXTSTATUS_OK);
}

#endif

// *** #if-def'd out due to lack of memory in SYM_DOS ***

#if defined(SYM_DOSX) || !defined(SYM_DOS)

// EXTRepairFileZarma arguments:
//
// lpCallBack           : pointer to callback structure for FileOpen, etc.
// lpsN30               : pointer to N30 structure containing VIRSCAN.DAT
//                        information and virus stats
// lpszFileName         : name of the file to repair
// lpbyWorkBuffer       : temporary 2-4K work buffer.
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK          if OK
//                        EXTSTATUS_NO_REPAIR   if unable to repair
//

WORD EXTRepairFileZarma (LPCALLBACKREV1 lpCallBack,
                     LPN30           lpsN30,
                     LPTSTR          lpszFileName,
                     LPBYTE         lpbyWorkBuffer)
{
    HFILE   hFile;
    UINT    uAttr;
    LPWORD  lpwRepairBuffer;            // lpbyWorkBuffer + 0x20
    WORD    i;
    BYTE    bKey = 0;

                                        // Open file

    if (RepairFileOpen(lpCallBack, lpszFileName, &hFile, &uAttr))
    {
        return(EXTSTATUS_NO_REPAIR);
    }
                                        // Get repair info
                                        // Seek to EOF - 1019

    if ((DWORD)-1 == lpCallBack->FileSeek(hFile, -1019, SEEK_END))
    {
        RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
        return(EXTSTATUS_NO_REPAIR);
    }
                                        // Read 0x20 bytes

    if ((UINT) -1 == lpCallBack->FileRead(hFile, lpbyWorkBuffer , 0x20))
    {
        RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
        return(EXTSTATUS_NO_REPAIR);
    }
                                        // Get decryption key
                                        // Virus always does byte encryption
                                        // First an add then xor both with the
                                        // same key.  We know underlying bytes
                                        // so we'll use brute force.

    for (i=0;i<257;i++)
    {
        if (( (BYTE) ((lpbyWorkBuffer[0] ^ i) - i) == (BYTE)0x73) &&
            ( (BYTE) ((lpbyWorkBuffer[6] ^ i) - i) == (BYTE)0xe3) &&
            ( (BYTE) ((lpbyWorkBuffer[7] ^ i) - i) == (BYTE)0x0e) &&
            ( (BYTE) ((lpbyWorkBuffer[8] ^ i) - i) == (BYTE)0x58) &&
            ( (BYTE) ((lpbyWorkBuffer[9] ^ i) - i) == (BYTE)0x2d) )
        {
            bKey = (BYTE)i;
            break;
        }
    }

    if (i == (WORD) 257)
    {
                                        // If we can't find the return fail.
        RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
        return(EXTSTATUS_NO_REPAIR);
    }
                                        // Use decryption key
    for (i=0;i<32;i++)
    {
        lpbyWorkBuffer[i] ^= bKey;
        lpbyWorkBuffer[i] -= bKey;
    }

                                        // Read in first 32 bytes of file.

    if ((DWORD)-1 == lpCallBack->FileSeek(hFile, 0, SEEK_SET))
    {
        RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
        return(EXTSTATUS_NO_REPAIR);
    }
                                        // Read 0x20 bytes

    if ((UINT) -1 == lpCallBack->FileRead(hFile, lpbyWorkBuffer + 0x20, 0x20))
    {
        RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
        return(EXTSTATUS_NO_REPAIR);
    }


    lpwRepairBuffer = (LPWORD)(lpbyWorkBuffer + 0x20);

                                        // EXE or COM ?
    if (AVDEREF_WORD(lpwRepairBuffer) == 0x5a4d ||
        AVDEREF_WORD(lpwRepairBuffer) == 0x4d5a)
    {
        WORD wPartPage, wPageCnt, wReloCS;

                                        // Fix up image size for exe's
        // if (lpwRepairBuffer[1] >= 0x168)
        // {
        //     lpwRepairBuffer[1] -= 0x168;
        //     lpwRepairBuffer[2] -= 0x04;
        // }
        // else
        // {
        //     lpwRepairBuffer[1] += 0x200 - 0x168;
        //     lpwRepairBuffer[2] -= 0x05;
        // }

        wPartPage = AVDEREF_WORD(lpwRepairBuffer + 1);
        wPageCnt = AVDEREF_WORD(lpwRepairBuffer + 2);
        if (wPartPage >= 0x168)
        {
            wPartPage -= 0x168;
            wPageCnt -= 0x04;
        }
        else
        {
            wPartPage += 0x200 - 0x168;
            wPageCnt -= 0x05;
        }
        AVASSIGN_WORD(lpwRepairBuffer + 1,wPartPage);
        AVASSIGN_WORD(lpwRepairBuffer + 2,wPageCnt);

                                        // Rebuild Exe header

                                        // IP
        // lpwRepairBuffer[10] = *(LPWORD)(lpbyWorkBuffer + 15);
        AVASSIGN_WORD(lpwRepairBuffer+10,AVDEREF_WORD(lpbyWorkBuffer + 15));

                                        // SP
        // lpwRepairBuffer[8] = *(LPWORD)(lpbyWorkBuffer + 18);
        AVASSIGN_WORD(lpwRepairBuffer+8,AVDEREF_WORD(lpbyWorkBuffer + 18));

                                        // SS

        // lpwRepairBuffer[7] = lpwRepairBuffer[11] - *(LPWORD)(lpbyWorkBuffer+23);
        wReloCS = AVDEREF_WORD(lpwRepairBuffer + 11);
        AVASSIGN_WORD(lpwRepairBuffer+7,
            wReloCS - AVDEREF_WORD(lpbyWorkBuffer + 23));

                                        // CS
        // lpwRepairBuffer[11] -= *(LPWORD)(lpbyWorkBuffer + 10);
        wReloCS -= AVDEREF_WORD(lpbyWorkBuffer + 10);
        AVASSIGN_WORD(lpwRepairBuffer+11,wReloCS);

    }
    else
    {
                                        // COM repair

                                        // Fix the destination of the
                                        // jump at TOF.
                                        // (Virus only infects E9 com files.)

        // *(LPWORD)(lpbyWorkBuffer + 0x21) = *(LPWORD)(lpbyWorkBuffer + 3) - 0x103;
        AVASSIGN_WORD(lpbyWorkBuffer + 0x21,
            AVDEREF_WORD(lpbyWorkBuffer + 3) - 0x103);
    }

                                        // Write repair info

                                        // Seek to TOF

    if ((DWORD)-1 == lpCallBack->FileSeek(hFile, 0, SEEK_SET))
    {
        RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
        return(EXTSTATUS_NO_REPAIR);
    }

                                        // Write 32 bytes to TOF

    if ((UINT) -1 == lpCallBack->FileWrite(hFile,
                                           (LPBYTE) lpwRepairBuffer,
                                           32))
    {
        RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
        return(EXTSTATUS_NO_REPAIR);
    }

                                        // Truncate the file

    if (RepairFileTruncate(lpCallBack, hFile, 2408))
    {
        RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
        return(EXTSTATUS_NO_REPAIR);
    }

                                        // Close file

    if (RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr))
    {
        return(EXTSTATUS_NO_REPAIR);
    }
                                        // Return success
    return (EXTSTATUS_OK);
}
#endif

//
// *** #if-def'd out due to lack of memory in SYM_DOS ***
//
#if defined(SYM_DOSX) || !defined(SYM_DOS)

// EXTRepairFileFairz arguments:
//
// lpCallBack           : pointer to callback structure for FileOpen, etc.
// lpsN30               : pointer to N30 structure containing VIRSCAN.DAT
//                        information and virus stats
// lpszFileName         : name of the file to repair
// lpbyWorkBuffer       : temporary 2-4K work buffer.
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK          if OK
//                        EXTSTATUS_NO_REPAIR   if unable to repair
// Notes:
//
//     Sloppy repair, leaves the paragraph padding at the end of hosts.

WORD EXTRepairFileFairz (LPCALLBACKREV1 lpCallBack,
                     LPN30           lpsN30,
                     LPTSTR          lpszFileName,
                     LPBYTE         lpbyWorkBuffer)
{
    HFILE   hFile;
    UINT    uAttr;
    WORD    i, wRepairBytes = 24;
    WORD    wBookMark_2090 = 0x071f, wBookMark_2340 = 0x7abe;
    UINT    uVariant;

                                        // Open file

    if (RepairFileOpen(lpCallBack, lpszFileName, &hFile, &uAttr))
    {
        return(EXTSTATUS_NO_REPAIR);
    }
                                        //Differentiate variants by bookmarks,
                                        //located 45 bytes from end-of-file

    if ((DWORD)-1 == lpCallBack->FileSeek(hFile, -0x2d, SEEK_END))
    {
        RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
        return(EXTSTATUS_NO_REPAIR);
    }
                                        //Read 2-byte Bookmark

    if ((UINT) -1 == lpCallBack->FileRead(hFile, lpbyWorkBuffer, 2))
    {
        RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
        return(EXTSTATUS_NO_REPAIR);
    }

                                        //Set variant size

    if (wBookMark_2090 == AVDEREF_WORD(lpbyWorkBuffer))
        uVariant = 2090;
    else if (wBookMark_2340 == AVDEREF_WORD(lpbyWorkBuffer))
        uVariant = 2340;
    else
    {
        RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
        return(EXTSTATUS_NO_REPAIR);
    }
                                        // Get repair info
                                        // Seek to EOF - 0x330 for 2090
                                        // Seek to EOF - 0x2dd for 2340
    if (2090 == uVariant)
    {
        if ((DWORD)-1 == lpCallBack->FileSeek(hFile, -0x330, SEEK_END))
        {
            RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
            return(EXTSTATUS_NO_REPAIR);
        }
    }
    else if (2340 == uVariant)
    {
        if ((DWORD)-1 == lpCallBack->FileSeek(hFile, -0x2dd, SEEK_END))
        {
            RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
            return(EXTSTATUS_NO_REPAIR);
        }
    }

                                        // Read 30 bytes

    if ((UINT) -1 == lpCallBack->FileRead(hFile, lpbyWorkBuffer, 30))
    {
        RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
        return(EXTSTATUS_NO_REPAIR);
    }
                                        // Do simple xor decryption
                                        // Xor key is previous word.
    for (i=28;i>1;i--)
    {
        lpbyWorkBuffer[i] ^= lpbyWorkBuffer[i - 2];
    }
                                        // Determine com or exe repair?
                                        // Seek to TOF

    if ((DWORD)-1 == lpCallBack->FileSeek(hFile, 0, SEEK_SET))
    {
        RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
        return(EXTSTATUS_NO_REPAIR);
    }
                                        // Read 2 bytes

    if ((UINT) -1 == lpCallBack->FileRead(hFile, lpbyWorkBuffer + 100, 2))
    {
        RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
        return(EXTSTATUS_NO_REPAIR);
    }
                                        // Exe's need an MZ at the start of
                                        // repair info and 26 bytes written
                                        // instead of 24.

    if (AVDEREF_WORD(lpbyWorkBuffer + 100) == 0x5a4d ||
        AVDEREF_WORD(lpbyWorkBuffer + 100) == 0x4d5a)
    {
        AVASSIGN_WORD(lpbyWorkBuffer + 3,0x5a4d);
        wRepairBytes = 26;
    }
                                        // Seek to TOF

    if ((DWORD)-1 == lpCallBack->FileSeek(hFile, 0, SEEK_SET))
    {
        RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
        return(EXTSTATUS_NO_REPAIR);
    }

                                        // Write 24 bytes to TOF for com's
                                        // or 26 bytes for exe's.
    if ((UINT) -1 == lpCallBack->FileWrite(hFile,
                                           lpbyWorkBuffer + 3,
                                           wRepairBytes))
    {
        RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
        return(EXTSTATUS_NO_REPAIR);
    }
                                        // Truncate the file
                                        // 0x827 for the 2090 variant
                                        // 0x924 for the 2340 variant

    if (2090 == uVariant)
    {
        if (RepairFileTruncate(lpCallBack, hFile, 0x827))
        {
            RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
            return(EXTSTATUS_NO_REPAIR);
        }
    }
    else if (2340 == uVariant)
    {
        if (RepairFileTruncate(lpCallBack, hFile, 0x924))
        {
            RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
            return(EXTSTATUS_NO_REPAIR);
        }
    }
                                        // Close file

    if (RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr))
    {
        return(EXTSTATUS_NO_REPAIR);
    }
                                        // Return success
    return (EXTSTATUS_OK);



}
#endif

// EXTRepairFileDH arguments:
//
// lpCallBack           : pointer to callback structure for FileOpen, etc.
// lpsN30               : pointer to N30 structure containing VIRSCAN.DAT
//                        information and virus stats
// lpszFileName         : name of the file to repair
// lpbyWorkBuffer       : temporary 2-4K work buffer.
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK          if OK
//                        EXTSTATUS_NO_REPAIR   if unable to repair

WORD EXTRepairFileDH(LPCALLBACKREV1 lpCallBack,
                     LPN30           lpsN30,
                     LPTSTR          lpszFileName,
                     LPBYTE         lpbyWorkBuffer)
{
    HFILE   hFile;
    UINT    uAttr;
    WORD    i;

                                        // Open file

    if (RepairFileOpen(lpCallBack, lpszFileName, &hFile, &uAttr))
    {
        return(EXTSTATUS_NO_REPAIR);
    }

                                        // Seek to EOF - 3898

    if ((DWORD)-1 == lpCallBack->FileSeek(hFile, -3898, SEEK_END))
    {
        RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
        return(EXTSTATUS_NO_REPAIR);
    }

                                        // Read 24 bytes

    if ((UINT) -1 == lpCallBack->FileRead(hFile, lpbyWorkBuffer, 24))
    {
        RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
        return(EXTSTATUS_NO_REPAIR);
    }
                                        // Do simple not decryption
    for (i=0;i<24;i++)
    {
        lpbyWorkBuffer[i] = ~lpbyWorkBuffer[i];
    }
                                        // Seek to TOF

    if ((DWORD)-1 == lpCallBack->FileSeek(hFile, 0, SEEK_SET))
    {
        RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
        return(EXTSTATUS_NO_REPAIR);
    }

                                        // Write 24 bytes to TOF

    if ((UINT) -1 == lpCallBack->FileWrite(hFile, lpbyWorkBuffer, 24))
    {
        RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
        return(EXTSTATUS_NO_REPAIR);
    }

                                        // Seek to EOF - 4000

    if ((DWORD)-1 == lpCallBack->FileSeek(hFile, -4000, SEEK_END))
    {
        RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
        return(EXTSTATUS_NO_REPAIR);
    }

                                        // Truncate file

    if ((UINT) -1 == lpCallBack->FileWrite(hFile, lpbyWorkBuffer, 0))
    {
        RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
        return(EXTSTATUS_NO_REPAIR);
    }

                                        // Close file

    if (RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr))
    {
        return(EXTSTATUS_NO_REPAIR);
    }
                                        // Return success
    return (EXTSTATUS_OK);
}

//
// *** #if-def'd out due to lack of memory in SYM_DOS ***
//
#if defined(SYM_DOSX) || !defined(SYM_DOS)

// EXTRepairFileYesmile arguments:
//
// Purpose: Remove 5504 bytes from the TOF to repair exe prepending virus.
// Note: It is now modified to remove [infection_size] bytes from the TOF.
//       Modified 4/2/97 AOONWAL
//
//
// lpCallBack           : pointer to callback structure for FileOpen, etc.
// lpsN30               : pointer to N30 structure containing VIRSCAN.DAT
//                        information and virus stats
// lpszFileName         : name of the file to repair
// lpbyWorkBuffer       : temporary 2-4K work buffer.
//
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK          if OK
//                        EXTSTATUS_NO_REPAIR   if unable to repair

#define YS_MAX_FILE_SIZE 0x70000000
#define YS_BUFFER_SIZE 0x800

WORD EXTRepairFileYesmile(LPCALLBACKREV1 lpCallBack,
                          LPN30           lpsN30,
                          LPTSTR          lpszFileName,
                          LPBYTE         lpbyWorkBuffer)
{
    HFILE   hFile;
    UINT    uAttr, cuRead;
    DWORD   dwTargetOfs = 0, dwSourceOfs = 0 ;
    DWORD   dwReturn;

    dwSourceOfs += lpsN30->wInfSize;
                                        // Open file

    if (RepairFileOpen(lpCallBack, lpszFileName, &hFile, &uAttr))
    {
        return(EXTSTATUS_NO_REPAIR);
    }

    do
    {
                                        // Seek to Source ptr
        if ((DWORD)-1 ==
            (dwReturn = lpCallBack->FileSeek(hFile, dwSourceOfs, SEEK_SET)))
        {
            RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
            return(EXTSTATUS_NO_REPAIR);
        }
        if (dwReturn > YS_MAX_FILE_SIZE)
        {
            // We expect EOF by now, an error has occurred.
            // 0x7fffffff => a 1.8GB file.
            RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
            return(EXTSTATUS_NO_REPAIR);
        }
        dwSourceOfs += YS_BUFFER_SIZE;

                                            // Read YS_BUFFER_SIZE (800h) bytes

        if ((UINT) -1 ==
            (cuRead = lpCallBack->FileRead(hFile, lpbyWorkBuffer, YS_BUFFER_SIZE)))
        {
            RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
            return(EXTSTATUS_NO_REPAIR);
        }


                                            // Seek to Target Offset

        if ((DWORD)-1 == lpCallBack->FileSeek(hFile, dwTargetOfs, SEEK_SET))
        {
            RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
            return(EXTSTATUS_NO_REPAIR);
        }
        dwTargetOfs += YS_BUFFER_SIZE;
                                            // Write cuRead bytes to Target offset

        if ((UINT) -1 == lpCallBack->FileWrite(hFile, lpbyWorkBuffer, cuRead))
        {
            RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
            return(EXTSTATUS_NO_REPAIR);
        }
    }
    while (cuRead == YS_BUFFER_SIZE);


                                        // Truncate file

    if ((UINT) -1 == lpCallBack->FileWrite(hFile, lpbyWorkBuffer, 0))
    {
        RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
        return(EXTSTATUS_NO_REPAIR);
    }

                                        // Close file

    if (RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr))
    {
        return(EXTSTATUS_NO_REPAIR);
    }
                                        // Return success
    return (EXTSTATUS_OK);
}
#endif

//
// *** #if-def'd out due to lack of memory in SYM_DOS ***
//
#if defined(SYM_DOSX) || !defined(SYM_DOS)

// EXTRepairFileUnknown arguments:
//
// Purpose: Rebuild EXE header or 1st three bytes of COM file, and chop off
//          the virus.
//
// lpCallBack           : pointer to callback structure for FileOpen, etc.
// lpsN30               : pointer to N30 structure containing VIRSCAN.DAT
//                        information and virus stats
// lpszFileName         : name of the file to repair
// lpbyWorkBuffer       : temporary 2-4K work buffer.
//
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK          if OK
//                        EXTSTATUS_NO_REPAIR   if unable to repair
//                        EXTSTATUS_FILE_ERROR  if file-handling error
//
// With some EXE files, the virus changes SS and SP in the EXE header w/o
// saving the original values, so we cannot fix reliably.  EXE file repair
// attempts will automatically return EXTSTATUS_NO_REPAIR.

#define HDR_BYTE_LEN    0x100           // Length of file ops for this fix
#define DEC_BYTE_LEN    0x200           // Length of file read to decrypt
#define JMP_SEARCH      4               // Max bytes to search for jmp in COM
#define SEARCH_START    0x140           // Offset into decrypt buf to search
#define MAX_SEARCH      0x28            // Maximum length to search for string
#define DATA_OFFSET     7               // Offset of fix data to search string
#define SEARCHLEN       0x0d            // Length of search string

#define XCHG(a, b)      {btmp = a; a = b; b = btmp;}

WORD EXTRepairFileUnknown(LPCALLBACKREV1 lpCallBack,
                          LPN30           lpsN30,
                          LPTSTR          lpszFileName,
                          LPBYTE         lpbyWorkBuffer)
{
    HFILE   hFile;
    UINT    uAttr, uFix1, uFix2, i, uCX;
    LPWORD  lpuBuf;
    DWORD   dwChop;                     // Chop here to remove virus
    BYTE    bAL, bAH, bDL, btmp;        // Register emulations
    BYTE    bSearcher[SEARCHLEN] = { 'U', 'N', 'K', 'N', 'O', 'W', 'N', ' ',
                            'V', 'i', 'r', 'u', 's'};   // "UNKNOWN Virus"
    // _asm    int 3                    // Remove after debugging! &&&
                                        // Open file
    if (0 != RepairFileOpen(lpCallBack, lpszFileName, &hFile, &uAttr))
        return(EXTSTATUS_FILE_ERROR);

    if (HDR_BYTE_LEN != lpCallBack->FileRead(hFile, lpbyWorkBuffer,
                                                                HDR_BYTE_LEN))
        CLOSEOUT(EXTSTATUS_FILE_ERROR);

    if (('M' == (char) *lpbyWorkBuffer) || ('Z' == (char) *lpbyWorkBuffer))
        CLOSEOUT(EXTSTATUS_NO_REPAIR);      // We won't try to fix EXE files

    for (i = 0; (0xe9 != lpbyWorkBuffer[i]) && (i < JMP_SEARCH); i++)
                        ;                   // Look for the JMP instruction

    if (JMP_SEARCH == i)                    // Bail if we didn't find the jmp
        CLOSEOUT(EXTSTATUS_NO_REPAIR);

    i++;                                    // Get jump displacement
    dwChop = (DWORD) AVDEREF_WORD(lpbyWorkBuffer + i);
    dwChop += (DWORD) i;                    // Add offset of jmp instruction
    dwChop += 2;                            // Fudge factor for jmp offsets

    if (dwChop != lpCallBack->FileSeek(hFile, (LONG) dwChop, SEEK_SET))
        CLOSEOUT(EXTSTATUS_FILE_ERROR);     // Go to beginning of virus

    uCX = DEC_BYTE_LEN;

    if (uCX != lpCallBack->FileRead(hFile, lpbyWorkBuffer, uCX))
        CLOSEOUT(EXTSTATUS_FILE_ERROR);     // Read the beginning of the virus

    bAH = lpbyWorkBuffer[1];                // Load decrypt byte    mov ah, ??
    bDL = bAH;                              //                      mov dl, ah

    i = (UINT) AVDEREF_WORD(lpbyWorkBuffer + 3);    // Get the starting offset
    i -= (UINT) dwChop;                     // Compensate for COM file offsets
    i -= 0x100;

    uCX -= i;               // Reduce count by number of bytes not decrypted

    do
    {
        bAL = lpbyWorkBuffer[i];            // lodsb    (SI == DI at entry)
        bAH ^= bAL;                         // xor      ah, al
        bAH -= bDL;                         // sub      ah, dl
        bDL = bAH;                          // mov      dl, ah
        XCHG(bAH, bAL);                     // xchg     ah, al
        lpbyWorkBuffer[i++] = bAL;          // stosb
    } while (0 < --uCX);                    // loop     do_statement

    i = ScanString((lpbyWorkBuffer + SEARCH_START), MAX_SEARCH, bSearcher,
                                                                SEARCHLEN);
    if (0xffff == i)
        CLOSEOUT(EXTSTATUS_NO_REPAIR);

    i += (SEARCH_START - DATA_OFFSET);      // Get offset of fix data

    uFix1 = AVDEREF_WORD(lpbyWorkBuffer + i);   // Get fix data
    i += 2;                                     // Point to next word
    uFix2 = AVDEREF_WORD(lpbyWorkBuffer + i);

    if (0 != lpCallBack->FileSeek(hFile, 0, SEEK_SET))
        CLOSEOUT(EXTSTATUS_FILE_ERROR);     // Go to beginning of file

    if (HDR_BYTE_LEN != lpCallBack->FileRead(hFile, lpbyWorkBuffer,
                                                                HDR_BYTE_LEN))
        CLOSEOUT(EXTSTATUS_FILE_ERROR);

    lpuBuf = (LPWORD) lpbyWorkBuffer;
    i = 0;

    // lpuBuf[i++] = uFix1;                        // Store fix values
    // lpuBuf[i] = uFix2;

    AVASSIGN_WORD(lpuBuf + i,uFix1);
    ++i;
    AVASSIGN_WORD(lpuBuf + i,uFix2);

    if (0 == lpCallBack->FileSeek(hFile, 0, SEEK_SET))  // Go to TOF
        if (HDR_BYTE_LEN == lpCallBack->FileWrite(hFile, lpbyWorkBuffer,
                                         HDR_BYTE_LEN)) // Write the fixes
            if (dwChop == lpCallBack->FileSeek(hFile, dwChop, SEEK_SET))
                if (0 == lpCallBack->FileWrite(hFile, lpbyWorkBuffer, 0))
                    if (0 == RepairFileClose(lpCallBack, lpszFileName, hFile,
                                                                    uAttr))
                        return(EXTSTATUS_OK);   // If we successfully went to
                                                // TOP, wrote the fixes,
                                                // chopped the virus, and
                                                // close the file, exit OK
    CLOSEOUT(EXTSTATUS_FILE_ERROR);             // Else we exit 'bad'.
}           // EXTRepairFileUnknown

#endif

//
// *** #if-def'd out due to lack of memory in SYM_DOS ***
//
#if defined(SYM_DOSX) || !defined(SYM_DOS)

// EXTRepairFileGodz arguments:
//
// Purpose: Rebuild 1st four bytes of COM file, and chop off the virus.
//
// lpCallBack           : pointer to callback structure for FileOpen, etc.
// lpsN30               : pointer to N30 structure containing VIRSCAN.DAT
//                        information and virus stats
// lpszFileName         : name of the file to repair
// lpbyWorkBuffer       : temporary 2-4K work buffer.
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK          if OK
//                        EXTSTATUS_NO_REPAIR   if unable to repair
//
// This virus only infects COM files, making my life much easier.

#define WORKBUFLEN  4           // Length of temporary work buffer
#define JMPOFFSET   3           // Add to jump offset to get file offset
#define FIXDATAOFF  0x329       // Offset of fix data from virus entry

WORD EXTRepairFileGodz(LPCALLBACKREV1   lpCallBack,
                        LPN30            lpsN30,
                        LPTSTR           lpszFileName,
                        LPBYTE          lpbyWorkBuffer)
{
    HFILE   hFile;
    UINT    uAttr;
    WORD    x;
    DWORD   dwChop, dwFixLoc;           // Chop here to remove virus
    BYTE    byFixData[WORKBUFLEN] = { 0x7d, 0x7e, 0x7f, 0x00 };
                                        // Fix data for decryption
    // _asm    int 3                    // Remove after debugging! &&G
                                        // Open file
    if (0 != RepairFileOpen(lpCallBack, lpszFileName, &hFile, &uAttr))
        return(EXTSTATUS_FILE_ERROR);

    if (WORKBUFLEN != lpCallBack->FileRead(hFile, lpbyWorkBuffer, WORKBUFLEN))
        CLOSEOUT(EXTSTATUS_FILE_ERROR);

    if (0xe9 != *lpbyWorkBuffer)                // Begin w/ JMP instruction?
        CLOSEOUT(EXTSTATUS_NO_REPAIR);

    dwChop = (DWORD) AVDEREF_WORD(lpbyWorkBuffer + 1);   // Get JMP disp.
    dwChop += JMPOFFSET;                             // Convert to file offset
    dwFixLoc = dwChop + FIXDATAOFF;                  // Get fix data location

    if (dwFixLoc != lpCallBack->FileSeek(hFile, dwFixLoc, SEEK_SET))
        CLOSEOUT(EXTSTATUS_FILE_ERROR);         // Go to encrypted fix bytes

    if (WORKBUFLEN != lpCallBack->FileRead(hFile, lpbyWorkBuffer, WORKBUFLEN))
        CLOSEOUT(EXTSTATUS_FILE_ERROR);         // Read encrypted fix bytes

    for (x = 0; x < WORKBUFLEN; x++)        // Decrypt original bytes from
        lpbyWorkBuffer[x] ^= byFixData[x];  // beginning of file

    if (0 == lpCallBack->FileSeek(hFile, 0, SEEK_SET))  // Go to TOF
        if (WORKBUFLEN == lpCallBack->FileWrite(hFile, lpbyWorkBuffer,
                                            WORKBUFLEN)) // Write the fixes
            if (dwChop == lpCallBack->FileSeek(hFile, dwChop, SEEK_SET))
                if (0 == lpCallBack->FileWrite(hFile, lpbyWorkBuffer, 0))
                    if (0 == RepairFileClose(lpCallBack, lpszFileName, hFile,
                                                                    uAttr))
                        return(EXTSTATUS_OK);   // If we successfully went to
                                                // TOP, wrote the fixes,
                                                // chopped the virus, and
                                                // close the file, exit OK
    CLOSEOUT(EXTSTATUS_FILE_ERROR);             // Else we exit 'bad'.
}           // EXTRepairFileGodz


// EXTRepairFileVaga arguments:                 &&V
//
// Purpose: Rebuild 1st four bytes of COM file, and chop off the virus.
//
// lpCallBack           : pointer to callback structure for FileOpen, etc.
// lpsN30               : pointer to N30 structure containing VIRSCAN.DAT
//                        information and virus stats
// lpszFileName         : name of the file to repair
// lpbyWorkBuffer       : temporary 2-4K work buffer.
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK          if OK
//                        EXTSTATUS_NO_REPAIR   if unable to repair
//                        EXTSTATUS_FILE_ERROR  if file or disk error
//
// This virus only infects COM files, making my life much easier.

#define WORKBUFLEN  4           // Length of work buffer
#define XOROFFSET   9           // Offset of XOR byte from beginning of body
#define JMPOFFSET   3           // Add to jump offset to get file offset
#define FIXDATALOC  0x333       // Offset of fix data from virus entry
#define VAGASIZE    1246        // Size of Vagabond infection

WORD EXTRepairFileVaga(LPCALLBACKREV1   lpCallBack,
                        LPN30            lpsN30,
                        LPTSTR           lpszFileName,
                        LPBYTE          lpbyWorkBuffer)
{
    HFILE   hFile;
    UINT    uAttr;
    WORD    x, x2;
    DWORD   dwChop;                     // Chop here to remove virus
    BYTE    byFixByte;                  // XOR Byte to fix with
    BYTE    byFixBuf[WORKBUFLEN];       // Place to hold fix bytes

    // _asm    int 3                    // Remove after debugging! &&V
                                        // Open file
    if (0 != RepairFileOpen(lpCallBack, lpszFileName, &hFile, &uAttr))
        return(EXTSTATUS_FILE_ERROR);

    if (WORKBUFLEN != lpCallBack->FileRead(hFile, lpbyWorkBuffer, WORKBUFLEN))
        CLOSEOUT(EXTSTATUS_FILE_ERROR);

    if (0xe9 != *lpbyWorkBuffer)                // Begin w/ JMP instruction?
        CLOSEOUT(EXTSTATUS_NO_REPAIR);

    dwChop = (DWORD) AVDEREF_WORD(lpbyWorkBuffer + 1);   // Get JMP disp.
    dwChop += JMPOFFSET;                             // Convert to file offset

    if (dwChop != lpCallBack->FileSeek(hFile, dwChop, SEEK_SET))
        CLOSEOUT(EXTSTATUS_FILE_ERROR);         // Go to virus body

    if (VAGASIZE != lpCallBack->FileRead(hFile, lpbyWorkBuffer, VAGASIZE))
        CLOSEOUT(EXTSTATUS_FILE_ERROR);         // Read virus body

    for (x = 0; ((0xbf != lpbyWorkBuffer[x]) && (x < VAGASIZE)); x++)
        ;   // Find beginning of virus "meat"

    if (VAGASIZE == x)                          // If we didn't find meat
        CLOSEOUT(EXTSTATUS_NO_REPAIR);          // We bail

    byFixByte = lpbyWorkBuffer[x + XOROFFSET];  // Get byte to XOR w/ fix data
    x += FIXDATALOC;                            // Point x to fix data

    for (x2 = 0; x2 < WORKBUFLEN; x2++)
        byFixBuf[x2] = lpbyWorkBuffer[x + x2] ^ byFixByte;

    if (0 == lpCallBack->FileSeek(hFile, 0, SEEK_SET))  // Go to TOF
        if (WORKBUFLEN == lpCallBack->FileWrite(hFile, byFixBuf, WORKBUFLEN))
            if (DWERROR != RepairFileTruncate(lpCallBack, hFile, VAGASIZE))
                if (0 == RepairFileClose(lpCallBack, lpszFileName, hFile,
                                                                    uAttr))
                    return(EXTSTATUS_OK);   // If we successfully went to TOF,
                                            // wrote the fixes, chopped the
                                            // virus, and closed the file,
                                            // exit OK
    CLOSEOUT(EXTSTATUS_FILE_ERROR);         // Else we exit 'bad'.
}           // EXTRepairFileVaga
#endif

//
// *** #if-def'd out due to lack of memory in SYM_DOS ***
//
#if defined(SYM_DOSX) || !defined(SYM_DOS)

// EXTRepairFileJackal arguments:                   &&J
//
// Purpose: Rebuild 1st four bytes of COM file, or copy 1st 28 bytes of EXE
//          header back to beginning of file and chop off the virus.
//
// lpCallBack           : pointer to callback structure for FileOpen, etc.
// lpsN30               : pointer to N30 structure containing VIRSCAN.DAT
//                        information and virus stats
// lpszFileName         : name of the file to repair
// lpbyWorkBuffer       : temporary 2-4K work buffer.
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK          if OK
//                        EXTSTATUS_NO_REPAIR   if unable to repair
//                        EXTSTATUS_FILE_ERROR  if file or disk error
//

#define COMBUFLEN   4           // Length of work buffer for COM files
#define EXEBUFLEN   28          // Length of work buffer for EXE files
#define JACKBOOK    0x1a        // Offset from entry point to check for strain
#define JACKREADLEN 0x20        // Amount to read from entry point

WORD EXTRepairFileJackal(LPCALLBACKREV1 lpCallBack,
                            LPN30       lpsN30,
                            LPTSTR      lpszFileName,
                            LPBYTE      lpbyWorkBuffer)
{
    HFILE   hFile;
    WORD    wJackSize;                  // Size of this infection
    UINT    uAttr, uBufLen;             // Attribute & file header length
    DWORD   dwFilePos;

    //_asm    int 3                     // Remove after debugging! &&J
                                        // Open file
    if (0 != FILEOPEN())
        return(EXTSTATUS_FILE_ERROR);

    if (0 != SEEK_TOF())  // Go to TOF
        CLOSEOUT(EXTSTATUS_FILE_ERROR);

    if (EXEBUFLEN != FILEREAD(lpbyWorkBuffer, EXEBUFLEN))
        CLOSEOUT(EXTSTATUS_FILE_ERROR);
                                            // Figure out needed buffer size
    uBufLen = (ISEXEHDR(lpbyWorkBuffer)) ? EXEBUFLEN : COMBUFLEN;
 
    dwFilePos = GetEntryPtOffset(lpbyWorkBuffer);       // Get entry point

    if (dwFilePos != FILESEEK(dwFilePos))               // Read bytes at
        CLOSEOUT(EXTSTATUS_FILE_ERROR);                 // entrypoint

    if (JACKREADLEN != FILEREAD(lpbyWorkBuffer, JACKREADLEN))
        CLOSEOUT(EXTSTATUS_FILE_ERROR);

    switch (lpbyWorkBuffer[JACKBOOK])   // Determine what strain we have
    {
        case 0x81 :
            wJackSize = 3118;
            break;
 
        case 0x83 :
            wJackSize = 3120;
            break;

        default :
            CLOSEOUT(EXTSTATUS_NO_REPAIR);
            break;
    }

    dwFilePos = FILESIZE();                 // Get file size

    if (DWERROR == dwFilePos)               // Error check
        CLOSEOUT(EXTSTATUS_FILE_ERROR);

    dwFilePos -= wJackSize;                 // Location of virus beginning

    if (0 != (dwFilePos & 0x0f))            // Pad dwFilePos up to the nearest
    {                                       // multiple of 16 if needed
        DWORD   tmp;

        tmp = (~dwFilePos & 0x0f) + 1;
        dwFilePos += tmp;
    }

    if (dwFilePos != FILESEEK(dwFilePos))
        CLOSEOUT(EXTSTATUS_FILE_ERROR);         // Go to original file header

    if (uBufLen != FILEREAD(lpbyWorkBuffer, uBufLen))
        CLOSEOUT(EXTSTATUS_FILE_ERROR);         // Read file header

    if (0 == SEEK_TOF())                        // Go to TOF
        if (uBufLen == FILEWRITE(lpbyWorkBuffer, uBufLen))
            if (DWERROR != FILETRUNCATE((long) wJackSize))
                if (0 == FILECLOSE())
                    return(EXTSTATUS_OK);   // If we successfully went to TOF,
                                            // wrote the fixes, chopped the
                                            // virus, and closed the file,
                                            // exit OK
    CLOSEOUT(EXTSTATUS_FILE_ERROR);         // Else we exit 'bad'.
}           // EXTRepairFileJackal
#endif

// Repair function for Emmie.2707 Emmie.2823 and Emmie.3097
// Original Asm created 7.17.95.  Ported to AVEX/C on 9.25.95

//
// *** #if-def'd out due to lack of memory in SYM_DOS ***
//
#if defined(SYM_DOSX) || !defined(SYM_DOS)

#define Emmie2702       0       //Just some defines to keep track of
#define Emmie2823       1       //which variant of Emmie I'm repairing
#define Emmie3097       2

WORD EXTRepairEmmie(LPCALLBACKREV1 lpCallBack,
                    LPN30          lpsN30,
                    LPTSTR         lpszFileName,
                    LPBYTE         WorkBuf)
{
  WORD          wTemp;
  WORD          Key=0, SeekLen=0, EmmieCallOffset=0, CallOffset;
  LONG          VarSeek;
  BYTE          SigBuf[0x10], WhichEmmie;
  BOOL          FoundEmmie=FALSE;
  int           i;
  HFILE         hFile;
  UINT          uAttr;


  // Open file for READ/WRITE and read in 0x13 bytes.  I read in
  // 0x13 bytes because I took an educated guess  that the largest valid
  // instruction is about 0x10 bytes (so I guess you could have an
  // instruction with like 50 CS overrides, but that would never happen
  // in the real word), plus 2 bytes for the offset of the call and
  // 1 byte just in case.

                                        // Open file

    if (RepairFileOpen(lpCallBack, lpszFileName, &hFile, &uAttr))
    {
        return(EXTSTATUS_NO_REPAIR);
    }


//
//  if((handle=lpCallBack->FileOpen(lpszFileName,READ_WRITE)) == -1)
//    return (EXTSTATUS_FILE_ERROR);
//


  if(lpCallBack->FileRead(hFile, WorkBuf, 0x13) == -1)
  {
    RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
    return(EXTSTATUS_NO_REPAIR);
  }


  // Check if first instruction is a call or jump.  If it is we must
  // follow it because that is where Emmie put the call to it's code.

  if(WorkBuf[0] == 0xeb || WorkBuf[0] == 0xe8 || WorkBuf[0] == 0xe9)
  {

    // An EB is a short jump, so we get the byte after the EB and add
    // 2 to it to get the offset from the TOF.
    // For E8 and E9 we get the word after it and add 3 to it to
    // get the offset from the TOF.

    if(WorkBuf[0] == 0xeb)
      SeekLen=(WORD)(*(LPBYTE)(WorkBuf+1) + 2);
    else
      SeekLen=AVDEREF_WORD(WorkBuf+1) + 3;


    // Seek to that offset and read in a new 0x13 byte buffer.

    if(lpCallBack->FileSeek(hFile, (LONG)SeekLen, SEEK_SET) == -1)
    {
        RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
        return(EXTSTATUS_NO_REPAIR);
    }

    if(lpCallBack->FileRead(hFile, WorkBuf, 0x13) == -1)
    {
        RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
        return(EXTSTATUS_NO_REPAIR);
    }
  }

  // Go through 0x10 bytes looking for an 0xe8.  When/if we find one
  // we must follow it and see if that is the call to Emmie's code.

  for(i=0; i < 0x10; i++)
  {
    if(WorkBuf[i] == 0xe8)
    {

      // WorkBuf+i+1 is the offset into the buffer of the word after
      // the 0xe8.  I add six because I want to look at the bytes 6
      // bytes after where this call points to.  CallOffset will be
      // the distance from either the TOF _or_ the location of the
      // jmp/call in the case where the host started off with one.

      CallOffset=AVDEREF_WORD(WorkBuf+i+1) + 6;


      // If the orrignal host started with some type of jmp/call
      // then we only need to subtract 0x13 bytes from CallOffset.
      // Otherwise we need to subtract (0x13 minus the offset from TOF
      // where the call is.

      if(SeekLen)
        CallOffset-=0x13;
      else
        CallOffset-=(0x13 - i);


      // Seek to this new location and read in 6 bytes that we will use
      // to verify whether or not we followed the right call and to
      // determine which varient of Emmie it is.

      if(lpCallBack->FileSeek(hFile, (LONG)CallOffset, SEEK_CUR) == -1)
      {
        RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
        return(EXTSTATUS_NO_REPAIR);
      }

      if(lpCallBack->FileRead(hFile, (LPBYTE) SigBuf, 6) == -1)
      {
        RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
        return(EXTSTATUS_NO_REPAIR);
      }


      // Here's where we do the verification.  If one of the 6 bytes
      // strings match we set WhichEmmie equal to one of those defines
      // I made up above, set FoundEmmie to true, and break out of the
      // loop.

      if(SigBuf[0] == 0x72 && SigBuf[1] == 0xfb
         && SigBuf[2] == 0x5f && SigBuf[3] == 0x83
         && SigBuf[4] == 0xef && SigBuf[5] == 0x03)
      {
        WhichEmmie=Emmie2702;
        FoundEmmie=TRUE;
        break;
      }

      if((SigBuf[0] == 0xe8 && SigBuf[1] == 0x00        // 2823.A
         && SigBuf[2] == 0x00 && SigBuf[3] == 0x5d
         && SigBuf[4] == 0xfc && SigBuf[5] == 0x8d) ||
        (SigBuf[0] == 0xe8 && SigBuf[1] == 0x00         // 2823.B
         && SigBuf[2] == 0x00 && SigBuf[3] == 0xfc
         && SigBuf[4] == 0x5d && SigBuf[5] == 0x8d))
      {
        WhichEmmie=Emmie2823;
        FoundEmmie=TRUE;
        break;
      }

      if(SigBuf[0] == 0xe8 && SigBuf[1] == 0x00
         && SigBuf[2] == 0x00 && SigBuf[3] == 0x5d
         && SigBuf[4] == 0xfa && SigBuf[5] == 0x8b)
      {
        WhichEmmie=Emmie3097;
        FoundEmmie=TRUE;
        break;
      }

      // If none of those strings matched it means that we followed a
      // bogus call (i.e. it was an 0xe8 that was a data byte and not
      // an actual call).  So we must reset the file pointer back to
      // 0x13 bytes from TOF.  We don't have to worry about the case
      // in which the host starts with a jmp/call because if that is
      // the case then Emmie will place it's call where that first jmp/call
      // points to so we will always follow the right call.

      if(lpCallBack->FileSeek(hFile, 0x13, SEEK_SET) == -1)
      {
        RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
        return(EXTSTATUS_NO_REPAIR);
      }
    }
  }

  // If we've come out of that loop and FoundEmmie is false, then it
  // means that there was a big booboo in detection or I need to scan
  // through more than 0x10 bytes.  But I really doubt the existance of
  // a valid instruction that is larger than 0x10 bytes!  If someone finds
  // one I guess I owe them a quarter (formy@cs.ucr.edu)

  if(!FoundEmmie)
  {
    RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
    return(EXTSTATUS_NO_REPAIR);
  }


  // EmmieCallOffset stores the offset of Emmie's call from where we read
  // in our 0x13 byte buffer.  In a normal call the will be form the TOF.
  // In the case where the host starts out with a jmp/call this will be 0.

  EmmieCallOffset=i;


  // In this section I need to forge a key to the decryption.  Emmie.2702
  // does not encrypt the original host bytes, so we just skip this
  // section of code if we are handling Emmie.2702.

  if(WhichEmmie!=Emmie2702)
  {

    // Do a seek to the proper location depending on which varient we
    // are dealing with and read in two bytes which are part of Emmie's
    // encrypted body.

    if(WhichEmmie==Emmie2823) VarSeek=0x0b;
    if(WhichEmmie==Emmie3097) VarSeek=0x0e;

    if(lpCallBack->FileSeek(hFile, VarSeek, SEEK_CUR) == -1)
    {
      RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
      return(EXTSTATUS_NO_REPAIR);
    }

    if(lpCallBack->FileRead(hFile, WorkBuf, 2) == -1)
    {
      RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
      return(EXTSTATUS_NO_REPAIR);
    }

    // To forge the key I just XOR the word I just read in with that
    // same word unencrypted.  Store this key for later use.

    if(WhichEmmie==Emmie2823)
      Key=AVDEREF_WORD(WorkBuf) ^ 0xf8e2;
    else
      Key=AVDEREF_WORD(WorkBuf) ^  0xf7e2;
  }


  // Now we are going to seek to where the orignal host bytes are stored.
  // Must put the proper offset in VarSeek depending  upon which varient
  // we have here.  Then I read in the three bytes that Emmie overwrote with
  // it's call.

  if(WhichEmmie==Emmie2702) VarSeek=-0x92;
  if(WhichEmmie==Emmie2823) VarSeek=0x95c;
  if(WhichEmmie==Emmie3097) VarSeek=0xa72;

  if(lpCallBack->FileSeek(hFile, VarSeek, SEEK_CUR) == -1)
  {
    RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
    return(EXTSTATUS_NO_REPAIR);
  }

  if(lpCallBack->FileRead(hFile, WorkBuf, 3) == -1)
  {
    RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
    return(EXTSTATUS_NO_REPAIR);
  }


  // Do the decrypt.  A simple XOR.  If we have Emmie.2702 it just get's
  // XOR'ed with 0 (that's what we initialized Key to be).

  // *(LPWORD)WorkBuf=Key ^ *(LPWORD)WorkBuf;
  wTemp = AVDEREF_WORD(WorkBuf);
  AVASSIGN_WORD(WorkBuf,wTemp ^ Key);
  *(WorkBuf+2)=(BYTE)Key ^ *(LPBYTE)(WorkBuf+2);


  // Now we seek to the location of Emmie's call.  This is calculated by
  // adding the distance from TOF to Emmie's call in the case of no jmp/call
  // in the orriginal host (EmmieCallOffset) plus the distance from TOF to
  // Emmie's call in the case of a jmp/call in the orriginal host.  In any
  // case, one of these variables will be 0.  Then we write the three bytes
  // we decrypted back to that location.

  if(lpCallBack->FileSeek(hFile, SeekLen + EmmieCallOffset, SEEK_SET) == -1)
  {
    RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
    return(EXTSTATUS_NO_REPAIR);
  }
  if(lpCallBack->FileWrite(hFile, WorkBuf, 3) == -1)
  {
    RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
    return(EXTSTATUS_NO_REPAIR);
  }


  // Truncation:  Seek to EOF - lenght of particular varient and write
  // 0 bytes (this truncates the file from that point on).

  if(WhichEmmie==Emmie2702) VarSeek=-0xa8e;
  if(WhichEmmie==Emmie2823) VarSeek=-0xb07;
  if(WhichEmmie==Emmie3097) VarSeek=-0xc19;

  if(lpCallBack->FileSeek(hFile, VarSeek, SEEK_END) == -1)
  {
    RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
    return(EXTSTATUS_NO_REPAIR);
  }
  if(lpCallBack->FileWrite(hFile, WorkBuf, 0) == -1)
  {
    RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
    return(EXTSTATUS_NO_REPAIR);
  }


  // If we got to this point it means that everything was hunky-dorry so
  // we close the file and return OK!

  RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
  return (EXTSTATUS_OK);
}
#endif // (defined for SYM_DOSX only)


// EXTRepairFileManzon arguments:

// lpCallBack           : pointer to callback structure for FileOpen, etc.
// lpsN30               : pointer to N30 structure containing VIRSCAN.DAT
//                        information and virus stats
// lpszFileName         : name of the file to repair
// lpbyWorkBuffer       : temporary 2-4K work buffer.
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK          if OK
//                        EXTSTATUS_NO_REPAIR   if unable to repair
//

#define MZR_ADD     1
#define MZR_SUB     2
#define MZR_XOR     3
#define MZR_ROL     4
#define MZR_ROR     5
#define MZR_DEC     6
#define MZR_INC     7
#define MZR_NEG     8

WORD EXTRepairFileManzon (LPCALLBACKREV1 lpCallBack,
                          LPN30           lpsN30,
                          LPTSTR          lpszFileName,
                          LPBYTE         lpbyWorkBuffer)

{
    HFILE   hFile;
    UINT    uAttr,i,k;
    LPBYTE  lpbyRepairBuffer;           // used to hold 32 byte repair info
    BYTE    byDecryptKey[6],            // keys for decryptors
            cbDecryptorId,              // count of slots used
            byDecryptorId[13] =         // 13 slots containing decryptors
            {0,0,0,0,0,0,0,0,0,0,0,0,0};
    WORD    wEncryptOfs;                // offset in lpbyWorkBuffer of encrypt
                                        // body of virus.
    DWORD   dwFileSize, dwEntryPt, dwVirusSize;
    BOOL    bComRepair;
    WORD    wPartPage, wPageCnt, wMinMem;

    (void)lpszFileName;
                                        // The first 0x600 bytes of the
                                        // work buffer we'll reserve for
                                        // the virus after which we'll
                                        // store repair info.

    lpbyRepairBuffer = lpbyWorkBuffer + 0x600;

                                        // Open file

    if (RepairFileOpen(lpCallBack, lpszFileName, &hFile, &uAttr))
    {
        return(EXTSTATUS_NO_REPAIR);
    }

    if ((DWORD)-1 == (dwFileSize = lpCallBack->FileSize(hFile)))
    {
        RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
        return(EXTSTATUS_NO_REPAIR);
    }
                                        // 1430 is min infection size
    if (dwFileSize < 1430)
    {
        RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
        return(EXTSTATUS_NO_REPAIR);
    }

                                        // Seek TOF
    if ((DWORD)-1 == lpCallBack->FileSeek(hFile, 0, SEEK_SET))
    {
        RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
        return(EXTSTATUS_NO_REPAIR);
    }

                                        // Read 18h bytes from TOF
    if ((UINT) -1 == lpCallBack->FileRead(hFile, lpbyRepairBuffer, 0x18))
    {
        RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
        return(EXTSTATUS_NO_REPAIR);
    }

                                        // Determine com or exe infection
                                        // Calculate entry pt
                                        // and ep vs EOF = virus size
    if (lpbyRepairBuffer[0] == 0xe9)
    {
        bComRepair = TRUE;
        dwEntryPt = (DWORD) AVDEREF_WORD(lpbyRepairBuffer + 1) + 3;
    }
    else
    {
        bComRepair = FALSE;

        // segment offset + initial ip + header size = entry point
        dwEntryPt = ((DWORD) AVDEREF_WORD(lpbyRepairBuffer + 0x16)) << 4;
        dwEntryPt +=(DWORD) AVDEREF_WORD(lpbyRepairBuffer + 0x14);
        dwEntryPt +=((DWORD) AVDEREF_WORD(lpbyRepairBuffer + 0x8)) << 4;
    }

    dwVirusSize = dwFileSize - dwEntryPt;

    if (dwVirusSize < 1430 || dwVirusSize > 1500 || dwEntryPt < 0)
    {
        RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
        return(EXTSTATUS_NO_REPAIR);
    }

                                        // Get bytes from entry pt

    if ((DWORD)-1 == lpCallBack->FileSeek(hFile, -(LONG)dwVirusSize, SEEK_END))
    {
        RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
        return(EXTSTATUS_NO_REPAIR);
    }

                                        // Read virus bytes

    if ((UINT) dwVirusSize != lpCallBack->FileRead(hFile,
                                                   lpbyWorkBuffer,
                                                   (UINT)dwVirusSize))
    {
        RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
        return(EXTSTATUS_NO_REPAIR);
    }
                                        // Check bookmark
    if (0xb9 != lpbyWorkBuffer[0])
    {
        RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
        return(EXTSTATUS_NO_REPAIR);
    }
                                        // Scan for decryptors
                                        // There may be as many as 13 blocks.
                                        // The virus starts with 6 bytes we'll
                                        // skip.  4 byte blocks of code follow
                                        // some of which are decryptors, some
                                        // of which are garbage.
    cbDecryptorId = 0;
    i=6;
    while (i < (6 + 4 * 13))
    {

                                        // All decryptors start with 0x2e
        if (0x2e == lpbyWorkBuffer[i])
        {
            switch (*(LPWORD)(lpbyWorkBuffer + i + 1))
            {
                case 0x0480 :           // ADD 1
                    byDecryptorId[cbDecryptorId] = MZR_ADD;
                    byDecryptKey[MZR_ADD] = lpbyWorkBuffer[i+3];
                    break;

                case 0x2c80 :           // SUB 2
                    byDecryptorId[cbDecryptorId] = MZR_SUB;
                    byDecryptKey[MZR_SUB] = lpbyWorkBuffer[i+3];
                    break;

                case 0x3480 :           // XOR 3
                    byDecryptorId[cbDecryptorId] = MZR_XOR;
                    byDecryptKey[MZR_XOR] = lpbyWorkBuffer[i+3];
                    break;

                case 0x04c0 :           // ROL 4
                    byDecryptorId[cbDecryptorId] = MZR_ROL;
                    byDecryptKey[MZR_ROL] = lpbyWorkBuffer[i+3] & 0x07;
                    break;

                case 0x0cc0 :           // ROR 5
                    byDecryptorId[cbDecryptorId] = MZR_ROR;
                    byDecryptKey[MZR_ROR] = lpbyWorkBuffer[i+3] & 0x07;
                    break;

                // The last 3 decryptors are unary => no key.

                case 0x0cfe :           // DEC 6
                    byDecryptorId[cbDecryptorId] = MZR_DEC;
                    break;

                case 0x04fe :           // INC 7
                    byDecryptorId[cbDecryptorId] = MZR_INC;
                    break;

                case 0x1cf6 :           // NEG 8
                    byDecryptorId[cbDecryptorId] = MZR_NEG;
                    break;
            }
            cbDecryptorId++;

        } // end if
                                        // Check for end of decrypt loop
                                        // 0x4e or 0x83 is the beginning of
                                        // loop index incr => end of loop.
        else if (0x4e == lpbyWorkBuffer[i] || 0x83 == lpbyWorkBuffer[i])
        {

            break;
        }

        i += 4;
    } // end while
                                        // There are 6 bytes following
                                        // the decryptors/garbage bytes
                                        // then the encrypted body begins.
    wEncryptOfs=i + 6;
                                        // Decrypt first layer

                                        // last byte we need is at ofs 1409
                                        // from beginning of encrypted body.
    for (i=wEncryptOfs;i<wEncryptOfs + 1410;i++)
    {
                                        // loop through valid decryptors
        for (k=0;(k < 13) && (byDecryptorId[k] != 0);k++)
        {
            switch (byDecryptorId[k])
            {
                case MZR_ADD  :
                    lpbyWorkBuffer[i] += byDecryptKey[MZR_ADD];
                    break;

                case MZR_SUB  :
                    lpbyWorkBuffer[i] -= byDecryptKey[MZR_SUB];
                    break;

                case MZR_XOR  :
                    lpbyWorkBuffer[i] ^= byDecryptKey[MZR_XOR];
                    break;

                case MZR_ROL  :
                    lpbyWorkBuffer[i] =
                     (lpbyWorkBuffer[i]<<byDecryptKey[MZR_ROL]) |
                     ((unsigned)lpbyWorkBuffer[i]>>(sizeof(BYTE)*8-byDecryptKey[MZR_ROL]));
                    break;

                case MZR_ROR  :
                    lpbyWorkBuffer[i] =
                     (lpbyWorkBuffer[i]>>byDecryptKey[MZR_ROR]) |
                     (lpbyWorkBuffer[i]<<(sizeof(BYTE)*8-byDecryptKey[MZR_ROR]));
                    break;

                case MZR_DEC  :
                    lpbyWorkBuffer[i]--;
                    break;

                case MZR_INC  :
                    lpbyWorkBuffer[i]++;
                    break;

                case MZR_NEG  :
                    lpbyWorkBuffer[i] = -lpbyWorkBuffer[i];
                    break;
            }
        } // end for k
    } // end for i
                                        // Get Inner encryption key
    byDecryptKey[0] = lpbyWorkBuffer[wEncryptOfs + 1409];

                                        // Decrypt Repair bytes
                                        // Repair strings begins 473 bytes
                                        // from beginning of Encrypted body and
                                        // is 23 bytes long.

    for (i=wEncryptOfs + 473;i < wEncryptOfs + 473 + 23;i++)
    {
        lpbyWorkBuffer[i] ^= byDecryptKey[0];
    }
                                        // Check bookmarks


    if (AVDEREF_DWORD(lpbyWorkBuffer + wEncryptOfs + 473) != 0x4e02c683)
    {
        RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
        return(EXTSTATUS_NO_REPAIR);
    }
                                        // Write to repair buffer
    if (TRUE == bComRepair)
    {
        lpbyRepairBuffer[0] = lpbyWorkBuffer[wEncryptOfs + 480];
        lpbyRepairBuffer[1] = lpbyWorkBuffer[wEncryptOfs + 481];
        lpbyRepairBuffer[2] = lpbyWorkBuffer[wEncryptOfs + 482];
    }
    else
    {
                                        // exe repair

                                        // ss
        // *(LPWORD)(lpbyRepairBuffer + 0x0e) =
        //     *(LPWORD)(lpbyWorkBuffer + wEncryptOfs + 490);
        AVASSIGN_WORD(lpbyRepairBuffer + 0x0e,
            AVDEREF_WORD(lpbyWorkBuffer + wEncryptOfs + 490));

                                        // sp
        // *(LPWORD)(lpbyRepairBuffer + 0x10) =
        //     *(LPWORD)(lpbyWorkBuffer + wEncryptOfs + 492);
        AVASSIGN_WORD(lpbyRepairBuffer + 0x10,
            AVDEREF_WORD(lpbyWorkBuffer + wEncryptOfs + 492));

                                        // cs
        // *(LPWORD)(lpbyRepairBuffer + 0x16) =
        //     *(LPWORD)(lpbyWorkBuffer + wEncryptOfs + 486);
        AVASSIGN_WORD(lpbyRepairBuffer + 0x16,
            AVDEREF_WORD(lpbyWorkBuffer + wEncryptOfs + 486));

                                        // ip
        // *(LPWORD)(lpbyRepairBuffer + 0x14) =
        //     *(LPWORD)(lpbyWorkBuffer + wEncryptOfs + 488);
        AVASSIGN_WORD(lpbyRepairBuffer + 0x14,
            AVDEREF_WORD(lpbyWorkBuffer + wEncryptOfs + 488));

 // Recycle dwFileSize, now it is the
 // repaired file size not the infected
 // file size.

        dwFileSize -= dwVirusSize;

                                        // Adjust image size in exe header

        // (*(LPWORD)(lpbyRepairBuffer + 4)) = (WORD) (dwFileSize / 512);
        //
        // if (0 != (*(LPWORD)(lpbyRepairBuffer + 2) =
        //     (WORD)(dwFileSize % 512)))
        // {
        //     (*(LPWORD)(lpbyRepairBuffer + 4))++;
        // }

        wPageCnt = (WORD) (dwFileSize / 512);
        wPartPage = (WORD) (dwFileSize % 512);
        if (0 != wPartPage)
            ++wPageCnt;

        AVASSIGN_WORD(lpbyRepairBuffer + 4,wPageCnt);
        AVASSIGN_WORD(lpbyRepairBuffer + 2,wPartPage);

                                        // Adjust memory required above
                                        // program
        // if (*(LPWORD)(lpbyRepairBuffer + 0x0a) >= 0x71)
        //     *(LPWORD)(lpbyRepairBuffer + 0x0a) -= 0x71;

        wMinMem = AVDEREF_WORD(lpbyRepairBuffer + 0x0a);
        if (wMinMem >= 0x71)
            AVASSIGN_WORD(lpbyRepairBuffer + 0x0a,wMinMem - 0x71);

    }
                                        // Write repair buffer back to file

                                        // Seek TOF
    if ((DWORD)-1 == lpCallBack->FileSeek(hFile, 0, SEEK_SET))
    {
        RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
        return(EXTSTATUS_NO_REPAIR);
    }

                                        // Write back repair header
    if ((UINT) -1 == lpCallBack->FileWrite(hFile, lpbyRepairBuffer, 0x18))
    {
        RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
        return(EXTSTATUS_NO_REPAIR);
    }

                                        // Truncate the file

    if (RepairFileTruncate(lpCallBack, hFile, (long)dwVirusSize))
    {
        RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
        return(EXTSTATUS_NO_REPAIR);
    }

                                        // Close file

    if (RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr))
    {
        return(EXTSTATUS_NO_REPAIR);
    }
                                        // Return success
    return (EXTSTATUS_OK);
}


//
// *** #if-def'd out due to lack of memory in SYM_DOS ***
//
#if defined(SYM_DOSX) || !defined(SYM_DOS)

// EXTRepairFileVMem arguments:
//
// lpCallBack           : pointer to callback structure for FileOpen, etc.
// lpsN30               : pointer to N30 structure containing VIRSCAN.DAT
//                        information and virus stats
// lpszFileName         : name of the file to repair
// lpbyWorkBuffer       : temporary 2-4K work buffer.
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK          if OK
//                        EXTSTATUS_NO_REPAIR   if unable to repair

WORD EXTRepairFileVMem(LPCALLBACKREV1 lpCallBack,
                       LPN30           lpsN30,
                       LPTSTR          lpszFileName,
                       LPBYTE         lpbyWorkBuffer)
{
    LPBYTE  lpbyVirus;
    HFILE   hFile;
    UINT    uAttr;
    DWORD   dwFileSize;
    WORD    i;
    BYTE    byKey;
    WORD    wPartPage, wPageCnt;

                                        // Open file
    if (RepairFileOpen(lpCallBack, lpszFileName, &hFile, &uAttr))
    {
        return(EXTSTATUS_NO_REPAIR);
    }

                                        // Seek to EOF - 3291

    if ((DWORD)-1 == lpCallBack->FileSeek(hFile, -3291, SEEK_END))
    {
        RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
        return(EXTSTATUS_NO_REPAIR);
    }

                                        // Read 512 bytes

    if (512 != lpCallBack->FileRead(hFile, lpbyWorkBuffer, 512))
    {
        RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
        return(EXTSTATUS_NO_REPAIR);
    }

    for (i=35;i<51;i++)
    {
        if (AVDEREF_WORD(lpbyWorkBuffer + i) == 0x00e8)

                                        // e8 00 is at offset 32h
                                        // after virus does segment fix up
                                        // We'll use the same offsets.

        {
             lpbyVirus = lpbyWorkBuffer + i - 34;
             break;
        }
    }



                                        // Do simple decryption
                                        // Looking for TERMINATOR
                                        // Enc alg is byte xor by key
                                        // where key += 8 on each iteration.
    byKey = lpbyVirus[0x169] ^ 0x54;
    if (((BYTE)((byKey + 8) ^ lpbyVirus[0x16a]) == 0x45) &&
        ((BYTE)((byKey + 16) ^ lpbyVirus[0x16b]) == 0x52) &&
        ((BYTE)((byKey + 24) ^ lpbyVirus[0x16c]) == 0x4d))
    {
                                        // Decrypt com and exe repair info
        for (i=0x168;i>0x15d;i--)
        {
            byKey-=8;
            lpbyVirus[i] ^= byKey;
        }
    }
    else
    {
        RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
        return(EXTSTATUS_NO_REPAIR);
    }
                                        // Seek to TOF

    if ((DWORD)-1 == lpCallBack->FileSeek(hFile, 0, SEEK_SET))
    {
        RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
        return(EXTSTATUS_NO_REPAIR);
    }

                                        // Read 24 bytes at TOF

    if (24 != lpCallBack->FileRead(hFile, lpbyWorkBuffer + 512, 24))
    {
        RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
        return(EXTSTATUS_NO_REPAIR);
    }

    if (lpbyWorkBuffer[512] == 0xe9)
    {
        lpbyWorkBuffer[512] = lpbyVirus[0x166];
        lpbyWorkBuffer[513] = lpbyVirus[0x167];
        lpbyWorkBuffer[514] = lpbyVirus[0x168];
    }
    else
    {
                                        // SS
        // *(LPWORD)(lpbyWorkBuffer + 512 + 0x0e) =
        //     *(LPWORD)(lpbyVirus + 0x164);
        AVASSIGN_WORD(lpbyWorkBuffer + 512 + 0x0e,
            AVDEREF_WORD(lpbyVirus + 0x164));

                                        // SP
        // *(LPWORD)(lpbyWorkBuffer + 512 + 0x10) =
        //     *(LPWORD)(lpbyVirus + 0x162);
        AVASSIGN_WORD(lpbyWorkBuffer + 512 + 0x10,
            AVDEREF_WORD(lpbyVirus + 0x162));

                                        // CS
        // *(LPWORD)(lpbyWorkBuffer + 512 + 0x16) =
        //     *(LPWORD)(lpbyVirus + 0x160);
        AVASSIGN_WORD(lpbyWorkBuffer + 512 + 0x16,
            AVDEREF_WORD(lpbyVirus + 0x160));

                                        // IP
        // *(LPWORD)(lpbyWorkBuffer + 512 + 0x14) =
        //     *(LPWORD)(lpbyVirus + 0x15e);
        AVASSIGN_WORD(lpbyWorkBuffer + 512 + 0x14,
            AVDEREF_WORD(lpbyVirus + 0x15e));

                                        // Adjust image size in exe header

        if ((DWORD)-1 == (dwFileSize = lpCallBack->FileSeek(hFile, -3291, SEEK_END)))
        {
            RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
            return(EXTSTATUS_NO_REPAIR);
        }

        // (*(LPWORD)(lpbyWorkBuffer + 512 + 4)) = (WORD) (dwFileSize / 512);
        //
        // if (0 != (*(LPWORD)(lpbyWorkBuffer + 512 + 2) =
        //     (WORD)(dwFileSize % 512)))
        // {
        //     (*(LPWORD)(lpbyWorkBuffer + 512 + 4))++;
        // }

        wPageCnt = (WORD) (dwFileSize / 512);
        wPartPage = (WORD) (dwFileSize % 512);
        if (0 != wPartPage)
            ++wPageCnt;
        AVASSIGN_WORD(lpbyWorkBuffer + 512 + 4,wPageCnt);
        AVASSIGN_WORD(lpbyWorkBuffer + 512 + 2,wPartPage);
    }

                                        // Seek to TOF

    if ((DWORD)-1 == lpCallBack->FileSeek(hFile, 0, SEEK_SET))
    {
        RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
        return(EXTSTATUS_NO_REPAIR);
    }

                                        // Write back repair header
    if ((UINT) -1 == lpCallBack->FileWrite(hFile, lpbyWorkBuffer+512, 24))
    {
        RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
        return(EXTSTATUS_NO_REPAIR);
    }

                                        // Truncate the file

    if (RepairFileTruncate(lpCallBack, hFile, 3291))
    {
        RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
        return(EXTSTATUS_NO_REPAIR);
    }

                                        // Close file

    if (RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr))
    {
        return(EXTSTATUS_NO_REPAIR);
    }
                                        // Return success
    return (EXTSTATUS_OK);
}
#endif

// EXTRepairFileCordobes arguments:                 &&C
//
// Purpose: Decrypt the CS, IP, SS, and SP, then restore and rebuild the
//          EXE header.
//
// lpCallBack           : pointer to callback structure for FileOpen, etc.
// lpsN30               : pointer to N30 structure containing VIRSCAN.DAT
//                        information and virus stats
// lpszFileName         : name of the file to repair
// lpbyWorkBuffer       : temporary 2-4K work buffer.
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK          if OK
//                        EXTSTATUS_NO_REPAIR   if unable to repair
//                        EXTSTATUS_FILE_ERROR  if file or disk error
//
// This virus encrypts itself by performing a rotate, and an
// add or subtract on a WORD at a time.  Order of ops DOES vary.

#define CORDCRYPTLN 0x0a        // Length of decrypt buffer
#define CORDOBESLEN 3334        // Length of Cordobes virus
#define CORDREAD    0x200       // Amount to read from entry point
#define DECLOOPLOC  0x1f        // Location of decryption loop
#define ENDLOOPLOC  0x28        // End of loop "meat"
#define CORDFIXLOC  0x13f       // Offset of fix data from virus entry
#define CORD_SSLOC  0x140
#define CORD_SPLOC  0x142
#define CORD_IPLOC  0x144
#define CORD_CSLOC  0x146

#define PUSHCXFOUND 0x01        // Bit flags for decrypt loop
#define LODSWFOUND  0x02        // All are set if true
#define MOVCLFOUND  0x04
#define ADDSUBFOUND 0x08
#define ROTATEFOUND 0x10
#define ALL_FOUND   (PUSHCXFOUND | LODSWFOUND | MOVCLFOUND | ADDSUBFOUND | \
                                                                ROTATEFOUND)
#define ADD_FIRST   0x20

WORD EXTRepairFileCordobes(LPCALLBACKREV1   lpCallBack,
                           LPN30             lpsN30,
                           LPTSTR            lpszFileName,
                           LPBYTE           lpbyWorkBuffer)
{
    HFILE       hFile;
    UINT        uAttr;
    WORD        wAddNum, i, x, x2;
    BYTE        byStatus, byRotate;     // Status byte, and amount to rotate
    DWORD       dwResult;               // File offset of virus entry point
    LPWORD      lpwHandy;               // Handy pointer
    EXEHEADER   CordHead;               // Place to hold fix bytes

    // _asm    int 3                    // Remove after debugging! &&C
                                        // Open file
    if (0 != FILEOPEN())
        return(EXTSTATUS_FILE_ERROR);

    if (sEXEHEADER != FILEREAD(((LPBYTE) &CordHead), sEXEHEADER))
        CLOSEOUT(EXTSTATUS_FILE_ERROR);

    dwResult = GetEntryPtOffset((LPBYTE) &CordHead);

    if (dwResult != FILESEEK(dwResult))
        CLOSEOUT(EXTSTATUS_FILE_ERROR);

    if (CORDREAD != FILEREAD(lpbyWorkBuffer, CORDREAD))
        CLOSEOUT(EXTSTATUS_FILE_ERROR);

    byStatus = 0;
    i = DECLOOPLOC;

    do                  // Read through decrypt loop so we can duplicate it
    {
        switch (lpbyWorkBuffer[i++])
        {
            case 0x05 :                                     // ADD instruction
            case 0x2d :                                     // SUB
                byStatus |= ADDSUBFOUND;                    // Mark as found

                if (0 == (byStatus & ROTATEFOUND))  // Determine 1st operation
                    byStatus |= ADD_FIRST;

                wAddNum = AVDEREF_WORD(lpbyWorkBuffer + i); // Get operand

                if (0x2d == lpbyWorkBuffer[i - 1])      // If SUB instruction
                    wAddNum = ~(--wAddNum);             // Negate wAddNum

                i += 2;
                break;

            case 0x51 :                                 // PUSH CX
                byStatus |= PUSHCXFOUND;
                break;

            case 0xad :                                 // LODSW
                byStatus |= LODSWFOUND;
                break;

            case 0xb1 :                                 // MOV CL, ??
                byStatus |= MOVCLFOUND;
                byRotate = lpbyWorkBuffer[i++];     // Get amount to rotate
                break;

            case 0xd3 :                                 // RO?  AX, CL
                byStatus |= ROTATEFOUND;

                if (0xc8 == lpbyWorkBuffer[i++])
                    byRotate = 16 - byRotate;
                break;

            default :
                break;
        }
    } while (i < ENDLOOPLOC);

    if (ALL_FOUND != (byStatus & ALL_FOUND))
        CLOSEOUT(EXTSTATUS_NO_REPAIR);

    lpwHandy = (LPWORD) (lpbyWorkBuffer + CORDFIXLOC);

    for (i = 0; i < (CORDCRYPTLN / 2); i++)
    {
        x = *lpwHandy;

        if (0 != (byStatus & ADD_FIRST))    // Do addition/substraction if
            x += wAddNum;                   // it comes first

        if (0 != (byRotate & 0x0f))         // If we should rotate the bytes
        {
            x2 = x;                         // Do the rotation
            x <<= byRotate;
            x2 >>= (16 - byRotate);
            x |= x2;
        }

        if (0 == (byStatus & ADD_FIRST))    // Do addition/subtraction if
            x += wAddNum;                   // it's last

        *lpwHandy++ = x;
    }

    lpwHandy = (LPWORD) (lpbyWorkBuffer + CORD_SSLOC);  // Point to decrypted
                                                        // stuff
    CordHead.exInitSS = AVDEREF_WORD(lpwHandy);         // Fixup header
    ++lpwHandy;
    CordHead.exInitSP = AVDEREF_WORD(lpwHandy);
    ++lpwHandy;
    CordHead.exInitIP = AVDEREF_WORD(lpwHandy);
    ++lpwHandy;
    CordHead.exInitCS = AVDEREF_WORD(lpwHandy);

    x = EXTSTATUS_FILE_ERROR;

    dwResult = FILESIZE();

    if (DWERROR != dwResult)
    {
        dwResult -= CORDOBESLEN;

        CordHead.exExtraBytes = (WORD) (dwResult & 0x1ff);

        dwResult >>= 9;             // Convert byte count to number of pages

        if (0 != CordHead.exExtraBytes)
            dwResult++;

        CordHead.exPages = (WORD) dwResult;

        CordHead.exInitSS = WENDIAN(CordHead.exInitSS);
        CordHead.exInitSP = WENDIAN(CordHead.exInitSP);
        CordHead.exInitIP = WENDIAN(CordHead.exInitIP);
        CordHead.exInitCS = WENDIAN(CordHead.exInitCS);
        CordHead.exExtraBytes = WENDIAN(CordHead.exExtraBytes);
        CordHead.exPages = WENDIAN(CordHead.exPages);

        if (0 == SEEK_TOF())                        // Write new EXE header
            if (sEXEHEADER == FILEWRITE((LPBYTE) &CordHead, sEXEHEADER))
                if (0 == FILETRUNCATE(CORDOBESLEN)) // Chop off the virus
                    x = EXTSTATUS_OK;
    }

    CLOSEOUT(x);
}   // EXTRepairFileCordobes

//
// *** #if-def'd out due to lack of memory in SYM_DOS ***
//
#if defined(SYM_DOSX) || !defined(SYM_DOS)

// EXTRepairFileDiamond arguments:              &&D
//
// Purpose: Except for changing the minimum memory requirement for the program
//          at offset 0x0a in the EXE header, this would be a very stock
//          virscan repair.  This subtracts 0x13 from the minimum memory
//          requirement in the EXE header, and restores original values for
//          CS, IP, SS, and SP.
//
// lpCallBack           : pointer to callback structure for FileOpen, etc.
// lpsN30               : pointer to N30 structure containing VIRSCAN.DAT
//                        information and virus stats
// lpszFileName         : name of the file to repair
// lpbyWorkBuffer       : temporary 2-4K work buffer.
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK          if OK
//                        EXTSTATUS_NO_REPAIR   if unable to repair
//                        EXTSTATUS_FILE_ERROR  if file or disk error

#define DIAMONDLEN  666     // Infection Length
#define DIAMONDREAD 0x80    // Amount to read for file fixing
#define DIAMONDBKMK 0x6d    // Bookmark
#define DIAMONDVAL  0x5a4d  // Bookmark value
#define DIMDFIXREAD 0x10    // Amout to read to get fix data
#define DIAMONDFIX  0x0284  // File offset of 1st fix word
#define DIMDALLCFIX 0x13    // Decrease memory requirement by this amount

WORD EXTRepairFileDiamond(LPCALLBACKREV1    lpCallBack,
                            LPN30           lpsN30,
                            LPTSTR          lpszFileName,
                            LPBYTE          lpbyWorkBuffer)
{
    WORD        wRetVal;
    DWORD       dwFileLen, dwReadPos;
    HFILE       hFile;
    UINT        uAttr;
    EXEHEADER   ExeHdr;
    LPWORD      lpwBuff;

    //  _asm    int 3                   // Remove after debugging! &&D
 
    wRetVal = EXTSTATUS_FILE_ERROR;
 
    if (0 != FILEOPEN())                // Open file
        return(wRetVal);
                                        // Get file header
    if (sEXEHEADER == FILEREAD((LPBYTE) &ExeHdr, sEXEHEADER))
    {
        dwReadPos = GetEntryPtOffset((LPBYTE) &ExeHdr);
                                        // Goto entry point
        if (dwReadPos == FILESEEK(dwReadPos))
        {                               // Read the entry point
            if (DIAMONDREAD == FILEREAD(lpbyWorkBuffer, DIAMONDREAD))
                if (DIAMONDVAL != AVDEREF_WORD(lpbyWorkBuffer + DIAMONDBKMK))
                    wRetVal = EXTSTATUS_NO_REPAIR;  // Bookmark check
                else
                {
                    dwReadPos += DIAMONDFIX;            // Move to fix data

                    if (dwReadPos == FILESEEK(dwReadPos))   // Read fix data
                        if (DIMDFIXREAD ==
                                        FILEREAD(lpbyWorkBuffer, DIMDFIXREAD))
                        {                           // Fix header
                            lpwBuff = (LPWORD) lpbyWorkBuffer;
 
                            ExeHdr.exInitSP = AVDEREF_WORD(lpwBuff);
                            lpwBuff++;
                            ExeHdr.exInitSS = AVDEREF_WORD(lpwBuff);
                            lpwBuff++;
                            ExeHdr.exInitIP = AVDEREF_WORD(lpwBuff);
                            lpwBuff++;
                            ExeHdr.exInitCS = AVDEREF_WORD(lpwBuff);
                                                    // Virscan can't do this
                            dwFileLen = FILESIZE();         // Get file size

                            if (DWERROR != dwFileLen)
                            {
                                dwFileLen -= DIAMONDLEN;

                                ExeHdr.exExtraBytes =
                                                (WORD) (dwFileLen & 0x1ff);
                                        // Convert byte count to page count
                                dwFileLen >>= 9;

                                if (0 != ExeHdr.exExtraBytes)
                                    dwFileLen++;

                                ExeHdr.exPages = (WORD) dwFileLen;
 
                                ExeHdr.exInitSP = WENDIAN(ExeHdr.exInitSP);
                                ExeHdr.exInitSS = WENDIAN(ExeHdr.exInitSS);
                                ExeHdr.exInitIP = WENDIAN(ExeHdr.exInitIP);
                                ExeHdr.exInitCS = WENDIAN(ExeHdr.exInitCS);
                                ExeHdr.exMinAlloc = WENDIAN(ExeHdr.exMinAlloc);
                                ExeHdr.exExtraBytes = WENDIAN(ExeHdr.exExtraBytes);
                                ExeHdr.exPages = WENDIAN(ExeHdr.exPages);

                                if (0 == SEEK_TOF())    // Write new header
                                    if (sEXEHEADER ==
                                      FILEWRITE((LPBYTE) &ExeHdr, sEXEHEADER))
                                        if (0 == FILETRUNCATE(DIAMONDLEN))
                                            wRetVal = EXTSTATUS_OK;
                            }
                        }
                }
        }
    }

    CLOSEOUT(wRetVal);
}   // EXTRepairFileDiamond


// EXTRepairFileWerewolf arguments:         &&W
//
// Purpose: Decrypt the CS, IP, SS, and SP, then restore and rebuild the
//          EXE header.
//
// lpCallBack           : pointer to callback structure for FileOpen, etc.
// lpsN30               : pointer to N30 structure containing VIRSCAN.DAT
//                        information and virus stats
// lpszFileName         : name of the file to repair
// lpbyWorkBuffer       : temporary 2-4K work buffer.
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK          if OK
//                        EXTSTATUS_NO_REPAIR   if unable to repair
//                        EXTSTATUS_FILE_ERROR  if file or disk error
//
// This virus encrypts itself by performing a rotate on the high or low
// half of a word, and then XORs one half into the other.  The order of
// operation does vary between infections.  The rotate is either an ROL
// or ROR, but is always by one bit.  The XOR is either XOR rH, rL or
// XOR rL, rH.  For each iteration of the decrypt loop, the counter is
// incremented by *ONE*, and then another word is twiddled.

#define WEREREAD    0x60        // Bytes to read at entry pt
#define WEREWOLFLEN 1500        // Length of Werewolf virus
#define DECRYPTLOC  0x14        // Start decrypting here
#define WERE_ROT    0x00        // Bit flags for operations
#define WERE_XOR    0x01
#define WERE_HI     0x02
#define WERE_RITE   0x04

#define WERE_SSLOC  0x3d
#define WERE_SPLOC  0x43
#define WERE_IPLOC  0x4a
#define WERE_CSLOC  0x4c
#define WERE_COMFIX 0x4f

WORD EXTRepairFileWerewolf(LPCALLBACKREV1   lpCallBack,
                            LPN30           lpsN30,
                            LPTSTR          lpszFileName,
                            LPBYTE          lpbyWorkBuffer)
{
    HFILE       hFile;
    UINT        uAttr;
    WORD        i, j, x, x2;
    BYTE        byEXEFile, byOpCount,   // EXE File? / Count of Operations
                byOp[2];                // Array of Operations
    DWORD       dwResult;               // File offset of virus entry point
    EXEHEADER   FileHead;               // Place to hold fix bytes

    //_asm    int 3                    // Remove after debugging! &&W
                                        // Open file
    if (0 != FILEOPEN())
        return(EXTSTATUS_FILE_ERROR);

    if (sEXEHEADER != FILEREAD(((LPBYTE) &FileHead), sEXEHEADER))
        CLOSEOUT(EXTSTATUS_FILE_ERROR);

    dwResult = GetEntryPtOffset((LPBYTE) &FileHead);

    if (dwResult != FILESEEK(dwResult))
        CLOSEOUT(EXTSTATUS_FILE_ERROR);

    if (WEREREAD != FILEREAD(lpbyWorkBuffer, WEREREAD))
        CLOSEOUT(EXTSTATUS_FILE_ERROR);

    byEXEFile = ISEXEHDR(&FileHead);
 
    i = byOpCount = 0;

    do                  // Read through decrypt loop so we can duplicate it
    {
        switch (lpbyWorkBuffer[i++])
        {
            case 0x87 :                     // XCHG ??, ??
            case 0x8b :                     // MOV  BP, ??
                i++;
                break;
 
            case 0xbc :                     // MOV  SP, ????
                i += 2;
                break;

            case 0x32 :                     // XOR
                byOp[byOpCount] = WERE_XOR;
 
                if (0 != (lpbyWorkBuffer[i++] & 0x20))  // if destination
                    byOp[byOpCount] |= WERE_HI;         // is rH

                byOpCount++;
                break;

            case 0xd0 :                     // ROL/ROR
                byOp[byOpCount] = WERE_ROT;

                if (0 != (lpbyWorkBuffer[i] & 0x08))    // if ROR
                    byOp[byOpCount] |= WERE_RITE;

                if (0 != (lpbyWorkBuffer[i++] & 0x04))  // if dest is rH
                    byOp[byOpCount] |= WERE_HI;

                byOpCount++;
                break;

            default :
                break;
        }
    } while ((DECRYPTLOC > i) && (2 > byOpCount));

    if (DECRYPTLOC == i)
        CLOSEOUT(EXTSTATUS_NO_REPAIR);

// Commented out because SYM_CPU_X86 isn't defined, even when it should be,
// so I guess we don't need it
//#ifndef   SYM_CPU_X86
//#pragma message ("Possible endian dependencies on code at line "__LINE__".")
//#endif

    // Possible endian dependencies in the loop below!!

    for (i = DECRYPTLOC; i < (WEREREAD - 1); i++)
    {
        for (j = 0; j < 2; j++)
        {
            if (WERE_XOR == (byOp[j] & WERE_XOR))   // If this op is XOR
            {
                if (WERE_HI == (byOp[j] & WERE_HI))
                    lpbyWorkBuffer[i + 1] ^= lpbyWorkBuffer[i];
                else
                    lpbyWorkBuffer[i] ^= lpbyWorkBuffer[i + 1];
            }
            else                                    // If it's RO?
            {
                x = (WERE_HI == (byOp[j] & WERE_HI)) ? 1 : 0;

                if (WERE_RITE == (byOp[j] & WERE_RITE)) // If it's ROR
                {
                    x2 = lpbyWorkBuffer[i + x] & 0x01;

                    lpbyWorkBuffer[i + x] >>= 1;

                    if (0 != x2)
                        lpbyWorkBuffer[i + x] |= 0x80;
                }
                else                                    // If it's ROL
                {
                    x2 = lpbyWorkBuffer[i + x] & 0x80;

                    lpbyWorkBuffer[i + x] <<= 1;

                    if (0 != x2)
                        lpbyWorkBuffer[i + x] |= 0x01;
                }
            }
        }
    }
 
    if (TRUE == byEXEFile)
    {                                       // Fixup EXE header
        FileHead.exInitSS = AVDEREF_WORD(lpbyWorkBuffer + WERE_SSLOC);
        FileHead.exInitSS = WENDIAN(FileHead.exInitSS);
        FileHead.exInitSP = AVDEREF_WORD(lpbyWorkBuffer + WERE_SPLOC);
        FileHead.exInitSP = WENDIAN(FileHead.exInitSP);
        FileHead.exInitIP = AVDEREF_WORD(lpbyWorkBuffer + WERE_IPLOC);
        FileHead.exInitIP = WENDIAN(FileHead.exInitIP);
        FileHead.exInitCS = AVDEREF_WORD(lpbyWorkBuffer + WERE_CSLOC);
        FileHead.exInitCS = WENDIAN(FileHead.exInitCS);
    }
    else
    {                                       // Fixup COM file
        FileHead.exSignature = AVDEREF_WORD(lpbyWorkBuffer + WERE_COMFIX);
        FileHead.exSignature = WENDIAN(FileHead.exSignature);
        FileHead.exExtraBytes =
                    AVDEREF_WORD(lpbyWorkBuffer + WERE_COMFIX + sizeof(WORD));
        FileHead.exExtraBytes = WENDIAN(FileHead.exExtraBytes);
    }

    x = EXTSTATUS_FILE_ERROR;

    dwResult = FILESIZE();

    if (DWERROR != dwResult)
    {
        dwResult -= WEREWOLFLEN;

        if (TRUE == byEXEFile)
        {
            FileHead.exExtraBytes = (WORD) (dwResult & 0x1ff);

            dwResult >>= 9;         // Convert byte count to number of pages

            if (0 != FileHead.exExtraBytes)
                dwResult++;

            FileHead.exPages = (WORD) dwResult;

            FileHead.exExtraBytes = WENDIAN(FileHead.exExtraBytes);
            FileHead.exPages = WENDIAN(FileHead.exPages);
        }

        if (0 == SEEK_TOF())                        // Write new EXE header
            if (sEXEHEADER == FILEWRITE((LPBYTE) &FileHead, sEXEHEADER))
                if (0 == FILETRUNCATE(WEREWOLFLEN)) // Chop off the virus
                    x = EXTSTATUS_OK;
    }

    CLOSEOUT(x);
}   // EXTRepairFileWerewolf
#endif

// EXTRepairFileBurglar arguments:
//
// lpCallBack           : pointer to callback structure for FileOpen, etc.
// lpsN30               : pointer to N30 structure containing VIRSCAN.DAT
//                        information and virus stats
// lpszFileName         : name of the file to repair
// lpbyWorkBuffer       : temporary 2-4K work buffer.
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK          if OK
//                        EXTSTATUS_NO_REPAIR   if unable to repair

WORD EXTRepairFileBurglar(LPCALLBACKREV1 lpCallBack,
                       LPN30          lpsN30,
                       LPTSTR         lpszFileName,
                       LPBYTE         lpbyWorkBuffer)
{
    LPBYTE  lpbyRepair;
    HFILE   hFile;
    UINT    uAttr;
    DWORD   dwFileSize;
    DWORD   dwEntryPt;
    long    lTruncSize;
    WORD    i;
    WORD    wPageCnt, wPartPage;

                                        // Open file
    if (RepairFileOpen(lpCallBack, lpszFileName, &hFile, &uAttr))
    {
        return(EXTSTATUS_NO_REPAIR);
    }


                                        // Seek to TOF

    if ((DWORD)-1 == lpCallBack->FileSeek(hFile, 0, SEEK_SET))
    {
        RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
        return(EXTSTATUS_NO_REPAIR);
    }

                                        // Read 24 bytes at TOF
    if (24 != lpCallBack->FileRead(hFile, lpbyWorkBuffer, 24))
    {
        RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
        return(EXTSTATUS_NO_REPAIR);
    }

                                        // Get the entry point of the virus

    dwEntryPt = GetEntryPtOffset(lpbyWorkBuffer);

                                        // Reality check for MZ in the header.

    //if ((AVDEREF_WORD(lpbyWorkBuffer) != 0x4d5a) &&
    //    (AVDEREF_WORD(lpbyWorkBuffer) != 0x5a4d))
    //{
    //    RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
    //    return(EXTSTATUS_NO_REPAIR);
    //}

                                        // Adjust image size in exe header

    dwFileSize = dwEntryPt;

    //if ((DWORD)-1 == (dwFileSize = lpCallBack->FileSeek(hFile, -1150, SEEK_END)))
    //{
    //    RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
    //    return(EXTSTATUS_NO_REPAIR);
    //}

    // (*(LPWORD)(lpbyWorkBuffer + 4)) = (WORD) (dwFileSize / 512);
    //
    // if (0 != (*(LPWORD)(lpbyWorkBuffer + 2) = (WORD)(dwFileSize % 512)))
    // {
    //     (*(LPWORD)(lpbyWorkBuffer + 4))++;
    // }

    wPartPage = (WORD) (dwFileSize % 512);
    wPageCnt = (WORD) (dwFileSize / 512);
    if (0 != wPartPage);
        ++wPageCnt;

    AVASSIGN_WORD(lpbyWorkBuffer + 4,wPageCnt);
    AVASSIGN_WORD(lpbyWorkBuffer + 2,wPartPage);

                                        // Seek to EOV - 127

    if ((DWORD)-1 == lpCallBack->FileSeek(hFile, (long) (dwEntryPt + 0x3ff), SEEK_SET))
    {
        RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
        return(EXTSTATUS_NO_REPAIR);
    }

                                        // Point lpbyRepair into work buffer
                                        // 512 is arbitrary location
                                        // after TOF buffer.

    lpbyRepair = lpbyWorkBuffer + 512;
                                        // Read 14 bytes

    if (14 != lpCallBack->FileRead(hFile, lpbyRepair, 14))
    {
        RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
        return(EXTSTATUS_NO_REPAIR);
    }

                                        // Check for bookmark value
    if (AVDEREF_DWORD(lpbyRepair) != 0x90c358f9)
    {
        RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
        return(EXTSTATUS_NO_REPAIR);
    }
                                        // Do simple decryption
                                        // This reduces to first byte xor by
                                        // 76 and the rest byte xor by 1.

    for (i=0;i<10;i++)
    {
        WORD wTemp;

        // *(LPWORD)(lpbyRepair + 4 + i ) ^= 0x7776;
        wTemp = AVDEREF_WORD(lpbyRepair + 4 + i);
        AVASSIGN_WORD(lpbyRepair + 4 + i,wTemp ^ 0x7776);
                                        // Put decrypted bytes back in exe hdr.
                                        // 0x0e is location of SS offset in hdr.
        lpbyWorkBuffer[i + 0x0e] = lpbyRepair[i + 4];
    }

                                    // Seek to TOF

    if ((DWORD)-1 == lpCallBack->FileSeek(hFile, 0, SEEK_SET))
    {
        RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
        return(EXTSTATUS_NO_REPAIR);
    }

                                        // Write back repair header
    if ((UINT) -1 == lpCallBack->FileWrite(hFile, lpbyWorkBuffer, 24))
    {
        RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
        return(EXTSTATUS_NO_REPAIR);
    }

                                        // Truncate the file

    lTruncSize = lpCallBack->FileSize(hFile) - dwEntryPt;

    if (RepairFileTruncate(lpCallBack, hFile, lTruncSize))
    {
        RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
        return(EXTSTATUS_NO_REPAIR);
    }

                                        // Close file

    if (RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr))
    {
        return(EXTSTATUS_NO_REPAIR);
    }
                                        // Return success
    return (EXTSTATUS_OK);
}


// EXTRepairFilePredt arguments:            &&P
//
// Purpose: Decrypt the CS, IP, SS, and SP, then restore and rebuild the
//          EXE header.
//
// lpCallBack           : pointer to callback structure for FileOpen, etc.
// lpsN30               : pointer to N30 structure containing VIRSCAN.DAT
//                        information and virus stats
// lpszFileName         : name of the file to repair
// lpbyWorkBuffer       : temporary 2-4K work buffer.
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK          if OK
//                        EXTSTATUS_NO_REPAIR   if unable to repair
//                        EXTSTATUS_FILE_ERROR  if file or disk error
//
// This virus encrypts itself by performing two 2-byte instructions in
// its decryption loop.  This routine parses these instructions and
// duplicates their operations in its own loop.  Like the virus loop, the
// decryption loop here is "bottom up" - it decrements the address to
// decrypt as the loop executes.  Note the slight bit of bit-twiddling
// used to restore the minimum memory requirement for EXE files.

#define PREDREAD    18          // Bytes to read at entry pt
#define PRED_AXINIT 6           // Displacement to AX initial value
#define PRED_CXVAL  0x4bd       // CX initial value
#define PREDOPDSP   14          // Displacement to opcodes to parse
#define PREDLEN     2448        // Length of Werewolf virus

#define PRED_ADD    0x00        // Set these for an add operation
#define PRED_SUB    0x20        // subtract
#define PRED_XOR    0x40        // XOR
#define PRED_ROR    0x60        // Rotate right
#define PRED_ROL    0x80        // Rotate left
#define PRED_NEG    0xa0        // Negate
#define PRED_NOT    0xc0        // NOT

#define PRED_WORD   0x10        // Indicates WORD operation

#define PRED_BAL    0           // Byte source or dest is AL
#define PRED_BAH    1           // AH
#define PRED_BCL    2           // You get the idea
#define PRED_BCH    3

#define PRED_WAX    0           // Word source or dest is AX
#define PRED_WCX    1           // CX
#define PRED_WDI    2           // Destination is WORD PTR [DI]

#define PRED_COMFIX 0x853       // Location for COM fix data
#define PRED_IPLOC  0x855       // EXE fix data
#define PRED_CSLOC  0x857
#define PRED_SPLOC  0x859
#define PRED_SSLOC  0x85b

WORD EXTRepairFilePredt(LPCALLBACKREV1  lpCallBack,
                        LPN30           lpsN30,
                        LPTSTR          lpszFileName,
                        LPBYTE          lpbyWorkBuffer)
{
    HFILE       hFile;
    UINT        uAttr;
    WORD        i, j, x;
    BYTE        byEXEFile, byOp[2];     // EXE File? / Array of Operations
    DWORD       dwResult;               // File offset of virus entry point
    LPBYTE      lpbByteOp[4];           // Byte pointers for operands
    LPWORD      lpwPtr, lpwWordOp[3];   // Word pointers for instructions, operands
    EXEHEADER   FileHead;               // Place to hold fix bytes
    WORD        wTemp0, wTemp1;

    union
    {
        WORD    reg16;
        struct
        {
            BYTE    l, h;
        } reg8;
    } ax, cx;                           // Operand holders

    // _asm    int 3                    // Remove after debugging! &&P
                                        // Open file
    if (0 != FILEOPEN())
        return(EXTSTATUS_FILE_ERROR);

    if (sEXEHEADER != FILEREAD(((LPBYTE) &FileHead), sEXEHEADER))
        CLOSEOUT(EXTSTATUS_FILE_ERROR);

    dwResult = GetEntryPtOffset((LPBYTE) &FileHead);

    if (dwResult != FILESEEK(dwResult))
        CLOSEOUT(EXTSTATUS_FILE_ERROR);

    if (PREDREAD != FILEREAD(lpbyWorkBuffer, PREDREAD))
        CLOSEOUT(EXTSTATUS_FILE_ERROR);

    byEXEFile = ISEXEHDR(&FileHead);

    ax.reg16 = AVDEREF_WORD(lpbyWorkBuffer + PRED_AXINIT);
    cx.reg16 = PRED_CXVAL;

    lpbByteOp[0] = &ax.reg8.l;          // Since the file reads above were
    lpbByteOp[1] = &ax.reg8.h;          // successful, we'll spend the time
    lpbByteOp[2] = &cx.reg8.l;          // do this initialization
    lpbByteOp[3] = &cx.reg8.h;

    lpwWordOp[0] = &ax.reg16;
    lpwWordOp[1] = &cx.reg16;
 
// Commented out because SYM_CPU_X86 isn't defined, even when it should be,
// so I guess we don't need it
//#ifndef   SYM_CPU_X86
//#pragma message ("Possible endian dependencies on code at line "__LINE__".")
//#endif

    // Possible endian dependencies in the loop below!!

    // _asm int 3                                   // Remove after debugging &&P
 
    for (i = 0; i < 2; i++) // Read through decrypt loop so we can duplicate it
    {
        byOp[i] = 0;
 
        lpwPtr = ((LPWORD) (lpbyWorkBuffer + PREDOPDSP)) + i;
 
        switch (AVDEREF_WORD(lpwPtr) & 0x00f0)
        {
            case 0x20 :                 // SUB
                byOp[i] |= PRED_SUB;
 
            case 0x00 :                 // ADD - no op bits are set for adds

                if (0 != (AVDEREF_WORD(lpwPtr) & 0x01))  // See if it's a word op
                {
                    byOp[i] |= PRED_WORD;
                                            // Check dest - AX or DI for WORDs
                    byOp[i] |= (0 == (AVDEREF_WORD(lpwPtr) & 0xf000)) ? (PRED_WDI << 2) :
                                                            (PRED_WAX << 2);
                                            // Check src - AX or CX
                    byOp[i] |= (0x0500 == (AVDEREF_WORD(lpwPtr) & 0x0f00)) ? PRED_WAX :
                                                                    PRED_WCX;
                }
                else                        // It's a byte op
                {                           // AL or AH for byte op dest
                    byOp[i] |= (0xc000 == (AVDEREF_WORD(lpwPtr) & 0xf000)) ?
                                            (PRED_BAL << 2) : (PRED_BAH << 2);
                    byOp[i] |= (0x0100 == (AVDEREF_WORD(lpwPtr) & 0x0f00)) ? PRED_BCL :
                                                                    PRED_BCH;
                }

                break;

            case 0x30 :                 // XOR
                byOp[i] = (PRED_XOR | PRED_WORD | (PRED_WDI << 2));
 
                byOp[i] |= (0x0500 == (AVDEREF_WORD(lpwPtr) & 0x0f00)) ? PRED_WAX :
                                                                    PRED_WCX;
 
                break;
 
            case 0xd0 :                 // ROL / ROR
                byOp[i] = (0 == (AVDEREF_WORD(lpwPtr) & 0x0800)) ? PRED_ROL : PRED_ROR;

                if (0 != (AVDEREF_WORD(lpwPtr) & 0x01))  // See if it's a word op
                {
                    byOp[i] |= PRED_WORD;
                                            // Check dest - AX or DI for WORDs
                    byOp[i] |= (0 == (AVDEREF_WORD(lpwPtr) & 0xf000)) ? (PRED_WDI << 2) :
                                                            (PRED_WAX << 2);
                                            // Check src - AX or CX
                    byOp[i] |= (0 == (AVDEREF_WORD(lpwPtr) & 0x0002)) ? PRED_WAX :
                                                                    PRED_WCX;
                }
                else
                {                           // AL or AH for byte op dest
                    byOp[i] |= (0x0400 == (AVDEREF_WORD(lpwPtr) & 0x0400)) ?
                                            (PRED_BAH << 2) : (PRED_BAL << 2);
                    byOp[i] |= (0x0002 == (AVDEREF_WORD(lpwPtr) & 0x0002)) ? PRED_BCL :
                                                                    PRED_BCH;
                }

                break;
 
            case 0xf0 :                 // NEG/NOT
                byOp[i] = (0 == (AVDEREF_WORD(lpwPtr) & 0x0800)) ? PRED_NOT : PRED_NEG;

                byOp[i] |= PRED_WORD;

                byOp[i] |= (0 == (AVDEREF_WORD(lpwPtr) & 0x8000)) ? (PRED_WDI << 2) :
                                                            (PRED_WAX << 2);
                break;
        }
    }
                        // Find out where we can begin reading decrypt data
    dwResult += (1 == byEXEFile) ? PRED_IPLOC : PRED_COMFIX;
 
    dwResult--;
 
    if (dwResult != FILESEEK(dwResult))             // Go to it
        CLOSEOUT(EXTSTATUS_FILE_ERROR);

    j = PREDLEN - ((1 == byEXEFile) ? PRED_IPLOC : PRED_COMFIX) + 1;

    if (j != FILEREAD(lpbyWorkBuffer, j))
        CLOSEOUT(EXTSTATUS_FILE_ERROR);

    lpwWordOp[2] = (LPWORD) (lpbyWorkBuffer + j - sizeof(WORD));
                                                        // Point to last word
     // _asm    int 3                           // remove after debugging &&P
 
    do
    {
        cx.reg16--;

        for (i = 0; i < 2; i++)
        {
            switch (byOp[i] & 0xe0)
            {
                case PRED_ADD :
                    if (PRED_WORD == (byOp[i] & PRED_WORD))
                    {
                        // *lpwWordOp[(byOp[i] & 0x0c) >> 2] +=
                        //                         *lpwWordOp[byOp[i] & 0x03];

                        wTemp0 = AVDEREF_WORD(lpwWordOp[(byOp[i] & 0x0c) >> 2]);
                        wTemp1 = AVDEREF_WORD(lpwWordOp[byOp[i] & 0x03]);
                        AVASSIGN_WORD(lpwWordOp[(byOp[i] & 0x0c) >> 2],
                            wTemp0 + wTemp1);
                    }
                    else
                        *lpbByteOp[(byOp[i] & 0x0c) >> 2] +=
                                                *lpbByteOp[byOp[i] & 0x03];
                    break;
 
                case PRED_SUB :
                    if (PRED_WORD == (byOp[i] & PRED_WORD))
                    {
                        // *lpwWordOp[(byOp[i] & 0x0c) >> 2] -=
                        //                         *lpwWordOp[byOp[i] & 0x03];

                        wTemp0 = AVDEREF_WORD(lpwWordOp[(byOp[i] & 0x0c) >> 2]);
                        wTemp1 = AVDEREF_WORD(lpwWordOp[byOp[i] & 0x03]);
                        AVASSIGN_WORD(lpwWordOp[(byOp[i] & 0x0c) >> 2],
                            wTemp0 - wTemp1);
                    }
                    else
                        *lpbByteOp[(byOp[i] & 0x0c) >> 2] -=
                                                *lpbByteOp[byOp[i] & 0x03];
                    break;
 
                case PRED_XOR :
                    if (PRED_WORD == (byOp[i] & PRED_WORD))
                    {
                        // *lpwWordOp[(byOp[i] & 0x0c) >> 2] ^=
                        //                         *lpwWordOp[byOp[i] & 0x03];

                        wTemp0 = AVDEREF_WORD(lpwWordOp[(byOp[i] & 0x0c) >> 2]);
                        wTemp1 = AVDEREF_WORD(lpwWordOp[byOp[i] & 0x03]);
                        AVASSIGN_WORD(lpwWordOp[(byOp[i] & 0x0c) >> 2],
                            wTemp0 ^ wTemp1);
                    }
                    else
                        *lpbByteOp[(byOp[i] & 0x0c) >> 2] ^=
                                                *lpbByteOp[byOp[i] & 0x03];
                    break;

                case PRED_ROR :
                case PRED_ROL :
                    {
                        WORD    rotmain, rot2;
 
                        if (PRED_WORD == (byOp[i] & PRED_WORD))
                        {
                            WORD    result;
 
                            result = AVDEREF_WORD(lpwWordOp[(byOp[i] & 0x0c) >> 2]);
 
                            if (PRED_WAX == (byOp[i] & 0x03))
                                rotmain = 1;
                            else
                                rotmain = cx.reg8.l & 0x0f;

                            rot2 = 16 - rotmain;

                            if (PRED_ROR == (byOp[i] & 0xe0))
                            {
                                // *lpwWordOp[(byOp[i] & 0x0c) >> 2] =
                                //     (result >> rotmain) | (result << rot2);

                                AVASSIGN_WORD(lpwWordOp[(byOp[i] & 0x0c) >> 2],
                                    (result >> rotmain) | (result << rot2));
                            }
                            else
                            {
                                // *lpwWordOp[(byOp[i] & 0x0c) >> 2] =
                                //     (result << rotmain) | (result >> rot2);

                                AVASSIGN_WORD(lpwWordOp[(byOp[i] & 0x0c) >> 2],
                                    (result << rotmain) | (result >> rot2));
                            }
                        }
                        else
                        {
                            BYTE    result;
 
                            result = *lpbByteOp[(byOp[i] & 0x0c) >> 2];
 
                            if (PRED_BCH == (byOp[i] & 0x03))
                                rotmain = 1;
                            else
                                rotmain = cx.reg8.l & 0x07;

                            rot2 = 8 - rotmain;

                            if (PRED_ROR == (byOp[i] & 0xe0))
                                *lpbByteOp[(byOp[i] & 0x0c) >> 2] =
                                    (result >> rotmain) | (result << rot2);
                            else
                                *lpbByteOp[(byOp[i] & 0x0c) >> 2] =
                                    (result << rotmain) | (result >> rot2);
                        }
                    }
 
                    break;

                case PRED_NEG :
                    if (PRED_WORD == (byOp[i] & PRED_WORD))
                    {
                        x = AVDEREF_WORD(lpwWordOp[(byOp[i] & 0x0c) >> 2]);
 
                        // *lpwWordOp[(byOp[i] & 0x0c) >> 2] =
                        //                                 (WORD) -((SWORD) x);

                        AVASSIGN_WORD(lpwWordOp[(byOp[i] & 0x0c) >> 2],
                            (WORD) -((SWORD) x));
                    }
                    else
                        *lpbByteOp[(byOp[i] & 0x0c) >> 2] = (BYTE)
                         -((signed char) (*lpbByteOp[(byOp[i] & 0x0c) >> 2]));

                    break;
 
                case PRED_NOT :
                    if (PRED_WORD == (byOp[i] & PRED_WORD))
                    {
                        x = AVDEREF_WORD(lpwWordOp[(byOp[i] & 0x0c) >> 2]);
 
                        // *lpwWordOp[(byOp[i] & 0x0c) >> 2] = ~x;

                        AVASSIGN_WORD(lpwWordOp[(byOp[i] & 0x0c) >> 2],~x);
                    }
                    else
                        *lpbByteOp[(byOp[i] & 0x0c) >> 2] =
                                        ~(*lpbByteOp[(byOp[i] & 0x0c) >> 2]);

                    break;
            }
        }

        lpwWordOp[2]--;
        j -= 2;
    } while (0 != j);

    if (TRUE == byEXEFile)
    {                                       // Fixup EXE header
        FileHead.exMinAlloc = WENDIAN(FileHead.exMinAlloc) -
            (WENDIAN(FileHead.exInitSP) >> 4);
        FileHead.exMinAlloc = WENDIAN(FileHead.exMinAlloc);
        FileHead.exInitSS =
                AVDEREF_WORD(lpbyWorkBuffer + PRED_SSLOC - PRED_IPLOC + 1);
        FileHead.exInitSS = WENDIAN(FileHead.exInitSS);
        FileHead.exInitSP =
                AVDEREF_WORD(lpbyWorkBuffer + PRED_SPLOC - PRED_IPLOC + 1);
        FileHead.exInitSP = WENDIAN(FileHead.exInitSP);
        FileHead.exInitIP = AVDEREF_WORD(lpbyWorkBuffer + 1);
        FileHead.exInitIP = WENDIAN(FileHead.exInitIP);
        FileHead.exInitCS =
                AVDEREF_WORD(lpbyWorkBuffer + PRED_CSLOC - PRED_IPLOC + 1);
        FileHead.exInitCS = WENDIAN(FileHead.exInitCS);
    }
    else
    {                                       // Fixup COM file
        FileHead.exSignature = AVDEREF_WORD(lpbyWorkBuffer + 1);
        FileHead.exSignature = WENDIAN(FileHead.exSignature);
        *((LPBYTE) &(FileHead.exExtraBytes)) =
                                        *(lpbyWorkBuffer + 1 + sizeof(WORD));
    }

    x = EXTSTATUS_FILE_ERROR;

    dwResult = FILESIZE();

    if (DWERROR != dwResult)
    {
        dwResult -= PREDLEN;

        if (TRUE == byEXEFile)
        {
            FileHead.exExtraBytes = (WORD) (dwResult & 0x1ff);

            dwResult >>= 9;         // Convert byte count to number of pages

            if (0 != FileHead.exExtraBytes)
                dwResult++;

            FileHead.exPages = (WORD) dwResult;

            FileHead.exExtraBytes = WENDIAN(FileHead.exExtraBytes);
            FileHead.exPages = WENDIAN(FileHead.exPages);
        }

        if (0 == SEEK_TOF())                        // Write new EXE header
            if (sEXEHEADER == FILEWRITE((LPBYTE) &FileHead, sEXEHEADER))
                if (0 == FILETRUNCATE(PREDLEN))     // Chop off the virus
                    x = EXTSTATUS_OK;
    }

    CLOSEOUT(x);
}   // EXTRepairFilePredt
//
// *** #if-def'd out due to lack of memory in SYM_DOS ***
//
#if defined(SYM_DOSX) || !defined(SYM_DOS)

// EXTRepairFileTanpro arguments:           &&T
//
// Purpose: Remove the virus from the beginning of the file, and fix the
//          first word of the host, which is XORed by the virus.  This XOR
//          is what made a NAVEX repair necessary.
//
// lpCallBack           : pointer to callback structure for FileOpen, etc.
// lpsN30               : pointer to N30 structure containing VIRSCAN.DAT
//                        information and virus stats
// lpszFileName         : name of the file to repair
// lpbyWorkBuffer       : temporary 2-4K work buffer.
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK          if OK
//                        EXTSTATUS_NO_REPAIR   if unable to repair
//                        EXTSTATUS_FILE_ERROR  if file or disk error
//

#define TANPROLEN   524             // This many bytes are added to the file
#define TANPROFIX   0x0d            // Location of XOR value for 1st word
#define TANPROREAD  0x10            // Number of bytes to read to fix host

WORD EXTRepairFileTanpro(LPCALLBACKREV1 lpCallBack,
                            LPN30       lpsN30,
                            LPTSTR      lpszFileName,
                            LPBYTE      lpbyWorkBuffer)
{
    HFILE   hFile;
    UINT    uAttr;
    WORD    wFix;
    WORD    wTemp;
    DWORD   dwBegRead, dwBegWrite, dwNumBytes;  // Constants for moving bytes

    // _asm    int 3                    // Remove after debugging! &&T
 
    if (0 != FILEOPEN())                // Open file
        return(EXTSTATUS_FILE_ERROR);
                                        // Read beginning of file
    if (TANPROREAD != FILEREAD(lpbyWorkBuffer, TANPROREAD))
        CLOSEOUT(EXTSTATUS_FILE_ERROR);

    wFix = AVDEREF_WORD(lpbyWorkBuffer + TANPROFIX);    // Get fix value
 
    if (TANPROLEN != FILESEEK(TANPROLEN))   // Go to beginning of host
        CLOSEOUT(EXTSTATUS_FILE_ERROR);
                                            // Read beginning of host
    if (TANPROREAD != FILEREAD(lpbyWorkBuffer, TANPROREAD))
        CLOSEOUT(EXTSTATUS_FILE_ERROR);

    // *((LPWORD) lpbyWorkBuffer) ^= wFix;     // Fix first word of host
    wTemp = AVDEREF_WORD(lpbyWorkBuffer);
    AVASSIGN_WORD(lpbyWorkBuffer,wTemp ^ wFix);

    if (0 != SEEK_TOF())                    // Go to TOF
        CLOSEOUT(EXTSTATUS_FILE_ERROR);
                                            // Write beginning of fixed host
    if (TANPROREAD != FILEWRITE(lpbyWorkBuffer, TANPROREAD))
        CLOSEOUT(EXTSTATUS_FILE_ERROR);

    dwBegRead = TANPROLEN + TANPROREAD;     // Set place to begin reading data
    dwBegWrite = TANPROREAD;                // Set place to begin writing data
 
    dwNumBytes = FILESIZE();                // Compute number of bytes to move
    dwNumBytes -= dwBegRead;
 
    wFix = EXTSTATUS_NO_REPAIR;
 
    if (0 == RepairFileMoveBytes(lpCallBack, hFile, lpbyWorkBuffer,
                                    dwBegRead, dwBegWrite, dwNumBytes))
        if (0 == FILETRUNCATE(TANPROLEN))
            wFix = EXTSTATUS_OK;

    CLOSEOUT(wFix);
}
#endif

//
// *** #if-def'd out due to lack of memory in SYM_DOS ***
//
#if defined(SYM_DOSX) || !defined(SYM_DOS)

// EXTRepairFileOxana arguments:            &&O
//
// Purpose: Remove the virus from the end of the file, and fix up the EXE
//          header.  Since the detection picks up both 1670 and 1671
//          strains, we check to see which strain is being fixed.
//
//          The name on the def says 1654/1670 - I was unable to get a
//          sample of 1654, if one exists, to test this repair.
//
// lpCallBack           : pointer to callback structure for FileOpen, etc.
// lpsN30               : pointer to N30 structure containing VIRSCAN.DAT
//                        information and virus stats
// lpszFileName         : name of the file to repair
// lpbyWorkBuffer       : temporary 2-4K work buffer.
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK          if OK
//                        EXTSTATUS_NO_REPAIR   if unable to repair
//                        EXTSTATUS_FILE_ERROR  if file or disk error
//

#define OXBASELEN   1670            // This many bytes are added to the file
#define OXJMPHI     0x05            // Hi byte of entrypt jmp offset
#define OXJMPLO_0   0x2f            // Lo byte of entrypt jmp offset for 1670
#define OXJMPLO_1   0x2b            // Lo byte of entrypt jmp offset for 1671
#define OXFIX       0x5d7           // Offset - fix info to entrypt
#define OXREAD      0x18            // Bytes to read to get fix data

WORD EXTRepairFileOxana(LPCALLBACKREV1  lpCallBack,
                        LPN30           lpsN30,
                        LPTSTR          lpszFileName,
                        LPBYTE          lpbyWorkBuffer)
{
    HFILE       hFile;
    UINT        uAttr;
    WORD        wFix, x;
    DWORD       dwChop, dwFileLoc;              // File offsets, lengths
//    LPWORD      lpwSrc, lpwDest;                // Used for copying fix info
    LPBYTE      lpbySrc, lpbyDest;
    EXEHEADER   FileHead;

    // _asm    int 3                    // Remove after debugging! &&O
                                        // Open file
    if (0 != FILEOPEN())
        return(EXTSTATUS_FILE_ERROR);

    if (sEXEHEADER != FILEREAD(((LPBYTE) &FileHead), sEXEHEADER))
        CLOSEOUT(EXTSTATUS_FILE_ERROR);

    wFix = 0;
    dwChop = OXBASELEN;
    dwFileLoc = GetEntryPtOffset((LPBYTE) &FileHead);

    if (0 != (dwFileLoc & 0x0f))        // Offset should be multiple of 16
        CLOSEOUT(EXTSTATUS_NO_REPAIR);

    if (dwFileLoc != FILESEEK(dwFileLoc))               // Move to entrypt
        CLOSEOUT(EXTSTATUS_FILE_ERROR);

    if (OXREAD != FILEREAD(lpbyWorkBuffer, OXREAD))     // Read entry pt code
        CLOSEOUT(EXTSTATUS_FILE_ERROR);
                                        // Verify jump instruction at entrypt
    if ((0xe9 != lpbyWorkBuffer[0]) || (OXJMPHI != lpbyWorkBuffer[2]))
        CLOSEOUT(EXTSTATUS_NO_REPAIR);  // JMP destination verified by virscan

    if (OXJMPLO_1 == lpbyWorkBuffer[1]) // If we're fixing a 1671 infection
    {
        dwChop++;
        dwFileLoc++;
    }
    else if (OXJMPLO_0 != lpbyWorkBuffer[1])    // else make sure it's a 1670
        CLOSEOUT(EXTSTATUS_NO_REPAIR);

    dwFileLoc += OXFIX;

    if (dwFileLoc != FILESEEK(dwFileLoc))               // Go to fix data
        CLOSEOUT(EXTSTATUS_FILE_ERROR);

    if (OXREAD != FILEREAD(lpbyWorkBuffer, OXREAD))     // Read fix data
        CLOSEOUT(EXTSTATUS_FILE_ERROR);

    // lpwDest = ((LPWORD) &FileHead) + 1;
    // lpwSrc = (LPWORD) lpbyWorkBuffer;
    //
    // for (x = (OXREAD / sizeof(WORD)); x > 0; x--)   // Copy fix data to header
    //     *lpwDest++ = *lpwSrc++;

    lpbyDest = ((LPBYTE)&FileHead) + 2;
    lpbySrc = lpbyWorkBuffer;

    for (x = OXREAD; x > 0; x--)
        *lpbyDest++ = *lpbySrc++;

    x = EXTSTATUS_FILE_ERROR;

    if (0 == SEEK_TOF())                            // Seek and write fix data
        if (sEXEHEADER == FILEWRITE(((LPBYTE) &FileHead), sEXEHEADER))
            if (0 == FILETRUNCATE(dwChop))
                x = EXTSTATUS_OK;

    CLOSEOUT(x);
}   // EXTRepairFileOxana

#endif
//
// *** #if-def'd out due to lack of memory in SYM_DOS ***
//
#if defined(SYM_DOSX) || !defined(SYM_DOS)

// EXTRepairFileGenrat arguments:               &&GR
//
// Purpose: Except for subracting 0x319 from the SP value in the EXE header,
//          we could have done a stock virscan repair on this.
//
// lpCallBack           : pointer to callback structure for FileOpen, etc.
// lpsN30               : pointer to N30 structure containing VIRSCAN.DAT
//                        information and virus stats
// lpszFileName         : name of the file to repair
// lpbyWorkBuffer       : temporary 2-4K work buffer.
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK          if OK
//                        EXTSTATUS_NO_REPAIR   if unable to repair
//                        EXTSTATUS_FILE_ERROR  if file or disk error

#define GENRATLEN   785     // Infection Length
#define GENRATREAD  0x30    // Amount to read for file fixing
#define GENRATVAL   0x18e9  // Bookmark value
#define GENRATSS    0x0019  // SS offset from entry point
#define GENRATCS    0x0029  // CS offset
#define GENRATSPFIX 0x0319  // Value to adjust SP with
#define GENRATSSFIX 0x0100  // Value to adjust SS with, if needed

WORD EXTRepairFileGenrat(LPCALLBACKREV1 lpCallBack,
                            LPN30       lpsN30,
                            LPTSTR      lpszFileName,
                            LPBYTE      lpbyWorkBuffer)
{
    WORD        wRetVal;
    DWORD       dwFileLen, dwReadPos;
    HFILE       hFile;
    UINT        uAttr;
    EXEHEADER   ExeHdr;
    LPWORD      lpwBuff;

    // _asm    int 3                    // Remove after debugging! &&GR
 
    wRetVal = EXTSTATUS_FILE_ERROR;
 
    if (0 != FILEOPEN())                // Open file
        return(wRetVal);
                                        // Get file header
    if (sEXEHEADER == FILEREAD((LPBYTE) &ExeHdr, sEXEHEADER))
    {
        dwReadPos = GetEntryPtOffset((LPBYTE) &ExeHdr);
                                        // Goto entry point
        if (dwReadPos == FILESEEK(dwReadPos))
        {                               // Read the entry point
            if (GENRATREAD == FILEREAD(lpbyWorkBuffer, GENRATREAD))
                if (GENRATVAL != AVDEREF_WORD(lpbyWorkBuffer))
                    wRetVal = EXTSTATUS_NO_REPAIR;  // Bookmark check
                else
                {                           // Fix header
                    lpwBuff = (LPWORD) (lpbyWorkBuffer + GENRATSS);

                    ExeHdr.exInitSS = AVDEREF_WORD(lpwBuff);
                    lpwBuff++;
                    ExeHdr.exInitSP = AVDEREF_WORD(lpwBuff);
                                        // Fix SP & SS - Virscan can't do this
                    if (ExeHdr.exInitSP < GENRATSPFIX)
                        ExeHdr.exInitSS += GENRATSSFIX;

                    ExeHdr.exInitSP -= GENRATSPFIX;

                    lpwBuff = (LPWORD) (lpbyWorkBuffer + GENRATCS);
 
                    ExeHdr.exInitCS = AVDEREF_WORD(lpwBuff);
                    lpwBuff++;
                    ExeHdr.exInitIP = AVDEREF_WORD(lpwBuff);

                    dwFileLen = FILESIZE();         // Get file size

                    if (DWERROR != dwFileLen)
                    {
                        dwFileLen -= GENRATLEN;

                        ExeHdr.exExtraBytes =
                                        (WORD) (dwFileLen & 0x1ff);
                                // Convert byte count to page count
                        dwFileLen >>= 9;

                        if (0 != ExeHdr.exExtraBytes)
                            dwFileLen++;

                        ExeHdr.exPages = (WORD) dwFileLen;
 
                        ExeHdr.exInitSP = WENDIAN(ExeHdr.exInitSP);
                        ExeHdr.exInitSS = WENDIAN(ExeHdr.exInitSS);
                        ExeHdr.exInitIP = WENDIAN(ExeHdr.exInitIP);
                        ExeHdr.exInitCS = WENDIAN(ExeHdr.exInitCS);
                        ExeHdr.exExtraBytes = WENDIAN(ExeHdr.exExtraBytes);
                        ExeHdr.exPages = WENDIAN(ExeHdr.exPages);

                        if (0 == SEEK_TOF())    // Write new header
                            if (sEXEHEADER ==
                                    FILEWRITE((LPBYTE) &ExeHdr, sEXEHEADER))
                                if (0 == FILETRUNCATE(GENRATLEN))
                                    wRetVal = EXTSTATUS_OK;
                    }
                }
        }
    }

    CLOSEOUT(wRetVal);
}   // EXTRepairFileGenrat

// EXTRepairFileFour11 arguments:               &&4
//
// Purpose: The real entry point on this virus varies with the mod 16 of the
//          file length, and we can't determine the entry point because this
//          virus does a PUSH/RETF at the beginning of the file.  EXE files
//          are fixed with virscan, because they don't have the PUSH/RETF at
//          the beginning of the file - the header is simply changed.
//
// lpCallBack           : pointer to callback structure for FileOpen, etc.
// lpsN30               : pointer to N30 structure containing VIRSCAN.DAT
//                        information and virus stats
// lpszFileName         : name of the file to repair
// lpbyWorkBuffer       : temporary 2-4K work buffer.
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK          if OK
//                        EXTSTATUS_NO_REPAIR   if unable to repair
//                        EXTSTATUS_FILE_ERROR  if file or disk error

#define FOUR11LEN   2128    // Infection Length
#define FOUR11SMRD  0x0c    // Small read for file fixing
#define FOUR11LGRD  0x64    // Large read for file fixing
#define FOUR11FIX   0x1f    // Offset of fix data from entry point
#define FOUR11BOOK  0x9999  // Bookmark value

WORD EXTRepairFileFour11(LPCALLBACKREV1 lpCallBack,
                            LPN30       lpsN30,
                            LPTSTR      lpszFileName,
                            LPBYTE      lpbyWorkBuffer)
{
    WORD        wRetVal;
    DWORD       dwReadPos;
    HFILE       hFile;
    UINT        uAttr;

    //_asm    int 3                     // Remove after debugging! &&4
 
    wRetVal = EXTSTATUS_FILE_ERROR;
 
    if (0 != FILEOPEN())                // Open file
        return(wRetVal);
                                        // Get file header
    if (FOUR11SMRD == FILEREAD(lpbyWorkBuffer, FOUR11SMRD))
    {
        dwReadPos = (DWORD) (AVDEREF_WORD((LPWORD) &lpbyWorkBuffer[4]));
        dwReadPos <<= 4;                // Compute location of fix data
        dwReadPos += FOUR11FIX;
                                        // Goto fix data
        if (dwReadPos == FILESEEK(dwReadPos))
        {                               // Read fix data and bookmark
            if (FOUR11LGRD == FILEREAD(lpbyWorkBuffer, FOUR11LGRD))
            {
                if (FOUR11BOOK ==
                      AVDEREF_WORD((LPWORD) &lpbyWorkBuffer[FOUR11LGRD - 2]))
                    if (0 == SEEK_TOF())
                        if (FOUR11SMRD ==
                                        FILEWRITE(lpbyWorkBuffer, FOUR11SMRD))
                            if (0 == FILETRUNCATE(FOUR11LEN))
                                wRetVal = EXTSTATUS_OK;
            }
        }
    }

    CLOSEOUT(wRetVal);
}   // EXTRepairFileFour11
#endif

// EXTRepairFileBatman2 arguments:          &&B2
//
// Purpose: Between the end of the original host and the entry point is a
//          bunch of scratchpad and repair data for this virus.  The XOR
//          decrypt key is easy to figure out, because several strings of 0's
//          were "encrypted".  We read this encrypted data, get key, decrypt,
//          and fix up the header of the file.
//
// lpCallBack           : pointer to callback structure for FileOpen, etc.
// lpsN30               : pointer to N30 structure containing VIRSCAN.DAT
//                        information and virus stats
// lpszFileName         : name of the file to repair
// lpbyWorkBuffer       : temporary 2-4K work buffer.
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK          if OK
//                        EXTSTATUS_NO_REPAIR   if unable to repair
//                        EXTSTATUS_FILE_ERROR  if file or disk error

#define BMAN2LEN    3372    // Infection Length
#define BMAN2EPOFF  0x1fc   // Offset of entry point
#define BMAN2DECKEY 0x90    // Offset of decryption key
#define BMAN2FIXOFF 0       // Offset of fix data
#define BMAN2READ   0x34    // Amount of bytes to read for fix data
#define BMAN2IPFIX  4       // Offset of IP in fix data
#define BMAN2CSFIX  6       // CS
#define BMAN2SSFIX  8       // SS
#define BMAN2SPFIX  0x0a    // SP
#define BMAN2BKMK   0x31    // Bookmark location in fix data

WORD EXTRepairFileBatman2(LPCALLBACKREV1    lpCallBack,
                            LPN30           lpsN30,
                            LPTSTR          lpszFileName,
                            LPBYTE          lpbyWorkBuffer)
{
    WORD        wRetVal, wKey, wBookmark;
    DWORD       dwReadPos, dwFileLen;
    HFILE       hFile;
    UINT        uAttr;
    EXEHEADER   ExeHdr;
    LPWORD      lpwBuf;

    // _asm    int 3                    // Remove after debugging! &&B2

    wRetVal = EXTSTATUS_FILE_ERROR;
 
    if (0 != FILEOPEN())                // Open file
        return(wRetVal);
                                        // Get file header
    if (sEXEHEADER == FILEREAD((LPBYTE) &ExeHdr, sEXEHEADER))
    {
        dwReadPos = GetEntryPtOffset((LPBYTE) &ExeHdr);
                                        // Goto entry point
        dwReadPos -= (BMAN2EPOFF - BMAN2DECKEY);
                                        // Goto decrypt key
        if (dwReadPos == FILESEEK(dwReadPos))
        {                               // Read it
            if (sizeof(WORD) == FILEREAD((LPBYTE) &wKey, sizeof(WORD)))
            {
                dwReadPos -= BMAN2DECKEY;   // Point to fix data

                if (dwReadPos == FILESEEK(dwReadPos))
                {                           // Read fix data
                    if (BMAN2READ == FILEREAD(lpbyWorkBuffer, BMAN2READ))
                    {
                        wKey = WENDIAN(wKey);   // Convert if necessary
                                                // Get bookmark
                        wBookmark = lpbyWorkBuffer[BMAN2BKMK];
                        wBookmark <<= 8;
                        wBookmark |= lpbyWorkBuffer[BMAN2BKMK + 1];
 
                        wBookmark ^= wKey;  // Decrypt it

                        if (ISEXEHDR(&wBookmark))
                        {
                            lpwBuf = (LPWORD) (lpbyWorkBuffer + BMAN2IPFIX);

                            ExeHdr.exInitIP = AVDEREF_WORD(lpwBuf) ^ wKey;
                            lpwBuf++;
                            ExeHdr.exInitCS = AVDEREF_WORD(lpwBuf) ^ wKey;
                            lpwBuf++;
                            ExeHdr.exInitSS = AVDEREF_WORD(lpwBuf) ^ wKey;
                            lpwBuf++;
                            ExeHdr.exInitSP = AVDEREF_WORD(lpwBuf) ^ wKey;

                            dwFileLen = FILESIZE();         // Get file size

                            if (DWERROR != dwFileLen)
                            {
                                dwFileLen -= BMAN2LEN;

                                ExeHdr.exExtraBytes =
                                                (WORD) (dwFileLen & 0x1ff);
                                        // Convert byte count to page count
                                dwFileLen >>= 9;

                                if (0 != ExeHdr.exExtraBytes)
                                    dwFileLen++;

                                ExeHdr.exPages = (WORD) dwFileLen;
 
                                ExeHdr.exInitSP = WENDIAN(ExeHdr.exInitSP);
                                ExeHdr.exInitSS = WENDIAN(ExeHdr.exInitSS);
                                ExeHdr.exInitIP = WENDIAN(ExeHdr.exInitIP);
                                ExeHdr.exInitCS = WENDIAN(ExeHdr.exInitCS);
                                ExeHdr.exExtraBytes =
                                                WENDIAN(ExeHdr.exExtraBytes);
                                ExeHdr.exPages = WENDIAN(ExeHdr.exPages);

                                if (0 == SEEK_TOF())    // Write new header
                                    if (sEXEHEADER ==
                                      FILEWRITE((LPBYTE) &ExeHdr, sEXEHEADER))
                                        if (0 == FILETRUNCATE(BMAN2LEN))
                                            wRetVal = EXTSTATUS_OK;
                            }
                        }
                        else
                            wRetVal = EXTSTATUS_NO_REPAIR;
                    }
                }
            }
        }
    }

    CLOSEOUT(wRetVal);
}   // EXTRepairFileBatman2

// EXTRepairFileTentatrick arguments:   &&TT
//
// Purpose: This virus usually, but not always, puts itself at the end of
//          a host.  On occaision it installs itself in the middle of the
//          host, requiring a bunch of data to be move for the repair.  At
//          the entrypoint of the virus is a far jump, and the locations for
//          everything are based on that.
//
// lpCallBack           : pointer to callback structure for FileOpen, etc.
// lpsN30               : pointer to N30 structure containing VIRSCAN.DAT
//                        information and virus stats
// lpszFileName         : name of the file to repair
// lpbyWorkBuffer       : temporary 2-4K work buffer.
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK          if OK
//                        EXTSTATUS_NO_REPAIR   if unable to repair
//                        EXTSTATUS_FILE_ERROR  if file or disk error

#define TTBACK  10424       // Distance back from entry pt to decrypt loop
#define TTEND   72          // Distance from entry pt to end of virus
#define TTFIX   0x2873      // Offset from decrypt loop to fix data
#define TTSTAT  0x2c        // Offset of 1st static bytes from decrypt loop
#define TTBYTE1 0x1e        // 1st static bytes of virus to derive
#define TTBYTE2 0x06        // decryption key
#define TTDBUFL 0x2e        // decrypt buffer length
#define TTOLDFL 0x2a        // offset of original host length in dec buffer

WORD EXTRepairFileTentatrick(LPCALLBACKREV1 lpCallBack,
                                LPN30       lpsN30,
                                LPTSTR      lpszFileName,
                                LPBYTE      lpbyWorkBuffer)
{
    BYTE        bDecCheck[2], bDecKey;
    LPBYTE      bpFix;
    BYTE        bDecUnEnc[2] = { TTBYTE1, TTBYTE2 };
    WORD        wReloc, wRes, wRetVal;
    WORD        wFixData[TTDBUFL / 2];
    DWORD       dwEntryPt, dwFileLen, dwReadPos, dwOrigLen, dwRes, dwVirTop,
                dwVirLen, dwWritePos;
    HFILE       hFile;
    UINT        uAttr;
    EXEHEADER   ExeHdr;

    // _asm    int 3                    // Remove after debugging! &&TT
 
    wRetVal = EXTSTATUS_FILE_ERROR;
 
    if (0 != FILEOPEN())                // Open file
        return(wRetVal);
                                        // Get file header
    if (sEXEHEADER != FILEREAD((LPBYTE) &ExeHdr, sEXEHEADER))
        CLOSEOUT(wRetVal);

    dwEntryPt = GetEntryPtOffset((LPBYTE) &ExeHdr);
    dwFileLen = FILESIZE();             // Get file size

    if ((0 == dwEntryPt) || (DWERROR == dwFileLen))
        CLOSEOUT(wRetVal);

    dwVirTop = dwEntryPt - TTBACK;      // Point to top of virus (w/o padding)
    dwReadPos = dwVirTop + TTSTAT;      // Find 1st static bytes

    dwRes = FILESEEK(dwReadPos);        // Go to them

    if (dwRes != dwReadPos)
        CLOSEOUT(wRetVal);

    wRes = FILEREAD(bDecCheck, 2);      // Read them

    if (2 != wRes)
        CLOSEOUT(wRetVal);

    bDecCheck[0] ^= bDecUnEnc[0];       // Compute decrypt key from
    bDecCheck[1] ^= bDecUnEnc[1];       // known bytes

    if (bDecCheck[0] != --bDecCheck[1])
        CLOSEOUT(wRetVal);

    wRes = TTFIX - TTSTAT + bDecCheck[0];   // Find decrypt key for fix data
    bDecKey = (BYTE) (wRes & 0x00ff);

    dwReadPos = dwVirTop + TTFIX;

    dwRes = FILESEEK(dwReadPos);

    if (dwRes != dwReadPos)
        CLOSEOUT(wRetVal);

    bpFix = (LPBYTE) wFixData;
    wRes = FILEREAD(bpFix, TTDBUFL);        // Read data to decrypt

    if (TTDBUFL != wRes)
        CLOSEOUT(wRetVal);

    for (wRes = 0; wRes < TTDBUFL; wRes++)  // Decrypt the fix data
        bpFix[wRes] ^= bDecKey++;
                                            // Get original host length
    dwOrigLen = AVDEREF_DWORD((LPDWORD) (bpFix + TTOLDFL));
    dwVirLen = dwFileLen - dwOrigLen;       // Compute total virus length

                        // Write out fix data
    ExeHdr.exInitSP = WENDIAN(wFixData[0]);
    ExeHdr.exInitSS = WENDIAN(wFixData[1]);
    ExeHdr.exInitIP = WENDIAN(wFixData[2]);
    ExeHdr.exInitCS = WENDIAN(wFixData[3]);
                        // Virus decrements maxalloc, increments reloc items
    ExeHdr.exMaxAlloc = WENDIAN(ExeHdr.exMaxAlloc) + 1;
 
    wReloc = WENDIAN(ExeHdr.exRelocItems) - 1;
    ExeHdr.exRelocItems = wReloc;   // Save decrement reloc count

    wReloc <<= 2;       // Compute location of last reloc item
    wReloc += WENDIAN(ExeHdr.exRelocTable);

    wFixData[0] = 0;    // Make zeroes to overwrite last reloc item
    wFixData[1] = 0;
 
    ExeHdr.exExtraBytes = (WORD) (dwOrigLen & 0x1ff);
 
    dwOrigLen >>= 9;                // Convert byte count to page count

    if (0 != ExeHdr.exExtraBytes)
        dwOrigLen++;

    ExeHdr.exPages = (WORD) dwOrigLen;

    ExeHdr.exExtraBytes = WENDIAN(ExeHdr.exExtraBytes);
    ExeHdr.exPages = WENDIAN(ExeHdr.exPages);
    ExeHdr.exRelocItems = WENDIAN(ExeHdr.exRelocItems);
    ExeHdr.exMaxAlloc = WENDIAN(ExeHdr.exMaxAlloc);
    ExeHdr.exInitSS = WENDIAN(ExeHdr.exInitSS);
    ExeHdr.exInitSP = WENDIAN(ExeHdr.exInitSP);
    ExeHdr.exInitIP = WENDIAN(ExeHdr.exInitIP);
    ExeHdr.exInitCS = WENDIAN(ExeHdr.exInitCS);

    dwRes = SEEK_TOF();

    if (0 != dwRes)
        CLOSEOUT(wRetVal);

    wRes = FILEWRITE((LPBYTE) &ExeHdr, sEXEHEADER);

    if (sEXEHEADER != wRes)
        CLOSEOUT(wRetVal);

    dwRes = FILESEEK((DWORD) wReloc);

    if (dwRes != (DWORD) wReloc)    // Seek to last reloc table entry
        CLOSEOUT(wRetVal);

    wRes = FILEWRITE((LPBYTE) wFixData, 4); // Overwrite it w/ 0's

    if (4 != wRes)
        CLOSEOUT(wRetVal);

    if (TTEND < (dwFileLen - dwEntryPt))    // If entry pt is far from EOF
    {
        dwReadPos = dwEntryPt + TTEND;      // Start reading here
        dwWritePos = dwReadPos - dwVirLen;  // Start writing here
        dwRes = dwFileLen - dwReadPos;      // How many bytes to move

        wRes = RepairFileMoveBytes(lpCallBack, hFile, lpbyWorkBuffer,
                                    dwReadPos, dwWritePos, dwRes);
        if (0 != wRes)
            CLOSEOUT(wRetVal);
    }

    if (0 == FILETRUNCATE(dwVirLen))
        wRetVal = EXTSTATUS_OK;

    CLOSEOUT(wRetVal);
}   // EXTRepairFileTentatrick

//#if defined(SYM_DOSX) || !defined(SYM_DOS)

// EXTRepairFileManuel arguments:   &&M
//
// Purpose: This virus uses XOR encryption with a static value.  There are
//          actually two encryption loops, but the two XORs can be simplified
//          into one.  Between the two encryption loops is a spin loop that
//          waits for the timer tick to change, which confounds PAM.  The
//          repair data for this is not encrypted, but a Navex repair is
//          needed because the CS and SS repair values stored are subtracted
//          from the current CS and SS value.  If not for this, virscan would
//          work.
//
// lpCallBack           : pointer to callback structure for FileOpen, etc.
// lpsN30               : pointer to N30 structure containing VIRSCAN.DAT
//                        information and virus stats
// lpszFileName         : name of the file to repair
// lpbyWorkBuffer       : temporary 2-4K work buffer.
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK          if OK
//                        EXTSTATUS_NO_REPAIR   if unable to repair
//                        EXTSTATUS_FILE_ERROR  if file or disk error

#define MANLEN      2209        // Length of Virus
#define MANCOMFIX   0x0896      // COM fix data
#define MANCS_FIX   0x0896      // CS Fix Data Location
#define MANIP_FIX   0x0898      // IP Location
#define MANSS_FIX   0x089d      // SS Fix Data Location
#define MANSP_FIX   0x089f      // SP Location
#define MANFIXLEN   (MANLEN - MANCOMFIX)    // Length of fix data buffer
#define MANCOMLEN   3                       // Number of bytes to fix COM file

WORD EXTRepairFileManuel(LPCALLBACKREV1 lpCallBack,
                            LPN30       lpsN30,
                            LPTSTR      lpszFileName,
                            LPBYTE      lpbyWorkBuffer)
{
    BOOL        exefile;
    WORD        wRes, wRetVal, wWrtLen;
    DWORD       dwFileLen, dwReadPos, dwRes;
    HFILE       hFile;
    UINT        uAttr;
    EXEHEADER   ExeHdr;

    // _asm    int 3                       // Remove after debugging! &&M
 
    wRetVal = EXTSTATUS_FILE_ERROR;
 
    if (0 != FILEOPEN())                // Open file
        return(wRetVal);
                                        // Get file header
    if (sEXEHEADER != FILEREAD((LPBYTE) &ExeHdr, sEXEHEADER))
        CLOSEOUT(wRetVal);

    exefile = ISEXEHDR((LPWORD) &ExeHdr);

    dwFileLen = FILESIZE();             // Get file size

    dwReadPos = dwFileLen - MANLEN + MANCOMFIX; // Find fix and constant data

    dwRes = FILESEEK(dwReadPos);        // Go to it

    if (dwRes != dwReadPos)
        CLOSEOUT(wRetVal);

    wRes = FILEREAD(lpbyWorkBuffer, MANFIXLEN); // Read it

    if (MANFIXLEN != wRes)
        CLOSEOUT(wRetVal);

    if (TRUE == exefile)                        // Write out fix data
    {                                           // EXE repair
        ExeHdr.exInitCS = WENDIAN(ExeHdr.exInitCS) -
                        AVDEREF_WORD(lpbyWorkBuffer + MANCS_FIX - MANCOMFIX);
        ExeHdr.exInitIP =
                        AVDEREF_WORD(lpbyWorkBuffer + MANIP_FIX - MANCOMFIX);
        ExeHdr.exInitSS = WENDIAN(ExeHdr.exInitSS) -
                        AVDEREF_WORD(lpbyWorkBuffer + MANSS_FIX - MANCOMFIX);
        ExeHdr.exInitSP =
                        AVDEREF_WORD(lpbyWorkBuffer + MANSP_FIX - MANCOMFIX);

        dwFileLen -= MANLEN;

        ExeHdr.exExtraBytes = (WORD) (dwFileLen & 0x1ff);
 
        dwFileLen >>= 9;                // Convert byte count to page count

        if (0 != ExeHdr.exExtraBytes)
            dwFileLen++;

        ExeHdr.exPages = (WORD) dwFileLen;

        ExeHdr.exExtraBytes = WENDIAN(ExeHdr.exExtraBytes);
        ExeHdr.exPages = WENDIAN(ExeHdr.exPages);
        ExeHdr.exInitSS = WENDIAN(ExeHdr.exInitSS);
        ExeHdr.exInitSP = WENDIAN(ExeHdr.exInitSP);
        ExeHdr.exInitIP = WENDIAN(ExeHdr.exInitIP);
        ExeHdr.exInitCS = WENDIAN(ExeHdr.exInitCS);

        wWrtLen = sEXEHEADER;       // Number of bytes to write
    }
    else                                        // COM Repair
    {
        wWrtLen = MANCOMLEN;
 
        for (wRes = 0; wRes < wWrtLen; wRes++)
            *(((LPBYTE) &ExeHdr) + wRes) = lpbyWorkBuffer[wRes];
    }
 
    dwRes = SEEK_TOF();

    if (0 != dwRes)
        CLOSEOUT(wRetVal);

    wRes = FILEWRITE((LPBYTE) &ExeHdr, wWrtLen);

    if (wWrtLen != wRes)
        CLOSEOUT(wRetVal);

    if (0 == FILETRUNCATE(MANLEN))
        wRetVal = EXTSTATUS_OK;

    CLOSEOUT(wRetVal);
}   // EXTRepairFileManuel

//#endif

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
                     UINT FAR *lpuAttr)
{
                                        // Save attributes
    if ((UINT)0 != lpCallBack->FileGetAttr((LPTSTR)lpszFileName, lpuAttr))
    {
        return ((WORD)-1);
    }
                                        // Clear attributes
    if ((UINT)0 != lpCallBack->FileSetAttr((LPTSTR)lpszFileName, 0))
    {
        return ((WORD)-1);
    }
                                        // Open READWRITE
    if ((HFILE) -1 == (*lphFile = lpCallBack->FileOpen((LPTSTR)lpszFileName,2)))
    {
        return ((WORD)-1);
    }

    return (0);
}

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
                     UINT  uAttr)
{

                                        // Close file
    if ((HFILE) -1 == lpCallBack->FileClose(hFile))
    {
        return ((WORD)-1);
    }
                                        // Restore attributes
    if ((UINT)0 != lpCallBack->FileSetAttr((LPTSTR)lpszFileName, uAttr))
    {
        return ((WORD)-1);
    }

    return ((WORD)0);
}

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
                         long           lOffset)
{


    if ((DWORD)-1 == lpCallBack->FileSeek(hFile, -lOffset, SEEK_END))
    {
        return ((WORD)-1);
    }
                                        // Truncate file

    if ((UINT) -1 == lpCallBack->FileWrite(hFile, NULL, 0))
    {
        return ((WORD)-1);
    }

    return (0);
}

// RepairFileMoveBytes
//
// lpCallBack           : pointer to callback structure for FileOpen, etc.
// hFile                : File handle
// lpbyWorkBuffer       : Buffer to use while moving data - must be at least
//                          REPMOVBUFSZ in size.
// dwBegRead            : File offset to begin reading data
// dwBegWrite           : File offset to begin writing data - this should be
//                          less than dwBegRead
// dwNumBytes           : Number of bytes to move
//
// Returns:
//
//     WORD             : 0            on success
//                      : non-zero     on failure
//
// This routine moves blocks of bytes towards the beginning of the file.
// This reads dwNumbytes starting at a file offset of dwBegRead, and writes
// them at file offset dwBegWrite.  dwBegWrite *must* be less than dwBegRead!
// dwBegRead and dwBegWrite must not be past EOF.  There should be *at least*
// dwNumBytes between dwBegRead and EOF.  Bytes in the file file past
// (dwBegRead + dwNumBytes) are not touched.  This does not truncate the file.

#define REPMOVBUFSZ     0x400           // Size of buffer used to move data

WORD RepairFileMoveBytes (LPCALLBACKREV1    lpCallBack,
                            HFILE           hFile,
                            LPBYTE          lpbyWorkBuffer,
                            DWORD           dwBegRead,
                            DWORD           dwBegWrite,
                            DWORD           dwNumBytes)
{
    WORD    wMove;          // Number of bytes to move in current pass
 
    do                      // Move data towards beginning of file
    {
        wMove = (REPMOVBUFSZ < dwNumBytes) ? REPMOVBUFSZ : (WORD) dwNumBytes;
 
        if (dwBegRead != FILESEEK(dwBegRead))
            return(WERROR);

        if (wMove != FILEREAD(lpbyWorkBuffer, wMove))   // Read data to move
            return(WERROR);
 
        if (dwBegWrite != FILESEEK(dwBegWrite))
            return(WERROR);

        if (wMove != FILEWRITE(lpbyWorkBuffer, wMove))  // Write data
            return(WERROR);

        dwNumBytes -= wMove;                            // Update pointers
        dwBegRead += wMove;                             // and variables
        dwBegWrite += wMove;
    } while (0 != dwNumBytes);

    return(0);
}

// RepairFileYKey_Karn arguments:
//
// lpCallBack           : pointer to callback structure for FileOpen, etc.
// lpszFileName         : name of the file to repair
// lpbyWorkBuffer       : temporary 2-4K work buffer.
// woffsetEOF           : Offset from the end of file where EXE header info
//                        is available
//
// Returns:
//
//     Word             : 0          if OK
//                      : non-zero   if unable to repair
//
// It builts an EXE header from the virus code based on the given offset
// form the end of file. This function is added because of similar repair
// to both Y-key.2346 and Karnivali.1971
//

WORD RepairFileYKey_Karn (LPCALLBACKREV1 lpCallBack,
                        LPTSTR         lpszFileName,
                        LPBYTE         lpbyWorkBuffer,
                        int            offsetEOF)
{
    BYTE    byHeader[0x18],
            byRegs[8];
    DWORD   dwImgSize;
    HFILE   hFile;
    UINT    uAttr;

 
    // Open file and read in infected Header

    if (RepairFileOpen(lpCallBack, lpszFileName, &hFile, &uAttr))
    {
        return((WORD)-1);
    }

    if ((UINT) -1 == lpCallBack->FileRead(hFile, (LPBYTE)byHeader, 0x18))
    {
        RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
        return((WORD)-1);
    }


    // Seek to where the original values of the registers are
    // and read them into a buffer.

    if ((DWORD)-1 == lpCallBack->FileSeek(hFile, offsetEOF, SEEK_END))
    {
        RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
        return((WORD)-1);
    }

    if ((UINT) -1 == lpCallBack->FileRead(hFile, (LPBYTE)byRegs, 8))
    {
        RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
        return((WORD)-1);
    }


    // Read in the first 14 bytes of the original EXE header that
    // the virus has saved for us.

    if ((UINT) -1 == lpCallBack->FileRead(hFile, (LPBYTE)byHeader, 0x0e))
    {
        RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
        return((WORD)-1);
    }

 
    // Put the originals regs in there proper places in the Header.
    // We must calulate CS and SS since their original values are
    // not saved for us.

    AVASSIGN_WORD((byHeader + 0x0e), AVDEREF_WORD(byHeader + 0x16) -
                                     AVDEREF_WORD(byRegs + 6));

    AVASSIGN_WORD((byHeader + 0x16), AVDEREF_WORD(byHeader + 0x16) -
                                     AVDEREF_WORD(byRegs + 2));

    AVASSIGN_WORD((byHeader + 0x10), AVDEREF_WORD(byRegs + 4));

    AVASSIGN_WORD((byHeader + 0x14), AVDEREF_WORD(byRegs + 0));


    // Seek to the beginning of the file and write out our
    // newly rebuilt header.

    if ((DWORD)-1 == lpCallBack->FileSeek(hFile, 0, SEEK_SET))
    {
        RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
        return((WORD)-1);
    }

    if ((UINT)-1 == lpCallBack->FileWrite(hFile, (LPBYTE)byHeader, 0x18))
    {
        RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
        return((WORD)-1);
    }


    // Calculate the orginal size of the file using the info in
    // the uninfected EXE header.
 
    dwImgSize = ((DWORD(AVDEREF_WORD(byHeader + 4) - 1)) * 0x200) +
                  AVDEREF_WORD(byHeader + 2);


    // Then seek to that point and truncate.

    if ((DWORD)-1 == lpCallBack->FileSeek(hFile, (LONG) dwImgSize, SEEK_SET))
    {
        RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
        return((WORD)-1);
    }

    if ((UINT)-1 == lpCallBack->FileWrite(hFile, NULL, 0))
    {
         RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr);
         return((WORD)-1);
    }

    if (RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr))
    {
        return((WORD)-1);
    }

    return (0);
}

#if defined(SYM_DOS)

MODULE_EXPORT_TABLE_TYPE module_export_table =
{
  EXPORT_STRING,
  (void (*)())EXTRepairFile
};

void main(void)
{
}

#endif

#endif  // #ifndef SYM_NLM
