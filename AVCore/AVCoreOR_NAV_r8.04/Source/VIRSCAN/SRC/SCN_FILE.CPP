// Copyright 1993-1997 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/VIRSCAN/VCS/scn_file.cpv   1.30   09 Apr 1997 12:51:34   DALLEE  $
//
// Description:
//      Contains the routines to single files, or all files in a
//      specified drive/directory, and routines to repair or delete
//      infected files.
//
// Contains:
//      VirScanFile()
//      VirScanRepFile()
//      VirScanDelFile()
//      VirScanDriveDir()
//
// See Also:
//      SCN_INIT.C for ScanInit()
//************************************************************************
// $Log:   S:/VIRSCAN/VCS/scn_file.cpv  $
// 
//    Rev 1.30   09 Apr 1997 12:51:34   DALLEE
// Renamed BackupInfectedFile() to VirusScanBackupFile() and made LOCAL to
// remove name conflicts with version in AVAPI(in NAVFAMLY).
// 
// Also to avoid confusion with yet another BackupInfectedFile() function
// inside the various NAV codebases in their NAVXUTIL projects.
// 
//    Rev 1.29   13 Feb 1997 13:17:48   MKEATIN
// Ported changes from the F branch.
// 
//    Rev 1.28.1.1   16 Jan 1997 17:21:12   JBRENNA
// Change VirusScanFile (PAM logic) to dynamically determine first hard drive
// for the C:\NAVEMUL!.DAT file. On NEC machines this becomes A:\NAVEMUL!.DAT.
// 
//    Rev 1.28.1.0   23 Oct 1996 13:14:06   RJACKSO
// Branch base for version QAKF
// 
//    Rev 1.28   14 Oct 1996 16:35:04   DALLEE
// Minor DBCS work for BackupInfectedFile().
// 
//    Rev 1.27   30 Jul 1996 11:46:28   CNACHEN
// Removed WIN95 ifdefs from code for NAVEMUL creation via registry.
// 
// 
//    Rev 1.26   28 Jun 1996 10:55:00   JBRENNA
// Merge LuigiPlus into Gryphon:
//     CNachen - Added registry key checking for NAVEMUL.
//   Note that this is only compiled with NAV95_CODE defined.
// 
//    Rev 1.25   25 Jun 1996 11:10:32   CNACHEN
// Changed FileDelete to VirusScanDeleteFile after PAM repair...
// 
//    Rev 1.24   21 May 1996 14:36:32   RHAYES
// Merged Luigi OEM<->Ansi code for more robust support of
// Non-Ansi directory and filenames.
// 
//    Rev 1.23   02 May 1996 18:02:28   CNACHEN
// Fixed an oopsie - renamed duplicate PAMGreenLight function in VIRSCAN.
// There was also one in NAVFAMLY.
// 
//    Rev 1.22   30 Apr 1996 17:19:20   CNACHEN
// Added ALG exclusion support for PAM scanning.
// 
//    Rev 1.21   14 Feb 1996 16:07:28   JWORDEN
// Set file APIs to oem for file scanning and repairing. File names
// should now always be passed in OEM mode
// 
//    Rev 1.20   14 Feb 1996 10:02:34   CNACHEN
// Updated call to PAMRepairFile.
// 
// 
//    Rev 1.19   06 Feb 1996 21:30:48   BARRY
// Fix LPSTR to LPBYTE problems due to Unicode effort
// 
//    Rev 1.18   06 Feb 1996 21:24:30   BARRY
// Fix LPSTR to LPBYTE problems due to Unicode effort
// 
//    Rev 1.17   12 Jan 1996 07:11:40   JWORDEN
// Port changes from Quake 8
// 
//    Rev 1.16.1.11   31 Oct 1995 16:05:32   YVANG
// Repeated the same modification done in Rev.1.16.1.10, which was based 
// incorrectly on Rev.1.16.1.8 and did not include all changes made 
// in Rev.1.16.1.9.
// 
// 
//    Rev 1.16.1.10   31 Oct 1995 14:54:02   YVANG
// Replaced the 'VxDIntBasedCall()' with the 'SetFileTime()' function to fix
// the file attributes corruption problem on NTFS network drive.
// 
//    Rev 1.16.1.9   31 Oct 1995 13:05:58   CNACHEN
// Added (IFDEF'd) PAM support for W32
// 
//    Rev 1.16.1.8   30 Oct 1995 19:28:52   YVANG
// No change.
// 
//    Rev 1.16.1.7   26 Oct 1995 16:51:56   CNACHEN
// Made sure to reset file attribute if we're unable to open the file and error
// out of repair...
// 
//    Rev 1.16.1.6   25 Oct 1995 16:39:44   CNACHEN
// Updated VirusRepairFile to change attributes immediately before and after
// calling REP4_ME.  Attributes are not changed before the NAVEX call...
// 
//    Rev 1.16.1.5   17 Oct 1995 17:01:04   CNACHEN
// fixed bug on 100+ failed repairs returning success...
// 
//    Rev 1.16.1.4   13 Oct 1995 14:38:02   DCHI
// Changed avex.h to navex.h.
// 
//    Rev 1.16.1.3   09 Oct 1995 12:07:38   DCHI
// Corrected indexing off of array bounds for virus name.
// 
//    Rev 1.16.1.2   19 Sep 1995 17:42:22   DCHI
// fixed memory overlap bug
// 
//    Rev 1.16.1.1   07 Sep 1995 13:49:24   CNACHEN
// Added external virus detection code.
// 
//    Rev 1.16.1.0   12 Jul 1995 18:59:52   MPYE
// Branch base for version QAK8
// 
//    Rev 1.16   10 Jul 1995 20:47:54   MARKL
// Fixed Defect #39332.  NAVW should no longer change the last access
// date.
// 
//    Rev 1.15   31 Jan 1995 17:39:48   SZIADEH
// SCN4_ME returns UINT, so check for error using UINT.
// 
//    Rev 1.14   27 Dec 1994 19:05:58   DALLEE
// Use CERTLIBN for DOS platform now, too.
// 
//    Rev 1.13   19 Dec 1994 22:05:20   DALLEE
// Convert .WIN to CERTLIBN
// 
//    Rev 1.12   06 Dec 1994 15:34:36   DALLEE
// Put back file scanning for DOS.
// 
//    Rev 1.11   30 Nov 1994 12:33:30   DALLEE
// - Use CTS4.H for SYM_DOS, too.
// - N30NOTES.wControl2 should be assigned N30.wControl2, not wControl.
// 
//    Rev 1.10   29 Nov 1994 15:02:20   MFALLEN
// wControl -> wControl2
// 
//    Rev 1.9   28 Nov 1994 15:29:16   MFALLEN
// include sts4.h if W16 platform.
// 
//    Rev 1.8   29 Jun 1994 17:20:12   MFALLEN
// Temporarely added standard FileDelete().
// 
//    Rev 1.7   27 May 1994 15:24:04   MFALLEN
// Added boot scannign.
// 
//    Rev 1.6   24 May 1994 14:03:12   MFALLEN
// Reintroduced REP4_ME
// 
//    Rev 1.5   17 May 1994 12:54:24   DALLEE
// #include "disk.h" for DiskGet/Set() -- No longer in file.h
// 
//    Rev 1.4   12 May 1994 11:04:02   MFALLEN
// cts4 2 ctsn
// 
//    Rev 1.2   27 Apr 1994 10:56:48   BRAD
// Use SYM_MAX_PATH only
// 
//    Rev 1.1   22 Apr 1994 18:16:10   MFALLEN
// Changed _loadds to SYM_EXPORT. If the TSR will need _loadds then this must
// be a macro. Without this change the LINK32 will not generate an import lib.
// 
//    Rev 1.0   22 Apr 1994 10:58:54   MFALLEN
// Initial revision.
// 
//    Rev 1.39   15 Mar 1994 16:52:54   BRAD
// Cleaned up for SYM_WIN32
// 
//    Rev 1.38   15 Mar 1994 11:09:32   BRUCE
// Changed SYM_EXPORT to SYM_EXPORT
// 
//    Rev 1.37   12 Mar 1994 22:45:54   BRAD
// Cleaned up for SYM_WIN32
// 
//    Rev 1.36   02 Mar 1994 17:12:34   BRAD
// AWK script applied
// 
//    Rev 1.35   11 Feb 1994 15:54:48   MARKL
// Quake 2 changes moved to trunk.
// 
//    Rev 1.34.1.0   19 Oct 1993 08:56:18   SCOTTP
// Branch base for version QAK2
// 
//    Rev 1.34   14 Oct 1993 13:24:32   MFALLEN
// #ifndef SYM_WIN-ed all the FileCheckError since this function can't be called
// from a dll outside SYMKRNL. The function GP's in Windows since the DS is
// from the calling program. How did this work before ???
// 
//    Rev 1.33   12 Oct 1993 16:42:52   PATRICKM
// Baseline from QAK1 branch
// 
//    Rev 1.32.1.3   14 Sep 1993 22:27:30   BRAD
// If drive we don't have access to root, move up from the current location,
// until we get access.
// 
//    Rev 1.32.1.2   12 Sep 1993 01:31:30   MFALLEN
// Removed N30NOTES.bFlag since it's not used.
// 
//    Rev 1.32.1.1   07 Sep 1993 02:06:44   DALLEE
// Placed calls to FileCheckError after CERTLIB4 calls in order to clear
// the critical error handler (DOS) and set the _error variable.
// 
//    Rev 1.32.1.0   03 Sep 1993 22:52:50   SCOTTP
// Branch base for version QAK1
// 
//    Rev 1.32   02 Sep 1993 12:35:44   BARRY
// Fixed usage of MemUnlock for debug builds
// 
//    Rev 1.31   01 Sep 1993 00:24:52   DALLEE
// Set file attr's to NORMAL during repairs so we can open R/W.  Restore after.
// 
//    Rev 1.30   25 Aug 1993 13:06:50   DALLEE
// New parameters for SCN4_ME.  Added bIsExecutable parameter to VirusScanFile().
// 
//    Rev 1.29   23 Aug 1993 01:51:22   BARRY
// Added MODULE_NAME and fixed MemUnlock params for VMM debugging
// 
//    Rev 1.28   18 Aug 1993 15:42:12   MFALLEN
// Ignore internal DOS errors.
// 
//    Rev 1.27   17 Aug 1993 04:16:46   BARRY
// Support new FIL4_FIND_WILD function
// 
//    Rev 1.26   14 Aug 1993 16:17:28   BRAD
// When scanning files, open with OF_SHARE_DENY_NONE.
// 
//    Rev 1.25.1.1   13 Aug 1993 03:10:30   DALLEE
// When scanning files, open with OF_SHARE_DENY_NONE.
// 
//    Rev 1.25.1.0   03 Aug 1993 13:40:18   PATRICKM
// Branch base for version QAK0
// 
//    Rev 1.25   02 Aug 1993 18:40:42   CAREY
// changed call to REP4_ME so as to pass the filename in as the last argument so
// we can correctly repair DIR-2 infections in the DOS scanner.
// 
//    Rev 1.24   28 Jul 1993 14:18:00   MFALLEN
// Bug
// 
//    Rev 1.23   28 Jul 1993 14:15:04   MFALLEN
// If can't open file, return successfully scanned.
// 
//    Rev 1.22   24 Jul 1993 19:01:24   MFALLEN
// Clear all work buffers so memory scanning should not complain about
// virus found in memory.
// 
//    Rev 1.21   21 Jul 1993 14:39:48   MFALLEN
// NULL the virus name buffer before scanning file so it shouldn't contains
// garbage if the scan code doesn't fill in the virus name.
// 
//    Rev 1.20   12 Jul 1993 20:41:30   DALLEE
// Do only REPAIR_ITERATIONS attempts at repairing.
// Use uVirusID from rescan of repaired file for next repair, if still infected.
// Close file opened for repairs before rescanning.
// 
//    Rev 1.19   09 Jul 1993 13:05:22   MFALLEN
// When repairing files, the file is repaired and scanned while the file is
// infected, and the repair is successful.
// 
//    Rev 1.18   18 Jun 1993 00:58:22   DALLEE
// Return the Signiture in Memory Index value and set N30NOTES.wVirID
// to the VIRSCAN.INF lookup index value.
// 
//    Rev 1.17   17 Jun 1993 16:04:24   DALLEE
// [Fix] Moved MEMSET of scanning buffer.  I was clearing the buffer
// before reading the virus information which was returned in the
// work buffer.
// 
//    Rev 1.16   17 Jun 1993 15:27:20   JMILLARD
// move programmed scanner to progscan.h for all platforms
// 
//    Rev 1.15   15 Jun 1993 14:00:16   DALLEE
// Clear the scanning buffer after finding an infected file so we
// do not do a false ID on following mem scans.
// 
//    Rev 1.14   10 Jun 1993 02:51:12   CGUC
// Changed SYM_EXPORT to _loadds
// 
//    Rev 1.13   03 Jun 1993 16:13:50   MFALLEN
// Now closing file handle after file repair.
// 
//    Rev 1.12   19 May 1993 00:48:52   MFALLEN
// Removed callback SCN4_MTE()
// 
//    Rev 1.11   17 May 1993 15:43:24   DALLEE
// Use VIRSCAN.szVirscanFile instead of VIRSCAN.lpScannerBuffer.
// 
//    Rev 1.10   15 May 1993 16:44:50   JWELLS
// 
//    Rev 1.9   15 May 1993 14:28:20   JWELLS
// Callbacks re-enabled
// 
// 
//    Rev 1.8   10 May 1993 19:36:16   DALLEE
// Repair functions prototypes are now in REPAIR.H, not CTS4.H
// #include'd REPAIR.H
// 
//    Rev 1.7   28 Apr 1993 09:10:14   MFALLEN
// 
//    Rev 1.6   22 Apr 1993 10:39:28   MFALLEN
// N30Notes
// 
//    Rev 1.5   21 Apr 1993 09:19:16   MFALLEN
// Commented out MTE code
// 
//    Rev 1.4   14 Apr 1993 15:17:24   MFALLEN
// 
//    Rev 1.3   26 Mar 1993 14:53:16   DALLEE
// Support for DOS.
//************************************************************************

