// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
/* Copyright 1992 - 2003, 2005 Symantec Corporation. All rights reserved.     */
/************************************************************************
 *                                                                      *
 * $Header:   S:/INCLUDE/VCS/file.h_v   1.27   18 Feb 1998 12:02:48   mdunn  $ *
 *                                                                      *
 * Description:                                                         *
 *      All things having to do with file and directory operations.     *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * $Log:   S:/INCLUDE/VCS/file.h_v  $ *
// 
//    Rev 1.27   18 Feb 1998 12:02:48   mdunn
// Ported ROSWELL and ROSNEC changes from K branch
// 
//    Rev 1.26   26 Aug 1997 19:32:52   DDREW
// Added 4 byte attribute prototypes for NLM platform
// 
//    Rev 1.25   04 Aug 1997 09:55:48   BGERHAR
// Added FileCreateTemporaryPath() to control where temp files are created
// 
//    Rev 1.24   20 Jun 1997 22:40:10   MKEATIN
// Added the prototype for FileWipe().
// 
//    Rev 1.23   25 May 1997 20:30:02   RStanev
// Added FileCreateNativeNTNameEx().
//
//    Rev 1.22   25 Feb 1997 23:37:20   RSTANEV
// Prototyped FileCopyHandle() for SYM_VXD.
//
//    Rev 1.21   21 Feb 1997 14:41:36   THOFF
// Added the O_BINARY flag for all files opened using FileOpen under OS/2.
//
//    Rev 1.20   06 Nov 1996 14:56:34   SPASNIK
// One and only one, FILETIME
//
//    Rev 1.19   06 Nov 1996 14:20:30   SPAGLIA
// Fixed byHours and byMinutes
//
//    Rev 1.18   06 Nov 1996 14:16:20   SPAGLIA
// Fixed missing semi;
//
//    Rev 1.17   06 Nov 1996 13:51:40   SPAGLIA
// More improvements to OS/2
//
//    Rev 1.16   18 Oct 1996 21:13:44   RSTANEV
// Prototyped FileCreateNativeNTName() and FileDestroyNativeNTName().
//
//    Rev 1.15   29 Aug 1996 18:17:02   DHERTEL
// Merged changed from Quake E:
// Added prototype for FileCopySecurityInformation.
//
//    Rev 1.14   27 Aug 1996 13:47:18   BMCCORK
// Updated the comments on the prototype of FileCreateTemporary()
//
//    Rev 1.13   15 Aug 1996 10:58:02   BMCCORK
// Changed FileGetTime/Date string functions to STDCALL
//
//    Rev 1.12   09 Aug 1996 10:40:18   TLE
// Organize the function prototype of common FILE apis
//
//    Rev 1.11   25 Jul 1996 11:10:24   BMCCORK
// Changed FileBufferedGetLine to use buffer length instead of string length and added asserts
//
//    Rev 1.10   19 Jul 1996 16:46:10   BMCCORK
// Added FA_TEMPORARY and FA_COMPRESSED for Win32 compatability
//
//    Rev 1.9   11 Jul 1996 14:13:18   MARKK
// Fixed missing backslash
//
//    Rev 1.8   11 Jul 1996 13:30:58   TLE
//
//    Rev 1.7   07 Jun 1996 10:21:36   GDZIECI
// Define SEEK_FROM_BEGINNING, FA_NORMAL, FA_READ_ONLY, FA_HIDDEN and FA_SYSTEM
// conditionally: only if not defined yet.
// This fixes conflicts with Novell SDK include files.
//
//    Rev 1.6   24 May 1996 17:57:36   THOFF
// Fixed unconditional redefinition of FA_HIDDEN and FA_SYSTEM under OS/2.
//
//    Rev 1.5   22 May 1996 15:48:56   RSTANEV
// Prototyped FileCopyHandle().
//
//    Rev 1.4   22 May 1996 15:20:32   RSTANEV
// Prototyped FileCopySecurityInformation().
//
//    Rev 1.3   19 Mar 1996 17:29:18   THOFF
// Added FileFindFirst / FileFindNext support for OS/2 -- yes, OS/2...
//
//    Rev 1.2   30 Jan 1996 17:41:48   RSTANEV
// Prototyped FileLockRegion() and FileUnlockRegion().
//
//    Rev 1.1   30 Jan 1996 10:44:48   RSTANEV
// More Unicode changes.
//
//    Rev 1.0   26 Jan 1996 20:18:54   RJACKSO
// Initial revision.
//
//    Rev 1.98   24 Jan 1996 09:34:44   RSTANEV
// SYM_NTK changes.
//
//    Rev 1.97   18 Jan 1996 09:39:52   RSTANEV
// Added Unicode support.
//
//    Rev 1.96   20 Dec 1995 18:52:32   RSTANEV
// Changed the prototype of FileRename() to accept TCHAR.
//
//    Rev 1.95   11 Dec 1995 15:56:44   HENRI
// Fixed compile compatibility for C++ and UNICODE compiles
//
//    Rev 1.94   08 Dec 1995 14:22:32   RSTANEV
// Prototyped some more functions to use TCHAR.
//
//    Rev 1.93   08 Dec 1995 10:08:02   RSTANEV
// Prototyped FileDelete() to accept LPTSTR for SYM_NTK.
//
//    Rev 1.92   05 Dec 1995 17:46:34   RSTANEV
// Different prototype of FileLength() for SYM_NTK platform.
//
//    Rev 1.91   05 Dec 1995 17:36:02   RSTANEV
// Properly prototyping file functions for SYM_NTK.
//
//    Rev 1.90   01 Dec 1995 18:17:46   RSTANEV
// Added TCHAR support for NTK platform.
//
//    Rev 1.89   30 Nov 1995 14:36:56   RSTANEV
// Fixed a conflict with declaration of OpenFile().
//
//    Rev 1.88   30 Nov 1995 13:21:16   ASUZDALT
// Changed macros and inlines for file read, write, seek and close:
// Added functions WIN_Filexxxx for WIN16 and WIN32.
//
//    Rev 1.87   10 Nov 1995 16:03:44   GDZIECI
// Added prototypes for FileGet/SetTimeByName() - NLM platform only.
//
//    Rev 1.86   21 Jun 1995 13:48:04   PGRAVES
// Added COPY_READ_ERROR.
//
//    Rev 1.85   11 Apr 1995 16:15:12   REESE
// Merged QAK6 brank into trunk.
//
//    Rev 1.84   09 Mar 1995 21:20:16   RSTANEV
// Prototyped a couple of functions for DOS platform.
//
//    Rev 1.83   01 Feb 1995 14:09:54   TONY
// Applied NLM covers for SymKrnl FOP_DIR.C functions to OS/2 also.
//
//    Rev 1.82   27 Dec 1994 16:26:30   BRUCE
// Changed pathtok and pathrevtok input parm to be an LPCSTR
//
//    Rev 1.81   21 Dec 1994 05:02:40   BRUCE
// Added missing inline for NameHasDriveSpec() macro
//
//    Rev 1.80   21 Dec 1994 04:25:14   BRUCE
// Made NUMEROUS changes to support UNCs
//
//    Rev 1.79   14 Dec 1994 12:47:06   BRAD
// DTASet() and DTAGet() needed for VxDs, too
//
//    Rev 1.78   11 Nov 1994 11:27:04   TONY
// Changed FileCreate() for OS/2 to a function in SymKrnl, to fully support attributes.
//
//    Rev 1.77   10 Nov 1994 12:56:22   TONY
// Added FileCreate to OS/2
//
//    Rev 1.76   02 Nov 1994 16:43:42   BRAD
// Added DBCS changes
//
//    Rev 1.75   01 Nov 1994 14:42:54   JMILLARD
//
// add includes for SYM_NLM users of file.h
//
//    Rev 1.74   24 Oct 1994 16:28:18   BRUCE
// Added FileGetDate/TimeString() functions for Win32
//
//    Rev 1.73   19 Oct 1994 15:54:30   BRAD
// They change the date/time creation/access #s
//
//    Rev 1.72   23 Sep 1994 12:30:04   TONY
// Added support for OS/2 file ops
//
//    Rev 1.71   29 Aug 1994 18:26:52   JMILLARD
// add DONT_DEFINE_OPENFILE to work around a Mario nested include problem
//
//    Rev 1.70   25 Aug 1994 09:22:34   JMILLARD
// provide #define maps for SYM_NLM for DirMake (mkdir), DirSet (chdir),
// FileDelete (unlink), FileOpen (open), FileCreate (creat), FileSetAttr (chmod)
// These match the original maps from Quake 2
//
//    Rev 1.69   23 Aug 1994 17:43:08   JMILLARD
// provide the usual overrides for FileOpen, FileClose, etc for SYM_NLM
//
//    Rev 1.68   05 Aug 1994 22:22:22   RSTANEV
// Fixed VxD compilation errors.
//
//    Rev 1.67   14 Jul 1994 12:40:32   BRAD
// Need to call FileTime functions for WIN32 now, since need to call ...LocalFileTime() first
//
//    Rev 1.66   14 May 1994 16:01:20   BRAD
// Removed functions which are prototyped in DISK.H.  Removed unused buffered i/o
//
//    Rev 1.65   09 May 1994 17:05:54   BRAD
// FileLength is macro for non-Windows platforms that are 32-bit
//
//    Rev 1.64   04 May 1994 15:09:48   BRAD
// FileDelete() and FileRename() are functions, not macros for WIN32
//
//    Rev 1.63   04 May 1994 01:33:18   BRAD
// Don't use NFileOpen()
//
//    Rev 1.62   02 May 1994 17:48:18   BRUCE
// Changed to use LPCFILETIME
//
//    Rev 1.61   26 Apr 1994 16:38:12   BRAD
// Removed PVCS Logs
 ************************************************************************/

