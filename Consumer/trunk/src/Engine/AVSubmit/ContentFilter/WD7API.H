////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

//************************************************************************
//
// $Header:   S:/INCLUDE/VCS/wd7api.h_v   1.9   21 Sep 1998 16:09:50   DCHI  $
//
// Description:
//  Header file for Word 6.0/7.0/95 document access API
//
//************************************************************************
// $Log:   S:/INCLUDE/VCS/wd7api.h_v  $
// 
//    Rev 1.9   21 Sep 1998 16:09:50   DCHI
// Added WD7GetMCDInfo() prototype.
// 
//    Rev 1.8   26 Mar 1998 18:33:56   DCHI
// Added WD7FIB_T structure definition, WD7ClearDocument() prototype,
// and WD7EncryptedWriteByte() prototype.
// 
//    Rev 1.7   20 Nov 1997 16:04:24   ECHIEN
// Added WD7DeleteDelta() prototype.
// 
//    Rev 1.6   16 Sep 1997 14:54:30   DCHI
// Added prototype for WD7SetTDTSize().
// 
//    Rev 1.5   13 Jun 1997 12:46:44   DCHI
// Added CRC function.
// 
//    Rev 1.4   09 Jun 1997 14:10:10   DCHI
// Reworked pack pragmas to work on UNIX gcc which doesn't replace tokens
// within pragmas.
// 
//    Rev 1.3   02 Jun 1997 17:33:12   DCHI
// Added prototype for WD7GetULMacroDescAtIndex().
// 
//    Rev 1.2   30 May 1997 11:38:10   DCHI
// Added WD7SetMacroNameAtIndex(), WD7GetMacroInfoAtULName(),
// WD7GetMacroInfoAtULName(), and WD7SetULMacroNameAtIndex().
// 
//    Rev 1.1   13 May 1997 17:27:00   DCHI
// Added WD7HasAutoText() function.
// 
//    Rev 1.0   05 May 1997 13:58:10   DCHI
// Initial revision.
// 
//************************************************************************

#ifndef _WD7API_H_

#define _WD7API_H_

#include "olessapi.h"

//////////////////////////////////////////////////////////////////////
// Default return type
//////////////////////////////////////////////////////////////////////

typedef int WD7_STATUS;

#define WD7_STATUS_OK       0
#define WD7_STATUS_ERROR    1


//////////////////////////////////////////////////////////////////////
// WordDocument stream structures
//////////////////////////////////////////////////////////////////////

#define WD7_FLAG_DOT                0x0001
#define WD7_FLAG_GLSY               0x0002
#define WD7_FLAG_COMPLEX            0x0004
#define WD7_FLAG_HAS_PIC            0x0008
#define WD7_FLAG_COUNT_QUICK_SAVES  0x00F0
#define WD7_FLAG_ENCRYPTED          0x0100
#define WD7_FLAG_RESERVED           0x0200
#define WD7_FLAG_READ_ONLY_REC      0x0400
#define WD7_FLAG_WRITE_RESERVATION  0x0800
#define WD7_FLAG_EXT_CHAR           0x1000
#define WD7_FLAG_UNUSED             0xE000

#if defined(SYM_WIN32) || defined(SYM_NTK) || defined(SYM_VXD) || defined(SYM_400)
#include "pshpack1.h"
#elif defined(__MACINTOSH__) || defined(SYM_AIX) 
#pragma options align=packed
#else
#pragma pack(1)
#endif

// File Information Block structure

