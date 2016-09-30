// Copyright 1996 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/INCLUDE/VCS/avenge.h_v   1.21   17 Jul 1997 17:49:56   DDREW  $
//
// Description:
//  Contains prototypes for functions exported by AVENGE.
//
// Contains:
//
// See Also:
//
//************************************************************************
// $Log:   S:/INCLUDE/VCS/avenge.h_v  $
// 
//    Rev 1.21   17 Jul 1997 17:49:56   DDREW
// Moved some "ifndef SYM_NLM's" around
// 
//    Rev 1.20   08 Jul 1997 17:20:24   MKEATIN
// No longer pass the defualt NULL in any protypes.  This won't compile for
// the NLM C compiler.
// 
//    Rev 1.19   02 Jul 1997 10:17:34   CNACHEN
// added proper w32/ntk pragma pack(push/pops)
// 
// 
//    Rev 1.18   03 Jun 1997 18:13:00   MKEATIN
// Added the prototype for the new EngGetNumClaimedViruses().
// 
//    Rev 1.17   20 May 1997 13:18:22   CNACHEN
// 
//    Rev 1.16   19 May 1997 10:30:36   CNACHEN
// Added bootlist section ID.
// 
//    Rev 1.15   15 May 1997 12:21:40   MKEATIN
// Added a lpvProvidedFileInfo parameter to EngGetVirusName().
// 
//    Rev 1.14   13 May 1997 18:25:50   MKEATIN
// Ported Carey's boot info caching code
// 
//    Rev 1.13   28 Apr 1997 18:37:36   MKEATIN
// type defined HGENG as GENG FAR * instead of tagGENG FAR *
// 
//    Rev 1.12   28 Apr 1997 18:31:26   MKEATIN
// Changed tagHENG to tagHGENG - woops.
// 
//    Rev 1.11   28 Apr 1997 18:26:40   MKEATIN
// Gave names to structs GENG and LENG: tagGENG and tagLENG.
// 
//    Rev 1.10   18 Apr 1997 13:44:26   MKEATIN
// Latest header from NEWVIR/INCLUDE
// 
//    Rev 1.13   20 Nov 1996 15:23:08   AOONWAL
// No change.
// 
//    Rev 1.12   23 Oct 1996 11:36:14   DCHI
// Corrected EngGlobalInit prototype.
// 
//    Rev 1.11   23 Oct 1996 11:18:22   DCHI
// Added IPC callback support.
// 
//    Rev 1.10   22 Oct 1996 12:10:30   AOONWAL
// No change.
// 
//    Rev 1.9   13 Aug 1996 10:19:08   CNACHEN
// Added new bit fields for Macro, Windows and Agent viruses.
// 
//    Rev 1.8   12 Aug 1996 17:43:00   DCHI
// Modifications for compilation on UNIX.
// 
//    Rev 1.7   19 Jul 1996 18:22:12   RAY
// Added support for 1k and 2k cluster DMF generic boot repair.
// 
//    Rev 1.6   18 Jul 1996 11:05:28   DCHI
// Reorganized location of ENG_CACHE_T w.r.t. FSTARTDATA_T so that
// NAVEX requests immediately follow FSTARTDATA_T.
// 
//    Rev 1.5   06 Jun 1996 18:41:58   DCHI
// Added defined AVENGE_TYPE_NO_GENERIC of wFlags of virus info.
// 
//    Rev 1.4   06 Jun 1996 18:31:40   DCHI
// Added AVENGE_TYPE_NOCLEAN and AVENGE_TYPE_NOT_IN_TSR for wFlags of info.
// 
//    Rev 1.3   06 Jun 1996 17:41:42   DCHI
// Removed skip fields from alg signature structures.
// 
//    Rev 1.2   05 Jun 1996 16:20:26   CNACHEN
// Added inclusion of n30type.h
// 
//    Rev 1.1   05 Jun 1996 14:49:28   CNACHEN
// Added prototype for EngGetN30HVIRUS()
// 
//    Rev 1.0   04 Jun 1996 17:40:10   DCHI
// Initial revision.
// 
//************************************************************************

#ifndef _AVENGE_H

#define _AVENGE_H

#include "avtypes.h"        // (includes platform.h)
#include "avcb.h"           // callback functions
#include "avobject.h"       // object information
#include "avendian.h"       // for endian conversion
#include "avdatfil.h"       // data file access functions
#include "n30type.h"        // contains old n30 structure definition

///////////////////////////////////////////////////////////////////////////
//
// Virus data file section information
//
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
//
// VIRSCAN1.DAT (Database) information
//
///////////////////////////////////////////////////////////////////////////

#define DATA_FILE_ID_VIRSCAN1DAT        1

#define SECTION_ID_VIRINFO              10
#define SECTION_ID_NAMESIDX             11
#define SECTION_ID_NAMES                12
#define SECTION_ID_BOOTLIST             13
#define SECTION_ID_CLAIMED_NUM_VIRUSES  500


///////////////////////////////////////////////////////////////////////////
//
// VIRSCAN2.DAT repair information and PAM stuff
//
///////////////////////////////////////////////////////////////////////////

#define DATA_FILE_ID_VIRSCAN2DAT        2

#define SECTION_ID_PLG_REPAIR           20
#define SECTION_ID_PLG_MEM_REPAIR       21


///////////////////////////////////////////////////////////////////////////
//
// VIRSCAN3.DAT repair information
//
///////////////////////////////////////////////////////////////////////////

#define DATA_FILE_ID_VIRSCAN3DAT        3

#define SECTION_ID_CRC_REPAIR           30
#define SECTION_ID_CRC_MEMORY_REPAIR    31
#define SECTION_ID_ALG_BOO_REPAIR       32
#define SECTION_ID_ALG_BOO_MEM_REPAIR   33


///////////////////////////////////////////////////////////////////////////
//
// VIRSCAN4.DAT (inscan) information.
//
///////////////////////////////////////////////////////////////////////////

#define DATA_FILE_ID_VIRSCAN4DAT        4

#define SECTION_ID_MEM_SIG              40
#define SECTION_ID_BOO_SIG              41
#define SECTION_ID_CRC_SIG              42
#define SECTION_ID_ALG_ITW_SIG          43
#define SECTION_ID_ALG_ZOO_SIG          44



///////////////////////////////////////////////////////////////////////////
//
// AVENGE data types
//
///////////////////////////////////////////////////////////////////////////

// invalid HVIRUS value...

#define INVALID_VIRUS_HANDLE        0xFFFFU
    
