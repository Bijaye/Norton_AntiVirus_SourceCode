// Copyright 1993-1994 Symantec Corporation
//===========================================================================
//
// $Header:   S:/INCLUDE/VCS/ctsn.h_v   1.77   10 Jun 1997 11:06:20   DBUCHES  $
//
// Description:  Declares function prototypes, macros, equates and types
//               for the CERTLIBN project.
//
// See Also:
//
//---------------------------------------------------------------------------
// $Log:   S:/INCLUDE/VCS/ctsn.h_v  $
//
//    Rev 1.77   10 Jun 1997 11:06:20   DBUCHES
// Added 1 byte packing for EXCLUDEITEM struct.
//
//    Rev 1.76   10 Jun 1997 11:03:28   DBUCHES
// Added 1 byte packing for EXCLUDEITEM struct.
//
//    Rev 1.75   10 Jun 1997 11:00:52   DBUCHES
// Added 1 byte packing for EXCLUDEITEM struct.
//
//    Rev 1.74   28 Apr 1997 18:20:16   MKEATIN
// Included n30type.h and removed redudant defines.
//
//    Rev 1.73   12 Aug 1996 17:04:40   RPULINT
// Added macro, windows and agent virus types
//
//    Rev 1.72   27 Jun 1996 21:47:38   DALLEE
// LuigiPlus port:
// Changed SYM_W32 #define to standard SYM_WIN32
//
//    Rev 1.71   14 May 1996 20:36:56   JWILBER
// Changed the offset of the non-resident name table in the NE header
// structure definition to a DWORD instead of a WORD.
//
//    Rev 1.70   06 May 1996 14:54:04   RSTANEV
// Merged Ron's Unicode changes.
//
//    Rev 1.69   16 Apr 1996 10:36:20   TCASHIN
// Removed ScanMemoryBufferEX prototype.
//
//    Rev 1.68   15 Apr 1996 22:10:58   TCASHIN
// Changed callback parameter in ScanMemoryBufferEX.
//
//    Rev 1.67   15 Apr 1996 20:32:54   TCASHIN
// Added prototype for ScanMemoryBufferEX.
//
//    Rev 1.66   14 Apr 1996 02:47:58   JWILBER
// Changed the #define for sEXEHEADER to "(sizeof(EXEHEADER)-34)" instead
// of "sizeof(EXEHEADER)-66", which evaluated to -2 and caused errors.
//
//    Rev 1.65   28 Mar 1996 23:07:06   JWILBER
// Changed comments to begin with "//" in BOOTLOCK equates instead of
// ";", which was left by some assembly-language programmer.
//
//    Rev 1.64   06 Feb 1996 15:24:54   RSTANEV
// Changed a few more prototypes
//
//    Rev 1.63   06 Feb 1996 14:18:06   RSTANEV
// Changed some LPSTRs in the prototype of FIL4_PROTECT to LPVOID.  They
// should be LPBYTE, but this will cause too many compilation problems.
//
//    Rev 1.62   02 Feb 1996 21:44:22   RSTANEV
// Changed the prototype of FileSelfTest() to support Unicode.
//
//    Rev 1.61   02 Feb 1996 20:07:18   RSTANEV
// Changed the prototypes of some APIs to be Unicode compatible.
//
//    Rev 1.60   11 Jan 1996 16:34:54   JWORDEN
// Port changes from Quake 8
//
//    Rev 1.59   09 Jan 1996 17:02:14   RSTANEV
// Added TCHAR support (enough only to compile CERTLIBN!)
//
//    Rev 1.58   29 Nov 1995 14:22:16   DCHI
// Added FIL4_GET_START and SCN4_GET_NAME prototypes.
//
//    Rev 1.57   23 Jun 1995 13:14:26   SZIADEH
// prototype for MUS4_BEEP()
//
//    Rev 1.56   06 Jun 1995 16:44:08   SZIADEH
// more prototypes.
//
//    Rev 1.55   31 May 1995 11:52:28   SZIADEH
// prototype for IsOneHalfInMemory()
//
//    Rev 1.54   25 May 1995 02:15:36   SZIADEH
// added DIS4_LOC_BOOTSECTOR().
//
//    Rev 1.53   16 May 1995 11:11:32   RSTANEV
// SIZE_VIRUSNAME does not include EOS.
//
//    Rev 1.52   11 May 1995 16:20:14   SZIADEH
// declared GetInfectionData for the one_half virus.
//
//    Rev 1.51   29 Apr 1995 17:09:26   DALLEE
// New #define CTSN_LOADDS: 16-bit platforms get LOADDS.
// This should be set for all "C" functions called from CERTLIBN since
// it occasionally resets DS.  :-)
//
//    Rev 1.50   11 Apr 1995 12:32:22   RSTANEV
// Added FileSelfTest() prototype.
//
//    Rev 1.49   20 Mar 1995 21:08:46   SZIADEH
// one additional parameter to UNC_GetServerVolumeName()
//
//    Rev 1.48   20 Mar 1995 19:35:28   SZIADEH
// added UNC_GetServerVolumeName()
//
//    Rev 1.47   17 Mar 1995 19:39:52   SZIADEH
// added two new functions:
//  IsUNCName() and
//  UNC_SkipServerAndVolumeName()
//
//    Rev 1.46   02 Feb 1995 16:57:40   SZIADEH
// protyped create inscans function.
//
//    Rev 1.45   31 Jan 1995 16:17:32   SZIADEH
// changed SCN4_ME and SCN4_BOO to return UINT.
//
//    Rev 1.44   30 Jan 1995 19:24:28   BRAD
// Added LPFP
//
//    Rev 1.43   30 Jan 1995 18:31:16   BRAD
// Incorrectly checked in
//
//    Rev 1.42   30 Jan 1995 18:25:46   BRAD
// Added FP union
//
//    Rev 1.41   30 Jan 1995 17:45:06   BRAD
// Added FILE_PROTECTL structure
//
//    Rev 1.40   30 Jan 1995 17:35:08   BRAD
// Changed Inoc prototypes to take size of item record
//
//    Rev 1.39   27 Jan 1995 19:18:10   SZIADEH
// added a flag to allow using default settings in InitCertlib()
//    INIT_CERTLIB_USE_DEFAULT
//
//    Rev 1.38   13 Jan 1995 16:36:44   MFALLEN
// Renamed EXCLUDEITEM to OLDEXCLUDEITEM and created a new EXCLUDEITEM structure
// that can handle LFNs.
//
//    Rev 1.37   03 Jan 1995 18:31:02   MFALLEN
// Make sure the EXCLUDEITEM structure uses the MAX_PATH_SIZE define instead of
// SYM_MAX_PATH
//
//    Rev 1.36   22 Dec 1994 03:15:22   JMILLARD
// add uSigHeaderSize to ScanMemoryCD
//
//    Rev 1.35   21 Dec 1994 23:17:18   JMILLARD
// change ScanMemoryBuffer and ScanMemoryCD to UINT args throughout
//
//    Rev 1.34   20 Dec 1994 17:37:52   BRAD
// Added another parameter to FIL4_PROTECT
//
//    Rev 1.33   12 Dec 1994 19:12:44   MFALLEN
// Added 19 prototypes for functions being used in SYMINTEG. Also added
// InitCertlib() as per Mr. Z's request.
//
//    Rev 1.32   12 Dec 1994 17:34:48   SZIADEH
// removed assembly stype comments.
//
//    Rev 1.31   12 Dec 1994 17:20:30   SZIADEH
// sync with CTS4.H.
//
//    Rev 1.30   06 Dec 1994 17:54:40   BRAD
// Added a mess of Inoc routines
//
//    Rev 1.29   06 Dec 1994 17:41:38   BRAD
// Added FILE_PROTECT and DBC4_HEADER
//
//    Rev 1.28   06 Dec 1994 16:17:50   JMILLARD
// check in interim version of Sami's stuff
//
//    Rev 1.26   22 Nov 1994 13:40:20   SZIADEH
// deleted garbage chars
//
//    Rev 1.25   22 Nov 1994 13:15:58   SZIADEH
// changed prototype of memory scanning functions.
//
//    Rev 1.24   22 Nov 1994 10:23:24   MFALLEN
// Commented out FIL4_FIND... since they are not valid in W95 environment.
//
//    Rev 1.23   21 Nov 1994 13:09:16   MFALLEN
// Reintroduced HPN30NOTES and other similar defines due to backward compatibility
// resons.
//
//    Rev 1.22   08 Nov 1994 17:27:14   SZIADEH
// changed memory scanning prototypes.
//
//    Rev 1.21   26 Oct 1994 19:28:08   keith
// Add some prototypes and related to make the build more reproducable
//
//    Rev 1.20   06 Sep 1994 18:04:46   DALLEE
// Re-CHKIN rev 1.18
//
//    Rev 1.19   06 Sep 1994 18:02:30   DALLEE
// Same as r1.17 except addition of MEM4_CRC() and FIL4_GET_START().
// Temp revision for LUIGI build only.
//
//    Rev 1.18   02 Sep 1994 17:14:50   CAREY
// added protos for FIL4_GET_START and MEM4_CRC and removed FILE_PROTECT,
// DBC4_HEADER and CHAIN structures...
//
//    Rev 1.17   29 Aug 1994 22:19:24   SZIADEH
// - added two constants MEMORY_ALLXCHG and MEMORY_413XCHG
// - changed the prototype for vlscanforcd
//
//    Rev 1.16   28 Aug 1994 11:25:42   SZIADEH
// changed constants MEMORY_413 and MEMORY_ALL to become 1 based instead
// of 0 based.
//
//    Rev 1.15   26 Aug 1994 21:39:02   SZIADEH
// fixed spelling.
//
//    Rev 1.14   26 Aug 1994 21:37:28   SZIADEH
// two new functions for memory scanning.
//
//    Rev 1.13   19 Aug 1994 15:08:36   SZIADEH
// added constants for memory detection and boot repair.
//
//    Rev 1.12   12 Aug 1994 18:28:04   SZIADEH
// added bitCTRL_BOOT_REPAIR constant.
//
//    Rev 1.11   24 Jun 1994 15:25:20   RSTANEV
// Put EXCLUDECHECK again.
//
//    Rev 1.10   24 Jun 1994 14:56:50   RSTANEV
// EXCLUDECHECK should be ExcludeCheck.
//
//    Rev 1.9   24 Jun 1994 14:38:10   RSTANEV
// Added EXCLUDECHECK.
//
//    Rev 1.8   22 Jun 1994 16:21:34   RSTANEV
// Added a new structure for FIL4_FIND_ functions.
//
//    Rev 1.7   22 Jun 1994 09:14:24   RSTANEV
// Added FIL4_IS_AT.
//
//    Rev 1.6   27 May 1994 12:07:18   RSTANEV
// Added FIL4_FIND_WILD.
//
//    Rev 1.5   27 May 1994 11:46:44   MFALLEN
// FIL4_FIND_ALL was reintroduced once again since it's needed.
//
//    Rev 1.4   26 May 1994 11:02:28   MFALLEN
// scn4_verify_name
//
//    Rev 1.3   26 May 1994 10:23:04   RSTANEV
// Fixed prototypes.
//
//    Rev 1.2   16 May 1994 13:05:52   RSTANEV
// A temporary fix of a misunderstanding with CTS4.H.
//
//    Rev 1.1   09 May 1994 18:44:56   RSTANEV
// Added FIL4_FIND_ALL.
//
//    Rev 1.0   05 May 1994 12:48:52   RSTANEV
// Initial revision.
//
//---------------------------------------------------------------------------