typedef struct tagWD7_FIB_T
{                                           // Offset
    short               shIdent;            //   0   0
    short               shFIBVersion;       //   2   2
    short               shProductVersion;   //   4   4
    short               shLanguageStamp;    //   6   6
    short               shNext;             //   8   8
    WORD                wFlags;             //  10   A
    WORD                shFIBBackVersion;   //  12   C
    long                lKey;               //  14   E
    BYTE                byEnvironment;      //  18  12
    BYTE                byReserved0;        //  19  13
    WORD                wCHSE;              //  20  14
    WORD                wCHSETables;        //  22  16
    DWORD               fcMin;              //  24  18
    DWORD               fcMac;              //  28  1C
    DWORD               dwCBMac;            //  32  20
    DWORD               fcSpare0;           //  36  24
    DWORD               fcSpare1;           //  40  28
    DWORD               fcSpare2;           //  44  2C
    DWORD               fcSpare3;           //  48  30
    DWORD               dwCCPText;          //  52  34
    DWORD               dwCCPFtn;           //  56  38
    DWORD               dwCCPHdd;           //  60  3C
    DWORD               dwCCPMcr;           //  64  40
    DWORD               dwCCPAtn;           //  68  44
    DWORD               dwCCPEdn;           //  72  48
    DWORD               dwCCPTxbx;          //  76  4C
    DWORD               dwCCPHdrTxbx;       //  80  50
    DWORD               dwCCPSpare2;        //  84  54
    DWORD               fcStshfOrig;        //  88  58
    DWORD               lcbStshfOrig;       //  92  5C
    DWORD               fcStshf;            //  96  60
    DWORD               lcbStshf;           // 100  64
    DWORD               fcPlcffndRef;       // 104  68
    DWORD               lcbPlcffndRef;      // 108  6C
    DWORD               fcPlcffndTxt;       // 112  70
    DWORD               lcbPlcffndTxt;      // 116  74
    DWORD               fcPlcfandRef;       // 120  78
    DWORD               lcbPlcfandRef;      // 124  7C
    DWORD               fcPlcfandTxt;       // 128  80
    DWORD               lcbPlcfandTxt;      // 132  84
    DWORD               fcPlcfsed;          // 136  88
    DWORD               lcbPlcfsed;         // 140  8C
    DWORD               fcPlcfpad;          // 144  90
    DWORD               lcbPlcfpad;         // 148  94
    DWORD               fcPlcfphe;          // 152  98
    DWORD               lcbPlcfphe;         // 156  9C
    DWORD               fcSttbfglsy;        // 160  A0
    DWORD               lcbSttbfglsy;       // 164  A4
    DWORD               fcPlcfglsy;         // 168  A8
    DWORD               lcbPlcfglsy;        // 172  AC
    DWORD               fcPlcfhdd;          // 176  B0
    DWORD               lcbPlcfhdd;         // 180  B4
    DWORD               fcPlcfbteChpx;      // 184  B8
    DWORD               lcbPlcfbteChpx;     // 188  BC
    DWORD               fcPlcfbtePapx;      // 192  C0
    DWORD               lcbPlcfbtePapx;     // 196  C4
    DWORD               fcPlcfsea;          // 200  C8
    DWORD               lcbPlcfsea;         // 204  CC
    DWORD               fcSttbfffn;         // 208  D0
    DWORD               lcbSttbfffn;        // 212  D4
    DWORD               fcPlcffldMom;       // 216  D8
    DWORD               lcbPlcffldMom;      // 220  DC
    DWORD               fcPlcffldHdr;       // 224  E0
    DWORD               lcbPlcffldHdr;      // 228  E4
    DWORD               fcPlcffldFtn;       // 232  E8
    DWORD               lcbPlcffldFtn;      // 236  EC
    DWORD               fcPlcffldAtn;       // 240  F0
    DWORD               lcbPlcffldAtn;      // 244  F4
    DWORD               fcPlcffldMcr;       // 248  F8
    DWORD               lcbPlcffldMcr;      // 252  FC
    DWORD               fcSttbfbkmk;        // 256 100
    DWORD               lcbSttbfbkmk;       // 260 104
    DWORD               fcPlcfbkf;          // 264 108
    DWORD               lcbPlcfbkf;         // 268 10C
    DWORD               fcPlcfbkl;          // 272 110
    DWORD               lcbPlcfbkl;         // 276 114
    DWORD               fcCmds;             // 280 118
    DWORD               lcbCmds;            // 284 11C
} WD7_FIB_T, FAR *LPWD7_FIB;

////////////////////////////////////////////////////////////
// Complete Word 6.0/95 FIB structure

#define WD7FIB_FLAGS0_DOT               0x01
#define WD7FIB_FLAGS0_GLSY              0x02
#define WD7FIB_FLAGS0_COMPLEX           0x04
#define WD7FIB_FLAGS0_HAS_PIC           0x08
#define WD7FIB_FLAGS0_QUICK_SAVES_MASK  0xF0

#define WD7FIB_FLAGS1_ENCRYPTED         0x01
#define WD7FIB_FLAGS1_READ_ONLY_REC     0x04
#define WD7FIB_FLAGS1_WRITE_RESERVATION 0x08
#define WD7FIB_FLAGS1_EXT_CHAR          0x10

