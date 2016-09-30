// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#include <io.h>
#include <fcntl.h>
#include <share.h>
#define _WIN32_DCOM
#include "stdafx.h"
#include "CRTVScanStorage.h"
#include "vpstrutils.h"

// Constants
const HANDLE		CRTVScanStorage::DefaultStorageHandle			= (HANDLE) 1;
const DWORD			CRTVScanStorage::DefaultInstanceID				= 1;

// Statics
STORAGEINFO			CRTVScanStorage::storageInfo;
IDEF				CRTVScanStorage::instanceDefinition;
STORAGESTATUS		CRTVScanStorage::currentStatus;
STORAGEDATA			CRTVScanStorage::storageData;

SFUNCTIONS			CRTVScanStorage::storageFunctions;
NODEFUNCTIONS		CRTVScanStorage::snodeFunctions;
SFILETABLE			CRTVScanStorage::ioFunctions;
PSSFUNCTIONS		CRTVScanStorage::storageManagerFunctions		= NULL;

HANDLE				CRTVScanStorage::exitEventHandle				= NULL;
TCHAR				CRTVScanStorage::currentUserName[MAX_PATH+1]	= {""};
bool				CRTVScanStorage::initialized					= false;

CRTVScanStorage*	CRTVScanStorage::storageObject					= NULL;