#ifndef _CTS4_H
#define _CTS4_H

#include "callback.h"
#include "n30type.h"

//------------------------------------------------------------------------------
// CTSN.H - Protos, equates, and structures for 'C'
//------------------------------------------------------------------------------

// "C" functions called from within CERTLIB must have LOADDS since
// CERTLIB is very naughty.

#if defined (SYM_DOS) || defined (SYM_WIN16)
    #define CTSN_LOADDS     LOADDS
#else
    #define CTSN_LOADDS
#endif

//------------------------------------------------------------------------------
// EQUATE Section
//------------------------------------------------------------------------------
#define fREAD            0                   // Open types
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

#define fSEEK_BEG        1                   // Used for Seeking handle in file
#define fSEEK_FOR        2
#define fSEEK_BAK        4
#define fSEEK_END        8

#define DIR_ENTRY_LEN    256                 // MAX Length of directory entry

#define GET_INFO         0                   // Get or set info flag
#define SET_INFO         1

#define MAXPAD           64                  // Used for Get Start padding count

#ifndef MAX_PATH_SIZE
#define MAX_PATH_SIZE    144                 // Max with of a DOS path
#endif

#ifndef MAX_NET_PATH
#define MAX_NET_PATH     256                 // Maximum file/path length on network
#endif

