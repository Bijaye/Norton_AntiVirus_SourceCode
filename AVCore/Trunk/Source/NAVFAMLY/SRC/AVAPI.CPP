// Copyright 1994 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/AVAPI1/VCS/avapi.cpv   1.129   28 Apr 1997 18:13:26   MKEATIN  $
//
// Description:
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/AVAPI1/VCS/avapi.cpv  $
// 
//    Rev 1.129   28 Apr 1997 18:13:26   MKEATIN
// Included avenge.h
// 
//    Rev 1.128   28 Apr 1997 14:58:40   MKEATIN
// Removed an errant simicolon.
// 
//    Rev 1.127   28 Apr 1997 13:39:16   MKEATIN
// This file is now an api to the AVENGE scanning engine.  Severl "info" api's
// still need to be updated.
// 
//    Rev 1.126   07 Mar 1997 15:12:28   RSTANEV
// Merged revision 1.126 from NEWVIR\NAVFAMLY\AVAPI.CPP.
//
//    Rev 1.125   15 Oct 1996 15:22:28   DALLEE
// :) Finished disabling PAM based on VLINIT_NO_PAM - missed disabling
// in scan file and repair file.
//
//    Rev 1.124   14 Oct 1996 16:25:28   DALLEE
// Redid bad DBCS work in BackupInfectedFile().
// Still need to check in DBCS STRNCPY() for this to work, though.
// Depends on a STRNCPY() that takes # of bytes and doesn't leave hanging
// lead bytes.
//
//    Rev 1.123   13 Oct 1996 20:38:50   DALLEE
// Partially fixed chunkified DB backup extension code.
// CharCnt() was hanging since string pointer never advance for SB chars.
//
// Creating extensions is still munged since there's lots of confusion
// between character length and byte length (in BackupInfectedFile).
//
//    Rev 1.122   13 Oct 1996 15:14:28   DALLEE
// Corrected bad checks on FileOpen's return value. Should check vs HFILE_ERROR.
//
//    Rev 1.121   12 Oct 1996 13:47:52   DALLEE
// Added handling for VLINIT_NO_PAM flag. Disables PAM scanning.
//
//    Rev 1.120   16 Sep 1996 18:52:24   DCHI
// Added extra result pointer parameter to GetScanFileData for specifying
// type of error.
//
//    Rev 1.119   16 Sep 1996 10:40:46   DCHI
// Changed sharing mode of VLScanFile's open from DENY_WRITE to DENY_NONE.
//
//    Rev 1.118   08 Sep 1996 19:02:56   DALLEE
// Replaced calls to WideCharToMultiByte() in VXD platform.
// This is part of the new CORE functionality. Calls should be put back
// when we're through kluging VXD to build on old QAKC files.
//
//    Rev 1.117   26 Aug 1996 14:10:06   DCHI
// For SYM_VXD and SYM_NTK, VLScanFile opens the file to scan in shared
// mode denying writes.
//
//    Rev 1.116   12 Aug 1996 17:03:18   RPULINT
// Added macro,windows & agent virus types
//
//    Rev 1.115   25 Jul 1996 18:57:02   DALLEE
// For VXD, WIN16 and WIN32, guarantee prExportTable = NULL on
// VLScanInit().
// Added calls to EXTInitWin() and EXTCloseWin() for SYM_WIN to dynamically
// load NAVEX DLL and fill out export table in hVContext.
//
//    Rev 1.114   03 Jul 1996 16:18:12   CNACHEN
// Fixed yet another problem in pamrepair...  uResult was not being set to
// TRUE if the PAM repair was successful.
//
//    Rev 1.113   01 Jul 1996 11:20:48   jmillar
// change szBackup buffer to TCHAR - the type is incorrect if actually building
// for UNICODE
//
//    Rev 1.112   28 Jun 1996 15:34:44   RAY
// More AVAPI_MT work.
//
//    Rev 1.111   27 Jun 1996 20:53:48   DALLEE
// Changed bogus SYM_W32 #defines to standard SYM_WIN32.
//
//    Rev 1.110   26 Jun 1996 16:04:54   RAY
// Added AVAPI_MT macro for thread-safe code previously used by VXD and NTK only.
//
//    Rev 1.109   26 Jun 1996 14:14:50   CNACHEN
// Updated VLRepairFile after code review with Ray.
//
//    Rev 1.108   26 Jun 1996 13:57:04   CNACHEN
// When deleting ERS read-only swap file, FIL4_KILL is used now instead of
// the FileDelete callback.  This overwrites the file with 0's before deleting
// it.
//
//    Rev 1.107   04 Jun 1996 13:49:12   CNACHEN
// Internal MTE/V2P6Z/Maltese Amoeba scanners removed for non-NAVOEM.
// NAVOEM does not (currently) use PAM, so we must still use these internal-
// to-the-engine scanners under NAVOEM.
//
//    Rev 1.106   30 Apr 1996 17:04:22   CNACHEN
// Added ALG scanner checking before PAM is applied.
//
//    Rev 1.105   25 Apr 1996 14:24:26   CNACHEN
// Updated BackupInfectedFile to convert from OEM to the current string-type
// before calling FileExists or FileCopy.
//
//    Rev 1.104   16 Apr 1996 09:50:22   RSTANEV
// Added NAVEX support for SYM_NTK.
//
//    Rev 1.103   15 Apr 1996 11:32:52   BRUCE
// Fixed handling of _T() macro so that 16 bit compiles work w/c800
//
//    Rev 1.102   11 Apr 1996 13:30:46   BRUCE
// Added conditional include of tchar.h, which is needed when is compiled from
// the context of NAVHPC in env of VIRLIB.
//
//    Rev 1.101   28 Mar 1996 18:01:10   TCASHIN
// Made VLValidContext an EXPORT.
//
//    Rev 1.100   14 Mar 1996 18:24:22   CNACHEN
// moved #define USE_PAM to avapi.h so that all CPP files that include AVAPI.H
// will properly define USE_PAM and the hContext will be of the proper size.
// It's defined for NLM, W32 and WIN16...
//
//    Rev 1.99   12 Mar 1996 10:31:56   CNACHEN
// Make sure we do a local close for both VXD and NTK.
//
//    Rev 1.98   26 Feb 1996 15:33:04   RSTANEV
// SYM_NTK calls PAMLocalInit() from VLScanFile().
//
//    Rev 1.97   26 Feb 1996 12:10:54   JWORDEN
// More DBCS stuff
//
//    Rev 1.95   14 Feb 1996 17:54:34   CNACHEN
// free global afte local now for NLM.
//
//    Rev 1.94   09 Feb 1996 19:04:58   RSTANEV
// SYM_NTK platform should not use the global work buffer in each HCONTEXT,
// because it does not exist.
//
//    Rev 1.93   09 Feb 1996 11:20:32   CNACHEN
// Added PAM support.
//
//    Rev 1.92   08 Feb 1996 16:25:28   DCHI
// Uncommented USE_PAM for VXD.  Added (PWSTR) cast.
//
//    Rev 1.91   08 Feb 1996 10:57:40   CNACHEN
// Use VIRSCAN2.DAT for PAM stuff...
//
//    Rev 1.90   07 Feb 1996 15:33:50   CNACHEN
// Added PAM support for NLM platform.
//
//    Rev 1.89   02 Feb 1996 20:12:38   RSTANEV
// Added Unicode support & cleaned up SYM_VXD platform.
//
//    Rev 1.88   25 Jan 1996 18:04:20   GDZIECI
// Fixed for NLM.
//
//    Rev 1.87   24 Jan 1996 16:23:04   GDZIECI
// Fixed 2 compiler warnings.
//
//    Rev 1.86   24 Jan 1996 14:41:16   JWORDEN
// Rename FreeScanFileData() to FreeAlgSigMemory() to avoid conflict
// with a similar function in virscan.h
//
//    Rev 1.85   11 Jan 1996 07:50:24   JWORDEN
// Merge changes for Quake 8 and Quake 6 back to the trunk
//
//    Rev 1.81.1.16   02 Jan 1996 10:32:30   DCHI
// Added PAM repair.
//
//    Rev 1.81.1.15   04 Dec 1995 20:59:54   DCHI
// Changed buffer size allocation in VLScanBoot and VLScanPart
// from SECTOR_SIZE to 8 * SECTOR_SIZE.
//
//    Rev 1.81.1.14   30 Oct 1995 13:04:54   CNACHEN
// Added ifdef'd out PAM support for VXD platform...
//
//    Rev 1.81.1.13   25 Oct 1995 17:19:10   CNACHEN
// Updated VLRepairFile to change attributes of the infected file before and
// after REP4_ME call.
//
//    Rev 1.81.1.12   17 Oct 1995 18:06:30   DCHI
// Changed avex.h to navex.h.
//
//    Rev 1.81.1.11   12 Oct 1995 14:08:54   DCHI
// EXTScanMemory takes bScanHighMemory parameter.
//
//    Rev 1.81.1.10   12 Oct 1995 12:10:08   CNACHEN
// Fixed filename being passed into EXTScanFile...
//
//    Rev 1.81.1.9   11 Oct 1995 14:35:08   CNACHEN
// Fixed bug in file/boot/part repair where NO_ACCESS return from VLScan*
// caused a potential null pointer reference...
//
//    Rev 1.81.1.8   09 Oct 1995 10:54:40   DCHI
// Fixed some AVEX stuff after the code review found some problems:
// 1. Boot repair now passes in cyl,sec,head to EXTRepairBoot.
// 2. Boot scanning now initializes cyl,sec,head to defaults for floppy boot
//    sector scan.  this was not being done before...
//
//    Rev 1.81.1.7   20 Sep 1995 17:24:48   DCHI
// Added support for INSCANF.DAT being greater than 64K.
//
//    Rev 1.81.1.6   19 Sep 1995 17:46:28   CNACHEN
// fixed some NO_ACCESS_NO_HANDLE bugs in new external code and buffer overlap
// problem
//
//    Rev 1.81.1.5   07 Sep 1995 13:50:28   CNACHEN
// added AVEX code...
//
//    Rev 1.81.1.4   29 Aug 1995 16:35:34   CNACHEN
// Added new argument to EXTScanBoot
//
//    Rev 1.81.1.3   29 Aug 1995 12:23:04   CNACHEN
// Added additional parameter to EXTRepairBoot
//
//    Rev 1.81.1.2   25 Aug 1995 16:30:14   CNACHEN
// fixed some if statements. :)
//
//    Rev 1.81.1.1   17 Aug 1995 13:03:32   CNACHEN
// Added W32 support and fixed bugs in VLScanFile, VLScanBoot, and VLScanPart.
// If the EXT* functions returned error result codes, the old version would
// improperly deal with this.  The new version should handle it correctly.
//
//    Rev 1.81.1.0   12 Jul 1995 19:03:12   RJACKSO
// Branch base for version QAK8
//
//    Rev 1.81   10 Jul 1995 12:34:58   CNACHEN
// Removed VLFileOpen calls which caused breakage because hContext's weren't
// being passed in.  This needs to be fixed after ship.
//
//    Rev 1.79   30 Jun 1995 17:54:20   SZIADEH
// Make sure hContext is initialized before usage in functions that
// use lpLocalWorkBuffer.
//
//    Rev 1.78   26 Jun 1995 20:17:02   SZIADEH
// Support reentrancy in VXD platform by allocating the work buffer
// on the stack where a work buffer is used.
//
//    Rev 1.77   25 May 1995 17:22:30   RSTANEV
// Now HVCONTEXT does not keep the structure with the callbacks, but it
// keeps only a pointer to it.
//
//    Rev 1.76   25 May 1995 08:38:42   RSTANEV
// Fixed an #endif.
//
//    Rev 1.75   25 May 1995 03:43:00   SZIADEH
// error checking in VLVirusInfo() for virus name.
//
//    Rev 1.74   24 May 1995 20:54:40   RSTANEV
// Added support for multiple character set environments (SYM_VXD).
//
//    Rev 1.73   11 May 1995 18:46:52   SZIADEH
// added VLSaveMemoryInfectionData()
//
//    Rev 1.72   05 May 1995 16:58:58   KEITH
// Fix STS# 34581 - record 0 is a valid comment for "No information".
// The test that returned an empty string for this case was removed.
//
//    Rev 1.71   17 Apr 1995 09:10:38   RSTANEV
// Added functions for overriding the protection for DIR2 repair and deletion.
//
//    Rev 1.70   30 Mar 1995 14:11:36   AMATIC
// Changed dwOffset into LONG and did cast *lpMem to WORD in calucalation of
// dwOffset for i_size != 4.
// Added check if ( dwOffset  ) than Do reading else ... return!
// avapi.cpp -> S:\navfamly\vcs\avapi.cpv
// Enter description of changes made.
//
//    Rev 1.69   08 Mar 1995 19:41:20   MFALLEN
// VLFillListTable() should use TempMemoryAlloc instead of PermMemoryAlloc to
// allocate bazilion lil'chunks of memory for HVIRUSENTRY's.
//
//    Rev 1.68   07 Mar 1995 19:29:36   SZIADEH
// fixed bugs (sts): 31270, 28904, 28901, 28897.
//
//    Rev 1.67   07 Mar 1995 11:31:58   DALLEE
// 1. #ifdef'd REP4_BOOT_LoadN30 callback stuff for W32, VXD, NLM
//    (shouldn't really be for NLM, but that's how it was in REP4_BOO...)
// 2. VLLoadN30 prototype changed from LOCAL to STATIC since it's used
//    for the callback.
// 3. Removed "C++" code since NLM platform is really compiling this as "C".
//
//    Rev 1.66   03 Mar 1995 13:37:02   RSTANEV
// Now REP4_BOOT will use callback to load the N30 record of the virus.
//
//    Rev 1.65   02 Mar 1995 07:56:42   RSTANEV
// VLCacheVirusData() works only for 32-bit platforms.
//
//    Rev 1.64   02 Mar 1995 07:30:10   RSTANEV
// Enabled caching of N30 records for all platforms.
//
//    Rev 1.63   28 Feb 1995 20:06:56   BARRY
// ifdef definition containing LPN30E structure so it's only active to VxD
//
//    Rev 1.62   27 Feb 1995 09:50:44   RSTANEV
// Oops.  Cut and paste doesn't work for me today...
//
//    Rev 1.61   27 Feb 1995 09:41:04   RSTANEV
// Fixed a pointer problem.
//
//    Rev 1.60   27 Feb 1995 09:12:14   RSTANEV
// VLLoadCachedN30() should be accessible from VxD platform only.
//
//    Rev 1.59   27 Feb 1995 09:10:06   RSTANEV
// Added VLLoadCachedN30().  Now VLLoadN30() will call this function if
// there is cached data.
//
//    Rev 1.58   24 Feb 1995 22:53:22   RSTANEV
// Added VLCacheVirusData() and all the stuff to support it.  VLLoadN30() is
// not yet using the cached information.
//
//    Rev 1.57   22 Feb 1995 15:46:58   JMILLARD
// fix problem with virus alias
// fix problems recovering from failed memory allocations
//
//    Rev 1.56   17 Feb 1995 11:56:00   DALLEE
// Replaced bogus STRCAT()'s with NameAppendFile().
// INSCAN detection/creation was broken for root directory.
//
//    Rev 1.55   16 Feb 1995 15:16:44   MFALLEN
// Month field for virus defs is not zero based.
//
//    Rev 1.54   08 Feb 1995 18:05:56   SZIADEH
// check if VLoadN30 had failed in getting virus info, and
// if so, return FALSE when asked for virus name.
//
//    Rev 1.53   08 Feb 1995 17:02:32   SZIADEH
// inscanX.dat creation support.
//
//    Rev 1.52   30 Jan 1995 18:56:08   JMILLARD
// add VLTrueNumVirusEntries
// change to use internal routine that calculates actual virus entries
//
//    Rev 1.51   27 Jan 1995 17:15:04   JMILLARD
// fix problem with out of synch virus list - it was using the wrong index
// Thanks to Carey
//
//    Rev 1.50   26 Jan 1995 15:44:02   MFALLEN
// Pointer was modified and then freed.
//
//    Rev 1.49   19 Jan 1995 19:31:44   MARKL
// Fixed function header comments for VLScanMemory().
//
//    Rev 1.48   19 Jan 1995 17:45:40   MARKL
// VLScanMemory now builds for VxD platform.
//
//    Rev 1.47   13 Jan 1995 11:17:08   SZIADEH
// removed sorting routine and deleted all refereces to it.
//
//    Rev 1.46   29 Dec 1994 11:10:56   SZIADEH
// fixed sorting routine.
//
//    Rev 1.45   28 Dec 1994 17:27:38   SZIADEH
// fixed memory detection.
//
//    Rev 1.44   20 Dec 1994 17:09:38   DALLEE
// All for SortMemorySigs():
// 1.  Size of a sig in mem image of INSCANM is SIZE_SIZE(17) bytes, not 19.
// 2.  lpMemSig incremented in loop to get to next sig.
// 3.  Replaces SYM_ASSERT's on MemAlloc's with error handling code -- this
//     is a valid run-time error.
// 4.  Removed hard tabs which seemed set for 4 spaces.
// 5.* Still needs type of lpMemSig changed - LPRECORD_MEMORY_SIGNATURE is wrong.
//
//    Rev 1.43   20 Dec 1994 14:23:46   JMILLARD
// add fast virus check code for NLM
//
//    Rev 1.42   19 Dec 1994 15:19:36   DALLEE
// Convert LPBYTE to LPWORD, not WORD *.
//
//    Rev 1.41   14 Dec 1994 14:05:32   BRAD
// FIL4_KILL is alive and well.  Converted to 'C'.
//
//    Rev 1.40   13 Dec 1994 20:41:10   JMILLARD
// file.h must precede ctsn.h for NLM
//
//    Rev 1.39   09 Dec 1994 18:18:56   RSTANEV
// Oops, restored VLScanMemory(), but nuked everything in it...
//
//    Rev 1.38   09 Dec 1994 18:16:26   RSTANEV
// Got rid of VLScanMemory... (just kidding... only for VxD platform)
//
//    Rev 1.37   06 Dec 1994 16:27:04   JMILLARD
// check in Sami's interim changes
//
//    Rev 1.36   01 Nov 1994 13:55:26   JMILLARD
// ifdef out some stuff for SYM_NLM - add NLM versions of others
//
//    Rev 1.35   26 Oct 1994 19:33:36   RSTANEV
// FIL4_DELETE's return value was not being processed correctly.
//
//    Rev 1.34   26 Oct 1994 16:26:26   KEITH
// Set VLGetScanData to SYM_EXPORT, remove some troublesome casting.
//
//    Rev 1.33   20 Oct 1994 09:13:34   KEITH
// For 16 bit, fixed an occurance of MEMSET that would set 0 bytes.
//
//    Rev 1.32   28 Sep 1994 16:34:12   MFALLEN
// Sami added some temporary cluge so we can identify infected boot records.
//
//    Rev 1.31   16 Sep 1994 07:53:36   MFALLEN
// Somebody busted VLDeleteFile() when FIL4_KILL was replaced with FIL4_DELETE.
// This has now been fixed.
//
//    Rev 1.30   05 Sep 1994 15:45:26   SZIADEH
// Memory scanning:
// - reduced amount of memory allocated to 0x10000 instead of 0xa0000
// - added lpMemSigsTemp to act as a place holder in the for loops.
//
//    Rev 1.29   30 Aug 1994 17:23:06   SZIADEH
// deleted unused include files.
//
//    Rev 1.28   30 Aug 1994 16:50:08   KEITH
// Commented out the debug code.
//
//    Rev 1.27   30 Aug 1994 12:35:10   SZIADEH
//
//    Rev 1.26   28 Aug 1994 20:06:12   SZIADEH
// memory detection.
// all apis now return VSTATUS.
//
//    Rev 1.25   25 Aug 1994 15:46:22   RSTANEV
// Added a fake MemCopyPhysical() so that VxD platform doesn't yell at me...
//
//    Rev 1.24   25 Aug 1994 14:57:04   MFALLEN
// Made sure local functions are prototyped correctly.
//
//    Rev 1.23   25 Aug 1994 14:50:12   SZIADEH
// fixed the casting problem in vlscan413.
//
//    Rev 1.22   25 Aug 1994 14:15:14   SZIADEH
// temporarely commented out VLScanMemoryBuffer to make martinf happy.
//
//    Rev 1.21   24 Aug 1994 16:42:14   CAREY
//
//    Rev 1.20   24 Aug 1994 14:17:18   CAREY
// changed SECTOR_SIZE to wDepth in VLVirusInfo.
//
//    Rev 1.19   24 Aug 1994 13:26:56   SZIADEH
// code for memory detection.
//
//    Rev 1.18   23 Aug 1994 18:06:16   SZIADEH
// corrected return types for vl functions.
//
//    Rev 1.17   23 Aug 1994 17:50:16   SZIADEH
// modified VL functions to return VSTATUS
// handle multiple infections
//
//    Rev 1.16   19 Aug 1994 15:44:28   MFALLEN
// Now builds correctly for 16-bit platform.
//
//    Rev 1.15   17 Aug 1994 15:29:04   CAREY
// Fixed bugs with VLGetDBInfo
//
//    Rev 1.14   11 Aug 1994 13:57:56   CAREY
//
//    Rev 1.13   10 Aug 1994 14:48:18   CAREY
// new version to fix pointer problems
//
//    Rev 1.12   10 Aug 1994 11:13:22   SZIADEH
//
//    Rev 1.11   10 Aug 1994 10:10:20   MFALLEN
//
//    Rev 1.10   09 Aug 1994 18:27:14   CAREY
// added some SYM_ASSERT's to check for invalid callback ptrs
//
//    Rev 1.9   09 Aug 1994 16:59:44   CAREY
// removed drive type stuff, includes callback.h
//
//    Rev 1.8   08 Aug 1994 18:43:08   CAREY
// fixed memory not free'd bugs
//
//    Rev 1.7   05 Aug 1994 18:25:00   CAREY
// added +1 to SYM_MAX_PATH for strings
//
//    Rev 1.6   01 Aug 1994 16:11:54   DALLEE
// Removed VIRUSDBF.H - This is only Windows virus list information.
//
//    Rev 1.5   01 Aug 1994 16:03:36   DALLEE
// Include virusinf.h now and fixed a superflous if comparison.
//************************************************************************

#include "platform.h"
#if !defined (UNICODE)
#include "dbcs.h"
#endif
#include "file.h"
#include "xapi.h"
#include "disk.h"
#include "repair.h"
#include "avapi.h"
#include "avenge.h"
#include "navcb.h"
#include "avenge.h"

#ifndef _T                              // BEM - This is needed when this is compiled from
                                        // BEM - the context of NAVHPC in env of VIRLIB
    #ifdef SYM_WIN16
        #define __T(x) x
        #define _T(x) __T(x)
    #else
        #include <tchar.h>
    #endif
#endif

#ifdef SYM_WIN
#define QUAKE6                      // make sure we use stCallBack instead of
#endif

#include "navex.h"                  // lpstCallBack...

//////////////////////////////////////////////////////////////////////////////
// PAM
//////////////////////////////////////////////////////////////////////////////
#ifdef USE_PAM
#include "pamapi.h"
#endif
//////////////////////////////////////////////////////////////////////////////

// --------------------------------------------------------------------------
//                      LOCAL FUNCTION PROTOTYPES
// --------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
// API support routines (not accessable to user)
///////////////////////////////////////////////////////////////////////////////

BOOL STATIC VLLoadN30 (
    HVCONTEXT   hContext,
    UINT        uIndex,
    LPN30       lpN30 );

BOOL LOCAL VLLoadCachedN30 (
    HVCONTEXT hContext,
    UINT      uIndex,
    UINT      uN30ListEntries,
    LPN30E    lpN30E,
    LPN30     lpN30 );

