// Copyright 1993-1996 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/NAVC/VCS/scanboot.c_v   1.8   01 Feb 1999 14:01:18   MBROWN  $
//
// Description:
//      These are the DOS NAV boot record scanning functions.
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/NAVC/VCS/scanboot.c_v  $
// 
//    Rev 1.8   01 Feb 1999 14:01:18   MBROWN
// Backed out of large sector fix for Phoenix
//
//    Rev 1.6   25 Jul 1998 17:42:46   tcashin
// Fixed a bug in ScanMBR(). Set bIsExtInt13 in the AbsDisk structure
// to ISINT13X_UNKNOWN (defect #119014).
//
//    Rev 1.5   11 May 1998 18:15:52   mdunn
// Fixed references to ABSDISK fields.
//
//    Rev 1.4   29 Jul 1997 11:37:26   MKEATIN
// If we fail to read a boot record - don't try reading again.
//
//    Rev 1.3   24 Jul 1997 18:27:14   MKEATIN
// If we get a read error scanning the boot just move on to the next item in
// the list.
//
//    Rev 1.2   17 Jul 1997 11:27:38   MKEATIN
// Added a missing comma.
//
//    Rev 1.1   17 Jul 1997 10:59:54   MKEATIN
// If VirusScanBoot() returns ERR_VIRSCAN_READBOOT we now respond properly.
//
//    Rev 1.0   06 Feb 1997 20:56:22   RFULLER
// Initial revision
//
//    Rev 1.1   06 Jan 1997 21:14:10   MKEATIN
//
//
//    Rev 1.0   31 Dec 1996 15:20:04   MKEATIN
// Initial revision.
//
//    Rev 1.8   26 Dec 1996 17:40:40   JBELDEN
// removed false scanboots and returning ok when the scan isn't
// actuall done.
//
//    Rev 1.7   19 Dec 1996 14:48:36   JBELDEN
// fixed so that shows multiple scans of different boots and
// doesn't say boot records scanned when an invalid path is given.
//
//    Rev 1.6   03 Dec 1996 17:09:18   JBELDEN
// fixed drive number for display of MBR scanning
//
//    Rev 1.5   27 Nov 1996 11:13:08   JBELDEN
// fixed drive letter shown during message about mbr being infected.
//
//    Rev 1.4   31 Oct 1996 18:02:50   JBELDEN
// removed more dialog code
//
//    Rev 1.3   14 Oct 1996 10:47:16   JBELDEN
// changed DOSPrint back to printf and added fprintf for logging
//
//    Rev 1.2   09 Oct 1996 17:16:18   JBELDEN
// fixed problem with giving OK message improperly.
//
//    Rev 1.1   03 Oct 1996 17:54:34   JBELDEN
// added DOSPrint messages and deleted some code.
//
//    Rev 1.0   02 Oct 1996 12:58:48   JBELDEN
// Initial revision.
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

extern FILE  *fLogFile;
extern BOOL   bAutoExportLog;


MODULE_NAME;