typedef struct tagWD7FIB
{
    WORD        wIdent;                 //   0   0: Magic number
    WORD        wFIB;                   //   2   2: FIB version
    WORD        wProduct;               //   4   4: Product version
    WORD        wLID;                   //   6   6: Language stamp
    WORD        wPN;                    //   8   8:
    BYTE        byFlags0;               //  10   A:
    BYTE        byFlags1;               //  11   B:
    WORD        wFIBBack;               //  12   C:
    DWORD       dwKey;                  //  14   E: File encrypted key
    BYTE        byEnvr;                 //  18  12: Creation environment
    BYTE        byReserved0;            //  19  13:
    WORD        wChSe;                  //  20  14: Default ext text char set
    WORD        wChSeTables;            //  22  16: Default ext data char set
    DWORD       dwFCMin;                //  24  18: First char file offset
    DWORD       dwFCMac;                //  28  1C: Last char file offset
    DWORD       dwCBMac;                //  32  20: Last byte file offset
    DWORD       dwFCSpare0;             //  36  24:
    DWORD       dwFCSpare1;             //  40  28:
    DWORD       dwFCSpare2;             //  44  2C:
    DWORD       dwFCSpare3;             //  48  30:
    DWORD       dwCCPText;              //  52  34: Length of main doc text
    DWORD       dwCCPFtn;               //  56  38: Length of footnote text
    DWORD       dwCCPHdd;               //  60  3C: Length of header text
    DWORD       dwCCPMcr;               //  64  40: Length of macro text
    DWORD       dwCCPAtn;               //  68  44: Length of annotation text
    DWORD       dwCCPEdn;               //  72  48: Length of endnote text
    DWORD       dwCCPTxbx;              //  76  4C: Length of textbox text
    DWORD       dwCCPHdrTxbx;           //  80  50: Length of hdr textbox text
    DWORD       dwCCPSpare2;            //  84  54:
    DWORD       dwFCStshfOrig;          //  88  58: Orig STSH file offset
    DWORD       dwLCBStshfOrig;         //  92  5C: Orig STSH size
    DWORD       dwFCStshf;              //  96  60: STSH file offset
    DWORD       dwLCBStshf;             // 100  64: STSH size
    DWORD       dwFCPlcffndRef;         // 104  68: Footnote ref PLC offset
    DWORD       dwLCBPlcffndRef;        // 108  6C: Footnote ref PLC size
    DWORD       dwFCPlcffndTxt;         // 112  70: Footnote text PLC offset
    DWORD       dwLCBPlcffndTxt;        // 116  74: Footnote text PLC size
    DWORD       dwFCPlcfandRef;         // 120  78: Annotation ref PLC offset
    DWORD       dwLCBPlcfandRef;        // 124  7C: Annotation ref PLC size
    DWORD       dwFCPlcfandTxt;         // 128  80: Annotation text PLC offset
    DWORD       dwLCBPlcfandTxt;        // 132  84: Annotation text PLC size
    DWORD       dwFCPlcfsed;            // 136  88: Section desc PLC offset
    DWORD       dwLCBPlcfsed;           // 140  8C: Section desc PLC size
    DWORD       dwFCPlcfpad;            // 144  90: Paragraph desc PLC offset
    DWORD       dwLCBPlcfpad;           // 148  94: Paragraph desc PLC size
    DWORD       dwFCPlcfphe;            // 152  98: Paragraph height PLC offset
    DWORD       dwLCBPlcfphe;           // 156  9C: Paragraph height PLC size
    DWORD       dwFCSttbfglsy;          // 160  A0: Glossary str table offset
    DWORD       dwLCBSttbfglsy;         // 164  A4: Glossary str table size
    DWORD       dwFCPlcfglsy;           // 168  A8: Glossary PLC offset
    DWORD       dwLCBPlcfglsy;          // 172  AC: Glossary PLC size
    DWORD       dwFCPlcfhdd;            // 176  B0: Header PLC offset
    DWORD       dwLCBPlcfhdd;           // 180  B4: Header PLC size
    DWORD       dwFCPlcfbteChpx;        // 184  B8: Char prop bin table offset
    DWORD       dwLCBPlcfbteChpx;       // 188  BC: Char prop bin table size
    DWORD       dwFCPlcfbtePapx;        // 192  C0: Para prop bin table offset
    DWORD       dwLCBPlcfbtePapx;       // 196  C4: Para prop bin table size
    DWORD       dwFCPlcfsea;            // 200  C8: Private PLC offset
    DWORD       dwLCBPlcfsea;           // 204  CC: Private PLC size
    DWORD       dwFCSttbfffn;           // 208  D0: Font info table offset
    DWORD       dwLCBSttbfffn;          // 212  D4: Font info table size
    DWORD       dwFCPlcffldMom;         // 216  D8: Main doc field PLC offset
    DWORD       dwLCBPlcffldMom;        // 220  DC: Main doc field PLC size
    DWORD       dwFCPlcffldHdr;         // 224  E0: Header field PLC offset
    DWORD       dwLCBPlcffldHdr;        // 228  E4: Header field PLC size
    DWORD       dwFCPlcffldFtn;         // 232  E8: Footnote field PLC offset
    DWORD       dwLCBPlcffldFtn;        // 236  EC: Footnote field PLC size
    DWORD       dwFCPlcffldAtn;         // 240  F0: Annotation field PLC offset
    DWORD       dwLCBPlcffldAtn;        // 244  F4: Annotation field PLC size
    DWORD       dwFCPlcffldMcr;         // 248  F8: Macro field PLC offset
    DWORD       dwLCBPlcffldMcr;        // 252  FC: Macro field PLC size
    DWORD       dwFCSttbfbkmk;          // 256 100: Bookmark name table offset
    DWORD       dwLCBSttbfbkmk;         // 260 104: Bookmark name table size
    DWORD       dwFCPlcfbkf;            // 264 108: Bookmark begin PLC offset
    DWORD       dwLCBPlcfbkf;           // 268 10C: Bookmark begin PLC size
    DWORD       dwFCPlcfbkl;            // 272 110: Bookmark end PLC offset
    DWORD       dwLCBPlcfbkl;           // 276 114: Bookmark end PLC size
    DWORD       dwFCCmds;               // 280 118: Macro commands offset
    DWORD       dwLCBCmds;              // 284 11C: Macro commands size
    DWORD       dwFCUnused1;            // 288 120:
    DWORD       dwLCBUnused1;           // 292 124:
    DWORD       dwFCUnused2;            // 296 128:
    DWORD       dwLCBUnused2;           // 300 12C:
    DWORD       dwFCPrDrvr;             // 304 130: Printer driver info offset
    DWORD       dwLCBPrDrvr;            // 308 134: Printer driver info size
    DWORD       dwFCPrEnvPort;          // 312 138: Portrait print env offset
    DWORD       dwLCBPrEnvPort;         // 316 13C: Portrait print env size
    DWORD       dwFCPrEnvLand;          // 320 140: Landscape print env offset
    DWORD       dwLCBPrEnvLand;         // 324 144: Landscape print env size
    DWORD       dwFCWss;                // 328 148: Window save state offset
    DWORD       dwLCBWss;               // 332 14C: Window save state size
    DWORD       dwFCDop;                // 336 150: Document property offset
    DWORD       dwLCBDop;               // 340 154: Document property size
    DWORD       dwFCSttbfAssoc;         // 344 158: Assoc str table offset
    DWORD       dwLCBSttbfAssoc;        // 348 15C: Assoc str table size
    DWORD       dwFCClx;                // 352 160: Complex file info offset
    DWORD       dwLCBClx;               // 356 164: Complex file info size
    DWORD       dwFCReserved0;          // 360 168:
    DWORD       dwLCBReserved1;         // 364 16C:
    DWORD       dwFCAutosaveSource;     // 368 170: Orig file name offset
    DWORD       dwLCBAutosaveSource;    // 372 174: Orig file name size
    DWORD       dwFCGrpStAtnOwners;     // 376 178: Annotation owners offset
    DWORD       dwLCBGrpStAtnOwners;    // 380 17C: Annotation owners size
    DWORD       dwFCSttbfAtnbkmk;       // 384 180: Annotation bkmk name offset
    DWORD       dwLCBSttbfAtnbkmk;      // 388 184: Annotation bkmk name size
    WORD        wSpare4Fib;             // 392 188:
    WORD        wPNChpFirst;            // 394 18A: First page w/CHPX FKP info
    WORD        wPNPapFirst;            // 396 18C: First page w/PAPX FKP info
    WORD        wCPNBteChp;             // 398 18E: CHPX FKP count
    WORD        wCPNBtePap;             // 400 190: PAPX FKP count
    DWORD       dwFCPlcfdoaMom;         // 402 192: Drawn object PLC offset
    DWORD       dwLCBPlcfdoaMom;        // 406 196: Drawn object PLC size
    DWORD       dwFCPlcfdoaHdr;         // 410 19A: Hdr drawn object PLC offset
    DWORD       dwLCBPlcfdoaHdr;        // 414 19E: Hdr drawn object PLC size
    DWORD       dwFCUnused3;            // 418 1A2:
    DWORD       dwLCBUnused3;           // 422 1A6:
    DWORD       dwFCUnused4;            // 426 1AA:
    DWORD       dwLCBUnused4;           // 430 1AE:
    DWORD       dwFCPlcfAtnbkf;         // 434 1B2: Annotation BKF PLC offset
    DWORD       dwLCBPlcfAtnbkf;        // 438 1B6: Annotation BKF PLC size
    DWORD       dwFCPlcfAtnbkl;         // 442 1BA: Annotation BKL PLC offset
    DWORD       dwLCBPlcfAtnbkl;        // 446 1BE: Annotation BKL PLC size
    DWORD       dwFCPMS;                // 450 1C2: Print merge state offset
    DWORD       dwLCBPMS;               // 454 1C6: Print merge state size
    DWORD       dwFCFormFldSttbf;       // 458 1CA: Form field table offset
    DWORD       dwLCBFormFldSttbf;      // 462 1CE: Form field table size
    DWORD       dwFCPlcfendRef;         // 466 1D2: Endnote ref PLC offset
    DWORD       dwLCBPlcfendRef;        // 470 1D6: Endnote ref PLC size
    DWORD       dwFCPlcfendTxt;         // 474 1DA: Endnote text PLC offset
    DWORD       dwLCBPlcfendTxt;        // 478 1DE: Endnote text PLC size
    DWORD       dwFCPlcffldEdn;         // 482 1E2: Endnote field PLC offset
    DWORD       dwLCBPlcffldEdn;        // 486 1E6: Endnote field PLC size
    DWORD       dwFCUnused5;            // 490 1EA:
    DWORD       dwLCBUnused5;           // 494 1EE:
    DWORD       dwFCUnused6;            // 498 1F2:
    DWORD       dwLCBUnused6;           // 502 1F6:
    DWORD       dwFCSttbfRMark;         // 506 1FA: Author abbrev table offset
    DWORD       dwLCBSttbfRMark;        // 510 1FE: Author abbrev table size
    DWORD       dwFCSttbfCaption;       // 514 202: Caption titles table offset
    DWORD       dwLCBSttbfCaption;      // 518 206: Caption titles table size
    DWORD       dwFCAutoCaption;        // 522 20A: Auto caption table offset
    DWORD       dwLCBAutoCaption;       // 526 20E: Auto caption table size
    DWORD       dwFCPlcfwkb;            // 530 212: Contrib doc PLC offset
    DWORD       dwLCBPlcfwkb;           // 534 216: Contrib doc PLC size
    DWORD       dwFCUnused7;            // 538 21A:
    DWORD       dwLCBUnused7;           // 542 21E:
    DWORD       dwFCPlcftxbxTxt;        // 546 222: Textbox text PLC offset
    DWORD       dwLCBPlcftxbxTxt;       // 550 226: Textbox text PLC size
    DWORD       dwFCPlcffldTxbx;        // 554 22A: Textbox field PLC offset
    DWORD       dwLCBPlcffldTxbx;       // 558 22E: Textbox field PLC size
    DWORD       dwFCPlcfHdrtxbxTxt;     // 562 232: Header textbox PLC offset
    DWORD       dwLCBPlcfHdrtxbxTxt;    // 566 236: Header textbox PLC size
    DWORD       dwFCPlcffldHdrTxbx;     // 570 23A: Hdr txtbox field PLC offset
    DWORD       dwLCBPlcffldHdrTxbx;    // 574 23E: Hdr txtbox field PLC size
    DWORD       dwFCStwUser;            // 578 242: Macro user storage offset
    DWORD       dwLCBStwUser;           // 582 246: Macro user storage size
    DWORD       dwFCSttbttmbd;          // 586 24A: Embedded TTF table offset
    DWORD       dwLCBSttbttmbd;         // 590 24E: Embedded TTF table size
    DWORD       dwFCUnused8;            // 594 252:
    DWORD       dwLCBUnused8;           // 598 256:
    DWORD       dwFCPgdMotherFCPgd;     // 602 25A: Main text PGD PLC offset
    DWORD       dwFCPgdMotherLCBPgd;    // 606 25E: Main text PGD PLC size
    DWORD       dwFCPgdMotherFCBkd;     // 610 262: Main txt brk PGD PLC offset
    DWORD       dwFCPgdMotherLCBBkd;    // 614 266: Main txt brk PGD PLC size
    DWORD       dwFCPgdFtnFCPgd;        // 618 26A: Footnote PGD PLC offset
    DWORD       dwFCPgdFtnLCBPgd;       // 622 26E: Footnote PGD PLC size
    DWORD       dwFCPgdFtnFCBkd;        // 626 272: Foonote brk PGD PLC offset
    DWORD       dwFCPgdFtnLCBBkd;       // 630 276: Foonote brk PGD PLC size
    DWORD       dwFCPgdEdnFCPgd;        // 634 27A: Endnote PGD PLC offset
    DWORD       dwFCPgdEdnLCBPgd;       // 638 27E: Endnote PGD PLC size
    DWORD       dwFCPgdEdnFCBkd;        // 642 282: Endnote brk PGD PLC offset
    DWORD       dwFCPgdEdnLCBBkd;       // 646 286: Endnote brk PGD PLC size
    DWORD       dwFCSttbfIntlFld;       // 650 28A: Field keyword table offset
    DWORD       dwLCBSttbfIntlFld;      // 654 28E: Field keyword table size
    DWORD       dwFCRouteSlip;          // 658 292: Mailer route slip offset
    DWORD       dwLCBRouteSlip;         // 662 296: Mailer route slip size
    DWORD       dwFCSttbSavedBy;        // 666 29A: Saved by users table offset
    DWORD       dwLCBSttbSavedBy;       // 670 29E: Saved by users table size
    DWORD       dwFCSttbFnm;            // 674 2A2: Filename ref table offset
    DWORD       dwLCBSttbFnm;           // 678 2A6: Filename ref table size
} WD7FIB_T, FAR *LPWD7FIB;