#include "platform.h"
#include "stddos.h"
#include "xapi.h"
#include "disk.h"
#include "file.h"

#include "ctsn.h"
#include "repairn.h"
#include "virscan.h"

#include "navex.h"                      // for external detection/repair
#include "callback.h"                   // for external detection/repair
#include "avexprot.h"

#include "symcfg.h"                     // for registry manipulation

MODULE_NAME;

//************************************************************************
// DEFINES
//************************************************************************

#define REPAIR_ITERATIONS   100

#define OPEN_ACCESS_RO_NOMODLASTACCESS  0x4

//************************************************************************
// LOCAL PROTOTYPES
//************************************************************************

BOOL LOCAL VirusScanBackupFile (LPSTR lpszInfectedFile,
                                LPSTR lpszBackupExtension,
                                LPSTR lpszBackupFile);

//************************************************************************
// EXTERNAL VARIABLES
//************************************************************************

extern VIRSCAN ScanData;

//************************************************************************
//************************************************************************
char    SZ_STAR_STAR[]  = "*.*";


//************************************************************************
// DetermineVirusIndex()
//
// lpszDataBase         : path and filename of the VIRSCAN.DAT database
// wVirusID             : virus ID of the offending virus
// lpwVirusIndex        : changed by the function to provide the index
//                        into VIRSCAN.DAT where the record is.
// lpN30Buffer          : buffer where N30 data is placed.
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK              if no virus found
//                        EXTSTATUS_FILE_ERROR      if file error occured
//
// If EXTSTATUS_OK is returned, and *lpwVirusIndex != 0, then we have the
// proper record in VIRSCAN.DAT.  If == 0, then a record containing the
// specified virus ID was not found in the VIRSCAN.DAT
//
//************************************************************************
// 8/95 Carey created.
//************************************************************************

