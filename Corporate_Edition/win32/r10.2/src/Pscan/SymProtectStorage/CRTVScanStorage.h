// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#ifndef RTVSCAN_STORAGE_BASE
#define RTVSCAN_STORAGE_BASE

#include "vpcommon.h"
#include "clientreg.h"
#include "veapi.h"
#include <list>
#include <string>
#include <shlwapi.h>
// Include SHLWAPI.LIB in default libraries for developer convenience
#pragma comment(lib, "shlwapi.lib")

// A helper class used to make implementing RTVScan storages easy.  To use it:
//		1.  Create a Win32 DLL project and include this header + it's CPP file in it.  You can implement only *one* storage per DLL.
//		2.  Create a new class inheriting from CRTVScanStorage.
//		3.  In the CPP for your storage, instantiate the Init function template for your storage class, as in:
//			Storage initialization function
//			extern "C"
//			{
//				template DWORD _declspec(dllexport) StorageInit<CSymProtectStorage>( DWORD flags, PSTORAGEINFO* storageInfo, HANDLE* storageInstance, PSSFUNCTIONS storageManagerFunctions );
//			}
//		4.  Implement all of the following members:
//				PreInitializeGetConfig
//				Initialize
//				DeInitialize
//		5.  Implement just the members you need.  All others will return ERROR_FUNCTION_NOT_SUPPORTED.
//		6.  Build the DLL and create the registry entries so RTVScan will register it at runtime.
//		7.  Update RTVScan's VerifyValidStorageType function to recognize the name of your new storage as a valid one.
//		8.  (Optional) For install/uninstall support, implement a DllRegisterServer/DllUnregisterServer function pair, export them via a DEF file, and use the template functions provided
//			 to implement them, as in
//				STDAPI DllRegisterServer()
//				{
//					return DllRegisterServerDo<CSymProtectStorage>();
//				}
//				STDAPI DllUnregisterServer()
//				{
//					return DllUnregisterServerDo<CSymProtectStorage>();
//				}
// NOTES:
// 1.  There will only be a single instance of this object in RTVScan, as this is how RTVScan is designed.
// 2.  DllRegisterServerDo/DllUnregisterServerDo instantiate instances of your class to call the GetConfig member.
// 3.  Exporting DllRegisterServer/DllUnregisterServer via a DEF file is necessary because using __declspec(dllexport) with __stdcall functons will result in mangled names.
class CRTVScanStorage
{
public:
	// ** DATA TYPES **
	// Gets storage configuration information from the inheritor class
	struct StorageConfig
	{
		std::string		storageName;		// Name of the storage (Snap-In subkey and Storages subkey)
		std::string		fileNodeName;		// Node name and extension of the storage DLL
		std::string		dllPath;			// Path to the storage DLL, with a trailing backslash
		std::string		entrypoint;			// Name of the storage's Initialize export
		DWORD			type;				// Storage's Type code
		std::string		notifyTitle;		// If specified, overrides the title of the notify dialog
		DWORD			notifyEventA;		// If != 0, the ID of notify event A
		DWORD			notifyEventB;		// If != 0, the ID of notify event B
		DWORD			notifyEventC;		// If != 0, the ID of notify event C

		StorageConfig();
		~StorageConfig();
	};

	// Storage Extension Functions (SFUNCTIONS implementations)
	// Called to get configuration options, for registration and during PreInitialize before Initialize is called.
	virtual DWORD GetConfig( StorageConfig* config ) = 0;
	// Called by CRTVScanStorage::CoreInitialize during storage initialization for your own initialization.  When called, all basic initialization, such as
	// key/event initialization, setting storageManagerFunctions - is already done.
	// Return ERROR_SUCCESS on succes
    virtual DWORD Initialize( void ) = 0;
	// Called by CRTVScanStorage::CoreDeInitialize during storage shutdown for your own shutdown processing.
	// Return ERROR_SUCCESS on success
	virtual DWORD DeInitialize( void ) = 0;