#define ENGSTATUS_OK        0
#define ENGSTATUS_ERROR     1

typedef unsigned int    ENGSTATUS;
typedef WORD            HVIRUS;
typedef HVIRUS FAR      *LPHVIRUS;


// -----------
// Drive Types
// -----------

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

///////////////////////////////////////////////////////////////////////////
//
// Algorithmic scanner data structures for hash tables/pools
//
///////////////////////////////////////////////////////////////////////////

#define INVALID_ALG_POOL_NUM    0xFF
#define MAX_ALG_POOLS           140     // > than computed in the design doc
#define MAX_POOL_BYTE_SIZE      65000UL

typedef struct
{
    BYTE        byPoolNum;              // Buffer index
    WORD        wPoolOffset;            // offset in the specified
                                        // buffer where signatures
                                        // begin
    WORD        wNumSigs;               // how many sigs to apply?
} ALG_HASH_ENTRY_T, FAR *LPALG_HASH_ENTRY;

typedef struct
{
    WORD       wBuffSize;               // Size of buffer
    LPBYTE     lpbyAlgPoolPtr;          // Pointer to buffer
} ALG_BUFF_ENTRY_T, FAR *LPALG_BUFF_ENTRY;

typedef struct
{
    WORD                wNumAlgPools;
    ALG_BUFF_ENTRY_T    AlgPoolArray[MAX_ALG_POOLS];
} POOL_INFO_STRUCT_T, FAR *LPPOOL_INFO_STRUCT;

typedef struct
{
    // four byte hash info

    WORD                wFourByteHashSize;
    LPALG_HASH_ENTRY    lpFourByteTable;

    // two byte hash info

    WORD                wTwoByteHashSize;
    LPALG_HASH_ENTRY    lpTwoByteTable;

    // one byte hash info

    WORD                wOneByteHashSize;
    LPALG_HASH_ENTRY    lpOneByteTable;

    // no hash info

    ALG_HASH_ENTRY_T    stNoHashInfo;

    // pointer to our pool information data structure

    LPPOOL_INFO_STRUCT  lpPoolInfoStruct;

} ALG_SCAN_INFO_T, FAR *LPALG_SCAN_INFO;


///////////////////////////////////////////////////////////////////////////
//
// FSTART defines and data structures
//
///////////////////////////////////////////////////////////////////////////

#define FSTART_BUFFER_SIZE      256
#define FSTART_MAX_PAD_BYTES    64
#define FSTART_ADDRESS_MASK     0xFFFFFLU

#define FSTART_MAX_DEPTH        100

#define JUMP_TWO_BYTE           0xE9
#define CALL_TWO_BYTE           0xE8
#define JUMP_ONE_BYTE           0xEB

#define JUMP_TWO_BYTE_SIZE      3
#define JUMP_ONE_BYTE_SIZE      2
#define JUMP_PUSH_RET_SIZE1     5
#define JUMP_PUSH_RET_SIZE2     6

#define RET_INSTRUCTION         0xC3
#define CLI_INSTRUCTION         0xFA

typedef struct
{
    BYTE    byFStartBuffer[FSTART_BUFFER_SIZE];
    BYTE    byTOFBuffer[FSTART_BUFFER_SIZE];
    BYTE    byFloatingBuffer[FSTART_BUFFER_SIZE];
} FSTARTDATA_T, FAR *LPFSTARTDATA;

typedef struct
{
    WORD    wFileType;
    DWORD   dwSegmentBase;
    DWORD   dwFirstLandingOffset;
    DWORD   dwFStartBufferOffset;
    WORD    wEntryPointNum;             // for use in SYS files...
    WORD    wFStartBufferLen;
} FSTARTINFO_T, FAR *LPFSTARTINFO;

#define FSTART_INVALID_FILE_TYPE        0   // file could not harbor a virus
#define FSTART_COM_FILE_TYPE            1   // typical COM file
#define FSTART_EXE_FILE_TYPE            2
#define FSTART_SYS_FILE_TYPE            3

#if defined(SYM_WIN32) || defined(SYM_NTK)
#pragma pack(push,1)
#else
#pragma pack(1)
#endif

typedef struct
{
    WORD exesig;            /* MZ */
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
} EXEHEADER_T, FAR *LPEXEHEADER;

#if defined(SYM_WIN32) || defined(SYM_NTK)
#pragma pack(pop)
#else
#pragma pack()
#endif

#define EXE_SIG1            0x4D5A
#define EXE_SIG2            0x5A4D
#define SYS_SIG             0xFFFFU

#define EXE_HEADER_SIZE     0x1c
#define SYS_HEADER_SIZE     0x12

#define STRATEGY_ENTRY      0
#define INTERRUPT_ENTRY     1

#define COM_INITIAL_IP      0x100
#define COM_IP_SUB_CONST    0x100

#if defined(SYM_WIN32) || defined(SYM_NTK)
#pragma pack(push,1)
#else
#pragma pack(1)
#endif

typedef struct
{
    WORD offset, segment;
    WORD dev_atr;
    WORD strategy_ip;
    WORD int_ip;
    BYTE buffer[8];
} SYSHEADER_T, FAR *LPSYSHEADER;

#if defined(SYM_WIN32) || defined(SYM_NTK)
#pragma pack(pop)
#else
#pragma pack()
#endif

///////////////////////////////////////////////////////////////////////////
//
// Algorithmic scanner file cache
//
///////////////////////////////////////////////////////////////////////////

#define ENG_EOF_CACHE_SIZE      512
#define ENG_NUM_CACHE_BUFFERS   8

typedef struct
{
    WORD                    wNumBytesInEOFCache;
    DWORD                   dwEOFCacheStartOffset;
    BYTE                    byEOFCache[ENG_EOF_CACHE_SIZE+FSTART_BUFFER_SIZE];

    // Cache for everything else except the EOF

    WORD                    wNumOtherFilled;
    WORD                    wNextFreeEntry;

    DWORD                   dwOtherOffset[ENG_NUM_CACHE_BUFFERS];
    BYTE                    byOtherCache[ENG_NUM_CACHE_BUFFERS]
                                [FSTART_BUFFER_SIZE];

    DWORD                   dwFileLength;
} ENG_CACHE_T, FAR *LPENG_CACHE;

///////////////////////////////////////////////////////////////////////////
//
// Algorithmic scanner data structures signature application
//
///////////////////////////////////////////////////////////////////////////

#define MAX_NAVEX_REQUESTS              512

