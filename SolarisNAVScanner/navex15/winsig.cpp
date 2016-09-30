//************************************************************************
//
// $Header:   S:/NAVEX/VCS/WINSIG.CPv   1.92   17 Dec 1998 14:54:22   MMAGEE  $
//
// Description:
//      Definitions for Windows viruses.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/WINSIG.CPv  $
// 
//    Rev 1.92   17 Dec 1998 14:54:22   MMAGEE
// removed #ifdef/endif for SARC_CUST_9029;  W95.K32;  vid 0x25d1
// 
//    Rev 1.91   09 Dec 1998 14:33:10   DKESSNE
// removed #ifdefs for Zoo viruses, and added W95.K32
// 
//    Rev 1.90   09 Dec 1998 12:52:16   MMAGEE
// removed #ifdef/endif for SARC_CUST_7557
// 
//    Rev 1.89   19 Nov 1998 15:28:00   DKESSNE
// added W95.Lud.Yel 0x25c1 and W95.Lud.Hill 0x25c2
// 
//    Rev 1.88   19 Nov 1998 12:32:56   DKESSNE
// added W95.Shaitan 0x25c0
// 
//    Rev 1.87   17 Nov 1998 15:01:52   DKESSNE
// added signature for W95.Padania, CI 7557
// 
//    Rev 1.86   17 Nov 1998 09:59:48   AOONWAL
// Removed #ifdef SARC_CUST_6689
// 
//    Rev 1.85   12 Nov 1998 19:05:56   DKESSNE
// removed W95.HPS sig for CI 6689
// 
//    Rev 1.84   20 Oct 1998 17:55:36   DKESSNE
// added W95.Evil 0x25b8
// 
//    Rev 1.83   14 Oct 1998 17:16:58   DKESSNE
// added W95.IKX
// 
//    Rev 1.82   14 Oct 1998 10:46:32   DKESSNE
// removed ifdefs for Zoo defs
// 
//    Rev 1.81   09 Oct 1998 12:56:56   DKESSNE
// added signature for Olivia.3378 PE infections (0x25a3)
// 
//    Rev 1.80   08 Oct 1998 17:08:26   DKESSNE
// added sigs for Win.VfW.988 and Win.Vecna.832, and added SCAN_LAST_SEG
// signature for Homer.A, using the same string.
// 
//    Rev 1.79   08 Oct 1998 12:33:42   DKESSNE
// changed a byte in W95.Yurn to wildcard
// 
//    Rev 1.78   08 Oct 1998 11:34:06   DKESSNE
// added W95.Harry.B 0x2599
// 
//    Rev 1.77   15 Sep 1998 01:37:16   JWILBER
// Changed some constants in RedTeam sig to #defines for easy use
// by the detection sig.
//
//    Rev 1.76   09 Sep 1998 15:02:56   MMAGEE
// removed ifdef/endif for sarc_cust_markj
//
//    Rev 1.75   01 Sep 1998 13:07:48   DKESSNE
// added sig for W95.MarkJ
//
//    Rev 1.74   27 Aug 1998 14:10:04   JWILBER
// Removed SARC_ZOO #ifdefs.
//
//    Rev 1.73   21 Aug 1998 11:24:54   DKESSNE
// changed one byte in gpwPECIHSig to Wildcard so that we can get the
// CIH.TNN variant
//
//    Rev 1.72   20 Aug 1998 18:04:12   DKESSNE
// added W95.Punch
//
//    Rev 1.71   19 Aug 1998 23:17:02   jwilber
// Fixed sig for Anxiety.E, VID 61ad.
//
//    Rev 1.70   19 Aug 1998 21:32:08   jwilber
// Added Windows sigs for Anxiety.E, VID 61ad, and Apparition.W95.B,
// VID 61ae.
//
//    Rev 1.69   19 Aug 1998 13:04:08   CNACHEN
// updated cabanas.e sig to search a larger area.
//
//    Rev 1.68   19 Aug 1998 12:46:16   CNACHEN
// Updated windows engine to scan last section of PE files, added
// W95.Cabanas.E
//
//    Rev 1.67   18 Aug 1998 13:23:40   DKESSNE
// added W95.Zombie 0x2570
//
//    Rev 1.66   17 Aug 1998 22:07:38   jwilber
// Removed SARC_CST_WIN1, since we're shipping the stuff that's been
// #ifdef'd with it.  Added zoo def sigs:
// Apparition.C 61a6
// Apparition.D 61a7
// Apparition.A 61a8
// Skim.1454    61a9
//
//    Rev 1.65   17 Aug 1998 17:19:52   DKESSNE
// added w95.Harry 0x256f
//
//    Rev 1.64   17 Aug 1998 11:40:36   DKESSNE
// added strings for W95.DarkSide and W95.SGWW
//
//    Rev 1.63   03 Aug 1998 12:00:06   PDEBATS
// Removed sarc_cst_pecih references for if/endif
//
//    Rev 1.62   07 Jul 1998 16:09:58   AOONWAL
// commented out ifdef SARC_CST_PECIH
//
//    Rev 1.61   26 Jun 1998 13:21:22   DKESSNE
// changed reference to VID_PECIH to VID_CIH
//
//    Rev 1.60   26 Jun 1998 12:33:04   jwilber
// Added sig for PE_CIH, vid 671e.
//
//    Rev 1.59   11 Jun 1998 19:34:06   AOONWAL
// Added #define SARC_ZOO_WIN1
// so that cm build will pick up these zoo defs
//
//    Rev 1.58   09 Jun 1998 21:45:38   JWILBER
// Commented out detection for Homer.Payload due to FP.
//
//    Rev 1.57   09 Jun 1998 21:38:48   JWILBER
// Restored Zoo Defs that were added in 1.54 and 1.55:
// 6165 Homer.A
//    6 Homer.Payload
//    7 SemiSoft.60416
//    8 Memorial
//    9 Yurn.1179
//    b Jacky.1107
//
//    Rev 1.56   04 Jun 1998 16:47:44   AOONWAL
// revert back to rev 1.53
//
//    Rev 1.53   18 May 1998 18:09:00   DKESSNE
// added sig for W95.HPS 0x6160 for CI 1946
//
//    Rev 1.52   27 Apr 1998 15:57:30   PDEBATS
// Removed ifdef/endif statements for cst1746 sex98 vid 6149
//
//    Rev 1.51   21 Apr 1998 18:16:34   DKESSNE
// added sig for W95.LoRez 0x2515
//
//    Rev 1.50   18 Apr 1998 14:58:18   JWILBER
// Added sig for Sex 98, VID 6149, for CI 1746.
//
//    Rev 1.49   03 Apr 1998 15:01:58   FBARAJA
// Removed #ifdef SARC_CST_1575
//
//    Rev 1.48   26 Mar 1998 11:42:32   FBARAJA
// Removed #ifdef REDTEAM
//
//    Rev 1.47   25 Mar 1998 23:26:38   JWILBER
// Added sig for Anxiety.Poppy.III.1750, VID 6133, CI 1575.
//
//    Rev 1.46   24 Mar 1998 20:31:48   JWILBER
// Added sig for RedTeam, VID 6130.
//
//    Rev 1.45   12 Mar 1998 15:04:38   RELNITI
// ADD W95.Klunky detection
//
//    Rev 1.44   15 Jan 1998 18:40:40   JWILBER
// Added defs for these Viruses:
// VKit.650   60ef
// VKit.1241  60f0
// Winvik.B   60f1
// Winvir.961 60f2
//
//    Rev 1.43   01 Dec 1997 23:20:42   RELNITI
// ADD Anxiety.II.1600 (VID_ANXIETYPOPPYII)
//
//    Rev 1.42   01 Dec 1997 20:26:50   JWILBER
// No change.
//
//    Rev 1.41   01 Dec 1997 20:24:28   JWILBER
// Added detection for Vicodin.1168, VID 608d for CI 643.
// Added detection for Skim.1455, VID 608e for CI 644.
//
//    Rev 1.40   30 Oct 1997 14:21:24   JWILBER
// Added sig for Poppy.1052, VID 6071, CI 417.
//
//    Rev 1.39   11 Jun 1997 18:15:38   JWILBER
// Added def for Password-Stealing Trojan, 4d1f.
//
//    Rev 1.38   27 May 1997 11:11:34   AOONWAL
// Modified during June 97 build
//
//    Rev JUNE97 19 May 1997 19:53:18   JWILBER
// Added detection for AoTaun Trojan, 4ebd.
//
//    Rev 1.37   26 Dec 1996 15:22:34   AOONWAL
// No change.
//
//    Rev 1.36   04 Dec 1996 19:59:36   AOONWAL
// Modified Wintvpo sig
//
//    Rev 1.35   04 Dec 1996 19:33:52   AOONWAL
// Added sig for Tvpo virus
//
//    Rev 1.34   02 Dec 1996 14:00:24   AOONWAL
// No change.
//
//    Rev 1.33   29 Oct 1996 12:59:20   AOONWAL
// No change.
//
//    Rev 1.32   12 Jun 1996 19:39:36   JWILBER
// Added def for Tentacle II, 1f8d.
//
//    Rev 1.31   11 Jun 1996 16:29:32   SCOURSE
// Removed entry for WinHuge
//
//    Rev 1.30   11 Jun 1996 12:51:28   SCOURSE
// ...really desparate
//
//    Rev 1.29   11 Jun 1996 12:45:06   SCOURSE
// We're starting to get desparate here...
//
//    Rev 1.28   11 Jun 1996 12:41:32   SCOURSE
// Signature length change
//
//    Rev 1.27   11 Jun 1996 12:38:58   SCOURSE
// Signature length change
//
//    Rev 1.26   11 Jun 1996 12:33:50   SCOURSE
// This virus is starting to bug me (Winhuge)
//
//    Rev 1.25   11 Jun 1996 12:12:06   SCOURSE
// Modified entry for WinHuge
//
//    Rev 1.24   11 Jun 1996 12:07:18   SCOURSE
// Shortened entry for WinHuge.  Only bytes 1-12 match.
//
//    Rev 1.23   10 Jun 1996 17:49:24   SCOURSE
// Added entry for WinHuge
//
//    Rev 1.22   10 Jun 1996 17:24:58   SCOURSE
// Removed entry for Winfector
//
//    Rev 1.21   10 Jun 1996 17:17:58   SCOURSE
// Added entry for Winfector
//
//    Rev 1.20   18 Apr 1996 18:36:12   SCOURSE
// Changed two bytes
//
//    Rev 1.19   18 Apr 1996 18:21:30   SCOURSE
// Added entry for Gen1 Tentacle sample
//
//    Rev 1.18   18 Apr 1996 17:45:42   SCOURSE
// WinLamer string contained an extra comma at the end.  Removed it.
//
//    Rev 1.17   18 Apr 1996 17:06:50   SCOURSE
//
//    Rev 1.11   09 Apr 1996 12:35:04   SCOURSE
// Added entry for Tentacle
//
//    Rev 1.10   04 Apr 1996 12:09:18   SCOURSE
// Changed location to where WinLamer points.
//
//    Rev 1.9   26 Mar 1996 16:41:10   SCOURSE
// Added entry for WinLamer
//
//    Rev 1.8   26 Mar 1996 16:12:36   SCOURSE
// Add signature for WinSurf
//
//    Rev 1.7   26 Mar 1996 15:42:04   SCOURSE
// Added entry for WinTiny
//
//    Rev 1.6   15 Feb 1996 16:04:26   SCOURSE
// Commented out #if 0 statements
//
//    Rev 1.5   13 Feb 1996 17:06:50   JSULTON
// Removed #if def for Winvik Windows 3.1 virus
//
//    Rev 1.4   13 Feb 1996 17:03:40   DSHANNON
// No change.
//
//    Rev 1.3   13 Feb 1996 16:54:34   JSULTON
// No change.
//
//    Rev 1.2   13 Feb 1996 11:22:40   JSULTON
// Removed #if def's NE file structure info and for WinVir virus
// information
//
//    Rev 1.1   09 Feb 1996 15:57:34   CNACHEN
// Modification to BOZA.
//
//    Rev 1.0   05 Feb 1996 17:23:36   DCHI
// Initial revision.
//
//************************************************************************

