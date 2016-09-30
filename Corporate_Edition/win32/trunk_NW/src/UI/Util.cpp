#include "stdafx.h"
#include "Util.h"
#include <msi.h>
#include <msiquery.h>
#include <psapi.h>
#include "VPExceptionHandling.h"
#include "SymSaferRegistry.h"
#include "ClientReg.h"
#include "StrSafe.h"

// Add link libraries for build
#pragma comment(lib, "psapi.lib")
#pragma comment(lib, "msi.lib")

HRESULT EnumCategoryComponents( CATID componentCategory, CLSIDList* components )
// Sets *components equal to a list of the CLSIDs in the specified component category
// Returns S_OK on success else the error code of the failure
{
    CComPtr<ICatInformation>    categoryManager;
    CComPtr<IEnumGUID>          componentGUIDs;
    CLSID                       currCLSID           = GUID_NULL;
    ULONG                       noFetched           = 0;
    CComBSTR                    shortDescription;
    HRESULT                     returnValHR         = E_FAIL;

    // Validate parameters
    if (components == NULL)
        return E_POINTER;

    returnValHR = categoryManager.CoCreateInstance(CLSID_StdComponentCategoriesMgr);
    if (SUCCEEDED(returnValHR))
    {
        returnValHR = categoryManager->EnumClassesOfCategories(1, &componentCategory, 0, NULL, &componentGUIDs);
        if (SUCCEEDED(returnValHR))
        {
            returnValHR = S_OK;
            while (componentGUIDs->Next(1, &currCLSID, &noFetched) == S_OK)
            {
                try
                {
                    components->push_back(currCLSID);
                }
                catch(std::bad_alloc&)
                {
                    returnValHR = E_OUTOFMEMORY;
                    break;
                }
            }
            componentGUIDs.Release();
        }
        categoryManager.Release();
    }

    return returnValHR;
}


// CSIMPLEPERFORMANCETIMER
CSimplePerformanceTimer::CSimplePerformanceTimer() : startTime(0), stopTime(0)
{
    // Nothing for now
}

CSimplePerformanceTimer::~CSimplePerformanceTimer()
{
    // Nothing for now
}

void CSimplePerformanceTimer::StartTimer()
{
    startTime = GetTickCount();
}

void CSimplePerformanceTimer::StopTimer()
{
    stopTime = GetTickCount();
}

DWORD CSimplePerformanceTimer::GetElapsedTime( void )
{
    DWORD   elapsedTime     = 0;

    if (stopTime >= startTime)
        elapsedTime = stopTime - startTime;
    else
        elapsedTime = (0xffffffff - startTime) + stopTime;

    return elapsedTime;
}

void CSimplePerformanceTimer::OutputElapsedTime( LPCTSTR timerDescription )
{
    TCHAR debugLine[MAX_PATH+1] = _T("");

    StringCchPrintf(debugLine, sizeof(debugLine)/sizeof(debugLine[0]), _T("%s:%d\n"), timerDescription, GetElapsedTime());
    OutputDebugString(debugLine);
}