#ifndef INCL_FILE
#define INCL_FILE

#ifdef __cplusplus
extern "C" {
#endif

#ifdef SYM_NLM
    #include <io.h>                     // I provide the includes, since the
    #include <stdio.h>                  // caller has no idea what the real
                                        // functions are, or where they come
                                        // from

#endif

#ifdef SYM_OS2
    #include <fcntl.h>
    #include <dir.h>
    #include <dos.h>
#endif

//***********************************************************************
// UNIX Implementation
//
// The Unix version of the library is used by the NAV scanning engine,
// particularly PAM and NAVAPI.  Only the small subset of the library
// that is actually needed for NAV is implemented for Unix.
//***********************************************************************

#if defined (SYM_UNIX)

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

// Values for FileOpen() uOpenMode flags (Windows names)
// Warning: OF_SHARE flags are defined but not supported in Unix
#define OF_READ               0x00000000
#define OF_WRITE              0x00000001
#define OF_READWRITE          0x00000002
#define OF_SHARE_COMPAT       0x00000000
#define OF_SHARE_EXCLUSIVE    0x00000010
#define OF_SHARE_DENY_READ    0x00000020
#define OF_SHARE_DENY_WRITE   0x00000020
#define OF_SHARE_DENY_NONE    0x00000040

// Values for FileOpen() uOpenMode flags (Symantec names)
#define READ_ONLY_FILE        OF_READ
#define WRITE_ONLY_FILE       OF_WRITE
#define READ_WRITE_FILE       OF_READWRITE

// Values for FileGetTime()/FileSetTime() uType
#define TIME_LASTWRITE      0
#define TIME_LASTACCESS     4
#define TIME_CREATION       6

// Bit flags for DOS file attribute values
#define FA_NORMAL    0
#define FA_READ_ONLY 1

// Error return value for file handle functions
#define HFILE_ERROR ((HFILE)-1)

// Error return values for FileCopy()
#define COPY_SUCCESSFUL            NOERR
#define COPY_BREAK_PRESSED         NOERR+200
#define COPY_MEMORY_ERROR          NOERR+201
#define COPY_CRITICAL_ERROR        NOERR+202
#define COPY_DISK_IS_FULL          NOERR+203
#define COPY_READ_ERROR            NOERR+208

// FILETIME data type
// (number of 100-nanosecond intervals since Jan 1, 1601)
typedef struct 
    {
    DWORD dwLowDateTime;
    DWORD dwHighDateTime;
    }
    FILETIME, *LPFILETIME;
typedef const FILETIME FAR *LPCFILETIME;

// File functions.
// Caution: Error return values differ by function.
HFILE FileOpen(LPCTSTR lpName, UINT uOpenMode);
HFILE FileClose(HFILE hFile);
HFILE FileCreate(LPCTSTR lpFileName, UINT uAttr);
HFILE FileCreateTemporary(LPTSTR lpFileName, UINT uAttr);
UINT FileRead(HFILE hFile, LPVOID lpBuffer, UINT uBytes);
UINT FileWrite(HFILE hFile, LPVOID lpBuffer, UINT uBytes);
DWORD FileLength(HFILE hFile);
DWORD FileSeek(HFILE hFile, LONG dwOffset, int nFlag);
UINT FileGetAttr(LPCTSTR lpFileName, UINT* lpuAttr);
UINT FileSetAttr(LPCTSTR lpFileName, UINT uAttr);
UINT FileGetTime(HFILE hFile, UINT uType, LPFILETIME lpFileTime);
UINT FileSetTime(HFILE hFile, UINT uType, LPFILETIME lpFileTime);
BOOL FileExists(LPCTSTR lpFileName);
UINT FileDelete(LPCTSTR lpFileName);
UINT FileKill(LPCTSTR lpFileName);
UINT FileWipe(LPCTSTR lpFileName, UINT uTimesToWipe);
typedef BYTE (WINAPI *BREAKFUNC)(void);
int FileCopy(LPCTSTR lpFromFile, LPCTSTR LPToFile, BREAKFUNC fnBreakCheck);
UINT FileRename(LPCTSTR lpOldName, LPCTSTR lpNewName);
BOOL FileTimeToDosTime(LPCFILETIME lpFileTime, LPWORD lpFatDate, LPWORD lpFatTime);
BOOL DosTimeToFileTime(WORD wFatDate, WORD wFatTime, LPFILETIME lpFileTime);

// Ignore the rest of file.h.

#else  // !defined(SYM_UNIX)


/*----------------------------------------------------------------------*
 * Basic specs for file and path functions.  These are DWORD
 * representations of strings that are used to form drive and path
 * specs.  Instead of doing this:
 *                      StringCopy (path, "x:\\");
 *                      StringCopy (drive, "x:");
 *         do this:
 *                      FileInitPath (path);
 *                      FileInitDrive (drive);
 *----------------------------------------------------------------------*/


#ifndef FILE_AND_NAME_MACROS            // ALSO IN XAPI.H
   #define FILE_AND_NAME_MACROS
 #if !defined(UNICODE)
   #define BASIC_PATH              0x005C3A78L     // "x:\\"
   #define BASIC_DRIVE             0x3A78          // "x:"

   #ifndef __cplusplus
      #define FileInitPath(string)    (*(LPDWORD)((LPSTR)(string)) = BASIC_PATH)
      #define FileInitDrive(string)   \
        { *(LPWORD)((LPSTR)(string)) = BASIC_DRIVE;(string)[2]='\0'; }

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
         return(*(LPDWORD)filename == 0x2E2E && !filename[2]);
         }

      inline BOOL NameIsUNC(LPCSTR filename)
         {
         return(*(LPWORD)filename == 0x5C5C);
         }


      inline BOOL NameHasDriveSpec(LPCSTR string)
         {
         return(string[0]!='\0' && string[1]==':');
         }


      #ifdef INCL_XDOS                  // Only define this inline if
                                        // NameGetType() has been declared
                                        // by including XAPI.H

         inline BOOL NameIsFull(LPCSTR path)
            {
            if (NameIsUNC(path))
               return(NameGetType(path) & NGT_IS_FULL);
            else
               return(*path=='\\' || (path[1]==':' && path[2]=='\\'));
            }
      #endif                            // #ifdef INCL_XDOS
   #endif                               // #ifndef __cplusplus/#else
 #else                                  // #if !defined(UNICODE)

      #define NameIsDot(filename)        (*((LPDWORD)filename)==0x0000002EL)
      #define NameIsDoubleDot(filename)  (*((LPDWORD)filename)==0x002E002EL && !((LPCTSTR)filename)[2])
      #define NameIsUNC(filename)        (*((LPDWORD)filename)==0x005C005CL)

      #define NameHasDriveSpec(string)   ((string)[0]!='\0' && (string)[1]==':')

 #endif
