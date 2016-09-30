// Copyright 1996 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/INCLUDE/VCS/n30type.h_v   1.8   12 May 1997 12:24:16   MKEATIN  $
//
// Description:
//  Contains structure definition for old-style N30 structure
//
// Contains:
//
// See Also:
//
//************************************************************************
// $Log:   S:/INCLUDE/VCS/n30type.h_v  $
// 
//    Rev 1.8   12 May 1997 12:24:16   MKEATIN
// Removed a reference the to the HUGE keyword - breaks pyro.
// 
//    Rev 1.7   08 May 1997 13:01:56   MKEATIN
// This now matches the n30type.h in AVCORE which has addition info.
// 
//    Rev 1.3   18 Apr 1997 13:44:22   MKEATIN
// Latest header from NEWVIR/INCLUDE
// 
//    Rev 1.6   31 Jan 1997 19:10:28   CNACHEN
// Added pam repair (flag 2g of control word) to criteria for determing whether
// or not a def has repair.
// 
//    Rev 1.5   20 Nov 1996 15:17:14   AOONWAL
// No change.
// 
//    Rev 1.4   22 Oct 1996 12:10:14   AOONWAL
// No change.
// 
//    Rev 1.3   13 Aug 1996 14:39:32   CNACHEN
// Added several control bits.
// 
//    Rev 1.2   06 Jun 1996 18:27:52   DCHI
// Added defines of bitINFO and CTRL flags.
// 
//    Rev 1.1   06 Jun 1996 16:47:40   DCHI
// Changed #define N30_SIZE_SIG from 16 to 17.
// 
//    Rev 1.0   05 Jun 1996 13:27:14   CNACHEN
// Initial revision.
// 
//************************************************************************

#ifndef _N30TYPE_H

#define _N30TYPE_H

#include "avtypes.h"

#define CRCtype                 0
#define ALGtype                 1
#define BOOtype                 2
#define MACtype                 3

#define SIZE_SIG        17
#define SIZE_FIXED      64
#define SIZE_BOOTSIG    14
#define SIZE_COMMENT    ((5 * 60) + 1) //5 lines of 60 chars per line + EOS
#define SIZE_ALIAS      40
#define SIZE_CRC        (sizeof(WORD))
#define SIZE_CHECK_DATA (sizeof(CHECKDATA)) // Check + control word

#define bitINFO_COMFILE     0x0001
#define bitINFO_EXEFILE     0x0002
#define bitINFO_SYSFILE     0x0004
#define bitINFO_COMCOM      0x0008
#define bitINFO_FLOPBOOT    0x0010
#define bitINFO_HDISKBOOT   0x0020
#define bitINFO_MBOOTREC    0x0040
#define bitINFO_DIR2        0x0080
#define bitINFO_RESIDENT    0x0100
#define bitINFO_SIZESTLTH   0x0200
#define bitINFO_FULLSTLTH   0x0400
#define bitINFO_ENCRYPT     0x0800
#define bitINFO_POLYMORPH   0x1000
#define bitINFO_TRIGGERED   0x2000
#define bitINFO_INTHEWILD   0x4000
#define bitINFO_MULTIPARTY  0x8000

// These are compared with wControl2 flag in n30 unlike others
// which are compared with wInfo
#define bitINFO_MACRO       0x4000
#define bitINFO_WINDOWS     0x8000
#define bitINFO_AGENT       0xC000
// This is to mask the above three types
#define bitINFO_MASKEXT     0xC000

#define bitCTRL_BOOT_REPAIR 0x2000
#define bitCTRL_NOTCLEAN    0x8000

#define N30_CTRL_GENERIC_COM         0x01
#define N30_CTRL_GENERIC_EXE         0x02
#define N30_CTRL_PREPENDING          0x04
#define N30_CTRL_REBUILD_EXE         0x08
#define N30_CTRL_IMPROVED            0x10
#define N30_CTRL_OVERWRITE           0x20
#define N30_CTRL_BIT_COMPANION       0x40
#define N30_CTRL_BIT_REDIRECT        0x80
#define N30_CTRL_SECONDBOOK          0x200
#define N30_CTRL_SLIDER              0x400
#define N30_CTRL_DECRYPT             0x800
#define N30_CTRL_BOOT_REPAIR         0x2000
#define N30_CTRL_DELETE_ITEM         0x4000
#define N30_CTRL_NOCLEAN             0x8000

