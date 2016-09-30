// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
/* Copyright 1992 - 2003, 2005 Symantec Corporation. All rights reserved.     */
/************************************************************************
 *                                                                      *
 * $Header:   S:/INCLUDE/VCS/xapi.h_v   1.17   20 Oct 1997 18:09:18   MBROWN  $ *
 *                                                                      *
 * Description:                                                         *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * $Log:   S:/INCLUDE/VCS/xapi.h_v  $ *
//
//    Rev 1.17   20 Oct 1997 18:09:18   MBROWN
// New prototypes.
//
//    Rev 1.15   11 Nov 1996 19:18:24   SPAGLIA
// CharToLower and CharToUpper fixed for Win32
//
//    Rev 1.14   19 Sep 1996 13:38:40   SPAGLIA
// Fixed function declarations for DOS_Buff() functions
//
//    Rev 1.13   16 Sep 1996 17:23:46   JOUYANG
// Ansi to Oem and Oem to Ansi conversion for DOS agent added
//
//    Rev 1.12   10 Sep 1996 16:17:06   JBRENNA
// Add ARE_FILE_APIS_ANSI so that we have a consistent naming convention.
// Wish I could eliminate AREFILEAPISANSI, but no why to tell who uses it.
//
//    Rev 1.11   10 Sep 1996 15:40:06   JBRENNA
// Move comment from a #define SET_FILE_APIS_TO_OEM for safety reasons.
// I believe that the #define would expand to include the comment, removing
// anything following the SET_FILE_APIS_TO_OEM by making it a comment (for
// that line only). Though I have not confirmed this as a problem, consider
// it preventive maintenance.
//
//    Rev 1.10   24 Apr 1996 17:40:58   RSTANEV
// Removed some character conversion functions from compilation when
// UNICODE is defined.
//
//    Rev 1.9   23 Apr 1996 23:17:26   JMILLAR
// fix anachronism that msvc 4.1 doesn't like with specifiers for data (NTK only)
//
//    Rev 1.8   16 Apr 1996 15:29:34   PMARTIN
// Added DiskGetBootDriveEx(), which is a cover for DiskGetBootDrive().
//
//    Rev 1.7   27 Feb 1996 18:10:58   Basil
// Added SymkrnlVxDGet/SetName functions for changing the Symkrnl.VxD name.
//
//    Rev 1.6   14 Feb 1996 15:56:42   JWORDEN
// Add define for AREFILEAPISANSI
//
//    Rev 1.5   14 Feb 1996 15:25:56   JWORDEN
// Add definitions for SET_FILE_APIS_TO_OEM and SET_FILE_APIS_TO_ANSI
//
//    Rev 1.4   08 Feb 1996 17:58:38   DBUCHES
// Oops... no ; for macros.
//
//    Rev 1.3   08 Feb 1996 15:04:06   DBUCHES
// Added protos MemoryHugeCompare().
//
//    Rev 1.2   06 Feb 1996 18:49:46   RSTANEV
// Generalized prototypes of CharToUpper() and CharToLower() for SYM_NTK
// and SYM_VXD platforms.
//
//    Rev 1.1   30 Jan 1996 11:06:32   RSTANEV
// _ConvertWordToString() and _ConvertLongToString() prototyped to
// support Unicode.
//
//    Rev 1.0   26 Jan 1996 20:17:30   RJACKSO
// Initial revision.
//
//    Rev 1.157   18 Jan 1996 10:37:22   RSTANEV
// Added Unicode support.
//
//    Rev 1.156   14 Dec 1995 20:38:24   DHERTEL
// Added prototype for _HWIsNec()
//
//    Rev 1.155   12 Dec 1995 10:51:38   DBUCHES
// Added DOS_7_10 for Windows 96 Dos version
//
//    Rev 1.154   11 Dec 1995 17:04:28   HENRI
// Fixed compile problem with WIN16 and C++
//
//    Rev 1.153   11 Dec 1995 15:56:46   HENRI
// Fixed compile compatibility for C++ and UNICODE compiles
//
//    Rev 1.152   01 Dec 1995 18:27:06   RSTANEV
// Fixed conflicts with SYM_NTK platform.
//
//    Rev 1.151   30 Nov 1995 13:31:20   ASUZDALT
// Added decl. for LibraryLoad()
//
//    Rev 1.150   28 Nov 1995 21:13:48   RSTANEV
// Added initial SYM_NTK support.
//
//    Rev 1.149   05 Oct 1995 09:23:24   KKIYOHAR
// Fixed inline version of NameIsDoubleDot. Changed cast from LPDWORD to LPWORD.
//
//    Rev 1.148   18 Sep 1995 16:59:14   SSAWKIN
// Missing #ifdef from last change(1.147), fixed it
//
//    Rev 1.147   18 Sep 1995 10:43:02   DLEVITON
// Back out last change.
//
//    Rev 1.146   17 Sep 1995 15:02:24   DLEVITON
// Try to get nefcfg to build for W32.
//
//    Rev 1.145   02 Aug 1995 01:42:16   AWELCH
// Merge changes from Quake 7.
//
//    Rev 1.144   01 Jun 1995 09:56:50   MARKK
// Added PackedNameIsDevice
//
//    Rev 1.143   30 May 1995 18:35:04   REESE
// Added HWIsPCI function.
//
//    Rev 1.142   27 Apr 1995 16:38:10   BRAD
// Added SystemWin95ShellPresent()
//
//    Rev 1.141   12 Apr 1995 15:22:36   MFALLEN
// MAX_INTL_STR was defined as 255. I changed this to be SYM_MAX_PATH+1 because
// NAnsiToOem inline function was breaking when a long LFN file was converted.
//
// #############################################################################
// Furthermore NAnsiToOemBuff() will break if somebody passes a large buffer to
// it.
// #############################################################################
//
//    Rev 1.140   17 Mar 1995 17:16:24   Basil
// Added conversion routines to handle double (8byte) numbers.
//
//    Rev 1.139   20 Feb 1995 19:16:08   BRUCE
// Updated comments on obsolete functions - NameIsValidFileChar()
//
//    Rev 1.138   16 Feb 1995 09:30:34   MFALLEN
// Added prototype for NameReturnIllegalChars(). Note that NameGetIllegalChars()
// should not be used anymore.
//
//    Rev 1.137   02 Feb 1995 17:57:08   BRAD
// Added SYMKRNL VxD changes from QAK6
//
//    Rev 1.136   20 Jan 1995 09:41:58   MARKL
// GetProtModePtr now supported by VxD.
//
//    Rev 1.135   28 Dec 1994 18:00:28   BRUCE
// Added ifdef around NameShortenFileNameRect() proto
//
//    Rev 1.134   28 Dec 1994 17:55:08   BRUCE
// Added NameShortenFileNameRect()
//
//    Rev 1.133   27 Dec 1994 17:10:48   BRUCE
// Added bFmtForDisp parm to NameReturnLongName()
//
//    Rev 1.132   21 Dec 1994 17:49:10   DAVID
// Added NameHasDriveSpec() to the FILE_AND_NAME_MACROS section to get it
// back in sync with the same section in file.h.
//
//    Rev 1.131   21 Dec 1994 16:10:00   BRAD
// Made long/short name functions consistent with Quake APIs.xapi.h -> S:\include\vcs\xapi.h_v
// Enter description of changes made.
//
//    Rev 1.130   21 Dec 1994 04:25:06   BRUCE
// Made NUMEROUS changes to support UNCs
//
//    Rev 1.129   09 Dec 1994 15:55:10   BRAD
// Removed SystemIsWin...() and replaced with SystemGetWindowsType()
//
//    Rev 1.128   08 Dec 1994 17:13:22   BRAD
// NameIsUNC
//
//    Rev 1.127   08 Dec 1994 14:29:46   BRAD
// Added NameAppendExtension() and StringAppendNChars
//
//    Rev 1.126   06 Dec 1994 16:41:00   BRAD
// Added NameReturnRoot
//
//    Rev 1.125   29 Nov 1994 12:35:12   BRAD
// Added StringSkipChar, StringSkipChars, and STRCPYCHR
//
//    Rev 1.124   22 Nov 1994 16:24:56   BRAD
// Added SystemIsWin* functions
//
//    Rev 1.123   21 Nov 1994 19:06:08   BRUCE
// Added NameGetLong/ShortName() functions and thunks to get to them
//
//    Rev 1.122   18 Nov 1994 12:01:26   BRAD
// GetProtModePtr() for non-Protecected and WIN32 is macro
//
//    Rev 1.121   16 Nov 1994 18:08:54   BRAD
// Added WIN32 version of GetProtModePtr()
//
//    Rev 1.120   16 Nov 1994 13:27:52   BRUCE
// Made StringGetLastChar() take an LPCSTR
//
//    Rev 1.119   02 Nov 1994 20:29:26   BRUCE
// Changed NameWildcardMatch() to take LPCSTR parms
//
//    Rev 1.118   02 Nov 1994 16:43:36   BRAD
// Added DBCS changes
//
//    Rev 1.117   18 Oct 1994 17:06:34   DALLEE
// "Uncovered" PrivateProfile macros for VXD platform, too.
//
//    Rev 1.116   18 Oct 1994 15:06:26   TONY
// Uncovered the Windows private profile APIs Peter put over OS/2 - OS/2 already has implementations for these, so the covers aren't needed.
//
//    Rev 1.115   13 Oct 1994 15:06:04   PGRAVES
// Covers of Windows private profile APIs for non-Windows platforms.
//
//    Rev 1.114   04 Oct 1994 14:39:46   BRAD
// Added HWIsNEC()
//
//    Rev 1.113   02 Sep 1994 14:01:42   JMILLARD
// add asserts to Ansi/OEM functions for non-Windows
//
//    Rev 1.112   23 Aug 1994 18:07:24   BRAD
// Added MathPower()
//
//    Rev 1.111   26 Jul 1994 11:31:24   BRAD
// Added NameWildcardMatch()
//
//    Rev 1.110   22 Jul 1994 15:02:52   BRAD
// Use Quake functions, not _fstr* functions
//
//    Rev 1.109   22 Jul 1994 14:11:58   BRAD
// Changed NAME_xxxx from 257 to 255
//
//    Rev 1.108   12 Jul 1994 21:39:38   PGRAVES
// Exported EnvironmentSubst (Win32).
//
//    Rev 1.107   12 Jul 1994 18:42:30   HENRI
// Moved thunked functions to symkrnl's thunk.h
//
//    Rev 1.106   10 Jul 1994 23:02:52   ENRIQUE
// Added NameReturnFileEx and VolumeInformationGet.
//
//    Rev 1.105   22 Jun 1994 12:27:46   HENRI
// Added thunked prototypes
//
//    Rev 1.104   05 Jun 1994 20:35:12   ENRIQUE
// Increased NAME_FILENAMESIZE to 257.  We need to rework the name functions
// to handle UNCs and LFNs.
//
//    Rev 1.103   20 May 1994 02:05:14   GVOGEL
// Added a (commented) note about the CharIs... functions relating to the
// Geologist project.
//
//    Rev 1.102   17 May 1994 14:39:46   DAVID
// Added DOS_6_22
//
//    Rev 1.101   10 May 1994 16:10:22   MEIJEN
// Add EISACMOSRead EISACMOSWrite for RESCUE future use
//
//    Rev 1.100   05 May 1994 16:01:32   BRAD
// Added Int()
//
//    Rev 1.99   04 May 1994 17:46:38   HENRI
// Add lpFAT param to DOSMungTheSFT and DOSFlushOFT stuff
//
//    Rev 1.98   04 May 1994 10:22:42   MARKK
// Activate MemoryNormalizeAddress properly under DX
//
//    Rev 1.97   04 May 1994 06:11:10   MARKK
// Added DOSGetPSP
//
//    Rev 1.96   04 May 1994 05:50:52   MARKK
// DOSX work
//
//    Rev 1.95   02 May 1994 13:23:52   BRAD
// Use defined(), rather than just the define itself
//
//    Rev 1.94   29 Apr 1994 17:06:44   MARKK
// DOSX work
//
//    Rev 1.93   29 Apr 1994 16:49:50   MARKK
// Define fewer selector calls under DOSX
//
//    Rev 1.92   29 Apr 1994 16:45:38   MARKK
// Define selector calls under DOSX
//
//    Rev 1.91   27 Apr 1994 13:25:06   BRAD
// Don't define selector routines for WIN32
//
//    Rev 1.90   26 Apr 1994 16:49:06   BRAD
// Removed PVCS Logs
 ************************************************************************/