typedef struct tagWD7PCD
{
    BYTE        byFlags0;
    BYTE        byFlags1;
    DWORD       dwFC;
    WORD        wPRM;
} WD7PCD_T, FAR *LPWD7PCD;

typedef struct tagWD7FLD
{
    BYTE        byType;
    BYTE        byData;
} WD7FLD_T, FAR *LPWD7FLD;

typedef struct tagWD7SED
{
    WORD        wFlags;
    DWORD       dwFC;
    WORD        wFNMpr;
    DWORD       dwFCMpr;
} WD7SED_T, FAR *LPWD7SED;

// Macro descriptor structure

typedef struct tagWD7_MCD
{
	BYTE	byCMG;
	BYTE	byEncrypt;	// Encryption byte

	WORD	wID;		// ID in macro name table
	WORD	wULNameIdx;	// Index of name in UL name table
	WORD	wMenuHelp;	// Index of macro description
						// string in UL name table

	DWORD	dwFirst;
	DWORD	dwSize;		// Size of macro in bytes
	DWORD	dwN;  		// Zero for invalid macro
	DWORD	dwOffset;	// Offset of macro in document stream
} WD7_MCD_T, FAR *LPWD7_MCD;

typedef struct tagDELTA
{
    WORD   wInst;         // Instruction
    WORD   wMenu;         // Menu Position or Record0x10 Index
    WORD   wMenuItem;     // Menu Item Record0x10 Index or Menu Record0x10 Index
    WORD   wFunction;     // Function
    WORD   wMacroIndex;   // Macro Record0x11 Index
    WORD   wMenuItemPos;  // Menu Position or Menu Item Position
} DELTA_T, FAR *LPDELTA;