EXTSTATUS DetermineVirusIndex(LPSTR  lpszDataBase,
                              WORD   wVirusID,
                              LPWORD lpwVirusIndex,
                              LPN30  lpN30Buffer)
{
    HFILE   hFile;
    WORD    wCurRecord;
    UINT    uBytesRead;
    N30     temp;


    hFile = FileOpen(lpszDataBase, READ_ONLY_FILE);
    if (-1 == hFile)
        return(EXTSTATUS_FILE_ERROR);

    if (FileSeek(hFile,uSizeVirscanHeader,SEEK_SET) !=
        uSizeVirscanHeader)
    {
        FileClose(hFile);
        return(EXTSTATUS_FILE_ERROR);
    }

    wCurRecord = 0;

    do
    {
        uBytesRead = FileRead(hFile,&temp,sN30);
        if (sN30 == uBytesRead)
        {
            if (temp.wVirID == wVirusID)
            {
                FileClose(hFile);

                // all records returned 1-based; 0 is invalid.

                *lpwVirusIndex = wCurRecord + 1;

                if (lpN30Buffer != NULL)
                    *lpN30Buffer = temp;

                return(EXTSTATUS_OK);
            }

        }

        wCurRecord++;

    } while (uBytesRead == sN30);

    FileClose(hFile);

    *lpwVirusIndex = 0;

    return(EXTSTATUS_ENTRY_NOT_FOUND);
}