DWORD CRTVScanStorage::CoreInitialize( DWORD flags, PSTORAGEINFO* rtvscanStorageInfo, HANDLE* storageInstance, PSSFUNCTIONS newStorageManagerFunctions )
{
	StorageConfig	config;
	HKEY			storageKeyHandle	= NULL;
	DWORD			returnValDW			= ERROR_GENERAL;

	// Ensure COM has been initialized for us
	CoInitializeEx(NULL, COINIT_MULTITHREADED);

	// Validate state
	if (initialized)
		return ERROR_FUNCTION_NOT_SUPPORTED;
	// Validate parameters
	if (newStorageManagerFunctions == NULL)
		return ERROR_BAD_PARAM;
	if ((flags & S_WANT_RTS) == 0)
        return ERROR_FUNCTION_NOT_SUPPORTED;
	if (rtvscanStorageInfo == NULL)
		return ERROR_BAD_PARAM;
	if (storageInstance == NULL)
		return ERROR_BAD_PARAM;
	// Validate state
	if (initialized)
		return ERROR_FUNCTION_NOT_SUPPORTED;

	// Initialize
	ZeroMemory(&storageInfo, sizeof(storageInfo));
	ZeroMemory(&instanceDefinition, sizeof(instanceDefinition));
	ZeroMemory(&currentStatus, sizeof(currentStatus));
	ZeroMemory(&storageData, sizeof(storageData));
	ZeroMemory(&storageFunctions, sizeof(storageFunctions));
	ZeroMemory(&snodeFunctions, sizeof(snodeFunctions));
	ZeroMemory(&ioFunctions, sizeof(ioFunctions));
	storageManagerFunctions = NULL;
	exitEventHandle = NULL;
	ZeroMemory(&currentUserName, sizeof(currentUserName));

	// Get the storage name - necessary to proceed
	returnValDW = GetConfig(&config);
	if (returnValDW != ERROR_SUCCESS)
		return returnValDW;

	// Open the storage configuration key and create the shutdown event
	storageManagerFunctions = newStorageManagerFunctions;
    storageManagerFunctions->Open(reinterpret_cast<HKEY>(HKEY_VP_STORAGE_ROOT), const_cast<char*>(config.storageName.c_str()), &storageKeyHandle);
    if (storageKeyHandle != NULL)
		storageManagerFunctions->Open(storageKeyHandle, szReg_Key_Storage_RealTime, &(storageInfo.hRTSConfigKey));
	if ( storageInfo.hRTSConfigKey == NULL )
		storageManagerFunctions->Create(storageKeyHandle, szReg_Key_Storage_RealTime, &(storageInfo.hRTSConfigKey));
	if ( storageInfo.hRTSConfigKey != NULL )
	{
		exitEventHandle = CreateEvent(NULL, TRUE, FALSE, NULL);
		if (exitEventHandle != NULL)
			returnValDW = ERROR_SUCCESS;
		else
			returnValDW = ERROR_MEMORY;
	}
	else
	{
		returnValDW = ERROR_NO_KEY;
	}

	// Initialize storage configuration info for RTVScan
	if (returnValDW == ERROR_SUCCESS)
	{
		// STORAGEINFO
		storageInfo.Size				= sizeof(storageInfo);
		storageInfo.Version				= 0x01;
		storageInfo.InstanceCount		= 1;
		storageInfo.InstanceBlocks		= &instanceDefinition;
		storageInfo.Functions			= &storageFunctions;
		storageInfo.Status				= &currentStatus;
		strcpy(storageInfo.Name, config.storageName.c_str());
		storageInfo.InfFilename[0]		= '\0';
		storageInfo.Type				= IT_CAN_RTS;
		storageInfo.RTSData				= &storageData;
		storageInfo.MaxThreads			= 1;

		// Instance Definition - one per logical volume supported by this storage
		instanceDefinition.InstanceID	= DefaultInstanceID;
		instanceDefinition.Type			= IT_CAN_RTS;
		strcpy(instanceDefinition.DisplayName, "");			// Unused for non-walk storages.
		strcpy(instanceDefinition.VolumeName, "");			// Unused for non-walk storages.

		// SFUNCTIONS implementation
		storageFunctions.FindFirstNode			= Redirect_FindFirstNode;
		storageFunctions.FindNextNode			= Redirect_FindNextNode;
		storageFunctions.CreateSNode			= Redirect_CreateSNode;
		storageFunctions.FindClose				= Redirect_FindClose;
		storageFunctions.Deinit					= Redirect_CoreDeInitialize;
		storageFunctions.BeginRTSWatch			= Redirect_BeginRTSWatch;
		storageFunctions.StopRTSWatches			= Redirect_StopRTSWatches;
		storageFunctions.RefreshInstanceData	= Redirect_RefreshInstanceData;
		storageFunctions.ReloadRTSConfig		= Redirect_ReloadRTSConfig;
		storageFunctions.Reinit					= Redirect_ReInit;
		storageFunctions.ProcessPacket			= Redirect_ProcessPacket;
		storageFunctions.FormatMessage			= Redirect_FormatMessage;
		storageFunctions.ChangeUser				= Redirect_ChangeUser;

		// SFILETABLE implementation -- SNODE IO table
		ioFunctions.open						= Redirect_StorageNode_Open;
		ioFunctions.close						= Redirect_StorageNode_Close;
		ioFunctions.read						= Redirect_StorageNode_Read;
		ioFunctions.write						= Redirect_StorageNode_Write;
		ioFunctions.lseek						= Redirect_StorageNode_LSeek;
		ioFunctions.access						= Redirect_StorageNode_Access;
		ioFunctions.GetState					= Redirect_StorageNode_GetState;
		ioFunctions.SetState					= Redirect_StorageNode_SetState;
		ioFunctions.GetExtendedData				= Redirect_StorageNode_GetExtendedData;
		ioFunctions.SetExtendedData				= Redirect_StorageNode_SetExtendedData;
		ioFunctions.GetFullKey					= Redirect_StorageNode_GetFullKey;

		// NODEFUNCTIONS implementation -- Specialized SNODE functions.
		snodeFunctions.ReleaseSNode				= Redirect_NodeFunction_Release;
		snodeFunctions.NodeHasViruses			= Redirect_NodeFunction_HasViruses;
		snodeFunctions.RenameNode				= Redirect_NodeFunction_Rename;
		snodeFunctions.RemoveNode				= Redirect_NodeFunction_Remove;

		// STORAGESTATUS - unused.
		currentStatus.Flags						= 0;

		// STORAGEDATA - high-level storage configuration
		storageData.ZipDepth      = 3;
		storageData.FileType      = 0;
		storageData.Trap          = 0;
		storageData.CheckSum      = 0;
		storageData.Types         = 6;
		storageData.Exts[0]       = '\0';
		storageData.ZipExts[0]    = '\0';
		storageData.TrapExts[0]   = '\0';
		storageData.ZipFile       = 1;
		storageData.Softmice      = 0;
	}

	// Perform class-specific initialization for the derived class
	if (returnValDW == ERROR_SUCCESS)
		returnValDW = Initialize();

	// Shutdown and return
	if (storageKeyHandle != NULL)
	{
		storageManagerFunctions->Close(storageKeyHandle);
		storageKeyHandle = NULL;
	}
	if (returnValDW == ERROR_SUCCESS)
	{
		initialized = true;
		*rtvscanStorageInfo = &storageInfo;
		*storageInstance = DefaultStorageHandle;
	}
	else
	{
		// Cleaning up after a failure.  
		CoreDeInitialize(DefaultStorageHandle);
	}
	return returnValDW;
}

DWORD CRTVScanStorage::CoreDeInitialize( HANDLE storageHandle )
{
	DWORD returnValDW = ERROR_GENERAL;

	// Validate state
	if (!initialized)
		return ERROR_FUNCTION_NOT_SUPPORTED;
	if (storageHandle != DefaultStorageHandle)
		return ERROR_BAD_PARAM;

	// Call user's deinitialization function
	returnValDW = DeInitialize();

	// Release resources
	if (storageInfo.hRTSConfigKey != NULL)
	{
		storageManagerFunctions->Close(storageInfo.hRTSConfigKey);
		storageInfo.hRTSConfigKey = NULL;
	}
	storageManagerFunctions = NULL;
	if (exitEventHandle != NULL)
	{
		// Nothing should be waiting on this, but in case something is, give it a proper exit signal before closing the handle
		SetEvent(exitEventHandle);
		CloseHandle(exitEventHandle);
		exitEventHandle = NULL;
	}

	// Clear all data structures
	ZeroMemory(&storageInfo, sizeof(storageInfo));
	ZeroMemory(&instanceDefinition, sizeof(instanceDefinition));
	ZeroMemory(&currentStatus, sizeof(currentStatus));
	ZeroMemory(&storageData, sizeof(storageData));
	ZeroMemory(&storageFunctions, sizeof(storageFunctions));
	ZeroMemory(&snodeFunctions, sizeof(snodeFunctions));
	ZeroMemory(&ioFunctions, sizeof(ioFunctions));
	ZeroMemory(&currentUserName, sizeof(currentUserName));

	CoUninitialize();

	return returnValDW;
}


