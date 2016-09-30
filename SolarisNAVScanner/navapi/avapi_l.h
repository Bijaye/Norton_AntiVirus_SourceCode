//************************************************************************
//
// $Header:   S:/NAVAPI/VCS/avapi_l.h_v   1.11   28 Oct 1998 15:04:24   MKEATIN  $
//
// Description:
//      AV Engine header file.
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/NAVAPI/VCS/avapi_l.h_v  $
// 
//    Rev 1.11   28 Oct 1998 15:04:24   MKEATIN
// Added VI_VIRUS_ID to enumVirusInfo.
//
//    Rev 1.10   03 Sep 1998 20:02:44   dhertel
// Define AVAPI_MT for NLM
//
//    Rev 1.9   25 Aug 1998 13:51:44   dhertel
//
//    Rev 1.8   29 Jul 1998 16:03:58   MKEATIN
// Removed bPreserveLastAccess from VLRepairFile.
//
//    Rev 1.5   23 Jul 1998 22:37:30   ksackin
// Added DB_VERSION_STAMP to the DatabaseInfo enumeration.
//
//    Rev 1.4   27 May 1998 20:37:52   MKEATIN
// Defining AVAPI_MT for Win32
//
//    Rev 1.3   27 May 1998 19:47:28   MKEATIN
// Added tag_VCONTEXTTYPE and tag_HVIRUS
//
//    Rev 1.2   27 May 1998 17:42:26   MKEATIN
// We now include navcb_l.h instead of navcb.h
//
//    Rev 1.1   21 May 1998 20:32:38   MKEATIN
// Changed pamapi.h to pamapi_l.h
//
//    Rev 1.0   21 May 1998 20:22:36   MKEATIN
// Initial revision.
//
//    Rev 1.90   04 Mar 1998 17:55:22   DBuches
// Added VLDetermineVirusIndex.
//
//    Rev 1.89   19 Feb 1998 16:23:04   RStanev
// Prototyped VLQuarantineFile().
//
//    Rev 1.88   09 Sep 1997 12:38:18   MKEATIN
// Added the szNavexInfFile to HVCONTEXT.
//
//    Rev 1.87   13 Aug 1997 19:04:32   MKEATIN
// Woops, I should not have checked that last version into the trunk.
//
//    Rev 1.85   31 Jul 1997 16:51:00   MKEATIN
// Added two parameters to VLGetBootSectorLocation().
//
//    Rev 1.84   31 Jul 1997 11:23:02   DDREW
// Mod to include more structure members for NLM platform
//
//    Rev 1.83   18 Jun 1997 16:59:06   MKEATIN
// Removed prototype for VLGetDefsLocation().
//
//    Rev 1.82   17 Jun 1997 14:46:30   MKEATIN
// Modified VLScanInit() to take an LPAVGLOBALCALLBACKS and added the
// prototype VLGetDefsLocation().
//
//    Rev 1.81   04 Jun 1997 13:08:24   MKEATIN
// Added the prototype for VLNumClaimedVirusEntries().
//
//    Rev 1.80   31 May 1997 16:34:48   MKEATIN
// Added lpvCookie to the repair functions
//
//    Rev 1.79   27 May 1997 19:00:04   MKEATIN
// Added the NTK platform to NAVEX 1.5 technology.
//
//
//    Rev 1.78   27 May 1997 16:52:04   RStanev
// Fixed a typo.
//
//    Rev 1.77   27 May 1997 16:36:10   RStanev
// Fixed the prototype of VLMoveFile().  Replaced VLInocMoveFile() with
// VLMoveFileEx().
//
//    Rev 1.76   21 May 1997 16:09:44   MKEATIN
// Aded the SYM_VXD platform to the NAVEX 1.5 technology.
//
//    Rev 1.75   20 May 1997 19:06:10   MKEATIN
// removed the VLDeleteEntry() prototype.
//
//    Rev 1.74   20 May 1997 12:46:50   MKEATIN
// Removed prototype for VLCacheVirusInfo().  Also defined the new flag
// VLINIT_CREATE_BOOT_INFO_CACHE.
//
//    Rev 1.73   13 May 1997 13:48:10   TIVANOV
// prototype for the VLInocMoveFile
//
//    Rev 1.72   12 May 1997 17:38:34   MKEATIN
// Added two new members to hContext:
//  - lpstNavexMemoryHandle and lpstEngineMemoryHandle
//
//
//    Rev 1.71   09 May 1997 17:41:02   MKEATIN
// More temp build fixes
//
//    Rev 1.70   09 May 1997 17:10:16   MKEATIN
// Fixed build issues for NAVAP
//
//    Rev 1.69   08 May 1997 15:50:00   MKEATIN
// VLScanInit now takes a CALLBACKREV2 structer or pass NULL to use the
// local default structer.
//
//    Rev 1.65   28 Apr 1997 18:21:30   MKEATIN
// rollback to 1.63 and include n30type.h instead of avenge.h
//
//    Rev 1.63   28 Apr 1997 13:43:00   MKEATIN
// Made changes to support the AVENGE scanning engine.
//
//    Rev 1.62   26 Feb 1997 16:01:06   RSTANEV
// Prototyped VLMoveFile(), VLRenameFile(), and BuildUniqueFileName().
//
//    Rev 1.61   25 Feb 1997 23:31:32   RSTANEV
// Exported BackupInfectedFile() for SYM_VXD.
//
//    Rev 1.60   12 Aug 1996 17:04:14   RPULINT
// Added macro, windows and agent virus types
//
//    Rev 1.59   25 Jul 1996 18:59:04   DALLEE
// For SYM_WIN, added prExportTable and hExportTable to HVCONTEXT
// for dynamically loading NAVEX DLL and getting EXTScanXXX pointers.
//
//    Rev 1.58   27 Jun 1996 20:56:50   DALLEE
// LuigiPlus port:
// Nothing from VIRLIB 8, but I did take the opportunity to remove the
// SYM_W32 kluge and change the #ifdef's to SYM_WIN32.
// Also applied the necessary changes to NAVFAMLY.
//
//    Rev 1.57   24 Jun 1996 15:13:14   MZAREMB
// Added DOSX to the list of USE_PAM customers.
//
//    Rev 1.56   21 May 1996 17:17:32   CNACHEN
// Make sure to define SYM_W32 AND SYM_WIN32 because both
// are used! :)
//
//    Rev 1.55   16 May 1996 14:44:42   RSTANEV
// Defining USE_PAM for SYM_NTK platform.
//
//    Rev 1.54   01 May 1996 17:07:40   RSTANEV
// Added VLINIT_NO_PAM.
//
//    Rev 1.53   16 Apr 1996 10:37:44   RSTANEV
// SYM_NTK changes.
//
//    Rev 1.52   01 Apr 1996 19:50:04   RSTANEV
// Exported BackupInfectedFile().
//
//    Rev 1.51   28 Mar 1996 18:01:58   TCASHIN
// Made VLValidContext an EXPORT.
// Check to see if USE_PAM needs to be #undef'd (for NAVOEM).
//
//    Rev 1.50   14 Mar 1996 18:25:08   CNACHEN
// moved #define USE_PAM to avapi.h so that all CPP files that include AVAPI.H
// will properly define USE_PAM and the hContext will be of the proper size.
// It's defined for NLM, W32 and WIN16...
//
//
//    Rev 1.49   07 Feb 1996 15:34:04   CNACHEN
// Added PAM support for NLM platform...
//
//    Rev 1.48   07 Feb 1996 11:08:30   CNACHEN
// Added PAM changes for NLM...
//
//    Rev 1.47   05 Feb 1996 09:30:52   RSTANEV
// Synchronized APIs with AVAPI.CPP.
//
//    Rev 1.46   02 Feb 1996 20:02:52   RSTANEV
// Added Unicode support, cleaned up SYM_VXD platform and converted
// the base types of some structure members & related APIs.
//
//    Rev 1.45   11 Jan 1996 07:50:56   JWORDEN
// Merge changes for Quake 8 and Quake 6 back to the trunk
//
//    Rev 1.43   20 Nov 1995 10:38:24   GDZIECI
// Added VLGetNameDB() prototype.
//
//    Rev 1.42   13 Nov 1995 12:26:48   GDZIECI
// Merged VIRLIB 6 changes (with the exception of NAVEX support - will be added
// later).
//
//    Rev 1.41   11 Aug 1995 14:31:52   RSTANEV
// Added prExportTable to HVCONTEXT.
//
//    Rev 1.40   25 May 1995 17:19:26   RSTANEV
// Now HVCONTEXT does not keep the structure with the callbacks, but it
// keeps only a pointer to it.
//
//    Rev 1.39   25 May 1995 08:36:04   RSTANEV
// Oops.
//
//    Rev 1.38   25 May 1995 08:29:00   RSTANEV
// The inline functions will not be available if not compiling C++.
//
//    Rev 1.37   25 May 1995 03:39:48   SZIADEH
// corrected a compile problem for W32.
//
//    Rev 1.36   25 May 1995 03:28:56   SZIADEH
// no changes.
//
//    Rev 1.34   24 May 1995 20:54:04   RSTANEV
// Added support for multiple character set environments.
//
//    Rev 1.33   11 May 1995 16:18:48   SZIADEH
// declation for VLSaveMemoryInfectionData()
//
//    Rev 1.32   17 Apr 1995 09:00:14   RSTANEV
// Added flags and function prototypes for overriding the repair and deletion
// protections for DIR2.
//
//    Rev 1.31   31 Mar 1995 15:44:22   AMATIC
// No change.
//
//    Rev 1.30   02 Mar 1995 07:30:34   RSTANEV
// Enabled caching of N30 records for all platforms.
//
//    Rev 1.29   24 Feb 1995 22:30:26   RSTANEV
// Added the prototype of VLCacheVirusData(), and also supporting structures
// and defines.
//
//    Rev 1.28   17 Feb 1995 09:56:36   RSTANEV
// Added MAX_RECURSIVE_FILE_REPAIRS.
//
//    Rev 1.27   17 Feb 1995 09:19:04   RSTANEV
// Added PHVIRUS.
//
//    Rev 1.26   08 Feb 1995 17:03:50   SZIADEH
// prototyped VLCreateSupportFiles()
//
//    Rev 1.25   30 Jan 1995 18:45:58   JMILLARD
//
// add VLTrueNumVirusEntries
//
//    Rev 1.24   19 Jan 1995 16:07:10   SZIADEH
// added VL_SCAN640 const
//
//    Rev 1.22   20 Dec 1994 14:14:16   JMILLARD
// add fast virus checks (NLM only for now) to VL context struct
// add VLRegisterFastVirusDB
//
//    Rev 1.21   06 Dec 1994 16:20:08   JMILLARD
// interim check in of Sami's changes
//
//    Rev 1.20   02 Sep 1994 08:03:18   RSTANEV
// Added NAVCore_Is_Compatible().
//
//    Rev 1.19   30 Aug 1994 22:45:44   RSTANEV
// Added NAVCore_Get_Version() prototype.
//
//    Rev 1.18   30 Aug 1994 12:37:16   SZIADEH
// . defined a structure of type to keep track of memory records (virus id,
// sig type, and actual signature ) called RECORD_MEMORY_SIGNATURE.
//
//    Rev 1.17   26 Aug 1994 18:32:26   SZIADEH
// added vlinit_handle_no_access flag.  Set in VLInit call to return
// an HVIRUS on failed access.
// This changes the default behavior of the scanning routines -- by default, no
// handle will be returned on failed access.  VS_NO_ACCESS will be returned
// by the scanning functions regardless of the flag setting and value in HVIRUS.
//
//
//    Rev 1.16   26 Aug 1994 17:33:06   DALLEE
// Added this header, so I don't have to use CHKLOG all the time!
//************************************************************************

