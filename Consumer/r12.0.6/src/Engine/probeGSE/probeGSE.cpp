// probeGSE.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "probeGSE.h"
#include "navopt32.h"
#include "time.h"

#include "ccLibStaticLink.h"
#include "ccLibStd.h"

#define INITIIDS
#include "ccVerifyTrustStatic.h"
#include "ccVerifyTrustLibLink.h"

SYM_OBJECT_MAP_BEGIN()
SYM_OBJECT_MAP_END()

#include "ccSymDebugOutput.h"
ccSym::CDebugOutput g_DebugOutput(_T("probeGSE"));
IMPLEMENT_CCTRACE(::g_DebugOutput);

ccVerifyTrust::IVerifyTrustPtr g_pVerifyTrust = NULL;

// Use a marker since the time logged will be the same for all items in the same second
static unsigned long g_ulMarker = 0;

// Local helper function prototypes
BOOL CheckTrust();
void GetDataFileName(LPSTR, DWORD);
HNAVOPTS32 AllocateAndLoadFile(LPCSTR);
void RemoveOldestItem(HNAVOPTS32 hGSEDat, PNAVOPT32_VALUE_INFO pInfo, unsigned int unCount, LPCTSTR pcszVIDToBeAdded);

#define MAX_ITEMS 100

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
        // Performance Optimization: see MSDN docs on DisableThreadLibraryCalls() func.
        DisableThreadLibraryCalls((HMODULE)hModule);

        g_hMod = (HMODULE)hModule;
        break;

	case DLL_PROCESS_DETACH:
        if( g_pVerifyTrust )
        {
            g_pVerifyTrust->Destroy();
            g_pVerifyTrust.Release();
        }
		break;
	}
    return TRUE;
}

extern "C" __declspec(dllexport) BOOL WINAPI GSECheckVID(unsigned long ulVid)
{
    // Validate callers signature
    if( !CheckTrust() )
        return FALSE;

    BOOL bRet = FALSE;

    // Get the data file name
    char szFileName[MAX_PATH] = {0};
    GetDataFileName(szFileName, MAX_PATH);

    // If this file doesn't exist then we can exit now
    if( 0xFFFFFFFF == GetFileAttributes(szFileName) )
    {
        CCTRACEI("GSECheckVID() - No data file to check items for: %s", szFileName);
        return FALSE;
    }

    // Get the data
    HNAVOPTS32 hGSEDat = AllocateAndLoadFile(szFileName);

    if( NULL == hGSEDat )
        return FALSE;

    // Check for the VID passed in
    TCHAR szValueToCheck[64] = {0};
    _ultoa(ulVid, szValueToCheck, 10);
    DWORD dwExists = 0;
    if( NAVOPTS32_OK == NavOpts32_GetDwordValue(hGSEDat, szValueToCheck, &dwExists, 0) && dwExists != 0 )
        bRet = true;
    else
        bRet = false;

    // Free the data
    if( hGSEDat )
        NavOpts32_Free(hGSEDat);

    if( !bRet )
        CCTRACEI("GSECheckVID() - Did not find the VID %s.", szValueToCheck);
    else
        CCTRACEI("GSECheckVID() - Found the VID %s.", szValueToCheck);

	return bRet;
}

