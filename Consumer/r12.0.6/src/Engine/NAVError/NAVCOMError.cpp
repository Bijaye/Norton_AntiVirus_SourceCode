// NAVCOMError.cpp : Implementation of CNAVCOMError
#include "stdafx.h"

// SymInterface stuff. Goes before you use any SymInterface's.
//
#include "SymInterface.h"
SYM_OBJECT_MAP_BEGIN()
SYM_OBJECT_MAP_END()

#include "NAVCOMError.h"
#define _INIT_COSVERSIONINFO

#include "NAVSettingsHelperEx.h"

using namespace std;

#include "NAVTrust.h"
#include "NAVEventFactoryLoader.h"

#include "ccErrorDisplayInterface.h"
#include "ccErrorDisplayLoader.h"

/////////////////////////////////////////////////////////////////////////////
// CNAVCOMError

CNAVCOMError::CNAVCOMError()
{
    init();
}

CNAVCOMError::~CNAVCOMError()
{
}

STDMETHODIMP CNAVCOMError::get_Message(BSTR *pVal)
{
    _bstr_t bstrTemp ( m_strMessage );
    *pVal = bstrTemp.copy ();
	return S_OK;
}

STDMETHODIMP CNAVCOMError::put_Message(BSTR newVal)
{
    try 
    {
        _bstr_t bstrTemp = newVal;
	    m_strMessage = (LPCSTR) bstrTemp;
        m_bAlreadyLogged = false;
	    return S_OK;
    }
    catch ( _com_error e )
    {
        return e.Error ();
    }
}

STDMETHODIMP CNAVCOMError::get_ErrorID(long *pVal)
{
	*pVal = m_lErrorID;
    return S_OK;
}

STDMETHODIMP CNAVCOMError::put_ErrorID(long newVal)
{
	m_lErrorID = newVal;
    m_bAlreadyLogged = false;

	return S_OK;
}

STDMETHODIMP CNAVCOMError::get_ModuleID(long *pVal)
{
	*pVal = m_lModuleID;

	return S_OK;
}

STDMETHODIMP CNAVCOMError::put_ModuleID(long newVal)
{
	m_lModuleID = newVal;
    m_bAlreadyLogged = false;

	return S_OK;
}

STDMETHODIMP CNAVCOMError::get_HResult(long *pVal)
{
	*pVal = m_lErrorResult;

	return S_OK;
}

STDMETHODIMP CNAVCOMError::put_HResult(long newVal)
{
	m_lErrorResult = newVal;
    m_bAlreadyLogged = false;

	return S_OK;
}

HRESULT CNAVCOMError::Log()
{
    //::MessageBox ( ::GetDesktopWindow (), "Attempting log", "NAVCOMError", MB_OK);    
    
	if ( m_bAlreadyLogged )
        return S_FALSE;

    // If there is nothing to log, just return
    //
    if ( m_lErrorID == 0 && m_strMessage.IsEmpty())
        return S_FALSE;

	// Pointer to logging object.
    AV::IAvEventFactoryPtr pLogger;

    if (SYM_FAILED (AV::NAVEventFactory_IAVEventFactory::CreateObject (GETMODULEMGR(), &pLogger)) ||
        pLogger.m_p == NULL )
    {
        CCTRACEE ( _T("CActivityLogger - Could not create IAvLogger object. - %d"), ::GetLastError() );
        return E_FAIL;
    }
    
	CreateErrorString();

	// Create an event.
	CEventData eventData;

	// Populate and submit the event.
    eventData.SetData ( AV::Event_Base_propType, AV::Event_ID_Error );

    // Fill out the data elements
    //
    eventData.SetData ( AV::Event_Error_propErrorID, m_lErrorID );
    eventData.SetData ( AV::Event_Error_propModuleID, m_lModuleID );
    eventData.SetData ( AV::Event_Error_propResult, m_lErrorResult );
    eventData.SetData ( AV::Event_Error_propMessage, m_strMessage );
    
    CNAVVersion Version;
    eventData.SetData ( AV::Event_Error_propProductVersion, Version.GetPublicRevision());

	SYMRESULT result = pLogger->BroadcastAvEvent(eventData);
	if( SYM_FAILED( result ) )
        return E_FAIL;

    m_bAlreadyLogged = true;

	return S_OK;
}

