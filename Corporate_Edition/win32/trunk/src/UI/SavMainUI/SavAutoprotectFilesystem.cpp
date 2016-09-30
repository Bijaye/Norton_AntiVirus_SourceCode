// SavAutoprotectFilesystem.cpp : Implementation of CSavAutoprotectFilesystem

#include "stdafx.h"
#include "SavAutoprotectFilesystem.h"
#include "vpcommon.h"
#include "SavrtModuleInterface.h"
#include "ClientReg.h"
#include "ScanShared.h"
#include "SymSaferRegistry.h"
#include "FilesystemApPage.h"


// CSavAutoprotectFilesystem
CSavAutoprotectFilesystem::CSavAutoprotectFilesystem()
{
    // Nothing for now (should probably go in FinalConstruct instead)
}

HRESULT CSavAutoprotectFilesystem::FinalConstruct()
{
    HRESULT                         returnValHR         = E_FAIL;

    // Initialize trust checking
    returnValHR = trustVerifier.Initialize(CTrustVerifier::VerifyMode_CommonClient);
    return returnValHR;
}

void CSavAutoprotectFilesystem::FinalRelease()
{
    // Nothing for now
    trustVerifier.Shutdown();
}

// IPROTECTION_AUTOPROTECT
HRESULT CSavAutoprotectFilesystem::get_ID( GUID* id )
{
    // Validate parameters
    if (id == NULL)
        return E_POINTER;
    // Validate caller security
    if (trustVerifier.IsCallerProcessTrusted() != S_OK)
        return E_ACCESSDENIED;
    
    *id = CLSID_SavAutoprotectFilesystem;
    return S_OK;
}

HRESULT CSavAutoprotectFilesystem::get_DisplayName( BSTR* shortName )
{
    // Validate parameters
    if (shortName == NULL)
        return E_POINTER;
    // Validate caller security
    if (trustVerifier.IsCallerProcessTrusted() != S_OK)
        return E_ACCESSDENIED;

    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    CComBSTR nameBuffer;
    nameBuffer.LoadStringW(IDS_AP_FILESYSTEM_DISPLAYNAME);
    *shortName = nameBuffer.Detach();
    return S_OK;
}

HRESULT CSavAutoprotectFilesystem::get_Description( BSTR* userDescription )
{
    // Validate parameters
    if (userDescription == NULL)
        return E_POINTER;
    // Validate caller security
    if (trustVerifier.IsCallerProcessTrusted() != S_OK)
        return E_ACCESSDENIED;

    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    CComBSTR userString;
    userString.LoadStringW(IDS_AP_FILESYSTEM_DESCRIPTION);
    *userDescription = userString.Detach();
    return S_OK;
}

HRESULT CSavAutoprotectFilesystem::get_ProtectionStatus( ProtectionStatus* currStatus )
{
    SAVRT_PTR           pSAVRT(CSavrtModuleInterface::Init());
    IServiceControl*    savService;
    BOOL                apStatus                                = FALSE;
    DWORD               returnValDW                             = ERROR_OUT_OF_PAPER;
    HRESULT             returnValHR                             = E_FAIL;

    // Validate parmaeters
    if (currStatus == NULL)
        return E_POINTER;
    // Validate caller security
    if (trustVerifier.IsCallerProcessTrusted() != S_OK)
        return E_ACCESSDENIED;

    // First, check the service status
    // *** DML TEMP DEV
//    CSimplePerformanceTimer thisTimer;
//    CSimplePerformanceTimer createTimer;
//    thisTimer.StartTimer();
//    createTimer.StartTimer();
    // *** DML
//    returnValHR = CoCreateLDVPObject(CLSID_CliProxy, IID_IServiceControl, reinterpret_cast<void**>(&savService));
    // *** DML TEMP DEV
//    createTimer.StopTimer();
//    createTimer.OutputElapsedTime(_T("=> SAV FileSystem GetStatus Object Create"));
	returnValHR = S_OK;
    // ***
    if (SUCCEEDED(returnValHR))
    {
        // Determine if AP is ON or OFF
        if (pSAVRT.Get() != NULL)
        {
            // NOTE:  SAVRT does not pay attention to the ccVerifyTrust flag.  To use an unsigned client,
            // you must overwrite SAVRT32.DLL with the bin.ida version.
            // A return of 4 typically indicates this.
            returnValDW = pSAVRT->GetEnabledState(&apStatus);
            if (returnValDW == ERROR_SUCCESS)
            {
                if (apStatus)
                    *currStatus = ProtectionStatus_On;
                else
                    *currStatus = ProtectionStatus_Off;
            }
            else
            {
                *currStatus = ProtectionStatus_Error;
            }
            returnValHR = S_OK;
			pSAVRT->DecRef();
        }
//        savService->Release();
    }
    else
    {
        *currStatus = ProtectionStatus_Error;
        returnValHR = S_OK;
    }
    // *** DML TEMP DEV
//    thisTimer.StopTimer();
  //  thisTimer.OutputElapsedTime(_T("=> SAV FileSystem GetStatus"));
    // ***
    return returnValHR;
}

