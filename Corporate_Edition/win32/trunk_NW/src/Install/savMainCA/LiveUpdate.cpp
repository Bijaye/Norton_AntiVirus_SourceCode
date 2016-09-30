// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#include "stdafx.h"
#include "savMainCA.h"

// The LuComServer type library
#import "..\..\..\..\..\Release\LiveUpdate\include\LuCom.tlb"

// The LuComServer COM include file.
#include "..\..\..\..\..\Release\LiveUpdate\include\LuCom.h"

//////////////////////////////////////////////////////////////////////////
//
// Function: SetLiveUpdateSettings
//
// Parameters:
//		MSIHANDLE - Handle to the current install session
//
// Returns:
//		UINT - ERROR_SUCCESS always.
//
// Description:
//		This custom action will set LU to run in Corporate mode, as
//		well as disable LU Express.
//
//////////////////////////////////////////////////////////////////////////
// 02/10/04 - Thomas Brock - function created.
//////////////////////////////////////////////////////////////////////////
UINT __stdcall SetLiveUpdateSettings( MSIHANDLE hInstall )
{
	HRESULT hr;

	hr = CoInitialize( NULL );
	if( SUCCEEDED( hr ) )
	{
        try
        {
            STLIVEUPDATECOMLib::IstCheckForUpdatesPtr ptrLiveUpdate;
            STLIVEUPDATECOMLib::IstSettingsPtr ptrSettings;

            // Define these after CoInit
            const _bstr_t LUCONST_NO = L"NO";
            const _bstr_t bstrPREFERENCES		= L"PREFERENCES"; 
            const _bstr_t bstrPREFS_ENVIRONMENT = bstrPREFERENCES + L"\\ENVIRONMENT"; 
            const _bstr_t bstrENVIRONMENT_CORPORATE = L"CORPORATE";
            const _bstr_t bstrEXPRESS_MODE = L"\\EXPRESS_MODE";
            const _bstr_t bstrPREFS_EXPRESS_MODE_ON = bstrPREFERENCES + bstrEXPRESS_MODE + L"\\ENABLED";
            const _bstr_t bstrPREFS_EXPRESS_MODE_AUTO_START = bstrPREFERENCES + bstrEXPRESS_MODE + L"\\AUTO_START";
            const _bstr_t bstrPREFS_EXPRESS_MODE_AUTO_EXIT = bstrPREFERENCES + bstrEXPRESS_MODE + L"\\AUTO_EXIT";

            // Create the the LiveUpdate CheckForUpdates object
            hr = ptrLiveUpdate.CreateInstance( __uuidof( stCheckForUpdates ), NULL, CLSCTX_INPROC_SERVER | CLSCTX_LOCAL_SERVER );
            if( SUCCEEDED( hr ) )
            {
                // Load the settings file from disk.
                hr = ptrLiveUpdate->LoadSettings();
                if( SUCCEEDED( hr ) )
                {
                    // Get a pointer to the Settings file object.
                    ptrSettings = ptrLiveUpdate->GetSettings();
                    if( ptrSettings != NULL )
                    {
                        // Set the Environment property
                        ptrSettings->PutProperty( bstrPREFS_ENVIRONMENT, bstrENVIRONMENT_CORPORATE );

                        // Turn of LU express
                        ptrSettings->PutProperty( bstrPREFS_EXPRESS_MODE_ON, LUCONST_NO );
                        ptrSettings->PutProperty( bstrPREFS_EXPRESS_MODE_AUTO_START, LUCONST_NO );
                        ptrSettings->PutProperty( bstrPREFS_EXPRESS_MODE_AUTO_EXIT, LUCONST_NO );

                        MSILogMessage( hInstall, _T("Successfully configured LiveUpdate for Corporate mode operation.") );
                    }
                }
            }
        }
        catch(...)
        {
            MSILogMessage(hInstall, _T("SetliveUpdateSettings:  FAILED, exception occured during processing."));
        }
		CoUninitialize();
	}
	return ERROR_SUCCESS;
}