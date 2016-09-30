// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// SymProtectStorage.cpp : Defines the entry point for the DLL application.

// Due to a problem with ATL caused by Common Client includes, we cannot implement RegisterServer/UnregistersServer through the storage class.
// Common Client includes ATLBASE, which declares a template with this export, although it is not actually exported.  However, the compiler refuses
// to allow another definition of DllRegisterServer/DllUnregisterServerif ATLBASE.H is #included in the same module.  Since CSymProtectStorage uses
// Common Client, there is therefore no way to reference CSymProtectStorage directly from here.

#define INITIIDS
#define _ATL_NO_COMMODULE
#define _WIN32_DCOM
#define INCL_WINSOCK_API_TYPEDEFS 1
#include <winsock2.h>
#include <windows.h>
#include <msi.h>
#include <msiquery.h>
#include <aclapi.h>
#include <list>
#include <conio.h>
#include "stdafx.h"
#include "ccSymDebugOutput.h"
#include "ccSettingsManagerHelper.h"
#include "ccCoInitialize.h"
#include "ccEventManagerHelper.h"
#include "ccSubscriberHelper.h"
#include "ccEventFactoryHelper.h"
#include "SymProtectEventsInterface.h"
#include "SymProtectStorage.h"
#include "CRTVScanStorage.h"
#include "SymProtectController.h"
#include "resource.h"
#include "acta.h"
#include "vpstrutils.h"
#include "navver.h"
#include "SymSaferRegistry.h"
#include "password.h"
#include "SymProtectInstallHelper.h"
#include "VPExceptionHandling.h"
#include "bbsystemeventsinterface.h"

// ** CONSTANTS **
const LPCTSTR CSymProtectStorage::FilterExecutable_XTAgent				    = "XTAgent.exe";
const LPCTSTR CSymProtectStorage::FilterExecutable_NalAgent				    = "NalAgent.exe";
const LPCTSTR CSymProtectStorage::FilterExecutable_WMIadap                  = "\\WBEM\\WMIADAP.EXE";
const LPCTSTR CSymProtectStorage::FilterExecutable_WMIapsvr                 = "\\WBEM\\WMIAPSVR.EXE";
const LPCTSTR CSymProtectStorage::FilterExecutable_WMIwmic                  = "\\WBEM\\WMIC.EXE";
const LPCTSTR CSymProtectStorage::FilterExecutable_WMIprvse                 = "\\WBEM\\WMIPRVSE.EXE";
const LPCTSTR CSymProtectStorage::FilterExecutable_BlastCln                 = "\\blastcln.exe";
const LPCTSTR CSymProtectStorage::FilterExecuteable_MicrosoftUpdate         = "mrt.exe";
const LPCTSTR CSymProtectStorage::FilterExecutable_Accessibility_Narrator   = "\\narrator.exe";
const LPCTSTR CSymProtectStorage::FilterExecutable_Accessibility_Magnify    = "\\magnify.exe";
const LPCTSTR CSymProtectStorage::FilterExecutable_WholeSecurity_KeyPath    = "SYSTEM\\CurrentControlSet\\Services\\ConfidenceOnlineEE";
const LPCTSTR CSymProtectStorage::FilterExecutable_WholeSecurity_ValueName  = "ImagePath";
const LPCTSTR CSymProtectStorage::FilterExecutable_WholeSecurity_Service    = "WSService2K.exe";
const LPCTSTR CSymProtectStorage::FilterExecutable_WholeSecurity_ActiveX    = "WSOOPScan.exe";
const LPCTSTR CSymProtectStorage::DefaultAuthorization_DrWatson             = "\\drwtsn32.exe";
const LPCTSTR CSymProtectStorage::DefaultAuthorization_DumpPrep             = "\\dumprep.exe";
const LPCTSTR CSymProtectStorage::DefaultAuthorization_CSRSS_Prefix         = "\\??\\";
const LPCTSTR CSymProtectStorage::DefaultAuthorization_CSRSS_Filename       = "\\csrss.exe";
const LPCTSTR CSymProtectStorage::DefaultAuthorization_SMSS_Prefix          = "\\SystemRoot\\";
const LPCTSTR CSymProtectStorage::DefaultAuthorization_SMSS_Filename        = "\\smss.exe";
const LPCTSTR CSymProtectStorage::WriteCcSettingsDoneFlagKey			    = "Software\\SymantecVolatile\\Common Client\\ccSettings";

// ** GLOBALS **
ccSym::CDebugOutput		g_DebugOutput("outputdebugfilename");
IMPLEMENT_CCTRACE(g_DebugOutput)
HINSTANCE				CSymProtectStorage::moduleHandle			= NULL;

// ** MACROS **
#define SETLOGERRORSTART            if (FAILED(returnValHRb)) \
                                    { \
                                        storageManagerFunctions->dPrintf
#define SETLOGERROREND              ; \
                                        if (SUCCEEDED(returnValHR)) \
                                            returnValHR = returnValHRb; \
                                    }
#define SETLOGERROR1(formatString)                          SETLOGERRORSTART(formatString)SETLOGERROREND
#define SETLOGERROR2(formatString, a0)                      SETLOGERRORSTART(formatString, a0)SETLOGERROREND
#define SETLOGERROR3(formatString, a0, a1)                  SETLOGERRORSTART(formatString, a0, a1)SETLOGERROREND
#define SETLOGERROR4(formatString, a0, a1, a2)              SETLOGERRORSTART(formatString, a0, a1, a2)SETLOGERROREND
#define SETLOGERROR5(formatString, a0, a1, a2, a3)          SETLOGERRORSTART(formatString, a0, a1, a2, a3)SETLOGERROREND
#define SETLOGERROR6(formatString, a0, a1, a2, a3, a4)      SETLOGERRORSTART(formatString, a0, a1, a2, a3, a4)SETLOGERROREND
#define SETERROR                    if (FAILED(returnValHRb)) \
                                    { \
                                        if (SUCCEEDED(returnValHR)) \
                                            returnValHR = returnValHRb; \
                                    }

SYM_OBJECT_MAP_BEGIN()
SYM_OBJECT_MAP_END()

// ** EXPORTS **
extern "C"
{
	template DWORD _declspec(dllexport) StorageInit<CSymProtectStorage>( DWORD flags, PSTORAGEINFO* storageInfo, HANDLE* storageInstance, PSSFUNCTIONS storageManagerFunctions );
}


BOOL APIENTRY DllMain( HANDLE newModuleHandle, DWORD reasonCode, LPVOID lpReserved )
{
	switch (reasonCode)
	{
	case DLL_PROCESS_ATTACH:
		CSymProtectStorage::SetModuleHandle((HINSTANCE) newModuleHandle);
		DisableThreadLibraryCalls((HMODULE) newModuleHandle);
		break;
	case DLL_PROCESS_DETACH:
		break;
	}
    return TRUE;
}


// Storage Extension Functions (SFUNCTIONS implementations)
DWORD CSymProtectStorage::GetConfig( StorageConfig* config )
{
	TCHAR		filenameBuffer[2*MAX_PATH+1]	= {""};
	LPTSTR		lastBackslash					= NULL;
    DWORD       returnValDW                     = ERROR_BAD_PARAM;

	// Validate parameters
	if (config == NULL)
		return ERROR_BAD_PARAM;

    try
    {
        config->storageName = szReg_Key_Storage_SymProtect;
        config->type = 0x80000000 | IT_CAN_RTS | SNAP_IN_SYMPROTECT;
        config->entrypoint = "StorageInit";
        GetModuleFileName(moduleHandle, filenameBuffer, sizeof(filenameBuffer)/sizeof(filenameBuffer[0]));
        lastBackslash = (LPTSTR) _mbsrchr((unsigned char*) filenameBuffer, L'\\');
        if (lastBackslash != NULL)
        {
            *lastBackslash = NULL;
            config->dllPath = filenameBuffer;
            config->dllPath.push_back('\\');
            config->fileNodeName = lastBackslash + sizeof(char);
        }
        else
        {
            config->fileNodeName = filenameBuffer;
            config->dllPath = ".\\";
        }
        config->notifyEventA = GL_EVENT_SECURITY_SYMPROTECT_POLICYVIOLATION;
        config->notifyEventB = 0;
        config->notifyEventC = 0;
        config->notifyTitle = "";
        returnValDW = ERROR_SUCCESS;
    }
    VP_CATCH_MEMORYEXCEPTIONS(returnValDW = ERROR_MEMORY;);

	return returnValDW;
}

DWORD CSymProtectStorage::Initialize( void )
{
	DWORD       returnValDW     = ERROR_GENERAL;

	if (initialized)
		storageManagerFunctions->dPrintf("CSymProtectStorage::Initialize:  Called when already initialized!\n");

    // Initialize synchronization events
    InitializeCriticalSection(&importSettingsLock);
    InitializeCriticalSection(&configLock);

    // Create the RTS thread interrupt event
    rtsThreadInterrupt = CreateEvent(NULL, FALSE, FALSE, NULL);
    if (rtsThreadInterrupt != NULL)
    {
        returnValDW = ERROR_SUCCESS;
    }
    else
    {
        returnValDW = GetLastError();
        storageManagerFunctions->dPrintf("CSymProtectStorage::Initialize:  Error 0x%08x creating event.\n", HRESULT_FROM_WIN32(returnValDW));
    }

	// Cleanup and return result
	if (returnValDW == ERROR_SUCCESS)
        initialized = true;
    else
		DeInitialize();
	return returnValDW;
}

DWORD CSymProtectStorage::DeInitialize( void )
{
    if (!initialized)
        storageManagerFunctions->dPrintf("CSymProtectStorage::DeInitialize:  Called when not initialized!\n");

    // Release synchronization objects
    DeleteCriticalSection(&importSettingsLock);
    DeleteCriticalSection(&configLock);

    if (rtsThreadInterrupt != NULL)
    {
        CloseHandle(rtsThreadInterrupt);
        rtsThreadInterrupt = NULL;
    }
	initialized = false;

	return ERROR_SUCCESS;
}

