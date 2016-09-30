//************************************************************************
//
// $Header:   S:/NAVEX/VCS/macrovid.h_v   1.635   15 Jan 1999 12:47:06   relniti  $
//
// Description:
//      Contains NAVEX macro VIDs.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/macrovid.h_v  $
// 
//    Rev 1.635   15 Jan 1999 12:47:06   relniti
// #11031 0x6532,33 VID_SUGARB, VID_SUGARBPAYLOAD, X97M.Sugar.B and
//  W97M.Sugar.B.Payload
// 
//    Rev 1.634   14 Jan 1999 18:17:18   relniti
// 0x6531 VID_LAROUXEO_VAR1 XM.Laroux.EO.var
// 
//    Rev 1.633   14 Jan 1999 15:23:04   relniti
// 0X6530 VID_LAROUXBO_VAR1 XM.Laroux.BO.var
// 
//    Rev 1.632   14 Jan 1999 11:40:42   relniti
// VID_LAROUXDX_VAR1 0x652F
// 
//    Rev 1.631   13 Jan 1999 13:52:20   CFORMUL
// Added VID_ETHANA
// 
//    Rev 1.630   12 Jan 1999 18:43:16   DKESSNE
// added LAROUXFX 0x652d and LAROUXFY 0x652e
// 
//    Rev 1.629   12 Jan 1999 14:16:28   relniti
// #10513 W97M.vmpck1.bg VID_VMPCK1BG
// 
//    Rev 1.628   08 Jan 1999 17:18:40   relniti
// RENAME 0x61fc XM.Laroux.FW => XM.Forecast
// add 0x652b VID_LAROUXFW XM.Laroux.FW for #10750
// 
//    Rev 1.627   07 Jan 1999 15:19:30   relniti
// #10676: 0x652a X97M.Laroux.HN VID_LAROUXHN
// rename Laroux.DW => Laroux.EI
// 
//    Rev 1.626   06 Jan 1999 16:53:20   relniti
// #10559 VID_LAROUXHK 0x6529 X97M.Laroux.HK
// 
//    Rev 1.625   30 Dec 1998 14:24:48   CFORMUL
// Added VID_AMGA
// 
//    Rev 1.624   29 Dec 1998 11:05:12   ECHIEN
// Added niceday.aa
// 
//    Rev 1.623   23 Dec 1998 01:49:58   relniti
// #9721 0x6525 VID_COLDAPEC W97M.ColdApe.C
// 
//    Rev 1.622   22 Dec 1998 09:11:40   AOONWAL
// Added #define VID_LAROUXFG  0x6524
// 
//    Rev 1.621   15 Dec 1998 12:06:38   DCHI
// Added #define for VID_VICA.
// 
//    Rev 1.620   15 Dec 1998 11:12:44   relniti
// 0x6521 VID_SCHUMANNAA WM.Schumann.AA & 0x6522 VID_VMPCK1BF W97M.VMPCK1.BF
// 
//    Rev 1.619   14 Dec 1998 16:52:14   ppak
// CI 8951: Added VID_NOTTICEK
// 
//    Rev 1.618   13 Dec 1998 13:51:38   ppak
// CI 9141: Added VID_MDMABK
// 
//    Rev 1.617   11 Dec 1998 14:33:14   relniti
// wm.mentes.c 651e
// 
//    Rev 1.616   11 Dec 1998 11:45:36   ppak
// Screwed up, changed 0x651d to Wazzu_FQ (was FK).
// 
//    Rev 1.615   11 Dec 1998 11:38:14   ppak
// Added VID for 651D.
// 
//    Rev 1.614   08 Dec 1998 19:23:50   ppak
// CI 8749: Added VID for VMPCK1.BE
// 
//    Rev 1.613   08 Dec 1998 14:34:56   DKESSNE
// added TWNOARTW 0x6519
// 
//    Rev 1.612   07 Dec 1998 15:30:50   ppak
// CI 8047:  Added VID for Imposter.G
// 
//    Rev 1.611   03 Dec 1998 16:42:58   CFORMUL
// Added VID_BAUER
// 
//    Rev 1.610   02 Dec 1998 13:57:38   CFORMUL
// Added VID_FOOTPRINT
// 
//    Rev 1.609   30 Nov 1998 14:30:34   CFORMUL
// Added STALLC
// 
//    Rev 1.608   30 Nov 1998 11:04:52   ECHIEN
// Changed Raul's VMacro additions due to dumplicate VID names
// 
//    Rev 1.607   29 Nov 1998 21:01:24   relniti
// VMacro XM November 98
// 
//    Rev 1.606   29 Nov 1998 20:23:50   relniti
// VMacro W97M November98
// 
//    Rev 1.605   29 Nov 1998 19:13:34   relniti
// VMacro Processing November 98
// 
//    Rev 1.604   25 Nov 1998 20:28:28   relniti
// #8171 0x650e VID_SETMDD for WM.Setmd.D
// 
//    Rev 1.603   25 Nov 1998 16:42:50   relniti
// W97M.Steroid.Variant VID_STEROIDVARIANT 0x650d
// 
//    Rev 1.602   24 Nov 1998 22:20:38   relniti
// #7757: W97M.Passbox.C VID_PASSBOXC 0x650c
// 
//    Rev 1.601   24 Nov 1998 20:34:08   relniti
// #7738: 0x650B VID_CARRIERD W97M.Carrier.D
// 
//    Rev 1.600   24 Nov 1998 15:50:12   relniti
// #8124 0x650A XM.Laroux.FA
// 
//    Rev 1.599   23 Nov 1998 19:26:04   relniti
// #8045: XM.Laroux.GG VID_LAROUXGG 0x6509
// 
//    Rev 1.598   23 Nov 1998 19:01:02   relniti
// #8045 W97M.Chack variant VID_CHACKVARIANT 0x6508
// 
//    Rev 1.597   23 Nov 1998 16:43:16   relniti
// #7328 0x6507 VID_ZMKO W97M.ZMK.O
// 
//    Rev 1.596   23 Nov 1998 13:50:02   ppak
// CI 7942:  Added entry for Cartman.E
// 
//    Rev 1.595   22 Nov 1998 11:52:24   relniti
// VID_TOXD AM.Tox.D 0x64b9
// 
//    Rev 1.594   20 Nov 1998 19:25:26   relniti
// add VID_DUMBSAMPLE for Access Macro Dumb sample
// 
//    Rev 1.593   17 Nov 1998 13:12:18   ECHIEN
// Added dwmvck1C
// 
//    Rev 1.592   16 Nov 1998 15:36:10   AOONWAL
// Changed VID_MALAYSIA1998 to VID_PROTECEDA
// 
//    Rev 1.591   16 Nov 1998 15:28:20   AOONWAL
// Added #define VID_MALAYSIA1998  0x6503
// 
//    Rev 1.590   16 Nov 1998 12:48:12   AOONWAL
// Added #define VID_LAROUXEZ  0x6502
// 
//    Rev 1.589   13 Nov 1998 23:35:54   Relniti
// #7358 W97M.VMPCK1.BD 0x6501 VID_VMPCK1BD
// 
//    Rev 1.588   13 Nov 1998 21:06:02   Relniti
// VMACRO Processing 111398 XM.VCX.A,B,C 64b0,b6,b7
// 
//    Rev 1.587   13 Nov 1998 17:01:48   Relniti
// #7401: VID_LAROUXGA 0x6500 XM.Laroux.GA
// 
//    Rev 1.586   12 Nov 1998 18:56:40   Relniti
// #7323: XM.Cauli.A 61ff VID_CAULI
// 
//    Rev 1.585   09 Nov 1998 17:36:14   Relniti
// VID_COPYCAP_VARIANT 61fe WM.CopyCap.variant
// 
//    Rev 1.584   09 Nov 1998 11:23:46   Relniti
// VMacro Processing: 64af, b1-b5, b8:
//  W97M.Astia.C, Chack.L, Godzilla.A, Iis.C, Nightshade.D, Nottice.J, VMPCK1.BC
// 
//    Rev 1.583   09 Nov 1998 10:16:00   Relniti
// VMacro Processing 110998: 64a1-ae:
//  WM.Aber.A, Alliance.I, Concept.CO, CP, Fries.A, Mental.D, Niceday.Z,
//     NOP.Z:De, Tanjung.A, Toten.D:De, Wazzu.FJ, FL, FM, FO
// 
//    Rev 1.582   09 Nov 1998 09:03:24   Relniti
// change VID_INTELTESTFILE to 64a0 from 63f4
// 
//    Rev 1.581   06 Nov 1998 18:07:44   ECHIEN
// added wazzufp
// 
//    Rev 1.580   06 Nov 1998 12:22:54   ECHIEN
// Added LarouxFw
// 
//    Rev 1.579   05 Nov 1998 15:50:14   ECHIEN
// Added VampireP
// 
//    Rev 1.578   05 Nov 1998 13:47:20   CFORMUL
// Added VID_PAIXDAM
// 
//    Rev 1.577   05 Nov 1998 12:12:16   ECHIEN
// Added badtempa instead of badtemp
// 
//    Rev 1.576   04 Nov 1998 19:11:30   ECHIEN
// Added BadTemp
// 
//    Rev 1.575   04 Nov 1998 17:47:32   Relniti
// #6675: VID_LMN 649F for W97M.LMN.A
// 
//    Rev 1.574   02 Nov 1998 22:56:04   ECHIEN
// Added Chack.I and Chack.J
// 
//    Rev 1.573   02 Nov 1998 15:24:06   Relniti
// #6500: sample from Virus Bulletin's Nick Fitzgerald
// VID_COLDAPE 649E W97M.ColdApe.A
// 
//    Rev 1.572   02 Nov 1998 15:11:56   CFORMUL
// Adde VMPCK1BB
// 
//    Rev 1.571   02 Nov 1998 12:05:06   ECHIEN
// repalced chacki with warninge
// 
//    Rev 1.570   02 Nov 1998 10:52:52   ECHIEN
// Added Chack.I
// 
//    Rev 1.569   29 Oct 1998 16:39:26   ppak
// Added VID_FORMATERB.
// 
//    Rev 1.568   29 Oct 1998 12:38:06   DKESSNE
// added VID_VMPCK1BA 0x61f1
// 
//    Rev 1.567   28 Oct 1998 14:26:02   DKESSNE
// added VID_LAROUXFO 0x61f0
// 
//    Rev 1.566   28 Oct 1998 12:57:26   DKESSNE
// added VID_SCHUMANNY and VID_SCHUMANNZ, 0x61ee and 0x61ef
// 
//    Rev 1.565   23 Oct 1998 16:58:32   relniti
// #5814 VID_STEROIDJ 61EC W97M.Steroid.J
// 
//    Rev 1.564   23 Oct 1998 16:08:28   relniti
// #6061: VID_LAROUXFM 61EB XM.Laroux.FM and its downconvert
// 
//    Rev 1.563   23 Oct 1998 13:44:16   AOONWAL
// Added #define VID_LAROUXFL 0x61ea
// 
//    Rev 1.562   22 Oct 1998 18:46:16   AOONWAL
// Added #define VID_WAZZUFK  0x61e9
// 
//    Rev 1.561   22 Oct 1998 12:52:26   AOONWAL
// Added define VID_LAROUXEO 0x61e8
// 
//    Rev 1.560   21 Oct 1998 14:00:36   relniti
// zoo: 6496-9a W97M.Encr.A, Minimal.I, N, NJWMDLK1.H, Carrier.B
// 
//    Rev 1.559   21 Oct 1998 13:28:34   relniti
// ZOO: 6492-95: W97M.Class.E,G,K,L
// 
//    Rev 1.558   21 Oct 1998 12:57:50   relniti
// VMacro: 648E-91: XM.Dado.A, Hidemod.A, Perfid.A, Spellchecker.A
// 
//    Rev 1.557   21 Oct 1998 12:34:18   relniti
// VMacro: ADD XF.Sic.A 648D
// 
//    Rev 1.556   21 Oct 1998 12:02:02   relniti
// VMacro 102198: reuse 646A-6B, add 648A-8C for
//  W97M.Iis.A, Metamorph.A, Passbox.A, VMPCK2.D,E
// 
//    Rev 1.555   21 Oct 1998 11:26:34   relniti
// VMacro 102198: 6466-6489, & reusing 63c8, 645E, 6464, 6463
//  W97M.VMPCK1.D,L-S,V,X-Z,AA-AX,AZ
// 
//    Rev 1.554   21 Oct 1998 10:45:36   relniti
// VMacro 102198: 6452-6465, and reusing 644B
//  W97M.Iis.B, Inquisitor.A, Jedi.D,E,F, Lena.A, Lizard.A, Lulung.A,
//       Outbreak.A,B, Rubbish.A,B,C, Smac.A, Steroid.H,I, Storm.A, ZMK.N
// 
//    Rev 1.553   21 Oct 1998 09:44:20   relniti
// VMacro 102198: 6448-6451
//  W97M.Beep.A, Carrier.A, Casc.A, Chack.G,H, Cola.A, Dreams.A,B, Gipsy.A,
//       Groov.D
// 
//    Rev 1.552   20 Oct 1998 13:32:52   AOONWAL
// Added #define VID_ANTIMARCA 0x61e7
// 
//    Rev 1.551   20 Oct 1998 07:40:02   relniti
// VMacro Processing 102098: 6435-6447:
//  WM.Padania.D:It, Rats.F, Rehenes.B, Sidor.B, Sig.A, Twno.AL, Twno.AM:Tw,
//  Twno.AN:Tw, VP.A, Wazzu.EV, Wazzu.EW, Wazzu.EY, Wazzu.EZ, Wazzu.FA,
//  Wazzu.FB, Wazzu.FC, Wazzu.FE, Wazzu.FG, Wazzu.FI,
// 
// 
//    Rev 1.550   19 Oct 1998 12:12:18   relniti
// 63BF: VID_PRESIDENTA => VID_PRESIDENTA97
// 
//    Rev 1.549   18 Oct 1998 14:57:18   relniti
// VMacro Processing 101898:
//  6428-6434: Parol.A, President.A, Query.A, Schumann.X:De, Stall.B,
//  Stupid.A.trojan, Svetlana.A.int, Swlabs.R, TM.A, Ultras.C, Unhas.C,
//  Vaca.A, WME.A
// 
//    Rev 1.548   18 Oct 1998 14:28:24   relniti
// VMacro Processing 101698:
// ADD 6412-6427: CopyCap.B, CountTen.F, Ferias.A, Kompu.L, Kompu.M, Kompu.N,
//  Lupita.A, Matey.A, Mental.E, Mercado.C:Br, Milli.A, Miminal.AC, Mortal.A,
//  Munch.B, Narmol.A, Nein.A, Niceday.X, Niceday.y, NJWMDLK1.P, Nop.Y:De
//  NoPrint.C, Nuclear.AD
// 
//    Rev 1.547   15 Oct 1998 20:40:56   relniti
// ADD W97M.Groov.Variant VID_GROOV_VAR 61e6 for #5722, #5723
// 
//    Rev 1.546   14 Oct 1998 20:45:32   relniti
// #5534: XM.Soldier.A VID_SOLDIER 0x61E5
// 
//    Rev 1.545   14 Oct 1998 17:24:34   relniti
// ADD 6134 VID_CLASS_S W97M.Class.S for #5652
// 
//    Rev 1.544   09 Oct 1998 19:21:38   relniti
// VMacro Processing 100898: 6406-6410:
//  WM.FHD.A, Gabi.A, Gob.A, GoldFish.G,H, Hawk.A, Ivana.Variant, Ivana.B,C,D,
//     Kann.B:De
// 
//    Rev 1.543   09 Oct 1998 14:44:44   relniti
// VMacro Processing 100898: 63f5-6405
//  WM.ABC.B, Appder.Y,Z, Bond.A, Concept,CL,CM,CN, Crow.A, DelMacro.A,
//     DelWord.A, Dirty.A, Divina.S,U,V, DMV.M,O,P
// 
//    Rev 1.542   08 Oct 1998 16:20:38   ppak
// Added WM.Panjang.C
// 
//    Rev 1.541   08 Oct 1998 10:16:36   ECHIEN
// Added VERMA
// 
//    Rev 1.540   07 Oct 1998 18:53:46   ECHIEN
// dded Copycapc
// 
//    Rev 1.539   06 Oct 1998 17:39:50   CFORMUL
// Added define for Laroux.FF
// 
//    Rev 1.538   05 Oct 1998 16:40:18   RELNITI
// Added Laroux.FD -ECC
// 
//    Rev 1.537   02 Oct 1998 11:32:14   relniti
// ADD W97M.Solafish.A VID_SOLAFISH 61D8 for #5205
// 
//    Rev 1.536   01 Oct 1998 19:31:40   relniti
// ADD VID_SETMDC 61D7 for W97M.Setmd.C for #5186
// 
//    Rev 1.535   01 Oct 1998 16:04:04   relniti
// ADD 0x61D6 VID_LAROUXFC XM.Laroux.FC for #5179
// 
//    Rev 1.534   29 Sep 1998 15:52:00   relniti
// ADD WM.SC Family using VID_SCFAMILY1, VID_SCFAMILY2 61D5 for #5110
// 
//    Rev 1.533   28 Sep 1998 12:10:02   relniti
// ADD 61D4
// 
//    Rev 1.532   24 Sep 1998 17:42:44   AOONWAL
// Added #define VID_CVCK1J  0x61d3
// 
//    Rev 1.531   24 Sep 1998 17:33:26   relniti
// ADD 63F4 for INTEL LanDesk test file
// 
//    Rev 1.530   24 Sep 1998 14:23:48   AOONWAL
// Added #define VID_LAROUXDX  0x61d2
// 
//    Rev 1.529   16 Sep 1998 15:48:44   JWILBER
// Added #define for WM.CopyCap.B, VID 1ad8, for CI 4514.
//
//    Rev 1.528   10 Sep 1998 19:01:54   relniti
// ADD 61D1 VID_LAROUXEN XM.Laroux.EN for #4521
//
//    Rev 1.527   10 Sep 1998 16:30:48   relniti
// ADD VID_IVANAA 61D0 for WM.Ivana.A on issue #4508
//
//    Rev 1.526   09 Sep 1998 14:30:28   relniti
// ADD/SPLIT O97M.Shiver(63f0) to .A(63f0), .B(63f1) , .C(63f2) as named in VMacro
//
//    Rev 1.525   09 Sep 1998 13:40:54   DKESSNE
// added TWNOAPTW 0x61cc
//
//    Rev 1.524   04 Sep 1998 17:58:58   JWILBER
// Added define for WM.Schumann.V:DE, VID 61bd, for CI 4344.
//
//    Rev 1.523   04 Sep 1998 16:52:30   AOONWAL
// Added
// #define vid_mdmabi  0x61bb
// #define vid_mdmabi  0x61bc
//
//    Rev 1.522   04 Sep 1998 15:50:10   JWILBER
// Added VID_COPYCAP_DAMAGED for VID 61ba, CI 4340.
//
//    Rev 1.521   04 Sep 1998 15:36:36   JWILBER
// Added VID_NUCLEARAD for VID 61b9, CI 4325.
//
//    Rev 1.520   02 Sep 1998 13:14:04   AOONWAL
// Added VID_LENORA 0x61b6
//
//    Rev 1.519   02 Sep 1998 12:59:42   JWILBER
// Added VID for NOP.L, VID 61b4, for CI 4230.
//
//    Rev 1.518   01 Sep 1998 15:46:26   relniti
// VID_SHIVER 63F0 for O97M.Shiver.A
//
//    Rev 1.517   01 Sep 1998 11:43:10   AOONWAL
// Updated from SNDBOT after the corruption in USSM-SARC\VOL1
//
//    Rev 1.517   28 Aug 1998 15:41:10   DCHI
// Added VID_UNUSED 0xFFFF
//
//    Rev 1.516   26 Aug 1998 00:35:32   relniti
// ZOO: 63EF: VID_CLASSF W97M.Class.F (poly a.k.a. Woobie)
//
//    Rev 1.515   25 Aug 1998 23:54:10   relniti
// ZOO: 63ED-EE VID_DWMVCK1GEN, VID_VMPCK1AGEN for W97M.DWMVCK1/ZMK.Gen,
//   W97M.VMPCK1A.Gen
//
//    Rev 1.514   25 Aug 1998 20:35:46   relniti
// ZOO MACRO: ADD VID_VMPCK1CGEN 63EC for W97M.VMPCK1C.Gen
//
//    Rev 1.513   24 Aug 1998 14:12:32   RELNITI
// THE Previous Rev had 2 VID that wasn't supposed to be checked in.
//
//    Rev 1.512   23 Aug 1998 16:59:40   RELNITI
// NO CHANGE
//
//    Rev 1.511   21 Aug 1998 17:08:10   jwilber
// Added #define for WM.Formater.A, VID 61b2, for CI 2679.
//
//    Rev 1.510   21 Aug 1998 09:52:32   RELNITI
// ZOO: 63E7-EB: XM.Disaster.A, AM.Jaring.intended, AM.Accessiv.C,
//  W97M.Bernie.A, W97M.Chaos.A
//
//    Rev 1.509   20 Aug 1998 12:42:54   RELNITI
// VMacro: 63DF-E6: WM.SWLABS.Q, SWT.A.trojan, Toten.C:De, Veneno.B:De,
//  Wazzu.ES,ET,EU, Therese.A
//
//    Rev 1.508   19 Aug 1998 23:14:34   RELNITI
// VMacro 63d6-DE: EMT.B, EZLN.A, Makrone.C, NJ-WMVCK2.U, No_Va.C, Rats.E,
//     Schumann.Q,R,S.
//
//    Rev 1.507   19 Aug 1998 20:43:58   RELNITI
// VMacro 63D0-D5: WM.Colors.BZ, CA, Concept.CK, Counter.C, Criminal.A, Crypt.A
//
//    Rev 1.506   19 Aug 1998 18:03:12   ECHIEN
// Added VMPCK1D
//
//    Rev 1.505   19 Aug 1998 18:01:12   RELNITI
// VMacro O97M.Teocatl.B 63CF
//
//    Rev 1.504   19 Aug 1998 16:41:36   RELNITI
// ADD W97M.Troz.A trojan 63ce
//
//    Rev 1.503   19 Aug 1998 16:29:16   RELNITI
// VMacro: W97M.Boring.A Trojan 63cD
//
//    Rev 1.502   18 Aug 1998 23:58:58   RELNITI
// ZOO Macro: 63c9-63cc W97M.PolyM.A, W97M.Remark, WM.Marbles, WM.Anarchy.5838
//
//    Rev 1.501   18 Aug 1998 15:21:14   ECHIEN
// Added WazzuFD
//
//    Rev 1.500   18 Aug 1998 13:15:04   ECHIEN
// Changed Incarnet to incarnat
//
//    Rev 1.499   18 Aug 1998 12:36:28   ECHIEN
// Added VID_INCARNETA
//
//    Rev 1.498   18 Aug 1998 11:41:10   RELNITI
// ZOOMACRO: ADD VID_MULTIH 63c8 W97M.Multi.H
//
//    Rev 1.497   13 Aug 1998 22:26:52   jwilber
// Checked in VID_NOTTICEF for w97M.Nottice.F, VID 61a5, CI 2612.
//
//    Rev 1.496   13 Aug 1998 20:38:04   jwilber
// Added VID_KITTYA, for VID 61a4, W97M.Kitty.A, for CI 2609.
//
//    Rev 1.495   13 Aug 1998 12:22:56   RELNITI
// add 61a3 WM.MDMA.BH for #2607
//
//    Rev 1.494   12 Aug 1998 18:08:30   RELNITI
// ADD VID_CONCEPTCN 61A0 WM.Concept.CN for #2598
//
//    Rev 1.493   11 Aug 1998 22:18:26   jwilber
// Added #defines for:
// CI   Name           VID
// 2583 WM.Breaktime.A 619c
// 2584 XM.Laroux.DV   619d
// 2585 XM.Laroux.DW   619e
// 2591 W97M.Cartman.D 619f
//
//    Rev 1.492   11 Aug 1998 17:14:14   jwilber
// Added #define VID_LAROUXDU for VID 619b, CI 2578.
//
//    Rev 1.491   11 Aug 1998 12:51:54   RELNITI
// VMacro W97M: 63BE-C6: W97M.Minimal.AB, President.A, Ulang.A, Unhelpful.A,
//   VMPCK1.B, ZMK.I, K, M
//
//    Rev 1.490   11 Aug 1998 10:18:50   RELNITI
// VMacro W97M: MBug.A-I 63b5-bD VID_MBUGA-VID_MBUGI
//
//    Rev 1.489   07 Aug 1998 14:54:22   RELNITI
// VMacro: 63b2-b4 for W97M.Steroid.F,G, Satan.B
//
//    Rev 1.488   07 Aug 1998 14:13:42   RELNITI
// VMacro: 63b1 VID_PAIXB for XF.Paix.B
//
//    Rev 1.487   06 Aug 1998 20:12:22   jwilber
// Added VID_REPLICATOR for W97M.Replicator, VID 6199, for CI 2558.
//
//    Rev 1.486   06 Aug 1998 18:22:14   RELNITI
// VMACRO W97M: 63AC-63B0: WM.Draw.A, Edds.C, Eraser.V, Jedi.A,B
//
//    Rev 1.485   06 Aug 1998 16:45:20   RELNITI
// ADD W97M.Chack.B,C,D from VMacro posting 63A9-AB
//
//    Rev 1.484   06 Aug 1998 10:55:48   RELNITI
// ZOO Macro: W97M.Kitty.B 63a7
//            W97M.Nono.A  63a8
//
//    Rev 1.483   06 Aug 1998 10:07:44   RELNITI
// ZOOMacro W97M.Class VID_CLASSA 63a6
//
//    Rev 1.482   05 Aug 1998 20:27:52   jwilber
// Added VID_CARTMANC for VID 6198, W97M.Cartman.C, for CI 2551.
//
//    Rev 1.481   05 Aug 1998 14:33:56   jwilber
// Added VID_LAROUXDT, VID 6197, for CI 2545.
//
//    Rev 1.480   04 Aug 1998 13:54:12   DCHI
// Changes for Excel heuristics.
//
//    Rev 1.479   03 Aug 1998 15:39:12   jwilber
// Added VID_JERKA for W97M.Jerk.A, VID 6196, for CI 2529.
//
//    Rev 1.478   30 Jul 1998 09:49:34   RELNITI
// ADD 6195 VID_VAMPIREO
//
//    Rev 1.477   28 Jul 1998 19:40:02   RELNITI
// add 0x6193 for W97M.Jedi.C
//     0x6194 for WM.DMV.L
//
//    Rev 1.476   28 Jul 1998 12:55:04   RELNITI
// ADD 6192 VID_TABEJC for XM.Tabej.C #2494
//
//    Rev 1.475   27 Jul 1998 21:07:42   RELNITI
// ADD 0x6191
//
//    Rev 1.474   27 Jul 1998 17:19:06   RELNITI
// ADD VID_DMVH 6190
//
//    Rev 1.473   24 Jul 1998 14:13:44   jwilber
// Added define for WM.Nuclear.AC, VID 618f, for CI 2406.
//
//    Rev 1.472   23 Jul 1998 16:43:04   jwilber
// Added #define for XM.Extras.F, VID 618e, for CI 2453.
//
//    Rev 1.471   23 Jul 1998 16:12:02   AOONWAL
// Added #define VID_COMPATA 0x618d
//
//    Rev 1.470   23 Jul 1998 13:18:58   jwilber
// Fixed typo.
//
//    Rev 1.469   23 Jul 1998 13:15:34   jwilber
// Added #defines for:
//   WM.Panjang.B  618b   VID_PANJANGB
//   XM.Laroux.DS  618c   VID_LAROUXDS
//
//    Rev 1.468   16 Jul 1998 19:18:44   jwilber
// Added VID_GROOVB for W97M.Groov.B, CI 2363.
//
//    Rev 1.467   16 Jul 1998 13:19:38   jwilber
// Added VID_LAROUXDR for CI 2392, VID 6186.
//
//    Rev 1.466   14 Jul 1998 18:17:04   jwilber
// Added VID_LAROUXDQ for VID 6184, CI 2367.
//
//    Rev 1.465   14 Jul 1998 14:52:24   jwilber
// Added VID_MDMABG for VID 6183, CI 2365.
//
//    Rev 1.464   13 Jul 1998 11:59:14   jwilber
// Changed VID_KILLERB to VID_MENTESB for VID 6182.
//
//    Rev 1.463   13 Jul 1998 11:35:36   jwilber
// Added VID_KILLERB for WM.Killer.B, VID 6182, for CI 2349.
//
//    Rev 1.462   10 Jul 1998 14:26:12   jwilber
// Added VID_EXTRASE for VID 6181, CI 2322.
//
//    Rev 1.461   08 Jul 1998 17:28:56   RELNITI
// ADD 6180 VID_TABEJB XM.Tabej.B for #2297
//
//    Rev 1.460   07 Jul 1998 12:30:24   RELNITI
// ADD VID_ZMKJ 639C for W97M.ZMK.J
//
//    Rev 1.459   01 Jul 1998 16:35:44   RELNITI
// ADD VID_APPDER_FAMILY2 6078
//
//    Rev 1.458   30 Jun 1998 11:49:42   RELNITI
// ADD 62FA VID_DIVINAR
//
//    Rev 1.457   29 Jun 1998 13:37:50   RELNITI
// add 617f
//
//    Rev 1.456   26 Jun 1998 18:56:48   RELNITI
// ADD 639B
//
//    Rev 1.455   22 Jun 1998 18:43:40   AOONWAL
// Added #define VID_BLASHYRKHA 0x617d
//
//    Rev 1.454   19 Jun 1998 16:37:52   JWILBER
// Added VID_TEMPLES for WM.Temple.S, VID 617c, for CI 2178.
//
//    Rev 1.453   18 Jun 1998 15:34:38   JWILBER
// Added VID_TABEJA for XM.Tabej.A, VID 617b, for CI 2167.
//
//    Rev 1.452   17 Jun 1998 21:27:04   JWILBER
// Added VID_DZTK, VID 617a, for WM.DZT.K, for CI 2125.
//
//    Rev 1.451   17 Jun 1998 13:26:50   RELNITI
// add 6399-9a
//
//    Rev 1.450   17 Jun 1998 13:12:44   JWILBER
// Added VID_DMVK for VID 6179, WM.DMV.K for CI 2124.
//
//    Rev 1.449   17 Jun 1998 12:28:10   RELNITI
// ERROR FIX
//
//    Rev 1.448   17 Jun 1998 02:11:00   RELNITI
// ZOO Work: 6379-6398 Vesselin's W97M Collection
//
//    Rev 1.447   16 Jun 1998 19:07:46   JWILBER
// Added VID_LAROUXDL, VID 6175, for CI 2147.
//
//    Rev 1.446   16 Jun 1998 17:29:20   JWILBER
// Added VID_LAROUXDK for VID 6174.
//
//    Rev 1.445   10 Jun 1998 23:47:30   JWILBER
// Added VID_ANTICONM for CI 2103, VID 0x6173.
//
//    Rev 1.444   10 Jun 1998 17:47:28   DKESSNE
// added WM.Czech.B 0x6172 for CI 2101
//
//    Rev 1.443   10 Jun 1998 13:02:40   DCHI
// Added VID_W7H2
//
//    Rev 1.442   09 Jun 1998 21:53:38   JWILBER
// Added #define for VID_RAZERC, 6071 for CI 2088.
//
//    Rev 1.441   09 Jun 1998 11:57:58   RELNITI
// VID_DUMB_LAROUX 1fa9 (notice, it refers to XM.Laroux.A)
//  for DUMB XM.Laroux.A samples. These are marked "NoRepair"
//
//    Rev 1.440   05 Jun 1998 15:00:26   AOONWAL
// Added #define VID_GROOVA  0x6170
//
//    Rev 1.439   05 Jun 1998 13:57:04   AOONWAL
// Added #define VID_CARTMANB 0x616f
//
//    Rev 1.438   05 Jun 1998 12:08:38   AOONWAL
// renamed VID_LAROUXEDAM to VID_LAROUXDAM
//
//    Rev 1.437   05 Jun 1998 11:10:44   RELNITI
// ZOO Macro Work: Vesselin's Excel collection 6370-6377
//
//    Rev 1.436   05 Jun 1998 09:26:30   RELNITI
// ZOO: Vesselin's Excel Collection 6345, 6338, 6333, 6331, 6356-636f
//
//    Rev 1.435   03 Jun 1998 19:25:02   RELNITI
// #2053: 0x616E WM.MDMA.BF
//
//    Rev 1.434   03 Jun 1998 16:08:24   DKESSNE
// changed VID_DMVB to VID_XLDMVB  0x6327
//
//    Rev 1.433   03 Jun 1998 11:27:00   RELNITI
// MAY98 Zoo Macro Work: 6350-6355:
//  X97M.Import.A, Neg.A, Neg.B, Teocatl, Trash, Yohimbe.B
//
//    Rev 1.432   02 Jun 1998 23:41:00   RELNITI
// ADD 6327-634F from Vesselin's XM collections
//
//    Rev 1.431   02 Jun 1998 11:46:06   AOONWAL
// Added #define VID_LAROUXS 0x616d
//
//    Rev 1.430   01 Jun 1998 14:19:58   AOONWAL
// Added #define VID_MDMAAU  0x616c
//
//    Rev 1.429   31 May 1998 03:14:30   RELNITI
// ZOO MACRO May98c: 6309-6326:
//  W97M.AOS.A, Crazy.A, B, Elbag.A, Gambler.B, Jack.A, B, NightShade.C,
//       Box.G, H, Cipher.A, LameJoke.A, Multi.A, B, C, D, Steroid.D, E,
//       Acid.A, ZMK.D, Multi.E, DWMVCK1.B, A, Eraser.R, U, Talon.I, J,
//       Vampire.J, N, VMPCK1.A
//
//    Rev 1.428   29 May 1998 15:28:08   RELNITI
// ZOO MACRO MAY98b: 6300-6308:
//  W97M.AntiSR1.B, Bismark.F, Box.F, Hate.A, Steph.A, Steroid.A, Killer.A,
//       Edds.A, Pomp.A
//
//    Rev 1.427   29 May 1998 08:50:36   relniti
// REMOVE 6165 as it is duplicate of 60D7
// ADD 6164 VID_SCHOOB for WM.Schoo.B #2030/2031
//
//    Rev 1.426   26 May 1998 11:42:34   RELNITI
// ReDEF 6165 to VID_NO_VAB
//
//    Rev 1.425   22 May 1998 16:26:52   JWILBER
// Added VID_VOTIMA for CI 2001, VID 6165.
//
//    Rev 1.424   21 May 1998 22:29:52   JWILBER
// Added a #define for XM.Extras Variant, VID 62fe, for CI 1985.
//
//    Rev 1.423   21 May 1998 20:19:10   JWILBER
// Added VID_EXTRASC for CI 1985, VID 6163.
//
//    Rev 1.422   20 May 1998 20:28:56   JWILBER
// Added VID_VENENOC for 0x6162, CI 1990.
//
//    Rev 1.421   20 May 1998 10:57:52   relniti
// VMacro Processing 052098: 62FB-62FD:
//  Alliance.H, Surabaya.B, Wazzu.ER
//
//    Rev 1.420   15 May 1998 17:55:44   DKESSNE
// added VID_WARNINGD  0x615f
//
//    Rev 1.419   14 May 1998 15:03:28   DKESSNE
// added VID_LAROUXDE  0x615e
//
//    Rev 1.418   13 May 1998 20:00:54   relniti
// RENAME 615B: XM.Laroux.CX => XM.Laroux.DB
//        615C: XM.Laroux.CY => XM.Laroux.DC
// ADD 615D: XM.Laroux.CX from what John added into Laroux.CF
//
//    Rev 1.417   12 May 1998 21:06:56   JWILBER
// Added #define for XM.Laroux.CY, VID 615c, for CI 1915.
//
//    Rev 1.416   12 May 1998 14:26:32   JWILBER
// Added #define for VID_LAROUXCX, VID 615b, for CI 1910.
//
//    Rev 1.415   12 May 1998 12:56:28   relniti
// VMacro Processing 051298: 62EE-62F9
//  Appder.V, W, Boom.C:De, Fehler.A:De, Kann.A, Mensagem.A, NJ-WMDLK1.O,
//  Padania.A, B:It, Pelo.A, Stall.A, Swlabs.P
//
//    Rev 1.414   11 May 1998 19:18:06   relniti
// ADD 62ed WM.Wazzu.EQ
//
//    Rev 1.413   11 May 1998 18:21:56   relniti
// VMacro Processing: 62EC WM.MDMA.BE
//
//    Rev 1.412   11 May 1998 17:58:46   DCHI
// Added VID_A2_ACCESSIVA 0x62E4, VID_TOXA 0x62E5, and VID_TOXB 0x62E6
//
//    Rev 1.411   07 May 1998 19:55:38   JWILBER
// Added VID_NAPROXEN for VID 615a, CI 1903.
//
//    Rev 1.410   06 May 1998 23:25:26   JWILBER
// Changed VID_APPDERR to VID_APPDERS for VID 6158.
//
//    Rev 1.409   06 May 1998 23:06:56   JWILBER
// Added define for WM.Appder.R, VID 6158, for CI 1876.
//
//    Rev 1.408   06 May 1998 21:03:44   JWILBER
// Added define for VID_WALLPAPERB, VID 6157, for CI 1888.
//
//    Rev 1.407   06 May 1998 19:17:20   JWILBER
// Added define for XM.Laroux.CW, VID 6156, for CI 1900.
//
//    Rev 1.406   06 May 1998 16:32:34   relniti
// RENAMING Blee.D => Disco.D
//
//    Rev 1.405   06 May 1998 11:46:52   JWILBER
// Added define for WM.Lamah.B:Br, VID 6155, for CI 1891.
//
//    Rev 1.404   05 May 1998 21:11:50   relniti
// ADD 6154 W97M.Chack.A for #1887
//
//    Rev 1.403   05 May 1998 20:25:52   JWILBER
// Added define for WM.Wazzu.EP, VID 6153, for CI 1881.
//
//    Rev 1.402   05 May 1998 11:35:54   relniti
// ADD 62E8-62EB for W97M.AntiSR1.A, W97M.Blee.A, W97M.Disco.B,C
//
//    Rev 1.401   04 May 1998 08:41:28   relniti
// ZOO Macro: 62E7 WM.Angus.A
//
//    Rev 1.400   01 May 1998 16:09:58   AOONWAL
// Added #define VID_BLEED 0x6151
//
//    Rev 1.399   30 Apr 1998 18:41:44   AOONWAL
// Added VID_LAROUXCK 0x6150
//
//    Rev 1.398   29 Apr 1998 21:54:18   relniti
// VMacro Batch Processing 043098: 62DE-62E3:
//  Clock.M:De, Kompu.K, MDMA.BD, Mentes.A, Padania.C, Swlabs.O
//
//    Rev 1.397   29 Apr 1998 20:47:26   relniti
// MODIFY 6102: VID_SCHUMANNFDE => VID_SCHUMANNGDE
//        611D: VID_SCHUMANNMDE => VID_SCHUMANNNDE
//        613D: VID_SCHUMANNNDE => VID_SCHUMANNPDE
// VMacro Batch Processing 042998: add 62C9-62DD
//
//    Rev 1.396   24 Apr 1998 12:35:44   relniti
// ADD 614E VID_MDMAAT for #1818
//
//    Rev 1.395   22 Apr 1998 15:12:32   DCHI
// Added comment requiring mod of virscan6.mak when adding #includes.
//
//    Rev 1.394   20 Apr 1998 22:01:24   RELNITI
// ADD 614D VID_SWLABSE
// change 6108 VID_SWLABSF95
//
//    Rev 1.393   20 Apr 1998 21:27:00   RELNITI
// ADD 614C
//
//    Rev 1.392   20 Apr 1998 18:26:04   RELNITI
// ADD 614B XM.Extras.B
//
//    Rev 1.391   20 Apr 1998 16:23:52   JWILBER
// Uncommented #includes for foreign office *.h files.
//
//    Rev 1.390   20 Apr 1998 11:24:36   RELNITI
// VMacro Batch Processing 042098: 62BD - 62C8 for:
// Box.I:Tw, Buero.B:De, Buero.C:De, Geni.A, GoldFish.F, Meldung.A,
// Niknat.I, NJ-WMDLK1.N, Nuclear.AA, Swlabs.M, Taguchi.B, Wazzu.EO
//
//    Rev 1.389   20 Apr 1998 09:52:38   RELNITI
// ADD VID_SWLABSM 614A for #1778
//
//    Rev 1.388   17 Apr 1998 22:47:06   JWILBER
// Added VID_SCHUMANNODE for CI 1773, VID 6148.
//
//    Rev 1.387   17 Apr 1998 15:32:34   RELNITI
// Ooops.. VID_LAROUXEDAM should've been 6147
//
//    Rev 1.386   17 Apr 1998 15:12:12   RELNITI
// ADD VID_LAROUXEDAM 6247 for #1775
//
//    Rev 1.385   17 Apr 1998 11:14:42   DCHI
// Added VID_ACCESSIVA and VID_ACCESSIVB.
//
//    Rev 1.384   16 Apr 1998 13:16:30   RELNITI
// ADD 62B3-62BA
//
//    Rev 1.383   15 Apr 1998 18:23:56   RELNITI
// Converting XL95 detection for XL95/XL5 new engine: changes of #define
// on some VID_LAROUX?? for modify or new detection
//
//    Rev 1.382   14 Apr 1998 12:02:32   RELNITI
// ADD VID_SWITCHERK 6146 for #1751
//
//    Rev 1.381   14 Apr 1998 08:20:34   RELNITI
// ADD VID_LAROUXCV 0x6145 for #1750
//
//    Rev 1.380   10 Apr 1998 14:51:00   RELNITI
// VMacro Batch Processing Apr98b: VID 6299-62B1 for
// Twno.AC:Tw, Twno.AH:Tw, Twno.AI:Tw, Twno.AK:Tw, Uka.C,
// Ultra.A, Ultra.B, Unhas.B, Wazzu.BL, Wazzu.DT, Wazzu.DV, Wazzu.DW,
// Wazzu.DY, Wazzu.DZ, Wazzu.EA, Wazzu.EB, Wazzu.EE, Wazzu.EF, Wazzu.EG,
// Wazzu.EH, Wazzu.EI, Wazzu.EJ, Wazzu.EK, Wazzu.EL, Wazzu.EM
//
//    Rev 1.379   09 Apr 1998 15:27:00   DKESSNE
// added WM.Switcher.J 0x6144 for CI 1710
//
//    Rev 1.378   09 Apr 1998 13:17:22   DKESSNE
// renamed WazzuEL -> WazzuEC
//
//    Rev 1.377   09 Apr 1998 11:44:06   RELNITI
// VMacro Batch Processing Apr98
// ADD 0x6287-6298
// Split VID_NICEDAY_FAMILY => VID_NICEDAY_FAMILY1, 2.
//
//    Rev 1.376   08 Apr 1998 16:56:52   DKESSNE
// renamed VID_STEROIDF -> VID_STEROIDB
//
//    Rev 1.375   08 Apr 1998 15:31:58   DKESSNE
// added VID_STEROIDF  0x6143
//
//    Rev 1.374   08 Apr 1998 11:18:30   RELNITI
// VMacro Batch Processing Mar98: Appder.R, Attach.A, Colors.BY,
//  Concept.CG, CopyCap.A, Decaf.A, Divina.P, Doggie.F, Dude.A, Dust.A,
//  DZT.I, Easy.C, Elohim.A
//
//    Rev 1.373   07 Apr 1998 12:38:26   DKESSNE
// added VID_TWNOAJ  0x6142
//
//    Rev 1.372   06 Apr 1998 17:57:00   DKESSNE
// added VID_SWA  0x6141
//
//    Rev 1.371   06 Apr 1998 16:23:30   DKESSNE
// added VID_WAZZUEL  0x6140
//
//    Rev 1.370   03 Apr 1998 15:44:04   AOONWAL
// Added #define VID_MUNCHB 0x613e
//
//    Rev 1.369   02 Apr 1998 17:29:42   DKESSNE
// added VID_SCHUMANNNDE  0x613d
//
//    Rev 1.368   02 Apr 1998 10:52:28   AOONWAL
// Added define VID_MDMAAZ 0x613c
//
//    Rev 1.367   02 Apr 1998 07:47:44   AOONWAL
// CHANGED
// 0x6279 -> 0x6138
// 0x627a -> 0x6139
// 0x627b -> 0x613a
// 0x627c -> 0x613b
//
//    Rev 1.366   01 Apr 1998 18:05:44   DKESSNE
// added VID_LAROUXCQ  0x6137
//
//    Rev 1.365   01 Apr 1998 16:38:40   AOONWAL
// Added define VID_CECILIACO 0x627c
//
//    Rev 1.364   01 Apr 1998 14:12:16   DKESSNE
// added VID_LAROUXCP  0x6136  (CI 1658)
//
//    Rev 1.363   31 Mar 1998 12:30:28   DKESSNE
// added VID_EXTRASA  0x6135  for CI 1638
//
//    Rev 1.362   30 Mar 1998 17:53:44   AOONWAL
// Added #define VID_STEROIDC 0x627b
//
//    Rev 1.361   30 Mar 1998 14:52:42   AOONWAL
// added VID_CONCEPTCJ 627a
//
//    Rev 1.360   26 Mar 1998 18:47:26   AOONWAL
// Added VID_CAPEP 0x6279
//
//    Rev 1.359   25 Mar 1998 18:47:16   JWILBER
// Added VID_LAROUXAN for CI 1608, VID 6132.
//
//    Rev 1.358   25 Mar 1998 18:09:14   DKESSNE
// added VID_SHOWOFFDB  0x6131
//
//    Rev 1.357   20 Mar 1998 15:44:32   DKESSNE
// added VID_LAROUXBO  0x612f
//
//    Rev 1.356   19 Mar 1998 11:37:20   DKESSNE
// added VID_LAROUXCK  0x612e,
// changed VID_LAROUXEMAC to VID_LAROUXAP
//
//    Rev 1.355   18 Mar 1998 17:33:30   DKESSNE
// added VID_MDMAAY 0x612d for CI 1548
//
//    Rev 1.354   17 Mar 1998 18:16:52   DKESSNE
// added VID_LAROUXAZ  0x612c
//
//    Rev 1.353   11 Mar 1998 18:56:10   RELNITI
// ADD VID_MDMAAX 6124 for #1493
//
//    Rev 1.352   09 Mar 1998 12:30:26   RELNITI
// ADD VID_LAROUXCE 6120 for #1476
//
//    Rev 1.351   09 Mar 1998 10:03:02   RELNITI
// ADD VID_LAROUXG97 611F for #1471
//
//    Rev 1.350   06 Mar 1998 18:24:44   DKESSNE
// added VID_LAROUXCJ  0x611e
//
//    Rev 1.349   05 Mar 1998 17:23:56   DKESSNE
// added WM.Schumann.M 0x611d  CI 1452
//
//    Rev 1.348   05 Mar 1998 12:28:22   DKESSNE
// added VID_LAROUXBY  0x611c (CI 1433)
//
//    Rev 1.347   05 Mar 1998 11:18:32   DKESSNE
// added VID_LAROUXBX 0x611b (CI 1449)
//
//    Rev 1.346   04 Mar 1998 17:48:14   DKESSNE
// added VID_WAZZUDX 0x611a (CI 1454)
//
//    Rev 1.345   04 Mar 1998 15:43:12   JWILBER
// Added #define for Laroux.BW, VID 0x6119, CI tbd.
//
//    Rev 1.344   04 Mar 1998 15:07:22   DKESSNE
// added VID_SWLABSL  0x6118 (CI 1451)
//
//    Rev 1.343   03 Mar 1998 18:04:32   DKESSNE
// added VID_WARNINGC  0x6117
//
//    Rev 1.342   03 Mar 1998 13:46:22   DKESSNE
// Wazzu.DK -> Wazzu.DW
// Wazzu.DS -> Wazzu.DK
//
//    Rev 1.341   03 Mar 1998 11:28:22   DKESSNE
// added VID_LAROUXBV 0x6116 (CI 1426)
//
//    Rev 1.340   03 Mar 1998 09:48:54   DKESSNE
// added VID_WAZZUDS  0x6115   (CI 1427)
//
//    Rev 1.339   02 Mar 1998 17:36:12   AOONWAL
// Added 0x6114
//
//    Rev 1.338   02 Mar 1998 12:37:38   DKESSNE
// added VID_LAROUXBU  0x6113 (CI 1413/1416)
//
//    Rev 1.337   02 Mar 1998 11:36:34   AOONWAL
// Updated from Defwork
//
//    Rev MAR98   27 Feb 1998 13:00:12   ECHIEN
// Added VID_DIVINAQ
//
//    Rev MAR98   26 Feb 1998 15:33:04   CFORMUL
// Added VID_WAZZUDR
//
//    Rev MAR98   24 Feb 1998 17:29:28   ECHIEN
// Added VID_CONCEPTI_FAMILY
//
//    Rev MAR98   24 Feb 1998 13:03:56   ECHIEN
// Added VID_WARNINGB
//
//    Rev MAR98   18 Feb 1998 18:07:00   DKESSNE
// added VID_LAROUXEMAC 0x610e for CI 1288/1290
//
//    Rev MAR98   17 Feb 1998 12:18:52   RELNITI
// RENAME 6101 VID_MDMAAM => VID_MDMAAH
//        6089 VID_MDMAAH => VID_MDMAAB
//        6016 VID_MDMAAB => VID_MDMAAN
//        606D VID_MDMAAC => VID_MDMAAM
// ADD 610C VID_MDMAAV for #1282
//
//    Rev 1.336   26 Feb 1998 18:00:22   JWILBER
// Commented out #includes for foreign office *.h files.
//
//    Rev 1.335   12 Feb 1998 20:10:24   RELNITI
// VMACRO Batch Processing: ADD #define for:
// Appder.Q, Attention.B, Breeder.A, Chaos.C, CleanUp.A, Concept.BY,
// Concept.BZ, Concept.CA, Concept.CB, Concept.CE, Divina.O, Dracula.B,
// Wazzu.DG, Fake.A, Grass.A:De, Gurre.A, Hassle.B, Innuendo.A, Kompu.J,
// HaBir.A, Lorax.A, Minimal.AA, NewYear.A:Fr, NOP.R:Br, NOP.S:Es, NOP.T:It,
// NoPrint.B, Nottice.C, Peaceful.A, Alliance.G, SC.A, Sidor.A, Spell.A,
// Swlabs.K, Tabula.A:De, Taguchi.A, Twno.AG, Uka.B, Unhas.A, VHDL.C:Tw,
// Wazzu.DQ
//
//    Rev 1.334   12 Feb 1998 12:43:28   RELNITI
// 6104: VID_LAROUXBK => VID_LAROUXBQ
// 6106: VID_LAROUXBL => VID_LAROUXAM
//
//    Rev 1.333   11 Feb 1998 21:11:10   RELNITI
// CHANGE 610A to VID_LAROUXBP
//
//    Rev 1.332   11 Feb 1998 20:53:08   RELNITI
// ADD VID_LAROUXBM 610A for #1268
//
//    Rev 1.331   11 Feb 1998 13:51:46   RELNITI
// add vid_swlabsf 6108
//
//    Rev 1.330   10 Feb 1998 11:58:42   DKESSNE
// added VID_KOMCONA  0x6107 for CI 1248
//
//    Rev 1.329   09 Feb 1998 19:49:16   DKESSNE
// added VID_LAROUXBL  0x6106  for CI 1211
//
//    Rev 1.328   09 Feb 1998 15:55:44   JWILBER
// Added #includes for remote office include files.
//
//    Rev 1.327   05 Feb 1998 11:59:38   DKESSNE
// added VID_LAROUXBK 0x6104 for CI 1210
//
//    Rev 1.326   03 Feb 1998 18:50:08   DKESSNE
// added VID_NICEDAYP  0x6103
//
//    Rev 1.325   03 Feb 1998 14:18:18   RELNITI
// COMMENT OUT 60F4 VID_DMVK
//
//    Rev 1.324   03 Feb 1998 13:49:48   DKESSNE
// added VID_SCHUMANNFDE 0x6102 for CI 1194
//
//    Rev 1.323   03 Feb 1998 11:48:48   DKESSNE
// added VID_MDMAAM  0x6101
//
//    Rev 1.322   29 Jan 1998 18:54:50   DCHI
// Added VID_PAIX for XF.Paix.A
//
//    Rev 1.321   28 Jan 1998 18:12:42   JWILBER
// Removed #define for VID_DON as 624f.  Left in the one defining
// it as 60fc.
//
//    Rev 1.320   28 Jan 1998 18:02:42   JWILBER
// Pulled #define for VID_CMDA as 6244.  Left in the one for
// 60fd.
//
//    Rev 1.319   28 Jan 1998 17:37:24   JWILBER
// Reconciled w/ DEFWORK by adding #defines:
// VID_RAZERB, VID_NUMBER1A, VID_LAROUXAG, VID_DMVK, VID_ANTINS,
// VID_NICEDAYO, VID_LAROUXBJ, VID_GAMBLEA, VID_DON, VID_LMVD,
// VID_VICTORA, VID_NOTTICEB.
//
//    Rev 1.318   19 Jan 1998 18:20:26   RELNITI
// Jan98 Blitz: Joe Wells' W97M, XM, X97M collection
//
//    Rev 1.317   15 Jan 1998 14:51:02   JWILBER
// Removed #defines for VID 60ec, WM.Number1.A, and 60ed, XM.Laroux.AG,
// and moved to DEFWORK.
//
//    Rev 1.316   15 Jan 1998 01:47:36   JWILBER
// Added #defines for:
// WM.Number1.A VID 60ec CI 1015
// XM.Laroux.AG VID 60ed CI 1016
//
//    Rev 1.315   13 Jan 1998 16:00:30   RELNITI
// Jan98 blitz addition: 6200-621D
//
//    Rev 1.314   10 Jan 1998 07:27:22   JWILBER
// Added define for XM.Laroux.AF, VID 60e7, for CI 970.
//
//    Rev 1.313   09 Jan 1998 05:35:02   JWILBER
// Added define for WM.Cartman, VID 60e6, for CI 964.
//
//    Rev 1.312   09 Jan 1998 04:19:46   JWILBER
// Added define for WM.Divina.N, VID 60e5, for CI 967.
//
//    Rev 1.311   06 Jan 1998 13:47:52   JWILBER
// Added #define for WM.Cap.BE, 60e4, for CI 948.
//
//    Rev 1.310   05 Jan 1998 19:58:14   JWILBER
// Added VID_CONCEPTBZ, 60e3, for CI 943.
//
//    Rev 1.309   29 Dec 1997 17:58:56   JWILBER
// Modified during Jan98 build.
//
//    Rev JAN98   29 Dec 1997 14:43:22   RELNITI
// ADD VID_BLEEB 60E2 FOR #903
//
//    Rev JAN98   19 Dec 1997 17:23:04   CFORMUL
// Added VID_LAROUXBB
//
//    Rev JAN98   19 Dec 1997 13:00:40   DKESSNE
// Added LarouxAE 60e0
//
//    Rev JAN98   18 Dec 1997 20:02:02   RELNITI
// VMacro Renaming:
//  4fBE: VID_DMVD => VID_HELPERI
//  4cAE: VID_DMVH => VID_DMVD
//  6013: VID_TEMPLEG => VID_TEMPLEK
//
//    Rev JAN98   18 Dec 1997 18:53:58   RELNITI
// MODIFY 6074: VID_DIVINAK => VID_DIVINAL
//        6096: VID_MDMAAI  => VID_MDMAAK
// ADD 60B0: VID_DIVINAK
//
//    Rev JAN98   18 Dec 1997 17:03:38   RELNITI
// ADD vid 60cc- 60df for WM.Appder.P, Counter.B, Divina.M, DMV.J, Hot.B,
//  Jerm.B, Louvado.A, MDMA.AL, Minimal.Y, Z, MVDK1.A, No_Va.B, Talon.L,
//  Trash.B, Tunguska.A, Vampire.K:Tw, Vampire.L:Tw, Vampire.M:Tw, Wazzu.DO,
//  Wazzu.DP
//
//    Rev JAN98   18 Dec 1997 16:05:00   DKESSNE
// Added Yaka.A
//
//    Rev JAN98   17 Dec 1997 16:59:24   CFORMUL
// Added VID_LAROUXBA
//
//    Rev JAN98   16 Dec 1997 16:08:36   DKESSNE
// added VID_LAROUX_AV, vid 60c9, cust 792
//
//    Rev JAN98   12 Dec 1997 17:59:50   RELNITI
// ADD VID_ANARCHY6093A 0x60c7 for #582
//
//    Rev 1.308   11 Dec 1997 17:41:32   AOONWAL
// Added 0x60c6 "XM.Laroux.J"
//
//    Rev 1.307   11 Dec 1997 16:40:22   AOONWAL
// Added 0x60c5 "XM.Laroux.AI"
//
//    Rev 1.306   10 Dec 1997 18:07:26   AOONWAL
// Added define VID_LAROUXAU 0x60c4
//
//    Rev 1.305   09 Dec 1997 20:04:30   AOONWAL
// Added define VID_CONCEPTBY 0x60c0
//
//    Rev 1.304   08 Dec 1997 16:12:56   AOONWAL
// Modified define VID_PESANC 60bf
//
//    Rev 1.303   08 Dec 1997 16:11:32   AOONWAL
// Added define VID_PESANC 60bf
//
//    Rev 1.302   04 Dec 1997 22:33:06   RELNITI
// VMacro Batch Processing 120497:
// Alliance.C, D, E, F, Bishkek.A, Clock.K, Concept.BT, BU, BV, BX,
// Counter.A, DMV.I, Eraser.S:Tw, FiveA.A, Germ.A, Gnomo.A, Groben.A,
// Gsis.A, Kompu.I, MDMA.AJ, Mercado.A:Br 609B-60AF
// Minimal.V, W, Nop.P:De, Q:Es,
// Ochoy.A, Schoo.A, Tamago.B:Br, Trash.A, Twno.AE:Tw, Wazzu.DH, Wazzu.DM,
// Wompat.A, Wompie.A, Yaka.B   60B1-60BE
//
//    Rev 1.301   04 Dec 1997 15:16:04   RELNITI
// ADD VID_OBLOMH, F, G 6098-9A
//
//    Rev 1.300   03 Dec 1997 13:43:26   JWILBER
// Added VID_WMMDMAAI for VID 6096 - CI 665.
//
//    Rev 1.299   02 Dec 1997 19:43:44   RELNITI
// MODIFY VID_CAP_CORRUPTED to refer to 0x4F21 WM.Cap (Damage) instead
//  of 4E00 Macro Component
//
//    Rev 1.298   02 Dec 1997 15:05:28   RELNITI
// vid_paycheck_family 6093
//
//    Rev 1.297   26 Nov 1997 13:53:14   ECHIEN
// Added APPDER.O 608C
//
//    Rev 1.296   26 Nov 1997 10:13:04   ECHIEN
// Added NuclearV 608B
//
//    Rev 1.295   20 Nov 1997 19:51:04   JWILBER
// Modified during Dec97 build.
//
//    Rev DEC97   19 Nov 1997 15:19:24   RELNITI
// ADD VID_VHDLATW 608A for #591 and also VMacro posting
//
//    Rev DEC97   19 Nov 1997 13:42:36   RELNITI
// ADD VID_MDMAAH 6089 for #588
//
//    Rev DEC97   18 Nov 1997 13:55:20   RELNITI
// ADD 6085-6088 VID_MDMAAD - VID_MDMAAG
//
//    Rev DEC97   14 Nov 1997 19:42:32   RELNITI
// ADD 6084 VID_BOXETW
//
//    Rev DEC97   14 Nov 1997 18:49:50   RELNITI
// ADD 0x6082, 6083
//
//    Rev DEC97   13 Nov 1997 17:06:38   RELNITI
// ADD VID_NIKNAT_FAMILY? 0x6081 for #531
//
//    Rev DEC97   13 Nov 1997 12:53:12   RELNITI
// ADD 0x6080 VID_WAZZUDL95
//
//    Rev DEC97   12 Nov 1997 16:56:54   RELNITI
// ADD 607F VID_NINJAA
//
//    Rev DEC97   12 Nov 1997 14:15:56   RELNITI
// ADD 607D-7E VID_GOLDFISHD,E
//
//    Rev DEC97   12 Nov 1997 13:46:58   RELNITI
// ADD VID_TEMPLE_FAMILY1, 2, 3 607C
//
//    Rev DEC97   12 Nov 1997 13:14:54   RELNITI
// ADD 607C VID_TEMPLE_FAMILY
//
//    Rev DEC97   12 Nov 1997 11:54:28   RELNITI
// CHANGE 0x607A VID_INFLUENZAB
// ADD 0x607B VID_INFLUENZAA
//
//    Rev DEC97   12 Nov 1997 11:37:20   RELNITI
// ADD 0x607A VID_INFLUENZAA for #515
//
//    Rev 1.294   11 Nov 1997 10:19:06   RELNITI
// ADD VID_SWLABSI 0x6079 for #495
//
//    Rev 1.293   11 Nov 1997 08:18:38   RELNITI
// ADD VID_APPDER_FAMILY 6078
//
//    Rev 1.292   10 Nov 1997 15:00:12   RELNITI
// ADD VID_LAROUXL 6077 for #489
//
//    Rev 1.291   10 Nov 1997 13:34:54   ECHIEN
// Added CONCEPTBS 0x6076
//
//    Rev 1.290   07 Nov 1997 17:25:32   ECHIEN
// Added NIKNATB
//
//    Rev 1.289   04 Nov 1997 15:13:28   ECHIEN
// Added vid_divinak 6074
//
//    Rev 1.288   31 Oct 1997 16:40:52   RELNITI
// ADD VID_APPDERN 6073
//
//    Rev 1.287   31 Oct 1997 12:48:28   RELNITI
// ADD VID_SHOWOFFCK 6072
//
//    Rev 1.286   29 Oct 1997 19:26:34   RELNITI
// ADD 6070 for VID_LAROUXAC
//
//    Rev 1.285   29 Oct 1997 15:54:04   RELNITI
// ADD 606F VID_WAZZUDL for #418
//
//    Rev 1.284   28 Oct 1997 18:41:06   RELNITI
// ADD 0x606E VID_LAROUXAA for #414
// MOVE 0x601D VID_GOODNIGHTFAM into Macro Virus Family block.
//
//    Rev 1.283   28 Oct 1997 14:05:12   RELNITI
// ADD VID_MDMAAC 606D
//
//    Rev 1.282   27 Oct 1997 12:38:00   JWILBER
// Added modification made during NOV97 build.
//
//    Rev NOV97   24 Oct 1997 14:35:08   DANG
// VID 0x606C changed from TEMPLEI to TEMPLEJ
//
//    Rev NOV97   24 Oct 1997 14:09:08   DANG
// Added Temple.I for CI#385
//
//    Rev NOV97   24 Oct 1997 13:03:16   DANG
// Added Wazzu.DK for CI#392.
//
//    Rev NOV97   24 Oct 1997 11:59:02   AOONWAL
// Added VID_APPDERL 0x606a
//
//    Rev NOV97   24 Oct 1997 10:49:38   RELNITI
// VMacro Batch Processing 102497: ADD VID 0x601E-0x6069 for:
// Cap.BA,BB,BC,BD, Colors.BT, Concept.BT, Crema.A, Date.C,D, Easy.B,
// Enfk.A.Kit, Friendly.B:De, Gas.B, Horn.A, Johnny.P, KillProt.B, Lord.A,
// Malice.A, MG.A, MG.B, Minimal.U, Muck.Z,AA,AB, NJ-WMVCK2.C, NOP.O:De,
// Npad.DL,DM,DN,DO,DP,DQ,DR,DS,DT,DU,DV,DW,DX,DY,DZ,EA,EB,EC,ED,
// OldPad.A,B, Plushad.A, Pwd.A, Ramses.A:It, Rapi.AM,AM1, Saver.B,
// Schumann.D:De, Shadow.A, ShowOff.CI,CJ, Silly.A, Spooky.E:De,
// Switcher.I, Tamago.A, Temple.I, Twno.AD:Tw, TwoLines.R, Uka.A,
// Wazzu.CY,CZ,DA,DB,DC,DD,DE,DF,DI,DJ, WiederOeffnen.A:De
//
//    Rev NOV97   22 Oct 1997 21:07:38   AOONWAL
// Added VID_GOODNIGHTFAM 0x601d
//
//    Rev NOV97   20 Oct 1997 14:31:46   AOONWAL
// Added define VID_GOODNIGHTM 0x601c
//
//    Rev NOV97   17 Oct 1997 18:35:28   ECHIEN
// Added GoodnightL
//
//    Rev NOV97   17 Oct 1997 17:10:02   ECHIEN
// Added KIllLufB 601b
//
//    Rev NOV97   17 Oct 1997 14:48:46   CFORMUL
// Added LUNCHG
//
//    Rev NOV97   17 Oct 1997 11:01:52   CFORMUL
// Added GoodNight.K
//
//    Rev 1.281   15 Oct 1997 16:54:26   ECHIEN
// Added Goodnight.J 6017
//
//    Rev 1.280   15 Oct 1997 16:33:54   ECHIEN
// added MDMA.AB 6016
//
//    Rev 1.279   14 Oct 1997 11:47:24   CFORMUL
// Added VID_WARNINGA
//
//    Rev 1.278   10 Oct 1997 16:08:44   AOONWAL
// Added define for VID_LAROUXG 0x6014
//
//    Rev 1.277   10 Oct 1997 13:42:14   ECHIEN
// Added VID_TEMPLEG 0x6013
//
//    Rev 1.276   09 Oct 1997 18:13:52   AOONWAL
// Added define VID_GOODNIGHTI 0x6012
//
//    Rev 1.275   08 Oct 1997 00:37:50   RELNITI
// ADD 0x6011 VID_JUNKFACE
//
//    Rev 1.274   07 Oct 1997 23:20:24   RELNITI
// ADD 0X600F 0X6010
//
//    Rev 1.273   07 Oct 1997 21:16:10   RELNITI
// VMacro Batch Processing 100797: 0x4cF7-0x4cFF and 0x6000-0x600E for
//  Alien.H, Ammy.A:Tw, Cap.AV, Cap.AW, Cap.AX, Cap.AY, Cap.AZ,
//  Colors.BS, Concept.BQ,
//  Concept.BR, Muck.Q, Muck.S, Muck.T, Muck.U, Muck.V, Muck.W, Muck.X,
//  Nottice.A, Npad.DK, Nuclear.U, Setmd.B:Tw, Switcher.H, Wazzu.CW, Wazzu.CX
//
//    Rev 1.272   07 Oct 1997 13:24:14   AOONWAL
// Added define for VID_GOODNIGHTH 0x4cf6
//
//    Rev 1.271   05 Oct 1997 20:12:48   JWILBER
// Added VID for WM.Muck.R, 4cf5.
//
//    Rev 1.270   02 Oct 1997 16:24:04   AOONWAL
// Added define VID_COUNTTENC 0x4cf4
//
//    Rev 1.269   01 Oct 1997 19:13:28   AOONWAL
// Added VID_SMALLA 0x4cf2
//
//    Rev 1.268   01 Oct 1997 14:52:22   AOONWAL
// Added VID_LUNCHF 4cf1
//
//    Rev 1.267   30 Sep 1997 14:33:46   JWILBER
// Added VID 4cf0 for WM.Temple.F
//
//    Rev 1.266   25 Sep 1997 13:30:20   ECHIEN
// Added VID_HYBRID_FAMILY 0x4cEF
//
//    Rev 1.265   23 Sep 1997 23:40:46   JWILBER
// Modified during Oct97 def build.
//
//    Rev Sep97   23 Sep 1997 17:44:58   ECHIEN
// Added TWNOAB 0x4CEE
//
//    Rev Sep97   23 Sep 1997 16:19:28   RELNITI
// VMacro Batch Processing 092397 0x4cDC - 0x4cED for:
// Cap.AT, Cap.AU, Colors.BR, Concept.BP, DZT.H, India.A, Lamah.A:Br,
// NPad.DE, NPad.DF, NPad.DG, NPad.DH, NPad.DI, NPad.DJ, ShowOff.CF
// ShowOff.CG, ShowOff.CH, Switcher.G, Twno.AA:Tw
//
//    Rev Sep97   22 Sep 1997 18:51:08   RELNITI
// ADD VID_ANAK_FAMILY 0x4cDB
//
//    Rev Sep97   22 Sep 1997 15:59:44   ECHIEN
// Added GoodNightG 0x4CDA
//
//    Rev Sep97   22 Sep 1997 13:09:38   ECHIEN
// Added NiknatA 0x4cd9
//
//    Rev Sep97   22 Sep 1997 12:03:14   ECHIEN
// Added GoodnightF 0x4cd8
//
//    Rev Sep97   18 Sep 1997 10:17:32   CFORMUL
// Added VID_HYBRIDK
//
//    Rev 1.264   16 Sep 1997 15:44:48   DANG
// Added VID for XL.Laroux.G
//
//    Rev 1.263   16 Sep 1997 13:25:22   DANG
// Added VID for Muck.Q for CI#85
//
//    Rev 1.262   15 Sep 1997 14:49:08   CFORMUL
// Added MUCKP
//
//    Rev 1.261   15 Sep 1997 14:04:14   RELNITI
// VMacro Batch Processing 091597: 4cCC-4cD4 for:
// WM.Bandung.BC, Cap.AQ, Cap.AR, Cap.AS, Concept.BO, MDMA.AA, Npad.DC,
// Npad.DD, ShowOff.CE
//
//    Rev 1.260   13 Sep 1997 13:26:58   RELNITI
// VMacro Batch Processing 091397 4cC2-4cCB for:
// WM.Bandung.BB, Cap.AN, Cap.AO, Clock.L:De, Colors.BQ, Divina.J, EMT.A,
// Npad.DB, ShowOff.CD, Wazzu.CV
//
//    Rev 1.259   12 Sep 1997 13:09:10   DANG
// Added VID for Kompu.G
//
//    Rev 1.258   10 Sep 1997 21:06:38   JWILBER
// Added VID_ZMBBDE for CI 60, VID 4cc0, for WM.ZMB.B:De.
//
//    Rev 1.257   10 Sep 1997 15:32:16   DANG
// Added VID entry for Swlabs.H for CI#53
//
//    Rev 1.256   10 Sep 1997 12:17:10   DANG
// Changed VID_GAS to VID_GASA.
//
//    Rev 1.255   10 Sep 1997 12:11:36   DANG
// Added Gas.A for CI#38
//
//    Rev 1.254   09 Sep 1997 15:01:10   RELNITI
// VMacro Batch Processing 090997: VID 4cA7-4cBD for:
//  Cap.AH, .AI, .AJ, .AK, .AL, .AM, Concept.BN, DMV.H, Lox.B, MDMA.Z, Mess.A,
//  Minimal.S, .T, Muck.O, Npad.CZ, Nuclear.T, Oblom.E, ShowOff.CB, .CC,
//  SuperStitious.A, Wazzu.CS, .CT, .CU
//
//    Rev 1.253   09 Sep 1997 14:06:46   JWILBER
// Added VID for WM.Goodnight.E, 4ca6.
//
//    Rev 1.252   05 Sep 1997 16:21:32   AOONWAL
// Removed VID_DAMHYBRIDA 0x4cff. Was added for the local build
//
//    Rev 1.251   05 Sep 1997 11:57:54   AOONWAL
// renamed VID_DZTC -> VID_DZTD
//
//    Rev 1.250   04 Sep 1997 15:18:26   AOONWAL
// Added define VID_GOOGNIGHTD 0x4ca4
//
//    Rev 1.249   29 Aug 1997 08:02:00   RELNITI
// VMacro Batch Processing 082897: 4cA3: Npad.CY
//
//    Rev 1.248   28 Aug 1997 10:09:56   RELNITI
// VMacro Batch Processing 082897: VID 0x4c96-A2 for
//  Cap.AB, AC, AD, AE, AF, Colors.BP, Johnny.O, Npad.CX, ShowOff.BY, BZ, CA,
//  Switcher.E, F
//
//    Rev 1.247   27 Aug 1997 17:01:18   ECHIEN
// Added VID_APPDERK 0x4c95
//
//    Rev 1.246   27 Aug 1997 15:45:10   ECHIEN
// Added VID_HEADA 0x4C94
//
//    Rev 1.245   26 Aug 1997 14:16:06   RELNITI
// VMacro Batch Processing 082697: 4c82-4c93
//
//    Rev 1.244   26 Aug 1997 13:39:54   RELNITI
// REDEFINE 0x4c63: VID_MUCKJ => VID_MUCKL
//            4dAD: VID_CAPZ  => VID_CAPB
//            4dB3: VID_CAPAA => VID_CAPC
//            1eFC: VID_CAPB  => VID_CAPZ
//            4fE7: VID_CAPC  => VID_CAPAA
//
//    Rev 1.243   26 Aug 1997 12:06:44   ECHIEN
// Added VID_LAROUXF 0x4c81
//
//    Rev 1.242   26 Aug 1997 10:36:58   JWILBER
// Modified during Sep 97 build.
//
//    Rev 1.253   26 Aug 1997 10:06:04   RELNITI
// MODIFY 0x4c7C VID_CONCEPT_BI => _BJ
//        0x4c7E VID_CONCEPTBN  => BK
//
//    Rev 1.252   25 Aug 1997 15:44:48   CFORMUL
// Added VID_HYBRIDJ
//
//    Rev 1.251   22 Aug 1997 12:24:00   RELNITI
// ADD VID_DNTC_FAMILY 4c7F
//
//    Rev 1.250   21 Aug 1997 20:24:10   RELNITI
// ADD 0x4c7E VID_CONCEPTBN
//
//    Rev 1.249   21 Aug 1997 15:15:28   RELNITI
// ADD VID_SWITCHER_FAMILY 4c7D
//
//    Rev 1.248   21 Aug 1997 15:11:18   JWILBER
// Fixed type on #define for CONCEPT_BB.
//
//    Rev 1.247   21 Aug 1997 14:46:24   JWILBER
// Added VID_CONCEPTBI, 4c7c, for CI 12955.
//
//    Rev 1.246   20 Aug 1997 17:40:48   JWILBER
// Added VID for WM.Nuclear.S, 4c7b.
//
//    Rev 1.245   20 Aug 1997 15:22:12   RELNITI
// ADD VID_MUCK_FAMILY1, 2, 3 0x4c7A
//
//    Rev 1.244   20 Aug 1997 10:14:36   RELNITI
// ADD VID_TEMPLEE 4c79
//
//    Rev 1.243   18 Aug 1997 18:20:04   RELNITI
// ADD VID_TEMPLED 4c78
//
//    Rev 1.242   18 Aug 1997 17:08:38   RELNITI
// ADD VID_ALIENG 4c77
//
//    Rev 1.241   15 Aug 1997 18:13:06   RELNITI
// ADD 4c65-4c76 from VMacro Batch Processing 081597
//
//    Rev 1.240   15 Aug 1997 12:55:18   RELNITI
// VID_PLDT => VID_LAROUXE
//
//    Rev 1.239   14 Aug 1997 19:32:42   AOONWAL
// Changed VID_KILLLUF to VID_KILLLUFA
//
//    Rev 1.238   14 Aug 1997 19:13:14   AOONWAL
// Added VID_KILLLUF 0x4c64
//
//    Rev 1.237   14 Aug 1997 16:30:12   AOONWAL
// Added VID_MUCKJ 0x4c63
//
//    Rev 1.236   14 Aug 1997 15:37:34   AOONWAL
// Added VID_DAMHYBRIDA 0x4cff
//
//    Rev 1.235   14 Aug 1997 12:27:02   DANG
// Added Switcher.D VID for CI#12889
//
//    Rev 1.234   14 Aug 1997 11:49:36   AOONWAL
// Added VID_HYBRIDI 0x4c61
//
//    Rev 1.233   11 Aug 1997 15:43:58   AOONWAL
// Added 4c60
//
//    Rev 1.232   11 Aug 1997 10:42:58   RELNITI
// ADD 4c55-5F for WM.Npad.CP, CQ, CR, Oblom.C, Vampire.G:Tw, H:Tw,
//     Schumann.C:De, Spy.A, Bandung.AZ, Cap.X, Vicinity.C:De
//
//    Rev 1.231   08 Aug 1997 14:34:52   ECHIEN
// Added VID_NUCLEARR 0x4c54
//
//    Rev 1.230   08 Aug 1997 12:09:26   ECHIEN
// Added VID_MUCKI 0x4c53
//
//    Rev 1.229   07 Aug 1997 19:19:36   RELNITI
// SAMPLE WORK: ADD 4c51: WM.Chaka.A and 4c52: WM.Dakota.A
//
//    Rev 1.228   05 Aug 1997 19:35:04   ECHIEN
// Added VID_SWITCHERC 0x4c4f
//
//    Rev 1.227   05 Aug 1997 19:03:24   RELNITI
// ADD 4c4A-4E for WM.Oblom.B, Temple.C, Friday.E:De, Obay.A, Wazzu.CL
//     based on VMacro Posting
//
//    Rev 1.226   04 Aug 1997 19:44:46   RELNITI
// ADD 4c48, 4c49
//
//    Rev 1.225   04 Aug 1997 16:26:32   RELNITI
// RENAME: 4dB9: DNTCA => CONCEPT_BB
//         4c02: DNTCB => CONCEPTBE
//         4dD2: DATEC => DIVINAG
//         4dD6: DATED => DIVINAH
//
//    Rev 1.224   04 Aug 1997 13:26:30   ECHIEN
// Added VID_PAYCHECKF 0x4c47
//
//    Rev 1.223   04 Aug 1997 10:40:18   ECHIEN
// Added VID_MDMAY 0x4c46
//
//    Rev 1.222   01 Aug 1997 19:00:02   RELNITI
// ADD 4c40-5
//
//    Rev 1.221   01 Aug 1997 18:31:30   RELNITI
// ADD 4c3B-3F
//
//    Rev 1.220   01 Aug 1997 16:15:44   AOONWAL
// Added VID_ANAKB 4c3a
//
//    Rev 1.219   31 Jul 1997 15:10:50   AOONWAL
// RENAMED VID-GOODNIGHTC1 TO VID-GOODNIGHTC
//
//    Rev 1.218   31 Jul 1997 14:42:26   AOONWAL
// Added VID_GOODNIGHTC1 4c39
//
//    Rev 1.217   31 Jul 1997 14:07:08   AOONWAL
// renamed VID_MWSC to VID_DMVG
//
//    Rev 1.216   31 Jul 1997 12:09:14   AOONWAL
// Added VID_MWSCA 4c38
//
//    Rev 1.215   30 Jul 1997 18:28:08   JWILBER
// Commented out VID_RANDOMA for VID 4e78.  Superceded by VID 4c36,
// w/ the same name.
//
//    Rev 1.214   30 Jul 1997 17:04:54   JWILBER
// Added #define for WM.DZT.G, 4c37.
//
//    Rev 1.213   30 Jul 1997 16:21:26   AOONWAL
// Added VID_RANDOMA 0x4c36
//
//    Rev 1.212   30 Jul 1997 15:12:18   JWILBER
// Changed #define VID_DNTCC to VID_CONCEPTBG.
//
//    Rev 1.211   30 Jul 1997 14:44:06   JWILBER
// Define 4c24 for WM.DNTC.C.
//
//    Rev 1.210   30 Jul 1997 11:28:26   RELNITI
// REMOVE 4c24, 4c36, please re-use
//
//    Rev 1.209   29 Jul 1997 19:53:30   JWILBER
// Added VID 4c0d, for Friday.D, for CI 12725.
//
//    Rev 1.208   29 Jul 1997 19:00:40   RELNITI
// REUSE 4c1C, ADD 4C20-4C36
//
//    Rev 1.207   29 Jul 1997 17:01:34   RELNITI
// Duplicate VID: VID_APPDERI removed
//
//    Rev 1.206   29 Jul 1997 15:52:20   RELNITI
// ADD 4c0E-4c1F
//
//    Rev 1.205   29 Jul 1997 14:48:24   AOONWAL
// Removed VID_CONCEPTBG
//
//    Rev 1.204   29 Jul 1997 14:15:30   AOONWAL
// added VID_CONCEPTBG
//
//    Rev 1.203   28 Jul 1997 13:44:24   AOONWAL
// added VID_APPDERJ
//
//    Rev 1.202   28 Jul 1997 12:59:12   AOONWAL
// Modified during AUG97 update
//
//    Rev AUG97   24 Jul 1997 13:34:06   RELNITI
// ADD 0x4c0B
//
//    Rev AUG97   23 Jul 1997 17:30:02   RELNITI
// add 0x4c0A
//
//    Rev AUG97   23 Jul 1997 16:43:34   RELNITI
// ADD VID_PLDT 0x4c09 for #12683
//
//    Rev AUG97   23 Jul 1997 13:47:04   RELNITI
// ADD VID_PAYCHECKE 0x4c08
//
//    Rev AUG97   22 Jul 1997 16:13:10   RELNITI
// ADD VID_MDMAX 0x4c06
//
//    Rev AUG97   22 Jul 1997 10:02:10   RELNITI
// ADD VID_NPADCM 0x4c05
//
//    Rev AUG97   21 Jul 1997 16:47:40   RELNITI
// ADD 4c04 VID_ZMBADE
//
//    Rev AUG97   21 Jul 1997 13:09:44   JWILBER
// Added VID 4c01 for WM.Nuclear.O.
//
//    Rev AUG97   21 Jul 1997 11:21:54   RELNITI
// ADD VID_NICEDAYN 4c03
//
//    Rev AUG97   21 Jul 1997 11:15:16   JWILBER
// Removed VID for WM.Cap.Z, 4c01 - it's available for re-use.
//
//    Rev AUG97   18 Jul 1997 17:13:50   CFORMUL
// Removed VID_APPDERJ
//
//    Rev AUG97   18 Jul 1997 16:58:14   CFORMUL
// Added VID_APPDERJ
//
//    Rev AUG97   18 Jul 1997 16:35:04   CFORMUL
// Added VID_DNTCB
//
//    Rev AUG97   18 Jul 1997 15:06:00   JWILBER
// Added VID for Cap.Z, 4c01, for CI 12634.
//
//    Rev 1.201   16 Jul 1997 20:38:38   CFORMUL
// Added ConceptBC
//
//    Rev 1.200   16 Jul 1997 19:22:18   RELNITI
// ADD 4dDB-4dFF
// RENAME APPDERG=>APPDERH
//
//    Rev 1.199   11 Jul 1997 15:32:30   AOONWAL
// Added VID_HYBRIDH as 4dda
//
//    Rev 1.198   10 Jul 1997 17:36:14   AOONWAL
// Added VID_SWLABSE as 4dd8
//
//    Rev 1.197   10 Jul 1997 16:44:50   AOONWAL
// Added VID_DATED as 4dd6
//
//    Rev 1.196   10 Jul 1997 15:17:58   ECHIEN
// VID_HYBRIDG 0x4DD5
//
//    Rev 1.195   10 Jul 1997 12:51:40   AOONWAL
// Added VID_CONCEPTBA
//
//    Rev 1.194   08 Jul 1997 11:38:12   AOONWAL
// Added VID_DATEC = 4dd2
//
//    Rev 1.193   07 Jul 1997 18:53:04   RELNITI
// ADD 4dC7-D0
//
//    Rev 1.192   07 Jul 1997 16:49:04   RELNITI
// ADD 4dC0-C6 VID_NPADBX-CD
//
//    Rev 1.191   07 Jul 1997 13:19:22   AOONWAL
// Added VID 4dbf for VID_APPDERG
//
//    Rev 1.190   03 Jul 1997 21:49:54   RELNITI
// ADD VID_RAPI_FAMILY4
//
//    Rev 1.189   02 Jul 1997 16:13:36   ECHIEN
// Added VID_CAP_FAMILY 0x4dBE
//
//    Rev 1.188   01 Jul 1997 16:12:08   ECHIEN
// Added VID_TWOLINES_FAMILY 0x4dbc and VID_NICEDAY_FAMILY 0x4dbd
//
//    Rev 1.187   01 Jul 1997 15:29:20   RELNITI
// ADD 0x4dBA VID_SHOWOFFBS
//     0x4dBB VID_APPDERF
//
//    Rev 1.186   01 Jul 1997 12:05:40   RELNITI
// ADD 4dB9 VID_DNTCA
//
//    Rev 1.185   01 Jul 1997 11:45:58   AOONWAL
// Modified during July-MidMonth
//
//    Rev July-MidMonth  30 Jun 1997 20:05:00   RELNITI
// ADD 4dB7, B8
//
//    Rev July-MidMonth  30 Jun 1997 16:24:08   CFORMUL
// Added VID_MACRO_HEURISTICS
//
//    Rev July-MidMonth  30 Jun 1997 13:51:48   DANG
// Added NiceDay.M for CI#12400
//
//    Rev July-MidMonth  30 Jun 1997 12:36:40   DANG
// Added MDMA.V for CI#12403
//
//    Rev July-MidMonth  30 Jun 1997 11:55:34   DANG
// Added TwoLines.N for CI#12430
//
//    Rev July-MidMonth  27 Jun 1997 17:05:06   AOONWAL
// Removed 4db4 = VID_CAPZ . As it is same as WM.CapY
//
//    Rev July-MidMonth  27 Jun 1997 16:25:38   AOONWAL
// Added 4db4 = VID_CAPZ
//
//    Rev July-MidMonth  27 Jun 1997 14:32:08   RELNITI
// Mirror the additional VID_CAP_CORRUPTED 4e00 in Newvir
//
//    Rev July-MidMonth  26 Jun 1997 21:28:04   JWILBER
// Added #define for VID_CAPY, 4db3 for CI 12388.
//
//    Rev 1.183   25 Jun 1997 14:08:50   AOONWAL
// Added define VID_cebua 4db2
//
//    Rev 1.182   24 Jun 1997 13:33:00   JWILBER
// Changed VID 4dae from TwoLines.L to TwoLines.M.
//
//    Rev 1.181   24 Jun 1997 13:28:38   JWILBER
// Added #define for WM.TwoLines.L, 4dae.
//
//    Rev 1.180   23 Jun 1997 18:45:06   JWILBER
// Removed VID for WM.Cap.Y, 4dae, since it's not infectious.
//
//    Rev 1.179   23 Jun 1997 16:27:20   JWILBER
// Added VIDs for WM.Cap.X, 4dad, and WM.Cap.Y, 4dae.
//
//    Rev 1.178   23 Jun 1997 15:41:00   AOONWAL
// Modified during JULY 97 build
//
//    Rev JULY97  23 Jun 1997 14:34:10   RELNITI
// MODIFY 0x4dAC: VID_UBAHFILEA => PESANB
//
//    Rev JULY97  23 Jun 1997 09:58:58   ECHIEN
// Added VID_UBAHFILEA 0x4DAC
//
//    Rev JULY97  19 Jun 1997 16:16:42   CFORMUL
// Changed VID_CAPV -> VID_CAPU
//         VID_CAPW -> VID_CAPV
// Added VID_CAPW
//
//    Rev JULY97  19 Jun 1997 16:12:24   ECHIEN
// Twolines.L
//
//    Rev JULY97  19 Jun 1997 09:37:38   CFORMUL
// Added VID_CAPW
//
//    Rev JULY97  18 Jun 1997 17:17:58   CFORMUL
// Added VID_CAPV
//
//    Rev JULY97  18 Jun 1997 15:16:36   ECHIEN
// Typo
//
//    Rev JULY97  18 Jun 1997 14:51:54   ECHIEN
// Added LunchD 0x4ab7
//
//    Rev JULY97  18 Jun 1997 13:52:36   CFORMUL
// Added VID_MDMAT
//
//    Rev JULY97  18 Jun 1997 13:07:04   ECHIEN
// Added VID_MDMAS 0x4DA5
//
//    Rev JULY97  17 Jun 1997 12:35:20   ECHIEN
// added VID_KOMPUD 0x4da4
//
//    Rev JULY97  17 Jun 1997 11:32:44   ECHIEN
// Added 0x4DA3 VID_TWOLINESK
//
//    Rev JULY97  16 Jun 1997 18:00:40   DANG
// Changed ZZZ to Czech.A
//
//    Rev JULY97  16 Jun 1997 17:53:04   DANG
// Renamed VID_MUCKF to VID_MUCKE and then added a new MUCKF
//
//    Rev JULY97  16 Jun 1997 16:48:14   DANG
// Added Muck.F for CI#12220
// Renamed VID_RUSSIANPOLY to VID_DEMONA per JW's request.
//
//    Rev 1.177   16 Jun 1997 06:18:18   DANG
// Pulled Johnny.O and niceDay.M.  Repeats
//
//    Rev 1.176   16 Jun 1997 05:17:10   DANG
// Added customer issues
// 12191 --> Johnny.O
// 12218 --> Irish.R
// 12223 --> ShowOff.BO and ZZZ
// 12224 --> Cap.U and NiceDay.M
//
//    Rev 1.175   16 Jun 1997 00:22:36   DANG
// Added SwLabs.D for CI#12190.  Moved Russian Poly to 4D9D
//
//    Rev 1.174   14 Jun 1997 13:04:18   JWILBER
// Added define for WM.Russian Poly, 4d9c.
//
//    Rev 1.173   13 Jun 1997 20:46:00   RELNITI
// ADD 4d9a-b
//
//    Rev 1.172   13 Jun 1997 20:34:54   RELNITI
// ADD 4d97-99
//
//    Rev 1.171   13 Jun 1997 20:16:02   RELNITI
// ADD 4d93-96
//
//    Rev 1.170   13 Jun 1997 19:49:46   RELNITI
// MODIFY 4D19: SHOWOFFBL => BM
//
//    Rev 1.169   13 Jun 1997 19:43:34   RELNITI
// ADD 4d89-4d92
//
//    Rev 1.168   13 Jun 1997 18:41:06   RELNITI
// RE-USE 4d54: => VID_WAZZUCH
//
//    Rev 1.167   13 Jun 1997 16:57:58   DANG
// Added Cap.T for CI#12206 and 12207. Re-used VID 4D37
// Added ShowOff.BN for CI#12208. Re-used VID 4D51
//
//    Rev 1.166   13 Jun 1997 15:32:16   RELNITI
// ADD 4D69-4D88
// MODIFY 1E9F: VID_TWNOK=>BOXA
//
//    Rev 1.165   13 Jun 1997 14:27:22   DANG
// Added Colors.BK for CI#12188.  Re-used VID 0x4D31
//
//    Rev 1.164   13 Jun 1997 13:53:24   RELNITI
// ADD 4d5e-4d68
//
//    Rev 1.163   13 Jun 1997 12:46:04   RELNITI
// FIX some VID typos
//
//    Rev 1.162   13 Jun 1997 12:33:12   RELNITI
// ADD 4d22-4d5d
//
//    Rev 1.161   12 Jun 1997 18:40:36   JWILBER
// Added define for WM.Niceday.L, 4d21.
//
//    Rev 1.160   12 Jun 1997 15:19:00   DANG
// Added NPad.BV for CI#12180
//
//    Rev 1.159   12 Jun 1997 11:20:06   RELNITI
// ADD VID_IRISH_FAMILY2, VID_SHOWOFF_FAMILY2
//
//    Rev 1.158   11 Jun 1997 19:45:18   JWILBER
// Commented out #define for 4d20.
//
//    Rev 1.157   11 Jun 1997 18:54:54   JWILBER
// Added #define for WM.Colors.BK, 4d20.
//
//    Rev 1.156   11 Jun 1997 18:04:08   RELNITI
// ADD VID_RAPI_FAMILY2, VID_RAPI_FAMILY3
//
//    Rev 1.155   11 Jun 1997 16:51:24   RELNITI
// ADD VID_SHOWOFF, RAPI, BANDUNG, COLORS, IRISH_FAMILY (0x4d1a-1e)
//
//    Rev 1.154   11 Jun 1997 16:23:24   DANG
// Added ShowOff.BL for CI#12178
//
//    Rev 1.153   11 Jun 1997 14:46:22   DANG
// Added Alien.F for CI#12171
//
//    Rev 1.152   11 Jun 1997 00:35:56   JWILBER
// Added define for WM.Cap.R, VID 4d17.
//
//    Rev 1.151   10 Jun 1997 22:11:06   JWILBER
// Added #defines for VID_JOHNNY.L, 4d15, and VID_CAPQ, 4d16.
//
//    Rev 1.150   10 Jun 1997 16:30:46   JWILBER
// Added #define for VID_JOHNNY_VARIANT, 4d14.
//
//    Rev 1.149   10 Jun 1997 11:59:36   JWILBER
// Changed define for VID 4d13 from WM.Kompu to WM.Safwan.
//
//    Rev 1.148   09 Jun 1997 20:38:26   JWILBER
// Added #define for WM.Kompu, 4d13.
//
//    Rev 1.147   06 Jun 1997 18:00:58   RELNITI
// ADD VID_CAPP 4d12
//
//    Rev 1.146   06 Jun 1997 16:47:24   RELNITI
// ADD VID_COLORSBJ 4d11
//
//    Rev 1.145   06 Jun 1997 14:04:08   CFORMUL
// Added VID_COLORSBB
//
//    Rev 1.144   05 Jun 1997 17:24:24   RELNITI
// ADD VID_SHOWOFFBK 4d0F
//
//    Rev 1.143   05 Jun 1997 15:08:46   CFORMUL
// Added VID_CEBUC
//
//    Rev 1.142   05 Jun 1997 14:14:34   RELNITI
// ADD VID_HYBRIDE 0x4d0d
//
//    Rev 1.141   05 Jun 1997 13:59:28   CFORMUL
// Added DMV.F
//
//    Rev 1.140   05 Jun 1997 12:01:18   RELNITI
// ADD VID_IRISHO 4d0B
// MOD 4eDF VID_IRISHO => VID_IRISHP
//
//    Rev 1.139   05 Jun 1997 11:18:12   CFORMUL
// Added VID_GOLDSECRETA
//
//    Rev 1.138   04 Jun 1997 11:40:50   CFORMUL
// Added VID_PAYCHECKB
//
//    Rev 1.137   04 Jun 1997 11:34:38   RELNITI
// ADD VID_NPADBR, MDMAQ, NPADBT, SHOWOFFAU, AV, AX, BB, BC, WAZZUCC, CE
//  4d00-4d08
//
//    Rev 1.136   03 Jun 1997 15:43:40   RELNITI
// ADD VID_SHOWOFFBI 4EFE
//
//    Rev 1.135   03 Jun 1997 15:35:52   CFORMUL
// Added VID_NPADBS
//
//    Rev 1.134   03 Jun 1997 15:01:20   CFORMUL
// Added VID_SHOWOFFBH
//
//    Rev 1.133   03 Jun 1997 09:40:32   RELNITI
// MODIFY 4eD0: VID_NPADBH => _BI
//        4eD7: VID_NPADBK => _BM
//        4eDC: VID_NPADBL => _BO
// ADD VID_NPADBB, BC, BF, BH, BJ, BK, BL, BN, BP, BQ (4eF2-4eFB)
//
//    Rev 1.132   02 Jun 1997 18:53:24   RELNITI
// ADD VID_CAPO 4eF1
//
//    Rev 1.131   02 Jun 1997 18:17:56   CFORMUL
// Added VID_COLORSBE
//
//    Rev 1.130   02 Jun 1997 17:14:40   CFORMUL
// Added VID_COLORSBD
//
//    Rev 1.129   02 Jun 1997 17:02:26   RELNITI
// ADD VID_RAPIAJ2 4EEE
//
//    Rev 1.128   02 Jun 1997 16:13:30   CFORMUL
// Added VID_TWOLINESJ
//
//    Rev 1.127   02 Jun 1997 12:26:06   RELNITI
// ADD VID_SWITCHERA 0x4eEC
//
//    Rev 1.126   02 Jun 1997 11:39:18   RELNITI
// ADD VID_SHOWOFFBG 4eEB
//
//    Rev 1.125   30 May 1997 15:11:16   RELNITI
// ADD VID_IRISHQ 4eE8
//
//    Rev 1.124   30 May 1997 15:08:42   JWILBER
// Freed up VID_COLORSBB, 4ee8 for re-use.
//
//    Rev 1.123   30 May 1997 14:13:38   ECHIEN
// Added VID_CAPN 0x4eea
//
//    Rev 1.122   30 May 1997 13:49:34   JWILBER
// Added #defines for ShowOff.BE 4ee4
//                    ShowOff.BF 4ee7
//                    Colors.BB  4ee8
//                    NiceDay.I  4ee9
//
//    Rev 1.121   30 May 1997 11:45:42   RELNITI
// ADD VID_SHOWOFFBD 4EE6
//
//    Rev 1.120   28 May 1997 18:25:36   ECHIEN
// Removed 0x4ee4 VID_APPDERF
//
//    Rev 1.119   28 May 1997 18:10:54   AOONWAL
// Added #define VID_SHOWOFFBA  0x4ee5
//
//    Rev 1.118   28 May 1997 16:49:54   ECHIEN
// Added VID_APPDERF 0x4ee4
//
//    Rev 1.117   28 May 1997 15:32:50   ECHIEN
// Added VID_BANDUNGAM 0x4ee3
//
//    Rev 1.116   28 May 1997 14:03:24   AOONWAL
// Added #define VID_FRIDAYC 0x4ee2
//
//    Rev 1.115   28 May 1997 13:59:08   JWILBER
// Added #define for VID_SHOWOFFAZ, 4ee1.
//
//    Rev 1.114   28 May 1997 10:02:16   RELNITI
// ADD VID_WAZZUCD 4eE0
//
//    Rev 1.113   28 May 1997 09:01:58   RELNITI
// ADD VID_IRISHO 4eDF
//
//    Rev 1.112   28 May 1997 08:19:30   RELNITI
// ADD VID_CONCEPTO 4eDE
//
//    Rev 1.111   27 May 1997 17:27:22   AOONWAL
// Added VID_CONCEPTAS 0x4edd
//
//    Rev 1.110   27 May 1997 15:46:50   RELNITI
// ADD 4eDB VID_MDMAR, 4eDC VID_NPADBL
//
//    Rev 1.109   27 May 1997 14:50:12   CFORMUL
// Added VID_COLORSBA and VID_SHOWOFFAY
//
//    Rev 1.108   27 May 1997 14:35:22   DANG
// Added Irish.N for CI#11989
// Changed NPad.BJ --> NPad.BK
//
//    Rev 1.107   27 May 1997 11:23:40   AOONWAL
// Modified during JUNE97 build
//
//    Rev JUNE97  26 May 1997 18:59:02   DANG
// Added NPad.BJ for CI#11952
//
//    Rev JUNE97  23 May 1997 14:57:02   RELNITI
// TYPO...
//
//    Rev JUNE97  23 May 1997 14:56:16   RELNITI
// ADD 4eD6 Nicedayh
//
//    Rev JUNE97  23 May 1997 12:55:02   ECHIEN
// Removed syntax error
//
//    Rev JUNE97  23 May 1997 12:32:00   AOONWAL
// commented out VID_CAPJ and renamed VID_CAPN to VID_CAPJ
//
//    Rev JUNE97  22 May 1997 21:17:30   CFORMUL
// Added LAROUXD
//
//    Rev JUNE97  22 May 1997 19:04:20   DANG
// Added MDMA.P for CI#11935
//
//    Rev JUNE97  22 May 1997 18:33:58   JWILBER
// Changed VID_RAPIAH to VID_RAPIAI.
//
//    Rev JUNE97  22 May 1997 16:54:04   JWILBER
// Added #define for VID_RAPIAH, 4ed3.
//
//    Rev JUNE97  22 May 1997 15:52:26   RELNITI
// ADD 4eD2 VID_BANDUNGAI
//
//    Rev JUNE97  22 May 1997 13:10:46   AOONWAL
// Added VID_COLORSAX 0x4ed1
//
//    Rev JUNE97  22 May 1997 11:41:10   CFORMUL
// Added VID_NPADBH
//
//    Rev JUNE97  22 May 1997 09:36:26   RELNITI
// ADD VID_CONCEPTAR, VID_CONCEPTAQ
//
//    Rev JUNE97  21 May 1997 20:05:06   RELNITI
// ADD 4eCD VID_TWTROJ
//
//    Rev JUNE97  21 May 1997 17:30:38   CFORMUL
// Added VID_MUCKF
//
//    Rev JUNE97  21 May 1997 16:54:46   CFORMUL
// Added Npad.BG
//
//    Rev JUNE97  21 May 1997 13:53:36   DANG
// Name changed.
// ShowOff.AX --> ShowOff.AW
//
//    Rev JUNE97  21 May 1997 13:17:54   ECHIEN
// Changed NPADBB -> BE 0x4ebf(?)
//
//    Rev JUNE97  21 May 1997 12:58:58   RELNITI
// ADD VID_IRISHM 4eCA
//
//    Rev JUNE97  21 May 1997 11:04:02   AOONWAL
// Added VID_COLORSAW 0x4ec9
//
//    Rev JUNE97  20 May 1997 21:36:48   JWILBER
// Added #define for Colors.AV, 4ec8.
//
//    Rev JUNE97  20 May 1997 19:31:42   DANG
// Added Bandung.AH for CI#11924
//
//    Rev JUNE97  20 May 1997 17:45:54   DANG
// Added ShowOff.AX for CI#11851
//
//    Rev JUNE97  20 May 1997 15:28:38   DANG
// Added TwoLines.I for CI#11921
//
//    Rev JUNE97  20 May 1997 14:37:32   AOONWAL
// Added 4ec4
//
//    Rev JUNE97  19 May 1997 17:12:52   DANG
// Added Cap.M for CI#11883
//
//    Rev JUNE97  19 May 1997 08:57:04   ECHIEN
// Added VID_NPADBD 0x4ec2
//
//    Rev JUNE97  16 May 1997 18:07:50   DANG
// Added Colors.AU for CI#11860
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

