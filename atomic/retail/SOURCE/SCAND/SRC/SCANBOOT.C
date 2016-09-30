// Copyright 1993 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/SCAND/VCS/SCANBOOT.C_v   1.4   01 Feb 1999 14:05:28   MBROWN  $
//
// Description:
//      These are the DOS NAV boot record scanning functions.
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/SCAND/VCS/SCANBOOT.C_v  $
// 
//    Rev 1.4   01 Feb 1999 14:05:28   MBROWN
// Backed out of large sector fix for Phoenix
//
//    Rev 1.2   27 Jul 1998 10:27:14   tcashin
// In ScanMBR(), make sure ABSDISKREC.bIsExtInt13 is set to ISINT13X_UNKNOWN
// (defect #119072).
//
//    Rev 1.1   11 May 1998 18:06:58   mdunn
// Fixed references to ABSDISKREC members.
//
//    Rev 1.0   06 Feb 1997 21:08:42   RFULLER
// Initial revision
//
//    Rev 1.1   08 Aug 1996 13:26:18   JBRENNA
// Ported from DOSNAV environment
//
//
//    Rev 1.25   30 Jul 1996 16:49:06   JALLEE
// Check for gbContinueScan during boot scan.
//
//    Rev 1.24   23 Jul 1996 14:48:44   JALLEE
// When we come across a drive with no boot record ( or unrecognized type ) we
// will now scan it.  How can we scan the boot record if it doesn't exist?  We
// will simply show the progress bar and increment a counter!  STS #58506
//
//    Rev 1.23   06 May 1996 09:07:06   MZAREMB
// Initial Pegasus update: added NAVSCAN #ifdefs.
//
//    Rev 1.22   10 Nov 1995 14:06:44   JWORDEN
// Force filename for MBR viruses to conform to the format x:
// where x is the physical drive number - 80h
//
//    Rev 1.21   02 Nov 1995 17:04:18   JWORDEN
// Convert MasterBootRec booleans to counters to allow for multiple MBRs
//
//    Rev 1.20   24 Oct 1995 20:37:16   JWORDEN
// Alter processing for MBR to scan all hard drives
//
//    Rev 1.19   14 Mar 1995 17:46:02   DALLEE
// Hide display during startup util problems are found.
//
//    Rev 1.18   29 Dec 1994 16:46:28   DALLEE
// Include syminteg.h before nonav.h
//
//    Rev 1.17   28 Dec 1994 14:15:48   DALLEE
// Latest NAVBOOT revision.
//
    //    Rev 1.2   27 Dec 1994 19:36:24   DALLEE
    // Convert to use CERTLIBN.
    //
    //    Rev 1.1   05 Dec 1994 18:10:16   DALLEE
    // CVT1 script.
    //
//    Rev 1.16   28 Dec 1994 13:53:20   DALLEE
// Basis for NAVBOOT code from NAV v3.05.
// First (.DX) port to trunk is previous version labeled "PRENAVBOOT".
//
    //    Rev 1.0   23 Nov 1994 15:38:20   DALLEE
    // Initial revision.
    //
    //    Rev 1.12   15 Sep 1993 05:10:44   DALLEE
    // SkipDriveTimedErrDialog() now takes UINT type of error instead of
    // BOOL boot record error.
//************************************************************************

#include "platform.h"
#include "xapi.h"
#include "stddos.h"
#include "stdio.h"
#include "disk.h"
#include "tsr.h"

#include "ctsn.h"
#include "virscan.h"

#include "options.h"
#include "navutil.h"
#include "syminteg.h"
#include "nonav.h"
#include "navdprot.h"
#include "tsrcomm.h"

#include "scand.h"

MODULE_NAME;

//************************************************************************
// ScanMBR()
//
// This routine scans for viruses in the master boot record.
//
// Parameters:
//      LPNONAV     lpNoNav             Pointer to info for this scan.
//
// Returns:
//      Nothing
//************************************************************************
// 5/27/93 DALLEE, Function created.
//************************************************************************

