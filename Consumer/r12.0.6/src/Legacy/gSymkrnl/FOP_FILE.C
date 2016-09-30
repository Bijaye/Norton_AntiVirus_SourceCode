/* Copyright 1992 Symantec Corporation                                  */
/************************************************************************
 *                                                                      *
 * $Header:   S:/SYMKRNL/VCS/FOP_FILE.C_v   1.52   18 Aug 1998 16:49:48   mdunn  $ *
 *                                                                      *
 * Description:                                                         *
 *                                                                      *
 * Contains:                                                            *
 *      DOSEnvSearchPathForFile                                         *
 *      FileInUseByWindows                                              *
 *      FileCreate  (OS/2 only                                          *
 *      FileCreateWorker (SYM_NTK only)                                 *
 *      FileDelete                                                      *
 *      FileRename                                                      *
 *      FileLength                                                      *
 *      FileGetAttr                                                     *
 *      FileSetAttr                                                     *
 *      FileGetTime                                                     *
 *      FileSetTime                                                     *
 *      FileFlush                                                       *
 *      FileCommit                                                      *
 *      FileDup                                                         *
 *      FileOpen                                                        *
 *      FileSetSize                                                     *
 *      FileHugeRead                                                    *
 *      FileHugeWrite                                                   *
 *      FileCopy                                                        *
 *      FileCopyHandle                                                  *
 *      FileCopySecurityInformation                                     *
 *      OpenFile (for non-Windows platforms)                            *
 *      FileOpenRootHandle (NT Kernel mode/Native API)                  *
 *      FileCloseRootHandle (NT Kernel mode/Native API)                 *
 *      FileRelativeToRootName (NT Kernel mode/Native API)              *
 *      FileCopyRootName (NT Kernel mode/Native API)                    *
 *      FileCreateNativeNTName (NT Kernel mode/Native API)              *
 *      FileCreateNativeNTNameEx (NT Kernel mode/Native API)            *
 *      FileDestroyNativeNTName (NT Kernel mode/Native API)             *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * $Log:   S:/SYMKRNL/VCS/FOP_FILE.C_v  $ *
// 
//    Rev 1.52   18 Aug 1998 16:49:48   mdunn
// Ported Atomic changes from N branch
// 
//    Rev 1.51   30 Jul 1998 21:03:12   dhertel
// Added NLM implementations of FileRead, FileWrite
// 
//    Rev 1.50   29 Jul 1998 15:38:10   dhertel
// Added some NLM function implementations from rev 1.45.1
// 
//    Rev 1.49   01 Jun 1998 22:59:42   RStanev
// Fixed a typo.
//
//    Rev 1.48   01 Jun 1998 22:51:30   RStanev
// Correctly work with file attributes in NT kernel mode.
//
//    Rev 1.47   05 Mar 1998 20:20:38   dhertel
// Changed an assert in FileSetAttr
//
//    Rev 1.46   03 Mar 1998 17:36:16   RFULLER
// Updated Trunk with the correct version of FOP_FILE.C
//
//    Rev 1.41.1.2   30 Dec 1997 16:00:06   CEATON
// Modified FileOpen() so that file sharing can be enabled and disabled.
//
//
//    Rev 1.41.1.1   13 Aug 1997 22:34:48   RStanev
// Fixed FileCreate() for SYM_NTK to use FILE_CREATE instead of CREATE_NEW.
//
//    Rev 1.41.1.0   12 Aug 1997 22:52:44   hkim
// Branch base for version QAKK
//
//    Rev 1.41   06 Aug 1997 14:33:00   RStanev
// Fixed a SYM_ASSERT() in FileCrate().
//
//    Rev 1.40   04 Aug 1997 09:59:04   BGERHAR
// Added FileCreateTemporaryPath() to control where temp files are created
//
//    Rev 1.39   02 Jul 1997 16:36:52   MKEATIN
// Don't free our buffer too soon in FileWipe().
//
//    Rev 1.38   20 Jun 1997 23:45:46   MKEATIN
// Made a fix to FileWipe().
//
//    Rev 1.37   20 Jun 1997 22:39:32   MKEATIN
// Added FileWipe().
//
//    Rev 1.36   16 Jun 1997 15:56:00   RStanev
// Changed the ZwCreateFile() call in FileDelete() to accept FILE_OVERWRITE
// instead of FILE_OPEN.  This solves a problem in protected floppy disks
// where the OS would crash if the file was not able to be truncated to
// 0 length before the close.
//
//    Rev 1.35   04 Jun 1997 10:28:44   RStanev
// SYM_NTK now understands filenames starting with "\??\".
//
//    Rev 1.34   03 Jun 1997 14:27:48   RStanev
// Enhanced FileCreate() for SYM_NTK to support uAttr flag 0x80000000.
//
//    Rev 1.33   25 May 1997 20:31:22   RStanev
// Added FileCreateNativeNTNameEx() and changed FileOpen() for SYM_NTK to
// use it instead of FileCreateNativeNTName().
//
//    Rev 1.32   25 Feb 1997 23:35:06   RSTANEV
// Allowed FileCopyHandle() to compile for SYM_VXD.
//
//    Rev 1.31   27 Oct 1996 17:11:38   RSTANEV
// Fixed a FileExists() SYM_NTK return code.
//
//    Rev 1.30   18 Oct 1996 22:04:52   RSTANEV
// Limited the usage of root handles in SYM_NTK.
//
//    Rev 1.29   29 Aug 1996 17:59:54   DHERTEL
// Copied changes from Quake E:
// Added FileCopySecurityInformation for the SYM_WIN32 platform.
//
//    Rev 1.28   29 Aug 1996 10:30:48   BMCCORK
// Fixed compile error (oops)
//
//    Rev 1.27   28 Aug 1996 11:34:20   MZAREMB
// A renamed function parameter caused a problem for the DX platform. Now fixed.
//
//    Rev 1.26   27 Aug 1996 13:46:46   BMCCORK
// Added conversion of FA_ flags to FILE_ATTRIBUTE_ flags in CreateTemporaryFile
//
//    Rev 1.25   13 Aug 1996 22:17:36   RSTANEV
// SYM_NTK: Using FILE_SHARE_DELETE flag in certain cases when calling
// ZvOpenFile() and ZvCreateFile().
//
//    Rev 1.24   31 Jul 1996 14:06:04   SKURTZ
// W32: In __FileCopy(), FileCreate returns an error that is a DWORD value. ERR is defined as (WORD)-1
//
//    Rev 1.23   24 Jul 1996 14:55:58   RSTANEV
// The new implementation of FileDelete() for SYM_NTK was calling ZwClose()
// instead of ZvClose().  This was messing up the debug code counting open
// file handles.
//
//    Rev 1.22   23 Jul 1996 14:42:04   RSTANEV
// Changed FileDelete() for SYM_NTK platform to use ZwCreateFile()
// instead of ZwDelteFile().  This allows us to probe for oplocks and
// fail to delete oplocked files.
//
//    Rev 1.21   22 Jul 1996 18:24:40   RSTANEV
// SYM_NTK: Using the NT_SUCCESS() macro.
//
//    Rev 1.20   22 Jul 1996 17:44:12   RSTANEV
// FileCreate() for SYM_NTK checks for operation under NT AUTHORITY.
//
//    Rev 1.19   19 Jul 1996 16:49:54   BMCCORK
// Made the Win32 implementations of get/setattr convert from FA_ defines
//
//    Rev 1.18   18 Jul 1996 12:06:54   RSTANEV
// Added extra error checking in FileOpen() for SYM_NTK platform.
//
//    Rev 1.17   10 Jul 1996 10:33:02   BMCCORK
// Added asserts() on wAttributes for Win32 version of FileSetAttr()
//
//    Rev 1.16   24 Jun 1996 19:03:40   RSTANEV
// Now using the Zv...() wraps instead of the Zw...() APIs.
//
//    Rev 1.15   18 Jun 1996 10:55:28   RSTANEV
// Fixed some of the flags passed to ZwCreateFile() and ZwOpenFile() in
// SYM_NTK.
//
//    Rev 1.14   12 Jun 1996 21:40:04   RSTANEV
// FileSetAttr() for SYM_NTK now forces the FILE_ATTRIBUTE_NORMAL flag
// when calling NtSetInformationFile().  This protects us from clients
// passing 0 as wAttributes, which for some reason NTOSKRNL doesn't
// complain about - it just leaves file's attributes unchanged.
//
//    Rev 1.13   23 May 1996 15:54:30   BMCCORK
// Added create functionality to FileOpen()
//
//    Rev 1.12   23 May 1996 09:05:30   RSTANEV
// Changed the code to transfer KeGetPreviousMode() to KernelMode before
// calling NtSetSecurityObject().  This is because the current process
// or thread may not have enough rights to perform the operation.
//
//    Rev 1.11   22 May 1996 16:22:34   RSTANEV
// Added FileCopyHandle().  Also, switched to using NtSetSecurityObject()
// instead of ZwSetSecurityObject().
//
//    Rev 1.10   22 May 1996 15:42:20   RSTANEV
// __FileCopy for SYM_NTK now transfers DACL and SACL from the source to
// the destination.
//
//    Rev 1.9   22 May 1996 15:21:58   RSTANEV
// Added FileCopySecurityInformation().
//
//    Rev 1.8   22 May 1996 14:28:36   RSTANEV
// SYM_NTK: Changed FileCreate() to set the owner of the new files to
// the Administrators group if the target FS is local to the system and
// if it supports permanent ACLs.
//
//    Rev 1.7   15 May 1996 14:56:50   SSAWKIN
// Fixed problem in _FileCopy on DOS platform
//     FileCopy would fail with COPY_MEMORY_ERROR for X86 due to call to
//     MemAvail(=VMMAvail for DOS) which always returned 0 as the size of
//     the largest free block. Fix is to avoid the MemAvail call for DOS
//     and change the default allocation(MAX_BUFFER_SIZE) to 0x1000 or
//     4K for DOS/X86 only.
//
//    Rev 1.6   11 Apr 1996 16:20:30   RSTANEV
// Corrected a buffer size in FileRename() for SYM_NTK.
//
//    Rev 1.5   11 Apr 1996 13:34:44   RSTANEV
// Fixed a problem in FileOpenRootHandle() where we had to fail the
// function if the passed UNC filename was invalid, and instead we
// trashed random memory.
//
//    Rev 1.4   06 Apr 1996 09:28:36   RSTANEV
// SYM_NTK changes: Fixed an open-file-handle leak in SYM_NTK;  FileLength()
// does not call FileSeek() anymore.
//
//    Rev 1.3   09 Feb 1996 18:45:24   RSTANEV
// Restored back to use Zw... functions.
//
//    Rev 1.2   08 Feb 1996 15:39:54   DHERTEL
// Changed FileHugeRead and FileHugeWrite to avoid problems with huge DOSX
// buffers that have non-contiguous 64K segments.
//
//    Rev 1.1   30 Jan 1996 15:22:18   RSTANEV
// Added Unicode and SYM_NTK support for the FileCopy() functions.
//
//    Rev 1.0   26 Jan 1996 20:22:46   JREARDON
// Initial revision.
//
//    Rev 1.118   24 Jan 1996 20:14:48   RSTANEV
// Added FileCopyRootName() and extra parameter checking in some functions.
//
//    Rev 1.117   19 Jan 1996 17:44:08   RSTANEV
// OpenRootHandle() does not need locked memory.
//
//    Rev 1.116   19 Jan 1996 16:35:38   RSTANEV
// More SYM_NTK changes and added Unicode support.
//
//    Rev 1.115   05 Jan 1996 17:19:42   RSTANEV
// Cleaned up SYM_NTK platform.
//
//    Rev 1.114   03 Jan 1996 15:53:06   RSTANEV
// Fixed a stupid FileRename() bug in SYM_NTK platform.
//
//    Rev 1.113   22 Dec 1995 17:43:12   RSTANEV
// Now using Nt... APIs instead of Zw... APIs.  Also, added code for
// FileRename() but NtSetInformationFile() still returns error message.
//
//    Rev 1.112   08 Dec 1995 19:04:42   RSTANEV
// Added SYM_NTK platform support.
//
//    Rev 1.111   09 Nov 1995 16:38:16   GDZIECI
// Implementd FileGetTime() for NLM. Added FileGetTimeByName() and
// FileSetTimeByName() - NLM platform only.
//
//    Rev 1.110   21 Jun 1995 13:46:14   PGRAVES
// Cleanup in _FileCopy.
//
//    Rev 1.109   19 Apr 1995 20:15:06   MFALLEN
// Clear internal error indicator when entering _FileCopy().
//
//    Rev 1.108   20 Feb 1995 13:54:38   DOUGN
// Changed searchpath code to not blow away dest buffer past it's size.
//
//
//    Rev 1.107   02 Feb 1995 16:37:36   BRAD
// Set ECX to 0, for FileOpen to get around MS bug
//
//    Rev 1.106   13 Dec 1994 11:18:48   SZIADEH
// This is really Brad.  Fixed W32 problem
//
//    Rev 1.105   12 Dec 1994 17:18:24   BRAD
// Added VxD support
//
//    Rev 1.104   11 Nov 1994 11:26:54   TONY
// Added FileCreate() for OS/2
//
//    Rev 1.103   09 Nov 1994 16:41:56   BRAD
// DBCS and NEC-enabled
//
//    Rev 1.102   01 Nov 1994 13:31:56   JMILLARD
//
// pick up some functions for NLM platform
//
//    Rev 1.101   23 Sep 1994 15:37:06   TONY
// OS/2 port work from 4 to 6
//
//    Rev 1.100   08 Sep 1994 15:10:28   MARKK
// Commented out OutpytDebugString
//
//    Rev 1.99   14 Jul 1994 18:16:44   BRAD
// They added a parameter to the extended file delete.  Can now have wildcards.
// We disable this feature.
//
//    Rev 1.98   12 Jul 1994 21:39:28   PGRAVES
// Exported EnvironmentSubst (Win32).
//
//    Rev 1.97   28 Jun 1994 08:41:08   MFALLEN
// Cleared internal error indicator for the FileCopy() functions so we don't
// inherit any error from previously failed operation.
//
//    Rev 1.96   03 Jun 1994 15:46:32   MFALLEN
// As FileAppend is called clear the internal error variable.
//
//    Rev 1.95   01 Jun 1994 13:13:42   MARKK
// Don't make extended calls under DX
//
//    Rev 1.94   24 May 1994 13:31:12   MARKK
// Rational doesn't handle extended calls well
//
//    Rev 1.93   23 May 1994 15:33:20   MARKK
// File DOSX problem with FileOpen
//
//    Rev 1.92   11 May 1994 11:00:36   MFALLEN
// FileHugeRead() was busted. It was causing GPF's since the bytes read from the
// file was exceeding the bytes specified by the application.
//
//    Rev 1.91   05 May 1994 13:16:02   BRAD
// DX support
//
//    Rev 1.90   04 May 1994 16:06:32   MARKK
// Fixed simple errors
//
//    Rev 1.89   04 May 1994 15:11:12   MFALLEN
// Had a typo.
//
//    Rev 1.88   04 May 1994 14:14:56   BRAD
// Need to cast return value from CreateFile() to HFILE
//
//    Rev 1.87   04 May 1994 09:03:42   MARKK
// Don't do LDS on an array!
//
//    Rev 1.86   04 May 1994 08:48:52   MARKK
// Don't do LDS on an array!
//
//    Rev 1.85   04 May 1994 00:11:06   BRAD
// Don't use NFileOpen()
//
//    Rev 1.84   04 May 1994 00:04:20   BRAD
// Protect against NULL being passed to ExpandAndConvertFile()
//
//    Rev 1.83   03 May 1994 22:56:10   BRAD
// Remove functions NFileOpen() and NFileCreate()
//
//    Rev 1.82   26 Apr 1994 17:51:22   BRAD
// Use SYM_MAX_PATH only
//
//    Rev 1.81   20 Apr 1994 19:41:42   BRAD
// Buffer being passed to ExpandAndConvertFile() is smaller than file buffer.  Would GPF
//
//    Rev 1.80   18 Apr 1994 11:21:28   BRAD
// Added FileDup() for WIN32
//
//    Rev 1.79   08 Apr 1994 17:06:58   BRAD
// High bit is 8, not 1
//
//    Rev 1.78   08 Apr 1994 16:58:00   BRAD
// For Win32s, EnvironmentSubst() is a NOP
//
//    Rev 1.77   08 Apr 1994 16:30:22   BRAD
// ExpandEnvironmentStrings() not supported in Win32s.  Need to dynalink
// to DoEnvironmentSubst().
//
//    Rev 1.76   06 Apr 1994 17:04:44   BRAD
// We weren't popping DS off stack in DOS's FileGetAttr()
//
//    Rev 1.75   31 Mar 1994 13:44:54   BRAD
// Some of the new extended functions weren't setting STC
//
//    Rev 1.74   30 Mar 1994 16:09:22   BRAD
// In EnvironmentSubst(), we were passing parameters incorrectly.
//
//    Rev 1.79   28 Mar 1994 20:46:58   BRAD
// 'Dangling' elif
//
//    Rev 1.78   28 Mar 1994 18:14:30   BRAD
// Fixed up for NLM
//
//    Rev 1.77   28 Mar 1994 13:33:40   BRAD
// Fixed typo
//
//    Rev 1.76   28 Mar 1994 13:21:00   BRAD
// Added EnvironmentSubst()
//
//    Rev 1.75   26 Mar 1994 14:35:34   BRAD
// Changed DATETIME_ to TIME_
//
//    Rev 1.74   26 Mar 1994 12:42:00   BRAD
// Use FILETIME for setting/getting
//
//    Rev 1.73   26 Mar 1994 12:29:02   BRAD
// Changed file date/time, so takes FILETIME parameter
//
//    Rev 1.72   23 Mar 1994 20:13:10   BRAD
// Cleaned up some compile warnings
//
//    Rev 1.71   23 Mar 1994 19:41:42   BRAD
// Added LFN support for DOS and WIN16
//
//    Rev 1.70   23 Mar 1994 16:27:48   BRAD
// Cleaned up for new FindFirst handle stuff
//
//    Rev 1.69   22 Mar 1994 10:43:48   BRUCE
// Fixed #pragmas and #if/#endif matching
//
//    Rev 1.68   21 Mar 1994 20:02:24   BRAD
// Made more routines visible to WIN32
//
//    Rev 1.67   21 Mar 1994 15:38:36   BRUCE
// Cleaned up pragmas and other compile errors
//
//    Rev 1.66   21 Mar 1994 15:15:36   BRAD
// Move pragmas outside of functions
//
//    Rev 1.65   21 Mar 1994 12:37:20   BRAD
// FileKill is needed for WIN32
//
//    Rev 1.64   21 Mar 1994 00:20:10   BRAD
// Cleaned up for WIN32
//
//    Rev 1.63   15 Mar 1994 12:32:20   BRUCE
// Changed EXPORT to SYM_EXPORT
//
//    Rev 1.62   25 Feb 1994 15:04:30   BRUCE
// Reinstated FP_SEG, FP_OFF, MAKELP
//
//    Rev 1.61   25 Feb 1994 12:22:22   BRUCE
// Ran scripts to change to SYM_ constants and get ready for Win32
//
//    Rev 1.60   17 Feb 1994 22:52:54   PGRAVES
// Win32.
//
//    Rev 1.59   14 Feb 1994 13:25:46   PGRAVES
// Win32 changes, QAK2 to trunk.
//
//    Rev 1.58   03 Feb 1994 21:40:50   PGRAVES
// Win32 change: FileExists is WINAPI.
//
//    Rev 1.57   02 Feb 1994 19:06:22   PGRAVES
// Changed WIN32 to SYM_WIN32.
//
//    Rev 1.56   01 Feb 1994 17:30:06   BRUCE
// Added win32 platform code
//
//    Rev 1.55   28 Jan 1994 11:09:00   TONY
// Removed dos-specific error resetting under OS/2
//
//    Rev 1.54   28 Jan 1994 10:34:44   TONY
// Added ExpandAndConvertFile conversion for OS/2
//
//    Rev 1.53   27 Jan 1994 19:13:30   TONY
// OS/2 conversion of routines needed for Anvil's NAINV
//
//    Rev 1.52   15 Dec 1993 08:37:50   BARRY
// Merge changes from QAK2
//
//    Rev 1.46.1.4   08 Dec 1993 12:11:42   BARRY
// Now tries harder to get a buffer in _FileCopy()
//
//    Rev 1.51   08 Dec 1993 17:30:52   DAVID
// The DOS version of FileOpen() now sets the nErrCode field of the OFSTRUCT.
//
//    Rev 1.50   29 Nov 1993 09:54:26   BARRY
// Change prototype for FileCmpDateTime() to match FILE.H (EXPORT WINAPI)
//
//    Rev 1.49   29 Nov 1993 09:48:26   BARRY
// Merge QAK2 changes
//
//    Rev 1.46.1.3   19 Nov 1993 15:17:10   BARRY
// Removed trapping_enabled control from FileCmpDateTime()
//
//    Rev 1.46.1.2   18 Nov 1993 06:50:40   BARRY
// Disable trapping_enabled handling for SYM_WIN
//
//    Rev 1.46.1.1   17 Nov 1993 17:59:32   BARRY
// Added FileCmpDateTime()
//
//    Rev 1.46.1.0   19 Oct 1993 09:30:46   SCOTTP
// Branch base for version QAK2
//
//    Rev 1.46   13 Oct 1993 17:42:16   MFALLEN
// DiskGetClusterSize() now returns a DWORD
//
//    Rev 1.45   13 Oct 1993 15:25:48   MFALLEN
// From Quake branch 1:
//
// -       Added Barry's _ClearErrorFar() function calls, and
// -       Added Brad's changes to the FileCreateTemporary()
//
//    Rev 1.44   09 Sep 1993 16:25:46   ED
// Allow the second parameter of FileGetAttr to be NULL.
//
//    Rev 1.43   28 Aug 1993 15:14:38   JMILLARD
// fix warning from last change for SYM_WIN
//
//    Rev 1.42   28 Aug 1993 14:54:12   BRAD
// Expanded FileCreateTempororary() for DOS, such that it uses the environment
// variables, TMP or TEMP.  If not found, then will use the root directory of
// C:.
//
//    Rev 1.41   19 Jul 1993 23:03:40   MFALLEN
// FileExists() modified to return TRUE only if files.
//
//    Rev 1.40   02 Jul 1993 08:52:00   DOUGN
// Change for conversion to C8.  Moved "include <dos.h>" to be above the
// "include "platform.h"" line so that MK_FP would be defined by the
// C8 dos.h include file and platform.h wouldn't redefine it and cause
// warnings.
//
//
//    Rev 1.39   18 Jun 1993 13:23:18   EHU
// Don't include shellapi.h for SYM_NLM.
//
//    Rev 1.38   17 Jun 1993 19:56:32   MARKL
// SHELL.DLL is now dynamically loaded by SymKrnl.  The rouitines RegOpenKey(),
// RegCloseKey(), RegQueryValue(), and DoEnvironmentSubst() are now called
// through function pointers.  In the case of DOS, a function pointer has
// been established for DoEnvironmentSubst() to allow consistant code in both
// the Windows and DOS platforms.
//
//    Rev 1.37   08 Jun 1993 07:28:04   DDREW
// Function ED added needed to be within indef SYM_NLM or 'caused SYM_NLM complie error
//
//    Rev 1.36   07 Jun 1993 08:42:04   ED
// [Enh] Added FileKill, the FileDelete function that refuses to give up.
// It's the Terminator of file management, morphing to a town near you.
//
//    Rev 1.35   28 May 1993 11:57:00   MARKL
// FileCreateTemporary() now uses the Windows function GetTempFileName()
// since the concept of the current working directory doesn't really
// work in Windows Apps too well.  This will guarentee that we have a
// fully qualified path to the temporary file that was created.
//
//    Rev 1.34   27 May 1993 16:44:28   JMILLARD
// build FileSetSize for SYM_NLM
//
//    Rev 1.33   19 May 1993 19:12:26   BRAD
// Added FileExists().
//
//    Rev 1.32   11 May 1993 14:13:52   DALLEE
//
//    Rev 1.31   11 May 1993 13:58:58   DALLEE
// Turned off global optimizations around inline _asm.
//
//    Rev 1.30   03 May 1993 19:22:14   MFALLEN
// Fixed problem with FileHugeWrite()
//
//    Rev 1.29   02 May 1993 14:42:02   ENRIQUE
// Added FileCreateTemporary.  This function used int 21h 5Ah.
//
//    Rev 1.28   27 Apr 1993 21:10:00   BRAD
// Now uses new MAX_* values.
//
//    Rev 1.27   21 Apr 1993 17:50:28   DLEVITON
// Put OF_SHARE_... flags in all FileOpens to avoid share errors.
//
//
//    Rev 1.26   19 Apr 1993 14:43:08   DLEVITON
// Copying an empty file no longer gives a memory allocation error.
//
//
//    Rev 1.25   25 Mar 1993 18:54:36   BRAD
// No change.
//
//    Rev 1.24   25 Mar 1993 18:54:14   BRAD
//
//    Rev 1.23   07 Mar 1993 15:40:34   BRAD
// Modified FileHugeRead/FileHugeWrite, when amount read/written
// isn't same as amount requested.
//
//    Rev 1.22   23 Feb 1993 17:55:32   BRAD
// FileOpen() wasn't searching the path of the executable
// file itself for the file.  It now works in the same
// order as Windows' OpenFile().
//
//    Rev 1.21   21 Feb 1993 19:19:56   BRAD
// FileOpen() didn't work for DOS at all.  Problem was the
// checking of values from SearchPath().
//
//    Rev 1.20   21 Feb 1993 18:31:14   BRAD
// Fixed up STRNCPY() problems, where wasn't terminating with EOS.
//
//    Rev 1.19   21 Feb 1993 18:00:28   BRAD
// Cleaned up some STRNCPY() routines.
//
//    Rev 1.18   21 Feb 1993 00:21:06   BRAD
// Added FileHugeRead() and FileHugeWrite().
//
//    Rev 1.17   15 Feb 1993 21:04:34   TORSTEN
// Fixup for -W3/-WX.
//
//    Rev 1.16   15 Feb 1993 08:25:40   ED
// No change.
//
//    Rev 1.15   12 Feb 1993 04:28:50   ENRIQUE
// No change.
//
//    Rev 1.14   09 Feb 1993 13:29:22   DOUGN
// Type-cast of assignment to wRet = ERR to be wRet = (UINT)ERR.
//
//
//    Rev 1.13   08 Feb 1993 08:39:18   ED
// Added FileMove, a substitute for FileRename in cases where the
// new and old names may not be on the same drive (in which case,
// FileRename will not work!)
//
//    Rev 1.12   11 Jan 1993 15:50:10   CRAIG
// Scorpio update: FileCopyWithProgress(), COPY_DISK_IS_FULL error.
//
//    Rev 1.11   31 Dec 1992 10:50:52   ED
// Fixed some reversed if statements
//
//    Rev 1.10   30 Dec 1992 22:00:46   JOHN
// Changed to use Quake MemAvail() rather than Windows GetFreeSpace().
// This code is also used on DOS!
//
//    Rev 1.9   30 Dec 1992 17:24:34   ED
// Added FileCopy and FileAppend
//
//    Rev 1.8   23 Nov 1992 17:04:04   DOUGN
// Fixed function prototype for Search() (changed LPSTR to LPCSTR)
//
//    Rev 1.7   20 Oct 1992 09:36:04   ED
// ASSEMBLY TRICK #12: If you push DS, pop it later for best performance
//
//    Rev 1.6   19 Oct 1992 18:31:06   BILL
// const changes
//
//    Rev 1.5   15 Sep 1992 09:59:20   ED
// Removed NSTRING.H
//
//    Rev 1.4   10 Sep 1992 13:43:58   BRUCE
// Eliminated unnecessary includes
//
//    Rev 1.3   04 Sep 1992 12:47:14   ED
// Fixed a _CheckError problem
//
//    Rev 1.2   03 Sep 1992 08:19:42   ED
// No change.
//
//    Rev 1.1   03 Sep 1992 08:14:14   ED
// Added functions to process error codes consistently
//
//    Rev 1.0   03 Sep 1992 08:02:46   ED
// Initial revision.
//
//    Rev 1.0   27 Aug 1992 09:35:08   ED
// Initial revision.
 ************************************************************************/