DWORD CSymProtectStorage::BeginRTSWatch( PROCESSRTSNODE ProcessRTSNode, PSNODE currStorageNode, void* context )
{
    StorageConfig                           storageConfig;
    TCHAR                                   storageSettingsKeyPath[2*MAX_PATH+1]    = {""};
    HKEY                                    storageSettingsKeyHandle                = NULL;
    ccEvtMgr::CEventEx*                     newEvent                                = NULL;
    bool                                    gotItem                                 = false;
    bool                                    interruptEventSignaled                  = false;
    bool                                    spConfigured                            = false;
    bool                                    subscriberConnected                     = false;
    DWORD                                   timeoutAdjustment                       = 0;
    DWORD                                   loopWait                                = INFINITE;
    SymProtectEvt::CPolicyEventExPtr        policyEvent;
    SymProtectEventInfo*                    newEventInfo                            = NULL;
    bool                                    excludeEvents                           = false;
    bool                                    excludeEvent                            = false;
    std::string                             nodeName;
    PSID                                    ownerSID                                = NULL;
    PSECURITY_DESCRIPTOR                    descriptor                              = NULL;
    SID_NAME_USE                            accountType                             = SidTypeInvalid;
    HANDLE                                  processHandle                           = NULL;
    ISymProtectController*                  spController                            = NULL;
    ISymProtectController::ErrorLocation    errorLocation                           = ISymProtectController::ErrorLocation_None;
    HRESULT                                 returnValHR                             = E_FAIL;
    HRESULT                                 returnValHRb                            = E_FAIL;
    SYMRESULT                               returnValSR                             = SYMERR_UNKNOWN;
    DWORD                                   returnValDW                             = ERROR_FILE_NOT_FOUND;

    // Validate state
    runRTSwatch = true;
    if (!initialized)
    {
        storageManagerFunctions->dPrintf("CSymProtectStorage::BeginRTSWatch:  CALLED WHEN NOT INITIALIZED.\n");
        return ERROR_GENERAL;
    }

    // Initialize COM
    returnValHR = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    if (FAILED(returnValHR))
    {
        storageManagerFunctions->dPrintf("CSymProtectStorage::ReloadRTSConfig:  CoInitializeEx failed 0x%08x.\n", returnValHR);
        return ERROR_GENERAL;
    }

    // Open storage configuration key
    returnValHR = GetConfig(&storageConfig);
    if (FAILED(returnValHR))
    {
        CoUninitialize();
        return returnValHR;
    }
    sssnprintf(storageSettingsKeyPath, sizeof(storageSettingsKeyPath), "%s\\%s", storageConfig.storageName.c_str(), szReg_Key_Storage_RealTime);
    returnValDW = storageManagerFunctions->Open(reinterpret_cast<HKEY>(HKEY_VP_STORAGE_ROOT), storageSettingsKeyPath, &storageSettingsKeyHandle);
    if (returnValDW != ERROR_SUCCESS)
        storageManagerFunctions->dPrintf("CSymProtectStorage::BeginRTSWatch:  Error %d opening storage settings key.\n", returnValDW);

    // Process events we receive
    while (runRTSwatch)
    {
        // Loop initialization
        interruptEventSignaled = false;

        // Connect to ccEventManager
        if (!subscriberConnected)
        {
            returnValHR = SubscriberInitialize();
            if (SUCCEEDED(returnValHR))
            {
                subscriberConnected = true;
                // Always reconfigure if we have to reconnect
                spConfigured = false;
            }
        }

        // Start SymProtect if it is not already running
        if (!spConfigured)
        {
            // Try to turn on SymProtect and update all settings
            returnValHR = ImportSettings();
            if (SUCCEEDED(returnValHR))
                spConfigured = true;
            else
                storageManagerFunctions->dPrintf("CSymProtectStorage::BeginRTSWatch:  ImportSettings failed.\n");
        }

        // If either of the above failed, use a non-infinite timeout for the wait so we can retry to repair things
        // after an appropriate delay
        if (spConfigured && subscriberConnected)
        {
            loopWait = INFINITE;
            timeoutAdjustment = 0;
        }
        else
        {
            // First failure is likely a transcient problem because event manager isn't setup yet, so retry but not too fast.
            // If that fails, maybe this is a really slow box, so back off to 5 minute polls.
            // If that fails, our standard reconnection attempt is once every hour.  Chances are here something is quite damaged
            // and the process of connecting might not be beneficial, so don't be too agressive about it.  Most likely it would
            // take a reboot or admin futzing to fix things here, but we continue to try just in case since we don't know.
            if (timeoutAdjustment < 3)
                loopWait = 30*1000;
            else if (timeoutAdjustment < 10)
                loopWait = 5*60*1000;
            else
                loopWait = 60*60*1000;
            timeoutAdjustment += 1;
        }
        
        // Is the event subscriber connected?
        if (subscriberConnected)
        {
            storageManagerFunctions->dPrintf("SymProtect BeginRTSWatch:  Waiting for events...\n");
            // Yes, process any events from it for as long as requested.
			returnValHR = S_OK;
            while (!interruptEventSignaled && SUCCEEDED(returnValHR) && runRTSwatch)
            {
                // Item initialization
                newEvent = NULL;
                policyEvent = NULL;
                newEventInfo = NULL;
                returnValHR = eventList->WaitForItem(&newEvent, &gotItem, rtsThreadInterrupt, &interruptEventSignaled, loopWait);
                if (SUCCEEDED(returnValHR))
                {
                    if (gotItem)
                    {
                        // Initialize the node structure
                        ZeroMemory(currStorageNode, sizeof(*currStorageNode));

                        returnValSR = newEvent->QueryInterface(SymProtectEvt::IID_PolicyEvent, (void**) &policyEvent);
                        if (SYM_SUCCEEDED(returnValSR))
                        {
                            newEventInfo = new(std::nothrow) SymProtectEventInfo;
                            if (newEventInfo != NULL)
                            {
                                returnValHR = newEventInfo->AssignFrom(policyEvent);
                                if (SUCCEEDED(returnValHR))
                                {
                                    // Determine if we should exclude this event, if configured to do so and it meets the criteria
                                    excludeEvent = false;
                                    excludeEvents = (GetVal(storageSettingsKeyHandle, szReg_Val_SymProtect_DisableDefaultEventExclusions, 0) == 0);
                                    if (excludeEvents)
                                        excludeEvent = ExcludeEvent(*newEventInfo);

                                    // Check to see if this is an empty event - if so, filter it out.  No idea why we're getting these.
 									if ((newEventInfo->actionType == SymProtectEvt::ACTION_UNKNOWN) && (newEventInfo->actorPID == 0) && (newEventInfo->targetPID == 0))
									{
										storageManagerFunctions->dPrintf("CSymProtectStorage::BeginRTSWatch:  Blank event detected.\n");
 										excludeEvent = true;
 									}

                                    if (!excludeEvent)
                                    {
                                        currStorageNode->IO                     = &ioFunctions;
                                        currStorageNode->Context                = newEventInfo;
                                        currStorageNode->Flags                  = N_PASSTHROUGH;
                                        currStorageNode->Operations             = 0;
                                        currStorageNode->InstanceID             = 0;
                                        currStorageNode->InternalPath           = NULL;
                                        currStorageNode->UID                    = 0;
                                        currStorageNode->Functions              = &snodeFunctions;
                                        currStorageNode->InstanceID             = DefaultInstanceID;
                                        currStorageNode->dwPID                  = newEventInfo->actorPID;
                                        GetFakeSID(&currStorageNode->Sid);
                                        // We don't have any file extension
                                        _tcscpy(currStorageNode->Ext, _T("."));

                                        GetProcessOwner(newEventInfo->actorPID, &(currStorageNode->Sid), sizeof(currStorageNode->Sid));
                                        ssStrnCpy(currStorageNode->Description, newEventInfo->actorProcessName.c_str(), sizeof(currStorageNode->Description));
                                        GetNodeName(newEventInfo->targetPathname.c_str(), &nodeName);
                                        ssStrnCpy(currStorageNode->Name, nodeName.c_str(), sizeof(currStorageNode->Name));

                                        // Send this node into the depths of RTVScan
                                        ProcessRTSNode(currStorageNode, context);
                                    }
                                    else
                                    {
                                        if (newEventInfo->actorProcessName.c_str() != "")
                                            storageManagerFunctions->dPrintf("CSymProtectStorage::BeginRTSWatch:  Excluding event for actor %s, target %s, action 0x%08x.\n", newEventInfo->actorProcessName.c_str(), newEventInfo->targetPathname.c_str(), newEventInfo->actionType);
                                        else
                                            storageManagerFunctions->dPrintf("CSymProtectStorage::BeginRTSWatch:  Excluding event.\n");
                                    }
                                }
                                else
                                {
                                    storageManagerFunctions->dPrintf("CSymProtectStorage::BeginRTSWatch:  Error 0x%08x converting event data to event info.\n", returnValHR);
                                }

                                currStorageNode->Context = NULL;
                                delete newEventInfo;
                                newEventInfo = NULL;
                            }
                            else
                            {
                                storageManagerFunctions->dPrintf("CSymProtectStorage::BeginRTSWatch:  Out of memory converting event data to event info.\n");
                            }
                            policyEvent.Release();
                        }
                        else
                        {
                            storageManagerFunctions->dPrintf("CSymProtectStorage::BeginRTSWatch:  Error 0x%08x converting received event to policy event.\n", returnValSR);
                        }
                        commonClientSubscriber->DeleteEvent(newEvent);
                    }
                    // Do not abort loop due to failures processing items, only due to failures from WaitForItem
                    returnValHR = S_OK;
                }
                else
                {
                    // This is very bad.  Force an exit as though RTVScan asked us to exit so we don't peg the CPU.
                    interruptEventSignaled = true;
                    runRTSwatch = false;
                    returnValHR = HRESULT_FROM_WIN32(GetLastError());
                    storageManagerFunctions->dPrintf("CSymProtectStorage::BeginRTSWatch:  Error 0x%08x returned from WaitForItem, forcing RTS thread exit.\n", returnValHR);
                }
            }
        }
        else
        {
            // No.  Wait for the requested time.
            storageManagerFunctions->dPrintf("CSymProtectStorage::BeginRTSWatch:  Reconnect delay %d seconds...\n", loopWait/1000);
            returnValDW = WaitForSingleObject(rtsThreadInterrupt, loopWait);
            if (returnValDW == WAIT_TIMEOUT)
            {
                // Nothing in particular to do, just reloop
            }
            else if (returnValDW == WAIT_OBJECT_0)
            {
                interruptEventSignaled = true;
            }
            else if (returnValDW == WAIT_FAILED)
            {
                // This is very bad.  Force an exit as though RTVScan asked us to exit so we don't peg the CPU.
                interruptEventSignaled = true;
                runRTSwatch = false;
                returnValHR = HRESULT_FROM_WIN32(GetLastError());
                storageManagerFunctions->dPrintf("CSymProtectStorage::BeginRTSWatch:  Error 0x%08x returned from Wait, forcing RTS thread exit.\n", returnValHR);
            }
        }
        
        // Was the interrupt event signaled?
        if (interruptEventSignaled)
        {
            // Yes.  Do what was requested.
            // Currently, that means either the ccEventSubscriber was disconnected or RTVScan wants us to shutdown.
            // In either case, the subscriber must be cleaned up
            if (runRTSwatch)
                storageManagerFunctions->dPrintf("CSymProtectStorage::BeginRTSWatch:  Interrupt requested, ccEvent manager shutdown.  Unloading event subscriber.\n");
            else
                storageManagerFunctions->dPrintf("CSymProtectStorage::BeginRTSWatch:  Interrupt requested, shutting down RTS thread.\n");
            SubscriberDeInitialize();
            subscriberConnected = false;
        }
    }

    // Disconnect the event subscriber
    SubscriberDeInitialize();

    // Has SymProtect been configured to remain active after we shutdown (RTVScan exits)?
    if (GetVal(storageSettingsKeyHandle, szReg_Val_SymProtect_ProtectStandalone, FALSE) == FALSE)
    {
        // No, shut it down now
        if (IsServiceRunning("ccEvtMgr") == S_OK)
        {
            returnValHR = ISymProtectController::Create(&spController);
            if (SUCCEEDED(returnValHR))
            {
                returnValHR = spController->Initialize(&errorLocation);
                if (SUCCEEDED(returnValHR))
                {
                    // This status must always be true for the ccVerifyTrust AlternateTrustPolicy protection to be maintained.
                    returnValHRb = spController->SetSymProtectAndBehaviorBlockingEnabled(false, &errorLocation);
                    SETLOGERROR4("Error 0x%08x setting enabled to %d (%s).\n", returnValHRb, (DWORD) true, spController->ErrorLocationToString(errorLocation))
                    returnValHRb = spController->SetFileProtection(SymProtectEvt::PROTECTION_DISABLED, &errorLocation);
                    SETLOGERROR4("Error 0x%08x setting file protection level to %d (%s).\n", returnValHRb, (DWORD) SymProtectEvt::PROTECTION_DISABLED, spController->ErrorLocationToString(errorLocation))
                    returnValHRb = spController->SetRegistryProtection(SymProtectEvt::PROTECTION_DISABLED, &errorLocation);
                    SETLOGERROR4("Error 0x%08x setting registry protection level to %d (%s).\n", returnValHRb, (DWORD) SymProtectEvt::PROTECTION_DISABLED, spController->ErrorLocationToString(errorLocation))
                    returnValHRb = spController->SetProcessProtection(SymProtectEvt::PROTECTION_DISABLED, &errorLocation);
                    SETLOGERROR4("Error 0x%08x setting process protection level to %d (%s).\n", returnValHRb, (DWORD) SymProtectEvt::PROTECTION_DISABLED, spController->ErrorLocationToString(errorLocation))
                    returnValHRb = spController->SetNamedObjectProtection(SymProtectEvt::PROTECTION_DISABLED, &errorLocation);
                    SETLOGERROR4("Error 0x%08x setting named object protection level to %d (%s).\n", returnValHRb, (DWORD) SymProtectEvt::PROTECTION_DISABLED, spController->ErrorLocationToString(errorLocation))

                    spController->Shutdown();
                }
                else
                {
                    storageManagerFunctions->dPrintf("ISymProtectController::Initialize failed, code 0x%08x (%s).\n", returnValHR, ISymProtectController::ErrorLocationToString(errorLocation));
                }
                ISymProtectController::Destroy(&spController);
            }
            else
            {
                storageManagerFunctions->dPrintf("ISymProtectController::Create failed, code 0x%08x.\n", returnValHR);
            }
        }
    }
    else
    {
        storageManagerFunctions->dPrintf("SymProtect warning:  SymProtect configured for always-on protection, staying on even though RTVScan is shutting down.\n");
    }

    // Shutdown and return
    if (storageSettingsKeyHandle != NULL)
    {
        storageManagerFunctions->Close(storageSettingsKeyHandle);
        storageSettingsKeyHandle = NULL;
    }
    CoUninitialize();
    return ERROR_SUCCESS;
}

DWORD CSymProtectStorage::StopRTSWatches()
{
	// Validate state
	if (!initialized)
	{
		storageManagerFunctions->dPrintf("CSymProtectStorage::StopRTSWatches:  CALLED WHEN NOT INITIALIZED.\n");
		return ERROR_GENERAL;
	}

    runRTSwatch = false;
    SetEvent(rtsThreadInterrupt);
    // Shutdown work is done in BeginRTSWatch, since it does the init work

    return ERROR_SUCCESS;
}

DWORD CSymProtectStorage::ProcessPacket( WORD functionCode, BYTE *sendBuffer, DWORD sendBufferSize, BYTE *replyBuffer, DWORD *replyBufferSize, CBA_Addr *address )
{
    DWORD returnValDW = ERROR_FUNCTION_NOT_SUPPORTED;

	if (!initialized)
		storageManagerFunctions->dPrintf("CSymProtectStorage::ProcessPacket:  CALLED WHEN NOT INITIALIZED.\n");

	switch (functionCode)
    {
        case SFUNCTION_PROCESSPACKET_FUNCTIONCODE_GETEVENTBLOCK:
			returnValDW = ProcessPacket_GetEventBlock(functionCode, sendBuffer, sendBufferSize, replyBuffer, replyBufferSize, address);
            break;
        case SFUNCTION_PROCESSPACKET_FUNCTIONCODE_GETLOCATIONSTRING:
			returnValDW = ProcessPacket_GetLocationString(functionCode, sendBuffer, sendBufferSize, replyBuffer, replyBufferSize, address);
            break;
        case SFUNCTION_PROCESSPACKET_FUNCTIONCODE_GETACTIONSTRING:
			returnValDW = ProcessPacket_GetActionString(functionCode, sendBuffer, sendBufferSize, replyBuffer, replyBufferSize, address);
            break;
        case SFUNCTION_PROCESSPACKET_FUNCTIONCODE_GETFULLLOGGERSTRING:
			returnValDW = ProcessPacket_GetFullLoggerString(functionCode, sendBuffer, sendBufferSize, replyBuffer, replyBufferSize, address);
            break;
		case SFUNCTION_PROCESSPACKET_FUNCTIONCODE_GETEVENTNAME:
			returnValDW = ProcessPacket_GetEventName(functionCode, sendBuffer, sendBufferSize, replyBuffer, replyBufferSize, address);
			break;
        case SFUNCTION_PROCESSPACKET_FUNCTIONCODE_GETDISPLAYALERTFOREVENT:
            returnValDW = ProcessPacket_GetDisplayAlertForEvent(functionCode, sendBuffer, sendBufferSize, replyBuffer, replyBufferSize, address);
            break;
        default:
            break;
    }

    return returnValDW;
}