	// SFunctions (Storage Functions)
	virtual DWORD FindFirstNode( HANDLE parentHandle, DWORD instanceID, const char *pszPath, PQNODE pQNode, PSNODE pSNode, HANDLE *findHandle );
    virtual DWORD FindNextNode( HANDLE findHandle, PQNODE pQNode, PSNODE pSNode );
    virtual DWORD FindClose( HANDLE findHandle );
    virtual DWORD CreateSNode( DWORD instanceID, const char *pszPath, PSNODE pSNode );
    virtual DWORD BeginRTSWatch( PROCESSRTSNODE ProcessRTSNode, PSNODE pSNode, void *context );
    virtual DWORD StopRTSWatches( );
    virtual DWORD RefreshInstanceData( );
    virtual DWORD ReloadRTSConfig( );
    virtual DWORD ReInit( DWORD flags );
    virtual DWORD ProcessPacket( WORD functionCode, BYTE *sendBuffer, DWORD sendBufferSize, BYTE *replyBuffer, DWORD *replyBufferSize, CBA_Addr *address );
    virtual DWORD FormatMessage( char *resultBuffer, size_t nNumResultBufBytes, const char *messageFormat, PEVENTBLOCK thisEvent, DWORD userParam );
    virtual DWORD ChangeUser(const char *pszUser, HANDLE hAccessToken );

    // Storage Node IO Functions (SFILETABLE implementations)
	// Opens the specified storage node
    virtual DWORD StorageNode_Open( PSNODE pNode, DWORD dwAccess );
	// Closes the specified storage node
    virtual DWORD StorageNode_Close( DWORD hFile );
	// Reads data from the specified storage node
    virtual DWORD StorageNode_Read( DWORD fileHandle, void *dataBuffer, DWORD dataBufferSize );
	// Writes data to the specified storage node
    virtual DWORD StorageNode_Write( DWORD fileHandle, void *dataBuffer, DWORD dataBufferSize );
	// Seeks the file pointer on the specified storage node
    virtual DWORD StorageNode_LSeek( DWORD fileHandle, long offset, DWORD origin );
	// Performs an access call on the specified storage node
    virtual DWORD StorageNode_Access( PSNODE thisNode, int accessCode );
	// Sets pData equal to file time and attribute information about a node for walk scans on the file system storage
    virtual DWORD StorageNode_GetState( PSNODE thisNode, BYTE *stateData );
	// Sets the file time and attribute information for a node, given previous GetState data
    virtual DWORD StorageNode_SetState( PSNODE thisNode, BYTE *stateData );
	// Purpose unknown
    virtual DWORD StorageNode_GetExtendedData( PSNODE thisNode, char *keyName, BYTE *dataBuffer, DWORD dataBufferSize );
	// Purpose unknown
    virtual DWORD StorageNode_SetExtendedData( PSNODE thisNode, char *keyName, BYTE *dataBuffer, DWORD dataBufferSize );
	// Sets *buffer equal to the full path excluding volume (instance) information for the given SNODE.
    virtual DWORD StorageNode_GetFullKey( PSNODE thisNode, char* keyPathBuffer, DWORD keyPathBufferSize );

    // Storage Node Functions (NODEFUNCTIONS implementations)
	// Frees this storage node
    virtual DWORD NodeFunction_Release( PSNODE pNode ) = 0;
	// Called when infections are found in a storage node passed to ProcessRTSNode().
    virtual DWORD NodeFunction_HasViruses( PSNODE pNode, PEVENTBLOCK pEventBlock ) = 0;
	// Change the extension of the file specified by the SNODE.
    virtual DWORD NodeFunction_Rename( PSNODE pNode, char* newExt ) = 0;
	// Delete the specified storage node
    virtual DWORD NodeFunction_Remove( PSNODE pNode ) = 0;

	// Constructor-destructor
    CRTVScanStorage();
    virtual ~CRTVScanStorage();

protected:
	// ** DATA TYPES **
	// Used by FormatMessageHelper to replace tokens in the FormatMessage string with strings
	struct ReplacementInfo
	{
		char tokenCharacter;
		std::string replacementString;

		ReplacementInfo();
		ReplacementInfo( char newTokenCharacter, std::string newReplacementString );
	};
	typedef std::list<ReplacementInfo> ReplacementInfoList;