HRESULT CSavAutoprotectFilesystem::get_ProtectionStatusDescription( BSTR* statusDescription )
{
    ProtectionStatus    currProtectionStatus;
    CComBSTR            protectionStatusDescription;
    HRESULT             returnValHR                     = E_FAIL;

    // Validate parmaeters
    if (statusDescription == NULL)
        return E_POINTER;
    // Validate caller security
    if (trustVerifier.IsCallerProcessTrusted() != S_OK)
        return E_ACCESSDENIED;

    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    returnValHR = get_ProtectionStatus(&currProtectionStatus);
    if (SUCCEEDED(returnValHR) && (currProtectionStatus != ProtectionStatus_Error))
    {
        protectionStatusDescription = _T("OK");
    }
    else
    {
        protectionStatusDescription = _T("Unknown error");
    }
    *statusDescription = protectionStatusDescription.Detach();
    returnValHR = S_OK;
    return returnValHR;
}

HRESULT CSavAutoprotectFilesystem::get_ProtectionStatusReadOnly( VARIANT_BOOL* isStatusReadOnly )
{
    IServiceControl*    savService;
    DWORD               returnValDW         = ERROR_SERVICE_NOT_RUNNING;
    HRESULT             returnValHR         = E_FAIL;

    // Validate parmaeter
    if (isStatusReadOnly == NULL)
        return E_POINTER;
    // Validate caller security
    if (trustVerifier.IsCallerProcessTrusted() != S_OK)
        return E_ACCESSDENIED;

    // Determine if the service is running or not
    returnValHR = CoCreateLDVPObject(CLSID_CliProxy, IID_IServiceControl, reinterpret_cast<void**>(&savService));
    if (SUCCEEDED(returnValHR))
    {
        returnValDW = savService->GetStatus();
        if (returnValDW == ERROR_SUCCESS)
            returnValHR = S_OK;
        else
            returnValHR = E_FAIL;
        // Status description text should reflect RTVScan not running somehow
        savService->Release();
    }

    // Return result.  We need to always succeed.
    if (SUCCEEDED(returnValHR))
        *isStatusReadOnly = VARIANT_FALSE;
    else
        *isStatusReadOnly = VARIANT_TRUE;
    return S_OK;
}

HRESULT CSavAutoprotectFilesystem::get_ProtectionConfiguration( VARIANT_BOOL* protectionEnabled )
{
    HKEY        keyHandle       = NULL;
    BOOL        valueData       = FALSE;
    DWORD       valueDataType   = REG_SZ;
    DWORD       valueDataSize   = 0;
    DWORD       returnValDW     = ERROR_OUT_OF_PAPER;

    // Validate parameter
    if (protectionEnabled == NULL)
        return E_POINTER;
    // Validate caller security
    if (trustVerifier.IsCallerProcessTrusted() != S_OK)
        return E_ACCESSDENIED;

    returnValDW = RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T(szReg_Key_Main) _T("\\") _T(szReg_Key_Storages) _T("\\") _T(szReg_Key_Storage_File) _T("\\") _T(szReg_Key_Storage_RealTime), NULL, KEY_READ, &keyHandle);
    if (returnValDW == ERROR_SUCCESS)
    {
        valueDataSize = sizeof(valueData);
        returnValDW = SymSaferRegQueryValueEx(keyHandle, _T(szReg_Val_RTSScanOnOff), NULL, NULL, reinterpret_cast<BYTE*>(&valueData), &valueDataSize);
        if (returnValDW == ERROR_SUCCESS)
            *protectionEnabled = valueData;
        RegCloseKey(keyHandle);
        keyHandle = NULL;
    }

    return HRESULT_FROM_WIN32(returnValDW);
}