DWORD CSymProtectStorage::FormatMessage( char *resultBuffer, size_t nNumResultBufBytes, const char *messageFormat, PEVENTBLOCK thisEvent, DWORD userParam )
{
	CRTVScanStorage::ReplacementInfoList	replacements;
	SymProtectEventInfo						eventInfo;
	TCHAR									actorPIDstringBuffer[30]					= {""};
	std::string								actorPIDstring;
	TCHAR									targetPIDstringBuffer[30]					= {""};
	std::string								targetPIDstring;
	TCHAR									targetTerminalSessionIDstringBuffer[30]		= {""};
	std::string								targetTerminalSessionIDstring;
	std::string								actionName;
	std::string								entityType;
	std::string								actionTaken;
	DWORD									returnValDW									= ERROR_GENERAL;
	HRESULT									returnValHR									= E_FAIL;

	// Validate parameters
	if ((resultBuffer == NULL) || (messageFormat == NULL) || (thisEvent == NULL))
		return ERROR_BAD_PARAM;
	if (!initialized)
		storageManagerFunctions->dPrintf("CSymProtectStorage::ProcessPacket:  CALLED WHEN NOT INITIALIZED.\n");

	returnValHR = eventInfo.ReadFromLogField((LPCTSTR) thisEvent->EventData);
	if (SUCCEEDED(returnValHR))
	{
        try
        {
            // RTVScan does not pass the buffer size to us.  However, it always uses uses buffers of MAX_MESSAGE_SIZE size to call this API
            sssnprintf(actorPIDstringBuffer,  sizeof(actorPIDstringBuffer), "%d", eventInfo.actorPID);
            sssnprintf(targetPIDstringBuffer, sizeof(targetPIDstringBuffer), "%d", eventInfo.targetPID);
            sssnprintf(targetTerminalSessionIDstringBuffer, sizeof(targetTerminalSessionIDstringBuffer), "%d", eventInfo.targetTerminalSessionID);
            actorPIDstring						= actorPIDstringBuffer;
            targetPIDstring						= targetPIDstringBuffer;
            targetTerminalSessionIDstring		= targetTerminalSessionIDstringBuffer;
            eventInfo.GetActionTakenString(moduleHandle, &actionTaken);
            eventInfo.GetActionDescriptions(moduleHandle, &actionName, &entityType);

            // Escape all backslashes with another backslash.  This works around conversion code in the global FormatMessage
            // that treats \n and \\ as special sequences, replaced with 0x0a and '\', respectively
            StringReplace(&actionTaken, "\\", "\\\\");
            StringReplace(&actionName, "\\", "\\\\");
            StringReplace(&entityType, "\\", "\\\\");
            StringReplace(&actorPIDstring, "\\", "\\\\");
            StringReplace(&eventInfo.actorProcessName, "\\", "\\\\");
            StringReplace(&eventInfo.targetPathname, "\\", "\\\\");
            StringReplace(&targetPIDstring, "\\", "\\\\");
            StringReplace(&targetTerminalSessionIDstring, "\\", "\\\\");

            // Initialize string replacements
            replacements.push_back(ReplacementInfo(Replacement_Token_ActionTaken, actionTaken));
            replacements.push_back(ReplacementInfo(Replacement_Token_ActionName, actionName));
            replacements.push_back(ReplacementInfo(Replacement_Token_EntityType, entityType));
            replacements.push_back(ReplacementInfo(Replacement_Token_ActorPID, actorPIDstring));
            replacements.push_back(ReplacementInfo(Replacement_Token_ActorProcessName, eventInfo.actorProcessName));
            replacements.push_back(ReplacementInfo(Replacement_Token_TargetPathname, eventInfo.targetPathname));
            replacements.push_back(ReplacementInfo(Replacement_Token_TargetPID, targetPIDstring));
            replacements.push_back(ReplacementInfo(Replacement_Token_TargetTerminalSessionID, targetTerminalSessionIDstring));

            // Replace tokens
            returnValDW = FormatMessageHelper(messageFormat, replacements, resultBuffer, nNumResultBufBytes);
        }
        VP_CATCH_MEMORYEXCEPTIONS(returnValDW = ERROR_MEMORY;);
	}

	return returnValDW;
}

DWORD CSymProtectStorage::ReloadRTSConfig( )
{
	HRESULT			returnValHR			= E_FAIL;

    // Initialize
    returnValHR = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    if (FAILED(returnValHR))
    {
        storageManagerFunctions->dPrintf("CSymProtectStorage::ReloadRTSConfig:  CoInitializeEx failed 0x%08x.\n", returnValHR);
        return ERROR_GENERAL;
    }

	returnValHR = ImportSettings();

    CoUninitialize();
	if (SUCCEEDED(returnValHR))
		return ERROR_SUCCESS;
	else
		return ERROR_GENERAL;
}

DWORD CSymProtectStorage::NodeFunction_Release( PSNODE thisNode )
{
	SymProtectEventInfo*				eventInfo	= NULL;

	// Validate parameter
	if (thisNode == NULL)
		return ERROR_BAD_PARAM;
	if (!initialized)
		storageManagerFunctions->dPrintf("CSymProtectStorage::NodeFunction_Release:  CALLED WHEN NOT INITIALIZED.\n");

	// Free the policy event
	eventInfo = reinterpret_cast<SymProtectEventInfo*>(thisNode->Context);
	if (eventInfo != NULL)
	{
		delete eventInfo;
		eventInfo = NULL;
		thisNode->Context = NULL;
	}
	return ERROR_SUCCESS;
}

DWORD CSymProtectStorage::NodeFunction_HasViruses( PSNODE pNode, PEVENTBLOCK pEventBlock )
{
	return ERROR_SUCCESS;
}

DWORD CSymProtectStorage::NodeFunction_Rename( PSNODE pNode, char* newExt )
{
	return ERROR_SUCCESS;
}

DWORD CSymProtectStorage::NodeFunction_Remove( PSNODE pNode )
{
	return ERROR_SUCCESS;
}

HRESULT CSymProtectStorage::GetNodeName( std::string pathname, std::string* nodeName )
//Returns the name of the specified file or registry entry given a full pathname to that entity
{
	LPCTSTR lastBackslash   = NULL;
    HRESULT returnValHR     = S_OK;

	if (nodeName == NULL)
		return E_POINTER;

    try
    {
        lastBackslash = (LPCTSTR) _mbsrchr((LPBYTE) pathname.c_str(), TEXT('\\'));
        if (lastBackslash != NULL)
            *nodeName = CharNext(lastBackslash);
        else
            *nodeName = pathname;
        returnValHR = S_OK;
            
    }
    catch (std::bad_alloc&)
    {
        returnValHR = E_OUTOFMEMORY;
    }
	return returnValHR;
}

void CSymProtectStorage::SetModuleHandle( HINSTANCE newModuleHandle )
// Sets the DLL module handle
{
	moduleHandle = newModuleHandle;
}


// ** CONSTRUCTOR-DESTRUCTOR **
CSymProtectStorage::CSymProtectStorage() : eventList(NULL), commonClientSubscriber(NULL), rtsThreadInterrupt(NULL), runRTSwatch(false)
{
	HKEY			keyHandle						= NULL;
	DWORD			valueType						= REG_DWORD;
	TCHAR			stringBuffer[2*MAX_PATH+1]		= {""};
	DWORD			stringBufferSize				= 0;
	std::string		programDirectory;
	std::string		systemDirectory;
	std::string		windowsDirectory;
    std::string     wholeSecurityPath;
	LPTSTR			lastCharacter					= NULL;
	FilterCriteria	newFilterCriteria;
	DWORD			returnValDW						= ERROR_FILE_NOT_FOUND;

    try
    {
        // Initialize the filter list
        // Determine directories
        returnValDW = RegOpenKeyEx(HKEY_LOCAL_MACHINE, szReg_Key_Sym_InstalledApps, NULL, KEY_READ, &keyHandle);
        if (returnValDW == ERROR_SUCCESS)
        {
            stringBufferSize = sizeof(stringBuffer);
            returnValDW = SymSaferRegQueryValueEx(keyHandle, "SAVCE", NULL, &valueType, (LPBYTE) &stringBuffer, &stringBufferSize);
            RegCloseKey(keyHandle);
            keyHandle = NULL;
        }
        if (returnValDW == ERROR_SUCCESS)
        {
            lastCharacter = CharPrev(stringBuffer, stringBuffer+lstrlen(stringBuffer));
            if (*lastCharacter == '\\')
                *lastCharacter = NULL;
            programDirectory = stringBuffer;
        }
        if (GetSystemDirectory(stringBuffer, sizeof(stringBuffer)/sizeof(stringBuffer[0])) != 0)
        {
            lastCharacter = CharPrev(stringBuffer, stringBuffer+lstrlen(stringBuffer));
            if (*lastCharacter == '\\')
                *lastCharacter = NULL;
            systemDirectory = stringBuffer;
        }
        if (GetWindowsDirectory(stringBuffer, sizeof(stringBuffer)/sizeof(stringBuffer[0])) != 0)
        {
            lastCharacter = CharPrev(stringBuffer, stringBuffer+lstrlen(stringBuffer));
            if (*lastCharacter == '\\')
                *lastCharacter = NULL;
            windowsDirectory = stringBuffer;
        }
        returnValDW = RegOpenKeyEx(HKEY_LOCAL_MACHINE, FilterExecutable_WholeSecurity_KeyPath, NULL, KEY_READ, &keyHandle);
        if (returnValDW == ERROR_SUCCESS)
        {
            stringBufferSize = sizeof(stringBuffer);
            returnValDW = SymSaferRegQueryValueEx(keyHandle, FilterExecutable_WholeSecurity_ValueName, NULL, &valueType, (LPBYTE) &stringBuffer, &stringBufferSize);
            if (returnValDW == ERROR_SUCCESS)
            {
                lastCharacter = CharPrev(stringBuffer, stringBuffer+lstrlen(stringBuffer));
                if (*lastCharacter == '\\')
                    *lastCharacter = NULL;
                wholeSecurityPath = stringBuffer;
                // Remove double-quoting
                StringReplace(&wholeSecurityPath, "\"", "");
            }
            RegCloseKey(keyHandle);
            keyHandle = NULL;
        }
        

        // Store directories in config
        config.filterExplorerPathname       = windowsDirectory + std::string("\\explorer.exe");
        config.filterWinLogonPathname       = systemDirectory + std::string("\\WinLogon.exe");
        config.filterWinMgmtPathname        = systemDirectory + std::string("\\WBEM\\WinMgmt.exe");
        config.filterWinMgmtAdapPathname	= systemDirectory + std::string(FilterExecutable_WMIadap);
        config.filterDSentryPathname        = systemDirectory + std::string("\\dsentry.exe");
        config.filterCtfmonPathname         = systemDirectory + std::string("\\ctfmon.exe");
        config.filterCsrssPathname          = systemDirectory + std::string("\\csrss.exe");
        config.filterBlastCleanPathname     = systemDirectory + std::string(FilterExecutable_BlastCln);

        // Filter list population (filter out common system activity only, for now)
        // Filter General initialization
        newFilterCriteria.matchActor        = true;
        newFilterCriteria.actorIsPathname   = true;
        newFilterCriteria.targetPathname    = "";
        newFilterCriteria.matchTarget       = false;
        newFilterCriteria.matchEventTypes   = true;

        // Process event filters
        newFilterCriteria.eventTypes.clear();
        newFilterCriteria.eventTypes.push_back(SymProtectEvt::ACTION_API_ZWOPENPROCESS);
        newFilterCriteria.eventTypes.push_back(SymProtectEvt::ACTION_API_ZWOPENTHREAD);
        newFilterCriteria.eventTypes.push_back(SymProtectEvt::ACTION_API_OPEN_PROCESS_TOKEN);
        newFilterCriteria.eventTypes.push_back(SymProtectEvt::ACTION_API_OPEN_THREAD_TOKEN);

        newFilterCriteria.actorFilename = config.filterWinMgmtAdapPathname;
        config.filterList.push_back(newFilterCriteria);
        newFilterCriteria.actorFilename = systemDirectory + std::string(FilterExecutable_WMIapsvr);
        config.filterList.push_back(newFilterCriteria);
        newFilterCriteria.actorFilename = systemDirectory + std::string(FilterExecutable_WMIwmic);
        config.filterList.push_back(newFilterCriteria);
        newFilterCriteria.actorFilename = systemDirectory + std::string(FilterExecutable_WMIprvse);
        config.filterList.push_back(newFilterCriteria);
        newFilterCriteria.actorFilename = config.filterDSentryPathname;
        config.filterList.push_back(newFilterCriteria);
        newFilterCriteria.actorFilename = config.filterCtfmonPathname;
        config.filterList.push_back(newFilterCriteria);
        newFilterCriteria.actorFilename = config.filterBlastCleanPathname;
        config.filterList.push_back(newFilterCriteria);

        // WinLogon and CSRSS need additional filters for notifies during fast user switching
        newFilterCriteria.eventTypes.push_back(SymProtectEvt::ACTION_API_SET_INFO_PROCESS);
        newFilterCriteria.actorFilename = config.filterWinLogonPathname;
        config.filterList.push_back(newFilterCriteria);
        newFilterCriteria.actorFilename = config.filterCsrssPathname;
        config.filterList.push_back(newFilterCriteria);
        newFilterCriteria.eventTypes.pop_back();
        
        // Accessibility utilitieis are guilty of different behavior - ALLOC_VIRTUAL_MEMORY's
        newFilterCriteria.eventTypes.clear();
        newFilterCriteria.eventTypes.push_back(SymProtectEvt::ACTION_API_ALLOC_VIRTUAL_MEM);
        newFilterCriteria.actorFilename = systemDirectory + std::string(FilterExecutable_Accessibility_Narrator);
        config.filterList.push_back(newFilterCriteria);
        newFilterCriteria.actorFilename = systemDirectory + std::string(FilterExecutable_Accessibility_Magnify);
        config.filterList.push_back(newFilterCriteria);
        
        // Process event filters - actor is node name
        newFilterCriteria.actorIsPathname = false;
        newFilterCriteria.actorFilename = FilterExecutable_XTAgent;
        config.filterList.push_back(newFilterCriteria);
        newFilterCriteria.actorFilename = FilterExecutable_NalAgent;
        config.filterList.push_back(newFilterCriteria);
        newFilterCriteria.actorFilename = FilterExecuteable_MicrosoftUpdate;
        config.filterList.push_back(newFilterCriteria);

        // Other filters
        // Explorer Open Process/Thread token/process
        newFilterCriteria.actorIsPathname = true;
        newFilterCriteria.actorFilename = config.filterExplorerPathname;
        newFilterCriteria.eventTypes.clear();
        newFilterCriteria.eventTypes.push_back(SymProtectEvt::ACTION_API_ZWOPENPROCESS);
        newFilterCriteria.eventTypes.push_back(SymProtectEvt::ACTION_API_ZWOPENTHREAD);
        newFilterCriteria.eventTypes.push_back(SymProtectEvt::ACTION_API_OPEN_PROCESS_TOKEN);
        newFilterCriteria.eventTypes.push_back(SymProtectEvt::ACTION_API_OPEN_THREAD_TOKEN);
        newFilterCriteria.eventTypes.push_back(SymProtectEvt::ACTION_FS_OPEN);
        config.filterList.push_back(newFilterCriteria);

        // WinMgmt open process/thread token/process, reg key open
        newFilterCriteria.actorFilename = config.filterWinMgmtPathname;
        newFilterCriteria.eventTypes.clear();
        newFilterCriteria.eventTypes.push_back(SymProtectEvt::ACTION_API_ZWOPENPROCESS);
        newFilterCriteria.eventTypes.push_back(SymProtectEvt::ACTION_API_ZWOPENTHREAD);
        newFilterCriteria.eventTypes.push_back(SymProtectEvt::ACTION_API_OPEN_PROCESS_TOKEN);
        newFilterCriteria.eventTypes.push_back(SymProtectEvt::ACTION_API_OPEN_THREAD_TOKEN);
        newFilterCriteria.eventTypes.push_back(SymProtectEvt::ACTION_REG_OPEN_KEY);
        config.filterList.push_back(newFilterCriteria);

        // RTVScan accesses of VPDEBUG.LOG
        if ((windowsDirectory != "") && (programDirectory != ""))
        {
            newFilterCriteria.actorIsPathname   = true;
            newFilterCriteria.actorFilename = programDirectory + std::string("\\rtvscan.exe");
            newFilterCriteria.eventTypes.clear();
            newFilterCriteria.eventTypes.push_back(SymProtectEvt::ACTION_FS_SETATTRIBUTES);
            newFilterCriteria.targetPathname = programDirectory + std::string("\\rtvscan.exe");
            newFilterCriteria.matchTarget       = true;
            config.filterList.push_back(newFilterCriteria);
        }

        // WholeSecurity checks (Unsure of what events they generate, so ignore all / Must filter out install-caused events as well)
        newFilterCriteria.matchActor        = true;
        newFilterCriteria.matchEventTypes   = false;
        if (wholeSecurityPath != "")
        {
            newFilterCriteria.actorFilename     = wholeSecurityPath;
            newFilterCriteria.actorIsPathname   = true;
        }
        else
        {
            newFilterCriteria.actorFilename     = FilterExecutable_WholeSecurity_Service;
            newFilterCriteria.actorIsPathname   = false;
        }
        newFilterCriteria.eventTypes.clear();
        newFilterCriteria.targetPathname    = "";
        newFilterCriteria.matchTarget       = false;
        config.filterList.push_back(newFilterCriteria);
        // WholeSecurity ActiveX control (Can't locate in registry)
        newFilterCriteria.matchActor        = true;
        newFilterCriteria.actorFilename     = FilterExecutable_WholeSecurity_ActiveX;
        newFilterCriteria.actorIsPathname   = false;
        newFilterCriteria.matchEventTypes   = false;
        newFilterCriteria.eventTypes.clear();
        newFilterCriteria.targetPathname    = "";
        newFilterCriteria.matchTarget       = false;
        config.filterList.push_back(newFilterCriteria);
        
        // Exclude any System Restore notification exclusions (these aren't protection notifications anyway)
        newFilterCriteria.matchActor        = false;
        newFilterCriteria.actorFilename     = "";
        newFilterCriteria.actorIsPathname   = false;
        newFilterCriteria.matchEventTypes   = true;
        newFilterCriteria.eventTypes.clear();
		newFilterCriteria.eventTypes.push_back(SymProtectEvt::ACTION_SR_SUCCESS);
		newFilterCriteria.eventTypes.push_back(SymProtectEvt::ACTION_SR_FAILURE);
        newFilterCriteria.targetPathname    = "";
        newFilterCriteria.matchTarget       = false;
        config.filterList.push_back(newFilterCriteria);

        // Exclude any open token notifies - we get noise from SMS and ZenWorks from these.
        newFilterCriteria.eventTypes.clear();
        newFilterCriteria.eventTypes.push_back(SymProtectEvt::ACTION_API_OPEN_PROCESS_TOKEN);
        newFilterCriteria.eventTypes.push_back(SymProtectEvt::ACTION_API_OPEN_THREAD_TOKEN);
        config.filterList.push_back(newFilterCriteria);
    }
    catch (std::bad_alloc&)
    {
    }
}

