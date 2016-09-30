// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// SymProtectManifestUpdater.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include <tchar.h>
#include <windows.h>
#include <shlobj.h>
#include <msi.h>
#include <string>
#include "resource.h"
#include "DarwinResCommon.h"
#include "navver.h"
#include "SymSaferStrings.h"

// ** CONSTANTS **
#define SYMANTEC_SHARED_DIRNAME			"Symantec Shared"
#define MANIFEST_DIRECTORY_NODENAME		"SPManifests"

#define MANIFEST_BASENAME_SCS			"scs-scs"
#define MANIFEST_BASENAME_SAV			"scs-sav"
#define MANIFEST_EXT_MAIN				".spm"
#define MANIFEST_EXT_GUARD				".grd"
#define MANIFEST_EXT_SIGNATURE			".sig"

#define EXITCODE_SUCCESS				0
#define EXITCODE_PRODUCTNOTFOUND		1
#define EXITCODE_GENERALFAILURE			2

#ifdef _UNICODE
	typedef std::wstring tstring;
#else
	typedef std::string tstring;
#endif

// ** PROTOTYPES **
// Loads a string resource and returns it as an STL string, or "" if any errors occur loading it
tstring LoadString( UINT resourceID );
// Extracts file resource resourceName to the file targetFilename and reports an error to the console on failure
// Returns S_OK on success, else the error code of the failure
HRESULT ExtractResourceToFileUI( tstring resourceName, tstring targetFilename );
// Copies sourceFilename to targetFilename and reports an error to the console if a failure occurs
// Returns S_OK on success, else the error code of the failure
HRESULT CopyFileUI( tstring sourceFilename, tstring targetFilename, BOOL failIfExists );
// Deletes targetFilename and reports an error to the console if a failure occurs
// Returns S_OK on success, else the error code of the failure
HRESULT DeleteFileUI( tstring targetFilename );
//Extracts the resource named resourceName to targetFilename
HRESULT ExtractResourceToFile( tstring resourceName, tstring targetFilename );

// ** GLOBALS **
HMODULE moduleHandle = NULL;
DARWIN_DECLARE_RESOURCE_LOADER(_T("SymProtectManifestUpdaterRes.dll"), _T("SymProtectManifestUpdater"))


