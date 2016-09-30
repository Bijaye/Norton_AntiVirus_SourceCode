//**********************************************************************
// Copyright (C) 1992-1993 Symantec, Peter Norton Group
//**********************************************************************
//
// $Header:   S:/INCLUDE/VCS/cts4.h_v   1.93   14 May 1996 20:33:54   JWILBER  $
//
// Description: C include file for Certus library and scanner
//              MUST correspond to cts4.inc, the assembler equivalent!!!
//
//
//**********************************************************************
// $Log:   S:/INCLUDE/VCS/cts4.h_v  $
// 
//    Rev 1.93   14 May 1996 20:33:54   JWILBER
// Changed the offset of the non-resident name table in the NE header
// structure definition to a DWORD instead of a WORD.
//
//    Rev 1.92   14 Apr 1996 02:50:40   JWILBER
// Changed #define for sEXEHEADER to "(sizeof(EXEHEADER)-34)" instead
// of "sizeof(EXEHEADER)-66", which evaluated to -4 and caused errors.
//
//    Rev 1.91   28 Mar 1996 23:12:24   JWILBER
// Changed ";" for comments left by an assembly programmer into "//" for
// C programmers in BOOTLOCK #defines.
//
//    Rev 1.90   01 Jun 1995 13:47:06   DSACKING
// Ported changes from 3.0.6
//
//    Rev 1.89   27 Jan 1995 16:37:50   DALLEE
// Oh boy... This file's active again -- in use by everyone's favorite NAVTSR!
// #define MAX_PATH_SIZE and MAX_NET_PATH ifndef.
//
//    Rev 1.88   13 Jan 1995 16:37:16   MFALLEN
// Renamed EXCLUDEITEM to OLDEXCLUDEITEM and created a new EXCLUDEITEM structure
// that can handle LFNs.
//
//    Rev 1.87   29 Nov 1994 14:44:04   DALLEE
// Changes to compile on trunk.
//
//    Rev 1.86   29 Nov 1994 14:10:28   DALLEE
// Reget from QAK2 post NAV305.
//
//    Rev 1.70.1.8   24 Oct 1994 10:39:42   DALLEE
// Added defines for N30.wControl2 -- NO_GENERIC and DISPLAY_DIALOG.
// Made rude change to N30NOTES -- Changed wControl to wControl2.
//    No code ever read this value.
//    Only VIRLIST wrote to this value, and wControl2 will work as well.
//    Can't change the size since this is passed betweeen .DLL's.
//    Scanner code now needs to reference the wControl flags.
//
//    Rev 1.70.1.7   13 Oct 1994 21:10:58   SZIADEH
// prototyped GetInfectionData()
//
//    Rev 1.70.1.6   07 Oct 1994 19:05:50   SZIADEH
// added BufferOffset component to STARTCODE struct.
//
//    Rev 1.70.1.5   30 Mar 1994 10:30:28   PATRICKM
// Checked in from latest QAK2 src
//
//    Rev 1.70.1.4   08 Mar 1994 13:35:38   KEITH
// Bankers Trust mods
//
//    Rev 1.70.1.3   02 Mar 1994 18:18:04   DALLEE
// Added excREG_CHANGE and incREG_CHANGE.
//
//    Rev 1.70.1.2   16 Dec 1993 15:41:12   DALLEE
// Added parameter to SCN4_BOO for calling code chunks from VIRSCAN.DAT file.
//
//    Rev 1.70.1.1   01 Nov 1993 15:40:52   MFALLEN
// GetAnsiFlag() and SetAnsiFlag()
//
//    Rev 1.70.1.0   19 Oct 1993 11:30:52   SCOTTP
// Branch base for version QAK2
//
//    Rev 1.70   13 Oct 1993 17:17:52   BARRY
// Merged QAK1 changes
//**********************************************************************

#ifdef SYM_WIN32
#error The W32 platform should include CTSN.H, not CTS4.H
#else

#ifndef _CTS4_H
#define _CTS4_H

//------------------------------------------------------------------------------
// CTS4.H - Protos, equates, and structures for 'C'
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// EQUATE Section
//------------------------------------------------------------------------------
#define fREAD            0              // Open types
#define fWRITE           1
#define fREAD_WRITE      2
#define fCREATE          4
#define fAPPEND          8
                                        // Share bit flags
#define fSHARE_COMPAT     0x0000
#define fSHARE_EXCLUSIVE  0x0010
#define fSHARE_DENY_WRITE 0x0020
#define fSHARE_DENY_READ  0x0030
#define fSHARE_DENY_NONE  0x0040

#define fSEEK_BEG        1              // Used for Seeking handle in file
#define fSEEK_FOR        2
#define fSEEK_BAK        4
#define fSEEK_END        8

#define DIR_ENTRY_LEN    256            // MAX Length of directory entry

#define GET_INFO         0              // Get or set info flag
#define SET_INFO         1

//#define TRUE           1              // Typical T/F
//#define FALSE          0

#define MAXPAD           64             // Used for Get Start padding count

#ifndef MAX_PATH_SIZE
#define MAX_PATH_SIZE    144            // Max with of a DOS path
#endif

#ifndef MAX_NET_PATH
#define MAX_NET_PATH     256            // Maximum file/path length on network
#endif

#ifndef MAX_NET_PATH                    // Martin //
#define MAX_NET_PATH 256                // Maximum file/path length on network
#endif

#define PROTECT_ADD      0              // Used for File protection
#define PROTECT_TEST     1
#define PROTECT_DELETE   2
#define PROTECT_REPAIR   4
#define PROTECT_CREATE   8
#define PROTECT_QUICK_INOC 16

#define HIDDEN           2              // Used to search for files
#define SYSTEM           4
#define VOLLABEL         8
#define DIR              16

#define IS_COM           0              // Used for Get Start
#define IS_EXE           1
#define IS_SYS           2
#define IS_BCOM          3              // Bulgarian COM
#define IS_VCOM          4              // Verified COM
#define IS_ZIP           5
#define IS_LZEXE         6
#define IS_PKEXE         7
#define IS_WHALE         8
#define STARTSIZE        256

#define IS_FILE          0              // Used by Find All
#define IS_DIRECTORY     1
#define DIR_DEPTH        33             // How many levels of directories

#define MEM_LOCKED       0x01           // Memory flags
#define MEM_FIXED        0x02
#define MEM_READONLY     0x04
#define MEM_NORMAL       0x00

#define MEM_REQUEST_UMB  0x10
#define MEM_RELEASE_UMB  0x11
#define MEM_ALLOCATE_XMSU 0x09
#define MEM_FREE_XMS     0x0A
#define MEM_LOCK_XMS     0x0C
#define MEM_UNLOCK_XMS   0x0D

//#define CR             0x0D
//#define LF             0x0A

#define CL_FLAG              1              // /FLAG
#define CL_STRING            2              // "This string" or String
#define CL_FILENAME          2              // FILENAME
#define CL_NUMERIC           3              // /NUM 1234
#define CL_OTHER             4              // /STR "This string" or String
#define CL_HEX               5              // /HEX AB

//------------------------------------------------------------------------------
// ERROR EQUates
//------------------------------------------------------------------------------
#define FIL4_NOT_FOUND  1
#define REPAIR_FAIL     4
#define FILE_CHANGED    5
#define INVALID_INPUT   6

#define SELF_TEST_BUFFER_SIZE   512
                                        // When computing, if the self test
                                        // CRC == 0, fudge it to this value,
#define SELF_TEST_ZERO_CRC      1       // since 0 means SELF_TEST_NO_CRC.

#define SELF_TEST_SUCCESS       0       // Passed Self Test.
#define SELF_TEST_NO_CRC        1       // File has not been stamped.
#define SELF_TEST_FAIL          2       // CRC does not match.
#define SELF_TEST_FILE_ERROR    3       // Error reading file.


//------------------------------------------------------------------------------
// Virus detection equates
//------------------------------------------------------------------------------

#define CRCtype         0
#define ALGtype         1
#define BOOtype         2
#define MACtype         3               // Don't blame me, its Dennis' fault