CSymProtectStorage::~CSymProtectStorage()
{
	// Nothing for now...
}

// ** HELPERS **
DWORD CSymProtectStorage::ProcessPacket_GetEventBlock( WORD functionCode, BYTE *sendBuffer, DWORD sendBufferSize, BYTE *replyBuffer, DWORD *replyBufferSize, CBA_Addr *address )
// Performs SFUNCTION_PROCESSPACKET_FUNCTIONCODE_GETEVENTBLOCK
{
	StorageConfig						storageConfig;
	PSNODE								thisNode							= NULL;
	EVENTBLOCK*							thisEvent							= NULL;
	STORAGEOBJECT*						thisStorageObject					= NULL;
	SymProtectEventInfo*				eventInfo							= NULL;
	std::string							stringRepresentation;
	LPTSTR								persistentStringRepresentation		= NULL;
	TCHAR								actorName[2*MAX_PATH+1]				= {""};
	TCHAR								keyPathname[2*MAX_PATH+1]			= {""};
	DWORD								bufferSize							= 0;
	DWORD								returnValDW							= ERROR_BAD_PARAM;

	// sendBuffer = PSNODE, sendBufferSize = sizeof(*PSNODE), replyBuffer = EVENTBLOCK*, *replyBufferSize = sizeof(EVENTBLOCK), address = STORAGEOBJECT* from ProcessRTSNode
    if ((sendBuffer != NULL) && (sendBufferSize == sizeof(SNODE)) && (replyBuffer != NULL) && (replyBufferSize != NULL) && (address != NULL))
    {
		if (*replyBufferSize >= sizeof(EVENTBLOCK))
		{
			// Initialize
			thisNode = (PSNODE) sendBuffer;
			thisEvent = (EVENTBLOCK*) replyBuffer;
			eventInfo = reinterpret_cast<SymProtectEventInfo*>(thisNode->Context);
			thisStorageObject = (STORAGEOBJECT*) address;
			GetConfig(&storageConfig);
            sssnprintf(keyPathname, sizeof(keyPathname), "%s\\%s", storageConfig.storageName.c_str(), szReg_Key_Storage_RealTime);

			// Fill in basic EVENTBLOCK fields
			thisEvent->Category				= GL_CAT_SECURITY;
			thisEvent->Event                = GL_EVENT_SECURITY_SYMPROTECT_POLICYVIOLATION;
			thisEvent->pSid                 = (PSID) &thisNode->Sid;
			thisEvent->Access               = thisNode->Operations & (FA_READ|FA_WRITE|FA_EXEC|FA_DELETE|FA_RENAME);
			thisEvent->so                   = thisStorageObject;
			thisEvent->Flags                = thisNode->Flags | thisStorageObject->Flags;
			thisEvent->logger               = LOGGER_SymProtect;
			thisEvent->Time					= time(NULL);
			storageManagerFunctions->Open(reinterpret_cast<HKEY>(HKEY_VP_STORAGE_ROOT), keyPathname, &(thisEvent->hKey[0]));

			// Save the policy data to the userData field of the EVENTBLOCK
			if (eventInfo != NULL)
			{
				eventInfo->WriteToLogField(&stringRepresentation);
				persistentStringRepresentation = new(std::nothrow) TCHAR[stringRepresentation.size()+1];
				if (persistentStringRepresentation != NULL)
				{
                    ssStrnCpy(persistentStringRepresentation, stringRepresentation.c_str(), (DWORD) stringRepresentation.size()+1);
					thisEvent->EventData = (BYTE*) persistentStringRepresentation;
				}

				// Fill in general EVENTBLOCK fields not already filled in for us
				bufferSize = lstrlen(eventInfo->targetPathname.c_str())+1;
				thisEvent->Description = new(std::nothrow) TCHAR[bufferSize];
				if (thisEvent->Description != NULL)
                    ssStrnCpy(thisEvent->Description, eventInfo->targetPathname.c_str(), bufferSize);
				if (eventInfo->actorProcessName != "")
                    ssStrnCpy(actorName, eventInfo->actorProcessName.c_str(), sizeof(actorName));
				else
                    sssnprintf(actorName, sizeof(actorName), "Process ID %d", eventInfo->actorPID);
				bufferSize = lstrlen(actorName)+1;
				thisEvent->VirusName = new(std::nothrow) TCHAR[bufferSize];
				if (thisEvent->VirusName != NULL)
                    ssStrnCpy(thisEvent->VirusName, actorName, bufferSize);
			}
			returnValDW = ERROR_SUCCESS;
		}
    }
    else
    {
        returnValDW = ERROR_BAD_PARAM;
    }
	return returnValDW;
}

DWORD CSymProtectStorage::ProcessPacket_GetLocationString( WORD functionCode, BYTE *sendBuffer, DWORD sendBufferSize, BYTE *replyBuffer, DWORD *replyBufferSize, CBA_Addr *address )
// Performs SFUNCTION_PROCESSPACKET_FUNCTIONCODE_GETLOCATIONSTRING
{
	DWORD				returnValDW			= ERROR_BAD_PARAM;

	// sendBuffer = EVENTBLOCK, sendBufferSize = sizeof(EVENTBLOCK), replyBuffer = char*, *replyBufferSize = sizeof reply buffer, address = NULL
    if ((sendBuffer != NULL) && (sendBufferSize >= sizeof(EVENTBLOCK)) && (replyBuffer != NULL) && (replyBufferSize != 0))
    {
		::LoadString(moduleHandle, IDS_SYMPROTECT_STORAGE_LOCATIONNAME, (LPSTR) replyBuffer, *replyBufferSize);
		returnValDW = ERROR_SUCCESS;
    }
    else
    {
        returnValDW = ERROR_BAD_PARAM;
    }

	return returnValDW;
}

DWORD CSymProtectStorage::ProcessPacket_GetActionString( WORD functionCode, BYTE *sendBuffer, DWORD sendBufferSize, BYTE *replyBuffer, DWORD *replyBufferSize, CBA_Addr *address )
// Performs SFUNCTION_PROCESSPACKET_FUNCTIONCODE_GETACTIONSTRING
{
	EVENTBLOCK*				thisEvent				= NULL;
	SymProtectEventInfo		eventInfo;
	std::string				actionDescription;
	DWORD					returnValDW				= ERROR_BAD_PARAM;
    HRESULT                 returnValHR             = E_FAIL;

	// sendBuffer = EVENTBLOCK, sendBufferSize = sizeof(EVENTBLOCK), replyBuffer = char*, *replyBufferSize = sizeof reply buffer, address = NULL
    if ((sendBuffer != NULL) && (sendBufferSize >= sizeof(EVENTBLOCK)) && (replyBuffer != NULL) && (replyBufferSize != NULL))
    {
		thisEvent = (EVENTBLOCK*) sendBuffer;
		eventInfo.ReadFromLogField((LPCTSTR) thisEvent->EventData);
        returnValHR = eventInfo.GetActionTakenString(moduleHandle, &actionDescription);
        if (SUCCEEDED(returnValHR))
        {
            ssStrnCpy((LPSTR) replyBuffer, actionDescription.c_str(), *replyBufferSize);
	    	returnValDW = ERROR_SUCCESS;
        }
        else
        {
            returnValDW = ERROR_MEMORY;
        }
    }
    else
    {
        returnValDW = ERROR_BAD_PARAM;
    }

	return returnValDW;
}

DWORD CSymProtectStorage::ProcessPacket_GetFullLoggerString( WORD functionCode, BYTE *sendBuffer, DWORD sendBufferSize, BYTE *replyBuffer, DWORD *replyBufferSize, CBA_Addr *address )
// SFUNCTION_PROCESSPACKET_FUNCTIONCODE_GETFULLLOGGERSTRING
{
	DWORD				returnValDW			= ERROR_BAD_PARAM;

	// sendBuffer = EVENTBLOCK, sendBufferSize = sizeof(EVENTBLOCK), replyBuffer = char*, *replyBufferSize = sizeof reply buffer, address = NULL
    if ((sendBuffer != NULL) && (sendBufferSize >= sizeof(EVENTBLOCK)) && (replyBuffer != NULL) && (replyBufferSize != 0))
    {
		::LoadString(moduleHandle, IDS_SYMPROTECT_STORAGE_LOGGERNAME, (LPSTR) replyBuffer, *replyBufferSize);
		returnValDW = ERROR_SUCCESS;
    }
    else
    {
        returnValDW = ERROR_BAD_PARAM;
    }

	return returnValDW;
}

DWORD CSymProtectStorage::ProcessPacket_GetEventName( WORD functionCode, BYTE *sendBuffer, DWORD sendBufferSize, BYTE *replyBuffer, DWORD *replyBufferSize, CBA_Addr *address )
// SFUNCTION_PROCESSPACKET_FUNCTIONCODE_GETEVENTNAME
// Sets replyBuffer equal to the description for the event ID specified by sendBuffer
{
	std::string			locationString;
	DWORD				returnValDW			= ERROR_BAD_PARAM;

	// sendBuffer = NULL, sendBufferSize = event ID, replyBuffer = char*, replyBufferSize = sizeof reply buffer, address = NULL
    if ((sendBuffer == NULL) && (replyBuffer != NULL) && (replyBufferSize != NULL))
    {
		switch (sendBufferSize)
		{
		case GL_EVENT_SECURITY_SYMPROTECT_POLICYVIOLATION:
			::LoadString(moduleHandle, IDS_SYMPROTECT_EVENT_POLICYVIOLATION, (LPSTR) replyBuffer, *replyBufferSize);
			returnValDW = ERROR_SUCCESS;
			break;
		default:
			returnValDW = ERROR_BAD_PARAM;
			break;
		}
		returnValDW = ERROR_SUCCESS;
    }
    else
    {
        returnValDW = ERROR_BAD_PARAM;
    }

	return returnValDW;
}