// If message string (m_strMessage) was not set by user, generate it from 
// m_lErrorResID [and product name].
// Return true if there is an error string, either preexisting or successfully created
// Return false if we failed to create an error string
bool CNAVCOMError::CreateErrorString()
{
	if(!m_strMessage.IsEmpty())
	{
		CCTRCTXI1(_T("Error message already exists: %s"), m_strMessage);
		return true;
	}

	if(0 == m_lErrorResID)
	{
		CCTRCTXW0(_T("No Error resource ID"));
		return false;
	}

	CBrandingRes BrandRes;
	m_strProductName = BrandRes.ProductName();

	// Get error string
	CString csFormat;
	bool bRet = csFormat.LoadString(_Module.GetResourceInstance(), m_lErrorResID);
	if(!bRet)
		return false;

	CCTRCTXI1(_T("Error message format: %s"), csFormat);

	// Format error string with product name
	switch(m_lErrorResID)
	{
	case IDS_NAVERROR_INTERNAL:
	case IDS_NAVERROR_NAVAPSCR_REGISTRYPERMISSIONS:
	case IDS_NAVERROR_NAVLNCH_NO_QCONSOLE:
	case IDS_NAVERROR_NAVLNCH_NO_LU:
	case IDS_NAVERROR_NAVLNCH_NO_RESCUE:
	case IDS_NAVERROR_NAVLNCH_RESCUE_NOT_INSTALLED:
	case IDS_NAVERROR_NAVLNCH_NO_HELP:
		m_strMessage.Format(csFormat, m_strProductName);
		break;

	case IDS_NAVERROR_INTERNAL_REINSTALL:
	case IDS_NAVERROR_NAVLNCH_FAILED_START_EMAIL:
	case IDS_NAVERROR_NAVLNCH_NO_UEW:
	case IDS_NAVERROR_NAVLNCH_NO_WIPEINFO:
	case IDS_NAVERROR_NO_COMPONENT:
	case IDS_NAVERROR_NAVLNCH_FAILED_LAUNCH_SUBSWIZ:
	case IDS_NAVERROR_NAVLNCH_NO_NAV:
	case IDS_NAVERROR_NDSCHEDULER:
		m_strMessage.Format(csFormat, m_strProductName, m_strProductName);
		break;

	default:
		break;
	}

	CCTRCTXI1(_T("Error message: %s"), m_strMessage);

	if(m_strMessage.IsEmpty())
		return false;
	else
		return true;
}


HRESULT CNAVCOMError::Display(long hwnd)
{
	CCTRACEI(_T("CNAVCOMError::Display() - m_lErrorResID: %d"), m_lErrorResID);

	CBrandingRes BrandRes;
	m_strProductName = BrandRes.ProductName();

	CString csFormat;
	csFormat.LoadString(_Module.GetResourceInstance(), IDS_CAPTION);
	m_strCaption.Format(csFormat, m_strProductName);

	CreateErrorString();	

    // If there is nothing to log, just return
    //
    if ( m_lErrorID == 0 && m_strMessage.IsEmpty())
        return S_FALSE;

    //::MessageBox ( ::GetDesktopWindow (), "Attempting display", "NAVCOMError", MB_OK);
    CCTRACEI ( "CNAVCOMError::Display %d,%d - %s", m_lModuleID, m_lErrorID, m_strMessage);

    cc::ccErrorDisplay_IErrorDisplay factoryErrDisp;
    cc::IErrorDisplayPtr pErrDsp;

    if(SYM_FAILED(factoryErrDisp.CreateObject(&pErrDsp)) || !pErrDsp.m_p)
    {
        // No ccErrorDisplay installed
        //
        CCTRACEE ( "CNAVCOMError::Display - no ccErrDsp installed" );
        return E_FAIL;
    }

    // Try ccError Display first, message box if it fails.
    //
    CNAVVersion Version;
    DWORD dwNumParameters = 1;
    LPCTSTR lpszKeys [1];
    LPCTSTR lpszValues [] = {Version.GetBuildType()};

    lpszKeys[0] = _T("build");

	// Removing: Hard-coded minor version to "0". See defect # 404200.
	//
    // Defect 1-40JUWL: Use the entire version string.
	// Major#.Minor#.Inline#.Build#
    TCHAR szVersion [100] = {0};
    wsprintf(szVersion, "%u.%u.%u.%u", Version.GetMajorVersion(), 
									   Version.GetMinorVersion(), 
									   Version.GetInlineVersion(), 
									   Version.GetBuildNumber());

    if ( !pErrDsp->DisplayError (reinterpret_cast<HWND>(hwnd),      // Handle to parent window
                                 m_lModuleID,    
                                 m_lErrorID,     
                                 m_strCaption,      
                                 m_strMessage,
                                 Version.GetProductAbbr (), 
                                 szVersion,
                                 dwNumParameters,
                                 &lpszKeys[0],
                                 &lpszValues[0] ))
    {
        // Failed to display the error...use a normal message box instead and add the Module/Error ID
        TCHAR szErrorID[100] = {0};
        _stprintf(szErrorID, _T(" %d:%d"), m_lModuleID, m_lErrorID);
        m_strMessage += szErrorID;
        ::MessageBox(reinterpret_cast<HWND>(hwnd), m_strMessage, m_strCaption, MB_OK | MB_ICONERROR);
    }

    // Release the interface before the loader, just in case.
    //
    if ( pErrDsp )
        pErrDsp.Release();

    return S_OK;
}