#define SIZE_VIRUSNAME  (21 + 1)        // This includes the EOS character
#define SIZE_VINFO      32
#define SIZE_MEM        15
#define SIZE_SIG        17
#define SIZE_FIXED      64
#define SIZE_BOOTSIG    14
#define SIZE_COMMENT    ((5 * 60) + 1)  //5 lines of 60 chars per line + EOS
#define SIZE_ALIAS      40
#define SIZE_CRC        (sizeof(WORD))
#define SIZE_CHECK_DATA (sizeof(CHECKDATA))     // Check + control word

#define uSizeVirscanHeader  256

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
#define bitCTRL_NOTCLEAN    0x8000

#define bitDSSI         0x0001  // File name located in DS:SI not DS:DX
#define bitINT13        0x0002  // If use an INT 13 directly use this flag
#define bitTSR          0x8000  // If set TSR doing work else UI
#define bitWINDOWS      0x4000  // If set Windows version doing work else DOS

// bit Setting for Buttons
#define bitREPAIR       0x0001
#define bitDELETE       0x0002
#define bitINOCULATE    0x0004
#define bitREINOCULATE  0x0008
#define bitCONTINUE     0x0010
#define bitSTOP         0x0020
#define bitEXCLUDE      0x0040

#define bitAUTOREPAIR   0x0100
#define bitAUTODELETE   0x0200
#define bitAUTOINOCUL   0x0400

#ifdef BANKERS
#define bitREG          0x1000
#define bitREREG        0x2000
#endif

#define excVIOLATING    0x0002  // Program executing is doing the violation
#define excVIRUS_FOUND  0x0004  // Virus found
#define excINOC_CHANGE  0x0008  // File changed
#define excFILE_WATCH   0x0010
#define excLOW_FORMAT   (0x0020 + excVIOLATING)
#define excWR_HBOOT     (0x0040 + excVIOLATING)
#define excWR_FBOOT     (0x0080 + excVIOLATING)
#define excWR_ATLIST    (0x0100 + excVIOLATING)
#define excRO_CHANGE    (0x0200 + excVIOLATING)
#define excREG_CHANGE   0x0400

// Inclusion database flags (for BANKERS file registration).
#define incREG_CHANGE   0x0001


#define ObjectAtList    1
#define ObjectLogName   2
#define ObjectNetUsers  3
#define ObjectExcept    4
#define ObjectLogCMB    5
#define ObjectPath      6
#define ObjectMessCMB   7
#define ObjectN30       8
#define ObjectNetMess   9
#define ObjectUserName  10
#define ObjectNetINOC   11
#define ObjectScanAll   12
#define ObjectChkBothFl 13
#define ObjectRemoveAft 14
#define ObjectSeconds   15
#define ObjectBeepTsr   16
#define ObjectAlertUsr  17
#define ObjectAlertCon  18
#define ObjectAlrtKnown 19
#define ObjectAlrtUnkno 20
#define ObjectAlrtInocu 21
#define ObjectAlrtVLike 22
#define ObjectActKnown  23
#define ObjectActUnknow 24
#define ObjectActInocu  25
#define ObjectActVLike  26
#define ObjectBackupRep 27
#define ObjectBackupExt 28
#define ObjectAlertMsg  29
#define ObjectInocName  30
#define ObjectUnLevel   31              // Level of flags for file watch

//--------------------------------------------------------------------------
// BOOTLOCK equates
//--------------------------------------------------------------------------
#define BPB_BYTES               0x24
#define BPB_SIGNATURE           0x00    // Offsets into te BPB for info
#define BPB_OEM                 0x03
#define BPB_BYTES_SECTOR        0x0B
#define BPB_SECT_PER_ALLOC      0x0D
#define BPB_RESERVED_SECT       0x0E
#define BPB_NUM_FATS            0x10
#define BPB_NUM_ROOT_ENTRY      0x11
#define BPB_TOTAL_SECT          0x13
#define BPB_MEDIA_DESCRIPT      0x15
#define BPB_NUM_SECT_PER_FAT    0x16
#define BPB_SECT_PER_TRACK      0x18
#define BPB_NUM_HEADS           0x1A
#define BPB_NUM_HIDDEN          0x1C
#define BPB_TOTAL_32            0x20
#define BC_CYLINDER             0x11A
#define BC_HEAD                 0x11B
#define BC_SECTOR               0x11C
#define BPB_OFFSET              0x0b     // offset into the boot sector
#define PARTITION_OFFSET        0x1be    // from begin of MBR
#define global_offset           0x7C00   // Global offset for boot code
#define int13address            0x4C     // offest of int 13h in IVT
#define ISR13seg                0x09FE0  // where to stick our 13h
#define ISR13offset             0x0000   // ditto

//--------------------------------------------------------------------------
// BEEP Level defines
//--------------------------------------------------------------------------
#define BEEP_UNDEFINED          20
#define BEEP_KNOWNBOX           5
#define BEEP_FBOOTBOX           5
#define BEEP_INOCBOX            15
#define BEEP_REINOCBOX          7
#define BEEP_AUDITERROR         12
#define BEEP_INOCERROR          10
#define BEEP_BOOTPROTECT        10
#define BEEP_PTABPROTECT        10
#define BEEP_SYSFILEPROTECT     10
#define BEEP_WBOOTSCAN          10
#define BEEP_DURATION           1
#define BEEP_MONBOX             1


//---------------------------------------------------------------------------
// FIXED ID equates
//---------------------------------------------------------------------------
#define FID_MTE         2      //  1 Value is offset in word array
#define FID_POGUE       4      //  2 converted to zerobase.(-1)*2
#define FID_WHALE       6      //  3 Thus ID 1 = 0, 2 = 2, 3 = 4, 4 = 6, 5 = 8
#define FID_TPE         8      //  4 Fixed ID
#define FID_V2PX        10     //  5
#define FID_CASCADE     12     //  6 For Repair Decrypt
#define FID_TEQUILA     14     //  7 For Repair Decrypt
#define FID_MALTESE     16     //  8
#define FID_PX2000      18     //  9
#define FID_NOINT       20     // 10 For Mem detect
#define FID_FLIP2343    22     // 11 For Repair Decrypt
#define FID_FLIP2153    24     // 12 For Repair Decrypt
#define FID_DIR2        26     // 13 For custom repair
#define FID_TREMOR      28     // 14 Detection
#define FID_TODOR       30     // 15 Detection
#define FID_GROOVE      32     // 16 Detection

//----------------------------------------------------------------------------
// Control Word 2 defines
//----------------------------------------------------------------------------

#define NO_GENERIC      0x200
#define DISPLAY_DIALOG  0x400
#define NOT_IN_TSR      0x800

//----------------------------------------------------------------------------
// Netware API equates
//----------------------------------------------------------------------------
#define NOV_MAXMESSLEN  55      // max length of broadcast messages

//----------------------------------------------------------------------------
// Character conversion equates
//----------------------------------------------------------------------------
#define CHR_LOWER       0       // convert character to lowercase
#define CHR_UPPER       1       // convert character to uppercase


//******************************************************************************
// STRUCT Section
//******************************************************************************
//------------------------------------------------------------------------------
// File protection used with Inoculation/FileWatch/SysCheck
//------------------------------------------------------------------------------
typedef struct
    {
#ifdef SYM_NLM
    char    Filename[MAX_NET_PATH+30];
#else
    char    Filename[MAX_PATH_SIZE+1];
#endif
    BYTE    Fileattr;
    WORD    Filetime;
    WORD    Filedate;
    DWORD   Filesize;
    BYTE    Header[35];
    BYTE    Version;
    WORD    wReserved1;
    WORD    wReserved2;
    WORD    StartCRC;
    WORD    CRCAllBuf;
    WORD    CRCLastK;
    } FILE_PROTECT;
typedef FILE_PROTECT FAR * LPFILE_PROTECT;
#define sFILE_PROTECT   sizeof(FILE_PROTECT)

