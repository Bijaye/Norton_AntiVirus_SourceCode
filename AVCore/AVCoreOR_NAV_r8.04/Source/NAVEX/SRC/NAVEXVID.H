//************************************************************************
//
// $Header:   S:/NAVEX/VCS/navexvid.h_v   1.6   15 May 1997 15:41:26   JWILBER  $
//
// Description:
//      Contains NAVEX non-macro VIDs.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/navexvid.h_v  $
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
#define ID_WEED5850     0x4E2A          // Weed v2   (Weed.5850)


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
#define ID_SAIGON       0x5341          // Boot Repair for Saigon

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
#define VID_HARE_BOOT	    0x1f99
#define VID_MOLOCH          0x1f9a
#define VID_PARASITE1       0x1f9b
#define VID_NOP1            0x1f9c
#define VID_NOPA            0x1f9c
#define VID_MDMADMV         0X1f9d
#define VID_MDMAA           0X1f9d
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
#define VID_POLITE			0x1fa7
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
#define VID_BURONEU1		0x1fb8
#define VID_FISHFOOD        0x1fb9
#define VID_GOLDFISH        0x1fb9
#define VID_XENIXOS1		0x1fba
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
#define VID_TWNOK           0x1e9f
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

#define VID_CE              0x4f76

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
#define VID_WINAOTAUN   0x4ebd

// Windows PE viruses

#define VID_BOZA        0x1f59

// Java viruses

#define VID_JAVATEST    0x1f77
#define VID_JAVATEST2   0x1f78

#endif // #ifndef _NAVEXVID_H_