#define PROTECT_ADD      0                   // Used for File protection
#define PROTECT_TEST     1
#define PROTECT_DELETE   2
#define PROTECT_REPAIR   4
#define PROTECT_CREATE   8
#define PROTECT_QUICK_INOC 16

#define HIDDEN           2                   // Used to search for files
#define SYSTEM           4
#define VOLLABEL         8
#define SEARCH_DIR       16                  // was DIR - collides with Novell

#define IS_COM           0                   // Used for Get Start
#define IS_EXE           1
#define IS_SYS           2
#define IS_BCOM          3                   // Bulgarian COM
#define IS_VCOM          4                   // Verified COM
#define IS_ZIP           5
#define IS_LZEXE         6
#define IS_PKEXE         7
#define IS_WHALE         8
#define STARTSIZE        256

#define IS_FILE          0                   // Used by Find All
#define IS_DIRECTORY     1
#define DIR_DEPTH        33                  // How many levels of directories

#define MEM_LOCKED       0x01                  // Memory flags
#define MEM_FIXED        0x02
#define MEM_READONLY     0x04
#define MEM_NORMAL       0x00

#define MEM_REQUEST_UMB  0x10
#define MEM_RELEASE_UMB  0x11
#define MEM_ALLOCATE_XMSU 0x09
#define MEM_FREE_XMS     0x0A
#define MEM_LOCK_XMS     0x0C
#define MEM_UNLOCK_XMS   0x0D