//------------------------------------------------------------------------------
// Format of a standard Disk Transfer Area
//------------------------------------------------------------------------------
typedef struct
    {
        BYTE    DTA_Search[21];
        BYTE    DTA_Attr;
        WORD    DTA_Time;
        WORD    DTA_Date;
        WORD    DTA_SizeL;
        WORD    DTA_SizeH;
        BYTE    DTA_FILENAME[13];
    } DISK_TA,DTA_;
#define sDISK_TA EQU sizeof(DISK_TA)
#define DTA_SIZE sizeof(DTA_)

//------------------------------------------------------------------------------
// Header of a Hashed Database structure
//------------------------------------------------------------------------------
typedef struct
    {
    DWORD   Free_Pointer;
    WORD    Elem_Size;
    WORD    Total_Elem_Size;
    DWORD   IndexSize;
    DWORD   DataStart;
    DWORD   IndexStart;
    DWORD   QuickStart;
    DWORD   Comp_Func;
    DWORD   Hash_Func;
    DWORD   Quick_Func;
    WORD    FileHandle;
    } DBC4_HEADER;
typedef DBC4_HEADER FAR * LPDBC4_HEADER;
#define sDBC4_HEADER    sizeof(DBC4_HEADER)

//------------------------------------------------------------------------------
// Type of chaining used in hash table from DBC4_HEADER
//------------------------------------------------------------------------------
typedef struct
    {
    DWORD   Next;
    BYTE    Active;
    } CHAIN_TYPE;
#define sCHAIN_TYPE     sizeof(CHAIN_TYPE)

//------------------------------------------------------------------------------
// Structure for Get Start code routine
//------------------------------------------------------------------------------
typedef struct
    {
    BYTE    Filetype;
    DWORD   FirstLanding;
    DWORD   BufferOffset;
    BYTE    Padding;
    } STARTCODE;
#define sSTARTCODE      sizeof(STARTCODE)

//------------------------------------------------------------------------------
// Initialization for GET_ME_MEMORY
//------------------------------------------------------------------------------
typedef struct
    {
    WORD    memTotal;        // Total bytes wanted in paragraphs
    WORD    memUMB;          // Total bytes wanted in UMB paragraphs
    WORD    memEMS;          // Total bytes wanted in EMS paragraphs
    WORD    memHMA;          // Total bytes wanted in HMA paragraphs
    WORD    memCONV;         // Total bytes wanted in CONV paragraphs
    WORD    memlpCONV;       // Pointer to CONV to use
    WORD    memDATA;         // Total bytes wanted in DATA paragraphs
    WORD    memlpDATA;       // Pointer to DATA to use
    WORD    memXMS;          // Total bytes wanted in XMS
    DWORD   memlpFilename;   // Pointer to temp file to create
    } INIT_MEMORY;
#define sINIT_MEMORY    sizeof(INIT_MEMORY)

//------------------------------------------------------------------------------
// GET_ME_MEMORY structure
//------------------------------------------------------------------------------
typedef struct
    {
    WORD    lpUMB;           // Segment of UMB memory
    WORD    lpCONV;          // Segment of CONV memory
    WORD    lpDATA;          // Segment of DATA memory
    DWORD   lpHMA;           // Pointer to HMA memory
    WORD    hEMS;            // Handle to EMS memory
    WORD    hXMS;            // Handle to XMS memory
    WORD    hSwapFile;       // Handle to swap file
    WORD    uLastHandle;     // Last handle allocated
    WORD    uFreeHandle;     // Known freed handle
    } GMEMORY;
#define sGMEMORY    sizeof(GMEMORY)

//------------------------------------------------------------------------------
// Local memory struct for allocation
//------------------------------------------------------------------------------
typedef struct
    {
    WORD    hHandle;         // Handle to memory
    WORD    uSize;           // Size of memory in paragraphs
    WORD    lpNEXT;          // Segment of next memory
    WORD    lpPREV;          // Segment of previous memory
    WORD    uFlags;          // Flags of this memory seg
    } LMEMORY;
#define sLMEMORY    sizeof(LMEMORY)

//-----------------------------------------------------------------------------
// Real Mode structure for calling bios interrupts
//-----------------------------------------------------------------------------
typedef struct
    {
    DWORD  RM_DI;
    DWORD  RM_SI;
    DWORD  RM_BP;
    DWORD  RM_RES;
    DWORD  RM_BX;
    DWORD  RM_DX;
    DWORD  RM_CX;
    DWORD  RM_AX;
    WORD   RM_FL;
    WORD   RM_ES;
    WORD   RM_DS;
    WORD   RM_FS;
    WORD   RM_GS;
    WORD   RM_IP;
    WORD   RM_CS;
    WORD   RM_SP;
    WORD   RM_SS;
    } REALMODE;
#define sREALMODE   sizeof(REALMODE)