extern "C" __declspec(dllexport) BOOL WINAPI GSEAdd(unsigned long ulVid, const char* pcszDatFileToUse)
{
    BOOL bRet = FALSE;

    // Validate callers signature
    if( !CheckTrust() )
        return FALSE;

    // Get the data file to use
    char szFileName[MAX_PATH] = {0};

    if( NULL == pcszDatFileToUse )
        GetDataFileName(szFileName, MAX_PATH);
    else
        strncpy(szFileName, pcszDatFileToUse, MAX_PATH);

    // Get the data
    HNAVOPTS32 hGSEDat = AllocateAndLoadFile(szFileName);

    if( NULL == hGSEDat )
        return FALSE;

    // Does this item need to be added?
    bool bAdd = true;

    // Get the name of the item to add
    char szName[64] = {0};
    _ultoa(ulVid, szName, 10);

    // Make sure we haven't exceeded the max
    PNAVOPT32_VALUE_INFO pInfo = NULL;
    unsigned int unCount = 0;
    if( NAVOPTS32_OK == NavOpts32_GetValuesInfo( hGSEDat, &pInfo, &unCount ) && pInfo )
    {
        if( unCount >= MAX_ITEMS )
        {
            RemoveOldestItem(hGSEDat, pInfo, unCount, szName);
        }
    }
    else
    {
        CCTRACEE("GSEAdd() - Error: Unable to get the values info for file %s", szFileName);
    }

    // Free the data
    NavOpts32_FreeValuesInfo(pInfo);
    pInfo = NULL;

    // Now add the item with the current time as the value
    time_t CurrentTime;
	time(&CurrentTime);

    // As long as 100 items aren't stored within the same second we should be good
    unsigned long ulStoreVal = (unsigned long)CurrentTime*100 + g_ulMarker;

    if( NAVOPTS32_OK == NavOpts32_SetDwordValue(hGSEDat, szName, ulStoreVal) )
    {
        CCTRACEI("GSEAdd() - Successfully added: %s with value %u", szName, ulStoreVal);

        // Now save the change
        if( NAVOPTS32_OK == NavOpts32_Save(szFileName, hGSEDat) )
        {
            g_ulMarker++;
            bRet = TRUE;
        }
        else
            CCTRACEE("GSEAdd() - Error saving %s file", szFileName);
    }
    else
    {
        CCTRACEE("GSEAdd() - Error: Unable to add the value %u", CurrentTime);
    }

    // Free the data
    if( hGSEDat )
        NavOpts32_Free(hGSEDat);

    return bRet;
}

extern "C" __declspec(dllexport) BOOL WINAPI GSERemove(unsigned long ulVid)
{
    // Validate callers signature
    if( !CheckTrust() )
        return FALSE;

    BOOL bRet = FALSE;

    // Get the data file name
    char szFileName[MAX_PATH] = {0};
    GetDataFileName(szFileName, MAX_PATH);

    // If this file doesn't exist then we can exit now
    if( 0xFFFFFFFF == GetFileAttributes(szFileName) )
    {
        CCTRACEI("GSERemove() - No data file to remove values from: %s", szFileName);
        return FALSE;
    }

    // Get the data
    HNAVOPTS32 hGSEDat = AllocateAndLoadFile(szFileName);

    if( NULL == hGSEDat )
        return FALSE;

    // Get the value we want to remove
    TCHAR szValueToRemove[64] = {0};
    _ultoa(ulVid, szValueToRemove, 10);

    // Attempt to remove the item
    if( NAVOPTS32_OK != NavOpts32_RemoveValue(hGSEDat, szValueToRemove) )
    {
        CCTRACEE("GSERemove() - Could not remove item %s", szValueToRemove);
        bRet = FALSE;
    }
    else
    {
        CCTRACEI("GSERemove() - Successfully removed item %s", szValueToRemove);

        // Now save the changes
        if( NAVOPTS32_OK == NavOpts32_Save(szFileName, hGSEDat) )
        {
            bRet = TRUE;
        }
        else
        {
            CCTRACEE("GSERemove() - Could not save the file %s after removing %s", szFileName, szValueToRemove);
            bRet = FALSE;
        }
    }

    // Free the data
    if( hGSEDat )
        NavOpts32_Free(hGSEDat);

	return bRet;
}

BOOL CheckTrust()
{
    // Load the ccVerify trust dll if we don't have the object
    if( !g_pVerifyTrust )
    {
        // Create the object
        g_pVerifyTrust.m_p = ccVerifyTrust::CVerifyTrustStatic::CreateVerifyTrust();
        
        if( !g_pVerifyTrust )
        {
            CCTRACEE("CheckTrust() - Error getting trust object.");
            return FALSE;
        }

        if ( ccVerifyTrust::eNoError != g_pVerifyTrust->Create(true) )
	    {
            CCTRACEE("CheckTrust() - Error creating trust object.");
            return FALSE;
        }
    }

    // Check the current process for a Symantec digital signature
    if( ccVerifyTrust::eNoError != g_pVerifyTrust->VerifyCurrentProcess(ccVerifyTrust::eSymantecSignature) )
    {
        CCTRACEE("CheckTrust() - The current process does not have a valid digital signature.");
        return FALSE;
    }

    // Signature checked out
    return TRUE;
}