DWORD CSymProtectStorage::ProcessPacket_GetDisplayAlertForEvent( WORD functionCode, BYTE *sendBuffer, DWORD sendBufferSize, BYTE *replyBuffer, DWORD *replyBufferSize, CBA_Addr *address )
// SFUNCTION_PROCESSPACKET_FUNCTIONCODE_GETDISPLAYALERTFOREVENT
{
    StorageConfig                           storageConfig;
    HKEY                                    storageSettingsKeyHandle                = NULL;
    TCHAR                                   storageSettingsKeyPath[2*MAX_PATH+1]    = {""};
    SymProtectEventInfo                     spEventInfo;
    ISymProtectController::ProtectionType   spProtectionType                        = ISymProtectController::ProtectionType_Process;
    LPCTSTR                                 alertValueName                          = NULL;
    bool                                    displayAlert                            = false;
    DWORD                                   returnValDW                             = ERROR_BAD_PARAM;
    HRESULT                                 returnValHR                             = E_FAIL;

    // sendBuffer = &EVENTBLOCK, sendBufferSize = sizeof(EVENTBLOCK), replyBuffer = bool*, replyBufferSize = sizeof(bool), address = NULL
    if ((sendBuffer != NULL) && (sendBufferSize >= sizeof(EVENTBLOCK)) && (replyBuffer != NULL) && (replyBufferSize != NULL))
    {
        if (*replyBufferSize == sizeof(bool))
        {
            returnValHR = GetConfig(&storageConfig);
            if (SUCCEEDED(returnValHR))
                returnValHR = spEventInfo.ReadFromLogField((LPCTSTR) ((EVENTBLOCK*) sendBuffer)->EventData);
            if (SUCCEEDED(returnValHR))
            {
                sssnprintf(storageSettingsKeyPath, sizeof(storageSettingsKeyPath), "%s\\%s", storageConfig.storageName.c_str(), szReg_Key_Storage_RealTime);
                returnValDW = storageManagerFunctions->Open(reinterpret_cast<HKEY>(HKEY_VP_STORAGE_ROOT), storageSettingsKeyPath, &storageSettingsKeyHandle);
            }
            if (returnValDW == ERROR_SUCCESS)
            {
                // Determine the type of action blocked
                returnValHR = ISymProtectController::GetActionProtectionType(spEventInfo.actionType, &spProtectionType);
                if (SUCCEEDED(returnValHR))
                {
                    switch (spProtectionType)
                    {
                    case ISymProtectController::ProtectionType_Process:
                        alertValueName = szReg_Val_SymProtect_Alert_Process;
                        break;
                    case ISymProtectController::ProtectionType_File:
                        alertValueName = szReg_Val_SymProtect_Alert_File;
                        break;
                    case ISymProtectController::ProtectionType_Registry:
                        alertValueName = szReg_Val_SymProtect_Alert_Registry;
                        break;
                    case ISymProtectController::ProtectionType_NamedObject:
                        alertValueName = szReg_Val_SymProtect_Alert_NamedObject;
                        break;
                    }
                }

                // Load the appropriate value and return it                
                if (alertValueName != NULL)
                {
                    displayAlert = (GetVal(storageSettingsKeyHandle, alertValueName, false) != 0);
                }
                else
                {
                    storageManagerFunctions->dPrintf("SymProtect:  Unrecognized SP event type 0x%08x\n", spEventInfo.actionType);
                    displayAlert = (GetVal(storageSettingsKeyHandle, szReg_Val_SymProtect_ShowNotify, false) != 0);
                }
                *(bool*) replyBuffer = displayAlert;
                returnValDW = ERROR_SUCCESS;

                storageManagerFunctions->Close(storageSettingsKeyHandle);
                storageSettingsKeyHandle = NULL;            
            }
        }
    }

    return returnValDW;
}

HRESULT CSymProtectStorage::SubscriberInitialize( void )
// Creates commonClientSubscriber and connects it.
{
    HRESULT     returnValHR             = E_FAIL;

    // Validate state
    if (!initialized)
    {
        storageManagerFunctions->dPrintf("CSymProtectStorage::SubscriberInitialize:  Called when not initialized!\n");
        return E_ABORT;
    }

    // Make sure service dependancies are running
    returnValHR = EnsureServiceDependanciesRunning();
    if (returnValHR == S_OK)
    {
        // Allocate helper objects
        commonClientSubscriber = new(std::nothrow) CSymProtectEventSubscriber;
        if (commonClientSubscriber != NULL)
            eventList = new(std::nothrow) CSynchronizedList<ccEvtMgr::CEventEx*>;
        else
            returnValHR = E_OUTOFMEMORY;;

        // Initialize event subscriber
        if (SUCCEEDED(returnValHR))
            returnValHR = commonClientSubscriber->SetShutdownEvent(rtsThreadInterrupt);
        if (SUCCEEDED(returnValHR))
        {
            returnValHR = commonClientSubscriber->SetEventList(eventList);
            if (SUCCEEDED(returnValHR))
            {
                returnValHR = commonClientSubscriber->InitializeMain();
                if (SUCCEEDED(returnValHR))
                {
                    storageManagerFunctions->dPrintf("SymProtect note:  Subscriber connected.\n");
                    returnValHR = S_OK;
                }
                else
                {
                    storageManagerFunctions->dPrintf("CSymProtectStorage::SubscriberInitialize:  commonClientSubscriber->InitializeMain failed, code 0x%08x.\n", returnValHR);
                }
            }
            else
            {
                storageManagerFunctions->dPrintf("CSymProtectStorage::SubscriberInitialize:  commonClientSubscriber->SetEventList failed, code 0x%08x.\n", returnValHR);
            }
        }
        else
        {
            storageManagerFunctions->dPrintf("CSymProtectStorage::SubscriberInitialize:  commonClientSubscriber->SetShutdownEvent failed, code 0x%08x.\n", returnValHR);
        }
    }
    else
    {
        if (returnValHR != S_FALSE)
            storageManagerFunctions->dPrintf("CSymProtectStorage::SubscriberInitialize:  EnsureServiceDependanciesRunning returned 0x%08x.\n", returnValHR);
        returnValHR = HRESULT_FROM_WIN32(ERROR_SERVICE_DEPENDENCY_FAIL);
    }

    // Log result and cleanup if failed
    if (returnValHR == E_OUTOFMEMORY)
        storageManagerFunctions->dPrintf("CSymProtectStorage::SubscriberInitialize:  DANGER:  OUT OF MEMORY ATTEMPTING TO INITIALIZE.");
    if (FAILED(returnValHR))
        SubscriberDeInitialize();

    // Return result
    return returnValHR;
}

HRESULT CSymProtectStorage::SubscriberDeInitialize( void )

{
    CSynchronizedList<ccEvtMgr::CEventEx*>::ListType::iterator      currEvent;
    HRESULT                                                         returnValHR         = S_FALSE;

    if (!initialized)
        storageManagerFunctions->dPrintf("CSymProtectStorage::SubscriberDeInitialize:  CALLED WHEN NOT INITIALIZED.\n");

    // Shutdown the common client componentry
    if (commonClientSubscriber != NULL)
    {
        returnValHR = S_OK;
        // Disconnect the shutdown event
        returnValHR = commonClientSubscriber->SetShutdownEvent(NULL);
        if (FAILED(returnValHR))
            storageManagerFunctions->dPrintf("CSymProtectStorage::SubscriberDeInitialize:  commonClientSubscriber->SetShutdownEvent failed code 0x%08x.\n", returnValHR);
        // Disconnect the event list
        returnValHR = commonClientSubscriber->SetEventList(NULL);
        if (FAILED(returnValHR))
            storageManagerFunctions->dPrintf("CSymProtectStorage::SubscriberDeInitialize:  commonClientSubscriber->SetEventList failed code 0x%08x.\n", returnValHR);
        // Free any events in the list
        for(currEvent = eventList->begin(); currEvent != eventList->end(); currEvent++)
            commonClientSubscriber->DeleteEvent(*currEvent);
        // Shutdown subscriber
        returnValHR = commonClientSubscriber->ShutdownMain();
        if (FAILED(returnValHR))
            storageManagerFunctions->dPrintf("CSymProtectStorage::SubscriberDeInitialize:  commonClientSubscriber->ShutdownMain failed code 0x%08x.\n", returnValHR);
        delete commonClientSubscriber;
        commonClientSubscriber = NULL;
    }
    // Deallocate the event list (should be empty)
    if (eventList != NULL)
    {
        eventList->clear();
        delete eventList;
        eventList = NULL;
    }

    return returnValHR;
}