//------------------------------------------------------------------------------
// Structure for EXE header
//------------------------------------------------------------------------------
typedef struct
    {
        WORD    exSignature;     // Sig of .EXE
        WORD    exExtraBytes;    // Number of extra bytes in last page
        WORD    exPages;             // Number of pages
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

//------------------------------------------------------------------------------
// NE Header structure
//------------------------------------------------------------------------------
typedef struct
    {
    WORD    neSignature;     // Sig of the WINDOWS.EXE
    BYTE    neLinkVersion;   // Version of LINK used
    BYTE    neLinkRevision;  // Revision of LINK used
    WORD    neEntryOffset;   // Offset to entry table from EH Header
    WORD    neEntryLength;   // Length of entry table
    WORD    neNextDLL;       // Reserved for windows
    WORD    neReserved1;     // Reserved
    WORD    neExeFlags;      // Flags describing EXE information
    WORD    neAutoDataNumb;  // Number of auto data segment
    WORD    neHeapSize;      // Initial size of heap
    WORD    neStackSize;     // Initial size of stack
    DWORD   neCSIP;          // Segment:Offset of CS:IP
    DWORD   neSSSP;          // Segment:Offset of SS:SP
    WORD    neSegTabNumb;    // Number of entries in the seg table
    WORD    neModRefNumb;    // Number of entries in the Mod ref table
    WORD    neNonresNumb;    // Number of entries in the NonResName table
    WORD    neSegTabOff;     // Relative offset to the segment table
    WORD    neResourceOff;   // Relative offset to the resource table
    WORD    neResidentOff;   // Relative offset to the ResidentName table
    WORD    neModRefOff;     // Relative offset to the ModuleRef table
    WORD    neImportOff;     // Relative offset to the ImportedName table
    DWORD   neNonresOff;     // Relative (beg of file) to Nonres name table
    WORD    neMovable;       // Number of movable entry points
    WORD    neShift;         // Shift count to align the logical sectors
    WORD    neResourceNumb;  // Number of resource segments
    BYTE    neOSFlags;       // Flags describing the OS System
    BYTE    neExeFlags2;     // Flags describing EXE information
    DWORD   neFastSector;    // Offset of fast load area in sectors
    WORD    neFastLength;    // Length of fast load area in sectors
    WORD    neVersion;       // Windows version number
    } NEHEADER;
#define sNEHEADER           sizeof(NEHEADER)

//------------------------------------------------------------------------------
// NE Support structures
//------------------------------------------------------------------------------
typedef struct
    {
    WORD    neSegDataSec;    // Offset, in sectors, to the segment data (relative to begin of file)
    WORD    neSegLength;     // Segment length in file
    WORD    neSTFlags;       // Segment table flags
    WORD    neSegMin;        // Minimum size of segment
    } NESEGMENT_TABLE;
#define sNESEGMENT_TABLE            sizeof(NESEGMENT_TABLE)

typedef struct
    {
    WORD    neModuleOffset;  // Offset to names in imported-name table
    } NEMODULE_REFERENCE;
#define sNEMODULE_REFERENCE sizeof(NEMODULE_REFERENCE)

typedef struct
    {
    BYTE    neBundleCount;   // Number of entries in bundle, 0 = END
    BYTE    neBundleFlag;    // Type of bundle (FF=Fixed, FE=Movable)
    } NEENTRY_TABLE;
#define sNEENTRY_TABLE      sizeof(NEENTRY_TABLE)

typedef struct
    {
    BYTE    neMovableFlag;   // Flags for movable code
    WORD    neMovable3Fh;    // The INTERRUPT 3F
    BYTE    neMovableSeg;    // Specifies the segment number
    WORD    neMovableOff;    // Specifies the segment offset
    } NEENTRY_MOVABLE;
#define sNEENTRY_MOVABLE    sizeof(NEENTRY_MOVABLE)

typedef struct
    {
    BYTE    neFixedFlag;     // Flags for fixed code
    WORD    neFixedOffset;   // Specifies an offset
    } NEENTRY_FIXED;
#define sNEENTRY_FIXED  sizeof(NEENTRY_FIXED)

typedef struct
    {
    WORD    neCount;         // Number of relocations
    } NERELOCATION_HEADER;
#define sNERELOCATION_HEADER    sizeof(NERELOCATION_HEADER)

typedef struct
    {
    BYTE    neAddressType;   // Type of address to fix
    BYTE    neRelType;       // Type of relocation to fix
    WORD    neRelFixMe;      // Offset needing to be fixed
    WORD    neIndexModule;   // Index to module reference table
    WORD    neOrdalNumber;   // Ordial number
    } NEREL_IMP_NAME;
#define sNEREL_IMP_NAME sizeof(NEREL_IMP_NAME)

typedef struct
    {
    BYTE    neAddressType;   // Type of address to fix
    BYTE    neRelType;       // Type of relocation to fix
    WORD    neRelFixMe;      // Offset needing to be fixed
    WORD    neIndexModule;   // Index to module reference table
    WORD    neNameOffset;    // Offset in name table
    } NEREL_IMP_ORDIAL;
#define sNEREL_IMP_ORDIAL   sizeof(NEREL_IMP_ORDIAL)

typedef struct
    {
    BYTE    neAddressType;   // Type of address to fix
    BYTE    neRelType;       // Type of relocation to fix
    WORD    neRelFixMe;      // Offset needing to be fixed
    BYTE    neSegmentNumber; // Segment number
    BYTE    Empty;           // Always a 0
    WORD    neOffset;        // Offset in segment
    } NEREL_INT_FIXED;
#define sNEREL_INT_FIXED    sizeof(NEREL_INT_FIXED)

typedef struct
    {
    BYTE    neAddressType;   // Type of address to fix
    BYTE    neRelType;       // Type of relocation to fix
    WORD    neRelFixMe;      // Offset needing to be fixed
    BYTE    neFF;            // Always a 0FFh
    BYTE    ne0;             // Always a 0
    WORD    neOrdial;        // Ordial to segment entry table
    } NEREL_INT_MOVE;
#define sNEREL_INT_MOVE sizeof(NEREL_INT_MOVE)

//------------------------------------------------------------------------------
// Dynamic loading of a DLL for DOS structure
//------------------------------------------------------------------------------
typedef struct
    {
    DWORD   lpSegment;       // Pointer to segment table
    DWORD   lpResource;      // Pointer to resource table
    DWORD   lpResident;      // Pointer to resident name table
    DWORD   lpModuleRef;     // Pointer to module reference table
    DWORD   lpImport;        // Pointer to import name table
    DWORD   lpEntry;         // Pointer to entry table
    DWORD   lpNonres;        // Pointer to non resident table
    DWORD   lpNextDLL;       // Pointer to next loaded DLL
    WORD    hFIlehandle;     // Handle to DLL to load from
    } DYNA_HEADER;
#define sDYNA_HEADER    sizeof(DYNA_HEADER)

//------------------------------------------------------------------------------
// Register structure for TSR work
//------------------------------------------------------------------------------
typedef struct
    {
        WORD    r_ES;
        WORD    r_DS;
        UINT    r_DI;
        UINT    r_SI;
        UINT    r_BP;
        UINT    r_SP;
        UINT    r_BX;
        UINT    r_DX;
        UINT    r_CX;
        UINT    r_AX;
    } TSR_REGS;
#define sTSR_REGS       sizeof(TSR_REGS)

//-----------------------------------------------------------------------------
// THE TSR Structure
//-----------------------------------------------------------------------------
typedef struct
    {
        TSR_REGS FAR *lpRegs;      // Register structure, not used in here
    DWORD   lpBuffer;    // Work buffer
    DWORD   lpExtra;     // Extra text buffer pointer
    WORD    uSizeBuffer; // Size of work buffer
    WORD    uSizeExtra;  // Size of extra buffer
    WORD    uMessage1;   // Message to place for user
    WORD    uQuestion1;  // Question 1 to ask
    WORD    uMessage2;   // Message to place for user if first failed.
    WORD    uQuestion2;  // Question 2 to ask
    WORD    uLogBase;    // Base to add to log
    LPSTR   lpData;      // Pointer to data for function
    WORD    uLFlags;     // Flags for messages (FLAGS1)
    WORD    uCurrPSP;    // Current Program Segment Prefix
    LPSTR   lpGlobal;    // Extension of GLOBAL Data
    DWORD   lpCallBack;  // Extension of CALLBACK Data
    } TSR_STRUCT;
#define sTSR_STRUCT     sizeof(TSR_STRUCT)

//-----------------------------------------------------------------------------
typedef struct
    {
    WORD    Interrupt;   // Interrupt to intercept
    WORD    Registers;   // Kernel function to hook
    BYTE    Multi;       // Set to TRUE if uses previous TSR_STRUCT
    BYTE    EVDLL[20];   // EV Function DLL
    BYTE    EVFunc[40];  // EV Function
    BYTE    DataDLL[20]; // Data dll
    BYTE    DataFunc[40];// Data build routine
    } HOOK;
#define sHOOK       sizeof(HOOK)

//-----------------------------------------------------------------------------
typedef struct
    {
    BYTE    Header[128];
    BYTE    EndOvl;
    BYTE    Signature[4];
    UINT    uNumStructs;
    UINT    uNumHooks;
    UINT    uNumCallbacks;
    BYTE    bUSEUMB;
    BYTE    bUSEEMS;
    BYTE    bUSEXMS;
    UINT    uTsrID;         // Id number for interrupt 2F
    UINT    uTsrSize;
    UINT    uWorkBuffer;
    UINT    uExtraBuffer;
    BYTE    GlobalDLL[20];
    BYTE    GlobalFunc[40];
    BYTE    EndOvl2;
    } BINHEADER;
#define sBINHEADER      sizeof(BINHEADER)

//-----------------------------------------------------------------------------
// Event Verify Structure
//-----------------------------------------------------------------------------
typedef struct
    {
        WORD    evINTERRUPT;     // Interrupt
        WORD    evFUNCTION;          // Function, wild cards = ff
        FARPROC evROUTINE;       // Pointer to far routine
        TSR_STRUCT FAR *evTSR_STRUCT;        // Pointer to TSR_STRUCT for routine
    } EV;
#define sEV         sizeof(EV)

//------------------------------------------------------------------------------
// Structure for callbacks in OVL file
//------------------------------------------------------------------------------
typedef struct
    {
    BYTE    CallbackDLL[20]; // DLL Name
    BYTE    CallbackFunc[40];// Function name
    } EVCALLBACK;
#define sEVCALLBACK sizeof(EVCALLBACK)

//------------------------------------------------------------------------------
// Structure for CMB file
//------------------------------------------------------------------------------
typedef struct
    {
        DWORD   dwPointer;              // Pointer to where in file message is
        UINT    uMessageSize;           // Size of this message
    } MESSAGE_HEADER;
#define sMESSAGE_HEADER     sizeof(MESSAGE_HEADER)


//-----------------------------------------------------------------------------
// Window Structure
//-----------------------------------------------------------------------------
typedef struct
    {
    WORD    wRow;
    WORD    wColumn;
    WORD    wWidth;
    WORD    wHeigth;
    BYTE    wAttrib;
    DWORD   wBuffer;
    DWORD   wVideo;
    } WINDOW;
#define sWINDOW     sizeof(WINDOW)

//-----------------------------------------------------------------------------
// NOVELL Request/Reply structures
//-----------------------------------------------------------------------------
typedef struct
    {
    WORD    Request;
    BYTE    id;
    BYTE    MessLen;
    BYTE    Message[55];
    WORD    Reply;
    } Novell_SendConsole;
#define sSendConsole_Reply    2
#define sSendConsole_Request  sizeof(Novell_SendConsole) - sSendConsole_Reply

typedef struct
    {
    WORD    Request;
    BYTE    id;
    BYTE    Count;
    BYTE    UserID;
    BYTE    MessLen;
    BYTE    Message[55];
    WORD    Reply;
    BYTE    Cnt;
    BYTE    Result[2];
    } Novell_SendMessage;
#define sSendMessage_Reply      5
#define sSendMessage_Request    sizeof(Novell_SendMessage) - sSendMessage_Reply

typedef struct
    {
    WORD    Request;
    BYTE    id;
    BYTE    Blank;
    BYTE    ObjType;
    BYTE    ObjLen;
    BYTE    ObjName[47];
    WORD    Reply;
    BYTE    NumCon;
    BYTE    ConList[100];
    } Novell_GetUserNumb ;
#define sGetUserNumb_Reply      103
#define sGetUserNumb_Request    sizeof(Novell_GetUserNumb) - sGetUserNumb_Reply

typedef struct
    {
    WORD    Request;
    BYTE    id;
    BYTE    ConNumb;
    WORD    Reply;
    DWORD   ObjID;
    BYTE    Blank;
    BYTE    ObjType;
    BYTE    ObjName[48];
    BYTE    LogTime[7];
    } Novell_GetUserName ;
#define sGetUserName_Reply      63
#define sGetUserName_Request    sizeof(Novell_GetUserName) - sGetUserName_Reply

typedef struct
    {
    WORD    Request;
    BYTE    id;
    DWORD   LastObj;
    BYTE    B1;
    BYTE    ObjType;
    BYTE    ObjLen;
    BYTE    ObjName;
    WORD    Reply;
    DWORD   ObjID;
    BYTE    B2;
    BYTE    rType;
    BYTE    rText[48];
    BYTE    OFlag;
    BYTE    OSec;
    BYTE    OProp;
    } Novell_ScanBindery ;
#define sScanBindery_Reply      59
#define sScanBindery_Request    sizeof(Novell_ScanBindery) - sScanBindery_Reply

//------------------------------------------------------------------------------
// Command line stuff
//------------------------------------------------------------------------------
typedef struct
    {
    BYTE    CL_Type;         // Type of command line option
    DWORD   CL_MASK;         // Look for this
    DWORD   CL_STORE;        // Where to store value
    } COMMANDLINE;
#define sCOMMANDLINE    sizeof(COMMANDLINE)

//------------------------------------------------------------------------------
// Virus database structes
//------------------------------------------------------------------------------
typedef struct
    {
    WORD    wVirID;          // Virus ID Number
    WORD    wType;           // Type
    WORD    wCrc;            // CRC
    WORD    wControl;        // Control word for Cleaning
    WORD    wInfo;           // Information Word
    WORD    wFixedID;        // Fixed algo virus ID number

    char    lpVName[SIZE_VIRUSNAME]; // Virus Name
    BYTE    lpMem[SIZE_MEM];  // Memory Signature
    BYTE    lpSig[SIZE_SIG];  // Regular Signature

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
   WORD    uNumALGs;        // # of Algorythmic virus signatures
   WORD    uNumBOOs;        // # of Boot virus signatures
   WORD    uNumCRCs;        // # of CRC virus signatures
   WORD    uNumREMs;        // # of signatures which are turned off
   WORD    wFixedIDs[SIZE_FIXED];  // N30 Record Numbers
   } HEADER;

typedef HEADER FAR *LPHEADER;
#define sHEADER sizeof(HEADER)

typedef struct tagCHECKDATA
    {

    WORD    wCheckData;         // check data
    WORD    wControl;           // control data

    } CHECKDATA;


typedef BYTE VIRUSNAME[SIZE_VIRUSNAME];

typedef CHECKDATA FAR *LPCHECKDATA;
typedef VIRUSNAME FAR *LPVIRUSNAME;

typedef struct
    {
    char    lpVirName[SIZE_VIRUSNAME];
    WORD    wVirID;
    WORD    wType;
    WORD    wControl2;
    WORD    wInfo;
    WORD    wSize;
    } N30NOTES, FAR *LPN30NOTES;
#define sN30NOTES   sizeof(N30NOTES)

//------------------------------------------------------------------------------
// SDA Information block
//------------------------------------------------------------------------------
typedef struct
    {
    DWORD   lpFrom;
    DWORD   lpTo;
    DWORD   lpINDOS;
    WORD    uSmall;
    WORD    uLarge;
    } SDA;
#define sSDA    sizeof(SDA)

//----------------------------------------------------------------------------
// Structure for Bios Parameter Block (BPB)
//-----------------------------------------------------------------------------
typedef struct
  {
  WORD   BytesPerSec;                  // bytes per sector
  BYTE   SecPerClust;                  // sectors per cluster
  WORD   ResSectors;                   // number of reserved sectors
  BYTE   FATs;                         // number of file allocation tables
  WORD   RootDirEnts;                  // number of root-directory entries */
  WORD   Sectors;                      // total number of sectors (if 0 refer to HugeSectors
  BYTE   Media;                        // media descriptor
  WORD   FATsecs;                      // number of sectors per FAT
  WORD   SecPerTrack;                  // sectors per track
  WORD   Heads;                        // number of heads
  DWORD  HiddenSecs;                   // number of hidden sectors
  DWORD  HugeSectors;                  // number of sectors if Sectors == 0
  } BPB_struct;
#define sBPB_struct   sizeof(BPB_struct)

//----------------------------------------------------------------------------
// Structure for Partition Table Entries
//----------------------------------------------------------------------------
typedef struct
  {
  BYTE   peBootable;                   // 80h - bootable, 00h = nonbootable
  BYTE   peBeginHead;                  // beginning head
  BYTE   peBeginSector;                // beginning sector
  BYTE   peBeginCylinder;              // beginning cylinder
  BYTE   peFileSystem;                 // name of file system
  BYTE   peEndHead;                    // ending head
  BYTE   peEndSector;                  // ending sector
  BYTE   peEndCylinder;                // ending cylinder
  DWORD  peStartSector;                // starting sector (relative to beg. of disk)
  DWORD  peSectors;                    // number of sectors in partition
  } PARTENTRY;
#define sPARTENTRY      sizeof(PARTENTRY)

//---------------------------------------------------------------------------
// Drive Parameter Block (DPB)
//---------------------------------------------------------------------------
typedef struct
{
  BYTE dpbDrive;                        // drive number (0=A,1=B,etc.)
  BYTE dpbUnit;                         // unit number for drive
  WORD dpbSectorSize;                   // sector size, in bytes
  BYTE dpbClusterMask;                  // sectors per cluster - 1
  BYTE dpbClusterShift;                 // sectors per cluster, as power of 2
  WORD dpbFirstFAT;                     // first sector containing FAT
  BYTE dpbFATCount;                     // number of FATs
  WORD dpbRootEntries;                  // number of root-directory entries
  WORD dpbFirstSector;                  // first sector of first cluster
  WORD dpbMaxCluster;                   // number of clusters on drive + 1
  WORD dpbFATSize;                      // number of sectors occupied by FAT
  WORD dpbDirSector;                    // first sector containing director
  DWORD dpbDriverAddr;                  // address of device driver
  BYTE dpbMedia;                        // media descriptor
  BYTE dpbFirstAccess;                  // indicates access to the drive
  DWORD dpbNextDPB;                     // address of next drive paramter block
  WORD dpbNextFree;                     // last allocated cluster
  WORD dpbFreeCnt;                      // number of free cluster
} DPB;
#define sDPB    sizeof(DPB)

//------------------------------------------------------------------------------
// Exclude structure to read/write/test exclude bin file
//------------------------------------------------------------------------------

                                        // -----------------------------------
                                        // SHOULD NOT BE USED BY APPLICATIONS
                                        // IT'S REQUIRED FOR COMPATIBILITY.
                                        // -----------------------------------

typedef struct tagOLDEXCLUDEITEM
{
    char    szText[MAX_PATH_SIZE+1];    // important it remains the same
    BYTE    bSubDirectory;
    WORD    wBits;

} OLDEXCLUDEITEM, FAR * LPOLDEXCLUDEITEM;
#define sOLDEXCLUDEITEM sizeof(OLDEXCLUDEITEM)

                                        // -----------------------------------
                                        // Use this structure when needed.
                                        // -----------------------------------
typedef struct tagEXCLUDEITEM
{
    char    szText[SYM_MAX_PATH+1];     // Can handle LFNs.
    BYTE    bSubDirectory;
    WORD    wBits;

} EXCLUDEITEM, FAR * LPEXCLUDEITEM;
#define sEXCLUDEITEM sizeof(EXCLUDEITEM)

#ifdef __cplusplus
extern "C" {
#endif

//------------------------------------------------------------------------------
// PROTO Type section
//------------------------------------------------------------------------------
UINT  WINAPI BOX4_BUTTON         (LPSTR,LPSTR);
UINT  WINAPI BOX4_CLOSE          (LPSTR);
UINT  WINAPI BOX4_OPEN           (LPSTR);
UINT  WINAPI BOX4_PRINTF         (LPSTR,LPSTR);
UINT  WINAPI BOX4_PRINT          (LPSTR,LPSTR);

BYTE  WINAPI CHR4_UPPER          (BYTE);
BYTE  WINAPI CHR4_LOWER          (BYTE);
VOID  WINAPI CHR4_PRINT          (BYTE);
BYTE  WINAPI CHR4_TO_ALT_CASE    (BYTE,BYTE);

BOOL  WINAPI CMB4_CLOSE          (UINT);
BOOL  WINAPI CMB4_DELETE         (UINT,UINT,LPSTR,UINT);
UINT  WINAPI CMB4_OPEN           (LPSTR);
UINT  WINAPI CMB4_READ           (UINT,UINT,LPSTR,UINT);
UINT  WINAPI CMB4_WRITE          (UINT,UINT,LPSTR,UINT);
UINT  WINAPI CMB4_CREATE         (LPSTR,WORD);
WORD  WINAPI CMB4_LOAD_EXTERN    (LPSTR, LPVOID);

VOID  WINAPI DAT4_ASCII_TO_ENCODE(UINT FAR *,LPSTR);
VOID  WINAPI DAT4_ENCODE_TO_ASCII(UINT,LPSTR);
UINT  WINAPI DAT4_ENCODE         (VOID);
VOID  WINAPI DAT4_SPLIT_ENCODE   (UINT, UINT FAR *,UINT FAR *,UINT FAR *);

VOID  WINAPI DIS4_RESET          (VOID);
BOOL  WINAPI DIS4_INT13          (LPSTR);
VOID  WINAPI DIS4_INT25          (LPSTR,DWORD);
VOID  WINAPI DIS4_INT26          (LPSTR,DWORD);
UINT  WINAPI DIS4_SET_IO         (LPSTR,DWORD,WORD);
UINT  WINAPI DIS4_READ_LOGICAL   (LPSTR,BYTE,DWORD,WORD);
UINT  WINAPI DIS4_READL_PHYSICAL (LPSTR,BYTE,WORD,BYTE,BYTE,BYTE);
UINT  WINAPI DIS4_READ_PHYSICAL  (LPSTR,BYTE,WORD,BYTE,BYTE,BYTE);
BOOL  WINAPI DIS4_INT13          (LPSTR);
VOID  WINAPI DIS4_INT25          (LPSTR,DWORD);
VOID  WINAPI DIS4_INT26          (LPSTR,DWORD);
UINT  WINAPI DIS4_WRITE_LOGICAL  (LPSTR,BYTE,DWORD,WORD);
UINT  WINAPI DIS4_WRITEL_PHYSICAL(LPSTR,BYTE,WORD,BYTE,BYTE,BYTE);
UINT  WINAPI DIS4_WRITE_PHYSICAL (LPSTR,BYTE,WORD,BYTE,BYTE,BYTE);
WORD  WINAPI DIS4_LOGICAL_TO_CLUSTER  (DWORD, BYTE);
DWORD WINAPI DIS4_ABSOLUTE_TO_LOGICAL(WORD, BYTE, BYTE , BYTE);
DWORD WINAPI DIS4_CLUSTER_TO_LOGICAL (WORD , BYTE);
DWORD WINAPI DIS4_LOGICAL_TO_ABSOLUTE(DWORD, BYTE, WORD, WORD);
WORD  WINAPI DIS4_READ_ROOT(BYTE, LPSTR);
DWORD WINAPI DIS4_CLUSTER_TO_ABSOLUTE(BYTE, DWORD, LPSTR);
WORD  WINAPI DIS4_IS_BOOTLOCKED(BYTE, LPSTR);
DWORD WINAPI DIS4_LOC_BOOTSECTOR(BYTE, LPSTR);
DWORD WINAPI DIS4_MODIFY_BOOTCODE(BYTE, WORD, BYTE, BYTE, DWORD, WORD, LPSTR);
DWORD WINAPI DIS4_MOVE_MBR(BYTE, WORD, BYTE, BYTE, BYTE, LPSTR);
WORD  WINAPI DIS4_READ_BOOTSECTOR(BYTE, LPSTR);
WORD  WINAPI DIS4_RECOVER(LPSTR, WORD, LPSTR);
WORD  WINAPI DIS4_READ_MBR(BYTE, LPSTR);
WORD  WINAPI DIS4_GET_BPB(BYTE, BPB_struct*, LPSTR);
WORD  WINAPI DIS4_BOOTLOCKED_STATE(WORD, LPSTR);
WORD  WINAPI DIS4_READ_ROOT(BYTE, LPSTR);
DWORD WINAPI DIS4_LOC_BOOTSECTOR_NUM(BYTE, WORD, LPSTR);
WORD  WINAPI DIS4_CREATE_RECOVER(LPSTR, LPSTR);
WORD  WINAPI DIS4_UNLOCK_BOOT(LPSTR);
DWORD WINAPI DIS4_LOC_HIDDEN_MBR(BYTE,LPSTR);
WORD  WINAPI DIS4_GET_BOOTABLE_PARTITION(BYTE,PARTENTRY*,LPSTR);
WORD  WINAPI DIS4_GET_DPB(BYTE,DPB*);
DWORD WINAPI DIS4_LOCK_LASTFAT(BYTE,VOID FAR *,VOID FAR *,LPSTR);
DWORD WINAPI DIS4_LOG_TO_ABS(DWORD, BYTE, LPSTR);
DWORD WINAPI DIS4_CLUS_TO_LOG(WORD, BYTE);
DWORD WINAPI DIS4_UNMARK_FAT(BYTE,DWORD,WORD,LPSTR);
WORD  WINAPI DIS4_GET_MARKED_FAT(BYTE,DWORD,DWORD,LPSTR);

UINT  WINAPI DLL4_FREELIBRARY     (WORD);
DWORD WINAPI DLL4_GETPROCADDRESS (WORD, LPSTR, WORD);
WORD  WINAPI DLL4_LOADLIBRARY     (LPSTR);
WORD  WINAPI DLL4_GETORDIAL       (LPSTR, WORD);
DWORD WINAPI DLL4_GETMOVABLEENTRY(WORD, WORD);
DWORD WINAPI DLL4_GETSEGMENTENTRY(BYTE, WORD);
WORD  WINAPI DLL4_LOADSEGMENTENTRY(LPSTR,WORD, WORD);
WORD  WINAPI DLL4_LOADSEGMENT     (BYTE, WORD, WORD);

UINT  WINAPI DB_ROUTINES          (UINT,LPSTR);

UINT  WINAPI DBC4_OPEN(LPSTR,DWORD,UINT,FARPROC,FARPROC,FARPROC,DBC4_HEADER FAR *,BOOL);
UINT  WINAPI DBC4_CLOSE(DBC4_HEADER FAR *);
UINT  WINAPI DBC4_CREATE(LPSTR,DWORD,UINT,FARPROC,FARPROC,FARPROC,DBC4_HEADER FAR *);
UINT  WINAPI DBC4_DELETE(DBC4_HEADER FAR *, LPVOID, LPVOID);
UINT  WINAPI DBC4_INSERT(DBC4_HEADER FAR *, LPVOID, LPVOID);
UINT  WINAPI DBC4_OPEN_CREATE(LPSTR,DWORD,UINT,FARPROC,FARPROC,FARPROC,DBC4_HEADER FAR *);
UINT  WINAPI DBC4_SEARCH(DBC4_HEADER FAR *, LPVOID, LPVOID);
UINT  WINAPI DBC4_SEARCH_QUICK(DBC4_HEADER FAR *, LPVOID, LPVOID);
BOOL  WINAPI DBC4_LOAD_QUICK(DBC4_HEADER FAR *, WORD FAR *);
DWORD WINAPI DBC4_GET_NUM_ENTRIES(DBC4_HEADER FAR *Info);
BOOL  WINAPI DBC4_GET_ENTRY(DBC4_HEADER FAR *Info,
                            DWORD Num,
                            void FAR *Buffer);

BOOL  WINAPI ExcludeCheck(LPEXCLUDEITEM, LPSTR, WORD);

WORD  WINAPI FIL4_ATTR           (LPSTR,UINT,UINT);
WORD  WINAPI FIL4_OPEN           (LPSTR,UINT);
DWORD WINAPI FIL4_CHANGE_DTA     (LPSTR);
WORD  WINAPI FIL4_CLOSE          (WORD);
UINT  WINAPI FIL4_COPY           (LPSTR,LPSTR,LPSTR,UINT);
UINT  WINAPI FIL4_COPY_HANDLE    (UINT,UINT,LPSTR,UINT,DWORD);
UINT  WINAPI FIL4_CRC            (UINT,WORD,DWORD,LPSTR,UINT);
DWORD WINAPI FIL4_DATETIME       (UINT,WORD,WORD,WORD);
UINT  WINAPI FIL4_FIND_FIRST     (LPSTR,WORD);
UINT  WINAPI FIL4_IS_AT          (LPSTR,LPSTR);
WORD  WINAPI FIL4_IS_RO          (LPSTR);
WORD  WINAPI FIL4_FIND_NEXT      (VOID);
WORD  WINAPI FIL4_PROTECT        (LPSTR,WORD,LPSTR,LPSTR,LPSTR);
WORD  WINAPI FIL4_READ           (UINT,LPSTR,UINT);
DWORD WINAPI FIL4_SEEK           (UINT,DWORD,WORD);
DWORD WINAPI FIL4_GET_START      (UINT,LPSTR,LPVOID);
BOOL  WINAPI FIL4_STAT           (LPSTR,LPSTR,LPSTR);
WORD  WINAPI FIL4_WRITE          (UINT,LPSTR,UINT);
BOOL  WINAPI FIL4_DELETE         (LPSTR);
BOOL  WINAPI FIL4_KILL           (LPSTR,LPSTR,WORD,WORD);
UINT  WINAPI FIL4_DUP_HANDLE     (UINT);
VOID  WINAPI FIL4_FLUSH          (WORD);
BOOL  WINAPI FIL4_HANDLE_TO_NAME (LPSTR,UINT,WORD);
BOOL  WINAPI FIL4_FIND_ALL       (FARPROC,LPARAM,BOOL,LPSTR);
BOOL  WINAPI FIL4_FIND_WILD      (LPSTR,FARPROC,LPARAM,BOOL,LPSTR);
DWORD WINAPI FIL4_SIZE           (UINT);
BOOL  WINAPI FIL4_DIRECTORY      (BYTE,LPSTR,WORD);
BOOL  WINAPI FIL4_CHANGE_DIR     (LPSTR);
BOOL  WINAPI FIL4_RENAME         (LPSTR, LPSTR);
BOOL  WINAPI FIL4_DRIVE          (LPSTR, WORD);
DWORD WINAPI FIL4_GET_DTA        (VOID);
UINT  WINAPI FIL4_OPEN_GLOBAL    (BYTE, WORD, LPSTR, LPSTR);
WORD  WINAPI FIL4_SELF_TEST      (LPSTR,LPSTR);
WORD  WINAPI FIL4_CRCFILE        (LPSTR,LPSTR);
BOOL  WINAPI FIL4_EXISTS(LPSTR lpFilename);
BOOL  WINAPI FIL4_WRITELOG(UINT Handle,VOID FAR *LogArray);
UINT  WINAPI FIL4_MEDIA          (LPSTR);
UINT  WINAPI FIL4_FILE_DESTINATION(LPSTR,LPSTR,LPSTR);

BOOL  WINAPI KEY4_SHIFTSTATE     (WORD);

DWORD WINAPI MEM4_CONV_ALLOCATE (WORD);
WORD  WINAPI MEM4_CONV_FREE      (WORD);
UINT  WINAPI MEM4_CRC            (WORD, LPSTR, WORD);
UINT  WINAPI MEM4_SEGTOSEL       (WORD);
WORD  WINAPI MEM4_GALLOCATE      (WORD, WORD, WORD);
VOID  WINAPI MEM4_COPY           (LPSTR, LPSTR, UINT);
UINT  WINAPI MEM4_COPY_TO_CHAR   (LPSTR, LPSTR, UINT, BYTE);
WORD  WINAPI MEM4_GDESTROY       (WORD, LPVOID);
DWORD WINAPI MEM4_EMS_ALLOCATE  (WORD);
WORD  WINAPI MEM4_GFREE          (WORD, WORD);
WORD  WINAPI MEM4_GINIT          (LPVOID);
UINT  WINAPI MEM4_REPLACE        (LPSTR, BYTE, BYTE, UINT);
WORD  WINAPI MEM4_GLOCK          (WORD, WORD);

UINT  WINAPI MEM4_SRCH_ALPHA     (LPSTR, UINT);
UINT  WINAPI MEM4_SRCH_CHAR      (LPSTR, BYTE, UINT);
UINT  WINAPI MEM4_SET            (LPSTR, BYTE, UINT);
WORD  WINAPI MEM4_EMS_FREE       (WORD);
WORD  WINAPI MEM4_EMS_LOCK       (WORD);
WORD  WINAPI MEM4_EMS_GETPAGE    (VOID);
UINT  WINAPI MEM4_SRCH_STRING    (LPSTR, LPSTR, UINT);
WORD  WINAPI MEM4_EMS_UNLOCK     (WORD);
WORD  WINAPI MEM4_GUNLOCK        (WORD, WORD);

LPSTR WINAPI MEM4_GETOBJECT     (BYTE, LPSTR);
VOID  WINAPI MEM4_SETOBJECT      (BYTE, LPSTR, LPSTR, WORD);
DWORD WINAPI MEM4_UMB_ALLOCATE  (WORD);
WORD  WINAPI MEM4_UMB_FREE       (WORD);
UINT  WINAPI MEM4_SEGTOSEL       (WORD);
WORD  WINAPI MEM4_XMS_ALLOCATE   (WORD);
WORD  WINAPI MEM4_XMS_FREE       (WORD);
DWORD WINAPI MEM4_XMS_LOCK      (WORD);
WORD  WINAPI MEM4_XMS_UNLOCK     (WORD);
WORD  WINAPI MEM4_GCONV          (WORD);

VOID  WINAPI MUS4_BEEP(BYTE);

UINT  WINAPI NOV4_GETLOGGEDIN    (LPSTR,LPSTR);
UINT  WINAPI NOV4_GETALLUSERS    (LPSTR,LPSTR);
UINT  WINAPI NOV4_GETUSERCONNECT (LPSTR,LPSTR);
UINT  WINAPI NOV4_SENDMESSAGE    (LPSTR,LPSTR,LPSTR);
UINT  WINAPI NOV4_SEND_LIST(LPSTR ToList, LPSTR Message, LPSTR Buffer);

UINT  WINAPI NUM4_MODULUS        (DWORD,DWORD);

WORD  WINAPI OTH4_COMMAND_LINE   (LPVOID);


DWORD WINAPI SCN4_VERIFY_NAME   (UINT, WORD, WORD, LPSTR, LPSTR, LPSTR);
DWORD WINAPI SCN4_ME            (WORD, LPSTR, LPSTR, WORD, LPSTR, LPSTR, LPVOID, WORD);
UINT  WINAPI SCN4_CRC            (LPSTR,WORD,LPSTR,WORD);
UINT  WINAPI SCN4_ALG            (WORD,LPSTR,WORD,LPSTR);
UINT  WINAPI GetInfectionData    (LPSTR, LPSTR, DWORD);
UINT  WINAPI SCN4_MALTA          (LPSTR, WORD, LPSTR, LPVOID);
UINT  WINAPI SCN4_V2P            (LPSTR, WORD, LPSTR, LPVOID);
UINT  WINAPI SCN4_MTE            (LPSTR, WORD, LPSTR, LPVOID);
UINT  WINAPI SCN4_BOOT           (LPSTR,LPSTR,BOOL);
UINT  WINAPI SCN4_MEMORY         (LPSTR lpData, BYTE byScanHighMemory);
UINT  WINAPI SCN4_LOAD_CRC       (UINT, LPSTR, LPSTR);
UINT  WINAPI SCN4_COUNT_CRC      (UINT, LPSTR);
UINT  WINAPI SCN4_LOAD_ALG       (UINT, LPSTR, LPSTR, LPSTR);
UINT  WINAPI SCN4_COUNT_ALG      (UINT, LPSTR);
UINT  WINAPI SCN4_LOAD_BOOT      (UINT, LPSTR, LPSTR);
UINT  WINAPI SCN4_COUNT_BOOT     (UINT, LPSTR);
UINT  WINAPI SCN4_LOAD_MEMORY    (UINT, LPSTR, LPSTR);
UINT  WINAPI SCN4_COUNT_MEMORY   (UINT, LPSTR);
UINT  WINAPI SCN4_COUNT_REM      (UINT, LPSTR);
BOOL  WINAPI SCN4_GET_ENTRY      (UINT, WORD, LPSTR);
UINT  WINAPI SCN4_LOAD_CRC_CHECK_DATA(UINT, LPSTR, LPSTR);
UINT  WINAPI SCN4_LOAD_NAMES     (UINT, LPSTR, LPSTR);
UINT  WINAPI SCN4_DELETE         (UINT, LPSTR, LPSTR);

UINT  WINAPI SCR4_GET_VIDEOSEG   (VOID);
VOID  WINAPI SCR4_BLANK          (VOID);
BYTE  WINAPI SCR4_GET_ATTR       (VOID);

VOID  WINAPI STR4_UPPER          (LPSTR);
VOID  WINAPI STR4_LOWER          (LPSTR);
UINT  WINAPI STR4_ASCII_TO_INT   (LPSTR);
WORD  WINAPI STR4_ASCII_TO_WORD  (LPSTR);
VOID  WINAPI STR4_INT_TO_ASCII   (WORD,LPSTR);
VOID  WINAPI STR4_WORD_TO_ASCII  (WORD,LPSTR);
VOID  WINAPI STR4_FORMAT         (LPVOID, LPSTR);
UINT  WINAPI STR4_LENGTH         (LPSTR);
VOID  WINAPI STR4_COPY           (LPSTR, LPSTR);
UINT  WINAPI STR4_ASCIIHEX_TO_HEX(LPSTR, LPSTR);
UINT  WINAPI STR4_COPY_TO_CHAR   (LPSTR, LPSTR, BYTE);
UINT  WINAPI STR4_LENGTH_TO_CHAR (LPSTR, BYTE);
VOID  WINAPI STR4_PRINT          (LPSTR);
BOOL  WINAPI STR4_COMPARE        (LPSTR, LPSTR);
BOOL  WINAPI STR4_ICOMPARE       (LPSTR, LPSTR);
UINT  WINAPI STR4_COUNT_CHAR(LPSTR lpString,BYTE Char);
VOID  WINAPI STR4_APPEND(LPSTR lpOne,LPSTR lpTwo);
WORD  WINAPI STR4_SRCH_CHAR(LPSTR lpString,BYTE Char);
UINT  WINAPI STR4_COUNT_STRINGS(LPSTR lpStringTable);
DWORD WINAPI STR4_DWORD_CONV(DWORD,WORD,LPSTR,WORD);
WORD  WINAPI STR4_ASCIIHEX_TO_WORD(LPSTR);
VOID  WINAPI STR4_WORD_TO_ASCIIHEX(WORD,LPSTR);
DWORD WINAPI STR4_ASCII_CONV(LPSTR,WORD,LPSTR,WORD);
VOID  WINAPI STR4_MODIFY_LONG_FILENAME(LPSTR,LPSTR);

VOID  WINAPI SetEncryptINI       (BOOL);

BOOL  WINAPI SYS4_IS_PS2         (VOID);
BOOL  WINAPI SYS4_GET_SDA        (LPVOID);
VOID  WINAPI SYS4_HALT           (VOID);
VOID  WINAPI SYS4_TSR            (WORD);
WORD  WINAPI SYS4_GET_MCB_CHAIN  (WORD);
BOOL  WINAPI SYS4_UNVECTOR       (BYTE, FARPROC, LPVOID);
DWORD WINAPI SYS4_GET_MYTSR     (WORD);
DWORD WINAPI SYS4_GET_ORIG13(WORD ROM_START, WORD ROM_END);
DWORD WINAPI SYS4_GET_VECTOR(BYTE Vector);
DWORD WINAPI SYS4_GET2_ORIG13(VOID);
VOID  WINAPI SYS4_VECTOR         (BYTE, FARPROC,LPVOID);
VOID  WINAPI SYS4_DOSCALL(FARPROC lpSetTo, LPVOID *lpStoreHere);
VOID  WINAPI SYS4_UNDOSCALL(FARPROC lpRoutine, LPVOID lpRestore);
VOID  WINAPI SYS4_HOOK_21h(VOID);
VOID  WINAPI SYS4_HOOK_DOS(VOID);
VOID  WINAPI SYS4_HOOK_13h(VOID);
VOID  interrupt SYS4_PRE_21h(VOID);
VOID  interrupt SYS4_POST_21h(VOID);
VOID  interrupt SYS4_PRE_DOS(VOID);
VOID  interrupt SYS4_POST_DOS(VOID);
VOID  interrupt SYS4_PRE_13h(VOID);
VOID  interrupt SYS4_POST_13h(VOID);
WORD  WINAPI SYS4_GET_CPU_TYPE(VOID);
BOOL WINAPI SYS4_GET_NAMES(LPSTR);
BYTE WINAPI SYS4_FIND_QEMM(LPSTR lpfnQPI);
BYTE WINAPI SYS4_QEMM_INSTALLED(VOID);
WORD WINAPI SYS4_QEMM_VERSION(VOID);
BYTE WINAPI SYS4_QEMM_STEALTH_MODE(VOID);
BYTE WINAPI SYS4_QEMM_COPY_STEALTH_ROM(LPSTR lpDest,LPSTR lpSource,DWORD cNum);
BYTE WINAPI SYS4_GET_SYSTEM_ID(LPSTR lpSysID);

VOID WINAPI SetAnsiFlag (WORD);
WORD WINAPI GetAnsiFlag (VOID);

VOID  WINAPI TIM4_ASCII_TO_ENCODE(UINT FAR *,LPSTR);
VOID  WINAPI TIM4_ENCODE_TO_ASCII(UINT,LPSTR);
UINT  WINAPI TIM4_ENCODE         (VOID);
VOID  WINAPI TIM4_SPLIT_ENCODE   (UINT, UINT FAR *,UINT FAR *,UINT FAR *);

#ifdef __cplusplus
}
#endif

#endif

#endif  // SYM_WIN32 , else