#if defined(SYM_WIN32) || defined(SYM_NTK) || defined(SYM_VXD) || defined(SYM_400)
#include "poppack.h"
#elif defined(__MACINTOSH__) || defined(SYM_AIX) 
#pragma options align=reset
#else
#pragma pack()
#endif

//////////////////////////////////////////////////////////////////////
// Reverse key definitions and structures
//////////////////////////////////////////////////////////////////////

#define WD7_ENC_KEY_LEN     16

typedef struct tagWDREVKEY_T
{
    BYTE            byHashLen;

    // This is used to find the reverse key if the hash fits
    //  Must be sixteen bytes

    BYTE            abyRevKey0[WD7_ENC_KEY_LEN];

    // This is used to verify the key
    //  Must be byHashLen bytes

    BYTE            abyRevKey1[WD7_ENC_KEY_LEN];
} WDREVKEY_T, FAR *LPWDREVKEY, FAR * FAR *LPLPWDREVKEY;

typedef struct tagWDREVKEYRING_T
{
    // Number of reverse keys in this ring

    WORD            wKeyCount;

    // Index of first reverse key pointer in reverse key array

    WORD            wFirstRevKeyIdx;
} WDREVKEYRING_T, FAR *LPWDREVKEYRING;

typedef struct tagWDREVKEYLOCKER_T
{
    // Array of indices into key ring array
    //  If zero, then no such key ring exists

    BYTE            abyRevKeyIdx[256];

    // Pointer to array of pointers to reverse keys

    LPLPWDREVKEY    lpalpstRevKey;

    // Array of reverse key rings.
    //  The key at index I contains reverse keys whose first byte is I

    LPWDREVKEYRING  lpastRing;
} WDREVKEYLOCKER_T, FAR *LPWDREVKEYLOCKER;

