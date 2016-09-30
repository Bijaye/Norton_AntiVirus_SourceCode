//************************************************************************
//
// $Header:   S:/ENGINE15/VCS/navexshr.h_v   1.1   28 Jul 1997 17:24:24   DCHI  $
//
// Description:
//      Contains ENGINE 1.5 code shared among all modules.
//
//************************************************************************
// $Log:   S:/ENGINE15/VCS/navexshr.h_v  $
// 
//    Rev 1.1   28 Jul 1997 17:24:24   DCHI
// Synced up with NAVEX project AnsiNext stuff.
// 
//    Rev 1.0   15 May 1997 18:23:22   jsulton
// Initial revision.
//************************************************************************

#ifndef _NAVEXSHR_H

#define _NAVEXSHR_H

// make sure to define our VIDs

//#include "navexvid.h"

#if defined(SYM_NLM) || defined(SYM_NTK)

#define SEEK_SET    0
#define SEEK_CUR    1
#define SEEK_END    2

#endif

#define READ        0
#define WRITE       1
#define READ_WRITE  2

// Error Stuff

#define WERROR          0xffff          // WORD error indicator
#define DWERROR         0xffffffff      // DWORD error indicator
#define UERROR          ((UINT) -1)     // UINT error indicator

// Partition Table Stuff

#define PART_TAB_OFF        0x1be   // Offset of partition table
#define MAX_PART_ENTS       4       // Maximum partition table entries
#define CHECK_VAL_OFF       0x1fe   // Offset of 0xaa55 for validity checking
				    // Also used in boot sectors
#define GOODSEC             0xaa55  // Good boot and MBRs end with this

typedef struct {
    BYTE    status;                 // Bootable (0x80) or not (0x00)
    BYTE    st_head;                // Starting head
    WORD    st_cylsec;              // Starting cylinder/sector value
    BYTE    type;                   // Partition type (FAT12, FAT16, etc.)
    BYTE    en_head;                // Ending head
    WORD    en_cylsec;              // Ending cylinder/sector value
    DWORD   st_abssec;              // Starting absolute sector
    DWORD   sectors;                // Number of sectors in partition
    }   PART_ENT;

#define PART_ENT_SIZE   sizeof(PART_ENT)    // Size of partition table entry

// Boot Sector Offsets
// These are offsets into the boot sector for important bits of data

#define OEM_NAME_OFF        0x03    // OEM Name
#define BYTESPERSEC_OFF     0x0b    // Bytes per Sector
#define SECTORSPERUNIT_OFF  0x0d    // Sectors per Allocation Unit
#define RESERVEDSECTORS_OFF 0x0e    // Reserved Sectors
#define NUM_FATS_OFF        0x10    // Number of FATs
#define NUMROOTDIRENTS_OFF  0x11    // Number of Root Directory Entries
#define OLDLOGVOLSEC_OFF    0x13    // Sectors in Logical Volume (DOS < 4.0)
#define MEDIA_BYTE_OFF      0x15    // Media Descriptor Byte
#define NUMSECSPERFAT_OFF   0x16    // Number of Sectors per FAT
#define SECSPERTRACK_OFF    0x18    // Number of Sectors per Track
#define NUMHEADS_OFF        0x1a    // Number of Heads
#define NUMHIDSECS_OFF      0x1c    // Number of Hidden Sectors
#define LOGVOLSECS_OFF      0x20    // Sectors in Logical Volume (DOS 4.0+)
#define PHYSDRVNUM_OFF      0x24    // Physical Drive Number
#define EXBOOTSIGREC_OFF    0x26    // Extended Boot Signature Record
#define VOLUME_ID_OFF       0x27    // Volume ID
#define VOLUME_LAB_OFF      0x2b    // Volume Label

// ... or you can use this structure at offset 0x0b for floppy BPBs
typedef struct tagFLOPPYBPB
{
    WORD        wBytesPerSector;        // Bytes per sector
    BYTE        bySectorsPerCluster;    // Sectors per cluster
    WORD        wReservedSectors;       // First sector of Fat
    BYTE        byNumberOfFats;         // FAT Copies
    WORD        wMaxRootEntries;        // Maximum entries in Root dir
    WORD        wTotalSectors;          // Total number of sectors
    BYTE        byMediaDescriptor;      // Media descriptor byte
    WORD        wSectorsPerFat;         // Sectors per Fat, 0 = 32 bit FAT
    WORD        wSectorsPerTrack;       // Sectors per Track
    BYTE        bySides;                // Number of sides
} FLOPPYBPB, FAR * LPFLOPPYBPB;

// --------------------------------------
// Drive Types used by NavexGetFloppyType
// --------------------------------------

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

