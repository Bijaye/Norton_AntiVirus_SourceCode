// QuarantineThread.cpp: implementation of the CQuarantineThread class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "QuarantineThread.h"
#include "const.h"

#include "iquaran.h"

#include "QuarantineDllLoader.h"

using namespace std;

//////////////////////////////////////////////////////////////////////
// CQuarantineThread::CQuarantineThread()

CQuarantineThread::CQuarantineThread()
{
	// Create event.
	m_eventIncoming.Create( NULL, FALSE, NULL, NULL, FALSE );
	if( m_eventIncoming == NULL )
		throw runtime_error( "Could not create event." );

	// Get location of incoming folder.
	CRegKey reg;
	DWORD dwSize = _MAX_PATH;
	TCHAR szBuffer[ _MAX_PATH ];
	if( ERROR_SUCCESS == reg.Open( HKEY_LOCAL_MACHINE, REGKEY_QUARANTINE ) )
	{
		if( ERROR_SUCCESS == reg.QueryStringValue( REGVAL_INCOMINGDIR, szBuffer, &dwSize ) )
		{
			m_sIncomingDir = szBuffer;
			m_sIncomingDir += _T("\\*.*");
		}
		else
			throw runtime_error( "Could not read incoming regval." );
	}
}

//////////////////////////////////////////////////////////////////////
// CQuarantineThread::~CQuarantineThread()

CQuarantineThread::~CQuarantineThread()
{
}

//////////////////////////////////////////////////////////////////////
// CQuarantineThread::Run()

int CQuarantineThread::Run()
{
    // Set up waiting objects.
	HANDLE aHandles[2] = { m_Terminating.GetHandle(), m_eventIncoming };

	for(;;)
	{
        // Q: Do we have work to do?
	    if( itemsToProcess() )
		    processItems();

        // Wait here for something to happen
		DWORD dwWait = WaitForMultipleObjects( 2, aHandles, FALSE, INFINITE );
		
		// Check for error.
		if( dwWait == WAIT_FAILED )
		{
			break;
		} 
		else if( dwWait - WAIT_OBJECT_0 == 0 )
		{
			// exit signaled.
			break;
		}
	}
	return 0;
}

void CQuarantineThread::processItems ()
{
	// Process the incomming items.
	QuarDLL_Loader	QuarDllLoader;
	try
	{
        // Create the quarantine server object.
		CSymPtr<IQuarantineDLL> pQuarantine;

        if(SYM_SUCCEEDED(QuarDllLoader.CreateObject(&pQuarantine)) &&
           pQuarantine.m_p != NULL )
		{
			// Process the quarantine stuff.
			if( SUCCEEDED( pQuarantine->Initialize() ) )
				pQuarantine->ProcessAllIncomingFiles();
            else
                CCTRACEE ("CQuarantineThread::processItems () - failed to Initalize() quarantine");
		}
		else
		{
            CCTRACEE ( _T("CQuarantineThread::processItems () - Quarantine thread - couldn't get quarantine dll object"));
            _Module.LogEvent( "Quarantine thread - Could not quarantine item." );
		}
	}
	catch(...)
	{
        CCTRACEE ( _T("CQuarantineThread::processItems () - Unhandled exception."));
	}
}

//////////////////////////////////////////////////////////////////////
// CQuarantineThread::Signal()

void CQuarantineThread::Signal()
{
	// Set our event.
	m_eventIncoming.SetEvent();
}

//////////////////////////////////////////////////////////////////////
// CQuarantineThread::itemsToProcess()

bool CQuarantineThread::itemsToProcess()
{
	// The plan here is to see if there are any files in the incomming
	// directory.  
	int iFileCount = 0;
	WIN32_FIND_DATA findData;
	HANDLE hFind = FindFirstFile( m_sIncomingDir.c_str(), &findData );
	if( hFind != INVALID_HANDLE_VALUE )
	{
		// Loop until we find at least 1 file.
		while( iFileCount == 0 && FindNextFile( hFind, &findData ) )
		{
			// Skip directories.
			if( findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
				continue;

			// Looks like we found a file.
			++iFileCount;
		}

		// Cleanup
		FindClose( hFind );
	}

	// No files
	return iFileCount != 0;
}