DWORD CRTVScanStorage::FindFirstNode( HANDLE parentHandle, DWORD instanceID, const char *pszPath, PQNODE pQNode, PSNODE pSNode, HANDLE *findHandle )
{
	return ERROR_FUNCTION_NOT_SUPPORTED;
}

DWORD CRTVScanStorage::FindNextNode( HANDLE findHandle, PQNODE pQNode, PSNODE pSNode )
{
	return ERROR_FUNCTION_NOT_SUPPORTED;
}

DWORD CRTVScanStorage::FindClose( HANDLE findHandle )
{
	return ERROR_FUNCTION_NOT_SUPPORTED;
}

DWORD CRTVScanStorage::CreateSNode( DWORD instanceID, const char *pszPath, PSNODE pSNode )
{
	return ERROR_FUNCTION_NOT_SUPPORTED;
}

DWORD CRTVScanStorage::BeginRTSWatch( PROCESSRTSNODE ProcessRTSNode, PSNODE pSNode, void *pStorageManagerContext )
{
	return ERROR_FUNCTION_NOT_SUPPORTED;
}

DWORD CRTVScanStorage::StopRTSWatches( )
{
	return ERROR_FUNCTION_NOT_SUPPORTED;
}

DWORD CRTVScanStorage::RefreshInstanceData( )
{
	return ERROR_FUNCTION_NOT_SUPPORTED;
}

DWORD CRTVScanStorage::ReloadRTSConfig( )
{
	return ERROR_FUNCTION_NOT_SUPPORTED;
}

DWORD CRTVScanStorage::ReInit( DWORD flags )
{
	return ERROR_FUNCTION_NOT_SUPPORTED;
}

DWORD CRTVScanStorage::ProcessPacket( WORD functionCode, BYTE *sendBuffer, DWORD sendBufferSize, BYTE *replyBuffer, DWORD *replyBufferSize, CBA_Addr *address )
{
	return ERROR_FUNCTION_NOT_SUPPORTED;
}

DWORD CRTVScanStorage::FormatMessage( char *resultBuffer, size_t nNumResultBufBytes, const char *messageFormat, PEVENTBLOCK thisEvent, DWORD userParam )
{
	return ERROR_FUNCTION_NOT_SUPPORTED;
}

DWORD CRTVScanStorage::FormatMessageHelper( LPCTSTR format, ReplacementInfoList replacementList, LPTSTR outputBuffer, size_t outputBufferBytes )
{
	ReplacementInfoList::iterator	currReplacement;
	TCHAR							currTokenString[10]		= {0};
	LPTSTR							workBuffer				= NULL;

	// Initialize
	workBuffer = new(std::nothrow) TCHAR[outputBufferBytes / sizeof (TCHAR)];
	if (workBuffer == NULL)
		return ERROR_MEMORY;

    ssStrnCpy(outputBuffer, format, outputBufferBytes);
	for (currReplacement = replacementList.begin(); currReplacement != replacementList.end(); currReplacement++)
	{
		// Replace all instances of ~(currReplacement->tokenCharacter) with currReplacement->replacementString
		vpsnprintf(currTokenString, sizeof (currTokenString), "~%c", currReplacement->tokenCharacter);
		StringReplace(workBuffer, outputBufferBytes/sizeof (TCHAR), outputBuffer, currTokenString, currReplacement->replacementString.c_str());
        ssStrnCpy(outputBuffer, workBuffer, outputBufferBytes);
	}
    ssStrnCpy(outputBuffer, workBuffer, outputBufferBytes);

	// Shutdown and return
	if (workBuffer != NULL)
	{
		delete [] workBuffer;
		workBuffer = NULL;
	}
	return ERROR_SUCCESS;
}


DWORD CRTVScanStorage::ChangeUser( char *pszUser, HANDLE hAccessToken )
{
    ssStrnCpy(currentUserName, pszUser, sizeof(currentUserName));
	return ERROR_SUCCESS;
}

// Storage Node IO Functions (SFILETABLE implementations)
DWORD CRTVScanStorage::StorageNode_Open( PSNODE thisNode, DWORD accessMode )
{
	DWORD	openMode		= 0;
	DWORD	shareMode		= 0;
	DWORD	returnValDW		= ERROR_GENERAL;

	returnValDW = FileModeToSOpenMode(accessMode, &openMode, &shareMode);
	if (returnValDW == ERROR_SUCCESS)
		returnValDW = _sopen(thisNode->InternalPath, openMode, shareMode);

	return returnValDW;
}