// external callbacks end


//---------------------------------------------------------------------------
//
// BOOL LOCAL VirusScanBackupFile (
//      LPSTR   lpszInfectedFile,       // Infected file (Input)
//      LPSTR   lpszBackupExtension,    // Extension for backup file (Input)
//      LPSTR   lpszBackupFile          // Name of backup file (Output)
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
BOOL LOCAL VirusScanBackupFile (LPSTR lpszInfectedFile,
                                LPSTR lpszBackupExtension,
                                LPSTR lpszBackupFile)
{
    BOOL    bStatus = TRUE;
    UINT    i = 0;
    int     nExtLen, nCountLen;
    char    szCount[10];
    char    szNewExtension[SYM_MAX_PATH+1];


    STRCPY(lpszBackupFile, lpszInfectedFile);
    NameStripExtension(lpszBackupFile);
    NameAppendExtension(lpszBackupFile, lpszBackupExtension);

                                        // Need to get a unique name for the
                                        // backup.
    while (FileExists(lpszBackupFile))
        {
        if (i > 999)
            {
            bStatus = FALSE;
            break;
            }

        _ConvertWordToString (i++, szCount, 10, 0);
        nCountLen = STRLEN(szCount);

                                        // Use up to 3 chars max for extension
                                        // plus counter.
        STRNCPY(szNewExtension, lpszBackupExtension, 3 - nCountLen);
        szNewExtension [3 - nCountLen] = EOS;

                                        // May need to pad with 0's.
        nExtLen = STRLEN(szNewExtension);

        if (nCountLen + nExtLen < 3)
            {
            StringAppendNChars(szNewExtension, '0', 3 - (nCountLen + nExtLen));
            }

        STRCAT(szNewExtension, szCount);

        NameStripExtension (lpszBackupFile);
        NameAppendExtension(lpszBackupFile, szNewExtension);
        }

    if (bStatus)
        bStatus = !FileCopy(lpszInfectedFile, lpszBackupFile, NULL);

    return(bStatus);
}


//---------------------------------------------------------------------------
//
// BOOL WINAPI PAMGreenLightVScan ( LPBYTE lpbyFStartBuffer )
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

BOOL WINAPI PAMGreenLightVScan(LPBYTE lpbyFStartBuffer)
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




//************************************************************************
// VirScanFile()
//
// Scans a given file for known viruses
//
// Parameters: 
//  lpFileName;  Full path to the file to scan              
//  lpVirusName; a minimum 40 bytes long buffer where a    
//               NULL terminated virus name is placed if   
//               file is infected.                         
// Returns:
//  0xFFFF if file can't be opened for read/ or DOS error 
//  0 = File is not infected                              
//  0 > File is infected, this is the virus ID            
//                                           
//************************************************************************
// 3/\93 Martin created.
//************************************************************************

