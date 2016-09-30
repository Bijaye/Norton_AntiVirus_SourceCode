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

/////////////////////////////////////////////////////////////////////////////////////////////
// RegisterWithLU(): This exported function register NAV with LiveUpdate
//
/////////////////////////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport) UINT __stdcall RegisterWithLU(MSIHANDLE hInstall)
{
#ifdef _DEBUG
	MessageBox(NULL, "Attach to this MessageBox to Debug RegisterWithLU()", "Debug Break", NULL);
#endif

	g_Log.Log("RegisterWithLU() Starting.");
	
	try
	{
		HRESULT hResult = S_OK;
		hResult = CoInitialize(NULL);
		  
        // wait for 10 seconds
        InstallToolBox::CLiveUpdate LU;
        LU.CheckForLUMutex();

		CNavCommandLines cmdLines;
		HRESULT hr = cmdLines.RegisterCommandLines();
			
		if (SUCCEEDED(hResult))
		{
			CoUninitialize();
		}
		else
		{
			g_Log.Log ("RegisterWithLU: Failed to register");
		}
	}
	catch(exception &ex)
	{
		g_Log.LogEx(ex.what());
	}
	catch(...)
	{
		g_Log.LogEx("Unknown Exception in RegisterWithLU()");
	}

	g_Log.Log("RegisterWithLU() Finished.");

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

	g_Log.Log("UnregisterWithLU() Starting.");
	

	HRESULT hResult = S_OK;
	hResult = CoInitialize(NULL);

	CNavCommandLines cmdLines;
	HRESULT hr = cmdLines.UnregisterCommandLines();

	if (SUCCEEDED(hResult))
		CoUninitialize();
	
	g_Log.Log("UnregisterWithLU() Finished.");

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

	g_Log.Log("SetupAutoLiveUpdate() Starting.");
	
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

	catch(exception& ex)
	{
		bResult = false;		
		g_Log.LogEx(ex.what());
	}
	catch(...)
	{
		bResult = false;
		g_Log.LogEx("Unknown Exception in SetupAutoLiveUpdate");
	}
	
	if(SUCCEEDED(hrCoInit))
	{
		CoUninitialize();
	}

	g_Log.Log("SetupAutoLiveUpdate() Finished.");

	return ERROR_SUCCESS;
}