#ifndef INCL_XDOS
#define INCL_XDOS

#ifdef __cplusplus
extern "C" {
#endif

//***********************************************************************
// UNIX Implementation
//
// The Unix version of the library is used by the NAV scanning engine,
// particularly PAM and NAVAPI.  Only the small subset of the library
// that is actually needed for NAV is implemented for Unix.
//***********************************************************************

#if defined (SYM_UNIX)

#include "file.h"

UINT SYM_EXPORT WINAPI   ConvertDateToEncoded(UINT wMonth, UINT wDay, UINT wYear);
UINT SYM_EXPORT WINAPI   ConvertTimeToEncoded(UINT wHours, UINT wMinutes, UINT wSeconds);
void SYM_EXPORT WINAPI   ConvertEncodedTimeToNormal(UINT time, UINT FAR * hour, UINT FAR * minute, UINT FAR * seconds);
void SYM_EXPORT WINAPI   ConvertEncodedDateToNormal(UINT date, UINT FAR * month, UINT FAR * day, UINT FAR * year);
VOID SYM_EXPORT WINAPI   DateGetCurrentDateTime(UINT FAR * datePtr, UINT FAR * timePtr);
LPTSTR SYM_EXPORT WINAPI _ConvertWordToString(UINT wNumber, LPTSTR lpString, UINT wRadix, TCHAR byThousandsChar);
LPTSTR SYM_EXPORT WINAPI _ConvertLongToString(DWORD dwNumber, LPTSTR lpDest, UINT  wRadix, TCHAR byThousandsChar);
LPTSTR SYM_EXPORT WINAPI StringAppendChar(LPTSTR lpString, WCHAR wChar);
LPTSTR SYM_EXPORT WINAPI StringAppendNChars(LPTSTR lpString, WCHAR wChar, int nCount);
WCHAR SYM_EXPORT WINAPI  StringGetLastChar(LPCTSTR lpszString);
LPTSTR SYM_EXPORT WINAPI StringGetEnd(LPTSTR lpszString);
UINT SYM_EXPORT WINAPI   NameReturnExtension(LPCTSTR lpszFileSpec, LPTSTR lpszExtension);
UINT SYM_EXPORT WINAPI   NameReturnExtensionEx(LPCTSTR lpszFileSpec, LPTSTR lpszExtension, UINT cbExtBuffSize);
LPTSTR SYM_EXPORT WINAPI NameStripExtension(LPTSTR lpName);
VOID SYM_EXPORT WINAPI   NameAppendExtension(LPTSTR lpPathname, LPCTSTR lpszExtension);
UINT SYM_EXPORT WINAPI   NameReturnFile (LPCTSTR lpszFileSpec, LPTSTR lpszFileName);
LPTSTR SYM_EXPORT WINAPI NameStripFile(LPTSTR lpName);
VOID SYM_EXPORT WINAPI   NameAppendFile(LPTSTR lpPathname, LPCTSTR lpFilename);
VOID                     UnixTimeToFileTime(time_t* pt, LPFILETIME pft);
VOID                     FileTimeToUnixTime(LPFILETIME pft, time_t* pt);

// Ignore the rest of xapi.h

#else  // !defined(SYM_UNIX)

//-----------------------------------------------------------------------
// NameWildcardMatch() match types
//-----------------------------------------------------------------------
#define WILDCARDMATCH_QUERY     0
#define WILDCARDMATCH_SHORT     1
#define WILDCARDMATCH_LONG      2

#ifndef SYM_WIN                           // DOS-only things
#ifndef SYM_NTK                           // NTK is not DOS
                                          // MSVC 4.1 doesn't like
                                          //          _near _cdecl data
// Speedup things just a bit by taking advantage of startup code
// --------------------------------------------------------------

#ifdef  _MSC_VER                        // Microsoft C
        extern unsigned char _near _cdecl _osmajor;
        extern unsigned char _near _cdecl _osminor;
UINT SYM_EXPORT WINAPI     DOSGetVersion(VOID); // This used to be a macro on DOS platform
#endif

#ifdef  _BORLANDC_                      // Borland
  extern unsigned _Cdecl _version;
  #define DOSGetVersion()         (_version)
#endif

#ifdef __ZTC__                          // Zortech
  extern unsigned char _cdecl _osmajor;
  extern unsigned char _cdecl _osminor;
  #define DOSGetVersion()         (_osminor | (((UINT)_osmajor) << 8))
#endif

#endif   // #ifndef SYM_NTK

// Eliminate these functions so code doesn't get too convoluted
// --------------------------------------------------------------
#define CriticalSectionBegin()          (void)(0)
#define CriticalSectionEnd()            (void)(0)

#endif

/*----------------------------------------------------------------------*/
// Different types of Windows.
/*----------------------------------------------------------------------*/
#define SYM_SYSTEM_WIN_95           1
#define SYM_SYSTEM_WIN_NT           2
#define SYM_SYSTEM_WIN_32S          3
#define SYM_SYSTEM_WIN_31           4

/*----------------------------------------------------------------------*/
/* Different versions of Windows, according to the low-order word of    */
/* the Windows GetVersion() function.                                   */
/*----------------------------------------------------------------------*/
#define         WIN_3_00                (0x0003)
#define         WIN_3_10                (0x0A03)

/*----------------------------------------------------------------------*/
/* Different versions of DOS                                            */
/* according to the DOSGetVersion() function.                           */
/*----------------------------------------------------------------------*/
#define         DOS_3_00                (0x0300)
#define         DOS_3_10                (0x030A)
#define         DOS_3_20                (0x0314)
#define         DOS_3_21                (0x0315)
#define         DOS_3_30                (0x031E)
#define         DOS_3_31                (0x031F)
#define         DOS_4_00                (0x0400)
#define         DOS_5_00                (0x0500)
#define         DOS_6_00                (0x0600)
#define         DOS_6_22                (0x0616)
#define     DOS_7_00        (0x0700)    // Windows 95.
#define     DOS_7_10        (0x070A)    // Windows 96 (Nashville).
#define         DOS_OS2_1_00    (0x0A00)
#define         DOS_OS2_2_00    (0x1400)
#define         DOS_OS2_2_10    (0x140A)

// Flags in dosVersionFlags
#define         DOS_IN_ROM              0x08
#define         DOS_IN_HMA              0x10

/*----------------------------------------------------------------------*/
/* DOS Extended Error Codes.                                            */
/*      These error codes are returned by DOSGetExtendedError()         */
/*      after a DOS error has occurred.                                 */
/*                                                                      */
/*      These are not all of the values, just the ones we need for now. */
/*      Please proceed all of them with 'DOS_ERROR'                     */
/*----------------------------------------------------------------------*/
#define DOS_ERROR_NO_ERROR                      0
#define DOS_ERROR_BAD_FUNCTION_NUMBER           1
#define DOS_ERROR_FILE_NOT_FOUND                2
#define DOS_ERROR_PATH_NOT_FOUND                3
#define DOS_ERROR_TOO_MANY_OPEN_FILES           4
#define DOS_ERROR_ACCESS_DENIED                 5
#define DOS_ERROR_INVALID_FILE_HANDLE           6
#define DOS_ERROR_MEM_BLOCKS_DESTROYED          7
#define DOS_ERROR_INSUFFICIENT_MEMORY           8
#define DOS_ERROR_BAD_MEMORY_BLOCK_ADDR         9
#define DOS_ERROR_BAD_ENVIRONMENT               10
#define DOS_ERROR_BAD_FORMAT                    11
#define DOS_ERROR_BAD_ACCESS_CODE               12
#define DOS_ERROR_BAD_DATA                      13
#define DOS_ERROR_BAD_DRIVE_SPECIFIED           15
#define DOS_ERROR_CANT_DEL_CURRENT_DIR          16
#define DOS_ERROR_NOT_SAVE_DEVICE               17
#define NO_MORE_FILES                           18
#define DOS_ERROR_NO_MORE_FILES                 NO_MORE_FILES
#define DOS_ERROR_WRITE_PROTECTED               19
#define DOS_ERROR_DRIVE_NOT_READY               21
#define DOS_ERROR_CRC                           23
#define DOS_ERROR_SECTOR_NOT_FOUND              27
#define DOS_ERROR_WRITE_FAULT                   29
#define DOS_ERROR_READ_FAULT                    30
#define DOS_ERROR_GENERAL_FAILURE               31
#define DOS_ERROR_SHARING_VIOLATION             32
#define DOS_ERROR_LOCK_VIOLATION                33
#define DOS_ERROR_FLOPPY_NOT_INSERTED           83

/*----------------------------------------------------------------------*
 * Symantec Library Critical Error codes                                *
 *                                                                      *
 * The global variable "error" will contain a value of 256 or greater   *
 * for critical errors.                                                 *
 *----------------------------------------------------------------------*/

#define ErrorClear()                    (error = 0)
#define ErrorIsCritical()               (error >= 256)

#define CRIT_ERROR_WRITE_PROTECTED      0x100
#define CRIT_ERROR_UNKNOWN_DRIVE        0x101
#define CRIT_ERROR_DRIVE_NOT_READY      0x102
#define CRIT_ERROR_UNKNOWN_COMMAND      0x103
#define CRIT_ERROR_DATA_ERROR           0x104
#define CRIT_ERROR_BAD_REQUEST_LEN      0x105
#define CRIT_ERROR_SEEK_ERROR           0x106
#define CRIT_ERROR_NON_DOS_DISK         0x107
#define CRIT_ERROR_SECTOR_NOT_FOUND     0x108
#define CRIT_ERROR_OUT_OF_PAPER         0x109
#define CRIT_ERROR_WRITE_FAULT          0x10A
#define CRIT_ERROR_READ_FAULT           0x10B
#define CRIT_ERROR_GENERAL_FAILURE      0x10C


#ifndef  BREAK_OFF
#define  BREAK_OFF      0
#define  BREAK_ON       1
#endif

/*----------------------------------------------------------------------*/
/* Error states returned by thunked functions                           */
/*----------------------------------------------------------------------*/
#define THUNK_NO_ERROR                  0x00000000
#define THUNK_ERROR_INVALID_FUNC        0x00000001

/*----------------------------------------------------------------------*/
/* Error states returned by Lock and Unlock of DOS memory               */
/*----------------------------------------------------------------------*/
#define ALLOC_SELECTOR_FAILED       2001
#define SET_SELECTOR_LIMIT_FAILED   2002
#define SET_SELECTOR_BASE_FAILED    2003
#define FREE_SELECTOR_FAILED        2004

/*----------------------------------------------------------------------*
 * Program Segment Prefix Record                                        *
 *----------------------------------------------------------------------*/
#define PSP_START_BYTES         0x20CD

typedef struct PSPRecord
    {
    WORD        exitCall;                       /* 0xCD, 0x20                   */
    WORD        blockLength;
    BYTE        reserve1;
    BYTE        cpmCall[5];                     /* 0x9A, ...                    */
    LPVOID      prevExitRoutine;
    LPVOID      prevCtrlCRoutine;
    LPVOID      prevFatalError;
    WORD        parentID;                       /* (undocumented)               */
    BYTE        handleTable[20];
    WORD        environmentSegPtr;
    LPVOID      userStatck;
    WORD        handleCount;
    LPVOID      handleAddress;
    BYTE        reserve2[24];
    BYTE        dosCall[3];                     /* 0xCD, 0x21, 0xCB             */
    BYTE        reserve3[9];
    BYTE        defaultFCB[16];
    BYTE        secondFCB[16];
    BYTE        reserve4[4];
    BYTE        commandLineLength;
    BYTE        commandLine[127];
    } PSPRecord;


/*----------------------------------------------------------------------*
 * DOS Memory Control Block Record definition                           *
 *----------------------------------------------------------------------*/
typedef struct MemoryBlockRecord
    {
    BYTE id;                            /* 'Z' for last block, else 'M' */
    WORD pspSeg;                        /* Segment pointer to P.S.P.    */
    WORD size;                          /* Size of this block           */
    BYTE reserved[3];                   /* Unused                       */
    BYTE owner[8];                      /* DOS 4+: Owner name           */
    } MemoryBlockRecord;


/*----------------------------------------------------------------------*
 * Device Driver Header Structure                                       *
 *----------------------------------------------------------------------*/
typedef struct DeviceDriverRec
    {
    struct DeviceDriverRec FAR *next;           /* Next device driver in chain  */
    WORD attribute;                     /* Attribute of driver          */
    WORD strategyRoutine;               /* Driver strategy routine      */
    WORD interruptRoutine;              /* Driver interrupt routine     */
    BYTE name[8];                       /* Driver name                  */
    } DeviceDriverRec;


/*----------------------------------------------------------------------*
 * "nType" parameter to DOSGetOpenFileName() and DOSGetOpenFileInfo()   *
 *----------------------------------------------------------------------*/

#define PROCESS_FILE_HANDLE     1
#define SYSTEM_FILE_HANDLE      2

/*------------------------------------------------------------------------*
 * Information on an open file, in a "DOS version neutral" format         *
 *                                                                        *
 * This structure is filled by:                                           *
 *                                                                        *
 *      DOSGetOpenFileInfo()                                              *
 *      DOSGetInfoOnAllOpenFiles()                                        *
 *                                                                        *
 * This information is gathered by Symantec Library by inspecting the SFT *
 * entry for a file.  Not all versions of DOS supply all information.     *
 * Additional information is kept in the SFT, but is not included here    *
 * because it seemed highly unlikely we would ever care about it.         *
 *------------------------------------------------------------------------*/

typedef struct OpenFileInfoRec
    {
    WORD        wNumHandles;            // Number of handles referring to this file
    BYTE        bCharDevice;            // TRUE if character device
    BYTE        bNetworkFile;           // TRUE if network file
    BYTE        bModified;              // TRUE if data written to file
    BYTE        byDriveLetter;          // Drive letter
    BYTE        szName[13];             // Filename in NAME.EXT format
    WORD        wOpenMode;              // Mode from AL register on open call
    BYTE        byAttr;                 // File attribute
    WORD        wDate;                  // File date in packed format
    WORD        wTime;                  // File time in packed format
    DWORD       dwSize;                 // File size
    DWORD       dwPosition;             // Current file position
    WORD        wStartCluster;          // First data cluster
    DWORD       dwDirSector;            // Sector containing directory entry
    BYTE        byDirEntryNumber;       // Directory entry within sector
    WORD        wOwnerPSP;              // PSP that owns file
    void        far *lpDriver;          // Character device driver or DPB
    WORD        wSFTEntry;              // This records position in the SFT
    void        far *lpSFT;             // A REAL mode pointer to the SFT
                                        // that this corresponds to.
    } OpenFileInfoRec;

/*----------------------------------------------------------------------*
 * DOS internal data structures                                         *
 *----------------------------------------------------------------------*/

typedef struct SFTHeaderRec
    {
    struct SFTHeaderRec far     *lpNext;        // Next SFT block
    WORD                        wFileCount;     // Number of files in this block
    BYTE                        SFT[1];         // First SFT entry
    } SFTHeaderRec, *PSFTHeaderRec, FAR *LPSFTHeaderRec;

typedef struct Dos2SFT
    {
    BYTE        byNumHandles;           // Number of handles referring to this file
    BYTE        byOpenMode;             // Mode from AL register on open call
    BYTE        byAttr;                 // File attribute
    BYTE        byDrive;                // 0 = Device, 1 = A:, 2 = B:, etc.
    BYTE        sName[11];              // Filename in FCB format
    WORD        wUnknown1;
    WORD        wUnknown2;
    DWORD       dwSize;                 // File size
    WORD        wDate;                  // File date in packed format
    WORD        wTime;                  // File time in packed format
    BYTE        byDevAttr;              // Device attribute
    union
        {
        struct
            {
            void far *lpDriver;         // Character device driver
            } Char;
        struct
            {
            WORD wStartCluster;         // First data cluster
            WORD wLastAccessed;         // Relative cluster of last access
            } Block;
        } CB;
    WORD        wCurrentCluster;        // Current cluster number
    WORD        wUnknown3;
    DWORD       dwPosition;             // Current file position
    } Dos2SFT;

typedef struct Dos30SFT
    {
    WORD        wNumHandles;            // Number of handles referring to this file
    WORD        wOpenMode;              // Mode from AL register on open call
    BYTE        byAttr;                 // File attribute
    WORD        wDeviceInfo;            // Device information word
    void        far *lpDriver;          // Character device driver or DPB
    WORD        wStartCluster;          // First data cluster
    WORD        wTime;                  // File time in packed format
    WORD        wDate;                  // File date in packed format
    DWORD       dwSize;                 // File size
    DWORD       dwPosition;             // Current file position
    WORD        wLastAccessed;          // Relative cluster of last access
    WORD        wCurrentCluster;        // Current cluster number
    WORD        wDirSector;             // Sector containing directory entry
    WORD        wDirEntryOffset;        // Directory entry within sector
    BYTE        sName[11];              // Filename in FCB format
    DWORD       dwShare1;
    WORD        wShare2;
    WORD        wOwnerPSP;              // PSP that owns file
    WORD        wShare3;
    } Dos30SFT;

typedef struct Dos31SFT
    {
    WORD        wNumHandles;            // Number of handles referring to this file
    WORD        wOpenMode;              // Mode from AL register on open call
    BYTE        byAttr;                 // File attribute
    WORD        wDeviceInfo;            // Device information word
    void        far *lpDriver;          // Character device driver or DPB
    WORD        wStartCluster;          // First data cluster
    WORD        wTime;                  // File time in packed format
    WORD        wDate;                  // File date in packed format
    DWORD       dwSize;                 // File size
    DWORD       dwPosition;             // Current file position
    WORD        wLastAccessed;          // Relative cluster of last access
    WORD        wCurrentCluster;        // Current cluster number
    WORD        wDirSector;             // Sector containing directory entry
    BYTE        byDirEntryNumber;       // Directory entry within sector
    BYTE        sName[11];              // Filename in FCB format
    DWORD       dwShare1;
    WORD        wShare2;
    WORD        wOwnerPSP;              // PSP that owns file
    WORD        wShare3;
    } Dos31SFT;

typedef struct Dos4SFT
    {
    WORD        wNumHandles;            // Number of handles referring to this file
    WORD        wOpenMode;              // Mode from AL register on open call
    BYTE        byAttr;                 // File attribute
    WORD        wDeviceInfo;            // Device information word
    void        far *lpDriver;          // Character device driver or DPB
    WORD        wStartCluster;          // First data cluster
    WORD        wTime;                  // File time in packed format
    WORD        wDate;                  // File date in packed format
    DWORD       dwSize;                 // File size
    DWORD       dwPosition;             // Current file position
    WORD        wLastAccessed;          // Relative cluster of last access
    DWORD       dwDirSector;            // Sector containing directory entry
    BYTE        byDirEntryNumber;       // Directory entry within sector
    BYTE        sName[11];              // Filename in FCB format
    DWORD       dwShare1;
    WORD        wShare2;
    WORD        wOwnerPSP;              // PSP that owns file
    WORD        wShare3;
    WORD        wShare4;
    DWORD       dwIFS1;
    } Dos4SFT;

typedef struct VFAT311OFT       // VFAT WFW 3.11 Open File Table
    {
    DWORD   dwOFTNext;          // next OFT structure
    BYTE    abyUnknown1[0x26];  // ???
    BYTE    byOpen;             // Open mode (non-zero means write????)
    BYTE    abyUnknown2[0x09];  // ???
    DWORD   dwVolTable;         // 32 bit pointer to volume table
    WORD    wStartCluster;      // SCN
    BYTE    abyUnknown3[0x08];  // ???
    BYTE        byDirOffset;            // NOTE: Unknown info in high nibble
    BYTE    sName[11];          // 8.3 name of file
    DWORD   dwDirSector;        // sector number for directory
    WORD    wUnknown4;          // ???
    WORD    wUnknown5;          // ???
    WORD    wCacheCnt;          // number of clusters cached
    } VFAT311OFT, far *LPVFAT311OFT;

typedef struct VFAT311VT        // VFAT WFW 3.11 Volume Table
    {
    BYTE    abyUnknown1[0x0D];  // ???
    BYTE    byDrvID;            // Drive number/id (A=0)
    } VFAT311VT, far *LPVFAT311VT;

typedef union SFTEntryRec
    {
#ifdef  __cplusplus
    SFTEntryRec();                      // fake constructor, for some
#endif                                  // silly reason.  why? - EJC
    const Dos2SFT       Dos2;
    const Dos30SFT      Dos30;
    const Dos31SFT      Dos31;
    const Dos4SFT       Dos4;
    } SFTEntryRec, *PSFTEntryRec, FAR *LPSFTEntryRec;

#define JFT_PTR_WITHIN_PSP      (0x34)
#define UNUSED_JFT_ENTRY        ((BYTE) 0xFF)

WORD SYM_EXPORT PASCAL  DOSGetProcessFileHandleCount(WORD wPSP);
WORD SYM_EXPORT PASCAL  DOSGetSystemFileHandleCount(void);
WORD SYM_EXPORT PASCAL  DOSGetOpenFileCount(BYTE byDriveLetter, BYTE bOnlyIfWritten);
BYTE SYM_EXPORT PASCAL  DOSGetOpenFileName(int nType, WORD wHandle, BYTE *pszName, WORD wPSP);
BYTE SYM_EXPORT PASCAL  DOSIsFileOpen(BYTE byDriveLetter, WORD wStartCluster);
WORD SYM_EXPORT PASCAL  DOSGetSystemFileHandle(WORD wProcessHandle, WORD wPSP);
BYTE SYM_EXPORT PASCAL  DOSGetOpenFileInfo(int nType, WORD wHandle, struct OpenFileInfoRec far *lpInfo, WORD wPSP);
WORD SYM_EXPORT PASCAL  DOSGetInfoOnAllOpenFiles(WORD wMaxFiles, struct OpenFileInfoRec far *lpInfo);
WORD SYM_EXPORT PASCAL  DOSGetOpenFilesOnDrive(BYTE byDrive, LPHANDLE lpHandle);
WORD SYM_EXPORT WINAPI  DOSGetPSP(VOID);

LPSFTEntryRec SYM_EXPORT PASCAL DOSGetSystemFileTableEntry(WORD wHandle);
void SYM_EXPORT WINAPI DOSChangeSFTCluster(WORD wOld, WORD wNew, OpenFileInfoRec far *lpOpenFileRec);
void SYM_EXPORT WINAPI DOSMungTheSFT(OpenFileInfoRec far *lpInfo, WORD wNOpenFiles, struct DISKREC far *lpDisk, struct DIRHEADERREC huge *lpDirHeader, struct FAT far *lpFAT);
VOID SYM_EXPORT PASCAL DOSFlushAllWriteFiles(struct DISKREC far *lpDisk,  struct FAT far *lpFAT );
LPSFTHeaderRec SYM_EXPORT PASCAL DOSGetFirstSFT(void);
void SYM_EXPORT PASCAL DOSConvertSFTToOurFormat(SFTEntryRec far *lpSFTEntry, OpenFileInfoRec far *lpInfo);
UINT SYM_EXPORT PASCAL DOSSFTEntrySize(void);



/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
typedef struct DOS_20_VarsRec
    {
    WORD        wMCB;
    LPBYTE      lpDPB;
    LPBYTE      lpSFT;
    LPBYTE      lpClockDevice;
    LPBYTE      lpConDevice;
    BYTE        byLogicalDrivesSupported;
    WORD        wLargestSectorSizeSupported;
    LPBYTE      lpDosBuffers;
    } DOS_20_VarsRec;


/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
typedef struct DOS_30_VarsRec
    {
    WORD        wMCB;
    LPBYTE      lpDPB;
    LPBYTE      lpSFT;
    LPBYTE      lpClockDevice;
    LPBYTE      lpConDevice;
    WORD        wLargestSectorSizeSupported;
    LPBYTE      lpDosBuffers;
    LPBYTE      lpDrivePathTable;
    LPBYTE      lpFCBTable;
    WORD        wFcbTableSize;
    BYTE        byLogicalDrivesSupported;
    BYTE        byLastDrive;
    } DOS_30_VarsRec, FAR *LPDOS3CVT;


/*----------------------------------------------------------------------*/
/* System File Table Entry                                              */
/*----------------------------------------------------------------------*/
// The SFTENTRY and SFT types are obsolete.  You should use the
// functions that work with OpenFileInfoRec structures instead.

#define SIZE_OF_SFT_ENTRY       (59)

typedef struct SFTENTRY
    {
    WORD        wNumHandles;
    WORD        wOpenMode;
    BYTE        byFileAttr;
    WORD        wDeviceInfo;
    LPBYTE      lpPtr;
    WORD        wStartingCluster;
    WORD        wTime;
    WORD        wDate;
    DWORD       dwSize;
    DWORD       dwOffset;
    WORD        wRelativeCluster;
    WORD        wAbsCluster;
    WORD        wDirSector;
    BYTE        byDirEntry;
    BYTE        byFileName[11];
    LPBYTE      lpPrevSFT;
    WORD        wNetworkMachine;
    WORD        wOwnerPSP;
    // ...
    } SFTENTRY, FAR *LPSFTENTRY;        // for DOS 3.0+

/*----------------------------------------------------------------------*/
/* System File Table                                                    */
/*----------------------------------------------------------------------*/
typedef struct SFT
    {
    struct SFT FAR *lpNextSFT;
    WORD        wNumFiles;
    SFTENTRY    SFTFirstEntry;
    } SFT, FAR *LPSFT;


/*----------------------------------------------------------------------*/
/* DPMIRec                                                              */
/* Used to perform simulated software interrupts thru DPMI.             */
/* NOTE: Make sure you memset this structure to 0, before               */
/* setting the new values.                                              */
/*----------------------------------------------------------------------*/

typedef struct DPMIRec
    {
    DWORD       edi;
    DWORD       esi;
    DWORD       ebp;
    DWORD       reserved;
    DWORD       ebx;
    DWORD       edx;
    DWORD       ecx;
    DWORD       eax;
    WORD        cflag;
    WORD        es;
    WORD        ds;
    WORD        fs;
    WORD        gs;
    WORD        ip;
    WORD        cs;
    WORD        sp;
    WORD        ss;
    BYTE        simulatedInt;
    } DPMIRec;

                                        // We have a WIN32 version of this.
#if defined(SYM_PROTMODE) || defined(SYM_WIN) || defined(SYM_VXD)
LPVOID SYM_EXPORT PASCAL   GetProtModePtr(LPVOID lpRealMode);
#else
#define GetProtModePtr(lpReal)  (lpReal)
#endif

#if defined(SYM_PROTMODE)

#ifdef  SYM_WIN
VOID SYM_EXPORT WINAPI     CriticalSectionBegin(VOID);
VOID SYM_EXPORT WINAPI     CriticalSectionEnd(VOID);
#endif
#define Int(x,y,z)      IntWin(x,y,z)
UINT SYM_EXPORT WINAPI     IntWin(BYTE intNum, union REGS FAR *regs, struct SREGS FAR *segregs);
UINT SYM_EXPORT WINAPI     RealModeFarCall(LPVOID lpCallAddress, union REGS FAR *regs, struct SREGS FAR *segregs);
BOOL SYM_EXPORT WINAPI     DPMISimulateInt(DPMIRec FAR *DPMIRecPtr);
BOOL SYM_EXPORT WINAPI     DPMISimulateFarCall(DPMIRec FAR *DPMIRecPtr);
UINT SYM_EXPORT WINAPI     DPMISegmentToDescriptor(UINT wSegment);

UINT SYM_EXPORT WINAPI     SelectorAlloc(UINT wSegment, UINT wLimit, UINT FAR *lpwSelector);
UINT SYM_EXPORT WINAPI     SelectorFree(UINT wSelector);
UINT SYM_EXPORT WINAPI     SelectorSetDosAddress(UINT FAR *lpwSelector, LPSTR lpRealAddress);
UINT SYM_EXPORT WINAPI     SelectorGetDosAddress(UINT wSelector, LPSTR FAR *lpRealAddress);

#else

UINT SYM_EXPORT WINAPI     Int(BYTE intNum, union REGS FAR *regs, struct SREGS FAR *segregs);
#define SelectorFree(wSeg)

#endif

/************************************************************************
 * PROTOTYPES                                                           *
 ************************************************************************/

#ifdef SYM_WIN
UINT SYM_EXPORT WINAPI     DOSGetVersion(VOID); // This is a macro on DOS platform
#endif

BOOL SYM_EXPORT WINAPI     DOSGetInternalVariables(BYTE FAR * FAR *buffer);

#ifdef SYM_WIN32
#ifdef __cplusplus
inline UINT DOSGetEnvironment(LPCTSTR lpszVarName, LPTSTR lpszValue, UINT wSize)
    { return GetEnvironmentVariable(lpszVarName, lpszValue, wSize) ? NOERR : ERR; }
#else
#define DOSGetEnvironment(lpszVarName, lpszValue, wSize) \
    (GetEnvironmentVariable(lpszVarName, lpszValue, wSize) ? NOERR : ERR)
#endif
#else                                   // not SYM_WIN32
UINT SYM_EXPORT WINAPI     DOSGetEnvironment (LPCSTR lpszVarName, LPSTR lpszValue, UINT wSize);
#endif                                  // not SYM_WIN32

#if defined(SYM_WIN32)
DWORD SYM_EXPORT WINAPI EnvironmentSubst( LPSTR, UINT );
#else
#define EnvironmentSubst(s,c)   DoEnvironmentSubst(s,c)
DWORD WINAPI DoEnvironmentSubst(LPSTR szString, UINT cbString);
#endif

#if defined(SYM_WIN16) || defined(SYM_DOS)
UINT SYM_EXPORT WINAPI     DOSGetPCMOSVersion(VOID);
UINT SYM_EXPORT WINAPI     DOSGetDRDOSVersion(VOID);
#else
#define DOSGetPCMOSVersion()    (0)
#define DOSGetDRDOSVersion()    (0)
#endif

#ifdef SYM_WIN32
//***********************************************************************
// Generic VxD Services provided by SymKrnl.VxD
//***********************************************************************
typedef struct
    {
    DWORD EAX;
    DWORD EBX;
    DWORD ECX;
    DWORD EDX;
    DWORD ESI;
    DWORD EDI;
    DWORD EBP;
    WORD  DS;
    WORD  ES;
    WORD  FS;
    WORD  GS;
    DWORD EFlags;
    } SYM_REGS, FAR *LPSYM_REGS;

BOOL SYM_EXPORT WINAPI VxDServicesInit();
BOOL SYM_EXPORT WINAPI VxDStkBasedCall( DWORD dwVxDService, LPDWORD lpdwResult, DWORD dwNumParams, ... );
BOOL SYM_EXPORT WINAPI VxDRegBasedCall( DWORD dwVxDService, LPSYM_REGS lpRegs );
BOOL SYM_EXPORT WINAPI VxDIntBasedCall( int Interrupt, LPSYM_REGS lpRegs );
void SYM_EXPORT WINAPI SymkrnlVxDSetName( LPCSTR szVxDName );
LPCSTR SYM_EXPORT WINAPI SymkrnlVxDGetName( void );
#endif

UINT SYM_EXPORT WINAPI     DOSGetEnvironmentCase (LPCSTR lpszVarName, LPSTR lpszValue, UINT wSize);
BYTE SYM_EXPORT WINAPI     DOSGetDate (UINT FAR * monthPtr, UINT FAR * dayPtr, UINT FAR * yearPtr);
VOID SYM_EXPORT WINAPI     DOSGetTime (UINT FAR * hourPtr, UINT FAR * minPtr, UINT FAR * secPtr, UINT FAR * hundPtr);
BYTE SYM_EXPORT WINAPI     DOSOEMNumber(VOID);
VOID SYM_EXPORT WINAPI     DOSSetBreakStatus(BYTE flag);
BYTE SYM_EXPORT WINAPI     DOSGetBreakStatus(VOID);
BOOL SYM_EXPORT WINAPI     DOSGetIntHandler(BYTE intNum, BYTE FAR * FAR *intHandlerPtr);
BOOL SYM_EXPORT WINAPI     DOSSetIntHandler (BYTE intNum, const LPBYTE intHandler);
VOID SYM_EXPORT WINAPI     DOSGetErrorInfo(UINT FAR * wExtendedError, LPBYTE byErrorClass,
                                LPBYTE bySuggestedAction, LPBYTE byLocus);
UINT SYM_EXPORT WINAPI     DOSGetExtendedError(VOID);
void SYM_EXPORT WINAPI     DOSGetProgramName (LPSTR lpName);

void SYM_EXPORT WINAPI     NDosSetError(UINT wError);
UINT SYM_EXPORT WINAPI     NDosGetError(void);

typedef int (WINAPI *COMPPROC)(void huge*, void huge*, UINT, DWORD);
typedef void (WINAPI *SWAPPROC)(void huge*, void huge*);

UINT SYM_EXPORT WINAPI     ConvertTimeToEncoded(UINT wHours,UINT wMinutes,UINT wSeconds);
void SYM_EXPORT WINAPI     ConvertEncodedTimeToNormal(UINT time, UINT FAR * hour, UINT FAR * minute, UINT FAR * seconds);
VOID SYM_EXPORT WINAPI     DateGetCurrentDateTime (UINT FAR * datePtr, UINT FAR * timePtr);
UINT SYM_EXPORT WINAPI     ConvertDateToEncoded(UINT wMonth, UINT wDay, UINT wYear);
void SYM_EXPORT WINAPI     ConvertEncodedDateToNormal(UINT date, UINT FAR * month, UINT FAR * day, UINT FAR * year);
UINT SYM_EXPORT WINAPI     DiskCache (UINT wCommand);
DWORD SYM_EXPORT WINAPI    DiskGetCacheSize(UINT FAR    *pwBlockSize,
                                UINT FAR    *pwCurrentBlocks,
                                UINT FAR    *pwDosBlocks,
                                UINT FAR    *pwWinBlocks);
BOOL SYM_EXPORT WINAPI     DiskGetCacheHits(DWORD far *lpdwHits, DWORD far *lpdwMisses);
UINT SYM_EXPORT WINAPI     DiskEnableCache(void);
UINT SYM_EXPORT WINAPI     DiskDisableCache(void);
UINT SYM_EXPORT WINAPI     DiskCacheInstalled(void);
UINT SYM_EXPORT WINAPI     DiskCacheSuspend(void);
UINT SYM_EXPORT WINAPI     DiskCacheResume(void);
UINT SYM_EXPORT WINAPI     DiskCacheDumpWrites(void);
VOID SYM_EXPORT WINAPI     DiskCacheSetWriteThru(BYTE byDrive);
VOID SYM_EXPORT WINAPI     DiskCacheResetWriteThru(BYTE byDrive);
LPVOID SYM_EXPORT WINAPI   DiskGetUnderSmartDrive(BYTE dl);
LPVOID SYM_EXPORT WINAPI   DiskGetUnderSymEvent(BYTE dl);
BYTE SYM_EXPORT WINAPI     CMOSRead(BYTE address);
VOID SYM_EXPORT WINAPI     CMOSWrite(BYTE address, BYTE CMOSdata);
BOOL SYM_EXPORT WINAPI     HWIsNEC(VOID);
BOOL                       _HWIsNEC(void); /* DOSTOOL getequip.c */
BOOL SYM_EXPORT WINAPI     HWIsPCI(VOID);
BOOL SYM_EXPORT WINAPI     HWIsEISA(VOID);
BOOL SYM_EXPORT WINAPI     HWIsMicroChannel(VOID);
UINT SYM_EXPORT WINAPI     DiskMonitor (UINT wCommand);
UINT SYM_EXPORT WINAPI     DiskEraseProtect (UINT wCommand);
BOOL SYM_EXPORT WINAPI     DiskIsVFATInstalled(void);
BOOL SYM_EXPORT WINAPI     MemorySearch(LPVOID lpBuffer, UINT wBufferLen, LPVOID lpStr, UINT wStrLen);
BYTE SYM_EXPORT WINAPI     EISACMOSRead(BYTE     slot,
                                        BYTE     funct,
                                        LPBYTE   address);
BYTE SYM_EXPORT WINAPI     EISACMOSWrite(WORD     length,
                                         LPBYTE   address);

// This function is a cover to the WIN32 API call GetVolumeInformation
BOOL SYM_EXPORT WINAPI VolumeInformationGet(
    LPCTSTR lpRootPathName,
    LPTSTR lpVolumeNameBuffer,
    DWORD nVolumeNameSize,
    LPDWORD lpVolumeSerialNumber,
    LPDWORD lpMaximumComponentLength,
    LPDWORD lpFileSystemFlags,
    LPTSTR lpFileSystemNameBuffer,
    DWORD nFileSystemNameSize
    );


// Low nibble specified src.  High nibble is dest
// Set to 0, if you want src to be physical, dest to be virtual
#define MCP_SRC_PHYSICAL        0x01
#define MCP_SRC_VIRTUAL         0x02
#define MCP_DEST_PHYSICAL       0x04
#define MCP_DEST_VIRTUAL        0x08
BOOL SYM_EXPORT WINAPI     MemCopyPhysical(LPVOID lpDest, LPVOID lpSrc, WORD wSize, UINT uFlags);

#ifdef SYM_WIN32
#define MemoryHugeMove(dest, src, count) \
    memmove(dest, src, count)
#define MemoryHugeCopy(dest, src, dwBytes) \
    memcpy(dest, src, dwBytes)
#define MemoryHugeCompare(p1, p2, c) \
    memcmp(p1, p2, c)
#else
VOID SYM_EXPORT WINAPI     MemoryHugeMove (void huge *dest, void huge *src, DWORD count);
                                        // Copy huge data
void huge * SYM_EXPORT WINAPI MemoryHugeCopy(
    void huge *dest,                    // HPVOID
    const void huge *src,               // HPCVOID
    DWORD dwBytes);
                                        // Compare huge data
int SYM_EXPORT WINAPI MemoryHugeCompare(HPBYTE hpData1, HPBYTE hpData2, DWORD dwBytes);
#endif

#if defined(SYM_32BIT)
#define MemoryNormalizeAddress(address) (address)
#else                                   // not SYM_WIN32
// On the DOS platform, this function normalizes a segment:offset.
// On Windows, it allocates and returns a new selector because we can't
// do segment arithmatic on huge pointers.
LPVOID SYM_EXPORT WINAPI MemoryNormalizeAddress(LPVOID address);
#endif                                  // not SYM_WIN32

#ifdef SYM_PROTMODE
//                                                  FP_SEG(address)
#define FreeNormalizedAddress(address) FreeSelector(HIWORD(address))
#else
#define FreeNormalizedAddress(x)
#endif

void SYM_EXPORT WINAPI     QuickSort (BYTE huge * hpBase, UINT wNumItems, UINT wSize,
                                COMPPROC fnCompare, UINT wParam, DWORD lParam,
                                SWAPPROC fnSwap, UINT wSwapCost);
DWORD SYM_EXPORT WINAPI    MathPower(UINT uX, UINT uY);
BOOL  SYM_EXPORT WINAPI    MathIsPower2 (UINT num);
UINT  SYM_EXPORT WINAPI    MathDiff (UINT number1, UINT number2);
DWORD SYM_EXPORT WINAPI    MathLongDiff (DWORD number1, DWORD number2);

VOID   SYM_EXPORT WINAPI   StringCompress(LPSTR compressedStr, LPSTR sourceStr);
VOID   SYM_EXPORT WINAPI   StringUnCompress (LPSTR destStr, LPSTR compressedStr);
UINT   SYM_EXPORT WINAPI   StringMaxLength(LPTSTR *s);
LPTSTR SYM_EXPORT WINAPI   StringSkipChar(LPTSTR lpString, TCHAR cChar);
LPTSTR SYM_EXPORT WINAPI   StringSkipChars(LPTSTR lpString, LPTSTR lpszCharsStart);
VOID   SYM_EXPORT WINAPI   STRCPYCHR(LPTSTR FAR *lplpszDest, LPTSTR FAR *lplpszSrc, TCHAR cChar);
LPTSTR SYM_EXPORT WINAPI   StringSkipWhite(LPTSTR lpString);
LPTSTR SYM_EXPORT WINAPI   StringSkipToWhite(LPTSTR lpString);
LPTSTR SYM_EXPORT WINAPI   StringSkipIniDelim(LPTSTR lpString);
LPTSTR SYM_EXPORT WINAPI   StringSkipToIniDelim(LPTSTR lpString);
LPTSTR SYM_EXPORT WINAPI   StringRemoveTrailingSpaces(LPTSTR lpString);
LPTSTR SYM_EXPORT WINAPI   StringGetEnd(LPTSTR lpszString);
LPSTR  SYM_EXPORT WINAPI   StringReverse(LPSTR lpszString);
LPTSTR SYM_EXPORT WINAPI   StringSeparate(LPTSTR lpString);
LPTSTR SYM_EXPORT WINAPI   StringRemoveCRLF(LPTSTR lpString);
LPTSTR SYM_EXPORT WINAPI   StringAppendChar(LPTSTR lpString, WCHAR wChar);
LPTSTR SYM_EXPORT WINAPI   StringAppendNChars(LPTSTR lpString, WCHAR wChar, int nCount);
WCHAR  SYM_EXPORT WINAPI   StringGetLastChar(LPCTSTR lpszString);
LPTSTR SYM_EXPORT WINAPI   StringStripChar(LPTSTR lpString);
LPTSTR SYM_EXPORT WINAPI   StringStripSpaces(LPTSTR lpString);
VOID SYM_EXPORT WINAPI     DOSSetDate (UINT month, UINT day, UINT year);
UINT SYM_EXPORT WINAPI     DOSSetTime (UINT hour, UINT minutes, UINT seconds, UINT hundreths);
UINT SYM_EXPORT WINAPI     HWHasCMOS (void);

UINT SYM_EXPORT WINAPI     SzzStripMatches ( LPSTR lpszzList , LPCSTR lpszMatch ,
                                         BOOL bSubStr ) ;
VOID SYM_EXPORT WINAPI     SzzStripDuplicates ( LPSTR lpszzList ) ;
VOID SYM_EXPORT WINAPI     SzzRemoveEntry ( LPSTR lpszzList ) ;
LPSTR SYM_EXPORT WINAPI    SzzFindNext ( LPCSTR lpszzList ) ;
LPSTR SYM_EXPORT WINAPI    SzzFindEnd ( LPCSTR lpszzList ) ;

#ifndef BASE_10
#define BASE_10         10              // base-10 radix
#define BASE_16         16              // base-16 radix
#endif  // BASE_10

LPSTR SYM_EXPORT WINAPI    _ConvertLongToAbbreviation(DWORD number, LPBYTE string, LPCSTR kiloStr,  LPCSTR megaStr);
LPSTR SYM_EXPORT WINAPI    _ConvertLongKBsToAbbreviation(DWORD number, LPBYTE string, LPCSTR megaStr,  LPCSTR gigaStr);
LPSTR SYM_EXPORT WINAPI    _ConvertDoubleToAbbreviation(double ddNumber, LPBYTE string, LPCSTR kiloStr,  LPCSTR megaStr, LPCSTR gigaStr);
LPTSTR SYM_EXPORT WINAPI   _ConvertLongToString (DWORD dwNumber, LPTSTR lpDest, UINT  wRadix, TCHAR byThousandsChar);
LPSTR SYM_EXPORT WINAPI    _ConvertDoubleToString (double ddNumber, LPSTR lpDest, UINT  wRadix, BYTE byThousandsChar);
LPSTR SYM_EXPORT WINAPI    _ConvertStringToLong(DWORD FAR * lpdwValue, LPSTR lpszString, UINT wRadix, BYTE bIgnoreSeparators);
LPSTR SYM_EXPORT WINAPI    _ConvertStringToDouble(double FAR * lpddValue, LPSTR lpszString, UINT wRadix, BYTE bIgnoreSeparators);
LPTSTR SYM_EXPORT WINAPI   _ConvertWordToString (UINT wNumber,LPTSTR lpString,UINT wRadix,TCHAR byThousandsChar);
LPSTR SYM_EXPORT WINAPI    ConvertLongToAbbreviation(DWORD number, LPSTR string);
LPSTR SYM_EXPORT WINAPI    ConvertLongKBsToAbbreviation(DWORD number, LPSTR string);
LPSTR SYM_EXPORT WINAPI    ConvertDoubleToAbbreviation(double ddNumber, LPSTR string);
LPSTR SYM_EXPORT WINAPI    ConvertLongToString (DWORD dwNumber, LPSTR lpDest);
LPSTR SYM_EXPORT WINAPI    ConvertDoubleToString (double ddNumber, LPSTR lpDest);
LPSTR SYM_EXPORT WINAPI    ConvertSizeToFullString(DWORD number, LPSTR string);
LPSTR SYM_EXPORT WINAPI    ConvertDoubleSizeToFullString(double ddNumber, LPSTR string);
LPSTR SYM_EXPORT WINAPI    ConvertSizeToShortString(DWORD number, LPSTR string);
LPSTR SYM_EXPORT WINAPI    ConvertDoubleSizeToShortString(double ddNumber, LPSTR string);
LPSTR SYM_EXPORT WINAPI    ConvertStringToWord (UINT FAR * lpwValue, LPSTR lpBuffer);
LPSTR SYM_EXPORT WINAPI    ConvertStringToLong (DWORD FAR * lpdwValue, LPSTR lpBuffer);
LPSTR SYM_EXPORT WINAPI    ConvertStringToDouble (double FAR * lpddValue, LPSTR lpBuffer);
LPSTR SYM_EXPORT WINAPI    ConvertWordToString (UINT wNumber,LPSTR lpString);
int SYM_EXPORT WINAPI      lstrncmp(LPCSTR lpString1, LPCSTR lpString2, UINT wLength);
int SYM_EXPORT WINAPI      lstrncmpi(LPCSTR lpString1, LPCSTR lpString2, UINT wLength);

#ifdef  SYM_WIN
int SYM_EXPORT WINAPI      DOSIntlStringCompare(LPCSTR lpszString1, LPCSTR lpszString2);
#else
#define DOSIntlStringCompare(lpszString1, lpszString2) STRCMPI(lpszString1, lpszString2)
#endif

// Macro covers for preparing strings for DOS and for getting strings
// back from DOS.  Use these for international support at the lowest
// level (NDLL)

                                        // -----------------------------------
                                        // MAX_INTL_STR was defined as 255
                                        // and this causes crashes when QA
                                        // pushes the max LFN path limit.
                                        // -----------------------------------

#define MAX_INTL_STR    (SYM_MAX_PATH+1)

                                        // Use this constant to declare
                                        // strings to be used with these
                                        // macros

                                        // Microsoft recommends doing the
                                        // upper before the ansi to oem
                                        // conversion (DOS needs upper case).

#if defined(SYM_WIN) && !defined(UNICODE)

#ifdef  __cplusplus

inline void NAnsiToOemBuff(LPSTR lpAnsiStr, LPSTR lpOemStr, WORD wLength)
                {
                char szAnsiUpper[MAX_INTL_STR];

                STRNCPY(szAnsiUpper, lpAnsiStr, wLength);
                AnsiUpperBuff(szAnsiUpper, wLength);
                AnsiToOemBuff(szAnsiUpper, lpOemStr, wLength);
                }

inline void NAnsiToOem(LPSTR lpAnsiStr, LPSTR lpOemStr)
                {
                char  szAnsiUpper[MAX_INTL_STR];
#ifdef SYM_WIN32
                strcpy(szAnsiUpper, lpAnsiStr);
#else
                lstrcpy(szAnsiUpper, lpAnsiStr);
#endif
                AnsiUpper(szAnsiUpper);
                AnsiToOem(szAnsiUpper, lpOemStr);
                }

                                        // Use these covers for symmetry and
                                        // in case we have to add some
                                        // processing later.
inline void NOemToAnsiBuff(LPSTR lpOemStr, LPSTR lpAnsiStr, WORD wLength)
                { OemToAnsiBuff(lpOemStr, lpAnsiStr, wLength); }

inline void NOemToAnsi(LPSTR lpOemStr, LPSTR lpAnsiStr)
                { OemToAnsi(lpOemStr, lpAnsiStr); }

#else                                   // not __cplusplus

#define NAnsiToOemBuff(lpAnsiStr, lpOemStr, nLength) \
    {                                                \
    char szAnsiUpper[MAX_INTL_STR];                  \
                                                     \
    STRNCPY(szAnsiUpper, lpAnsiStr, nLength);      \
    AnsiUpperBuff(szAnsiUpper, nLength);             \
    AnsiToOemBuff(szAnsiUpper, lpOemStr, nLength);   \
    }

#define NAnsiToOem(lpAnsiStr, lpOemStr)              \
    {                                                \
    char  szAnsiUpper[MAX_INTL_STR];                  \
                                                     \
    lstrcpy(szAnsiUpper, lpAnsiStr);                 \
    AnsiUpper(szAnsiUpper);                          \
    AnsiToOem(szAnsiUpper, lpOemStr);                \
    }

                                        // Use these covers for symetry and
                                        // in case we have to add some
                                        // processing later.
#define NOemToAnsiBuff(lpOemStr, lpAnsiStr, nLength) \
    OemToAnsiBuff(lpOemStr, lpAnsiStr, nLength)

#define NOemToAnsi(lpOemStr, lpAnsiStr)              \
    OemToAnsi(lpOemStr, lpAnsiStr)

#endif                                  // not __cplusplus

#else                                   // These aren't used in DOS

#define NAnsiToOemBuff(a,o,l) { SYM_ASSERT( o != NULL && a != NULL ); STRNCPY(o, a, l); }
#define NAnsiToOem(a,o)       { SYM_ASSERT( o != NULL && a != NULL ); STRCPY(o, a); }
#define NOemToAnsiBuff(o,a,l) { SYM_ASSERT( o != NULL && a != NULL ); STRNCPY(a, o, l); }
#define NOemToAnsi(o,a)       { SYM_ASSERT( o != NULL && a != NULL ); STRCPY(a, o); }

LPSTR SYM_EXPORT WINAPI DOS_AnsiToOem(LPCSTR str1, LPSTR str2);
LPSTR SYM_EXPORT WINAPI DOS_AnsiToOemBuff(LPCSTR str1, LPSTR str2, UINT length);
LPSTR SYM_EXPORT WINAPI DOS_OemToAnsi(LPCSTR str1, LPSTR str2);
LPSTR SYM_EXPORT WINAPI DOS_OemToAnsiBuff(LPCSTR str1, LPSTR str2, UINT length);

#endif

#ifdef  SYM_WIN32

#   define SET_FILE_APIS_TO_OEM  SetFileApisToOEM ()
#   define SET_FILE_APIS_TO_ANSI SetFileApisToANSI ()
#   define ARE_FILE_APIS_ANSI    AREFILEAPISANSI
#   define AREFILEAPISANSI       AreFileApisANSI ()

#else
                                    // These are available only in Win32
#   define SET_FILE_APIS_TO_OEM
#   define SET_FILE_APIS_TO_ANSI
#   define ARE_FILE_APIS_ANSI       AREFILEAPISANSI
#   define AREFILEAPISANSI          FALSE

#endif   // SYM_WIN32

/*----------------------------------------------------------------------*
 * CMOS Port Values for use with CMOSRead and CMOSWrite                 *
 *----------------------------------------------------------------------*/

#ifndef CMOS_SECONDS

#define CMOS_SECONDS            0x00
#define CMOS_MINUTE             0x02
#define CMOS_HOUR               0x04
#define CMOS_DAY_OF_WEEK        0x06
#define CMOS_DAY                0x07
#define CMOS_MONTH              0x08
#define CMOS_YEAR               0x09
#define CMOS_STATUS_REGA        0x0A    // Status Register A
#define CMOS_STATUS_REGB        0x0B    // Status Register B
#define CMOS_STATUS_REGC        0x0C    // Status Register C
#define CMOS_STATUS_REGD        0x0D    // Status Register D
#define CMOS_DIAGNOSTIC         0x0E    // diagnostic register
#define CMOS_SHUTDOWN           0x0F    // shutdown code
#define CMOS_DISKETTE           0x10    // floppy drive types
#define CMOS_FIXED_DISK         0x12    // hard drive types
#define CMOS_PS2_FIXED_DISK1    0x11    // PS/2 hard drive 1
#define CMOS_PS2_FIXED_DISK2    0x12    // PS/2 hard drive 2
#define CMOS_EQUIPMENT          0x14    // Equipment installed (INT 11h)
#define CMOS_BASE_MEM_LO        0x15    // Base memory size
#define CMOS_BASE_MEM_HI        0x16
#define CMOS_EXT_MEM_LO         0x17    // Extended memory size
#define CMOS_EXT_MEM_HI         0x18
#define CMOS_EXT_DRIVE_C        0x19    // extended type (16-49) for fixed disk 1
#define CMOS_EXT_DRIVE_D        0x1A    // extended type (16-49) for fixed disk 2
#define CMOS_CHECKSUM_HI        0x2E    // checksum for contents 10h-2Dh                CHECK_HI         /* Check sum value             */
#define CMOS_CHECKSUM_LO        0x2F
#define CMOS_POST_MEM_LO        0x30    // extended memory reported by POST
#define CMOS_POST_MEM_HI        0x31
#define CMOS_CENTURY            0x32    // century
#define CMOS_SYS_INFO           0x33    // system information byte

#endif  // CMOS_SECONDS

#define NAME_INVAL_FILE_SPEC           ERR_SEV_NUM (ERR_SEV_WARNING, \
                                                     ERR_FILE_SPEC_INVAL)

                                        // These constants are used internally
                                        // by the Name functions to validate
                                        // component lengths.  You probably
                                        // shouldn't use them.