//////////////////////////////////////////////////////////////////////
// Key structure
//////////////////////////////////////////////////////////////////////

typedef struct tagWD7ENCKEY_T
{
    BOOL            bEncrypted;

    // If a key is found, the following two should be set

    WORD            wID;

    DWORD           dwReverseKeyOffset;

    // Aligned on 16-byte boundary

    BYTE            abyKey[WD7_ENC_KEY_LEN];
} WD7ENCKEY_T, FAR *LPWD7ENCKEY;

//////////////////////////////////////////////////////////////////////
// Function prototypes
//////////////////////////////////////////////////////////////////////

typedef struct tagWD7_MACRO_SCAN
{
    LPSS_STREAM             lpstStream;
    LPWD7ENCKEY             lpstKey;

    ////////////////////////////////////////////////////////
    // MCD buffering
    ////////////////////////////////////////////////////////

    // Offset in WordDocument stream of first MCD

    DWORD                   dwMCDTableOffset;

    // Global MCD count

    WORD                    wNumMCDs;

    // Global index of the first buffered MCD

    WORD                    wBufferedMCDStartIndex;

    // Local count of number of buffered MCDs

    WORD                    wNumBufferedMCDs;

    // Local index of next MCD to scan

    WORD                    wNextMCDScanIndex;

    // Maximum number of buffered MCDs (must be >= 1)

    WORD                    wMaxBufferedMCDs;

    // Ptr to MCD buffer memory

    LPWD7_MCD               lpstMCDs;

    ////////////////////////////////////////////////////////
    // Macro name buffering
    //  The names are buffered in the same form as they
    //  appear in the WordDocumentStream:
    //      {wID, byLen, abyName[byLen+1]}[wNumMacroNames]
    //  The name is zero-terminated.
    ////////////////////////////////////////////////////////

    // Offset in WordDocument stream of the first name
    //  in the macro name table

    DWORD                   dwMacroNameTableOffset;
    DWORD                   dwMacroNameTableLimit;

    // Global macro name count

    WORD                    wNumMacroNames;

    // Global offset of the first buffered name

    DWORD                   dwFirstBufferedNameOffset;

    // Index of the first buffered name

    WORD                    wFirstBufferedNameIndex;

    // Count of the number of buffered name bytes

    WORD                    wNumBufferedNameBytes;

    // Local index of the name at wNextNameOffset

    WORD                    wNextNameIndex;

    // Offset into buffer of the first name to check on the next
    //  macro to scan.  Everything before this offset must already
    //  have been associated with an already scanned macro.

    WORD                    wNextNameOffset;

    // Size of name buffer

    WORD                    wMaxNameBufferSize;

    // Ptr to name buffer memory

    LPBYTE                  lpbyNames;
} WD7_MACRO_SCAN_T, FAR *LPWD7_MACRO_SCAN, FAR * FAR *LPLPWD7_MACRO_SCAN;


