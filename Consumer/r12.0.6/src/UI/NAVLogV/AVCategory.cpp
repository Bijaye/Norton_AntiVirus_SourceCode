// AVCategory.cpp: implementation of the CAVCategory class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AVCategory.h"
#include "AVRESBranding.h"

#include "NAVTrust.h"
#include "NAVHelpLauncher.h"
#include "ccSymModuleLifetimeMgrHelper.h"

using namespace ccEvtMgr;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
_variant_t CAVCategory::g_vComputerName; // it's all the same machine so... why store it?
CString CAVCategory::m_csProductName;
CString CAVCategory::m_csThreatInfoURL;
CAVFeatureNameMap CAVCategory::m_Features;

CAVCategory::CAVCategory() : 
    m_nEventType(NULL),
	m_pFactory(NULL),
    m_iCategoryID(0),
    m_hEventIcons(NULL),
    m_dwHelpID(0),
    m_pEventFactory(NULL),
    m_nContextID(0),
    m_nEventCount(0)
{
    // Load the factory and proxy DLLs
    if (SYM_FAILED(AV::NAVEventFactory_CEventFactoryEx2::CreateObject (GETMODULEMGR(), &m_pEventFactory)) ||
        m_pEventFactory == NULL ||
        SYM_FAILED(AV::NAVEventFactory_CProxyFactoryEx::CreateObject(GETMODULEMGR(), &m_pProxyFactory)) ||
        m_pProxyFactory == NULL ||
        m_EventManager.Create(m_pEventFactory, m_pProxyFactory) == FALSE)
    {
        CCTRACEE ( _T("CAVCategory - Could not create EventFactory object. - %d"), ::GetLastError() );
    }

    ASSERT ( m_pEventFactory );
    ASSERT ( m_pProxyFactory );

    // Create log helper
    if (m_pEventFactory != NULL)
    {
        if (m_LogManagerHelper.Create(m_pEventFactory) == FALSE)
        {
            ASSERT(FALSE);
        }
    }

    // &$ is this the best way to check for empty?
    _variant_t vEmpty;
    if ( g_vComputerName == vEmpty)
    {
        char szCompName [MAX_COMPUTERNAME_LENGTH + 1] = {0};
        DWORD dwTempNameSize = sizeof (szCompName);

        if ( ::GetComputerName ( &szCompName[0], &dwTempNameSize ) > 0 )
        {
            g_vComputerName = szCompName;
        }
    }

	if(m_csProductName.IsEmpty() || m_csThreatInfoURL.IsEmpty())
	{
		CBrandingRes BrandRes;
		m_csProductName = BrandRes.ProductName();
		m_csThreatInfoURL = BrandRes.GetString(IDS_SARC_THREAT_INFO_URL);
	}
}

CAVCategory::~CAVCategory()
{
    clearSearchContexts();
    deleteEvents ();

    m_LogManagerHelper.Destroy();

    // Delete the factories in reverse order before the loaders,
    // otherwise crashy - defect 1-2KFZ89.
    //
    if ( m_pProxyFactory )
    {
        m_pProxyFactory.Release ();
    }

    if ( m_pEventFactory )
    {
        m_pEventFactory.Release ();
    }

    m_EventManager.Destroy ();
}

HRESULT CAVCategory::GetCategoryID(int& iCategoryID)
{
	iCategoryID = m_iCategoryID;
	return S_OK;
}

HRESULT CAVCategory::GetCategoryDescription( LPTSTR szCategoryDesc,
            							    /* [in/out] */ DWORD& dwSize)
{
    if ( dwSize < m_strCategoryDescription.length() )
	{
		// cc::ILogViewerEvent spec specifies this call should
		//  return the size of the buffer required.
		dwSize = m_strCategoryDescription.length();
		return HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
	}
	
	_tcscpy ( szCategoryDesc, m_strCategoryDescription.c_str());
    dwSize = m_strCategoryDescription.length();
    return S_OK;
}

HRESULT CAVCategory::GetCategoryName(LPTSTR szCategoryName,
						 /* [in/out] */ DWORD& dwSize)
{
    if ( dwSize < m_strCategoryName.length() )
	{
		// cc::ILogViewerEvent spec specifies this call should
		//  return the size of the buffer required.
		dwSize = m_strCategoryName.length();
		return HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
	}

    _tcscpy ( szCategoryName, m_strCategoryName.c_str() );
    dwSize = m_strCategoryName.length();
    return S_OK;
}