HRESULT GetProcessInfo( LPCTSTR processBaseFilename, ProcessInfo* thisProcessInfo )
// Sets *thisProcessInfo equal to basic information on the first process found whose base filename matches the specified filename
{
    DWORD*      processIDs                  = NULL;
    DWORD       processIDsAllocated         = 0;
    DWORD       processIDsActual            = 0;
    DWORD       bytesNeeded                 = 0;
    DWORD       currProcessIDno             = 0;
    HANDLE      processHandle               = NULL;
    HMODULE     moduleHandle                = NULL;
    wchar_t     stringBuffer[2*MAX_PATH+1]  = _T("");
    DWORD       returnValDW                 = ERROR_OUT_OF_PAPER;
    BOOL        returnValBOOL               = FALSE;
    HRESULT     returnValHR                 = E_FAIL;

    // Validate parameters
    if (thisProcessInfo == NULL)
        return E_POINTER;

    // Get a list of a sufficiently large number of processes.
    processIDsAllocated = 1024;
    processIDs = new (std::nothrow) DWORD[processIDsAllocated];
    if (processIDs != NULL)
    {
        returnValBOOL = EnumProcesses(processIDs, processIDsAllocated * sizeof(processIDs[0]), &bytesNeeded);
        if (returnValBOOL)
        {
            returnValHR = S_FALSE;
            processIDsActual = min(bytesNeeded/sizeof(processIDs[0]), processIDsAllocated);
            for(currProcessIDno = 0; (currProcessIDno < processIDsActual) && (returnValHR == S_FALSE); currProcessIDno++)
            {
                processHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processIDs[currProcessIDno]);
                if (processHandle != NULL)
                {
                    stringBuffer[0] = NULL;
                    returnValBOOL = EnumProcessModules(processHandle, &moduleHandle, sizeof(moduleHandle), &bytesNeeded);
                    if (returnValBOOL)
                    {
                        returnValDW = GetModuleBaseName(processHandle, moduleHandle, stringBuffer, sizeof(stringBuffer)/sizeof(stringBuffer[0]));
                        if (returnValDW != 0)
                        {
                            if (_wcsicmp(stringBuffer, processBaseFilename) == 0)
                            {
                                try
                                {
                                    thisProcessInfo->id = processIDs[currProcessIDno];
                                    thisProcessInfo->imageBaseFilename = stringBuffer;
                                    thisProcessInfo->imageFullPathname = _T("");
                                    stringBuffer[0] = NULL;
                                    returnValDW = GetModuleFileNameEx(processHandle, moduleHandle, stringBuffer, sizeof(stringBuffer)/sizeof(stringBuffer[0]));
                                    if (returnValDW != 0)
                                        thisProcessInfo->imageFullPathname = stringBuffer;
                                    else
                                        returnValDW = GetLastError();
                                    returnValHR = S_OK;
                                }
                                catch (std::bad_alloc&)
                                {
                                    returnValHR = E_OUTOFMEMORY;
                                }
                            }
                        }
                    }
                    CloseHandle(processHandle);
                    processHandle = NULL;
                    moduleHandle = NULL;
                }
            }
        }
        else
        {
            returnValHR = HRESULT_FROM_WIN32(GetLastError());
        }
        delete [] processIDs;
        processIDs = NULL;
    }
    else
    {
        returnValHR = E_OUTOFMEMORY;
    }

    return returnValHR;
}