#include "winsig.h"

#include "platform.h"
#include "callback.h"

#include "navexshr.h"

#ifdef SARCBOT
#include "config.h"
#endif

WORD gpwFearSig[] =
{
    0x60, 0x1E, 0x06, 0xB8, 0xFE, 0x51, 0xCD, 0x21,
    0x3D, 0x51, 0xFF, 0x74, 0x6F, 0xB8, 0x0A, 0x00
};

WORD gpwCyberiotSig[] =
{
    0x06, 0x1E, 0x60, 0x8B, 0xEC, 0x81, 0xEC, 0x00,
    0x03, 0xC7, 0x46, 0xFE, 0x01, 0x00, 0x89, 0xBE,
    0x7E, 0xFF, 0xB8, 0x06, 0x00, 0x0E, 0x50, 0x9A
};

WORD gpwTwitchSig[] =
{
    0x9A, 0xFF, 0xFF, 0x00, 0x00, 0x9A, 0xFF, 0xFF,
    0x00, 0x00, 0x55, 0x89, 0xE5, 0xE8, 0x16, 0xFF,
    0xC9, 0x31, 0xC0, 0x9A, 0xFF, 0xFF, 0x00, 0x00
};

WORD gpwWinvikSig[] =
{
    0x50, 0x53, 0x51, 0x52, 0x56, 0x57, 0xE8, 0x00,
    0x00, 0x5B, 0x83, 0xEB, 0x09, 0x55, 0x1E, 0x06,
    0x8C, 0xD0, 0x8E, 0xD8, 0x8E, 0xC0, 0x81, 0xEC
};

