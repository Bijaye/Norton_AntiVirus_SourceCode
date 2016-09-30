// DetectMedia.cpp: implementation of the CDetectMedia class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DetectMedia.h"
#include <process.h>            // threading
#include "AutoProtectWrapper.h"

/////////////////////////////////////////////////////////////////////////////
// CDetectMedia::IsMediaPresent()

bool CDetectMedia::IsMediaPresent( char chDriveLetter, bool bLongTimeOut )
{
    CCTRACEI ( _T("CDetectMedia::IsMediaPresent - %c"), chDriveLetter);

    bool     bPresent = false;
    HANDLE   hThread;
    unsigned uThread;

	for( int i = 0; i < 2; i++ )
	{
		// Begin separate thread to ping
		// drive.
		hThread = (HANDLE) _beginthreadex( NULL, 0, CDetectMedia::isMediaPresentProc, (void *)chDriveLetter, 0, &uThread );
    
		if ( NULL != hThread )
		{
            ccLib::CMessageLock msgLock ( TRUE, TRUE );
            
			if ( WAIT_TIMEOUT == msgLock.Lock ( hThread, bLongTimeOut ? 40000 : 3000 ))
			{
				CCTRACEE( _T("Timed out waiting to access drive %c"), chDriveLetter );
				TerminateThread( hThread, 0 );
				bPresent = false;
			}
			else 
			{
				// The problem being addressed is that in some situations, NT is 
				// doing a lot of boot record access on a CDROM drive where there 
				// is no CD present.  That makes its way up to us and we try 
				// repeatedly to scan a drive that has no media.  This chews up the
				// CPU and give the impression that we are hung when we are not.
				// 
				// With this change, we will only try to scan drives when media is 
				// present.  Perfect.  Defect #345496(I think) & 345642
				DWORD dwExitCode;
				GetExitCodeThread( hThread, &dwExitCode );
				bPresent = dwExitCode != 0;
			}
        
			CloseHandle( hThread );
		}

		// If we didn't detect media, try again. Try checking the drive again 
		// -- this works around a bug in NT where no alert is generated if 
		// the floppy drive hasn't been accessed in your current session, and you 
		// put in an infected floppy and then start/stop the service.  The first
		// CheckForDrive() is returning FALSE for some reason.
		if( bPresent == true ) 
			break;
	}
    
    return bPresent;
}

/////////////////////////////////////////////////////////////////////////////
// CDetectMedia::isMediaPresentProc()

unsigned __stdcall CDetectMedia::isMediaPresentProc( void *pvDrive )
{
    auto  char    szDrive [] = _T("X:\\");
    szDrive[0] = (TCHAR) pvDrive;

    ::SetErrorMode ( SEM_FAILCRITICALERRORS );
    CAutoProtectWrapper APWrapper;

    if ( !APWrapper.DisableAPForThisThread())
        return false;
    
	// Ping the drive.
	BOOL bReturn = GetVolumeInformation( szDrive, NULL, 0, NULL, NULL, NULL, NULL, 0 );
	CCTRACEI( _T("Media present in drive %s: %s"), szDrive, bReturn ? _T("TRUE") : _T("FALSE") );
    
    APWrapper.EnableAPForThisThread();

	// Save the state in the thread exit code.
	_endthreadex( static_cast<unsigned>( bReturn ) );
	return bReturn;
}

