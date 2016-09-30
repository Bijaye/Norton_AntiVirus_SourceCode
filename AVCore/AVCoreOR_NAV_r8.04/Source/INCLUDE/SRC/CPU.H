// Copyright 1995 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/INCLUDE/VCS/CPU.H_v   1.9   29 Apr 1997 17:47:44   JBRENNA  $
//
// Description:
//
//  Contains constants and data structures required by the core components
//  of the CPU emulator.
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/INCLUDE/VCS/CPU.H_v  $
// 
//    Rev 1.9   29 Apr 1997 17:47:44   JBRENNA
// Only define PATHLEN if there is not already one defined. MSVC42:LMCONS.H
// defines this as 256. Use the LMCONS.H definition of PATHLEN, if included
// by the application.
// 
//    Rev 1.8   29 Apr 1997 17:31:30   JBRENNA
// Add #ifndef wrapper for files that include this multiple times.
// 
//    Rev 1.7   06 Sep 1996 14:04:00   CNACHEN
// Updated header to use proper #pragma pack() for NTK/WIN32.
// 
//    Rev 1.7   06 Sep 1996 14:01:10   CNACHEN
// Added proper #pragma pack() commands for NTK/WIN32 platforms.
// 
//    Rev 1.6   26 Apr 1996 09:53:36   CNACHEN
// Changed the iteration stop constant ITER_CHECK_POINT from 0x4000 to 2K.  This
// increases the number of string scans and reduces scan time on infected files.
// 
// 
//    Rev 1.5   15 Mar 1996 13:27:26   CNACHEN
// Added INVALID_INDEX exclusions...
// 
//    Rev 1.4   01 Mar 1996 12:22:56   CNACHEN
// Added JUMP/CALL following for COM-file signature-based exclusions.
// 
// 
//    Rev 1.3   20 Feb 1996 11:27:22   CNACHEN
// Changed all LPSTRs to LPTSTRs.
// 
//    Rev 1.2   07 Feb 1996 11:15:58   CNACHEN
// changed 0x0200 to 512 for sector size...
// 
//    Rev 1.1   02 Feb 1996 11:45:30   CNACHEN
// Added new dwFlags and exclusion fields...
// 
//    Rev 1.0   01 Feb 1996 10:17:38   CNACHEN
// Initial revision.
// 
//    Rev 1.15   22 Jan 1996 17:23:02   DCHI
// Added instructionFetchCount field to CPU_TYPE structure.
// 
//    Rev 1.14   22 Jan 1996 13:40:44   CNACHEN
// Added new fault support.
// 
// 
//    Rev 1.13   19 Dec 1995 19:08:06   CNACHEN
// Added prefetch queue support!
// 
// 
//    Rev 1.12   15 Dec 1995 18:37:22   CNACHEN
// Added LOW_MEM_SIZE...
// 
//    Rev 1.11   14 Dec 1995 12:58:22   CNACHEN
// Added facility to relinquish control from NLM..
// 
//    Rev 1.10   14 Dec 1995 10:49:16   CNACHEN
// Fixed repair stuff...
// 
//    Rev 1.9   15 Nov 1995 20:15:22   CNACHEN
// Added new Parity JCOND stuff...
// 
//    Rev 1.8   03 Nov 1995 11:24:08   CNACHEN
// Updated iteration checkpoint from 32768 to 16384...
// 
//    Rev 1.7   23 Oct 1995 12:51:24   CNACHEN
// Added #pragma pack(1) directives around all structure definitions
// 
//    Rev 1.6   19 Oct 1995 18:39:26   CNACHEN
// Initial revision... With comment headers... :)
//************************************************************************

#ifndef CPU_H_INCLUDED
#define CPU_H_INCLUDED

/* defines */

#define TRUE	1
#define FALSE	0

#define FLAG_SYS_FILE               0x01

#define INVALID_INTERRUPT   0xFFFFU

#define ITER_CHECK_POINT    2048        // stop every 2K iterations to look
                                        // for signatures...

// how often (# of iterations) should we relinquish control on the NLM
#define NLM_ITER_CHECK_POINT    20


// This is already defined as 256 when MSVC42:LMCONS.H is included.
#ifndef PATHLEN
#define PATHLEN			128
#endif

#define PARAGRAPH		16			/* 16 bytes */

#define EFFECTIVE_ADDRESS_MASK  0xFFFFFLU

#define COM_START_SEG   0x7F0       /* start segment for COM files */
#define EXE_START_SEG   0x800       /* start segment for EXE files */
                                    /* this must be on a 512 byte boundary! */