// ----------------------------------------------------------------
// BPB used by NavexGetFloppyType() and EXTRepairBootDMFGeneric()
// ----------------------------------------------------------------

					// -------------
					// 360k Floppies
					// -------------
#define BPB_360K    { 0x00, 0x02, 0x02, 0x01, 0x00, 0x02, 0x70, 0x00, \
		      0xD0, 0x02, 0xFD, 0x02, 0x00, 0x09, 0x00, 0x02 }

					// -------------
					// 720k Floppies
					// -------------
#define BPB_720K    { 0x00, 0x02, 0x02, 0x01, 0x00, 0x02, 0x70, 0x00, \
		      0xA0, 0x05, 0xF9, 0x03, 0x00, 0x09, 0x00, 0x02 }

					// -------------
					// 1.2M Floppies
					// -------------
#define BPB_12M     { 0x00, 0x02, 0x01, 0x01, 0x00, 0x02, 0xE0, 0x00, \
		      0x60, 0x09, 0xF9, 0x07, 0x00, 0x0F, 0x00, 0x02 }

					// --------------
					// 1.44M Floppies
					// --------------
#define BPB_144M    { 0x00, 0x02, 0x01, 0x01, 0x00, 0x02, 0xE0, 0x00, \
		      0x40, 0x0B, 0xF0, 0x09, 0x00, 0x12, 0x00, 0x02 }

					// --------------
					// 2.88M Floppies
					// --------------
#define BPB_288M    { 0x00, 0x02, 0x02, 0x01, 0x00, 0x02, 0xF0, 0x00, \
		      0x80, 0x16, 0xF0, 0x09, 0x00, 0x24, 0x00, 0x02 }

					// -------------
					// 160k Floppies
					// -------------
#define BPB_160K    { 0x00, 0x02, 0x01, 0x01, 0x00, 0x02, 0x40, 0x00, \
		      0x40, 0x01, 0xFE, 0x01, 0x00, 0x08, 0x00, 0x01 }

					// -------------
					// 180k Floppies
					// -------------
#define BPB_180K    { 0x00, 0x02, 0x01, 0x01, 0x00, 0x02, 0x40, 0x00, \
		      0x68, 0x01, 0xFC, 0x02, 0x00, 0x09, 0x00, 0x01 }

					// -------------
					// 320k Floppies
					// -------------
#define BPB_320K    { 0x00, 0x02, 0x02, 0x01, 0x00, 0x02, 0x70, 0x00, \
		      0x80, 0x02, 0xFF, 0x01, 0x00, 0x08, 0x00, 0x02 }

					// -------------
					// 640k Floppies
					// -------------
#define BPB_640K    { 0x00, 0x02, 0x02, 0x01, 0x00, 0x02, 0x70, 0x00, \
		      0x00, 0x05, 0xFB, 0x02, 0x00, 0x08, 0x00, 0x02 }

					// --------------------------
					// 1.2M Floppies (1k sectors)
					// --------------------------
#define BPB_12M_2   { 0x00, 0x04, 0x01, 0x01, 0x00, 0x02, 0xC0, 0x00, \
		      0xD0, 0x04, 0xFE, 0x02, 0x00, 0x08, 0x00, 0x02 }

					// --------------------------------
					// 1.68M DMF Floppies (1k clusters)
					// --------------------------------
#define BPB_1KDMF   { 0x00, 0x02, 0x02, 0x01, 0x00, 0x02, 0x10, 0x00, \
		      0x20, 0x0D, 0xF0, 0x05, 0x00, 0x15, 0x00, 0x02 }

					// --------------------------------
					// 1.68M DMF Floppies (2k clusters)
					// --------------------------------
#define BPB_2KDMF   { 0x00, 0x02, 0x04, 0x01, 0x00, 0x02, 0x10, 0x00, \
		      0x20, 0x0D, 0xF0, 0x03, 0x00, 0x15, 0x00, 0x02 }


#define BPB_INDEX_360K      0x00
#define BPB_INDEX_720K      0x01
#define BPB_INDEX_12M       0x02
#define BPB_INDEX_144M      0x03
#define BPB_INDEX_288M      0x04
#define BPB_INDEX_160K      0x05
#define BPB_INDEX_180K      0x06
#define BPB_INDEX_320K      0x07
#define BPB_INDEX_640K      0x08
#define BPB_INDEX_12M_2     0x09
#define BPB_INDEX_1KDMF     0x0A
#define BPB_INDEX_2KDMF     0x0B

#define NUM_BPBs            0x0C

#define sBPB                0x10
					// -----------------------------
#define BPB_MATCH_THRESHOLD     7       // out of 10 fields causes match
					// -----------------------------


