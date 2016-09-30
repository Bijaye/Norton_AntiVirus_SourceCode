// NAVAPService.cpp : Implementation of CNAVAPService
#include "stdafx.h"
#include "Navapsvc.h"
#include "NAVAPService.h"
#include "navapcommands.h"

/////////////////////////////////////////////////////////////////////////////
// CNAVAPService::RegisterHandler()

STDMETHODIMP CNAVAPService::RegisterHandler(INAVAPEventHandler *pHandler)
{
	// Always make sure the service is up and running before 
	// processing a command.
	_Module.WaitForStartup();

	HRESULT hr = S_OK;
	try
	{
		_Module.InsertHandler( pHandler );
	}
	catch(...)
	{
		hr = E_UNEXPECTED;
	}

	// Force swap of all non-locked memory pages.  
	SetProcessWorkingSetSize( GetCurrentProcess(), -1, -1 );

	return hr;
}

/////////////////////////////////////////////////////////////////////////////
// CNAVAPService::UnRegisterHandler()

STDMETHODIMP CNAVAPService::UnRegisterHandler(INAVAPEventHandler *pHandler)
{
	// Always make sure the service is up and running before 
	// processing a command.
	_Module.WaitForStartup();

	HRESULT hr = S_OK;
	try
	{
		_Module.RemoveHandler( pHandler );
	}
	catch(...)
	{
		hr = E_UNEXPECTED;
	}

	// Force swap of all non-locked memory pages.  
	SetProcessWorkingSetSize( GetCurrentProcess(), -1, -1 );
	
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CNAVAPService::GetAutoProtectEnabled()

STDMETHODIMP CNAVAPService::GetAutoProtectEnabled( BOOL * pbEnabled )
{
	// Always make sure the service is up and running before 
	// processing a command.
	_Module.WaitForStartup();

	HRESULT hr = S_OK;
	try
	{
		*pbEnabled = _Module.IsAPEnabled() == true;
	}
	catch(...)
	{
		hr = E_FAIL;
	}
	
	// Force swap of all non-locked memory pages.  
	SetProcessWorkingSetSize( GetCurrentProcess(), -1, -1 );
	
	return hr;
}

/////////////////////////////////////////////////////////////////////////////
// CNAVAPService::SendCommand()

STDMETHODIMP CNAVAPService::SendCommand( BSTR pCommand, VARIANT vArrayData )
{
	HRESULT hr = S_OK;
	try
	{
		// Always make sure the service is up and running before 
		// processing a command.
		_Module.WaitForStartup();

		// Figure out which command was issued.
		if( lstrcmpW( NAVAPCMD_ENABLE, pCommand ) == 0 )
			_Module.EnableAP( true );
		else if( lstrcmpW( NAVAPCMD_DISABLE, pCommand ) == 0 )
			_Module.EnableAP( false );
		else if( lstrcmpW( NAVAPCMD_RESTART, pCommand ) == 0 )
			hr = _Module.ReloadAP(true) ? S_OK : E_FAIL ;  // Force AVAPI to reload when we are told to restart
		else if( lstrcmpW( NAVAPCMD_RELOADSETTINGS, pCommand ) == 0 )
			hr = _Module.ReloadOptions() ? S_OK : E_FAIL ;
		else if( lstrcmpW( NAVAPCMD_WAITFORSTARTUP, pCommand ) == 0 )
			hr = _Module.WaitForStartup();
        else if( lstrcmpW( NAVAPCMD_CANCELSCAN, pCommand ) == 0 )
            hr = _Module.CancelScan (vArrayData);
        else if( lstrcmpW( NAVAPCMD_MANUALSCANSPYWAREDETECTION, pCommand ) == 0 )
            hr = _Module.ManualScanSpywareDetection (vArrayData);
		else
			hr = E_INVALIDARG; // Unknown command.
	}
	catch(...)
	{
		return hr = E_UNEXPECTED;
	}

	// Force swap of all non-locked memory pages.  
	SetProcessWorkingSetSize( GetCurrentProcess(), -1, -1 );

	return hr;
}