///////////////////////////////////////////////////////////////////////////////
// Data structures
///////////////////////////////////////////////////////////////////////////////

#ifndef AVAPI_INCLUDE

#define AVAPI_INCLUDE

#include "pamapi_l.h"
#include "avtypes.h"
#include "callbk15.h"
#include "n30type.h"
#include "navcb_l.h"

// This macro causes AVAPI to compile in a threadsafe manner.  Work
// buffers are localized to a single scan/repair and AVENGE/PAM is locally
// initialized and closed per scan.

#if defined(SYM_VXD) || defined(SYM_NTK) || defined(SYM_WIN32) || defined(SYM_NLM)
    #if !defined(AVAPI_MT)
        #define AVAPI_MT
    #endif
#endif

//////////////////////
// early definitions needed for VCONTEXT type
//////////////////////

// this structure is used by SCN4_Verify_Virus and VLRegisterFastVirusDB do
// fast virus verifications in memory

typedef struct
{
    WORD    wVirID;                     // Virus ID Number for safety
    WORD    wCheckData;                 // CRC check value
    WORD    wControl;                   // various flags - returned to caller

    char    szVName[N30_SIZE_VIRUSNAME];    // Virus Name

} FASTVIRUSCHECK;
typedef FASTVIRUSCHECK FAR *LPFASTVIRUSCHECK;

