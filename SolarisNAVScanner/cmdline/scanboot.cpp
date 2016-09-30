// Copyright 1996 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/CMDLINE/VCS/scanboot.cpv   1.5   02 Jul 1996 12:33:46   RAY  $
//
// Description:
//  Functions used for scanning all Boot sectors and MBRs.
//
// Contains:
//  CMDLineScanBoot()
//
// See Also:
//
//************************************************************************
// $Log:   S:/CMDLINE/VCS/scanboot.cpv  $
// 
//    Rev 1.5   02 Jul 1996 12:33:46   RAY
// Added code for /NOBOOT and /NOPART switches
// 
//    Rev 1.4   17 Jun 1996 18:31:28   RAY
// Moved Boot init and close into CMDLineScanBootArea() in scanboot.cpp
// 
//    Rev 1.3   17 Jun 1996 15:10:20   RAY
// Various bug fixes to Boot Repair stuff
// 
//    Rev 1.2   12 Jun 1996 20:30:16   RAY
// added linear read/write retries and code to zero out sector buffers
// 
//************************************************************************

#include "memory.h"
#include "scanboot.h"
#include "clcb.h"
#include "report.h"
#include "msgstr.h"

//*************************************************************************
//
// Function:
//  ScanBoot()
//
// Parameters:
//  lpScanGlobal    Pointer to global data
//  lprLinear       Linear object to scan
//  bPartNumber     Partition of lprLinear to scan
//  lpbVirusFound   Pointer to BOOL to store virus found status
//  lphVirus        Pointer to HVIRUS to store virus handle if virus found
//
// Description:
//  Scans a Boot sector for viruses by calling EngScanBoot().
//  If the specified partition number is unused, returns TRUE and sets
//  lpbVirusFound to FALSE.
//
// Returns:
//  FALSE           On error
//  TRUE            On success
//
//*************************************************************************

BOOL ScanBoot
(
    LPSCANGLOBAL        lpScanGlobal,
    LPLINEAROBJECT      lprLinear,
    BYTE                byPartNumber,
    LPBOOL              lpbVirusFound,
    LPHVIRUS            lphVirus
)
{
    BYTE        abySector [ SECTOR_SIZE ];
    DWORD       dwActual;
    BOOL        bResult = TRUE;
    BOOL        bPartitioned;
    PARTENTRY   rPartEntry;
    DWORD       dwLinearLow;

    *lphVirus = NULL;
    *lpbVirusFound = FALSE;

    lprLinear->lpCallBacks->LinearIsPartitionedMedia ( lprLinear->lpvInfo, &bPartitioned );

    if ( bPartitioned )
        {

                                            // ------------
                                            // Read the MBR
                                            // ------------
        if ( lprLinear->lpCallBacks->LinearRead ( lprLinear->lpvInfo,
                                                  0,
                                                  0,
                                                  abySector,
                                                  1,
                                                  &dwActual ) != CBSTATUS_OK )
            {
            bResult = FALSE;
            }

        if ( TRUE == bResult && dwActual != 1 )
            {
            bResult = FALSE;
            }

        if ( TRUE == bResult )
            {
                                            // --------------------------
                                            // Parse MBR for byPartNumber
                                            // --------------------------
            EngParseMBRPartition ( abySector,
                                   byPartNumber,
                                   &rPartEntry );

            if ( PARTFS_NOT_USED != rPartEntry.byFileSystem )
                {
                                            // ------------------------------
                                            // Convert physical loc to linear
                                            // ------------------------------
                lprLinear->lpCallBacks->LinearConvertFromPhysical ( lprLinear->lpvInfo,
                                                                    rPartEntry.byStartSide,
                                                                    rPartEntry.wStartCylinder,
                                                                    rPartEntry.byStartSector,
                                                                    &dwLinearLow );

                                            // --------------------
                                            // Read the Boot Sector
                                            // --------------------
                if ( lprLinear->lpCallBacks->LinearRead ( lprLinear->lpvInfo,
                                                          dwLinearLow,
                                                          0,
                                                          abySector,
                                                          1,
                                                          &dwActual ) != CBSTATUS_OK )
                    {
                    bResult = FALSE;
                    }

                if ( TRUE == bResult && dwActual != 1 )
                    {
                    bResult = FALSE;
                    }

                if ( TRUE == bResult )
                    {
                                            // --------------------
                                            // Scan the Boot Sector
                                            // --------------------
                    if ( EngScanBoot ( lpScanGlobal->hLEng,
                                       abySector,
                                       SECTOR_SIZE,
                                       lpbVirusFound,
                                       lphVirus ) != ENGSTATUS_OK )
                        {
                        bResult = FALSE;
                        }
                    }
                }
            }
        }
    else
        {
                                    // --------------------
                                    // Read the Boot Sector
                                    // --------------------
        if ( lprLinear->lpCallBacks->LinearRead ( lprLinear->lpvInfo,
                                                  0,
                                                  0,
                                                  abySector,
                                                  1,
                                                  &dwActual ) != CBSTATUS_OK )
            {
            bResult = FALSE;
            }

        if ( TRUE == bResult && dwActual != 1 )
            {
            bResult = FALSE;
            }

        if ( TRUE == bResult )
            {
                                    // --------------------
                                    // Scan the Boot Sector
                                    // --------------------
            if ( EngScanBoot ( lpScanGlobal->hLEng,
                               abySector,
                               SECTOR_SIZE,
                               lpbVirusFound,
                               lphVirus ) != ENGSTATUS_OK )
                {
                bResult = FALSE;
                }
            }
        }
                                        // Remove virus' shadow from memory
    memset ( abySector, 0, SECTOR_SIZE );

    return ( bResult );
}