#define SYS_START_SEG   0x800       /* start segment for SYS files */
#define COM_START_OFF	0x100		/* 100h */

#define PHYS_LOAD_ADDR  0x8000

#define LOW_MEM_SIZE    0x8000      // same as PHYS_LOAD_ADDR

#ifndef SECTOR_SIZE
#define SECTOR_SIZE     512
#endif

#define MAX_COM_SIZE    0xFF00U     // com files bigger than this are bad
#define MAX_SYS_SIZE    0x10000LU   // sys files bigger than this are bad

#define MAX_COM_JUMP_DEPTH  32      // # of jumps to follow in COM files for
                                    // signature exclusions...

#define STACK_START     0x0000      /* start STACK at end of segment for COM */

#define VECT_SIZE		4

#define PREFIX_REPZ     1
#define PREFIX_REPNZ    2

#define DEFAULT_SEG     0xff        /* -1 for default seg */
#define CS_OVER			1
#define DS_OVER         3
#define ES_OVER         0
#define SS_OVER         2
#define FS_OVER         4
#define GS_OVER         5


#define	REGISTER		1
#define WORD_SIZE		2
#define DWORD_SIZE      4

#define REG_AX			0
#define REG_CX			1
#define REG_DX			2
#define REG_BX			3
#define	REG_SP			4
#define REG_BP			5
#define REG_SI			6
#define REG_DI			7

#define REG_AL			0
#define REG_CL			1
#define REG_DL			2
#define REG_BL			3
#define REG_AH			4
#define REG_CH			5
#define REG_DH			6
#define REG_BH			7

#define REG_ES			0
#define REG_CS			1
#define REG_SS			2
#define REG_DS			3
#define REG_FS          4
#define REG_GS          5

#define REG_EAX         0
#define REG_ECX         1
#define REG_EDX         2
#define REG_EBX         3
#define REG_ESP         4
#define REG_EBP         5
#define REG_ESI         6
#define REG_EDI         7

/* Jump types */

/* short jumps */

#define JA_SHORT			0x77
#define JAE_SHORT			0x73
#define JC_SHORT			0x72
#define JBE_SHORT			0x76
#define JZ_SHORT			0x74
#define JG_SHORT			0x7f
#define JGE_SHORT			0x7d
#define JL_SHORT			0x7c
#define JLE_SHORT			0x7e
#define JNZ_SHORT			0x75
#define JNO_SHORT			0x71
#define JNS_SHORT			0x79
#define JO_SHORT			0x70
#define JS_SHORT			0x78
#define JPE_SHORT           0x7a
#define JPO_SHORT           0x7b


/* 386 0F prefix opcodes */

#define JA_NEAR             0x87
#define JAE_NEAR            0x83
#define JC_NEAR             0x82
#define JBE_NEAR            0x86
#define JZ_NEAR             0x84
#define JG_NEAR             0x8f
#define JGE_NEAR            0x8d
#define JL_NEAR             0x8c
#define JLE_NEAR            0x8e
#define JNZ_NEAR            0x85
#define JNO_NEAR            0x81
#define JNS_NEAR            0x89
#define JO_NEAR             0x80
#define JS_NEAR             0x88
#define JPE_NEAR            0x8a
#define JPO_NEAR            0x8b

#define BSF_OPCODE          0xBC
#define BSR_OPCODE          0xBD
#define BSWAP_OPCODE        0xC8
#define BT_REG_OPCODE       0xA3
#define BT_IMMED_OPCODE     0xBA
#define BTC_REG_OPCODE      0xBB
#define BTR_REG_OPCODE      0xB3
#define BTS_REG_OPCODE      0xAB
#define BT_DETERMINE        0
#define CLTS_OPCODE         0x06
#define CMPXCHGB_OPCODE     0xB0
#define CMPXCHGW_OPCODE     0xB1
#define INVD_OPCODE         0x08
#define LGDT_OPCODE         0x01
#define LSS_OPCODE          0xB2
#define LFS_OPCODE          0xB4
#define LGS_OPCODE          0xB5
#define MOVSX8_OPCODE       0xBE
#define MOVSX16_OPCODE      0xBF
#define MOVZX8_OPCODE       0xB6
#define MOVZX16_OPCODE      0xB7
#define POP_FS_OPCODE       0xA1
#define POP_GS_OPCODE       0xA9
#define PUSH_FS_OPCODE      0xA0
#define PUSH_GS_OPCODE      0xA8