	// ** FUNCTION MEMBERS **
	// Gets a fake SID for the current user.
	static DWORD GetFakeSID( mSID* thisFakeSID );
	// Sets *outputBuffer equal to an RTVScan formatted message using the supplied format and token replacements
	// Not sensitive to token character case
	static DWORD FormatMessageHelper( LPCTSTR format, ReplacementInfoList replacementList, LPTSTR outputBuffer, size_t outputBufferBytes );
	// Translate an RTVScan FTO file mode to a pair of sopen mode constants
	static DWORD FileModeToSOpenMode( DWORD ftoMode, DWORD* openMode, DWORD* shareMode );

	//Searches for findString in subjectString and replaces all occurances with replaceString
	static int StringReplace( std::string* subjectString, const std::string& findString, const std::string& replaceString );
	//Identical to StringReplace, but takes C++ strings for args
	static int StringReplace( const std::string& sourceString, const std::string& findString, const std::string& replaceString, std::string* resultString );
	//Replace all occurances of lpszFind in lpszStringIn with lpszReplace
	//Returns the number of replacements made, or -1 if buffer was too small
	//Ripped from Phoenix (NSW2) install
	static int StringReplace( LPTSTR lpszResult, size_t cchResult, LPCTSTR lpszStringIn, LPCTSTR lpszFind, LPCTSTR lpszReplaceWith );
	//DBCS-compatible string compare function.  count is # of chars to compare, not including terminator
	//Returns 0 if strings match
	//Ripped from Phoenix (NSW2) install
	static int StrCmp( LPCTSTR string1, LPCTSTR string2, int count, BOOL fIgnoreCase );

	// ** REDIRECTORS **
	// SFunctions (Storage Functions)
	static DWORD _cdecl Redirect_FindFirstNode( HANDLE parentHandle, DWORD instanceID, const char *pszPath, PQNODE pQNode, PSNODE pSNode, HANDLE *findHandle );
    static DWORD _cdecl Redirect_FindNextNode( HANDLE findHandle, PQNODE pQNode, PSNODE pSNode );
    static DWORD _cdecl Redirect_FindClose( HANDLE findHandle );
    static DWORD _cdecl Redirect_CreateSNode( DWORD instanceID, const char *pszPath, PSNODE pSNode );
    static DWORD _cdecl Redirect_BeginRTSWatch( PROCESSRTSNODE ProcessRTSNode, PSNODE pSNode, void *context );
    static DWORD _cdecl Redirect_StopRTSWatches( );
    static DWORD _cdecl Redirect_RefreshInstanceData( );
    static DWORD _cdecl Redirect_ReloadRTSConfig( );
    static DWORD _cdecl Redirect_ReInit( DWORD flags );
    static DWORD _cdecl Redirect_ProcessPacket( WORD functionCode, BYTE *sendBuffer, DWORD sendBufferSize, BYTE *replyBuffer, DWORD *replyBufferSize, CBA_Addr *address );
    static DWORD _cdecl Redirect_FormatMessage( char *resultBuffer, size_t nNumResultBufBytes, const char *messageFormat, PEVENTBLOCK thisEvent, DWORD userParam );
    static DWORD _cdecl Redirect_ChangeUser(const char *pszUser, HANDLE hAccessToken );
    // Storage Node IO Functions (SFILETABLE implementations)
    static DWORD _cdecl Redirect_StorageNode_Open( PSNODE pNode, DWORD dwAccess );
    static DWORD _cdecl Redirect_StorageNode_Close( DWORD hFile );
    static DWORD _cdecl Redirect_StorageNode_Read( DWORD fileHandle, void *dataBuffer, DWORD dataBufferSize );
    static DWORD _cdecl Redirect_StorageNode_Write( DWORD fileHandle, void *dataBuffer, DWORD dataBufferSize );
    static DWORD _cdecl Redirect_StorageNode_LSeek( DWORD fileHandle, long offset, DWORD origin );
    static DWORD _cdecl Redirect_StorageNode_Access( PSNODE thisNode, int accessCode );
    static DWORD _cdecl Redirect_StorageNode_GetState( PSNODE thisNode, BYTE *stateData );
    static DWORD _cdecl Redirect_StorageNode_SetState( PSNODE thisNode, BYTE *stateData );
    static DWORD _cdecl Redirect_StorageNode_GetExtendedData( PSNODE thisNode, char *keyName, BYTE *dataBuffer, DWORD dataBufferSize );
    static DWORD _cdecl Redirect_StorageNode_SetExtendedData( PSNODE thisNode, char *keyName, BYTE *dataBuffer, DWORD dataBufferSize );
    static DWORD _cdecl Redirect_StorageNode_GetFullKey( PSNODE thisNode, char* keyPathBuffer, DWORD keyPathBufferSize );
    // Storage Node Functions (NODEFUNCTIONS implementations)
    static DWORD _cdecl Redirect_NodeFunction_Release( PSNODE pNode );
    static DWORD _cdecl Redirect_NodeFunction_HasViruses( PSNODE pNode, PEVENTBLOCK pEventBlock );
    static DWORD _cdecl Redirect_NodeFunction_Rename( PSNODE pNode, char* newExt );
    static DWORD _cdecl Redirect_NodeFunction_Remove( PSNODE pNode );
	// Initialization-Deinitialization
	static DWORD _cdecl Redirect_CoreDeInitialize( HANDLE storageHandle );