//////////////////////
// enhanced structures
//////////////////////
typedef struct N30E
    {
    UINT uVirusEnumerator;
    N30  rN30;
    } N30E, *PN30E, FAR *LPN30E;

//////////////////////
// vstatus definitions
//////////////////////

typedef WORD    VSTATUS;

#define VS_OK                   ((VSTATUS)0)
#define VS_MEMALLOC             ((VSTATUS)1)
#define VS_FILE_ERROR           ((VSTATUS)2)
#define VS_CRIT_ERROR           ((VSTATUS)3)
#define VS_CANT_REPAIR          ((VSTATUS)4)
#define VS_DRIVE_ERROR          ((VSTATUS)5)
#define VS_INVALID_ARG          ((VSTATUS)6)    // bad argument to function
#define VS_ENTRY_UNAVAIL        ((VSTATUS)7)    // virus list entry deleted/mac
#define VS_NO_EXIST             ((VSTATUS)8)
#define VS_WRONG_VERSION        ((VSTATUS)9)
#define VS_CANT_DELETE          ((VSTATUS)10)
#define VS_NO_ACCESS            ((VSTATUS)11)
#define VS_SORTERROR            ((VSTATUS)12)
#define VS_INSCAN_CREATION      ((VSTATUS)13)

#define DEAD_VIRUS_ENTRY        0xDEAD

// callbacks are now defined in CALLBACK.H

///////////////////////////////////////////////
// tagGENG and tagLENG are defined in avenge.h
///////////////////////////////////////////////

typedef struct tagGENG FAR *HGENG;
#if !defined(AVAPI_MT)
    typedef struct tagLENG FAR *HLENG;
#endif