UINT SYM_EXPORT WINAPI VirusScanFile (LPSTR lpFileName,
                                   LPN30NOTES lpNotes,
                                   BOOL bIsExecutable)
{
    auto    HFILE    hFile;
    auto    LPN30    lpN30;
                                        // uVirusSigIndex is the number of the
    auto    UINT     uVirusSigIndex = 0;// sig in memory.
#ifdef SYM_WIN32
    auto    FILETIME Time; 
#endif

                                        // The wVirID field in the N30 structs
                                        // is the index into the VIRSCAN.INF
                                        // file.
#include "progscan.h"                   // table of dynamically programmed
                                        // scanners (lpfnProgrammedScanners)

#ifdef SYM_WIN32
    BOOL bApisWereAnsi = AreFileApisANSI ();
    if (bApisWereAnsi)
        SetFileApisToOEM ();
#endif
    hFile = FileOpen( lpFileName, READ_ONLY_FILE | DENY_WRITE_FILE );
#ifdef SYM_WIN32
    if (bApisWereAnsi)
        SetFileApisToANSI ();
#endif

    if ( hFile != HFILE_ERROR )
        {
        *lpNotes->lpVirName = EOS;

#ifdef SYM_WIN32
                                        // Get the last access time so
                                        // we can restore it.
        GetFileTime ( (HANDLE)hFile, NULL, &Time, NULL );
#endif

        uVirusSigIndex =  SCN4_ME ( hFile,
                                    ScanData.szVirscanFile,
                                    (LPBYTE)ScanData.lpWorkMem,
                                    SIZE_WORK_BUFFER,
                                    (LPBYTE)ScanData.lpFileScanData,
                                    lpNotes->lpVirName,
                                    lpfnProgrammedScanners,
                                    bIsExecutable );

///////////////////////////////////////////////////////////////////////////////
// PAM
///////////////////////////////////////////////////////////////////////////////
#ifdef USE_PAM

        if (uVirusSigIndex == 0 &&
            ScanData.hGPAM != NULL &&
            PAMGreenLightVScan((LPBYTE)ScanData.lpWorkMem))
            {
            PAMSTATUS           pamStatus;
            WORD                wVirusID, wVirusIndex;

            pamStatus = PAMScanFile(ScanData.hLPAM,
                                    hFile,
                                    0,      // no flags for now; need one for SYS
                                    &wVirusID);

            //////////////////////////////////////////////////////////////////
            // For External Test:
            //////////////////////////////////////////////////////////////////

#ifdef SYM_WIN32
            
            if (ScanData.hLPAM->CPU.iteration > 128 &&
                pamStatus == PAMSTATUS_OK)
            {
                extern              char    szNAVWConfigRegProduct[],
                                            szNAVWConfigRegClinic[],
                                            szNAVWConfigRegKeyname[];

                static CFGREGKEY rNAVAPRegistryLocation = {
                                              HKEY_LOCAL_MACHINE,
                                              NULL,
                                              NULL,
                                              NULL,
                                              szNAVWConfigRegProduct,
                                              szNAVWConfigRegClinic,
                                              szNAVWConfigRegKeyname,
                                              NULL
                                              };

                DWORD   dwDestinationLength, dwRegKeyType;
                DWORD   dwIter;

                dwDestinationLength = sizeof(dwIter);
                dwRegKeyType = REG_DWORD;

                if ( ConfigRegQueryValue ( &rNAVAPRegistryLocation,
                                           NULL,
                                           &dwRegKeyType,
                                           &dwIter,
                                           &dwDestinationLength ) == ERROR_SUCCESS )
                {
                    if (ScanData.hLPAM->CPU.iteration >= dwIter &&
                        dwIter != 0)
                    {
                        BYTE    FirstHardDrive = DiskGetFirstFixedDrive();
                        TCHAR   szPath[1024], szIter[20];
                        TCHAR   szNavEmulPath[32];
                        HFILE   hFile;

                        if (FirstHardDrive == 0)
                            FirstHardDrive = 'C';

                                    // Setup the Emul file such that the drive
                                    // letter is assigned. NEC ==> A:
                        STRCPY(szNavEmulPath, _T("x:\\NAVEMUL!.DAT"));
                        szNavEmulPath[0] = FirstHardDrive;

                        STRCPY(szPath,lpFileName);
                        STRCAT(szPath,_T(" ") );
                        ConvertLongToString(ScanData.hLPAM->CPU.iteration, szIter);
                        STRCAT(szPath,szIter);
                        STRCAT(szPath, _T("\n") );

                        hFile = FileOpen(szNavEmulPath, READ_ONLY_FILE);
                        if (hFile == (HFILE)-1)
                        {
                            hFile = FileCreate(szNavEmulPath, FA_NORMAL);
                        }
                        if (hFile != (HFILE)-1)
                            FileClose(hFile);

                        hFile = FileOpen(szNavEmulPath, READ_WRITE_FILE);
                        if (hFile != (HFILE)-1)
                        {
                            FileSeek(hFile,0,SEEK_END);
                            FileWrite(hFile,szPath,STRLEN(szPath));
                            FileClose(hFile);
                        }
                    }
                }
            }
#endif // SYM_WIN32

            //////////////////////////////////////////////////////////////////

            if (PAMSTATUS_OK == pamStatus)
                {
                uVirusSigIndex = 0;
                }
            else if (PAMSTATUS_VIRUS_FOUND == pamStatus)
                {
                if (EXTSTATUS_OK ==
                DetermineVirusIndex(ScanData.szVirscanFile,
                                    wVirusID,
                                    &wVirusIndex,
                                    (LPN30)ScanData.lpWorkMem))
                    {
                    uVirusSigIndex = wVirusIndex;

                    // copy that virus name in there!


                    MEMCPY(lpNotes->lpVirName,
                           ((LPN30)ScanData.lpWorkMem)->lpVName,
                           SIZE_VIRUSNAME);
                    lpNotes->lpVirName[SIZE_VIRUSNAME] = 0;
                    }
                else
                    uVirusSigIndex = 0;     // error
                }
            else
                uVirusSigIndex = 0;         // error
            }
#endif
///////////////////////////////////////////////////////////////////////////////

// external start

        if (uVirusSigIndex == 0)            // no virus was found in the file!
            {
            EXTSTATUS           extResult;
            WORD                wVirusID;
            WORD                wVirusIndex;

            //////////////////////////////////////////////////////////////////////
            // No virus found using NAV 3 engine.  Try "fail-safe" detections!
            //
            // lpLocalWorkBuffer:
            //    First 256 bytes   = FSTART returned code
            //    Next  256 bytes   = first 64 bytes of host file
            //    Next ~1024 bytes  = external scan requests
            //    Next 4K-1536 bytes = work buffer for whatever
            //////////////////////////////////////////////////////////////////////


            extResult = EXTScanFile((LPCALLBACKREV1)ScanData.lpCallbacks,
                                    lpFileName,
                                    hFile,
                                    (LPBYTE)ScanData.lpWorkMem,
                                    ((LPBYTE)ScanData.lpWorkMem) + 1536,
                                    EXTERNAL_DETECT_VERSION,
                                    &wVirusID);

            if (EXTSTATUS_OK == extResult)
                {
                uVirusSigIndex = 0;
                }
            else if (EXTSTATUS_VIRUS_FOUND == extResult)
                {
                if (EXTSTATUS_OK ==
                    DetermineVirusIndex(ScanData.szVirscanFile,
                                        wVirusID,
                                        &wVirusIndex,
                                        (LPN30)ScanData.lpWorkMem))
                    {
                    uVirusSigIndex = wVirusIndex;

                    // copy that virus name in there!


                    MEMCPY(lpNotes->lpVirName,
                           ((LPN30)ScanData.lpWorkMem)->lpVName,
                           SIZE_VIRUSNAME);
                    lpNotes->lpVirName[SIZE_VIRUSNAME-1] = 0;
                    }
                else
                    uVirusSigIndex = 0;     // error
                }
            else
                uVirusSigIndex = 0;         // error
            }

// external end


        FileClose (hFile);
#ifdef SYM_WIN32
                                        // This is so we don't screw up
                                        // the last access time.  NOTE:
                                        // SetFileTime doesn't work with
                                        // files opened for read only.
    if ( (SystemGetWindowsType() != SYM_SYSTEM_WIN_NT) &&
         (Time.dwLowDateTime && Time.dwHighDateTime) )
        {
#if 1
                                        // Set the last access time. NOTE:
                                        // The return value from this 
                                        // function with these parameters
                                        // is FALSE even the function
                                        // succeeds.  (Refer to
                                        // GetLastError()and SetLastError()
                                        // descriptions.)
        SetFileTime ( (HANDLE)hFile, NULL, &Time, NULL );
#else
        SYM_REGS sRegisters;
        WORD wDate;
        WORD wTime;

        FileTimeToDosDateTime( &Time, &wDate, &wTime );
        sRegisters.EAX = 0x00007143;            // get/set file attributes 
        sRegisters.EBX = 0x00000005;            // set last access date 
        sRegisters.ECX = wTime;                 // last access time 
        sRegisters.EDX = ( DWORD ) lpFileName;  // file name 
        sRegisters.EDI = wDate;                 // last access date 

        VxDIntBasedCall( 0x21, &sRegisters );
#endif
        }
#endif

        if ( ( (UINT) -1 ) == uVirusSigIndex )
            uVirusSigIndex = 0 ;

        if (uVirusSigIndex)
            {
            lpN30 = (LPN30)ScanData.lpWorkMem;

            lpNotes->wVirID   = lpN30->wVirID;
            lpNotes->wType    = lpN30->wType;
            lpNotes->wControl2= lpN30->wControl2;
            lpNotes->wInfo    = lpN30->wInfo;
            lpNotes->wSize    = lpN30->wInfSize;
            }
                                        // Clear the scan buffer of virus
                                        // traces so we do not false id
                                        // on a following mem scan.
        MEMSET(ScanData.lpWorkMem, '\0', SIZE_WORK_BUFFER);
        }
    else
        {
        MEMSET (lpNotes, EOS, sizeof(N30NOTES));
        }

    return (uVirusSigIndex);
} // End VirScanFile()