	// ** CONSTANTS **
	static const HANDLE			DefaultStorageHandle;		// Handle of this storage for RTVScan.  There is only one.
															// Do NOT use 0 or INVALID_HANDLE_VALUE as these may fail validation checks.
	static const DWORD			DefaultInstanceID;			// If you do not support volumes, always use this for the instance ID

	// ** DATA MEMBERS **
	// Storage description data
	static STORAGEINFO			storageInfo;				// Describes this storage
    static IDEF					instanceDefinition;			// One per logical volume.
    static STORAGESTATUS		currentStatus;
    static STORAGEDATA			storageData;				// High-level storage configuration options
	// Storage exported data
	static SFUNCTIONS			storageFunctions;			// Functions for walk scan, RT scan, lifecycle.
    static SFILETABLE			ioFunctions;				// SNODE IO table.
    static NODEFUNCTIONS		snodeFunctions;				// SNODE functions.
	// Storage data received from storage manager
	static PSSFUNCTIONS			storageManagerFunctions;	// Utility functions provided by Storage Manager.
	// Storage state data
	static bool					initialized;				// TRUE if the storage has been initialized, else FALSE
	static HANDLE				exitEventHandle;			// Set when helper threads should exit
	static TCHAR				currentUserName[MAX_PATH+1];	// Holds the current username provided by the storage manager
private:
	// ** FUNCTION MEMBERS **
	// Initializes the storage
	DWORD CoreInitialize( DWORD flags, PSTORAGEINFO* storageInfo, HANDLE* storageInstance, PSSFUNCTIONS storageManagerFunctions );
	// Storage DeInit export.  Calls the user's DeInitialize() function, then closes storageInfo.hRTSConfigKey and returns the code
	// from DeInitialize()
	DWORD CoreDeInitialize( HANDLE storageHandle );

	// ** DATA MEMBERS **
	static CRTVScanStorage* storageObject;

	// ** FRIENDS **
	// Storage Init export
	// This must be exported with a non-decorated name, and so cannot be a class member.
	template<class T> friend DWORD StorageInit( DWORD flags, PSTORAGEINFO* storageInfo, HANDLE* storageInstance, PSSFUNCTIONS storageManagerFunctions );
};

template<class T> DWORD StorageInit( DWORD flags, PSTORAGEINFO* storageInfo, HANDLE* storageInstance, PSSFUNCTIONS storageManagerFunctions )
// Storage Init export
// Called from RTVScan after the storage DLL is loaded to initialize it.
// Returns ERROR_SUCCESS on success, else the RTVScan error code of the failure
{
	T*				newStorage		= NULL;
    DWORD           returnValDW     = ERROR_GENERAL;

    // Validate parameters
    if ((storageInfo == NULL) || (storageInstance == NULL) || (storageManagerFunctions == NULL))
        return ERROR_BAD_PARAM;

	// Create the new storage
	newStorage = new(std::nothrow) T;
	if (newStorage != NULL)
	{
		returnValDW = newStorage->CoreInitialize(flags, storageInfo, storageInstance, storageManagerFunctions);
		if (returnValDW == ERROR_SUCCESS)
		{
			CRTVScanStorage::storageObject = dynamic_cast<CRTVScanStorage*>(newStorage);
			SAVASSERT(CRTVScanStorage::storageObject != NULL);
			*storageInstance = CRTVScanStorage::DefaultStorageHandle;
		}
	}

	// Cleanup and return result
	if (returnValDW != ERROR_SUCCESS)
	{
		if (newStorage != NULL)
		{
			delete newStorage;
			newStorage = NULL;
		}
	}

    return returnValDW;
}