typedef struct tag_VCONTEXTTYPE
{
        DWORD               dwSignature;   // make sure this is valid!
        LPCALLBACKREV2      lpstCallBack;  // NAVEX callback
        LPAVGLOBALCALLBACKS lpGlobalCallBacks;

        ///////////////////////////////////////////////////////////////////
        // Current Engine Specific Information
        //
        // This will be changed/replaced if and when we switch engine tech.
        ///////////////////////////////////////////////////////////////////

        TCHAR   szVirScanDataFile[SYM_MAX_PATH + 1];
        TCHAR   szVirScanInfoFile[SYM_MAX_PATH + 1];
        TCHAR   szPAMScanDataFile[SYM_MAX_PATH + 1];
        TCHAR   szNavexInfFile[SYM_MAX_PATH + 1];

#if defined(SYM_VXD)

        char    szAnsiVirScanDataFile[SYM_MAX_PATH];
        char    szAnsiVirScanInfoFile[SYM_MAX_PATH];
        char    szAnsiPAMScanDataFile[SYM_MAX_PATH + 1];
        char    szUniVirScanDataFile[SYM_MAX_PATH * sizeof(WCHAR)];
        char    szUniVirScanInfoFile[SYM_MAX_PATH * sizeof(WCHAR)];
        char    szUniPAMScanDataFile[SYM_MAX_PATH * sizeof(WCHAR)];

#endif

        WORD    wFlags;

        UINT    uN30EFCount;
        UINT    uN30EBCount;

        LPN30E  lprN30EFBuffer;             // perm
        LPN30E  lprN30EBBuffer;             // perm

        HGENG   hGEng;                      // AVENGE Global Handle
        PAMGHANDLE hGPAM;                   // PAM Global Handle

#if !defined(AVAPI_MT)

        HLENG   hLEng;                      // AVENGE Local Handle
        PAMLHANDLE hLPAM;                   // PAM Local Handle

#endif

        LPVOID  prExportTable;              // NAVEX function pointers.
        LPVOID  prEngineTable;              // NAVENG function pointers.

#if defined(SYM_WIN) || defined(SYM_NLM)

        HINSTANCE  hExportLib;              // NAVEX library handle.
        HINSTANCE  hEngineLib;              // NAVENG library handle.

#elif defined(SYM_DOSX)

        LPSTR  lpstNavexMemoryHandle;
        LPSTR  lpstEngineMemoryHandle;

#elif defined(SYM_UNIX)

        void* hExportLib;                  // NAVEX library handle
        void* hEngineLib;                  // NAVENG library handle

#endif

#ifdef SYM_NLM

        LPFASTVIRUSCHECK
                lpFastCheckDB;              // currently used by NLM
        UINT    uTotalDefs;

#endif
} VCONTEXTTYPE;


typedef VCONTEXTTYPE FAR *HVCONTEXT;

#define sVCONTEXTTYPE sizeof(VCONTEXTTYPE)


typedef struct tag_HVIRUS
{
        HVCONTEXT       lpVContext;             // scanner context information

        struct          tag_HVIRUS FAR *hNext;

        UINT            uDBEntryNum;            // virus entry # in VIRSCAN.DAT
        N30             stVirusInfoBlock;       // virus entry from VIRSCAN.DAT

        UINT            uDate;
        UINT            uTime;

        DWORD           dwFlags;                // status flags

        DWORD           dwPOFlags;              // protection overriding flags

        union
        {
                struct
                {
                        TCHAR   szFileName[SYM_MAX_PATH + 1];
#if defined(SYM_VXD)
                        char    szAnsiFileName[SYM_MAX_PATH];
                        char    szUniFileName[SYM_MAX_PATH * sizeof(WCHAR)];
#endif

                } FileInfo;

                struct
                {
                        BYTE    byDriveNum;     // A:=0, B:=1, C:=0x80, etc
                        BYTE    byBootSecNum;
                        BYTE    byBootFlags;
                        BYTE    bySystemType;   // read from MBR (DOS16, etc)
                } BootInfo;
        };

} VIRINFOTYPE;

typedef VIRINFOTYPE    FAR *HVLVIRUS;
typedef HVLVIRUS         FAR *LPHVLVIRUS;
typedef HVLVIRUS         *PHVLVIRUS;

#define sHVLVIRUS sizeof(VIRINFOTYPE)


typedef struct
{
        HVCONTEXT       lpVContext;             // scanner context information
        UINT            uDBEntryNum;            // our entry number in database
        WORD            wFlags;                 // information about entry
        N30             stVirusInfoBlock;       // virus entry from VIRSCAN.DAT

} VIRUSENTRYTYPE;

typedef VIRUSENTRYTYPE FAR *HVLVIRUSENTRY;
#if defined(SYM_WIN16)
typedef HVLVIRUSENTRY _huge *HVLVIRUSENTRYTABLE;
#else
typedef HVLVIRUSENTRY FAR *HVLVIRUSENTRYTABLE;
#endif

#define sVIRUSENTRYTYPE sizeof(VIRUSENTRYTYPE)


typedef struct
{
    WORD     wVirusId;
    WORD     wSigType;
    LPBYTE   lpSignature;

}RECORD_MEMORY_SIGNATURE;

typedef RECORD_MEMORY_SIGNATURE FAR *LPRECORD_MEMORY_SIGNATURE;
#define sRECORD_MEMORY_SIGNATURE sizeof(RECORD_MEMORY_SIGNATURE);


typedef struct {
    LPBYTE  lpStartRegion;
    WORD    wSize;
    WORD    wMemoryType;
}MEMORY_REGION;

typedef MEMORY_REGION FAR *LPMEMORY_REGION;
#define sMEMORY_REGION sizeof(MEMORY_REGION);

typedef struct {
    LPBYTE  lpCD;
    WORD    CDCount;
    LPBYTE  lpTOM;
    WORD    TOMCount;
    LPBYTE  lpALL;
    WORD    ALLCount;
} MEMORY_SIG_TYPES;