#endif                                  // #ifndef FILE_AND_NAME_MACROS


/* file attributes */

#if !defined(SYM_NLM) && !defined(SYM_OS2)

// NOTE: (BEM - 7/18/96)
// THESE FA_ DEFINES ARE THE ORIGINAL DOS ATTRIBUTE VALUES, BUT THEY ARE
// ALSO NOW USED AS THE CROSS PLATFORM VALUES TO BE USED WITH
// FileGetAttrib() and FileSetAttrib().
// FA_COMPRESSED and FA_TEMPORARY were added for Win32 support, and
// they are ignored in non-Win32 environments.
#if !defined(FA_NORMAL)
    #define FA_NORMAL   ((BYTE)   0)    /* Normal */
#endif
#if !defined(FA_READ)
    #define FA_READ     ((BYTE)   1)    /* read only */
#endif
#if !defined(FA_READ_ONLY)
    #define FA_READ_ONLY    FA_READ
#endif
#define FA_RDONLY       FA_READ
#define FA_HDDN         ((BYTE)   2)    /* hidden    */
#if !defined(FA_HIDDEN)
    #define FA_HIDDEN   FA_HDDN
#endif
#define FA_SYST         ((BYTE)   4)    /* system    */
#if !defined(FA_SYSTEM)
    #define FA_SYSTEM   FA_SYST
#endif
#define FA_HS           ((BYTE) FA_HIDDEN | FA_SYSTEM)
#define FA_VOLM         ((BYTE)   8)    /* volume    */
#define FA_VOLUME       FA_VOLM
#define FA_DIR          ((BYTE)  16)    /* directory */
#define FA_ARCH         ((BYTE)  32)    /* archive   */
#define FA_ARCHIVE      FA_ARCH
#define FA_NONREAD      ((BYTE)0xFE)    /* turn off read (in WF) */
#define FA_NONSHR       ((BYTE)0xF8)    /* turn off system/hidden/readonly in WF */
#define FA_VD           (FA_DIR | FA_VOLUME)

// THE FOLLOWING ARE FOR UPWARD COMPATABILITY WITH WIN32 - THEY ARE NOT SUPPORTED UNDER DOS
#ifndef FA_TEMPORARY
    #define FA_TEMPORARY     ((BYTE) 64)// MAPS TO FILE_ATTRIBUTE_TEMPORARY IN WIN32
#endif
#ifndef FA_COMPRESSED
    #define FA_COMPRESSED    ((BYTE) 128)// MAPS TO FILE_ATTRIBUTE_COMPRESSED IN WIN32
#endif
#endif   // !SYM_NLM

#ifdef SYM_OS2
#define FA_READ         ((BYTE)   1)    /* read only */
#define FA_READ_ONLY    FA_READ
#define FA_HDDN         ((BYTE)   2)    /* hidden    */
#if !defined(FA_HIDDEN)
    #define FA_HIDDEN       FA_HDDN
#endif
#define FA_SYST         ((BYTE)   4)    /* system    */
#if !defined(FA_SYSTEM)
    #define FA_SYSTEM       FA_SYST
#endif
#define FA_HS           ((BYTE) FA_HIDDEN | FA_SYSTEM)
#define FA_VOLM         ((BYTE)   8)    /* volume    */
#define FA_VOLUME       FA_VOLM
#define FA_DIR          ((BYTE)  16)    /* directory */
#define FA_ARCHIVE      FA_ARCH
#define FA_NONREAD      ((BYTE)0xFE)    /* turn off read (in WF) */
#define FA_NONSHR       ((BYTE)0xF8)    /* turn off system/hidden/readonly in WF */
#define FA_VD           (FA_DIR | FA_VOLUME)
#endif

#ifndef SYM_NLM
#define AttrIsDir(a)            ((a) & FA_DIR)
#define AttrIsVolume(a)         ((a) & FA_VOLM)
#define AttrIsArchive(a)        ((a) & FA_ARCH)
#define AttrIsHidden(a)         ((a) & FA_HDDN)
#define AttrIsSystem(a)         ((a) & FA_SYST)
#define AttrIsReadOnly(a)       ((a) & FA_READ)
#endif  // !SYM_NLM

                                        // The following constants may be
                                        // used to construct an open file
                                        // mode (access code | sharing mode |
                                        //  inheritance).
                                        // The corresponding OF_ constants
                                        // in Windows.h can also be used.

                                        // Access code (bits 0-2)
#define READ_ONLY_FILE        OF_READ
#define WRITE_ONLY_FILE       OF_WRITE
#define READ_WRITE_FILE       OF_READWRITE

                                        // Sharing Mode (bits 4-6)
                                        // ?? (Same as 2.x open?)
#define COMPATIBILITY_MODE    OF_SHARE_COMPAT
                                        // Deny any access by other processes
#define DENY_READ_WRITE_FILE  OF_SHARE_EXCLUSIVE
                                        // Deny write access to other process