//******!!!!!!!!*********!!!!**********!!!!!********!!!!
// IF YOU ADD ADDITIONAL #include "*.*" FILES,
// YOU MUST ALSO MODIFY VIRSCAN6.MAK!!!!!!!!!!!!!!
//******!!!!!!!!*********!!!!**********!!!!!********!!!!

// Include VIDs for defs made in SARC international offices.
// These have separate files for easy reconcilation with our files.

// Commented out 26 Feb by jjw until we work out reconciliation issues.
// Added back in 20 Apr by jjw, since Darren made the work.
#include "japanvid.h"
#include "austrvid.h"
#include "europvid.h"

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
#define VID_BOXA            0x1e9f
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

// Way out-of-order VID - here due to problems w/ def61.src

#define VID_COPYCAPB        0x1ad8

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
#define VID_CAPZ            0x1efc
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
#define VID_HELPERI         0x4fbe
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
#define VID_CAPAA           0x4fe7
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
//#define VID_CAPJ            0x4E36  This def is pulled.
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
//#define VID_SHUFFLEA        0x4e56
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
//#define VID_RANDOMA         0x4e78
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
#define VID_DZTD            0x4eab
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
#define VID_CONCEPTAP       0x4eba
#define VID_NICEDAYI        0x4ebb
#define VID_BANDUNGO        0x4ebc
#define VID_DMVE            0x4ebe
#define VID_NPADBE          0x4ebf
#define VID_APPDERE         0x4ec0
#define VID_COLORSAU        0x4ec1
#define VID_NPADBD          0x4ec2
#define VID_CAPM            0x4ec3
#define VID_CAPJ            0x4ec4
#define VID_TWOLINESI       0x4ec5
#define VID_SHOWOFFAW       0x4ec6
#define VID_BANDUNGAH       0x4ec7
#define VID_COLORSAV        0x4ec8
#define VID_COLORSAW        0x4ec9
#define VID_IRISHM          0x4eca
#define VID_NPADBG          0x4ecb
#define VID_MUCKE           0x4ecc
#define VID_CONCEPTAR       0x4ece
#define VID_CONCEPTAQ       0x4ecf
#define VID_NPADBI          0x4ed0
#define VID_COLORSAX        0x4ed1
#define VID_BANDUNGAI       0x4ed2
#define VID_RAPIAI          0x4ed3
#define VID_MDMAP           0x4ed4
#define VID_NICEDAYH        0x4ed6
#define VID_NPADBM          0x4ed7
#define VID_IRISHN          0x4ed8
#define VID_COLORSBA        0x4ed9
#define VID_SHOWOFFAY       0x4eda
#define VID_MDMAR           0x4edb
#define VID_NPADBO          0x4edc
#define VID_CONCEPTAS       0x4edd
#define VID_CONCEPTO        0x4ede
#define VID_IRISHP          0x4edf
#define VID_WAZZUCD         0x4ee0
#define VID_SHOWOFFAZ       0x4ee1
#define VID_FRIDAYC         0x4ee2
#define VID_BANDUNGAM       0x4ee3
#define VID_SHOWOFFBE       0x4ee4
#define VID_SHOWOFFBA       0x4ee5
#define VID_SHOWOFFBD       0x4ee6
#define VID_SHOWOFFBF       0x4ee7
#define VID_IRISHQ          0x4ee8
#define VID_NICEDAYJ        0x4ee9
#define VID_CAPN            0x4eea
#define VID_SHOWOFFBG       0x4eEB
#define VID_SWITCHERA       0x4eEC
#define VID_TWOLINESJ       0x4eed
#define VID_RAPIAJ2         0x4eEE
#define VID_COLORSBF        0x4eef
#define VID_COLORSBE        0x4ef0
#define VID_CAPO            0x4eF1
#define VID_NPADBB          0x4eF2
#define VID_NPADBC          0x4eF3
#define VID_NPADBF          0x4eF4
#define VID_NPADBH          0x4eF5
#define VID_NPADBJ          0x4eF6
#define VID_NPADBK          0x4eF7
#define VID_NPADBL          0x4eF8
#define VID_NPADBN          0x4eF9
#define VID_NPADBP          0x4eFA
#define VID_NPADBQ          0x4eFB
#define VID_SHOWOFFBH       0x4efc
#define VID_NPADBS          0x4efd
#define VID_SHOWOFFBI       0x4eFE
#define VID_NPADBR          0x4eFF
#define VID_MDMAQ           0x4d00
#define VID_NPADBT          0x4d01
#define VID_SHOWOFFAU       0x4d02
#define VID_SHOWOFFAV       0x4d03
#define VID_SHOWOFFAX       0x4d04
#define VID_SHOWOFFBB       0x4d05
#define VID_SHOWOFFBC       0x4d06
#define VID_WAZZUCC         0x4d07
#define VID_WAZZUCE         0x4d08
#define VID_PAYCHECKB       0x4d09
#define VID_GOLDSECRETA     0x4d0a
#define VID_IRISHO          0x4d0b
#define VID_DMVF            0x4d0c
#define VID_HYBRIDE         0x4d0d
#define VID_CEBUC           0x4d0e
#define VID_SHOWOFFBK       0x4d0f
#define VID_COLORSBB        0x4d10
#define VID_COLORSBJ        0x4d11
#define VID_CAPP            0x4d12
#define VID_SAFWAN          0x4d13
// 0x4d14 is defined below for Johnny Variant
#define VID_JOHNNYN         0x4d15
#define VID_CAPQ            0x4d16
#define VID_CAPR            0x4d17
#define VID_ALIENF          0x4d18
#define VID_SHOWOFFBM       0x4d19
#define VID_NPADBV          0x4d20
#define VID_NICEDAYL        0x4d21
#define VID_ANAKA           0x4d22
#define VID_ATTACKA         0x4d23
#define VID_BANDUNGAG       0x4d24
#define VID_BANDUNGAJ       0x4d25
#define VID_BANDUNGAK       0x4d26
#define VID_BANDUNGAL       0x4d27
#define VID_BANDUNGAN       0x4d28
#define VID_BANDUNGAO       0x4d29
#define VID_BANDUNGAP       0x4d2A
#define VID_BEAMA           0x4d2B
#define VID_BEEPERA         0x4d2C
#define VID_BEEPERB         0x4d2D
#define VID_CEBUB           0x4d2E
#define VID_COLORSAY        0x4d2F
#define VID_COLORSAZ        0x4d30
#define VID_COLORSBK        0x4d31
#define VID_COLORSBC        0x4d32
#define VID_COLORSBD        0x4d33
#define VID_COLORSBG        0x4d34
#define VID_COLORSBH        0x4d35
#define VID_COLORSBI        0x4d36
#define VID_CAPT            0x4d37
#define VID_CONCEPTAT       0x4d38
#define VID_CULTA           0x4d39
#define VID_CVCK1C          0x4d3A
#define VID_CVCK1D          0x4d3B
#define VID_CVCK1E          0x4d3C
#define VID_CVCK1I          0x4d3D
#define VID_DANCEA          0x4d3E
#define VID_DEFNDRA         0x4d3F
#define VID_DOGGIEE         0x4d40
#define VID_DRACULAA        0x4d41
#define VID_DZTE            0x4d42
#define VID_ENVADERA        0x4d43
#define VID_HARKA           0x4d44
#define VID_HYBRIDD         0x4d45
#define VID_HYPERA          0x4d46
#define VID_JAJA            0x4d47
#define VID_JOHNNYL         0x4d48
#define VID_JOHNNYM         0x4d49
#define VID_KILLOKA         0x4d4A
//#define VID_KILLOKB         0x4d4B
//#define VID_KILLOKC         0x4d4C
#define VID_KOMPUC          0x4d4D
#define VID_LOXA            0x4d4E
#define VID_MINIMALP        0x4d4F
#define VID_NICEDAYG        0x4d50
#define VID_SHOWOFFBN       0x4d51
#define VID_NICEDAYK        0x4d52
#define VID_NOPRINTA        0x4d53
#define VID_WAZZUCH         0x4d54
#define VID_NPADBU          0x4d55
#define VID_ORHEYA          0x4d56
#define VID_PAYCHKD         0x4d57
#define VID_PESANA          0x4d58
#define VID_PREALIEN        0x4d59
#define VID_RAPIAH          0x4d5A
#define VID_RAPIAH1         0x4d5B
#define VID_RAPIAH2         0x4d5C
#define VID_RAPIM1          0x4d5D
#define VID_SHOWOFFBJ       0x4d5E
#define VID_SMILEYC         0x4d5F
#define VID_SOCKSA          0x4d60
#define VID_STREZZA         0x4d61
#define VID_SWITCHERB       0x4d62
#define VID_UNDERGROUND     0x4d63
#define VID_VENENOA         0x4d64
#define VID_WAZZUCF         0x4d65
#define VID_WAZZUCG         0x4d66
#define VID_ZASHIB          0x4d67
#define VID_ZOOLOGA         0x4d68
#define VID_ANTA            0x4d69
#define VID_ANTB            0x4d6A
#define VID_EMPERORA        0x4d6B
#define VID_EMPERORB        0x4d6C
#define VID_EMPERORC        0x4d6D
#define VID_EPIDEMICB       0x4d6E
#define VID_EPIDEMICC       0x4d6F
#define VID_MARKA           0x4d70
#define VID_MARKB           0x4d71
#define VID_PIGA            0x4d72
#define VID_PIGB            0x4d73
#define VID_PIGC            0x4d74
#define VID_PIGD            0x4d75
#define VID_PIGE            0x4d76
#define VID_RELLIKA         0x4d77
#define VID_TWNOK           0x4d78
#define VID_TWNOL           0x4d79
#define VID_TWNOM           0x4d7A
#define VID_TWNON           0x4d7B
#define VID_TWNOO           0x4d7C
#define VID_TWNOP           0x4d7D
#define VID_TWNOQ           0x4d7E
#define VID_TWNOR           0x4d7F
#define VID_TWNOS           0x4d80
#define VID_TWNOT           0x4d81
#define VID_TWNOU           0x4d82
#define VID_TWNOV           0x4d83
#define VID_TWNOW           0x4d84
#define VID_TWNOX           0x4d85
#define VID_VARMINTA        0x4d86
#define VID_WMVH1B          0x4d87
#define VID_WMVH1C          0x4d88
#define VID_BALROGA         0x4d89
#define VID_CAPS            0x4d8A
#define VID_CLOCKI          0x4d8B
#define VID_CVCK1F          0x4d8C
#define VID_CVCK1G          0x4d8D
#define VID_CVCK1H          0x4d8E
#define VID_PAYCHECKC       0x4d8F
#define VID_SHOWOFFBL       0x4d90
#define VID_SWLABSC         0x4d91
#define VID_NJWMDLK1J       0x4d92
#define VID_EMPERORD        0x4d93
#define VID_TWNOY           0x4d94
#define VID_VAMPIREA        0x4d95
#define VID_VAMPIREB        0x4d96
#define VID_MAKRONEA        0x4d97
#define VID_REDA            0x4d98
#define VID_VICINITY        0x4d99
#define VID_ORDOA           0x4d9a
#define VID_QUICKA          0x4d9b
#define VID_SWLABSD         0x4d9c
#define VID_DEMONA          0x4d9d
#define VID_IRISHR          0x4d9e
#define VID_SHOWOFFBO       0x4d9f
#define VID_CZECHA          0x4da0
#define VID_MUCKF           0x4da2
#define VID_TWOLINESK       0x4da3
#define VID_KOMPUD          0x4da4
#define VID_MDMAS           0x4da5
#define VID_MDMAT           0x4da6
#define VID_LUNCHD          0x4da7
#define VID_CAPU            0x4da8
#define VID_CAPV            0x4da9
#define VID_TWOLINESL       0x4daa
#define VID_CAPW            0x4dab
#define VID_PESANB          0x4dac
#define VID_CAPB            0x4dad
#define VID_TWOLINESM       0x4dae
#define VID_CEBUA           0x4db2
#define VID_CAPC            0x4db3
#define VID_TWOLINESN       0x4db4
#define VID_MDMAV           0x4db5
#define VID_NICEDAYM        0x4db6
#define VID_TWOLINESO       0x4db7
#define VID_TWOLINESP       0x4db8
#define VID_CONCEPT_BB      0x4db9
#define VID_SHOWOFFBS       0x4dba
#define VID_APPDERF         0x4dbb
#define VID_APPDERH         0x4dbf
#define VID_NPADBX          0x4dc0
#define VID_NPADBY          0x4dc1
#define VID_NPADBZ          0X4dc2
#define VID_NPADCA          0x4dc3
#define VID_NPADCB          0x4dc4
#define VID_NPADCC          0X4dc5
#define VID_NPADCD          0X4dc6
#define VID_BANDUNGAQ       0x4dc7
#define VID_BANDUNGAR       0x4dc8
#define VID_CONCEPTAU       0x4dc9
#define VID_CONCEPTAV       0x4dca
#define VID_CONCEPTAW       0x4dcb
#define VID_CONCEPTAX       0x4dcc
#define VID_CONCEPTAY       0x4dcd
#define VID_SHOWOFFBP       0x4dce
#define VID_SHOWOFFBQ       0x4dcf
#define VID_SHOWOFFBR       0x4dd0
#define VID_DIVINAG         0x4dd2
#define VID_CONCEPTBA       0x4dd4
#define VID_HYBRIDG         0x4dd5
#define VID_DIVINAH         0x4dd6
#define VID_SWLABSF95       0x4dd8
#define VID_HYBRIDH         0x4dda
#define VID_ALLIANCEB       0x4ddb
#define VID_APPDERG         0x4ddc
#define VID_APPDERI         0x4ddd
#define VID_BANDUNGAS       0x4dde
#define VID_BANDUNGAT       0x4ddf
#define VID_BANDUNGAV       0x4de0
#define VID_BANDUNGAW       0x4de1
#define VID_CHILDISHA       0x4de2
#define VID_COLORSBL        0x4de3
#define VID_COLORSBN        0x4de4
#define VID_TWOLINESQ       0x4de5
#define VID_CONCEPTAZ       0x4de6
#define VID_DZTF            0x4de7
#define VID_GOLDFISHC       0x4de8
#define VID_GOLDSECRETB     0x4de9
#define VID_HYBRIDF         0x4dea
#define VID_ILLITERATEA     0x4deb
#define VID_IMPOSTERD       0x4dec
#define VID_IMPOSTERE       0x4ded
#define VID_INCARNATEA      0x4dee
#define VID_KOMPUE          0x4def
#define VID_KOMPUF          0x4df0
#define VID_LUNCHE          0x4df1
#define VID_MDMAU           0x4df2
#define VID_REHENESA        0x4df3
#define VID_MUCKG           0x4df4
#define VID_MUCKH           0x4df5
#define VID_NPADCG          0x4df6
#define VID_NUCLEARN        0x4df7
#define VID_NUKERA          0x4df8
#define VID_PANJANGA        0x4df9
#define VID_PERCENTATW      0x4dfa
#define VID_SINCEA          0x4dfb
#define VID_TEMPLEB         0x4dfc
#define VID_VICINITYBDE     0x4dfd
#define VID_VOLCANOAIT      0x4dfe
#define VID_WAZZUCI         0x4dff
#define VID_CONCEPTBC       0x4c00
#define VID_NUCLEARO        0x4c01
#define VID_CONCEPTBE       0x4c02
#define VID_NICEDAYN        0x4c03
#define VID_ZMBADE          0x4c04
#define VID_NPADCM          0x4c05
#define VID_MDMAX           0x4c06
#define VID_PAYCHECKE       0x4c08
#define VID_NUCLEARP        0x4c0a
#define VID_NUCLEARQ        0x4c0b
#define VID_APPDERJ         0x4c0c
#define VID_FRIDAYD         0x4c0d
#define VID_ANGELA          0x4c0e
#define VID_TWNOZTW         0x4c0f
#define VID_COLORSBM        0x4c10
#define VID_BANDUNGAU       0x4c11
#define VID_CHAOSB          0x4c12
#define VID_BARBAROAIT      0x4c13
#define VID_ANTCTW          0x4c14
#define VID_ANTDTW          0x4c15
#define VID_BLACKA          0x4c16
#define VID_HELPERF         0x4c17
#define VID_HELPERG         0x4c18
#define VID_HELPERH         0x4c19
#define VID_LUCYA           0x4c1a
#define VID_SAMBTW          0x4c1b
#define VID_WAZZUCK         0x4c1c
#define VID_SHOWOFFBT       0x4c1d
#define VID_WAZZUCJ         0x4c1e
#define VID_CONCEPTBD       0x4c1f
#define VID_CONCEPTBF       0x4c20
#define VID_BANDUNGAX       0x4c21
#define VID_BANDUNGAY       0x4c22
#define VID_ALEXCTW         0x4c23
#define VID_CONCEPTBG       0x4c24
#define VID_ALEXETW         0x4c25
#define VID_ARCHERA         0x4c26
#define VID_SHOWOFFBU       0x4c27
#define VID_SHOWOFFBV       0x4c28
#define VID_SHOWOFFBW       0x4c29
#define VID_HOUATW          0x4c2A
#define VID_MDMAW           0x4c2B
#define VID_NOPMDE          0x4c2C
#define VID_NPADCI          0x4c2D
#define VID_NPADCJ          0x4c2E
#define VID_NPADCK          0x4c2F
#define VID_NPADCL          0x4c30
#define VID_NPADCN          0x4c31
#define VID_NPADCO          0x4c32
#define VID_OBLOMA          0x4c33
#define VID_CHEATA          0x4c34
#define VID_CHEATB          0x4c35
#define VID_RANDOMA         0x4c36
#define VID_DZTG            0x4c37
#define VID_DMVG            0x4c38
#define VID_GOODNIGHTC      0x4c39
#define VID_ANAKB           0x4c3a
#define VID_FORMATSA        0x4c3B
#define VID_SWLABSG         0x4c3C
#define VID_RAZERA          0x4c3D
#define VID_MULAIA          0x4c3E
#define VID_HITMANA         0x4c3F
#define VID_BOXCTW          0x4c40
#define VID_PIGFTW          0x4c41
#define VID_VAMPIRECTW      0x4c42
#define VID_VAMPIREDTW      0x4c43
#define VID_VAMPIREETW      0x4c44
#define VID_VAMPIREFTW      0x4c45
#define VID_MDMAY           0x4c46
#define VID_PAYCHECKF       0x4c47
#define VID_SCHUMANNADE     0x4c48
#define VID_SCHUMANNBDE     0x4c49
#define VID_OBLOMB          0x4c4A
#define VID_TEMPLEC         0x4c4B
#define VID_FRIDAYEDE       0x4c4C
#define VID_OBAYA           0x4c4D
#define VID_WAZZUCL         0x4c4E
#define VID_SWITCHERC       0x4c4F