HRESULT GetProcessModules( DWORD processID, ModuleInfoList* moduleInfos )
// Sets *moduleInfos equal to a list of moduleInfos on for the process specified by processID
{
    HANDLE      processHandle               = NULL;
    HMODULE*    moduleHandles               = NULL;
    DWORD       moduleHandlesAllocated      = 0;
    DWORD       moduleHandlesActual         = 0;
    DWORD       bytesNeeded                 = 0;
    DWORD       currModuleNo                = 0;
    wchar_t     stringBuffer[2*MAX_PATH+1]  = _T("");
    ModuleInfo  newModuleInfo;
    DWORD       returnValDW                 = ERROR_OUT_OF_PAPER;
    BOOL        returnValBOOL               = FALSE;
    HRESULT     returnValHR                 = E_FAIL;

    // Validate parameters
    if (moduleInfos == NULL)
        return E_POINTER;

    processHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID);
    if (processHandle != NULL)
    {
        stringBuffer[0] = NULL;
        returnValBOOL = EnumProcessModules(processHandle, moduleHandles, 0, &bytesNeeded);
        if (returnValBOOL)
        {
            moduleInfos->clear();
            moduleHandlesAllocated = static_cast<DWORD>(1.5 * (bytesNeeded/sizeof(HMODULE)));
            moduleHandles = new (std::nothrow) HMODULE[moduleHandlesAllocated];
            if (moduleHandles != NULL)
            {
                returnValBOOL = EnumProcessModules(processHandle, moduleHandles, moduleHandlesAllocated*sizeof(HMODULE), &bytesNeeded);
                if (returnValBOOL)
                {
                    moduleHandlesActual = min(bytesNeeded/sizeof(HMODULE), moduleHandlesAllocated);
                    returnValHR = S_OK;
                    for (currModuleNo = 0; currModuleNo < moduleHandlesActual; currModuleNo++)
                    {
                        returnValDW = GetModuleBaseName(processHandle, moduleHandles[currModuleNo], stringBuffer, sizeof(stringBuffer)/sizeof(stringBuffer[0]));
                        if (returnValDW != 0)
                        {
                            try
                            {
                                newModuleInfo.baseFilename = stringBuffer;
                                newModuleInfo.fullPathname = _T("");
                                stringBuffer[0] = NULL;
                                returnValDW = GetModuleFileNameEx(processHandle, moduleHandles[currModuleNo], stringBuffer, sizeof(stringBuffer)/sizeof(stringBuffer[0]));
                                if (returnValDW != 0)
                                    newModuleInfo.fullPathname = stringBuffer;
                                else
                                    returnValDW = GetLastError();
                                moduleInfos->push_back(newModuleInfo);
                            }
                            catch (std::bad_alloc&)
                            {
                                returnValHR = E_OUTOFMEMORY;
                            }
                        }
                    }
                }
                else
                {
                    returnValHR = HRESULT_FROM_WIN32(GetLastError());
                }
                delete [] moduleHandles;
                moduleHandles = NULL;
            }
            else
            {
                returnValHR = E_OUTOFMEMORY;
            }
        }
        else
        {
            returnValHR = HRESULT_FROM_WIN32(GetLastError());
        }

        CloseHandle(processHandle);
        processHandle = NULL;
    }
    else
    {
        returnValHR = HRESULT_FROM_WIN32(GetLastError());
    }

    return returnValHR;
}

HRESULT GetProductsWithUpgradeCode( LPCTSTR upgradeCode, DWORD dwMinMajorVer, DWORD dwMinMinorVer, DWORD dwMaxMajorVer, DWORD dwMaxMinorVer, StringList* productCodes )
// Appends a list of all product codes installed with the specified upgrade code to *productCodes
{
    TCHAR       productCode[50]     = _T("");
    DWORD       productNo           = 0;
    DWORD       returnValDW         = ERROR_OUT_OF_PAPER;
    TCHAR       majorVer[12]        = {0};
    DWORD       productMajorVer     = 0;
    TCHAR       minorVer[12]        = {0};
    DWORD       productMinorVer     = 0;
    DWORD       stringSize          = 0;

    // Validate parameters
    if (upgradeCode == NULL)
        return E_POINTER;
    if (productCodes == NULL)
        return E_POINTER;

    returnValDW = MsiEnumRelatedProducts(upgradeCode, NULL, productNo, productCode);
    while (returnValDW == ERROR_SUCCESS)
    {
        try
        {
            productMajorVer = 0;
            productMinorVer = 0;

            stringSize = sizeof(majorVer)/sizeof(majorVer[0]);
            returnValDW = MsiGetProductInfo(productCode, INSTALLPROPERTY_VERSIONMAJOR, majorVer, &stringSize);
            if (returnValDW == ERROR_SUCCESS)
            {
                productMajorVer = _ttoi(majorVer);

                stringSize = sizeof(minorVer)/sizeof(minorVer[0]);
                returnValDW = MsiGetProductInfo(productCode, INSTALLPROPERTY_VERSIONMINOR, minorVer, &stringSize);
                if (returnValDW == ERROR_SUCCESS)
                {
                    productMinorVer = _ttoi(minorVer);

                    if ( ( (dwMinMajorVer < productMajorVer) || ( (dwMinMajorVer == productMajorVer) && (dwMinMinorVer <= productMinorVer) ) )
                          && ( (dwMaxMajorVer > productMajorVer) || ( (dwMaxMajorVer == productMajorVer) && (dwMaxMinorVer >= productMinorVer) ) ) )
                    {
                        productCodes->push_back(productCode);
                    }
                }
            }

            productNo += 1;
            returnValDW = MsiEnumRelatedProducts(upgradeCode, NULL, productNo, productCode);
        }
        VP_CATCH_MEMORYEXCEPTIONS(returnValDW = ERROR_NOT_ENOUGH_MEMORY;);
    }

    // Return result
    if (returnValDW == ERROR_NO_MORE_ITEMS)
        return S_OK;
    else
        return HRESULT_FROM_WIN32(returnValDW);
}