HRESULT CSavAutoprotectFilesystem::put_ProtectionConfiguration( VARIANT_BOOL newStatus )
{
    HKEY        keyHandle       = NULL;
    BOOL        newStatusBOOL   = FALSE;
    DWORD       returnValDW     = ERROR_OUT_OF_PAPER;
    HRESULT     returnValHR     = E_FAIL;

    // Validate caller security
    if (trustVerifier.IsCallerProcessTrusted() != S_OK)
        return E_ACCESSDENIED;

    // Set the configuration setting
    returnValDW = RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T(szReg_Key_Main) _T("\\") _T(szReg_Key_Storages) _T("\\") _T(szReg_Key_Storage_File) _T("\\") _T(szReg_Key_Storage_RealTime), NULL, KEY_WRITE, &keyHandle);
    if (returnValDW == ERROR_SUCCESS)
    {
        newStatusBOOL = newStatus;
        returnValDW = RegSetValueEx(keyHandle, _T(szReg_Val_RTSScanOnOff), NULL, REG_DWORD, reinterpret_cast<BYTE*>(&newStatusBOOL), sizeof(newStatusBOOL));
        RegCloseKey(keyHandle);
        keyHandle = NULL;
    }
    returnValHR = HRESULT_FROM_WIN32(returnValDW);

    // Wait for AP to enable, making this API atomic
    if (returnValDW == ERROR_SUCCESS)
    {
        ProtectionStatus        currStatus          = ProtectionStatus_Error;
        ProtectionStatus        desiredStatus       = ProtectionStatus_On;
        DWORD                   startTime           = 0;
        DWORD                   currTime            = 0;
        const DWORD             timeout             = 30000;    // Enabling AP normally takes 8 seconds
        DWORD                   elapsedTime         = 0;
        HRESULT                 returnValHR         = E_FAIL;

        if (!newStatus)
            desiredStatus = ProtectionStatus_Off;
        startTime = GetTickCount();
        get_ProtectionStatus(&currStatus);
        while ((currStatus != desiredStatus) && (elapsedTime < timeout))
        {
            get_ProtectionStatus(&currStatus);

            currTime = GetTickCount();
            if (currTime >= startTime)
                elapsedTime = currTime - startTime;
            else
                elapsedTime = (0xffffffff - startTime) + currTime;
            Sleep(100);
        }
        if (currStatus == desiredStatus)
            returnValHR = S_OK;
        else
            returnValHR = E_FAIL;
    }

    return returnValHR;

    // The following is swiped from VPTray.  However, since we are switching to the Sygate management system,
    // we shouldn't need to worry about config objects anymore.  There sole purpose was to 
    /*
    SAVRT_PTR   pSAVRT(CSavrtModuleInterface::Init());
    IScanConfig*    thisScanConfig                          = NULL;
    DWORD       newAPstatusDW                           = FALSE;
    BOOL        lockValue                               = FALSE;
    DWORD       returnValDW                             = ERROR_OUT_OF_PAPER;
    HRESULT     returnValHR                             = E_FAIL;

    // Reconfigure AP
    if (pSAVRT.Get() != NULL)
    {
        returnValHR = CreateScanConfig(CLSID_CliProxy, HKEY_VP_STORAGE_ROOT, szReg_Key_Storage_File, thisScanConfig);
        if (returnValHR == S_OK)
        {
            returnValHR = thisScanConfig->SetSubOption(szReg_Key_Storage_RealTime);
            if (returnValHR == S_OK)
            {
                // Are we allowed to modify the RTS OnOff value?
                returnValHR = thisScanConfig->GetIntOption(szReg_Val_RTSScanOnOff "-L", (long*)&lockValue, FALSE);
                if (lockValue == 0)
                {
                    // Yes
                    newAPstatusDW = newStatus;
                    returnValHR = thisScanConfig->SetOption(szReg_Val_RTSScanOnOff, REG_DWORD, (BYTE*)&newAPstatusDW, sizeof(newAPstatusDW));
                }
            }
            thisScanConfig->Release();
        }
    }
    return returnValHR;
    */
}

HRESULT CSavAutoprotectFilesystem::ShowConfigureUI( HWND parentWindowHandle )
{
    // Validate caller security
    if (trustVerifier.IsCallerProcessTrusted() != S_OK)
        return E_ACCESSDENIED;

    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    CPropertySheet configWizard;
    CFilesystemApPage filesystemAP;

    configWizard.AddPage(&filesystemAP);
    configWizard.DoModal();

    return S_OK;
}

HRESULT CSavAutoprotectFilesystem::ShowLogUI( HWND parentWindowHandle )
{
    // Validate caller security
    if (trustVerifier.IsCallerProcessTrusted() != S_OK)
        return E_ACCESSDENIED;

    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    MessageBox(parentWindowHandle, _T("Visualize UI controls here"), _T("Log UI"), MB_OK | MB_ICONINFORMATION);

    return E_NOTIMPL;
}