typedef MEMORY_SIG_TYPES FAR *LPMEMORY_SIG_TYPES;

#define sMEMORY_SIG_TYPES sizeof(MEMORY_SIG_TYPES);

//////////////////////////////////////////////////////////////////////////////


/////////////////////
// dwFlags bit settings
/////////////////////

#define VIT_SCANNED             0x0001
#define VIT_REPAIRED            0x0002
#define VIT_UNREPAIRABLE        0x0004
#define VIT_VALID_N30           0x0008
#define VIT_FOUND_IN_MEMORY     0x0010
#define VIT_NO_ACCESS           0x0020
#define VIT_HAS_A_VIRUS         0x0040
#define VIT_BOOTPART            0x0080
#define VIT_UNKOWN_VIRUS        0x0100
#define VIT_FILE                0x0200
#define VIT_DISEMBODIED         0x0400
#define VIT_DELETED             0x0800
#define VIT_CANNOT_DELETE       0x1000

///////////////////////////
// dwPOFlags bit settings
///////////////////////////

#define fPO_DIR2_DELETE         0x0001
#define fPO_DIR2_REPAIR         0x0002

//////////////////////////////////////////////////////////
// wFlags bit settings (for HVLVIRUSENTRYs)
//////////////////////////////////////////////////////////

#define VET_DEF_DELETED         0x0001

///////////////////////////
// wFlags for memory scanning
///////////////////////////
#define  VL_SCANHI               0x0001
#define  VL_SCAN640              0x0000              // scan up to 640K
#define  VL_ONE_HALF_ID          0x08EE              // VIRUS_ID as defined in
                                                     // def08.src for the
                                                     // one_half(m) virus

/////////////////////////////
// Flags for dat file caching
/////////////////////////////
#define fVL_CACHE_FILE          0x0001
#define fVL_CACHE_BOOT          0x0002

///////////////////////////
// byBootFlags bit settings
///////////////////////////

#define BI_BOOT_SECTOR          1
#define BI_PARTITION_SECTOR     2

#define HARD_DRIVE_START        0x80


// memory allocation size constants

#define     WORK_BUFFER_SIZE                4096
#define     SECTOR_SIZE                     512
#define     NUM_OVERWRITE_PASSES            6

#define     CHUNK_N30_ENTRIES               100

// revision constants

#define     VL_REVISION                     1

// invalid virus entry number handle

#define INVALID_ENTRY_NUM                   0

// MSCANFLAG flag values

typedef WORD    MSCANFLAG;


#define MSF_SCAN_LOW                        0
#define MSF_SCAN_HIGH                       1


// # of repair tries

#define MAX_BOOT_REPAIR_TRIES                5
#define MAX_FILE_REPAIR_TRIES               20
#define MAX_RECURSIVE_FILE_REPAIRS           5


// VLScanInit flags

#define VLINIT_NO_ACCESS_NO_HANDLE          0
#define VLINIT_HANDLE_NO_ACCESS             1
#define VLINIT_NO_PAM                       2
#define VLINIT_CREATE_BOOT_INFO_CACHE       4


//////////////////////////
// Virus info enumerations
//////////////////////////

enum    enumVirusInfo   {

        ///////////////////////////////////////////////////////////////////////
        // N30 specific queries.
        ///////////////////////////////////////////////////////////////////////

        VI_VIRUS_NAME,                  // obtain virus's name
        VI_VIRUS_ALIAS,                 // obtain virus's alias list
        VI_VIRUS_INFO,                  // obtain virus information
        VI_VIRUS_SIZE,                  // obtain virus's size
        VI_VIRUS_ID,                    // obtain virus's ID

        VI_INFECTION_DEPTH,             // how many times is object infected

        VI_BOOL_INFECT_FILES,           // does this virus infect FILES in general? (MAC too)
        VI_BOOL_INFECT_EXE,             // does this virus infect EXE's?
        VI_BOOL_INFECT_COM,             // does this virus infect COM's?
        VI_BOOL_INFECT_COMCOM,          // does this virus infect COMMAND.COM?
        VI_BOOL_INFECT_SYS,             // does this virus infect SYS's?
        VI_BOOL_INFECT_MBR,             // does this virus infect MBR's?
        VI_BOOL_INFECT_FLOPPY_BOOT,     // does this virus infect floppies?
        VI_BOOL_INFECT_HD_BOOT,         // does this virus infect HD bootsecs?

        VI_BOOL_INFECT_WILD,            // is this virus in the wild?

        VI_BOOL_MEMORY_RES,             // can this virus go memory resident?
        VI_BOOL_SIZE_STEALTH,           // does this virus size stealth?
        VI_BOOL_FULL_STEALTH,           // does this virus full stealth?
        VI_BOOL_TRIGGER,                // does this virus have a trigger?
        VI_BOOL_ENCRYPTING,             // is this virus encrypting?
        VI_BOOL_POLYMORPHIC,            // is this virus polymorphic?
        VI_BOOL_MULTIPART,              // is this virus multipartite?
        VI_BOOL_MAC_VIRUS,              // macintosh virus
        VI_BOOL_MACRO_VIRUS,            // macro virus
        VI_BOOL_WINDOWS_VIRUS,          // windows virus
        VI_BOOL_AGENT_VIRUS,            // agent virus