#define NAME_DOSPATHFILENAMESIZE        SYM_MAX_PATH
#define NAME_FILENAMESIZE               255
#define NAME_FILEEXTSIZE                255

#define NGT_IS_DIR      (0x0001)        // name is a directory
#define NGT_IS_FILE     (0x0002)        // name is a file
#define NGT_IS_ROOT     (0x0004)        // name is a root directory (which includes \\SERVER\VOLUME)
#define NGT_IS_FULL     (0x0008)        // name has a drive, path, and filename (or is full UNC spec)
#define NGT_IS_UNC      (0x0010)        // name starts with '\\'
#define NGT_IS_SERVER   (0x0020)        // name is "\\SERVER" or "\\SERVER\"

#define NameIsDirectory(n)      (NameGetType(n) & NGT_IS_DIR)
#define NameIsFile(n)           (NameGetType(n) & NGT_IS_FILE)

BOOL SYM_EXPORT WINAPI   NameIsRoot(LPCTSTR lpName);

                                        // Get long version of every component
                                        // of lpszSpec
STATUS SYM_EXPORT WINAPI NameReturnLongName(
   LPCTSTR lpszSpec,                    // Existing name you want as long
   LPTSTR lpszLongSpec,                 // Place to return long name
   BOOL bFmtForDisp);                   // TRUE to format for display

                                        // Get short (8.3) version of
                                        // every component of lpszSpec