#define WD7_MACRO_TABLE_TYPE_NONE       0x00000000ul

#define WD7_MACRO_TABLE_TYPE_0X01       0x00000001ul
#define WD7_MACRO_TABLE_TYPE_0X02       0x00000002ul
#define WD7_MACRO_TABLE_TYPE_0X03       0x00000004ul
#define WD7_MACRO_TABLE_TYPE_0X04       0x00000008ul
#define WD7_MACRO_TABLE_TYPE_0X05       0x00000010ul
#define WD7_MACRO_TABLE_TYPE_0X06       0x00000020ul
#define WD7_MACRO_TABLE_TYPE_0X10       0x00000040ul
#define WD7_MACRO_TABLE_TYPE_0X11       0x00000080ul
#define WD7_MACRO_TABLE_TYPE_0X12       0x00000100ul
#define WD7_MACRO_TABLE_TYPE_0X40       0x00000200ul

typedef struct tagWD7_TDT_INFO
{
    LPSS_STREAM         lpstStream;
    LPWD7ENCKEY         lpstKey;

    DWORD               dwTDTOffset;
    DWORD               dwTDTSize;

    DWORD               dwMacroTableContents;

    DWORD               dwMacroNameTableOffset;
    WORD                wNumMacroNames;
    DWORD               dwMCDTableOffset;
    WORD                wNumMCDs;
    DWORD               dwULNameTableOffset;
    WORD                wULNameTableSize;
    DWORD               dwMenuDeltaTableOffset;
} WD7_TDT_INFO_T, FAR *LPWD7_TDT_INFO;

BOOL WD7IsKnownMagicNumber
(
    WORD                wMagicNumber
);

WD7_STATUS WD7FindFirstKey
(
    LPSS_STREAM         lpstStream,
    LPWD7ENCKEY         lpstKey,
    LPWDREVKEYLOCKER    lpstLocker
);

WD7_STATUS WD7FindNextKey
(
    LPSS_STREAM         lpstStream,
    LPWD7ENCKEY         lpstKey,
    LPWDREVKEYLOCKER    lpstLocker
);

DWORD WD7EncryptedRead
(
    LPSS_STREAM         lpstStream,
    LPWD7ENCKEY         lpstKey,
    DWORD               dwOffset,
    LPBYTE              lpbyBuffer,
    DWORD               dwBytesToRead
);

DWORD WD7EncryptedWrite
(
    LPSS_STREAM         lpstStream,
    LPWD7ENCKEY         lpstKey,
    DWORD               dwOffset,
    LPBYTE              lpbyBuffer,
    DWORD               dwBytesToWrite
);

DWORD WD7EncryptedWriteByte
(
    LPSS_STREAM         lpstStream,
    LPWD7ENCKEY         lpstKey,
    DWORD               dwOffset,
    BYTE                byByte,
    DWORD               dwBytesToWrite
);

WD7_STATUS WD7AllocMacroScanStruct
(
    LPVOID              lpvRootCookie,
    LPLPWD7_MACRO_SCAN  lplpstMacroScan,
    WORD                wMaxBufferedMCDs,
    WORD                wNameBufferSize
);

WD7_STATUS WD7FreeMacroScanStruct
(
    LPVOID              lpvRootCookie,
    LPWD7_MACRO_SCAN    lpstMacroScan
);

WD7_STATUS WD7MacroScanInit
(
    LPSS_STREAM         lpstStream,
    LPWD7ENCKEY         lpstKey,
    LPWD7_MACRO_SCAN    lpstMacroScan,
    LPBOOL              lpbHasMacros
);

WD7_STATUS WD7GetNextMacroForScan
(
    LPWD7_MACRO_SCAN    lpstMacroScan,
    LPBYTE              lpbyName,
    LPDWORD             lpdwOffset,
    LPDWORD             lpdwSize,
    LPBYTE              lpbyEncryptByte,
    LPBOOL              lpbNoMore
);

