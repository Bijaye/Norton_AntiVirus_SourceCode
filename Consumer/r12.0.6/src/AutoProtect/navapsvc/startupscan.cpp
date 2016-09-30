/////////////////////////////////////////////////////////////////////////////
// startupscan.cpp
//
// This file contains code to perform startup scans.
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#define INITIIDS
#include "AvEvents.h"
#include "Scanner.h"
#include "NAVTrust.h"
#include "QuarantineDllLoader.h"
#include "NAVSettingsHelperEx.h"
#include "iquaran.h"
#include "NAVDefutilsLoader.h"
#include "ExclusionManagerInterface.h"

#include "navapsvc.h"
#include "scanqueue.h"
#include "DetectMedia.h"

using namespace std;

/////////////////////////////////////////////////////////////////////////////
// Local routines

#define	FIRST_FIXED_DISK		0x80

/////////////////////////////////////////////////////////////////////////////
// CServiceModule::doStartupScan()

void CServiceModule::DoStartupScan()
{
	TCHAR  szDrive[] = _T("A:\\");

	// Make sure we have work to do.
	if( Config().GetStartupScan() == false )
		return;

    // If the scan queue failed to create, exit.
    if ( !m_pScanQueue )
    {
        if (!CreateScanQueue())
            return;
    }

	// Vector of items to scan.
	vector< CScanQueueItem > vItems;

	// Check to see if MBR scanning is enabled.
	if( Config().GetScanMasterBootRecords() )
	{
		// Scan the MBRs
		m_pScanQueue->AddItem( CScanQueueItem( FIRST_FIXED_DISK ) ); 
	}

	// Check to see if boot record scanning is enabled.

	// Examine all drives, and add them to vector as appropriate.
	DWORD dwDriveBit = 1;
	DWORD dwDrives = GetLogicalDrives();
	for( TCHAR chDrive = _T('A'); chDrive <= _T('Z'); ++chDrive, dwDriveBit <<= 1 )
	{
		// Make sure we have a drive.
		if( (dwDrives & dwDriveBit) == 0 )
			continue;
		
		szDrive[0] = chDrive;

		// Get drive information
        UINT uDriveType = GetDriveType(szDrive);

        // Q: Is this a network drive?
		if( DRIVE_REMOTE == uDriveType ||
            DRIVE_UNKNOWN == uDriveType ||
            DRIVE_NO_ROOT_DIR == uDriveType )
		{
			// Yes.  Don't scan it.
			continue;
		}
		
		// Q: Is there media in the drive?
		if( CDetectMedia::IsMediaPresent( chDrive, true ) == false )
		{
			// Nope, keep on processing.
			continue;
		}

		// Add drive to list.
		vItems.push_back( CScanQueueItem( (BYTE) chDrive ) );
	}

	// Submit these items for scanning.
	m_pScanQueue->AddItems( vItems );
}