WORD gpwWinvirSig[] =
{
    0xFC, 0x06, 0x1E, 0x07, 0xBE, 0x6E, 0x01, 0xBF,
    0x81, 0x01, 0xB9, 0x0D, 0x00, 0xF3, 0xA4, 0xBA,
    0x50, 0x01, 0xB4, 0x1A, 0xCD, 0x21, 0xBA, 0x7B,
    0x01, 0x33, 0xC9, 0xB4, 0x4E, 0xCD, 0x21, 0x72
};

WORD gpwWinTinySig[] =
{
    0x9c, 0x60, 0x1e, 0x06, 0xb8, 0x86, 0x16, 0xcd,
    0x2f, 0x0b, 0xc0, 0x74, 0x09, 0x07, 0x1f, 0x61,
    0x9d, 0xea, 0x00, 0x00, 0xff, 0xff, 0xb8, 0x01
};

WORD gpwWinSurfSig[] =
{
    0xE9, 0x67, 0x02, 0x3D, 0x94, 0x18, 0x75, 0x04,
    0xB9, 0x34, 0x12, 0xCF, 0x60, 0x1E, 0x06, 0x80,
    0xFC, 0x4B, 0x75, 0x03, 0xE8, 0x08, 0x00, 0x07
};

WORD gpwWinLamerSig[] =
{
    0xFC, 0x90, 0x90, 0x90, 0xF3, 0x3A, 0x16, 0x4C,
    0x00, 0x0A, 0x06, 0x2F, 0x00, 0x19, 0xE0, 0x60,
    0x0A, 0xFE, 0x8B, 0x2E, 0x35, 0x00, 0x2B, 0xEE
};

WORD gpwWinTentacl1Sig[] =
{
    0x1E, 0x60, 0x0E, 0x1F, 0x81, 0xEC, 0xB7, 0x00,
    0x8B, 0xEC, 0xB4, 0x1A, 0x8D, 0x56, 0x00, 0x1E,
    0x16, 0x1F, 0xCD, 0x21, 0x1F, 0xBA, 0x23, 0x00
};

WORD gpwWinTentacl2Sig[] =
{
    0xB7, 0xFF, 0x50, 0x9A, 0xB4, 0x1A, 0x8D, 0x56,
    0x00, 0x1E, 0x16, 0x1F, 0xCD, 0x21, 0x1F, 0xBA,
    0x23, 0x00, 0xBB, 0x01, 0x00, 0xE8, 0x20, 0x00
};

WORD gpwWinNewTentSig[] =
{
    0x00, 0x00, 0x70, 0x29, 0x50, 0x1d, 0x70, 0x29,
    0xff, 0xff, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00,
    0xff, 0xff, 0x00, 0x00, 0x03, 0x00, 0x03, 0x01
};
WORD gpwWinTvpoSig[] =
{
    0x60, 0x1E, 0x06, 0xE8, 0x00, 0x00, 0x5E, 0x81,
    0xEE, 0x69, 0x02, 0xB8, 0x7F, 0x18, 0xBB, 0x53,
    0x44, 0xCD, 0x21, 0x81, 0xFB, 0xA1, 0x87, 0x75
};

WORD gpwWinAoTaunSig[] =
{
    0x43, 0x54, 0x54, 0x59, 0x20, 0x4e, 0x55, 0x4c,
    0x00, 0x00, 0x32, 0x61, 0x4b, 0x49, 0xed, 0x37,
    0x6d, 0x37, 0x5b, 0x37, 0x24, 0x21, 0x6a, 0x37,
    0x9a, 0x38, 0x14, 0x00, 0x62, 0x00, 0x0f, 0x00,
    0x40, 0x44, 0x45, 0x4c, 0x54, 0x52, 0x45, 0x45,
    0x20, 0x2d, 0x79, 0x20, 0x43, 0x3a, 0x5c, 0x00
};

WORD gpwWinPWStealSig[] =
{
    0x4b, 0x49, 0xb1, 0x67, 0xa7, 0x62, 0x00, 0x00,
    0x1a, 0x00, 0xfe, 0x37, 0x6d, 0x39, 0x03, 0x35,
    0x20, 0x00, 0x34, 0x38, 0xf0, 0x0a, 0xb0, 0x0e,
    0x6e, 0x4a, 0x1e, 0x00, 0x72, 0x4c, 0x06, 0xc0,
    0x4b, 0x49, 0xb1, 0x67, 0xa7, 0x62, 0x00, 0x00,
    0x1a, 0x00, 0xf8, 0x37, 0x6d, 0x39, 0x03, 0x35
};

WORD gpwWinVicodinSig[] =
{
    0x50, 0x51, 0x53, 0x52, 0x57, 0x56, 0xe8, 0x2d,
    0x00, 0x44, 0x61, 0x6d, 0x6e, 0x2e, 0x50, 0x6f
};

WORD gpwWinSkimSig[] =
{
    0xe9, 0x7e, 0x02, 0x3d, 0x94, 0x18, 0x75, 0x04,
    0xb9, 0x34, 0x12, 0xcf, 0x60, 0x1e, 0x06, 0x80
};

WORD gpwWinVKit650Sig[] =
{
    0x60, 0x1e, 0x06, 0x90, 0x90, 0xeb, 0x0d, 0x8c,
    0xc8, 0x8e, 0xd8, 0x2e, 0xc7, 0x06, 0x03, 0x00,
    0x90, 0x90, 0xeb, 0x1e, 0xb8, 0x86, 0x16, 0xcd,
    0x2f, 0x0b, 0xc0, 0x74, 0x02, 0xeb, 0x42, 0xb8
};

WORD gpwWinVKit1241Sig[] =
{
    0x60, 0x56, 0x57, 0x1e, 0x06, 0x90, 0x90, 0xeb,
    0x0d, 0x8c, 0xc8, 0x8e, 0xd8, 0x2e, 0xc7, 0x06,
    0x05, 0x00, 0x90, 0x90, 0xeb, 0x14, 0xb8, 0x86,
    0x16, 0xcd, 0x2f, 0x0b, 0xc0, 0x74, 0x02, 0xeb
};

WORD gpwWinWinvikBSig[] =
{
    0x9c, 0x50, 0x53, 0x51, 0x52, 0x56, 0x57, 0x55,
    0x1e, 0x06, 0xe8, 0x66, 0x00, 0xe8, 0x00, 0x00,
    0x5e, 0x81, 0xee, 0x10, 0x00, 0x89, 0x36, 0x1f,
    0x00, 0xe8, 0xa9, 0x00, 0xe8, 0xc9, 0x00, 0x75
};

WORD gpwWinWinvir961Sig[] =
{
    0x06, 0x1e, 0x07, 0xfc, 0xbe, 0x48, 0x58, 0x56,
    0x5e, 0x4c, 0x4c, 0x5f, 0x3b, 0xf7, 0x74, 0x04,
    0x2e, 0xcd, 0x19, 0x90, 0x33, 0xd2, 0xb4, 0x47,
    0xbe, 0x99, 0x01, 0xcd, 0x21, 0xcc, 0xb9, 0x0d
};