#define MASK_HIGH_NIBBLE                0xF0
#define MASK_LOW_NIBBLE                 0x0F
#define WILDCARD_HIGH_NIBBLE            0xD0
#define JUMP_1_BYTE                     0xEB
#define JUMP_CONDITIONAL_HIGH_NIBBLE    0x70

#define OFFSET_OF_SIZE_IN_PAGES         0x04    // in the EXE header
#define OFFSET_OF_MOD_512               0x02    // in the EXE header

#define STRING_SEARCH_SKIP_1_BYTE       0xD2

typedef struct
{
    FSTARTDATA_T        stFStartData;
    WORD                wNAVEXRequestCount;
    WORD                wNAVEXRequestBuffer[MAX_NAVEX_REQUESTS];
    ENG_CACHE_T         stEngCache;
    LPBYTE              lpbyFloatingBuffer;
} ALGSCANDATA_T, FAR *LPALGSCANDATA;


///////////////////////////////////////////////////////////////////////////
//
// Memory scanner data structures
//
///////////////////////////////////////////////////////////////////////////

// 16 to align on word boundary

#define SIZE_MEM                16

#define MEM_PAGE_OVERLAP        SIZE_MEM    // ??

#define MAX_MEM_SIGS_PER_POOL   2048        // Power of 2

#define MEM_SIGS_POOL_SHIFT     11          // log of MAX_MEM_SIGS_PER_POOL
#define MEM_SIGS_POOL_ENTRY_MASK (MAX_MEM_SIGS_PER_POOL-1)

#define MAX_MEM_SIG_POOLS       (65536ul/2048ul)

#define ADDRESS_640K        0x0A0000ul
#define ADDRESS_1M          0x100000ul

#if defined(SYM_WIN32) || defined(SYM_NTK)
#pragma pack(push,1)
#else
#pragma pack(1)
#endif

typedef struct
{
    WORD            wVirusRecordIndex;
    BYTE            bySig[SIZE_MEM];
} MEMSIG_T, FAR *LPMEMSIG;

typedef struct
{
    BYTE            bySigType;              // see MEMORY_* above
    BYTE            bySig[SIZE_MEM];
} MEMSIGREPAIR_T, FAR *LPMEMSIGREPAIR;

#if defined(SYM_WIN32) || defined(SYM_NTK)
#pragma pack(pop)
#else
#pragma pack()
#endif

typedef struct
{
    WORD            wNumSigs;
    LPMEMSIG        lpMemSigs;
} MEMSIGPOOL_T, FAR *LPMEMSIGPOOL;

typedef struct
{
    WORD            wNumSigs;
    BYTE            byQuickEliminate[256];
    LPBYTE                  lpbyQuickWordEliminate;
    WORD            wNumSigPools;
    MEMSIGPOOL_T    stMemSigPool[MAX_MEM_SIG_POOLS];
} MEMSIGGROUP_T, FAR *LPMEMSIGGROUP;

typedef struct
{
    MEMSIGGROUP_T   stCD;
    MEMSIGGROUP_T   stALL;
    MEMSIGGROUP_T   stTOM;
} MEMORY_SCAN_INFO_T, FAR *LPMEMORY_SCAN_INFO;


#define LENG_SCAN_TOP_OF_MEMORY     0x00000001ul

#define MEM_SIG_LEN_CD          15
#define MEM_SIG_LEN_ALL         13          // Same for ALL and TOM signatures

#if defined(SYM_WIN32) || defined(SYM_NTK)
#pragma pack(push,1)
#else
#pragma pack(1)
#endif

typedef struct
{
    WORD            wVirusRecordIndex;
    BYTE            bySig[MEM_SIG_LEN_CD];
} MEM_CD_SIG_T, FAR *LPMEM_CD_SIG;

typedef struct
{
    WORD            wVirusRecordIndex;
    BYTE            bySig[MEM_SIG_LEN_ALL];
} MEM_ALL_SIG_T, FAR *LPMEM_ALL_SIG;

#if defined(SYM_WIN32) || defined(SYM_NTK)
#pragma pack(pop)
#else
#pragma pack()
#endif

///////////////////////////////////////////////////////////////////////////
//
// CRC scanner data file info
//
///////////////////////////////////////////////////////////////////////////

#define CRC_ON_N_BYTES          16  // CRC is computed on 16 bytes

#define CRC_NUM_VERIFY_BYTES    9

#if defined(SYM_WIN32) || defined(SYM_NTK)
#pragma pack(push,1)
#else
#pragma pack(1)
#endif

typedef struct
{
    WORD    wVirusRecordIndex;  // Index into virus record database
    WORD    wCRC;               // CRC on first 16 bytes of FSTART buffer
    BYTE    byVerifyLen;        // Number of verification bytes
    BYTE    byVerifyData[CRC_NUM_VERIFY_BYTES];
} CRC_SIG_DATA_T, FAR *LPCRC_SIG_DATA;

#if defined(SYM_WIN32) || defined(SYM_NTK)
#pragma pack(pop)
#else
#pragma pack()
#endif

///////////////////////////////////////////////////////////////////////////
//
// CRC scanner data structures
//
///////////////////////////////////////////////////////////////////////////

#define MAX_CRC_SIGS_PER_POOL   4096
#define CRC_POOL_SHIFT          12
#define CRC_POOL_MASK           0x0FFF

#define MAX_CRC_POOLS           (65536UL / MAX_CRC_SIGS_PER_POOL)

typedef struct
{
	WORD            wNumSigs;           // # sigs in this pool
	LPCRC_SIG_DATA  lpCRCSigData;       // Pointer to data
} CRC_POOL_ENTRY_T, FAR *LPCRC_POOL_ENTRY;

typedef struct
{
	WORD                wNumSigs;       // Total # sigs in all pools
	WORD                wNumCRCPools;
    CRC_POOL_ENTRY_T    CRCPoolArray[MAX_CRC_POOLS];
} CRC_SCAN_INFO_T, FAR *LPCRC_SCAN_INFO;

///////////////////////////////////////////////////////////////////////////
//
// Virus information data structures
//
///////////////////////////////////////////////////////////////////////////

// Information flags