HRESULT CAVCategory::GetColumnName(int iColumn,
                                   LPTSTR szColumnName,
                                   /* [in/out] */ DWORD& dwSize)
{
    if ( dwSize < m_vecColumnTitles[iColumn].length() )
	{
		// cc::ILogViewerEvent spec specifies this call should
		//  return the size of the buffer required.
		dwSize = m_vecColumnTitles[iColumn].length();
		return HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
	}

    _tcscpy ( szColumnName, m_vecColumnTitles[iColumn].c_str() );
    dwSize = m_vecColumnTitles[iColumn].size();
    return S_OK;

}

HRESULT CAVCategory::GetImageList(HIMAGELIST& hImageList)
{
    if ( m_hEventIcons == NULL )
        return E_NOTIMPL;

    hImageList = m_hEventIcons;
	return S_OK;
}

HRESULT CAVCategory::GetColumnCount(int& iColumnCount)
{
	iColumnCount = m_vecColumnTitles.size();

	return S_OK;
}

HRESULT CAVCategory::GetEventCount(int& iEventCount)
{
    // We can't do caching based on the event count. When the user
    // hits Refresh, the log viewer just calls GetEventCount again
    // so we don't clear our events.

/* 
    if ( m_nEventCount )
    {
        iEventCount = m_nEventCount;
        return S_OK;
    }*/

    iEventCount = 0;

    HRESULT hr = loadBatch ();
    
    if ( SUCCEEDED (hr))
        iEventCount = m_nEventCount;

    return hr;
}

HRESULT CAVCategory::GetEvent(int iEventIndex, cc::ILogViewerEvent*& pEvent)
{
    pEvent = NULL;

    // Load event
    //
    if ( iEventIndex > (int) m_vecEventDataPtr.size ()-1)
        return E_FAIL;

    int error = ccEvtMgr::CError::eNoError;

    if (m_pFactory != NULL)
    {
        HRESULT hRes = m_pFactory->CreateInstance(cc::IID_LogViewerEvent, 
                                                 (void**)&pEvent);
	    
        ASSERT(SYM_SUCCEEDED(hRes));
    }

    // Set event properties
    if (pEvent != NULL)
    {
        if (SetEventProperties(m_vecEventDataPtr[iEventIndex]->m_pEvent, 
                               m_vecEventDataPtr[iEventIndex]->m_EventInfo, 
                               pEvent) == FALSE)
        {
            pEvent->Release();
            pEvent = NULL;
        }
    }

    if (pEvent != NULL)
    {
        return S_OK;
    }
    return E_FAIL;
}

void CAVCategory::clearSearchContexts()
{
    m_LogManagerHelper.DeleteSearchContext(m_nEventType, 
                                           m_nContextID);
    m_nEventCount = 0;
}


HRESULT CAVCategory::SetLogFileSize(int iLogFileSize)
{
	CCTRACEI("SetLogFileSize: %d", iLogFileSize);

    if ( !g_bIsAdmin )
        return E_ACCESSDENIED;

    if ( iLogFileSize < 1024  )
         return E_FAIL;
    
    // Get the first event type
    //
    if ( ccEvtMgr::CError::eNoError == m_LogManagerHelper.SetMaxLogSize(m_nEventType, (ULONGLONG)iLogFileSize))
        return S_OK;
    else
        return E_FAIL;
}

HRESULT CAVCategory::GetLogFileSize(int& iLogFileSize)
{
	CCTRACEI("GetLogFileSize");

    ULONGLONG ullTemp = 0;

    if ( ccEvtMgr::CError::eNoError == m_LogManagerHelper.GetMaxLogSize(m_nEventType, ullTemp))
    {
        iLogFileSize = (int)ullTemp;

		CCTRACEI("LogFileSize: iLogFileSize");
        return S_OK;
    }
    else
	{
		CCTRACEI("Failed!");
        return E_FAIL;
	}
}

HRESULT CAVCategory::LaunchHelp()
{
    try
    {
    NAVToolbox::CNAVHelpLauncher NAVHelp;

    if ( NAVHelp.LaunchHelp ( m_dwHelpID ))
        return S_OK;
    }
    catch (...)
    {
    }

        return E_FAIL;
}

HRESULT CAVCategory::ClearCategory()
{
    if ( !g_bIsAdmin )
        return E_ACCESSDENIED;

    clearSearchContexts ();

    return ccEvtMgr::CError::eNoError != m_LogManagerHelper.ClearLog(m_nEventType);
}

