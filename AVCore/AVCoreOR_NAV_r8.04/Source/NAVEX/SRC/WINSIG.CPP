//************************************************************************
//
// $Header:   S:/NAVEX/VCS/WINSIG.CPv   1.37   26 Dec 1996 15:22:34   AOONWAL  $
//
// Description:
//      Definitions for Windows viruses.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/WINSIG.CPv  $
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

//#if 0

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

//#endif

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

WINSIG_T gNESigs[] =
{
    VID_WINVIR,                 // VID
	WINSIG_SCAN_MAIN_EP,		// flags
	0,							// entry sub const
	0x20,						// signature length
	gpwWinvirSig,				// signature
	0x06FC,						// first word of signature
    0x20,                       // Distance to search

    VID_WINVIK,                 // VID
	WINSIG_SCAN_MAIN_EP,		// flags
	0,							// entry sub const
	0x18,						// signature length
	gpwWinvikSig,				// signature
	0x5350,						// first word of signature
	0x18,						// Distance to search

//#if 0
	VID_PH33R,					// VID
	WINSIG_SCAN_MAIN_EP,		// flags
	0,							// entry sub const
	0x10,						// signature length
	gpwFearSig,					// signature
	0x1E60,						// first word of signature
	0x10,						// Distance to search

	VID_CYBERIOT,				// VID
	WINSIG_SCAN_MAIN_EP,		// flags
	0,							// entry sub const
	0x18,						// signature length
	gpwCyberiotSig,				// signature
	0x1E06,						// first word of signature
	0x18,						// Distance to search

	VID_TWITCH,					// VID
	WINSIG_SCAN_MAIN_EP,		// flags
	0,							// entry sub const
	0x18,						// signature length
	gpwTwitchSig,				// signature
	0xFF9A,						// first word of signature
	0x18,						// Distance to search

    VID_WINTINY,                // VID
	WINSIG_SCAN_MAIN_EP,		// flags
	0,							// entry sub const
	0x18,						// signature length
    gpwWinTinySig,              // signature
    0x609C,                     // first word of signature
	0x18,						// Distance to search

    VID_WINSURF,                // VID
	WINSIG_SCAN_MAIN_EP,		// flags
	0,							// entry sub const
	0x18,						// signature length
    gpwWinSurfSig,              // signature
    0x67E9,                     // first word of signature
	0x18,						// Distance to search

    VID_WINLAMER,               // VID
	WINSIG_SCAN_MAIN_EP,		// flags
	0,							// entry sub const
	0x18,						// signature length
    gpwWinLamerSig,             // signature
    0x90FC,                     // first word of signature
	0x18,						// Distance to search

    VID_WINTENTACL1,            // VID
	WINSIG_SCAN_MAIN_EP,		// flags
	0,							// entry sub const
	0x18,						// signature length
    gpwWinTentacl1Sig,          // signature
    0x601E,                     // first word of signature
	0x18,						// Distance to search

    VID_WINTENTACL2,            // VID
	WINSIG_SCAN_MAIN_EP,		// flags
	0,							// entry sub const
	0x18,						// signature length
    gpwWinTentacl2Sig,          // signature
    0xFFB7,                     // first word of signature
	0x18,						// Distance to search

    VID_WINNEWTENT,             // VID
	WINSIG_SCAN_LAST_SEG,		// flags
	0,							// entry sub const
	0x18,						// signature length
    gpwWinNewTentSig,           // signature
    0x0000,                     // first word of signature
	0x18,						// Distance to search

    VID_WINTVPO,                // VID
	WINSIG_SCAN_MAIN_EP,		// flags
	0,							// entry sub const
	0x18,						// signature length
    gpwWinTvpoSig,              // signature
    0x1E60,                     // first word of signature
	0x18,						// Distance to search

//#endif

};

WORD gwNumNESigs = sizeof(gNESigs) / sizeof(WINSIG_T);


WORD gpwBozaSig[] =
{
    0xE8,    0x00,     0x00,    0x00,    0x00,    0x5D,    0x8B,    0xC5,
    0x2D,  0xFFFF,   0xFFFF,  0xFFFF,  0xFFFF,    0x50,    0x81,    0xED,
    0xFFFF,0xFFFF,   0xFFFF,  0xFFFF,    0x8B,    0x85,  0xFFFF,  0xFFFF,
    0xFFFF,0xFFFF,     0x81,    0x38

};

WINSIG_T gPESigs[] =
{
	VID_BOZA,					// VID
	WINSIG_SCAN_MAIN_EP, 		// flags
	0x00,						// entry sub const
	0x18,						// signature length
	gpwBozaSig,					// signature
	0x00E8,						// first word of signature
    0x1C,                       // Distance to search

};

WORD gwNumPESigs = sizeof(gPESigs) / sizeof(WINSIG_T);