int _tmain(int argc, _TCHAR* argv[])
{
	tstring				bannerTitle;
	tstring				bannerCopyright;
	tstring				forceSwitchSCS;
	tstring				forceSwitchSAV;
	TCHAR				pathBuffer[2*MAX_PATH+1]	= {""};
	tstring				tempDirectory;
	tstring				manifestDirectory;
	bool				scsInstalled				= false;
	bool				savInstalled				= false;
	tstring				targetPathname;
	tstring				manifestBaseName;
	tstring				tempManifestFilenameMain;
	tstring				tempManifestFilenameGuard;
	tstring				tempManifestFilenameSignature;
	tstring				currManifestFilenameMain;
	tstring				currManifestFilenameGuard;
	tstring				currManifestFilenameSignature;
	tstring				errorMessage;
	TCHAR				errorMessageTC[2*MAX_PATH+1]= {""};
	tstring				userMessage;
	DWORD				returnValDW					= ERROR_FILE_NOT_FOUND;
	HRESULT				returnValHR					= E_FAIL;

	// Initialize
	if(SUCCEEDED(g_ResLoader.Initialize()))
	{
		moduleHandle = g_ResLoader.GetResourceInstance();
	}
	else
	{
		return EXITCODE_GENERALFAILURE;
	}

	// Locate the manifest directory
	if (SHGetSpecialFolderPath(NULL, pathBuffer, CSIDL_PROGRAM_FILES_COMMON, TRUE))
	{
		manifestDirectory = pathBuffer;
		manifestDirectory = manifestDirectory + '\\' + SYMANTEC_SHARED_DIRNAME + '\\' + MANIFEST_DIRECTORY_NODENAME + '\\';
	}
	GetTempPath(sizeof(pathBuffer)/sizeof(pathBuffer[0]), pathBuffer);
	tempDirectory = pathBuffer;

	// Display banner
	bannerTitle = LoadString(IDS_SYMPROTECT_MANIFESTUPDATE_BANNER_TITLE);
	bannerCopyright = LoadString(IDS_SYMPROTECT_MANIFESTUPDATE_BANNER_COPYRIGHT);
	_tprintf(_T("%s\n"), bannerTitle.c_str());
	_tprintf(_T("%s\n"), bannerCopyright.c_str());
	_tprintf(_T("\n"));

	// Parse command line
	if (argc == 2)
	{
		forceSwitchSCS = LoadString(IDS_SYMPROTECT_MANIFESTUPDATE_SWITCH_FORCESCS);
		forceSwitchSAV = LoadString(IDS_SYMPROTECT_MANIFESTUPDATE_SWITCH_FORCESAV);
		if (_tcsicmp(forceSwitchSCS.c_str(), argv[1]) == 0)
			scsInstalled = true;
		else if (_tcsicmp(forceSwitchSAV.c_str(), argv[1]) == 0)
			savInstalled = true;
	}

	// If detection is not forced, auto-detect if SCS/SAV are installed
	if (!scsInstalled && !savInstalled)
	{
		// Determine what product is installed and abort if none found
		returnValDW = MsiQueryProductState(MSI_PRODUCTCODE_SCS);
		if (returnValDW == INSTALLSTATE_DEFAULT)
			scsInstalled = true;
		returnValDW = MsiQueryProductState(MSI_PRODUCTCODE_SAV);
		if (returnValDW == INSTALLSTATE_DEFAULT)
			savInstalled = true;
	}
	if (savInstalled)
	{
		manifestBaseName = MANIFEST_BASENAME_SAV;
	}
	else if (scsInstalled)
	{
		manifestBaseName = MANIFEST_BASENAME_SCS;
	}
	else
	{
		errorMessage = LoadString(IDS_SYMPROTECT_MANIFESTUPDATE_ERROR_NOPRODUCT);
		sssnprintf(errorMessageTC, sizeof(errorMessageTC), errorMessage.c_str(), 3, NAV_VERSION_MINOR, NAV_VERSION_INLINE, NAV_BUILD_NUMBER, NAV_VERSION_MAJOR, NAV_VERSION_MINOR, NAV_VERSION_INLINE, NAV_BUILD_NUMBER);
		_tprintf(_T("%s\n"), errorMessageTC);
		return EXITCODE_PRODUCTNOTFOUND;
	}

	// Extract the manifest fileset
	tempManifestFilenameMain		= tempDirectory + manifestBaseName + MANIFEST_EXT_MAIN;
	tempManifestFilenameGuard		= tempDirectory + manifestBaseName + MANIFEST_EXT_GUARD;
	tempManifestFilenameSignature	= tempDirectory + manifestBaseName + MANIFEST_EXT_SIGNATURE;
	currManifestFilenameMain		= manifestDirectory + manifestBaseName + MANIFEST_EXT_MAIN;
	currManifestFilenameGuard		= manifestDirectory + manifestBaseName + MANIFEST_EXT_GUARD;
	currManifestFilenameSignature	= manifestDirectory + manifestBaseName + MANIFEST_EXT_SIGNATURE;

	returnValHR = ExtractResourceToFileUI(manifestBaseName + MANIFEST_EXT_MAIN, tempManifestFilenameMain.c_str());
	if (SUCCEEDED(returnValHR))
		returnValHR = ExtractResourceToFileUI(manifestBaseName + MANIFEST_EXT_GUARD, tempManifestFilenameGuard.c_str());
	if (SUCCEEDED(returnValHR))
		returnValHR = ExtractResourceToFileUI(manifestBaseName + MANIFEST_EXT_SIGNATURE, tempManifestFilenameSignature.c_str());

	// Update the manifest fileset
	if (SUCCEEDED(returnValHR))
	{
		returnValHR = CopyFileUI(tempManifestFilenameMain.c_str(), currManifestFilenameMain.c_str(), FALSE);
		if (SUCCEEDED(returnValHR))
			returnValHR = CopyFileUI(tempManifestFilenameGuard.c_str(), currManifestFilenameGuard.c_str(), FALSE);
		if (SUCCEEDED(returnValHR))
			returnValHR = CopyFileUI(tempManifestFilenameSignature.c_str(), currManifestFilenameSignature.c_str(), FALSE);
	}
	// Cleanup temp files
	DeleteFileUI(tempManifestFilenameMain);
	DeleteFileUI(tempManifestFilenameGuard);
	DeleteFileUI(tempManifestFilenameSignature);

	// Return success or failure, depending on our status
	if (SUCCEEDED(returnValHR))
	{
		if (savInstalled)
			userMessage = LoadString(IDS_SYMPROTECT_MANIFESTUPDATE_UPDATED_SAV);
		else
			userMessage = LoadString(IDS_SYMPROTECT_MANIFESTUPDATE_UPDATED_SCS);
		_tprintf(_T("%s\n"), userMessage.c_str());
		return EXITCODE_SUCCESS;
	}
	else
	{
		return EXITCODE_GENERALFAILURE;
	}
}

tstring LoadString( UINT resourceID )
// Loads a string resource and returns it as an STL string, or "" if any errors occur loading it
{
	TCHAR stringBuffer[2*MAX_PATH+1] = {_T("")};

	::LoadString(moduleHandle, resourceID, stringBuffer, sizeof(stringBuffer)/sizeof(stringBuffer[0]));
	return stringBuffer;
}