template<class T> HRESULT DllRegisterServerDo()
// Adds all registration entries.  An RTVScan storage isn't a COM object, but it is convenient to use this export because
// of the widespread support for it.
{
	T									tempStorage;
	CRTVScanStorage::StorageConfig		config;
	TCHAR								storageSubkeyName[2*MAX_PATH+1]		= {""};
	HKEY								parentKeyHandle						= NULL;
	HKEY								targetKeyHandle						= NULL;
	HKEY								altTargetKeyHandle					= NULL;
	DWORD								returnValDW							= ERROR_FILE_NOT_FOUND;
	HRESULT								returnValHR							= E_FAIL;

	returnValHR = tempStorage.GetConfig(&config);

	if (SUCCEEDED(returnValHR))
		returnValDW = RegCreateKeyEx(HKEY_LOCAL_MACHINE, REGHEADER, NULL, NULL, NULL, KEY_ALL_ACCESS, NULL, &parentKeyHandle, NULL);
	if (returnValDW == ERROR_SUCCESS)
	{
		sssnprintf(storageSubkeyName, sizeof(storageSubkeyName), "%s\\%s", szReg_Key_Storages, config.storageName.c_str());
		returnValDW = RegCreateKeyEx(parentKeyHandle, storageSubkeyName, NULL, NULL, NULL, KEY_ALL_ACCESS, NULL, &targetKeyHandle, NULL);
	}
	if (returnValDW == ERROR_SUCCESS)
	{
		returnValHR = S_OK;
		returnValDW = RegSetValueEx(targetKeyHandle, DLL_NAME, NULL, REG_SZ, (BYTE*) config.fileNodeName.c_str(), lstrlen(config.fileNodeName.c_str()) + sizeof(char));
		if ((returnValDW != ERROR_SUCCESS) && (returnValHR == S_OK))
			returnValHR = HRESULT_FROM_WIN32(returnValDW);

		returnValDW = RegSetValueEx(targetKeyHandle, DLL_PATH, NULL, REG_SZ, (BYTE*) config.dllPath.c_str(), lstrlen(config.dllPath.c_str()) + sizeof(char));
		if ((returnValDW != ERROR_SUCCESS) && (returnValHR == S_OK))
			returnValHR = HRESULT_FROM_WIN32(returnValDW);

		returnValDW = RegSetValueEx(targetKeyHandle, DLL_ENTRY_POINT, NULL, REG_SZ, (BYTE*) config.entrypoint.c_str(), lstrlen(config.entrypoint.c_str()) + sizeof(char));
		if ((returnValDW != ERROR_SUCCESS) && (returnValHR == S_OK))
			returnValHR = HRESULT_FROM_WIN32(returnValDW);

		returnValDW = RegSetValueEx(targetKeyHandle, DISPLAY_NAME, NULL, REG_SZ, (BYTE*) config.storageName.c_str(), lstrlen(config.storageName.c_str()) + sizeof(char));
		if ((returnValDW != ERROR_SUCCESS) && (returnValHR == S_OK))
			returnValHR = HRESULT_FROM_WIN32(returnValDW);

		returnValDW = RegSetValueEx(targetKeyHandle, szReg_Val_Type, NULL, REG_DWORD, (BYTE*) &(config.type), sizeof(config.type));
		if ((returnValDW != ERROR_SUCCESS) && (returnValHR == S_OK))
			returnValHR = HRESULT_FROM_WIN32(returnValDW);

		if ((config.notifyTitle != "") || (config.notifyEventA != 0) || (config.notifyEventB != 0) || (config.notifyEventC != 0))
		{
			returnValDW = RegCreateKeyEx(targetKeyHandle, szReg_Key_Storage_RealTime, NULL, NULL, NULL, KEY_ALL_ACCESS, NULL, &altTargetKeyHandle, NULL);
			if (returnValDW == ERROR_SUCCESS)
			{
                if (config.notifyTitle != "")
                {
    				returnValDW = RegSetValueEx(altTargetKeyHandle, szReg_Val_MessageBoxTitle, NULL, REG_SZ, (BYTE*) config.notifyTitle.c_str(), lstrlen(config.notifyTitle.c_str())+sizeof(char));
    				if ((returnValDW != ERROR_SUCCESS) && (returnValHR == S_OK))
    					returnValHR = HRESULT_FROM_WIN32(returnValDW);
                }

				if (config.notifyEventA != 0)
				{
					returnValDW = RegSetValueEx(altTargetKeyHandle, szReg_Val_NotifyEventA, NULL, REG_DWORD, (BYTE*) &(config.notifyEventA), sizeof(config.notifyEventA));
					if ((returnValDW != ERROR_SUCCESS) && (returnValHR == S_OK))
						returnValHR = HRESULT_FROM_WIN32(returnValDW);
				}
				if (config.notifyEventB != 0)
				{
					returnValDW = RegSetValueEx(altTargetKeyHandle, szReg_Val_NotifyEventB, NULL, REG_DWORD, (BYTE*) &(config.notifyEventB), sizeof(config.notifyEventB));
					if ((returnValDW != ERROR_SUCCESS) && (returnValHR == S_OK))
						returnValHR = HRESULT_FROM_WIN32(returnValDW);
				}
				if (config.notifyEventC != 0)
				{
					returnValDW = RegSetValueEx(altTargetKeyHandle, szReg_Val_NotifyEventC, NULL, REG_DWORD, (BYTE*) &(config.notifyEventC), sizeof(config.notifyEventC));
					if ((returnValDW != ERROR_SUCCESS) && (returnValHR == S_OK))
						returnValHR = HRESULT_FROM_WIN32(returnValDW);
				}
			}
			else
			{
				returnValHR = HRESULT_FROM_WIN32(returnValDW);
			}
		}
	}
	else
	{
		returnValHR = HRESULT_FROM_WIN32(returnValDW);
	}

	// Shutdown and return
	if (targetKeyHandle != NULL)
	{
		RegCloseKey(targetKeyHandle);
		targetKeyHandle = NULL;
	}
	if (parentKeyHandle != NULL)
	{
		RegCloseKey(parentKeyHandle);
		parentKeyHandle = NULL;
	}
	return returnValHR;
}