#define DENY_WRITE_FILE       OF_SHARE_DENY_WRITE
                                        // Deny read access to other processes
#define DENY_READ_FILE        OF_SHARE_DENY_READ
                                        // Allow others to read and write at
                                        // the same time you do!  (To be used
                                        // with record locking?)
#define ALLOW_READ_WRITE      OF_SHARE_DENY_NONE

                                        // Inheritance - bit 7
#define CHILD_INHERIT_FILE    0x00      // Child process inherits file
#define CHILD_NO_INHERIT_FILE 0x80      // Child process doesn't inherit file


#define GET_DATE_AND_TIME       0
#define SET_DATE_AND_TIME       1


/*--------------------------------------------------------------*
 * Seek methods used by the FileSeek(...) routine               *
 *--------------------------------------------------------------*/

#ifndef SEEK_SET
#define SEEK_SET                0
#define SEEK_CUR                1
#define SEEK_END                2
#endif

#if !defined(SEEK_FROM_BEGINNING)
    #define SEEK_FROM_BEGINNING     SEEK_SET
#endif
#define SEEK_FROM_CURRENT_LOC   SEEK_CUR
#define SEEK_TO_END             SEEK_END

#define FILE_NOT_FOUND          2
#define PATH_NOT_FOUND          3
#define ACCESS_DENIED           5

#if 0
// Don't use this anymore.  Use FINDDATA defined below.
/*----------------------------------------------------------------------*/
/* FindFirstRec                                                         */
/*                                                                      */
/*  Record used when searching for a file using DOS's FindFirst         */
/*  routines.                                                           */
/*----------------------------------------------------------------------*/

typedef struct FINDFIRSTREC
    {
                                        /* This top portion must be     */
                                        /* exactly 21 BYTEs long        */
    BYTE        reserved1;              /* Drive #                      */
    BYTE        pattern[11];
    BYTE        searchAttribute;
    UINT        indexWithinParentsDir;  /* Index within dir, base-0     */
    UINT        parentsScn;             /* Parent's SCN                 */
    DWORD        reserved2;             /* UNKNOWN                      */

    BYTE         attribute;
    UINT          time;
    UINT          date;
    DWORD              size;
    char         name[13];
    BYTE        readDirectly;           /* Can we read directly from disk */
    } FindFirstRecord, FINDFIRSTREC, FindFirstRec, FAR *LPFFRECORD;

#endif

//========================================================================
// FILETIME structure and support
//========================================================================

//  File System time stamps are represented with the following structure:
//
// Note: Copied from Windows.h (Win32).  Note that this structure is also
// defined in STORAGE.H, from OLE SDK kit.  That's why we have the '_FILETIME_'
// around it.

#if !defined(SYM_WIN32)

#if !defined(_FILETIME_)
#define _FILETIME_
    typedef struct _FILETIME {
        DWORD dwLowDateTime;
        DWORD dwHighDateTime;
    } FILETIME;
#endif
    TYPEDEF_POINTERS(FILETIME, FILETIME)
#else

// Not defined for WIN32
typedef const FILETIME FAR *LPCFILETIME;
#endif

#if defined(SYM_WIN32)
#define FileTimeCompare         CompareFileTime
#endif

#if defined(SYM_WIN32) || defined(SYM_DOS)

UINT SYM_EXPORT WINAPI FileGetDateString(      // Get a string version of a file's date
    FILETIME FileTime,                  // File time whose date you want as a string
    LPSTR lpszDateString,               // Buffer to receive date as string
    UINT uBuffSize);                    // Pass 0 to get required size

UINT SYM_EXPORT WINAPI FileGetTimeString(// Get a string version of a file's time
    FILETIME FileTime,                  // File time whose time you want as a string
    LPSTR lpszTimeString,               // Buffer to receive time as string
    UINT uBuffSize);                    // Pass 0 to get required size

UINT SYM_EXPORT WINAPI FileGetDateTimeString(  // Get a string version of a file's date and time
    FILETIME FileTime,                  // File time whose date and time you want as a string
    LPSTR lpszDateTimeString,           // Buffer to receive date and time as string
    UINT uBuffSize);                    // Pass 0 to get required size

#endif

#if !defined(SYM_WIN32)

LONG SYM_EXPORT WINAPI FileTimeCompare(
        LPCFILETIME lpFileTime1,
        LPCFILETIME lpFileTime2);
#endif

BOOL SYM_EXPORT WINAPI FileTimeToDosTime(
        LPCFILETIME lpFileTime,
        LPWORD lpFatDate,
        LPWORD lpFatTime);

BOOL SYM_EXPORT WINAPI DosTimeToFileTime(
        WORD wFatDate,
        WORD wFatTime,
        LPFILETIME lpFileTime);

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
#if defined(SYM_OS2)

//========================================================================
// OS/2 OS2FILETIME
//========================================================================

    typedef struct _OS2FILETIME {
        BYTE byTwosecs;
        BYTE byMinute;
        BYTE byHour;
        BYTE byDay;
        BYTE byMonth;
        BYTE byYear;
    } OS2FILETIME, *LPOS2FILETIME;

//========================================================================
// OS/2 FINDDATA
//
//  This structure is similar to FINDDATA, date/time fields are
//  in DOS format and some fields are missing.
//========================================================================

    typedef struct _FINDDATA {
        long            dwFileSizeLow;
        unsigned long   dwFileAttributes;
        OS2FILETIME     ftCreation;
        OS2FILETIME     ftLastAccess;
        OS2FILETIME     ftLastWrite;
        char            szFileName[ 256 ];
        } FINDDATA, *LPFINDDATA;

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
#else

/*----------------------------------------------------------------------*/
/* FINDDATA                                                             */
/*                                                                      */
/*  This structure is IDENTICAL to the one found in Windows.h.  We      */
/*  dup it here, so that other platforms can get it.  Also, the name    */
/*  is different (In windows.h, its called WIN32_FIND_DATA).            */
/*----------------------------------------------------------------------*/

    typedef struct _FINDDATA {
        DWORD       dwFileAttributes;
        FILETIME    ftCreationTime;
        FILETIME    ftLastAccessTime;
        FILETIME    ftLastWriteTime;
        DWORD       dwFileSizeHigh;
        DWORD       dwFileSizeLow;
        DWORD       dwReserved0;
        DWORD       dwReserved1;
        char        szFileName[ 260 ];
        char        szAlternateFileName[ 14 ];
/////////////PRIVATE//////////////////////
#if defined(SYM_WIN32)
        BOOL        bSearchOnAttributes;
        DWORD       dwSearchAttributes;
#endif
    } FINDDATA;

#endif
TYPEDEF_POINTERS(FINDDATA, FINDDATA)


/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/

typedef struct FCB
    {
    char        drive;                  /* Drive number 1=A, ...        */
    char        filename[8];            /* File name, padded with ' 's  */
    char        extension[3];           /* Extension, padded with ' 's  */
    int         block_num;
    int         record_size;
    long        file_size;
    int         date;
    char        reserved[10];
    char        block_record;
    long        relative_record;
    } FCB;