// -------------------------------------
// Miscellaneous defines for Boot Repair
// -------------------------------------

					// --------------
					// JMP 0040 & NOP
					// --------------
#define JMP_AND_NOP { 0xEB, 0x3E, 0x90 }
#define sJMP_AND_NOP 0x03

					// -------------------
					// OEM ID = "SYMANTEC"
					// -------------------
#define OEM_ID { "SYMANTEC" }
#define sOEM_ID 0x08

					// ----------------------------
					// Volume Label = "GENERICBOOT"
					// ----------------------------
#define VOLUME_LABEL { "GENERICBOOT" }
#define sVOLUME_LABEL 0x0B



#define NON_BOOTABLE_CODE { 0xFA, 0x33, 0xC0, 0x8E, 0xD0, 0xBC, 0x00, 0x7C, \
			    0x8E, 0xD8, 0x8E, 0xC0, 0xFB, 0xBE, 0x80, 0x7C, \
			    0xE8, 0x06, 0x00, 0x30, 0xE4, 0xCD, 0x16, 0xCD, \
			    0x19, 0xAC, 0x08, 0xC0, 0x74, 0x09, 0xB4, 0x0E, \
			    0xBB, 0x07, 0x00, 0xCD, 0x10, 0xEB, 0xF2, 0xC3 }

#define sNON_BOOTABLE_CODE 0x28

					// ----------------------------------
					// 0140 FA            CLI
					// 0141 33C0          XOR     AX,AX
					// 0143 8ED0          MOV     SS,AX
					// 0145 BC007C        MOV     SP,7C00
					// 0148 8ED8          MOV     DS,AX
					// 014A 8EC0          MOV     ES,AX
					// 014C FB            STI
					// 014D BE807C        MOV     SI,7C80
					// 0150 E80600        CALL    0159
					// 0153 30E4          XOR     AH,AH
					// 0155 CD16          INT     16
					// 0157 CD19          INT     19
					// 0159 AC            LODSB
					// 015A 08C0          OR      AL,AL
					// 015C 7409          JZ      0167
					// 015E B40E          MOV     AH,0E
					// 0160 BB0700        MOV     BX,0007
					// 0163 CD10          INT     10
					// 0165 EBF2          JMP     0159
					// 0167 C3            RET
					// ----------------------------------


#define NON_BOOTABLE_TEXT { "\0x13\0x10This is not a bootable diskette.\0x13\0x10Remove it and press a key to restart...\0x13\0x10" }
#define sNON_BOOTABLE_TEXT 0x4D

#define SECTOR_SIZE     512


// Infection Buffer Stuff

#define FSTART_OFF      0               // Offset of FSTART buffer
#define FSTART_LEN      256             // Length of FSTART buffer
#define HEADER_OFF      256             // Offset of file header
#define HEADER_LEN      64              // Length of file header
#define INFBUF_OFF      512             // Offset of infection buffer

// Aliases used by repair for N30 struct members and values.

#ifndef REP_EXEC_CODE

#define REP_EXEC_CODE   20
#define EXEC_CODE       0x20
#define wHardType       wInfSize
#define wHardLoc1       wHostJmp

#endif

// Prototypes for shared functions

// EXTScanDanish arguments:
//
// lpcallback           : Pointer to callback structure
// scanbuffer           : Buffer containing sector to scan
// lpwVID               : Pointer used to return VID of Danish Boot, if found
//
// Returns:
//
//  WORD                : EXTSTATUS_OK                  if no virus found
//                        EXTSTATUS_VIRUS_FOUND         if virus was found
//
//  This is called from EXTScanPartDanish and EXTScanBootDanish, and used to
// check for the Danish Boot virus (of course, you say).  It's the same
// algorithm in both cases, so I put it here to save space, and keep it
// maintainable.

WORD EXTScanDanish(LPCALLBACKREV1 lpcallback,
		    LPBYTE scanbuffer,
		    LPWORD lpwVID);

// ScanString arguments:
//
// lpbyBuffer1          : Buffer in which to search.
// cwBuffer1            : Count of bytes in Buffer1.
//                      : cwBuffer1 must be > cwBuffer2.
// lpbyBuffer2          : String to search for in Buffer1.
// cwBuffer2            : Count of bytes in Buffer2.
//                      : cwBuffer2 ust be >= 2 bytes.
//
// Returns:
//
//     WORD             : -1            no match
//                        n >= 0        offset of buffer2 in buffer1.

WORD ScanString (LPBYTE lpbyBuffer1,
		 WORD cwBuffer1,
		 LPBYTE lpbyBuffer2,
		 WORD cwBuffer2);

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
		     UINT FAR *lpuAttr);

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
		     UINT  uAttr);

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
			 long           lOffset);