template<class T> HRESULT DllUnregisterServerDo()
// Removes all registration entries.  An RTVScan storage isn't a COM object, but it is convenient to use this export because
// of the widespread support for it.
{
	T									tempStorage;
	CRTVScanStorage::StorageConfig		config;
	TCHAR								storageSubkeyName[2*MAX_PATH+1]		= {""};
	HKEY								parentKeyHandle						= NULL;
	DWORD								returnValDW							= ERROR_FILE_NOT_FOUND;
	HRESULT								returnValHR							= E_FAIL;

	returnValHR = tempStorage.GetConfig(&config);
	if (SUCCEEDED(returnValHR))
		returnValDW = RegOpenKeyEx(HKEY_LOCAL_MACHINE, REGHEADER, OPEN_EXISTING, KEY_ALL_ACCESS, &parentKeyHandle);
	if (returnValDW == ERROR_SUCCESS)
	{
        sssnprintf(storageSubkeyName, sizeof(storageSubkeyName), "%s\\%s", szReg_Key_Storages, config.storageName.c_str());
		returnValDW = SHDeleteKey(parentKeyHandle, storageSubkeyName);
		RegCloseKey(parentKeyHandle);
		parentKeyHandle = NULL;
	}
	returnValHR = HRESULT_FROM_WIN32(returnValDW);

	return returnValHR;
}

#endif // RTVSCAN_STORAGE_BASE