STATUS SYM_EXPORT WINAPI NameReturnShortName(
   LPCTSTR lpszSpec,                    // Spec to get short version of
   LPTSTR lpszShortSpec);               // Place to return short version

UINT SYM_EXPORT WINAPI   NameConvertFwdSlashes(LPTSTR lpPath);
UINT SYM_EXPORT WINAPI   NameReturnExtensionEx(LPCTSTR lpszFileSpec, LPTSTR lpszExtension, UINT cbExtBuffSize);
UINT SYM_EXPORT WINAPI   NameReturnExtension (LPCTSTR lpszFileSpec, LPTSTR lpszExtension);
UINT SYM_EXPORT WINAPI   NameReturnFileEx (     /* Return file name             */
    LPCTSTR      lpszFileSpec,                  /* File spec containing ext.    */
    LPTSTR       lpszFileName);                 /* Buffer to return file name   */
UINT SYM_EXPORT WINAPI   NameReturnFile (LPCTSTR lpszFileSpec, LPTSTR lpszFileName);
LPTSTR SYM_EXPORT WINAPI NameStripFile(LPTSTR lpName);
LPTSTR SYM_EXPORT WINAPI NameStripExtension(LPTSTR lpName);
VOID SYM_EXPORT WINAPI   NameReturnPath(LPCTSTR name, LPTSTR path_name);
VOID SYM_EXPORT WINAPI   NameReturnRoot(LPCTSTR lpszFile, LPTSTR lpszRoot);
VOID SYM_EXPORT WINAPI   NameReturnStartDir(LPCTSTR lpszString, LPTSTR lpszStartDir, LPTSTR lpszCmdLine);
VOID SYM_EXPORT WINAPI   NameAppendFile(LPTSTR lpPathname, LPCTSTR lpFilename);
VOID SYM_EXPORT WINAPI   NameAppendExtension(LPTSTR lpPathname, LPCTSTR lpszExtension);
VOID SYM_EXPORT WINAPI   NameToUnpacked(LPTSTR dst, LPCTSTR src);
VOID SYM_EXPORT WINAPI   NameToPacked(LPTSTR dst,LPCTSTR src);
VOID SYM_EXPORT WINAPI   NameConvertPath(LPTSTR lpPath,LPCTSTR lpSrc);
VOID SYM_EXPORT WINAPI   NameCompact(LPTSTR name);
BOOL SYM_EXPORT WINAPI   NameWildcardMatch(LPCTSTR lpszWildcard, LPCTSTR lpszFile, UINT uMatchType);
VOID SYM_EXPORT WINAPI   NameUnWildcard (LPCTSTR lpOrig, LPTSTR lpNew);
UINT SYM_EXPORT WINAPI   NameWildtoName(LPTSTR lpFullName, LPCTSTR lpName);
BOOL SYM_EXPORT WINAPI   NameIsDevice(LPCTSTR lpSrc);
BOOL SYM_EXPORT WINAPI   PackedNameIsDevice(LPCTSTR lpSrc);
BOOL SYM_EXPORT WINAPI   NameReplacePathChar(LPTSTR lp);

