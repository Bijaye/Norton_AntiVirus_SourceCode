////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// AlertList.cpp: implementation of the CAlertList class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "Resource.h"
#include "ole2.h"
#include "Simon.h"
#include "SymAlertInterface.h"
#include "AlertList.h"
#include "ISVersion.h"
#include "ISymMceCmdLoader.h"	// detect optional Media center components

using namespace std;

//////////////////////////////////////////////////////////////////////
// CAlertList::CAlertList()

CAlertList::CAlertList() 
{
	// Intentionally empty.
}

//////////////////////////////////////////////////////////////////////
// CAlertList::~CAlertList()

CAlertList::~CAlertList()
{
	// Stop the thread.
    ::SetEvent(m_shStopEvent);

	// Wait here for thread to terminate.
	if( m_shThread )
	{
		// Wait for a maximum of 2 seconds.

		if(WAIT_TIMEOUT == StahlSoft::WaitForSingleObjectWithMessageLoop( m_shThread, 2000))
		{
			// Thread is stuck.  Kill it.
            TerminateThread( m_shThread, 0xFFFFFFFF );
		}

	}
}

//////////////////////////////////////////////////////////////////////
// CAlertList::Initialize()

bool CAlertList::Initialize(HWND hWndParent)
{
	bool bSuccess = false;

	STAHLSOFT_HRX_TRY(hr)
	{
		m_hWndParent = hWndParent;

		// Create event objects.
		m_shInsertion = ::CreateEvent(NULL, FALSE, FALSE, NULL);
		m_shStopEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
		hrx << ((m_shInsertion == NULL || m_shStopEvent == NULL) ? E_POINTER : S_OK);

		// Create thread object
		unsigned thrdaddr;
		m_shThread = (HANDLE) _beginthreadex( NULL, 0, &CAlertList::threadProc, this, 0, &thrdaddr);
		hrx << ((m_shThread == NULL) ? E_POINTER : S_OK);

		// Create object to display alert dialogs.
		hrx << SIMON::CreateInstanceByDLLInstance(_Module.GetModuleInstance(), CLSID_CAlertDlg, IID_IAlertDlg,(void**)&m_spAlertDlg);

		// All good.
		bSuccess = true;
	}
	STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr)
	CCTRCTXI1(L"Exit. HR: 0x%08X", hr);

	return bSuccess;
}

//////////////////////////////////////////////////////////////////////
// CAlertList::Insert()

void CAlertList::Insert( ALERT_ITEM alert)
{
    //~~--~~//~~--~~//~~--~~//~~--~~//~~--~~//~~--~~//~~--~~
    StahlSoft::CSmartCritSection smCrit(&m_critList);
    //~~--~~//~~--~~//~~--~~//~~--~~//~~--~~//~~--~~//~~--~~
	
	// Check to see if there's already an 
	//  alert of the same type in the list.
	list<ALERT_ITEM>::iterator itr;
	for(itr = m_list.begin(); itr != m_list.end(); ++itr)
	{
		if((*itr).alertMap.Type == alert.alertMap.Type)
			return;
	}

	// Add it to the list
	m_list.push_back( alert );

	// Set the insertion event.
	::SetEvent(m_shInsertion);
}

//////////////////////////////////////////////////////////////////////
// CAlertList::processAlert()

void CAlertList::processAlert(ALERT_ITEM alert)
{
	STAHLSOFT_HRX_TRY(hr)
	{
		// Save licensing alert type.
		if(alert.alertMap.Category == SYMALERT_CATEGORY_LICENSING)
		{
			_SetAlertDword(ALERT_LastLicAlert, alert.alertMap.Type);
		}

		SymMCE::ISymMceCmdLoader	mce;

		// At this point an alert has been queued for display, since alerts don't yet know how to display
		// themselves in the 10' UI we need to tell the user to return to the Desktop (2' UI)
		// NOTE: if user cancels this message, the normal 2' UI message will not be displayed
		if (SYM_SUCCEEDED(mce.DoNotAlertToDesktop(CISVersion::GetProductName(), SymMCE::MCE_ALERT_CHECK_STATE)))
		{
			CCTRACEI (_T("CAlertList::processAlert - MCE user has surpressed this alert!"));
		}
		else
		{
			// Show alert dialog.
			hrx << m_spAlertDlg->Show(alert.alertMap.Type, alert.hWndParent, FALSE);
		}
	}
	STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr)
}

//////////////////////////////////////////////////////////////////////
// CAlertList::getNextAlert()

bool CAlertList::getNextAlert( ALERT_ITEM& alert )
{
    //~~--~~//~~--~~//~~--~~//~~--~~//~~--~~//~~--~~//~~--~~
    StahlSoft::CSmartCritSection smCrit(&m_critList);
    //~~--~~//~~--~~//~~--~~//~~--~~//~~--~~//~~--~~//~~--~~
	if(m_list.empty() == true)
	{
		return false;
	}

	alert = m_list.front();

	// We must not pop the alert here because we want to prevent CDefAlertPlugin 
	//	from inserting an alert if the same alert is already in the list or it is 
	//  currently displaying on the screen.
	//  We should pop the alert after the alert window is closed.

	return true;
}

//////////////////////////////////////////////////////////////////////
// CAlertList::processQueue()

void CAlertList::processQueue()
{
	ALERT_ITEM alert;
	
	// Process the queue until we are told to stop.
	while( getNextAlert( alert ) && 
		WaitForSingleObject( m_shStopEvent, 0 ) != WAIT_OBJECT_0 )
	{
		// Display alert item.
		processAlert(alert);

		// We must pop the alert to prevent CDefAlertPlugin from inserting
		//  an alert if the same alert is already in the list or it is 
		//  currently displaying on the screen.

		//~~--~~//~~--~~//~~--~~//~~--~~//~~--~~//~~--~~//~~--~~
		StahlSoft::CSmartCritSection smCrit(&m_critList);
		//~~--~~//~~--~~//~~--~~//~~--~~//~~--~~//~~--~~//~~--~~
		m_list.pop_front();
	}
}

//////////////////////////////////////////////////////////////////////
// CAlertList::threadProc()

unsigned CAlertList::threadProc( void* pData )
{
	if( FAILED(CoInitializeEx( NULL, COINIT_APARTMENTTHREADED )))
	{
		// Will never happen.
		return -1;
	}
		

	CAlertList* pThis = (CAlertList*) pData;
	HANDLE aHandles[2] = 
    { 
          pThis->m_shStopEvent
        , pThis->m_shInsertion 
    };

	long nCount = (sizeof(aHandles)/sizeof(HANDLE));
	
	for(;;)
	{

		// Wait here for something to do.
		DWORD dwWait = StahlSoft::WaitForMultipleObjectsWithMessageLoop( nCount
                                                                         , aHandles, INFINITE );
			

        switch(dwWait)
        {
        default:
        case WAIT_OBJECT_0:
            goto cleanup;
            break;
        case WAIT_OBJECT_0 + 1:
            // Process the queue.
            pThis->processQueue();
            break;
        }
	}

cleanup:
	CoUninitialize();
	return 0;
}