HRESULT CSymProtectStorage::ImportSettings( void )
// Imports SymProtect settings from the SAVCorp registry into SymProtect.  Always attempts to set all settings regardless of previous failures.
// Calling thread MUST have called CoInitialize with multithreaded apartment model!
// Returns S_OK unless a critical error occurs that prevents it from even attempting to configure SymProtect.
{
	StorageConfig								storageConfig;
	HKEY										storageSettingsKeyHandle				= NULL;
	TCHAR										storageSettingsKeyPath[2*MAX_PATH+1]	= {""};
	DWORD										timeoutSet								= DefaultEventTimeout;
	DWORD										timeoutQuery							= DefaultEventTimeout;
	DWORD										timeoutDefault							= DefaultEventTimeout;
	BOOL										disabled								= FALSE;
	bool										isEnabled								= false;
    SymProtectEvt::CQueryStateEventEx::SP_QUERY_STATE spState                           = SymProtectEvt::CQueryStateEventEx::SP_STATE_OFF;
	SymProtectEvt::SP_PROTECTION_LEVEL			fileProtection							= SymProtectEvt::PROTECTION_ENABLED;
	SymProtectEvt::SP_PROTECTION_LEVEL			registryProtection						= SymProtectEvt::PROTECTION_ENABLED;
    SymProtectEvt::SP_PROTECTION_LEVEL          namedObjectProtection                   = SymProtectEvt::PROTECTION_ENABLED;
	SymProtectEvt::SP_PROTECTION_LEVEL			processProtection						= SymProtectEvt::PROTECTION_ENABLED;
	SymProtectEvt::SP_PROTECTION_LEVEL			currProtectionLevel						= SymProtectEvt::PROTECTION_ENABLED;
    TCHAR                                       directoryPath[MAX_PATH+1]               = {_T("")};
    DWORD                                       directoryPathSize                       = sizeof(directoryPath)/sizeof(directoryPath[0]);
    TCHAR                                       targetPathname[2*MAX_PATH+1]            = {_T("")};
    HKEY                                        keyHandle                               = NULL;
    DWORD                                       valueType                               = REG_DWORD;
    TCHAR                                       clearText[PASS_MAX_PLAIN_TEXT_BYTES+1]  = {""};
	LPTSTR										lastCharacter							= NULL;
    std::string                                 drwatsonPathname;
    std::string                                 dumpprepPathname;
    std::string                                 csrssPathname;
    std::string                                 smssPathname;
	std::string									windowsSystemSubdirName;
    DWORD                                       noAuthorizations                        = 0;
	ISymProtectController*						spController							= NULL;
	HRESULT										returnValHR								= E_FAIL;
    HRESULT                                     returnValHRb                            = E_FAIL;
    HRESULT                                     returnValHRc                            = E_FAIL;
    DWORD                                       returnValDW                             = ERROR_OUT_OF_PAPER;
	ISymProtectController::ErrorLocation		errorLocation							= ISymProtectController::ErrorLocation_None;

    // Ensure no other instances of ImportSettings can run simultaneously (RTVScan has been observed to do so)
    EnterCriticalSection(&importSettingsLock);
    storageManagerFunctions->dPrintf("SymProtect:  ImportSettings called.\n");

    

	// Open storage config key
	returnValHR = GetConfig(&storageConfig);
	if (SUCCEEDED(returnValHR))
    {
        sssnprintf(storageSettingsKeyPath, sizeof(storageSettingsKeyPath), "%s\\%s", storageConfig.storageName.c_str(), szReg_Key_Storage_RealTime);
        returnValDW = storageManagerFunctions->Open(reinterpret_cast<HKEY>(HKEY_VP_STORAGE_ROOT), storageSettingsKeyPath, &storageSettingsKeyHandle);
        if (returnValDW == ERROR_SUCCESS)
        {
            // Make sure service dependancies are running.  If they aren't yet because ccEventManager hasn't been configured,
            // don't try to do anything else yet.
            returnValHRb = EnsureServiceDependanciesRunning();
            if (returnValHRb == S_FALSE)
                returnValHRb = E_FAIL;
            SETERROR

            // Get a SymProtectController
            if (returnValHRb == S_OK)
            {
                returnValHRb = ISymProtectController::Create(&spController);
                SETLOGERROR2("ISymProtectController::Create failed, code 0x%08x.\n", returnValHR);
            }
            if (SUCCEEDED(returnValHR))
            {
                returnValHRb = spController->Initialize(&errorLocation);
                SETLOGERROR2("Error 0x%08x initializing SymProtect controller.\n", returnValHR);
            }
        }
        else
        {
            storageManagerFunctions->dPrintf("SymProtect:  ImportSettings failed to open configuration key, error %d.\n", returnValDW);
            returnValHR = HRESULT_FROM_WIN32(returnValDW);
        }
    }
    else
    {
        storageManagerFunctions->dPrintf("SymProtect:  Error 0x%08x occured getting storage configuration.\n", returnValHR);
    }

    if (SUCCEEDED(returnValHR))
    {
        //make sure symprotect is always loaded
        returnValHRb = spController->SetBBSystemEnabled(true, &errorLocation);
        SETLOGERROR3("Error 0x%08x enabling BB (%s).\n", returnValHRb, spController->ErrorLocationToString(errorLocation))

        // Read in event timeout overrides (undocumented)
        timeoutSet = GetVal(storageSettingsKeyHandle, szReg_Val_SymProtect_EventTimeoutQuery, DefaultEventTimeout);
        timeoutQuery = GetVal(storageSettingsKeyHandle, szReg_Val_SymProtect_EventTimeoutSet, DefaultEventTimeout);
        timeoutDefault = GetVal(storageSettingsKeyHandle, szReg_Val_SymProtect_EventTimeoutDefault, DefaultEventTimeout);
        returnValHRb = spController->SetEventTimeout(timeoutQuery, timeoutSet, timeoutDefault);
#ifdef _DEBUG
        if (FAILED(returnValHRb))
        {
            storageManagerFunctions->dPrintf("ImportSettings:  Warning - debug build blocked due to SP signature checking, returning.\n");
            return S_FALSE;
        }
#endif

        // Read in the base SymProtect configuration entries
        disabled = GetVal(storageSettingsKeyHandle, szReg_Val_SymProtect_Disabled, disabled);
        // This status must always be true for the ccVerifyTrust AlternateTrustPolicy protection to be maintained.
        returnValHRb = spController->SetSymProtectAndBehaviorBlockingEnabled(!disabled, &errorLocation);
        SETLOGERROR5("Error 0x%08x setting enabled from %d to %d (%s).\n", returnValHRb, (DWORD) isEnabled, (DWORD) true, spController->ErrorLocationToString(errorLocation))


        fileProtection		= SymProtectEvt::PROTECTION_DISABLED;
        returnValHRb = spController->GetFileProtection(&currProtectionLevel, &errorLocation);
        SETLOGERROR3("Error 0x%08x retrieving file protection level (%s).\n", returnValHRb, spController->ErrorLocationToString(errorLocation))
        if (currProtectionLevel != fileProtection)
        {
            returnValHRb = spController->SetFileProtection(fileProtection, &errorLocation);
            SETLOGERROR5("Error 0x%08x setting file protection level from %d to %d (%s).\n", returnValHRb, (DWORD) currProtectionLevel, (DWORD) fileProtection, spController->ErrorLocationToString(errorLocation))
        }

        registryProtection	= SymProtectEvt::PROTECTION_DISABLED;
        returnValHRb = spController->GetRegistryProtection(&currProtectionLevel, &errorLocation);
        SETLOGERROR3("Error 0x%08x retrieving registry protection level (%s).\n", returnValHRb, spController->ErrorLocationToString(errorLocation))
        if (currProtectionLevel != registryProtection)
        {
            returnValHRb = spController->SetRegistryProtection(registryProtection, &errorLocation);
            SETLOGERROR5("Error 0x%08x setting registry protection level from %d to %d (%s).\n", returnValHRb, (DWORD) currProtectionLevel, (DWORD) registryProtection, spController->ErrorLocationToString(errorLocation))
        }
        
        processProtection	= (SymProtectEvt::SP_PROTECTION_LEVEL) GetVal(storageSettingsKeyHandle, szReg_Val_SymProtect_Protection_Process, processProtection);
        returnValHRb = spController->GetProcessProtection(&currProtectionLevel, &errorLocation);
        SETLOGERROR3("Error 0x%08x retrieving process protection level (%s).\n", returnValHRb, spController->ErrorLocationToString(errorLocation))
        if (currProtectionLevel != processProtection)
        {
            returnValHRb = spController->SetProcessProtection(processProtection, &errorLocation);
            SETLOGERROR5("Error 0x%08x setting process protection level from %d to %d (%s).\n", returnValHRb, (DWORD) currProtectionLevel, (DWORD) processProtection, spController->ErrorLocationToString(errorLocation))
        }
        
        namedObjectProtection = (SymProtectEvt::SP_PROTECTION_LEVEL) GetVal(storageSettingsKeyHandle, szReg_Val_SymProtect_Protection_NamedObject, namedObjectProtection);
        returnValHRb = spController->GetNamedObjectProtection(&currProtectionLevel, &errorLocation);
        SETLOGERROR3("Error 0x%08x retrieving named object protection level (%s).\n", returnValHRb, spController->ErrorLocationToString(errorLocation))
        if (currProtectionLevel != namedObjectProtection)
        {
            returnValHRb = spController->SetNamedObjectProtection(namedObjectProtection, &errorLocation);
            SETLOGERROR5("Error 0x%08x setting named object protection level from %d to %d (%s).\n", returnValHRb, (DWORD) currProtectionLevel, (DWORD) namedObjectProtection, spController->ErrorLocationToString(errorLocation))
        }

		// Handle default authorizations
        if (GetSystemDirectory(directoryPath, sizeof(directoryPath)/sizeof(directoryPath[0])) != 0)
        {
            try
            {
                lastCharacter = CharPrev(directoryPath, directoryPath+lstrlen(directoryPath));
                if (*lastCharacter == '\\')
                    *lastCharacter = NULL;
                drwatsonPathname = directoryPath + std::string(DefaultAuthorization_DrWatson);
                dumpprepPathname = directoryPath + std::string(DefaultAuthorization_DumpPrep);
                if (IsWin2000())
                {
                    GetNodeName(directoryPath, &windowsSystemSubdirName);
                    csrssPathname = std::string(DefaultAuthorization_CSRSS_Prefix) + directoryPath + std::string(DefaultAuthorization_CSRSS_Filename);
                    smssPathname = std::string(DefaultAuthorization_SMSS_Prefix) + windowsSystemSubdirName + std::string(DefaultAuthorization_SMSS_Filename);
                }
                else
                {
                    csrssPathname = directoryPath + std::string(DefaultAuthorization_CSRSS_Filename);
                    smssPathname = directoryPath + std::string(DefaultAuthorization_SMSS_Filename);
                }
                noAuthorizations = GetVal(storageSettingsKeyHandle, szReg_Val_SymProtect_Authorization_Count, -1);
                if (noAuthorizations == -1)
                {
                    returnValHRb = AddLocationAuthorization(csrssPathname, "Internal debug support 1", spController);
                    SETERROR
                    returnValHRb = AddLocationAuthorization(smssPathname, "Internal debug support 2", spController);
                    SETERROR
                    returnValHRb = AddLocationAuthorization(drwatsonPathname, "Debug support", spController);
                    SETERROR
                    returnValHRb = AddLocationAuthorization(dumpprepPathname, "Dump support", spController);
                    SETERROR
                }
                else
                {
                    returnValHRb = RemoveLocationAuthorization(csrssPathname, "Internal debug support 1", spController);
                    SETERROR
                    returnValHRb = RemoveLocationAuthorization(smssPathname, "Internal debug support 2", spController);
                    SETERROR
                    returnValHRb = RemoveLocationAuthorization(drwatsonPathname, "Debug support", spController);
                    SETERROR
                    returnValHRb = RemoveLocationAuthorization(dumpprepPathname, "Dump support", spController);
                    SETERROR
                }
            }
            VP_CATCH_MEMORYEXCEPTIONS(storageManagerFunctions->dPrintf("SymProtect:  Out of memory processing default authorizations.\n"););
        }

        // Add any requested, local-only, automatic authorizations.  The strings used for these are encryted to frustrate hackers.
        // SSC install's request for MMC
        returnValDW = GetSystemDirectory(directoryPath, sizeof(directoryPath)/sizeof(directoryPath[0]));
        if ((returnValDW != 0) && (returnValDW <= (sizeof(directoryPath)/sizeof(directoryPath[0]))))
        {
            returnValDW = ERROR_SUCCESS;
            UnMakeEP(EncryptedText_MMCfilename, (int) strlen(EncryptedText_MMCfilename)+1, PASS_KEY3, PASS_KEY5, clearText, sizeof(clearText));
            sssnprintf(targetPathname, sizeof(targetPathname), "%s\\%s", directoryPath, clearText);
            returnValHRb = HandleRequestedAuthorization(storageSettingsKeyHandle, szReg_Val_SymProtect_InstallRequestAuthorization_MMC, targetPathname, returnValDW, spController);
            SETERROR
        }
        else
        {
            returnValDW = GetLastError();
            returnValHRb = HRESULT_FROM_WIN32(returnValDW);
            SETERROR
            storageManagerFunctions->dPrintf("SymProtect:  Error 0x%08x getting system directory.\n", returnValHRb);
        }
        // SCFA install's request for JAVA
        UnMakeEP(EncryptedText_JavawAppPath, (int) strlen(EncryptedText_JavawAppPath)+1, PASS_KEY3, PASS_KEY5, clearText, sizeof(clearText));
        returnValDW = RegOpenKeyEx(HKEY_LOCAL_MACHINE, clearText, NULL, KEY_READ, &keyHandle);
        if (returnValDW == ERROR_SUCCESS)
        {
            directoryPath[0] = NULL;
            UnMakeEP(EncryptedText_JavawPathValue, (int) strlen(EncryptedText_JavawPathValue)+1, PASS_KEY3, PASS_KEY5, clearText, sizeof(clearText));
            returnValDW = RegQueryValueEx(keyHandle, clearText, NULL, &valueType, (LPBYTE) &directoryPath, &directoryPathSize);
            if ((returnValDW == ERROR_SUCCESS) && (valueType == REG_SZ))
            {
                UnMakeEP(EncryptedText_JavawFilename, (int) strlen(EncryptedText_JavawFilename)+1, PASS_KEY3, PASS_KEY5, clearText, sizeof(clearText));
                sssnprintf(targetPathname, sizeof(targetPathname), "%s\\%s", directoryPath, clearText);
                returnValHRb = HandleRequestedAuthorization(storageSettingsKeyHandle, szReg_Val_SymProtect_InstallRequestAuthorization_Java, targetPathname, returnValDW, spController);
                SETERROR
            }
            RegCloseKey(keyHandle);
            keyHandle = NULL;
        }

	}

	// Shutdown and return
    if (spController != NULL)
    {
        returnValHRc = spController->Shutdown(&errorLocation);
        if (FAILED(returnValHRc))
            storageManagerFunctions->dPrintf("ISymProtectController::Shutdown failed, code 0x%08x (%s).\n", returnValHR, spController->ErrorLocationToString(errorLocation));
        returnValHRc = ISymProtectController::Destroy(&spController);
        if (FAILED(returnValHRc))
            storageManagerFunctions->dPrintf("ISymProtectController::Destroy failed, code 0x%08x.\n", returnValHR);
        spController = NULL;
    }
    if (storageSettingsKeyHandle != NULL)
    {
        storageManagerFunctions->Close(storageSettingsKeyHandle);
        storageSettingsKeyHandle = NULL;
    }
	storageManagerFunctions->dPrintf("SymProtect:  ImportSettings finished.\n");
    LeaveCriticalSection(&importSettingsLock);
    if (SUCCEEDED(returnValHR))
        return S_OK;
    else
        return returnValHR;
}

bool CSymProtectStorage::ExcludeEvent( SymProtectEventInfo& eventInfo )
// Filters out certain standard innocuous system events.
// Returns TRUE if event should be filtered out, else FALSE
{
	FilterCriteriaList::iterator	currFilterCriteria;
    SPActionTypeList::iterator      currActionType;
	std::string						nodeName;
    bool                            criteriaResult      = false;
    bool                            evaluatedCriteria   = false;
    bool                            currRuleResult      = false;

	EnterCriticalSection(&configLock);
    for (currFilterCriteria = config.filterList.begin(); (currFilterCriteria != config.filterList.end()) && (!currRuleResult); currFilterCriteria++)
    {
        evaluatedCriteria = false;
        criteriaResult = false;
        
        if (currFilterCriteria->matchEventTypes)
        {
            for (currActionType = currFilterCriteria->eventTypes.begin(); (currActionType != currFilterCriteria->eventTypes.end()) && (!criteriaResult); currActionType++)
            {
                if (eventInfo.actionType == *currActionType)
                {
                    criteriaResult = true;
                    currRuleResult = evaluatedCriteria ? (criteriaResult && currRuleResult) : criteriaResult;
                    evaluatedCriteria = true;
                }
            }
        }
        
        if (currFilterCriteria->matchActor)
        {
            if (currFilterCriteria->actorIsPathname)
            {
                criteriaResult = (_mbsicmp((BYTE*) eventInfo.actorProcessName.c_str(), (BYTE*) currFilterCriteria->actorFilename.c_str()) == 0);
                currRuleResult = evaluatedCriteria ? (criteriaResult && currRuleResult) : criteriaResult;
                evaluatedCriteria = true;
            }
            else
            {
                GetNodeName(eventInfo.actorProcessName, &nodeName);
                criteriaResult = (_mbsicmp((BYTE*) nodeName.c_str(), (BYTE*) currFilterCriteria->actorFilename.c_str()) == 0);
                currRuleResult = evaluatedCriteria ? (criteriaResult && currRuleResult) : criteriaResult;
                evaluatedCriteria = true;
            }
        }
        
        if (currFilterCriteria->matchTarget)
        {
            criteriaResult = (_mbsicmp((BYTE*) eventInfo.targetPathname.c_str(), (BYTE*) currFilterCriteria->targetPathname.c_str()) == 0);
            currRuleResult = evaluatedCriteria ? (criteriaResult && currRuleResult) : criteriaResult;
            evaluatedCriteria = true;
        }
        
	}
	LeaveCriticalSection(&configLock);

    return currRuleResult;
}

HRESULT CSymProtectStorage::EnsureServiceDependanciesRunning( void )
// Ensures that all needed dependant services are running
// Returns S_OK if they are, S_FALSE if not yet ready to, else the error code of the failure
{
    HRESULT         returnValHR             = E_FAIL;

    // Start ccSettings
    returnValHR = EnsureServiceRunning("ccSetMgr", 30000);
    if (SUCCEEDED(returnValHR))
    {
        if (returnValHR == S_OK)
            storageManagerFunctions->dPrintf("SymProtect note:  Started ccSetMgr.\n");
    }
    else
    {
        storageManagerFunctions->dPrintf("SymProtect note:  Error 0x%08x ensuring ccSetMgr is running.\n", returnValHR);
    }
    
    if (SUCCEEDED(returnValHR))
    {
        // Is ccEventManager running?
        returnValHR = IsServiceRunning("ccEvtMgr");
        if (returnValHR == S_OK)
        {
            // Yes - SP has everything it needs running
            storageManagerFunctions->dPrintf("SymProtect:  EnsureServiceDependanciesRunning:  event manager alread running.\n");
        }
        else if (returnValHR == S_FALSE)
        {
            // No.  Is ccEventManager configured yet?
            returnValHR = IsEventManagerConfigured(5*60*1000);
            if ((returnValHR == S_OK) || (returnValHR == S_TIMEOUT))
            {
                // Yes, or we've waited too long, so start it anyway.
                if (returnValHR == S_TIMEOUT)
                    storageManagerFunctions->dPrintf("SymProtect note:  Event manager configuration timeout exceeded, starting anyway.\n");
                returnValHR = EnsureServiceRunning("ccEvtMgr", 30000);
                if (SUCCEEDED(returnValHR))
                {
                    if (returnValHR == S_OK)
                        storageManagerFunctions->dPrintf("SymProtect note:  Started ccEvtMgr.\n");
                    returnValHR = S_OK;
                }
                else
                {
                    storageManagerFunctions->dPrintf("SymProtect note:  Error 0x%08x ensuring ccEvtMgr is running.\n", returnValHR);
                }
            }
            else if (returnValHR == S_FALSE)
            {
                // No - keep waiting to start it
                storageManagerFunctions->dPrintf("SymProtect:  ccEvent not yet configured in ccSettings - cannot communicate with SymProtect at this time.\n");
            }
        }
        else
        {
            storageManagerFunctions->dPrintf("SymProtect:  EnsureServiceDependanciesRunning:  IsServiceRunning failed 0x%08x.\n", returnValHR);
        }
    }
    
    return returnValHR;
}