void GetDataFileName(LPSTR pszFileName, DWORD dwSize)
{
    // Get the virus defs directory
    HKEY hKey;
    if( ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("Software\\Symantec\\InstalledApps"), 0, KEY_QUERY_VALUE, &hKey) )
    {
        DWORD dwType;
        if( ERROR_SUCCESS == RegQueryValueEx(hKey, _T("AVENGEDEFS"), NULL, &dwType, reinterpret_cast<LPBYTE>(pszFileName), &dwSize) )
        {
            // Add trailing backslash if necessary
            if( pszFileName[strlen(pszFileName)-1] != '\\' )
                strcat(pszFileName, "\\");
        }
        else
            CCTRACEE("GetDataFileName() - Error querying avengedefs installed apps reg value.");

        RegCloseKey(hKey);
    }
    else
        CCTRACEE("GetDataFileName() - Error opening installed apps reg key.");

    strcat(pszFileName, "gse.dat");
}

HNAVOPTS32 AllocateAndLoadFile(LPCSTR pcszFile)
{
    // Allocate the data
    HNAVOPTS32 hGSEDat = NULL;
    if( NAVOPTS32_OK != NavOpts32_Allocate(&hGSEDat) )
    {
        CCTRACEE("AllocateAndLoadFile() - Failed to allocate the options primitive data");
        return NULL;
    }

    // Load the file if it already exists
    if( 0xFFFFFFFF != GetFileAttributes(pcszFile) )
    {
        // Try to synchronize loads using a mutex unique to the file being loaded
        TCHAR szMutexName[MAX_PATH*2] = {0};

        // Remove everything but the file name for the mutex name
        _splitpath(pcszFile, NULL, NULL, szMutexName, NULL);

        // Append a GUID to the mutex name
        strcat(szMutexName, _T("_PROBEGSE_{CE9694CE-C0F4-4119-B37E-A24723442CB5}"));

        //--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--
        ccLib::CMutex cMutex(NULL,FALSE,szMutexName,TRUE);
        ccLib::CSingleLock(&cMutex, INFINITE, FALSE);
	    //--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--

        NAVOPTS32_STATUS status = NavOpts32_Load(pcszFile, hGSEDat, TRUE);
        if( NAVOPTS32_OK == status )
        {
            CCTRACEI("AllocateAndLoadFile() - Successfully loaded the file %s.", pcszFile);
        }
        else if( NAVOPTS32_INVALID_FILE == status || NAVOPTS32_FILE_TAMPERED_WITH == status )
        {
            CCTRACEE("AllocateAndLoadFile() - The file %s is either invalid or tampered with the old data is lost.", pcszFile);
        }
        else
        {
            CCTRACEE("AllocateAndLoadFile() - Load for file %s failed with return value 0x%X. Bailing out.", pcszFile, status);
            NavOpts32_Free(hGSEDat);
            hGSEDat = NULL;
        }
    }
    else
    {
        CCTRACEI("AllocateAndLoadFile() - The file %s does not exist. Using new empty file.", pcszFile);
    }

    return hGSEDat;
}

void RemoveOldestItem(HNAVOPTS32 hGSEDat, PNAVOPT32_VALUE_INFO pInfo, unsigned int unCount, LPCTSTR pcszVIDToBeAdded)
{
    PNAVOPT32_VALUE_INFO pCurrent = NULL;

    if( pInfo != NULL )
    {
        char szValToRemove[65] = {0};
        DWORD dwMinVal = 0;
        // Walk the values looking for the oldest one
        for( unsigned int i = 0; i < unCount; i++ )
        {
            pCurrent = &pInfo[i];
            DWORD dwCurVal = 0;
            if( NAVOPTS32_OK == NavOpts32_GetDwordValue(hGSEDat, pCurrent->szValue, &dwCurVal, 0) )
            {
                if( 0 == strcmp(pcszVIDToBeAdded, pCurrent->szValue) )
                {
                    // This item was already here, overwrite the same VID with the updated value
                    strncpy(szValToRemove, pCurrent->szValue, 64);
                    break;
                }

                if( i == 0 )
                {
                    dwMinVal = dwCurVal;
                    strncpy(szValToRemove, pCurrent->szValue, 64);
                }

                if( dwCurVal < dwMinVal )
                {
                    // Save this value as the oldest one
                    dwMinVal = dwCurVal;
                    strncpy(szValToRemove, pCurrent->szValue, 64);
                }
            }
        }

        CCTRACEI("RemoveOldestItem() - The oldest VID (or already existing VID that is being added) is %s with value %u. Removing it.", szValToRemove, dwMinVal);
        if( NAVOPTS32_OK != NavOpts32_RemoveValue(hGSEDat, szValToRemove) )
            CCTRACEE("RemoveOldestItem() - Error: Failed to remove the VID %s.", szValToRemove);
    }
    else
        CCTRACEE("RemoveOldestItem() - Error: Invalid values info pointer");
}