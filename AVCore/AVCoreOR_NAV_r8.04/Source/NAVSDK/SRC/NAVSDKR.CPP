// Copyright 1995 Symantec, Peter Norton Product Group
//***************************************************************************
//
// $Header:   S:/NAVSDK/VCS/Navsdkr.cpv   1.2   10 Feb 1998 16:31:44   PVENKAT  $
//
// Description:
//
//      This file contain the repair functions for NAVOEM.
//
// Contains:
//      VirusLibraryRepairFile()
//
//***************************************************************************

//***************************************************************************
// Include Files
//***************************************************************************
#include "platform.h"
#include "xapi.h"
#include "file.h"
#include "disk.h"
#include "avapi.h"
#include "navsdk.h"
#include "navsdkr.h"
#include "oemapi.h"
#include "device.h"
#include "navutil.h"

/*@API:**********************************************************************
@Declaration:
    NAVSDKAPI WORD WINAPI VirusLibraryRepairFile (
        HVIRUSSCANNER   hScannerHandle,
        LPCSTR          lpszFile )

@Description:
    This function repairs the specified file if it is infected.

@Parameters:
    $hScannerHandle$    The handle to the NAVSDK scanning context.
    $lpszFile$          Name of the file to repair.

@Returns:
    VIRLIB_OK		if the file was repaired.
    VIRLIB_INFECTED if the file could not be repaired and is still infected
    VIRLIB_ERROR	if an error is returned during the scan or repair.

@See:

@Include:

@Compatibility: DOS DX W16 W32
****************************************************************************/
NAVSDKAPI WORD WINAPI VirusLibraryRepairFile (  // Repair file
    HVIRUSSCANNER   hScannerHandle,     // [in] Scanner Handle
    LPCSTR          lpszFile )          // [in] File to scan
{
    auto    HVLVIRUS     hVirus;
    auto    LPOEMCONTEXT lpOEMContext;
    auto    VSTATUS      nStatus;

    if ( !ValidOEMContext( hScannerHandle, &lpOEMContext ) )
        return ( VIRLIB_INVALID_ARG );

    //&? Need to fix this for MT enable, disable?

    AutoProtectOff();

    nStatus = VLScanFile( lpOEMContext->hContext,
                          (LPSTR)lpszFile,
    	                  &hVirus );

    if ( ( VS_OK == nStatus ) && ( NULL != hVirus ) )
    {
        nStatus = VLRepairFile( hVirus, lpOEMContext );
        VLReleaseVirusHandle( hVirus );
    }

    lpOEMContext->vStatus = nStatus;

    AutoProtectOn();

    return ( (WORD)nStatus );
} // VirusLibraryRepairFile()



/*@API:**********************************************************************
@Declaration:
    NAVSDKAPI WORD WINAPI VirusLibraryRepairSysArea (
        HVIRUSSCANNER   hScannerHandle,
        WORD            wDriveFlags,
        BYTE            lpDrive)
        

@Description:
    This function repairs the system area of a drive to see if it is
    infected.  If it is infected the function returns VIRLIB_INFECTED
    and sets the name of the virus in lpszVirusNameBuf.

    The system area currently is defined as the MBR and boot sectors of
    all partitions on a drive.

    A single drive can be scanned by pointing lpDrive to a physical
    drive number and setting the VIRLIB_SCAN_DRIVE_FLAG in wDriveFlags.

    All hard drives can be scanned by setting the VIRLIB_FIXED_FLAG. All
    floppy drives can be scanned by setting the VIRLIB_REMOVABLE_FLAG. All
    drives on a system can be scanned by setting both flags.

    If a virus is found when one or both of these flags are set, the drive
    where the virus was found is returned in lpDrive. The partition number
    or MBR info is available by calling VirusLibraryGetLastVirusInfo.

    Logical or physical drive designators are allowed. Logical drives are the
    default. If physical drive numbers are to be used, then VIRLIB_PHYSICAL_DRIVES
    should be set in wDriveFlags. Logical drives are not allowed in NT.

@Parameters:
    $hScannerHandle$    The handle to the virus context.
    $wDriveFlags$       Drive scanning flags.
    $lpDrive$           Drive designator
    $lpszVirusNameBuf$  Buffer to receive virus name if system area infected.

@Returns:
    VIRLIB_OK              if not infected
    VIRLIB_INVALID_ARG     if no initialization done before calling this function
    VIRLIB_INFECTED        if infected and not-repaired.
    VIRLIB_NOT_ENAB        if SysArea scanning is not enabled
    VIRLIB_ERROR           if an error is returned during the scan.
    VIRLIB_NO_LOGICAL      if logical drive mapping is not supported
    VIRLIB_NOT_BOOT_DEVICE if logical drive is not part of a physical device.

@See:

@Include:
    navsdk.h

@Compatibility: W32
****************************************************************************/
NAVSDKAPI WORD WINAPI VirusLibraryRepairSysArea ( // Scans system areas for viruses
    HVIRUSSCANNER   hScannerHandle,     // [in] Scanner Handle
    WORD            wDriveFlags,        // [in] Drive scanning flags
    BYTE            byDrive )           // [in] Drive number designator
{
    auto    HVLVIRUS        hVirus = NULL;
    auto    LPOEMCONTEXT    lpOEMContext;
    auto    VSTATUS         nStatus;
    auto    char            szVirusName [VIRLIB_VIRUS_NAME_SIZE] ;
    auto    VIRUSINFO       VirInfo ;

    if ( !ValidOEMContext( hScannerHandle, &lpOEMContext ) )
    {
        return VIRLIB_INVALID_ARG ;
    }

    // See if the function is enabled.

    if ( 0 == lpOEMContext->FuncEnable.SysAreaFlags )
    {
        return VIRLIB_NOT_ENAB;
    }

    //&? Need to fix this for MT enable, disable?

    AutoProtectOff ( ) ;

    // Scan to see if infected
    nStatus = VirusLibraryScanSysArea ( hScannerHandle, 
                                        wDriveFlags,
                                        &byDrive,
                                        szVirusName ) ;

    // Attempt to repair only if infected
    if ( VIRLIB_INFECTED == nStatus ) 
    {
        nStatus = VirusLibraryGetLastVirusInfo ( hScannerHandle, &VirInfo ) ;

        if ( nStatus == VIRLIB_OK )
        {
            // To get hVirus from MBR or BR scan it again.
            if ( 0xFF == VirInfo.byInfectedPartitionNumber )
                // MBR
                nStatus = VLScanPart ( lpOEMContext -> hContext,
                                        VirInfo.byInfectedPhysicalDrive,
                                        NULL,
                                        &hVirus ) ;
            else    // BOOT Record
                nStatus = VLScanBoot ( lpOEMContext -> hContext,
                                        VirInfo.byInfectedPhysicalDrive,
                                        VirInfo.byInfectedPartitionNumber,
                                        NULL,
                                        &hVirus ) ;

            // Attempt to repair only if we have valid Virus details.
            if ( ( VIRLIB_OK == nStatus ) && ( NULL != hVirus ) ) 
            {
                nStatus = VLRepairBootPart ( hVirus, NULL ) ;

                VLReleaseVirusHandle ( hVirus ) ;
                lpOEMContext -> vStatus = nStatus ;
            }
        }
        else
            nStatus = VIRLIB_ERROR ;
    }

    AutoProtectOn ( ) ;

    return nStatus ;

} // VirusLibraryRepairSysArea()