DWORD CRTVScanStorage::StorageNode_Close( DWORD fileHandle )
{
	return _close(fileHandle);
}

DWORD CRTVScanStorage::StorageNode_Read( DWORD fileHandle, void *dataBuffer, DWORD dataBufferSize )
{
	return _read(fileHandle, dataBuffer, dataBufferSize);
}

DWORD CRTVScanStorage::StorageNode_Write( DWORD fileHandle, void *dataBuffer, DWORD dataBufferSize )
{
	return _write(fileHandle, dataBuffer, dataBufferSize);
}

DWORD CRTVScanStorage::StorageNode_LSeek( DWORD fileHandle, long offset, DWORD origin )
{
	return _lseek(fileHandle, offset, origin);
}

DWORD CRTVScanStorage::StorageNode_Access( PSNODE thisNode, int accessCode )
{
	return _access(thisNode->InternalPath, accessCode);
}

DWORD CRTVScanStorage::StorageNode_GetState( PSNODE thisNode, BYTE *stateData )
{
	return ERROR_FUNCTION_NOT_SUPPORTED;
}

DWORD CRTVScanStorage::StorageNode_SetState( PSNODE thisNode, BYTE *stateData )
{
	return ERROR_FUNCTION_NOT_SUPPORTED;
}

DWORD CRTVScanStorage::StorageNode_GetExtendedData( PSNODE thisNode, char *keyName, BYTE *dataBuffer, DWORD dataBufferSize )
{
	return ERROR_FUNCTION_NOT_SUPPORTED;
}

DWORD CRTVScanStorage::StorageNode_SetExtendedData( PSNODE thisNode, char *keyName, BYTE *dataBuffer, DWORD dataBufferSize )
{
	return ERROR_FUNCTION_NOT_SUPPORTED;
}

DWORD CRTVScanStorage::StorageNode_GetFullKey( PSNODE thisNode, char* keyPathBuffer, DWORD keyPathBufferSize )
{
	return ERROR_FUNCTION_NOT_SUPPORTED;
}

// Storage Node Functions (NODEFUNCTIONS implementations)
DWORD CRTVScanStorage::NodeFunction_Release( PSNODE pNode )
{
	return ERROR_FUNCTION_NOT_SUPPORTED;
}

DWORD CRTVScanStorage::NodeFunction_HasViruses( PSNODE pNode, PEVENTBLOCK pEventBlock )
{
	return ERROR_FUNCTION_NOT_SUPPORTED;
}

DWORD CRTVScanStorage::NodeFunction_Rename( PSNODE pNode, char * newExt )
{
	return ERROR_FUNCTION_NOT_SUPPORTED;
}

DWORD CRTVScanStorage::NodeFunction_Remove( PSNODE pNode )
{
	return ERROR_FUNCTION_NOT_SUPPORTED;
}

// ** Redirectors **
DWORD CRTVScanStorage::Redirect_FindFirstNode( HANDLE parentHandle, DWORD instanceID, const char *pszPath, PQNODE pQNode, PSNODE pSNode, HANDLE *findHandle )
{
	if (storageObject != NULL)
		return storageObject->FindFirstNode(parentHandle, instanceID, pszPath, pQNode, pSNode, findHandle);
	else
		return ERROR_FUNCTION_NOT_SUPPORTED;
}

DWORD CRTVScanStorage::Redirect_FindNextNode( HANDLE findHandle, PQNODE pQNode, PSNODE pSNode )
{
	if (storageObject != NULL)
		return storageObject->FindNextNode(findHandle, pQNode, pSNode);
	else
		return ERROR_FUNCTION_NOT_SUPPORTED;
}

DWORD CRTVScanStorage::Redirect_FindClose( HANDLE findHandle )
{
	if (storageObject != NULL)
		return storageObject->FindClose(findHandle);
	else
		return ERROR_FUNCTION_NOT_SUPPORTED;
}

DWORD CRTVScanStorage::Redirect_CreateSNode( DWORD instanceID, const char *pszPath, PSNODE pSNode )
{
	if (storageObject != NULL)
		return storageObject->CreateSNode(instanceID, pszPath, pSNode);
	else
		return ERROR_FUNCTION_NOT_SUPPORTED;
}

DWORD CRTVScanStorage::Redirect_BeginRTSWatch( PROCESSRTSNODE ProcessRTSNode, PSNODE pSNode, void *context )
{
	if (storageObject != NULL)
		return storageObject->BeginRTSWatch(ProcessRTSNode, pSNode, context);
	else
		return ERROR_FUNCTION_NOT_SUPPORTED;
}

DWORD CRTVScanStorage::Redirect_StopRTSWatches()
{
	if (storageObject != NULL)
		return storageObject->StopRTSWatches();
	else
		return ERROR_FUNCTION_NOT_SUPPORTED;
}