//*************************************************************************
//
// Function:
//  ScanMBR()
//
// Parameters:
//  lpScanGlobal    Pointer to global data
//  lprLinear       Linear object to scan
//  lpbVirusFound   Pointer to BOOL to store virus found status
//  lphVirus        Pointer to HVIRUS to store virus handle if virus found
//
// Description:
//  Scans an MBR on lprLinear by calling EngScanBoot().
//
// Returns:
//  FALSE           On error
//  TRUE            On success
//
//*************************************************************************

BOOL ScanMBR
(
    LPSCANGLOBAL        lpScanGlobal,
    LPLINEAROBJECT      lprLinear,
    LPBOOL              lpbVirusFound,
    LPHVIRUS            lphVirus
)
{
    BYTE        abySector [ SECTOR_SIZE ];
    DWORD       dwActual;
    BOOL        bResult = TRUE;
    BOOL        bPartitioned;

    *lphVirus = NULL;
    *lpbVirusFound = FALSE;

    lprLinear->lpCallBacks->LinearIsPartitionedMedia ( lprLinear->lpvInfo, &bPartitioned );

    if ( !bPartitioned )
        {
        return ( TRUE );
        }

                                        // ------------
                                        // Read the MBR
                                        // ------------
    if ( lprLinear->lpCallBacks->LinearRead ( lprLinear->lpvInfo,
                                              0,
                                              0,
                                              abySector,
                                              1,
                                              &dwActual ) != CBSTATUS_OK )
        {
        bResult = FALSE;
        }

    if ( TRUE == bResult && dwActual != 1 )
        {
        bResult = FALSE;
        }

    if ( TRUE == bResult )
        {
                                        // ------------
                                        // Scan the MBR
                                        // ------------
        if ( ENGSTATUS_OK != EngScanBoot ( lpScanGlobal->hLEng,
                                           abySector,
                                           SECTOR_SIZE,
                                           lpbVirusFound,
                                           lphVirus ))
            {
            bResult = FALSE;
            }
        }

                                        // Clear bytes so virus is not
                                        // detected in memory!
    memset ( abySector, 0, SECTOR_SIZE );

    return ( bResult );
}


//*************************************************************************
//
// Function:
//  RepairBoot()
//
// Parameters:
//  lpScanGlobal    Pointer to global data
//  lprLinear       Linear object to repair
//  byPartNumber    Partition to repair
//  hVirus          The virus handle of the detected virus
//  lpbRepaired     Was the virus repaired?
//
// Description:
//  Repairs Boot Sector by calling EngRepairBoot().
//
// Returns:
//  FALSE           On error
//  TRUE            On success
//
//*************************************************************************

BOOL RepairBoot
(
    LPSCANGLOBAL    lpScanGlobal,
    LPLINEAROBJECT  lprLinear,
    BYTE            byPartNumber,
    HVIRUS          hVirus,
    LPBOOL          lpbRepaired
)
{
    return ( (BOOL) EngRepairBoot ( lpScanGlobal->hLEng,
                                    hVirus,
                                    lprLinear,
                                    byPartNumber,
                                    lpbRepaired ));
}

//*************************************************************************
//
// Function:
//  RepairMBR()
//
// Parameters:
//  lpScanGlobal    Pointer to global data
//  lprLinear       Linear object to repair
//  hVirus          The virus handle of the detected virus
//  lpbRepaired     Was the virus Repaired?
//
// Description:
//  Repairs MBR by calling EngRepairMBR().
//
// Returns:
//  FALSE           On error
//  TRUE            On success
//
//*************************************************************************