VOID LOCAL VLSetHVLVIRUSBoot (
    HVLVIRUS      hVirus,
    HVCONTEXT   lpVContext,
    UINT        uDBEntryNum,
    DWORD       dwFlags,
    BYTE        byDriveNum,
    BYTE        byBootSecNum,
    BYTE        byBootFlags,
    BYTE        bySystemType );

VOID LOCAL VLSetHVLVIRUSFile (
    HVLVIRUS      hVirus,
    HVCONTEXT   lpVContext,
    UINT        uDBEntryNum,
    DWORD       dwFlags,
    LPTSTR      lpszFileName );

#if 0
BOOL LOCAL VLGetVirusInfo (
    HVCONTEXT   hContext,
    WORD        wVirusID,
    LPTSTR      lpAlias,
    LPTSTR      lpComment );

BOOL LOCAL VLGetVirusText (
    HVCONTEXT   hContext,
    UINT        uDBEntryNum,
    LPTSTR      lpszVirusText );

BOOL LOCAL VLGetVirusAlias (
    HVCONTEXT   hContext,
    UINT        uDBEntryNum,
    LPTSTR      lpszVirusAlias );
#endif

BOOL LOCAL VLEngLocalInit (
    HVCONTEXT hContext, 
    LPHLENG   lphLEng );

VOID LOCAL VLEngLocalClose (
    HLENG     hLEng );

WORD LOCAL VLGetInfectionDepth (
   HVLVIRUS hVirus);

BOOL WINAPI VLIsFileUpToDate(
    LPTSTR Path,
    LPTSTR FileName,
    FILETIME FileTime);

BOOL WINAPI VLVerifyDataFile(
    LPTSTR Path,
    LPTSTR FileName );


////////////////////////////////////////////////////////////////////////////////////
// One global variable to hold the call back info for access by low level functions
////////////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
extern "C" {
#endif

// this overrides the forced convention there are no _ in front of names
// this is a Novell convention

#ifdef SYM_NLM
#define lpgCallBack _lpgCallBack
#endif

extern LPCALLBACKREV1 lpgCallBack;

#ifdef __cplusplus
}
#endif


/*@API:*************************************************************************
@Declaration:  HVCONTEXT  WINAPI VLScanInit(UINT uRevision,
LPCALLBACKREV1 lpCallBack, WORD wFlags)

@Description:
This function can be used to initialize the Virus Library API.  The user of
the API must fill in a structure of type CALLBACKREV1 and pass in a 1 as the
revision number.  The callback functions should take the same arguments and
return values as the QUAKE functions of the same name...

@Parameters:
$uRevision$ Should always be 1; it identifies which structure version we are
registering.

$lpCallBack$ is a far pointer to a structure which has been initialized.
The CALLBACKREV1 data type can be found in the header file AVAPI.H. The
structure pointed by lpCallBack should be valid as long as the HVCONTEXT
handle returned by VLScanInit() is used.

$wFlags$ is a word which contains configuration flags:

    VLINIT_HANDLE_NO_ACCESS:
       return a handle if an item is inaccessable.

@Returns:
This function returns a handle to a virus context (HVCONTEXT) structure on
success or NULL on failure.  The only reason it would return NULL is if
the registered memory allocation function was unable to allocate memory for
the context structure.

@Include: avapi.h
*******************************************************************************/

HVCONTEXT SYM_EXPORT WINAPI VLScanInit
(
    UINT            uRevision,          // [in] callback revision number
    LPCALLBACKREV1  lpCallBack,         // [in] filled in callback structure
    WORD            wFlags              // [in] flags to specify options
)
{
    auto    HVCONTEXT    hContext;

    if (uRevision != VL_REVISION)
        return(NULL);

    SYM_ASSERT(lpCallBack->PermMemoryAlloc);
    SYM_ASSERT(lpCallBack->PermMemoryFree);
    SYM_ASSERT(lpCallBack->TempMemoryAlloc);
    SYM_ASSERT(lpCallBack->TempMemoryFree);

    hContext = (HVCONTEXT)lpCallBack->PermMemoryAlloc(sVCONTEXTTYPE);

    if (hContext == NULL)               // Error allocating context structure
        return(NULL);

                                        // Save callback structure

    hContext->lpstCallBack = lpCallBack;

    SYM_ASSERT(VLFileOpen);
    SYM_ASSERT(VLFileClose);
    SYM_ASSERT(VLFileRead);
    SYM_ASSERT(VLFileSeek);
    SYM_ASSERT(VLFileSize);

    ////////////////////////////////////////////////////////////////////////
    // Now initialize the szVirScanDataFile/szVirScanInfoFile filenames
    ////////////////////////////////////////////////////////////////////////

    hContext->szVirScanDataFile[0] = 0;
    hContext->szVirScanInfoFile[0] = 0;

#if defined(SYM_VXD)
    hContext->szAnsiVirScanDataFile[0] = 0;
    hContext->szAnsiVirScanInfoFile[0] = 0;
    *(PWCHAR)hContext->szUniVirScanDataFile = 0;
    *(PWCHAR)hContext->szUniVirScanInfoFile = 0;
#endif

    ////////////////////////////////////////////////////////////////////////
    // Now initialize the buffer pointers
    ////////////////////////////////////////////////////////////////////////

    hContext->hGEng                     = NULL;
    hContext->hGPAM                     = NULL;

#if !defined(AVAPI_MT)
    hContext->hLEng                     = NULL;
    hContext->hLPAM                     = NULL;
#endif

    hContext->prExportTable             = NULL;

#if defined(SYM_WIN)
    hContext->hExportLib                = NULL;
#endif
#if defined(SYM_NLM)
    hContext->lpFastCheckDB             = NULL;
#endif

    hContext->uN30EFCount               = 0;
    hContext->uN30EBCount               = 0;

    hContext->lprN30EFBuffer            = NULL;
    hContext->lprN30EBBuffer            = NULL;

    //////////////////////////////////////////////////////////////////////////
    // Set up a signature for verification
    //////////////////////////////////////////////////////////////////////////

    hContext->dwSignature    = 0xDEADFECEL;

    hContext->wFlags         = wFlags;

    return(hContext);
}


/*@API:*************************************************************************
@Declaration:  VSTATUS WINAPI VLScanClose(HVCONTEXT hContext)

@Description:
This closes up a virus scanning session.  All HVLVIRUS and HVLVIRUSENTRY structures
must be released before closing the Anti Virus API.

@Parameters:
$hContext$ This is a valid handle to a context provided by VLScanInit.

@Returns:
This function returns a result of type VSTATUS (type definition found in
AVAPI.H).

    VS_OK           will be returned on success
    VS_INVALID_ARG  will be returned if the context is invalid

@Include: avapi.h
*******************************************************************************/

VSTATUS SYM_EXPORT WINAPI VLScanClose
(
    HVCONTEXT hContext                  // [in] a valid HVCONTEXT handle
)
{
    //////////////////////////////////////////////////////////////////////////
    // Verify the structure pointer and its signature
    //////////////////////////////////////////////////////////////////////////

    if (!VLValidContext(hContext))
        return(VS_INVALID_ARG);

    //////////////////////////////////////////////////////////////////////////
    // Close external NLM APIs.
    //////////////////////////////////////////////////////////////////////////

#if defined(SYM_NLM)
    EXTCloseNLM ( hContext );
#endif

    //////////////////////////////////////////////////////////////////////////
    // Close external VxD / NTK APIs.
    //////////////////////////////////////////////////////////////////////////

#if defined(SYM_VXD)
    if ( EXTCloseVxD ( hContext ) != EXTSTATUS_OK )
        {
        SYM_ASSERT ( FALSE );
        }
#endif

#if defined(SYM_NTK)
    if ( EXTCloseNTK ( hContext ) != EXTSTATUS_OK )
        {
        SYM_ASSERT ( FALSE );
        }
#endif

    //////////////////////////////////////////////////////////////////////////
    // Close external VxD / NTK APIs.
    //////////////////////////////////////////////////////////////////////////
#if defined(SYM_WIN)
    if ( EXTCloseWin ( hContext ) != EXTSTATUS_OK )
        {
        SYM_ASSERT ( FALSE );
        }
#endif


    //////////////////////////////////////////////////////////////////////////
    // Free temporary data structures
    //////////////////////////////////////////////////////////////////////////

#if defined(SYM_NLM)
    if (hContext->lpFastCheckDB != NULL)
        {
        VLPermMemoryFree( hContext->lpFastCheckDB );
        }
#endif
#if !defined(AVAPI_MT)                  // Only single threaded apps leave
    if (hContext->hLPAM != NULL)        // Local Handles open between scans, 
        PAMLocalClose(hContext->hLPAM); // so only these apps need to close 
                                        // them here.
    if (hContext->hLEng != NULL)       
        EngLocalClose(hContext->hLEng);
#endif 

    if (hContext->hGPAM != NULL)
        PAMGlobalClose(hContext->hGPAM);

    if (hContext->hGEng != NULL)
        EngGlobalFileClose(hContext->hGEng);

    if (hContext->hGEng != NULL)
        EngGlobalClose(hContext->hGEng);

#if defined(SYM_WIN)
    if (hContext->hExportLib != NULL)
        FreeLibrary(hContext->hExportLib);
#endif

#if !defined(AVAPI_MT)
    hContext->hLPAM = NULL;
    hContext->hLEng = NULL;
#endif
    hContext->hGPAM = NULL;
    hContext->hGEng = NULL;

    hContext->dwSignature = 0;

    VLPermMemoryFree(hContext);

    return(VS_OK);
}


#ifndef SYM_NLM
/*@API:*************************************************************************
@Declaration:  VSTATUS WINAPI VLReleaseVirusHandle(HVLVIRUS hInfo)

@Description:
This routine is used to release a handle to a virus which was obtained during
scanning.

@Parameters:
$hInfo$ This is a virus handle.

@Returns:
This function returns a result of type VSTATUS (type definition found in
AVAPI.H).

    VS_OK           will be returned on success
    VS_INVALID_ARG  will be returned if the context is invalid

@Include: avapi.h
*******************************************************************************/

VSTATUS SYM_EXPORT WINAPI VLReleaseVirusHandle
(
    HVLVIRUS hInfo                        // [in] a valid HVLVIRUS handle
)
{
    VOID (WINAPI *PermMemoryFree)(LPVOID lpBuffer);

    if (hInfo != NULL)
    {
       VLReleaseVirusHandle(hInfo->hNext);
       PermMemoryFree = hInfo->lpVContext->lpstCallBack->PermMemoryFree;
       hInfo->lpVContext = NULL;
       PermMemoryFree(hInfo);
    }

    return(VS_OK);
}



/*@API:*************************************************************************
@Declaration:  VSTATUS WINAPI VLReleaseEntry(HVLVIRUSENTRY hInfo)

@Description:
This routine is used to release a handle to a virus information entry used in
virus information-list displays.

@Parameters:
$hInfo$ This is a virus information entry handle.

@Returns:
This function returns a result of type VSTATUS (type definition found in
AVAPI.H).

    VS_OK           will be returned on success
    VS_INVALID_ARG  will be returned if the context is invalid

@Include: avapi.h
*******************************************************************************/

VSTATUS SYM_EXPORT WINAPI VLReleaseEntry
(
    HVLVIRUSENTRY hInfo                   // [in] a valid HVLVIRUSENTRY handle
)
{
    VOID (WINAPI *TempMemoryFree)(LPVOID lpBuffer);

    if (hInfo == NULL)
        return(VS_INVALID_ARG);

    ///////////////////////////////////////////////////////////////////
    // Obtain a function pointer to the permanent memory free callback
    ///////////////////////////////////////////////////////////////////

    TempMemoryFree = hInfo->lpVContext->lpstCallBack->TempMemoryFree;

    ///////////////////////////////////////////////////////////////////
    // Invalidate our HVLVIRUS structure by NULLing the context pointer
    ///////////////////////////////////////////////////////////////////

    hInfo->lpVContext = NULL;

    TempMemoryFree(hInfo);

    return(VS_OK);
}
#endif



/*@API:*************************************************************************
@Declaration:  VSTATUS WINAPI VLRegisterVirusDB(HVCONTEXT hContext,
LPTSTR lpszDBFile, LPTSTR lpszInfoFile)

@Description:
This routine is used to "register" a virus database (VIRSCAN.DAT currently),
and its accompanying virus information database (VIRSCAN.INF currently).  This
must be done after VLScanInit and before any scanning functions are called.

@Parameters:
$hContext$ This is a valid handle to a context provided by VLScanInit.
$lpszDBFile$ This is the filename of the virus DB file (OEM for VxD!)
$lpszInfoFile$ This is the filename of the virus INF file (OEM for VxD!)

@Returns:
This function returns a result of type VSTATUS (type definition found in
AVAPI.H).

    VS_OK           will be returned on success
    VS_INVALID_ARG  will be returned if the context is invalid
    VS_FILE_ERROR   will be returned if a specified data file can't be accessed
    VS_MEMALLOC     will be returned if memory can't be allocated

@Include: avapi.h
*******************************************************************************/

VSTATUS SYM_EXPORT WINAPI VLRegisterVirusDB
(
    HVCONTEXT hContext,                 // [in] a valid HVCONTEXT handle
    LPTSTR    lpszDBFile,               // [in] path & filename of VIRSCAN.DAT
    LPTSTR    lpszInfoFile              // [in] path & filename of VIRSCAN.INF
)
{
    TCHAR       szPAMFile[SYM_MAX_PATH +1];
    extern char szNAVPath[];

    //////////////////////////////////////////////////////////////////////////
    // Verify the structure pointer and its signature
    //////////////////////////////////////////////////////////////////////////

    if (!VLValidContext(hContext))
        return(VS_INVALID_ARG);


    //////////////////////////////////////////////////////////////////////////
    // Copy our antivirus database name into our structure
    //////////////////////////////////////////////////////////////////////////

    STRCPY(hContext->szVirScanDataFile, lpszDBFile);
    STRCPY(hContext->szVirScanInfoFile, lpszInfoFile);
    STRCPY(szNAVPath, lpszInfoFile);
    NameStripFile(szNAVPath);

#if defined(SYM_VXD)
    OemToCharW ( lpszDBFile, (PWSTR)hContext->szUniVirScanDataFile );
  #if 0
                                        //&? Not using this function until
                                        // we're tested and working with
                                        // CORE VXD platform. 08/09/1996 DALLEE
    WideCharToMultiByte ( CP_ACP,
                          0,
                          (PWSTR)hContext->szUniVirScanDataFile,
                          -1,
                          hContext->szAnsiVirScanDataFile,
                          sizeof(hContext->szAnsiVirScanDataFile),
                          NULL,
                          NULL );
  #else
    OemToCharA ( lpszDBFile, hContext->szAnsiVirScanDataFile );
  #endif

    OemToCharW ( lpszInfoFile, (PWSTR)hContext->szUniVirScanInfoFile );
  #if 0
    WideCharToMultiByte ( CP_ACP,
                          0,
                          (PWSTR)hContext->szUniVirScanInfoFile,
                          -1,
                          hContext->szAnsiVirScanInfoFile,
                          sizeof(hContext->szAnsiVirScanInfoFile),
                          NULL,
                          NULL );
  #else
    OemToCharA ( lpszInfoFile, hContext->szAnsiVirScanInfoFile );
  #endif
#endif

      STRCPY (szPAMFile, hContext->szVirScanDataFile );
      NameStripFile(szPAMFile);  
      NameAppendFile (szPAMFile, _T("VIRSCAN2.DAT") );  
  
      STRCPY(hContext->szPAMScanDataFile, szPAMFile);
  
#ifdef SYM_VXD
      OemToCharW ( szPAMFile, (PWSTR)hContext->szUniPAMScanDataFile );  
  #if 0  
      WideCharToMultiByte ( CP_ACP,  
                            0,  
                            (PWSTR)hContext->szUniPAMScanDataFile,  
                            -1,  
                            hContext->szAnsiPAMScanDataFile,  
                            sizeof(hContext->szAnsiPAMScanDataFile),  
                            NULL,  
                            NULL );  
  #else  
      OemToCharA ( szPAMFile, hContext->szAnsiPAMScanDataFile );  
  #endif  
#endif // SYM_VXD  
  
      if (EngGlobalInit(&hContext->hGEng,
                        &gstNAVGeneralCallBacks,  
                        &gstNAVProgressCallBacks,  
                        &gstNAVDataFileCallBacks,  
                        &gstNAVIPCCallBacks,  
                        0xDEADBEEFUL) == ENGSTATUS_ERROR)  
      {  
          return(VS_FILE_ERROR);  
      }  
    
      if (EngGlobalMemoryInit(hContext->hGEng) == ENGSTATUS_ERROR)  
      {  
          return(VS_FILE_ERROR);  
      }  
    
      if (EngGlobalBootInit(hContext->hGEng) == ENGSTATUS_ERROR)  
      {  
          return(VS_FILE_ERROR);  
      }  
    
      if (EngGlobalFileInit(hContext->hGEng,  
                            GENG_INIT_FILE_FLAG_LOAD_ALL) == ENGSTATUS_ERROR)  
      {  
          return(VS_FILE_ERROR);  
      }  
    
      if (!(hContext->wFlags & VLINIT_NO_PAM) &&
          PAMGlobalInit(szPAMFile,  
                        &hContext->hGPAM) != PAMSTATUS_OK)  
      {  
          return(VS_FILE_ERROR);  
      }
  
#if !defined(AVAPI_MT)                // Single threaded apps only
      if (EngLocalInit(hContext->hGEng,  
                       &hContext->hLEng,  
                       0xDEADBEE0UL) == ENGSTATUS_ERROR)  
      {  
          return(VS_FILE_ERROR);  
      }  
  
      if (hContext->hGPAM != NULL &&
          PAMLocalInit(szPAMFile,
                       NULL,       
                       hContext->hGPAM,  
                       &hContext->hLPAM) != PAMSTATUS_OK)  
      {  
          return(VS_FILE_ERROR);  
      }  
#endif  
  
    //////////////////////////////////////////////////////////////////////////
    // Initialize NavEx
    //////////////////////////////////////////////////////////////////////////

#if defined(SYM_NLM)
    if (EXTInitNLM(hContext) != EXTSTATUS_OK)
        {
        return(VS_FILE_ERROR);
        }
#elif defined(SYM_VXD)
    if (EXTInitVxD(hContext) != EXTSTATUS_OK)
        {
        return(VS_FILE_ERROR);
        }
#elif defined(SYM_NTK)
    if (EXTInitNTK(hContext) != EXTSTATUS_OK)
        {
        return(VS_FILE_ERROR);
        }
#elif defined(SYM_WIN)
    if (EXTInitWin(hContext) != EXTSTATUS_OK)
        {
        return(VS_FILE_ERROR);
        }
#elif defined(SYM_DOSX)
    if (EXTInitDX (hContext) != EXTSTATUS_OK)
        {
        return(VS_FILE_ERROR);
        }
#endif

    return(VS_OK);
}

/*@API:*************************************************************************
@Declaration: VSTATUS WINAPI VLCacheVirusData (HVCONTEXT hContext,
UINT uFlags)


@Description:
This function caches the N30 entries for a particular type of viruses.
After the successful return from this function, any calls to VLLoadN30()
will look up the cache for virus information before a reference to the
DAT files is made.

@Parameters:
$hContext$ This is a valid handle to a context provided by VLScanInit.

$uFlags$ Should contain a combination of the following flags:
    fVL_CACHE_FILE
    fVL_CACHE_BOOT

@Returns:

    VS_OK if data cached, or VS_? if error
    VS_WRONG_VERSION if platform not supported

@Include: avapi.h

@Compatibility: VxD, W32, NLM, NTK
*******************************************************************************/

VSTATUS SYM_EXPORT WINAPI VLCacheVirusData (
    HVCONTEXT hContext,
    UINT      uFlags
    )
    {
        return (VS_OK);
    }


#if 0
VSTATUS SYM_EXPORT WINAPI VLCacheVirusData (
    HVCONTEXT hContext,
    UINT      uFlags
    )
    {
#if defined(SYM_VXD) || defined(SYM_WIN32) || defined(SYM_NLM) || defined(SYM_NTK)

    auto HFILE  hFile;
    auto UINT   uDefLen, uDefCount, uDefLenRead, uDefLeft, uCurrentDef;
    auto LPN30  lprN30, lprCurrentN30;
    auto UINT   uFEntryCount, uBEntryCount;
    auto LPN30E lprN30EFEntry, lprN30EBEntry;

                                        // Validate input parameters

    if ( !VLValidContext ( hContext ) ||
         ( uFlags & (~(fVL_CACHE_FILE | fVL_CACHE_BOOT)) )
       )
        {
        return ( VS_INVALID_ARG );
        }

                                        // Update the global context

    lpgCallBack = hContext->lpstCallBack;

                                        // Open the virus def file

    hFile = VLFileOpen ( HVC_szVirScanDataFile(hContext), READ_ONLY_FILE );

    if ( hFile == HFILE_ERROR )
        {
        return ( VS_FILE_ERROR );
        }

                                        // Determine the "real" file size and
                                        // the number of defs in it.

    uDefLen   = VLFileSize ( hFile ) - sizeof(VIRSCANDATHEADER);
    uDefCount = uDefLen / sizeof(N30);
    uDefLen   = uDefCount * sizeof(N30);

    if ( VLFileSize ( hFile ) <= sizeof(VIRSCANDATHEADER) ||
         uDefCount == 0 )
        {
        VLFileClose ( hFile );
        return ( VS_FILE_ERROR );
        }

                                        // Allocate memory for the defs

    lprN30 = (LPN30) VLTempMemoryAlloc ( uDefLen );

    if ( lprN30 == NULL )
        {
        VLFileClose ( hFile );
        return ( VS_MEMALLOC );
        }

                                        // Load defs; Read 32K at a time

    VLFileSeek ( hFile, sizeof(VIRSCANDATHEADER), SEEK_SET );

    lprCurrentN30 = lprN30;

    do
        {
        uDefLenRead = uDefLen <= 0x8000 ? uDefLen : 0x8000;

        if ( VLFileRead ( hFile, lprCurrentN30, uDefLenRead ) != uDefLenRead )
            {
            VLTempMemoryFree ( lprN30 );
            VLFileClose ( hFile );
            return ( VS_FILE_ERROR );
            }

        *(LPBYTE*)(&lprCurrentN30) += uDefLenRead;

        uDefLen -= uDefLenRead;

        } while ( uDefLen );

    VLFileClose ( hFile );

                                        // Count the N30 records of each
                                        // virus type that we need.

    uFEntryCount = \
    uBEntryCount = 0;

    lprCurrentN30 = lprN30;

    uDefLeft = uDefCount;

    while ( uDefLeft-- )
        {
        switch ( lprCurrentN30->wType )
            {
            case ALGtype:
            case CRCtype:
                uFEntryCount++;
                break;

            case BOOtype:
                uBEntryCount++;
                break;
            }

        lprCurrentN30++;
        }

                                        // Now that we have the counts,
                                        // allocate memory for each type

    if ( !(uFlags & fVL_CACHE_FILE) )
        {
        uFEntryCount = 0;
        }

    if ( !(uFlags & fVL_CACHE_BOOT) )
        {
        uBEntryCount = 0;
        }

    if ( uFEntryCount )
        {
        lprN30EFEntry = (LPN30E) VLPermMemoryAlloc ( uFEntryCount *
                                                     sizeof(N30E) );

        if ( lprN30EFEntry == NULL )
            {
            VLTempMemoryFree ( lprN30 );
            return ( VS_MEMALLOC );
            }
        }
    else
        {
        lprN30EFEntry = NULL;
        }

    if ( uBEntryCount != 0 )
        {
        lprN30EBEntry = (LPN30E) VLPermMemoryAlloc ( uBEntryCount *
                                                     sizeof(N30E) );

        if ( lprN30EBEntry == NULL )
            {
            if ( lprN30EFEntry )
                {
                VLPermMemoryFree ( lprN30EFEntry );
                }
            VLTempMemoryFree ( lprN30 );
            return ( VS_MEMALLOC );
            }
        }
    else
        {
        lprN30EBEntry = NULL;
        }

    hContext->uN30EFCount = uFEntryCount;
    hContext->uN30EBCount = uBEntryCount;

    hContext->lprN30EFBuffer = lprN30EFEntry;
    hContext->lprN30EBBuffer = lprN30EBEntry;

                                        // Copy defs to appropriate buffers

    lprCurrentN30 = lprN30;

    uCurrentDef = 1;

    while ( uCurrentDef <= uDefCount )
        {
        switch ( lprCurrentN30->wType )
            {
            case ALGtype:
            case CRCtype:
                if ( uFEntryCount )
                    {
                    lprN30EFEntry->uVirusEnumerator = uCurrentDef;
                    lprN30EFEntry->rN30             = *lprCurrentN30;
                    lprN30EFEntry++;
                    }
                break;

            case BOOtype:
                if ( uBEntryCount )
                    {
                    lprN30EBEntry->uVirusEnumerator = uCurrentDef;
                    lprN30EBEntry->rN30             = *lprCurrentN30;
                    lprN30EBEntry++;
                    }
                break;
            }

        uCurrentDef++;
        lprCurrentN30++;
        }

                                        // Free up temporary memory

    VLTempMemoryFree ( lprN30 );

                                        // Return success

    return ( VS_OK );
#else
    return ( VS_WRONG_VERSION );
#endif
    }
