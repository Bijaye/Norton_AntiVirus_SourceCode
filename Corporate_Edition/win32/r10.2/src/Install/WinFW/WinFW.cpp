///////////////////////////////////////////////////////////////////////
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005, Symantec Corporation, All rights reserved.
///////////////////////////////////////////////////////////////////////
// WinFW.cpp
///////////////////////////////////////////////////////////////////////
//	Created: JZ, 1/21/2005
///////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "SymInterface.h"
#include "IWinFW.h"
#include "WinFW.h"
#include "syminterfaceloader.h"
#include "comdef.h"

///////////////////////////////////////////////////////////////////////
//
//	Constructor
///////////////////////////////////////////////////////////////////////

CWinFW::CWinFW()
:m_nLastError(S_OK),
m_bInitOK(false),
m_bWinFWInstalled(false)
{
	try
	{
		m_bInitOK = Init();
	}
	catch(_com_error& e)
	{
		m_nLastError = e.Error();
	}
	catch(...)
	{
		m_nLastError = E_FAIL;
	}
}
///////////////////////////////////////////////////////////////////////
//	destructor
///////////////////////////////////////////////////////////////////////
CWinFW::~CWinFW()
{
	//nothing to do
}
HRESULT CWinFW::CheckHr(HRESULT hr)
{
	if(FAILED(hr))
		m_nLastError = hr;
	return hr;
	
}
///////////////////////////////////////////////////////////////////////
//	Function:	Object Initialization
//	Return:
//		true - everything is ok
//		false - failed
///////////////////////////////////////////////////////////////////////
bool CWinFW::Init ()
{
	//
	//	create the firewall mgr
	//
	try
	{
		CheckHr(CoCreateInstance(	__uuidof(NetFwMgr),
									NULL,
									CLSCTX_INPROC_SERVER,
									__uuidof(INetFwMgr),
									(void**)&m_spFwMgr) );
		//
		//	if the component is not registered, then it is not installed
		//
		m_bWinFWInstalled = (m_nLastError != REGDB_E_CLASSNOTREG);

		//
		//	if we can not create the mgr or the firewall is not installed
		//
		if(!m_bWinFWInstalled ||FAILED(m_nLastError))
			return false;

		// Retrieve the local firewall policy.
		if(FAILED(CheckHr(m_spFwMgr->get_LocalPolicy(&m_spFwPolicy))))
			return false;

		// Retrieve the firewall profile currently in effect.
		if(FAILED(CheckHr( m_spFwPolicy->get_CurrentProfile(&m_spFwProfile))))
			return false;
	}
	catch(_com_error& e)
	{
		m_nLastError = e.Error ();
		return false;
	}
	return true;
}
///////////////////////////////////////////////////////////////////////
//	Function:	See if windows firewall installed
//	Return:
//	true - installed
//	false - not installed
///////////////////////////////////////////////////////////////////////
bool CWinFW::IsWinFwInstalled() const
{
	return m_bWinFWInstalled;
}
///////////////////////////////////////////////////////////////////////
//	return last error
///////////////////////////////////////////////////////////////////////
HRESULT CWinFW::GetLastError() const
{
	return m_nLastError;
}
///////////////////////////////////////////////////////////////////////
//	Function:	See windows firewall is turned on or off
//	Return:
//		S_OK - ok
//		others - failed
///////////////////////////////////////////////////////////////////////
HRESULT CWinFW::IsWinFwOn(OUT bool& bOn) 
{
	bOn = false;
	if(!m_bInitOK || !m_bWinFWInstalled || !m_spFwMgr || !m_spFwProfile || !m_spFwPolicy)
		return E_FAIL;

	// Get the current state of the firewall.
	try
	{
		VARIANT_BOOL bFwEnabled;
		if(FAILED(CheckHr(m_spFwProfile->get_FirewallEnabled(&bFwEnabled))))
			return m_nLastError;

		bOn =  (bFwEnabled == VARIANT_TRUE);
	}
	catch(_com_error& e)
	{
		m_nLastError = e.Error ();
		return m_nLastError;
	}
	catch(...)
	{
		m_nLastError = E_FAIL;
		return m_nLastError;
	}
	return m_nLastError;
}
///////////////////////////////////////////////////////////////////////
//	Function: turn windows firewall on/off
//	Return:
//		S_OK - ok
//		others - failed
///////////////////////////////////////////////////////////////////////
HRESULT 
CWinFW::TurnOnWinFw(IN bool bTurnOn)
{
	if(!m_bInitOK || !m_bWinFWInstalled || !m_spFwMgr || !m_spFwProfile || !m_spFwPolicy)
		return E_FAIL;
	try
	{
		//
		//	check current firewall status
		//
		bool bOned = false;
		if(FAILED(IsWinFwOn(bOned))) 
			return m_nLastError;
		//
		//	if no change is needed, just return
		//
		if(bOned == bTurnOn)
			return S_OK;
		//
		// Turn the firewall on//off
		//
		CheckHr(m_spFwProfile->put_FirewallEnabled((bTurnOn == true)?VARIANT_TRUE:VARIANT_TRUE));
	}
	catch(_com_error& e)
	{
		m_nLastError = e.Error ();
		return m_nLastError;
	}
	catch(...)
	{
		m_nLastError = E_FAIL;
		return m_nLastError;
	}
	return S_OK;

}
///////////////////////////////////////////////////////////////////////
//	Function: check if an application is aurthorized
//	Return:
//		S_OK - ok
//		others - failed
///////////////////////////////////////////////////////////////////////
HRESULT 
CWinFW::IsAppAuthorized(IN	LPCTSTR szProcesImageFileName,	//path to the app
						OUT	bool& bLocalSubnetOnly,			//authorized for subnet only
						OUT	bool& bAuthorized					//is authorized?
						)
{
	bLocalSubnetOnly = false;
	bAuthorized = false;
	if(!szProcesImageFileName)
	{
		m_nLastError = E_INVALIDARG;
		return m_nLastError;
	}
	if(!m_bInitOK || !m_bWinFWInstalled || !m_spFwMgr || !m_spFwProfile || !m_spFwPolicy)
		return E_FAIL;	

	try
	{
		CComPtr<INetFwAuthorizedApplications> spFwApps;
		//
		// Retrieve the authorized application collection.
		//
		if(FAILED(CheckHr(m_spFwProfile->get_AuthorizedApplications(&spFwApps))))
			return m_nLastError;

		//
		// Allocate a BSTR for the process image file name.
		//
		CComBSTR  bstrAppPath = szProcesImageFileName;
	   
		//
		// Attempt to retrieve the authorized application.
		//
		CComPtr<INetFwAuthorizedApplication> spFwApp;

		if(SUCCEEDED(CheckHr(spFwApps->Item(bstrAppPath, &spFwApp))))
		{
			//
			// Find out if the authorized application is enabled.
			//
			VARIANT_BOOL bFwEnabled = VARIANT_FALSE;
			if(FAILED(CheckHr(spFwApp->get_Enabled(&bFwEnabled))))
				return m_nLastError;

			bAuthorized = (bFwEnabled == VARIANT_TRUE);
			//
			//	find the scope
			//
			NET_FW_SCOPE scope;
			if(FAILED(CheckHr( spFwApp->get_Scope(&scope))))
				return m_nLastError;

			bLocalSubnetOnly = (scope !=NET_FW_SCOPE_ALL);

		}
		else
		{
			if(m_nLastError == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
                m_nLastError = S_OK;//this application is not there at all
		}
	}
	catch(_com_error & e)
	{
		m_nLastError = e.Error ();
		return m_nLastError;
	}
	catch(...)
	{
		m_nLastError = E_FAIL;
		return m_nLastError;
	}
	return S_OK;
}
///////////////////////////////////////////////////////////////////////
//	Function: authorize an application
//	Return:
//		S_OK - ok
//		others - failed
///////////////////////////////////////////////////////////////////////
HRESULT 
CWinFW::AuthorizeApp(	IN	LPCTSTR szProcessImageFileName,	//the app	
						IN	LPCTSTR szDisplayName,			//friendly name
						IN	bool bLocalSubnetOnly,			//for subnet only?
						IN	bool bEnableIt					//add and authorize
						)
{
	if(!szProcessImageFileName || !szDisplayName)
	{
		m_nLastError = E_INVALIDARG;
		return m_nLastError;
	}
	if(!m_bInitOK || !m_bWinFWInstalled || !m_spFwMgr || !m_spFwProfile || !m_spFwPolicy)
		return E_FAIL;	
	//
    // check to see if the application is already authorized.
	//
	bool bSubnetOnly = false;
	bool fwAppEnabled = false;
	try
	{
		if(SUCCEEDED(CheckHr(IsAppAuthorized(szProcessImageFileName,bSubnetOnly,fwAppEnabled))))
		{
			//
			//	remove it
			//
			if(fwAppEnabled)
 				if(FAILED(CheckHr(this->RemoveAuthorizedApp(szProcessImageFileName))))
 					return m_nLastError;

		}
		
		
		//
		//	to authorize this app, retrieve the authorized application collection first
		//
		CComPtr<INetFwAuthorizedApplications>	spFwApps;
		if(FAILED(CheckHr( m_spFwProfile->get_AuthorizedApplications(&spFwApps))))
			return m_nLastError;
		//
		// Create an instance of an authorized application.
		//
		CComPtr<INetFwAuthorizedApplication>	spFwApp;
		if(FAILED(CheckHr(CoCreateInstance(	__uuidof(NetFwAuthorizedApplication),
											NULL,
											CLSCTX_INPROC_SERVER,
											__uuidof(INetFwAuthorizedApplication),
											(void**)&spFwApp))))
			return m_nLastError;
		//
		// Set property
		//
		CComBSTR bstrAppPath = szProcessImageFileName;
		if(FAILED(CheckHr(spFwApp->put_ProcessImageFileName(bstrAppPath))))
			return m_nLastError;

		if(FAILED(CheckHr(spFwApp->put_IpVersion(NET_FW_IP_VERSION_ANY))))
			return m_nLastError;

		if(FAILED(CheckHr(spFwApp->put_Enabled(bEnableIt?VARIANT_TRUE:VARIANT_FALSE))))
			return m_nLastError;

		if(FAILED(CheckHr(spFwApp->put_Scope (bLocalSubnetOnly?NET_FW_SCOPE_LOCAL_SUBNET:NET_FW_SCOPE_ALL))))
			return m_nLastError;

		CComBSTR strDisplayName = szDisplayName;
		if(FAILED(CheckHr(spFwApp->put_Name(strDisplayName))))
			return m_nLastError;

		// Add the application to the collection.
		m_nLastError = spFwApps->Add(spFwApp);
	}
	catch(_com_error& e)
	{
		m_nLastError = e.Error ();
		return m_nLastError;
	}
	catch(...)
	{
		m_nLastError = E_FAIL;
		return m_nLastError;
	}
	return S_OK;
}
///////////////////////////////////////////////////////////////////////
//	Function: check to see if an port is authorized
//	Return:
//		S_OK - ok
//		others - failed
///////////////////////////////////////////////////////////////////////
HRESULT 
CWinFW::IsPortAuthorized(	IN	long lPortNumber,				//port number
							IN	NET_FW_IP_PROTOCOL ipProtocol,	//protocol
							OUT	bool& bSubnetOnly,				//for subnet only?
							OUT	bool& bEnabled					//authorized?
							)
{
	bSubnetOnly = false;
	if(!m_bInitOK || !m_bWinFWInstalled || !m_spFwMgr || !m_spFwProfile || !m_spFwPolicy)
		return E_FAIL;	

    bEnabled = false;
	try
	{
		//
		// Retrieve the globally open ports collection.
		//
		CComPtr<INetFwOpenPorts> spFwOpenPorts;
		
		if(FAILED(CheckHr( m_spFwProfile->get_GloballyOpenPorts(&spFwOpenPorts))))
			return m_nLastError;
	  long lc=0;
		spFwOpenPorts->get_Count(&lc);
		//
		// Attempt to retrieve the globally open port.
		//
		CComPtr<INetFwOpenPort> spFwOpenPort;
		//	
		//	
		if(FAILED(CheckHr(spFwOpenPorts->Item(lPortNumber, ipProtocol, &spFwOpenPort))))
		{
			if(m_nLastError == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
			{
				m_nLastError = S_OK;
				return m_nLastError;
			}
		}
		


		// Find out if the globally open port is enabled.
		VARIANT_BOOL bFwEnabled;

		if(FAILED(CheckHr(spFwOpenPort->get_Enabled(&bFwEnabled))))
			return m_nLastError;

		bEnabled = (bFwEnabled == VARIANT_TRUE);
		//
		//	check the scope
		//
		NET_FW_SCOPE scope;
		if(FAILED(CheckHr(spFwOpenPort->get_Scope (&scope))))
			return m_nLastError;
		bSubnetOnly = (scope == NET_FW_SCOPE_LOCAL_SUBNET);
	}
	catch(_com_error& e)
	{
		m_nLastError = e.Error ();
		return m_nLastError;
	}
	catch(...)
	{
		m_nLastError = E_FAIL;
		return m_nLastError;
	}
	return S_OK;
}
///////////////////////////////////////////////////////////////////////
//	Function: autorize a port
//	Return:
//		S_OK - ok
//		others - failed
///////////////////////////////////////////////////////////////////////
HRESULT 
CWinFW::AuthorizePort(	IN	long lPortNumber,				//port
						IN	NET_FW_IP_PROTOCOL ipProtocol,	//tcp or udp ?
						IN	LPCTSTR szDisplayName,			//friendly name
						IN	bool bLocalSubnetOnly,			//subnet only
						IN	bool bEnableIt					//enable it
					  )
{
	if(!m_bInitOK || !m_bWinFWInstalled || !m_spFwMgr || !m_spFwProfile || !m_spFwPolicy)
		return E_FAIL;	


    CComBSTR strDisplayName = szDisplayName;
	if(!strDisplayName.Length ())
	{
		m_nLastError = E_INVALIDARG;
		return m_nLastError;
	}
	try
	{
		//
		// First check to see if the port is already added.
		//
		bool bEnabled =false;
		bool bSubOnly;
		if(FAILED(CheckHr(IsPortAuthorized(lPortNumber,ipProtocol,bSubOnly,bEnabled))))
			return m_nLastError;
		//	
		//	if exists, remove it first
		// 
		if(bEnabled)
			this->RemoveAuthorizedPort(lPortNumber,ipProtocol);
		//
		// Retrieve the collection of globally open ports.
		//
		CComPtr<INetFwOpenPorts> spFwOpenPorts;
		if(FAILED(CheckHr( m_spFwProfile->get_GloballyOpenPorts(&spFwOpenPorts))))
			return m_nLastError;
		//
		//	Create an instance of an open port.
		//
		CComPtr<INetFwOpenPort> spFwOpenPort;
		if(FAILED(CheckHr(CoCreateInstance(	__uuidof(NetFwOpenPort),
											NULL,
											CLSCTX_INPROC_SERVER,
											__uuidof(INetFwOpenPort),
											(void**)&spFwOpenPort))))
		return m_nLastError;
	      

		// 
		//	Set the properties
		//
		if(FAILED(CheckHr(spFwOpenPort->put_Port(lPortNumber))))
			return m_nLastError;
	 
		if(FAILED(CheckHr(spFwOpenPort->put_Protocol(ipProtocol))))
			return m_nLastError;

		if(FAILED(CheckHr(spFwOpenPort->put_Name(strDisplayName))))
			return m_nLastError;

  		if(FAILED(CheckHr(spFwOpenPort->put_IpVersion(NET_FW_IP_VERSION_ANY))))
			return m_nLastError;

		if(FAILED(CheckHr(spFwOpenPort->put_Enabled(bEnableIt?VARIANT_TRUE:VARIANT_FALSE))))
			return m_nLastError;

		if(FAILED(CheckHr(spFwOpenPort->put_Scope (bLocalSubnetOnly?NET_FW_SCOPE_LOCAL_SUBNET:NET_FW_SCOPE_ALL))))
			return m_nLastError;
		//
		// Opens the port and adds it to the collection.
		//
		CheckHr(spFwOpenPorts->Add(spFwOpenPort));
	}
	catch(_com_error& e)
	{
		m_nLastError = e.Error ();
		return m_nLastError;
	}
	catch(...)
	{
		m_nLastError = E_FAIL;
		return m_nLastError;
	}
	return S_OK;

}
///////////////////////////////////////////////////////////////////////
//	Function: see if exception list is allowed
//	Return:
//		S_OK - ok
//		others - failed
///////////////////////////////////////////////////////////////////////
HRESULT CWinFW::IsExceptionsAllowed(bool& bAllow)
{
	bAllow = false;

	if(!m_bInitOK || !m_bWinFWInstalled || !m_spFwMgr || !m_spFwProfile || !m_spFwPolicy)
		return E_FAIL;	
	//
	// Retrieve the authorized application collection.
	//
	try
	{
		VARIANT_BOOL bNotAllowed = VARIANT_FALSE;
		if(FAILED(CheckHr( m_spFwProfile->get_ExceptionsNotAllowed(&bNotAllowed))))
			return m_nLastError;
		bAllow = (bNotAllowed == VARIANT_FALSE);
	}
	catch(_com_error& e)
	{
		m_nLastError = e.Error ();
		return m_nLastError;
	}
	catch(...)
	{
		m_nLastError = E_FAIL;
		return m_nLastError;
	}
	return S_OK;
}
///////////////////////////////////////////////////////////////////////
//	Function:	allow or disallow exception list
//	Return:
//		S_OK - ok
//		others - failed
///////////////////////////////////////////////////////////////////////
HRESULT CWinFW::AllowExceptions(bool bAllow)
{


	if(!m_bInitOK || !m_bWinFWInstalled || !m_spFwMgr || !m_spFwProfile || !m_spFwPolicy)
		return E_FAIL;	
	//
	// Retrieve the authorized application collection.
	//
	VARIANT_BOOL bNotAllowed = VARIANT_FALSE;
	try
	{
		CheckHr( m_spFwProfile->put_ExceptionsNotAllowed(bAllow? VARIANT_FALSE:VARIANT_TRUE));
	}
	catch(_com_error& e)
	{
		m_nLastError = e.Error ();
		return m_nLastError;
	}
	catch(...)
	{
		m_nLastError = E_FAIL;
		return m_nLastError;
	}
	return S_OK;
}
///////////////////////////////////////////////////////////////////////
//	Function: Remove an application from the authorized list
//	Return:
//		S_OK - ok
//		others - failed
///////////////////////////////////////////////////////////////////////
HRESULT 
CWinFW::RemoveAuthorizedApp(IN LPCTSTR szAppPath) 
{
	if(!m_bInitOK || !m_bWinFWInstalled || !m_spFwMgr || !m_spFwProfile || !m_spFwPolicy)
		return E_FAIL;	
	if(!szAppPath)
	{
		m_nLastError = E_INVALIDARG;
		return m_nLastError;
	}
	try
	{
		//
		//	first check to see if this application is in the authorization list
		//
		CComPtr<INetFwAuthorizedApplications> spFwApps;
		if(FAILED(CheckHr(m_spFwProfile->get_AuthorizedApplications(&spFwApps))))
			return m_nLastError;
		//
		// remove it
		//
		CComBSTR szPath = szAppPath;
		if(FAILED(CheckHr(spFwApps->Remove(szPath))))
			return m_nLastError;
	}
	catch(_com_error& e)
	{
		m_nLastError = e.Error ();
		return m_nLastError;
	}
	catch(...)
	{
		m_nLastError = E_FAIL;
		return m_nLastError;
	}
	return S_OK;
}
///////////////////////////////////////////////////////////////////////
//	Function:	remove specific port from the authrized list
//	Return:
//		S_OK - ok
//		others - failed
///////////////////////////////////////////////////////////////////////
HRESULT 
CWinFW::RemoveAuthorizedPort(	IN	LONG portNumber, 
								IN	NET_FW_IP_PROTOCOL ipProtocol
							)
{
	if(!m_bInitOK || !m_bWinFWInstalled || !m_spFwMgr || !m_spFwProfile || !m_spFwPolicy)
		return E_FAIL;	
	try
	{
		//
		//	Retrieve the collection of globally open ports.
		//
		CComPtr<INetFwOpenPorts> spFwOpenPorts;
		if(FAILED(CheckHr(m_spFwProfile->get_GloballyOpenPorts(&spFwOpenPorts))))
			return m_nLastError;
		if(FAILED(CheckHr(spFwOpenPorts->Remove (portNumber,ipProtocol))))
			return m_nLastError;
	}
	catch(_com_error& e)
	{
		m_nLastError = e.Error ();
		return m_nLastError;
	}
	catch(...)
	{
		m_nLastError = E_FAIL;
		return m_nLastError;
	}
	return S_OK;
	
}
///////////////////////////////////////////////////////////////////////
//	Configure Windows Firewall to remove SCS related configuration
///////////////////////////////////////////////////////////////////////
HRESULT ConfigWinFw4Apps (	LPCTSTR szWinFwDll, SYMAPP_LIST& apps)
{
	if(!szWinFwDll || !apps.size ())
		return E_INVALIDARG;

	HRESULT hr = S_OK;
	//
	//	initialize com
	//
	HRESULT hrInit = CoInitializeEx(NULL,0 );
	if(SUCCEEDED(hrInit) || hrInit == RPC_E_CHANGED_MODE)
	{
		//
		//	load WinFW
		//
		CWinFWLoader wfLoader;
		do
		{
			if(FAILED(wfLoader.Initialize(szWinFwDll)))
			{
				hr = E_FAIL;
				break;
			}
			//
			//	Create IWinFWPtr
			//
			IWinFWPtr pI = 0;
			if(FAILED( wfLoader.CreateObject (&pI)))
			{
				hr = E_FAIL;
				break;
			}
			//
			//	try to autorize an SCS applications
			//
			SYMAPP_LIST::const_iterator iter;
			
			for (iter=apps.begin(); iter != apps.end(); iter++)
			{
				SYMAPP theApp=(*iter);
				//
				//	dont quit if one fails, just remember one failure
				//
				if(FAILED(hr))
					pI->AuthorizeApp (theApp.strPath.c_str (),theApp.strFriendlyName.c_str (),theApp.bSubnetOnly,true);

				else
					hr= pI->AuthorizeApp (theApp.strPath.c_str (),theApp.strFriendlyName.c_str (),theApp.bSubnetOnly,true);
				//
				//	if exception list is unckecked, check it
				//
				bool bAllowed = false;
				if(SUCCEEDED(pI->IsExceptionsAllowed(bAllowed))&&!bAllowed)
				{
					pI->AllowExceptions(true);	
				}
			}

		}while(0);

		//
		//	uninitialize com
		//
		if(hrInit != RPC_E_CHANGED_MODE)
			CoUninitialize();
	}
	return hr;
}

///////////////////////////////////////////////////////////////////////
//	Configure Windows Firewall to allow SCS component function properly
///////////////////////////////////////////////////////////////////////
HRESULT UnConfigWinFw4Apps (	LPCTSTR szWinFwDll, SYMAPP_LIST& apps)
{
	if(!szWinFwDll || !apps.size ())
		return E_INVALIDARG;

	HRESULT hr = S_OK;
	//
	//	initialize com
	//
	HRESULT hrInit = CoInitializeEx(NULL,0 );
	if(SUCCEEDED(hrInit) || hrInit == RPC_E_CHANGED_MODE)
	{
		//
		//	load WinFW
		//
		CWinFWLoader wfLoader;
		do
		{
			if(FAILED(wfLoader.Initialize(szWinFwDll)))
			{
				hr = E_FAIL;
				break;
			}
			//
			//	Create IWinFWPtr
			//
			IWinFWPtr pI = 0;
			if(FAILED( wfLoader.CreateObject (&pI)))
			{
				hr = E_FAIL;
				break;
			}
			//
			//	try to autorize an SCS applications
			//
			SYMAPP_LIST::const_iterator iter;
			
			for (iter=apps.begin(); iter != apps.end(); iter++)
			{
				SYMAPP theApp=(*iter);
				//
				//	dont quit if one fails, just remember one failure
				//
				if(FAILED(hr))
					pI->RemoveAuthorizedApp  (theApp.strPath.c_str ());

				else
					hr= pI->RemoveAuthorizedApp (theApp.strPath.c_str ());
				
			}

		}while(0);

		//
		//	uninitialize com
		//
		if(SUCCEEDED(hrInit))
			CoUninitialize();
	}
	return hr;
}