DWORD CRTVScanStorage::Redirect_RefreshInstanceData()
{
	if (storageObject != NULL)
		return storageObject->RefreshInstanceData();
	else
		return ERROR_FUNCTION_NOT_SUPPORTED;
}

DWORD CRTVScanStorage::Redirect_ReloadRTSConfig()
{
	if (storageObject != NULL)
		return storageObject->ReloadRTSConfig();
	else
		return ERROR_FUNCTION_NOT_SUPPORTED;
}

DWORD CRTVScanStorage::Redirect_ReInit( DWORD flags )
{
	if (storageObject != NULL)
		return storageObject->ReInit(flags);
	else
		return ERROR_FUNCTION_NOT_SUPPORTED;
}

DWORD CRTVScanStorage::Redirect_ProcessPacket( WORD functionCode, BYTE *sendBuffer, DWORD sendBufferSize, BYTE *replyBuffer, DWORD *replyBufferSize, CBA_Addr *address )
{
	if (storageObject != NULL)
		return storageObject->ProcessPacket(functionCode, sendBuffer, sendBufferSize, replyBuffer, replyBufferSize, address);
	else
		return ERROR_FUNCTION_NOT_SUPPORTED;
}

DWORD CRTVScanStorage::Redirect_FormatMessage( char *resultBuffer, size_t nNumResultBufBytes, const char *messageFormat, PEVENTBLOCK thisEvent, DWORD userParam )
{
	if (storageObject != NULL)
		return storageObject->FormatMessage(resultBuffer, nNumResultBufBytes, messageFormat, thisEvent, userParam);
	else
		return ERROR_FUNCTION_NOT_SUPPORTED;
}

DWORD CRTVScanStorage::Redirect_ChangeUser( char *pszUser, HANDLE hAccessToken )
{
	if (storageObject != NULL)
		return storageObject->ChangeUser(pszUser, hAccessToken);
	else
		return ERROR_FUNCTION_NOT_SUPPORTED;
}

DWORD CRTVScanStorage::Redirect_StorageNode_Open( PSNODE pNode, DWORD dwAccess )
{
	if (storageObject != NULL)
		return storageObject->StorageNode_Open(pNode, dwAccess);
	else
		return ERROR_FUNCTION_NOT_SUPPORTED;
}

DWORD CRTVScanStorage::Redirect_StorageNode_Close( DWORD hFile )
{
	if (storageObject != NULL)
		return storageObject->StorageNode_Close(hFile);
	else
		return ERROR_FUNCTION_NOT_SUPPORTED;
}

DWORD CRTVScanStorage::Redirect_StorageNode_Read( DWORD fileHandle, void *dataBuffer, DWORD dataBufferSize )
{
	if (storageObject != NULL)
		return storageObject->StorageNode_Read(fileHandle, dataBuffer, dataBufferSize);
	else
		return ERROR_FUNCTION_NOT_SUPPORTED;
}

DWORD CRTVScanStorage::Redirect_StorageNode_Write( DWORD fileHandle, void *dataBuffer, DWORD dataBufferSize )
{
	if (storageObject != NULL)
		return storageObject->StorageNode_Write(fileHandle, dataBuffer, dataBufferSize);
	else
		return ERROR_FUNCTION_NOT_SUPPORTED;
}

DWORD CRTVScanStorage::Redirect_StorageNode_LSeek( DWORD fileHandle, long offset, DWORD origin )
{
	if (storageObject != NULL)
		return storageObject->StorageNode_LSeek(fileHandle, offset, origin);
	else
		return ERROR_FUNCTION_NOT_SUPPORTED;
}

DWORD CRTVScanStorage::Redirect_StorageNode_Access( PSNODE thisNode, int accessCode )
{
	if (storageObject != NULL)
		return storageObject->StorageNode_Access(thisNode, accessCode);
	else
		return ERROR_FUNCTION_NOT_SUPPORTED;
}

DWORD CRTVScanStorage::Redirect_StorageNode_GetState( PSNODE thisNode, BYTE *stateData )
{
	if (storageObject != NULL)
		return storageObject->StorageNode_GetState(thisNode, stateData);
	else
		return ERROR_FUNCTION_NOT_SUPPORTED;
}

DWORD CRTVScanStorage::Redirect_StorageNode_SetState( PSNODE thisNode, BYTE *stateData )
{
	if (storageObject != NULL)
		return storageObject->StorageNode_SetState(thisNode, stateData);
	else
		return ERROR_FUNCTION_NOT_SUPPORTED;
}

DWORD CRTVScanStorage::Redirect_StorageNode_GetExtendedData( PSNODE thisNode, char *keyName, BYTE *dataBuffer, DWORD dataBufferSize )
{
	if (storageObject != NULL)
		return storageObject->StorageNode_GetExtendedData(thisNode, keyName, dataBuffer, dataBufferSize);
	else
		return ERROR_FUNCTION_NOT_SUPPORTED;
}