#endif

#ifdef SYM_NLM

/*@API:*************************************************************************
@Declaration:  VSTATUS WINAPI VLRegisterFastVirusDB(HVCONTEXT hContext,
LPSTR lpszDBFile, LPSTR lpszInfoFile)

@Description:
This routine is used to "register" a virus database for fast access - it
preloads the virus names and verification information.
and its accompanying virus information database (VIRSCAN.INF currently).  This
must be done after VLScanInit and before any scanning functions are called.

@Parameters:
$hContext$ This is a valid handle to a context provided by VLScanInit.

@Returns:
This function returns a result of type VSTATUS (type definition found in
AVAPI.H).

    VS_OK           will be returned on success
    VS_INVALID_ARG  will be returned if the context is invalid
    VS_FILE_ERROR   will be returned if a specified data file can't be accessed
    VS_MEMALLOC     will be returned if memory can't be allocated

@Include: avapi.h
*******************************************************************************/

VSTATUS SYM_EXPORT WINAPI VLRegisterFastVirusDB
(
    HVCONTEXT hContext,                 // [in] a valid HVCONTEXT handle
    LPSTR     lpszDBFile,               // [in] path & filename of VIRSCAN.DAT
    LPSTR     lpszInfoFile              // [in] path & filename of VIRSCAN.INF
)
{
    UINT                uDefCount;
    LPFASTVIRUSCHECK    lpFastVirusCheck = NULL;


    (void) lpszDBFile;
    (void) lpszInfoFile;

    //////////////////////////////////////////////////////////////////////////
    // Verify the structure pointer and its signature
    //////////////////////////////////////////////////////////////////////////

    if (!VLValidContext(hContext))
        return(VS_INVALID_ARG);

    // find out how many defs there are

    uDefCount = VLTrueNumVirusEntries( hContext )+1;

    // allocate an array to hold all the definition check information

    hContext->lpFastCheckDB =
        (LPFASTVIRUSCHECK) VLPermMemoryAlloc(
                        uDefCount * sizeof(*lpFastVirusCheck));

    if ( hContext->lpFastCheckDB )
        {
        hContext->uTotalDefs =
            VLFillFastCheckDB( hContext, uDefCount, hContext->lpFastCheckDB );
        }
    else
        {
        return(VS_MEMALLOC);
        }

    return(VS_OK);
}

#endif



// DetermineVirusIndex arguments:
//
// lpszDataBase         : path and filename of the VIRSCAN.DAT database
//                        (in the current character set)
// wVirusID             : virus ID of the offending virus
// lpwVirusIndex        : changed by the function to provide the index
//                        into VIRSCAN.DAT where the record is.
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK              if no virus found
//                        EXTSTATUS_FILE_ERROR      if file error occured
//
// If EXTSTATUS_OK is returned, and *lpwVirusIndex != 0, then we have the
// proper record.  If == 0, then a record containing the specified virus ID 
// was not found.


EXTSTATUS WINAPI DetermineVirusIndex(HVCONTEXT hContext,
                                     LPTSTR lpszDataBase,
                                     WORD   wVirusID,
                                     LPWORD lpwVirusIndex)
{                      
    BOOL bFound;

    if (EngVIDToHVIRUS(hContext->hGEng,
                       wVirusID,
                       lpwVirusIndex,
                       &bFound) == ENGSTATUS_ERROR)
    {
        return(EXTSTATUS_FILE_ERROR);
    }

    return(EXTSTATUS_OK);
}


#ifndef SYM_NLM

//---------------------------------------------------------------------------
//
// BOOL WINAPI BuildUniqueFileName (
//      LPCTSTR lpszBaseFile,
//      LPCTSTR lpszExtension,
//      LPTSTR  lpszUniqueFile
//      );
//
// This function builds a new filename based on the filename specified by
// lpszBaseFile.  First, this function will replace the extension of the
// base filename with lpszExtension.  If the produced file exists, then
// this function will start increasing numbers at the end of the new
// filename's extension until a unique filename is found, or until it runs
// out of numbers.
//
// Entry:
//      lpszBaseFile   - base filename
//      lpszExtension  - extension to replace lpszBaseFile's extension with;
//                       if NULL, lpszBaseFile's extension will be preserved
//      lpszUniqueFile - buffer to place the resulting filename in;
//                       SYM_MAX_PATH * sizeof(TCHAR) bytes long
//
// Exit:
//      TRUE  if unique filename was created in lpszUniqueFile
//      FALSE if error (the contents of lpszUniqueFile is undefined)
//
// Note:
//      SYM_VXD: all strings in OEM!
//      Some of the code in this function was derived from
//      BackupInfectedFile().
//
//---------------------------------------------------------------------------
BOOL WINAPI BuildUniqueFileName (
    LPCTSTR lpszBaseFile,
    LPCTSTR lpszExtension,
    LPTSTR  lpszUniqueFile
    )
    {
    auto UINT  i;
    auto int   nExtLen, nCountLen, nExtensionLength;
    auto TCHAR szCount[10];
    auto TCHAR szNewExtension[SYM_MAX_PATH+1];
    auto TCHAR szExtension[SYM_MAX_PATH];

                                        // Validate input parameters

    SYM_ASSERT ( lpszBaseFile );
    SYM_VERIFY_STRING ( lpszBaseFile );
    SYM_ASSERT ( lpszUniqueFile );
    SYM_VERIFY_BUFFER ( lpszUniqueFile, SYM_MAX_PATH * sizeof(TCHAR) );

                                        // Create a valid extension in
                                        // szExtension.  This is based on
                                        // what the user gave us in
                                        // lpszExtension.

    if ( lpszExtension )
        {
        SYM_VERIFY_STRING ( lpszExtension );

        if ( STRLEN ( lpszExtension ) >= SYM_MAX_PATH )
            {
            return ( FALSE );
            }

        STRCPY ( szExtension, lpszExtension );
        }
    else
        {
        if ( NameReturnExtensionEx ( lpszBaseFile,
                                     szExtension,
                                     SYM_MAX_PATH ) != NOERR )
            {
            return ( FALSE );
            }
        }

    nExtensionLength = STRLEN ( szExtension );

                                        // Create initial unique filename

    STRCPY ( lpszUniqueFile, lpszBaseFile );

    NameStripExtension ( lpszUniqueFile );

    if ( STRLEN ( lpszUniqueFile ) +
         ( nExtensionLength ? nExtensionLength + STRLEN ( _T(".") ) : 0 ) >=
         SYM_MAX_PATH
       )
        {
        return ( FALSE );
        }

    NameAppendExtension ( lpszUniqueFile, szExtension );

    SYM_ASSERT ( STRLEN ( lpszUniqueFile ) < SYM_MAX_PATH );

                                        // Adjust nExtensionLength to contain
                                        // not the current length of
                                        // szExtension, but the minimum
                                        // length of the extension of the
                                        // produced unique filename.  The
                                        // minimum length is determined by
                                        // the length of the largest number
                                        // in an extension that describes a
                                        // unique file.

    if ( nExtensionLength < 3 )
        {
        nExtensionLength = 3;
        }

                                        // Start the loop for searching for
                                        // unique filename.

    for ( i = 0; i <= 999; i++ )
        {
                                        // Check if the current filename
                                        // exists.  If it doesn't, then
                                        // return success.

#if defined(SYM_VXD)
        auto char szUniqueFile[SYM_MAX_PATH*sizeof(WCHAR)];

        VxDGenericStringConvert ( lpszUniqueFile,
                                  CHARSET_OEM,
                                  szUniqueFile,
                                  CHARSET_CURRENT );

        if ( !FileExists ( szUniqueFile ) )
#else
        if ( !FileExists ( lpszUniqueFile ) )
#endif
            {
            return ( TRUE );
            }

                                        // If the current filename exists,
                                        // then generate a number to replace
                                        // part of its extension.

        _ConvertWordToString ( i, szCount, 10, 0 );
        nCountLen = STRLEN ( szCount );

                                        // Validate that the textual value of
                                        // i can fit in a buffer
                                        // nExtensionLength characters long.

        SYM_ASSERT ( nCountLen <= nExtensionLength );

                                        // Copy the first few szExtension
                                        // characters to szNewExtension, such
                                        // that after appending the newly
                                        // generated number to
                                        // szNewExtension, the produced
                                        // string is not longer than
                                        // szExtension.

        STRNCPY ( szNewExtension, szExtension, nExtensionLength - nCountLen );
        szNewExtension[nExtensionLength - nCountLen] = EOS;

                                        // Pad szNewExtension with 0's if the
                                        // newly generated string (after
                                        // appending the number) would
                                        // contain less than nExtensionLength
                                        // characters.

        nExtLen = STRLEN ( szNewExtension );

        if ( nCountLen + nExtLen < nExtensionLength )
            {
            StringAppendNChars ( szNewExtension,
                                 _T('0'),
                                 nExtensionLength - ( nCountLen + nExtLen ) );

            nExtLen = STRLEN ( szNewExtension );
            }

                                        // Concatenate szNewExtension with
                                        // the new number.

        STRCAT ( szNewExtension, szCount );

        SYM_ASSERT ( STRLEN ( szNewExtension ) == nExtensionLength );

                                        // Replace the old extension of the
                                        // unique file with the new
                                        // extension.

        NameStripExtension ( lpszUniqueFile );

        if ( STRLEN ( lpszUniqueFile ) +
             ( nExtLen ? nExtLen + STRLEN ( _T(".") ) : 0 ) >=
             SYM_MAX_PATH
           )
            {
            return ( FALSE );
            }

        NameAppendExtension ( lpszUniqueFile, szNewExtension );
        }

    return ( FALSE );
    }


//---------------------------------------------------------------------------
//
// BOOL WINAPI BackupInfectedFile (
//      LPTSTR  lpszInfectedFile,       // Infected file (Input)
//      LPTSTR  lpszBackupExtension,    // Extension for backup file (Input)
//      LPTSTR  lpszBackupFile          // Name of backup file (Output)
//      );
//
// This function backs up an infected file, applying the extension
// specified by the user.
//
// Entry:
//      lpszInfectedFile
//      lpszBackupExtension
//      lpszBackupFile
//
// Exit:
//      TRUE  if the backup was successful.
//      FALSE if the backup was not successful.
//
//---------------------------------------------------------------------------
BOOL WINAPI BackupInfectedFile (LPTSTR lpszInfectedFile,
                                LPTSTR lpszBackupExtension,
                                LPTSTR lpszBackupFile)
    {
    if ( BuildUniqueFileName ( lpszInfectedFile,
                               lpszBackupExtension,
                               lpszBackupFile ) )
        {
#if defined(SYM_VXD)
        auto char szInfected[SYM_MAX_PATH*sizeof(WCHAR)];
        auto char szBackup[SYM_MAX_PATH*sizeof(WCHAR)];

        VxDGenericStringConvert ( lpszInfectedFile,
                                  CHARSET_OEM,
                                  szInfected,
                                  CHARSET_CURRENT );

        VxDGenericStringConvert ( lpszBackupFile,
                                  CHARSET_OEM,
                                  szBackup,
                                  CHARSET_CURRENT );

        return ( !FileCopy(szInfected, szBackup, NULL) );
#else
        return ( !FileCopy(lpszInfectedFile, lpszBackupFile, NULL) );
#endif
        }

    return ( FALSE );
    }
#endif      // #ifndef SYM_NLM


//---------------------------------------------------------------------------
//
// BOOL WINAPI PAMGreenLight ( LPBYTE lpbyFStartBuffer )
//
// This function examines the NAVEX request buffer from the ALG scanner to
// decide whether or not to suspend PAM.
//
// Entry:
//      lpbyFStartBuffer + 768 is where the NAVEX request buffer is....
//
// Exit:
//      TRUE  if PAM should scan
//      FALSE if a request of 0 was found and PAM should not scan...
//
//---------------------------------------------------------------------------

BOOL WINAPI PAMGreenLight(LPBYTE lpbyFStartBuffer)
{
    LPWORD      lpwTemp;
    WORD        wCount, wMax;

    // point at our NAVEX request buffer

    lpwTemp = (LPWORD)(lpbyFStartBuffer + 768);

    wMax = *lpwTemp;

    lpwTemp++;

    // if we have a NAVEX request value of zero, then don't apply PAM!

    for (wCount=0;wCount<wMax;wCount++)
        if (*lpwTemp++ == 0)
            return(FALSE);

    return(TRUE);
}



///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//
// Virus Scanning Functions
//
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/*@API:*************************************************************************
@Declaration: VSTATUS SYM_EXPORT WINAPI VLScanFile ( HVCONTEXT hContext,
LPTSTR lpszFileName, LPHVLVIRUS lphVirus )

@Description:
This routine scans the specified file for *FILE* viruses.  If the specified
file can be accessed and does not has a virus, then the function will return
a VS_OK and lphVirus will point to a NULL.If the file can't be accessed, the
function will returned appropriate erro code.  If the specified file can be
accessed and does have a virus, the function will return a VS_OK and set
lphVirus to a valid non-NULL handle of type HVLVIRUS. This should not be used
on BOO files.

@Parameters:
$hContext$ This is a valid handle to a context provided by VLScanInit.

$lpszFileName$ This is a far pointer to a zero terminated string containing the
full path and filename of the file to be scanned.

$lphVirus$ This is a pointer to HVLVIRUS handle, and is set by this function
to a non-NULL value if a virus was found, otherwise it is set to NULL.
Information can be obtained about the handle by calling VLVirusInfo.

@Returns:

This function returns
     VS_OK if scanning was successfully completed whether or not a virus
           was found. A non-NULL value is lphVirus indicates infection.

     on error:
         VS_INVALID_ARG
         VS_MEMALLOC
         VS_NO_ACCESS
         VS_ENTRY_UNAVAIL
         VS_FILE_ERROR


@Include: avapi.h
*******************************************************************************/
VSTATUS SYM_EXPORT WINAPI VLScanFile
(
    HVCONTEXT hContext,                 // [in] a valid HVCONTEXT handle
    LPTSTR    lpszFileName,             // [in] a filename to scan for viruses
    LPHVLVIRUS  lphVirus                  // [in/out] HVLVIRUS to fill in or NULL
)
{
    HVIRUS      hVirusIndex = 0;
    HVLVIRUS    hVirus;
    FILEOBJECT_T stFileObject;
    FILEINFO_T  stFileInfo;
    BOOL        bFoundVirus;
    HLENG       hLEng = NULL;
    PAMLHANDLE  hLPAM = NULL;

    //////////////////////////////////////////////////////////////////////////
     SYM_ASSERT(lphVirus);
     *lphVirus = NULL;

    //////////////////////////////////////////////////////////////////////////
    // Verify the structure pointer and its signature
    //////////////////////////////////////////////////////////////////////////

    if (!VLValidContext(hContext))
        return(VS_INVALID_ARG);

    //////////////////////////////////////////////////////////////////////////
    // make sure we have a valid global context
    //////////////////////////////////////////////////////////////////////////

    lpgCallBack = hContext->lpstCallBack;

    //////////////////////////////////////////////////////////////////////////
    // create a file object for AVENGE and open the file
    //////////////////////////////////////////////////////////////////////////

    STRCPY(stFileInfo.lpstrPathName,lpszFileName);
    stFileObject.lpParent = NULL;
    stFileObject.lpvInfo = &stFileInfo;
    stFileObject.lpCallBacks = &gstNAVFileCallBacks;
    stFileObject.lpszName = NULL;

    //////////////////////////////////////////////////////////////////////////
    // Open the file to scan for read
    //////////////////////////////////////////////////////////////////////////

#if defined(SYM_VXD) || defined(SYM_NTK)

    // Allow shared reading but not writing for VXD and NTK

    stFileInfo.hFile = VLFileOpen(stFileInfo.lpstrPathName,READ_ONLY_FILE | OF_SHARE_DENY_NONE);

#else

    // Leave this the same for every other platform for fear of breakage

    stFileInfo.hFile = VLFileOpen(stFileInfo.lpstrPathName,READ_ONLY_FILE);

#endif // #if defined(SYM_VXD) || defined(SYM_NTK)

    if (stFileInfo.hFile == HFILE_ERROR)
    {
        if ( hContext->wFlags & VLINIT_HANDLE_NO_ACCESS )
        {
            hVirus = (HVLVIRUS)VLPermMemoryAlloc(sHVLVIRUS);

            if (hVirus == NULL)
                return ( VS_MEMALLOC );

            VLSetHVLVIRUSFile(hVirus,
                        hContext,
                        INVALID_ENTRY_NUM,
                        VIT_NO_ACCESS | VIT_FILE,
                        lpszFileName);

            *lphVirus = hVirus;

            DateGetCurrentDateTime(&hVirus->uDate,&hVirus->uTime);
         }
         return( VS_NO_ACCESS );
    }

    ///////////////////////////////////////////////////////////////////////////
    // Scan that file, BABY!
    // Avenge scanning first
    ///////////////////////////////////////////////////////////////////////////

    if (!VLEngLocalInit(hContext, &hLEng))
    {
        return(VS_NO_ACCESS);
    }

    if (EngScanFile(hLEng,
                    &stFileObject,
                    LENG_SCAN_FILE_FLAG_SCAN_ALL,
                    0xDEADBEEFUL,
                    &bFoundVirus,
                    &hVirusIndex) != ENGSTATUS_OK)
    {
        VLEngLocalClose(hLEng);
        VLFileClose(stFileInfo.hFile);
        return(VS_FILE_ERROR);
    }

    if (!bFoundVirus)
    {
        hVirusIndex = 0;
    }

    ///////////////////////////////////////////////////////////////////////////
    // Now, scan with PAM if no virus had been found with Avenge
    // 
    ///////////////////////////////////////////////////////////////////////////

    if (bFoundVirus == FALSE &&
        hContext->hGPAM != NULL &&
        PAMGreenLight((LPBYTE)&hLEng->stAlgScanData))  
    {
        PAMSTATUS   pamStatus;
        WORD        wVirusID;

#if defined (AVAPI_MT)

        pamStatus = PAMLocalInit(HVC_szPAMScanDataFile(hContext),
                                 NULL,                  // no swap file!
                                 (PAMGHANDLE)hContext->hGPAM,
                                 &hLPAM);

#else   

        pamStatus = PAMSTATUS_OK;
        hLPAM = hContext->hLPAM;

#endif  

        if (pamStatus == PAMSTATUS_OK)
        {
            pamStatus = PAMScanFile(hLPAM,
                                    stFileInfo.hFile,
                                    0,      // no flags for now; need one for SYS
                                    &wVirusID);

            if (PAMSTATUS_OK == pamStatus)
            {
                hVirusIndex = 0;
            }
            else if (PAMSTATUS_VIRUS_FOUND == pamStatus)
            {
                if (DetermineVirusIndex(hContext,
                                        HVC_szVirScanDataFile(hContext),
                                        wVirusID,
                                        &hVirusIndex) != EXTSTATUS_OK)
                {
                    hVirusIndex = 0;
                }

            }
            else
                hVirusIndex = 0;

#if defined(AVAPI_MT)
            PAMLocalClose(hLPAM);
#endif  

        }
    }
    ///////////////////////////////////////////////////////////////////

    if (hVirusIndex == 0)                   // no virus was found in the file!
    {
        EXTSTATUS   extResult;
        WORD        wVirusID;

        //////////////////////////////////////////////////////////////////////
        // No virus found using Avenge or PAM.  Try "fail-safe" detections!
        //
        // &hLEng->stAlgScanData:
        //    First 256 bytes   = FSTART returned code
        //    Next  256 bytes   = first 64 bytes of host file
        //    Next  1024 bytes  = external scan requests
        //    Next 4K-1536 bytes = work buffer for whatever
        //////////////////////////////////////////////////////////////////////

        extResult = EXTScanFile(hContext,
                                lpszFileName,
                                stFileInfo.hFile,
                                (LPBYTE)&hLEng->stAlgScanData,
                                (LPBYTE)&hLEng->stAlgScanData + 1536,
                                EXTERNAL_DETECT_VERSION,
                                &wVirusID);

        if (EXTSTATUS_OK == extResult)
        {
            VLEngLocalClose(hLEng);
            VLFileClose(stFileInfo.hFile);
            return(VS_OK);              // no virus found
        }

        if (EXTSTATUS_VIRUS_FOUND == extResult)
        {
            if (DetermineVirusIndex(hContext,
                                    HVC_szVirScanDataFile(hContext),
                                    wVirusID,
                                    &hVirusIndex) != EXTSTATUS_OK) 
            {
                hVirusIndex = 0xFFFFU;  // error
            }
        }
        else
            hVirusIndex = 0xFFFFU;      // error

    }

    //////////////////////////////////////////////////////////////////////////
    // Close the file and determine what to do!
    //////////////////////////////////////////////////////////////////////////

    VLFileClose(stFileInfo.hFile);

    VLEngLocalClose(hLEng);

    if (hVirusIndex == 0xFFFF)
    {
        if ( hContext->wFlags & VLINIT_HANDLE_NO_ACCESS )
        {
            hVirus = (HVLVIRUS)VLPermMemoryAlloc(sHVLVIRUS);
            if (hVirus == NULL)
               return ( VS_MEMALLOC );

            VLSetHVLVIRUSFile(hVirus,
                            hContext,
                            INVALID_ENTRY_NUM,
                            VIT_NO_ACCESS | VIT_FILE,
                            lpszFileName);

            *lphVirus = hVirus;

            DateGetCurrentDateTime(&hVirus->uDate,&hVirus->uTime);

        }
        return( VS_NO_ACCESS );
    }

    //////////////////////////////////////////////////////////////////////////
    // We have a virus!  Deal with it!
    //////////////////////////////////////////////////////////////////////////

    hVirus = (HVLVIRUS)VLPermMemoryAlloc(sHVLVIRUS);

    if (hVirus == NULL)
        return(VS_MEMALLOC);

    VLSetHVLVIRUSFile(hVirus,
                    hContext,
                    hVirusIndex,
                    VIT_SCANNED | VIT_VALID_N30 | VIT_HAS_A_VIRUS | VIT_FILE,
                    lpszFileName);

    if (!VLLoadN30(hContext,hVirusIndex,&hVirus->stVirusInfoBlock))
    {

        VLPermMemoryFree(hVirus);
        *lphVirus = NULL;
        return(VS_ENTRY_UNAVAIL );
    }

    ///////////////////////////////////////////////////////////////////////////
    // A-OK good buddy
    ///////////////////////////////////////////////////////////////////////////

    DateGetCurrentDateTime(&hVirus->uDate,&hVirus->uTime);

    *lphVirus = hVirus;

    return(VS_OK);
}