// GetEntryPtOffset
//
// lpCallBack           : pointer to callback structure for FileSeek, etc.
// hFile                : File Handle of opened file to get entry point
// lpbyBuff             : Buffer to use for reading file header - should hold
//                        at least 0x18 bytes.
// Returns:
//      DWORD           : File offset of program entry point.  This is
//                        assumed to be 0 if the file doesn't begin w/ MZ, ZM,
//                        or 0xe9.      on success
//                      : DWERROR       on failure

DWORD GetEntryPtOffset (LPBYTE          lpbyBuff);

// ----------------
// Floppy functions
// ----------------

BOOL NavexGetFloppyType
(
    LPBYTE              lpbySector,     // [in] Boot sector to anaylze
    LPWORD              lpwDiskType     // [out] Disk type
);


BOOL NavexRepairBootGeneric
(
    LPBYTE  lpbyNewBootSector,          // [in/out] The generic boot sector
    WORD    wDiskType                   // [in] Disk type
);


// ************
//
// Handy Macros
//
// ************

// These save a lot of typing in most circumstances

#define FILEOPEN()  RepairFileOpen(lpCallBack, lpszFileName, &hFile, &uAttr)
#define FILECLOSE() RepairFileClose(lpCallBack, lpszFileName, hFile, uAttr)
#define FILEREAD(b, s)  lpCallBack->FileRead(hFile, b, s)
#define FILEWRITE(b, s) lpCallBack->FileWrite(hFile, b, s)
#define FILESEEK(p) lpCallBack->FileSeek(hFile, p, SEEK_SET)
#define FILESIZE()  lpCallBack->FileSize(hFile)
#define FILETRUNCATE(t) RepairFileTruncate(lpCallBack, hFile, t)
#define SEEK_TOF()  lpCallBack->FileSeek(hFile, 0, SEEK_SET)

// Very handy for error returns when file is already open

#define CLOSEOUT(rtarg)     { FILECLOSE(); return(rtarg); }

// EXE Header Stuff

#define EXEHDRSIG   0x4d5a
#define RDHEXESIG   0x5a4d

// NEW_UNIX follows

#ifndef EXEHEADER_DEFINED
#define EXEHEADER_DEFINED

typedef struct
    {
    WORD    exSignature;     // Sig of .EXE
    WORD    exExtraBytes;    // Number of extra bytes in last page
    WORD    exPages;         // Number of pages
    WORD    exRelocItems;    // Number of pointers in relocation table
    WORD    exHeaderSize;    // Size of header in paragraphs
    WORD    exMinAlloc;      // Minimun allocation
    WORD    exMaxAlloc;      // Max allocation
    WORD    exInitSS;        // Initial SS value
    WORD    exInitSP;        // Initial SP value
    WORD    exCheckSum;      // Complemented checksum
    WORD    exInitIP;        // Initial IP value
    WORD    exInitCS;        // Initial CS value
    WORD    exRelocTable;    // bytes offset to relocation table
    WORD    exOverlay;       // Overlay number
    WORD    exReserved[16];
    WORD    exExtendOffset;  // Location of EH
    } EXEHEADER;
#define sEXEHEADER  (sizeof(EXEHEADER)-34)
#define sEXELARGE   sizeof(EXEHEADER)

#endif // #ifndef EXEHEADER_DEFINED

// NEW_UNIX above

#define ISEXEHDR(b) ((EXEHDRSIG == AVDEREF_WORD(b)) || \
					    (RDHEXESIG == AVDEREF_WORD(b)))

// Pack stuff

#ifdef __MACINTOSH__
	#define PACK1           options align=packed
	#define PACK            options align=reset
#else
	#define PACK1           pack(1)
	#define PACK            pack()
#endif

// Special fix for NTK and VXD

#if defined(SYM_NTK) || defined(SYM_VXD)

#if defined(AnsiNext)
#undef AnsiNext
#endif

#if defined(AnsiPrev)
#undef AnsiPrev
#endif

#endif // #if defined(SYM_NTK) || defined(SYM_VXD)

// AnsiNext stuff - from old version of platform.h

#if !defined(SYM_WIN) || defined(SYM_NTK)       // ##DBCS - don't redefine

#if !defined(AnsiNext)
#   define AnsiNext(lp)      ((*((LPTSTR)lp)!='\0') ? (((LPTSTR)lp)+1) : ((LPTSTR)lp))
#   define AnsiPrev(lpS,lpC) (((LPTSTR)lpC>(LPTSTR)lpS) ? (((LPTSTR)lpC)-1) : ((LPTSTR)lpS))
#endif

#endif

#endif  // _NAVEXSHR_H