BOOL RepairMBR
(
    LPSCANGLOBAL    lpScanGlobal,
    LPLINEAROBJECT  lprLinear,
    HVIRUS          hVirus,
    LPBOOL          lpbRepaired
)
{
    return ( (BOOL) EngRepairBoot ( lpScanGlobal->hLEng,
                                    hVirus,
                                    lprLinear,
                                    0,
                                    lpbRepaired ));
}

//*************************************************************************
//
// Function:
//  GetSystemDrives()
//
// Parameters:
//  lpbyFloppies        BYTE to recieve number of floppy drives
//  lpbyHardDrives      BYTE to recieve number of hard drives
//
// Description:
//  returns 0 for both in Win32
//
//*************************************************************************
VOID GetSystemDrives ( LPBYTE lpbyFloppies, LPBYTE lpbyHardDrives )
{
    BYTE byFloppies = 0;
    BYTE byHardDrives = 0;

#if defined (SYM_DOS) || defined(BORLAND) || defined (SYM_DOSX)

    BYTE byStatus;

    _asm {
        push    si
        push    di
        mov     ah, 08h
        mov     dl, 00h
        int     13h
        mov     byStatus, ah
        mov     byFloppies, dl
        pop     di
        pop     si
        }

    if ( byStatus != 0 ) byFloppies = 0;

    _asm {
        push    si
        push    di
        mov     ah, 08h
        mov     dl, 80h
        int     13h
        mov     byStatus, ah
        mov     byHardDrives, dl
        pop     di
        pop     si
        }

    if ( byStatus != 0 ) byHardDrives = 0;

#endif // SYM_DOS || BORLAND || SYM_DOSX

    *lpbyFloppies = byFloppies;
    *lpbyHardDrives = byHardDrives;
}

//*************************************************************************
//
// Function:
//  ParseDriveNum()
//
// Parameters:
//  lpszPath        Path string to parse
//
// Description:
//  Checks lpszPath for drive letter.
//
// Returns:
//  Drive number for local drive (0x00, 0x01, 0x80, 0x81)
//  INVALID_DRIVE_NUM for other drives
//
//*************************************************************************

BYTE ParseDriveNum
(
    LPSTR           lpszPath,
    LPSCANGLOBAL    lpScanGlobal,
    BYTE            byFloppies,
    BYTE            byHardDrives
)
{
    BYTE byDriveNum = INVALID_DRIVE_NUM;
    int nDriveLetter;


    if (( lpszPath [ 0 ] == '\\' ) &&
        ( lpszPath [ 1 ] == '\\' ))
        {
        byDriveNum = INVALID_DRIVE_NUM;
        }
    else if (( lpszPath [ 0 ] == '\0' ) ||
             ( lpszPath [ 1 ] != ':' ))
        {
        byDriveNum = ParseDriveNum ( lpScanGlobal->szInitialDir,
                                     lpScanGlobal,
                                     byFloppies,
                                     byHardDrives );
        }
    else
        {
        nDriveLetter = lpszPath [ 0 ];
        if ( nDriveLetter > 0x60 ) nDriveLetter -= 0x20;
        byDriveNum = nDriveLetter - 'A';
        if ( byDriveNum > 1 )
            {
            byDriveNum += 0x7E;
            if ( byDriveNum > 0x80 + byHardDrives )
                {
                byDriveNum = INVALID_DRIVE_NUM;
                }
            }
        }
    return ( byDriveNum );
}

//*************************************************************************
//
// Function:
//  CMDLineScanBoot()
//
// Parameters:
//  lpScanGlobal    Pointer to global data
//
// Description:
//  Scans all boot sectors on specified physical drives
//
// Returns:
//  FALSE           On error
//  TRUE            On success
//
//*************************************************************************

