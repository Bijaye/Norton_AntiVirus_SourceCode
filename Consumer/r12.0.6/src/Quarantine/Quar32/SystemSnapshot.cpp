//////////////////////////////////////////////////////////////////////////////
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2004 Symantec Corporation.
// All rights reserved.
//////////////////////////////////////////////////////////////////////////////
//
// SystemSnapshot.cpp
//

#include "stdafx.h"
#include "SystemSnapshot.h"
#include "SnapshotServerLoader.h"
#include "resource.h"

#include "ccSymMemoryStreamImpl.h"
#include "ccSymFileStreamImpl.h"

#include "ccExceptionInfo.h"
#include "ccCatch.h"

#include "QuarFileApi.h"
#include "NAVSettingsHelperEx.h"
#include "OptNames.h"



CSystemSnapshot::CSystemSnapshot(void)
{
}

CSystemSnapshot::~CSystemSnapshot(void)
{
}


HRESULT CSystemSnapshot::LoadDataFromResource(HINSTANCE hModule, UINT iResourceId, cc::IStream** ppStream)
{
    HRESULT hr = E_FAIL;

    HRSRC hResource = 0;

    hResource = ::FindResource( hModule, MAKEINTRESOURCE(iResourceId), _T("BIN") );
    ATLASSERT(hResource);

    if( hResource )
    {
        HGLOBAL hg = 0;

        // Load up the resource into an HGLOBAL
        DWORD dwSizeofResource = SizeofResource(hModule, hResource);
        hg = ::LoadResource( hModule, hResource );

        if( hg )
        {
            // Lock the resource, and get an LPVOID ptr to it
            LPVOID  lpResourceData = NULL;
            lpResourceData = ::LockResource( hg );

            cc::IStreamPtr pMemoryStream;
            pMemoryStream.Attach(ccSym::CMemoryStreamImpl::CreateMemoryStreamImpl());
            if(pMemoryStream)
            {
                if(pMemoryStream->Write(lpResourceData, dwSizeofResource))
                {
                    pMemoryStream->SetPosition(0);
                    hr = pMemoryStream->QueryInterface(cc::IID_Stream, (void**)ppStream);
                }
            }

            ::DeleteObject( hg );
        }
    }

    return hr;
} // END LoadDataFromResource