#ifndef SYM_NLM

/*@API:*************************************************************************
@Declaration:

VSTATUS SYM_EXPORT WINAPI VLScanBoot ( HVCONTEXT  hContext, BYTE byDrive,
BYTE byBootSecNum, LPBYTE lpBooBuffer, LPHVLVIRUS lphVirus )

@Description:
If lpBooBuffer == NULL:
    This routine scans the boot sector of the specified partition on the
    specified drive.

If lpBooBuffer != NULL:
    This routine scans the sector buffer pointed to by lpBooBuffer for Boot
    sector viruses.  HVLVIRUS handle will indicate that this is a "disembodied"
    infection and no repair can be applied to the specified handle.

    A valid HVLVIRUS handle is returned in lphVirus if a virus is found.

@Parameters:
$hContext$ This is a valid handle to a context provided by VLScanInit.

$byDrive$ This is the physical drive number to scan. 0x00=A:, 0x80=first
physical drive.

$byBootSecNum$ This is the partition number (0 to 3) which contains the boot
sector to scan on the specified physical drive.

$lpBooBuffer$ This is a far pointer to a sector-sized buffer which contains
a boot sector image.  If lpBooBuffer != NULL, then byDrive and byBootSecNum
are ignored, otherwise lpBooBuffer is ignored.

$lphVirus$ This is a pointer to HVLVIRUS handle, and is set by this function
to a non-NULL value if a virus was found, otherwise it is set to NULL.
Information can be obtained about the handle by calling VLVirusInfo.

@Returns:
This function returns
     VS_OK if scanning was successfully completed whether or not a virus
           was found. A non-NULL value is lphVirus indicates infection.

     on error:
         VS_INVALID_ARG
         VS_MEMALLOC
         VS_NO_ACCESS
         VS_ENTRY_UNAVAIL

@Include: avapi.h
*******************************************************************************/

VSTATUS SYM_EXPORT WINAPI VLScanBoot
(
    HVCONTEXT  hContext,                // [in] a valid HVCONTEXT handle
    BYTE       byDrive,                 // [in] physical drive # to scan
    BYTE       byBootSecNum,            // [in] partition # to scan (0-3)
    LPBYTE     lpBooBuffer,             // [in] disembodied buffer to scan
    LPHVLVIRUS   lphVirus

)
{
    LPBYTE      lpImage;
    DWORD       dwImageSize;
    WORD        wResult;
    WORD        wCylinder = 0;
    BYTE        bySector = 1;
    BYTE        byHead = 0;
    BYTE        bySystemType;
    HVLVIRUS    hVirus;
    BOOL        bDisembodied;
    BOOL        bFoundVirus;
    UINT        uResult;
    HLENG       hLEng = NULL;

    SYM_ASSERT(VLSDiskReadPhysical);
    *lphVirus = NULL;

    //////////////////////////////////////////////////////////////////////////
    // Verify the structure pointer and its signature
    //////////////////////////////////////////////////////////////////////////

    if (!VLValidContext(hContext))
        return(VS_INVALID_ARG);

    //////////////////////////////////////////////////////////////////////////
    // make sure we have a valid global context
    //////////////////////////////////////////////////////////////////////////

    lpgCallBack = hContext->lpstCallBack;

    dwImageSize = (SECTOR_SIZE * 8);

    if (lpBooBuffer == NULL)
    {
        ///////////////////////////////////////////////////////////////////////
        // Allocate memory for the boot sector image
        ///////////////////////////////////////////////////////////////////////

        lpImage = (LPBYTE)VLTempMemoryAlloc(dwImageSize);
        if (lpImage == NULL)
            return(VS_MEMALLOC );

        ///////////////////////////////////////////////////////////////////////
        // Get the partition table so we can determine where boot sectors are!
        // If this is a floppy drive, the sector we retrieve is the boot sector
        ///////////////////////////////////////////////////////////////////////
        //
        // 1) Get the drive type (on floppies, we don't have a partition table)
        // 2) Read 1 sec from Track 0, Sector 1, Side 0 from the proper drive
        //
        ///////////////////////////////////////////////////////////////////////

        uResult = VLSDiskReadPhysical(lpImage, 1, 0, 1, 0, byDrive);

        ///////////////////////////////////////////////////////////////////////
        // uResult == 0 on success
        ///////////////////////////////////////////////////////////////////////

        if (uResult == 0 && byDrive >= 0x80)
        {
            ///////////////////////////////////////////////////////////////////
            // The drive we're scanning is a hard drive AND we were able to
            // successfully read the partition table.  Now, determine where the
            // desired boot sector is and scan it!
            ///////////////////////////////////////////////////////////////////

            VLGetBootSectorLocation ( lpImage,
                                 (UINT)byBootSecNum,
                                 &wCylinder,
                                 &bySector,
                                 &byHead,
                                 &bySystemType);

            ///////////////////////////////////////////////////////////////////
            // Now we know where the boot sector is.  Get it!
            ///////////////////////////////////////////////////////////////////

            uResult = VLSDiskReadPhysical(lpImage,
                                          1,
                                          wCylinder,
                                          bySector,
                                          byHead,
                                          byDrive);
        }

        if (uResult != 0)
        {
            VLTempMemoryFree(lpImage);      // free our temp. sector buffer

            if (hContext->wFlags & VLINIT_HANDLE_NO_ACCESS )
            {
               hVirus = (HVLVIRUS)VLPermMemoryAlloc(sHVLVIRUS);

               if (hVirus == NULL)
                  return(VS_MEMALLOC);

               VLSetHVLVIRUSBoot(hVirus,
                               hContext,
                               INVALID_ENTRY_NUM,
                               VIT_NO_ACCESS | VIT_BOOTPART,
                               byDrive,
                               byBootSecNum,
                               BI_BOOT_SECTOR,
                               bySystemType);

               DateGetCurrentDateTime(&hVirus->uDate,&hVirus->uTime);

              *lphVirus = hVirus;
            }
            return ( VS_NO_ACCESS);
        }
    }
    else
        lpImage = lpBooBuffer;          // user passed in BOO buffer


    ///////////////////////////////////////////////////////////////////////////
    // At this point, the proper boot sector is in lpImage buffer
    //
    ///////////////////////////////////////////////////////////////////////////

    if (!VLEngLocalInit(hContext, &hLEng))
    {
        return(VS_NO_ACCESS);
    }

    if (EngScanBoot(hLEng,
                    lpImage,
                    dwImageSize,
                    &bFoundVirus,
                    (LPHVIRUS)&wResult) != ENGSTATUS_OK)
    {
        return(VS_NO_ACCESS);
    }

    VLEngLocalClose(hLEng);

    if (bFoundVirus == FALSE)
    {
        EXTSTATUS   extResult;
        WORD        wVirusID;
        WORD        wVirusIndex;
        BYTE        byTempDrive;

        //////////////////////////////////////////////////////////////////////
        // No virus found using Avenge engine.  Try "fail-safe" detections!
        //
        //////////////////////////////////////////////////////////////////////

        // first, if lpBooBuffer is passed in as opposed to the Cyl,Sec,Head,
        // tell the EXT engine that the buffer is all we have to give it!

        if (lpBooBuffer)
            byTempDrive = 0xFF;
        else
            byTempDrive = byDrive;

        extResult = EXTScanBoot(hContext,
                                byTempDrive,
                                byBootSecNum,
                                bySector, wCylinder, byHead,
                                lpImage,
                                (LPBYTE)NULL,
                                EXTERNAL_DETECT_VERSION,
                                &wVirusID);

        if (EXTSTATUS_OK == extResult)
        {
            if (lpBooBuffer == NULL)            // if BOO buffer wasn't passed in
                VLTempMemoryFree(lpImage);      // free our temp. sector buffer

            return(VS_OK);              // no virus found
        }

        if (EXTSTATUS_VIRUS_FOUND == extResult)
        {
            if (EXTSTATUS_OK ==
                DetermineVirusIndex(hContext,
                                    HVC_szVirScanDataFile(hContext),
                                    wVirusID,
                                    &wVirusIndex))
                wResult = wVirusIndex;
            else
                wResult = 0xFFFFU;      // error
        }
        else
            wResult = 0xFFFFU;

    }

    if (lpBooBuffer == NULL)            // if BOO buffer wasn't passed in
        VLTempMemoryFree(lpImage);      // free our temp. sector buffer

    if (wResult == 0xFFFFU)
    {
        if ( hContext->wFlags & VLINIT_HANDLE_NO_ACCESS )
        {
            hVirus = (HVLVIRUS)VLPermMemoryAlloc(sHVLVIRUS);

            if (hVirus == NULL)
                return(VS_MEMALLOC);

            VLSetHVLVIRUSBoot(hVirus,
                            hContext,
                            INVALID_ENTRY_NUM,
                            VIT_NO_ACCESS | VIT_BOOTPART,
                            byDrive,
                            byBootSecNum,
                            BI_BOOT_SECTOR,
                            bySystemType);

            DateGetCurrentDateTime(&hVirus->uDate,&hVirus->uTime);

            *lphVirus = hVirus;
        }
        return ( VS_NO_ACCESS);
    }

    hVirus = (HVLVIRUS)VLPermMemoryAlloc(sHVLVIRUS);

    if (hVirus == NULL)
        return(VS_MEMALLOC);

    if (lpBooBuffer == NULL)
        bDisembodied = 0;
    else
    {
        bDisembodied = VIT_DISEMBODIED;

        bySystemType    = 0;
        byDrive         = 0;
        byBootSecNum    = 0;
    }

    VLSetHVLVIRUSBoot(hVirus,
                    hContext,
                    wResult,
                    VIT_SCANNED |
                    VIT_BOOTPART |
                    VIT_VALID_N30 |
                    VIT_HAS_A_VIRUS |
                    bDisembodied,
                    byDrive,
                    byBootSecNum,
                    BI_BOOT_SECTOR,
                    bySystemType);

    DateGetCurrentDateTime(&hVirus->uDate,&hVirus->uTime);

    if (!VLLoadN30(hContext,wResult,&hVirus->stVirusInfoBlock))
    {

        VLPermMemoryFree(hVirus);
        return(VS_ENTRY_UNAVAIL);

    }

    ///////////////////////////////////////////////////////////////////////////
    // Return our HVLVIRUS handle
    ///////////////////////////////////////////////////////////////////////////

    *lphVirus = hVirus;
    return(VS_OK);
}

/*@API:*************************************************************************
@Declaration: VSTATUS SYM_EXPORT WINAPI VLScanPart ( HVCONTEXT hContext,
BYTE byDrive, LPBYTE lpPartBuffer, LPHVLVIRUS lphVirus )

@Description:
If lpPartBuffer == NULL:
    This routine scans the MBR sector of the specified drive.  If a virus is
    found, VS_OK is returned, and lphVirus will point to a valid HVLVIRUS handle.

If lpPartBuffer != NULL:
    This routine scans the sector buffer pointed to by lpPartBuffer for
    BOOT viruses.  If a virus is found VS_OK is returned, and lphVirus will
    point to a valid HVLVIRUS handle.  This handle will be "disembodied"
    infection.  In other words, no repair can be applied to the specified handle.

@Parameters:
$hContext$ This is a valid handle to a context provided by VLScanInit.

$byDrive$ This is the physical hard drive number to scan. 0x80=first physical
drive, etc.

$lpPartBuffer$ This is a far pointer to a sector-sized buffer which contains
a partition sector image.  If lpPartBuffer != NULL, then byDrive is ignored,
otherwise lpPartBuffer is ignored.

$lphVirus$ This is a pointer to HVLVIRUS handle, and is set by this function
to a non-NULL value if a virus was found, otherwise it is set to NULL.
Information can be obtained about the handle by calling VLVirusInfo.

@Returns:

This function returns
     VS_OK if scanning was successfully completed whether or not a virus
           was found. A non-NULL value is lphVirus indicates infection.

     on error:
         VS_INVALID_ARG
         VS_MEMALLOC
         VS_NO_ACCESS
         VS_ENTRY_UNAVAIL


@Include: avapi.h
*******************************************************************************/

VSTATUS SYM_EXPORT WINAPI VLScanPart
(
    HVCONTEXT hContext,                 // [in] a valid HVCONTEXT handle
    BYTE byDrive,                       // [in] the physical drive # to scan
    LPBYTE lpPartBuffer,                // [in] disembodied buffer to scan
    LPHVLVIRUS lphVirus

)
{
    LPBYTE      lpImage;
    DWORD       dwImageSize;
    WORD        wResult = 0;
    HVLVIRUS    hVirus;
    BOOL        bDisembodied;
    UINT        uResult;
    BOOL        bFoundVirus;
    HLENG       hLEng = NULL;

    SYM_ASSERT(VLSDiskReadPhysical);
    *lphVirus = NULL;

    //////////////////////////////////////////////////////////////////////////
    // Verify the structure pointer and its signature
    //////////////////////////////////////////////////////////////////////////

    if (!VLValidContext(hContext))
        return(VS_INVALID_ARG);

    //////////////////////////////////////////////////////////////////////////
    // make sure we have a valid global context
    //////////////////////////////////////////////////////////////////////////

    lpgCallBack = hContext->lpstCallBack;

    dwImageSize = (SECTOR_SIZE * 8);

    if (lpPartBuffer == NULL)
    {
        ///////////////////////////////////////////////////////////////////////
        // Allocate memory for the boot sector image
        ///////////////////////////////////////////////////////////////////////

        lpImage = (LPBYTE)VLTempMemoryAlloc(dwImageSize);
        if (lpImage == NULL)
            return(VS_MEMALLOC);

        ///////////////////////////////////////////////////////////////////////
        // Are we trying to scan the MBR on a floppy?? I hope not!
        ///////////////////////////////////////////////////////////////////////

        if (byDrive < 0x80)
        {
            VLTempMemoryFree(lpImage);      // free our temp. sector buffer
            return(VS_DRIVE_ERROR);
        }

        uResult = VLSDiskReadPhysical(lpImage, 1, 0, 1, 0, byDrive);

        // uResult == 0 means success

        if (uResult != 0)
        {
            VLTempMemoryFree(lpImage);      // free our temp. sector buffer

            if (hContext->wFlags & VLINIT_HANDLE_NO_ACCESS)
            {

                hVirus = (HVLVIRUS)VLPermMemoryAlloc(sHVLVIRUS);

                if (hVirus == NULL)
                    return(VS_MEMALLOC);

                VLSetHVLVIRUSBoot(hVirus,
                                hContext,
                                INVALID_ENTRY_NUM,
                                VIT_NO_ACCESS | VIT_BOOTPART,
                                byDrive,
                                0,
                                BI_PARTITION_SECTOR,
                                0);

                DateGetCurrentDateTime(&hVirus->uDate,&hVirus->uTime);
                *lphVirus = hVirus;
            }
            return(VS_NO_ACCESS);
        }
    }
    else
        lpImage = lpPartBuffer;

    //////////////////////////////////////////////////////////////////////////
    // At this point, the proper boot sector is in lpImage buffer
    //
    // The FALSE argument (last arg) should be removed and is used for DOS
    // callbacks.
    //////////////////////////////////////////////////////////////////////////

    if (!VLEngLocalInit(hContext, &hLEng))
    {
        return(VS_NO_ACCESS);
    }

    if (EngScanBoot(hLEng,
                    lpImage,
                    dwImageSize,
                    &bFoundVirus,
                    (LPHVIRUS)&wResult) != ENGSTATUS_OK)
    {
        return(VS_NO_ACCESS);
    }

    VLEngLocalClose(hLEng);

    if (bFoundVirus == FALSE)
    {
        EXTSTATUS   extResult;
        WORD        wVirusID;
        WORD        wVirusIndex;
        BYTE        byTempDrive;

        //////////////////////////////////////////////////////////////////////
        // No virus found using NAV 3 engine.  Try "fail-safe" detections!
        //////////////////////////////////////////////////////////////////////

        // first, if lpPartBuffer is passed in as opposed to the Cyl,Sec,Head,
        // tell the EXT engine that the buffer is all we have to give it!

        if (lpPartBuffer)
            byTempDrive = 0xFF;
        else
            byTempDrive = byDrive;

        extResult = EXTScanPart(hContext,
                                byTempDrive,
                                lpImage,
                                (LPBYTE)NULL,
                                EXTERNAL_DETECT_VERSION,
                                &wVirusID);

        if (EXTSTATUS_OK == extResult)
        {
            if (lpPartBuffer == NULL)       // if PAR buffer wasn't passed in
                VLTempMemoryFree(lpImage);  // free our temp. sector buffer

            return(VS_OK);              // no virus found
        }

        if (EXTSTATUS_VIRUS_FOUND == extResult)
        {
            if (EXTSTATUS_OK ==
                DetermineVirusIndex(hContext,
                                    hContext->szVirScanDataFile,
                                    wVirusID,
                                    &wVirusIndex))
                wResult = wVirusIndex;
            else
                wResult = 0xFFFFU;      // error
        }
        else
            wResult = 0xFFFFU;          // error

    }

    if (lpPartBuffer == NULL)
        VLTempMemoryFree(lpImage);      // free our temp. sector buffer

    if (wResult == 0xFFFF)
    {
        if ( hContext->wFlags & VLINIT_HANDLE_NO_ACCESS )
        {
            hVirus = (HVLVIRUS)VLPermMemoryAlloc(sHVLVIRUS);

            if (hVirus == NULL)
                return(VS_MEMALLOC);

            VLSetHVLVIRUSBoot(hVirus,
                            hContext,
                            INVALID_ENTRY_NUM,
                            VIT_NO_ACCESS | VIT_BOOTPART,
                            byDrive,
                            0,
                            BI_PARTITION_SECTOR,
                            0);

            DateGetCurrentDateTime(&hVirus->uDate,&hVirus->uTime);
            *lphVirus = hVirus;
        }
        return(VS_NO_ACCESS);
    }

    if (wResult == 0)
        return(VS_OK);

    hVirus = (HVLVIRUS)VLPermMemoryAlloc(sHVLVIRUS);

    if (hVirus == NULL)
        return(VS_MEMALLOC);

    if (lpPartBuffer == NULL)
        bDisembodied = 0;
    else
    {
        bDisembodied = VIT_DISEMBODIED;

        byDrive     = 0;
    }

    VLSetHVLVIRUSBoot(hVirus,
                    hContext,
                    wResult,
                    VIT_SCANNED |
                    VIT_BOOTPART |
                    VIT_VALID_N30 |
                    VIT_HAS_A_VIRUS |
                    bDisembodied,
                    byDrive,
                    0,
                    BI_PARTITION_SECTOR,
                    0);

    DateGetCurrentDateTime(&hVirus->uDate,&hVirus->uTime);

    if (!VLLoadN30(hContext,wResult,&hVirus->stVirusInfoBlock))
    {
        VLPermMemoryFree(hVirus);
        return(VS_ENTRY_UNAVAIL);
    }

    ///////////////////////////////////////////////////////////////////////////
    // Return our HVLVIRUS handle
    ///////////////////////////////////////////////////////////////////////////

    *lphVirus = hVirus;
    return(VS_OK);
}

#endif

/*@API:*************************************************************************
@Declaration: VSTATUS SYM_EXPORT WINAPI VLScanMemory (HVCONTEXT hContext,
WORD wFlags, LPHVLVIRUS lphVirus)

@Description:
This function scan memory for viruses.  It always scans real memory (640K),
and optionally scans hi memory. VS_OK is returned to indicate successfull
scan whether or not a virus was found. If lphVirus is pointing to a valid
non-NULL handle, then infection was found, otherwise, no viruses where found
in memory.

@Parameters:
$hContext$ This is a valid handle to a context provided by VLScanInit.

$wFlags$:  1 Scans High and low Memory.  0 scans only low memory.

$lphVirus$ This is a pointer to HVLVIRUS handle, and is set by this function
to a non-NULL value if a virus was found, otherwise it is set to NULL.
Information can be obtained about the handle by calling VLVirusInfo.

@Returns:

This function returns
     VS_OK if scanning was successfully completed whether or not a virus
           was found. A non-NULL value is lphVirus indicates infection.

     on error:
         VS_INVALID_ARG
         VS_MEMALLOC
         VS_NO_ACCESS
         VS_ENTRY_UNAVAIL

@Include: avapi.h
*******************************************************************************/

VSTATUS SYM_EXPORT WINAPI VLScanMemory
(
    HVCONTEXT   hContext,
    WORD        wFlags,
    LPHVLVIRUS  lphVirus
)
{
    HVLVIRUS         hVirus;
    WORD             wResult;
    DWORD            dwEndAddress;
    BOOL             bFoundVirus;
    MEMORYOBJECT_T   stMemoryObject;
    HLENG            hLEng = NULL;

    ///////////////////////////////////////////////////////////////////////////
    // Make sure we have a valid context
    ///////////////////////////////////////////////////////////////////////////

    *lphVirus = NULL;

    if (!VLValidContext(hContext))
        return(VS_INVALID_ARG);

    //////////////////////////////////////////////////////////////////////////
    // make sure we have a valid global context
    //////////////////////////////////////////////////////////////////////////

    lpgCallBack = hContext->lpstCallBack;

    //////////////////////////////////////////////////////////////////////////
    // Set up parameters for EngScanMemory()
    //////////////////////////////////////////////////////////////////////////

    stMemoryObject.lpCallBacks = &gstNAVMemoryCallBacks;
    dwEndAddress = wFlags ? 0x100000ul : 0xA0000ul;

    //////////////////////////////////////////////////////////////////////////
    // Scan memory with Avenge Engine
    //////////////////////////////////////////////////////////////////////////

    if (!VLEngLocalInit(hContext, &hLEng))
    {
        return(VS_NO_ACCESS);
    }

    if (EngScanMemory(hLEng,
                      &stMemoryObject,
                      0,
                      dwEndAddress,
                      LENG_SCAN_TOP_OF_MEMORY,
                      0,
                      &bFoundVirus,
                      (LPHVIRUS)&wResult) != ENGSTATUS_OK)
    {
        return(VS_NO_ACCESS);
    }

    VLEngLocalClose(hLEng);

    if (!bFoundVirus)                   // no virus found - scan with NavEx
    {
        EXTSTATUS   extResult;
        WORD        wVirusID;
        WORD        wVirusIndex;

        //////////////////////////////////////////////////////////////////////
        // No virus found using NAV 3 engine.  Try "fail-safe" detections!
        //
        //////////////////////////////////////////////////////////////////////

        extResult = EXTScanMemory(hContext,
                                  EXTERNAL_DETECT_VERSION,
                                  wFlags,   // Assume 0=don't scan high memory
                                  &wVirusID);

        if (EXTSTATUS_OK == extResult)
            return(VS_OK);              // no virus found

        if (EXTSTATUS_VIRUS_FOUND == extResult)
        {
            if (EXTSTATUS_OK ==
                DetermineVirusIndex(hContext,
                                    HVC_szVirScanDataFile(hContext),
                                    wVirusID,
                                    &wVirusIndex))
                wResult = wVirusIndex;
            else
                wResult = 0xFFFEU;      // FFFE -> Virus in memory but we
                                        // don't know its ID number, perhaps
                                        // we have an out of sync VIRSCAN.DAT
                                        // and AVEX.
        }
        else
            wResult = 0xFFFFU;          // error (VLLoadN30 will fail)
    }

    if (0xFFFFU == wResult)             // only = if we have an error return
        return(VS_NO_ACCESS);           // from EXTScanMemory


    ///////////////////////////////////////////////////////////////////////////
    // Virus found, return an hVirus structure...
    ///////////////////////////////////////////////////////////////////////////

    hVirus = (HVLVIRUS)VLPermMemoryAlloc(sHVLVIRUS);

    if (hVirus == NULL)
        return(VS_MEMALLOC);

    MEMSET (hVirus, 0, sizeof(VIRINFOTYPE));

    if (!VLLoadN30(hContext,wResult,&hVirus->stVirusInfoBlock))
    {
       hVirus->dwFlags |= VIT_UNKOWN_VIRUS;
    }

    hVirus->lpVContext  = hContext;
    hVirus->hNext       = NULL;
    hVirus->uDBEntryNum = wResult;
    hVirus->dwFlags     = VIT_SCANNED | VIT_UNREPAIRABLE | VIT_VALID_N30 |
                          VIT_FOUND_IN_MEMORY | VIT_HAS_A_VIRUS |
                          VIT_DISEMBODIED;

    DateGetCurrentDateTime(&hVirus->uDate,&hVirus->uTime);
    *lphVirus = hVirus;

    return(VS_OK);
}