HRESULT CSymProtectStorage::IsEventManagerConfigured( DWORD timeout )
// Determines if all the settings event manager needs to start up correctly have been written out to ccSettings
// Returns S_OK if event manager is good to go, S_FALSE if not but timeout from 1st check time hasn't elapsed, S_TIMEOUT if not and timeout has elapsed,
// else the error code of the failure.
{
	HKEY									keyHandle					= NULL;
    bool                                    installConfigured           = false;
    HANDLE                                  msiMutexHandle              = NULL;
    bool                                    msiMutexSet                 = false;
    DWORD                                   currTime                    = 0;
    DWORD                                   elapsedTime                 = 0;
    static DWORD                            firstCheckTimestamp         = 0;
    HRESULT                                 returnValHR                 = E_FAIL;
    SYMRESULT                               returnValSR                 = SYMERR_UNKNOWN;
	DWORD									returnValDW					= ERROR_OUT_OF_PAPER;

    // Start ccSettingsManager
    returnValHR = EnsureServiceRunning("ccSetMgr");
    if (SUCCEEDED(returnValHR))
    {
        if (returnValHR == S_OK)
            storageManagerFunctions->dPrintf("SymProtect note:  Started ccSetMgr.\n");
    }
    else
    {
        storageManagerFunctions->dPrintf("SymProtect note:  Error 0x%08x ensuring ccSetMgr is running.\n", returnValHR);
    }

    if (SUCCEEDED(returnValHR))
    {
        // Check for the flag indicating the WriteCcSettings custom action has completed.
		returnValDW = RegOpenKeyEx(HKEY_LOCAL_MACHINE, WriteCcSettingsDoneFlagKey, NULL, KEY_READ, &keyHandle);
		if (returnValDW == ERROR_SUCCESS)
		{
			installConfigured = true;
			RegCloseKey(keyHandle);
			keyHandle = NULL;
		}

        // Check for the _MSIMutex, indicating an install in progress.  This is indicated by the mutex existance,
		// not it's state, as MSI does not give us rights to Wait on it, but just in case, the code supports it.
		msiMutexHandle = OpenMutex(READ_CONTROL, FALSE, "_MSIExecute");
		if (msiMutexHandle != NULL)
		{
			returnValDW = WaitForSingleObject(msiMutexHandle, 0);
			if (returnValDW == WAIT_FAILED) 
			{
				if (GetLastError() == ERROR_ACCESS_DENIED)
					msiMutexSet = true;
			}
			else if (returnValDW == WAIT_TIMEOUT)
			{
				msiMutexSet = true;
			}
			CloseHandle(msiMutexHandle);
			msiMutexHandle = NULL;
		}
        else if (GetLastError() == ERROR_ACCESS_DENIED)
        {
            msiMutexSet = true;
        }
        
        // Log out status
        storageManagerFunctions->dPrintf("SymProtect:  ic = %d, mm = %d.\n", (DWORD) installConfigured, (DWORD) msiMutexSet);
        // Are we installing and the install configured flag has not yet been set?
        if (msiMutexSet && !installConfigured)
        {
            // Yes.  Determine if we've timed out or not.
            currTime = GetTickCount();
            InterlockedCompareExchange((LONG*) &firstCheckTimestamp, (LONG) currTime, 0);
            if (currTime >= firstCheckTimestamp)
                elapsedTime = currTime - firstCheckTimestamp;
            else
                elapsedTime = (0xffffffff - firstCheckTimestamp) + currTime;
            if (elapsedTime >= timeout)
                returnValHR = S_TIMEOUT;
            else
                returnValHR = S_FALSE;
        }
        else
        {
            // No - event manager should be set to go
            returnValHR = S_OK;
        }
    }

    return returnValHR;
}

HRESULT CSymProtectStorage::EnsureServiceRunning( LPCTSTR serviceName, DWORD timeout )
// Ensures serviceName is running
// Returns S_OK if service was started, S_FALSE if it is already running, HFW32(WAIT_TIMEOUT) if timed out before finished starting, else the error code of the failure
{
	SC_HANDLE		servManHandle			= NULL;
	SC_HANDLE		serviceHandle			= NULL;
	SERVICE_STATUS	status;
	DWORD           startTime               = 0;
    DWORD           currTime                = 0;
    DWORD           elapsedTime             = 0;
	const DWORD		pollTime				= 100;
	HRESULT			returnValHR				= E_FAIL;
	BOOL			returnValBOOL			= FALSE;

	//Open the service with as little access as needed
	servManHandle = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
	if (servManHandle != NULL)
	{
		serviceHandle = OpenService(servManHandle, serviceName, SERVICE_QUERY_STATUS);
		if (QueryServiceStatus(serviceHandle, &status))
		{
			if (status.dwCurrentState == SERVICE_RUNNING)
				returnValHR = S_FALSE;
		}
		CloseServiceHandle(serviceHandle);
		serviceHandle = NULL;
	}
	else
	{
		returnValHR = HRESULT_FROM_WIN32(GetLastError());
	}

	// Is the service running?
	if ((returnValHR != S_FALSE) && (servManHandle != NULL))
	{
		// No.  Re-open it and start it.
		serviceHandle = OpenService(servManHandle, serviceName, SERVICE_QUERY_STATUS | SERVICE_START);
		if (serviceHandle != NULL)
		{
            status.dwCurrentState = SERVICE_START_PENDING;
			returnValBOOL = StartService(serviceHandle, 0, NULL);
			startTime = GetTickCount();
            while (returnValBOOL && (status.dwCurrentState == SERVICE_START_PENDING) && (elapsedTime <= timeout))
			{
				//Wait and check status
				returnValBOOL = QueryServiceStatus(serviceHandle, &status);
                currTime = GetTickCount();
                if (currTime >= startTime)
                    elapsedTime = currTime - startTime;
                else
                    elapsedTime = (0xffffffff - startTime) + currTime;
				Sleep(pollTime);
			}
			if (status.dwCurrentState == SERVICE_RUNNING)
				returnValHR = S_OK;
			else if (!returnValBOOL)
				returnValHR = HRESULT_FROM_WIN32(GetLastError());
			else
				returnValHR = HRESULT_FROM_WIN32(WAIT_TIMEOUT);
			CloseServiceHandle(serviceHandle);
			serviceHandle = NULL;
		}
		else
		{
			returnValHR = HRESULT_FROM_WIN32(GetLastError());
		}
	}

	//Cleanup and shudown
	if (servManHandle != NULL)
	{
		CloseServiceHandle(servManHandle);
		servManHandle = NULL;
	}
	return returnValHR;
}

HRESULT CSymProtectStorage::IsServiceRunning( LPCTSTR serviceName, DWORD timeOut )
// Returns S_OK if service serviceName is running, S_FALSE if not, HFW32(WAIT_TIMEOUT) if timed out waiting for service to finish starting and a timeout was specified
{
    SC_HANDLE       servManHandle           = NULL;
    SC_HANDLE       serviceHandle           = NULL;
    SERVICE_STATUS  status;
    DWORD           startTime               = 0;
    DWORD           currTime                = 0;
    DWORD           elapsedTime             = 0;
    const DWORD     pollTime                = 100;
    HRESULT         returnValHR             = E_FAIL;
    BOOL            returnValBOOL           = FALSE;

    //Open the service with as little access as needed
    servManHandle = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
    if (servManHandle != NULL)
    {
        serviceHandle = OpenService(servManHandle, serviceName, SERVICE_QUERY_STATUS);
        if (QueryServiceStatus(serviceHandle, &status))
        {
            // Is the service already running?
            if (status.dwCurrentState == SERVICE_RUNNING)
            {
                // Yes
                returnValHR = S_OK;            
            }
            else if ((status.dwCurrentState == SERVICE_START_PENDING) && (timeOut != 0))
            {
                // No, but it's starting up - wait for it to finish
                startTime = GetTickCount();
                while (returnValBOOL && (status.dwCurrentState == SERVICE_START_PENDING) && (elapsedTime <= timeOut))
                {
                    //Wait and check status
                    returnValBOOL = QueryServiceStatus(serviceHandle, &status);
                    currTime = GetTickCount();
                    if (currTime >= startTime)
                        elapsedTime = currTime - startTime;
                    else
                        elapsedTime = (0xffffffff - startTime) + currTime;
                    Sleep(pollTime);
                }
                if (status.dwCurrentState == SERVICE_RUNNING)
                    returnValHR = S_OK;
                else if (!returnValBOOL)
                    returnValHR = HRESULT_FROM_WIN32(GetLastError());
                else
                    returnValHR = HRESULT_FROM_WIN32(WAIT_TIMEOUT);
            }
            else
            {
                returnValHR = S_FALSE;
            }
        }
        CloseServiceHandle(serviceHandle);
        serviceHandle = NULL;
    }
    else
    {
        returnValHR = HRESULT_FROM_WIN32(GetLastError());
    }

    //Cleanup and shudown
    if (servManHandle != NULL)
    {
        CloseServiceHandle(servManHandle);
        servManHandle = NULL;
    }
    return returnValHR;
}

HRESULT CSymProtectStorage::HandleRequestedAuthorization( HKEY settingsKeyHandle, LPCTSTR valueName, LPCTSTR filenameToAuthorize, DWORD filenameToAuthorizeDeterminedResult, ISymProtectController* spController )
// Handles an authorization requested by an install or other component.  The action taken is determined by the
// value of valueName, which must be an AuthorizationRequest enum value.  Logs failure.
// Synchronize changes here with InstallHelper.cpp::SetRequestedAuthorization
{
	HKEY									keyHandle							        = NULL;
    TCHAR                                   cipherText[PASS_MAX_CIPHER_TEXT_BYTES+1]    = {""};
    TCHAR                                   clearText[PASS_MAX_CIPHER_TEXT_BYTES+1]     = {""};
    int                                     noItemsRead                                 = 0;
    TCHAR                                   readMachineName[MAX_PATH]                   = {""};
	HMODULE                                 winsockModuleHandle                         = NULL;
	LPFN_GETHOSTNAME                        gethostnamePtr                              = NULL;
    TCHAR                                   hostName[MAX_PATH]                          = {""};
    TCHAR                                   productCodeString[2*MAX_PATH+1]		        = {""};
    _bstr_t                                 wideStringConverter;
    CLSID                                   productCodeCLSID					        = GUID_NULL;
	ISymProtectController::LocationAuthorizationList locationsAuthorized;
	ISymProtectController::LocationAuthorizationList::iterator currLocationAuthorization;
    INSTALLSTATE                            productInstallState					        = INSTALLSTATE_ABSENT;
    TCHAR                                   valueData[MAX_PATH+1]				        = {""};
    DWORD                                   valueDataSize						        = 0;
    MSIHANDLE                               msiHandle							        = NULL;
	DWORD									currRecognizedAuthorizationNo		        = 0;
	bool									authorizationRecognized				        = false;
	bool									authorizationExists					        = false;
    ISymProtectController::ErrorLocation    errorLocation						        = ISymProtectController::ErrorLocation_None;
    DWORD                                   returnValDW							        = ERROR_OUT_OF_PAPER;
    HRESULT                                 returnValHR							        = E_FAIL;

    // Validate parameters
    if (valueName == NULL)
        return E_POINTER;
    if (filenameToAuthorize == NULL)
        return E_POINTER;
    if (settingsKeyHandle == NULL)
        return E_INVALIDARG;
    if (spController == NULL)
        return E_POINTER;

    // Initialize
    // Read command string
	returnValDW = RegOpenKeyEx(settingsKeyHandle, szReg_Key_LocalData, NULL, KEY_READ, &keyHandle);
	if (returnValDW == ERROR_SUCCESS)
	{
	    GetStr(keyHandle, valueName, cipherText, sizeof(cipherText)/sizeof(cipherText[0]), "");
		RegCloseKey(keyHandle);
		keyHandle = NULL;
	}
    if (_mbsicmp((BYTE*) cipherText, (BYTE*) "") == 0)
        return S_FALSE;

    // Validate command string
    UnMakeEP(cipherText, (int) strlen(cipherText)+1, PASS_KEY3, PASS_KEY5, clearText, sizeof(clearText));
    noItemsRead = sscanf(clearText, "%100s %100s", &readMachineName, &productCodeString);
	ZeroMemory(clearText, sizeof(clearText));
    if (noItemsRead == 2)
    {
        // Ensure the product code string is a valid CLSID
        try
        {
			returnValHR = S_OK;
            wideStringConverter = productCodeString;
        }
        VP_CATCH_MEMORYEXCEPTIONS(returnValHR = E_OUTOFMEMORY;);
        if (SUCCEEDED(returnValHR))
        {
            returnValHR = CLSIDFromString(wideStringConverter, &productCodeCLSID);
        }
        else
        {
            storageManagerFunctions->dPrintf("SymProtect:  Error 0x%08x, invalid command for special request %s.\n", returnValHR, valueName);
            return E_ABORT;
        }
    }
    else
    {
        storageManagerFunctions->dPrintf("SymProtect:  Error 0x%08x, Bad command string for special request %s.\n", returnValHR, valueName);
        return E_ABORT;
    }
    if (filenameToAuthorizeDeterminedResult != ERROR_SUCCESS)
    {
        storageManagerFunctions->dPrintf("SymProtect:  Error %d processing special request %s.\n", filenameToAuthorizeDeterminedResult, valueName);
        return E_FAIL;
    }

	// Determine if an authorization for the requested filename already exists
	returnValHR = spController->AuthorizationLocationEnum(&locationsAuthorized, &errorLocation);
	if (SUCCEEDED(returnValHR))
	{
		authorizationExists = false;
		for(currLocationAuthorization = locationsAuthorized.begin(); currLocationAuthorization != locationsAuthorized.end(); currLocationAuthorization++)
		{
			if (_mbsicmp((BYTE*) currLocationAuthorization->pathname.c_str(), (BYTE*) filenameToAuthorize) == 0)
			{
                authorizationExists = true;
				break;
			}
		}
	}

    // Determine the product's install state
    productInstallState = MsiQueryProductState(productCodeString);
    if ((productInstallState == INSTALLSTATE_DEFAULT) || (productInstallState == INSTALLSTATE_ABSENT))
    {
        // Add the authorization
        if (!authorizationExists)
        {
			// Verify that the the decrypted host name matches the actual machine name
			// Do this only for adding the authorization, because the machine name may well change
			// when uninstall time comes
            UnMakeEP(EncryptedText_WinsockFilename, (int) strlen(EncryptedText_WinsockFilename)+1, PASS_KEY3, PASS_KEY5, clearText, sizeof(clearText));
			winsockModuleHandle = LoadLibrary(clearText);
			ZeroMemory(clearText, sizeof(clearText));
			if (winsockModuleHandle != NULL)
			{
                UnMakeEP(EncryptedText_WinSockFunctionName_Gethostname, (int) strlen(EncryptedText_WinSockFunctionName_Gethostname)+1, PASS_KEY3, PASS_KEY5, clearText, sizeof(clearText));
				gethostnamePtr = (LPFN_GETHOSTNAME) GetProcAddress(winsockModuleHandle, clearText);
				ZeroMemory(clearText, sizeof(clearText));
				if (gethostnamePtr != NULL)
				{
					returnValDW = gethostnamePtr(hostName, sizeof(hostName));
					if (returnValDW == ERROR_SUCCESS)
					{
						if (_mbsicmp((BYTE*) hostName, (BYTE*) readMachineName) == 0)
						{
							returnValHR = S_OK;
						}
						else
						{
							returnValHR = E_ABORT;
							storageManagerFunctions->dPrintf("SymProtect:  Error 0x%08x, invalid command string for special request %s.\n", returnValHR, valueName);
						}
					}
					else
					{
						returnValDW = GetLastError();
						returnValHR = HRESULT_FROM_WIN32(returnValDW);
						storageManagerFunctions->dPrintf("SymProtect:  Error %d occured verifying command string.\n", returnValDW);
					}
				}
				else
				{
					returnValDW = GetLastError();
					returnValHR = HRESULT_FROM_WIN32(returnValDW);
					storageManagerFunctions->dPrintf("SymProtect:  Failure %d occured verifying command string.\n", returnValDW);
				}
				FreeLibrary(winsockModuleHandle);
				winsockModuleHandle = NULL;
			}
			else
			{
				returnValDW = GetLastError();
				returnValHR = HRESULT_FROM_WIN32(returnValDW);
				storageManagerFunctions->dPrintf("SymProtect:  Error %d occured checking command string.\n", returnValDW);
			}

			if (SUCCEEDED(returnValHR))
			{
				returnValHR = spController->AuthorizationLocationAdd(filenameToAuthorize, true, &errorLocation);
		        if (SUCCEEDED(returnValHR))
			        storageManagerFunctions->dPrintf("SymProtect:  Processed special request %s.\n", valueName);
				else
					storageManagerFunctions->dPrintf("SymProtect:  Error 0x%08x adding special request %s filename authorization %s (%s)\n", returnValHR, valueName, filenameToAuthorize, spController->ErrorLocationToString(errorLocation));
			}
        }
    }
    else
    {
        // Remove the authorization
		if (authorizationExists)
		{
	        returnValHR = spController->AuthorizationLocationDelete(filenameToAuthorize, &errorLocation);
		    if (SUCCEEDED(returnValHR))
				storageManagerFunctions->dPrintf("SymProtect:  Deprocessed special request %s\n", valueName);
			else
				storageManagerFunctions->dPrintf("SymProtect:  Error 0x%08x removing special request %s filename authorization %s (%s)\n", returnValHR, valueName, filenameToAuthorize, spController->ErrorLocationToString(errorLocation));
		}
        // Next, remove the authorization request as it is no longer necessary
		returnValDW = RegOpenKeyEx(settingsKeyHandle, szReg_Key_LocalData, NULL, KEY_READ | KEY_SET_VALUE, &keyHandle);
		if (returnValDW == ERROR_SUCCESS)
		{
			returnValDW = RegDeleteValue(keyHandle, valueName);
			if (returnValDW != ERROR_SUCCESS)
				storageManagerFunctions->dPrintf("SymProtect:  Error %d deleting special request %s\n", returnValDW, valueName);
			RegCloseKey(keyHandle);
			keyHandle = NULL;
		}
		else
		{
			storageManagerFunctions->dPrintf("SymProtect:  Error %d removing special request %s\n", returnValDW, valueName);
		}
		// Consider this success if we successfully removed the authorization or it wasn't there
		if (SUCCEEDED(returnValHR) || !authorizationExists)
			returnValHR = S_OK;
		else
			returnValHR = HRESULT_FROM_WIN32(returnValDW);
    }
    return returnValHR;
}