// ######################################################################### //
// OBSOLETE : THIS API SHOULD NOT BE USED.
// USE NameIsValidFileCharEx() specifying LFN or SFN.
// This function will give backwards compatible behavior (which ignores LFN).
// ######################################################################### //
BOOL SYM_EXPORT WINAPI   NameIsValidFileChar (TCHAR ch, BOOL bAnyChar);
// ######################################################################### //

BOOL SYM_EXPORT WINAPI NameIsValidFileCharEx (
    TCHAR   character,                  // character to validate.
    BOOL    bAnyChar,                   // TRUE to accept '.' and ' ' without question
                                        // (normally pass FALSE for bAnyChar)
    BOOL    bValidateForSFN             // TRUE to validate as SFN, FALSE for LFN
    );
UINT SYM_EXPORT WINAPI   NameTitleFromCmdLine(LPCTSTR lpOrigStr, LPTSTR lpDocStr);
#ifdef SYM_WIN32
UINT SYM_EXPORT WINAPI   NameShortenFileNameRect(
   LPCTSTR lpPathName,                  // Original path spec
   LPTSTR lpShortName,                  // Place to return shortened version
   HDC hDc,                             // DC to paint text into
   LPCRECT lpRect);                     // Rectangle to fit text into
#endif                                  // ifdef SYM_WIN32
UINT SYM_EXPORT WINAPI   NameShortenFileName(LPCTSTR lpPathName, LPTSTR lpShortName, int nWidth);
UINT SYM_EXPORT WINAPI   NameTruncate (LPTSTR lpName);
int  SYM_EXPORT WINAPI   NamePathToEnvironment(LPCTSTR lpInput, LPTSTR lpOutput);
UINT SYM_EXPORT WINAPI   TrueNameGet (LPCTSTR lpRelativePath, LPTSTR lpTrueName);
UINT SYM_EXPORT WINAPI   NameGetType (LPCTSTR lpPath);
void SYM_EXPORT WINAPI   NameParsePath (LPTSTR lpPath, LPTSTR lpFile);
UINT SYM_EXPORT WINAPI   NameParseUNC(
   LPCTSTR lpszUNC,                     // UNC spec of at least "\\server"
   LPTSTR  lpszServer,                  // Place to return SERVER
   LPTSTR  lpszVolume,                  // Place to return VOLUME
   LPTSTR  lpszPath);                   // Place to return the rest