//************************************************************************
// VirScanRepairFile()
//
// Attempts to repair an infected file.
//
// Parameters: 
//  lpFileName; Full Path to the file to repair.           
//  uVirusID; the virus ID obtained from scanning.         
//
// Returns:
//  FALSE; error happened                                  
//  TRUE; we did our best                                 
//                                           
//************************************************************************
// 3/\93 Martin created.
// 7/12/93 DALLEE, Do only REPAIR_ITERATIONS repeated repair attempts.
//          Scan and get a new uVirusID after each repair attempt.
//          Close file opened for repairs before rescanning.
// 9/01/93 DALLEE, Set attributes to NORMAL so we can open the file R/W.
//************************************************************************

BOOL SYM_EXPORT WINAPI VirusScanRepairFile (LPSTR lpFileName, UINT uVirusID)
{
    auto    UINT        uRepairs;
    auto    HFILE       hFile;
    auto    BOOL        bStatus = TRUE;
    auto    N30NOTES    N30Notes;
    auto    UINT        uOrigAttr;

    #ifdef SYM_WIN32
        BOOL bApisWereAnsi = AreFileApisANSI ();
    #endif

// external start

    EXTSTATUS           extResult;
    N30                 stN30;
// external end

    // Do up to REPAIR_ITERATIONS attempts
    // at repairing as long as the repair
    // returns TRUE and the resulting file
    // is still infected.

    for (uRepairs = 0;
         (uRepairs < REPAIR_ITERATIONS) && (bStatus == TRUE);
         uRepairs++)
        {

        // external start

        if ((hFile = FileOpen (ScanData.szVirscanFile,
                               READ_ONLY_FILE)) != HFILE_ERROR)
            {
            SCN4_GET_ENTRY (hFile, (WORD)uVirusID, (char *)&stN30);
            FileClose (hFile);
            }


        #ifdef SYM_WIN32
            if (bApisWereAnsi)
                SetFileApisToOEM ();
        #endif

        extResult = EXTRepairFile((LPCALLBACKREV1)ScanData.lpCallbacks,
                                  EXTERNAL_DETECT_VERSION,
                                  &stN30,
                                  lpFileName,
                                  (LPBYTE)ScanData.lpWorkMem);

        #ifdef SYM_WIN32
            if (bApisWereAnsi)
                SetFileApisToANSI ();
        #endif

        if (EXTSTATUS_OK == extResult)
            {
            bStatus = TRUE;
            }
        else if (EXTSTATUS_FILE_ERROR == extResult)
            {
            bStatus = FALSE;
            }

///////////////////////////////////////////////////////////////////////////////
// PAM
///////////////////////////////////////////////////////////////////////////////
#ifdef USE_PAM

        else if (stN30.wControl2 & USE_PAM_REPAIR)
            {

            #ifdef SYM_WIN32
                if (bApisWereAnsi)
                    SetFileApisToOEM ();
            #endif

            FileGetAttr(lpFileName, &uOrigAttr);

            if (NOERR == FileSetAttr(lpFileName, FA_NORMAL))
                {
                BOOL        bResult;
                TCHAR       szBackup[SYM_MAX_PATH+1];
                PAMSTATUS   pamStatus;
                TCHAR       szPAM[SYM_MAX_PATH+1];

                bResult = VirusScanBackupFile (lpFileName, _T("PAM"), szBackup);

                if (FALSE == bResult)
                    {
                    FileSetAttr(lpFileName, uOrigAttr);
                    bStatus = FALSE;
                    break;
                    }

                VirusScanGetDatFile(szPAM, sizeof(szPAM) - 1);
                NameStripFile(szPAM);
                NameAppendFile(szPAM, _T("VIRSCAN2.DAT") );

                // File apis are oem at this point so we need to convert
                // the virscan2.dat file name in case the path contains
                // Ansi characters

                #ifdef SYM_WIN32
                    NAnsiToOem (szPAM, szPAM);
                #endif

                pamStatus = PAMRepairFile(ScanData.hLPAM,
                                          szPAM,
                                          szBackup,         // temp swap file
                                          lpFileName,       // infected program
                                          stN30.wBookMark,  // which repair #?
                                          FALSE);           // no SYS override


                // PAMSTATUS_NO_REPAIR or PAMSTATUS_*_ERROR

                if (PAMSTATUS_OK != pamStatus)
                    {
                    bStatus = FALSE;
                    }

                // reset the attributes when we're done and kill our temp file!

                FileSetAttr(lpFileName, uOrigAttr);
                VirusScanDeleteFile(szBackup,1);

                #ifdef SYM_WIN32
                    if (bApisWereAnsi)
                        SetFileApisToANSI ();
                    bApisWereAnsi = FALSE;
                #endif
                }
            else
                {
                #ifdef SYM_WIN32
                    if (bApisWereAnsi)
                        SetFileApisToANSI ();
                    bApisWereAnsi = FALSE;
                #endif

                bStatus = FALSE;
                break;
                }
            }

#endif
//////////////////////////////////////////////////////////////////////////////

        else                        // EXTSTATUS_NO_REPAIR
            {

            // external end

            #ifdef SYM_WIN32
                if (bApisWereAnsi)
                    SetFileApisToOEM ();
            #endif

            FileGetAttr(lpFileName, &uOrigAttr);

            if (NOERR == FileSetAttr(lpFileName, FA_NORMAL))
                {
                if ((hFile = FileOpen(lpFileName, READ_WRITE_FILE)) ==
                    HFILE_ERROR)
                    {
                    FileSetAttr(lpFileName, uOrigAttr);
                    bStatus = FALSE;
                    break;
                    }

                #ifdef SYM_WIN32
                    if (bApisWereAnsi)
                        SetFileApisToANSI ();
                    bApisWereAnsi = FALSE;
                #endif

                // the function call to REP4_ME has been changed so that the last
                // argument contains the filename of the file being repaired. This
                // is used by the DIR2 repair in REP4_FIL.ASM

                bStatus = REP4_ME ( (WORD) uVirusID,
                                    hFile,
                                    ScanData.szVirscanFile,
                                    ScanData.lpWorkMem,
                                    SIZE_WORK_BUFFER,
                                    lpFileName);

                FileClose(hFile);

                FileSetAttr(lpFileName, uOrigAttr);
                }
            else
                bStatus = FALSE;

            #ifdef SYM_WIN32
                if (bApisWereAnsi)
                    SetFileApisToANSI ();
                bApisWereAnsi = FALSE;
            #endif

            // external start
            }
            // external end

        if (0 == (uVirusID = VirusScanFile(lpFileName, &N30Notes, TRUE)))
            {
            break;
            }
        }

    if (uRepairs >= REPAIR_ITERATIONS)
        bStatus = FALSE;

    #ifdef SYM_WIN32
        if (bApisWereAnsi)
            SetFileApisToANSI ();
        bApisWereAnsi = FALSE;
    #endif


    return (bStatus);
} // End VirScanRepairFile()