HRESULT CSymProtectStorage::AddLocationAuthorization( const std::string& location, const std::string& locationLogDescription, ISymProtectController* spController )
// Ensures location is in the SymProtect location authorization list.
// Returns S_OK if authorization added, S_FALSE if already present, else the error code of the failure
{
    ISymProtectController::LocationAuthorizationList locationsAuthorized;
    ISymProtectController::LocationAuthorizationList::iterator currLocationAuthorization;
    bool                                    authorizationExists                 = false;
    ISymProtectController::ErrorLocation    errorLocation                       = ISymProtectController::ErrorLocation_None;
    DWORD                                   returnValDW                         = ERROR_OUT_OF_PAPER;
    HRESULT                                 returnValHR                         = E_FAIL;

    // Validate parameters
    if (location == "")
        return E_INVALIDARG;
    if (spController == NULL)
        return E_POINTER;

    // Determine if an authorization for the requested filename already exists
    returnValHR = spController->AuthorizationLocationEnum(&locationsAuthorized, &errorLocation);
    if (SUCCEEDED(returnValHR))
    {
        authorizationExists = false;
        for(currLocationAuthorization = locationsAuthorized.begin(); currLocationAuthorization != locationsAuthorized.end(); currLocationAuthorization++)
        {
            if (_mbsicmp((BYTE*) currLocationAuthorization->pathname.c_str(), (BYTE*) location.c_str()) == 0)
            {
                authorizationExists = true;
                break;
            }
        }
    }
    else
    {
        storageManagerFunctions->dPrintf("SymProtect:  Error 0x%08x determining if authorization for %s exists (%s).\n", returnValHR, locationLogDescription.c_str(), ISymProtectController::ErrorLocationToString(errorLocation));
    }

    if (authorizationExists)
    {
        returnValHR = S_FALSE;
    }
    else
    {
        returnValHR = spController->AuthorizationLocationAdd(location.c_str(), true, &errorLocation);
        if (SUCCEEDED(returnValHR))
            storageManagerFunctions->dPrintf("SymProtect:  Added default authorization for %s.\n", locationLogDescription.c_str());
        else
            storageManagerFunctions->dPrintf("SymProtect:  Error 0x%08x adding default authorization for %s (%s).\n", returnValHR, locationLogDescription.c_str(), ISymProtectController::ErrorLocationToString(errorLocation));
    }

    return returnValHR;
}

HRESULT CSymProtectStorage::RemoveLocationAuthorization( const std::string& location, const std::string& locationLogDescription, ISymProtectController* spController )
// Removes location from the SymProtect location authorization list.
// Returns S_OK if location was removed, S_FALSE is not present to begin with, else the error code of the failure
{
    ISymProtectController::LocationAuthorizationList                locationsAuthorized;
    ISymProtectController::LocationAuthorizationList::iterator      currLocationAuthorization;
    bool                                    authorizationExists                 = false;
    ISymProtectController::ErrorLocation    errorLocation                       = ISymProtectController::ErrorLocation_None;
    DWORD                                   returnValDW                         = ERROR_OUT_OF_PAPER;
    HRESULT                                 returnValHR                         = E_FAIL;

    // Validate parameters
    if (location == "")
        return E_INVALIDARG;
    if (spController == NULL)
        return E_POINTER;

    // Determine if an authorization for the requested filename already exists
    returnValHR = spController->AuthorizationLocationEnum(&locationsAuthorized, &errorLocation);
    if (SUCCEEDED(returnValHR))
    {
        authorizationExists = false;
        for(currLocationAuthorization = locationsAuthorized.begin(); currLocationAuthorization != locationsAuthorized.end(); currLocationAuthorization++)
        {
            if (_mbsicmp((BYTE*) currLocationAuthorization->pathname.c_str(), (BYTE*) location.c_str()) == 0)
            {
                authorizationExists = true;
                break;
            }
        }
    }
    else
    {
        storageManagerFunctions->dPrintf("SymProtect:  Error 0x%08x determining if authorization for %s exists (%s).\n", returnValHR, locationLogDescription.c_str(), ISymProtectController::ErrorLocationToString(errorLocation));
    }

    if (authorizationExists)
    {
        returnValHR = spController->AuthorizationLocationDelete(location.c_str(), &errorLocation);
        if (SUCCEEDED(returnValHR))
            storageManagerFunctions->dPrintf("SymProtect:  Removed default authorization for %s.\n", locationLogDescription.c_str());
        else
            storageManagerFunctions->dPrintf("SymProtect:  Error 0x%08x removing default authorization for %s (%s).\n", returnValHR, locationLogDescription.c_str(), ISymProtectController::ErrorLocationToString(errorLocation));
    }
    else
    {
        returnValHR = S_FALSE;
    }

    return returnValHR;
}

bool CSymProtectStorage::IsWinNT( void )
// Returns TRUE if running on Windows NT, else FALSE
{
	OSVERSIONINFO	versionInfo;
	bool			returnValBool		= false;

	ZeroMemory(&versionInfo, sizeof(versionInfo));
	if (GetVersionEx(&versionInfo))
		returnValBool = (versionInfo.dwPlatformId & VER_PLATFORM_WIN32_NT) != 0;

	return returnValBool;
}

bool CSymProtectStorage::IsWin2000( void )
// Returns TRUE if running on Windows 2000, else FALSE
{
	OSVERSIONINFO	versionInfo;
	bool			returnValBool		= false;

	ZeroMemory(&versionInfo, sizeof(versionInfo));
	versionInfo.dwOSVersionInfoSize = sizeof(versionInfo);
	if (GetVersionEx(&versionInfo))
		returnValBool = ((versionInfo.dwPlatformId & VER_PLATFORM_WIN32_NT) != 0) && (versionInfo.dwMajorVersion == 5) && (versionInfo.dwMinorVersion == 0);
	return returnValBool;
}

HRESULT CSymProtectStorage::GetProcessOwner( DWORD processID, PSID ownerSID, DWORD ownerSIDsize )
// Sets *ownerSID equal to a copy of the SID for the user account that processID is logged in under
{
	HANDLE		processHandle	= NULL;
	TOKEN_USER*	tokenUserInfo	= NULL;
	BYTE*		dataBuffer		= NULL;
	DWORD		dataBufferSize	= 0;
	DWORD		actualDataSize	= 0;
	DWORD		returnValDW		= ERROR_SUCCESS;
	HRESULT		returnValHR		= E_FAIL;

	processHandle = OpenProcess(PROCESS_ALL_ACCESS, TRUE, processID);
	if (processHandle != NULL)
	{
		HANDLE tokenHandle = NULL;
		if (OpenProcessToken(processHandle, TOKEN_QUERY, &tokenHandle))		// TOKEN_QUERY
		{
			GetTokenInformation(tokenHandle, TokenUser, (LPVOID) &tokenUserInfo, 0, &actualDataSize);
			if (ownerSIDsize >= actualDataSize)
			{
				dataBufferSize = actualDataSize;
				dataBuffer = new(std::nothrow) BYTE[dataBufferSize];
				if (dataBuffer != NULL)
				{
					if (GetTokenInformation(tokenHandle, TokenUser, (LPVOID) dataBuffer, dataBufferSize, &actualDataSize))
					{
						tokenUserInfo = (TOKEN_USER*) dataBuffer;
						if (CopySid(GetLengthSid(tokenUserInfo->User.Sid), ownerSID, tokenUserInfo->User.Sid))
							returnValDW = ERROR_SUCCESS;
						else
							returnValDW = GetLastError();
						tokenUserInfo = NULL;
					}
					else
					{
						returnValDW = GetLastError();
					}

					delete [] dataBuffer;
					dataBuffer = NULL;
					dataBufferSize = 0;
				}
			}
			else
			{
				returnValDW = ERROR_INSUFFICIENT_BUFFER;
			}
			CloseHandle(tokenHandle);
			tokenHandle = NULL;
		}
		else
		{
			returnValHR = GetLastError();
		}
		CloseHandle(processHandle);
		processHandle = NULL;
	}
	return HRESULT_FROM_WIN32(returnValDW);
}

HRESULT CSymProtectStorage::LoadString( UINT resourceID, std::string* stringBuffer )
// Loads a string resource and sets *stringBuffer equal to it as an STL string
{
    TCHAR       stringBufferTC[2*MAX_PATH+1]    = {""};
    HRESULT     returnValHR                     = E_FAIL;

    ::LoadString(moduleHandle, resourceID, stringBufferTC, sizeof(stringBufferTC)/sizeof(stringBufferTC[0]));
    try
    {
        *stringBuffer = stringBufferTC;
        returnValHR = S_OK;
    }
    catch (std::bad_alloc&)
    {
        returnValHR = E_OUTOFMEMORY;
    }
	return returnValHR;
}

DWORD CSymProtectStorage::GetStr( HKEY keyHandle, LPCTSTR valueName, LPTSTR valueData, DWORD valueDataSize, LPCTSTR defaultValueData )
// Reads in a string value from the registry
{
	DWORD		valueType				= REG_DWORD;
	DWORD		actualValueDataSize		= 0;
	DWORD		returnValDW				= ERROR_FILE_NOT_FOUND;

	actualValueDataSize = valueDataSize;
	returnValDW = SymSaferRegQueryValueEx(keyHandle, valueName, NULL, &valueType, (BYTE*) valueData, &actualValueDataSize);
	if (returnValDW != ERROR_SUCCESS)
        ssStrnCpy(valueData, defaultValueData, valueDataSize);
	return returnValDW;
}

DWORD CSymProtectStorage::GetVal( HKEY keyHandle, LPCTSTR valueName, DWORD defaultValue )
// Reads in a DWORD value from the registry
{
	DWORD		valueType		= REG_DWORD;
	DWORD		valueData		= 0;
	DWORD		valueDataSize	= 0;
	DWORD		returnValDW		= ERROR_FILE_NOT_FOUND;

	valueDataSize = sizeof(valueData);
	returnValDW = SymSaferRegQueryValueEx(keyHandle, valueName, NULL, &valueType, (BYTE*) &valueData, &valueDataSize);
	if (returnValDW == ERROR_SUCCESS)
		returnValDW = valueData;
	else
		returnValDW = defaultValue;
	return returnValDW;
}

HRESULT CSymProtectStorage::AuthorizationListFilterByType(AuthorizationList authorizations, const type_info& typeToSelect, AuthorizationList* authorizationsOfType)
// Sets *authorizationsOfType equal to an in-order list of all authorizations members whose typeid is typeToSelect
// Returns S_OK on success, else the error code of the failure
{
	AuthorizationList::iterator			currAuthorization;
    HRESULT                             returnValHR         = E_FAIL;

	if (authorizationsOfType == NULL)
		return E_POINTER;

    returnValHR = S_OK;
	authorizationsOfType->clear();
    for (currAuthorization = authorizations.begin(); (currAuthorization != authorizations.end()) && (SUCCEEDED(returnValHR)); currAuthorization++)
	{
		if (typeid(*(*currAuthorization)) == typeToSelect)
        {
            try
            {
    			authorizationsOfType->push_back(*currAuthorization);
            }
            catch (std::bad_alloc&)
            {
                returnValHR = E_OUTOFMEMORY;
            }
        }
	}
	return returnValHR;
}

STDAPI DllRegisterServer()
{
	return DllRegisterServerDo<CSymProtectStorage>();
}

STDAPI DllUnregisterServer()
{
	return DllUnregisterServerDo<CSymProtectStorage>();
}