#include <dos.h>
#include "platform.h"
#include <limits.h>
#include "xapi.h"
#include "file.h"
#include "disk.h"
#include "symkrnl.h"
#include "undoc.h"

#ifdef SYM_OS2
#   include <stdos2.h>
#   include <io.h>
#   include <errno.h>
    extern int errno;
#endif

#ifdef SYM_NLM
#include <io.h>                         // seek, write for SYM_NLM file covers
#include <nwdir.h>                      // ChangeDirectoryEntry()
#include <nwfile.h>
#include <direct.h>
#endif

MODULE_NAME;


/*--------------------------------------------------------------------------*/
/*                                                                          */
/*      Contains file functions that are not supplied with Windows.         */
/*                                                                          */
/*  NOTE: All input MUST be in ANSI.  The routines here which directly      */
/*        interface with DOS will upper case and convert to OEM before      */
/*        passing to DOS.  All output returned from DOS will be converted   */
/*        back to ANSI before being returned to the caller.                 */
/*                                                                          */
// NOTE ** NOTE ** NOTE ** NOTE
// The functions in this file call NDosSetError() when they get a DOS error
// The error may be retrieved using NDosGetError()
/*--------------------------------------------------------------------------*/

#if !defined(SYM_OS2) && !defined(SYM_VXD)
static BOOL bFileSharing = TRUE;
#endif

#ifndef SYM_NLM

#if !defined(SYM_NTK)
UINT    LOCAL PASCAL _SearchPath (LPCSTR lpFileName, LPSTR lpBuffer, UINT wMaxSize);
#endif

//----------------------------------------------------------------------
// FileInUseByWindows
//    This function is used to check if a file is in use by windows.
//
// INPUTS
//    LPBYTE  lpPath    long pointer to the file we want to check
//
// RETURNS
//   TRUE if the file is in use
//   FALSE if the file is not in use
//----------------------------------------------------------------------
#if !defined(SYM_WIN32) && !defined(SYM_OS2) && !defined(SYM_NTK)
UINT SYM_EXPORT WINAPI FileInUseByWindows (LPCBYTE lpPath)
{
#ifdef  SYM_WIN
    auto        HANDLE          hModule = NULL;
    auto        BYTE            szPathBuffer[SYM_MAX_PATH];

    //-----------------------------------------------------------------
    // We need to check if we are running in windows 3
    // because get GetModuleHandle can't handle path length that
    // are greater than 64
    //-----------------------------------------------------------------
    if (LOWORD(GetVersion()) > 3)          // If windows version > 3.0
        {
        hModule = GetModuleHandle(lpPath);
        }
    else
        {
        if (STRLEN(lpPath) < 64)
            hModule = GetModuleHandle(lpPath);
        }

    //-----------------------------------------------------------------
    // In general we will return because most files will not be in use
    // by kernel
    //-----------------------------------------------------------------
    if (hModule == NULL)
        return (FALSE);

    GetModuleFileName(hModule,szPathBuffer,SYM_MAX_PATH);

    return (STRICMP(szPathBuffer,lpPath) == 0 ? TRUE : FALSE);
#else
    return (FALSE);
#endif
}
#endif // #if !defined(SYM_WIN32) && !defined(SYM_OS2) && !defined(SYM_NTK)

/*---------------------------------------------------------------------------
 FileOpen
    This routine is used to open a file.


 INPUTS
    LPCTSTR lpszFileName Name of file
    UINT  uMode          OF_ flags indicating mode to open

 RETURNS
    The file handle of the successfully opened file, or HFILE_ERROR(-1) on error.
    DOS ONLY: If an error occurs, the error code is stored in the global wLastError
              and -1(DOS,WIN16) or HFILE_ERROR(other) is returned as the file handle.
              You can retrieve the real error by using NDosGetError()
    Windows platforms can use GetLastError() on failure.

 REMARKS:
    The OF_ flags are from the old Win 3.1 OpenFile API, but only the following
    flags are used:
        Access Flags:
            OF_READ, OF_WRITE, OF_READWRITE
        Share Flags:
            OF_SHARE_COMPAT, OF_SHARE_EXCLUSIVE, OF_SHARE_DENY_WRITE, OF_SHARE_DENY_READ,
            OF_SHARE_DENY_NONE
        Creation Flags:
            OF_CREATE
---------------------------------------------------------------------------*/

#if !defined(SYM_OS2) && !defined(SYM_VXD)
#pragma optimize("", off)
HFILE SYM_EXPORT WINAPI FileOpen (LPCTSTR lpFileName, UINT uMode)
{
#if defined(SYM_NTK)

    auto NTSTATUS          status;
    auto UINT              uTemp;
    auto PTSTR             pszNTFileName;
    auto UNICODE_STRING    FileName;
    auto OBJECT_ATTRIBUTES ObjectAttributes;
    auto ACCESS_MASK       AccessMask;
    auto UINT              ShareAccess;
    auto UINT              CreateDisposition;
    auto HANDLE            hFile;
    auto IO_STATUS_BLOCK   IoStatusBlock;

    SYM_ASSERT ( lpFileName );
    SYM_VERIFY_STRING ( lpFileName );

                                        // Initialize object attributes

    if ( ! ( pszNTFileName = FileCreateNativeNTNameEx ( lpFileName ) ) )
        {
        return ( HFILE_ERROR );
        }

    RtlInitUnicodeString ( &FileName, pszNTFileName );

    InitializeObjectAttributes ( &ObjectAttributes,
                                 &FileName,
                                 OBJ_CASE_INSENSITIVE,
                                 NULL,
                                 NULL );

                                        // Convert the uMode parameter to
                                        // access mask.

    uTemp = uMode & ( OF_READ | OF_WRITE | OF_READWRITE );
    if ( uTemp == OF_READ )
        AccessMask = GENERIC_READ | SYNCHRONIZE | FILE_READ_ATTRIBUTES;
    else if ( uTemp == OF_WRITE )
        AccessMask = GENERIC_WRITE | SYNCHRONIZE | FILE_READ_ATTRIBUTES;
    else if ( uTemp == OF_READWRITE )
        AccessMask = GENERIC_READ | GENERIC_WRITE | SYNCHRONIZE | FILE_READ_ATTRIBUTES;
    else
        AccessMask = SYNCHRONIZE | FILE_READ_ATTRIBUTES;

                                        // Initialize share access.  This has
                                        // been copied from the Win32 code,
                                        // but it may be wrong, since more
                                        // than one of the OF_SHARE_xxx flags
                                        // can be specified in uMode.
                                        // Note, that we will always open
                                        // with FILE_SHARE_DELETE flag, even
                                        // if the user does not want us to do
                                        // so.  This is because the only (so
                                        // far) NTK client of Core is NAVAP,
                                        // and it needs to open files while
                                        // they are opened for DELETE (scan
                                        // during rename event).  The ideal
                                        // solution for this would be to
                                        // instruct Core (per-thread-data) to
                                        // pass that share flag only during
                                        // the rename event and under any
                                        // other context to do without it.

    uTemp = uMode & (OF_SHARE_COMPAT | OF_SHARE_EXCLUSIVE |
                     OF_SHARE_DENY_WRITE | OF_SHARE_DENY_READ |
                     OF_SHARE_DENY_NONE);

    if ( bFileSharing == TRUE ) {
        switch (uTemp)
            {
            case OF_SHARE_DENY_READ:
                ShareAccess = FILE_SHARE_WRITE | FILE_SHARE_DELETE;
                break;

            case OF_SHARE_DENY_WRITE:
                ShareAccess = FILE_SHARE_READ | FILE_SHARE_DELETE;
                break;

            case OF_SHARE_DENY_NONE:
                ShareAccess = FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE;
                break;

            case OF_SHARE_COMPAT:
                ShareAccess = FILE_SHARE_READ | FILE_SHARE_DELETE; // Allow other folks to read
                break;                              // This mode is a hack, but it
                                                    // is the default.
            case OF_SHARE_EXCLUSIVE:
                ShareAccess = FILE_SHARE_DELETE; // no sharing
                break;

            default:
                SYM_ASSERT ( FALSE );       // Alert us in case this ever
                                            // happens.
                ShareAccess = FILE_SHARE_DELETE;
                break;
            }
        }
    else
        ShareAccess = FILE_SHARE_DELETE;

                                        // Set the creation disposition, mapping the old OF_ flag behavior
                                        // to the equivalent ZvCreateFile flags.
    if (uMode & OF_CREATE)
        {
                                        // Always create file, whether or not it exists
        CreateDisposition = FILE_OVERWRITE_IF;

                                        // If creating the file in such way,
                                        // then the new file will be owned by
                                        // the user impersonated by the
                                        // current thread/process.  This is
                                        // NOT how FileCreate() works.  New
                                        // files are always owned by
                                        // NT AUTHORITY.

        SYM_ASSERT ( FALSE );
        }
    else
                                        // Only open if it already exists
        CreateDisposition = FILE_OPEN;

                                        // Open file

    status = ZvCreateFile( &hFile,
                           AccessMask,
                           &ObjectAttributes,
                           &IoStatusBlock,
                           NULL,        // optional
                           FILE_ATTRIBUTE_NORMAL,
                           ShareAccess,
                           CreateDisposition,
                           FILE_NON_DIRECTORY_FILE | FILE_RANDOM_ACCESS | FILE_SYNCHRONOUS_IO_NONALERT,
                           NULL,        // optional
                           0 );

    FileDestroyNativeNTName ( pszNTFileName );

    if ( NT_SUCCESS ( status ) )
        {
        return ( (HFILE)hFile );
        }

    // Set last error.

    return ( HFILE_ERROR );
#else
    auto  TCHAR szConvertedFile[SYM_MAX_PATH];


    ExpandAndConvertFile (lpFileName, szConvertedFile);

#ifdef SYM_WIN32
    {
    DWORD       fdwAccess, fdwShare, fdwCreation;
    UINT        uOldAccess, uOldShare;

    SYM_ASSERT ( lpFileName );
    SYM_VERIFY_STRING ( lpFileName );
                                        // Translate access
    fdwAccess = 0;
    uOldAccess = uMode & (OF_READ | OF_WRITE | OF_READWRITE);
    if ( uOldAccess == OF_READ )
        fdwAccess = GENERIC_READ;
    else if ( uOldAccess == OF_WRITE )
        fdwAccess = GENERIC_WRITE;
    else if ( uOldAccess == OF_READWRITE )
        fdwAccess = GENERIC_READ | GENERIC_WRITE;

                                        // Translate share
    uOldShare = uMode & (OF_SHARE_COMPAT | OF_SHARE_EXCLUSIVE |
                         OF_SHARE_DENY_WRITE | OF_SHARE_DENY_READ |
                         OF_SHARE_DENY_NONE);
    switch (uOldShare)
        {
        case OF_SHARE_DENY_READ:
            fdwShare = FILE_SHARE_WRITE;
            break;

        case OF_SHARE_DENY_WRITE:
            fdwShare = FILE_SHARE_READ;
            break;

        case OF_SHARE_DENY_NONE:
            fdwShare = FILE_SHARE_READ | FILE_SHARE_WRITE;
            break;

        case OF_SHARE_COMPAT:
            fdwShare = FILE_SHARE_READ;         // Allow other folks to read
            break;                              // This mode is a hack, but it
                                                // is the default.
        case OF_SHARE_EXCLUSIVE:
        default:
            fdwShare = 0;               // no sharing
            break;
        }

                                        // Set the creation disposition, mapping the old OF_ flag behavior
                                        // to the equivalent CreateFile flags.
    if (uMode & OF_CREATE)
        fdwCreation = CREATE_ALWAYS;    // Always create file, whether or not it exists
    else
        fdwCreation = OPEN_EXISTING;    // Only open if it already exists


    return ( (HFILE) CreateFile(szConvertedFile,
                       fdwAccess,
                       fdwShare,
                       NULL,
                       fdwCreation,
                       FILE_ATTRIBUTE_NORMAL,
                       NULL) );
    }
#else                                   // not SYM_WIN32
    {
    auto   HFILE hFile;


    _ClearErrorFar();                   // Reset so we don't catch someone
                                        // else's error
    _asm
        {
        push    ds                      ; Save the DS value
        push    si

        lea     si,szConvertedFile      ; Load the path name
        push    ss
        pop ds
        mov     bx,uMode                ; Open mode (R/W/Share)
        xor     cx,cx                   ; Set to 0, to get around Win95 bug
        mov     dx,1                    ; Open the file, iff it exists
#if !defined(SYM_DOSX)
        mov     ax,716Ch                ; Extended
        stc                             ; Assume not supported
        DOS
        jnc     doneFileOpen
        cmp     ax,7100h                // Supported?
        jne     doneFileOpen            // Yes, but really an error
#endif

        lea     dx,szConvertedFile      ; Load the path name
        push    ss
        pop ds
        mov     ah,3Dh
        mov     al,BYTE PTR uMode       ; Set requested mode
        DOS

doneFileOpen:
        pop     si
        pop     ds                      ; Restore DS
        call    FileCheckError          ; set AX to ERR if bad call
        mov     hFile, ax               ; store the file handle
        }

    return (hFile);
    }
#endif                                  // not SYM_WIN32
#endif                                  // !SYM_NTK
}
#pragma optimize("", on)
#endif // !defined(SYM_OS2) && !defined(SYM_VXD)


////////////////////////////////////////////////////////////////////////
//   FileCreateTemporaryPath -- Create temp file at path given
//   FileCreateTemporary     -- Create temp file in TEMP or TMP dir
//
// Description:
//     This functions uses int 21h function 5Ah (in DOS)
// NOTE NOTE: The Attribute flags you should pass to as the uAttributes
//            should be the FA_ flags from FILE.H, since this is
//            CROSS PLATFORM CODE and those are the defines used in
//            DOS/Win16.  (BEM - 8/27/96)
//
// See Also:
//
//
////////////////////////////////////////////////////////////////////////
//  5/ 2/93 ETS Function created.
//  5/29/93 MSL Calls GetTempFileName() in windows.  We really need
//              a full path in windows because using the current working
//              directory in any windows app is a pandoras box.
////////////////////////////////////////////////////////////////////////

#if !defined(SYM_OS2) && !defined(SYM_VXD) && !defined(SYM_NTK)
#pragma optimize ("", off)              // Turn off global opt for inline asm
HFILE SYM_EXPORT WINAPI FileCreateTemporary(
        LPTSTR lpFileName,              // [in/out] Buffer to return temp file's name
        UINT    uAttributes             // [in] file attribute (use FA_ defines!!)
)
{
    return FileCreateTemporaryPath (NULL, lpFileName, uAttributes);
}