void SYM_EXPORT WINAPI   NameSeparatePath(LPCTSTR lpPath, LPTSTR lpDir, LPTSTR lpFile);
UINT SYM_EXPORT WINAPI   NameToFull (LPTSTR lpDest, LPCTSTR lpSource);
BOOL SYM_EXPORT WINAPI   NameIsWildcard (LPCTSTR lpPathName);
BOOL SYM_EXPORT WINAPI   NameValidatePath (LPTSTR lpPathName);
BOOL SYM_EXPORT WINAPI   NameValidatePathEx (LPTSTR lpPathName, BOOL bAllowWild);
UINT SYM_EXPORT WINAPI   NameMakeUnique (LPTSTR lpPath);
void SYM_EXPORT WINAPI   NameDerivePath (LPCTSTR lpPath, LPCTSTR lpNewDir, LPTSTR lpReturnPath);

// ######################################################################### //
//  This API should not be used. Use NameReturnIllegalChars() instead.
// ######################################################################### //
UINT SYM_EXPORT WINAPI   NameGetIllegalChars (LPTSTR pBuffer, BOOL bPathChars);
// ######################################################################### //

// ---------------------------------------------------------------------------
// ####                 OS_ILLEGAL_xxxx flag description                  ####
// ---------------------------------------------------------------------------
// These defines should be used with NameReturnIllegalChars() function.
// The OS_ILLEGAL_USEVOLUME flag has precedence over the specific
// operating system flags such as OS_ILLEGAL_DOS.
// The OS_ILLEGAL_WILDCARDCHARS flag adds the wildcard character to the list
// of invalid characters.
// The OS_ILLEGAL_PATHCHARS flag adds the path characters to the list of
// invalid characters.
// The OS_ILLEGAL_SFN_LOWERCASE flag is only checked if the OS for which we
// are obtaining invalid characters for does not support LFNs such as MS-DOS
// For a non LFN operating system if this flag is set than all the lower case
// letter are added to the list of invalid characters.
// ---------------------------------------------------------------------------