WORD gpwWinHomerSig[] =
{
    0x60, 0x1e, 0x06, 0xb8, 0x0a, 0x00, 0x8c, 0xcb,
    0xcd, 0x31, 0x8e, 0xd8, 0xb4, 0x51, 0xcd, 0x21,
    0x89, 0x1e, 0x7d, 0x00, 0x89, 0x1e, 0x81, 0x00,
    0x89, 0x1e, 0x85, 0x00, 0x8e, 0xc3, 0x26, 0xa1
};

WORD gpwWinHomerPaySig[] =
{
    0x53, 0x51, 0x06, 0x33, 0xc0, 0x50, 0x9a, 0xff,
    0xff, 0x00, 0x00, 0x58, 0x07, 0x59, 0x5b, 0x9a,
    0xff, 0xff, 0x00, 0x00, 0x0b, 0xc0, 0x75, 0x03,
    0xe9, 0xbe, 0x00, 0x36, 0xc7, 0x06, 0x14, 0x00
};

WORD gpwWinRedTeamSig[] =
{
    0x9c, 0x1e, 0x06, 0x66, 0x60, 0x8b, 0xec, 0xffff,
    0x03, 0xe8, 0x67, 0x09, 0xe8, 0x22, 0x07, 0xe8,
    0x2a, 0x07, 0xeb, 0x05, 0xea, 0xff, 0xff, 0x00,
    0x00, 0xe8, 0x3d, 0x01, 0xe8, 0x1f, 0x03, 0xe8
};

WORD gpwWinApparCSig[] =
{
    0x9a, 0xff, 0xff, 0x00, 0x00, 0x9a, 0xff, 0xff,
    0x00, 0x00, 0x9a, 0xff, 0xff, 0x00, 0x00, 0x55,
    0x89, 0xe5, 0x31, 0xc0, 0x9a, 0xff, 0xff, 0x00,
    0x00, 0xbf, 0x21, 0x54, 0x0e, 0x57, 0x9a, 0xff
};

WORD gpwWinApparDSig[] =
{
    0x9a, 0xff, 0xff, 0x00, 0x00, 0x9a, 0xff, 0xff,
    0x00, 0x00, 0x9a, 0xff, 0xff, 0x00, 0x00, 0x55,
    0x89, 0xe5, 0x31, 0xc0, 0x9a, 0xff, 0xff, 0x00,
    0x00, 0xbf, 0x5f, 0x63, 0x0e, 0x57, 0x9a, 0xff
};

WORD gpwWinApparASig[] =
{
    0x9a, 0xff, 0xff, 0x00, 0x00, 0x9a, 0xff, 0xff,
    0x00, 0x00, 0x9a, 0xff, 0xff, 0x00, 0x00, 0x55,
    0x89, 0xe5, 0x31, 0xc0, 0x9a, 0xff, 0xff, 0x00,
    0x00, 0xbf, 0x0a, 0x4a, 0x0e, 0x57, 0x9a, 0xff
};

WORD gpwWinSkim2Sig[] =         // 2nd byte is 7d, not 7e like Skim
{
    0xe9, 0x7d, 0x02, 0x3d, 0x94, 0x18, 0x75, 0x04,
    0xb9, 0x34, 0x12, 0xcf, 0x60, 0x1e, 0x06, 0x80
};


WORD gpwVfWSig[] =
{
    0xe8, 0x00, 0x00, 0x5d, 0x83, 0xed, 0x03, 0xb9,
    0xf4, 0x03, 0xe8, 0x21, 0x01, 0x73, 0x03, 0xe9,
    0x12, 0x01, 0x8e, 0xdb, 0x1e, 0x06, 0x06, 0xb8,
    0x0a, 0x00, 0x8c, 0xcb, 0xcd, 0x31, 0x73, 0x03
};

WORD gpwVecnaSig[] =
{
    0x60, 0xb8, 0x4f, 0x42, 0xcd, 0x21, 0x3d, 0x4b,
    0x4e, 0x74, 0x08, 0x1e, 0x06, 0x0e, 0xe8, 0x0f,
    0x00, 0x07, 0x1f, 0x61, 0x68, 0xffff, 0xffff, 0xc3
};