HFILE SYM_EXPORT WINAPI FileCreateTemporaryPath(
        LPTSTR lpFilePath,              // [in] Path to store temp file
                                        // NOTE: NULL = use TEMP variable ("find a place")
        LPTSTR lpFileName,              // [in/out] Buffer to return temp file's name
                                        // NOTE: Can reuse lpFilePath here
        UINT   uAttributes              // [in] file attribute (use FA_ defines!!)
)
{
    auto        TCHAR           szTempPath[SYM_MAX_PATH];


#ifdef SYM_WIN32
    {
    UINT uConvertedAttributes = 0;

                                        // CONVERT FA_ flags into Win32 flags - the user should be passing in
                                        // the FA_ flags!!
                                        // FILE_ATTRIBUTE_NORMAL conflicts with FILE_ATTRIBUTE_TEMPORARY, so
                                        // FA_NORMAL will be ignored (it is 0, so that makes it easier to ignore)

    if (uAttributes & FA_READ_ONLY)
        uConvertedAttributes |= FILE_ATTRIBUTE_READONLY;
    if (uAttributes & FA_HIDDEN)
        uConvertedAttributes |= FILE_ATTRIBUTE_HIDDEN;
    if (uAttributes & FA_SYSTEM)
        uConvertedAttributes |= FILE_ATTRIBUTE_SYSTEM;
    if (uAttributes & FA_ARCHIVE)
        uConvertedAttributes |= FILE_ATTRIBUTE_ARCHIVE;
    if (uAttributes & FA_TEMPORARY)
        uConvertedAttributes |= FILE_ATTRIBUTE_TEMPORARY;


    if (lpFilePath != NULL)
        STRCPY(szTempPath, lpFilePath);
    else
        GetTempPath(sizeof(szTempPath)/sizeof(*szTempPath), szTempPath);

    GetTempFileName(szTempPath, _T("TMP"), 0, lpFileName);
    return ( (HFILE) CreateFile(lpFileName,
                       GENERIC_READ | GENERIC_WRITE,
                       0,               // Don't share
                       NULL,
                       CREATE_ALWAYS,
                       FILE_ATTRIBUTE_TEMPORARY | uConvertedAttributes,
                       NULL) );
    }
#elif defined(SYM_WIN)
    {
    if (lpFilePath != NULL)
        STRCPY(szTempPath, lpFilePath);

    GetTempFileName (0, "TMP", 0, szTempPath);
    NOemToAnsi(szTempPath, lpFileName);
    return( FileCreate(lpFileName, uAttributes) );
    }
#else
    {
    auto        HFILE           hFile;
    auto        BYTE            byDrive;

    if (lpFilePath == NULL)
        {
                                        // Use temporary environment variables
        *szTempPath = EOS;
        DOSGetEnvironment ("TEMP", szTempPath, sizeof(szTempPath));
        if ( *szTempPath == EOS )
            DOSGetEnvironment("TMP", szTempPath, sizeof(szTempPath));
                                        // Make sure it is a valid directory
        if ((*szTempPath == EOS) ||
            (!(NameIsDirectory(szTempPath) ||
            NameIsRoot(szTempPath))))
            {
                                        // Use root of C:
            byDrive = DiskGetFirstFixedDrive();
            if ( DiskExists(byDrive) )
                {
                STRCPY(szTempPath, "x:");
                szTempPath[0] = byDrive;
                }
            else
                {                       // If no drive C:, use current dir
                szTempPath[0] = DiskGet();
                szTempPath[1] = ':';
                szTempPath[2] = EOS;
                }
            }
        }
    else
        {
        STRCPY(szTempPath, lpFilePath);
        }
                                        // String must end in '\'
    if (StringGetLastChar(szTempPath) != '\\')
        StringAppendChar(szTempPath, '\\');

    STRCPY(lpFileName, szTempPath);

    _ClearErrorFar ();                  // Reset so we don't catch someone
                                        // else's error
    _asm
        {
        push    ds                      //  Save the DS value
        lds     dx,lpFileName           //  Load the path name
        mov     cx,uAttributes          //  Set file attributes
        mov     ah,5Ah
        DOS
        pop     ds                      //  Restore DS
        call    FileCheckError          //  set AX to ERR if bad call
        mov     hFile, ax               //  store the file handle
        }

    return (hFile);
    }

#endif
}
#pragma optimize ("", on)               // Restore default optimizations
#endif // #if !defined(SYM_OS2) && !defined(SYM_VXD) && !defined(SYM_NTK)


/*---------------------------------------------------------------------------*/
/* FileCreate                                                                */
/*    This routine is used to create a file.                                 */
/*                                                                           */
/* INPUTS                                                                    */
/*    LPCTSTR lpszFileName Name of file                                      */
/*    UINT  wAttr        Attributes of new file                              */
/*                                                                           */
/* RETURNS                                                                   */
/*    The file handle of the successfully opened file.                       */
/*    If an error occurs, the error code is stored in the global wLastError  */
/*    and -1(DOS,WIN16) or HFILE_ERROR(other) is returned as the file handle.*/
/*    You can retrieve the real error by using NDosGetError()                */
/*                                                                           */
/* SYM_NTK: Existing files will be overwritten, unless they have their       */
/*          FILE_ATTRIBUTE_READONLY attribute set.                           */
/*          If the destination file is located on a local drive, with FS     */
/*          supporting permanent ACLs, the owner of the file will be set to  */
/*          Administrators group.  If the file is remote, or the FS does not */
/*          support permanent security, then the owner will be determined    */
/*          by the current thread/process context.                           */
/*                                                                           */
/* SYM_VXD and SYM_NTK: passing 0x80000000 together with the regular uAttr   */
/* will change the disposition to FILE_CREATE.                               */
/*                                                                           */
/*---------------------------------------------------------------------------*/

#if defined(SYM_NTK)

typedef struct
    {
    NTSTATUS Status;
    PKEVENT  SignalMe;
    LPCTSTR  lpFileName;
    UINT     uAttr;
    } FILE_CREATE_WORKER_PARAMS;

static VOID FileCreateWorker ( IN FILE_CREATE_WORKER_PARAMS *prParams )
    {
    auto PTSTR             pszNTFileName;
    auto UNICODE_STRING    FileName;
    auto OBJECT_ATTRIBUTES ObjectAttributes;
    auto HANDLE            hFile;
    auto IO_STATUS_BLOCK   IoStatusBlock;

    SYM_ASSERT ( prParams );
    SYM_ASSERT ( prParams->lpFileName );
    SYM_VERIFY_STRING ( prParams->lpFileName );
    SYM_ASSERT ( !( prParams->uAttr & ~( FILE_ATTRIBUTE_READONLY |
                                         FILE_ATTRIBUTE_HIDDEN |
                                         FILE_ATTRIBUTE_SYSTEM |
                                         FILE_ATTRIBUTE_ARCHIVE |
                                         FILE_ATTRIBUTE_NORMAL |
                                         0x80000000 ) ) );

                                        // Initialize object attributes

    if ( pszNTFileName = FileCreateNativeNTName ( prParams->lpFileName ) )
        {
        RtlInitUnicodeString ( &FileName, pszNTFileName );

        InitializeObjectAttributes ( &ObjectAttributes,
                                     &FileName,
                                     OBJ_CASE_INSENSITIVE,
                                     NULL,
                                     NULL );

                                        // Create file

        prParams->Status = ZvCreateFile ( &hFile,
                                          GENERIC_READ | GENERIC_WRITE | SYNCHRONIZE | FILE_READ_ATTRIBUTES,
                                          &ObjectAttributes,
                                          &IoStatusBlock,
                                          NULL,
                                          prParams->uAttr & 0x7FFFFFFF,
                                          FILE_SHARE_READ,
                                          (prParams->uAttr & 0x80000000) ? FILE_CREATE : FILE_OVERWRITE_IF,
                                          FILE_NON_DIRECTORY_FILE | FILE_RANDOM_ACCESS | FILE_SYNCHRONOUS_IO_NONALERT,
                                          NULL,
                                          0 );

        if ( NT_SUCCESS ( prParams->Status ) )
            {
            ZvClose ( hFile );
            }

        FileDestroyNativeNTName ( pszNTFileName );
        }
    else
        {
        // Set last error.  Note, that we have to use prParams to return
        // the error set by FileCreateNativeNTName() in order for it to
        // be transferred properly to the thread who initiated the call to
        // this function.
        }

    if ( prParams->SignalMe )
        {
        KeSetEvent ( prParams->SignalMe, 0, FALSE );
        }
    }

static BOOL FileCreateIsThisNTAuthority ( VOID )
    {
    auto NTSTATUS status;
    auto HANDLE   hToken;

    if ( CoreIsThisNTSystemProcess() )
        {
        status = ZwOpenThreadToken ( NtCurrentThread(),
                                     TOKEN_QUERY,
                                     TRUE,
                                     &hToken );

        if ( NT_SUCCESS ( status ) )
            {
            ZwClose ( hToken );
            }
        else if ( status == STATUS_NO_TOKEN )
            {
            return ( TRUE );
            }
        }

    return ( FALSE );
    }

#endif


#if !defined(SYM_OS2) && !defined(SYM_VXD)
#pragma optimize("", off)
HFILE SYM_EXPORT WINAPI FileCreate(LPCTSTR lpFileName, UINT uAttr)
{
#if defined(SYM_NTK)

    auto FILE_CREATE_WORKER_PARAMS             WorkerParams;
    auto HANDLE                                hRoot;
    auto IO_STATUS_BLOCK                       IoStatus;
    auto struct {
             union {
                 FILE_FS_DEVICE_INFORMATION    DevInfo;
                 FILE_FS_ATTRIBUTE_INFORMATION AttrInfo;
             };
             WCHAR                             szPadding[SYM_MAX_PATH];
         } FsInfo;
    auto WORK_QUEUE_ITEM                       WorkQueueItem;
    auto KEVENT                                WaitHere;

    WorkerParams.lpFileName = lpFileName;
    WorkerParams.uAttr      = uAttr;

                                        // In order for us to use a kernel
                                        // mode worker thread for the
                                        // file creation, we need to
                                        // ensure that...

                                        // ... we are not operating under the
                                        // context of NT AUTHORITY in a
                                        // system worker thread

    if ( ( hRoot = ( FileCreateIsThisNTAuthority() ?

                                        // ... and we successfully open a
                                        // root handle

                     NULL : FileOpenRootHandle ( lpFileName ) ) ) &&

                                        // ... and the device containing that
                                        // root handle is local

         NT_SUCCESS ( ZvQueryVolumeInformationFile ( hRoot,
                                                     &IoStatus,
                                                     &FsInfo.DevInfo,
                                                     sizeof(FsInfo.DevInfo),
                                                     FileFsDeviceInformation ) ) &&
         IoStatus.Information == sizeof(FsInfo.DevInfo) &&
         !( FsInfo.DevInfo.Characteristics & FILE_REMOTE_DEVICE ) &&

                                        // ... and the device containing that
                                        // handle supports persistent ACLs

         NT_SUCCESS ( ZvQueryVolumeInformationFile ( hRoot,
                                                     &IoStatus,
                                                     &FsInfo.AttrInfo,
                                                     sizeof(FsInfo),
                                                     FileFsAttributeInformation ) ) &&
         IoStatus.Information >= sizeof(FsInfo.AttrInfo) &&
         ( FsInfo.AttrInfo.FileSystemAttributes & FILE_PERSISTENT_ACLS )
       )
        {
        FileCloseRootHandle ( hRoot );

        KeInitializeEvent ( &WaitHere, NotificationEvent, FALSE );

        WorkerParams.SignalMe = &WaitHere;

        ExInitializeWorkItem ( &WorkQueueItem, FileCreateWorker, &WorkerParams );

        ExQueueWorkItem ( &WorkQueueItem, DelayedWorkQueue );

        if ( !NT_SUCCESS ( KeWaitForSingleObject ( &WaitHere,
                                                   Executive,
                                                   KernelMode,
                                                   FALSE,
                                                   (PLARGE_INTEGER) NULL ) )
           )
            {
            // Set last error
            SYM_ASSERT ( FALSE );
            return ( HFILE_ERROR );
            }
        }
    else
        {
        FileCloseRootHandle ( hRoot );

        WorkerParams.SignalMe = NULL;

        FileCreateWorker ( &WorkerParams );
        }

    if ( !NT_SUCCESS ( WorkerParams.Status ) )
        {
        // Set last error
        return ( HFILE_ERROR );
        }

    return ( FileOpen ( lpFileName, OF_READWRITE | OF_SHARE_DENY_WRITE ) );

#else
    auto  TCHAR szConvertedFile[SYM_MAX_PATH];


    ExpandAndConvertFile (lpFileName, szConvertedFile);

#ifdef SYM_WIN32
    return ( (HFILE) CreateFile(szConvertedFile,
                       GENERIC_READ | GENERIC_WRITE,
                       FILE_SHARE_READ,         // Allow read sharing
                       NULL,
                       CREATE_ALWAYS,
                       uAttr,
                       NULL) );
#elif defined(SYM_DOS) || defined(SYM_WIN)
    {
    auto   HFILE hFile;

    _ClearErrorFar ();                  // Reset so we don't catch someone
                                        // else's error
    _asm
        {
        push    ds                      ; Save the DS value
        push    si

        lea     si,szConvertedFile      ; Load the path name
    push    ss
    pop ds
        mov     bx,2                    ; Read/Write Open mode (R/W/Share)
        mov     cx,uAttr                ; Set file attributes
        mov     dx,0012h                ; Create/Replace file
#if !defined(SYM_DOSX)
        mov     ax,716Ch                ; Extended
        stc                             ; Assume not supported
        DOS
        jnc     doneFileCreate
        cmp     ax,7100h                // Supported?
        jne     doneFileCreate          // Yes, but really an error
#endif

        lea     dx,szConvertedFile      ; Load the path name
    push    ss
    pop ds
        mov     cx,uAttr                //  Set file attributes
        mov     ah,3Ch
        DOS

doneFileCreate:
        pop     si
        pop     ds                      ; Restore DS
        call    FileCheckError          //  set AX to ERR if bad call
        mov     hFile, ax               //  store the file handle
        }

    return (hFile);
    }
#else
    return (creat(szConvertedFile, uAttr));
#endif                                  // not SYM_WIN32
#endif                                  // !SYM_NTK
}
#pragma optimize("", on)
#endif // #if !defined(SYM_OS2) && !defined(SYM_VXD)


/*---------------------------------------------------------------------------*/
/* FileDelete                                                                */
/*    This routine is used to delete files, hence its name.                  */
/*                                                                           */
/* INPUTS                                                                    */
/*    LPTSTR            filename                                             */
/*                                                                           */
/* RETURNS                                                                   */
/*    DOS,WIN16:-1, other:ERR if error, else DOS:0, other:NOERR.             */
/*---------------------------------------------------------------------------*/

#if !defined(SYM_OS2) && !defined(SYM_VXD)
#pragma optimize("", off)
UINT SYM_EXPORT WINAPI FileDelete(LPCTSTR lpFileName)
{
#if defined(SYM_NTK)
    auto NTSTATUS          status;
    auto PTSTR             pszNTFileName;
    auto UNICODE_STRING    FileName;
    auto OBJECT_ATTRIBUTES ObjectAttributes;
    auto HANDLE            hFile;
    auto IO_STATUS_BLOCK   IoStatusBlock;

    SYM_ASSERT ( lpFileName );
    SYM_VERIFY_STRING ( lpFileName );

                                        // Initialize object attributes

    if ( ! ( pszNTFileName = FileCreateNativeNTName ( lpFileName ) ) )
        {
        return ( ERR );
        }

    RtlInitUnicodeString ( &FileName, pszNTFileName );

    InitializeObjectAttributes ( &ObjectAttributes,
                                 &FileName,
                                 OBJ_CASE_INSENSITIVE,
                                 NULL,
                                 NULL );

                                        // Delete file.  Do NOT use
                                        // ZwDeleteFile() because it blocks
                                        // on oplocked files until the oplock
                                        // is broken!

    status = ZvCreateFile ( &hFile,
                            DELETE,
                            &ObjectAttributes,
                            &IoStatusBlock,
                            NULL,
                            0,
                            FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                            FILE_OVERWRITE,
                            FILE_DELETE_ON_CLOSE | FILE_COMPLETE_IF_OPLOCKED,
                            NULL,
                            0 );

    FileDestroyNativeNTName ( pszNTFileName );

    if ( NT_SUCCESS ( status ) )
        {
        ZvClose ( hFile );

        return ( NOERR );
        }

    // Set last error.

    return ( ERR );
#else
    auto        TCHAR           szConvertedFile[SYM_MAX_PATH];


    ExpandAndConvertFile (lpFileName, szConvertedFile);

#ifdef SYM_WIN32
    return (DeleteFile(szConvertedFile) ? NOERR : ERR);
#else
    {
    auto        UINT            wRetCode;

    _asm
        {
        push    ds                      // Save the DS value
        push    si                      // Save this too

        lea     dx,szConvertedFile      // Load the path name
        mov     si,0                    // Don't allow wildcards/attributes
        push    ss
        pop     ds

#if !defined(SYM_DOSX)
        mov     ax,7141h                // Extended
        stc
        DOS
        jnc     doneFileDelete
        cmp     ax,7100h                // Supported?
        jne     doneFileDelete          // Yes, but really an error
#endif

        mov     ah,41h
        DOS

doneFileDelete:
        pop     si                      //
        pop     ds                      //
        call    FileGetReturnCode       //  set AX to to ERR or NOERR
        mov     wRetCode, ax            //  Get the return code
        }
    return (wRetCode);
    }
#endif
#endif                                  // !SYM_NTK

}
#pragma optimize("", on)
#endif // #if !defined(SYM_OS2) && !defined(SYM_VXD)


/*--------------------------------------------------------------------------*/
/* FileRename                                                               */
/*     This function call int 21 sub function 56h to do a DOS rename        */
/*                                                                          */
/* INPUTS                                                                   */
/*    LPCTSTR   lpOldName       old name                                    */
/*    LPCTSTR   lpNewName       new name                                    */
/*                                                                          */
/* RETURNS:                                                                 */
/*      DOS,WIN16:-1 other:ERR if error, else DOS:0 other:NOERR.            */
/*--------------------------------------------------------------------------*/

#if !defined(SYM_OS2) && !defined(SYM_VXD)
#pragma optimize("", off)
UINT SYM_EXPORT WINAPI FileRename(LPCTSTR lpOldName, LPCTSTR lpNewName)
{
#if defined(SYM_NTK)
    auto UINT                     uNewNameLen, uFileInformationLen;
    auto PFILE_RENAME_INFORMATION pFileRenameInformation;
    auto HANDLE                   hFile, hRoot;
    auto UNICODE_STRING           FileName;
    auto OBJECT_ATTRIBUTES        ObjectAttributes;
    auto IO_STATUS_BLOCK          IoStatusBlock;
    auto NTSTATUS                 Stat;

    SYM_ASSERT ( FALSE );               // This function has to be reviewed
                                        // as soon as NTK clients start using
                                        // it.  The usage of root handles
                                        // should also be eliminated.

    SYM_ASSERT ( lpOldName );
    SYM_VERIFY_STRING ( lpOldName );
    SYM_ASSERT ( lpNewName );
    SYM_VERIFY_STRING ( lpNewName );

                                        // Initialize local variables.

    uNewNameLen = STRLEN ( lpNewName ) * sizeof(TCHAR);

    uFileInformationLen = uNewNameLen +
                          sizeof(TCHAR) +
                          sizeof ( *pFileRenameInformation );

                                        // Create FILE_RENAME_INFORMATION
                                        // structure.

    pFileRenameInformation =
        (PFILE_RENAME_INFORMATION) MemAllocPtr ( GHND, uFileInformationLen );

    if ( !pFileRenameInformation )
        {
        // Set last error to "not enough memory to complete request."
        return ( ERR );
        }

#if 0
    pFileRenameInformation->ReplaceIfExists = FALSE;
    pFileRenameInformation->RootDirectory = NULL;
#endif
    pFileRenameInformation->FileNameLength = uNewNameLen;
    MEMCPY ( pFileRenameInformation->FileName, lpNewName, uNewNameLen );

                                        // Initialize object attributes

    hRoot = FileOpenRootHandle ( lpOldName );

    RtlInitUnicodeString ( &FileName, FileRelativeToRootName ( lpOldName ) );

    InitializeObjectAttributes ( &ObjectAttributes,
                                 &FileName,
                                 OBJ_CASE_INSENSITIVE,
                                 hRoot,
                                 NULL );

                                        // Open file

    Stat = ZvOpenFile ( &hFile,
                        DELETE | SYNCHRONIZE,
                        &ObjectAttributes,
                        &IoStatusBlock,
                        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                        FILE_SYNCHRONOUS_IO_NONALERT | FILE_OPEN_FOR_BACKUP_INTENT );

    if ( NT_SUCCESS ( Stat ) )
        {
        Stat = ZvSetInformationFile ( hFile,
                                      &IoStatusBlock,
                                      pFileRenameInformation,
                                      uFileInformationLen,
                                      FileRenameInformation );

        if ( NT_SUCCESS ( Stat ) )
            {
            Stat = ZvClose ( hFile );
            }
        else
            {
            ZvClose ( hFile );
            }
        }

    FileCloseRootHandle ( hRoot );

                                        // Release allocated resources

    MemFreePtr ( pFileRenameInformation );

    if ( NT_SUCCESS ( Stat ) )
        {
        return ( NOERR );
        }

    // Set last error.

    return ( ERR );
#else
    auto        TCHAR   szOldOemStr[SYM_MAX_PATH];
    auto        TCHAR   szNewOemStr[SYM_MAX_PATH];
    auto        LPTSTR  lpOldOemStr;
    auto        LPTSTR  lpNewOemStr;


    lpOldOemStr = ExpandAndConvertFile (lpOldName, szOldOemStr);
    lpNewOemStr = ExpandAndConvertFile (lpNewName, szNewOemStr);

#if defined(SYM_WIN32)
    return (MoveFile(lpOldOemStr, lpNewOemStr) ? NOERR : ERR);
#else
    {
    auto        UINT    wRetCode;

    _asm
        {
        push    DS
        push    di
        LDS     DX,lpOldOemStr
        LES     DI,lpNewOemStr          // Get a pointer to the ASCIIZ name

#if !defined(SYM_DOSX)
        mov     ax,7156h                // Extended
        stc                             ; Assume not supported
        DOS
        jnc     doneFileRename
        cmp     ax,7100h                // Supported?
        jne     doneFileRename          // Yes, but really an error
#endif

        mov     ah,56h
        DOS

doneFileRename:
        pop     di
        pop     ds
        call    FileGetReturnCode       //  set AX to to ERR or NOERR
        MOV     wRetCode, AX
        }

    return (wRetCode);
    }
#endif
#endif                                  // !SYM_NTK

}
#pragma optimize("", on)
#endif // #if !defined(SYM_OS2) && !defined(SYM_VXD)