#define VID_CHAKAA          0x4c51
#define VID_DAKOTAA         0x4c52
#define VID_MUCKI           0x4c53
#define VID_NUCLEARR        0x4c54
#define VID_NPADCP          0x4c55
#define VID_NPADCQ          0x4c56
#define VID_NPADCR          0x4c57
#define VID_OBLOMC          0x4c58
#define VID_VAMPIREGTW      0x4c59
#define VID_VAMPIREHTW      0x4c5A
#define VID_SCHUMANNCDE     0x4c5B
#define VID_SPYA            0x4c5C
#define VID_BANDUNGAZ       0x4c5D
#define VID_CAPX            0x4c5E
#define VID_VICINITYCDE     0x4c5F
#define VID_ANAKC           0x4c60
#define VID_HYBRIDI         0x4c61
#define VID_SWITCHERD       0x4C62
#define VID_MUCKL           0x4C63
#define VID_KILLLUFA        0x4C64
#define VID_CAPY            0x4C65
#define VID_CLOCKJ          0x4C66
#define VID_NPADCS          0x4C67
#define VID_BANDUNGBA       0x4C68
#define VID_NOFORCEA        0x4C69
#define VID_OBLOMD          0x4C6A
#define VID_VIVAA           0x4C6B
#define VID_CONCEPTBH       0x4C6C
#define VID_ARCHERB         0x4C6D
#define VID_MINIMALQ        0x4C6E
#define VID_MINIMALR        0x4C6F
#define VID_TEARB           0x4C70
#define VID_VAMPIREITW      0x4C71
#define VID_WAZZUCM         0x4C72
#define VID_WAZZUCN         0x4C73
#define VID_WAZZUCO         0x4C74
#define VID_SCREWA          0x4C75
#define VID_PAYCHECKG       0x4C76
#define VID_ALIENG          0x4c77
#define VID_TEMPLED         0x4c78
#define VID_TEMPLEE         0x4c79
//4C7A is taken for VID_MUCK_FAMILY?
#define VID_NUCLEARS        0x4c7b
#define VID_CONCEPT_BJ      0x4c7c
//4C7D is taken for VID_SWITCHER_FAMILY
#define VID_CONCEPTBK       0x4c7E
//4c7F is taken for VID_DNTC_FAMILY
#define VID_HYBRIDJ         0x4c80
//4C81 is taken for VID_LAROUXF
#define VID_CONCEPTBI       0x4C82
#define VID_CONCEPTBL       0x4C83
#define VID_CONCEPTBM       0x4C84
#define VID_DIVINAI         0x4C85
#define VID_NOPN            0x4C86
#define VID_NPADCT          0x4C87
#define VID_NPADCU          0x4C88
#define VID_NPADCV          0x4C89
#define VID_NPADCW          0x4C8A
#define VID_SHOWOFFBX       0x4C8B
#define VID_MUCKJ           0x4C8C
#define VID_MUCKK           0x4C8D
#define VID_MUCKM           0x4C8E
#define VID_MUCKN           0x4C8F
#define VID_WANTATW         0x4C90
#define VID_WAZZUCP         0x4C91
#define VID_WAZZUCQ         0x4C92
#define VID_WAZZUCR         0x4C93
#define VID_HEADA           0x4C94
#define VID_APPDERK         0x4C95
#define VID_CAPAB           0x4C96
#define VID_CAPAC           0x4C97
#define VID_CAPAD           0x4C98
#define VID_CAPAE           0x4C99
#define VID_CAPAF           0x4C9A
#define VID_COLORSBP        0x4C9B
#define VID_JOHNNYO         0x4C9C
#define VID_NPADCX          0x4C9D
#define VID_SHOWOFFBY       0x4C9E
#define VID_SHOWOFFBZ       0x4C9F
#define VID_SHOWOFFCA       0x4CA0
#define VID_SWITCHERE       0x4CA1
#define VID_SWITCHERF       0x4CA2
#define VID_NPADCY          0x4CA3
#define VID_GOODNIGHTD      0x4CA4
#define VID_GOODNIGHTE      0x4ca6
#define VID_CAPAH           0x4CA7
#define VID_CAPAI           0x4CA8
#define VID_CAPAJ           0x4CA9
#define VID_CAPAK           0x4CAA
#define VID_CAPAL           0x4CAB
#define VID_CAPAM           0x4CAC
#define VID_CONCEPTBN       0x4CAD
#define VID_DMVD            0x4CAE
#define VID_LOXB            0x4CAF
#define VID_MDMAZ           0x4CB0
#define VID_MESSA           0x4CB1
#define VID_MINIMALS        0x4CB2
#define VID_MINIMALT        0x4CB3
#define VID_MUCKO           0x4CB4
#define VID_NPADCZ          0x4CB5
#define VID_NUCLEART        0x4CB6
#define VID_OBLOME          0x4CB7
#define VID_SHOWOFFCB       0x4CB8
#define VID_SHOWOFFCC       0x4CB9
#define VID_SUPERSTITIOUSA	0x4CBA
#define VID_WAZZUCS         0x4CBB
#define VID_WAZZUCT         0x4CBC
#define VID_WAZZUCU         0x4CBD
#define VID_GASA            0x4CBE
#define VID_SWLABSH         0x4CBF
#define VID_ZMBBDE          0x4cc0
#define VID_KOMPUG          0x4CC1
#define VID_BANDUNGBB       0x4CC2
#define VID_CAPAN           0x4CC3
#define VID_CAPAO           0x4CC4
#define VID_CLOCKLDE        0x4CC5
#define VID_COLORSBQ        0x4CC6
#define VID_DIVINAJ         0x4CC7
#define VID_EMTA            0x4CC8
#define VID_NPADDB          0x4CC9
#define VID_SHOWOFFCD       0x4CCA
#define VID_WAZZUCV         0x4CCB
#define VID_BANDUNGBC       0x4CCC
#define VID_CAPAQ           0x4CCD
#define VID_CAPAR           0x4CCE
#define VID_CAPAS           0x4CCF
#define VID_CONCEPTBO       0x4CD0
#define VID_MDMAAA          0x4CD1
#define VID_NPADDC          0x4CD2
#define VID_NPADDD          0x4CD3
#define VID_SHOWOFFCE       0x4CD4
#define VID_MUCKP           0x4CD5
#define VID_MUCKQ           0x4CD6
#define VID_HYBRIDK         0x4CD7
#define VID_GOODNIGHTF      0x4CD8
#define VID_NIKNATA         0x4CD9
#define VID_GOODNIGHTG      0x4CDA
//4cDB is taken for VID_ANAK_FAMILY
#define VID_CAPAT           0x4CDC
#define VID_CAPAU           0x4CDD
#define VID_COLORSBR        0x4CDE
#define VID_CONCEPTBP       0x4CDF
#define VID_DZTH            0x4CE0
#define VID_INDIAA          0x4CE1
#define VID_LAMAHABR        0x4CE2
#define VID_NPADDE          0x4CE3
#define VID_NPADDF          0x4CE4
#define VID_NPADDG          0x4CE5
#define VID_NPADDH          0x4CE6
#define VID_NPADDI          0x4CE7
#define VID_NPADDJ          0x4CE8
#define VID_SHOWOFFCF       0x4CE9
#define VID_SHOWOFFCG       0x4CEA
#define VID_SHOWOFFCH       0x4CEB
#define VID_SWITCHERG       0x4CEC
#define VID_TWNOAATW        0x4CED
#define VID_TWNOABTW        0x4CEE
// 4cef is used by Hybrid_Family
#define VID_TEMPLEF         0x4cf0
#define VID_LUNCHF          0x4cf1
#define VID_SMALLA          0x4cf2
// 4cf3 is used by Slow Dog, due to Navex file repair
#define VID_COUNTTENC       0x4cf4
#define VID_MUCKR           0x4cf5
#define VID_GOODNIGHTH      0x4cf6
#define VID_ALIENH          0x4CF7
#define VID_AMMYATW         0x4CF8
#define VID_CAPAV           0x4CF9
#define VID_CAPAW           0x4CFA
#define VID_CAPAX           0x4CFB
#define VID_CAPAY           0x4CFC
#define VID_CAPAZ           0x4CFD
#define VID_COLORSBS        0x4CFE
#define VID_CONCEPTBQ       0x4CFF
#define VID_CONCEPTBR       0x6000
#define VID_MUCKQ2          0x6001
#define VID_MUCKS           0x6002
#define VID_MUCKT           0x6003
#define VID_MUCKU           0x6004
#define VID_MUCKV           0x6005
#define VID_MUCKW           0x6006
#define VID_MUCKX           0x6007
#define VID_NOTTICEA        0x6008
#define VID_NPADDK          0x6009
#define VID_NUCLEARU        0x600A
#define VID_SETMDBTW        0x600B
#define VID_SWITCHERH       0x600C
#define VID_WAZZUCW         0x600D
#define VID_WAZZUCX         0x600E
#define VID_DIETZELA        0x600F
//#define VID_INSIDEOUTA      0x6010
//#define VID_JUNKFACE        0x6011
#define VID_GOODNIGHTI      0x6012
#define VID_TEMPLEK         0x6013
#define VID_WARNINGA        0x6015
#define VID_MDMAAN          0x6016
#define VID_GOODNIGHTJ      0x6017
#define VID_GOODNIGHTK      0x6018
#define VID_LUNCHG          0x6019
#define VID_KILLLUFB        0x601A
#define VID_GOODNIGHTL      0x601B
#define VID_GOODNIGHTM      0x601C
//0x601D is taken for VID_GOODNIGHTFAM
#define VID_CAPBA           0x601E
#define VID_CAPBB           0x601F
#define VID_CAPBC           0x6020
#define VID_CAPBD           0x6021
#define VID_COLORSBT        0x6022
#define VID_CONCEPTBT       0x6023
#define VID_CREMAA          0x6024
#define VID_DATEC           0x6025
#define VID_DATED           0x6026
#define VID_EASYB           0x6027
#define VID_ENFKAKIT        0x6028
#define VID_FRIENDLYBDE     0x6029
#define VID_GASB            0x602A
#define VID_HORNA           0x602B
#define VID_JOHNNYP         0x602C
#define VID_KILLPROTB       0x602D
#define VID_LORDA           0x602E
#define VID_MALICEA         0x602F
#define VID_MGA             0x6030
#define VID_MGB             0x6031
#define VID_MINIMALU        0x6032
#define VID_MUCKZ           0x6033
#define VID_MUCKAA          0x6034
#define VID_MUCKAB          0x6035
#define VID_NJWMVCK2C       0x6036
#define VID_NOPODE          0x6037
#define VID_NPADDL          0x6038
#define VID_NPADDM          0x6039
#define VID_NPADDN          0x603A
#define VID_NPADDO          0x603B
#define VID_NPADDP          0x603C
#define VID_NPADDQ          0x603D
#define VID_NPADDR          0x603E
#define VID_NPADDS          0x603F
#define VID_NPADDT          0x6040
#define VID_NPADDU          0x6041
#define VID_NPADDV          0x6042
#define VID_NPADDW          0x6043
#define VID_NPADDX          0x6044
#define VID_NPADDY          0x6045
#define VID_NPADDZ          0x6046
#define VID_NPADEA          0x6047
#define VID_NPADEB          0x6048
#define VID_NPADEC          0x6049
#define VID_NPADED          0x604A
#define VID_OLDPADA         0x604B
#define VID_OLDPADB         0x604C
#define VID_PLUSHADA        0x604D
#define VID_PWDA            0x604E
#define VID_RAMSESAIT       0x604F
#define VID_RAPIAM          0x6050
#define VID_RAPIAM1         0x6051
#define VID_SAVERB          0x6052
#define VID_SCHUMANNDDE     0x6053
#define VID_SHADOWA         0x6054
#define VID_SHOWOFFCI       0x6055
#define VID_SHOWOFFCJ       0x6056
#define VID_SILLYA          0x6057
#define VID_SPOOKYEDE       0x6058
#define VID_SWITCHERI       0x6059
#define VID_TAMAGOA         0x605A
#define VID_TEMPLEI         0x605B
#define VID_TWNOADTW        0x605C
#define VID_TWOLINESR       0x605D
#define VID_UKAA            0x605E
#define VID_WAZZUCY         0x605F
#define VID_WAZZUCZ         0x6060
#define VID_WAZZUDA         0x6061
#define VID_WAZZUDB         0x6062
#define VID_WAZZUDC         0x6063
#define VID_WAZZUDD         0x6064
#define VID_WAZZUDE         0x6065
#define VID_WAZZUDF         0x6066
#define VID_WAZZUDI         0x6067
#define VID_WAZZUDJ         0x6068
#define VID_WIEDEROEFFNENADE 0x6069
#define VID_APPDERL         0x606A
#define VID_WAZZUDW         0x606B
#define VID_TEMPLEJ         0x606C
#define VID_MDMAAM          0x606D
//0x606E is taken for VID_LAROUXAA
#define VID_WAZZUDL         0x606F
//0x6070 is taken for VID_LAROUXAC
//0x6071 is taken for Polly a Win95 virus
#define VID_SHOWOFFCK       0x6072
#define VID_APPDERN         0x6073
#define VID_DIVINAL         0x6074
#define VID_NIKNATB         0x6075
#define VID_CONCEPTBS       0x6076
//0x6077 is taken for VID_LAROUXL
//0x6078 is taken for VID_APPDER_FAMILY
#define VID_SWLABSI         0x6079
#define VID_INFLUENZAB      0x607A
#define VID_INFLUENZAA      0x607B
//0x607C is taken for VID_TEMPLE_FAMILY
#define VID_GOLDFISHD       0x607D
#define VID_GOLDFISHE       0x607E
//0x607F is taken for VID_NINJAA
#define VID_WAZZUDL95       0x6080 //vid_wazzudl is used by w97m.wazzu.dl
//0x6081 is taken for VID_NIKNAT_FAMILY?
#define VID_SCHUMANNEDE     0x6082
#define VID_BOXDTW          0x6083
#define VID_BOXETW          0x6084
#define VID_MDMAAD          0x6085
#define VID_MDMAAE          0x6086
#define VID_MDMAAF          0x6087
#define VID_MDMAAG          0x6088
#define VID_MDMAAB          0x6089
#define VID_VHDLATW         0x608A
#define VID_NUCLEARV        0x608B
#define VID_APPDERO         0x608C
//0x608D-0x6092 are taken by Windows viruses & Weed.5850.D
//0x6093 is taken by VID_PAYCHECK_FAMILY.
//0x6094/5 is taken by Dubug.3999
#define VID_MDMAAK          0x6096
//0x6097 taken for a Trojan Horse
#define VID_OBLOMH          0x6098
#define VID_OBLOMF          0x6099
#define VID_OBLOMG          0x609A
#define VID_ALLIANCEC       0x609B
#define VID_ALLIANCED       0x609C
#define VID_ALLIANCEE       0x609D
#define VID_ALLIANCEF       0x609E
#define VID_BISHKEKA        0x609F
#define VID_CLOCKK          0x60A0
#define VID_CONCEPTBT2      0x60A1
#define VID_CONCEPTBU       0x60A2
#define VID_CONCEPTBV       0x60A3
#define VID_CONCEPTBX       0x60A4
#define VID_COUNTERA        0x60A5
#define VID_DMVI            0x60A6
#define VID_ERASERSTW       0x60A7
#define VID_FIVEAA          0x60A8
#define VID_GERMA           0x60A9
#define VID_GNOMOA          0x60AA
#define VID_GROBENA         0x60AB
#define VID_GSISA           0x60AC
#define VID_KOMPUI          0x60AD
#define VID_MDMAAJ          0x60AE
#define VID_MERCADOABR      0x60AF
#define VID_DIVINAK         0x60B0
#define VID_MINIMALV        0x60B1
#define VID_MINIMALW        0x60B2
#define VID_NOPPDE          0x60B3
#define VID_NOPQES          0x60B4
#define VID_OCHOYA          0x60B5
#define VID_SCHOOA          0x60B6
#define VID_TAMAGOBBR       0x60B7
#define VID_TRASHA          0x60B8
#define VID_TWNOAETW        0x60B9
#define VID_WAZZUDH         0x60BA
#define VID_WAZZUDM         0x60BB
#define VID_WOMPATA         0x60BC
#define VID_WOMPIEA         0x60BD
#define VID_YAKAB           0x60BE
#define VID_PESANC          0x60BF
#define VID_CONCEPTCC       0x60C0
//60c1 is taken by an entry in TROJSIGS.H
//taken by VID_PWSTEAL_TROJAN    0x60C2 in TROJSIGS.H
//taken by VID_AOL_TROJAN        0x60C3 in TROJSIGS.H
//taken by VID_LAROUXAU        0x60C4
//taken by VID_LAROUXAI        0x60C5
//taken by VID_LAROUXJ         0x60C6
#define VID_ANARCHY6093A    0x60C7
#define VID_YAKAA           0x60CB
#define VID_APPDERP         0x60CC
#define VID_COUNTERB        0x60CD
#define VID_DIVINAM         0x60CE
#define VID_DMVJ            0x60CF
#define VID_HOTB            0x60D0
#define VID_JERMB           0x60D1
#define VID_LOUVADOA        0x60D2
#define VID_MDMAAL          0x60D3
#define VID_MINIMALY        0x60D4
#define VID_MINIMALZ        0x60D5
#define VID_MVDK1A          0x60D6
#define VID_NOVAB           0x60D7
#define VID_TALONL          0x60D8
#define VID_TRASHB          0x60D9
#define VID_TUNGUSKAA       0x60DA
#define VID_VAMPIREKTW      0x60DB
#define VID_VAMPIRELTW      0x60DC
#define VID_VAMPIREMTW      0x60DD
#define VID_WAZZUDO         0x60DE
#define VID_WAZZUDP         0x60DF
#define VID_BLEEB           0x60E2
#define VID_CONCEPTBW       0x60e3
#define VID_CAPBE           0x60e4
#define VID_DIVINAN         0x60e5
#define VID_CARTMAN         0x60e6
// 60e7 is Laroux.AF
// 60e8-b are trojans
#define VID_RAZERB          0x60ec
#define VID_NUMBER1         0x60ed
// 60ee is Laroux.AG
// 60ef-f2 are Windows viruses
// 60f3 is a trojan
//#define VID_DMVK            0x60f4
#define VID_ANTINS          0x60f5
#define VID_NICEDAYO        0x60f6
// 60f7 is a trojan
// 60f8 is Laroux.BJ
#define VID_GAMBLEA         0x60f9
// 60fa/b are Excel viruses
#define VID_VICTORA         0x60fc
#define VID_CMD             0x60fd  //  jjw 23 Jan 98  VID 6244 in blitz!!!
// 60fe is a Virscan detection / Navex repair (Raid.5831)
#define VID_NOTTICEB        0x60ff
#define VID_MDMAAH          0x6101
#define VID_SCHUMANNGDE     0x6102
#define VID_NICEDAYP        0x6103
#define VID_KOMCONA         0x6107
#define VID_SWLABSF         0x6108
#define VID_MDMAAV          0x610C
#define VID_WARNINGB        0x610F
#define VID_HNLA            0x6114
#define VID_WAZZUDK         0x6115
#define VID_WARNINGC        0x6117
#define VID_SWLABSL         0x6118
#define VID_JULHOA          0x611a
#define VID_SCHUMANNNDE     0x611d
#define VID_SHOWOFFDB       0x6131
#define VID_MDMAAZ          0x613c
#define VID_SCHUMANNPDE     0x613d
#define VID_MUNCHB          0x613e
#define VID_WAZZUEC         0x6140
#define VID_TWNOAJ          0x6142
#define VID_STEROIDB        0x6143
#define VID_SWITCHERJ       0x6144
#define VID_SWITCHERK       0x6146
#define VID_SCHUMANNODE     0x6148
#define VID_SWLABSN         0x614A
#define VID_SCHUMANNJDE     0x614C
#define VID_SWLABSE         0x614D
#define VID_MDMAAT          0x614E
#define VID_DISCOD          0x6151
#define VID_WAZZUEP         0x6153
#define VID_CHACKA          0x6154
#define VID_LAMAHBBR        0x6155
#define VID_WALLPAPERB      0x6157
#define VID_APPDERS         0x6158
#define VID_NAPROXEN        0x615a
#define VID_WARNINGD        0x615f
#define VID_VENENOC         0x6162
#define VID_SCHOOB          0x6164
#define VID_MDMAAU          0x616c
#define VID_MDMABF          0x616E
#define VID_CARTMANB        0x616f
#define VID_GROOVA          0x6170
#define VID_RAZERC          0x6171
#define VID_CZECHB          0x6172
#define VID_ANTICONM        0x6173
#define VID_DMVK            0x6179
#define VID_DZTK            0x617a
#define VID_TEMPLES         0x617c
#define VID_BLASHYRKHA      0x617d
#define VID_EXTRASE         0x6181
#define VID_MENTESB         0x6182
#define VID_MDMABG          0x6183
#define VID_GROOVB          0x6188
#define VID_PANJANGB        0x618b
#define VID_NUCLEARAC       0x618f
#define VID_DMVH            0x6190
#define VID_DEFNDRB         0x6191
#define VID_JERKA           0x6196
#define VID_CARTMANC        0x6198
#define VID_REPLICATOR      0x6199
#define VID_BREAKTIMEA      0x619c
#define VID_CARTMAND        0x619f
#define VID_LEVELERA        0x61A0
#define VID_MDMABH          0x61A3
#define VID_KITTYA          0x61a4
#define VID_NOTTICEF        0x61a5
#define VID_INCARNATA       0x61aa
#define VID_WAZZUFD         0x61ab
#define VID_VMPCK1D         0x61af
#define VID_FORMATERA       0x61b2
#define VID_NOPL            0x61b4
#define VID_LEONORA         0x61b6
#define VID_NUCLEARAD       0x61b9
#define VID_COPYCAP_DAMAGED 0x61ba
#define VID_MDMABI          0x61bb
#define VID_MDMABJ          0x61bc
#define VID_SCHUMANNVDE     0x61bd
#define VID_TWNOAPTW        0x61cc
#define VID_IVANAA          0x61D0
#define VID_LAROUXEN        0x61D1
#define VID_LAROUXDX        0x61D2
#define VID_CVCK1J          0x61D3
#define VID_DNTCFAMILY97    0x61D4
#define VID_SCFAMILY1       0x61D5
#define VID_SCFAMILY2       0x61D5
#define VID_LAROUXFC        0x61D6
#define VID_SETMDC          0x61D7
#define VID_SOLAFISH        0x61D8
#define VID_LAROUXFD        0x61DA
#define VID_LAROUXFF        0x61df
#define VID_COPYCAPC        0x61E0
#define VID_VERMA           0x61E1
#define VID_PANJANGC        0x61e2
#define VID_CLASS_S         0x61E4
#define VID_SOLDIER         0x61E5
#define VID_GROOV_VAR       0x61E6
#define VID_ANTIMARCA       0x61E7
#define VID_LAROUXEO        0x61E8
#define VID_WAZZUFK         0x61E9
#define VID_LAROUXFL        0x61EA
#define VID_LAROUXFM        0x61EB
#define VID_STEROIDJ        0x61EC
#define VID_SCHUMANNY       0x61ee
#define VID_SCHUMANNZ       0x61ef
#define VID_LAROUXFO        0x61f0
#define VID_VMPCK1BA        0x61f1
#define VID_FORMATERB       0x61f2
#define VID_WARNINGE        0x61F3
#define VID_VMPCK1BB        0x61f4
#define VID_CHACKI          0x61f5
#define VID_CHACKJ          0x61f6
#define VID_BADTEMPA        0x61f7
#define VID_PAIXDAM         0x61f8
#define VID_VAMPIREP        0x61f9
#define VID_WAZZUFP         0x61FD
#define VID_CAULI           0x61FF
#define VID_CARTMANE        0x6506
#define VID_ZMKO            0x6507
#define VID_LAROUXGG        0x6509
#define VID_LAROUXFA        0x650A
#define VID_CARRIERD        0x650B
#define VID_PASSBOXC        0x650C
#define VID_SETMDD          0x650E
#define VID_FOOTPRINT       0x6513
#define VID_BAUER           0x6514
#define VID_IMPOSTERG       0x6517
#define VID_TWNOARTW        0x6519
#define VID_VMPCK1BE        0x651a
#define VID_WAZZUFQ         0x651d
#define VID_MENTESC         0x651E
#define VID_MDMABK          0x651f
#define VID_NOTTICEK        0x6520
#define VID_SCHUMANNAA      0x6521
#define VID_VMPCK1BF        0x6522
#define VID_COLDAPEC        0X6525
#define VID_NICEDAYAA       0x6527
#define VID_VMPCK1BG        0x652C