        ///////////////////////////////////////////////////////////////////////
        // item specific queries
        ///////////////////////////////////////////////////////////////////////

        VI_BOOL_REPAIRED,               // has this specific item been repaired?
        VI_BOOL_REPAIR_FAILED,          //
        VI_BOOL_REPAIRABLE,             // is this specific item repairable?
        VI_BOOL_DELETABLE,              // is this virus deletable?
        VI_BOOL_NO_ACCESS,              // check if we have access to this item
        VI_BOOL_DISEMBODIED,            // is the item disembodied (mem/boo)
        VI_BOOL_DELETED,                // has this file been deleted yet?
        VI_BOOL_DELETE_FAILED,          //
        VI_BOOL_DEF_DELETED,            // def been deleted from VIRSCAN.DAT?
        VI_BOOL_INFO_VALID,             // is this information valid?
        VI_BOOL_MEMORY_DETECT,          // is this item a memory detection?
        VI_BOOL_FILE_ITEM,              // is this item a file detection?
        VI_BOOL_BOOT_ITEM,              // is this item a boot detection?
        VI_BOOL_PART_ITEM,              // is this item a part detection?

        VI_ITEM_NAME,                   // the item name (filename or otherwise)
        VI_DETECT_DATE,
        VI_DETECT_TIME,
        VI_FILENAME,                    // obtain filename of scanned item
        VI_PHYSICAL_DRIVE_NUM,          // get physical drive # of scanned item
        VI_PARTITION_NUM,               // get partition num of scanned item
        VI_PARTITION_TYPE               // get partition type of scanned item


        };

typedef enum enumVirusInfo VLVIRINFO;




//////////////////////////////
// Virus database enumerations
//////////////////////////////

enum    enumDataBaseInfo   {

        DB_TOTAL_VIRUS_COUNT,
        DB_MONTH_STAMP,
        DB_DAY_STAMP,
        DB_YEAR_STAMP,
        DB_VERSION_STAMP
        };

typedef enum enumDataBaseInfo VLDBINFO;


// --------------------------------------------------------------------------
//                  EXPORTED FUNCTION PROTOTYPES
// --------------------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif

HVCONTEXT  SYM_EXPORT WINAPI VLScanInit (
    UINT                uRevision,
    LPAVGLOBALCALLBACKS lpCallBacks,
    WORD                wFlags );

VSTATUS WINAPI VLCreateSupportFiles (
    HVCONTEXT hContext );

VSTATUS SYM_EXPORT WINAPI VLScanClose (
    HVCONTEXT   hContext );

VSTATUS SYM_EXPORT WINAPI VLReleaseVirusHandle (
    HVLVIRUS  hInfo );

VSTATUS SYM_EXPORT WINAPI VLReleaseEntry (
    HVLVIRUSENTRY hInfo );

BOOL SYM_EXPORT WINAPI VLValidContext (
    HVCONTEXT hContext);


/////////////////////////////////////////////////////////////////
// Virus Database registration prototype (this call is required!)
/////////////////////////////////////////////////////////////////

VSTATUS SYM_EXPORT WINAPI VLRegisterVirusDB (
    HVCONTEXT   hContext,
    LPTSTR      lpszDBFile,
    LPTSTR      lpszInfoFile,
    LPTSTR      lpszNavexInfFile );

VSTATUS SYM_EXPORT WINAPI VLRegisterFastVirusDB (
    HVCONTEXT   hContext,
    LPSTR       lpszDBFile,
    LPSTR       lpszInfoFile );

UINT SYM_EXPORT WINAPI VLFillFastCheckDB
(
    HVCONTEXT           hContext,           // [in] a valid HVCONTEXT handle
    int                 nMaxEntries,        // [in] size of prealloc'd array
    LPFASTVIRUSCHECK    lpFastCheckDB       // [out] prealloc'd array
);

VSTATUS SYM_EXPORT WINAPI VLGetDBInfo (
    HVCONTEXT   hContext,               // [in] a valid HVCONTEXT handle
    VLDBINFO    enumInfo,               // [in] which info should we return?
    LPTSTR      lpszResultBuffer        // [out] string converted result
    );

///////////////////////////////////////////////////////////////////////////////
// Virus check routines
///////////////////////////////////////////////////////////////////////////////

DWORD SYM_EXPORT WINAPI  VLCheckFastDB(
    HVCONTEXT hContext,         // [in] a valid HVCONTEXT handle
    WORD      wFirstWord,       // [in] check value for CRC defs
    WORD      wVirusId,         // [in] virus id to check
    LPBYTE    szVirusName );    // [out] virus name returned

DWORD SYM_EXPORT WINAPI  VLGetNameDB(
    HVCONTEXT hContext,         // [in] a valid HVCONTEXT handle
    WORD      wVirusId,         // [in] virus id
    LPBYTE    szVirusName );    // [out] virus name returned

///////////////////////////////////////////////////////////////////////////////
// Virus scanning prototypes
///////////////////////////////////////////////////////////////////////////////

VSTATUS SYM_EXPORT WINAPI VLScanFile (
    HVCONTEXT   hContext,
    LPTSTR      lpszFileName,
    LPHVLVIRUS  lphVirus,
    BOOL        bPreserveLastAccess
    );