HRESULT CAVCategory::GetLoggingState(BOOL& bEnabled)
{
    bool bOn = false;

    if ( ccEvtMgr::CError::eNoError == m_LogManagerHelper.GetLogEnabled (m_nEventType, bOn))
    {
        bEnabled = bOn ? TRUE : FALSE;
        return S_OK;
    }
    return E_FAIL;
}

HRESULT CAVCategory::SetLoggingState(BOOL bEnabled)
{
    if ( !g_bIsAdmin )
        return E_ACCESSDENIED;

    if ( ccEvtMgr::CError::eNoError != m_LogManagerHelper.SetLogEnabled (m_nEventType, bEnabled?TRUE:FALSE))
    {
        return E_FAIL;
    }

    return S_OK;
}

HRESULT CAVCategory::GetDefaultSort (int& iColumn /* [out] */, int& iSortOrder /* [out] */)
{
    iColumn = ILogViewerCategory::SORTCOLUMN_NONE;
    iSortOrder = ILogViewerCategory::LV_SORT_NONE;
    return S_OK;
}

void CAVCategory::deleteEvents ()
{
    // Delete events
    //
    if (!m_vecEventDataPtr.empty ())
    {
        for ( vecEventDataPtr::iterator iter = m_vecEventDataPtr.begin ();
              iter < m_vecEventDataPtr.end ();
              iter ++ )
        {
            CLogEventData* pData = *iter;
            if ( pData )
            {
                pData->m_pEvent->Release();
            }
        }

        m_vecEventDataPtr.clear ();
    }
}

HRESULT CAVCategory::loadBatch ()
{
	CCTRACEI("loadBatch");

    clearSearchContexts ();
    
    // Clear existing events
    //
    deleteEvents ();

    // Search for events
    //
    CError::ErrorType eError = ccEvtMgr::CError::eNoError;

    long nContextId = 0;
    long nFail = 0;
    long nOk = 0;
    SYSTEMTIME timeStart, timeStop;
    ZeroMemory(&timeStart, sizeof(timeStart));
    ZeroMemory(&timeStop, sizeof(timeStop));

    eError = m_LogManagerHelper.SearchEvents( m_nEventType,
                                              timeStart,
                                              timeStop,
                                              true,
                                              0,
                                              nContextId,
                                              m_EventInfoVector);
    if (eError != CError::eNoError)
    {
        m_nEventCount = 0;

		CCTRACEE("SearchEvents failed!");
        return E_FAIL;
    }
    else
    {
        m_nEventCount = long(m_EventInfoVector.size());

        // Allocate memory to receive the CEventEx*
        // ??? Add exception handling
        CEventEx** pEventArray = new CEventEx*[m_nEventCount];
        ZeroMemory(pEventArray, sizeof(CEventEx*) * m_nEventCount);
        
        // Load all the events at once
        eError = m_LogManagerHelper.LoadEventBatch( m_nEventType,
                                                    m_EventInfoVector,
                                                    nContextId,
                                                    pEventArray,
                                                    m_nEventCount);
        if (eError != CError::eNoError)
        {
            nOk = 0;
            nFail = m_nEventCount;
            m_nEventCount = 0;
        }
        else
        {
            CLogManagerHelper::EventInfoVector::iterator pos;
            int nItem = 0;
            long i = 0;
            for (pos = m_EventInfoVector.begin();
                 pos != m_EventInfoVector.end();
                 pos ++)
            {
                //InsertEvent(nItem, *pos, *pEventArray[i]);
                CLogEventData* pTempData = new CLogEventData ( pEventArray[i], *pos );
                if ( pTempData )
                    m_vecEventDataPtr.push_back (pTempData);
                i ++;
            }
            nOk = m_nEventCount;
            nFail = 0;
        }

        // Delete temp events now that we have a local copy
        //
        if (pEventArray != NULL)
        {
            m_LogManagerHelper.DeleteBatchData(pEventArray, m_nEventCount);
            delete[] pEventArray;
            pEventArray = NULL;
        }
    }

    if (nContextId != 0)
    {
        m_LogManagerHelper.DeleteSearchContext(m_nEventType, nContextId);
        nContextId = 0;
    }

    if (nFail > 0)
    {
        //g_Log.Log(_T("Fail: %ld, Ok: %ld"), nFail, nOk);
    }

	CCTRACEI("EventCount: %d", m_nEventCount);
    return S_OK;

}