WINSIG_T gNESigs[] =
{
    VID_WINVIR,                 // VID
    WINSIG_SCAN_MAIN_EP,        // flags
    0,                          // entry sub const
    0x20,                       // signature length
    gpwWinvirSig,               // signature
    0x06FC,                     // first word of signature
    0x20,                       // Distance to search

    VID_WINVIK,                 // VID
    WINSIG_SCAN_MAIN_EP,        // flags
    0,                          // entry sub const
    0x18,                       // signature length
    gpwWinvikSig,               // signature
    0x5350,                     // first word of signature
    0x18,                       // Distance to search

    VID_PH33R,                  // VID
    WINSIG_SCAN_MAIN_EP,        // flags
    0,                          // entry sub const
    0x10,                       // signature length
    gpwFearSig,                 // signature
    0x1E60,                     // first word of signature
    0x10,                       // Distance to search

    VID_CYBERIOT,               // VID
    WINSIG_SCAN_MAIN_EP,        // flags
    0,                          // entry sub const
    0x18,                       // signature length
    gpwCyberiotSig,             // signature
    0x1E06,                     // first word of signature
    0x18,                       // Distance to search

    VID_TWITCH,                 // VID
    WINSIG_SCAN_MAIN_EP,        // flags
    0,                          // entry sub const
    0x18,                       // signature length
    gpwTwitchSig,               // signature
    0xFF9A,                     // first word of signature
    0x18,                       // Distance to search

    VID_WINTINY,                // VID
    WINSIG_SCAN_MAIN_EP,        // flags
    0,                          // entry sub const
    0x18,                       // signature length
    gpwWinTinySig,              // signature
    0x609C,                     // first word of signature
    0x18,                       // Distance to search

    VID_WINSURF,                // VID
    WINSIG_SCAN_MAIN_EP,        // flags
    0,                          // entry sub const
    0x18,                       // signature length
    gpwWinSurfSig,              // signature
    0x67E9,                     // first word of signature
    0x18,                       // Distance to search

    VID_WINLAMER,               // VID
    WINSIG_SCAN_MAIN_EP,        // flags
    0,                          // entry sub const
    0x18,                       // signature length
    gpwWinLamerSig,             // signature
    0x90FC,                     // first word of signature
    0x18,                       // Distance to search

    VID_WINTENTACL1,            // VID
    WINSIG_SCAN_MAIN_EP,        // flags
    0,                          // entry sub const
    0x18,                       // signature length
    gpwWinTentacl1Sig,          // signature
    0x601E,                     // first word of signature
    0x18,                       // Distance to search

    VID_WINTENTACL2,            // VID
    WINSIG_SCAN_MAIN_EP,        // flags
    0,                          // entry sub const
    0x18,                       // signature length
    gpwWinTentacl2Sig,          // signature
    0xFFB7,                     // first word of signature
    0x18,                       // Distance to search

    VID_WINNEWTENT,             // VID
    WINSIG_SCAN_LAST_SEG,       // flags
    0,                          // entry sub const
    0x18,                       // signature length
    gpwWinNewTentSig,           // signature
    0x0000,                     // first word of signature
    0x18,                       // Distance to search

    VID_WINTVPO,                // VID
    WINSIG_SCAN_MAIN_EP,        // flags
    0,                          // entry sub const
    0x18,                       // signature length
    gpwWinTvpoSig,              // signature
    0x1E60,                     // first word of signature
    0x18,                       // Distance to search

    VID_WINAOTAUN,              // VID
    WINSIG_SCAN_LAST_SEG,       // flags
    0,                          // entry sub const
    0x30,                       // signature length
    gpwWinAoTaunSig,            // signature
    0x5443,                     // first word of signature
    0x74,                       // Distance to search

    VID_WINPWTROJ,              // VID
    WINSIG_SCAN_LAST_SEG,       // flags
    0,                          // entry sub const
    0x30,                       // signature length
    gpwWinPWStealSig,           // signature
    0x494b,                     // first word of signature
    0x31,                       // Distance to search

    VID_VICODIN,                // VID
    WINSIG_SCAN_MAIN_EP,        // flags
    0,                          // entry sub const
    0x10,                       // signature length
    gpwWinVicodinSig,           // signature
    0x5150,                     // first word of signature
    0x11,                       // Distance to search

    VID_SKIM,                   // VID
    WINSIG_SCAN_MAIN_EP,        // flags
    0,                          // entry sub const
    0x10,                       // signature length
    gpwWinSkimSig,              // signature
    0x7ee9,                     // first word of signature
    0x11,                       // Distance to search

    VID_VKIT650,                // VID
    WINSIG_SCAN_LAST_SEG,       // flags
    0,                          // entry sub const
    0x20,                       // signature length
    gpwWinVKit650Sig,           // signature
    0x1e60,                     // first word of signature
    0x21,                       // Distance to search

    VID_VKIT1241,               // VID
    WINSIG_SCAN_LAST_SEG,       // flags
    0,                          // entry sub const
    0x20,                       // signature length
    gpwWinVKit1241Sig,          // signature
    0x5660,                     // first word of signature
    0x21,                       // Distance to search

    VID_WINVIKB,                // VID
    WINSIG_SCAN_MAIN_EP,        // flags
    0,                          // entry sub const
    0x20,                       // signature length
    gpwWinWinvikBSig,           // signature
    0x509c,                     // first word of signature
    0x21,                       // Distance to search

    VID_REDTEAM,                // VID
    WINSIG_SCAN_MAIN_EP,        // flags
    REDTEAMSUBCON,              // entry sub const
    REDTEAMSIGLEN,              // signature length
    gpwWinRedTeamSig,           // signature
    0x1e9c,                     // first word of signature
    0x28,                       // distance to search

    VID_HOMER,                  // VID
    WINSIG_SCAN_MAIN_EP,        // flags
    0,                          // entry sub const
    0x20,                       // signature length
    gpwWinHomerSig,             // signature
    0x1e60,                     // first word of signature
    0x21,                       // Distance to search

    VID_HOMER,                  // VID
    WINSIG_SCAN_LAST_SEG,       // flags
    0,                          // entry sub const
    0x20,                       // signature length
    gpwWinHomerSig,             // signature
    0x1e60,                     // first word of signature
    0x21,                       // Distance to search

//    VID_HOMERPAY,               // VID
//    WINSIG_SCAN_MAIN_EP,        // flags
//    0,                          // entry sub const
//    0x20,                       // signature length
//    gpwWinHomerPaySig,          // signature
//    0x5153,                     // first word of signature
//    0x21,                       // Distance to search

    VID_APPARC,                 // VID
    WINSIG_SCAN_MAIN_EP,        // flags
    0,                          // entry sub const
    0x20,                       // signature length
    gpwWinApparCSig,            // signature
    0xff9a,                     // first word of signature
    0x20,                       // Distance to search

    VID_APPARD,                 // VID
    WINSIG_SCAN_MAIN_EP,        // flags
    0,                          // entry sub const
    0x20,                       // signature length
    gpwWinApparDSig,            // signature
    0xff9a,                     // first word of signature
    0x20,                       // Distance to search

    VID_APPARA,                 // VID
    WINSIG_SCAN_MAIN_EP,        // flags
    0,                          // entry sub const
    0x20,                       // signature length
    gpwWinApparASig,            // signature
    0xff9a,                     // first word of signature
    0x20,                       // Distance to search

    VID_SKIM2,                  // VID
    WINSIG_SCAN_MAIN_EP,        // flags
    0,                          // entry sub const
    0x10,                       // signature length
    gpwWinSkim2Sig,             // signature
    0x7de9,                     // first word of signature
    0x10,                       // Distance to search

    VID_VFW,                    // VID
    WINSIG_SCAN_MAIN_EP,        // flags
    0,                          // entry sub const
    0x20,                       // signature length
    gpwVfWSig,                  // signature
    0x00e8,                     // first word of signature
    0x20,                       // Distance to search

    VID_VECNA,                  // VID
    WINSIG_SCAN_MAIN_EP,        // flags
    0,                          // entry sub const
    0x18,                       // signature length
    gpwVecnaSig,                // signature
    0xb860,                     // first word of signature
    0x18,                       // Distance to search

    VID_WINVIR961,              // VID
    WINSIG_SCAN_MAIN_EP,        // flags
    0,                          // entry sub const
    0x20,                       // signature length
    gpwWinWinvir961Sig,         // signature
    0x1e06,                     // first word of signature
    0x21                        // Distance to search
};

WORD gwNumNESigs = sizeof(gNESigs) / sizeof(WINSIG_T);

WORD gpwBozaSig[] =
{
    0xE8,    0x00,     0x00,    0x00,    0x00,    0x5D,    0x8B,    0xC5,
    0x2D,  0xFFFF,   0xFFFF,  0xFFFF,  0xFFFF,    0x50,    0x81,    0xED,
    0xFFFF,0xFFFF,   0xFFFF,  0xFFFF,    0x8B,    0x85,  0xFFFF,  0xFFFF,
    0xFFFF,0xFFFF,     0x81,    0x38

};

WORD gpwPoppySig[] =
{
    0x66, 0x2b, 0xff, 0xbf, 0x00, 0x10, 0x00, 0xc0,
    0xb8, 0xff, 0x00, 0x00, 0x00, 0xb9, 0xff, 0xff,
    0xff, 0xff, 0xf2, 0xae, 0x8b, 0xd9, 0x0b, 0xc9,
    0x0f, 0x84, 0x80, 0x00, 0x00, 0x00, 0x81, 0xff
};

WORD gpwAnxietyPoppyIISig[] =
{
    0x66, 0x2b, 0xd2, 0xbf, 0x00, 0x10, 0x00, 0xc0,
    0xb8, 0xff, 0x00, 0x00, 0x00, 0xb9, 0xff, 0xff,
    0xff, 0xff, 0xf2, 0xae, 0x8b, 0xd9, 0x0b, 0xc9,
    0x0f, 0x84, 0x80, 0x00, 0x00, 0x00, 0x81, 0xff
};