#define bitINFO_COMFILE         0x0001
#define bitINFO_EXEFILE         0x0002
#define bitINFO_SYSFILE         0x0004
#define bitINFO_COMCOM          0x0008
#define bitINFO_FLOPBOOT        0x0010
#define bitINFO_HDISKBOOT       0x0020
#define bitINFO_MBOOTREC        0x0040
#define bitINFO_DIR2            0x0080
#define bitINFO_RESIDENT        0x0100
#define bitINFO_SIZESTLTH       0x0200
#define bitINFO_FULLSTLTH       0x0400
#define bitINFO_ENCRYPT         0x0800
#define bitINFO_POLYMORPH       0x1000
#define bitINFO_TRIGGERED       0x2000
#define bitINFO_INTHEWILD       0x4000
#define bitINFO_MULTIPARTY      0x8000

// definition types

#define AVENGE_TYPE_CRC                         0x01
#define AVENGE_TYPE_ALG                         0x02
#define AVENGE_TYPE_BOO                         0x04
#define AVENGE_TYPE_MAC                         0x08

#define AVENGE_TYPE_HAS_REPAIR                  0x20
#define AVENGE_TYPE_DELETED                     0x40
#define AVENGE_TYPE_VIRSCAN2_REPAIR             0x80

#define AVENGE_TYPE_NOCLEAN                     0x100
#define AVENGE_TYPE_NOT_IN_TSR                  0x200
#define AVENGE_TYPE_NO_GENERIC                  0x400
#define AVENGE_TYPE_EXTENSION_LOW               0x800
#define AVENGE_TYPE_EXTENSION_HIGH              0x1000

#define MAX_VIRUS_NAME_LENGTH       256

#define NAME_INDEX_POINTER_SIZE     3

typedef struct
{
    HVIRUS  hVirus;

    WORD    wVirusID;
    DWORD   dwInfo;
    WORD    wInfSize;
    WORD    wFlags;

    char    sVirusName[MAX_VIRUS_NAME_LENGTH];
} VIRUS_USER_INFO_T, FAR *LPVIRUS_USER_INFO;

// data file structures...

#if defined(SYM_WIN32) || defined(SYM_NTK)
#pragma pack(push,1)
#else
#pragma pack(1)
#endif

typedef struct
{
    WORD    wVirusID;       // VIRUS ID # (only for NAVEX/PAM)
    DWORD   dwInfo;         // Information flags for NAV virus info
    WORD    wInfSize;       // Virus infection size
    WORD    wRepairIndex;   // Index to repair information
    WORD    wFlags;         // CRC/ALG/BOO/MAC (repair or not)
} VIRUS_INFO_T, FAR *LPVIRUS_INFO;

#if defined(SYM_WIN32) || defined(SYM_NTK)
#pragma pack(pop)
#else
#pragma pack()
#endif

///////////////////////////////////////////////////////////////////////////
//
// Top level AVENGE data structures
//
///////////////////////////////////////////////////////////////////////////

// Engine scan file initialization flags

#define GENG_INIT_FILE_FLAG_LOAD_ALL            0x00000001UL
#define GENG_INIT_FILE_FLAG_LOAD_WILD           0x00000002UL


// boot init flags

#define BOOT_INIT_NO_DISK_ACCESS                0x00000001UL


// Engine load status flags

#define GENG_FLAG_MEMORY_LOADED                 0x00000001UL
#define GENG_FLAG_FILE_WILD_LOADED              0x00000002UL
#define GENG_FLAG_FILE_CRC_LOADED               0x00000004UL
#define GENG_FLAG_FILE_ALL_LOADED               0x00000008UL
#define GENG_FLAG_BOOT_LOADED                   0x00000010UL
#define GENG_FLAG_BOOT_INFO_LOADED              0x00000020UL
#define GENG_FLAG_UNINITIALIZED                 0


// mutex defines

#define MUTEX_FREE                              0
#define MUTEX_USED                              1


///////////////////////////////////////////////////////////////////////////
//
// Virus information cache
//
///////////////////////////////////////////////////////////////////////////

#define INFO_CACHE_MUTEX_UNLOCKED    0
#define INFO_CACHE_MUTEX_LOCKED      1

#define VIRUS_INFO_CACHE_SIZE       16

#define INVALID_INFO_CACHE_LRU       0

typedef struct
{
    HVIRUS          hVirus;
    VIRUS_INFO_T    stVirusInfo;
    char            sVirusName[MAX_VIRUS_NAME_LENGTH];
    DWORD           dwLRUValue;
} VIRUS_INFO_CACHE_REC_T, FAR *LPVIRUS_INFO_CACHE_REC;


typedef struct
{
    VIRUS_INFO_CACHE_REC_T  stData[VIRUS_INFO_CACHE_SIZE];
    DWORD                   dwLRUValue;
    LPIPCCALLBACKS          lpIPCCallBacks;
    LPVOID                  lpvMutexInfo;
} VIRUS_INFO_CACHE_T, FAR *LPVIRUS_INFO_CACHE;

///////////////////////////////////////////////////////////////////////////
//
// Algorithmic scanner structures/defines/etc
//
///////////////////////////////////////////////////////////////////////////

#if defined(SYM_WIN32) || defined(SYM_NTK)
#pragma pack(push,1)
#else
#pragma pack(1)
#endif

typedef struct
{
    WORD                wFourByteHashSize;
    WORD                wTwoByteHashSize;
    WORD                wOneByteHashSize;
    WORD                wNumSignatures;
} HASH_INFO_T, FAR *LPHASH_INFO;

#define sHASH_INFO sizeof(HASH_INFO_T)

typedef struct
{
    WORD wVirusRecordIndex;         // Index into virus record database
    BYTE bySigLength;               // Length of signature
} ALG_SIG_HDR_T, FAR *LPALG_SIG_HDR;

// MAX_ALG_SIG_DATA_SIZE is sum of sizes of VirusRecordIndex,
//  SigLen, and SigData

#define MAX_ALG_SIG_DATA_SIZE   (sizeof(ALG_SIG_HDR_T) + \
                                 255 * sizeof(BYTE))

typedef struct
{
    WORD wVirusRecordIndex;         // Index into virus record database
    BYTE bySigLength;               // Length of signature
	BYTE bySigData[1];    			// Signature bytes
} ALG_SIG_DATA_T, FAR *LPALG_SIG_DATA, FAR * FAR *LPLPALG_SIG_DATA;

#if defined(SYM_WIN32) || defined(SYM_NTK)
#pragma pack(pop)
#else
#pragma pack()
#endif

#define MASK_HIGH_NIBBLE                0xF0
#define MASK_LOW_NIBBLE                 0x0F
#define WILDCARD_HIGH_NIBBLE            0xD0
#define JUMP_1_BYTE                     0xEB
#define JUMP_CONDITIONAL_HIGH_NIBBLE    0x70

// algorithmic functions