#define SETA_OPCODE         0x97
#define SETAE_OPCODE        0x93
#define SETB_OPCODE         0x92
#define SETBE_OPCODE        0x96
#define SETE_OPCODE         0x94
#define SETG_OPCODE         0x9F
#define SETGE_OPCODE        0x9D
#define SETL_OPCODE         0x9C
#define SETLE_OPCODE        0x9E
#define SETNE_OPCODE        0x95
#define SETNO_OPCODE        0x91
#define SETNP_OPCODE        0x9B
#define SETNS_OPCODE        0x99
#define SETO_OPCODE         0x90
#define SETP_OPCODE         0x9A
#define SETS_OPCODE         0x98

#define SHLD_CL             0xA5
#define SHLD_I8             0xA4

#define SHRD_CL             0xAD
#define SHRD_I8             0xAC

#define WBINVD_OPCODE       0x09

#define XADD8_OPCODE        0xC0
#define XADD16_OPCODE       0xC1


#define BT_TEST             0x04
#define BT_CARRY            0x07
#define BT_RESET            0x06
#define BT_SET              0x05

// the following SAVE macros are used during faults to record the state of
// the machine!

#define SAVE_CS_PREFIX      0x0001
#define SAVE_DS_PREFIX      0x0002
#define SAVE_ES_PREFIX      0x0004
#define SAVE_FS_PREFIX      0x0008
#define SAVE_GS_PREFIX      0x0010
#define SAVE_SS_PREFIX      0x0020
#define SAVE_REPZ_PREFIX    0x0040
#define SAVE_REPNZ_PREFIX   0x0080
#define SAVE_OPERAND_PREFIX 0x0100
#define SAVE_ADDRESS_PREFIX 0x0200


/* macros */

#define MOD_VALUE(b) ((b >> 6) & 3)     /* upper 2 bits shifted to be low */
#define RM_VALUE(b)  (b & 7)            /* lower 3 bits */
#define REG_VALUE(b) ((b >> 3) & 7)     /* bits 5 to 3 */
#define W_VALUE(b)	(b & 1)				/* width - low bit of opcode */
#define D_VALUE(b)  ((b >> 1) & 1)		/* dest - bit 1 of opcode */
#define S_VALUE(b)  ((b >> 1) & 1)
#define SEC_OP(b)   ((b >> 3) & 7)		/* secondary part of opcode in amb */
#define SS_VALUE(b) ((b >> 6) & 3)      /* upper 2 bits shifted to be low */
#define INDEX_VALUE(b) ((b >> 3) & 7)   /* bits 5 to 3 */
#define BASE_VALUE(b) (b & 7)           /* lower 3 bits */

/* types */

#ifdef BORLAND

typedef unsigned short WORD;
typedef unsigned char  BYTE;
typedef unsigned long  DWORD;
typedef unsigned long  DWORD;
typedef char           *LPTSTR;

#endif


#if defined(SYM_WIN32) || defined(SYM_NTK)
#pragma pack(push,1)
#else
#pragma pack(1)
#endif


typedef struct
{
    WORD AX, TAX, BX, TBX, CX, TCX, DX, TDX;
} word_reg;

typedef struct
{
	BYTE AL, AH;
    WORD TAX;
	BYTE BL, BH;
    WORD TBX;
	BYTE CL, CH;
    WORD TCX;
    BYTE DL, DH;
    WORD TDX;
} byte_reg;

typedef struct
{
    DWORD EAX;
    DWORD EBX;
    DWORD ECX;
	DWORD EDX;
} dword_reg;

typedef union
{
	dword_reg D;
	word_reg  X;
	byte_reg  H;
} primary;


typedef struct
{
	DWORD ESI;
	DWORD EDI;
	DWORD EBP;
	DWORD ESP;
} idword_reg;

typedef struct
{
	WORD SI, TSI;
	WORD DI, TDI;
	WORD BP, TBP;
	WORD SP, TSP;
} iword_reg;

typedef union
{
	idword_reg D;
	iword_reg  X;
} index_regs;

typedef struct
{
	WORD O:1;
	WORD D:1;
	WORD I:1;
	WORD T:1;
	WORD S:1;
	WORD Z:1;
	WORD A:1;
	WORD P:1;
	WORD C:1;
} flag_type;

#define MAX_PREFETCH_QUEUE_SIZE     0x21        // 33 entries
#define PREFETCH_FLAG_RELOAD        0x01
#define PREFETCH_FLAG_FAULT         0x02