WORD gpwMrKlunkySig[] =
{
   0x50, 0x60, 0xE8, 0x00, 0x00, 0x00, 0x00, 0x5D,
   0xBF, 0x00, 0x00, 0xF7, 0xBF, 0xB9, 0x00, 0x10,
   0x00, 0x00, 0xB8, 0x50, 0x45, 0x00, 0x00, 0xF2,
   0xAE, 0x0F, 0x85, 0xE3, 0x01, 0x00, 0x00, 0x39
};

WORD gpwAnxietyPoppyIIISig[] =
{                               // Byte #3 is different from PoppyII
    0x66, 0x2b, 0xc9, 0xbf, 0x00, 0x10, 0x00, 0xc0,
    0xb8, 0xff, 0x00, 0x00, 0x00, 0xb9, 0xff, 0xff,
    0xff, 0xff, 0xf2, 0xae, 0x8b, 0xd9, 0x0b, 0xc9,
    0x0f, 0x84, 0x80, 0x00, 0x00, 0x00, 0x81, 0xff
};

WORD gpwSex98Sig[] =
{
    0x55, 0x8b, 0xec, 0x83, 0xc4, 0xf4, 0xe8, 0x51,
    0x8d, 0xfd, 0xff, 0xe8, 0x94, 0xa0, 0xfd, 0xff
};

WORD gpwLoRezSig[] =
{
    0xe8, 0x00, 0x00, 0x00, 0x00, 0x8b, 0x2c, 0x24,
    0x8b, 0xc5, 0x81, 0xed, 0x0d, 0x10, 0x40, 0x00
};

WORD gpwSemiSoftSig[] =
{
    0x55, 0x8b, 0xec, 0x6a, 0xff, 0x68, 0x08, 0xb0,
    0x40, 0x00, 0x68, 0x88, 0x7c, 0x40, 0x00, 0x64,
    0xa1, 0x00, 0x00, 0x00, 0x00, 0x50, 0x64, 0x89,
    0x25, 0x00, 0x00, 0x00, 0x00, 0x83, 0xc4, 0xa8
};

WORD gpwMemorialSig[] =
{
    0xbd, 0x00, 0xd0, 0x40, 0x00, 0xb9, 0x50, 0x05,
    0x00, 0x00, 0x03, 0x8d, 0x29, 0x00, 0x00, 0x00,
    0x8d, 0xb5, 0x2e, 0x00, 0x00, 0x00, 0x8a, 0x85,
    0x2d, 0x00, 0x00, 0x00, 0x90, 0x90, 0x30, 0x06
};

WORD gpwYurnSig[] =
{
    0xbd, 0xc6, 0xffff, 0xffff, 0x00, 0x8b, 0xc5, 0x05,
    0x3a, 0x10, 0x40, 0x00, 0x2d, 0xffff, 0xffff, 0xffff,
    0x00, 0x50, 0x81, 0x7c, 0x24, 0x04, 0x00, 0x00,
    0x00, 0xbf, 0x0f, 0x8c, 0xc6, 0x00, 0x00, 0x00
};

WORD gpwJackySig[] =
{
    0x50, 0x60, 0xe8, 0x4a, 0x01, 0x00, 0x00, 0x5a,
    0xba, 0x5b, 0xc6, 0xd5, 0x3b, 0x99, 0x73, 0xff,
    0xb8, 0x35, 0x2e, 0x95, 0x09, 0x73, 0xa2, 0xc9,
    0x9a, 0x6f, 0x7a, 0x8c, 0xb9, 0x72, 0xa6, 0x5f
};

WORD gpwPECIHSig[] =
{
    0x55, 0x8d, 0x44, 0x24, 0xf8, 0xffff, 0xdb, 0x64,
    0x87, 0x03, 0xe8, 0x00, 0x00, 0x00, 0x00, 0x5b,
    0x8d, 0x4b, 0x42, 0x51, 0x50, 0x50, 0x0f, 0x01
};

WORD gpwHPSSig[] =
{
    0xff, 0xff, 0xff,
    0xe8, 0xffff, 0xff, 0xff, 0xff,
    0xe8, 0xffff, 0xff, 0xff, 0xff,
    0xe8, 0xffff, 0xff, 0xff, 0xff,
    0xe8, 0xffff, 0xff, 0xff, 0xff
};

WORD gpwDarkSideSig[] =
{
    0x60, 0x50, 0xba, 0x5b, 0x05, 0x00, 0x00, 0x03,
    0xd0, 0x8b, 0x02, 0x05, 0x00, 0x00, 0x40, 0x00,
    0x89, 0x44, 0x24, 0x20, 0xe8, 0xc9, 0x03, 0x00,
    0x00, 0x0b, 0xf6, 0x0f, 0x84, 0x94, 0x00, 0x00
};

WORD gpwSGWWSig[] =
{
    0x50, 0x60, 0x83, 0xc4, 0x24, 0xe8, 0x00, 0x00,
    0x00, 0x00, 0x5d, 0x81, 0xed, 0x0a, 0x10, 0x40,
    0x00, 0x8b, 0x85, 0x33, 0x16, 0x40, 0x00, 0x50,
    0x83, 0xec, 0x20, 0xb8, 0x0f, 0x00, 0x00, 0x00
};

WORD gpwHarrySig[] =
{
    0xbf, 0x00, 0x10, 0x00, 0xc0, 0xb8, 0xff, 0x00,
    0x00, 0x00, 0xb9, 0xff, 0xff, 0xff, 0xff, 0xf2,
    0xae, 0x8b, 0xd9, 0x0b, 0xc9, 0x0f, 0x84, 0x80,
    0x00, 0x00, 0x00, 0x81, 0xff, 0x00, 0xc0, 0x00
};

WORD gpwHarryBSig[] =
{
    0xbf, 0x00, 0x10, 0x00, 0xc0, 0x33, 0xc0, 0xb9,
    0xff, 0xff, 0xff, 0xff, 0xf2, 0xae, 0x8b, 0xd9,
    0x0b, 0xc9, 0x74, 0x78, 0x90, 0x90, 0x90, 0x90
};

WORD gpwZombieSig[] =
{
    0xb9, 0x23, 0x00, 0x00, 0x00, 0x8d, 0xa8, 0x00,
    0xf0, 0xbf, 0xff, 0x8d, 0x95, 0x28, 0x10, 0x40,
    0x00, 0xb9, 0x70, 0x04, 0x00, 0x00, 0xf7, 0x1a,
    0x81, 0x32
};

WORD gpwCabanaSig[] =
{
    0x61, 0xeb, 0x17, 0xeb,             0x85, 0x58, 0x56, 0x53,
    0x50, 0xb9, 0xFFFF, 0xFFFF,         0xFFFF, 0xFFFF, 0xe3, 0xf3,
    0x56, 0x53, 0x03, 0x4d,             0xd3, 0xff, 0x11, 0x91,
    0xe3, 0xe9, 0xc2, 0x08,             0x00, 0xb9
};