#define ALG_DF_FUNC                     0xDF

#define ALG_FUNC_QUICK_SLIDE            0x00
#define ALG_FUNC_SKIP_1                 0x01
#define ALG_FUNC_SKIP_2                 0x02
#define ALG_FUNC_SKIP_3                 0x03
#define ALG_FUNC_SKIP_4                 0x04
#define ALG_FUNC_SKIP_5                 0x05
#define ALG_FUNC_SKIP_6                 0x06
#define ALG_FUNC_SKIP_7                 0x07
#define ALG_FUNC_SKIP_8                 0x08
#define ALG_FUNC_SKIP_9                 0x09
#define ALG_FUNC_SLIDE                  0x0A
#define ALG_FUNC_SEEK_TOP               0x0B
#define ALG_FUNC_SKIP_FORWARD_N         0x0C
#define ALG_FUNC_SKIP_BACK_N            0x0D
#define ALG_FUNC_MATCH_HIGH_NIBBLE      0x0E

#define ALGX_BASE                       0x10

#define ALGX_FUNC_SCAN_HEADER           0x10
#define ALGX_FUNC_FOLLOWJMP2ABS         0x11
#define ALGX_FUNC_DIRECTJMP2ABS         0x12
#define ALGX_FUNC_SEEKBACK              0x13
#define ALGX_FUNC_FOLLOWJMPHEADERE9     0x14
#define ALGX_FUNC_STRINGSEARCH          0x15
#define ALGX_FUNC_UNUSED_06             0x16        // was Tremor detection
#define ALGX_FUNC_UNUSED_07             0x17        // was Satan Bug detection
#define ALGX_FUNC_UNUSED_08             0x18        // was TPE.Girafe
#define ALGX_FUNC_MATCH_LOW_NIBBLE      0x19
#define ALGX_FUNC_DETECT_DX             0x1A
#define ALGX_FUNC_FOLLOWJMP1            0x1B
#define ALGX_FUNC_EXEC                  0x1C
#define ALGX_FUNC_RET_FALSE             0x1D
#define ALGX_FUNC_UNUSED_0E             0x1E        // used to be continue
#define ALGX_FUNC_UNUSED_0F             0x1F
#define ALGX_FUNC_ENCRYPTED_SEARCH      0x20
#define ALGX_FUNC_UNUSED_11             0x21
#define ALGX_FUNC_UNUSED_12             0x22
#define ALGX_FUNC_FOLLOWJUMP            0x23
#define ALGX_FUNC_SCANMASK              0x24
#define ALGX_FUNC_ENTRYPOINT_IMAGE      0x25
#define ALGX_FUNC_ENTRYPOINT_FILESIZE   0x26

#define QUICK_SLIDE_DISTANCE            0x10

#define ALGX_SCAN_NOT_MASK              0x04
#define ALGX_SCAN_NEG_MASK              0x08
#define ALGX_SCAN_ROTATE_MASK           0x10
#define ALGX_SCAN_ADD_DELTA_MASK        0x20
#define ALGX_SCAN_XOR_DELTA_MASK        0x40
#define ALGX_SCAN_WORD_MASK             0x80

#define ALGX_ENCRYPTED_BYTE_SIG_SKIP    7
#define ALGX_ENCRYPTED_WORD_SIG_SKIP    11

// the following #defines are used to skip over algorithmic functions...
// The -1's are because the loop being used auto-increments the index..

#define ALG_QUICK_SLIDE_LEN             (2)
#define ALG_SKIP_1_9_LEN                (1)
#define ALG_SLIDE_LEN                   (3)
#define ALG_SEEK_TOP_LEN                (1)
#define ALG_SKIP_FORWARD_N_LEN          (2)
#define ALG_SKIP_BACK_N_LEN             (2)
#define ALG_MATCH_HIGH_NIBBLE_LEN       (2)

#define ALGX_SCAN_HEADER_LEN            (2)
#define ALGX_FOLLOWJMP2ABS_LEN          (2)
#define ALGX_DIRECTJMP2ABS_LEN          (4)
#define ALGX_SEEKBACK_LEN               (4)
#define ALGX_FOLLOWJMPHEADERE9_LEN      (2)
#define ALGX_STRINGSEARCH_LEN           (4)     // arbitrary (4 core bytes)
#define ALGX_UNUSED_06_LEN              (2)
#define ALGX_UNUSED_07_LEN              (2)
#define ALGX_UNUSED_08_LEN              (2)
#define ALGX_MATCH_LOW_NIBBLE_LEN       (3)
#define ALGX_DETECT_DX_LEN              (3)
#define ALGX_FOLLOWJMP1_LEN             (2)
#define ALGX_EXEC_LEN                   (4)
#define ALGX_RET_FALSE_LEN              (2)
#define ALGX_UNUSED_0E_LEN              (2)
#define ALGX_UNUSED_0F_LEN              (2)
#define ALGX_ENCRYPTED_SEARCH_LEN       (4)     // arbitrary (4 core bytes)
#define ALGX_UNUSED_11_LEN              (2)
#define ALGX_UNUSED_12_LEN              (2)
#define ALGX_FOLLOWJUMP_LEN             (2)
#define ALGX_SCANMASK_LEN               (5)
#define ALGX_ENTRYPOINT_IMAGE_LEN       (6)
#define ALGX_ENTRYPOINT_FILESIZE_LEN    (6)

#define ALG_FUNC_BASE                   0xD0

///////////////////////////////////////////////////////////////////////////
//
// Boot scanning data structures/defines
//
///////////////////////////////////////////////////////////////////////////

#define BOO_SIG_LEN             14

#if defined(SYM_WIN32) || defined(SYM_NTK)
#pragma pack(push,1)
#else
#pragma pack(1)
#endif

typedef struct
{
    WORD wVirusRecordIndex;         // Index into virus record database
    BYTE bySigData[BOO_SIG_LEN];    // Signature bytes
} BOO_SIG_DATA_T, FAR *LPBOO_SIG_DATA;

#if defined(SYM_WIN32) || defined(SYM_NTK)
#pragma pack(pop)
#else
#pragma pack()
#endif

///////////////////////////////////////////////////////////////////////////
//
// File repair structures/defines
//
///////////////////////////////////////////////////////////////////////////