VOID PASCAL ScanMBR (LPNONAV lpNoNav)
{
    extern  OutputFieldRec  ofScanMBRBar;

    auto    WORD            wVirID;
    auto    WORD            wResult;
    auto    BYTE            byDrive;
    auto    ABSDISKREC      rAbsDisk;
    auto    LPBYTE          lpbyBuffer;


    TSR_OFF;

                                        // -----------------------------------
                                        // Create a temporary buffer to hold
                                        // the disk sector during our test read
                                        // below
                                        // -----------------------------------

    lpbyBuffer = (LPBYTE)MemAllocPtr (GHND, PHYSICAL_SECTOR_SIZE);

                                        // -----------------------------------
                                        // Process each physical drive in turn
                                        // We determine that a physical drive
                                        // exists by attempting an absolute
                                        // read of sector 1. If it works the
                                        // Drive exists and must be scanned.
                                        // If it doesn't work we quit
                                        // -----------------------------------

    for (byDrive = 0x80; byDrive < 256; byDrive++)
    {
                                        // -----------------------------------
                                        // Is this a local hard drive?
                                        // If not, get out
                                        // -----------------------------------
        rAbsDisk.dn = byDrive;
        rAbsDisk.dwHead = 0;
        rAbsDisk.dwTrack = 0;
        rAbsDisk.dwSector = 1;
        rAbsDisk.numSectors = 1;
        rAbsDisk.buffer = lpbyBuffer;
        rAbsDisk.bIsExtInt13 = ISINT13X_UNKNOWN;

        if (DiskAbsOperation (READ_COMMAND, &rAbsDisk))
            break;

                                        // -----------------------------------
                                        // Since we were able to read, this is
                                        // a real hard drive.
                                        // -----------------------------------

        wVirID = VirusScanMasterBoot ((UINT)byDrive,
                                      &lpNoNav->Infected.Notes,
                                      &wResult);

        if (!wResult)
            {
            lpNoNav->Stats.MasterBootRec.uScanned++;

            ScanMemoryBootFillBar(&ofScanMBRBar, FALSE);

            if (wVirID && !wResult)
                {
                                        // MBR is infected, save this info
                                        // to the NONAV struct.
                lpNoNav->Infected.uVirusSigIndex = wVirID;
                lpNoNav->Stats.MasterBootRec.uInfected++;
                lpNoNav->Infected.szFullPath[0] = (UINT)(byDrive - 0x80);
                lpNoNav->Infected.szFullPath[1] = ':';
                lpNoNav->Infected.szFullPath[2] = 0x00;

                lpNoNav->Infected.wStatus = FILESTATUS_MASTERBOOTINFECTED;

                                        // Determine what action to take
                RespondToBootVirus(lpNoNav);
                }
            }
        }

    TSR_ON;
    MemFreePtr (lpbyBuffer);

} // End ScanMBR()


//************************************************************************
// LocalDiskHasBootRecord()
//
// This routine is copied from SYMKRNL because we can not change symkrnl
// and maintain compatibility with NDW in the field.
// The fix is to return that TYPE_RAMDRIVE does NOT have a valid
// boot sector that can be scanned.
//
// Parameters:
//      BYTE        dl                  Drive letter to check
//
// Returns:
//      TRUE  - Drive has a boot record
//      FALSE - Drive does not have a boot record
//************************************************************************
// 5/27/93 DALLEE, Function created.
//************************************************************************


BOOL PASCAL LocalDiskHasBootRecord( BYTE dl )
{
    auto        WORD    wMajor, wMinor;


                                        // OS/2 won't let you access the
                                        // boot record for HPFS volumes
    if ( DiskIsHPFS(dl) )
        return (FALSE);

    DiskGetType(dl, &wMajor, &wMinor);

    return ((BOOL)!(wMajor == TYPE_UNKNOWN ||
                    wMajor == TYPE_DOESNT_EXIST ||
                    wMajor == TYPE_PHANTOM ||
                    wMajor == TYPE_REMOTE ||
                    wMajor == TYPE_CDROM ||
                    wMajor == TYPE_RAMDISK ||
                  ((wMajor == TYPE_LOGICAL) && (wMinor == TYPE_STACKER))) );
}


//************************************************************************
// ScanBootRecords()
//
// This routine scans for viruses in boot records.
// If an error is encountered, it will display a Retry/Abort dialog.
// If the user aborts, it will disable scanning for all items in lpScanList
// that refer to the same drive.
//
// Parameters:
//      LPNONAV     lpNoNav             Pointer to info for this scan.
//
// Returns:
//      Nothing
//************************************************************************
// 5/27/93 DALLEE, Function created.
//************************************************************************