/*---------------------------------------------------------------------------*/
/* FileLength                                                                */
/*    This routine returns the file length for the specified file handle     */
/*    without changing the file pointer.                                     */
/*                                                                           */
/* INPUTS                                                                    */
/*    UINT       wFileHandle    handle to the file                           */
/*                                                                           */
/* RETURNS                                                                   */
/*    LONG       file length if successful                                   */
/*                           -1 if failure                                   */
/*---------------------------------------------------------------------------*/

#if !defined(SYM_OS2) && !defined(SYM_VXD)
DWORD SYM_EXPORT WINAPI FileLength(HFILE hFileHandle)
{
#if defined(SYM_NTK)

    auto NTSTATUS                  status;
    auto IO_STATUS_BLOCK           IoStatusBlock;
    auto FILE_STANDARD_INFORMATION FileStdInfo;

    SYM_ASSERT ( hFileHandle != HFILE_ERROR );

    status = ZvQueryInformationFile ( (HANDLE)hFileHandle,
                                      &IoStatusBlock,
                                      &FileStdInfo,
                                      sizeof(FileStdInfo),
                                      FileStandardInformation );

    if ( !NT_SUCCESS ( status ) )
        {
        // Set last error
        return ( (DWORD)-1L );
        }

    if ( FileStdInfo.EndOfFile.QuadPart >= 0xFFFFFFFF )
        {
        // Set last error to File_Too_Large_For_Core
        return ( (DWORD)-1L );
        }

    return ( FileStdInfo.EndOfFile.LowPart );
#else
    auto        DWORD           dwCurrentPos;
    auto        DWORD           dwFileLength;


                                        /* Get the current position     */
    dwFileLength = (UINT)ERR;
    dwCurrentPos = FileSeek(hFileHandle, 0L, SEEK_FROM_CURRENT_LOC);

    if (dwCurrentPos != (UINT)ERR)
        {                               /* Go to the end of the file    */
        dwFileLength = FileSeek(hFileHandle, 0L, SEEK_TO_END);
                                        /* Reset to previous position   */
        FileSeek(hFileHandle, dwCurrentPos, SEEK_FROM_BEGINNING);
        }

    return(dwFileLength);               /* Return length of the file    */
#endif
}
#endif // #if !defined(SYM_OS2) && !defined(SYM_VXD)

#endif  // ifndef SYM_NLM

/*----------------------------------------------------------------------*/
/*      Sets the size of a file.                                        */
/*----------------------------------------------------------------------*/

#if !defined(SYM_OS2) && !defined(SYM_NTK)
UINT SYM_EXPORT WINAPI FileSetSize(HFILE hFile, DWORD dwSizeNeeded)
{
    auto        UINT    wRet;

    wRet = (UINT)ERR;

    if (FileSeek (hFile, dwSizeNeeded, SEEK_FROM_BEGINNING) != (UINT)ERR)
        {
#if defined(SYM_WIN32)
        wRet = (SetEndOfFile((HANDLE) hFile)) ? NOERR : ERR;
#else
                                        /* Writing 0 bytes out sets     */
                                        /* size to current offset       */
        wRet = FileWrite(hFile, (LPSTR) &hFile, 0);
#endif

        }

    return(wRet);

}
#endif // #if !defined(SYM_OS2) && !defined(SYM_NTK)


/*----------------------------------------------------------------------
 FileGetAttr
    This function is used to get the attributes for a particular file

 INPUTS
    LPCTSTR       lpFileName      long pointer to the file name
    LPUINT        lpAttr          long pointer to the attributes

 NOTE NOTE: The Attribute flags you receive as *lpAttr
            will be the FA_ flags from FILE.H, since this is
            CROSS PLATFORM CODE and those are the defines used in
            DOS/Win16.  If you want to use the Win32 flags
            (e.g. FILE_ATTRIBUTE_NORMAL) then you should just call
            the Win32 API directly.  If you want to make your code
            cross platform, then you will have to use the FA_ flags
            and call this function.

            There is no FILE_ATTRIBUTE_VOLUME define, and so, the
            Win32 implementation of this function will never return
            a FA_VOLUME bit set.
            (BEM - 7/18/96)

 RETURN: NOERR or ERR (for platforms other than DOS & WIN16)
----------------------------------------------------------------------*/

#if !defined(SYM_VXD)
#pragma optimize("", off)
UINT SYM_EXPORT WINAPI FileGetAttr(LPCTSTR lpFileName, UINT FAR *lpAttr)
{
#if defined(SYM_NTK)
    auto NTSTATUS               Stat;
    auto PTSTR                  pszNTFileName;
    auto UNICODE_STRING         FileName;
    auto OBJECT_ATTRIBUTES      ObjectAttributes;
    auto HANDLE                 hFile;
    auto IO_STATUS_BLOCK        IoStatusBlock;
    auto FILE_BASIC_INFORMATION FileInfo;

    SYM_ASSERT ( lpFileName );
    SYM_VERIFY_STRING ( lpFileName );
    SYM_ASSERT ( lpAttr );
    SYM_VERIFY_BUFFER ( lpAttr, sizeof(*lpAttr) );

                                        // Initialize object attributes

    if ( ! ( pszNTFileName = FileCreateNativeNTName ( lpFileName ) ) )
        {
        return ( ERR );
        }

    RtlInitUnicodeString ( &FileName, pszNTFileName );

    InitializeObjectAttributes ( &ObjectAttributes,
                                 &FileName,
                                 OBJ_CASE_INSENSITIVE,
                                 NULL,
                                 NULL );

                                        // Open file

    Stat = ZvOpenFile ( &hFile,
                        FILE_READ_ATTRIBUTES | SYNCHRONIZE,
                        &ObjectAttributes,
                        &IoStatusBlock,
                        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                        FILE_NON_DIRECTORY_FILE );

    if ( NT_SUCCESS ( Stat ) )
        {
                                        // Get basic file information

        Stat = ZvQueryInformationFile ( hFile,
                                        &IoStatusBlock,
                                        &FileInfo,
                                        sizeof(FileInfo),
                                        FileBasicInformation );

        if ( NT_SUCCESS ( Stat ) )
            {
            *lpAttr = (UINT)FileInfo.FileAttributes;
            Stat = ZvClose ( hFile );
            }
        else
            {
            ZvClose ( hFile );
            }
        }

    FileDestroyNativeNTName ( pszNTFileName );

    if ( NT_SUCCESS ( Stat ) )
        {
        return ( NOERR );
        }

    // Set last error.

    return ( ERR );
#else
    auto        TCHAR   szConvertedFile[SYM_MAX_PATH];


    ExpandAndConvertFile (lpFileName, szConvertedFile);

#if defined(SYM_WIN32)

    {
    DWORD dwAttr = GetFileAttributes(szConvertedFile);

    if (lpAttr)
        {
        *lpAttr = dwAttr;
        if (dwAttr != 0xffffffff)
            {
            *lpAttr = 0;
            if (dwAttr & FILE_ATTRIBUTE_NORMAL)
                *lpAttr = FA_NORMAL;    // FA_NORMAL IS ZERO
            if (dwAttr & FILE_ATTRIBUTE_READONLY)
                *lpAttr |= FA_READ_ONLY;
            if (dwAttr & FILE_ATTRIBUTE_HIDDEN)
                *lpAttr |= FA_HIDDEN;
            if (dwAttr & FILE_ATTRIBUTE_SYSTEM)
                *lpAttr |= FA_SYSTEM;
            if (dwAttr & FILE_ATTRIBUTE_ARCHIVE)
                *lpAttr |= FA_ARCHIVE;
            if (dwAttr & FILE_ATTRIBUTE_TEMPORARY)
                *lpAttr |= FA_TEMPORARY;
            if (dwAttr & FILE_ATTRIBUTE_COMPRESSED)
                *lpAttr |= FA_COMPRESSED;
            if (dwAttr & FILE_ATTRIBUTE_DIRECTORY)
                *lpAttr |= FA_DIR;
                                        // THERE IS NO FILE_ATTRIBUTE_VOLUME WIN32 DEFINE
            }
        }

    return (dwAttr == 0xffffffff ? ERR : NOERR);
    }

#elif defined(SYM_OS2)

    *lpAttr = (UINT) _chmod(szConvertedFile, 0);
    if ((errno == ENOENT) || (errno == EACCES))
        return errno;
    return NOERR;

#elif defined(SYM_NLM)

    {
    int     nErr;
    struct  stat FileStatus;

    nErr = stat( lpFileName, &FileStatus );

    *lpAttr = FileStatus.st_mode;

    return nErr == 0 ? NOERR : ERR;

    }

#else

    {
    auto        UINT    wRetCode;
    auto        UINT    wTemp;



    _ClearErrorFar();                   // Reset so we don't catch someone
                                        // else's error
    _asm
        {
        push    ds

        lea     DX, szConvertedFile // Get the file name
    push    ss
    pop ds
        mov     bl,0                    // Get attributes
#if !defined(SYM_DOSX)
        mov     ax,7143h                // Extended
        stc
        DOS
        jnc     doneFileGetAttr
        cmp     ax,7100h                // Supported?
        je      tryOldWay               // No.  Try old way
        stc                             // Yes, really an error
        jmp     short doneFileGetAttr   //
#endif

tryOldWay:
        mov     ax,4300h
        DOS

doneFileGetAttr:
        pop     ds
#if defined(SYM_DOS)
        CALL    _CheckErrorFar          // Check for disk errors
#endif
        JNC     DONE_FILE_ATTR
        MOV     CX,0FFFFh               // set bogus attribute in CX register
DONE_FILE_ATTR:
        call    FileGetReturnCode       // set AX to to ERR or NOERR

        mov     wTemp, cx
        mov     wRetCode,ax
        }

    if (lpAttr)                         // return attribute if desired
        *lpAttr = wTemp;

    return (wRetCode);
    }

#endif
#endif                                  // !SYM_NTK
}
#pragma optimize("", on)
#endif // #if !defined(SYM_VXD)

#if defined SYM_NLM
UINT SYM_EXPORT WINAPI NLM_FileGetAttr(LPCTSTR lpFileName, UINT FAR *lpAttr)
    {
    DIR     DirEnt;
    DIR     *pDirEnt, *pDirStruct;
    UINT    nTest;

    pDirStruct = opendir(lpFileName);
    if( !pDirStruct )
        {
        return( ERR );
        }
    pDirEnt = readdir(pDirStruct);
     if( !pDirEnt )
        {
        closedir(pDirStruct);
        return( ERR );
        }
    
    nTest = (UINT)pDirEnt->d_attr;
    DirEnt.d_attr = pDirEnt->d_attr;
    *lpAttr = (UINT)pDirEnt->d_attr;
    closedir(pDirStruct);

    return( NOERR );
    }
#endif


/*----------------------------------------------------------------------*/
/* FileExists                                                           */
/*    This function tests if a file exists or not.  Does the test by    */
/*    attempting to get the attributes.                                 */
/*                                                                      */
/* INPUTS                                                               */
/*    LPCTSTR   lpFileName      long pointer to the file name           */
/*----------------------------------------------------------------------*/
/* 07/20/93 Martin, Function modified to return TRUE only for files.    */
/*----------------------------------------------------------------------*/

#if !defined(SYM_NLM)
BOOL SYM_EXPORT WINAPI FileExists(LPCTSTR lpFileName)
{
#if defined(SYM_NTK)
    auto NTSTATUS          Stat;
    auto PTSTR             pszNTFileName;
    auto UNICODE_STRING    FileName;
    auto OBJECT_ATTRIBUTES ObjectAttributes;
    auto HANDLE            hFile;
    auto IO_STATUS_BLOCK   IoStatusBlock;

    SYM_ASSERT ( lpFileName );
    SYM_VERIFY_STRING ( lpFileName );

                                        // Initialize object attributes

    if ( ! ( pszNTFileName = FileCreateNativeNTName ( lpFileName ) ) )
        {
        return ( FALSE );
        }

    RtlInitUnicodeString ( &FileName, pszNTFileName );

    InitializeObjectAttributes ( &ObjectAttributes,
                                 &FileName,
                                 OBJ_CASE_INSENSITIVE,
                                 NULL,
                                 NULL );

                                        // Open file

    Stat = ZvOpenFile ( &hFile,
                        0,
                        &ObjectAttributes,
                        &IoStatusBlock,
                        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                        FILE_NON_DIRECTORY_FILE );

    if ( NT_SUCCESS ( Stat ) )
        {
                                        // Close file

        Stat = ZvClose ( hFile );
        }

    FileDestroyNativeNTName ( pszNTFileName );

                                        // Check result of the above test

    if ( NT_SUCCESS ( Stat ) )
        {
        return ( TRUE );
        }

    // Set last error.

    return ( FALSE );

#elif defined(SYM_WIN32)

    DWORD dwAttributes = GetFileAttributes(lpFileName);

    if (dwAttributes == 0xffffffff)
        return FALSE;
    else if (dwAttributes & FILE_ATTRIBUTE_DIRECTORY)
        return FALSE;
    else
        return TRUE;

#else                                   // not SYM_WIN32

    auto    UINT    uAttr   = 0;
    auto    BOOL    bStatus = FALSE;

    if (FileGetAttr(lpFileName, &uAttr) != ERR)
        {
        if (uAttr & FA_DIR || uAttr & FA_VOLUME)
            bStatus = FALSE;
        else
            bStatus = TRUE;
        }

    return (bStatus);
#endif                                  // not SYM_WIN32 && not SYM_NTK
}

#else  // #if !defined(SYM_NLM)

BOOL SYM_EXPORT WINAPI FileExists(LPCTSTR lpFileName)

{
    struct  stat  filestatus;

    return( stat(lpFileName, &filestatus) ? FALSE : TRUE); 
}

#endif  // #if !defined(SYM_NLM)

/*----------------------------------------------------------------------
 FileSetAttr
    This function is used to set the attributes for a particular file.


 INPUTS
    LPCTSTR   lpFileName      long pointer to the file name
    UINT      wAttributes     contains the new attributes

 NOTE NOTE: The Attribute flags you should pass to as the wAttributes
            should be the FA_ flags from FILE.H, since this is
            CROSS PLATFORM CODE and those are the defines used in
            DOS/Win16.  If you want to use the Win32 flags
            (e.g. FILE_ATTRIBUTE_NORMAL) then you should just call
            the Win32 API directly.  If you want to make your code
            cross platform, then you will have to use the FA_ flags
            and call this function. (BEM - 7/18/96)

 RETURNS
    standard
----------------------------------------------------------------------*/

#if !defined(SYM_NLM) && !defined(SYM_VXD)
#pragma optimize("", off)
UINT SYM_EXPORT WINAPI FileSetAttr(LPCTSTR lpFileName, UINT wAttributes)
{
#if defined(SYM_NTK)
    auto NTSTATUS               Stat;
    auto PTSTR                  pszNTFileName;
    auto UNICODE_STRING         FileName;
    auto OBJECT_ATTRIBUTES      ObjectAttributes;
    auto HANDLE                 hFile;
    auto IO_STATUS_BLOCK        IoStatusBlock;
    auto FILE_BASIC_INFORMATION FileInfo;

    SYM_ASSERT ( lpFileName );
    SYM_VERIFY_STRING ( lpFileName );
    SYM_ASSERT ( !( wAttributes & ~( FILE_ATTRIBUTE_READONLY |
                                     FILE_ATTRIBUTE_HIDDEN |
                                     FILE_ATTRIBUTE_SYSTEM |
                                     FILE_ATTRIBUTE_DIRECTORY |
                                     FILE_ATTRIBUTE_ARCHIVE |
                                     FILE_ATTRIBUTE_NORMAL |
                                     FILE_ATTRIBUTE_TEMPORARY |
                                     FILE_ATTRIBUTE_COMPRESSED ) ) );

                                        // Initialize object attributes

    if ( ! ( pszNTFileName = FileCreateNativeNTName ( lpFileName ) ) )
        {
        return ( ERR );
        }

    RtlInitUnicodeString ( &FileName, pszNTFileName );

    InitializeObjectAttributes ( &ObjectAttributes,
                                 &FileName,
                                 OBJ_CASE_INSENSITIVE,
                                 NULL,
                                 NULL );

                                        // Open file

    Stat = ZvOpenFile ( &hFile,
                        FILE_WRITE_ATTRIBUTES | SYNCHRONIZE,
                        &ObjectAttributes,
                        &IoStatusBlock,
                        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                        FILE_NON_DIRECTORY_FILE );

    if ( NT_SUCCESS ( Stat ) )
        {
                                        // DDK & SDK folks don't talk to each
                                        // other, so we have to protect
                                        // ourselves from weird cases.

        SYM_ASSERT ( sizeof ( FileInfo.FileAttributes ) ==
                     sizeof ( wAttributes ) );

        MEMSET ( &FileInfo, 0, sizeof(FileInfo) );

        FileInfo.FileAttributes = wAttributes | FILE_ATTRIBUTE_NORMAL;

        Stat = ZvSetInformationFile ( (HANDLE)hFile,
                                      &IoStatusBlock,
                                      &FileInfo,
                                      sizeof(FileInfo),
                                      FileBasicInformation );
        }

                                        // Close file

    if ( NT_SUCCESS ( Stat ) )
        {
        Stat = ZvClose ( hFile );
        }
    else
        {
        ZvClose ( hFile );
        }

    FileDestroyNativeNTName ( pszNTFileName );

    if ( NT_SUCCESS ( Stat ) )
        {
        return ( NOERR );
        }

    // Set last error.

    return ( ERR );
#else                                   // Win16, Win32 case (or at least non-SYM_NTK)

    auto        TCHAR   szConvertedFile[SYM_MAX_PATH];
    auto        UINT    uConvertedAttributes = wAttributes;

    ExpandAndConvertFile (lpFileName, szConvertedFile);

#if defined(SYM_WIN32)

                                        // CONVERT FA_ flags into Win32 flags - the user should be passing in
                                        // the FA_ flags!!  There is no way to tell if they are using the wrong
                                        // defines (i.e. the Win32 flags), since there is overlap!

    uConvertedAttributes = 0;
    if (wAttributes == FA_NORMAL)       // FA_NORMAL IS ZERO
        uConvertedAttributes |= FILE_ATTRIBUTE_NORMAL;
    else
        {
        if (wAttributes & FA_READ_ONLY)
            uConvertedAttributes |= FILE_ATTRIBUTE_READONLY;
        if (wAttributes & FA_HIDDEN)
            uConvertedAttributes |= FILE_ATTRIBUTE_HIDDEN;
        if (wAttributes & FA_SYSTEM)
            uConvertedAttributes |= FILE_ATTRIBUTE_SYSTEM;
        if (wAttributes & FA_ARCHIVE)
            uConvertedAttributes |= FILE_ATTRIBUTE_ARCHIVE;
        if (wAttributes & FA_TEMPORARY)
            uConvertedAttributes |= FILE_ATTRIBUTE_TEMPORARY;

                                        // There is no way to set the volume attribute using
                                        // the Win32 API
        SYM_ASSERT(!(wAttributes & FA_VOLUME));

                                        // YOU CANNOT SET THE COMPRESSED OR DIRECTORY ATTRIBUTES USING
                                        // THE WIN32 API, BUT FOR COMPLETENESS OF THE CONVERSION,
                                        // THEY ARE HERE.
                                        // The assert below will be triggered in this case.
        if (wAttributes & FA_DIR)
            uConvertedAttributes |= FILE_ATTRIBUTE_DIRECTORY;
        if (wAttributes & FA_COMPRESSED)
            uConvertedAttributes |= FILE_ATTRIBUTE_COMPRESSED;
        }

                                        // According to the documentation, the only legal attributes to set using
                                        // SetFileAttributes() include ARCHIVE, NORMAL, HIDDEN, READONLY, SYSTEM,
                                        // and TEMPORARY (and COMPRESSED is allowed but ignored).
                                        // The following ASSERT() verifies that nothing other than
                                        // those are being attempted.
    SYM_ASSERT(!(uConvertedAttributes & ~(FILE_ATTRIBUTE_ARCHIVE |
                                 FILE_ATTRIBUTE_NORMAL |
                                 FILE_ATTRIBUTE_HIDDEN |
                                 FILE_ATTRIBUTE_READONLY |
                                 FILE_ATTRIBUTE_SYSTEM |
                                 FILE_ATTRIBUTE_TEMPORARY |
                                 FILE_ATTRIBUTE_COMPRESSED ) ) );


    SYM_ASSERT(uConvertedAttributes);   // According to Yoko Vang, passing zero as the attributes to be set
                                        // can cause a blue screen (in any case, it doesn't make sense, since
                                        // a file needs to have some attributes).
                                        // Specifically, this can cause a blue screen if used on a write protected
                                        // zip disk.


    return ( SetFileAttributes(szConvertedFile, uConvertedAttributes) ? NOERR : ERR);

#elif defined(SYM_OS2)

    _chmod(szConvertedFile, wAttributes);
    if ((errno == ENOENT) || (errno == EACCES))
        return errno;
    return NOERR;

#else
    {
    auto        UINT    wRetCode;
                                        // FA_TEMPORARY and FA_COMPRESSED are NOT SUPPORTED under DOS
    wAttributes &= ~(FA_TEMPORARY | FA_COMPRESSED);

    _asm
        {
        push    ds

        lea     DX,szConvertedFile      // Get the file name
        push    ss
        pop ds
        MOV     CX,wAttributes          // Get the new attribute for this file
        mov     bl,1                    // Set attributes
#if !defined(SYM_DOSX)
        mov     ax,7143h                // Extended
        stc
        DOS
        jnc     doneFileSetAttr
        cmp     ax,7100h                // Supported?
        jne     doneFileSetAttr         // Yes. But error
#endif

        mov     ax,4301h
        DOS

doneFileSetAttr:
        pop     ds
        call    FileGetReturnCode       //  set AX to to ERR or NOERR
        mov     wRetCode,ax
        }

    return (wRetCode);
    }
#endif // #ifndef SYM_OS2
#endif // !SYM_NTK
    }