#define CTRL_GENERIC_COM        0x00000001UL
#define CTRL_GENERIC_EXE        0x00000002UL
#define CTRL_PREPENDING         0x00000004UL
#define CTRL_REBUILD_EXE        0x00000008UL
#define CTRL_IMPROVED           0x00000010UL
#define CTRL_SECONDBOOK         0x00000200UL
#define CTRL_SLIDER             0x00000400UL
#define CTRL_DECRYPT            0x00000800UL
#define CTRL_BOOT_REPAIR        0x00002000UL
#define CTRL_NOCLEAN            0x00008000UL

#define CTRL2_CUT_BEGIN         0x00010000UL
#define CTRL2_TWEEK_CS_10       0x00020000UL
#define CTRL2_TWEEK_SS_10       0x00040000UL
#define CTRL2_BOOK_FROM_BEGIN   0x00080000UL
#define CTRL2_COM_DISTINCT      0x00100000UL
#define CTRL2_EXEC_CODE         0x00200000UL
#define CTRL2_DECRYPT_XOR       0x00400000UL
#define CTRL2_DECRYPT_ADD       0x00800000UL
#define CTRL2_DECRYPT_WORD      0x01000000UL
#define CTRL2_NO_GENERIC        0x02000000UL
#define CTRL2_DISPLAY_DIALOG    0x04000000UL
#define CTRL2_NOT_IN_TSR        0x08000000UL

#define CTRL2_BIT_FLAG2G        0x10000000UL    // PAM repair...
#define CTRL2_BIT_FLAG2H        0x20000000UL
#define CTRL2_BIT_FLAG2I        0x40000000UL
#define CTRL2_BIT_FLAG2J        0x80000000UL

#define EXE_REPAIR_MASK         (CTRL_GENERIC_EXE | CTRL_REBUILD_EXE)
#define COM_REPAIR_MASK         (CTRL_GENERIC_COM | CTRL_PREPENDING | \
                                 CTRL_IMPROVED)


#define DECRYPT_MEM_SIG_SIZE        8

#define SLIDE_DISTANCE              32          // +/- 32 bytes

#define BOOK_MARK_BUFFER_SIZE       (512+SLIDE_DISTANCE)

#define MAX_BOOK_OFFSET             512

#define BOOK_OFFSET_NO_CHECK_MASK   0x8000U

typedef void (*LPUPDATEBYTEKEY)(LPBYTE lpbyKey, BYTE byDelta);
typedef void (*LPDECRYPTBYTE)(LPBYTE lpbyData, BYTE byKey);

typedef void (*LPUPDATEWORDKEY)(LPWORD lpwKey, WORD wDelta);
typedef void (*LPDECRYPTWORD)(LPWORD lpwData, WORD wKey);

#if defined(SYM_WIN32) || defined(SYM_NTK)
#pragma pack(push,1)
#else
#pragma pack(1)
#endif

typedef struct
{
    WORD    wCRC;               // for iterating through CRC repairs
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
    DWORD   dwRepairFlags;          // CRC type
    WORD    wMemSigIndex;           // for repairs that rely on mem sigs
    WORD    wInfSize;               // Virus infection size
} VIRUS_REPAIR_T, FAR *LPVIRUS_REPAIR;

#if defined(SYM_WIN32) || defined(SYM_NTK)
#pragma pack(pop)
#else
#pragma pack()
#endif

///////////////////////////////////////////////////////////////////////////
//
// Boot scanner data structures
//
///////////////////////////////////////////////////////////////////////////

// used for VxD to cache all records

typedef struct tag_BOOT_INFO_LIST
{
    HVIRUS                      hVirus;
    VIRUS_INFO_T                stVirusInfo;
    VIRUS_REPAIR_T              stVirusRepair;
    BYTE                        byDecryptSig[DECRYPT_MEM_SIG_SIZE];
    LPSTR                       lpszVirusName;

    struct tag_BOOT_INFO_LIST * lpstNext;
} BOOT_INFO_LIST_T, FAR *LPBOOT_INFO_LIST;

typedef struct
{
    MEMSIGGROUP_T           stBOO;
    LPBOOT_INFO_LIST        lpstBootInfoList;
} BOOT_SCAN_INFO_T, FAR *LPBOOT_SCAN_INFO;

#define PARTITION_OFFSET        446
#define PARTENTRY_SIZE           16
#define PARTITION_ACTIVE       0x80
#define MAX_PARTITIONS            4
#define SECTOR_SIZE             512

// -------------------
// MBR Partition Entry
// -------------------

typedef struct tagPARTENTRY
{
    BOOL    bActive;
    BYTE    byStartSide;
    BYTE    byStartSector;
    WORD    wStartCylinder;
    BYTE    byFileSystem;
    BYTE    byEndSide;
    BYTE    byEndSector;
    WORD    wEndCylinder;
    DWORD   dwRelativeSectors;
    DWORD   dwTotalSectors;
} PARTENTRY, FAR * LPPARTENTRY;

// --------------------------------
// Partition entry file system IDs
// Snagged from core/include/disk.h
// --------------------------------
                                        // ----------------------------------
#define  PARTFS_NOT_USED         0x00   // none
#define  PARTFS_DOS_12           0x01   // DOS, 12-bit Fat
#define  PARTFS_XENIX1           0x02   // XENIX
#define  PARTFS_XENIX2           0x03   // XENIX
#define  PARTFS_DOS_16           0x04   // DOS, 16-bit Fat
#define  PARTFS_EXTENDED         0x05   // DOS, Extended
#define  PARTFS_BIGDOS           0x06   // DOS, 32Meg+ Partition
#define  PARTFS_HPFS             0x07   // HPFS partition
#define  PARTFS_OS2_BOOT_MGR     0x0A   // OS/2 boot manager
#define  PARTFS_DOS_32           0x0B   // DOS, 32-Bit Fat
#define  PARTFS_DOS_32_LBA       0x0C   // same as 0x0B w/ LBA int 13 extensions
#define  PARTFS_BIGDOS_LBA       0x0E   // same as 0x06 w/ LBA int 13 extensions
#define  PARTFS_EXTENDED_LBA     0x0F   // same as 0x05 w/ LBA int 13 extensions
#define  PARTFS_DM_RO            0x50   // Disk Manager R/O
#define  PARTFS_DM_RW            0x51   // Disk Manager R/W
#define  PARTFS_GOLDEN_BOW       0x56   // Golden Bow partition
#define  PARTFS_SS_UNKNOWN1      0x61   // SpeedStor partition
#define  PARTFS_386IX            0x63   // Interactive's 386/IX
#define  PARTFS_NETWARE_286      0x64   // Netware 286
#define  PARTFS_NETWARE_386      0x65   // Netware 386
#define  PARTFS_PCIX             0x75   // PCIX
#define  PARTFS_EVEREX_SUPERDOS  0xAA   // Everex Super DOS partitions <= 285M
#define  PARTFS_CPM              0xDB   // CPM
#define  PARTFS_SS_12BIT         0xE1   // SpeedStor 12-bit
#define  PARTFS_SS_UNKNOWN2      0xE3   // SpeedStor partition
#define  PARTFS_SS_16BIT         0xE4   // SpeedStor 16-bit
#define  PARTFS_SS_EXTENSION     0xF1   // SpeedStor Extension
#define  PARTFS_SS_LARGE         0xF4   // SpeedStor LARGE
#define  PARTFS_BBT              0xFF   // Bad Block Table (Unix)