//Jan98 Blitz (mostly polymorphic)
#define VID_DISHONORA       0x6200
#define VID_FOURA           0x6201
#define VID_FUTURENOTA      0x6202
#define VID_HUNTERC         0x6203
#define VID_INSIDEOUTA      0x6204
#define VID_JUNKFACEB       0x6205
#define VID_JUNKFACEC       0x6206
#define VID_KILLOKB         0x6207
#define VID_KILLOKC         0x6208
#define VID_MALARIA         0x6209
#define VID_MERCYB          0x620a
#define VID_NAVRHARA        0x620b
#define VID_NJWMDLK1H       0x620c
#define VID_NJ_WMDLK1M      0x620d
#define VID_NOPC            0x620e
#define VID_RANDOMICA       0x620f
#define VID_SLOWA           0x6210
#define VID_SLOWB           0x6211
#define VID_TESTDOT         0x6212
#define VID_UGLYKID         0x6213
#define VID_VICIS           0x6214
#define VID_ZEROA           0x6215
#define VID_JUNKFACE        0x6216
#define VID_SHUFFLEA        0x6217
#define VID_BLEEC           0x6218
#define VID_RATSASSA        0x6219
#define VID_FUNFUN          0x621A
#define VID_ERASERQ         0x621B
#define VID_VHDLBTW         0x621C
#define VID_DMVCK97         0x621D
//Jan98 Blitz from Joe Collection: W97M viruses that Macro Heur missed
#define VID_BISMARKA        0x621E
#define VID_BISMARKB        0x621F
#define VID_BISMARKC        0x6240
#define VID_BISMARKD        0x6241
#define VID_BISMARKE        0x6242
#define VID_CALENDAR        0x6243
// #define VID_CMD             0x6244  Moved to 60fd - available for reuse
#define VID_OPIM            0x6245
#define VID_SPLASH          0x6246
#define VID_TALONK          0x6247
//Jan98 Blitz from Joe Collection: Excel viruses
#define VID_DELTAB          0x6248
#define VID_HITA            0x6249
#define VID_HITB            0x624A
#define VID_HITC            0x624B
#define VID_HITD            0x624C
#define VID_TJORO           0x624D
#define VID_YOHIMBE         0x624E
#define VID_PAIX            0x624F
//VMacro Feb98
#define VID_APPDERQ         0x6250
#define VID_ATTENTIONB      0x6251
#define VID_BREEDERA        0x6252
#define VID_CHAOSC          0x6253
#define VID_CLEANUPA        0x6254
#define VID_CONCEPTBY       0x6255
#define VID_CONCEPTBZ       0x6256
#define VID_CONCEPTCA       0x6257
#define VID_CONCEPTCB       0x6258
#define VID_CONCEPTCE       0x6259
#define VID_DIVINAO         0x625A
#define VID_DRACULAB        0x625B
#define VID_WAZZUDG         0x625C
#define VID_FAKEA           0x625D
#define VID_GRASSADE        0x625E
#define VID_GURREA          0x625F
#define VID_HASSLEB         0x6260
#define VID_INNUENDOA       0x6261
#define VID_KOMPUJ          0x6262
#define VID_HABIRA          0x6263
#define VID_LORAXA          0x6264
#define VID_MINIMALAA       0x6265
#define VID_NEWYEARAFR      0x6266
#define VID_NOPRBR          0x6267
#define VID_NOPSES          0x6268
#define VID_NOPTIT          0x6269
#define VID_NOPRINTB        0x626A
#define VID_NOTTICEC        0x626B
#define VID_PEACEFULA       0x626C
#define VID_ALLIANCEG       0x626D
#define VID_SCA             0x626E
#define VID_SIDORA          0x626F
#define VID_SPELLA          0x6270
#define VID_SWLABSK         0x6271
#define VID_TABULAADE       0x6272
#define VID_TAGUCHIA        0x6273
#define VID_TWNOAG          0x6274
#define VID_UKAB            0x6275
#define VID_UNHASA          0x6276
#define VID_VHDLCTW         0x6277
#define VID_WAZZUDQ         0x6278
#define VID_WAZZUDR         0x6111
#define VID_DIVINAQ         0x6112
#define VID_MDMAAX          0x6124
#define VID_MDMAAY          0x612d
#define VID_CAPEP           0x6138
#define VID_CONCEPTCJ       0x6139
#define VID_STEROIDC        0x613A
//VMacro Mar98
#define VID_APPDERR         0x6279
#define VID_ATTACHA         0x627A
#define VID_COLORSBY        0x627B
#define VID_CONCEPTCG       0x627C
#define VID_COPYCAPA        0x627D
#define VID_DECAFA          0x627E
#define VID_DIVINAP         0x627F
#define VID_DOGGIEF         0x6280
#define VID_DUDEA           0x6281
#define VID_DUSTA           0x6282
#define VID_DZTI            0x6283
#define VID_EASYC           0x6284
#define VID_ELOHIMA         0x6285
//VMacro Apr98
#define VID_GESTA           0x6287
#define VID_GOBLINA         0x6288
#define VID_GSISB           0x6289
#define VID_KILLLUFC        0x628A
#define VID_LERA            0x628B
#define VID_MENTALA         0x628C
#define VID_MERCADOBBR      0x628D
#define VID_NICEDAYV        0x628E
#define VID_NOPUDE          0x628F
#define VID_NOPVDE          0x6290
#define VID_NOPWDE          0x6291
#define VID_NOPXDE          0x6292
#define VID_ONYXADE         0x6293
#define VID_SAFWANB         0x6294
#define VID_THEATREDTW      0x6295
#define VID_THEBMANA        0x6296
#define VID_TRIBUTEA        0x6297
#define VID_TRIBUTEB        0x6298
//VMacro Apr98b
#define VID_TWNOACTW        0x6299
#define VID_TWNOAHTW        0x629A
#define VID_TWNOAITW        0x629B
#define VID_TWNOAKTW        0x629C
#define VID_UKAC            0x629D
#define VID_ULTRAA          0x629E
#define VID_ULTRAB          0x629F
#define VID_UNHASB          0x62A0
#define VID_WAZZU_BL        0x62A1
#define VID_WAZZUDT         0x62A2
#define VID_WAZZUDV         0x62A3
#define VID_WAZZU_DW        0x62A4
#define VID_WAZZUDY         0x62A5
#define VID_WAZZUDZ         0x62A6
#define VID_WAZZUEA         0x62A7
#define VID_WAZZUEB         0x62A8
#define VID_WAZZUEE         0x62A9
#define VID_WAZZUEF         0x62AA
#define VID_WAZZUEG         0x62AB
#define VID_WAZZUEH         0x62AC
#define VID_WAZZUEI         0x62AD
#define VID_WAZZUEJ         0x62AE
#define VID_WAZZUEK         0x62AF
#define VID_WAZZUEL         0x62B0
#define VID_WAZZUEM         0x62B1
//VMacro Apr98c
#define VID_BOXITW          0x62BD
#define VID_BUEROBDE        0x62BE
#define VID_BUEROCDE        0x62BF
#define VID_GENIA           0x62C0
#define VID_GOLDFISHF       0x62C1
#define VID_MELDUNGA        0x62C2
#define VID_NIKNATI         0x62C3
#define VID_NJWMDLK1N       0x62C4
#define VID_NUCLEARAA       0x62C5
#define VID_SWLABSM         0x62C6
#define VID_TAGUCHIB        0x62C7
#define VID_WAZZUEO         0x62C8
//VMacro Apr98d
#define VID_NJWMVCK2D       0x62C9
#define VID_NJWMVCK2E       0x62CA
#define VID_NJWMVCK2F       0x62CB
#define VID_NJWMVCK2G       0x62CC
#define VID_NJWMVCK2H       0x62CD
#define VID_NJWMVCK2IDE     0x62CE
#define VID_NJWMVCK2JDE     0x62CF
#define VID_NJWMVCK2KDE     0x62D0
#define VID_NJWMVCK2LDE     0x62D1
#define VID_NJWMVCK2MDE     0x62D2
#define VID_NJWMVCK2NDE     0x62D3
#define VID_NJWMVCK2ODE     0x62D4
#define VID_NJWMVCK2PDE     0x62D5
#define VID_NJWMVCK2QDE     0x62D6
#define VID_NJWMVCK2RDE     0x62D7
#define VID_NJWMVCK2SDE     0x62D8
#define VID_SCHUMANNHDE     0x62D9
#define VID_SCHUMANNIDE     0x62DA
#define VID_SCHUMANNKDE     0x62DB
#define VID_SCHUMANNLDE     0x62DC
#define VID_SCHUMANNMDE     0x62DD
//VMacro Apr98e
#define VID_CLOCKMDE        0x62DE
#define VID_KOMPUK          0x62DF
#define VID_MDMABD          0x62E0
#define VID_MENTESA         0x62E1
#define VID_PADANIAC        0x62E2
#define VID_SWLABSO         0x62E3
//ZOO Macro May98
#define VID_ANGUS           0x62e7
#define VID_ANTISR1A        0x62e8
#define VID_BLEEA           0x62e9
#define VID_DISCOB          0x62eA
#define VID_DISCOC          0x62eB
#define VID_MDMABE          0x62EC
#define VID_WAZZUEQ         0x62ED
#define VID_APPDERV         0x62EE
#define VID_APPDERW         0x62EF
#define VID_BOOMCDE         0x62F0
#define VID_FEHLERADE       0x62F1
#define VID_KANNA           0x62F2
#define VID_MENSAGEMA       0x62F3
#define VID_NJWMDLK1O       0x62F4
#define VID_PADANIAAIT      0x62F5
#define VID_PADANIABIT      0x62F6
#define VID_PELOA           0x62F7
#define VID_STALLA          0x62F8
#define VID_SWLABSP         0x62F9
//VMacro 052098
#define VID_ALLIANCEH       0x62FB
#define VID_SURABAYAB       0x62FC
#define VID_WAZZUER         0x62FD
//ZOO Macro May98b
#define VID_KLAA            0x62FF
#define VID_ANTISR1B        0x6300
#define VID_BISMARKF        0x6301
#define VID_BOXF            0x6302
#define VID_HATEA           0x6303
#define VID_STEPHA          0x6304
#define VID_STEROIDA        0x6305
#define VID_KILLERA         0x6306
#define VID_EDDSA           0x6307
#define VID_POMPA           0x6308
//ZOO Macro May98c
#define VID_AOS             0x6309
#define VID_CRAZYA          0x630A
#define VID_CRAZYB          0x630B
#define VID_ELBAG           0x630C
#define VID_GAMBLERB        0x630D
#define VID_JACKA           0x630E
#define VID_JACKB           0x630F
#define VID_NIGHTSHADEC     0x6310
#define VID_BOXG            0x6311
#define VID_BOXH            0x6312
#define VID_CIPHER          0x6313
#define VID_LAMEJOKE        0x6314
#define VID_MULTIA          0x6315
#define VID_MULTIB          0x6316
#define VID_MULTIC          0x6317
#define VID_MULTID          0x6318
#define VID_STEROIDD        0x6319
#define VID_STEROIDE        0x631A
#define VID_ACID            0x631B
#define VID_ZMKD            0x631C
#define VID_MULTIE          0x631D
#define VID_DWMVCK1B        0x631E
#define VID_DWMVCK1A        0x631F
#define VID_ERASERR         0x6320
#define VID_ERASERU         0x6321
#define VID_TALONI          0x6322
#define VID_TALONJ          0x6323
#define VID_VAMPIREJ        0x6324
#define VID_VAMPIREN        0x6325
#define VID_VMPCK1A         0x6326
//VESS Excel Collection
#define VID_XLDMVB          0x6327
#define VID_LAROUXBL        0x6328
#define VID_LAROUXAD        0x6329
#define VID_LAROUXAG        0x632A
#define VID_LAROUXAI        0x632B
#define VID_LAROUXAO        0x632C
#define VID_LAROUXAR        0x632D
#define VID_LAROUXAS        0x632E
#define VID_LAROUXAT        0x632F
#define VID_LAROUXAW        0x6330
#define VID_LAROUXAY        0x6332
#define VID_LAROUXBD        0x6334
#define VID_LAROUXBE        0x6335
#define VID_LAROUXBF        0x6336
#define VID_LAROUXBG        0x6337
#define VID_LAROUXBI        0x6339
#define VID_LAROUXBK        0x633A
#define VID_LAROUXBM        0x633B
#define VID_LAROUXBT        0x633C
#define VID_LAROUXBW        0x633D
#define VID_LAROUXC         0x633E
#define VID_LAROUXCB        0x633F
#define VID_LAROUXCE        0x6340
#define VID_LAROUXCG        0x6341
#define VID_LAROUXM         0x6342
#define VID_LAROUXCS        0x6343
#define VID_LAROUXCT        0x6344
#define VID_LAROUXCW2       0x6346
#define VID_LAROUXCY        0x6347
#define VID_LAROUXCZ        0x6348
#define VID_LAROUXDF        0x6349
#define VID_LAROUXDG        0x634A
#define VID_LAROUXH         0x634B
#define VID_LAROUXI         0x634C
#define VID_LAROUXK         0x634D
#define VID_LAROUXBZ        0x634E
#define VID_LAROUXO         0x634F
//ZOO Macro May98d
#define VID_IMPORT          0x6350
#define VID_NEGA            0x6351
#define VID_NEGB            0x6352
#define VID_TEOCATL         0x6353
#define VID_TRASH           0x6354
#define VID_YOHIMBEB        0x6355
//VESS Excel Collection
#define VID_LAROUXT         0x6345
#define VID_LAROUXV         0x6338
#define VID_LAROUXW         0x6333
#define VID_LAROUXY         0x6331
#define VID_LAROUXZ         0x6356
#define VID_NINJAB          0x6357
#define VID_NINJAC          0x6358
#define VID_NINJAD          0x6359
#define VID_DELTAC          0x635A
#define VID_XMEMPERORA      0x635B
#define VID_XMEMPERORB      0x635C
#define VID_EXTRASC         0x635D
#define VID_TEAMA           0x635E
#define VID_TRASHERA        0x635F
#define VID_TRASHERB        0x6360
#define VID_TRASHERC        0x6361
#define VID_TRASHERD        0x6362
#define VID_TRASHERE        0x6363
#define VID_LMVA            0x6364
#define VID_LMVB            0x6365
#define VID_LAROUXBS        0x6366
#define VID_RENAMERA        0x6367
#define VID_RENAMERB        0x6368
#define VID_FORMATAJOKE     0x6369
#define VID_PANICNOWJOKE    0x636A
#define VID_DELTAD          0x636B
#define VID_LAROUXU         0x636C
#define VID_LAROUXX         0x636D
#define VID_LAROUXAH        0x636E
#define VID_LAROUXBY        0x636F
#define VID_LAROUXCC        0x6370
#define VID_LAROUXCL        0x6371
#define VID_LAROUXCM        0x6372
#define VID_LAROUXCQ        0x6373
#define VID_LAROUXCU        0x6374
#define VID_LAROUXDE        0x6375
#define VID_POLICE          0x6376
#define VID_RIOTS           0x6377
//Vess' W97M collection
#define VID_WAZZUDN         0x6379
#define VID_EASTA           0x637A
#define VID_APPDERS2        0x6158
#define VID_APPDERT         0x637B
#define VID_AUTODA          0x637C
#define VID_CHANGE          0x637D
#define VID_CROSSA          0x637E
#define VID_CROSSB          0x637F
#define VID_CROSSC          0x6380
#define VID_CROSSD          0x6381
#define VID_EDDSB           0x6382
#define VID_ERASERT         0x6383
#define VID_HAVIXA          0x6384
#define VID_HAZARDA         0x6385
#define VID_HONTEA          0x6386
#define VID_INDIAA2         0x6387
#define VID_SWSD            0x6388
#define VID_KIDA            0x6389
#define VID_MDPA            0x638A
#define VID_MINIMORPH       0x638B
#define VID_MULTIF          0x638C
#define VID_MULTIG          0x638D
#define VID_NOTTICED        0x638E
#define VID_NOTTICEE        0x638F
#define VID_SATANA          0x6390
#define VID_SPRHIDEA        0x6391
#define VID_ZMKA            0x6392
#define VID_ZMKB            0x6393
#define VID_ZMKC            0x6394
#define VID_ZMKE            0x6395
#define VID_ZMKF            0x6396
#define VID_ZMKG            0x6397
#define VID_ZMKH            0x6398
#define VID_ECHOA           0x6399
#define VID_EVELYNA         0x639A
#define VID_POLYPOSTER      0x639B
//VB wild review
#define VID_DIVINAR         0x62FA
#define VID_ZMKJ            0x639C
//Zoo Macro 0898
#define VID_CLASSA          0x63A6
#define VID_KITTYB          0x63A7
#define VID_NONOA           0x63A8
#define VID_CHACKB          0x63a9
#define VID_CHACKC          0x63aA
#define VID_CHACKD          0x63aB
#define VID_DRAWA           0x63AC
#define VID_EDDSC           0x63AD
#define VID_ERASERV         0x63AE
#define VID_JEDIA           0x63AF
#define VID_JEDIB           0x63B0
//VMacro: XF.Paix.B
#define VID_PAIXB           0x63B1
//VMacro w97m
#define VID_STEROIDF        0x63B2
#define VID_STEROIDG        0x63B3
#define VID_SATANB          0x63B4
#define VID_MBUGA           0x63B5
#define VID_MBUGB           0x63B6
#define VID_MBUGC           0x63B7
#define VID_MBUGD           0x63B8
#define VID_MBUGE           0x63B9
#define VID_MBUGF           0x63BA
#define VID_MBUGG           0x63BB
#define VID_MBUGH           0x63BC
#define VID_MBUGI           0x63BD
#define VID_MINIMALAB       0x63BE
#define VID_PRESIDENTA97    0x63BF
#define VID_ULANGA          0x63C0
#define VID_UNHELPFUL       0x63C1
#define VID_VMPCK1B         0x63C2
#define VID_ZMKI            0x63C3
#define VID_ZMKK            0x63C4
#define VID_ZMKL            0x63C5
#define VID_ZMKM            0x63C6
//ZOO Macro 0898
#define VID_VMPCK1L         0x63C8
#define VID_POLYM           0x63C9
#define VID_REMARK          0x63CA
#define VID_MARBLES         0x63CB
#define VID_ANARCHY5838A    0X63CC
#define VID_BORINGA         0x63CD
#define VID_TROZA           0x63CE
#define VID_TEOCATLB        0x63CF
#define VID_COLORBZ         0x63d0
#define VID_COLORCA         0x63D1
#define VID_CONCEPTCK       0x63D2
#define VID_COUNTERC        0x63D3
#define VID_CRIMINALA       0x63D4
#define VID_CRYPTA          0x63D5
#define VID_EMTB            0x63D6
#define VID_EZLNA           0x63D7
#define VID_MAKRONEC        0x63D8
#define VID_NJWMVCK2U       0x63D9
#define VID_NOVAC           0x63DA
#define VID_RATSE           0x63DB
#define VID_SCHUMANNQ       0x63DC
#define VID_SCHUMANNR       0x63DD
#define VID_SCHUMANNS       0x63DE
#define VID_SWLABSQ         0x63DF
#define VID_SWTA            0x63E0
#define VID_TOTENC          0x63E1
#define VID_VENENOB         0x63E2
#define VID_WAZZUES         0x63E3
#define VID_WAZZUET         0x63E4
#define VID_WAZZUEU         0x63E5
#define VID_THERESEA        0x63E6
#define VID_DISASTER        0x63E7
#define VID_JARING          0x63E8
#define VID_BERNIE          0x63EA
#define VID_CHAOSA          0x63EB
#define VID_VMPCK1CGEN      0x63EC
#define VID_DWMVCK1GEN      0x63ED
#define VID_VMPCK1AGEN      0x63EE
#define VID_CLASSF          0x63EF
#define VID_SHIVERA         0x63F0
#define VID_SHIVERB         0x63F1
#define VID_SHIVERC         0x63F2
#define VID_INTELTESTFILE   0x64a0
#define VID_ABCB            0x63F5
#define VID_APPDERY         0x63F6
#define VID_APPDERZ         0x63F7
#define VID_BONDA           0x63F8
#define VID_CONCEPTCL       0x63F9
#define VID_CONCEPTCM       0x63FA
#define VID_CONCEPTCN       0x63FB
#define VID_CROWA           0x63FC
#define VID_DELMACROA       0x63FD
#define VID_DELWORDA        0x63FE
#define VID_DIRTYA          0x63FF
#define VID_DIVINAS         0x6400
#define VID_DIVINAU         0x6401
#define VID_DIVINAV         0x6402
#define VID_DMVM            0x6403
#define VID_DMVO            0x6404
#define VID_DMVP            0x6405
#define VID_FHDA            0x6406
#define VID_GABIA           0x6407
#define VID_GOBA            0x6408
#define VID_GOLDFISHG       0x6409
#define VID_GOLDFISHH       0x640A
#define VID_HAWKA           0x640B
#define VID_IVANAVARIANT    0x640C
#define VID_IVANAB          0x640D
#define VID_IVANAC          0x640E
#define VID_IVANAD          0x640F
#define VID_KANNBDE         0x6410
#define VID_COUNTTENF       0x6412
#define VID_FERIASA         0x6413
#define VID_KOMPUL          0x6414
#define VID_KOMPUM          0x6415
#define VID_KOMPUN          0x6416
#define VID_LUPITAA         0x6417
#define VID_MATEYA          0x6418
#define VID_MENTALE         0x6419
#define VID_MERCADOCBR      0x641A
#define VID_MILLIA          0x641B
#define VID_MIMINALAC       0x641C
#define VID_MORTALA         0x641D
#define VID_MUNCHC          0x641E
#define VID_NARMOLA         0x641F
#define VID_NEINA           0x6420
#define VID_NICEDAYX        0x6421
#define VID_NICEDAYY        0x6422
#define VID_NJWMDLK1P       0x6423
#define VID_NOPYDE          0x6424
#define VID_NOPRINTC        0x6425
#define VID_NUCLEARAE       0x6426
#define VID_COPYCAPD        0x6427
#define VID_PAROLA          0x6428
#define VID_PRESIDENTA      0x6429
#define VID_QUERYA          0x642A
#define VID_SCHUMANNXDE     0x642B
#define VID_STALLB          0x642C
#define VID_STUPIDATROJAN	0x642D
#define VID_SVETLANAAINT	0x642E
#define VID_SWLABSR         0x642F
#define VID_TMA             0x6430
#define VID_ULTRASC         0x6431
#define VID_UNHASC          0x6432
#define VID_VACAA           0x6433
#define VID_WMEA            0x6434
#define VID_PADANIADIT      0x6435
#define VID_RATSF           0x6436
#define VID_REHENESB        0x6437
#define VID_SIDORB          0x6438
#define VID_SIGA            0x6439
#define VID_TWNOAL          0x643A
#define VID_TWNOAMTW        0x643B
#define VID_TWNOANTW        0x643C
#define VID_VPA             0x643D
#define VID_WAZZUEV         0x643E
#define VID_WAZZUEW         0x643F
#define VID_WAZZUEY         0x6440
#define VID_WAZZUEZ         0x6441
#define VID_WAZZUFA         0x6442
#define VID_WAZZUFB         0x6443
#define VID_WAZZUFC         0x6444
#define VID_WAZZUFE         0x6445
#define VID_WAZZUFG         0x6446
#define VID_WAZZUFI         0x6447
#define VID_BEEPA           0x6448
#define VID_CARRIERA        0x6449
#define VID_CASCA           0x644A
#define VID_INQUISITORA     0x644B
#define VID_CHACKH          0x644C
#define VID_COLAA           0x644D
#define VID_DREAMSA         0x644E
#define VID_DREAMSB         0x644F
#define VID_GIPSYA          0x6450
#define VID_GROOVD          0x6451
#define VID_IISB            0x6452
#define VID_JEDID           0x6453
#define VID_JEDIE           0x6454
#define VID_JEDIF           0x6455
#define VID_LENAA           0x6456
#define VID_LIZARDA         0x6457
#define VID_LULUNGA         0x6458
#define VID_OUTBREAKA       0x6459
#define VID_OUTBREAKB       0x645A
#define VID_RUBBISHA        0x645B
#define VID_RUBBISHB        0x645C
#define VID_RUBBISHC        0x645D
#define VID_VMPCK1M         0x645E
#define VID_SMACA           0x645F
#define VID_STEROIDH        0x6460
#define VID_STEROIDI        0x6461
#define VID_STORMA          0x6462
#define VID_VMPCK1N         0x6463
#define VID_VMPCK1O         0x6464
#define VID_ZMKN            0x6465
#define VID_VMPCK1P         0x6466
#define VID_VMPCK1Q         0x6467
#define VID_VMPCK1R         0x6468
#define VID_VMPCK1S         0x6469
#define VID_IISA            0x646A
#define VID_METAMORPHA      0x646B
#define VID_VMPCK1V         0x646C
#define VID_VMPCK1W         0x646D
#define VID_VMPCK1X         0x646E
#define VID_VMPCK1Y         0x646F
#define VID_VMPCK1Z         0x6470
#define VID_VMPCK1AA        0x6471
#define VID_VMPCK1AB        0x6472
#define VID_VMPCK1AC        0x6473
#define VID_VMPCK1AD        0x6474
#define VID_VMPCK1AE        0x6475
#define VID_VMPCK1AF        0x6476
#define VID_VMPCK1AG        0x6477
#define VID_VMPCK1AH        0x6478
#define VID_VMPCK1AI        0x6479
#define VID_VMPCK1AJ        0x647A
#define VID_VMPCK1AK        0x647B
#define VID_VMPCK1AL        0x647C
#define VID_VMPCK1AM        0x647D
#define VID_VMPCK1AN        0x647E
#define VID_VMPCK1AO        0x647F
#define VID_VMPCK1AP        0x6480
#define VID_VMPCK1AQ        0x6481
#define VID_VMPCK1AR        0x6482
#define VID_VMPCK1AS        0x6483
#define VID_VMPCK1AT        0x6484
#define VID_VMPCK1AU        0x6485
#define VID_VMPCK1AV        0x6486
#define VID_VMPCK1AW        0x6487
#define VID_VMPCK1AX        0x6488
#define VID_VMPCK1AZ        0x6489
#define VID_PASSBOXA        0x648A
#define VID_VMPCK2D         0x648B
#define VID_VMPCK2E         0x648C
#define VID_SICA            0x648D
#define VID_DADOA           0x648E
#define VID_HIDEMODA        0x648F
#define VID_PERFIDA         0x6490
#define VID_SPELLCHECKERA	0x6491
#define VID_CLASSE          0x6492
#define VID_CLASSG          0x6493
#define VID_CLASSK          0x6494
#define VID_CLASSL          0x6495
#define VID_ENCR            0x6496
#define VID_MINIMALI97      0x6497
#define VID_MINIMALN97      0x6498
#define VID_NJWMDLK1H97     0x6499
#define VID_CARRIERB        0x649A
#define VID_COLDAPE         0x649E
#define VID_LMN             0x649F
#define VID_ABERA           0x64A1
#define VID_ALLIANCEI       0x64A2
#define VID_CONCEPTCO       0x64A3
#define VID_CONCEPTCP       0x64A4
#define VID_FRIESA          0x64A5
#define VID_MENTALD         0x64A6
#define VID_NICEDAYZ        0x64A7
#define VID_NOPZDE          0x64A8
#define VID_TANJUNGA        0x64A9
#define VID_TOTENDDE        0x64AA
#define VID_WAZZUFJ         0x64AB
#define VID_WAZZUFL         0x64AC
#define VID_WAZZUFM         0x64AD
#define VID_WAZZUFO         0x64AE
#define VID_ASTIAC          0x64AF
#define VID_VCXA            0x64B0
#define VID_CHACKL          0x64B1
#define VID_GODZILLAA       0x64B2
#define VID_IISC            0x64B3
#define VID_NIGHTSHADED     0x64B4
#define VID_NOTTICEJ        0x64B5
#define VID_VCXB            0x64B6
#define VID_VCXC            0x64B7
#define VID_VMPCK1BC        0x64B8
//VMacro November98
#define VID_ANAR5838A       0x64BA
#define VID_CHIAA           0x64BB
#define VID_CLOCKN          0x64BC
#define VID_CLOCKO          0x64BD
#define VID_CONCEPTCR       0x64BE
#define VID_CONCEPTCS       0x64BF
#define VID_CONCEPTCT       0x64C0
#define VID_CONCEPTCU       0x64C1
#define VID_CONCEPTCV       0x64C2
#define VID_CONCEPTCW       0x64C3
#define VID_CONCEPTCX       0x64C4
#define VID_CONCEPTCY       0x64C5
#define VID_CONCEPTCZ       0x64C6
#define VID_CONCEPTDA       0x64C7
#define VID_CONCEPTDB       0x64C8
#define VID_CONCEPTDCTW     0x64C9
#define VID_FRIENDLYCDE     0x64CA
#define VID_GASC            0x64CB
#define VID_IMPOSTERF       0x64CC
#define VID_MENTALF         0x64CD
#define VID_MENTALG         0x64CE
#define VID_MENTALH         0x64CF
#define VID_NOMEJABR        0x64D0
#define VID_NUCLEARAF       0x64D1
#define VID_ODYSSEUSA       0x64D2
#define VID_SPYB            0x64D3
#define VID_SWLABST         0x64D4
#define VID_TWNOAQTW        0x64D5
#define VID_NJWMDLK1Q       0x64D6
#define VID_XENIXOSC        0x64D7
#define VID_ACTIVEA         0x64D8
#define VID_ACTIVEB         0x64D9
#define VID_ASTIAB          0x64DA
#define VID_ASTIAA          0x64DB
#define VID_BADMACROA       0x64DC
#define VID_BIOKA           0x64DD
#define VID_BREAKA          0x64DE
#define VID_COLDAPEB        0x64DF
#define VID_CONCEPTCQ       0x64E0
#define VID_INDIAB          0x64E1
#define VID_KOMPUO          0x64E2
#define VID_LOUDA           0x64E3
#define VID_MDMABO          0x64E4
#define VID_97MINIMALE      0x64E5
#define VID_97MINIMALL      0x64E6
#define VID_97MINIMALP      0x64E7
#define VID_97MINIMALW      0x64E8
#define VID_97NOPRINTB      0x64E9
#define VID_NOTTICEL        0x64EA
#define VID_NOTTICEM        0x64EB
#define VID_PASSBOXD        0x64EC
#define VID_SCREAMA         0x64ED
#define VID_ACTIVEC         0x64EE
#define VID_NEGC            0x64EF
#define VID_TOTALERA        0x64F0
#define VID_STALLC          0x64F1
#define VID_ETHANA          0x7e44