HRESULT RegQueryDword( HKEY keyHandle, LPCTSTR valueName, DWORD* valueData, DWORD defaultValue )
// Reads the specified DWORD value into valueData, or sets to defaultValue if not present/not DWORD/any error
// Returns S_OK if successfully read in, S_FALSE if not present, HFW32(ERROR_DATATYPE_MISMATCH) if value exists but is
// of wrong type, else the error code of the failure
// *valueData is ALWAYS set as specified in the beginning regardless of the return value
{
    DWORD       tempValueData   = 0;
    DWORD       valueDataSize   = sizeof(*valueData);
    DWORD       valueType       = REG_SZ;
    DWORD       returnValDW     = ERROR_OUT_OF_PAPER;
    HRESULT     returnValHR     = E_FAIL;

    // Validate parameters    
    if (keyHandle == NULL)
        return E_INVALIDARG;
    if (valueData == NULL)
        return E_POINTER;

    returnValDW = SymSaferRegQueryValueEx(keyHandle, valueName, NULL, &valueType, (LPBYTE) &tempValueData, &valueDataSize);
    if (returnValDW == ERROR_SUCCESS)
    {
        if (valueType == REG_DWORD)
        {
            *valueData = tempValueData;
            returnValHR = S_OK;
        }
        else
        {
            *valueData = defaultValue;
            returnValHR = HRESULT_FROM_WIN32(ERROR_DATATYPE_MISMATCH);
        }
    }
    else
    {
        *valueData = defaultValue;
        if (returnValDW == ERROR_FILE_NOT_FOUND)
            returnValHR = S_FALSE;
        else
            returnValHR = HRESULT_FROM_WIN32(returnValDW);
    }

    return returnValHR;
}

HRESULT RegQueryInt( HKEY keyHandle, LPCTSTR valueName, int* valueData, int defaultValue )
{
    DWORD       valueDataDW = 0;
    HRESULT     returnValHR     = E_FAIL;

    RegQueryDword(keyHandle, valueName, &valueDataDW, static_cast<DWORD>(defaultValue));
    *valueData = valueDataDW;
    return returnValHR;
}

HRESULT RegQueryBool( HKEY keyHandle, LPCTSTR valueName, bool* valueData, bool defaultValue )
// Same as RegQueryDword, but for bool's
{
    DWORD       valueDataDW     = FALSE;
    DWORD       defaultValueDW  = FALSE;
    HRESULT     returnValHR     = E_FAIL;
    
    if (defaultValue)
        defaultValueDW = TRUE;
    returnValHR = RegQueryDword(keyHandle, valueName, &valueDataDW, defaultValueDW);
    if (valueDataDW)
        *valueData = true;
    return returnValHR;
}

HRESULT RegQueryBOOL( HKEY keyHandle, LPCTSTR valueName, BOOL* valueData, BOOL defaultValue )
// Same as RegQueryBool, but for BOOL's
{
    return RegQueryDword(keyHandle, valueName, reinterpret_cast<DWORD*>(valueData), defaultValue);
}

HRESULT RegQueryVARIANTBOOL( HKEY keyHandle, LPCTSTR valueName, VARIANT_BOOL* valueData, BOOL defaultValue )
// Same as RegQueryBool, but for VARIANT_BOOL's
{
    BOOL        valueDataBOOL       = defaultValue;
    HRESULT     returnValHR         = E_FAIL;

    returnValHR = RegQueryBOOL(keyHandle, valueName, &valueDataBOOL, defaultValue);
    *valueData = valueDataBOOL;
    return returnValHR;
}