WORD gpwPunchSig[] =
{
    0xe8, 0x00, 0x00, 0x00, 0x00, 0x5d, 0x81, 0xed,
    0x05, 0x00, 0x00, 0x00, 0x8b, 0x9d, 0xc6, 0x02,
    0x00, 0x00, 0x01, 0x9d, 0xca, 0x02, 0x00, 0x00,
    0x01, 0x9d, 0xce, 0x02, 0x00, 0x00, 0x01, 0x9d
};

WORD gpwAnxietyESig[] =
{
    0x66, 0x2b, 0xdb, 0xbf, 0x00, 0x10, 0x00, 0xc0,
    0xb8, 0xff, 0x00, 0x00, 0x00, 0xb9, 0xff, 0xff,
    0xff, 0xff, 0xf2, 0xae, 0x8b, 0xd9, 0x0b, 0xc9,
    0x0f, 0x84, 0x80, 0x00, 0x00, 0x00, 0x81, 0xff
};

WORD gpwAppar95BSig[] =
{
    0xff, 0xfe, 0xdf, 0x06, 0x00, 0x66, 0x2b, 0xdb,
    0xbf, 0x00, 0x10, 0x00, 0xc0, 0xb8, 0xff, 0x00,
    0x00, 0x00, 0xb9, 0xff, 0xff, 0xff, 0xff, 0xf2,
    0xae, 0x8b, 0xd9, 0x0b, 0xc9, 0x0f, 0x84, 0x80
};

WORD gpwMarkJSig[] =
{
    0x3d, 0x00, 0x00, 0x40, 0x00, 0x75, 0x1b, 0x80,
    0x3d, 0xdf, 0x08, 0x00, 0xc0, 0x00, 0x75, 0x12,
    0x60, 0xe8, 0x00, 0x00, 0x00, 0x00, 0x58, 0x83,
    0xc0, 0x0b, 0x50, 0x68, 0x00, 0x00, 0x00, 0xc0
};

WORD gpwOliviaSig[] =
{
    0x50, 0x55, 0xe8, 0x00, 0x00, 0x00, 0x00, 0x5d,
    0x81, 0xed, 0x9a, 0x01, 0x00, 0x00, 0x6a, 0x00,
    0x8d, 0x85, 0xda, 0x01, 0x00, 0x00, 0x50, 0x8d,
    0x85, 0xf0, 0x01, 0x00, 0x00, 0x50, 0x6a, 0x00
};

WORD gpwIKXSig[] =
{
    0x60, 0xe8, 0xbb, 0x01, 0x00, 0x00, 0x81, 0xec,
    0x74, 0x01, 0x00, 0x00, 0x8b, 0xec, 0x83, 0xad,
    0x84, 0x01, 0x00, 0x00, 0x0b, 0x8b, 0x85, 0x7c,
    0x01, 0x00, 0x00, 0x01, 0x85, 0xa8, 0x01, 0x00
};

WORD gpwEvilSig[] =
{
    0x60, 0x1e, 0x06, 0x0f, 0xa0, 0x0f, 0xa8, 0xe8,
    0x00, 0x00, 0x00, 0x00, 0x5d, 0x81, 0xed, 0x44,
    0x00, 0x43, 0x00, 0x8b, 0x85, 0xf6, 0x03, 0x43
};


WORD gpwPadaniaSig[] =
{
    0x8b, 0x18, 0x81, 0xfb, 0x00, 0x20, 0x00, 0xc0,
    0x72, 0x11, 0x81, 0xfb, 0x00, 0x00, 0x02, 0xc0,
    0x77, 0x09, 0x81, 0x7b, 0x0c, 0x56, 0x4d, 0x4d,
    0x20, 0x74, 0x05, 0x40, 0xe2, 0xe2, 0xeb, 0x28
};


WORD gpwShaitanSig[] =
{
    0xe8, 0x00, 0x00, 0x00, 0x00, 0x5d, 0x8b, 0xdd,
    0x81, 0xed, 0x0a, 0x10, 0x40, 0x00, 0x2b, 0xd8,
    0x83, 0xeb, 0x0a, 0x53, 0xe8, 0x33, 0x07, 0x00,
    0x00, 0x5b, 0x89, 0x9d, 0x27, 0x1a, 0x40, 0x00
};

WORD gpwLudYelSig[] =
{
    0x60, 0xe8, 0x00, 0x00, 0x00, 0x00, 0x5f, 0x81,
    0xef, 0x51, 0x1f, 0x40, 0x00, 0x81, 0xec, 0xbb,
    0x1e, 0x00, 0x00, 0x8b, 0xec, 0xe8, 0x1a, 0x00,
    0x00, 0x00, 0x74, 0x0c, 0xe8, 0xb7, 0x00, 0x00
};

WORD gpwLudHillSig[] =
{
    0xe8, 0x00, 0x00, 0x00, 0x00, 0x5f, 0x81, 0xef,
    0xa4, 0x11, 0x40, 0x00, 0x55, 0x81, 0xec, 0x9f,
    0x06, 0x00, 0x00, 0x8b, 0xec, 0x8d, 0x45, 0x10,
    0x50, 0x8d, 0x87, 0x2a, 0x13, 0x40, 0x00, 0x50
};

WORD gpwK32Sig[] =
{
    0x80, 0x7c, 0x24, 0x03, 0xbf, 0x0f, 0x85, 0xac,
    0x07, 0x00, 0x00, 0x60, 0x55, 0xe8, 0x00, 0x00,
    0x00, 0x00, 0x5d, 0x81, 0xed, 0x30, 0x10, 0x40
};