#if 0
/*@API:*************************************************************************
@Declaration:
    BOOL SYM_EXPORT VLSaveMemoryInfectionData ( HVLVIRUS hVirus );
@Description:
this function is called after scanning memory with VLScamMemory(),
and a virus was found in memory.  It checks whether the infection was caused
by the One_half virus and if so, it save repair information.

@Parameters:
$hVirus$    virus handle as returned by VLScanMemory()
$uFlags$    for future use

@Returns:
TRUE  if success
FALSE otherwise

@Include: avapi.h
*******************************************************************************/
BOOL SYM_EXPORT VLSaveMemoryInfectionData ( HVLVIRUS hVirus, UINT uFlags )
{
     LPBYTE      lpBuffer;
     HVCONTEXT   hContext;

     hContext = hVirus->lpVContext;

     SYM_ASSERT ( hContext );

     lpBuffer = (LPBYTE)VLTempMemoryAlloc(2048);

     if (lpBuffer == NULL)
         return ( FALSE );

     if ( hVirus->uDBEntryNum == VL_ONE_HALF_ID )
         {
         GetInfectionData ( lpBuffer, NULL, 0 );
         }

     VLTempMemoryFree(lpBuffer);
     return ( TRUE);
}
#endif      // #ifndef SYM_NLM

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//
// Repair functions follow...
//
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


#ifndef SYM_NLM
/*@API:*************************************************************************
@Declaration: BOOL WINAPI VLCanRepairItem(HVLVIRUS hVirus)


@Description:
This function determines whether or not the specified item can be repaired.
Disembodied items, already repaired items, deleted items, and inaccessable
items cannot be repaired.

@Parameters:
$hVirus$ This is a valid HVLVIRUS handle.

@Returns:
    This function returns

        TRUE        if the specified item has a "chance" of being repaired.

        FALSE       if there is no way the specified item could be repaired.

@Include: avapi.h
*******************************************************************************/
BOOL SYM_EXPORT WINAPI VLCanRepairItem
(
    HVLVIRUS hVirus                       // [in] a valid HVLVIRUS handle
)
{
    ///////////////////////////////////////////////////////////////////////////
    // for now don't allow DIR2 repairs...
    ///////////////////////////////////////////////////////////////////////////

    if ( (hVirus->stVirusInfoBlock.wInfo & bitCTRL_NOTCLEAN) ||
         ((hVirus->stVirusInfoBlock.wInfo & bitINFO_DIR2) &&
           !(hVirus->dwPOFlags & fPO_DIR2_REPAIR)) )
        return(FALSE);

    ///////////////////////////////////////////////////////////////////////////
    // already tried repairing this file!
    ///////////////////////////////////////////////////////////////////////////

    if (hVirus->dwFlags & (VIT_UNREPAIRABLE | VIT_DISEMBODIED | VIT_DELETED
                            | VIT_NO_ACCESS))
        return(FALSE);

    if (hVirus->dwFlags & (VIT_FILE | VIT_BOOTPART))
        return(TRUE);

    return(FALSE);
}


/*@API:*************************************************************************
@Declaration: BOOL WINAPI VLCanDeleteItem(HVLVIRUS hVirus)


@Description:
This function determines whether or not the specified item can be deleted.
Disembodied items, already deleted items, and inaccessable items cannot be
deleted.  The DIR2 virus is an example of a virus which should not be
deleted from infected files.

@Parameters:
$hVirus$ This is a valid HVLVIRUS handle.

@Returns:
    This function returns

        TRUE        if the specified item may be deleted.

        FALSE       if the specified item cannot or should not be deleted

@Include: avapi.h
*******************************************************************************/

BOOL SYM_EXPORT WINAPI VLCanDeleteItem
(
    HVLVIRUS hVirus                       // [in] a valid HVLVIRUS handle
)
{
    if ( (hVirus->stVirusInfoBlock.wInfo & bitINFO_DIR2) &&
         !(hVirus->dwPOFlags & fPO_DIR2_DELETE) )
        return(FALSE);

    if (hVirus->dwFlags & (VIT_DELETED | VIT_NO_ACCESS | VIT_DISEMBODIED))
        return(FALSE);

    if (hVirus->dwFlags & VIT_FILE)
        return(TRUE);

    return(FALSE);
}


/*@API:*************************************************************************
@Declaration: VSTATUS WINAPI VLRepairItem(HVLVIRUS hVirus)

@Description:
This routine can be used to repair items (files, sectors) infected with
viruses.  It will automatically determine what type of repair is necessary
and apply it (multiple times, if necessary).  The HVLVIRUS handle will be updated to reflect
the repair status of the item.

@Parameters:
$hVirus$ This is a valid HVLVIRUS handle.

@Returns:
This function returns a result of type VSTATUS (type definition found in
AVAPI.H).

    VS_OK           will be returned on success
    VS_INVALID_ARG  will be returned if the context is invalid
    VS_CANT_REPAIR  will be returned if the item cannot be repaired
    VS_MEMALLOC     will be returned if memory cannot be allocated
    VS_FILE_ERROR   will be returned if there is an error accessing the item
    VS_CRIT_ERROR   will be returned on critical error
    VS_DRIVE_ERROR  will be returned on a drive access error

@Include: avapi.h
*******************************************************************************/

VSTATUS SYM_EXPORT WINAPI VLRepairItem
(
    HVLVIRUS hVirus                       // [in] a valid HVLVIRUS handle
)
{
    if (hVirus == NULL || hVirus->lpVContext == NULL)
    {
        return(VS_INVALID_ARG);
    }
    else if (VLCanRepairItem(hVirus) == FALSE)
    {
        return(VS_CANT_REPAIR);
    }
    else if (hVirus->dwFlags & VIT_FILE)
    {
        return(VLRepairFile(hVirus));
    }
    else if (hVirus->dwFlags & VIT_BOOTPART)
    {
        return(VLRepairBootPart(hVirus));
    }
    else
    {
        return(VS_CANT_REPAIR);
    }
}

/*@API:*************************************************************************
@Declaration: VSTATUS WINAPI VLRepairFile(HVLVIRUS hVirus)

@Description:
This routine can be used to repair files infected with viruses.  It will
automatically determine what type of repair is necessary and apply it
(multiple times, if necessary).  The HVLVIRUS handle will be updated to reflect
the repair status of the file.

@Parameters:
$hVirus$ This is a valid HVLVIRUS handle.

@Returns:
This function returns a result of type VSTATUS (type definition found in
AVAPI.H).

    VS_OK           will be returned on success
    VS_INVALID_ARG  will be returned if the context is invalid
    VS_CANT_REPAIR  will be returned if the item cannot be repaired
    VS_MEMALLOC     will be returned if memory cannot be allocated
    VS_FILE_ERROR   will be returned if there is an error accessing the item
    VS_CRIT_ERROR   will be returned on critical error

@Include: avapi.h
*******************************************************************************/

VSTATUS SYM_EXPORT WINAPI VLRepairFile
(
    HVLVIRUS hVirus                       // [in] a valid HVLVIRUS handle
)
{
    UINT        uRepairTries;
    UINT        uResult;
    HVCONTEXT   hContext;
    HVLVIRUS    hTempVirus, hCurVirus;
    VSTATUS     vsScanResult;
    EXTSTATUS   extResult;
    UINT        uAttr;
    BOOL        bFileRepaired;
    FILEOBJECT_T stFileObject;
    FILEINFO_T  stFileInfo;
    HLENG       hLEng = NULL;
    PAMLHANDLE  hLPAM = NULL;

    if (hVirus == NULL || hVirus->lpVContext == NULL)
        return(VS_INVALID_ARG);

    hContext = hVirus->lpVContext;

    SYM_ASSERT(VLFileWrite);
    SYM_ASSERT(VLFileGetDateTime);
    SYM_ASSERT(VLFileSetDateTime);
    SYM_ASSERT(VLFileGetAttr);
    SYM_ASSERT(VLFileSetAttr);

    //////////////////////////////////////////////////////////////////////////
    // make sure we have a valid global context
    //////////////////////////////////////////////////////////////////////////

    lpgCallBack = hContext->lpstCallBack;

    ///////////////////////////////////////////////////////////////////////////
    // Is the specified HVLVIRUS handle not pointing to a file OR have we already
    // tried to repair the file unsuccessfully?
    ///////////////////////////////////////////////////////////////////////////

    if ((hVirus->dwFlags & VIT_FILE) == 0 ||
        (hVirus->dwFlags & VIT_UNREPAIRABLE))
        return(VS_CANT_REPAIR);

    ///////////////////////////////////////////////////////////////////////////
    // Initialize local avenge engine
    ///////////////////////////////////////////////////////////////////////////

    if (!VLEngLocalInit(hContext, &hLEng))
    {
        return(VS_NO_ACCESS);
    }

    ///////////////////////////////////////////////////////////////////////////
    // Now go into a repair loop until the repairs are done...
    ///////////////////////////////////////////////////////////////////////////

    uRepairTries = MAX_FILE_REPAIR_TRIES;

    hCurVirus = hVirus;

    do
    {
        uResult = FALSE;

        ///////////////////////////////////////////////////////////////////////
        // first attempt an external repair and then go try normal repair if it
        // fails...
        ///////////////////////////////////////////////////////////////////////

        extResult = EXTRepairFile(hContext,
                                  EXTERNAL_DETECT_VERSION,
                                  &hCurVirus->stVirusInfoBlock,
                                  HV_FileInfo_szFileName(hVirus),
                                  (LPBYTE)&hLEng->stAlgScanData);

        if (EXTSTATUS_OK == extResult)
        {
            uResult = TRUE;
        }
        else if (EXTSTATUS_FILE_ERROR == extResult)
        {
            uResult = FALSE;
        }

    //////////////////////////////////////////////////////////////////////////
    // PAM (No virus detected with NAV traditional scanner. Try PAM)
    //////////////////////////////////////////////////////////////////////////

        else if ( ((hCurVirus->stVirusInfoBlock).wControl2 & N30_CTRL2_BIT_FLAG2G) &&
                  (NULL != hContext->hGPAM) )
        {
            PAMSTATUS   pamStatus = PAMSTATUS_OK;

            if (FileGetAttr(HV_FileInfo_szFileName(hVirus),&uAttr) ||
                FileSetAttr(HV_FileInfo_szFileName(hVirus),0))
            {
                VLEngLocalClose(hLEng);
                hVirus->dwFlags |= VIT_NO_ACCESS;
                return(VS_FILE_ERROR);
            }

#if defined(AVAPI_MT)
            pamStatus = PAMLocalInit(HVC_szPAMScanDataFile(hContext),
                                     NULL,                  // no swap file!
                                     (PAMGHANDLE)hContext->hGPAM,
                                     &hLPAM);
#else
            hLPAM = hContext->hLPAM;
#endif
            if (pamStatus == PAMSTATUS_OK)
            {
                // successful init, now perform repair!

                BOOL        bResult;
                TCHAR       szBackup[SYM_MAX_PATH];

                bResult = BackupInfectedFile (HV_FileInfo_szFileName(hVirus),
                                              _T("PAM"),
                                              szBackup);

                if (FALSE == bResult)
                {
#if defined(AVAPI_MT)
                    PAMLocalClose(hLPAM);
#endif   
                    VLEngLocalClose(hLEng);
                    FileSetAttr(HV_FileInfo_szFileName(hVirus),uAttr);
                    uResult = FALSE;
                }
                else
                {
                    // Successfully created the backup file,
                    // so call PAM's repair

                    pamStatus = PAMRepairFile(
                                    hLPAM,
                                    HVC_szPAMScanDataFile(hContext),
                                    szBackup,                       // temp swap file
                                    HV_FileInfo_szFileName(hVirus), // infected program
                                    (hCurVirus->stVirusInfoBlock).wBookMark, // which repair #?
                                    FALSE);                         // no SYS override

                    // PAMSTATUS_NO_REPAIR or PAMSTATUS_*_ERROR

                    if (PAMSTATUS_OK != pamStatus)
                    {
                        uResult  = FALSE;
                    }
                    else
                    {
                        uResult = TRUE;
                    }

#if defined(AVAPI_MT)
                    PAMLocalClose(hLPAM);
#endif

                    FileKill(szBackup);

                    // reset the attributes when we're done and kill our temp file!

                    if (FileSetAttr(HV_FileInfo_szFileName(hVirus),uAttr))
                    {
                        VLEngLocalClose(hLEng);
                        hVirus->dwFlags |= VIT_NO_ACCESS;
                        return(VS_FILE_ERROR);
                    }
                }
            }
        }

    //////////////////////////////////////////////////////////////////////////
    // Try AVENGE if repair has failed with navex and PAM
    //////////////////////////////////////////////////////////////////////////

        else // if (EXTSTATUS_NO_REPAIR == extResult)
        {

    //////////////////////////////////////////////////////////////////////////
    // create a file object for AVENGE and open the file
    //////////////////////////////////////////////////////////////////////////

            STRCPY(stFileInfo.lpstrPathName,HV_FileInfo_szFileName(hVirus));
            stFileObject.lpParent = NULL;
            stFileObject.lpvInfo = &stFileInfo;
            stFileObject.lpCallBacks = &gstNAVFileCallBacks;
            stFileObject.lpszName = HV_FileInfo_szFileName(hVirus);

            if (FileGetAttr(HV_FileInfo_szFileName(hVirus),&uAttr) ||
                FileSetAttr(HV_FileInfo_szFileName(hVirus),0))
            {
                hVirus->dwFlags |= VIT_NO_ACCESS;
                return(VS_FILE_ERROR);
            }

            if (EngRepairFile(hLEng,
                              hVirus->uDBEntryNum,
                              &stFileObject,
                              &bFileRepaired) == ENGSTATUS_ERROR)
            {

                uResult = FALSE;
            }
            else
            {
                uResult = TRUE;
            }

            if (FileSetAttr(HV_FileInfo_szFileName(hVirus),uAttr))
            {
                VLEngLocalClose(hLEng);
                hVirus->dwFlags |= VIT_NO_ACCESS;
                return(VS_FILE_ERROR);
            }
        }

        ///////////////////////////////////////////////////////////////////////
        // Check to see if the repair succeeded
        ///////////////////////////////////////////////////////////////////////

        if (uResult == TRUE)            // succesful repair so far...
        {
            vsScanResult = VLScanFile(hContext,
                                      HV_FileInfo_szFileName(hVirus),
                                      &hTempVirus);

            if (vsScanResult == VS_OK )
            {
                if ( hTempVirus == NULL )
                {
                    hVirus->dwFlags |= VIT_REPAIRED;
                    hVirus->dwFlags &= ~((DWORD)VIT_HAS_A_VIRUS);
                    VLEngLocalClose(hLEng);
                    return(VS_OK);
                }
                else
                {
                    hCurVirus->hNext = hTempVirus;
                    hCurVirus        = hCurVirus->hNext;
                }
            }
            else if ( vsScanResult == VS_NO_ACCESS)
            {
                if (hTempVirus)
                    VLReleaseVirusHandle(hTempVirus);
                hVirus->dwFlags |=  ( VIT_NO_ACCESS | VIT_UNREPAIRABLE );
                VLEngLocalClose(hLEng);
                return (VS_NO_ACCESS);
            }
            else
            {
                hVirus->dwFlags |=  VIT_UNREPAIRABLE;
                VLEngLocalClose(hLEng);
                return(vsScanResult);
            }
        }
        else
        {
            hVirus->dwFlags |= VIT_UNREPAIRABLE;
            VLEngLocalClose(hLEng);
            return(VS_CANT_REPAIR);
        }

        uRepairTries --;

    } while (uRepairTries > 0);


    hVirus->dwFlags |= VIT_UNREPAIRABLE;
    VLEngLocalClose(hLEng);
    return(VS_CANT_REPAIR);

}

/*@API:*************************************************************************
@Declaration: VSTATUS WINAPI VLRepairBootPart(HVLVIRUS hVirus)

@Description:
This routine can be used to repair boot sectors and MBRs infected with
viruses.  It will automatically determine what type of repair is necessary
and apply it (multiple times, if necessary).  The HVLVIRUS handle will be updated to reflect
the repair status of the item.

@Parameters:
$hVirus$ This is a valid HVLVIRUS handle.

@Returns:
This function returns a result of type VSTATUS (type definition found in
AVAPI.H).

    VS_OK           will be returned on success
    VS_INVALID_ARG  will be returned if the context is invalid
    VS_CANT_REPAIR  will be returned if the item cannot be repaired
    VS_MEMALLOC     will be returned if memory cannot be allocated
    VS_DRIVE_ERROR  will be returned on a drive access error
    VS_CRIT_ERROR   will be returned on critical error

@Include: avapi.h
*******************************************************************************/