typedef struct XFCB
    {
    BYTE        flag;
    BYTE        reserved[5];
    BYTE        attrib;
    FCB                fcb;
    } XFCB;

typedef struct tagFILEINFO
    {
    DWORD   dwVolume;                   // volume number
    DWORD   dwDirNum;                   // directory entry number
    DWORD   dwOwner;                    // owner uid
    DWORD   dwUpdater;                  // updater uid
    DWORD   dwArchiver;                 // archiver uid
    DWORD   dwGroup;
    DWORD   dwSize;                     // file size
    DWORD   dwAtime;                    // last access time
    DWORD   dwMtime;                    // last modified time
    DWORD   dwCtime;                    // creation time
    DWORD   dwBtime;                    // last backup time
    DWORD   dwAttr;                     // attributes
    BYTE    byOrgNameSpace;             // name space
    } FILEINFO;
TYPEDEF_POINTERS(FILEINFO, FILEINFO)

// ----------------------------------------------------------------------
// PROTOTYPES
// ----------------------------------------------------------------------

#ifdef SYM_WIN
HFILE SYM_EXPORT WINAPI WIN_FileClose(HFILE hFile);
LONG  SYM_EXPORT WINAPI WIN_FileSeek(HFILE hFile, LONG lOffset, int iOrigin);
UINT  SYM_EXPORT WINAPI WIN_FileRead(HFILE hFile, void _huge *lpBuffer, UINT wBytes);
UINT  SYM_EXPORT WINAPI WIN_FileWrite(HFILE hFile, const void _huge *lpBuffer, UINT wBytes);
#endif

/*----------------------------------------------------------------------*/
/* File Macros                                                          */
/*----------------------------------------------------------------------*/

#if defined(SYM_WIN)                   // DOS has functions for these
                                       // WIN and W32 functions are in fop_win.c
   #if defined(SYM_WIN32)
      #define FileReadOverlap   ReadFile
      #define FileWriteOverlap  WriteFile
    #endif // SYM_WIN32


   #ifdef __cplusplus
      inline LONG  FileSeek(
         HFILE hFile,
         LONG  lOffset,                 // Bytes to move
         int   nOrigin)                 // Position to move from
      {
      return WIN_FileSeek(hFile, lOffset, nOrigin);
      }

      inline UINT FileRead(
         HFILE hFile,
         HPVOID hpBuffer,
         UINT  uSize)
      {
         return WIN_FileRead(hFile, hpBuffer, uSize);
      }


      inline UINT FileWrite(
         HFILE hFile,
         HPCVOID hpBuffer,
         UINT uSize)
      {
          return WIN_FileWrite(hFile, (LPCSTR) hpBuffer, uSize);
      }

      inline HFILE FileClose(
         HFILE hFile)
      {
          return WIN_FileClose(hFile);
      }


   #else                                // else Windows but Non-C++ case:

      #define FileSeek          WIN_FileSeek
      #define FileRead          WIN_FileRead
      #define FileWrite         WIN_FileWrite
      #define FileClose         WIN_FileClose

   #endif                               // endif c++

#elif defined(SYM_OS2)
        #include <io.h>
        #define FileClose   close
        #define FileSeek    lseek
        #define FileRead    read
        #define FileWrite   write
        #define FileLength  filelength


#elif defined(SYM_NLM)

        #define FileClose   close
        #define FileSeek    lseek
        LONG FileRead( int hHandle, void *cBuffer, LONG lLen );
        LONG FileWrite( int hHandle, void *cBuffer, LONG lLen );
        #define FileLength  filelength

#elif defined(SYM_32BIT) && !defined(SYM_VXD) && !defined(SYM_NTK)
        #define FileClose   close
        #define FileSeek    lseek
//******#define FileRead    read
        LONG FileRead( int hHandle, void *cBuffer, LONG lLen );
//******#define FileWrite   write
        LONG FileWrite( int hHandle, void *cBuffer, LONG lLen );

#else                                   // else DOS, NTK and VxD wrap case:
        HFILE WINAPI FileClose(HFILE);
        LONG  WINAPI FileSeek(HFILE, LONG, int);
        UINT  WINAPI FileRead(HFILE, LPVOID, UINT);
        UINT  WINAPI FileWrite(HFILE, LPCVOID, UINT);
#endif                                  // endif SYM_WIN

/*----------------------------------------------------------------------*/
/* This structure is needed for using the PathTok and PathRevTok        */
/* functions.   Just declare one and pass its address every time.       */
/*----------------------------------------------------------------------*/

#ifndef TOK_BLOCK_DEFINED
#define TOK_BLOCK_DEFINED
typedef struct
    {
    LPBYTE lpStringPos;          // Saves last match position
    BOOL   bDone;
    } TOK_BLOCK, FAR *LPTOK_BLOCK;
#endif


                                        // Callback function for DirMakePathNotify()
typedef VOID (CALLBACK* DIRMAKENOTIFYPROC) (
    LPCSTR  lpszDir,                    // [in]     Directory Created
    DWORD   dwParam,                    // [in]     Caller's luggage
    DWORD   dwReserved                  // [in]     Reserved for future use
);


// ----------------------------------------------------------------------
// PROTOTYPES
// ----------------------------------------------------------------------

#ifdef SYM_WIN32
#ifdef __cplusplus
inline UINT DirMake(LPCTSTR lpPathName)
    { return (CreateDirectory(lpPathName, NULL) ? NOERR : ERR); }
inline UINT DirRemove(LPCTSTR lpPathName)
    { return (RemoveDirectory(lpPathName) ? NOERR : ERR); }
inline UINT DirGetCurrent(LPTSTR lpDirName)
    { return (GetCurrentDirectory(SYM_MAX_PATH, lpDirName) ? NOERR : ERR); }
inline UINT DirSet(LPCTSTR lpDirName)
    { return (SetCurrentDirectory(lpDirName) ? NOERR : ERR); }
#else
#define DirMake(lpPathName) \
    (CreateDirectory(lpPathName, NULL) ? NOERR : ERR)
#define DirRemove(lpPathName) \
    (RemoveDirectory(lpPathName) ? NOERR : ERR)
#define DirGetCurrent(lpDirName) \
    (GetCurrentDirectory(MAX_PATH, lpDirName) ? NOERR : ERR)
#define DirSet(lpDirName) \
    (SetCurrentDirectory(lpDirName) ? NOERR : ERR)
#endif
#endif                                  // SYM_WIN32

#ifndef SYM_WIN32

#if defined(SYM_NLM) || defined(SYM_OS2)
        #define DirMake mkdir
        #define DirSet  chdir
#else
UINT SYM_EXPORT WINAPI     DirMake(LPCTSTR lpPathName);
UINT SYM_EXPORT WINAPI     DirSet(LPCTSTR lpDirName);
#endif

UINT SYM_EXPORT WINAPI     DirRemove(LPCTSTR lpPathName);
UINT SYM_EXPORT WINAPI     DirGetCurrent(LPTSTR lpDirName);
#endif                                  // !SYM_WIN32

#ifdef SYM_OS2
#define DirGet      getcurdir
#else
UINT SYM_EXPORT WINAPI     DirGet(TCHAR cDriveLetter, LPTSTR lpDirName);
#endif

