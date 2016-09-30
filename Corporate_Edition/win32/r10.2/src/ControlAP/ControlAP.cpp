// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2006 Symantec Corporation. All rights reserved.
// ControlAP.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include "atlstr.h"

#include "vpcommon.h"
#include "pscan.h"
#include "ClientReg.h"
#include "SymSaferRegistry.h"
#include "OSUtils.h"
#include "VPExceptionHandling.h"

#include "resource.h"
#include "ControlAP.h"
#include "SrtValues.h"


#define szReg_ValSuffix_Lock  "-L"

VPREG VpRegBase[] = VP_REG_BASE_TABLE;


/** This executable sets the file system AP state.  For info on parameters, see
  * usage info below.
  * @returns 0 on success, other values on error
  */
int _tmain(int argc, _TCHAR* argv[])
{
	// Get the command.
	bool  bValidCommand = false;
	DWORD dwEnableAP    = 2;    // We will use a tristate, where 0 is disable, 1 is enable, 2 don't modify
    DWORD dwEnableSRS   = 2;    // Flag used for turning on/off Security Risk Scanning

	if ( (2 == argc || 3 == argc ) && argv[1] != NULL)
	{
		if ((_tcsicmp(argv[1], CONTROLAP_PARAM_ENABLE_AP) == 0) || 
           ( argc == 3 && _tcsicmp(argv[2], CONTROLAP_PARAM_ENABLE_AP) == 0))
		{
			bValidCommand = true;
			dwEnableAP    = TRUE;
		}
		if ((_tcsicmp(argv[1], CONTROLAP_PARAM_DISABLE_AP) == 0) ||
            ( argc == 3 && _tcsicmp(argv[2], CONTROLAP_PARAM_DISABLE_AP) == 0))
		{
			bValidCommand = true;
			dwEnableAP    = FALSE;
		}
        if ((_tcsicmp(argv[1], CONTROLAP_PARAM_ENABLE_RISKSCAN ) == 0 ) ||
            ( argc == 3 && _tcsicmp(argv[2], CONTROLAP_PARAM_ENABLE_RISKSCAN) == 0))
        {
            bValidCommand = true;
            dwEnableSRS = TRUE;
        }
        if (( _tcsicmp(argv[1], CONTROLAP_PARAM_DISABLE_RISKSCAN) == 0 ) ||
            ( argc == 3 && _tcsicmp(argv[2], CONTROLAP_PARAM_DISABLE_RISKSCAN) == 0))
        {
            bValidCommand = true;
            dwEnableSRS = FALSE;
        }
	}
	// If a command wasn't specified properly, display usage info and exit.
	if (!bValidCommand)
	{
		CString strError;

		try
		{
			strError.Format(IDS_USAGE_INFO,
							CONTROLAP_PARAM_ENABLE_AP,
							CONTROLAP_PARAM_DISABLE_AP,
                            CONTROLAP_PARAM_ENABLE_RISKSCAN,
                            CONTROLAP_PARAM_DISABLE_RISKSCAN);
			_tprintf(strError);
		}
		VP_CATCH_MEMORYEXCEPTIONS(;)
		return -1;
	}
	// Open the registry key for AP.
	HKEY    hAPKey = NULL;
    CString strAPKey;
	DWORD   dwRet = ERROR_SUCCESS;

	try
	{
		strAPKey.Format(VpRegBase[HKEY_VP_STORAGE_REALTIME&0xFF].Key,
						szReg_Key_Storage_File);
		dwRet = RegOpenKeyEx(VpRegBase[HKEY_VP_STORAGE_REALTIME&0xFF].hBase,
							 strAPKey,
							 0,
							 KEY_READ | KEY_WRITE,
							 &hAPKey);
	}
	VP_CATCH_MEMORYEXCEPTIONS
	(
		dwRet = ERROR_MEMORY;
	)
	if (dwRet != ERROR_SUCCESS)
	{
		CString strError;

		try
		{
			strError.Format(IDS_ERROR_OPENINGKEY, strAPKey, dwRet);
			_tprintf(strError);
		}
		VP_CATCH_MEMORYEXCEPTIONS(;)
		return -1;
	}
	// On XP, we should not toggle AP if it's locked.
	// On Vista, we'll let them override it, since they must have supplied
	// administrative credentials to be running this exe.
	// Check to see if AP is locked.
	if (!IsWindowsVista())
	{
        DWORD dwLock = FALSE;
		DWORD dwType = REG_DWORD;
		DWORD nBytes = sizeof(dwType);

        // Check if AP settings are locked
        if ( dwEnableAP != 2 ) {
	        SymSaferRegQueryValueEx(hAPKey,
								    szReg_Val_RTSScanOnOff szReg_ValSuffix_Lock,
								    0,
								    &dwType,
								    reinterpret_cast<BYTE*>(&dwLock),
								    &nBytes);
			    // Failure is ok.  If the lock value is missing, the it's effectively off.
		    if (REG_DWORD == dwType && sizeof(dwType) == nBytes && dwLock)
		    {
			    CString strSuccess;

			    try
			    {
				    strSuccess.LoadString(IDS_SUCCESS_AP_LOCK);
				    _tprintf(strSuccess);
			    }
			    VP_CATCH_MEMORYEXCEPTIONS(;)
			    return -2;
		    }
        }

        // Check if Security Risk Scanning is locked
        if ( dwEnableSRS != 2 ) {
            dwLock = FALSE;
	        SymSaferRegQueryValueEx(hAPKey,
								    szReg_Val_RespondToThreats szReg_ValSuffix_Lock,
								    0,
								    &dwType,
								    reinterpret_cast<BYTE*>(&dwLock),
								    &nBytes);
			// Failure is ok.  If the lock value is missing, the it's effectively off.
		    if (REG_DWORD == dwType && sizeof(dwType) == nBytes && dwLock)
		    {
			    CString strSuccess;

			    try
			    {
				    strSuccess.LoadString(IDS_SUCCESS_SRS_LOCK);
				    _tprintf(strSuccess);
			    }
			    VP_CATCH_MEMORYEXCEPTIONS(;)
			    return -2;
		    }
        }
	}

    try 
    {
        if ( dwEnableAP != 2 ) 
        {
	        // Set the AP registry value.
	        dwRet = RegSetValueEx(hAPKey,
						        szReg_Val_RTSScanOnOff,
						        0,
						        REG_DWORD,
						        reinterpret_cast<const BYTE*>(&dwEnableAP),
						        sizeof(dwEnableAP));

	        if (dwRet != ERROR_SUCCESS)
	        {
		        CString strError;
			    strError.Format(IDS_ERROR_SETTINGVALUE,
							    szReg_Val_RTSScanOnOff,
							    dwEnableAP,
							    dwRet);
			    _tprintf(strError);
                RegCloseKey(hAPKey);
		        return -1;
	        }
	        // Success!
	        CString strSuccess;
	        strSuccess.LoadString(IDS_SUCCESS_AP);
	        _tprintf(strSuccess);
        }

        if ( dwEnableSRS != 2 )
        {
            // Let's read the current value
            DWORD dwVal = 0;
            DWORD dwType = REG_DWORD;
            DWORD nBytes = sizeof(dwType);
            SymSaferRegQueryValueEx(hAPKey,
								    szReg_Val_RespondToThreats,
								    0,
								    &dwType,
								    reinterpret_cast<BYTE*>(&dwVal), 
								    &nBytes);

            if ( dwEnableSRS == TRUE ) {
                dwEnableSRS = dwVal | SRT_THREAT_TYPES_TO_SCAN_NONVIRAL;
            }
            else {
                dwEnableSRS = dwVal ^ SRT_THREAT_TYPES_TO_SCAN_NONVIRAL;
            }

            dwRet = RegSetValueEx(hAPKey,
					szReg_Val_RespondToThreats,
					0,
					REG_DWORD,
					reinterpret_cast<const BYTE*>(&dwEnableSRS),
					sizeof(dwEnableSRS));
	        
	        if (dwRet != ERROR_SUCCESS)
	        {
		        CString strError;
			    strError.Format(IDS_ERROR_SETTINGVALUE,
							    szReg_Val_RespondToThreats,
							    dwEnableSRS,
							    dwRet);
			    _tprintf(strError);
                RegCloseKey(hAPKey);
		        return -1;
	        }

	        // Success!
	        CString strSuccess;
	        strSuccess.LoadString(IDS_SUCCESS_SRS);
	        _tprintf(strSuccess);
        }

    }

	VP_CATCH_MEMORYEXCEPTIONS(;)

    RegCloseKey(hAPKey);
    return 0;
}