//Word Macro Virus Family Detection
#define VID_JOHNNY_VARIANT  0x4d14
#define VID_SHOWOFF_FAMILY  0x4d1A
#define VID_SHOWOFF_FAMILY2 0x4d1A
#define VID_BANDUNG_FAMILY  0x4d1B
#define VID_RAPI_FAMILY     0X4d1C
#define VID_RAPI_FAMILY2    0x4d1C
#define VID_RAPI_FAMILY3    0x4d1C
#define VID_RAPI_FAMILY4    0x4d1C
#define VID_COLORS_FAMILY   0x4d1D
#define VID_IRISH_FAMILY    0x4d1E
#define VID_IRISH_FAMILY2   0x4d1E
#define VID_TWOLINES_FAMILY 0x4dBC
#define VID_NICEDAY_FAMILY1 0x4dBD
#define VID_NICEDAY_FAMILY2 0x4dBD
#define VID_CAP_FAMILY      0x4dBE
#define VID_CAP_CORRUPTED   0x4f21 //notice this refers to Cap (Damage)
#define VID_MUCK_FAMILY1    0x4c7A
#define VID_MUCK_FAMILY2    0x4c7A
#define VID_MUCK_FAMILY3    0x4c7A
#define VID_SWITCHER_FAMILY 0x4c7D
#define VID_DNTC_FAMILY     0x4c7F
#define VID_ANAK_FAMILY     0x4cDB
#define VID_HYBRID_FAMILY   0x4cEF
#define VID_GOODNIGHTFAM    0x601D
#define VID_APPDER_FAMILY   0x6078
#define VID_APPDER_FAMILY2  0x6078
#define VID_TEMPLE_FAMILY1  0x607C
#define VID_TEMPLE_FAMILY2  0x607C
#define VID_TEMPLE_FAMILY3  0x607C
#define VID_NIKNAT_FAMILY1  0x6081
#define VID_NIKNAT_FAMILY2  0x6081
#define VID_PAYCHECK_FAMILY 0x6093
#define VID_CONCEPTI_FAMILY 0x6110
#define VID_CONCEPTZ_FAMILY1 0x6286
#define VID_CONCEPTZ_FAMILY2 0x6286

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
#define VID_COPYCAP_VARIANT 0x61FE
#define VID_CHACKVARIANT    0x6508
#define VID_STEROIDVARIANT  0x650d