#define CL_FLAG              1               // /FLAG
#define CL_STRING            2               // "This string" or String
#define CL_FILENAME          2               // FILENAME
#define CL_NUMERIC           3               // /NUM 1234
#define CL_OTHER             4               // /STR "This string" or String
#define CL_HEX               5               // /HEX AB

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

// one based since they're encrypted in memory
#define MEMORY_413      1
#define MEMORY_ALL      2

#define SIZE_VIRUSNAME  (21 + 1)        // This does NOT include EOS!
#define SIZE_VINFO      32

#define SIZE_MEM        15              // size of memory signature
#define SIZE_MEM_OLAP   16              // paragraph align memorysig this value

#define uSizeVirscanHeader  256

#define bitDSSI         0x0001     // File name located in DS:SI not DS:DX
#define bitINT13                0x0002     // If use an INT 13 directly use this flag
#define bitTSR          0x8000     // If set TSR doing work else UI
#define bitWINDOWS      0x4000     // If set Windows version doing work else DOS

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
  #define bitREG        0x1000
  #define bitREREG      0x2000
#endif

#define excVIOLATING    0x0002     // The program executing is doing the violation
#define excVIRUS_FOUND  0x0004     // Virus found
#define excINOC_CHANGE  0x0008     // File changed
#define excFILE_WATCH   0x0010
#define excLOW_FORMAT   (0x0020 + excVIOLATING)
#define excWR_HBOOT     (0x0040 + excVIOLATING)
#define excWR_FBOOT     (0x0080 + excVIOLATING)
#define excWR_ATLIST    (0x0100 + excVIOLATING)
#define excRO_CHANGE    (0x0200 + excVIOLATING)
#define excREG_CHANGE   0x0400

// Inclusion data flags (for BANKERS file registration).
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
#define BPB_SIGNATURE           0x00    // Offsets into the BPB for info
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
// flags to initialize certlibn
//----------------------------------------------------------------------------
#define INIT_CERTLIB_USE_DEFAULT 0x0001

//----------------------------------------------------------------------------
// Control Word 2 defines
//----------------------------------------------------------------------------

#define NO_GENERIC      0x200
#define DISPLAY_DIALOG  0x400
#define NOT_IN_TSR      0x800
#define USE_PAM_REPAIR  0x1000

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
    char    Filename[256+30];           //&? Fix this!  For compatibility
#else                                   // with NAVNLM v1.x
    char    Filename[144+1];            //&? Fix this!  For compatibility
#endif                                  // with NAV v3.x    -DALLEE
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
// File protection used with Inoculation/FileWatch/SysCheck
// Make sure this is kept in synch with FILE_PROTECT structure above.  Only
// difference between these 2 is the 'Filename' field.
//------------------------------------------------------------------------------
typedef struct
    {
    char    Filename[260+1];
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
    } FILE_PROTECTL;
typedef FILE_PROTECTL FAR *LPFILE_PROTECTL;
#define sFILE_PROTECTL   sizeof(FILE_PROTECTL)

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
typedef union
    {
    FILE_PROTECT    fileProtect;
    FILE_PROTECTL   fileProtectL;
    } FP;