///////////////////////////////////////////////////////////////////////////
//
// Global Engine Context
//
///////////////////////////////////////////////////////////////////////////

typedef struct tagGENG
{
    LPDATAFILECALLBACKS     lpDataFileCallBacks;    // Datafile callbacks
    LPPROGRESSCALLBACKS     lpProgressCallBacks;    // Progress callbacks
    LPGENERALCALLBACKS      lpGeneralCallBacks;     // General callbacks
    LPIPCCALLBACKS          lpIPCCallBacks;         // IPC callbacks

    DWORD                   dwInitStatus;           // bits to specify
                                                    // initialization status

    DWORD                   dwGlobalCookie;         // our global cookie

    ALG_SCAN_INFO_T         stWildALGScanInfo;      // Wild ALG scanning data
    ALG_SCAN_INFO_T         stGeneralALGScanInfo;   // General ALG scanning data
    CRC_SCAN_INFO_T         stCRCScanInfo;          // General CRC scanning data

    LPMEMORY_SCAN_INFO      lpMemoryScanInfo;
    LPBOOT_SCAN_INFO        lpBootScanInfo;

    VIRUS_INFO_CACHE_T      stVirusInfoCache;
} GENG;

typedef GENG FAR  *HGENG;
typedef HGENG FAR *LPHGENG;


///////////////////////////////////////////////////////////////////////////
//
// Local Engine Context
//
///////////////////////////////////////////////////////////////////////////

#define TEMP_BUFFER_SIZE        4096

typedef struct tagLENG
{
    HGENG                   hGEng;

    DWORD                   dwLocalCookie;

    ALGSCANDATA_T           stAlgScanData;
    FSTARTINFO_T            stFStartInfo;

    BYTE                    byTempBuffer[TEMP_BUFFER_SIZE];
} LENG;

typedef LENG FAR  *HLENG;
typedef HLENG FAR *LPHLENG;

///////////////////////////////////////////////////////////////////////////
//
// Data structures for virus list functions
//
///////////////////////////////////////////////////////////////////////////

#define NAME_BUFFER_SIZE    16384

typedef struct
{
    LPVOID  lpvFileInfo;
    BOOL    bNoFileHandle;
    LPBYTE  lpbyNameBuffer;
    DWORD   dwDataLeft;
    DWORD   dwBufferLeft;
    DWORD   dwBufferOffset;
    DWORD   dwSectionStart;
    DWORD   dwSectionOffset;
} NAME_LOAD_INFO_T, FAR *LPNAME_LOAD_INFO;

#define MAX_RECS_TO_HOLD        256
#define MAX_INFREC_BUFFER_SIZE  (MAX_RECS_TO_HOLD * sizeof(VIRUS_INFO_T))

typedef struct
{
    LPVOID  lpvFileInfo;
    BOOL    bNoFileHandle;
    LPBYTE  lpbyInfRecBuffer;
    DWORD   dwTotalRecsLeft;
    DWORD   dwTotalIndex;
    DWORD   dwBufferRecsLeft;
    DWORD   dwBufferIndex;
    DWORD   dwSectionStart;
    DWORD   dwRecSize;
    DWORD   dwRecsInBuffer;
} INFREC_LOAD_INFO_T, FAR *LPINFREC_LOAD_INFO;

typedef struct
{
    INFREC_LOAD_INFO_T      stInfRecInfo;
    NAME_LOAD_INFO_T        stNameInfo;
} BULK_LOAD_INFO_T, FAR *LPBULK_LOAD_INFO;



///////////////////////////////////////////////////////////////////////////
//
// Prototypes of exports
//
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
//
// Global Engine Context Initialization and Close
//
///////////////////////////////////////////////////////////////////////////

ENGSTATUS EngGlobalInit
(
    LPHGENG             lphGEng,
    LPGENERALCALLBACKS  lpGeneralCallBacks,
    LPPROGRESSCALLBACKS lpProgressCallBacks,
    LPDATAFILECALLBACKS lpDataFileCallBacks,
    LPIPCCALLBACKS      lpIPCCallBacks,
    DWORD               dwGlobalCookie
);

ENGSTATUS EngGlobalClose
(
    HGENG               hGEng
);


///////////////////////////////////////////////////////////////////////////
//
// Global Engine Context Memory Initialization and Close
//
///////////////////////////////////////////////////////////////////////////

ENGSTATUS EngGlobalMemoryInit
(
    HGENG               hGEng
);


ENGSTATUS EngGlobalMemoryClose
(
    HGENG               hGEng
);


///////////////////////////////////////////////////////////////////////////
//
// Global Engine Context Boot Initialization and Close
//
///////////////////////////////////////////////////////////////////////////

#ifndef SYM_NLM
ENGSTATUS EngGlobalBootInit
(
    HGENG               hGEng,
    DWORD               dwFlags
);

ENGSTATUS EngGlobalBootClose
(
    HGENG               hGEng
);
#endif

///////////////////////////////////////////////////////////////////////////
//
// Global Engine Context File Initialization and Close
//
///////////////////////////////////////////////////////////////////////////

// Engine scan file initialization flags

#define GENG_INIT_FILE_FLAG_LOAD_ALL            0x00000001UL
#define GENG_INIT_FILE_FLAG_LOAD_WILD           0x00000002UL

ENGSTATUS EngGlobalFileInit
(
    HGENG               hGEng,
    DWORD               dwFlags
);

ENGSTATUS EngGlobalFileClose
(
    HGENG               hGEng
);


///////////////////////////////////////////////////////////////////////////
//
// Local Engine Context Initialization and Close
//
///////////////////////////////////////////////////////////////////////////

ENGSTATUS EngLocalInit
(
    HGENG               hGEng,
    LPHLENG             lphLEng,
    DWORD               dwLocalCookie
);