BOOL ScanFloppyBootArea
(
    LPSCANGLOBAL    lpScanGlobal,
    LPLINEAROBJECT  lprLinear
)
{
    BOOL            bResult = TRUE;
    BOOL            bVirusFound;
    BOOL            bRepaired = FALSE;
    HVIRUS          hVirus;
    BYTE            byDriveNum = (BYTE) lprLinear->lpvInfo;

    bResult = ScanBoot ( lpScanGlobal,
                         lprLinear,
                         0,
                         &bVirusFound,
                         &hVirus );
    if ( !bResult )
        {
        ReportBootError(lpScanGlobal,
                        byDriveNum,
                        0,
                        REPORT_FLOPPY | REPORT_ERROR_SCAN );
        }
    else if ( bVirusFound )
        {
        if ( lpScanGlobal->lpCmdOptions->dwFlags &
             OPTION_FLAG_REPAIR )
            {
            bResult = RepairBoot ( lpScanGlobal,
                                   lprLinear,
                                   0,
                                   hVirus,
                                   &bRepaired );
            if ( bRepaired )
                {
                ReportBootVirusAndAction ( byDriveNum,
                                           0,
                                           lpScanGlobal,
                                           hVirus,
                                           ACTION_FLOPPY | ACTION_REPAIRED );
                }
            else
                {
                bResult = FALSE;
                ReportBootVirusAndAction ( byDriveNum,
                                           0,
                                           lpScanGlobal,
                                           hVirus,
                                           ACTION_FLOPPY | ACTION_REPAIR_FAILED );
                }

            }
        else
            {
            ReportBootVirusAndAction ( byDriveNum,
                                       0,
                                       lpScanGlobal,
                                       hVirus,
                                       ACTION_FLOPPY | ACTION_NO_ACTION );
            }
        }
    else
        {
        // report clean ?
        }

    return ( bResult );
}


//*************************************************************************
//
// Function:
//  ScanHardDriveBootArea
//
// Parameters:
//  lpScanGlobal    Pointer to global data
//
// Description:
//  Scans MBR on specified physical drives
//
// Returns:
//  FALSE           On error
//  TRUE            On success
//
//*************************************************************************

BOOL ScanHardDriveBootArea
(
    LPSCANGLOBAL    lpScanGlobal,
    LPLINEAROBJECT  lprLinear
)
{
    BOOL            bResult = TRUE;
    BOOL            bVirusFound;
    BOOL            bRepaired = FALSE;
    HVIRUS          hVirus;
    BYTE            byPartNumber;
    BYTE            byDriveNum = (BYTE) lprLinear->lpvInfo;

    if (( lpScanGlobal->lpCmdOptions->dwFlags & OPTION_FLAG_NOPART ) == 0)
        {
        bResult = ScanMBR ( lpScanGlobal,
                            lprLinear,
                            &bVirusFound,
                            &hVirus );

        if ( !bResult )
            {
            ReportBootError(lpScanGlobal,
                            byDriveNum,
                            0,
                            REPORT_MBR | REPORT_ERROR_SCAN );
            }
        else if ( bVirusFound )
            {
            if ( lpScanGlobal->lpCmdOptions->dwFlags &
                 OPTION_FLAG_REPAIR )
                {
                bResult = RepairMBR ( lpScanGlobal,
                                      lprLinear,
                                      hVirus,
                                      &bRepaired );
                if ( bRepaired )
                    {
                    ReportBootVirusAndAction ( byDriveNum,
                                               0,
                                               lpScanGlobal,
                                               hVirus,
                                               ACTION_MBR | ACTION_REPAIRED );
                    }
                else
                    {
                    bResult = FALSE;
                    ReportBootVirusAndAction ( byDriveNum,
                                               0,
                                               lpScanGlobal,
                                               hVirus,
                                               ACTION_MBR | ACTION_REPAIR_FAILED );
                    }

                }
            else
                {
                ReportBootVirusAndAction ( byDriveNum,
                                           0,
                                           lpScanGlobal,
                                           hVirus,
                                           ACTION_MBR | ACTION_NO_ACTION );
                }
            }
        else
            {
            // report clean ?
            }
        }

    if (( lpScanGlobal->lpCmdOptions->dwFlags & OPTION_FLAG_NOBOOT ) == 0)
        {
        for ( byPartNumber = 0; byPartNumber < MAX_PARTITIONS; ++byPartNumber )
            {
            bResult = ScanBoot ( lpScanGlobal,
                                 lprLinear,
                                 byPartNumber,
                                 &bVirusFound,
                                 &hVirus );
            if ( !bResult )
                {
                ReportBootError(lpScanGlobal,
                                byDriveNum,
                                byPartNumber,
                                REPORT_BOOT | REPORT_ERROR_SCAN );
                }
            else if ( bVirusFound )
                {
                if ( lpScanGlobal->lpCmdOptions->dwFlags &
                     OPTION_FLAG_REPAIR )
                    {
                    bResult = RepairBoot ( lpScanGlobal,
                                           lprLinear,
                                           byPartNumber,
                                           hVirus,
                                           &bRepaired );
                    if ( bRepaired )
                        {
                        ReportBootVirusAndAction ( byDriveNum,
                                                   byPartNumber,
                                                   lpScanGlobal,
                                                   hVirus,
                                                   ACTION_BOOT | ACTION_REPAIRED );
                        }
                    else
                        {
                        bResult = FALSE;
                        ReportBootVirusAndAction ( byDriveNum,
                                                   byPartNumber,
                                                   lpScanGlobal,
                                                   hVirus,
                                                   ACTION_BOOT | ACTION_REPAIR_FAILED );
                        }

                    }
                else
                    {
                    ReportBootVirusAndAction ( byDriveNum,
                                               byPartNumber,
                                               lpScanGlobal,
                                               hVirus,
                                               ACTION_BOOT | ACTION_NO_ACTION );
                    }
                }
            else
                {
                // report clean ?
                }
            }
        }
    return ( bResult );
}