HRESULT RegQueryString( HKEY keyHandle, LPCTSTR valueName, LPTSTR valueData, DWORD valueDataSize, LPCTSTR defaultValue )
// Sets *valueData equal to the specified registry string value.  For truncated values, reads in up to valueDataSize characters
// and returns success.  valueDataSize is in TCHARs.
{
    DWORD       valueDataSizeTemp               = 0;
    DWORD       returnValDW                     = ERROR_OUT_OF_PAPER;
    HRESULT     returnValHR                     = E_FAIL;

    // Validate parameters
    if (keyHandle == NULL)
        return E_POINTER;
    if (valueData == NULL)
        return E_POINTER;

    // Determine directories
    valueDataSizeTemp = valueDataSize;
    returnValDW = SymSaferRegQueryValueEx(keyHandle, valueName, NULL, NULL, (LPBYTE) valueData, &valueDataSizeTemp);
    if ((returnValDW == ERROR_SUCCESS) || (returnValDW == ERROR_MORE_DATA))
    {
        returnValHR = S_OK;
    }
    else
    {
        valueData[0] = NULL;
        if (defaultValue != NULL)
            StringCchCopy(valueData, valueDataSize, defaultValue);
        if (returnValDW == ERROR_FILE_NOT_FOUND)
            returnValHR = S_FALSE;
        else
            returnValHR = HRESULT_FROM_WIN32(returnValDW);
    }

    return returnValHR;
}

HRESULT RegQueryString( HKEY keyHandle, LPCTSTR valueName, std::wstring* valueData, LPCTSTR defaultValue )
{
    TCHAR       valueDataBuffer[3*MAX_PATH+1]       = _T("");
    HRESULT     returnValHR                         = E_FAIL;
    
    returnValHR = RegQueryString(keyHandle, valueName, valueDataBuffer, sizeof(valueDataBuffer)/sizeof(valueDataBuffer[0]), defaultValue);
    try
    {
        *valueData = valueDataBuffer;
    }
    catch (std::bad_alloc&)
    {
        returnValHR = E_OUTOFMEMORY;
    }
    return returnValHR;

}

HRESULT RegQueryString( HKEY keyHandle, LPCTSTR valueName, BSTR* valueData, LPCTSTR defaultValue )
{
    TCHAR       valueDataBuffer[3*MAX_PATH+1]       = _T("");
    CComBSTR    valueDataBSTR;
    HRESULT     returnValHR                         = E_FAIL;
    
    returnValHR = RegQueryString(keyHandle, valueName, valueDataBuffer, sizeof(valueDataBuffer)/sizeof(valueDataBuffer[0]), defaultValue);
    try
    {
        valueDataBSTR = valueDataBuffer;
        *valueData = valueDataBSTR.Detach();
    }
    catch (std::bad_alloc&)
    {
        returnValHR = E_OUTOFMEMORY;
    }
    return returnValHR;

}

bool FileExists( const std::wstring& filename )
// Returns TRUE if filename exists, else FALSE
{
    bool        returnValBool   = false;
    DWORD       returnValDW     = INVALID_FILE_ATTRIBUTES;

    returnValDW = GetFileAttributes(filename.c_str());
    if (returnValDW != INVALID_FILE_ATTRIBUTES)
    {
        if ((returnValDW & FILE_ATTRIBUTE_DIRECTORY) == 0)
            returnValBool = true;
    }

    return returnValBool;
}