//*************************************************************************
// DiskHasMBR()
//
// DiskHasMBR(
//           LPBYTE lpBuffer - buffer containing data read from CHS 001.
//           )
//
// Description: This routine will scan the specified drive for NEC 1.2MB
//              specific boot viruses.
//
// PARAM: lpBuffer <-- This buffer must be at least 512 bytes long!
//
// Returns: BYTE  - TRUE if 1st sector contains valid BPB
//                  FALSE otherwise (we assume it's an MBR)
//
//*************************************************************************
// 11/3/98 MBROWN, created
// 03/24/2000 <-- Function fixed. Yikes! I'm not sure why MBROWN assumed
//                every disk would be formated with FAT32. Also, some viruses
//                scramble the BPB, so checking the BPB is a bad idea.
//                Checking the MBR signature seems like a much better way.               
//*************************************************************************
BOOL DiskHasMBR( LPBYTE lpBuffer )
{
    BOOL bResult = FALSE;

    //
    // Make sure the buffer is not NULL.
    //
    if( lpBuffer )
    {
        //
        // Look for valid MBR signature.  If 0x55AA is not found, then 
        // we're not looking at a valid MBR.  I'm not casting to a word here 
        // because this is a very simple constant sig check.  
        // Nothing fancy is needed.
        //
        if( 0x55 == lpBuffer[510] && 0xAA == lpBuffer[511] )
        {
            bResult = TRUE;
        }
    }

    return bResult;
}

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
    extern char   szScanningMBR[];
    extern char   szMBRInfected[];
    extern char   szOK[];

    auto    WORD            wVirID;
    auto    WORD            wResult;
    auto    BYTE            byDrive;
    auto    ABSDISKREC      rAbsDisk;
    auto    LPBYTE          lpbyBuffer;
    auto    BYTE            byNumHardDisks;

    auto    BOOL            bOK = TRUE;

    TSR_OFF;

     printf(szScanningMBR);
                                        // Instead of trying to read from every
                                        // disk until the read fails, get the
                                        // number of disks from DiskGetPhysicalInfo
    rAbsDisk.dn = 0x80;
    DiskGetPhysicalInfo(&rAbsDisk);
    byNumHardDisks = rAbsDisk.numDrives;

                                        // -----------------------------------
                                        // Create a temporary buffer to hold
                                        // the disk sector during our test read
                                        // below
                                        // -----------------------------------

    lpbyBuffer = (LPBYTE)MemAllocPtr (GHND, 8 * PHYSICAL_SECTOR_SIZE);

                                        // -----------------------------------
                                        // Process each physical drive in turn
                                        // We determine that a physical drive
                                        // exists by attempting an absolute
                                        // read of sector 1. If it works the
                                        // Drive exists and must be scanned.
                                        // If it doesn't work we quit
                                        // -----------------------------------

    for (byDrive = 0x80; byDrive < (0x80 + byNumHardDisks); byDrive++)
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

        //
        // If the disk does not have a valid MBR, then we don't want to scan it. 
        // The buffer must be at lest 512 bytes long!
        // [CBROWN 03/24/2000]
        //
        if( ! DiskHasMBR(rAbsDisk.buffer) )
            continue;
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

            if (wVirID && !wResult)
                {

                bOK = FALSE;

                                        // MBR is infected, save this info
                                        // to the NONAV struct.
                lpNoNav->Infected.uVirusSigIndex = wVirID;
                lpNoNav->Stats.MasterBootRec.uInfected++;
                lpNoNav->Infected.szFullPath[0] = (UINT)(byDrive - 0x80);
                lpNoNav->Infected.szFullPath[1] = ':';
                lpNoNav->Infected.szFullPath[2] = 0x00;

                lpNoNav->Infected.wStatus = FILESTATUS_MASTERBOOTINFECTED;

                printf( szMBRInfected,(UINT)(byDrive - 0x80) , lpNoNav->Infected.Notes.lpVirName);
                if ( bAutoExportLog )
                     fprintf(fLogFile, szMBRInfected, (UINT)(byDrive - 0x80), lpNoNav->Infected.Notes.lpVirName);

                                        // Determine what action to take
                RespondToBootVirus(lpNoNav);
                }
            }
        }
    if (bOK == TRUE)
            printf(szOK);
    else
            printf("\n");

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
    extern  char szScanningBoot[];
    extern  char szBootInfected[];
    extern  char szOK[];

    auto    WORD            wVirID;
    auto    WORD            wResult;
    auto    LPSTR           lpScanList;
                                        // These are used in error handling:
    auto    char            szTempBuf [ 5 ] ;
    auto    BOOL            byaUsedDrives [ SYM_MAX_DOS_DRIVES ] ;
    auto    BYTE            byDrive;
    auto    BYTE            byDriveNum;
    auto    BOOL            bHasBoot;
    auto    BOOL            bOK = TRUE;


    _fmemset ( byaUsedDrives , FALSE , sizeof ( byaUsedDrives ) ) ;

    lpScanList = MemLock (lpNoNav->lpScan->hScanList);
    if (lpScanList != NULL)
        {

        while (*lpScanList && gbContinueScan)
            {
            bOK = TRUE;
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
                printf(szScanningBoot);
                TSR_OFF;

                wVirID = VirusScanBoot((WORD) byDrive,
                                       &lpNoNav->Infected.Notes,
                                       &wResult);
                TSR_ON;

                if ( wResult == ERR_VIRSCAN_READBOOT )
                    {
                    bOK = FALSE;
                    byaUsedDrives [byDriveNum] = TRUE;
                    lpScanList = SzzFindNext ( lpScanList ) ;
                    continue;         
                    } 
                                // On a valid scan, wResult is set to 0.
                else if (wResult == 0)
                    {
                                        // Only update progress bar and used
                                        // drive flags on success
                    byaUsedDrives [byDriveNum] = TRUE;

                    lpNoNav->Stats.BootRecs.uScanned++;

                    if (wVirID)
                        {

                        bOK = FALSE;
                        printf(szBootInfected, byDrive,
                                 lpNoNav->Infected.Notes.lpVirName);
                        if ( bAutoExportLog )
                           fprintf(fLogFile, szBootInfected, byDrive,
                                   lpNoNav->Infected.Notes.lpVirName);


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

                bOK = FALSE;
                                        // If drive doesn't have boot, mark
                                        // as already processed.

                                        // Also, PRETEND we scanned it!?
                                        // STS #58506
                byaUsedDrives [byDriveNum] = TRUE;

                //  lpNoNav->Stats.BootRecs.uScanned++;

                }

            if (bOK == TRUE)
                    printf(szOK);

                                // Advance to the next item to scan.
nextDrive:
            lpScanList = SzzFindNext ( lpScanList ) ;
            }

        MemUnlock(lpNoNav->lpScan->hScanList, lpScanList );
        }


} // End ScanBootRecords()