VSTATUS SYM_EXPORT WINAPI VLRepairBootPart
(
    HVLVIRUS hVirus                       // [in] a valid HVLVIRUS handle
)
{
    UINT        uRepairTries;
    UINT        uResult;
    VSTATUS     vsResult;
    HVCONTEXT   hContext;
    HVLVIRUS    hTempVirus, hCurrent;
    EXTSTATUS   extResult;
    BOOL        bBootRepaired;
    HLENG       hLEng = NULL;
    LINEAROBJECT rLinear;

    if (hVirus == NULL || hVirus->lpVContext == NULL)
        return(VS_INVALID_ARG);

    hContext = hVirus->lpVContext;

    rLinear.lpCallBacks = &gstNAVLinearCallBacks;
    rLinear.lpvInfo     = (LPVOID)hVirus->BootInfo.byDriveNum;

    //////////////////////////////////////////////////////////////////////////
    // make sure we have a valid global context
    //////////////////////////////////////////////////////////////////////////

    lpgCallBack = hContext->lpstCallBack;

    ///////////////////////////////////////////////////////////////////////////
    // Is the specified HVLVIRUS handle not pointing to a file OR have we already
    // tried to repair the file unsuccessfully?
    ///////////////////////////////////////////////////////////////////////////

    if ((hVirus->dwFlags & VIT_BOOTPART) == 0 ||
        (hVirus->dwFlags & VIT_UNREPAIRABLE))
        return(VS_CANT_REPAIR);
    
    if (!VLEngLocalInit(hContext, &hLEng))
    {
        return(VS_NO_ACCESS);
    }

    uRepairTries = MAX_BOOT_REPAIR_TRIES;
    hCurrent = hVirus;
    do
    {
        if (hCurrent->BootInfo.byBootFlags & BI_BOOT_SECTOR)
        {
            WORD wCylinder = 0;
            BYTE bySector = 1, byHead = 0, bySystemType;

            if (hVirus->BootInfo.byDriveNum >= 0x80)
            {
                // read in the MBR so we can determine where the BS resides

                uResult = VLSDiskReadPhysical((LPBYTE)&hLEng->stAlgScanData,
                                              1,
                                              0,
                                              1,
                                              0,
                                              hVirus->BootInfo.byDriveNum);

                if (uResult != 0)           // failure
                {
                    wCylinder = 0xFFFF;
                    bySector = byHead = 0xFF;
                }
                else
                {
                    VLGetBootSectorLocation ((LPBYTE)&hLEng->stAlgScanData,
                                             (UINT)hVirus->BootInfo.byBootSecNum,
                                             &wCylinder,
                                             &bySector,
                                             &byHead,
                                             &bySystemType);
                }
            }

            extResult = EXTRepairBoot(hContext,
                                      EXTERNAL_DETECT_VERSION,
                                      &hCurrent->stVirusInfoBlock,
                                      hVirus->BootInfo.byDriveNum,
                                      hVirus->BootInfo.byBootSecNum,
                                      bySector,wCylinder,byHead,
                                      (LPBYTE)&hLEng->stAlgScanData);
        }
        else if (hCurrent->BootInfo.byBootFlags & BI_PARTITION_SECTOR)
            extResult = EXTRepairPart(hContext,
                                      EXTERNAL_DETECT_VERSION,
                                      &hCurrent->stVirusInfoBlock,
                                      hVirus->BootInfo.byDriveNum,
                                      (LPBYTE)&hLEng->stAlgScanData);
        else
            extResult = EXTSTATUS_NO_REPAIR;

        if (EXTSTATUS_OK == extResult)
        {
            uResult = TRUE;
        }
        else //if (EXTSTATUS_NO_REPAIR == extResult)
        {

            if (EngRepairBoot (hLEng,
                               hVirus->uDBEntryNum,
                               &rLinear,
                               hVirus->BootInfo.byBootSecNum,
                               &bBootRepaired) == ENGSTATUS_ERROR)
            {

                uResult = FALSE;
            }
            else
            {
                uResult = bBootRepaired;
            }

        }

        ///////////////////////////////////////////////////////////////////////
        // check to see if the repair failed?!?
        ///////////////////////////////////////////////////////////////////////

        if (uResult == FALSE)
        {
            hVirus->dwFlags |= VIT_UNREPAIRABLE;
            VLEngLocalClose(hLEng);
            return(VS_CANT_REPAIR);
        }

        if (hVirus->BootInfo.byBootFlags & BI_BOOT_SECTOR)

            vsResult =   VLScanBoot(hContext,
                                    hVirus->BootInfo.byDriveNum,
                                    hVirus->BootInfo.byBootSecNum,
                                    NULL,
                                    &hTempVirus);
        else
            vsResult = VLScanPart(hContext,
                                  hVirus->BootInfo.byDriveNum,
                                  NULL,
                                  &hTempVirus);
        if (vsResult == VS_OK)
        {
            if ( hTempVirus == NULL)
            {
                hVirus->dwFlags |= VIT_REPAIRED;
                hVirus->dwFlags &= ~((DWORD)VIT_HAS_A_VIRUS);
                VLEngLocalClose(hLEng);
                return(VS_OK);
            }
            else
            {
                hCurrent->hNext = hTempVirus;
                hCurrent        = hCurrent->hNext;
            }
        }
        else if (vsResult == VS_NO_ACCESS)
        {
            if (hTempVirus)
            {
                VLReleaseVirusHandle(hTempVirus);
            }
            hVirus->dwFlags |=  ( VIT_NO_ACCESS | VIT_UNREPAIRABLE );
            VLEngLocalClose(hLEng);
            return(VS_NO_ACCESS);
        }
        else
        {
            hVirus->dwFlags |= VIT_UNREPAIRABLE;
            VLEngLocalClose(hLEng);
            return(vsResult);
        }

        uRepairTries--;
    } while (uRepairTries > 0);

    hVirus->dwFlags |= VIT_UNREPAIRABLE;

    VLEngLocalClose(hLEng);
    return(VS_CANT_REPAIR);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//
// Virus information functions...
//
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/*@API:*************************************************************************
@Declaration: BOOL WINAPI VLVirusInfo(HVLVIRUS hVirus, VLVIRINFO enumVirInfo,
LPTSTR lpszResultBuffer)

@Description:
This routine can be used to obtain virus information about the specified
HVLVIRUS handle.  This routine will return TRUE/FALSE for all boolean information
requests, and fill the lpszResultBuffer for all other requests.

@Parameters:
$hVirus$ This is a valid HVLVIRUS handle.

$enumVirInfo$ This is a value of type VLVIRINFO. (See AVAPI.H) for possible
arguments.  Exaples:

        VI_VIRUS_NAME,                  // obtain virus's name
        VI_VIRUS_ALIAS,                 // obtain virus's alias list
        VI_VIRUS_INFO,                  // obtain virus information
        VI_VIRUS_SIZE,                  // obtain virus's size

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
        VI_BOOL_ISMACVIRUS,             // macinsux virus.
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
        VI_PARTITION_TYPE,              // get partition type of scanned item
        VI_DRIVE_TYPE,                  // get drive type of scanned item

$lpszResultBuffer$ This is a far pointer to a buffer where numeric and textual
information results are placed.  All non BOOLEAN numerical/textual results are
converted to strings and placed in this buffer.

@Returns:

    This function returns:

        TRUE            if the query is a boolean query and the result is TRUE
                        OR if the query is a non-boolean query and the
                        information has been successfully retrieved.

        FALSE           if the query is a boolean query and the result is FALSE
                        OR if the  query is a non-boolean query and the
                        information has not been retrieved.

Note:
                        The MBCS version of this function will return FALSE
                        if STRLEN(virusname) > SIZE_VIRUSNAME (xref:
                        VI_VIRUS_NAME).  The WCHAR version of this function
                        will always return TRUE, but if STRLEN(virusname) >
                        SIZE_VIRUSNAME, only the first SIZE_VIRUSNAME - 1
                        characters will be copied to lpszResultBuffer.
@Include: avapi.h
*******************************************************************************/

BOOL SYM_EXPORT WINAPI VLVirusInfo
(
    HVLVIRUS        hVirus,               // [in] a valid HVLVIRUS handle
    WORD          wDepth,               // [in] level of infection
    VLVIRINFO     enumVirInfo,          // [in] which information to retrieve
    LPTSTR        lpszResultBuffer      // [out] place results in this buffer
)
{
    HVCONTEXT   hContext;

    if (wDepth > 0)
    {
        if (hVirus->hNext == NULL)
            return(FALSE);

        return(VLVirusInfo(hVirus->hNext,wDepth-1,enumVirInfo,lpszResultBuffer));
    }

    if (hVirus == NULL)
        return(FALSE);

    hContext = hVirus->lpVContext;

    //////////////////////////////////////////////////////////////////////////
    // make sure we have a valid global context
    //////////////////////////////////////////////////////////////////////////

    lpgCallBack = hContext->lpstCallBack;

    switch (enumVirInfo)
    {
        case VI_INFECTION_DEPTH:
            SYM_ASSERT(lpszResultBuffer);
            wDepth = VLGetInfectionDepth(hVirus);
            _ConvertWordToString(wDepth,lpszResultBuffer, 10, 0);
            return(TRUE);

        case VI_VIRUS_NAME:
            SYM_ASSERT(lpszResultBuffer);
                               // this flag would have been set only if
                               // VLoadN30 failed to load virus info.
            if (hVirus->dwFlags & VIT_UNKOWN_VIRUS )
               return ( FALSE );

                               // check for valid virus name
#if defined(UNICODE)
            OemToCharBuffW ( hVirus->stVirusInfoBlock.lpVName,
                             lpszResultBuffer,
                             SIZE_VIRUSNAME );
#else
            if (STRLEN ( hVirus->stVirusInfoBlock.lpVName)  > N30_SIZE_VIRUSNAME )
               return ( FALSE );

            STRCPY(lpszResultBuffer,hVirus->stVirusInfoBlock.lpVName);
#endif

            return(TRUE);
#if 0
        case VI_VIRUS_ALIAS:
            SYM_ASSERT(lpszResultBuffer);
            return(VLGetVirusAlias(hVirus->lpVContext,
                                   hVirus->stVirusInfoBlock.wVirID,
                                   lpszResultBuffer));
        case VI_VIRUS_INFO:
            SYM_ASSERT(lpszResultBuffer);
            return(VLGetVirusText(hVirus->lpVContext,
                                  hVirus->stVirusInfoBlock.wVirID,
                                  lpszResultBuffer));

#endif
        case VI_VIRUS_SIZE:
            SYM_ASSERT(lpszResultBuffer);
            if (hVirus->stVirusInfoBlock.wType == BOOtype)
                _ConvertWordToString(SECTOR_SIZE,lpszResultBuffer, 10, 0);
            else
                _ConvertWordToString(hVirus->stVirusInfoBlock.wInfSize,
                                    lpszResultBuffer, 10, 0);
            return(TRUE);

        case VI_BOOL_INFECT_EXE:
            if (hVirus->stVirusInfoBlock.wInfo & bitINFO_EXEFILE)
                return(TRUE);
            else
                return(FALSE);

        case VI_BOOL_INFECT_FILES:
            if ((hVirus->stVirusInfoBlock.wInfo &
                 (bitINFO_EXEFILE | bitINFO_COMFILE | bitINFO_COMCOM | bitINFO_SYSFILE)) ||
                hVirus->stVirusInfoBlock.wType != BOOtype)
                return(TRUE);
            else
                return(FALSE);

        case VI_BOOL_INFECT_COM:
            if (hVirus->stVirusInfoBlock.wInfo & bitINFO_COMFILE)
                return(TRUE);
            else
                return(FALSE);

        case VI_BOOL_INFECT_COMCOM:
            if (hVirus->stVirusInfoBlock.wInfo & bitINFO_COMCOM)
                return(TRUE);
            else
                return(FALSE);

        case VI_BOOL_INFECT_SYS:
            if (hVirus->stVirusInfoBlock.wInfo & bitINFO_SYSFILE)
                return(TRUE);
            else
                return(FALSE);

        case VI_BOOL_INFECT_MBR:
            if (hVirus->stVirusInfoBlock.wInfo & bitINFO_MBOOTREC)
                return(TRUE);
            else
                return(FALSE);

        case VI_BOOL_INFECT_FLOPPY_BOOT:
            if (hVirus->stVirusInfoBlock.wInfo & bitINFO_FLOPBOOT)
                return(TRUE);
            else
                return(FALSE);

        case VI_BOOL_INFECT_HD_BOOT:
            if (hVirus->stVirusInfoBlock.wInfo & bitINFO_HDISKBOOT)
                return(TRUE);
            else
                return(FALSE);

        case VI_BOOL_INFECT_WILD:
            if (hVirus->stVirusInfoBlock.wInfo & bitINFO_INTHEWILD)
                return(TRUE);
            else
                return(FALSE);

        case VI_BOOL_MEMORY_RES:
            if (hVirus->stVirusInfoBlock.wInfo & bitINFO_RESIDENT)
                return(TRUE);
            else
                return(FALSE);

        case VI_BOOL_SIZE_STEALTH:
            if (hVirus->stVirusInfoBlock.wInfo & bitINFO_SIZESTLTH)
                return(TRUE);
            else
                return(FALSE);

        case VI_BOOL_FULL_STEALTH:
            if (hVirus->stVirusInfoBlock.wInfo & bitINFO_FULLSTLTH)
                return(TRUE);
            else
                return(FALSE);

        case VI_BOOL_TRIGGER:
            if (hVirus->stVirusInfoBlock.wInfo & bitINFO_TRIGGERED)
                return(TRUE);
            else
                return(FALSE);

        case VI_BOOL_ENCRYPTING:
            if (hVirus->stVirusInfoBlock.wInfo & bitINFO_ENCRYPT)
                return(TRUE);
            else
                return(FALSE);

        case VI_BOOL_POLYMORPHIC:
            if (hVirus->stVirusInfoBlock.wInfo & bitINFO_POLYMORPH)
                return(TRUE);
            else
                return(FALSE);

        case VI_BOOL_MACRO_VIRUS:            // macro virus
            if ( bitINFO_MACRO ==
                (hVirus->stVirusInfoBlock.wControl2 & bitINFO_MASKEXT))
                return(TRUE);
            else
                return(FALSE);

        case VI_BOOL_WINDOWS_VIRUS:          // windows virus
            if (bitINFO_WINDOWS ==
               (hVirus->stVirusInfoBlock.wControl2 & bitINFO_MASKEXT))
                return(TRUE);
            else
                return(FALSE);

        case VI_BOOL_AGENT_VIRUS:            // agent virus
            if (bitINFO_AGENT ==
               (hVirus->stVirusInfoBlock.wControl2 & bitINFO_MASKEXT))
                return(TRUE);
            else
                return(FALSE);

        case VI_BOOL_MULTIPART:
            if (hVirus->stVirusInfoBlock.wInfo & bitINFO_MULTIPARTY)
                return(TRUE);
            else
                return(FALSE);

        case VI_BOOL_MAC_VIRUS:
            if (hVirus->stVirusInfoBlock.wType == MACtype)
                return(TRUE);
            else
                return(FALSE);

        case VI_BOOL_REPAIRED:
            if (hVirus->dwFlags & VIT_REPAIRED)
                return(TRUE);
            else
                return(FALSE);

        case VI_BOOL_DELETABLE:
            return(VLCanDeleteItem(hVirus));

        case VI_BOOL_REPAIRABLE:
            return(VLCanRepairItem(hVirus));

        case VI_BOOL_NO_ACCESS:
            if (hVirus->dwFlags & VIT_NO_ACCESS)
                return(TRUE);
            else
                return(FALSE);

        case VI_BOOL_DISEMBODIED:
            if (hVirus->dwFlags & VIT_DISEMBODIED)
                return(TRUE);
            else
                return(FALSE);

        case VI_BOOL_DELETED:
            if (hVirus->dwFlags & VIT_DELETED)
                return(TRUE);
            else
                return(FALSE);

        case VI_BOOL_INFO_VALID:
            if (hVirus->dwFlags & VIT_VALID_N30)
                return(TRUE);
            else
                return(FALSE);

        case VI_BOOL_MEMORY_DETECT:
            if (hVirus->dwFlags & VIT_FOUND_IN_MEMORY)
                return(TRUE);
            else
                return(FALSE);

        case VI_BOOL_FILE_ITEM:
            if (hVirus->dwFlags & VIT_FILE)
                return(TRUE);
            else
                return(FALSE);

        case VI_BOOL_BOOT_ITEM:
            if ((hVirus->dwFlags & VIT_BOOTPART) &&
                (hVirus->BootInfo.byBootFlags & BI_BOOT_SECTOR))
                return(TRUE);
            else
                return(FALSE);

        case VI_BOOL_PART_ITEM:
            if ((hVirus->dwFlags & VIT_BOOTPART) &&
                (hVirus->BootInfo.byBootFlags & BI_PARTITION_SECTOR))
                return(TRUE);
            else
                return(FALSE);

        case VI_FILENAME:
            SYM_ASSERT(lpszResultBuffer);
            if (hVirus->dwFlags & VIT_FILE)
            {
#if defined(SYM_VXD)
                                        // Not implemented for VxD
                SYM_ASSERT(FALSE);
                return(FALSE);
#else
                STRCPY(lpszResultBuffer,hVirus->FileInfo.szFileName);
                return(TRUE);
#endif
            }
            else
                return(FALSE);

        case VI_ITEM_NAME:
            SYM_ASSERT(lpszResultBuffer);
            if (hVirus->dwFlags & VIT_FILE)
            {
#if defined(SYM_VXD)
                                        // Not implemented for VxD
                SYM_ASSERT(FALSE);
                return(FALSE);
#else
                STRCPY(lpszResultBuffer,hVirus->FileInfo.szFileName);
                return(TRUE);
#endif
            }
            else if (hVirus->dwFlags & VIT_FOUND_IN_MEMORY )
            {
                STRCPY(lpszResultBuffer,_T("Memory"));
                return(TRUE);
            }
            else if (hVirus->dwFlags & VIT_BOOTPART )
            {
                TCHAR lpszTemp[20];

                _ConvertWordToString(hVirus->BootInfo.byDriveNum,
                                          lpszTemp, 10, 0);
                STRCAT (lpszTemp, _T(" ") );
                STRCPY(lpszResultBuffer, lpszTemp);

                _ConvertWordToString(hVirus->BootInfo.byBootSecNum,
                                        lpszTemp, 10, 0);   // partition number
                STRCAT (lpszTemp, _T(" ") );
                STRCAT (lpszResultBuffer, lpszTemp );

                _ConvertWordToString(hVirus->BootInfo.bySystemType,
                                        lpszTemp, 10, 0);
                STRCAT (lpszResultBuffer, lpszTemp );

            }
            else
            {
                SYM_ASSERT(0);
                return(FALSE);
            }

        case VI_DETECT_DATE:
            SYM_ASSERT(lpszResultBuffer);
            _ConvertLongToString ( (LONG)hVirus->uDate, lpszResultBuffer,
                                    10, 0 );
            return(TRUE);

        case VI_DETECT_TIME:
            SYM_ASSERT(lpszResultBuffer);
            _ConvertLongToString ( (LONG)hVirus->uTime, lpszResultBuffer,
                                   10, 0 );
            return(TRUE);

        case VI_BOOL_REPAIR_FAILED:
            if (hVirus->dwFlags & VIT_UNREPAIRABLE )
            {
                return(TRUE);
            }
            else
                return(FALSE);

        case VI_BOOL_DELETE_FAILED:
            if (hVirus->dwFlags & VIT_CANNOT_DELETE )
            {
                return(TRUE);
            }
            else
                return(FALSE);

        case VI_PHYSICAL_DRIVE_NUM:
            SYM_ASSERT(lpszResultBuffer);
            if (hVirus->dwFlags & VIT_BOOTPART)
            {
                _ConvertWordToString(hVirus->BootInfo.byDriveNum,
                                        lpszResultBuffer, 10, 0);
                return(TRUE);
            }
            else
                return(FALSE);

        case VI_PARTITION_NUM:
            SYM_ASSERT(lpszResultBuffer);
            if (hVirus->dwFlags & VIT_BOOTPART)
            {
                _ConvertWordToString(hVirus->BootInfo.byBootSecNum,
                                        lpszResultBuffer, 10, 0);
                return(TRUE);
            }
            else
                return(FALSE);

        case VI_PARTITION_TYPE:
            SYM_ASSERT(lpszResultBuffer);
            if (hVirus->dwFlags & VIT_BOOTPART)
            {
                _ConvertWordToString(hVirus->BootInfo.bySystemType,
                                        lpszResultBuffer, 10, 0);
                return(TRUE);
            }
            else
                return(FALSE);
    }

    return ( FALSE );
}
#endif



//********************************************************************
//
//********************************************************************

WORD LOCAL VLGetInfectionDepth (
    HVLVIRUS hVirus )
{
    if (hVirus != NULL)
        return( 1 + VLGetInfectionDepth(hVirus->hNext));
    else
        return(0);
}

#ifndef SYM_NLM
/*@API:*************************************************************************
@Declaration: BOOL WINAPI VLDefInfo(HVLVIRUSENTRY hVirus, VLVIRINFO enumVirInfo,
LPTSTR lpszResultBuffer)

@Description:
This routine can be used to obtain virus information about the specified
HVLVIRUSENTRY handle.  This routine will return TRUE/FALSE for all boolean
information requests, and fill the lpszResultBuffer for all other requests.

@Parameters:
$hVirus$ This is a valid HVLVIRUSENTRY handle.

$enumVirInfo$ This is a value of type VLVIRINFO. (See AVAPI.H) for possible
arguments.  Exaples:

        VI_VIRUS_NAME,                  // obtain virus's name
        VI_VIRUS_ALIAS,                 // obtain virus's alias list
        VI_VIRUS_INFO,                  // obtain virus information
        VI_VIRUS_SIZE,                  // obtain virus's size

        VI_BOOL_INFECT_FILES,           // does this virus infect FILES in general? (MAC too)
        VI_BOOL_INFECT_EXE,             // does this virus infect EXE's?
        VI_BOOL_INFECT_COM,             // does this virus infect COM's?
        VI_BOOL_INFECT_COMCOM,          // does this virus infect COMMAND.COM?
        VI_BOOL_INFECT_SYS,             // does this virus infect  SYS's?
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

        VI_BOOL_DEF_DELETED,            // def been deleted from VIRSCAN.DAT?
        VI_BOOL_INFO_VALID              // is this information valid?


$lpszResultBuffer$ This is a far pointer to a buffer where numeric and textual
information results are placed.  All non BOOLEAN numerical/textual results are
converted to strings and placed in this buffer.

@Returns:

    This function returns:

        TRUE            if the query is a boolean query and the result is TRUE
                        OR if the query is a non-boolean query and the
                        information has been successfully retrieved.

        FALSE           if the query is a boolean query and the result is FALSE
                        OR if the  query is a non-boolean query and the
                        information has not been retrieved.

@Include: avapi.h
*******************************************************************************/
#if 0
BOOL SYM_EXPORT WINAPI VLDefInfo
(
    HVLVIRUSENTRY hVirusEntry,          // [in] a valid HVLVIRUS handle
    VLVIRINFO     enumVirInfo,          // [in] which information to retrieve
    LPTSTR        lpszResultBuffer      // [out] place results in this buffer
)
{
    HVCONTEXT       hContext;
    VIRINFOTYPE     stVirInfo;

    if (hVirusEntry == NULL)
        return(FALSE);

    hContext = hVirusEntry->lpVContext;

    //////////////////////////////////////////////////////////////////////////
    // make sure we have a valid global context
    //////////////////////////////////////////////////////////////////////////

    lpgCallBack = hContext->lpstCallBack;

    stVirInfo.lpVContext            = hContext;
    stVirInfo.uDBEntryNum           = hVirusEntry->uDBEntryNum;
    stVirInfo.stVirusInfoBlock      = hVirusEntry->stVirusInfoBlock;
    stVirInfo.uDate                 = 0;
    stVirInfo.uTime                 = 0;
    stVirInfo.dwFlags               = VIT_VALID_N30;

    switch (enumVirInfo)
    {
        case VI_BOOL_DEF_DELETED:
            if (hVirusEntry->wFlags & VET_DEF_DELETED)
                return(TRUE);
            else
                return(FALSE);

        default:
            return(VLVirusInfo(&stVirInfo,0,enumVirInfo,lpszResultBuffer));
    }

//  return ( FALSE );
}
#endif


/*@API:*************************************************************************
@Declaration: VSTATUS WINAPI VLDeleteEntry(HVLVIRUSENTRY hDeleteEntry)

@Description:
This routine can be used to delete an entry from the VIRSCAN.DAT database.
The HVLVIRUSENTRY still needs to be released via a VLReleaseEntryHandle()

@Parameters:
$hDeleteEntry$ This is a valid HVLVIRUSENTRY handle.

@Returns:

This function returns a result of type VSTATUS (type definition found in
AVAPI.H).

    VS_OK           will be returned on success
    VS_CRIT_ERROR   will be returned if there is an error

@Include: avapi.h
*******************************************************************************/

#if 0
VSTATUS SYM_EXPORT WINAPI VLDeleteEntry (
    HVLVIRUSENTRY hDeleteEntry )
{
    HFILE               hFile;
    HVCONTEXT           hContext;
    int                 nResult;

    hContext = hDeleteEntry->lpVContext;

    SYM_ASSERT(VLFileWrite);
    SYM_ASSERT(VLFileGetDateTime);
    SYM_ASSERT(VLFileSetDateTime);

    //////////////////////////////////////////////////////////////////////////
    // make sure we have a valid global context
    //////////////////////////////////////////////////////////////////////////

    lpgCallBack = hContext->lpstCallBack;

    //////////////////////////////////////////////////////////////////////////
    // open the VIRSCAN.DAT file first...
    //////////////////////////////////////////////////////////////////////////

    hFile = VLFileOpen(HVC_szVirScanDataFile(hContext),READ_WRITE_FILE);
    if (hFile == HFILE_ERROR)
        return(VS_FILE_ERROR);

    //////////////////////////////////////////////////////////////////////////
    // Delete the entry from the database!
    //////////////////////////////////////////////////////////////////////////

    nResult = SCN4_DELETE(hFile,
                          (LPBYTE)&hLEng->stAlgScanData,
                          hDeleteEntry->stVirusInfoBlock.lpVName);
    VLFileClose(hFile);

    if ( nResult != -1 )
    {
        hDeleteEntry->wFlags |= VET_DEF_DELETED;

        return(VS_OK);
    }

    return(VS_CRIT_ERROR);
}
#endif

/*@API:*************************************************************************
@Declaration: VSTATUS WINAPI VLDeleteFile(HVLVIRUS hVirus)

@Description:
This routine can be used to delete (and wipe) a file infected with a virus.
The HVLVIRUS handle will be updated to reflect the deletion status of the file.

@Parameters:
$hVirus$ This is a valid HVLVIRUS handle.

@Returns:

This function returns a result of type VSTATUS (type definition found in
AVAPI.H).

    VS_OK           will be returned on success
    VS_CANT_DELETE  will be returned if the file should/could not be deleted
    VS_FILE_ERROR   will be returned if there is a file error

@Include: avapi.h
*******************************************************************************/

VSTATUS SYM_EXPORT WINAPI VLDeleteFile
(
    HVLVIRUS hVirus                       // [in] a valid HVLVIRUS handle
)
{
    UINT        uResult;
    HVCONTEXT   hContext;

    if (hVirus == NULL)
        return(VS_INVALID_ARG);

    hContext = hVirus->lpVContext;

    SYM_ASSERT(VLFileDelete);
    SYM_ASSERT(VLFileGetAttr);

                                        // ------------------------------------
                                        // make sure we have a valid global
                                        // context
                                        // ------------------------------------

    lpgCallBack = hContext->lpstCallBack;

                                        // ------------------------------------
                                        // Check to see if we can delete the file
                                        // ------------------------------------

    if (!VLCanDeleteItem(hVirus))
        return(VS_CANT_DELETE);

                                        // ------------------------------------
                                        // need to add in all of the wiping
                                        // garbage later
                                        // ------------------------------------

    uResult = FileKill(HV_FileInfo_szFileName(hVirus));

    if ( !uResult )
        {
        hVirus->dwFlags |= VIT_DELETED;
        hVirus->dwFlags &= ~((DWORD)VIT_HAS_A_VIRUS);

        return(VS_OK);
        }
    else
        hVirus->dwFlags |= VIT_CANNOT_DELETE;


    return(VS_FILE_ERROR);
}

#if !defined(SYM_NLM)

/*@API:*************************************************************************
@Declaration: VSTATUS SYM_EXPORT WINAPI VLMoveFile ( HVLVIRUS hVirus,
LPCTSTR pszNewLocation, LPTSTR pszMovedFile );

@Description:
This routine can be used to move (and wipe the original) file infected with a
virus.  The HVLVIRUS handle will be updated to reflect that the original file
has actually been deleted.

@Parameters:
$hVirus$ This is a valid HVLVIRUS handle.
$pszNewLocation$ A string containing the directory where to move the file.
This string should be in OEM in SYM_VXD!
$pszMovedFile$ A buffer (SYM_MAX_PATH characters) where this function will
copy the full path of the target file upon successful completion.  SYM_VXD: in OEM!

@Returns:

This function returns a result of type VSTATUS (type definition found in
AVAPI.H).

    VS_OK           will be returned on success
    VS_CANT_DELETE  will be returned if the file should/could not be deleted after the move
    VS_FILE_ERROR   will be returned if there is a file error

If this function does not complete with VS_OK, the contents of pszMovedFile
will be undefined.

@Include: avapi.h
*******************************************************************************/
VSTATUS SYM_EXPORT WINAPI VLMoveFile (
    HVLVIRUS  hVirus,
    LPCTSTR pszNewLocation,
    LPTSTR  pszMovedFile
    )
    {
    UINT        uResult;
    HVCONTEXT   hContext;
    TCHAR       szNewTarget[SYM_MAX_PATH];
#if defined(SYM_VXD)
    auto char   szTarget[SYM_MAX_PATH*sizeof(WCHAR)];
#endif

    if (hVirus == NULL)
        return(VS_INVALID_ARG);

    hContext = hVirus->lpVContext;

    SYM_ASSERT(VLFileDelete);
    SYM_ASSERT(VLFileGetAttr);

                                        // ------------------------------------
                                        // make sure we have a valid global
                                        // context
                                        // ------------------------------------

    lpgCallBack = hContext->lpstCallBack;

                                        // ------------------------------------
                                        // Check to see if we can delete the file
                                        // ------------------------------------

    if (!VLCanDeleteItem(hVirus))
        return(VS_CANT_DELETE);

                                        // ------------------------------------
                                        // Create a full pathname for the
                                        // target file and make sure it is
                                        // unique.
                                        // ------------------------------------

                                        // Copy the new location into our
                                        // local work buffer and make sure
                                        // the trailing backslash is removed.

    STRCPY ( szNewTarget, pszNewLocation );
    NameAppendFile ( szNewTarget, _T("?") );
    NameStripFile ( szNewTarget );

                                        // Get the name of the original file.

    if ( NameReturnFile ( hVirus->FileInfo.szFileName,
                          pszMovedFile ) != NOERR )
        {
        return ( VS_FILE_ERROR );
        }

                                        // Make sure that when we concatenate
                                        // the path and the name we will not
                                        // overflow our buffers.

    if ( STRLEN ( szNewTarget ) +
         STRLEN ( _T("\\") ) +
         STRLEN ( pszMovedFile ) >= SYM_MAX_PATH )
        {
        return ( VS_FILE_ERROR );
        }

                                        // Create a full specification of the
                                        // target file.

    NameAppendFile ( szNewTarget, pszMovedFile );

                                        // Adjust the filename so that it is
                                        // unique.

    if ( !BuildUniqueFileName ( szNewTarget, NULL, pszMovedFile ) )
        {
        return ( VS_FILE_ERROR );
        }

                                        // ------------------------------------
                                        // Copy the original file to the
                                        // unique file.
                                        // ------------------------------------

#if defined(SYM_VXD)
    VxDGenericStringConvert ( pszMovedFile,
                              CHARSET_OEM,
                              szTarget,
                              CHARSET_CURRENT );

    if ( FileCopy ( HV_FileInfo_szFileName(hVirus), szTarget, NULL ) )
#else
    if ( FileCopy ( hVirus->FileInfo.szFileName, pszMovedFile, NULL ) )
#endif
        {
        return ( VS_FILE_ERROR );
        }

                                        // ------------------------------------
                                        // need to add in all of the wiping
                                        // garbage later
                                        // ------------------------------------

    uResult = FileKill(HV_FileInfo_szFileName(hVirus));

    if ( !uResult )
        {
        hVirus->dwFlags |= VIT_DELETED;
        hVirus->dwFlags &= ~((DWORD)VIT_HAS_A_VIRUS);

        return(VS_OK);
        }
    else
        hVirus->dwFlags |= VIT_CANNOT_DELETE;

#if defined(SYM_VXD)
    FileKill(szTarget);
#else
    FileKill(pszMovedFile);
#endif

    return(VS_FILE_ERROR);

    }

/*@API:*************************************************************************
@Declaration: VSTATUS SYM_EXPORT WINAPI VLRenameFile ( HVLVIRUS hVirus,
LPCTSTR pszNewFileName, LPTSTR pszRenamedFile );

@Description:
This routine can be used to rename a file infected with a virus.  The HVLVIRUS
handle will be updated to reflect that the original file has actually
disappeared (deleted).

@Parameters:
$hVirus$ This is a valid HVLVIRUS handle.
$pszNewFileName$ A string containing the full name of the new file.  This
string should be in OEM in SYM_VXD!
$pszRenamedFile$ A buffer (SYM_MAX_PATH characters) where this function will
copy the full path of the renamed file upon successful completion.  SYM_VXD:
in OEM!

@Returns:

This function returns a result of type VSTATUS (type definition found in
AVAPI.H).

    VS_OK           will be returned on success
    VS_CANT_DELETE  will be returned if the file should/could not be renamed/deleted
    VS_FILE_ERROR   will be returned if there is a file error

If this function does not complete with VS_OK, the contents of pszRenamedFile
will be undefined.

@Include: avapi.h
*******************************************************************************/
VSTATUS SYM_EXPORT WINAPI VLRenameFile (
    HVLVIRUS  hVirus,
    LPCTSTR pszNewFileName,
    LPTSTR  pszRenamedFile
    )
    {
    HVCONTEXT   hContext;
#if defined(SYM_VXD)
    auto char   szTarget[SYM_MAX_PATH*sizeof(WCHAR)];
#endif

    if (hVirus == NULL)
        return(VS_INVALID_ARG);

    hContext = hVirus->lpVContext;

    SYM_ASSERT(VLFileGetAttr);

                                        // ------------------------------------
                                        // make sure we have a valid global
                                        // context
                                        // ------------------------------------

    lpgCallBack = hContext->lpstCallBack;

                                        // ------------------------------------
                                        // Check to see if we can delete the file
                                        // ------------------------------------

    if (!VLCanDeleteItem(hVirus))
        return(VS_CANT_DELETE);

                                        // ------------------------------------
                                        // Create a unique version of the
                                        // target file.
                                        // ------------------------------------

    if ( !BuildUniqueFileName ( pszNewFileName, NULL, pszRenamedFile ) )
        {
        return ( VS_FILE_ERROR );
        }

                                        // ------------------------------------
                                        // Rename the file.
                                        // ------------------------------------

#if defined(SYM_VXD)
    VxDGenericStringConvert ( pszRenamedFile,
                              CHARSET_OEM,
                              szTarget,
                              CHARSET_CURRENT );

    if ( FileRename ( HV_FileInfo_szFileName(hVirus), szTarget ) == NOERR )
#else
    if ( FileRename ( hVirus->FileInfo.szFileName, pszRenamedFile ) == NOERR )
#endif
        {
        hVirus->dwFlags |= VIT_DELETED;
        hVirus->dwFlags &= ~((DWORD)VIT_HAS_A_VIRUS);

        return(VS_OK);
        }
    else
        hVirus->dwFlags |= VIT_CANNOT_DELETE;

    return(VS_FILE_ERROR);
    }

#endif // #if !defined(SYM_NLM)

/*@API:*************************************************************************
@Declaration: VSTATUS WINAPI VLOverrideDIR2RepairProtection (HVLVIRUS hVirus)

@Description:
This routine is used to force VLCanRepairItem() and VLRepairFile() to allow
repair of DIR2 infections.  Use this function only when you are sure that
the repair will not destroy any data.

@Parameters:
$hVirus$ This is a valid HVLVIRUS handle.  If this handle does not point to
a DIR2 virus, the function will still return VS_OK.

@Returns:

This function returns a result of type VSTATUS (type definition found in
AVAPI.H).

    VS_OK           will be returned on success

@Include: avapi.h
*******************************************************************************/


VSTATUS SYM_EXPORT WINAPI VLOverrideDIR2RepairProtection
(
    HVLVIRUS hVirus                       // [in] a valid HVLVIRUS handle
)
    {
                                        // If hVirus does not point to a
                                        // DIR2 virus, return VS_OK

    if (!(hVirus->stVirusInfoBlock.wInfo & bitINFO_DIR2))
        return(VS_OK);

                                        // Set flags to force DIR2 repair

    hVirus->dwPOFlags |= fPO_DIR2_REPAIR;

                                        // Return to caller

    return(VS_OK);
    }


/*@API:*************************************************************************
@Declaration: VSTATUS WINAPI VLOverrideDIR2DeleteProtection (HVLVIRUS hVirus)

@Description:
This routine is used to force VLCanDeleteItem() and VLDeleteFile() to allow
deletion of DIR2 infections.  Use this function only when you are sure that
the deletion will not destroy any data.

@Parameters:
$hVirus$ This is a valid HVLVIRUS handle.  If this handle does not point to
a DIR2 virus, the function will still return VS_OK.

@Returns:

This function returns a result of type VSTATUS (type definition found in
AVAPI.H).

    VS_OK           will be returned on success

@Include: avapi.h
*******************************************************************************/


VSTATUS SYM_EXPORT WINAPI VLOverrideDIR2DeleteProtection
(
    HVLVIRUS hVirus                       // [in] a valid HVLVIRUS handle
)
    {
                                        // If hVirus does not point to a
                                        // DIR2 virus, return VS_OK

    if (!(hVirus->stVirusInfoBlock.wInfo & bitINFO_DIR2))
        return(VS_OK);

                                        // Set flags to force DIR2 repair

    hVirus->dwPOFlags |= fPO_DIR2_DELETE;

                                        // Return to caller

    return(VS_OK);
    }


/*@API:*************************************************************************
@Declaration: UINT WINAPI   VLNumVirusEntries( HVCONTEXT hvContext )

@Description:
This routine can be used to determine how many viruses there are in the
VIRSCAN.DAT database. This is a conceptual number of viruses, and may be
well less than the number of definitions. A virus may have several definitions,
but only be counted as one virus. This number is put in to the header from
the file vircount.dat. THE RESULT OF THIS FUNCTION SHOULD BE USED FOR DISPLAY
PURPOSES ONLY!

@Parameters:
$hContext$ This is a valid handle to a context provided by VLScanInit.

@Returns:
This routine returns the number of virus definitions in the database.

@Include: avapi.h
*******************************************************************************/


UINT SYM_EXPORT WINAPI VLNumVirusEntries
(
    HVCONTEXT hContext                  // [in] a valid HVCONTEXT handle
)
{
    WORD wNumVirusRecords = 0;
 
    EngGetNumVirusRecords (hContext->hGEng, &wNumVirusRecords);

    return ((UINT)wNumVirusRecords);
}
#endif



//***************************************************************************
// VLGetCountOfDefinitionsInFile()
//
// Description: calculates the number of definitions
//
// Parameters:
//
// Return Value:
//
// See Also:
//
//***************************************************************************
// 01/30/1995 JMILLARD Function Created.
//***************************************************************************
UINT SYM_EXPORT WINAPI VLGetCountOfDefinitionsInFile(
    HVCONTEXT          hContext,    // [in] a valid HVCONTEXT handle
    HFILE hFile )                   // [in] a handle for the file
{
    return (VLNumVirusEntries(hContext));
}



/*@API:*************************************************************************
@Declaration: UINT WINAPI   VLTrueNumVirusEntries( HVCONTEXT hvContext )

@Description:
This routine can be used to determine how many definitions there are in the
VIRSCAN.DAT database. This can be used to reliably size tables for
VLFillListTable.

@Parameters:
$hContext$ This is a valid handle to a context provided by VLScanInit.

@Returns:
This routine returns the actual number of virus definitions, empty or otherwise,
in the database.

@Include: avapi.h
*******************************************************************************/

UINT SYM_EXPORT WINAPI VLTrueNumVirusEntries
(
    HVCONTEXT          hContext             // [in] a valid HVCONTEXT handle
)
{
    return (VLNumVirusEntries(hContext));
}

#ifndef SYM_NLM
/*@API:*************************************************************************
@Declaration: HVLVIRUSENTRY WINAPI VLGetVirusEntry(HVCONTEXT hContext,
UINT uEntryNum)

@Description:
This routine obtains a single virus entry from the virus database.  The
specified entry number for NAV 3.0/4.0 is 1 based.  If the entry has been
deleted then NULL will be returned.

@Parameters:
$hContext$ This is a valid handle to a context provided by VLScanInit.

$uEntryNum$ Which entry should be retrieved from the database?

@Returns:
On success, this function returns a HVLVIRUSENTRY handle.  If there is an error,
or the entry has been deleted, then the function returns NULL, and the
specific error condition can be retrieved by calling the VLGetError() function.

@Include: avapi.h
*******************************************************************************/

HVLVIRUSENTRY SYM_EXPORT WINAPI VLGetVirusEntry
(
    HVCONTEXT hContext,                 // [in] a valid HVCONTEXT handle
    UINT uEntryNum                      // [in] which entry num to retrieve?
)
{
    HVLVIRUSENTRY hVirusEntry;

    hVirusEntry = (HVLVIRUSENTRY)VLPermMemoryAlloc(sizeof(VIRUSENTRYTYPE));

    if (hVirusEntry == NULL)
        return(NULL);

    ///////////////////////////////////////////////////////////////////////////
    // make sure we point to our parent context.
    ///////////////////////////////////////////////////////////////////////////

    hVirusEntry->lpVContext     = hContext;
    hVirusEntry->uDBEntryNum    = uEntryNum;
    hVirusEntry->wFlags         = 0;

    if (!VLLoadN30(hContext,uEntryNum,&hVirusEntry->stVirusInfoBlock))
    {
        VLPermMemoryFree(hVirusEntry);

        return(NULL);
    }

    if ( (hVirusEntry->stVirusInfoBlock.wType == ALGtype ||
          hVirusEntry->stVirusInfoBlock.wType == BOOtype ||
          hVirusEntry->stVirusInfoBlock.wType == MACtype ||
          hVirusEntry->stVirusInfoBlock.wType == CRCtype) &&
         (hVirusEntry->stVirusInfoBlock.wControl != DEAD_VIRUS_ENTRY) )
        return(hVirusEntry);

    VLPermMemoryFree(hVirusEntry);

    return(NULL);
}


/*@API:*************************************************************************
@Declaration: VSTATUS WINAPI VLGetDBInfo(HVCONTEXT hContext, VLDBINFO enumInfo,
LPTSTR lpszResultBuffer)

@Description:
This routine provides the caller with specific information about the
VIRSCAN.DAT database.

    DB_TOTAL_VIRUS_COUNT            : How many viruses do we claim we detect?
    DB_MONTH_STAMP                  : Month # of the virus database (1-12)
    DB_DAY_STAMP                    : Day # of the virus database (1-31)
    DB_YEAR_STAMP                   : Year # of the virus database (1900+)


@Parameters:
$hContext$ This is a valid handle to a context provided by VLScanInit.

$enumInfo$ Specifies what information to return to the user.

$lpszResultbuffer$ is a far pointer to a buffer where the result will be
placed.  It will have been converted to a string.

@Returns:

    VS_OK           on success
    VS_FILE_ERROR   on file error
    VS_INVALID_ARG  if an invalid enumInfo value is passed

@Include: avapi.h
*******************************************************************************/
VSTATUS SYM_EXPORT WINAPI VLGetDBInfo
(
    HVCONTEXT   hContext,               // [in] a valid HVCONTEXT handle
    VLDBINFO    enumInfo,               // [in] which info should we return?
    LPTSTR      lpszResultBuffer        // [out] string converted result
)
{
    lpszResultBuffer[0] = EOS;


#if 0

    VIRSCANDATHEADER        stHeader;
    HFILE                   hFile;
    UINT                    uResult;

    ///////////////////////////////////////////////////////////////////////////
    // open the VIRSCAN.DAT file to determine the number of viruses "detected"
    ///////////////////////////////////////////////////////////////////////////

    hFile = VLFileOpen(HVC_szVirScanDataFile(hContext),READ_ONLY_FILE);
    if (hFile == HFILE_ERROR)
        return(VS_FILE_ERROR);

    ///////////////////////////////////////////////////////////////////////////
    // Now read in the VIRSCAN.DAT header.
    ///////////////////////////////////////////////////////////////////////////

    uResult = VLFileRead(hFile,&stHeader,sizeof(stHeader));

    VLFileClose(hFile);

    if (uResult != sizeof(stHeader))
        return(VS_FILE_ERROR);

    switch(enumInfo)
    {
        case DB_TOTAL_VIRUS_COUNT:
            _ConvertWordToString(stHeader.wTotalVir,lpszResultBuffer,10,0);
            break;
        case DB_MONTH_STAMP:
            _ConvertWordToString((UINT)stHeader.byMonth,lpszResultBuffer,10,0);
            break;
        case DB_DAY_STAMP:
            _ConvertWordToString((UINT)stHeader.byDay,lpszResultBuffer,10,0);
            break;
        case DB_YEAR_STAMP:
            _ConvertWordToString((UINT)stHeader.wYear,lpszResultBuffer,10,0);
            break;
        default:
            return(VS_INVALID_ARG);
    }
#endif

    return(VS_OK);
}

/*@API:*************************************************************************
@Declaration: UINT WINAPI VLFillListTable(HVCONTEXT hContext,
HVIRUSENTRYRABLE lpFillTable)


@Description:
This function fills a (previously allocated) array of HVIRUSENTRY pointers with
valid pointers to HVIRUSENTRies.

@Parameters:
$hContext$ This is a valid handle to a context provided by VLScanInit.

$lpFillTable$ This is a table whose size is greater than or equal to

                    (sizeof(FAR *) * VLTrueNumVirusEntries(hContext))

@Returns:

    The number of table entries actually filled in.  These pointers which are
placed in the table must be explicitly freed by the user.

@Include: avapi.h
*******************************************************************************/
#if 0
UINT SYM_EXPORT WINAPI VLFillListTable
(
    HVCONTEXT          hContext,                 // [in] a valid HVCONTEXT handle
    HVIRUSENTRYTABLE  lpFillTable            // [out] prealloc'd array of pointers
)
{
    HFILE               hFile;
    UINT                uTotalRecords,
                        uCount,
                        uRecordsToRead,
                        uCurDefNum,
                        uTotalDefsInMemory,
                        uBytesToRead;
    LPN30               lpDefInMemory;
    LPN30               lpDefPtr;


    // make sure there is actually a table

    if (lpFillTable == NULL)
        {
        return(0);
        }

    ///////////////////////////////////////////////////////////////////////////
    // Open the VIRSCAN.DAT database
    ///////////////////////////////////////////////////////////////////////////

    hFile = VLFileOpen(HVC_szVirScanDataFile(hContext), READ_ONLY_FILE);

    if (hFile == HFILE_ERROR )
        return(0);

    uTotalRecords = VLGetCountOfDefinitionsInFile( hContext, hFile );

    ///////////////////////////////////////////////////////////////////////////
    // Allocate some memory for reading in a bunch of N30 entries at a time...
    ///////////////////////////////////////////////////////////////////////////

    lpDefInMemory = ( LPN30 )VLTempMemoryAlloc(CHUNK_N30_ENTRIES * sizeof(N30));

    if (lpDefInMemory == NULL)
    {
        VLFileClose(hFile);

        return(0);
    }

    ///////////////////////////////////////////////////////////////////////////
    // Seek past the header and start reading 100 records at a time...
    ///////////////////////////////////////////////////////////////////////////

    if (VLFileSeek(hFile,sizeof(VIRSCANDATHEADER),SEEK_SET) !=
        sizeof(VIRSCANDATHEADER))
    {
        VLFileClose(hFile);
        VLTempMemoryFree(lpDefInMemory);

        return(0);
    }

    ///////////////////////////////////////////////////////////////////////////
    // Init count of total defs processed (to keep track of entry numbers) and
    // of how many we've loaded into memory.
    ///////////////////////////////////////////////////////////////////////////

    uTotalDefsInMemory = 0;
    uCurDefNum = 1;                     // 1-based start

    while (uTotalRecords)
    {
        uRecordsToRead = (uTotalRecords > CHUNK_N30_ENTRIES) ?
                          CHUNK_N30_ENTRIES : uTotalRecords;

        uBytesToRead = uRecordsToRead * sizeof(N30);

        if (FileRead(hFile, lpDefInMemory, uBytesToRead) != uBytesToRead)
        {
            VLFileClose(hFile);
            VLTempMemoryFree(lpDefInMemory);

            return(uTotalDefsInMemory);
        }

        // process the block

        for (uCount = 0, lpDefPtr = lpDefInMemory;
             uCount < uRecordsToRead;
             uCount++,lpDefPtr++,uCurDefNum++)
        {
            if ( (lpDefPtr->wType == ALGtype ||
                  lpDefPtr->wType == BOOtype ||
                  lpDefPtr->wType == MACtype ||
                  lpDefPtr->wType == CRCtype)
              && (lpDefPtr->wControl != DEAD_VIRUS_ENTRY) )
            {
                lpFillTable[uTotalDefsInMemory] = (HVIRUSENTRY)
                    VLTempMemoryAlloc( sizeof(VIRUSENTRYTYPE) );

                if (lpFillTable[uTotalDefsInMemory] == NULL)
                {
                    VLFileClose(hFile);
                    VLTempMemoryFree(lpDefInMemory);


                    return(uTotalDefsInMemory);
                }

                ///////////////////////////////////////////////////////////////
                // Create a new virus database entry in memory
                ///////////////////////////////////////////////////////////////

                lpFillTable[uTotalDefsInMemory]->lpVContext     = hContext;
                lpFillTable[uTotalDefsInMemory]->wFlags         = 0;
                lpFillTable[uTotalDefsInMemory]->uDBEntryNum    = uCurDefNum;
                lpFillTable[uTotalDefsInMemory]->stVirusInfoBlock = *lpDefPtr;

                uTotalDefsInMemory++;
            }
        }
        uTotalRecords -= uRecordsToRead;
    }

    VLFileClose(hFile);
    VLTempMemoryFree(lpDefInMemory);

    return(uTotalDefsInMemory);
}
#endif
#endif


#ifdef SYM_NLM

/*@API:*************************************************************************
@Declaration: UINT WINAPI VLFillFastCheckDB(
    HVCONTEXT hContext,
    LPFASTVIRUSCHECK lpFastCheckDB)


@Description:
This function fills a previously allocated array of FASTVIRUSCHECK structs
with entries for all definitions, valid or otherwise

@Parameters:
$hContext$ This is a valid handle to a context provided by VLScanInit.

$lpFastCheckDB$ This is a table whose size is greater than or equal to

                    (sizeof(FASTVIRUSCHECK) * VLTrueNumVirusEntries(hContext))

@Returns:

    The number of table entries actually filled in.

@Include: avapi.h
*******************************************************************************/
#if 0
UINT SYM_EXPORT WINAPI VLFillFastCheckDB
(
    HVCONTEXT           hContext,           // [in] a valid HVCONTEXT handle
    int                 nMaxEntries,        // [in] size of prealloc'd array
    LPFASTVIRUSCHECK    lpFastCheckDB       // [out] prealloc'd array
)
{
    HFILE               hFile;
    UINT                uTotalRecords,
                        uCount,
                        uRecordsToRead,
                        uCurDefNum,
                        uTotalDefsInMemory,
                        uBytesToRead;
    LPN30               lpDefInMemory;
    LPN30               lpDefPtr;


    // make sure there is actually a table

    if (lpFastCheckDB == NULL)
        {
        return(0);
        }

    ///////////////////////////////////////////////////////////////////////////
    // Open the VIRSCAN.DAT database
    ///////////////////////////////////////////////////////////////////////////

    hFile = VLFileOpen(hContext->szVirScanDataFile, READ_ONLY_FILE);

    if (hFile == HFILE_ERROR )
        return(0);

    uTotalRecords = VLGetCountOfDefinitionsInFile( hContext, hFile );

    ///////////////////////////////////////////////////////////////////////////
    // Allocate some memory for reading in a bunch of N30 entries at a time...
    ///////////////////////////////////////////////////////////////////////////

    lpDefInMemory = ( LPN30 )VLTempMemoryAlloc(CHUNK_N30_ENTRIES * sizeof(N30));

    if (lpDefInMemory == NULL)
    {
        VLFileClose(hFile);

        return(0);
    }

    ///////////////////////////////////////////////////////////////////////////
    // Seek past the header and start reading 100 records at a time...
    ///////////////////////////////////////////////////////////////////////////

    if (VLFileSeek(hFile,sizeof(VIRSCANDATHEADER),SEEK_SET) !=
        sizeof(VIRSCANDATHEADER))
    {
        VLFileClose(hFile);
        VLTempMemoryFree(lpDefInMemory);

        return(0);
    }

    ///////////////////////////////////////////////////////////////////////////
    // Init count of total defs processed (to keep track of entry numbers) and
    // of how many we've loaded into memory.
    ///////////////////////////////////////////////////////////////////////////

    uTotalDefsInMemory = 0;
    uCurDefNum = 1;                     // 1-based start

    while (uTotalRecords)
    {
        uRecordsToRead = (uTotalRecords > CHUNK_N30_ENTRIES) ?
                          CHUNK_N30_ENTRIES : uTotalRecords;

        uBytesToRead = uRecordsToRead * sizeof(N30);

        if (FileRead(hFile, lpDefInMemory, uBytesToRead) != uBytesToRead)
        {
            VLFileClose(hFile);
            VLTempMemoryFree(lpDefInMemory);

            return(uTotalDefsInMemory);
        }


        for (uCount = 0, lpDefPtr = lpDefInMemory;
             uCount < uRecordsToRead;
             uCount++,lpDefPtr++,uCurDefNum++)
        {

        // check for table overflow!

        if (uTotalDefsInMemory > nMaxEntries)
            {
            return (0);
            }

        ///////////////////////////////////////////////////////////////
        // Create a new fast check entry
        ///////////////////////////////////////////////////////////////

        // apply the same filter as VLFIllListTable

        lpFastCheckDB[uTotalDefsInMemory].wVirID   = lpDefPtr->wVirID;

        if ( (lpDefPtr->wType == ALGtype ||
              lpDefPtr->wType == BOOtype ||
              lpDefPtr->wType == MACtype ||
              lpDefPtr->wType == CRCtype)
              && (lpDefPtr->wControl != DEAD_VIRUS_ENTRY) )
            {

            // decrypt check data, and save it

            lpFastCheckDB[uTotalDefsInMemory].wCheckData =
                ((BYTE) (lpDefPtr->lpSig[1] - 1)) << 8 |
                ((BYTE) (lpDefPtr->lpSig[0] - 1));

            lpFastCheckDB[uTotalDefsInMemory].wControl = lpDefPtr->wControl;

            // save virus name - still encrypted

            MEMCPY( lpFastCheckDB[uTotalDefsInMemory].szVName,
                    lpDefPtr->lpVName,
                    sizeof( lpFastCheckDB[uTotalDefsInMemory].szVName ) );
            }
        else
            {
            lpFastCheckDB[uTotalDefsInMemory].wControl = DEAD_VIRUS_ENTRY;
            lpFastCheckDB[uTotalDefsInMemory].szVName[0] = EOS;
            }

        uTotalDefsInMemory++;

        }
        uTotalRecords -= uRecordsToRead;
    }

    VLFileClose(hFile);
    VLTempMemoryFree(lpDefInMemory);

    return(uTotalDefsInMemory);
}
#endif




/*@API:*************************************************************************
@Declaration: DWORD SYM_EXPORT WINAPI  VLCheckFastDB(
    HVCONTEXT hContext,
    WORD      wFirstWord,
    WORD      wVirusId,
    LPTSTR    szVirusName )

@Description:
This function verifies a virus hit, and returns lpVirusData and the virus name

@Parameters:
$hContext$      This is a valid handle to a context provided by VLScanInit.
$wFirstWord$    First word of virus code - use to double check CRC defs
$wVirusId$      virus id to verify
$szVirusName$   pointer to return virus name

@Returns:

a DWORD containing the wControl value in the HIWORD and the virus id in
the LOWORD

@Include: avapi.h
*******************************************************************************/
#if 0
DWORD SYM_EXPORT WINAPI  VLCheckFastDB
(
    HVCONTEXT hContext,         // [in] a valid HVCONTEXT handle
    WORD      wFirstWord,       // [in] check value for CRC defs
    WORD      wVirusId,         // [in] virus id to check
    LPTSTR    lpszVirusName     // [out] virus name returned
)
{
    WORD    wZeroBaseId;
    WORD    wControl;

    WORD    i;
    LPTSTR  lpVirName;

    TCHAR   cCurrChar,
            cPrevChar;

    LPFASTVIRUSCHECK    lpFastVirusCheck = NULL;


    // validity check incoming virus id

    if (wVirusId == 0)
        {
        goto BailOut;
        }

    // if I have fast look up, do it, otherwise return error

    if (hContext->lpFastCheckDB)
        {

        // make sure it falls in the table

        if (wVirusId > hContext->uTotalDefs)
            {
            wVirusId = 0;
            goto BailOut;
            }

        // I need zero-base for my table lookups

        wZeroBaseId = wVirusId - 1;
        lpFastVirusCheck = &hContext->lpFastCheckDB[wZeroBaseId];

        if (lpFastVirusCheck->wControl == DEAD_VIRUS_ENTRY)
            {
            wVirusId = 0;
            goto BailOut;
            }

        if (wFirstWord != lpFastVirusCheck->wCheckData)
            {
            wVirusId = 0;           // mark no virus here
            goto BailOut;
            }


        // virus here - return control bits and name

        wControl = lpFastVirusCheck->wControl;

        if (lpszVirusName != NULL)
            {
            lpVirName = lpFastVirusCheck->szVName;

            cPrevChar = 0;

            for (i = 0; i < SIZE_VIRUSNAME; i++)
                {
                                        // Copy the current char into the
                                        // output string and save it in
                                        // cCurrChar. Bump the pointer for
                                        // the output but not the input yet
                cCurrChar = *lpVirName;
                *lpszVirusName = cCurrChar;
                lpszVirusName++;

#if !defined (UNICODE)
                                        // If the current char is a lead byte
                                        // copy the trail byte into the output
                                        // Note how the source pointer is bumped
                if (DBCSIsLeadByte (*lpVirName))
                    {
                    lpVirName++;
                    *lpszVirusName = *lpVirName;
                    lpszVirusName++;
                    }
#endif
                lpVirName++;

                                        // If this was a double byte it could
                                        // not have been a blank therefore this
                                        // test is still valid
                if (cCurrChar == _T(' '))
                    {
                    if (cPrevChar == _T(' '))      // double blank is end of name
                        {
                        lpszVirusName[-2] = 0;    // null terminate at last blank
                        break;
                        }
                    }

                cPrevChar = cCurrChar;
                }
            }
        }
    else
        {
        wVirusId = 0;
        }

BailOut:

    return (((DWORD)wControl << 16) + (DWORD)wVirusId);     // merge return codes
}
#endif
/*@API:*************************************************************************
@Declaration: DWORD SYM_EXPORT WINAPI  VLGetNameDB(
    HVCONTEXT hContext,
    WORD      wVirusId,
    LPTSTR    szVirusName )

@Description:
This function returns lpVirusData and the virus name

@Parameters:
$hContext$      This is a valid handle to a context provided by VLScanInit.
$wVirusId$      virus id
$szVirusName$   pointer to return virus name

@Returns:

a DWORD containing the wControl value in the HIWORD and the virus id in
the LOWORD

@Include: avapi.h
*******************************************************************************/
#if 0
DWORD SYM_EXPORT WINAPI  VLGetNameDB
(
    HVCONTEXT hContext,         // [in] a valid HVCONTEXT handle
    WORD      wVirusId,         // [in] virus id
    LPTSTR    szVirusName       // [out] virus name returned
)
{
    WORD    wZeroBaseId;
    WORD    wControl;

    WORD    i;
    LPTSTR  lpVirName;
    TCHAR   byChar, byLastChar;

    LPFASTVIRUSCHECK    lpFastVirusCheck = NULL;


    // validity check incoming virus id

    if (wVirusId == 0)
        {
        goto BailOut;
        }

    // if I have fast look up, do it, otherwise return error

    if (hContext->lpFastCheckDB)
        {

        // make sure it falls in the table

        if (wVirusId > hContext->uTotalDefs)
            {
            wVirusId = 0;
            goto BailOut;
            }

        // I need zero-base for my table lookups

        wZeroBaseId = wVirusId - 1;
        lpFastVirusCheck = &hContext->lpFastCheckDB[wZeroBaseId];

        if (lpFastVirusCheck->wControl == DEAD_VIRUS_ENTRY)
            {
            wVirusId = 0;
            goto BailOut;
            }

        // virus here - return control bits and name

        wControl = lpFastVirusCheck->wControl;

        if (szVirusName != NULL)
            {
            lpVirName = lpFastVirusCheck->szVName;

            byLastChar = 0;

            for (i=0; i<SIZE_VIRUSNAME; i++)
                {
                byChar = *lpVirName++;      // decrypt char ^ 0x47
                *szVirusName++ = byChar;

                if (byChar == _T(' '))
                    {
                    if (byLastChar == _T(' '))  // double blank is end of name
                        {
                        szVirusName[-2] = 0;    // null terminate at last blank
                        break;
                        }
                    }

                byLastChar = byChar;
                }
            }
        }
    else
        {
        wVirusId = 0;
        }

BailOut:

    return (((DWORD)wControl << 16) + (DWORD)wVirusId);     // merge return codes
}

#endif
#endif

/*@API:*************************************************************************
@Declaration: BOOL SYM_EXPORT WINAPI VLValidContext(HVCONTEXT hContext)

@Description:
    Verify that the hContext is valid.

@Parameters:
    $hContext$ This is a handle to a context provided
               by VLScanInit.
@Returns:
    TRUE  if hContext is valid
    FALSE otherwise
@Include: avapi.h
*******************************************************************************/
BOOL SYM_EXPORT WINAPI VLValidContext(HVCONTEXT hContext)
{
    if (hContext == NULL)
        return(FALSE);

    if (hContext->dwSignature != 0xDEADFECEL)
        return(FALSE);

    return(TRUE);
}


/*@API:*************************************************************************
@Declaration: BOOL WINAPI VLIsFileUpToDate(LPTSTR lpPath, LPTSTR FileName, FILETIME FileTime)

@Description:
   determines whether the target file is uptodate.
@Parameters:

LPSTR lpPath     : location of VIRSCAN.DAT, NAV, etc... (SYM_VXD: in OEM!)
LPSTR FileName   : file to check (SYM_VXD: in OEM!)
FILETIME FileTime: check against this these values

@Returns:
    TRUE  if up to date
    FALSE otherwise
@Include: avapi.h
*******************************************************************************/
BOOL WINAPI VLIsFileUpToDate(LPTSTR lpPath, LPTSTR lpFileName, FILETIME FileTime)
{
    HFILE     hFile;
    FILETIME  ft;
    TCHAR     szTargetFile[SYM_MAX_PATH + 1];

    STRCPY ( szTargetFile , lpPath );
    NameAppendFile ( szTargetFile, lpFileName );

#if defined(SYM_VXD)
    auto char szOSTargetFile[SYM_MAX_PATH * sizeof(WCHAR)];

    VxDGenericStringConvert (
        szTargetFile,
        THFLAG_OEM,
        szOSTargetFile,
        ((tcb_s*)Get_Cur_Thread_Handle())->TCB_Flags & THFLAG_CHARSET_MASK );

    if ( (hFile = FileOpen ( szOSTargetFile, READ_ONLY_FILE )) == HFILE_ERROR )
#else
    if ( (hFile = FileOpen ( szTargetFile, READ_ONLY_FILE )) == HFILE_ERROR )
#endif
       return ( FALSE );

    FileGetTime(hFile, TIME_LASTWRITE, &ft);
    FileClose(hFile);

    if ( MEMCMP ( &ft, &FileTime, sizeof(FILETIME) ) )
       return ( FALSE );

    return ( TRUE );

}

/*@API:*************************************************************************
@Declaration: BOOL WINAPI VLVerifyDataFile(LPTSTR lpPath, LPTSTR FileName )

@Description:
Verify that the file exists and has length greater than zero.

@Parameters:
LPSTR lpPath     : location of VIRSCAN.DAT, NAV, etc...
LPSTR FileName   : file to verify

@Returns:
    TRUE  if exists and size greater than zero (0)
    FALSE otherwise
@Include: avapi.h
*******************************************************************************/
BOOL WINAPI VLVerifyDataFile(LPTSTR lpPath, LPTSTR lpFileName )
{
    HFILE  hFile;
    TCHAR  szTargetFile[SYM_MAX_PATH + 1];
    DWORD  dwSize;

    STRCPY ( szTargetFile , lpPath );
    NameAppendFile ( szTargetFile, lpFileName );

#if defined(SYM_VXD)
    auto char szOSTargetFile[SYM_MAX_PATH * sizeof(WCHAR)];

    VxDGenericStringConvert (
        szTargetFile,
        THFLAG_OEM,
        szOSTargetFile,
        ((tcb_s*)Get_Cur_Thread_Handle())->TCB_Flags & THFLAG_CHARSET_MASK );

    if ( (hFile = FileOpen ( szOSTargetFile , READ_ONLY_FILE )) == HFILE_ERROR )
#else
    if ( (hFile = FileOpen ( szTargetFile , READ_ONLY_FILE )) == HFILE_ERROR )
#endif
       return ( FALSE );

    dwSize =  FileLength(hFile);

    FileClose ( hFile );

    return ( dwSize ? TRUE : FALSE );

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//
// AVAPI internal support routines!
//
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#ifndef SYM_NLM
#if 0
BOOL LOCAL VLGetVirusInfo
(
    HVCONTEXT hContext,                 // [in] a valid HVCONTEXT handle
    WORD      wVirusID,                 // [in] 1 based virus entry number
    LPTSTR    lpAlias,                  // [out] alias of virus
    LPTSTR    lpComment                 // [out] comments about virus
)

{
    HFILE       hFile;
    LPSTR       lpMem;
    LONG        dwOffset;
    INF_HEADER  inf;
    LPTSTR      lpData;

    if ((hFile = VLFileOpen(HVC_szVirScanInfoFile(hContext), READ_ONLY_FILE))
                != HFILE_ERROR )
    {

        ///////////////////////////////////////////////////////////////////////////
        // Allocate a chunk of memory big enough to store a comment
        // and an alias string.
        ///////////////////////////////////////////////////////////////////////////

#if defined(UNICODE)
        lpMem = (LPSTR)VLTempMemoryAlloc((SIZE_ALIAS + SIZE_COMMENT + 10) +
                                         (SIZE_ALIAS + SIZE_COMMENT + 10) * sizeof(WCHAR));

        lpData = (LPTSTR)(lpMem + (SIZE_ALIAS + SIZE_COMMENT + 10));
#else
        lpMem = (LPSTR)VLTempMemoryAlloc(SIZE_ALIAS + SIZE_COMMENT + 10);
        lpData = lpMem;
#endif

        if (lpMem == NULL)
        {
            VLFileClose(hFile);

            return(FALSE);
        }

        VLFileSeek (hFile, SIZE_COPYRIGHT, SEEK_SET);
        VLFileRead (hFile, &inf, sizeof(INF_HEADER));

        if (inf._i_version != 0)
        {
            VLTempMemoryFree(lpMem);
            VLFileClose(hFile);

            return(FALSE);
        }

        if (wVirusID > inf._i_highest_vid)
            wVirusID = 0;

        VLFileSeek (hFile, (LONG)wVirusID * (LONG)inf._i_size, SEEK_CUR);
        VLFileRead (hFile, lpMem, inf._i_size);

// &? Do we need to check for read error here?

        if (4 == inf._i_size)
            dwOffset = *(LONG FAR *)lpMem;
        else
            dwOffset = *(WORD FAR *)lpMem;

#if 0
                                        // dwOffset == 0 means to use the
                                        // "No info" record.
        if ( ! dwOffset )
            {
            *lpAlias = *lpComment = EOS;
            return(FALSE);
            }
#endif


        dwOffset = (dwOffset * inf._i_multiplier) + inf._i_no_info;

        VLFileSeek (hFile, dwOffset, SEEK_SET);
        VLFileRead (hFile, lpMem, SIZE_ALIAS + SIZE_COMMENT + 2);

        ///////////////////////////////////////////////////////////////////////
        // First comes the alias field. If its length is longer
        // than the maximum possible alias field then dump it.
        ///////////////////////////////////////////////////////////////////////

#if defined(UNICODE)
        OemToCharW ( (LPSTR)lpMem, lpData );
#endif

        if (STRLEN(lpData) <= SIZE_ALIAS)
        {
            STRCPY(lpAlias, lpData);
#if defined(UNICODE)
            OemToCharW ( &lpMem[strlen(lpMem)+1], lpData );
#else
            lpData = ( lpMem + (STRLEN(lpMem) + 1) );
#endif

            ///////////////////////////////////////////////////////////////////
            // The same goes for the comment field
            ///////////////////////////////////////////////////////////////////

            if ( STRLEN ( lpData ) <= SIZE_COMMENT )
            {
                // Convert '&&' to just one.  This is
                // for Windows compatibility.

                for ( ; *lpData; lpData++)
                {
                                        // ----------------------------------
                                        // Skip double '&&'
                                        // ----------------------------------
#if !defined (UNICODE)
                                        // If it's a DBCS char it can't be "&&"
                    if (DBCSIsLeadByte (*lpData))
                    {
                        *lpComment++ = *lpData++;
                        *lpComment++ = *lpData;
                        continue;
                    }
#endif
                    if ( *lpData == _T('&')
                    &&   *(lpData + 1) == _T('&') )
                        continue;
                                        // ----------------------------------
                                        // Copy this character
                                        // ----------------------------------

                    *lpComment++ = *lpData;
                }
                *lpComment = EOS;
            }
            else
            {
                lpComment[0] = EOS;

                VLTempMemoryFree(lpMem);
                VLFileClose(hFile);

                return(FALSE);
            }
        }
        else
        {
            lpAlias[0] = EOS;
            lpComment[0] = EOS;

            VLTempMemoryFree(lpMem);
            VLFileClose(hFile);

            return(FALSE);
        }

        VLTempMemoryFree (lpMem);
        VLFileClose(hFile);

        return(TRUE);
    }

    return(FALSE);
}
#endif
#endif



///////////////////////////////////////////////////////////////////////////////
// TRUE = success, FALSE = failure
///////////////////////////////////////////////////////////////////////////////

BOOL STATIC VLLoadN30
(
    HVCONTEXT hContext,                 // [in] a valid HVCONTEXT handle
    UINT uIndex,                        // [in] 1-based entry # in VIRSCAN.DAT
    LPN30 lpN30                         // [out] fill this buffer with N30
)
{
    //////////////////////////////////////////////////////////////////////////
    // make sure we have a valid global context
    //////////////////////////////////////////////////////////////////////////

    lpgCallBack = hContext->lpstCallBack;

    //////////////////////////////////////////////////////////////////////////
    // get from cache, if available
    //////////////////////////////////////////////////////////////////////////

    if ( hContext->uN30EFCount &&
         VLLoadCachedN30 ( hContext,
                           uIndex,
                           hContext->uN30EFCount,
                           hContext->lprN30EFBuffer,
                           lpN30 )
       )
        {
        return ( TRUE );
        }

    if ( hContext->uN30EBCount &&
         VLLoadCachedN30 ( hContext,
                           uIndex,
                           hContext->uN30EBCount,
                           hContext->lprN30EBBuffer,
                           lpN30 )
       )
        {
        return ( TRUE );
        }

#if defined(AVAPI_MT)
    HLENG       hLEng = NULL;

    if (EngLocalInit ( hContext->hGEng,
                       &hLEng,
                       0xDEADBEE0UL) == ENGSTATUS_ERROR)
        {
        return ( FALSE );
        }

    if (EngGetN30HVIRUS ( hLEng, 
                          uIndex,
                          lpN30 ) == ENGSTATUS_ERROR )
        {
        return ( FALSE );
        }

    EngLocalClose ( hLEng );
#else
    if (EngGetN30HVIRUS ( hContext->hLEng, 
                          uIndex,
                          lpN30 ) == ENGSTATUS_ERROR )
        {
        return ( FALSE );
        }
#endif

    return(TRUE);
}

///////////////////////////////////////////////////////////////////////////////
// TRUE = success, FALSE = failure
///////////////////////////////////////////////////////////////////////////////

BOOL LOCAL VLLoadCachedN30
(
    HVCONTEXT hContext,                 // [in] a valid HVCONTEXT handle
    UINT      uIndex,                   // [in] 1-based entry # in VIRSCAN.DAT
    UINT      uN30EEntries,             // [in] count of entries in lpN30E
    LPN30E    lpN30E,                   // [in] pointer to a list of N30E's
    LPN30     lpN30                     // [out] fill this buffer with N30
)
{
                                        // Validate input parameters

    SYM_ASSERT ( uN30EEntries );
    SYM_ASSERT ( lpN30E );
    SYM_ASSERT ( lpN30 );

    (void)hContext;

    // Search for uIndex

    while ( uN30EEntries-- )
        {
        if ( lpN30E->uVirusEnumerator == uIndex )
            {
            *lpN30 = lpN30E->rN30;
            return ( TRUE );
            }

        lpN30E++;
        }

    return ( FALSE );
}

///////////////////////////////////////////////////////////////////////////////
// Returns TRUE if O.K., FALSE otherwise
///////////////////////////////////////////////////////////////////////////////

#ifndef SYM_NLM

void LOCAL VLSetHVLVIRUSBoot (
    HVLVIRUS      hVirus,
    HVCONTEXT   lpVContext,
    UINT        uDBEntryNum,
    DWORD       dwFlags,
    BYTE        byDriveNum,
    BYTE        byBootSecNum,
    BYTE        byBootFlags,
    BYTE        bySystemType )
{
    hVirus->lpVContext              = lpVContext;
    hVirus->uDBEntryNum             = uDBEntryNum;
    hVirus->dwFlags                 = dwFlags;
    hVirus->BootInfo.byDriveNum     = byDriveNum;
    hVirus->BootInfo.byBootSecNum   = byBootSecNum;
    hVirus->BootInfo.byBootFlags    = byBootFlags;
    hVirus->BootInfo.bySystemType   = bySystemType;
    hVirus->hNext                   = NULL;
}

#endif

VOID LOCAL VLSetHVLVIRUSFile (
    HVLVIRUS      hVirus,
    HVCONTEXT   lpVContext,
    UINT        uDBEntryNum,
    DWORD       dwFlags,
    LPTSTR      lpszFileName )
{
    hVirus->lpVContext              = lpVContext;
    hVirus->uDBEntryNum             = uDBEntryNum;
    hVirus->dwFlags                 = dwFlags;
    hVirus->hNext                   = NULL;

#if defined(SYM_VXD)
    VxDGenericStringConvert ( lpszFileName, CHARSET_CURRENT,
                              hVirus->FileInfo.szFileName, CHARSET_OEM );

    VxDGenericStringConvert ( lpszFileName, CHARSET_CURRENT,
                              hVirus->FileInfo.szAnsiFileName, CHARSET_ANSI );

    VxDGenericStringConvert ( lpszFileName, CHARSET_CURRENT,
                              hVirus->FileInfo.szUniFileName, CHARSET_UNICODE );
#else
    STRCPY(hVirus->FileInfo.szFileName,lpszFileName);
#endif
}

#ifndef SYM_NLM
#if 0
BOOL LOCAL VLGetVirusText (
    HVCONTEXT   hContext,
    UINT        uDBEntryNum,
    LPTSTR      lpszVirusText )
{
    TCHAR   szAlias[SIZE_ALIAS+1];      // 1 for EOS

    return(VLGetVirusInfo(hContext,
                          uDBEntryNum,
                          szAlias,
                          lpszVirusText));
}

BOOL LOCAL VLGetVirusAlias (
    HVCONTEXT   hContext,
    UINT        uDBEntryNum,
    LPTSTR      lpszVirusAlias )
{
    TCHAR   szText[SIZE_COMMENT];      // EOS included in SIZE_COMMENT

    return(VLGetVirusInfo(hContext,
                          uDBEntryNum,
                          lpszVirusAlias,
                          szText));
}
#endif


///////////////////////////////////////////////////////////////////////////////
// TRUE = success, FALSE = failure
///////////////////////////////////////////////////////////////////////////////

BOOL LOCAL VLEngLocalInit (
    HVCONTEXT hContext, 
    LPHLENG lphLEng )
{

#if defined (AVAPI_MT)

    if (EngLocalInit(hContext->hGEng,
                     lphLEng,
                     0xDEADBEE0UL) != ENGSTATUS_OK)
    {
        return (FALSE);
    }

#else
   
    *lphLEng = hContext->hLEng;

#endif

    return (TRUE);
}

VOID LOCAL VLEngLocalClose (
    HLENG hLEng )
{

#if defined (AVAPI_MT)

    if (hLEng != NULL)
        EngLocalClose(hLEng);

#endif
    
}
#endif