STDMETHODIMP CNAVCOMError::LogAndDisplay(long hwnd)
{
    // Display and return result from the display
    HRESULT hr = Show(TRUE, TRUE, hwnd);
    init ();
    return hr;
}

STDMETHODIMP CNAVCOMError::Show(BOOL bDisplay, BOOL bLog, long hWnd)
{
	HRESULT hr = E_FAIL;

    // Log ignoring failures to log
    if ( bLog )
        hr = Log();

    if ( bDisplay )
        hr = Display ( hWnd );

    // I want script to know that this error object was empty.
    //
    if ( hr == S_FALSE )
        hr = E_FAIL;

    init ();

    return hr;
}

void CNAVCOMError::init()
{
    m_lErrorResult = E_FAIL;
    m_lErrorID = 0;
    m_lModuleID = 0;
    m_bAlreadyLogged = false;
	m_lForcedModuleID = 0;
	m_lForcedErrorID = 0;
	m_lErrorResID = 0;

	CRegKey rk;

	if (rk.Open(HKEY_LOCAL_MACHINE, "SOFTWARE\\Symantec\\Norton AntiVirus", KEY_READ) == ERROR_SUCCESS)
	{
        DWORD dwValue = 0;

		if (rk.QueryDWORDValue("Forced Error", dwValue) == ERROR_SUCCESS && dwValue == 1)
		{
			CNAVOptSettingsEx NavOpts;
			if ( NavOpts.Init() )
			{
				if ( SUCCEEDED(NavOpts.GetValue(ERROR_ForcedModuleID, dwValue, 0)) )
				{
			        m_lForcedModuleID = dwValue;

					if ( SUCCEEDED(NavOpts.GetValue(ERROR_ForcedErrorID, dwValue, 0)) )
						m_lForcedErrorID = dwValue;
				}
			}
		}
	}

	CCTRACEI("ForcedModuleID = %d, ForcedErrorID = %d", m_lForcedModuleID, m_lForcedErrorID);
}

STDMETHODIMP CNAVCOMError::get_ForcedErrorID(long *pVal)
{
	*pVal = m_lForcedErrorID;
	return S_OK;
}

STDMETHODIMP CNAVCOMError::put_ForcedErrorID(long newVal)
{
	m_lForcedErrorID = newVal;
	return S_OK;
}

STDMETHODIMP CNAVCOMError::get_ForcedModuleID(long *pVal)
{
	*pVal = m_lForcedModuleID;
	return S_OK;
}

STDMETHODIMP CNAVCOMError::put_ForcedModuleID(long newVal)
{
    m_lForcedErrorID = newVal;
	return S_OK;
}

STDMETHODIMP CNAVCOMError::get_ErrorResourceID(long* pVal)
{
	*pVal = m_lErrorResID;
	return S_OK;
}

STDMETHODIMP CNAVCOMError::put_ErrorResourceID(long newVal)
{
	m_lErrorResID = newVal;
	return S_OK;
}