VOID PASCAL ScanBootRecords (LPNONAV lpNoNav)
{
    extern  BOOL        gbContinueScan;
    extern  OutputFieldRec  ofScanBootRecordsBar;

    auto    WORD            wVirID;
    auto    WORD            wResult;
    auto    LPSTR           lpScanList;
                                        // These are used in error handling:
    auto    char            szTempBuf [ 5 ] ;
    auto    BOOL            byaUsedDrives [ SYM_MAX_DOS_DRIVES ] ;
    auto    BYTE            byDrive;
    auto    BYTE            byDriveNum;
    auto    BOOL            bHasBoot;


    _fmemset ( byaUsedDrives , FALSE , sizeof ( byaUsedDrives ) ) ;

    lpScanList = MemLock (lpNoNav->lpScan->hScanList);
    if (lpScanList != NULL)
        {
        while (*lpScanList && gbContinueScan)
            {
            byDrive = *lpScanList;
            byDriveNum = *lpScanList - 'A';
            if ( !(byDrive >= 'A' && byDrive <= 'Z') )
                goto nextDrive;

                                        // If already processed, continue
            if ( byaUsedDrives [byDriveNum] )
                goto nextDrive;
                                        // Used to be calling DiskIsNetwork(),
                                        // but there are some non-network drives
                                        // which also can't have their boot
                                        // records read.
            TSR_OFF;
            bHasBoot = LocalDiskHasBootRecord(byDrive);
            TSR_ON;

            if ( bHasBoot )
                {
                TSR_OFF;
                wVirID = VirusScanBoot((WORD) byDrive,
                                       &lpNoNav->Infected.Notes,
                                       &wResult);
                TSR_ON;

                if ( wResult == ERR_VIRSCAN_READBOOT )
                    {
                    extern  BOOL    bScreenStarted;
                    auto    UINT    uRv;

                    if (!bScreenStarted)
                        {
                        extern  DIALOG_RECORD   *glpScanFilesDialog;
                        extern  DIALOG_RECORD   *glpMemoryBootDialog;
                        InitDisplay(TRUE);

                        glpScanFilesDialog = OpenScanFilesDialog(lpNoNav);
                        glpMemoryBootDialog = OpenMemoryBootDialog(lpNoNav);
                        }

                                        // Default is IGNORE because Lantastic
                                        // and possibly others protect the boot
                                        // sector.
                    uRv = SkipDriveTimedErrDialog ( *lpScanList,
                                                    SKIPDRIVE_DLG_BOOT,
                                                    SKIPDRIVE_IGNORE,
                                                    lpNoNav );

                    if ( uRv == SKIPDRIVE_ABORT )
                        {               // Abort: drop drive from list
                        StringMaxCopy ( szTempBuf , lpScanList , 3 ) ;
                        SzzStripMatches ( lpScanList , szTempBuf , TRUE ) ;
                        continue ;      // Positioned at next in list
                        }
                    else if ( uRv == SKIPDRIVE_RETRY )
                        continue ;      // Try this element again
                    // else SKIPDRIVE_IGNORE -- fall through to next in list

                    } // if ( error reading boot record ) 

                // Other errors that are being ignored right now:
                // ERR_VIRSCAN_DOSFREE -- &? indicates VMM corruption
                // ERR_VIRSCAN_DOSALLOC -- out of memory

                                // On a valid scan, wResult is set to 0.
                else if (wResult == 0)
                    {
                                        // Only update progress bar and used
                                        // drive flags on success
                    byaUsedDrives [byDriveNum] = TRUE;

                    ScanMemoryBootFillBar(&ofScanBootRecordsBar, FALSE);

                    lpNoNav->Stats.BootRecs.uScanned++;

                    if (wVirID)
                        {
                                // This boot record is infected, save
                                // this info to the NONAV struct.
                                //*** DALLEE, Kludge here to put
                                // [drive]: into the infected item name.
                        lpNoNav->Infected.uVirusSigIndex = wVirID;
                        lpNoNav->Stats.BootRecs.uInfected++;
                                        // Just copy the drive letter info
                        StringMaxCopy ( lpNoNav->Infected.szFullPath,
                                        lpScanList , 3 ) ;
                        lpNoNav->Infected.wStatus = FILESTATUS_BOOTINFECTED;

                        RespondToBootVirus(lpNoNav);
                        }
                    }
                }
            else
                {
                                        // If drive doesn't have boot, mark
                                        // as already processed.

                                        // Also, PRETEND we scanned it!?
                                        // STS #58506
                byaUsedDrives [byDriveNum] = TRUE;

                ScanMemoryBootFillBar(&ofScanBootRecordsBar, FALSE);

                lpNoNav->Stats.BootRecs.uScanned++;
                }
                                // Advance to the next item to scan.
nextDrive:
            lpScanList = SzzFindNext ( lpScanList ) ;
            }

        MemUnlock(lpNoNav->lpScan->hScanList, lpScanList );
        }
} // End ScanBootRecords()