UINT SYM_EXPORT WINAPI     DirParsePath(LPSTR path);

UINT SYM_EXPORT WINAPI     DirMakePath(LPSTR lpTopLevel);

UINT SYM_EXPORT WINAPI     DirMakePathNotify (
    LPSTR               lpTopLevel,     // [in]     Top Level Directory
    DIRMAKENOTIFYPROC   fpCallback,     // [in]     Callback function (optional)
    DWORD               dwParam         // [in]     Caller's parameter
);

BOOL SYM_EXPORT WINAPI     DirIsRemoveable(LPCSTR szDirName);
BOOL SYM_EXPORT WINAPI     DirHasChildren(LPCSTR szDirName);
UINT SYM_EXPORT WINAPI     DirParentfromPath (LPSTR lpPathBuffer, LPSTR lpParent);
UINT SYM_EXPORT WINAPI     PathRevTok(LPTOK_BLOCK lpTokBlock, LPCSTR lpszPath,
                                int nSep, LPSTR lpTok, BOOL bFirst);
UINT SYM_EXPORT WINAPI     PathTok(LPTOK_BLOCK lpTokBlock, LPCSTR lpszPath,
                                int nSep, LPSTR lpTok, BOOL bFirst);

typedef BYTE (WINAPI *BREAKFUNC)(void);
typedef void (WINAPI *PROGFUNC)(DWORD,DWORD);

#define COPY_SUCCESSFUL            NOERR
#define COPY_CONTINUE              NOERR+199
#define COPY_BREAK_PRESSED         NOERR+200
#define COPY_MEMORY_ERROR          NOERR+201
#define COPY_CRITICAL_ERROR        NOERR+202
#define COPY_DISK_IS_FULL          NOERR+203
#define COPY_DISK_NOT_FORMATTED    NOERR+204
#define COPY_DISK_WRITE_PROTECTED  NOERR+205
#define COPY_DISK_NOT_READY        NOERR+206
#define COPY_SKIPPED               NOERR+207
#define COPY_READ_ERROR            NOERR+208

#if defined(SYM_NTK)
BOOL WINAPI FileCopySecurityInformation (
    HFILE                FromObject,        // [in] from object
    HFILE                ToObject,          // [in] to object
    SECURITY_INFORMATION SecurityInformation// [in] security information
    );
#endif

#if defined(SYM_NTK) || defined(SYM_VXD)
BOOL WINAPI FileCopyHandle (
    HFILE FromFile,             // [in] from file handle
    HFILE ToFile                // [in] to file handle
    );
#endif

#if defined(SYM_WIN32)
BOOL SYM_EXPORT WINAPI FileCopySecurityInformation (
    LPCTSTR              lpFromFile,
    LPCTSTR              lpToFile,
    SECURITY_INFORMATION SecurityInformation
    );
#endif

int SYM_EXPORT WINAPI FileCopy (
    LPCTSTR fromFile,           // [in] from file path name
    LPCTSTR toFile,             // [in] to file path name
    BREAKFUNC breakCheck        // [in] break check function
);

int SYM_EXPORT WINAPI FileCopyWithProgress(
    LPCTSTR     lpFromFile,     // [in] from file path name
    LPCTSTR     lpToFile,       // [in] to file path name
    BREAKFUNC   lpBreakFunc,    // [in] break function
    PROGFUNC    lpProgFunc      // [in] progress function
);

int SYM_EXPORT WINAPI FileAppend (
    LPCSTR      fromFile,       // [in] from file path name
    LPCSTR      toFile,         // [in] to file path name
    BREAKFUNC   breakCheck      // [in] break function
);

UINT SYM_EXPORT WINAPI FileInUseByWindows (
    LPCBYTE     lpPath          // [in] path
);

UINT SYM_EXPORT WINAPI FileRename(
    LPCTSTR     lpOldName,      // [in] old file name
    LPCTSTR     lpNewName       // [in] new file name
);

UINT SYM_EXPORT WINAPI FileMove(
    LPCTSTR lpOldName,          // [in] old file name
    LPCTSTR lpNewName           // [in] new file name
);

UINT SYM_EXPORT WINAPI FileKill(
    LPCTSTR lpFileName          // [in] file name
);

UINT SYM_EXPORT WINAPI FileWipe(
    LPCTSTR lpFileName,         // [in] file name
    UINT    uTimesToWipe        // [in] number of times to overwrite the file
);


#if defined(SYM_NLM)
        #define FileDelete  unlink
#elif defined(SYM_OS2)
        #define FileDelete  DosDelete
#else

UINT SYM_EXPORT WINAPI FileDelete(
        LPCTSTR lpFileName      // [in] file name
);

#endif

#if defined(SYM_32BIT) && !defined(SYM_WIN) && !defined(SYM_VXD) && !defined(SYM_NTK)
    #define FileLength  filelength
#else
    DWORD SYM_EXPORT WINAPI    FileLength(HFILE hFileHandle);
#endif

#if defined(SYM_NTK) || defined(SYM_VXD)

UINT WINAPI FileLockRegion (
    HFILE hFile,                // [in] file handle
    DWORD dwStart,              // [in] start at
    DWORD dwLength              // [in] length
);

UINT WINAPI FileUnlockRegion (
    HFILE hFile,                // [in] file handle
    DWORD dwStart,              // [in] start at
    DWORD dwLength              // [in] length
);

#endif

#ifdef SYM_WIN32
#ifdef __cplusplus
inline DWORD FileHugeRead(HFILE hFile, void _huge *hpBuffer, DWORD dwBytes)
    { return WIN_FileRead(hFile, hpBuffer, dwBytes); }
inline DWORD FileHugeWrite(HFILE hFile, void _huge *hpBuffer, DWORD dwBytes)
    { return WIN_FileWrite(hFile, (LPSTR)hpBuffer, dwBytes); }
#else                                   // SYM_WIN32 but not __cplusplus
#define FileHugeRead(hFile, hpBuffer, dwBytes) \
    WIN_FileRead(hFile, hpBuffer, dwBytes)
#define FileHugeWrite(hFile, hpBuffer, dwBytes) \
    WIN_FileWrite(hFile, (LPSTR)hpBuffer, dwBytes)
#endif                                  // SYM_WIN32 but not __cplusplus
#else                                   // not SYM_WIN32
DWORD SYM_EXPORT WINAPI    FileHugeRead (HFILE hFile,
                                     void _huge *hpBuffer,
                                     DWORD dwBytes);
DWORD SYM_EXPORT WINAPI    FileHugeWrite(HFILE hFile,
                                     void _huge *hpBuffer,
                                     DWORD dwBytes);
#endif                                  // not SYM_WIN32


DECLARE_HANDLE(HFIND);

HFIND  SYM_EXPORT WINAPI FileFindFirst(
        LPCSTR lpFileName,              // [in] file name
        LPFINDDATA lpFindData           // [in] data to be found
);

HFIND  SYM_EXPORT WINAPI FileFindFirstAttr(
        LPCSTR lpFileName,              // [in] file name
        UINT uAttr,                     // [in] file attribute
        LPFINDDATA lpFindData           // [in] data to found
);
BOOL SYM_EXPORT WINAPI FileFindNext(
        HFIND       hFind,              // [in] find handle
        LPFINDDATA  lpFindData          // [in] date to be found
);