#pragma optimize("", on)

#endif // #if !defined(SYM_NLM) && !defined(SYM_VXD)

#if defined(SYM_NLM)
UINT SYM_EXPORT WINAPI NLM_FileSetAttr(LPCTSTR lpFileName, UINT wAttributes)
    {
    int     nRet;

    nRet = chmod(lpFileName, wAttributes );

/*
    LPVOID  pNull;
    DWORD   dwNullStr = 0;

    pNull = (LPVOID)&dwNullStr;
    


    // Note: wAttributes will be a ulong, not a word, setting all of Netware
    //  attribs. Caller must have supervisor rights for this to work.
    nRet = SetFileInfo((char *)lpFileName, 0, (long)wAttributes,
                    pNull, pNull, pNull, pNull, 0);
*/

    return( (UINT)nRet );
    }

#endif // defined SYM_NLM

/*----------------------------------------------------------------------*/
/* FileGetTime                                                          */
/*    This function is used to get the date and time on a file          */
/*                                                                      */
/* INPUTS                                                               */
/*   HFILE      hFile           file handle                             */
/*   UINT       uType                                                   */
/*   LPFILETIME lpFileTime                                              */
/*                                                                      */
/* RETURNS                                                              */
/*    standard                                                          */
/*----------------------------------------------------------------------*/

#if !defined(SYM_OS2) && !defined(SYM_VXD)

#pragma optimize("", off)
UINT SYM_EXPORT WINAPI FileGetTime(HFILE hFile, UINT uType, LPFILETIME lpFileTime)
{
#if defined(SYM_WIN32)

    FILETIME    ftCreation, ftLastAccess, ftLastWrite;
    FILETIME    *pftWanted;
    BOOL        bOk;


    bOk = GetFileTime((HANDLE)hFile, &ftCreation, &ftLastAccess, &ftLastWrite);
    if ( uType == TIME_LASTWRITE )
         pftWanted = &ftLastWrite;
    else if ( uType == TIME_LASTACCESS )
         pftWanted = &ftLastAccess;
    else // uType == TIME_CREATION
         pftWanted = &ftCreation;
    *lpFileTime = *pftWanted;
    return ((bOk) ? NOERR : ERR);

#elif defined(SYM_NLM)

    UINT    uRet = ERR;
    struct  stat FileStatus;
    time_t  t_Wanted;

    if (0 == fstat( hFile, &FileStatus ))
        {
        if ( uType == TIME_LASTWRITE )
            t_Wanted = FileStatus.st_mtime;
        else if ( uType == TIME_LASTACCESS )
            t_Wanted = FileStatus.st_atime;
        else // uType == TIME_CREATION
            t_Wanted = FileStatus.st_ctime;

        // NLM uses only lower DWORD and format is t_time
        lpFileTime->dwLowDateTime = t_Wanted;
        lpFileTime->dwHighDateTime = 0;

        uRet = NOERR;
        }

    return uRet;

#elif defined(SYM_NTK)

    auto NTSTATUS               status;
    auto IO_STATUS_BLOCK        IoStatusBlock;
    auto FILE_BASIC_INFORMATION FileInfo;
    auto PLARGE_INTEGER         pliFileTime;

    SYM_ASSERT ( hFile != HFILE_ERROR );
    SYM_ASSERT ( uType == TIME_CREATION ||
                 uType == TIME_LASTACCESS ||
                 uType == TIME_LASTWRITE );
    SYM_ASSERT ( lpFileTime );
    SYM_VERIFY_BUFFER ( lpFileTime, sizeof(*lpFileTime) );

    status = ZvQueryInformationFile ( (HANDLE)hFile,
                                      &IoStatusBlock,
                                      &FileInfo,
                                      sizeof(FileInfo),
                                      FileBasicInformation );

    if ( NT_SUCCESS ( status ) )
        {
        switch ( uType )
            {
            case TIME_CREATION:
                pliFileTime = &FileInfo.CreationTime;
                break;
            case TIME_LASTACCESS:
                pliFileTime = &FileInfo.LastAccessTime;
                break;
            case TIME_LASTWRITE:
                pliFileTime = &FileInfo.LastWriteTime;
                break;
//            case TIME_CHANGE:
//                pliFileTime = &FileInfo.ChangeTime;
//                break;
            default:
                pliFileTime = &FileInfo.CreationTime;
            }

        lpFileTime->dwLowDateTime  = pliFileTime->LowPart;
        lpFileTime->dwHighDateTime = pliFileTime->HighPart;

        return ( NOERR );
        }

    // Set last error.

    return ( ERR );

#else
    auto        UINT    wRetCode;
    auto        WORD    wDate, wTime;

    _asm
        {
        MOV     BX,hFile                // Point to the file info block
        MOV     ah,57h                  // Get date and time
        mov     al,BYTE PTR uType       // Get the type of date/time wanted
        DOS
        call    FileGetReturnCode       //  set AX to to ERR or NOERR

        MOV     wDate,DX                // Store date
        MOV     wTime,CX                // Store the time

        mov     wRetCode,ax
        }

    DosTimeToFileTime(wDate, wTime, lpFileTime);

    return (wRetCode);
#endif
}
#pragma optimize("", on)

/*----------------------------------------------------------------------*/
/* FileSetTime                                                          */
/*    This function is used to get the date and time on a file          */
/*                                                                      */
/* INPUTS                                                               */
/*   HFILE      hFile           file handle                             */
/*   UINT       uType                                                   */
/*   LPFILETIME lpFileTime                                              */
/*                                                                      */
/* RETURNS                                                              */
/*    standard                                                          */
/*----------------------------------------------------------------------*/

#pragma optimize("", off)
UINT SYM_EXPORT WINAPI FileSetTime(HFILE hFile, UINT uType, LPFILETIME lpFileTime)
{
#if defined(SYM_WIN32)

    FILETIME    *pftCreation, *pftLastAccess, *pftLastWrite;


    pftCreation = pftLastAccess = pftLastWrite = NULL;
    if ( uType == TIME_LASTWRITE )
         pftLastWrite = lpFileTime;
    else if ( uType == TIME_LASTACCESS )
         pftLastAccess = lpFileTime;
    else // uType == TIME_CREATION
         pftCreation = lpFileTime;
    return ( SetFileTime((HANDLE)hFile, pftCreation,
                                        pftLastAccess,
                                        pftLastWrite) ? NOERR : ERR);

#elif defined(SYM_NLM)

    (void) hFile;
    (void) uType;
    (void) lpFileTime;

    return( NOERR );

#elif defined(SYM_NTK)

    auto IO_STATUS_BLOCK        IoStatusBlock;
    auto FILE_BASIC_INFORMATION FileInfo;
    auto PLARGE_INTEGER         pliFileTime;
    auto NTSTATUS               Stat;

    SYM_ASSERT ( hFile != HFILE_ERROR );
    SYM_ASSERT ( uType == TIME_CREATION ||
                 uType == TIME_LASTACCESS ||
                 uType == TIME_LASTWRITE );
    SYM_ASSERT ( lpFileTime );
    SYM_VERIFY_BUFFER ( lpFileTime, sizeof(*lpFileTime) );

    MEMSET ( &FileInfo, 0, sizeof(FileInfo) );

    switch ( uType )
        {
        case TIME_CREATION:
            pliFileTime = &FileInfo.CreationTime;
            break;
        case TIME_LASTACCESS:
            pliFileTime = &FileInfo.LastAccessTime;
            break;
        case TIME_LASTWRITE:
            pliFileTime = &FileInfo.LastWriteTime;
            break;
//            case TIME_CHANGE:
//                pliFileTime = &FileInfo.ChangeTime;
//                break;
        default:
            pliFileTime = &FileInfo.CreationTime;
        }

    pliFileTime->LowPart  = lpFileTime->dwLowDateTime;
    pliFileTime->HighPart = lpFileTime->dwHighDateTime;

    Stat = ZvSetInformationFile ( (HANDLE)hFile,
                                  &IoStatusBlock,
                                  &FileInfo,
                                  sizeof(FileInfo),
                                  FileBasicInformation );


    if ( NT_SUCCESS ( Stat ) )
        {
        return ( NOERR );
        }

    // Set last error.

    return ( ERR );

#else

    auto        UINT    wRetCode;
    auto        WORD    wDate, wTime;


    FileTimeToDosTime(lpFileTime, &wDate, &wTime);

    _asm
        {
        push    ds
        MOV     BX,hFile                // Point to the file info block
        MOV     DX,wDate                // Get the new date and time
        MOV     CX,wTime                // Get the new time
        MOV     ah,57h                  // Set date and time
        mov     al,BYTE PTR uType
        inc     al                      // Set values are 1 more than Gets
        DOS
        pop     ds
        call    FileGetReturnCode       //  set AX to to ERR or NOERR
        mov     wRetCode,ax
        }

    return (wRetCode);
#endif
}
#pragma optimize("", on)

#endif // #if !defined(SYM_OS2) && !defined(SYM_VXD)

#if !defined(SYM_NLM) && !defined(SYM_NTK)

//***************************************************************************
// FileCmpDateTime
//
// Description:
//      Compares the date and time stamp of two files.
//
// NOTE:
//      This file does not cope with critical errors.  You should probably
//      disable any critical error dialogs (i.e., trapping_enabled flag)
//      before calling this.  I originally had the code here but too many
//      users of this module didn't use the critical handler so I removed it.
//
// Return Value:
//      0 == both files have the same time and date stamp or neither exists
//     <0 == File1 is older than File2 (or File1 doesn't exist)
//     >0 == File1 is newer than File2 (or File2 doesn't exist)
//
//***************************************************************************
// 11/11/1993 BARRY Function Created.
//***************************************************************************

#ifndef SYM_OS2
int SYM_EXPORT WINAPI FileCmpTime (LPCSTR lpszFile1, LPCSTR lpszFile2, UINT uType)
{
    auto        HFILE       hFile;
    auto        FILETIME    fileTime1, fileTime2;
    auto        UINT        uFile1Rv = ERR;
    auto        UINT        uFile2Rv = ERR;


    if ((hFile = FileOpen(lpszFile1, OF_READ)) != HFILE_ERROR)
        {
        uFile1Rv = FileGetTime ((HFILE) hFile, uType, &fileTime1);
        FileClose(hFile);
        }

    if ((hFile = FileOpen(lpszFile2, OF_READ)) != HFILE_ERROR)
        {
        uFile2Rv = FileGetTime ((HFILE) hFile, uType, &fileTime2);
        FileClose(hFile);
        }

    if ( uFile1Rv != NOERR && uFile2Rv != NOERR )
        return (0);
    else if ( uFile1Rv != NOERR )
        return (-1);
    else if ( uFile2Rv != NOERR )
        return (1);

    // Both files exist.  Now compare the date and time
    return ((int)FileTimeCompare(&fileTime1, &fileTime2));
}
#endif // #ifndef SYM_OS2


/*----------------------------------------------------------------------*/
/* FileFlush                                                            */
/*    This function is used to flush the file contents to disk.         */
/*    First it tries the FileCommit() function, if that fails, It       */
/*    dups the handle and closes the duped handle.  This forces DOS     */
/*    to flush all dirty data to disk.                                  */
/*                                                                      */
/* INPUTS                                                               */
/*   UINT       hFile           file handle                             */
/*                                                                      */
/* RETURNS                                                              */
/*    standard                                                          */
/*----------------------------------------------------------------------*/

#ifndef SYM_OS2

UINT SYM_EXPORT WINAPI FileFlush(HFILE hFile)
{
#if defined(SYM_WIN32)

    return ( FlushFileBuffers((HANDLE) hFile) ? NOERR : ERR );

#else
    auto        HFILE   hDupedFile;
    auto        UINT    wRetCode;


    wRetCode = (UINT)ERR;

    if (FileCommit(hFile))              // If this fails, try old method
        if (FileDup(&hDupedFile, hFile) != (UINT)ERR)
            if (FileClose(hDupedFile) != (HFILE)ERR)
                wRetCode = NOERR;

    return(wRetCode);
#endif
}


/*----------------------------------------------------------------------*/
/* FileCommit  (DOS 3.3 or greater only)                                */
/*    This function is used to flush the file contents to disk.         */
/*                                                                      */
/*    Forces all data in MS-DOS's internal buffers associated with      */
/*    the handle to be physically written to the device.                */
/*                                                                      */
/*    Also updates directory entries for the file (if changed).         */
/*                                                                      */
/* INPUTS                                                               */
/*   UINT       hFile           file handle                             */
/*                                                                      */
/* RETURNS                                                              */
/*    standard                                                          */
/*----------------------------------------------------------------------*/

#if !defined(SYM_VXD)
#pragma optimize("", off)
UINT SYM_EXPORT WINAPI FileCommit(HFILE hFile)
{
#if defined(SYM_WIN32)

    return ( FlushFileBuffers((HANDLE) hFile) ? NOERR : ERR );

#else

    UINT        wRetCode = (UINT)ERR;

    if (DOSGetVersion() >= DOS_3_30)
        {
        _asm
            {
            MOV  bx, hFile              //  Point to the file info block
            MOV  ah, 68h                //  Commit file
            DOS
            call    FileGetReturnCode   //  set AX to to ERR or NOERR
            mov     wRetCode,ax
            }
        }

    return(wRetCode);
#endif
}
#pragma optimize("", on)

/*----------------------------------------------------------------------*/
/* FileDup                                                              */
/*    Duplicates a file handle, using DOS system call.                  */
/*                                                                      */
/* INPUTS                                                               */
/*                                                                      */
/*   UINT FAR * lphDupFilePtr   Ptr. to dup file handle                 */
/*   UINT       hFile           Handle to duplicate                     */
/*                                                                      */
/* RETURNS                                                              */
/*    standard                                                          */
/*----------------------------------------------------------------------*/

#pragma optimize("", off)
UINT SYM_EXPORT WINAPI FileDup(LPHFILE lphDupFile, HFILE hFile)
{
#if defined(SYM_WIN32)
    HANDLE      hProcess;

    hProcess = GetCurrentProcess();
    return ( DuplicateHandle(hProcess, (HANDLE) hFile,
                             hProcess, (LPHANDLE) lphDupFile,
                             0, FALSE,
                             DUPLICATE_SAME_ACCESS) ? NOERR : ERR );
#else
    auto        UINT    wRetCode;

    _asm
        {
        MOV     BX,hFile                //  Get handle to DUP
        MOV     ah,45h                  //  DUP handle function call
        DOS
        mov     si,ax                   //  save duped handle
        call    FileGetReturnCode       //  set AX to to ERR or NOERR

        les     di,lphDupFile           //  Get address to dup handle
        mov     es:[di],si              //  Save duped handle
        mov     wRetCode,ax
        }

    return (wRetCode);
#endif
}
#pragma optimize("", on)



VOID SYM_EXPORT WINAPI DOSEnvSearchPathForFile(LPCSTR lpFileName, LPSTR lpRetFullPath)
{
    *lpRetFullPath = EOS;

#if defined(SYM_WIN32)
    {
    char        szFoundFile[SYM_MAX_PATH];

    if ( SearchPath(NULL, lpFileName, NULL,
                    sizeof(szFoundFile), szFoundFile, NULL) )
        {
        STRCPY(lpRetFullPath, szFoundFile);
        }
    }
#else
    {
    auto        OFSTRUCT        of;

    if (OpenFile(lpFileName, &of, OF_EXIST|OF_SHARE_DENY_NONE) != HFILE_ERROR)
        STRCPY(lpRetFullPath, of.szPathName);
    }
#endif
}
#endif // #if !defined(SYM_VXD)


#if !defined(SYM_WIN) && !defined(SYM_VXD)
//------------------------------------------------------------------------
// OpenFile
//
// Same as the Windows API 'OpenFile()', but this is for non-Windows
// platforms.
//
// NOTE: Try not to call this function, since the OFSTRUCT is obsolete.
//
// Returns: HFILE
//------------------------------------------------------------------------

HFILE SYM_EXPORT WINAPI OpenFile (
   LPCSTR  lpFileName,                  // (INPUT) File name to open
   OFSTRUCT FAR *lpOF,                  // (INPUT/OUTPUT) OpenFile Structure
   UINT         uMode)                   // (INPUT) Open style to use
{
    char         szFilePath [SYM_MAX_PATH + 1];
    LPCSTR       lpName;


    lpName = ExpandAndConvertFile(lpFileName, szFilePath);

    {
    auto        HFILE   hFile = 0;
    auto        UINT    wAttr;


    lpOF->nErrCode = 0;         // no errors detected yet

    if (uMode & OF_REOPEN)              // we don't support REOPEN
        return (HFILE_ERROR);
                                        // if there is only a filename and
                                        // extension, or the OF_SEARCH flag
                                        // is specified, search for the
                                        // full path
    if (!(NameGetType (lpName) & NGT_IS_FULL))
        {
        _SearchPath (lpName, lpOF->szPathName, sizeof(lpOF->szPathName));
        }
    else if (uMode & OF_SEARCH)
        {
        BYTE    szTempPath [SYM_MAX_PATH + 1];

        NameReturnFile (lpName, szTempPath);
        _SearchPath (szTempPath, lpOF->szPathName, sizeof (lpOF->szPathName));
        }
    else
        {
        STRNCPY (lpOF->szPathName, lpName, sizeof(lpOF->szPathName)-1);
        lpOF->szPathName[sizeof(lpOF->szPathName)-1] = EOS;
        }

    if (lpOF->szPathName[0] == EOS) // failed search
        {
        lpOF->nErrCode = 2;     // file not found
        hFile = HFILE_ERROR;
    }

    else if (uMode & OF_EXIST)          // check for file existance
        {
        uMode = FileGetAttr (lpOF->szPathName, &wAttr);

                                        // if the file exists, and it is not
                                        // a directory, set the handle to a
                                        // positive value
        if (uMode != ERR && !(wAttr & FA_DIR))
            hFile = 1;
        else                            // otherwise, set to error
            hFile = HFILE_ERROR;
        }

    else if (uMode & OF_DELETE)         // delete the file
        {
        FileDelete (lpOF->szPathName);
        }

    else if (!(uMode & OF_PARSE))       // if not parsing, continue
        {
        if (uMode & OF_CREATE)
            {
            hFile = FileCreate (lpOF->szPathName, FA_NORMAL);
            }
        else
            {                           // clear all extra flags to leave just
                                        // the file open mode
            uMode &= (OF_READ |
                      OF_WRITE |
                      OF_READWRITE |
                      OF_SHARE_COMPAT |
                      OF_SHARE_EXCLUSIVE |
                      OF_SHARE_DENY_WRITE |
                      OF_SHARE_DENY_READ |
                      OF_SHARE_DENY_NONE);

            hFile = FileOpen (lpOF->szPathName, uMode);
            }

    if (hFile == HFILE_ERROR)
        lpOF->nErrCode = NDosGetError();
        }

    return (hFile);
    }
}
#endif


#endif // #ifndef SYM_OS2

#if !defined(SYM_WIN) && !defined(SYM_OS2)