//*************************************************************************
//
// Function:
//  CMDLineScanBootArea()
//
// Parameters:
//  lpScanGlobal    Pointer to global data
//
// Description:
//  Calls MBR and Boot scanning routines if necessary
//
// Returns:
//  FALSE           On error
//  TRUE            On success
//
//*************************************************************************

BOOL CMDLineScanBootArea
(
    LPSCANGLOBAL    lpScanGlobal
)
{
    BOOL            bResult = TRUE;
    LINEAROBJECT    rLinear;
    BOOL            bRepaired = FALSE;
    BYTE            byDriveNum;
    BYTE            byDrivesToScan [ 256 ];
    int             nCount;
    int             nDrivesToScan = 0;
    LPSCAN_NODE     lpScanNode;
    BYTE            byFloppies;
    BYTE            byHardDrives;

    // Boot init
    if ( EngGlobalBootInit ( lpScanGlobal->hGEng ) != ENGSTATUS_OK )
        {
        ReportError(lpScanGlobal,
                    gszMsgStrScanBootInit,
                    REPORT_ERROR_INIT);
        return ( FALSE );
        }

    rLinear.lpCallBacks = &gstCLCBLinearCallBacks;

    GetSystemDrives ( &byFloppies, &byHardDrives );

    for ( nCount = 0; nCount < 256; ++nCount )
        {
        byDrivesToScan [ nCount ] = INVALID_DRIVE_NUM;
        }

    lpScanNode = lpScanGlobal->lpCmdOptions->lpScanList;


    while (lpScanNode != NULL)
        {
        byDriveNum = ParseDriveNum ( lpScanNode->lpszScanThis,
                                     lpScanGlobal,
                                     byFloppies,
                                     byHardDrives );

                                        // valid drives
        if ( byDriveNum < 0x80 + byHardDrives )
            {
                                        // remove this line after adding
                                        // proper Logical to Physical
                                        // drive mapping
            if ( byDriveNum > 0x80 ) byDriveNum = 0x80;

            byDrivesToScan [ byDriveNum ] = byDriveNum;
            }

        lpScanNode = lpScanNode->lpNext;
        }

    for ( nCount = 0; nCount < 256; ++nCount )
        {
        if ( INVALID_DRIVE_NUM == byDrivesToScan [ nCount ])
            {
            continue;
            }

        if (( byDrivesToScan [ nCount ] < byFloppies ) &&
            ( lpScanGlobal->lpCmdOptions->dwFlags & OPTION_FLAG_NOBOOT ) == 0)

            {
                                        // valid floppy drive
            printf ( "Scanning Floppy Boot Sector...\n" );
            rLinear.lpvInfo = (LPVOID) byDrivesToScan [ nCount ];
            bResult = ScanFloppyBootArea ( lpScanGlobal, &rLinear );
            }

        else if ( byDrivesToScan [ nCount ] == 0x80 )
            {
                                        // valid hard drive
            byDriveNum = byDrivesToScan [ nCount ];
            printf ( "Scanning Hard Drive MBRs and Boot Sectors...\n" );
            while ( byDriveNum < 0x80 + byHardDrives )
                {
                rLinear.lpvInfo = (LPVOID) byDriveNum++;
                bResult = ScanHardDriveBootArea ( lpScanGlobal, &rLinear );
                }
            }

        if ( FALSE == bResult ) break;
        }

    // Boot close
    if ( EngGlobalBootClose ( lpScanGlobal->hGEng ) != ENGSTATUS_OK )
        {
        ReportError(lpScanGlobal,
                    gszMsgStrScanBootClose,
                    REPORT_ERROR_CLOSE);
        bResult = FALSE;
        }

    return ( bResult );
}