//Word Macro Virus Trash Dummy Detection
#define VID_WM_TRASH1       0x0013
#define VID_WM_TRASH2       0x0014

// Dumb Laroux Samples
#define VID_LAROUX_DUMB     0x1fa9

// Excel macro viruses
#define VID_LAROUX          0x1fa9
#define VID_LAROUXB         0x1e0a
#define VID_LAROUXD         0x4e7f
#define VID_LAROUXE         0x4c09
#define VID_LAROUXF         0x4ed5
#define VID_LAROUXG         0x611F
#define VID_LAROUXJ         0x60C6
#define VID_LAROUXQ         0x4c81
#define VID_LAROUXAA        0x6014
#define VID_LAROUXAB        0x606E
#define VID_LAROUXAC        0x6070
#define VID_LAROUXAE        0x60E0
#define VID_LAROUXAF        0x60e7
#define VID_LAROUXAJ        0x60C9
#define VID_LAROUXAK        0x6077
#define VID_LAROUXAL        0x60C5
#define VID_LAROUXAM        0x6106
#define VID_LAROUXAN        0x6132
#define VID_LAROUXAP        0x610e
#define VID_LAROUXAQ        0x6119
#define VID_LAROUXAU        0x60C4
#define VID_LAROUXAV        0x6116
#define VID_LAROUXAZ        0x612C
#define VID_LAROUXBA        0x60CA
#define VID_LAROUXBB        0x60e1
#define VID_LAROUXBJ        0x60f8
#define VID_LAROUXBO        0x612F
#define VID_LAROUXBP        0x610A
#define VID_LAROUXBQ        0x6104
#define VID_LAROUXBU        0x6113
#define VID_LAROUXBV        0x60ee
#define VID_LAROUXBX        0x611b
#define VID_LAROUXCA        0x62B2
#define VID_LAROUXCD        0x6120
#define VID_LAROUXCF        0x6137
#define VID_LAROUXCI        0x611c
#define VID_LAROUXCJ        0x611e
#define VID_LAROUXCN        0x612E
#define VID_LAROUXCO        0x613B
#define VID_LAROUXCP        0x6136
#define VID_LAROUXCV        0x6145
#define VID_LAROUXDAM       0x6147
#define VID_LAROUXCK        0x6150
#define VID_LAROUXL         0x6156
#define VID_JUNIORA         0x615b
#define VID_LAROUXDC        0x615c
#define VID_LAROUXCX        0x615D
#define VID_LAROUXDE2       0x615e
#define VID_LAROUXS         0x616d
#define VID_LAROUXDK        0x6174
#define VID_LAROUXDL        0x6175
#define VID_TABEJA          0x617b
#define VID_LAROUXDP        0x617F
#define VID_TABEJB          0x6180
#define VID_LAROUXDQ        0x6184
#define VID_LAROUXDR        0x6186
#define VID_LAROUXDS        0x618c
#define VID_EXTRASF         0x618e
#define VID_TABEJC          0x6192
#define VID_JEDIC           0x6193
#define VID_DMVL            0x6194
#define VID_VAMPIREO        0x6195
#define VID_LAROUXDT        0x6197
#define VID_LAROUXDU        0x619b
#define VID_LAROUXDV        0x619d
#define VID_LAROUXEI        0x619e
#define VID_FORECAST        0x61FC
#define VID_LAROUXGA        0x6500
#define VID_VMPCK1BD        0x6501
#define VID_LAROUXEZ        0x6502
#define VID_PROTECEDA       0x6503
#define VID_DWMVCK1C        0x6504
#define VID_LAROUXFG        0x6524
#define VID_LAROUXHK        0x6529
#define VID_LAROUXHN        0x652A
#define VID_LAROUXFW        0x652B
#define VID_LAROUXFX        0x652d
#define VID_LAROUXFY        0x652e
#define VID_SUGARB          0x6532
#define VID_SUGARBPAYLOAD   0x6533