UINT LOCAL PASCAL _SearchPath (LPCSTR lpFileName, LPSTR lpBuffer, UINT wMaxSize)
{
    auto        BYTE    szPath [512];
    auto        LPSTR   lpDir;
    static      char    SZ_TOKEN[] = ";";


                                        // try current directory first
    if (NameGetType (lpFileName) & NGT_IS_FILE)
        {
        STRNCPY (lpBuffer, lpFileName, wMaxSize-1);
        lpBuffer[wMaxSize-1] = EOS;
        return (TRUE);
        }

    if (!(NameGetType (lpFileName) & NGT_IS_FULL))
        {
                                        // Search directory containing the
                                        // executable file.
        DOSGetProgramName(lpBuffer);
        NameStripFile(lpBuffer);
        NameAppendFile(lpBuffer, lpFileName);
        if (NameGetType (lpBuffer) & NGT_IS_FILE)
            return (TRUE);

                                        // search all directories on the path
        DOSGetEnvironment ("PATH", szPath, sizeof (szPath)-1);

        lpDir = STRTOK (szPath, SZ_TOKEN);

        while (lpDir)
            {
#if 0

&? was blowing up the lpBuffer because the size passed in is actually
&? 128 (this causes a huge number of bytes to be blasted in... DEN 2/20/95
            STRNCPY (lpBuffer, lpDir, wMaxSize - SYM_MAX_PATH-1);
            lpBuffer[wMaxSize - SYM_MAX_PATH - 1] = EOS;
#endif

            STRNCPY (lpBuffer, lpDir, wMaxSize - 1);
            lpBuffer[wMaxSize - 1] = EOS;

            NameAppendFile (lpBuffer, lpFileName);

            if (NameGetType (lpBuffer) & NGT_IS_FILE)
                return (TRUE);

            lpDir = STRTOK (NULL, SZ_TOKEN);
            }
        }

    *lpBuffer = EOS;
    return (FALSE);
}

#endif

#endif  // #if !defined(SYM_NLM) && !defined(SYM_NTK)

/************************************************************************
 *                                                                      *
 * Description:                                                         *
 *      Does an EnvironmentSubst() on the filename.  If WIN16, converts *
 *      it to the OEM file name.                                        *
 *                                                                      *
 * Parameters:                                                          *
 *                                                                      *
 * Return Value:                                                        *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * 07/07/1992 ED Created.                                               *
 ************************************************************************/

#if !defined(SYM_NTK)
LPSTR WINAPI ExpandAndConvertFile (LPCSTR lpszOriginal, LPSTR lpszConverted)
#ifdef SYM_OS2
{
                                        //  Not implemented - just return orig.
    STRNCPY (lpszConverted, lpszOriginal, SYM_MAX_PATH);
    return (lpszConverted);
}
#elif defined(SYM_NLM)
{
                                        //  Not implemented - just return orig.
    STRNCPY (lpszConverted, lpszOriginal, SYM_MAX_PATH);
    return (lpszConverted);
}

#else

{
                                        // Copy filename to buffer
    if ( lpszOriginal )
        {
        STRNCPY (lpszConverted, lpszOriginal, SYM_MAX_PATH);
        lpszConverted[SYM_MAX_PATH-1] = EOS;
                                        // expand any environment variables
        EnvironmentSubst(lpszConverted, SYM_MAX_PATH);
#if defined(SYM_WIN16)
        NAnsiToOem (lpszConverted, lpszConverted);          // convert to OEM
#endif
        }
    else
        *lpszConverted = EOS;

    return (lpszConverted);
}
#endif // #if !defined(SYM_NTK)

#endif      //  os2

#if !defined(SYM_NLM)

/************************************************************************
 *                                                                      *
 * Description:                                                         *
 *                                                                      *
 * Parameters:                                                          *
 *                                                                      *
 * Return Value:                                                        *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 ************************************************************************/

#if defined(SYM_WIN32)
// All other platforms call this function using macro substitution.

DWORD SYM_EXPORT WINAPI EnvironmentSubst (LPSTR lpName, UINT uSize)
{
    DWORD       dwRet;
    DWORD       dwVersion;


    dwVersion = GetVersion();
    if ( dwVersion & 0x80000000 )       // Is this Win32s?
        {
        // I already tried dynalinking to DoEnvironmentSubst() for Win32s.
        // No luck.  Not sure what we are supposed to do for the Win32s case.
#if _DEBUG
        //OutputDebugString("No replacement for ExpandEnvironmentStrings() in Win32s");
#endif
        dwRet = 0;                      // Error
        }
    else
        {
        LPSTR       lpDest;

        lpDest = GlobalAllocPtr(GHND, uSize);
        dwRet = ExpandEnvironmentStrings(lpName, lpDest, uSize);
        if ( dwRet )
            {
            STRNCPY(lpName, lpDest, uSize);
            lpName[uSize-1] = EOS;
            }
        GlobalFreePtr(lpDest);
        }
    return(dwRet);
}

#endif

#if !defined(SYM_NTK)
#define CheckForCriticalError(a) (((a)==COPY_BREAK_PRESSED||(a)<256)?(a):COPY_CRITICAL_ERROR)
#endif

int LOCAL PASCAL _FileCopy ( HFILE      hFromFileHandle,
                             HFILE      hToFileHandle,
                             BYTE       byDestDrive,
                             BREAKFUNC  lpBreakFunc,
                             PROGFUNC   lpProgFunc );

int LOCAL PASCAL __FileCopy( LPCTSTR    lpFromFile,
                             LPCTSTR    lpToFile,
                             BREAKFUNC  lpBreakFunc,
                             PROGFUNC   lpProgFunc );

/*-----------------------------------------------------------------------
 *
 * Description:
 *      Copy a file to another file.
 *
 *      If you use a BREAKFUNC in Windows, make sure you SYM_EXPORT it!!!
 *
 * Parameters:
 *      TCHAR*fromFile                  The source file
 *      TCHAR*toFile                    The destination file
 *      Boolean (*breakCheck)(void)     Function to test for user break
 *
 * Return Value:
 *      COPY_SUCCESSFUL         The copy was successful
 *      > 0                     The specified DOS error occurred
 *      COPY_BREAK_PRESSED      User interrupted the copy with Ctrl-Break
 *      COPY_MEMORY_ERROR       Buffer couldn't be allocated from memory
 *      COPY_CRITICAL_ERROR     There was a critical disk error
 *
 * See Also:
 *      FileAppend()
 *      _FileCopy()
 *
 *-----------------------------------------------------------------------
 * 02/11/1992 PETERD Function Updated.
 *----------------------------------------------------------------------*/

#ifndef SYM_OS2
int SYM_EXPORT WINAPI FileCopy( LPCTSTR lpFromFile, LPCTSTR lpToFile, BREAKFUNC lpBreakFunc )
{
    return __FileCopy( lpFromFile, lpToFile, lpBreakFunc, NULL );
}
#endif // #ifndef SYM_OS2

/*-----------------------------------------------------------------------
 *
 * Description:
 *      This function seeks to the beginning of the source and target
 *      files, truncates the target file and calls _FileCopy()
 *
 *----------------------------------------------------------------------*/

#if defined(SYM_NTK) || defined(SYM_VXD)

BOOL WINAPI FileCopyHandle (
    HFILE FromFile,
    HFILE ToFile
    )
    {
    if ( FileSeek ( FromFile, 0, SEEK_SET ) ||
         FileSeek ( ToFile, 0, SEEK_SET ) ||
         FileWrite ( ToFile, (PVOID)-1, 0 ) )
        {
        return ( FALSE );
        }

    return ( !_FileCopy(FromFile, ToFile, 0, NULL, NULL) );
    }

#endif

/*-----------------------------------------------------------------------
 *
 * Description:
 *      Copy a file to another file with updates to a progress call back.
 *
 * Parameters:
 *      TCHAR*fromFile                  The source file
 *      TCHAR*toFile                    The destination file
 *      Boolean (*breakCheck)(void)     Function to test for user break
 *      void (*ProgressProc)(..)        Progress proc pointer
 *
 * Return Value:
 *      COPY_SUCCESSFUL         The copy was successful
 *      > 0                     The specified DOS error occurred
 *      COPY_BREAK_PRESSED      User interrupted the copy with Ctrl-Break
 *      COPY_MEMORY_ERROR       Buffer couldn't be allocated from memory
 *      COPY_CRITICAL_ERROR     There was a critical disk error
 *
 * See Also:
 *      FileAppend()
 *      _FileCopy()
 *
 *-----------------------------------------------------------------------
 * 02/11/1992 PETERD Function Updated.
 *----------------------------------------------------------------------*/

#ifndef SYM_OS2
int SYM_EXPORT WINAPI FileCopyWithProgress( LPCTSTR lpFromFile, LPCTSTR lpToFile,
                                        BREAKFUNC lpBreakFunc, PROGFUNC lpProgFunc )
{
    return __FileCopy( lpFromFile, lpToFile, lpBreakFunc, lpProgFunc );
}
#endif // #ifndef SYM_OS2

/*-----------------------------------------------------------------------
 *
 * Description:
 *      Copy a file to another file.
 *
 * Parameters:
 *      TCHAR*fromFile                  The source file
 *      TCHAR*toFile                    The destination file
 *      Boolean (*breakCheck)(void)     Function to test for user break
 *      void (*ProgressProc)(..)        Progress proc pointer
 *
 * Return Value:
 *      COPY_SUCCESSFUL         The copy was successful
 *      > 0                     The specified DOS error occurred
 *      COPY_BREAK_PRESSED      User interrupted the copy with Ctrl-Break
 *      COPY_MEMORY_ERROR       Buffer couldn't be allocated from memory
 *      COPY_CRITICAL_ERROR     There was a critical disk error
 *      ERR                     see code...
 *
 * See Also:
 *      FileAppend()
 *      _FileCopy()
 *
 *-----------------------------------------------------------------------
 * 02/11/1992 PETERD Function Updated.
 *----------------------------------------------------------------------*/

#ifndef SYM_OS2
int LOCAL PASCAL __FileCopy( LPCTSTR lpFromFile, LPCTSTR lpToFile,
                             BREAKFUNC lpBreakFunc, PROGFUNC lpProgFunc )
{
#if defined(SYM_NTK)
    auto HFILE hToFile;
    auto HFILE hFromFile;
    auto int   wReturn;

    hFromFile = FileOpen (lpFromFile, READ_ONLY_FILE|OF_SHARE_DENY_NONE);
    if (hFromFile == HFILE_ERROR)
        return (ERR);

    hToFile = FileCreate (lpToFile, FA_NORMAL);
    if (hToFile == HFILE_ERROR)
        {
        FileClose ( hFromFile );
        return (ERR);
        }

    if ( FileCopySecurityInformation ( hFromFile, hToFile, DACL_SECURITY_INFORMATION ) &&
         FileCopySecurityInformation ( hFromFile, hToFile, SACL_SECURITY_INFORMATION ) )
        {
        wReturn = _FileCopy(hFromFile, hToFile, 0, lpBreakFunc, lpProgFunc);
        }
    else
        {
        wReturn = COPY_CRITICAL_ERROR;
        }

    FileClose(hFromFile);
    FileClose(hToFile);

    if (wReturn != COPY_SUCCESSFUL)
        FileDelete (lpToFile);

    return (wReturn);
#else

#if defined(SYM_WIN32)
// defined as a 32 bit value
#define FILEOPENERR  ((HFILE)-1)
#else
#define FILEOPENERR  ERR
#endif


    auto        HFILE   hToFile;                /* Destination file handle      */
    auto        HFILE   hFromFile;              /* Source file handle           */
    auto        BYTE    byDestDrive;
    auto        int     wReturn;
    auto        UINT    wSaveError;
    auto        UINT    wAttr;

    NDosSetError(0);                    // Clear it from previous errors.

    hFromFile = FileOpen (lpFromFile, READ_ONLY_FILE|OF_SHARE_DENY_NONE);

    if (hFromFile == FILEOPENERR)
        return (ERR);

    hToFile = FileCreate (lpToFile, FA_NORMAL);         /* Create lpToFile */

    if (hToFile == FILEOPENERR)
        {
        wSaveError = NDosGetError();

        if (wSaveError == ACCESS_DENIED)        /* Was access denied?   */
            {
            FileGetAttr (lpToFile, &wAttr);

            if (wAttr & FA_READ)                /* Yes, read only file? */
                {
                FileSetAttr (lpToFile, 0);      /* Clear the attribute  */

                hToFile = FileCreate (lpToFile, FA_NORMAL);     /* Try again    */
                if (hToFile == FILEOPENERR)       /* Failed again?*/
                    {
                    wSaveError = NDosGetError();
                    FileClose (hFromFile);
                    return CheckForCriticalError(wSaveError);
                    }
                }
            else
                {
                FileClose (hFromFile);
                return CheckForCriticalError(wSaveError);
                }
            }
        else
            {
            FileClose (hFromFile);
            return CheckForCriticalError(wSaveError);
            }
        }

    if (lpToFile[1] == ':')
        byDestDrive = (BYTE) CharToUpper(*lpToFile);
    else
        {
        byDestDrive = DiskGet();
        if (wSaveError = NDosGetError())                /* Failed?*/
            {
            FileClose(hFromFile);
            FileClose(hToFile);
            return CheckForCriticalError(wSaveError);
            }
        }

    wReturn = _FileCopy(hFromFile, hToFile, byDestDrive, lpBreakFunc, lpProgFunc);

    FileClose(hFromFile);
    FileClose(hToFile);

    if (wReturn != COPY_SUCCESSFUL)
        FileDelete (lpToFile);                  /* clean up if possible */

    return (wReturn);
#endif // #if defined(SYM_NTK) ... #else
}
#endif // #ifndef SYM_OS2

//---------------------------------------------------------------------------
//
// BOOL WINAPI FileCopySecurityInformation (
//      HFILE                FromObject,
//      HFILE                ToObject,
//      SECURITY_INFORMATION SecurityInformation
//      );
//
// This function copies the security information from one file to another.
// The two objects should be created with the appropriate rights in order
// for this function to succeed.
//
// Entry:
//      FromObject          - handle to a file/object containing the source
//                            of the security information to be transferred.
//      ToObject            - handle to a file/object to which the security
//                            information is to be transferred.
//      SecurityInformation - one of:
//                                OWNER_SECURITY_INFORMATION
//                                GROUP_SECURITY_INFORMATION
//                                DACL_SECURITY_INFORMATION
//                                SACL_SECURITY_INFORMATION
//
// Exit:
//      TRUE if successful, FALSE if error.
//
// Note:
//      This function should fail if the target FS does not support
//      permanent ACLs, but currently it will return TRUE.  I just don't
//      have time right now to create separate function for checking the FS
//      type.  In the long run, clients should check the FS type on their
//      own (look at how this function does it for SYM_NTK) and not attempt
//      to call FileCopySecurityInformation() unless ACLs are supported.
//
//---------------------------------------------------------------------------
#if defined(SYM_NTK)

typedef struct
    {
    NTSTATUS             Status;
    HANDLE               hObject;
    SECURITY_INFORMATION SecurityInformation;
    PSECURITY_DESCRIPTOR pSD;
    } NTSETSECURITYPARAMS, *PNTSETSECURITYPARAMS;

void __cdecl NtSetSecurityObjectKernelMode (
    PNTSETSECURITYPARAMS pParams
    )
    {
    pParams->Status = ZvSetSecurityObject ( pParams->hObject,
                                            pParams->SecurityInformation,
                                            pParams->pSD );
    }

BOOL WINAPI FileCopySecurityInformation (
    HFILE                FromObject,
    HFILE                ToObject,
    SECURITY_INFORMATION SecurityInformation
    )
    {
    auto IO_STATUS_BLOCK                       IoStatus;
    auto struct {
             union {
                 FILE_FS_DEVICE_INFORMATION    DevInfo;
                 FILE_FS_ATTRIBUTE_INFORMATION AttrInfo;
             };
             WCHAR                             szPadding[SYM_MAX_PATH];
         } FsInfo;
    auto ULONG                                 Length;
    auto PSECURITY_DESCRIPTOR                  pSD;
    auto NTSETSECURITYPARAMS                   KMParams;

    SYM_ASSERT ( sizeof(HANDLE) == sizeof(HFILE) );

                                        // Filter out remote objects (from)

    if ( ( NT_SUCCESS ( ZvQueryVolumeInformationFile ( (HANDLE)FromObject,
                                                       &IoStatus,
                                                       &FsInfo.DevInfo,
                                                       sizeof(FsInfo.DevInfo),
                                                       FileFsDeviceInformation ) ) &&
           IoStatus.Information == sizeof(FsInfo.DevInfo) &&
           ( FsInfo.DevInfo.Characteristics & FILE_REMOTE_DEVICE ) ) ||

                                        // Filter out FS' that don't support
                                        // persistent ACLs (from)

         ( NT_SUCCESS ( ZvQueryVolumeInformationFile ( (HANDLE)FromObject,
                                                       &IoStatus,
                                                       &FsInfo.AttrInfo,
                                                       sizeof(FsInfo),
                                                       FileFsAttributeInformation ) ) &&
           IoStatus.Information >= sizeof(FsInfo.AttrInfo) &&
           !( FsInfo.AttrInfo.FileSystemAttributes & FILE_PERSISTENT_ACLS ) ) ||

                                        // Filter out remote objects (to)

         ( NT_SUCCESS ( ZvQueryVolumeInformationFile ( (HANDLE)ToObject,
                                                       &IoStatus,
                                                       &FsInfo.DevInfo,
                                                       sizeof(FsInfo.DevInfo),
                                                       FileFsDeviceInformation ) ) &&
           IoStatus.Information == sizeof(FsInfo.DevInfo) &&
           ( FsInfo.DevInfo.Characteristics & FILE_REMOTE_DEVICE ) ) ||

                                        // Filter out FS' that don't support
                                        // persistent ACLs (to)

         ( NT_SUCCESS ( ZvQueryVolumeInformationFile ( (HANDLE)ToObject,
                                                       &IoStatus,
                                                       &FsInfo.AttrInfo,
                                                       sizeof(FsInfo),
                                                       FileFsAttributeInformation ) ) &&
           IoStatus.Information >= sizeof(FsInfo.AttrInfo) &&
           !( FsInfo.AttrInfo.FileSystemAttributes & FILE_PERSISTENT_ACLS ) )
       )
        {
                                        // This should be changed to FALSE
                                        // when the code above is moved to
                                        // a separate function, called before
                                        // FileCopySecurityInformation().

        return ( TRUE );
        }

                                        // Get size of requested info.

    if ( NT_SUCCESS ( ZvQuerySecurityObject ( (HANDLE)FromObject,
                                              SecurityInformation,
                                              NULL,
                                              0,
                                              &Length ) ) )
        {
        return ( FALSE );
        }

    if ( !Length )
        {
        return ( TRUE );
        }

                                        // Allocate memory for it.

    if ( ! ( pSD = (PSECURITY_DESCRIPTOR) MemAllocPtr ( GHND, Length ) ) )
        {
        return ( FALSE );
        }

                                        // Now get the real info.

    if ( !NT_SUCCESS ( ZvQuerySecurityObject ( (HANDLE)FromObject,
                                               SecurityInformation,
                                               pSD,
                                               Length,
                                               &Length ) ) )
        {
        MemFreePtr ( pSD );
        return ( FALSE );
        }

                                        // Set the info of the ToObject.
                                        // Since ZvSetSecurityObject() is
                                        // not exported, we need to manually
                                        // change the previous mode to
                                        // kernel mode and then issue
                                        // NtSetSecurityObject().

    KMParams.hObject = (HANDLE)ToObject;
    KMParams.SecurityInformation = SecurityInformation;
    KMParams.pSD = pSD;


    if ( !SYMNTK_KernelModeXfer_CallFunction ( NtSetSecurityObjectKernelMode,
                                               &KMParams ) ||
         !NT_SUCCESS ( KMParams.Status ) )
        {
        MemFreePtr ( pSD );
        return ( FALSE );
        }

                                        // Return to caller.

    MemFreePtr ( pSD );

    return ( TRUE );
    }

#endif

//---------------------------------------------------------------------------
//
// BOOL WINAPI FileCopySecurityInformation (
//      HFILE                FromObject,
//      HFILE                ToObject,
//      SECURITY_INFORMATION SecurityInformation
//      );
//
// This function copies the security information from one file to another.
// The two objects should be created with the appropriate rights in order
// for this function to succeed.
//
// Entry:
//      FromObject          - handle to a file/object containing the source
//                            of the security information to be transferred.
//      ToObject            - handle to a file/object to which the security
//                            information is to be transferred.
//      SecurityInformation - one of:
//                                OWNER_SECURITY_INFORMATION
//                                GROUP_SECURITY_INFORMATION
//                                DACL_SECURITY_INFORMATION
//                                SACL_SECURITY_INFORMATION
//
// Exit:
//      TRUE if successful, FALSE if error.
//
// Note:
//      This function should fail if the target FS does not support
//      permanent ACLs, but currently it will return TRUE.  I just don't
//      have time right now to create separate function for checking the FS
//      type.  In the long run, clients should check the FS type on their
//      own (look at how this function does it for SYM_NTK) and not attempt
//      to call FileCopySecurityInformation() unless ACLs are supported.
//
//---------------------------------------------------------------------------
#if defined(SYM_WIN32)


BOOL SYM_EXPORT WINAPI FileCopySecurityInformation (
    LPCTSTR              lpFromFile,
    LPCTSTR              lpToFile,
    SECURITY_INFORMATION SecurityInformation
    )
    {
    auto    PSECURITY_DESCRIPTOR   pSD = NULL;
    auto    DWORD                  dwLength;
    auto    BOOL                   bReturn = FALSE;    // Assume failure

    auto    DWORD                  dwErrorCode;

                                        // First, find out how much memory
                                        // we need for the copy
    if( !GetFileSecurity (
             lpFromFile,
             SecurityInformation,
             NULL,
             0,
             &dwLength) )
        {
        dwErrorCode = GetLastError();

        if (ERROR_INSUFFICIENT_BUFFER != dwErrorCode)
            {
            goto Cleanup;
            }
        }

                                        // Allocate memory for it.
    pSD = (PSECURITY_DESCRIPTOR) MemAllocPtr ( GHND, dwLength );

                                        // Check our allocation
    if ( NULL == pSD )
        {
        goto Cleanup;
        }

    if( !GetFileSecurity (
             lpFromFile,
             SecurityInformation,
             pSD,
             dwLength,
             &dwLength) )
        {
        goto Cleanup;
        }

                                        // Now that we have the security
                                        // information, we need to copy
                                        // it to the new file

    if (!SetFileSecurity (
             lpToFile,
             SecurityInformation,
             pSD) )
        {
        goto Cleanup;
        }

                                        // Now we are done.  Set success flag
    bReturn = TRUE;

Cleanup:

    if (NULL != pSD)
        {
        MemFreePtr ( pSD );
        pSD = NULL;
        }

    return bReturn;
    }

#endif // ifdef SYM_WIN32

/*-----------------------------------------------------------------------
 *
 * Description:
 *      Append a file to another file.  If the destination file does
 *      not already exist, this becomes a FileCopy().
 *
 *      If you use a BREAKFUNC in Windows, make sure you SYM_EXPORT it!!!
 *
 * Parameters:
 *      Byte *lpFromFile                        The source file
 *      Byte *lpToFile                  The destination file
 *      Boolean (*lpBreakFunc)(void)    Function to test for user break
 *
 * Return Value:
 *      COPY_SUCCESSFUL         The append was successful
 *      > 0                     The specified DOS error occurred
 *      COPY_BREAK_PRESSED      User interrupted the append with Ctrl-Break
 *      COPY_MEMORY_ERROR       Buffer couldn't be allocated from memory
 *      COPY_CRITICAL_ERROR     There was a critical disk error
 *
 * See Also:
 *      FileCopy()
 *      _FileCopy()
 *
 *-----------------------------------------------------------------------
 * 02/11/1992 PETERD Function Created.
 *----------------------------------------------------------------------*/

#if !defined(SYM_OS2) && !defined(SYM_NTK)

int SYM_EXPORT PASCAL FileAppend (LPCSTR lpFromFile, LPCSTR lpToFile, BREAKFUNC lpBreakFunc)
{
    auto        HFILE   hToFile;                /* Destination file handle      */
    auto        HFILE   hFromFile;              /* Source file handle           */
    auto        BYTE    byDestDrive;
    auto        int     wReturn;
    auto        UINT    wSaveError;

    NDosSetError( NOERR );

    if (!(NameGetType(lpToFile) & NGT_IS_FILE))
        {
        return (FileCopy(lpFromFile, lpToFile, lpBreakFunc));
        }

    hToFile = FileOpen(lpToFile, READ_WRITE_FILE|OF_SHARE_EXCLUSIVE);
    if (hToFile == ERR)
        {
        wSaveError = NDosGetError();
        return CheckForCriticalError(wSaveError);
        }

    hFromFile = FileOpen (lpFromFile, READ_ONLY_FILE|OF_SHARE_DENY_NONE);
    if (hFromFile == ERR)
        {
        wSaveError = NDosGetError();
        FileClose (hToFile);
        return CheckForCriticalError(wSaveError);
        }

    if (lpToFile[1] == ':')
        byDestDrive = (BYTE) CharToUpper(*lpToFile);
    else
        {
        byDestDrive = DiskGet();
        if (wSaveError = NDosGetError())
            {
            FileClose(hFromFile);
            FileClose(hToFile);
            return CheckForCriticalError(wSaveError);
            }
        }

    FileSeek(hToFile, 0L, SEEK_END);

    wReturn = _FileCopy (hFromFile, hToFile, byDestDrive, lpBreakFunc, NULL);

    FileClose(hFromFile);
    FileClose(hToFile);

    return (wReturn);
}
#endif // #if !defined(SYM_OS2) && !defined(SYM_NTK)

/*-----------------------------------------------------------------------
 *
 * Description:
 *      This is the internal copying engine used by FileCopy and FileAppend.
 *      The actual file copying is performed here.
 *
 * Parameters:
 *      int hFromFile     Handle of source file
 *      int hToFile       Handle of destination file
 *      BYTE byDestDrive  Ignored.  Don't change _FileCopy() to rely on it.
 *      Boolean (*lpBreakFunc)(void)    Function to test for user break
 *
 * Return Value:
 *      COPY_SUCCESSFUL         The append was successful
 *      > 0                     The specified DOS error occurred
 *      COPY_BREAK_PRESSED      User interrupted the append with Ctrl-Break
 *      COPY_MEMORY_ERROR       Buffer couldn't be allocated from memory
 *      COPY_CRITICAL_ERROR     There was a critical disk error
 *
 * See Also:
 *      FileCopy()
 *      FileAppend()
 *
 *-----------------------------------------------------------------------
 * 02/11/1992 PETERD Function Created.
 *----------------------------------------------------------------------*/

#if !defined(SYM_OS2)
int LOCAL PASCAL _FileCopy (HFILE hFromFile, HFILE hToFile, BYTE byDestDrive,
                            BREAKFUNC lpBreakFunc, PROGFUNC lpProgFunc)
{
#ifdef SYM_X86
    const UINT MAX_BUFFER_SIZE = 0x1000; /* 4K is all we want to try to read at once */
#else
    const UINT MAX_BUFFER_SIZE = 0xFC00; /* ~64K is max that can be read at once */
#endif

    LPBYTE      lpBlock = NULL;          /* far memory buffer for the file */
    DWORD       dwFileSize = 0;          /* file size */
    UINT        uiBufferSize = MAX_BUFFER_SIZE;        /* actual buffer size used */
    HGLOBAL     hBlock = NULL;
    DWORD       dwTotalWritten = 0L;

    DWORD       dwBytesRemaining = 0;
    UINT        uiToDoLength;           /* # bytes to read/write */


    NDosSetError(0);                    // Clear it from previous errors.

                                        // Get File Size
    dwFileSize = FileLength (hFromFile);

    if (dwFileSize == 0)                // Otherwise, empty files give error
        return (COPY_SUCCESSFUL);

#if !defined(SYM_VXD) && !defined(SYM_NTK) && !defined( SYM_WIN ) && !defined( SYM_X86 )
    {
    DWORD dwFree = 0;
    DWORD dwLargest = 0;
    MemAvail( &dwFree, &dwLargest );

    if ( dwLargest == 0 )
        return (COPY_MEMORY_ERROR);

    if ( dwLargest < MAX_BUFFER_SIZE )
        uiBufferSize = (UINT)dwLargest;
    }
#endif

    if ( dwFileSize < uiBufferSize )
        uiBufferSize = (UINT)dwFileSize;

                                        // If VMM can't swap, dwBlockSize
                                        // can fail.  So, try until success
    while ( hBlock == NULL )
        {
        hBlock = MemAlloc (GHND, uiBufferSize);

        if ( hBlock == NULL )
            {
            if ( uiBufferSize <= 1024 )
                break;                  // We're not going to have enough memory.
            uiBufferSize -= 1024;
            }
        }

    if (hBlock == NULL)
        return (COPY_MEMORY_ERROR);

    lpBlock = (LPBYTE) MemLock (hBlock);

    if (lpBlock == NULL)
        {
        MemFree (hBlock);
        return (COPY_MEMORY_ERROR);
        }

                                        // Reaching here, we know file size != 0.
    dwBytesRemaining = dwFileSize;

    uiToDoLength = uiBufferSize;

    while ( dwBytesRemaining != 0 )
        {
        UINT uiActualLength;         /* # bytes actually read */

// *************************************************************
// If you use a BREAKFUNC in Windows, make sure you SYM_EXPORT it!!!
// *************************************************************

#if !defined(SYM_NTK) && !defined(SYM_VXD)
        if (lpBreakFunc != NULL && lpBreakFunc ())
            {
            MemUnlock( hBlock, lpBlock );
            MemFree( hBlock );
            return CheckForCriticalError( COPY_BREAK_PRESSED );
            }

// ************************************************************
// If you use a PROGFUNC in Windows, make sure you SYM_EXPORT it!!!
// ************************************************************

        if (lpProgFunc != NULL)                     // update progress bar
            lpProgFunc(dwTotalWritten, dwFileSize);

#endif

        if ( dwBytesRemaining < uiBufferSize )
            uiToDoLength = (UINT)dwBytesRemaining;

        uiActualLength = FileRead (hFromFile, lpBlock, uiToDoLength);

        if ( uiActualLength != uiToDoLength )
            {
            MemUnlock( hBlock, lpBlock );
            MemFree( hBlock );
            return COPY_READ_ERROR;
            }

        uiActualLength = FileWrite( hToFile, lpBlock, uiToDoLength );

        if ( uiActualLength != uiToDoLength )       // check for disk full
            {
            MemUnlock( hBlock, lpBlock );
            MemFree( hBlock );
            return COPY_DISK_IS_FULL;
            }

        dwTotalWritten += uiActualLength;

        dwBytesRemaining -= uiActualLength;
        }                               // end while

    if (lpProgFunc != NULL)
        lpProgFunc(dwTotalWritten, dwFileSize );    // set progress bar to 100%

    MemUnlock (hBlock, lpBlock);
    MemFree (hBlock);

    /*                                                              *
     * Get the date and time of the source File file and set them   *
     * to the destination file.                                     *
     *                                                              */

    {
    FILETIME    fileTime;
    FileGetTime (hFromFile, TIME_LASTWRITE, &fileTime);
    FileSetTime (hToFile, TIME_LASTWRITE, &fileTime);

    FileGetTime (hFromFile, TIME_LASTACCESS, &fileTime);
    FileSetTime (hToFile, TIME_LASTACCESS, &fileTime);
    }

    return (COPY_SUCCESSFUL);
}

/************************************************************************
 *                                                                      *
 * Description:                                                         *
 *      Moves a file, using FileRename if the source and dest are on    *
 *      the same drive, otherwise using FileCopy/FileDelete.            *
 *                                                                      *
 * Parameters:                                                          *
 *                                                                      *
 * Return Value:                                                        *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * 02/08/1993 ED Function Created.                                      *
 ************************************************************************/

UINT SYM_EXPORT WINAPI FileMove (LPCTSTR lpSource, LPCTSTR lpDest)
{
    BOOL        bSame, bSource, bDest;
    UINT        wRet = (UINT)ERR;

                                        // check for drive specs
    bSource = NameHasDriveSpec(lpSource);
    bDest = NameHasDriveSpec(lpDest);

                                        // if both paths have drive specs,
                                        // see if they are the same drive
    if (bSource && bDest)
        bSame = (BOOL)(CharToUpper (*lpSource) == CharToUpper (*lpDest));

                                        // if neither has a drive spec,
                                        // assume they are on the same drive
    else if (!bSource && !bDest)
        bSame = TRUE;
                                        // source has a drive, but not dest,
                                        // check if default drive is same as
                                        // the source
    else if (bSource)
#if defined(SYM_NTK)
        bSame = FALSE;
#else
        bSame = (BOOL)(DiskGet() == CharToUpper (*lpSource));
#endif

                                        // dest has a drive, but not source,
                                        // check if default drive is same as
                                        // the dest
    else
#if defined(SYM_NTK)
        bSame = FALSE;
#else
        bSame = (BOOL)(DiskGet() == CharToUpper (*lpDest));
#endif


    if (bSame)                          // if the same drive, just use rename
        {
        wRet = FileRename (lpSource, lpDest);
        }
    else
        {                               // if not same drive, copy and delete
#ifdef SYM_WIN32
        if (CopyFile(lpSource, lpDest, FALSE))
#else
        if (__FileCopy (lpSource, lpDest, NULL, NULL) == COPY_SUCCESSFUL)
#endif
            wRet = FileDelete (lpSource);
        else
            wRet = (UINT)ERR;
        }

    return (wRet);
}


/************************************************************************
 *                                                                      *
 * Description:                                                         *
 *      DWORD PASCAL FileHugeRead(HFILE hFile, const void _huge *hpBuffer,  *
 *                         DWORD dwBytes)                               *
 *                                                                      *
 * Parameters:                                                          *
 *                                                                      *
 * Return Value:                                                        *
 *      Returns the number of bytes actually read, or -1 for error.     *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * 02/20/1993 BRAD Created.                                             *
 ************************************************************************/

#if !defined(SYM_WIN32) && !defined(SYM_VXD)
DWORD SYM_EXPORT WINAPI FileHugeRead(HFILE hFile,
                                 void _huge *hpBuffer,
                                 DWORD dwBytes)
{
    WORD  wBytesToRead;
    WORD  wBytesRead;
    DWORD dwTotalBytesRead = 0;
    LPBYTE lpNew;


    for (wBytesRead = 0; dwBytes && (wBytesRead != -1); dwBytes -= (DWORD)wBytesRead)
        {
    // We must access the buffer in block sizes that are a power
        // of two but less than 64K or we will run into problems with
        // the DOS extender.
        wBytesToRead = (WORD)min((DWORD)0x8000, dwBytes);

    lpNew = MemoryNormalizeAddress(hpBuffer);
        wBytesRead = FileRead(hFile, lpNew, wBytesToRead);
    FreeNormalizedAddress(lpNew);

        if (wBytesRead == ERR)
        {
            dwTotalBytesRead = (DWORD) ERR;
            break;
            }

        dwTotalBytesRead += wBytesRead;
                                        // We have read all of the file
        if (wBytesRead != wBytesToRead)
            break;

    hpBuffer = (char _huge *) hpBuffer + wBytesRead;
        }

   return(dwTotalBytesRead);
}
#endif                                  // #ifndef SYM_WIN32 && !defined(SYM_VXD)


/************************************************************************
 *                                                                      *
 * Description:                                                         *
 *      DWORD PASCAL FileHugeWrite(HFILE hFile, const void _huge *hpBuffer, *
 *                         DWORD dwBytes)                               *
 *                                                                      *
 * Parameters:                                                          *
 *                                                                      *
 * Return Value:                                                        *
 *      Returns the number of bytes actually written, or -1 for error.  *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * 02/20/1993 BRAD Created.                                             *
 ************************************************************************/

#if !defined(SYM_WIN32) && !defined(SYM_VXD)
DWORD SYM_EXPORT WINAPI FileHugeWrite(HFILE hFile,
                                  void _huge *hpBuffer,
                                  DWORD dwBytes)
{
    WORD   wBytesToWrite;
    WORD   wBytesWritten;
    DWORD  dwTotalBytesWritten = 0;
    LPBYTE lpNew;

    for (wBytesWritten = 0; dwBytes && (wBytesWritten != -1); dwBytes -= (DWORD)wBytesWritten)
        {
    // We must access the buffer in block sizes that are a power
        // of two but less than 64K or we will run into problems with
        // the DOS extender.
        wBytesToWrite = (WORD)min(0x8000, dwBytes);

    lpNew = MemoryNormalizeAddress(hpBuffer);
        wBytesWritten = FileWrite(hFile, lpNew, wBytesToWrite);
    FreeNormalizedAddress(lpNew);

        if (wBytesWritten == ERR)
            {
            dwTotalBytesWritten = (DWORD) ERR;
            break;
            }

        dwTotalBytesWritten += wBytesWritten;
                                        // We have probably hit disk full
        if (wBytesWritten != wBytesToWrite)
            break;

    hpBuffer = (char _huge *) hpBuffer + wBytesWritten;
        }

    return(dwTotalBytesWritten);
}
#endif                                  // #ifndef SYM_WIN32 && !defined(SYM_VXD)


/************************************************************************
 *                                                                      *
 * Description:                                                         *
 *      Deletes a file, by whatever means necessary.  This is just      *
 *      like FileDelete, but it will different things if the file       *
 *      is stubborn ol' pesky little varmit.                            *
 *                                                                      *
 * Parameters:                                                          *
 *                                                                      *
 * Return Value:                                                        *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * 06/07/1993 ED Function Created.                                      *
 ************************************************************************/

UINT SYM_EXPORT WINAPI FileKill (LPCTSTR lpFileName)
{
    UINT        wRet;

    wRet = FileDelete (lpFileName);     // this may be easy

    if (wRet == ERR)
        {
        FileSetAttr (lpFileName, 0);    // clear the attributes

        wRet = FileDelete (lpFileName); // try again

                        // wouldn't it be great if we could delete
                        // Novell TTS files?
        }

    return (wRet);
}


/************************************************************************
 *                                                                      *
 * Description:                                                         *
 *  This function:                                                      *
 *                 1. Overwrites the entire file with zeros -           *
 *                    uTimesToWipe times.                               *
 *                 2. Truncates the file to 0 byes in size.             *
 *                 3. Deletes the file.                                 *
 *                                                                      *
 * Parameters:                                                          *
 *  lpFilename     Full Path of the file to Wipe.                       *
 *  uTimesToWipe   The number of times to overwrite the file with zeros *
 *                                                                      *
 * Returns:                                                             *
 *  NOERR if successful, ERR otherwise.                                 *
 *                                                                      *
 ************************************************************************
 * 06/19/1997 MKEATIN Function Created.                                 *
 ************************************************************************/

UINT SYM_EXPORT WINAPI FileWipe(LPCTSTR  lpFilename, UINT uTimesToWipe)
{
    auto  HFILE  hFile            = HFILE_ERROR;
    auto  LPVOID lpvBuffer        = NULL;
    const UINT   uMaxBytesToWrite = 0xFFFC;
    auto  UINT   uBytesToWrite;
    auto  DWORD  dwFileSize;
    auto  DWORD  dwBytesLeft;

                                        // make sure we can write to the file

    if (FileSetAttr(lpFilename, FA_NORMAL) != NOERR)
    {
        goto Cleanup;
    }                                   // open the file with write access

    if ((hFile = FileOpen(lpFilename, OF_WRITE)) == HFILE_ERROR)
    {
        goto Cleanup;
    }                                   // get the file's size

    if ((dwFileSize = FileLength(hFile)) == HFILE_ERROR)
    {
        goto Cleanup;
    }                                   // allocate a buffer 0xFFFC bytes
                                        // in size
    if ((lpvBuffer = MemAllocPtr(GHND, uMaxBytesToWrite)) == NULL)
    {
        goto Cleanup;
    }                                   // set the buffer to zeros

    MEMSET(lpvBuffer, 0, uMaxBytesToWrite);

    while (uTimesToWipe)                // write zeros to that file, baby!
    {
        dwBytesLeft = dwFileSize;
                                        // seek to the beginning of our file
        if (FileSeek(hFile, 0, SEEK_SET) == HFILE_ERROR)
        {
            goto Cleanup;
        }
                                        // we cannot write more than
        while (dwBytesLeft)             // 0xFFFC bytes at a time
        {
            uBytesToWrite = dwBytesLeft > uMaxBytesToWrite ?
                            uMaxBytesToWrite :
                            (UINT) dwBytesLeft;

            if (FileWrite(hFile, lpvBuffer, uBytesToWrite) != uBytesToWrite)
            {
                goto Cleanup;
            }

            dwBytesLeft -= uBytesToWrite;
        }
        uTimesToWipe--;
    }                                   // seek to the beginning of our file
                                        // for truncation purposes
    if (FileSeek(hFile, 0, SEEK_SET) == HFILE_ERROR)
    {
        goto Cleanup;
    }

#if defined(SYM_WIN32)                  // truncate the file

    if (!SetEndOfFile((HANDLE) hFile))
    {
        goto Cleanup;
    }
                                        // writing 0 bytes will truncate the
#else                                   // to file to 0

    if (FileWrite(hFile, lpvBuffer, 0) == HFILE_ERROR)
    {
        goto Cleanup;
    }

#endif
                                        // Free our buffer and close the file
    MemFreePtr(lpvBuffer);
    FileClose(hFile);
                                        // Delete that file, baby!
    return (FileDelete(lpFilename));

Cleanup:
                                        // Something went wrong if we're
    if (hFile != HFILE_ERROR)           // here
    {
        FileClose(hFile);
    }

    if (lpvBuffer)
    {
        MemFreePtr(lpvBuffer);
    }

    return (ERR);
}

#endif                                  // #ifndef SYM_OS2

#endif // #if !defined(SYM_NLM)

#ifdef SYM_OS2
/************************************************************************
 *                                                                      *
 * Description:                                                         *
 *      Creates a file with the specified attributes.  This code is     *
 *      only for OS/2 - other platforms have this native.               *
 *                                                                      *
 * Parameters:                                                          *
 *      lpFileName - pointer to szFileName to be created.               *
 *      wAttribute - attribute to set file open with.                   *
 *                                                                      *
 * Return Value:                                                        *
 *      file handle if successful, HFILE_ERROR if not.                  *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * 11/11/1994 - Created by Tony                                         *
 ************************************************************************/

UINT SYM_EXPORT WINAPI FileCreate (LPCSTR lpFileName, USHORT wAttribute)
{
    UINT    hFile;

    switch (wAttribute)
        {
        case 0:                         //  Normal - read/write with no restriction
            return _rtl_creat(lpFileName, 0);
            break;
        case 1:                         //  Read-only, can not write
            return _rtl_creat(lpFileName, FA_RDONLY);
            break;
        case 2:                         //  Hidden - no directory search
            return _rtl_creat(lpFileName, FA_HIDDEN);
            break;
        case 3:
            return _rtl_creat(lpFileName, FA_SYSTEM);
            break;

        default:
            return HFILE_ERROR;
        }
    return HFILE_ERROR;
}
#endif  //  #ifdef SYM_OS2


#ifdef SYM_NLM
 /*-----------------------------------------------------------------------
 *
 * Description:
 *      Copy a file to another file.
 *
 *      If you use a BREAKFUNC in Windows, make sure you SYM_EXPORT it!!!
 *
 * Parameters:
 *      TCHAR*fromFile                  The source file
 *      TCHAR*toFile                    The destination file
 *      Boolean (*breakCheck)(void)     Function to test for user break
 *
 * Return Value:
 *      COPY_SUCCESSFUL         The copy was successful
 *      > 0                     The specified DOS error occurred
 *      COPY_BREAK_PRESSED      User interrupted the copy with Ctrl-Break
 *      COPY_MEMORY_ERROR       Buffer couldn't be allocated from memory
 *      COPY_CRITICAL_ERROR     There was a critical disk error
 *
 * See Also:
 *      FileAppend()
 *      _FileCopy()
 *
 *-----------------------------------------------------------------------
 * 09/04/1997 DDREW Function Updated.
 *----------------------------------------------------------------------*/
#define LOCAL_BUFFER_SIZE 4096

int SYM_EXPORT WINAPI FileCopy(
    LPCTSTR     lpFromFile,
    LPCTSTR     lpToFile,
    BREAKFUNC lpBreakFunc )
{
    FILETIME    fileTime;
    HFILE       hFromFile, hToFile;
    BYTE        byTmp[ LOCAL_BUFFER_SIZE ];
    LPBYTE      lpBlock = &byTmp;
    DWORD       dwFileSize = 0;
    DWORD       dwTotalWritten = 0L;
    DWORD       dwBytesRemaining = 0;
    UINT        uiBufferSize = LOCAL_BUFFER_SIZE;
    UINT        uiToDoLength;
    
// don't do anything unless file strings seem ok
//    if( lpToFile && lpFromFile && *lpToFile && *lpFromFile )
//        return (ERR);

    hFromFile = FileOpen (lpFromFile, READ_ONLY_FILE|OF_SHARE_DENY_NONE);
    if (hFromFile == HFILE_ERROR)
        return (ERR);

    hToFile = FileCreate (lpToFile, FA_NORMAL);
    if (hToFile == HFILE_ERROR)
        {
        FileClose ( hFromFile );
        return (ERR);
        }

    uiToDoLength = uiBufferSize;
    dwFileSize = FileLength (hFromFile);
    dwBytesRemaining = dwFileSize;
    while ( dwBytesRemaining != 0 )
        {
        UINT uiActualLength;         /* # bytes actually read */

        if ( dwBytesRemaining < uiBufferSize )
            {
            uiToDoLength = (UINT)dwBytesRemaining;
            }

        uiActualLength = FileRead (hFromFile, lpBlock, uiToDoLength);

        if ( uiActualLength != uiToDoLength )
            {
            FileClose(hFromFile);
            FileClose(hToFile);
            return COPY_READ_ERROR;
            }

        uiActualLength = FileWrite( hToFile, lpBlock, uiToDoLength );

        if ( uiActualLength != uiToDoLength )       // check for disk full
            {
            FileClose(hFromFile);
            FileClose(hToFile);
            return COPY_DISK_IS_FULL;
            }

        dwTotalWritten += uiActualLength;
        dwBytesRemaining -= uiActualLength;
        }   // end while
  
    FileClose(hFromFile);
    FileClose(hToFile);

    // Get the date and time of the source File file and set them
    // to the destination file.
    FileGetTime (hFromFile, TIME_LASTWRITE, &fileTime);
    FileSetTime (hToFile, TIME_LASTWRITE, &fileTime);

    FileGetTime (hFromFile, TIME_LASTACCESS, &fileTime);
    FileSetTime (hToFile, TIME_LASTACCESS, &fileTime);

    return(COPY_SUCCESSFUL);
}


//***************************************************************************
// FileRead()
//***************************************************************************

LONG FileRead(int hHandle, void *cBuffer, LONG lLen)
{
    ThreadSwitchWithDelay();
    return read(hHandle, cBuffer, lLen);
}


//***************************************************************************
// FileWrite()
//***************************************************************************

LONG FileWrite(int hHandle, void *cBuffer, LONG lLen)
{
    ThreadSwitchWithDelay();
    return write(hHandle, cBuffer, lLen);
}


//***************************************************************************
// FileGetTimeByName()
//
// Description:
//      Gets file date and time (NLM only). Same as FileGetTime() but takes
// file path instead of handle. NetWare functions accessing file date/time
// take paths; we don't have to open the file to get date/time.
//
// Parameters:
//      LPCSTR lpPath               [in] file path
//      UINT uType                  [in] date/time type
//      LPFILETIME lpFileTime       [out] date/time
//
// Return Value:
//      ERR or NOERR
//
// See Also:
//
//***************************************************************************
// 05/02/1995 GDZIECI Function Created.
//***************************************************************************

UINT SYM_EXPORT WINAPI FileGetTimeByName(LPCSTR lpPath, UINT uType, LPFILETIME lpFileTime)
{
    UINT    uRet = ERR;
    struct  stat FileStatus;
    time_t  t_Wanted;

    if (0 == stat( lpPath, &FileStatus ))
        {
        if ( uType == TIME_LASTWRITE )
            t_Wanted = FileStatus.st_mtime;
        else if ( uType == TIME_LASTACCESS )
            t_Wanted = FileStatus.st_atime;
        else // uType == TIME_CREATION
            t_Wanted = FileStatus.st_ctime;

        // NLM uses only lower DWORD and format is t_time
        lpFileTime->dwLowDateTime = t_Wanted;
        lpFileTime->dwHighDateTime = 0;

        uRet = NOERR;
        }

    return uRet;
}


//***************************************************************************
// FileSetTimeByName()
//
// Description:
//      Sets file date and time (NLM only). Same as FileSetTime() but takes
// file path instead of handle. NetWare functions accessing file date/time
// take paths; we don't have to open the file to get date/time.
//
// Parameters:
//      LPCSTR lpPath               [in] file path
//      UINT uType                  [in] date/time type
//      LPFILETIME lpFileTime       [in] date/time
//
// Return Value:
//      ERR or NOERR
//
// See Also:
//
//***************************************************************************
// 05/02/1995 GDZIECI Function Created.
//***************************************************************************

UINT SYM_EXPORT WINAPI FileSetTimeByName (LPCSTR lpPath, UINT uType, LPFILETIME lpFileTime)
{
    struct _DOSDate fileDate;
    struct _DOSTime fileTime;
    struct  ModifyStructure Modify;
    LONG    modifyBits;
    UINT    uRet = ERR;

    _ConvertTimeToDOS (lpFileTime->dwLowDateTime, &fileDate, &fileTime);

    if ( uType == TIME_LASTWRITE )
        {
        Modify.MLastUpdatedDate = *((WORD *)&fileDate);
        Modify.MLastUpdatedTime = *((WORD *)&fileTime);

        modifyBits = MLastUpdatedDateBit | MLastUpdatedTimeBit;
        }
    else if ( uType == TIME_LASTACCESS )
        {
        Modify.MLastAccessedDate = *((WORD *)&fileDate);

        modifyBits = MLastAccessedDateBit;
        }
    else // uType == TIME_CREATION
        {
        Modify.MCreateDate = *((WORD *)&fileDate);
        Modify.MCreateTime = *((WORD *)&fileTime);

        modifyBits = MCreateDateBit | MCreateTimeBit;
        }

    if (!ChangeDirectoryEntry((LPSTR)lpPath, &Modify, modifyBits, 0))
        uRet = NOERR;

    return uRet;
}
#endif  //  #ifdef SYM_NLM

#if defined(SYM_NTK)
//***************************************************************************
// HANDLE FileOpenRootHandle(LPCTSTR lpszName)
//
// Description:
//      This function opens a handle to the root location of lpszFile.  If
//      lpszName points to a fully qualified non-UNC name, then
//      the opened handle will be that of "\DosDevices\d:\".  If lpszName
//      points to a UNC name, then the opened handle will be that of
//      "\DosDevices\UNC\server\share\".  If lpszName points to a relative
//      path somewhere on a local hard drive, then this function will return
//      NULL.
//
// Parameters:
//      LPCTSTR lpszName            [in] name of file/directory
//
// Return Value:
//      Returns an opened handle or NULL (see Description).  NULL will be
//      returned also when there is an error.
//
// See Also: FileCloseRootHandle(), FileRelativeToRootName(),
//           FileCopyRootName()
//
//***************************************************************************
HANDLE FileOpenRootHandle ( LPCTSTR lpszName )
    {
    auto HANDLE            hResult;
    auto UNICODE_STRING    FileName;
    auto OBJECT_ATTRIBUTES ObjectAttributes;
    auto IO_STATUS_BLOCK   IoStatusBlock;
    auto TCHAR             szRoot[16];
    auto PTSTR             pszLongRoot;

    SYM_ASSERT ( lpszName );
    SYM_VERIFY_STRING ( lpszName );

                                        // If lpszName starts with "\??\",
                                        // then skip it.

    if ( lpszName[0] == _T('\\') &&
         lpszName[1] == _T('?')  &&
         lpszName[2] == _T('?')  &&
         lpszName[3] == _T('\\') )
        {
        lpszName += 4;
        }

                                        // Based on lpszName, create FileName
                                        // for root directory.

    if ( *lpszName != '\0' && lpszName[1] == ':' && lpszName[2] == '\\' )
        {
                                        // it's a fully qualified path
        STRCPY ( szRoot, _T("\\DosDevices\\d:\\") );
        szRoot[12] = *lpszName;
        RtlInitUnicodeString ( &FileName, szRoot );
        pszLongRoot = NULL;
        }
    else if ( *lpszName == '\\' && lpszName[1] == '\\' )
        {
                                        // it's a UNC name
        auto int iCount, iDelimiters;
        for ( iCount = 2, iDelimiters = 0;
              iDelimiters < 2 && lpszName[iCount] != '\0';
              lpszName[iCount++] == '\\' && iDelimiters++ );
        if ( iDelimiters != 2 ||
             !(pszLongRoot=(PTSTR)MemAllocPtr(GHND,(iCount+15)*sizeof(TCHAR))) )
            {
            return ( (HANDLE)NULL );
            }
        STRCPY ( pszLongRoot, _T("\\DosDevices\\UNC") );
        MEMCPY ( &pszLongRoot[15], &lpszName[1], (iCount-1)*sizeof(TCHAR) );
        pszLongRoot[14+iCount] = '\0';
        RtlInitUnicodeString ( &FileName, pszLongRoot );
        }
    else
        {
                                        // lpszName is invalid
        return ( (HANDLE)NULL );
        }

                                        // Initialize object attributes

    InitializeObjectAttributes ( &ObjectAttributes,
                                 &FileName,
                                 OBJ_CASE_INSENSITIVE,
                                 NULL,
                                 NULL );

                                        // Open handle

    if ( !NT_SUCCESS ( ZvOpenFile ( &hResult,
                                    SYNCHRONIZE | FILE_TRAVERSE,
                                    &ObjectAttributes,
                                    &IoStatusBlock,
                                    FILE_SHARE_READ | FILE_SHARE_WRITE,
                                    FILE_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT ) ) )
        {
        hResult = (HANDLE) NULL;
        }

    if ( pszLongRoot )
        {
        MemFreePtr ( pszLongRoot );
        }

    return ( hResult );
    }

//***************************************************************************
// VOID FileCloseRootHandle(HANDLE hHandle)
//
// Description:
//      This function closes a handle opened by FileOpenRootHandle().
//
// Parameters:
//      HANDLE hHandle              [in] any result from FileOpenRootHandle()
//
// Return Value:
//      -
//
// See Also: FileOpenRootHandle(), FileRelativeToRootName(),
//           FileCopyRootName()
//
//***************************************************************************
VOID FileCloseRootHandle ( HANDLE hHandle )
    {
    if ( hHandle )
        {
        ZvClose ( hHandle );
        }
    }

//***************************************************************************
// LPCTSTR FileRelativeToRootName ( LPCTSTR lpszName )
//
// Description:
//      This function examines the file name at lpszName and returns a
//      pointer to a part of it which could be opened using the root handle
//      returned by FileOpenRootHandle().  The name should be in UNC, or a
//      fully qualified non-UNC (d:\path\file) format.  If the name starts
//      at a relative path, then lpszName will be returned.
//
// Parameters:
//      LPCTSTR lpszName - name of file
//
// Return Value:
//      LPCTSTR - pointer into lpszName.
//
// See Also: FileOpenRootHandle(), FileCloseRootHandle(), FileCopyRootName()
//
//***************************************************************************
LPCTSTR FileRelativeToRootName ( LPCTSTR lpszName )
    {
    SYM_ASSERT ( lpszName );
    SYM_VERIFY_STRING ( lpszName );

                                        // If lpszName starts with "\??\",
                                        // then skip it.

    if ( lpszName[0] == _T('\\') &&
         lpszName[1] == _T('?')  &&
         lpszName[2] == _T('?')  &&
         lpszName[3] == _T('\\') )
        {
        lpszName += 4;
        }

                                        // Parse lpszName.

    if ( *lpszName != '\0' && lpszName[1] == ':' && lpszName[2] == '\\' )
        {
                                        // if it's a fully qualified path,
                                        // then skip the "d:\" part
        return ( lpszName + 3 );
        }
    else if ( *lpszName == '\\' && lpszName[1] == '\\' )
        {
                                        // if it's a UNC name, then skip
                                        // the "\\volume\share\" part.
        auto int iCount, iDelimiters;
        for ( iCount = 2, iDelimiters = 0;
              iDelimiters < 2 && lpszName[iCount] != '\0';
              lpszName[iCount++] == '\\' && iDelimiters++ );
        if ( iDelimiters == 2 )
            {
            return ( lpszName + iCount );
            }
        }

    return ( lpszName );
    }

//***************************************************************************
// UINT FileCopyRootName ( LPCTSTR lpszName, LPTSTR lpszDest )
//
// Description:
//      This function copies the initial "d:\" or "\\server\share\" parts
//      of lpszName to lpszDest.  If lpszName does not start with one of
//      these strings, or the string to be copied is longer than
//      SYM_MAX_PATH-1, an empty "" string will be copied.  In this case,
//      the characters following the NULL character up to
//      lpszDest[SYM_MAX_PATH-1] will be undefined.
//
// Parameters:
//      LPCTSTR lpszName - name of file
//      LPTSTR  lpszDest - destination buffer
//
// Return Value:
//      Number of characters copied (not including terminating 0)
//
// See Also: FileOpenRootHandle(), FileCloseRootHandle(),
//           FileRelativeToRootName()
//
//***************************************************************************
UINT FileCopyRootName ( LPCTSTR lpszName, LPTSTR lpszDest )
    {
    SYM_ASSERT ( lpszName );
    SYM_VERIFY_STRING ( lpszName );
    SYM_ASSERT ( lpszDest );
    SYM_VERIFY_BUFFER ( lpszDest, SYM_MAX_PATH * sizeof(*lpszDest) );

                                        // If lpszName starts with "\??\",
                                        // then skip it.

    if ( lpszName[0] == _T('\\') &&
         lpszName[1] == _T('?')  &&
         lpszName[2] == _T('?')  &&
         lpszName[3] == _T('\\') )
        {
        lpszName += 4;
        }

                                        // Parse lpszName.

    if ( *lpszName != '\0' && lpszName[1] == ':' && lpszName[2] == '\\' )
        {
                                        // if it's a fully qualified path,
                                        // then return the "d:\" part
        lpszDest[0] = *lpszName;
        lpszDest[1] = ':';
        lpszDest[2] = '\\';
        lpszDest[3] = '\0';
        return ( 3 );
        }
    else if ( *lpszName == '\\' && lpszName[1] == '\\' )
        {
                                        // if it's a UNC name, then copy
                                        // the "\\volume\share\" part.
        auto int iCount, iDelimiters;
        for ( iCount = iDelimiters = 0;
              iCount < SYM_MAX_PATH;
              iCount++ )
            {
            if ( !lpszName[iCount] )
                {
                break;
                }
            lpszDest[iCount] = lpszName[iCount];
            if ( lpszName[iCount] == '\\' && ++iDelimiters == 4 )
                {
                lpszDest[++iCount] = '\0';
                return ( iCount );
                }
            }
        }

    lpszDest[0] = '\0';
    return ( 0 );
    }

//***************************************************************************
// PTSTR FileCreateNativeNTName (LPCTSTR lpszName)
//
// Description:
//      This function converts the fully qualified path at lpszName to
//      NT-native format.  If lpszName was formatted as "d:\path\file", the
//      resulting filename will look like "\DosDevices\d:\path\file".  If
//      lpszName was formatted as UNC "\\server\share\path\file", the
//      resulting filename will look like "\DosDevices\UNC\server\share\path
//      \file".  If lpszName points to a relative path somewhere on a local
//      hard drive, then this function will return NULL.
//
// Parameters:
//      LPCTSTR lpszName            [in] name of file/directory
//
// Return Value:
//      Returns a non-NULL pointer to allocated ASCIIZ string containing the
//      result.  If the returned value is NULL, then there was an error
//      in the conversion.
//
// See Also: FileCreateNativeNTNameEx(), FileDestroyNativeNTName()
//
// Note: Keep the common parts between this function and
//       FileCreateNativeNTNameEx() synchronized!!!
//
//***************************************************************************
PTSTR FileCreateNativeNTName ( LPCTSTR lpszName )
    {
    auto PTSTR pszResult;

    SYM_ASSERT ( lpszName );
    SYM_VERIFY_STRING ( lpszName );

                                        // If lpszName starts with "\??\",
                                        // then skip it.

    if ( lpszName[0] == _T('\\') &&
         lpszName[1] == _T('?')  &&
         lpszName[2] == _T('?')  &&
         lpszName[3] == _T('\\') )
        {
        lpszName += 4;
        }

                                        // Check the format of lpszName.

    if ( *lpszName != '\0' && lpszName[1] == ':' && lpszName[2] == '\\' )
        {
                                        // it's a fully qualified path
        if ( pszResult = MemAllocPtr ( GMEM_MOVEABLE,
                                       sizeof(WCHAR) * (STRLEN(lpszName)+13)
                                     )
           )
            {
            STRCPY ( pszResult, _T("\\DosDevices\\") );
            STRCPY ( &pszResult[12], lpszName );
            }
        }
    else if ( *lpszName == '\\' && lpszName[1] == '\\' )
        {
                                        // it's a UNC name
        if ( pszResult = MemAllocPtr ( GMEM_MOVEABLE,
                                       sizeof(WCHAR) * (STRLEN(lpszName)+15)
                                     )
           )
            {
            STRCPY ( pszResult, _T("\\DosDevices\\UNC") );
            STRCPY ( &pszResult[15], &lpszName[1] );
            }
        }
    else
        {
                                        // lpszName is invalid
        return ( NULL );
        }

    return ( pszResult );
    }

//***************************************************************************
// PTSTR FileCreateNativeNTNameEx (LPCTSTR lpszName)
//
// Description:
//      This function works just like FileCreateNativeNTName(), except that
//      it will succeed if the input string starts with "\DosDevices\" or
//      "\SystemRoot\" (the comparison is case sensitive).
//
// Parameters:
//      LPCTSTR lpszName            [in] name of file/directory
//
// Return Value:
//      Returns a non-NULL pointer to allocated ASCIIZ string containing the
//      result.  If the returned value is NULL, then there was an error
//      in the conversion.
//
// See Also: FileCreateNativeNTName(), FileDestroyNativeNTName()
//
// Note: Keep the common parts between this function and
//       FileCreateNativeNTName() synchronized!!!
//
//***************************************************************************
PTSTR FileCreateNativeNTNameEx ( LPCTSTR lpszName )
    {
    auto PTSTR pszResult;

    SYM_ASSERT ( lpszName );
    SYM_VERIFY_STRING ( lpszName );

                                        // If lpszName starts with "\??\",
                                        // then skip it.

    if ( lpszName[0] == _T('\\') &&
         lpszName[1] == _T('?')  &&
         lpszName[2] == _T('?')  &&
         lpszName[3] == _T('\\') )
        {
        lpszName += 4;
        }

                                        // Check the format of lpszName.

    if ( *lpszName != '\0' && lpszName[1] == ':' && lpszName[2] == '\\' )
        {
                                        // it's a fully qualified path
        if ( pszResult = MemAllocPtr ( GMEM_MOVEABLE,
                                       sizeof(WCHAR) * (STRLEN(lpszName)+13)
                                     )
           )
            {
            STRCPY ( pszResult, _T("\\DosDevices\\") );
            STRCPY ( &pszResult[12], lpszName );
            }
        }
    else if ( *lpszName == '\\' && lpszName[1] == '\\' )
        {
                                        // it's a UNC name
        if ( pszResult = MemAllocPtr ( GMEM_MOVEABLE,
                                       sizeof(WCHAR) * (STRLEN(lpszName)+15)
                                     )
           )
            {
            STRCPY ( pszResult, _T("\\DosDevices\\UNC") );
            STRCPY ( &pszResult[15], &lpszName[1] );
            }
        }
    else
        {
#if defined(UNICODE)
                                        // check for "\DosDevices\" or
                                        // "\SystemRoot\".

        if ( memcmp ( lpszName, L"\\DosDevices\\", 12*sizeof(WCHAR) ) &&
             memcmp ( lpszName, L"\\SystemRoot\\", 12*sizeof(WCHAR) ) )
            {
            return ( NULL );
            }

        if ( pszResult = MemAllocPtr ( GMEM_MOVEABLE,
                                       sizeof(WCHAR) * (STRLEN(lpszName)+1)
                                     )
           )
            {
            STRCPY ( pszResult, lpszName);
            }
#else

        return ( NULL );

#endif
        }

    return ( pszResult );
    }

//***************************************************************************
// VOID FileDestroyNativeNTName(PTSTR pszName)
//
// Description:
//      This function cleans up after FileCreateNativeNTName() or
//      FileCreateNativeNTNameEx().
//
// Parameters:
//      pszName - any result from FileCreateNativeNTName() or
//                FileCreateNativeNTNameEx().
//
// Return Value:
//      -
//
// See Also:
//           FileCreateNativeNTName(), FileCreateNativeNTNameEx()
//
//***************************************************************************
VOID FileDestroyNativeNTName ( PTSTR pszName )
    {
    if ( pszName )
        {
        MemFreePtr ( pszName );
        }
    }

#endif  // #if defined(SYM_NTK)

#if !defined(SYM_OS2) && !defined(SYM_VXD)
BOOL SYM_EXPORT WINAPI FileEnableFileSharing( BOOL bEnabled )
    {
    BOOL bCurrentFileSharing;

    bCurrentFileSharing = bFileSharing;

    bFileSharing = bEnabled;

    return( bCurrentFileSharing );
    }
#endif