#define PREFETCH_NO_REQUEST         0   // no more requests
#define PREFETCH_0_BYTE             1   // pentium queue
#define PREFETCH_8_BYTE             2   // 8086 queue is 8 bytes
#define PREFETCH_16_BYTE            3   // 80386 is 16 bytes
#define PREFETCH_32_BYTE            4   // 80486 is 32 bytes

#define PREFETCH_MODIFIED           0x8000U // high bit set on modified mem


// invalid index defines

#define VALID_INDEX_SI              0x0001
#define VALID_INDEX_DI              0x0002
#define VALID_INDEX_BX              0x0004
#define VALID_INDEX_BP              0x0008
#define VALID_INDEX_IGNORE          0x8000



typedef struct
{
    WORD wCurrentRequest;
    WORD wNextRequest;
    WORD wQueue[MAX_PREFETCH_QUEUE_SIZE];
    WORD wFlags;
    WORD wHead, wTail, wSize;
} prefetch_type;


typedef struct
{
	primary 	preg;			/* AX thru DX */
	index_regs  ireg;
	WORD IP;                    /* instruction pointer */
    WORD CS, DS, ES, SS, FS, GS;
	flag_type FLAGS;			/* flags */
	long offset_modifier;		/* ADD this to obtain correct position in FILE */
	long image_start_offset;    /* address in the file where image starts */
	HFILE stream;               /* opened EXE/COM file */
	DATAFILEHANDLE  hDataFile;  /* handle to our data file */
	HFILE low_stream;           /* low memory stream */
    DWORD ulLowStartOffset;     /* offset of low memory in DATAFILE */
	BYTE  seg_over;             /* seg override value */
	BYTE  op_size_over;         /* operand size override */
	BYTE  addr_size_over;       /* address size override */
    DWORD ulImageSize;          /* image size of the executed program */
    DWORD instructionFetchCount;/* # of instructions fetched so far */

    DWORD iteration;            /* # of instructions executed so far */
    DWORD immune_iteration;     /* # of immune instructions executed so far */

    DWORD max_iteration;        /* maximum number of iterations for non-excluded */
    DWORD max_immune_iteration; /* maximum number of iterations during immune */
    DWORD next_iter_stop;       /* when to stop next for scans */
    DWORD next_nlm_iter_stop;   /* when to stop next for NLM relinquish */
    DWORD next_nlm_immune_iter_stop;    /* ", but during iter immune regions */


    DWORD num_writes;           /* # memory writes during execution */
    BYTE  reset_seg;            /* for segment/operand/address override state */
    BYTE  rep_prefix;           /* REP(z/nz) was specified */
    prefetch_type   prefetch;   /* our prefetch queue! */
    DWORD suspend_fault[NUM_FAULTS]; /* if !0, then don't fault, but dec */

    // invalid index write data

    WORD    wIndexStatus;       /* bit status for index registers */
    DWORD   dwBadIndexIteration;/* iteration # of last bad index usage */

} CPU_TYPE;

typedef struct
{
	char exesig[2];                 /* MZ */
	WORD len_mod_512;       /* length of file % 512 */
	WORD page_size;         /* size of file in 512 byte pages */
	WORD reloc_table_items; /* # of relocation table items */
	WORD header_size;       /* size of header in paragraphs */
	WORD min_par_above;     /* min # of pars needed above program */
	WORD max_par_above;     /* max " */
	WORD ss;
	WORD sp;
	WORD checksum;
	WORD ip;
	WORD cs;
	WORD first_rel_offset;  /* offset of first relocation table */
} exeheader;

typedef struct
{
    WORD offset, segment;
    WORD dev_atr;
	WORD strategy_ip;
	WORD int_ip;
    BYTE buffer[8];
} sysheader;

#if defined(SYM_WIN32) || defined(SYM_NTK)
#pragma pack(pop)
#else
#pragma pack()
#endif



#define wsign_status(a) (!!((a)&0x8000))
#define dsign_status(a) (!!((a)&0x80000000L))
#define bsign_status(a) (!!((a)&0x80))
#define wzero_status(a) (!!a)
#define bzero_status(a) (!!a)


// flag #define's

#define wset_zero_flag(w) (hLocal->CPU.FLAGS.Z = !(w))
#define bset_zero_flag(b) (hLocal->CPU.FLAGS.Z = !(b))
#define wset_sign_flag(w) (hLocal->CPU.FLAGS.S = !!((w) & 0x8000))
#define bset_sign_flag(b) (hLocal->CPU.FLAGS.S = !!((b) & 0x80))

#endif // CPU_H_INCLUDED