#define VID_SOFA            0x1e0f
#define VID_DELTA           0x1e1a
#define VID_XLDMV           0x1548
#define VID_LEGEND          0x4f2c
#define VID_ROBOCOPA        0x4f2f
#define VID_LMVC            0x4eCD
#define VID_NINJAA          0x607F
#define VID_DON             0x60fa //this is also defined as 624F during blitz
#define VID_LMVD            0x60fb
#define VID_SWA             0x6141
#define VID_EXTRASA         0x6135
#define VID_EXTRASB         0x614B
#define VID_EXTRASD         0x6163

//Excel Variant Signature
#define VID_LAROUXA_VAR1    0x62B3
#define VID_LAROUXA_VAR2    0x62B4
#define VID_LAROUXA_VAR3    0x62B5
#define VID_LAROUXE_VAR1    0x62B6
#define VID_LAROUXE_VAR2    0x62B7
#define VID_LAROUXE_VAR3    0x62B8
#define VID_LAROUXD_VAR1    0x62B9
#define VID_LAROUXM_VAR1    0x62BA
#define VID_LAROUXDX_VAR1   0x652F
#define VID_LAROUXBO_VAR1   0x6530
#define VID_LAROUXEO_VAR1   0x6531
#define VID_EXTRAS_VAR      0x62fe

//Algorithmic signatures
#define VID_COMPATA         0x618d


// Macro Heuristics

#define VID_MACRO_HEURISTICS    0x4A06
#define VID_W7H2                0xFFFF
#define VID_XH                  0x4A0B

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

// Access 97 viruses

#define VID_DUMBSAMPLE      0x62BB
#define VID_ACCESSIVA       0x62BB
#define VID_ACCESSIVB       0x62BC
#define VID_ACCESSIVC       0x63E9

#define VID_TOXA            0x62E5
#define VID_TOXB            0x62E6
#define VID_TOXD            0x64B9

#define VID_AMGA            0x7e42

// Access 2.0 viruses

#define VID_A2_ACCESSIVA    0x62E4

// PowerPoint 97 viruses

#define VID_VICA            0x651B

#define VID_UNUSED          0xFFFF

#endif // #ifndef _MACROVID_H_