HRESULT CSystemSnapshot::GenerateSystemSnapshot(LPCSTR szOutputFile)
{
    cc::IStreamPtr pRules, pResults;
    ccSym::CFileStreamImplPtr pOutputFile;
    SnapServer_Loader cSDSnapDll;
    SnapServerSX::ISnapServerPtr pSnapServer;
    SYMRESULT sr;
    HRESULT hr, hrReturn = E_FAIL;
    bool bResult = FALSE;

    const DWORD dwTempBuffSize = 4096;
    LPBYTE pTempBuff = NULL;
    ULONGLONG qdwTotalSize = NULL;

    // Load up the compressed/encrypted command data from resources
    hr = LoadDataFromResource(g_hInstance, IDR_SDSNAPCMD_WINNT, &pRules);
    if(FAILED(hr))
    {
        hrReturn = E_FAIL;
        CCTRACEE("%s() - Unable to load SysSnap rule info.", __FUNCTION__);
        goto Exit_Function;
    }

    // Initialize the output file
    pOutputFile = new ccSym::CFileStreamImpl;
    if(!pOutputFile)
    {
        hrReturn = E_FAIL;
        CCTRACEE("%s() - Unable to allocate output pointer.", __FUNCTION__);
        goto Exit_Function;
    }

    if(!pOutputFile->GetFile().Open(szOutputFile, GENERIC_READ | GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL))
    {
        hrReturn = E_FAIL;
        CCTRACEE("%s() - Unable to init output file.", __FUNCTION__);
        goto Exit_Function;
    }

    // Find and load the SnapServer dll
    sr = cSDSnapDll.Initialize();
    if(SYM_FAILED(sr))
    {
        hrReturn = E_FAIL;
        CCTRACEE("%s() - Unable to load init SysSnap loader. (Initialize() returned: 0x%08X)", __FUNCTION__, sr);
        goto Exit_Function;
    }

    
    sr = cSDSnapDll.CreateObject(&pSnapServer);
    if(SYM_FAILED(sr))
    {
        hrReturn = E_FAIL;
        CCTRACEE("%s() - Unable to create SysSnap object. (CreateObject() returned: 0x%08X)", __FUNCTION__, sr);
        goto Exit_Function;
    }

    // initialize SnapServer
    hr = pSnapServer->Initialize(_T(""), NULL, &bResult);
    if(FAILED(hr) || !bResult)
    {
        hrReturn = E_FAIL;
        CCTRACEE("%s() - Unable to initialize SysSnap object. (Initialize() returned: 0x%08X, bResult == %d)", __FUNCTION__, hr, bResult);
        goto Exit_Function;
    }
    
    CCTRACEI("%s() - Calling GetSnapshot() on SysSnap object. ", __FUNCTION__);
    
    // Generate the snapshot into the specified stream
    hr = pSnapServer->GetSnapshot(pRules, &pResults, &bResult);
    if(FAILED(hr) || !bResult)
    {
        hrReturn = E_FAIL;
        CCTRACEE("%s() - Unable to generate Snapshot object. (GetSnapshot() returned: 0x%08X, bResult == %d)", __FUNCTION__, hr, bResult);
        goto Exit_Function;
    }
    
    CCTRACEI("%s() - Copying snapshot data buffer to file. ", __FUNCTION__);
    
    // Set up copy buffer
    if(pTempBuff)
        delete [] pTempBuff;
    pTempBuff = new BYTE[dwTempBuffSize];
    if(!pTempBuff)
    {
        hrReturn = E_OUTOFMEMORY;
        CCTRACEE("%s() - Unable to allocate temp buffer (requires %d bytes).", __FUNCTION__, dwTempBuffSize);
        goto Exit_Function;
    }

    // Copy Stream from Snapshot to our file
    pResults->SetPosition(0);
    pResults->GetSize(qdwTotalSize);
    while(qdwTotalSize)
    {
        DWORD dwBytesToCopy = min(qdwTotalSize, dwTempBuffSize);

        if(!pResults->Read(pTempBuff, dwBytesToCopy))
            break;

        if(!pOutputFile->Write(pTempBuff, dwBytesToCopy))
            break;

        qdwTotalSize -= dwBytesToCopy;
    }
    
    delete [] pTempBuff;
    pTempBuff = NULL;

    hrReturn = S_OK;


Exit_Function:
    CCTRACEI("%s() - Cleaning up. ", __FUNCTION__);

    // break it all down
    pResults.Release();
    pSnapServer.Release();
    pRules.Release();
    pOutputFile.Release();

    CCTRACEI("%s() - Returning. ", __FUNCTION__);
    return hrReturn;
} // END GenerateSystemSnapshot


STDMETHODIMP CSystemSnapshot::TakeSystemSnapshot(LPCTSTR szSnapShotPath)
{
	// Check NAV options to see if we should actually take the snapshot
	bool bAllowSnap = true;  // Defaulting to allowing the snapshot
	DWORD dwValue;
	CNAVOptSettingsEx NavOpts;

	try
	{
        // Quarantine operation can be occuring in OEM file mode but we must make
        // sure the settings manager is loaded in ANSI mode...
        CQuarFileAPI fileAPI;
        fileAPI.SwitchAPIToANSI();
        if(NavOpts.Init())
        {
			// Defaulting to allowing the snapshot
            if(FAILED(NavOpts.GetValue(QUARANTINE_QuarantineSnapShot, dwValue, 1)))
				bAllowSnap = true;
			else
				bAllowSnap = ((0 == dwValue) ? false : true);
        }
        else
            CCTRACEE("%s - Unable to initialize the options library.", __FUNCTION__);
        fileAPI.RestoreOriginalAPI();
	}
	catch(exception& Ex)
	{
        CCTRACEE(_T("%s - Caught Exception %s"), __FUNCTION__, Ex.what());
	}

	if(!bAllowSnap)
	{
		CCTRACEI(_T("%s - Setting %s indicates not to take snapshot"), __FUNCTION__, QUARANTINE_QuarantineSnapShot);
		return S_FALSE;
	}

//    ccLib::CExceptionInfo exceptionInfo(_T("calling CSystemSnapshot::GenerateSystemSnapshot() from CSystemSnapshot::TakeSystemSnapshot()"));
//    try
    {
        HRESULT hResult = GenerateSystemSnapshot(szSnapShotPath);
        if(FAILED(hResult))
        {
            CCTRACEE(_T("%s - Failed to take snapshot, hResult=0x%08X"), __FUNCTION__, hResult);
            return E_FAIL;
        }
    }
//    CCCATCH_ALL(exceptionInfo);

	return S_OK;
} // END TakeSystemSnapshot