ENGSTATUS EngLocalClose
(
    HLENG               hLEng
);


///////////////////////////////////////////////////////////////////////////
//
// Local Engine Context Scanning
//
///////////////////////////////////////////////////////////////////////////

// Engine scan file flags

#define LENG_SCAN_FILE_FLAG_SCAN_ONLY_WILD      0x00000001UL
#define LENG_SCAN_FILE_FLAG_SCAN_ALL            0x00000002UL

ENGSTATUS EngScanFile
(
    HLENG               hLEng,
    LPFILEOBJECT        lpFileObject,
    DWORD               dwFlags,
    DWORD               dwCookie,
    LPBOOL              lpbFoundSig,
    LPHVIRUS            lphVirus
);

ENGSTATUS EngScanMemory
(
    HLENG               hLEng,
    LPMEMORYOBJECT      lpMemoryObject,
    DWORD               dwStartAddress,
    DWORD               dwEndAddress,
    DWORD               dwFlags,
    DWORD               dwMemCookie,
    LPBOOL              lpbFoundSig,
    LPHVIRUS            lphVirus
);


#ifndef SYM_NLM
ENGSTATUS EngScanBoot
(
    HLENG       hLEng,
    LPBYTE      lpbyBootBuffer,
    DWORD       dwBufSize,
    LPBOOL      lpbFoundSig,
    LPHVIRUS    lphVirus
);
#endif

///////////////////////////////////////////////////////////////////////////
//
// Local Engine Context Repair
//
///////////////////////////////////////////////////////////////////////////

ENGSTATUS EngRepairFile
(
    HLENG               hLEng,              // temporary buffers and callbacks
    HVIRUS              hVirus,
    LPFILEOBJECT        lpFileObject,       // file object to repair
    LPBOOL              lpbFileRepaired     // OUT: was the file repaired?
);

#ifndef SYM_NLM
ENGSTATUS EngRepairBoot
(
    HLENG               hLocalEng,          // temporary buffers and callbacks
    HVIRUS              hVirus,
    LPLINEAROBJECT      lprLinear,          // linear object to repair
    BYTE                byPartNumber,       // partition to repair
    LPBOOL              lpbRepaired         // OUT: was the object repaired?
);
#endif

///////////////////////////////////////////////////////////////////////////
//
// Global Engine Context Information Retrieval
//
///////////////////////////////////////////////////////////////////////////

ENGSTATUS EngGetNumVirusRecords
(
    HGENG               hGEng,
    LPWORD              lpwNumVirusRecords
);

ENGSTATUS EngGetNumClaimedViruses
(
    HGENG               hGEng,
    LPDWORD             lpdwNumClaimedViruses
);

ENGSTATUS EngGetUserInfoIndex
(
    HGENG               hGEng,
    WORD                wIndex,
    LPVIRUS_USER_INFO   lpVirusUserInfo
);

ENGSTATUS EngGetVirusName
(
    HGENG               hGEng,
    HVIRUS              hVirus,
    LPSTR               lpstrVirusName,
    LPVOID              lpvProvidedFileInfo
);

ENGSTATUS EngGetUserInfoHVIRUS
(
    HGENG               hGEng,
    HVIRUS              hVirus,
    LPVIRUS_USER_INFO   lpVirusUserInfo
);

ENGSTATUS EngVIDToHVIRUS
(
    HGENG               hGEng,
    WORD                wVID,
    LPHVIRUS            lphVirus,
    LPBOOL              lpbFound
);

ENGSTATUS EngGetN30HVIRUS
(
    HLENG               hLEng,
    HVIRUS              hVirus,
    LPN30               lpstN30
);

ENGSTATUS EngDeleteVirusEntry
(
    HGENG               hGEng,
    HVIRUS              hVirus
);

ENGSTATUS EngCopyFile
(
    LPFILEOBJECT        lpSourceObject,
    LPFILEOBJECT        lpDestObject,
    LPBYTE              lpbyWorkBuffer,
    WORD                wBufferSize
);

#ifndef SYM_NLM
ENGSTATUS EngGetDiskType
(
    LPLINEAROBJECT   lprLinear,         // [in] linear object to analyze
    LPWORD           lpwDiskType        // [out] Disk Type
);

VOID EngParseMBRPartition
(
    LPBYTE      lpbyMBR,                // [in] Entire MBR
    BYTE        byPartNumber,           // [in] Partition to parse (0 - 3)
    LPPARTENTRY lprPartEntry            // [out] filled out PARTENTRY structure
);
#endif

ENGSTATUS EngGetInfoVID
(
    HGENG               hGEng,
    WORD                wVID,
    LPHVIRUS            lphVirus,
    LPVIRUS_INFO        lpVirusInfo,
    LPSTR               lpstrVirusName,
    LPBOOL              lpbFound
);

///////////////////////////////////////////////////////////////////////////
//
// Virus list functions
//
///////////////////////////////////////////////////////////////////////////

ENGSTATUS EngInitNameLoad
(
    HGENG                   hGEng,
    LPVOID                  lpvFileInfo,
    LPNAME_LOAD_INFO        lpstNLI
);

ENGSTATUS EngGetNextName
(
    HGENG                   hGEng,
    LPNAME_LOAD_INFO        lpstNLI,
    LPSTR                   lpszName
);

ENGSTATUS EngCloseNameLoad
(
    HGENG                   hGEng,
    LPNAME_LOAD_INFO        lpstNLI
);


ENGSTATUS EngInitInfRecLoad
(
    HGENG                       hGEng,
    LPVOID                      lpvFileInfo,
    LPINFREC_LOAD_INFO          lpstNLI
);

ENGSTATUS EngGetNextInfRec
(
    HGENG                       hGEng,
    LPINFREC_LOAD_INFO          lpstNLI,
    LPVIRUS_INFO                lpstVirusInfo,
    LPBOOL                      lpbDone
);

ENGSTATUS EngCloseInfRecLoad
(
    HGENG                       hGEng,
    LPINFREC_LOAD_INFO          lpstNLI
);


ENGSTATUS EngInitBulkLoad
(
    HGENG               hGEng,
    LPBULK_LOAD_INFO    lpstBLI
);

ENGSTATUS EngGetNextN30
(
    HGENG                   hGEng,
    LPBULK_LOAD_INFO        lpstBLI,
    LPN30                   lpstN30,
    LPBOOL                  lpbDone
);

ENGSTATUS EngCloseBulkLoad
(
    HGENG               hGEng,
    LPBULK_LOAD_INFO    lpstBLI
);


#endif