HRESULT ExtractResourceToFileUI( tstring resourceName, tstring targetFilename )
// Extracts file resource resourceName to the file targetFilename and reports an error to the console on failure
// Returns S_OK on success, else the error code of the failure
{
	tstring			errorTemplate;
	TCHAR			errorMessage[2*MAX_PATH+1]		= {_T("")};
	DWORD			returnValDW						= 0xffffffff;
	HRESULT			returnValHR						= E_FAIL;

	// Delete any existing file
	returnValDW = GetFileAttributes(targetFilename.c_str());
	if ((returnValDW != 0xffffffff) && ((returnValDW & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY))
	{
		SetFileAttributes(targetFilename.c_str(), FILE_ATTRIBUTE_NORMAL);
		DeleteFile(targetFilename.c_str());
	}
	// Extract resource to the file
	returnValHR = ExtractResourceToFile(resourceName, targetFilename);
	if (FAILED(returnValHR))
	{
		errorTemplate = LoadString(IDS_SYMPROTECT_MANIFESTUPDATE_ERROR_EXTRACT);
		sssnprintf(errorMessage, sizeof(errorMessage), errorTemplate.c_str(), returnValHR, resourceName.c_str(), targetFilename.c_str());
		_tprintf(_T("%s\n"), errorMessage);
	}
	return returnValHR;
}

HRESULT CopyFileUI( tstring sourceFilename, tstring targetFilename, BOOL failIfExists )
// Copies sourceFilename to targetFilename and reports an error to the console if a failure occurs
// Returns S_OK on success, else the error code of the failure
{
	tstring			errorTemplate;
	TCHAR			errorMessage[2*MAX_PATH+1]		= {_T("")};
	HRESULT			returnValHR						= E_FAIL;

	if (CopyFile(sourceFilename.c_str(), targetFilename.c_str(), failIfExists))
	{
		returnValHR = S_OK;
	}
	else
	{
		returnValHR = HRESULT_FROM_WIN32(GetLastError());
		errorTemplate = LoadString(IDS_SYMPROTECT_MANIFESTUPDATE_ERROR_COPY);
		sssnprintf(errorMessage, sizeof(errorMessage), errorTemplate.c_str(), returnValHR, targetFilename.c_str(), sourceFilename.c_str());
		_tprintf(_T("%s\n"), errorMessage);
	}

	return returnValHR;
}

HRESULT DeleteFileUI( tstring targetFilename )
// Deletes targetFilename and reports an error to the console if a failure occurs
// Returns S_OK on success, else the error code of the failure
{
	tstring			errorTemplate;
	TCHAR			errorMessage[2*MAX_PATH+1]		= {_T("")};
	HRESULT			returnValHR						= E_FAIL;

	if (DeleteFile(targetFilename.c_str()))
	{
		returnValHR = S_OK;
	}
	else
	{
		returnValHR = HRESULT_FROM_WIN32(GetLastError());
		errorTemplate = LoadString(IDS_SYMPROTECT_MANIFESTUPDATE_ERROR_DELETETEMP);
		sssnprintf(errorMessage, sizeof(errorMessage), errorTemplate.c_str(), returnValHR, targetFilename.c_str());
		_tprintf(_T("%s\n"), errorMessage);
	}

	return returnValHR;
}

HRESULT ExtractResourceToFile( tstring resourceName, tstring targetFilename )
//Extracts the resource named resourceName to targetFilename
//Returns:
//	HRESULT_FROM_WIN32(ERROR_RESOURCE_NAME_NOT_FOUND)	- named resource not found
//	STG_E_READFAULT										- error reading resource data
//	STG_E_LOCKVIOLATION									- error locking resource
//	CO_E_FAILEDTOCREATEFILE								- error creating targetFilename
//	STG_E_WRITEFAULT									- error writing data to targetFilename
{
	HRSRC			infoHandle				= NULL;						//Handle to info about the resource
	HGLOBAL			dataHandle				= NULL;						//Handle to the resource data
	LPBYTE			dataPointer				= NULL;						//Pointer to the binay resource data in memory
	DWORD			dataSize				= 0;						//Size of this resource
	HANDLE			targetFileHandle		= INVALID_HANDLE_VALUE;		//Handle to target file to save this data to
	DWORD			bytesWritten			= 0;						//Dummy for WriteFile
	HRESULT			returnVal				= HRESULT_FROM_WIN32(ERROR_RESOURCE_NAME_NOT_FOUND);

	infoHandle = FindResource(moduleHandle, resourceName.c_str(), RT_RCDATA);
	if (infoHandle != NULL)
	{
		dataHandle = LoadResource(moduleHandle, infoHandle);
		if (dataHandle)
		{
			dataPointer = (LPBYTE) LockResource(dataHandle);
			if (dataPointer)
			{
				dataSize = SizeofResource(moduleHandle, infoHandle);
				targetFileHandle = CreateFile(targetFilename.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
				if (targetFileHandle != INVALID_HANDLE_VALUE)
				{
					if (WriteFile(targetFileHandle, dataPointer, dataSize, &bytesWritten, NULL))
						returnVal = S_OK;
					else
						returnVal = STG_E_WRITEFAULT;
					CloseHandle(targetFileHandle);
				}
				else
				{
					returnVal = HRESULT_FROM_WIN32(ERROR_CANNOT_MAKE);
				}
			}
			else
			{
				returnVal = STG_E_LOCKVIOLATION;
			}
		}
		else
		{
			returnVal = HRESULT_FROM_WIN32(GetLastError());
		}
	}

	return returnVal;
}