HRESULT GetSavDirectory( std::wstring* pathBuffer )
// Sets *pathBuffer equal to the full pathname to the SAV directory with no terminating
// backslash.
{
    CRegKey     appsKey;
    TCHAR       stringBuffer[2*MAX_PATH+1]      = _T("");
    DWORD       stringBufferSize                = 0;
    LPTSTR      lastCharacter                   = NULL;
    DWORD       returnValDW                     = ERROR_OUT_OF_PAPER;
    HRESULT     returnValHR                     = E_FAIL;

    // Validate parameters
    if (pathBuffer == NULL)
        return E_POINTER;

    // Determine directories
    returnValDW = appsKey.Open(HKEY_LOCAL_MACHINE, _T(szReg_Key_Sym_InstalledApps), KEY_READ);
    if (returnValDW == ERROR_SUCCESS)
    {
        stringBufferSize = sizeof(stringBuffer)/sizeof(stringBuffer[0]);
        returnValDW = SymSaferRegQueryValueEx(appsKey.m_hKey, _T("SAVCE"), NULL, NULL, (LPBYTE) &stringBuffer, &stringBufferSize);
        appsKey.Close();
    }
    if (returnValDW == ERROR_SUCCESS)
    {
        lastCharacter = CharPrev(stringBuffer, stringBuffer+_tcslen(stringBuffer));
        if (*lastCharacter == '\\')
            *lastCharacter = NULL;

        try
        {
            *pathBuffer = stringBuffer;
            returnValHR = S_OK;
        }
        catch (std::bad_alloc&)
        {
            returnValHR = E_OUTOFMEMORY;
        }
    }
    else
    {
        returnValHR = HRESULT_FROM_WIN32(returnValDW);
    }

    return returnValHR;
}

HRESULT GetKeySubkeys( HKEY parentKeyHandle, LPCTSTR keyPath, StringList* subkeyNames )
// Sets *subkeyNames equal to a list of all keys under parentKeyHandle\keyPath.  keyPath is an optional subkey path.
{
	HKEY		enumKeyHandle					= NULL;
	bool		closeEnumKeyHandle				= false;
	TCHAR		currEnumScanName[MAX_PATH]		= _T("");
	DWORD		currEnumScanNo					= 0;
	DWORD		currEnumScanNamebufferSize		= 0;
	DWORD		returnValDW						= ERROR_OUT_OF_PAPER;
	HRESULT		returnValHR						= E_FAIL;
	
	// Validate parameters
	if (parentKeyHandle == NULL)
		return E_INVALIDARG;
	if (subkeyNames == NULL)
		return E_POINTER;

	// Open a subkey, if requested
	if (keyPath == NULL)
	{
		enumKeyHandle = parentKeyHandle;
	}
	else
	{
		returnValDW = RegOpenKeyEx(parentKeyHandle, keyPath, NULL, KEY_READ, &enumKeyHandle);
		closeEnumKeyHandle = true;
	}
	// Enumerate the subkeys
	if (returnValDW == ERROR_SUCCESS)
	{
		returnValHR = S_OK;
		currEnumScanNo = 0;
		currEnumScanNamebufferSize = sizeof(currEnumScanName)/sizeof(currEnumScanName[0]);
		returnValDW = RegEnumKeyEx(enumKeyHandle, currEnumScanNo, currEnumScanName, &currEnumScanNamebufferSize, NULL, NULL, NULL, NULL);
		while ((returnValDW == ERROR_SUCCESS) && (returnValHR == S_OK))
		{
			try
			{
				subkeyNames->push_back(currEnumScanName);
			}
			catch (std::bad_alloc&)
			{
				returnValHR = ERROR_OUTOFMEMORY;
			}
			currEnumScanNo += 1;
			currEnumScanNamebufferSize = sizeof(currEnumScanName)/sizeof(currEnumScanName[0]);
			returnValDW = RegEnumKeyEx(enumKeyHandle, currEnumScanNo, currEnumScanName, &currEnumScanNamebufferSize, NULL, NULL, NULL, NULL);
		}
	}
	else
	{
		returnValHR = HRESULT_FROM_WIN32(returnValDW);
	}
	
	// Cleanup and return
	if (closeEnumKeyHandle)
	{
		if (enumKeyHandle != NULL)
		{
			RegCloseKey(enumKeyHandle);
			enumKeyHandle = NULL;
		}
	}	
	return returnValHR;
}