#define N30_CTRL2_CUT_BEGIN           0x1
#define N30_CTRL2_TWEEK_CS_10         0x2
#define N30_CTRL2_TWEEK_SS_10         0x4
#define N30_CTRL2_BOOK_FROM_BEGIN     0x8
#define N30_CTRL2_COM_DISTINCT        0x10
#define N30_CTRL2_EXEC_CODE           0x20
#define N30_CTRL2_DECRYPT_XOR         0x40
#define N30_CTRL2_DECRYPT_ADD         0x80
#define N30_CTRL2_DECRYPT_WORD        0x100
#define N30_CTRL2_NO_GENERIC          0x200
#define N30_CTRL2_DISPLAY_DIALOG      0x400
#define N30_CTRL2_NOT_IN_TSR          0x800

#define N30_CTRL2_BIT_FLAG2G        0x1000      // PAM repair...
#define N30_CTRL2_BIT_FLAG2H        0x2000      // CRC 9-byte verify
#define N30_CTRL2_BIT_FLAG2I        0x4000
#define N30_CTRL2_BIT_FLAG2J        0x8000

#define N30_CTRL_ANY_REPAIR     (N30_CTRL_GENERIC_COM | \
N30_CTRL_GENERIC_EXE | \
N30_CTRL_PREPENDING  | \
N30_CTRL_REBUILD_EXE | \
N30_CTRL_IMPROVED    | \
N30_CTRL_SECONDBOOK  | \
N30_CTRL_SLIDER      | \
N30_CTRL_DECRYPT     | \
N30_CTRL_BOOT_REPAIR)

#define N30_CTRL2_ANY_REPAIR    (N30_CTRL2_CUT_BEGIN  | \
N30_CTRL2_TWEEK_CS_10| \
N30_CTRL2_TWEEK_SS_10| \
N30_CTRL2_BOOK_FROM_BEGIN | \
N30_CTRL2_COM_DISTINCT | \
N30_CTRL2_EXEC_CODE | \
N30_CTRL2_DECRYPT_XOR | \
N30_CTRL2_DECRYPT_ADD | \
N30_CTRL2_DECRYPT_WORD | \
N30_CTRL2_DISPLAY_DIALOG | \
N30_CTRL2_BIT_FLAG2G)

#define N30_SIZE_VIRUSNAME      22
#define N30_SIZE_MEM            15
#define N30_SIZE_SIG            17

typedef struct
{
    WORD    wVirID;          // Virus ID Number
    WORD    wType;           // Type
    WORD    wCrc;            // CRC
    WORD    wControl;        // Control word for Cleaning
    WORD    wInfo;           // Information Word
    WORD    wFixedID;        // Fixed algo virus ID number

    char    lpVName[N30_SIZE_VIRUSNAME]; // Virus Name
    BYTE    lpMem[N30_SIZE_MEM];  // Memory Signature
    BYTE    lpSig[N30_SIZE_SIG];  // Regular Signature

    WORD    wInfSize;
    WORD    wTagSize;
    WORD    wHostJmp;
    WORD    wJmpLen;
    WORD    wSS_reg;
    WORD    wSP_reg;
    WORD    wCS_reg;
    WORD    wIP_reg;
    WORD    wHeader;
    WORD    wHeadLen;
    WORD    wBookLocation;
    WORD    wBookMark;
    DWORD   dwReserved;
    WORD    wControl2;
} N30, FAR *LPN30;

#define sN30    sizeof(N30)

typedef struct
    {
    char    lpVirName[N30_SIZE_VIRUSNAME];
    WORD    wVirID;
    WORD    wType;
    WORD    wControl2;
    WORD    wInfo;
    WORD    wSize;
    } N30NOTES, FAR *LPN30NOTES;
#define sN30NOTES   sizeof(N30NOTES)


#endif