BOOL SYM_EXPORT WINAPI FileFindClose(
        HFIND hFind                     // [in] find handle
);

UINT SYM_EXPORT WINAPI FileGetAttr(
        LPCTSTR     lpFileName,         // [in] file name
        UINT FAR    *lpAttr             // [in/out] file attribute
);

UINT SYM_EXPORT WINAPI FileFlush(
        HFILE hFile                     // [in] file handle
);

UINT SYM_EXPORT WINAPI FileCommit(
        HFILE hFile                     // [in] file handle
);

UINT SYM_EXPORT WINAPI FileDup(
        HFILE FAR * lphDupFile,         // [in/out] duplicate file handle
        HFILE       hFile               // [in] file handle
);

#if defined(SYM_NLM)
        #define FileOpen    open
        #define FileCreate  creat
        #define FileSetAttr chmod

UINT SYM_EXPORT WINAPI NLM_FileSetAttr(
                    LPCTSTR lpFileName,     // [in] file name
                    UINT nAttributes        // [in] attributes (4 byte version)
                );
UINT SYM_EXPORT WINAPI NLM_FileGetAttr(
                    LPCTSTR lpFileName,     // [in] file name
                    UINT *nAttributes       // [out] attributes (4 byte version)
                );
#elif defined(SYM_OS2)
        #define FileOpen(filename,filemode)    open(filename, filemode | O_BINARY)
UINT SYM_EXPORT WINAPI FileCreate (LPCSTR lpFileName, USHORT wAttribute);

#else
BOOL SYM_EXPORT WINAPI FileEnableFileSharing( 
                    BOOL bEnabled    // TRUE if file sharing is OK, FALSE if not.
                 );

HFILE SYM_EXPORT WINAPI FileOpen (
                    LPCTSTR lpFileName, // [in] file name
                    UINT uMode      // [in] Mode
                 );
HFILE SYM_EXPORT WINAPI FileCreate(
                    LPCTSTR lpFileName,     // [in] file name
                    UINT uAttr              // [in] attribute
                );
UINT SYM_EXPORT WINAPI FileSetAttr(
                    LPCTSTR lpFileName,     // [in] file name
                    UINT wAttributes        // [in] attribute
                );
#endif

// DONT_DEFINE_OPENFILE works around a twisted include problem for Mario
// - file.h is sucked in from a number of places, but OpenFile absolutely
// collides with an undefined NetWare function that I must provide my own
// prototype for. DONT_DEFINE_OPENFILE is defined in mario\hooknlm\hooknlm.c

#if !defined(SYM_WIN) && !defined(SYM_VXD) && !defined(SYM_NTK) && !defined(DONT_DEFINE_OPENFILE)
HFILE SYM_EXPORT WINAPI    OpenFile (LPCSTR lpFileName, OFSTRUCT FAR *lpofstruct, UINT uMode);
#endif

UINT SYM_EXPORT WINAPI  FileSetSize(
        HFILE hFile,                    // [in] file handle
        DWORD dwSizeNeeded              // [in] size
);

HFILE SYM_EXPORT WINAPI FileCreateTemporaryPath(
        LPTSTR lpFilePath,              // [in] Path to store temp file
                                        // NOTE: NULL = use TEMP variable ("find a place")
        LPTSTR lpFileName,              // [in/out] Buffer to return temp file's name
                                        // NOTE: Can reuse lpFilePath here
        UINT   uAttributes              // [in] file attribute (use FA_ defines!!)
);

HFILE SYM_EXPORT WINAPI FileCreateTemporary(
        LPTSTR lpFileName,              // [in/out] Buffer to return temp file's name
        UINT    uAttributes             // [in] file attribute (use FA_ defines!!)
);

BOOL SYM_EXPORT WINAPI FileExists(
        LPCTSTR lpFileName              // [in] file name
);

VOID SYM_EXPORT WINAPI DOSEnvSearchPathForFile(
        LPCSTR lpFileName,              // [in] file name
        LPSTR lpRetFullPath             // [in/out] return full path
);

//************************************************************************
// Constants below are used in the 'uType' parameter to Set/Get
// dates/times on files.
//************************************************************************
#define TIME_LASTWRITE      0
#define TIME_LASTACCESS     4
#define TIME_CREATION       6

UINT SYM_EXPORT WINAPI FileGetTime(
        HFILE hFile,                // [in] file handle
        UINT uType,                 // [in] type
        LPFILETIME lpFileTime       // [in/out] time structure
);

UINT SYM_EXPORT WINAPI FileSetTime(
        HFILE hFile,                // [in] file handle
        UINT uType,                 // [in] type
        LPFILETIME lpFileTime       // [in] time structure
);

int  SYM_EXPORT WINAPI FileCmpTime(LPCSTR lpszFile1, LPCSTR lpszFile2, UINT uType);
#if defined(SYM_NLM)
UINT SYM_EXPORT WINAPI FileSetTimeByName(LPCSTR lpPath, UINT uType, LPFILETIME lpFileTime);
UINT SYM_EXPORT WINAPI FileGetTimeByName(LPCSTR lpPath, UINT uType, LPFILETIME lpFileTime);
#endif

        // routines to set the global error code for file operations

void SYM_EXPORT WINAPI   FileCheckError (void);
void SYM_EXPORT WINAPI   FileSetError (void);
void SYM_EXPORT WINAPI   FileGetReturnCode (void);
#if !defined(SYM_WIN)
VOID    CDECL   _CheckErrorFar(VOID);
VOID    CDECL   _ClearErrorFar(VOID);   // Call before an INT21 that checks
#else
#define _ClearErrorFar()                // NOP in Windows
#endif


//************************************************************************
// Only used by DOS or WIN16
//************************************************************************
VOID   SYM_EXPORT WINAPI   DTASet(LPCVOID lpDTA);
LPVOID SYM_EXPORT WINAPI   DTAGet(VOID);
#if defined(SYM_WIN16) || defined(SYM_DOS)
// Used in ASM code for calling the correct DOS entry point
#if defined(SYM_WIN16)
#define DOS     call    DOS3Call
#elif defined(SYM_DOS)
#define DOS     INT     21h
#endif
#endif

//************************************************************************
// Buffered IO routines
//************************************************************************
typedef struct
    {
    HFILE       hFile;                  // Handle of file
    HPBYTE      hpBuffer;               // Buffer
    DWORD       dwBufferSize;           // Size of buffer
    DWORD       dwCharsInBuffer;        // Actual # of chars in the buffer
    DWORD       dwBufferOff;            // Offset within buffer
    DWORD       dwBufferBase;           // Char Pos. of buffer base
    } FILE_BUFFERED, FAR *LPFILE_BUFFERED;

UINT SYM_EXPORT WINAPI FileBufferedOpen (LPFILE_BUFFERED lpfb,
                                        LPSTR lpszFilename,
                                        HPBYTE hpBuffer,
                                        DWORD dwBufferSize,
                                        UINT uMode);