#define SYM_ILLEGAL_USEVOLUME       0x8000
#define SYM_ILLEGAL_DOS             0x0001
#define SYM_ILLEGAL_WIN16           0x0002
#define SYM_ILLEGAL_WIN32s          0x0003
#define SYM_ILLEGAL_WIN95           0x0004
#define SYM_ILLEGAL_WINNT           0x0005
#define SYM_ILLEGAL_OS2             0x0006
#define SYM_ILLEGAL_WILDCARDCHARS   0x0010
#define SYM_ILLEGAL_PATHCHARS       0x0020
#define SYM_ILLEGAL_SFN_LOWERCASE   0x0040

EXTERNC UINT SYM_EXPORT WINAPI NameReturnIllegalChars ( // Get illegal characters
    WORD        wOsFlag,                // [in] use any OS_ILLEGAL_xxx
    LPCTSTR     lpVolume,               // [in] pointer to volume, for LFN
    LPTSTR      lpIllegals,             // [out] illegal chars placed here
        UINT            uSizeIllegals           // [in] max size of lpIllegals
        );

#ifdef  SYM_WIN
UINT SYM_EXPORT WINAPI   NameIsExecutable(LPCSTR lpPath, LPTSTR lpFullPath);
int  SYM_EXPORT WINAPI   NameIsExecutableInitDir(LPCTSTR lpCmdLine, LPCTSTR lpInitDir, LPTSTR lpFullExePath);
UINT SYM_EXPORT WINAPI   NameLooksLikeExecutable(LPCTSTR lpFilePath);
BOOL SYM_EXPORT WINAPI   NameFindExecutable(LPCTSTR lpExt, LPTSTR lpExeName);
UINT SYM_EXPORT WINAPI   NameReturnAssociate(LPCTSTR lpFileName, LPTSTR lpFullExePath);
#endif