WD7_STATUS WD7InitTDTInfo
(
    LPSS_STREAM         lpstStream,
    LPWD7ENCKEY         lpstKey,
    LPWD7_TDT_INFO      lpstTDTInfo
);

WD7_STATUS WD7CountActiveMacros
(
    LPWD7_TDT_INFO      lpstTDTInfo,
    LPWORD              lpwNumActiveMacros
);

WD7_STATUS WD7GetMacroInfoAtIndex
(
    LPWD7_TDT_INFO      lpstTDTInfo,
    WORD                wIndex,
    LPBYTE              lpbyName,
    LPDWORD             lpdwOffset,
    LPDWORD             lpdwSize,
    LPBYTE              lpbyEncryptByte,
    LPBOOL              lpbActive
);

WD7_STATUS WD7SetMacroNameAtIndex
(
    LPWD7_TDT_INFO      lpstTDTInfo,
    WORD                wIndex,
    LPBYTE              lpbyName
);

WD7_STATUS WD7GetMacroInfoAtULName
(
    LPWD7_TDT_INFO      lpstTDTInfo,
    LPBYTE              lpbyName,
    LPWORD              lpwIndex,
    LPDWORD             lpdwOffset,
    LPDWORD             lpdwSize,
    LPBYTE              lpbyEncryptByte,
    LPBOOL              lpbActive
);

WD7_STATUS WD7GetULMacroNameAtIndex
(
    LPWD7_TDT_INFO      lpstTDTInfo,
    WORD                wIndex,
    LPBYTE              lpbyName
);

WD7_STATUS WD7GetULMacroDescAtIndex
(
    LPWD7_TDT_INFO      lpstTDTInfo,
    WORD                wIndex,
    LPBYTE              lpbyDesc
);

WD7_STATUS WD7SetULMacroNameAtIndex
(
    LPWD7_TDT_INFO      lpstTDTInfo,
    WORD                wIndex,
    LPBYTE              lpbyName
);

WD7_STATUS WD7DeactivateMacroAtIndex
(
    LPWD7_TDT_INFO      lpstTDTInfo,
    WORD                wIndex
);

BOOL WD7TDTHasNonMacroInfo
(
    LPWD7_TDT_INFO      lpstTDTInfo
);

WD7_STATUS WD7HasAutoText
(
    LPSS_STREAM         lpstStream,
    LPWD7ENCKEY         lpstKey,
    LPBOOL              lpbHasAutoText
);

WD7_STATUS WD7ChangeToDocument
(
    LPSS_STREAM         lpstStream,
    LPWD7ENCKEY         lpstKey
);

WD7_STATUS WD7MacroBinaryCRC
(
    LPSS_STREAM         lpstStream,
    LPWD7ENCKEY         lpstKey,
    DWORD               dwOffset,
    DWORD               dwSize,
    BYTE                byEncrypt,
    LPDWORD             lpdwCRC
);

WD7_STATUS WD7SetTDTSize
(
    LPSS_STREAM         lpstStream,
    LPWD7ENCKEY         lpstKey,
    DWORD               dwTDTSize
);

#define WD7_DELTA_WILD  0xFFFFFFFF

typedef struct tagMENU_INFO
{
    DWORD   dwInst;         // Instruction
	DWORD	dwMenu;			// Menu Position or Record0x10 Index
	DWORD	dwMenuItem;		// Menu Item Record0x10 Index or Menu Record0x10 Index
	DWORD	dwFunction;		// Function
	DWORD	dwMacroIndex;	// Macro Record0x11 Index
	DWORD	dwMenuItemPos;	// Menu Position or Menu Item Position
    LPBYTE  lpbyMenuNameA;  // Pointer to Pascal string Menu
    LPBYTE  lpbyMenuNameB;  // Pointer to Pascal string Menu Item
} MENU_INFO_T, FAR *LPMENU_INFO;

WD7_STATUS WD7DeleteDelta
(
    LPSS_STREAM         lpstStream,
    LPWD7ENCKEY         lpstKey,
    LPWD7_TDT_INFO      lpstTDTInfo,
    LPMENU_INFO         lpstMenuInfo
);

BOOL WD7ClearDocument
(
    LPSS_STREAM         lpstStream,
    LPWD7ENCKEY         lpstKey,
    LPWD7FIB            lpstFIB,
    DWORD               dwFIBOffset
);

BOOL WD7GetMCDInfo
(
    LPSS_STREAM         lpstStream,
    LPWD7ENCKEY         lpstKey,
    DWORD               dwTDTOffset,
    DWORD               dwTDTSize,
    LPDWORD             lpdwMCDOffset,
    LPWORD              lpwMCDCount
);

#endif // #ifndef _WD7API_H_

