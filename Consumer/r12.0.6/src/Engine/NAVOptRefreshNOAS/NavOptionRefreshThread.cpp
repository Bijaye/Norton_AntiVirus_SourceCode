// NavOptionRefreshThread.cpp: implementation of the CNavOptionRefreshThread class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "NavOptionRefreshThread.h"
#define _NAVOPTREFRESH_CONSTANTS
#define _NAVOPTREFRESH_NOINTERFACES
#include "NAVOptRefresh.h"
#include "NAVOptionRefresh.h"

// Pulled from SymSetup.h . I couldn't use the header because it includes more
// classes and other headers that we don't need and can't compile with.
//
#define SYMSETUP_MUTEX _T("SYMSETUP_MUTEX")

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNavOptionRefreshThread::CNavOptionRefreshThread()
{

}

CNavOptionRefreshThread::~CNavOptionRefreshThread()
{

}

STDMETHODIMP CNavOptionRefreshThread::RunInstance()
{
	HRESULT hr = S_OK;
	StahlSoft::HRX hrx;
	try
	{
		hrx << ::CoInitialize(NULL);
		hr = _IT_InternalRunInstance();
		::CoUninitialize();
	}
	catch(_com_error& e)
	{
		hr = e.Error();
		CCTRACEE(_T("CNavOptionRefreshThread::RunInstance - CoInitialize failed %08X"), hr);
	}
	return hr;
}

HRESULT CNavOptionRefreshThread::_IT_InternalRunInstance()
{
#define _INITIAL_WAIT       (3*60*1000)
#define _SUBSEQUENT_WAIT    (6*60*60*1000)

	HRESULT hr = S_OK;
	StahlSoft::HRX hrx;
	bool bFiredOnThreadReady = false;
	try
	{
		CNAVOptionRefresh nor;

		m_shStop            = ::CreateEvent(NULL,FALSE,FALSE,NULL);
		m_shOnDemandRefresh = ::CreateEvent(NULL,FALSE,FALSE,szNAVOptionRefreshEngine_OnDemandRefreshEvent);
		m_shLicenseChangeEvent = clt::CreateLicenseChangeEvent();

		HANDLE hWait[3] = {m_shStop,m_shOnDemandRefresh, m_shLicenseChangeEvent};
		Fire_OnThreadReady(this);
		bFiredOnThreadReady = true;

		DWORD dwTimeOut = _INITIAL_WAIT;
		bool bForce = true;

		bool bRun = true;
		while(bRun)
		{
			DWORD dwWait = StahlSoft::WaitForMultipleObjectsWithMessageLoop( (sizeof(hWait)/sizeof(HANDLE))
				,hWait,dwTimeOut );
            
			switch(dwWait)
			{
			default:
				// Catastrophic
			case WAIT_OBJECT_0:			// On stop event
				bRun = false;
				break;
			case WAIT_TIMEOUT:
			case WAIT_OBJECT_0+1:		// On demand refresh event
			case WAIT_OBJECT_0+2:		// On license change event
                // Don't refresh if we are uninstalling!
                //
                if ( !isUninstalling() )
                {
                    CCTRACEI ("NavOptionRefreshThread::_IT_InternalRunInstance - refreshing reason:%d force:%d", dwWait, bForce);
                    Fire_OnEvent(NORTE_ForceRefresh);
				    nor.Refresh(bForce?REFRESH_FORCE_FULLY:REFRESH_COMMIT_ON_CHANGE_FULLY);
				    bForce = false;
                    dwTimeOut = _SUBSEQUENT_WAIT;
                }
				break;
			}
		}
	}
	catch(_com_error& e)
	{
		hr = e.Error();
		CCTRACEE(_T("CNavOptionRefreshThread::_IT_InternalRunInstance - Exception %08X"), hr);
	}

	if(!bFiredOnThreadReady)
		Fire_OnThreadReady(this);

	return hr;
}			


STDMETHODIMP CNavOptionRefreshThread::RequestExit()
{
	HRESULT hr = S_OK;
	StahlSoft::HRX hrx;
	try
	{
		if(m_shStop != NULL)
			::SetEvent(m_shStop);
	}
	catch(_com_error& e)
	{
		hr = e.Error();
	}
	return hr;
}		

// Checks to see if there is a Symantec product uninstalling.
// Unfortunately we can't tell if it's only NAV, just any Symantec
// using SymSetup.
//
bool CNavOptionRefreshThread::isUninstalling ()
{
    // Check to see if we are uninstalling. No point in writing data if we are.
    //
    ccLib::CMutex mutexUninstalling;

    if ( mutexUninstalling.Open ( SYNCHRONIZE, FALSE, SYMSETUP_MUTEX, TRUE))
	{
        CCTRACEI ("CNavOptionRefreshThread::IsUninstalling - we are uninstalling");
        return true;
    }
    
    return false;
}