HFILE SYM_EXPORT WINAPI FileBufferedClose(LPFILE_BUFFERED lpfb);
int SYM_EXPORT WINAPI FileBufferedGetChar(LPFILE_BUFFERED lpfb);
int SYM_EXPORT WINAPI FileBufferedReverseGetChar(LPFILE_BUFFERED lpfb);
VOID SYM_EXPORT WINAPI FileBufferedUngetChar(LPFILE_BUFFERED lpfb);
DWORD SYM_EXPORT WINAPI FileBufferedSetPtr(LPFILE_BUFFERED lpfb, DWORD dwNewPtr);
DWORD SYM_EXPORT WINAPI FileBufferedGetPtr(LPFILE_BUFFERED lpfb);
DWORD SYM_EXPORT WINAPI FileBufferedLength(LPFILE_BUFFERED lpfb);
int SYM_EXPORT WINAPI FileBufferedGetLine(
    LPFILE_BUFFERED pfb,                // [in] buffered file struct to read from
    LPSTR lpszLineBuffer,               // [in/out] Buffer to receive line
    UINT uLineBufferSize,               // [in] Size in BYTES of the buffer
    BOOL bFilter);                      // [in] map unprintable chars to '.'


#if defined(SYM_DOS)

//-----------------------------------------------------------------------
// Temporary file classes.  Used by FileCreateTemp() to create temp files
// in the appropriate directory
//-----------------------------------------------------------------------

#define TEMP_CLASS_SWAP         0       // VM Swap file
#define TEMP_CLASS_SAVE         1       // VM Save file
#define TEMP_CLASS_OVERLAY      2       // Code overlay swap area
#define TEMP_CLASS_TEMP         3       // Other temp files
#define TEMP_CLASS_SPECIFIED    4       // Use path specified in input name

UINT PASCAL FileCreateTemp(BYTE byClass, BYTE *pszRoot, BYTE *pszVar, DWORD dwSize, BYTE *pszFile);
void PASCAL FileReturnTempDir(BYTE byClass, BYTE *pszVar, BYTE *pszDirName);

extern  UINT NEAR       error;

// Directory Module

typedef void (*DISPLAYFUNC)(const BYTE *);

#define RMTREE_OK                       0
#define RMTREE_ABORT                    1
#define RMTREE_ERROR                    2

#define COPY_COMPLETE                   0
#define COPY_CANT_CREATE_DIR            1

#define MVTREE_COMPLETE                 0
#define MVTREE_ERR_CANT_CREATE_DIR      1
#define MVTREE_ERR_CANT_RENAME          2
#define MVTREE_ERR_CANT_REMOVE_DIR      3

BYTE    PASCAL  DirSearchPath (BYTE *foundPath, const BYTE *fileName);
BYTE    PASCAL  DirSearchSymantecPath (const BYTE *pszVar, const BYTE *file, BYTE *fullName);
void    PASCAL  DirNewSymantecPath (const BYTE *pszVar, const BYTE *pszSearch, BYTE *fullName, const BYTE *name);
BYTE    PASCAL  DirSearchProductPath (const BYTE *pszVar, const BYTE *file, BYTE *fullName);
void    PASCAL  DirNewProductPath (const BYTE *pszVar, const BYTE *pszSearch, BYTE *fullName, const BYTE *name);
BYTE    PASCAL  DirSearchProgramPath (const BYTE *pszProductVar, const BYTE *file, BYTE *fullName);
int     PASCAL  DirRemoveTree(const BYTE *path, DISPLAYFUNC displayFunc, BREAKFUNC breakFunc);
int             DirCopyTree(LPCSTR source, LPCSTR dest, DISPLAYFUNC displayFunc, BREAKFUNC breakFunc);
int             DirMoveTree(const BYTE *source, const BYTE *dest, DISPLAYFUNC displayFunc, BREAKFUNC breakFunc);
DWORD           DirSize(const BYTE *path, DWORD *clusters, UINT clusterSize);
DWORD           DirTreeSize(const BYTE *path, DWORD *clusters, UINT clusterSize);
UINT    PASCAL  DirGetLongestPath (const BYTE *sourceDir, const BYTE *destDir);

int     PASCAL  FCBParse (BYTE *fileName, void *FCBPtr);

BYTE    CDECL   FilePrint(HFILE wHandle, const BYTE *pszFormat, ...);
int     PASCAL  FileCreateUnique (BYTE *fileName, UINT attributes);
int     CDECL   FileDeleteFCB(const void *FCBPtr);
DWORD   PASCAL  FileSpecSize(const BYTE *filespec);
void    PASCAL  FileCreateUniqueName (BYTE *pszFileName, const BYTE *pszPrefix, const BYTE *pszExt);
BYTE    PASCAL  FileGetTextLine(int nFileHandle, BYTE * szLineBuffer, UINT wMaxLength);
UINT    PASCAL  FileIsWindowsEXE (const BYTE *pszPathname);


// File Icon Module

void PASCAL IconGet(const BYTE *pathName, BYTE *iconLeft, BYTE *iconRight);
void PASCAL IconGetPacked(const BYTE *pathName, BYTE *iconLeft, BYTE *iconRight);
void PASCAL IconGetFromExtension(const BYTE *pszExtension, BYTE *iconLeft, BYTE *iconRight);
void PASCAL IconGetDisk(BYTE dl, BYTE *iconPieces);
void PASCAL IconGetDiskType(UINT wMajorDriveType, UINT wMinorDriveType, BYTE *iconPieces);
void PASCAL IconGetOldDiskType(UINT wMajorDriveType, BYTE *iconPieces);
void PASCAL VDiskIconGetDisk(struct VDISKREC *pVDisk, BYTE dl, BYTE *iconPieces);

#endif  // NOOLDSYMANTEC


// NDOS Description Module

#define MAX_NDOS_DESCR_LEN      40

#if defined(SYM_DOS)
BOOL PASCAL DescriptionGet(struct VDISKREC * pVDiskRec, LPCSTR lpszFileName, LPSTR lpszDescription);
BOOL PASCAL DescriptionPut(struct VDISKREC * pVDiskRec, LPCSTR lpszFileName, LPCSTR lpszDescription);
#endif  // NOOLDSYMANTEC

UINT SYM_EXPORT WINAPI NDOSDescriptionGet (LPCSTR lpszFileName, LPSTR lpszFileDesc);
UINT SYM_EXPORT WINAPI NDOSDescriptionPut (LPCSTR lpszFileName, LPCSTR lpszFileDesc);

#if defined(SYM_NTK)
HANDLE FileOpenRootHandle ( LPCTSTR lpszName );
VOID FileCloseRootHandle ( HANDLE hHandle );
LPCTSTR FileRelativeToRootName ( LPCTSTR lpszName );
UINT FileCopyRootName ( LPCTSTR lpszName, LPTSTR lpszDest );
PTSTR FileCreateNativeNTName ( LPCTSTR lpszName );
PTSTR FileCreateNativeNTNameEx ( LPCTSTR lpszName );
VOID FileDestroyNativeNTName ( PTSTR pszName );
#endif

#endif  // !defined(SYM_UNIX)

#ifdef __cplusplus
}
#endif

#endif  // INCL_FILE