WINSIG_T gPESigs[] =
{
    VID_BOZA,                   // VID
    WINSIG_SCAN_MAIN_EP,        // flags
    0x00,                       // entry sub const
    0x18,                       // signature length
    gpwBozaSig,                 // signature
    0x00E8,                     // first word of signature
    0x1C,                       // Distance to search

    VID_POPPY,                  // VID
    WINSIG_SCAN_MAIN_EP,        // flags
    0x05,                       // entry sub const
    0x20,                       // signature length
    gpwPoppySig,                // signature
    0x2b66,                     // first word of signature
    0x30,                       // Distance to search

    VID_ANXIETYPOPPYII,         // VID
    WINSIG_SCAN_MAIN_EP,        // flags
    0x05,                       // entry sub const
    0x20,                       // signature length
    gpwAnxietyPoppyIISig,       // signature
    0x2b66,                     // first word of signature
    0x30,                       // Distance to search

    VID_ANXIETYPOPPYIII,        // VID
    WINSIG_SCAN_MAIN_EP,        // flags
    0x05,                       // entry sub const
    0x20,                       // signature length
    gpwAnxietyPoppyIIISig,      // signature
    0x2b66,                     // first word of signature
    0x30,                       // Distance to search

    VID_SEX98,                  // VID
    WINSIG_SCAN_MAIN_EP,        // flags
    0x00,                       // entry sub const
    0x10,                       // signature length
    gpwSex98Sig,                // signature
    0x8b55,                     // first word of signature
    0x12,                       // Distance to search

    VID_KLUNKY,                 // VID
    WINSIG_SCAN_MAIN_EP,        // flags
    0x00,                       // entry sub const
    0x20,                       // signature length
    gpwMrKlunkySig,             // signature
    0x6050,                     // first word of signature
    0x20,                       // Distance to search

    VID_LOREZ,                  // VID
    WINSIG_SCAN_MAIN_EP,        // flags
    0x00,                       // entry sub const
    0x10,                       // signature length
    gpwLoRezSig,                // signature
    0x00e8,                     // first word of signature
    0x10,                       // Distance to search

    VID_SEMISOFT,               // VID
    WINSIG_SCAN_MAIN_EP,        // flags
    0x00,                       // entry sub const
    0x20,                       // signature length
    gpwSemiSoftSig,             // signature
    0x8b55,                     // first word of signature
    0x21,                       // Distance to search

    VID_MEMORIAL,               // VID
    WINSIG_SCAN_MAIN_EP,        // flags
    0x00,                       // entry sub const
    0x20,                       // signature length
    gpwMemorialSig,             // signature
    0x00bd,                     // first word of signature
    0x21,                       // Distance to search

    VID_YURN,                   // VID
    WINSIG_SCAN_MAIN_EP,        // flags
    0x00,                       // entry sub const
    0x20,                       // signature length
    gpwYurnSig,                 // signature
    0xc6bd,                     // first word of signature
    0x21,                       // Distance to search

    VID_JACKY,                  // VID
    WINSIG_SCAN_MAIN_EP,        // flags
    0x00,                       // entry sub const
    0x20,                       // signature length
    gpwJackySig,                // signature
    0x6050,                     // first word of signature
    0x21,                       // Distance to search

    VID_DARKSIDE,               // VID
    WINSIG_SCAN_MAIN_EP,        // flags
    0x00,                       // entry sub const
    0x20,                       // signature length
    gpwDarkSideSig,             // signature
    0x5060,                     // first word of signature
    0x20,                       // Distance to search

    VID_SGWW,                   // VID
    WINSIG_SCAN_MAIN_EP,        // flags
    0x00,                       // entry sub const
    0x20,                       // signature length
    gpwSGWWSig,                 // signature
    0x6050,                     // first word of signature
    0x20,                       // Distance to search

    VID_HARRY,                  // VID
    WINSIG_SCAN_MAIN_EP,        // flags
    0x00,                       // entry sub const
    0x20,                       // signature length
    gpwHarrySig,                // signature
    0x00bf,                     // first word of signature
    0x20,                       // Distance to search

    VID_HARRYB,                 // VID
    WINSIG_SCAN_MAIN_EP,        // flags
    0x00,                       // entry sub const
    0x18,                       // signature length
    gpwHarryBSig,               // signature
    0x00bf,                     // first word of signature
    0x20,                       // Distance to search

    VID_ZOMBIE,                 // VID
    WINSIG_SCAN_MAIN_EP,        // flags
    0x00,                       // entry sub const
    0x1a,                       // signature length
    gpwZombieSig,               // signature
    0x23b9,                     // first word of signature
    0x20,                       // Distance to search

    VID_CABANAS_E,              // Cabanas.E
    WINSIG_SCAN_LAST_SEG,       // flags
    0x00,                       // entry sub const
    30,                         // signature length
    gpwCabanaSig,               // signature
    0xeb61,                     // first word of signature
    0x220,                      // Distance to search

    VID_PUNCH,                  // VID
    WINSIG_SCAN_MAIN_EP,        // flags
    0x00,                       // entry sub const
    0x20,                       // signature length
    gpwPunchSig,                // signature
    0x00e8,                     // first word of signature
    0x20,                       // Distance to search

    VID_ANXIETYE,               // Anxiety.E
    WINSIG_SCAN_MAIN_EP,        // flags
    0x05,                       // entry sub const
    0x20,                       // signature length
    gpwAnxietyESig,             // signature
    0x2b66,                     // first word of signature
    0x26,                       // Distance to search

    VID_APPAR95B,               // Apparition.W95.B
    WINSIG_SCAN_MAIN_EP,        // flags
    0x00,                       // entry sub const
    0x20,                       // signature length
    gpwAppar95BSig,             // signature
    0xfeff,                     // first word of signature
    0x20,                       // Distance to search

    VID_CIH,                    // VID
    WINSIG_SCAN_MAIN_EP,        // flags
    0x00,                       // entry sub const
    0x18,                       // signature length
    gpwPECIHSig,                // signature
    0x8d55,                     // first word of signature
    0x19,                       // Distance to search

    VID_MARKJ,                  // VID
    WINSIG_SCAN_MAIN_EP,        // flags
    0x00,                       // entry sub const
    0x20,                       // signature length
    gpwMarkJSig,                // signature
    0x003d,                     // first word of signature
    0x25,                       // Distance to search

    VID_OLIVIA,                 // VID
    WINSIG_SCAN_MAIN_EP,        // flags
    0x00,                       // entry sub const
    0x20,                       // signature length
    gpwOliviaSig,               // signature
    0x5550,                     // first word of signature
    0x20,                       // Distance to search

    VID_IKX,                    // VID
    WINSIG_SCAN_MAIN_EP,        // flags
    0x00,                       // entry sub const
    0x20,                       // signature length
    gpwIKXSig,                  // signature
    0xe860,                     // first word of signature
    0x28,                       // Distance to search


    VID_PADANIA,                // VID
    WINSIG_SCAN_LAST_SEG_START, // flags
    0x00,                       // entry sub const
    0x20,                       // signature length
    gpwPadaniaSig,              // signature
    0x188b,                     // first word of signature
    0x2a,                       // Distance to search

    VID_SHAITAN,                // VID
    WINSIG_SCAN_MAIN_EP,        // flags
    0x00,                       // entry sub const
    0x20,                       // signature length
    gpwShaitanSig,              // signature
    0x00e8,                     // first word of signature
    0x25,                       // Distance to search

    VID_LUDYEL,                 // VID
    WINSIG_SCAN_MAIN_EP,        // flags
    0x00,                       // entry sub const
    0x20,                       // signature length
    gpwLudYelSig,               // signature
    0xe860,                     // first word of signature
    0x20,                       // Distance to search

    VID_LUDHILL,                // VID
    WINSIG_SCAN_MAIN_EP,        // flags
    0x00,                       // entry sub const
    0x20,                       // signature length
    gpwLudHillSig,               // signature
    0x00e8,                     // first word of signature
    0x20,                       // Distance to search

    VID_K32,                    // VID
    WINSIG_SCAN_MAIN_EP,        // flags
    0x00,                       // entry sub const
    0x18,                       // signature length
    gpwK32Sig,                  // signature
    0x7c80,                     // first word of signature
    0x18,                       // Distance to search

    VID_EVIL,                   // VID
    WINSIG_SCAN_MAIN_EP,        // flags
    0x00,                       // entry sub const
    0x18,                       // signature length
    gpwEvilSig,                 // signature
    0x1e60,                     // first word of signature
    0x200                       // Distance to search

};

WORD gwNumPESigs = sizeof(gPESigs) / sizeof(WINSIG_T);