//************************************************************************
// VirScanDelFile()
//
// Deletes the specified file.
//
// Parameters: 
//  lpFullPath; of the file to delete                      
//  uWipe; number of times to zero out the file.           
//         Usually this can be 0 to improve speed.         
//
// Returns:
//  TRUE; everything when fine                            
//  FALSE; error happened                                  
//                                           
//************************************************************************
// 3/\93 Martin created.
//************************************************************************

BOOL SYM_EXPORT WINAPI VirusScanDeleteFile(LPSTR lpFullPath, UINT uWipe)
{
    auto    BOOL    bSuccess = FALSE;

    bSuccess = FIL4_KILL (lpFullPath, (LPBYTE)ScanData.lpWorkMem, SIZE_WORK_BUFFER, (WORD) uWipe);

    return (bSuccess);
} // End VirScanDelFile()


//************************************************************************
// VirScanDriveDir()
//
//  Does a super fast findfirst findnext on the specified drive\directory
//  (plus optional filespec). For every file found, the user specified 
//  callback function is called.                            
//
//  Examples:
//     INPUT:                              DIRECTORY            FILESPEC
//      C:\WORK (an existing directory) -> C:\WORK              *.*
//      C:\WORK (a file)                -> C:\                  WORK.
//      C:\WORK\*.C                     -> C:\WORK              *.C
//      C:\WORK\                        -> C:\WORK              *.*
//      C:\WORK\file.ext                -> C:\WORK              FILE.EXT
//
//      NOTE:  If the sub-dir flag is on, FILESPEC will be searched in
//             all sub-directories from DIRECTORY.
//
// Parameters: 
//  lpfnCallBack; should be a function expecting:
//  UINT WINAPI _loadds CallBackName (LPSTR lpFileName,
//                                   DTA_ FAR *lpDta,
//                                   LPARAM lYourData)
//  NOTES:
//         1.  for DOS use the __loadds modifier for the callback funtion
//             if SYM_EXPORT doesn't take care of loading the correct DS on entry.
//
//         2.  The callback function is responsible for calling
//             the actual scanning routine (such as VirScanFile()) to
//             detect whether or not the current file is infected.
//
// Returns:
//  TRUE                                Scan went normally
//  FALSE                               Error: Could not switch to directory,
//                                          or could not allocate memory for
//                                          working buffer.
//************************************************************************
// 3/\93 Martin created.
// 8/16/93 BarryG Updated to support filespec (including wildcards)
//************************************************************************