typedef FP FAR *LPFP;

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
// File protection used with Inoculation/FileWatch/SysCheck
//------------------------------------------------------------------------------
typedef struct tagFINDITEM
    {
    char   szLongName[260];
    char   szShortName[14];
    DWORD  dwCookie;
    BOOL   bIsDir;
    DWORD  dwFileSize;
    } FINDITEM, FAR* LPFINDITEM;

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

#ifndef SYM_UNIX // NEW_UNIX
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
#endif // SYM_UNIX

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
#ifdef SYM_WIN
#pragma pack(push, 1)
#endif
typedef struct tagEXCLUDEITEM
{
    TCHAR   szText[SYM_MAX_PATH+1];     // Can handle LFNs.
    BYTE    bSubDirectory;
    WORD    wBits;

} EXCLUDEITEM, FAR * LPEXCLUDEITEM;
#ifdef SYM_WIN
#pragma pack(pop)
#endif

#define sEXCLUDEITEM sizeof(EXCLUDEITEM)

#ifdef __cplusplus
extern "C" {
#endif

//----------------------------------------------------------------------------
// sync CTSN.H with CTS4.H
//---------------------------------------------------------------------------

BOOL  WINAPI CTSN_LOADDS FIL4_STAT           (LPSTR,LPSTR,LPSTR);
UINT  WINAPI FIL4_CRC            (UINT,WORD,DWORD,LPSTR,UINT);
WORD  WINAPI FIL4_PROTECT        (LPSTR,LPSTR,LPSTR,WORD,LPVOID,UINT,LPVOID,LPVOID);
WORD  WINAPI SYS4_GET_CPU_TYPE(VOID);

//---------------------------------------------------------------------------
// Function prototypes
//---------------------------------------------------------------------------

#ifndef SYM_WIN32
// These functions are not valid for W95. Identical stubs were created
// in the W95 projects.
BOOL  WINAPI FIL4_IS_AT         (LPSTR,LPSTR);
BOOL  WINAPI FIL4_FIND_ALL      (FARPROC,LPARAM,BOOL,LPSTR);
BOOL  WINAPI FIL4_FIND_WILD     (LPSTR,FARPROC,LPARAM,BOOL,LPSTR);
#endif


BOOL  WINAPI CreateInScan       (LPTSTR lpVirScanPath);
UINT  WINAPI SCN4_ME            (UINT,LPTSTR,LPBYTE,UINT,LPBYTE,LPTSTR,LPVOID,WORD);
UINT  WINAPI SCN4_BOOT          (LPBYTE,LPBYTE,BOOL);

DWORD WINAPI SCN4_VERIFY_NAME   (UINT,WORD,WORD,LPSTR,LPSTR,LPSTR);
DWORD WINAPI SCN4_GET_NAME      (UINT,WORD,LPSTR,LPSTR,LPSTR);
BOOL  WINAPI EXCLUDECHECK       (LPEXCLUDEITEM,LPSTR,WORD);

UINT  WINAPI ScanMemoryBuffer   (LPBYTE, UINT, LPBYTE, UINT, UINT, UINT );
UINT  WINAPI ScanMemoryCD       (LPBYTE, UINT, LPBYTE, UINT, UINT, UINT );
BOOL  WINAPI IsOneHalfInMemory  (VOID);


UINT  WINAPI SCN4_MEMORY         (LPBYTE, BOOL);
UINT  WINAPI SCN4_MTE            (LPSTR, WORD, LPSTR, LPVOID);
UINT  WINAPI SCN4_MALTA          (LPSTR, WORD, LPSTR, LPVOID);
UINT  WINAPI SCN4_V2P            (LPSTR, WORD, LPSTR, LPVOID);
UINT  WINAPI SCN4_DELETE         (UINT, LPBYTE, LPSTR);
BOOL  WINAPI FIL4_DELETE         (LPTSTR);
BOOL  WINAPI SCN4_GET_ENTRY      (UINT, WORD, LPSTR);
WORD  WINAPI MEM4_CRC            (WORD,LPSTR,UINT);
DWORD WINAPI FIL4_GET_START      (UINT,LPSTR,LPVOID);
DWORD WINAPI FIL4_NEW_GET_START  (UINT,LPSTR,LPVOID);

BOOL  WINAPI FIL4_CREATE_DIR     (LPSTR );
BOOL  WINAPI MKDIR_INOC_DB       (LPSTR );

BOOL  WINAPI FIL4_KILL           (LPTSTR,LPVOID,WORD,WORD);

UINT  WINAPI DBC4_OPEN(LPSTR,DWORD,UINT,FARPROC,FARPROC,FARPROC,DBC4_HEADER FAR *,BOOL);
UINT  WINAPI DBC4_CLOSE(DBC4_HEADER FAR *);
UINT  WINAPI DBC4_CREATE(LPSTR,DWORD,UINT,FARPROC,FARPROC,FARPROC,DBC4_HEADER FAR *);
UINT  WINAPI DBC4_DELETE(DBC4_HEADER FAR *, LPVOID, LPVOID);
UINT  WINAPI DBC4_INSERT(DBC4_HEADER FAR *, LPVOID, LPVOID);
UINT  WINAPI DBC4_OPEN_CREATE(LPSTR,DWORD,UINT,FARPROC,FARPROC,FARPROC,DBC4_HEADER FAR *);
UINT  WINAPI DBC4_SEARCH(DBC4_HEADER FAR *, LPVOID, UINT, LPVOID);
UINT  WINAPI DBC4_SEARCH_QUICK(DBC4_HEADER FAR *, LPVOID, LPVOID);
BOOL  WINAPI DBC4_LOAD_QUICK(DBC4_HEADER FAR *, WORD FAR *);
DWORD WINAPI DBC4_GET_NUM_ENTRIES(DBC4_HEADER FAR *Info);
BOOL  WINAPI DBC4_GET_ENTRY(DBC4_HEADER FAR *Info, DWORD Num, void FAR *Buffer);

BOOL  WINAPI IsUNCName  ( LPSTR lpFileName );
LPSTR WINAPI UNC_SkipServerVolumeName ( LPSTR lpFileName );
LPSTR WINAPI UNC_GetServerVolumeName  ( LPSTR lpFileName, LPSTR lpResult );

VOID  WINAPI MUS4_BEEP(BYTE);


//----------------------------------------------------------------------------
// flags to initialize certlibn defined near the top in this file
//----------------------------------------------------------------------------
//  INIT_CERTLIB_USE_DEFAULT  0x0001
//      Use this flag if you do NOT wish to provide your own functions.
//

BOOL SYM_EXPORT WINAPI InitCertlib (    // To initialize CERTLIB library
    LPCALLBACKREV1  lpCallback,         // [in] pointer to callback struct
    WORD            wFlags              // [in] Sami knows what flags.
    );

// ----------------------------------------------------------------------------
// These functions are used by the SYMINTEG project.
// ----------------------------------------------------------------------------

WORD  WINAPI FIL4_SELF_TEST         (LPSTR,LPSTR);
UINT  WINAPI FileSelfTest           (LPTSTR,LPVOID);
WORD  WINAPI FIL4_CRCFILE           (LPSTR,LPSTR);
DWORD WINAPI FIL4_SIZE              (UINT);
UINT  WINAPI FIL4_CLOSE             (UINT);
WORD  WINAPI FIL4_ATTR              (LPTSTR,UINT,UINT);
DWORD WINAPI FIL4_SEEK              (UINT,DWORD,WORD);
UINT  WINAPI FIL4_READ              (UINT,LPVOID,UINT);
WORD  WINAPI FIL4_WRITE             (UINT,LPVOID,UINT);
DWORD WINAPI FIL4_DATETIME          (UINT,WORD,WORD,WORD);
UINT  WINAPI FIL4_FIND_FIRST        (LPSTR,WORD);
DWORD WINAPI FIL4_GET_DTA           (VOID);
WORD  WINAPI DIS4_READ_MBR          (BYTE, LPSTR);
DWORD WINAPI DIS4_LOC_BOOTSECTOR_NUM(BYTE, WORD, LPSTR);
UINT  WINAPI DIS4_READ_PHYSICAL     (LPSTR,BYTE,WORD,BYTE,BYTE,BYTE);
UINT  WINAPI DIS4_WRITE_PHYSICAL    (LPSTR,BYTE,WORD,BYTE,BYTE,BYTE);
BOOL  WINAPI SYS4_GET_NAMES         (LPSTR);
UINT  WINAPI STR4_LENGTH            (LPSTR);
UINT  WINAPI MEM4_SET               (LPVOID, BYTE, UINT);
VOID  WINAPI MEM4_COPY              (LPSTR, LPSTR, UINT);
BOOL  WINAPI GetInfectionData       (LPBYTE, LPSTR, DWORD);
DWORD WINAPI DIS4_LOC_BOOTSECTOR(BYTE, LPSTR);

#ifdef __cplusplus
}
#endif

#endif