#ifndef FILE_AND_NAME_MACROS            // ALSO IN FILE.H
   #define FILE_AND_NAME_MACROS
 #if !defined(UNICODE)
   #define BASIC_PATH              0x005C3A78L     // "x:\\"
   #define BASIC_DRIVE             0x3A78          // "x:"

   #ifndef __cplusplus
      #define FileInitPath(string)    (*(LPDWORD)((LPSTR)(string)) = BASIC_PATH)
      #define FileInitDrive(string)   { *(LPWORD)((LPSTR)(string)) = BASIC_DRIVE;(string)[2]='\0'; }

      #define NameIsDot(filename)        (*(LPWORD)((LPCSTR)(filename))==0x002E)
      #define NameIsDoubleDot(filename)  (*(LPWORD)((LPCSTR)(filename))==0x2E2E && !((LPCSTR)filename)[2])
      #define NameIsUNC(filename)        (*(LPWORD)((LPCSTR)(filename))==0x5C5C)

      #define NameHasDriveSpec(string)   ((string)[0]!='\0' && (string)[1]==':')
      #define NameIsFull(path)                  \
         (NameIsUNC(path) ?                     \
            NameGetType(path) & NGT_IS_FULL :   \
            (*(path)=='\\' || ((path)[1]==':' && (path)[2]=='\\')))
   #else                                // C++ - use inline functions
      inline VOID FileInitPath(LPSTR string)
         {
         *(LPDWORD)string = BASIC_PATH;
         }

      inline VOID FileInitDrive(LPSTR string)
         {
         *(LPWORD)string = BASIC_DRIVE;
         string[2] = '\0';
         }

      inline BOOL NameIsDot(LPCSTR filename)
         {
         return(*(LPWORD)filename == 0x002E);
         }

      inline BOOL NameIsDoubleDot(LPCSTR filename)
         {
         return(*(LPWORD)filename == 0x2E2E && !filename[2]);
         }

      inline BOOL NameIsUNC(LPCSTR filename)
         {
         return(*(LPWORD)filename == 0x5C5C);
         }

      inline BOOL NameHasDriveSpec(LPCSTR string)
         {
         return(string[0]!='\0' && string[1]==':');
         }

      inline BOOL NameIsFull(LPCSTR path)
         {
         if (NameIsUNC(path))
            return(NameGetType(path) & NGT_IS_FULL);
         else
            return(*(path)=='\\' || ((path)[1]==':' && (path)[2]=='\\'));
         }
   #endif                               // #ifndef __cplusplus/#else
 #else                                  // #if !defined(UNICODE)

      #define NameIsDot(filename)        (*((LPDWORD)filename)==0x0000002EL)
      #define NameIsDoubleDot(filename)  (*((LPDWORD)filename)==0x002E002EL && !((LPCTSTR)filename)[2])
      #define NameIsUNC(filename)        (*((LPDWORD)filename)==0x005C005CL)

      #define NameHasDriveSpec(string)   ((string)[0]!='\0' && (string)[1]==':')

 #endif
#endif                                  // #ifndef FILE_AND_NAME_MACROS


BOOL SYM_EXPORT WINAPI   CharIsNumeric (UINT c);
BOOL SYM_EXPORT WINAPI   CharIsHexNumeric (UINT c);
BOOL SYM_EXPORT WINAPI   CharIsWhite (UINT c);


#ifdef SYM_WIN32

__inline int CharToUpper(int c)
    {
    TCHAR ch = (TCHAR)c;
    CharUpperBuff(&ch, 1);
    return ((int)ch);
    }

__inline int CharToLower(int c)
    {
    TCHAR ch = (TCHAR)c;
    CharLowerBuff(&ch, 1);
    return ((int)ch);
    }

#elif defined(SYM_VXD) || defined(SYM_NTK)

#if defined(UNICODE)
#define CharToUpper         towupper
#define CharToLower         towlower
#else
#define CharToUpper         toupper
#define CharToLower         tolower
#endif

#else

#if !defined(SYM_NLM) && !defined(SYM_OS2)

UINT SYM_EXPORT WINAPI   CharToUpper (UINT c);
UINT SYM_EXPORT WINAPI   CharToLower (UINT c);

#else
#   ifdef SYM_OS2
#       include <ctype.h>
#   endif
#   define CharToUpper         toupper
#   define CharToLower         tolower
#endif  // !SYM_NLM, OS2, SYM_NTK

#endif                                  // !SYM_WIN32
// CharIs... functions are documented in PRM_STR.C.
#ifndef SYM_WIN           // DOS char conversion functions

BOOL           WINAPI   CharIsAlpha (UINT c);
BOOL           WINAPI   CharIsAlphaNumeric (UINT c);
BOOL           WINAPI   CharIsLower (UINT c);
BOOL           WINAPI   CharIsUpper (UINT c);

#else   // SYM_WIN                Use Windows functions instead

#define CharIsAlpha(c)          IsCharAlpha((char)(c))
#define CharIsAlphaNumeric(c)   IsCharAlphaNumeric((char)(c))
#define CharIsLower(c)          IsCharLower((char)(c))
#define CharIsUpper(c)          IsCharUpper((char)(c))

#endif  // SYM_WIN

LPTSTR SYM_EXPORT WINAPI   StringMaxCopy (LPTSTR lpDest, LPCTSTR lpSource, UINT uMaxSize);

#ifndef SYM_WIN
extern  BYTE CDECL gbDirWasChanged;

#define SetDirChangedFlag()     gbDirWasChanged = TRUE
#else
#define SetDirChangedFlag()     { }
#endif


VOID SYM_EXPORT WINAPI BufferEncrypt(
    LPVOID  lpbyTxt,                    // buffer to encrypt
    LPVOID  lpbyKey,                    // key used to encrypt
    DWORD   dwTxtLen,                   // length of text
    DWORD   dwKeyLen);                  // length of key

VOID SYM_EXPORT WINAPI BufferDecrypt(
    LPVOID  lpbyTxt,                    // buffer to decrypt
    LPVOID  lpbyKey,                    // key used to decrypt
    DWORD   dwTxtLen,                   // length of text
    DWORD   dwKeyLen);                  // length of key


// -------------------------------------------------------------------------
// Text Search Routines - Memory and Files
// -------------------------------------------------------------------------

#define TS_MAX_SEARCH_LEN       256     // maximum length of search text

#define TS_CANT_ALLOC_MEMORY    0x1000
#define TS_UNITIALIZED_BLOCK    0x1001
#define TS_TEXT_TOO_LONG        0x1002

#define TS_MATCH_CASE           0x0001  // case-sensitive search
#define TS_OEM_CHARSET          0x0002  // use OEM character set
#define TS_NO_OPEN_FILE         0x0004  // file is already open
#define TS_USERBUFFER           0x0008  // caller provides buffer (use lpUserBuffer)
#define TS_FILESIZE             0x0010  // limit file searches (use lFileLimit)
#define TS_RESERVED             0x8000  // internal use

typedef struct TEXTSEARCHREC
    {
    BOOL        bFound;                 // return value
    UINT        wFlags;                 // search flags
    HFILE       hFile;                  // file handle (for TS_NO_OPEN_FILE)
    UINT        wError;                 // DOS error code
    UINT        wTextLen;               // length of the text spec
    BYTE        byaIndexTable[256];     // index table for all the ASCII values
    char        szLowerText[TS_MAX_SEARCH_LEN];
    char        szUpperText[TS_MAX_SEARCH_LEN];
    LPSTR       lpUserBuffer;           // caller's buffer (used with TS_USERBUFFER)
    LONG        lFileLimit;
    } TEXTSEARCHREC, FAR *LPTEXTSEARCHREC;

UINT SYM_EXPORT WINAPI TextSearchPrep (LPTEXTSEARCHREC lpPacket, LPSTR lpSearchText);
UINT SYM_EXPORT WINAPI TextSearchMemory (LPTEXTSEARCHREC lpPacket, LPSTR lpBuffer, UINT wSize);
UINT SYM_EXPORT WINAPI TextSearchFile (LPTEXTSEARCHREC lpPacket, LPSTR lpszFileName);


// -------------------------------------------------------------------------
// Covers of Windows private profile APIs for non-Windows platforms.
// -------------------------------------------------------------------------
#if !defined(SYM_WIN) && !defined(SYM_OS2) && !defined(SYM_VXD) && !defined(SYM_NTK)
int SYM_EXPORT WINAPI GetPrivateProfileString(
    LPCSTR lpSection,
    LPCSTR lpEntry,
    LPCSTR lpDefault,
    LPSTR  lpBuffer,
    int    iSize,
    LPCSTR lpFileName );

int SYM_EXPORT WINAPI WritePrivateProfileString(
    LPCSTR lpSection,
    LPCSTR lpEntry,
    LPCSTR lpString,
    LPCSTR lpFileName );

UINT SYM_EXPORT WINAPI GetPrivateProfileInt(
  LPCSTR lpSection,
  LPCSTR lpEntry,
  int    iDefault,
  LPCSTR lpFileName );
#endif

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------
UINT SYM_EXPORT WINAPI SystemGetWindowsType(VOID);
BOOL SYM_EXPORT WINAPI SystemWin95ShellPresent(VOID);

#ifdef SYM_WIN
HINSTANCE SYM_EXPORT WINAPI LibraryLoad (LPCSTR lpLibFileName);
#endif

#endif  // !defined(SYM_UNIX)

#ifdef __cplusplus                    // end prototypes
}
#endif

#endif  // INCL_XDOS
