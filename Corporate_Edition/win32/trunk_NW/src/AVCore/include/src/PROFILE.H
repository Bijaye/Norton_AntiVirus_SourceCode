// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 1995 - 2003, 2005 Symantec Corporation. All rights reserved.
//************************************************************************
//
// $Header:   S:/INCLUDE/VCS/PROFILE.H_v   1.3   06 Sep 1996 14:04:24   CNACHEN  $
//
// Description:
//
//  This source file contains constants and data structures which are used
//  to parse and load the compiled exclusion definition files from the PAM
//  data file.
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/INCLUDE/VCS/PROFILE.H_v  $
// 
//    Rev 1.3   06 Sep 1996 14:04:24   CNACHEN
// Updated header to use proper #pragma pack() for NTK/WIN32.
// 
//    Rev 1.3   06 Sep 1996 14:01:14   CNACHEN
// Added proper #pragma pack() commands for NTK/WIN32 platforms.
// 
//    Rev 1.2   15 Mar 1996 10:35:56   CNACHEN
// Added INVALID_INDEX_WRITE bit...
// 
//    Rev 1.1   07 Mar 1996 10:25:00   CNACHEN
// Added PROFILE_FLAG_NOT_IN_TSR #define...
// 
//    Rev 1.0   01 Feb 1996 10:17:44   CNACHEN
// Initial revision.
// 
//    Rev 1.6   23 Oct 1995 12:51:32   CNACHEN
// Added #pragma pack(1) directives around all structure definitions
// 
//    Rev 1.5   19 Oct 1995 18:39:20   CNACHEN
// Initial revision... With comment headers... :)
//************************************************************************


/* This file contains structure definitions for decryptor class
   data records */

#define INSTR_WORDS     (256/16)
#define DOS_FUNC_WORDS  (256/16)

#define INFECTS_COM     1       /* any non EXE/SYS file (data files included) */
#define INFECTS_EXE     2       /* any file beginning with MZ or ZM */
#define INFECTS_SYS     4       /* all SYS files */

#define PROFILE_FLAG_COM_JUMP          0x0001 /* COM file begins with E9 JMP */
#define PROFILE_FLAG_REP_MOVS          0x0002 /* REP used with MOVS */
#define PROFILE_FLAG_REP_STOS          0x0004 /* REP used with STOS */
#define PROFILE_FLAG_CALL128           0x0008 /* CALLS with destination > 128 bytes away */
#define PROFILE_FLAG_RELOC_AFTER_CSIP  0x0010 /* fixup allowed after CSIP */
#define PROFILE_FLAG_STOP_AFTER_16     0x0020
#define PROFILE_FLAG_STOP_AFTER_1024   0x0040
#define PROFILE_FLAG_STOP_AFTER_16384  0x0080
#define PROFILE_FLAG_STOP_AFTER_32768  0x0100
#define PROFILE_FLAG_STOP_AFTER_49152  0x0200
#define PROFILE_FLAG_STOP_AFTER_65536  0x0400
#define PROFILE_FLAG_STOP_AFTER_81920  0x0800
#define PROFILE_FLAG_STOP_AFTER_98304  0x1000
#define PROFILE_FLAG_STOP_AFTER_131072 0x2000
#define PROFILE_FLAG_RUN_32            0x4000 /* engine allowed to use runs of same instr */
#define PROFILE_FLAG_REP_SCAS          0x8000 /* don't forget! */
#define PROFILE_FLAG_NOT_IN_TSR        0x00010000UL
#define PROFILE_FLAG_INVALID_INDEX_WRITE    0x00020000UL    // ADD [BX+SI], AL
                                                            // type stuff
#define BIT_INT_05      0x0001
#define BIT_INT_10      0x0002
#define BIT_INT_12      0x0004
#define BIT_INT_13      0x0008
#define BIT_INT_16      0x0010
#define BIT_INT_17      0x0020
#define BIT_INT_1A      0x0040
#define BIT_INT_20      0x0080
#define BIT_INT_21      0x0100
#define BIT_INT_25      0x0200
#define BIT_INT_26      0x0400
#define BIT_INT_27      0x0800
#define BIT_INT_2F      0x1000
#define BIT_INT_33      0x2000
#define BIT_INT_67      0x4000

#if defined(SYM_WIN32) || defined(SYM_NTK)
#pragma pack(push,1)
#else
#pragma pack(1)
#endif


typedef struct
{
    /* bit fields are set up in the following manner:

            [B7 B6 B5 B4 B3 B2 B1 B0] [B15 B14 B13 B12 B11 B10 B9 B8] ...

    */


	WORD opcode_set[INSTR_WORDS];       /* bits are 1 for allowable, 0 for no */
	WORD good_dos_set[DOS_FUNC_WORDS];  /* valid DOS functions (AH = ??) */
    WORD bad_int_set;                   /* bit list of non-allowable int #'s */

    BYTE infect_set;                    /* COM/EXE/SYS */

    DWORD max_exe_eof_dist;             /* MAX bytes from EOF for EXE */
    DWORD min_exe_eof_dist;             /* MIN bytes from EOF for EXE */
    DWORD min_file_size;                /* MIN image size (EXE & COM) */
    DWORD flags;                        /* profile flags */

} profile_type;

#if defined(SYM_WIN32) || defined(SYM_NTK)
#pragma pack(pop)
#else
#pragma pack()
#endif