BOOL SYM_EXPORT WINAPI VirusScanDriveDir (LPSTR lpPath, FARPROC lpfnCallBack,
                                      BOOL bSubDirs, LPARAM lYourData)
{
    char szDir  [SYM_MAX_PATH + 1];
    char szFile [SYM_MAX_PATH + 1];

    // Split lpPath into Directory and Filespec components

    if ( NameIsWildcard ( lpPath ) ||
        (NameGetType    ( lpPath ) & NGT_IS_FILE) )
        {
        NameSeparatePath ( lpPath , szDir , szFile ) ;
        }
    else                            // If not really a dir, we'll catch below
        {
        STRCPY ( szDir , lpPath ) ;
        STRCPY ( szFile , SZ_STAR_STAR) ;
        }

    return VirusScanWildcard(szDir, szFile, lpfnCallBack, bSubDirs, lYourData) ;

} // End VirScanDriveDir()

//************************************************************************
// VirScanWildcard()
//
//  Does a super fast findfirst findnext on the specified path & filespec.
//  For every file found, the user specified callback function is called.                            
//  
// Parameters: 
//  lpfnCallBack; should be a function expecting:
//  UINT WINAPI _loadds CallBackName (LPSTR lpFileName,
//                                   DTA_ FAR *lpDta,
//                                   LPARAM lYourData)
//  lpPath -- Drive\directory path to start search
//  lpFilespec -- Filespec to search and match.  May contain wildcards
//  bSubDirs -- Flag indicating whether to search sub-directories
//
//  NOTES:
//         1.  for DOS use the __loadds modifier for the callback funtion
//             if SYM_EXPORT doesn't take care of loading the correct DS on entry.
//
//         2.  The callback function is responsible for calling
//             the actual scanning routine (such as VirScanFile()) to
//             detect whether or not the current file is infected.
//
// Returns:
//  TRUE                                Scan went normally
//  FALSE                               Error: Could not switch to directory,
//                                          or could not allocate memory for
//                                          working buffer.
//************************************************************************
// 8/16/93 BarryG Created.  Should replace VirusScanDriveDir
//************************************************************************

BOOL SYM_EXPORT WINAPI VirusScanWildcard (LPSTR lpPath, LPSTR lpFile , 
                                       FARPROC lpfnCallBack,
                                       BOOL bSubDirs, LPARAM lYourData)
{
    char szCWD[SYM_MAX_PATH + 1];       // Current working directory //
    BYTE byDisk;
    BOOL bStatus = TRUE;

    HGLOBAL hBuffer;
    LPSTR   lpBuffer;


    if ( (hBuffer = MemAlloc (GHND, SIZE_WORK_BUFFER)) == 0 )
        return (FALSE);

    if ( (lpBuffer = (LPSTR)MemLock (hBuffer)) != NULL )
        {
        STRUPR (lpPath);

        if (lpFile == NULL || *lpFile == EOS)
            lpFile = SZ_STAR_STAR;

        // ************************************************************* //
        // Save current working drive/directory so the user should curse //
        // because I change his directory.                               //
        // ************************************************************* //

        byDisk = DiskGet();                             

        // ***************************************************** //
        // Now we try to switch to the specified drive/directory //
        // ***************************************************** //

        DiskSet (lpPath[0]);
        DirGet  (lpPath[0], szCWD);     // Save current dir on this drive

        // FIL4_FIND_ALL is faster but doesn't handle wildcards other than *.*

        if ( DirSet (lpPath) == ERR )
            {
            bStatus = FALSE;
            if (NameIsRoot(lpPath))
                {
                                        // Try to move up from current dir,
                                        // until we can't.
                auto        char    szDirectory[SYM_MAX_PATH+1];


                if ( DirGet(*lpPath, szDirectory) != ERR )
                    {
                    do {
                        NameStripFile(szDirectory);
                       }
                    while (DirSet(szDirectory) != ERR && 
                           !NameIsRoot(szDirectory));                

                    bStatus = TRUE;
                    }
                }
            }

        if ( bStatus )
            {
/* CERTLIB4 is not kosher */
#ifdef SYM_DOS
            if ( STRCMP (lpFile, SZ_STAR_STAR) == 0 )
                FIL4_FIND_ALL (lpfnCallBack, lYourData, bSubDirs, lpBuffer);
            else
                FIL4_FIND_WILD (lpFile, lpfnCallBack, lYourData, bSubDirs, lpBuffer);
#endif
            bStatus = FALSE;
            }

        // *********************************************************** //
        // We switch back to the drive directory where we started from //
        // *********************************************************** //

        DirSet(szCWD);                  // Restore this drive's directory
        DiskSet(byDisk);                // Restore original drive

        MemRelease( hBuffer );
        }
    else                                // Failed to lock memory
        {
        bStatus = FALSE;
        }

    MemFree(hBuffer);

    return (bStatus);
} // End VirScanDriveDir()