DWORD CRTVScanStorage::Redirect_StorageNode_SetExtendedData( PSNODE thisNode, char *keyName, BYTE *dataBuffer, DWORD dataBufferSize )
{
	if (storageObject != NULL)
		return storageObject->StorageNode_SetExtendedData(thisNode, keyName, dataBuffer, dataBufferSize);
	else
		return ERROR_FUNCTION_NOT_SUPPORTED;
}

DWORD CRTVScanStorage::Redirect_StorageNode_GetFullKey( PSNODE thisNode, char* keyPathBuffer, DWORD keyPathBufferSize )
{
	if (storageObject != NULL)
		return storageObject->StorageNode_GetFullKey(thisNode, keyPathBuffer, keyPathBufferSize);
	else
		return ERROR_FUNCTION_NOT_SUPPORTED;
}

DWORD CRTVScanStorage::Redirect_NodeFunction_Release( PSNODE pNode )
{
	if (storageObject != NULL)
		return storageObject->NodeFunction_Release(pNode);
	else
		return ERROR_FUNCTION_NOT_SUPPORTED;
}

DWORD CRTVScanStorage::Redirect_NodeFunction_HasViruses( PSNODE pNode, PEVENTBLOCK pEventBlock )
{
	if (storageObject != NULL)
		return storageObject->NodeFunction_HasViruses(pNode, pEventBlock);
	else
		return ERROR_FUNCTION_NOT_SUPPORTED;
}

DWORD CRTVScanStorage::Redirect_NodeFunction_Rename( PSNODE pNode, char* newExt )
{
	if (storageObject != NULL)
		return storageObject->NodeFunction_Rename(pNode, newExt);
	else
		return ERROR_FUNCTION_NOT_SUPPORTED;
}

DWORD CRTVScanStorage::Redirect_NodeFunction_Remove( PSNODE pNode )
{
	if (storageObject != NULL)
		return storageObject->NodeFunction_Remove(pNode);
	else
		return ERROR_FUNCTION_NOT_SUPPORTED;
}

DWORD CRTVScanStorage::Redirect_CoreDeInitialize( HANDLE storageHandle )
{
	DWORD resultValDW = ERROR_OUT_OF_PAPER;
	if (storageObject != NULL)
    {
		resultValDW = storageObject->CoreDeInitialize(storageHandle);
    
	    // Clean up allocation from StorageInit template function
	    delete storageObject;
	    storageObject = NULL;

        return resultValDW;
    }
	else
    {
		return ERROR_FUNCTION_NOT_SUPPORTED;
    }
}

// Constructor-destructor
CRTVScanStorage::CRTVScanStorage()
{
	// Place all initialization in Initialize, not here
	storageManagerFunctions = NULL;
	exitEventHandle = NULL;
	ZeroMemory(&storageInfo, sizeof(storageInfo));
	ZeroMemory(&instanceDefinition, sizeof(instanceDefinition));
	ZeroMemory(&currentStatus, sizeof(currentStatus));
	ZeroMemory(&storageData, sizeof(storageData));
	ZeroMemory(&storageFunctions, sizeof(storageFunctions));
	ZeroMemory(&snodeFunctions, sizeof(snodeFunctions));
	ZeroMemory(&ioFunctions, sizeof(ioFunctions));
}

CRTVScanStorage::~CRTVScanStorage()
{
	// Nothing should go here.  Destructor-type code should go in DeInitialize();
}

DWORD CRTVScanStorage::GetFakeSID( mSID* thisFakeSID )
// Gets a fake SID for the current user
{
    mSID        newSID							= {0};
    LPTSTR      accountStringData				= NULL;
    DWORD		accountStringDataBufferSize		= SID_SIZE * (sizeof(DWORD)/sizeof(TCHAR));
    const TCHAR separator[]						= _T("\x01");

	// Validate parameters
	if (thisFakeSID == NULL)
		return ERROR_BAD_PARAM;

	// Initialize
    newSID.stuff[0] = 0xFFFFFFFF;
    accountStringDataBufferSize -= sizeof(DWORD)/sizeof(TCHAR);
    accountStringData = reinterpret_cast<LPTSTR>(&newSID.stuff[1]);

	// Fake SID Format:  <NULL><username><separator><computername>
    *accountStringData = _T('\0');
    _tcsncat(accountStringData, currentUserName, accountStringDataBufferSize - _tcslen(separator) - 1);
    _tcscat(accountStringData, separator);
    accountStringDataBufferSize -= (DWORD) _tcslen(accountStringData);
    GetComputerName(accountStringData + _tcslen(accountStringData), &accountStringDataBufferSize);

	// Return result
	*thisFakeSID = newSID;
	return ERROR_SUCCESS;
}

int CRTVScanStorage::StringReplace( std::string* subjectString, const std::string& findString, const std::string& replaceString )
//Searches for findString in subjectString and replaces all occurances with replaceString
{
	int returnVal = -1;
	std::string resultString;

	//Validate parameters
	if (subjectString == NULL)
		return -1;

    try
    {
        //Do the search/replace
        returnVal = StringReplace(*subjectString, findString, replaceString, &resultString);
        if (returnVal > 0)
            *subjectString = resultString;
    }
    catch (std::bad_alloc&)
    {
        returnVal = -1;
    }
	return returnVal;
}