VSTATUS SYM_EXPORT WINAPI VLScanBoot(
    HVCONTEXT   hContext,
    BYTE        byDrive,
    BYTE        byBootSecNum,
    LPBYTE      lpBooBuffer,
    LPHVLVIRUS  lphVirus );

VSTATUS SYM_EXPORT WINAPI VLScanPart (
    HVCONTEXT   hContext,
    BYTE        byDrive,
    LPBYTE      lpBooBuffer,
    LPHVLVIRUS  lphVirus );

VSTATUS SYM_EXPORT WINAPI VLScanMemory (
    HVCONTEXT   hContext,
    WORD        wFlags,
    LPHVLVIRUS  lphVirus );

///////////////////////////////////////////////////////////////////////////////
// Virus repair prototypes
///////////////////////////////////////////////////////////////////////////////

VSTATUS SYM_EXPORT WINAPI VLRepairItem (
    HVLVIRUS hVirus,
    LPVOID   lpvCookie);

BOOL SYM_EXPORT WINAPI VLCanRepairItem (
    HVLVIRUS hVirus);

BOOL SYM_EXPORT WINAPI VLCanDeleteItem (
    HVLVIRUS hVirus);

VSTATUS SYM_EXPORT WINAPI VLRepairFile (
    HVLVIRUS hVirus,
    LPVOID   lpvCookie);

VSTATUS SYM_EXPORT WINAPI VLRepairBootPart (
    HVLVIRUS hVirus,
    LPVOID   lpvCookie);

VSTATUS SYM_EXPORT WINAPI VLDeleteFile (
    HVLVIRUS  hVirus );

#if !defined(SYM_NLM)

VSTATUS SYM_EXPORT WINAPI VLQuarantineFile (
    HVLVIRUS hVirus,
    LPCTSTR  pszQuarantineLocation
    );

VSTATUS SYM_EXPORT WINAPI VLMoveFile (
    HVLVIRUS hVirus,
    LPCTSTR  pszNewLocation,
    LPCTSTR  pszFileExtension,
    LPTSTR   pszMovedFile
    );

VSTATUS SYM_EXPORT WINAPI VLMoveFileEx (
    LPCTSTR pszFileName,
    LPCTSTR pszNewLocation,
    LPCTSTR pszFileExtension,
    LPTSTR  pszMovedFile
    );

VSTATUS SYM_EXPORT WINAPI VLRenameFile (
    HVLVIRUS  hVirus,
    LPCTSTR pszNewFileName,
    LPTSTR  pszRenamedFile );

#endif

///////////////////////////////////////////////////////////////////////////////
// Functions for changing the value returned by VLCanRepairItem() and
// VLCanDeleteItem(), and for changing the functionality of VLRepairFile()
// and VLDeleteFile().
///////////////////////////////////////////////////////////////////////////////

VSTATUS SYM_EXPORT WINAPI VLOverrideDIR2RepairProtection (
    HVLVIRUS  hVirus );

VSTATUS SYM_EXPORT WINAPI VLOverrideDIR2DeleteProtection (
    HVLVIRUS  hVirus );

///////////////////////////////////////////////////////////////////////////////
// Virus information prototypes
///////////////////////////////////////////////////////////////////////////////

BOOL SYM_EXPORT WINAPI VLVirusInfo (
    HVLVIRUS      hVirus,
    WORD        wLevel,
    VLVIRINFO   enumVirInfo,
    LPTSTR      lpszResultBuffer );

///////////////////////////////////////////////////////////////////////////////
// Virus list prototypes
///////////////////////////////////////////////////////////////////////////////

UINT SYM_EXPORT WINAPI VLNumVirusEntries (
    HVCONTEXT   hContext );

DWORD SYM_EXPORT WINAPI VLNumClaimedVirusEntries (
    HVCONTEXT   hContext );

UINT SYM_EXPORT WINAPI VLTrueNumVirusEntries (
    HVCONTEXT   hContext );

VSTATUS SYM_EXPORT WINAPI VLDetermineVirusIndex(HVCONTEXT hContext,
                                     WORD   wVirusID,
                                     LPWORD lpwVirusIndex);

HVLVIRUSENTRY SYM_EXPORT WINAPI VLGetVirusEntry (
    HVCONTEXT   hContext,
    UINT        uEntryNum );

UINT SYM_EXPORT WINAPI VLFillListTable (
    HVCONTEXT        hContext,
    HVLVIRUSENTRYTABLE lpFillTable );

BOOL SYM_EXPORT WINAPI VLDefInfo (
    HVLVIRUSENTRY hVirus,
    VLVIRINFO   enumVirInfo,
    LPTSTR      lpszResultBuffer );

BOOL SYM_EXPORT VLSaveMemoryInfectionData (
    HVLVIRUS hVirus,
    UINT uFlags );

///////////////////////////////////////////////////////////////////////////////
// Other exported functions.
///////////////////////////////////////////////////////////////////////////////
#if defined(SYM_NTK) || defined(SYM_VXD)

BOOL WINAPI BuildUniqueFileName (
    LPCTSTR lpszBaseFile,
    LPCTSTR lpszExtension,
    LPTSTR  lpszUniqueFile
    );

