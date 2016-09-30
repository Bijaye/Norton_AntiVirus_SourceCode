////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////
// LUInstall.cpp
//
// --Contains exported functions used in MSI custom actions for installing LiveUpdate
//
/////////////////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "helper.h"
#include "NAVCommandLines.h"
#include "NetDetectController.h"                // For AutoLiveUpdate
#include "NetDetectController_i.c"
#include "LiveUpdateWrap.h"
#include "CustomAction.h"

/////////////////////////////////////////////////////////////////////////////////////////////
// RegisterWithLU(): This exported function register NAV with LiveUpdate
//
/////////////////////////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport) UINT __stdcall RegisterWithLU(MSIHANDLE hInstall)
{
#ifdef _DEBUG
	MessageBox(NULL, "Attach to this MessageBox to Debug RegisterWithLU()", "Debug Break", NULL);
#endif

	InstallToolBox::CCustomAction::LogMessage(hInstall,
		InstallToolBox::CCustomAction::LogSeverity::logStatus,
		_T("RegisterWithLU() Starting."));
	
	try
	{
		HRESULT hResult = S_OK;
		hResult = CoInitialize(NULL);
		if(SUCCEEDED(hResult))
		{
			//Add error checking
			CNavCommandLines cmdLines;
			HRESULT hr = cmdLines.RegisterCommandLines();

			if (SUCCEEDED(hResult))
			{
				CoUninitialize();
			}
			else
			{
				InstallToolBox::CCustomAction::LogMessage(hInstall,
					InstallToolBox::CCustomAction::LogSeverity::logStatus,
					_T("RegisterWithLU: Failed to register"));
			}
		}
		else
		{
		InstallToolBox::CCustomAction::LogMessage(hInstall,
			InstallToolBox::CCustomAction::LogSeverity::logError,
			_T("Error when CoInitialize (0x%08X)"), hResult);

		}
	}
	catch(HRESULT& hr)
	{
		_ASSERT(FALSE);
		InstallToolBox::CCustomAction::LogMessage(hInstall,
			InstallToolBox::CCustomAction::LogSeverity::logStatus,
			_T("_com_error Exception in RegisterWithLU() (0x%08X)"), hr);
	}
	catch(_com_error &ce)
	{
		InstallToolBox::CCustomAction::LogMessage(hInstall,
			InstallToolBox::CCustomAction::LogSeverity::logStatus,
			_T("_com_error Exception in RegisterWithLU() (0x%08X)"), ce.Error());
	}
	catch(exception &ex)
	{
		InstallToolBox::CCustomAction::LogMessage(hInstall,
			InstallToolBox::CCustomAction::LogSeverity::logStatus,
			ex.what());
	}

	InstallToolBox::CCustomAction::LogMessage(hInstall,
		InstallToolBox::CCustomAction::LogSeverity::logStatus,
		_T("RegisterWithLU() Finished."));

	return ERROR_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////////////////////
// UnregisterWithLU(): This exported function unregister NAV with LiveUpdate
//
/////////////////////////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport) UINT __stdcall UnregisterWithLU(MSIHANDLE hInstall)
{
#ifdef _DEBUG
	MessageBox(NULL, "Attach to this MessageBox to Debug UnregisterWithLU()", "Debug Break", NULL);
#endif

	InstallToolBox::CCustomAction::LogMessage(hInstall,
		InstallToolBox::CCustomAction::LogSeverity::logStatus,
		_T("UnregisterWithLU() Starting."));
	

	HRESULT hResult = S_OK;
	hResult = CoInitialize(NULL);

	CNavCommandLines cmdLines;
	HRESULT hr = cmdLines.UnregisterCommandLines();

	if (SUCCEEDED(hResult))
		CoUninitialize();
	
	InstallToolBox::CCustomAction::LogMessage(hInstall,
		InstallToolBox::CCustomAction::LogSeverity::logStatus,
		_T("UnregisterWithLU() Finished."));

	return ERROR_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////////////////////
//  SetupAutoLiveUpdate(): Setup AutoLiveUpdate with its Default Settings
//
/////////////////////////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport) BOOL __stdcall SetupAutoLiveUpdate(MSIHANDLE hInstall)
{
#ifdef _DEBUG
	MessageBox(NULL, "Attach to this MessageBox to Debug SetupAutoLiveUpdate()", "Debug Break", NULL);
#endif

	InstallToolBox::CCustomAction::LogMessage(hInstall,
		InstallToolBox::CCustomAction::LogSeverity::logStatus,
		_T("SetupAutoLiveUpdate() Starting."));
	
	BOOL bResult;
	HRESULT hrCoInit = E_FAIL;

	try
	{
        // Add an ALU scheduler task directly instead going through Aupdate.exe.
		hrCoInit = CoInitialize(NULL);

		if(FAILED(hrCoInit))
			throw runtime_error("CoInitialize failed.");

		CComPtr<INDScheduler> spScheduler;

		if(FAILED(spScheduler.CoCreateInstance(CLSID_NDScheduler, NULL, CLSCTX_INPROC_SERVER)))
			throw runtime_error("Unable to create the NSDScheduler object.");

		if(FAILED(spScheduler->Init()))
			throw runtime_error("spScheduler->Init() failed.");

		// Enable() failed because it's not there
		if(FAILED(spScheduler->AddTask()))
		{
			// Failed to add, it's probably because it exists already.
			// Try to make it go again.
			spScheduler->RestartSchedule();
		}

		if(FAILED(spScheduler->Disable()))
			throw runtime_error("spScheduler->Disable() failed.");
	}

	catch(_com_error &ce)
	{
		bResult = false;
		InstallToolBox::CCustomAction::LogMessage(hInstall,
			InstallToolBox::CCustomAction::LogSeverity::logStatus,
			_T("_com_error Exception in SetupAutoLiveUpdate. (0x%08X)"), ce.Error());
	}
	catch(exception& ex)
	{
		bResult = false;		
		InstallToolBox::CCustomAction::LogMessage(hInstall,
			InstallToolBox::CCustomAction::LogSeverity::logStatus,
			ex.what());
	}
	
	if(SUCCEEDED(hrCoInit))
	{
		CoUninitialize();
	}

	InstallToolBox::CCustomAction::LogMessage(hInstall,
		InstallToolBox::CCustomAction::LogSeverity::logStatus,
		_T("SetupAutoLiveUpdate() Finished."));

	return ERROR_SUCCESS;
}