int CRTVScanStorage::StringReplace( const std::string& sourceString, const std::string& findString, const std::string& replaceString, std::string* resultString )
//Identical to StringReplace, but takes C++ strings for args
{
	LPTSTR resultBuffer = NULL;
	DWORD resultBufferSize = 0;
	DWORD maxResultBufferSize = 32768;		//Max size we'll support
	int returnVal = -1;

	//Validate parameters
	if (resultString == NULL)
		return -1;

    try
    {
        //Only failure condition for StringReplace is insufficient buffer size
        //Call StringReplace.  If fails, repeat with larger buffer sizes, up to maxResultBufferSize
        for( resultBufferSize = 2048; (resultBufferSize <= maxResultBufferSize) && (returnVal == -1); resultBufferSize = resultBufferSize*2)
        {
            if (resultBuffer != NULL)
            {
                delete resultBuffer;
                resultBuffer = NULL;
            }
            resultBuffer = new(std::nothrow) TCHAR[resultBufferSize];
            if (resultBuffer != NULL)
            {
                resultBuffer[0] = NULL;
                returnVal = StringReplace(resultBuffer, resultBufferSize, sourceString.c_str(), findString.c_str(), replaceString.c_str());
            }
            else
            {
                break;
            }
        }

        // Store final string
        if (resultBuffer != NULL)
            *resultString = resultBuffer;
    }
    catch (std::bad_alloc&)
    {
        returnVal = -1;
    }

    // Cleanup and return result
    if (resultBuffer != NULL)
    {
        delete resultBuffer;
        resultBuffer = NULL;
    }
	return returnVal;
}

int CRTVScanStorage::StringReplace( LPTSTR lpszResult, size_t cchResult, LPCTSTR lpszStringIn, LPCTSTR lpszFind, LPCTSTR lpszReplaceWith )
//Replace all occurances of lpszFind in lpszStringIn with lpszReplace
//Returns the number of replacements made, or -1 if buffer was too small
//Ripped from Phoenix (NSW2) install
{
   // Reset result
   int nResult = -1;
   if (lpszResult && cchResult && lpszStringIn && lpszFind && lpszReplaceWith)
   {
	  LPTSTR lpszBuffer = (LPTSTR) new(std::nothrow) TCHAR[cchResult];
      LPCTSTR lpszCFS = lpszStringIn;
      LPCTSTR lpszLFS = lpszStringIn;
      LPCTSTR lpszCCS = lpszFind;
      LPTSTR lpszDest = lpszBuffer;
      int    cchSS = 0;

      // reset to 0
      nResult = 0;

      // Determine character count of text to find
      while (*lpszCCS)
      {
         cchSS++;
         lpszCCS = CharNext( lpszCCS );
      }

      if (lpszBuffer)
      {
         ZeroMemory(lpszBuffer, cchResult * sizeof (TCHAR));
         while (*lpszLFS)
         {
            // Start compares at last starting position
            lpszCFS = lpszLFS;

            // Reset find index ptr
            lpszCCS = lpszFind;

            // Compare the string we're looking for with the buffer
            if ((*lpszCFS == *lpszCCS) && 0 == StrCmp( lpszCFS, lpszCCS, cchSS, TRUE ))
            {
               // Increment our return value
               nResult++;

               // Concatenate the new string
               lstrcpy( lpszDest, lpszReplaceWith );

               // Update the dest ptr
               while (*lpszDest)
                  lpszDest = CharNext( lpszDest );

               // Move search ptr to end of the old string in the buffer
               int i = 0;
               while (i < cchSS)
               {
                  lpszLFS = CharNext( lpszLFS );
                  i++;
               }
            }
            else
            {  // The substring was not found starting at lpszLFS,
               // so copy it to the output buffer, move it forward
               // one character and try again.
               *lpszDest = *lpszLFS;               // Copy character

               // check for possible DBCS character
               #if defined(_MBCS) && !defined(_UNICODE)
               if (IsDBCSLeadByte(*lpszLFS))
                  *(lpszDest+1) = *(lpszLFS+1);
               #endif

               lpszDest = CharNext( lpszDest );    // Move dest forward
               lpszLFS = CharNext( lpszLFS );      // Move src forward
            }
         }
         // If the resulting string fits in the output buffer, save it,
         // otherwise, set the result to an error
         if ((size_t)lstrlen( lpszBuffer ) < cchResult)
            lstrcpy( lpszResult, lpszBuffer );
         else
         {
            TCHAR szMsg[255];
            vpsnprintf(szMsg, sizeof (szMsg), "StringReplace character size %d\nActual Buffer Length %d",cchResult,lstrlen(lpszBuffer));
            nResult = -1;
         };

         // Free our working buffer
		 delete lpszBuffer;
      }
   }

   return (nResult);
}