BOOL WINAPI BackupInfectedFile (
    LPTSTR lpszInfectedFile,
    LPTSTR lpszBackupExtension,
    LPTSTR lpszBackupFile );

#endif

// --------------------------------------------------------------------------
//                  END OF EXPORTED FUNCTION PROTOTYPES
// --------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif

///////////////////////////////////////////////////////////////////////////////
// #defines for quick access to callback functions
///////////////////////////////////////////////////////////////////////////////

#define     VLFileOpen         hContext->lpstCallBack->FileOpen
#define     VLFileClose        hContext->lpstCallBack->FileClose
#define     VLFileRead         hContext->lpstCallBack->FileRead
#define     VLFileWrite        hContext->lpstCallBack->FileWrite
#define     VLFileSeek         hContext->lpstCallBack->FileSeek
#define     VLFileSize         hContext->lpstCallBack->FileSize
#define     VLFileGetDateTime  hContext->lpstCallBack->FileGetDateTime
#define     VLFileSetDateTime  hContext->lpstCallBack->FileSetDateTime
#define     VLFileDelete       hContext->lpstCallBack->FileDelete
#define     VLFileGetAttr      hContext->lpstCallBack->FileGetAttr
#define     VLFileSetAttr      hContext->lpstCallBack->FileSetAttr
#define     VLFileCreate       hContext->lpstCallBack->FileCreate

#define     VLProgress         hContext->lpstCallBack.Progress

#define     VLSDiskReadPhysical  hContext->lpstCallBack->SDiskReadPhysical
#define     VLSDiskWritePhysical hContext->lpstCallBack->SDiskWritePhysical

#define     VLTempMemoryAlloc   hContext->lpstCallBack->TempMemoryAlloc
#define     VLPermMemoryAlloc   hContext->lpstCallBack->PermMemoryAlloc

#define     VLTempMemoryFree    hContext->lpstCallBack->TempMemoryFree
#define     VLPermMemoryFree    hContext->lpstCallBack->PermMemoryFree
// --------------------------------------------------------------------------
//   FUNCTION PROTOTYPES FOR INTERNAL USER BUT LOCATED IN A DIFFERENT FILE
// --------------------------------------------------------------------------

VOID WINAPI VLGetBootSectorLocation (
    HVCONTEXT hContext,
    BYTE    byDriveNumber,
    LPVOID  lpPartitionBuffer,
    UINT    uEntry,
    LPWORD  lpwCylinder,
    LPBYTE  lpbSector,
    LPBYTE  lpbHead,
    LPBYTE  lpbType );

WORD SYM_EXPORT WINAPI GetMemoryAndScanIt(
    HVCONTEXT           hContext,
    LPMEMORY_SIG_TYPES lpMemorySigTypes,
    WORD               wFlags);

//--------------------------------------------------------------------------
//
// Inline functions for accessing string members of HVCONTEXT and HVLVIRUS.
//
//--------------------------------------------------------------------------
#if defined(SYM_VXD)
PSTR inline HVC_szVirScanDataFile ( HVCONTEXT hVC )
    {
    switch ( VxDGetCurrentCHARSET() )
        {
        case CHARSET_UNICODE:
            return ( hVC->szUniVirScanDataFile );
        case CHARSET_ANSI:
            return ( hVC->szAnsiVirScanDataFile );
        }
    return ( hVC->szVirScanDataFile );
    }
#else
#define HVC_szVirScanDataFile(hVC)  ((hVC)->szVirScanDataFile)
#endif

#if defined(SYM_VXD)
PSTR inline HVC_szVirScanInfoFile ( HVCONTEXT hVC )
    {
    switch ( VxDGetCurrentCHARSET() )
        {
        case CHARSET_UNICODE:
            return ( hVC->szUniVirScanInfoFile );
        case CHARSET_ANSI:
            return ( hVC->szAnsiVirScanInfoFile );
        }
    return ( hVC->szVirScanInfoFile );
    }
#else
#define HVC_szVirScanInfoFile(hVC)  ((hVC)->szVirScanInfoFile)
#endif

#if defined(SYM_VXD)
PSTR inline HV_FileInfo_szFileName ( HVLVIRUS hVir )
    {
    switch ( VxDGetCurrentCHARSET() )
        {
        case CHARSET_UNICODE:
            return ( hVir->FileInfo.szUniFileName );
        case CHARSET_ANSI:
            return ( hVir->FileInfo.szAnsiFileName );
        }
    return ( hVir->FileInfo.szFileName );
    }
#else
#define HV_FileInfo_szFileName(hVir)    ((hVir)->FileInfo.szFileName)
#endif


#if defined(SYM_VXD)
PSTR inline HVC_szPAMScanDataFile ( HVCONTEXT hVC )
    {
    switch ( VxDGetCurrentCHARSET() )
        {
        case CHARSET_UNICODE:
            return ( hVC->szUniPAMScanDataFile );
        case CHARSET_ANSI:
            return ( hVC->szAnsiPAMScanDataFile );
        }
    return ( hVC->szPAMScanDataFile );
    }
#else
#define HVC_szPAMScanDataFile(hVC)  ((hVC)->szPAMScanDataFile)
#endif

#endif