int CRTVScanStorage::StrCmp( LPCTSTR string1, LPCTSTR string2, int count, BOOL fIgnoreCase )
//DBCS-compatible string compare function.  count is # of chars to compare, not including terminator
//Returns 0 if strings match
//Ripped from Phoenix (NSW2) install
{
   int   nRtn = 0;
   static BOOL fDBCS = GetSystemMetrics( SM_DBCSENABLED );

   if (fDBCS)
   {
      LPCTSTR  lpstr1, lpstr2;

      lpstr1 = string1;
      lpstr2 = string2;

      if (!lpstr1 && !lpstr2)
         nRtn = 0;
      else if (!lpstr1)
         nRtn = -1;
      else if (!lpstr2)
         nRtn = 1;
      else
      {
         if (!*lpstr1 && !*lpstr2)
            nRtn = 0;
         else if (!*lpstr1)
            nRtn = -1;
         else if (!*lpstr2)
            nRtn = 1;
         else
         {
            while (*lpstr1 && *lpstr2 && (count == -1 || count > 0) && nRtn == 0)
            {
               int   nRes;
               BOOL  fDBCS1;

               nRes = CompareString( LOCALE_USER_DEFAULT,
                                     fIgnoreCase?NORM_IGNORECASE:0,
                                     lpstr1, (fDBCS1 = IsDBCSLeadByte(*lpstr1))?2:1,
                                     lpstr2, IsDBCSLeadByte(*lpstr2)?2:1 );

               if (nRes == 2 && fDBCS1 && fIgnoreCase)
               {
                  unsigned short nch1, nch2;

                  nch1 = *((unsigned short *)lpstr1);
                  nch2 = *((unsigned short *)lpstr2);
                  nRtn = (nch1 - nch2);
               }
               else
               {
                  if (nRes == 1)
                     nRtn = -1;
                  else if (nRes == 3)
                     nRtn = 1;
               }

               if (count != -1)
               {
                  if (fDBCS1 && (count > 0))
                     count--;
                  if (count > 0)
                     count--;
               }

               lpstr1 = CharNext(lpstr1);
               lpstr2 = CharNext(lpstr2);
            }

            // Make sure unequal length strings are not returned as equal
            if ( (nRtn == 0) && (count == -1) )
            {
               if (!*lpstr1 && *lpstr2)
                  nRtn = -1;
               else if (*lpstr1 && !*lpstr2)
                  nRtn = 1;
            }
         }
      }
   }
   else
   {
      if (fIgnoreCase)
      {
         if (count == -1)
            nRtn = lstrcmpi( string1, string2 );
         else
            nRtn = -2 + CompareString( LOCALE_USER_DEFAULT, NORM_IGNORECASE, string1, count, string2, count );
//            nRtn = _strnicmp( string1, string2, count );
      }
      else
      {
         if (count == -1)
            nRtn = _mbscmp( (const unsigned char*) string1, (const unsigned char*) string2 );
         else
            nRtn = -2 + CompareString( LOCALE_USER_DEFAULT, 0, string1, count, string2, count );
//            nRtn = strncmp( string1, string2, count );
      }
   }

   return nRtn;
}

DWORD CRTVScanStorage::FileModeToSOpenMode( DWORD ftoMode, DWORD* openMode, DWORD* shareMode )
// Translate an RTVScan FTO file mode to a pair of sopen mode constants
{
	// Validate parameters
	if (openMode == NULL)
		return ERROR_BAD_PARAM;
	if (shareMode == NULL)
		return ERROR_BAD_PARAM;

    switch (ftoMode & (FTO_RDWR | FTO_RDONLY | FTO_WRONLY))
    {
    default:
    case FTO_RDONLY:
		*openMode |= _O_RDONLY;
		break;
    case FTO_WRONLY:
		*openMode |= _O_WRONLY;
		break;
    case FTO_RDWR:
		*openMode |= _O_RDWR;
		break;
    }

    if (ftoMode & FTO_APPEND)
		*openMode |= _O_APPEND;
    if (ftoMode & FTO_TRUNC)
		*openMode |= _O_TRUNC;
    if (ftoMode & FTO_BINARY)
		*openMode |= _O_BINARY;
    if (ftoMode & FTO_TEXT)
		*openMode |= _O_TEXT;

    *shareMode |= _SH_DENYNO;

	return ERROR_SUCCESS;
}

CRTVScanStorage::StorageConfig::StorageConfig() : type(0), notifyEventA(0), notifyEventB(0), notifyEventC(0)
{
	// Nothing for now
}

CRTVScanStorage::StorageConfig::~StorageConfig()
{
	// Nothing for now
}

CRTVScanStorage::ReplacementInfo::ReplacementInfo() : tokenCharacter(' ')
{
	// Nothing for now
}

CRTVScanStorage::ReplacementInfo::ReplacementInfo( char newTokenCharacter, std::string newReplacementString ) : tokenCharacter(newTokenCharacter), replacementString(newReplacementString)
{
    // Nothing for now
}

CRTVScanStorage::ReplacementInfo::~ReplacementInfo()
{
	// Nothing for now
}
