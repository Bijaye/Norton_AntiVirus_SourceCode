////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// OptHelper.h
#pragma once

#include "navopt32.h" 
#include "StahlSoft.h"
#include "UnicodeSupport.h"
#include <atlconv.h>

class COptFile
{
public:

    COptFile() : m_hOptions(NULL)
    {
        ::ZeroMemory(m_szFileName, sizeof(m_szFileName));
    }

	virtual ~COptFile()
    {	
	    Cleanup();	
    }

    /////////////////////////////////////////////////////////////////////////////
    //
    // Function:	Init
    //
    // Description:	Obtain a handle to navopts.dat.
    //
    /////////////////////////////////////////////////////////////////////////////
    BOOL Init(LPCTSTR lpcOptionPath)
    {
        StahlSoft::HRX hrx;
        BOOL bSuccess = TRUE;
        
        try
        {
			hrx << ((lpcOptionPath == NULL) ? E_POINTER : S_OK);

            if(m_hOptions == NULL)
            {
                
                // Allocates a HNAVOPTS32 object
                
                NAVOPTS32_STATUS Status = NavOpts32_Allocate(&m_hOptions);
                hrx << ((Status != NAVOPTS32_OK) ? E_FAIL : S_OK);

                // Save the file name we are loading from to allow merging to the same file
                // during saves
                _tcscpy(m_szFileName, lpcOptionPath);
                
                // Load options data from a file.
                
#ifdef _UNICODE
                Status = NavOpts32_LoadU(m_szFileName, m_hOptions, TRUE);
#else
                Status = NavOpts32_Load( m_szFileName, m_hOptions, TRUE);
#endif
                
                hrx << ((Status != NAVOPTS32_OK) ? E_FAIL : S_OK);
            }
        }
        catch(_com_error& )
        {
            bSuccess = FALSE;
        }
        
        return bSuccess;
    }
    /////////////////////////////////////////////////////////////////////////////
    //
    // Function:	Cleanup
    //
    // Description:	Release the handle to navopts.dat.
    //
    /////////////////////////////////////////////////////////////////////////////
    void Cleanup()
    {
        try
        {
            if(m_hOptions)
                NavOpts32_Free(m_hOptions);
            m_hOptions = NULL;
        }
        catch(...)
        {
        }
    }

    /////////////////////////////////////////////////////////////////////////////
    //
    // Function:	Save
    //
    // Description:	Merge and save the in-memory options to the dat file
    //
    /////////////////////////////////////////////////////////////////////////////
    BOOL Save()
    {
        StahlSoft::HRX hrx;
        BOOL bSuccess = TRUE;
        
        try
        {
            NAVOPTS32_STATUS Status = NAVOPTS32_OK;

#ifdef _UNICODE
            Status = NavOpts32_MergeU(&m_hOptions, m_szFileName);
            hrx << ((Status != NAVOPTS32_OK) ? E_FAIL : S_OK);
            Status = NavOpts32_SaveU(m_szFileName, m_hOptions);
#else
            Status = NavOpts32_Merge(&m_hOptions, m_szFileName);
            hrx << ((Status != NAVOPTS32_OK) ? E_FAIL : S_OK);
            Status = NavOpts32_Save( m_szFileName, m_hOptions);
#endif
                
            hrx << ((Status != NAVOPTS32_OK) ? E_FAIL : S_OK);
        }
        catch(_com_error& /*e*/)
        {
            bSuccess = FALSE;
        }
        
        return bSuccess;
    }
    
    /////////////////////////////////////////////////////////////////////////////
    //
    // Function:	GetValue
    //
    // Description:	Read value data of the given name in navopts.dat
    //				Return S_OK if succeeded otherwize E_
    //
    /////////////////////////////////////////////////////////////////////////////
	HRESULT GetValue(LPCWSTR lpszName,DWORD& dwVal,DWORD dwDefault)
	{
		return this->GetValue(ATL::CW2AEX<>(lpszName), dwVal, dwDefault);
	}
    HRESULT GetValue(LPCSTR lpszName,DWORD& dwVal,DWORD dwDefault)
    {
        StahlSoft::HRX hrx;
        HRESULT hr = S_OK;
        
        try
        {
            NAVOPTS32_STATUS Status = NAVOPTS32_OK;

            hrx << (lpszName ? S_OK : E_INVALIDARG);
            hrx << ((m_hOptions!=NULL) ? S_OK : E_POINTER);
            
            hrx << ((Init(m_szFileName) == FALSE)?E_FAIL:S_OK);
            Status = NavOpts32_GetDwordValue(m_hOptions, lpszName, &dwVal, dwDefault);
            hrx << ((Status != NAVOPTS32_OK) ? E_FAIL : S_OK);
        }
        catch(_com_error& e)
        {
            hr = e.Error();
        }
        return hr;
    }
	HRESULT GetValue(LPCWSTR lpszName,LPWSTR pszVal,DWORD dwSize,LPCWSTR szDefault)
	{
		return this->GetValue(ATL::CW2AEX<>(lpszName), unicode::AutoBuffer(pszVal, dwSize), dwSize, ATL::CW2AEX<>(szDefault));
	}
	HRESULT GetValue(LPCSTR lpszName,LPWSTR pszVal,DWORD dwSize,LPCWSTR szDefault)
	{
		return this->GetValue(lpszName, unicode::AutoBuffer(pszVal, dwSize), dwSize, ATL::CW2AEX<>(szDefault));
	}
    HRESULT GetValue(LPCSTR lpszName,LPSTR pszVal,DWORD dwSize,LPSTR szDefault)
    {
        StahlSoft::HRX hrx;
        HRESULT hr = S_OK;
        
        try
        {
            NAVOPTS32_STATUS Status = NAVOPTS32_OK;

            hrx << (lpszName ? S_OK : E_INVALIDARG);
            hrx << ((m_hOptions!=NULL) ? S_OK : E_POINTER);
            hrx << ((pszVal!=NULL) ? S_OK : E_INVALIDARG);
            
            hrx << ((Init(m_szFileName) == FALSE)?E_FAIL:S_OK);
            Status = NavOpts32_GetStringValue(m_hOptions, lpszName, pszVal, dwSize, szDefault);
            hrx << ((Status != NAVOPTS32_OK) ? E_FAIL : S_OK);
        }
        catch(_com_error& e)
        {
            hr = e.Error();
        }
        return hr;
    }
	HRESULT GetValue(LPCWSTR lpszName,LPBYTE pBuf,DWORD dwSize)
	{
		return this->GetValue(ATL::CW2AEX<>(lpszName), pBuf, dwSize);
	}
    HRESULT GetValue(LPCSTR lpszName,LPBYTE pBuf,DWORD dwSize)
    {
        StahlSoft::HRX hrx;
        HRESULT hr = S_OK;
        
        try
        {
            NAVOPTS32_STATUS Status = NAVOPTS32_OK;

            hrx << (lpszName ? S_OK : E_INVALIDARG);
            hrx << ((m_hOptions!=NULL) ? S_OK : E_POINTER);
            hrx << ((pBuf!=NULL) ? S_OK : E_INVALIDARG);
            
            hrx << ((Init(m_szFileName) == FALSE)?E_FAIL:S_OK);
            Status = NavOpts32_GetBinaryValue(m_hOptions, lpszName, pBuf, dwSize);
            hrx << ((Status != NAVOPTS32_OK) ? E_FAIL : S_OK);
        }
        catch(_com_error& e)
        {
            hr = e.Error();
        }
        return hr;
    }
    
    /////////////////////////////////////////////////////////////////////////////
    //
    // Function:	SetValue
    //
    // Description:	Set the value data of a value
    //				Return S_OK if succeeded otherwize E_
    //
    /////////////////////////////////////////////////////////////////////////////
	HRESULT SetValue(LPCWSTR lpszName, DWORD dwVal)
	{
		return this->SetValue(ATL::CW2AEX<>(lpszName), dwVal);
	}
    HRESULT SetValue(LPCSTR lpszName, DWORD dwVal)
    {
        StahlSoft::HRX hrx;
        HRESULT hr = S_OK;
        
        try
        {
            NAVOPTS32_STATUS Status = NAVOPTS32_OK;

            hrx << (lpszName ? S_OK : E_INVALIDARG);
            hrx << ((m_hOptions!=NULL) ? S_OK : E_POINTER);
            
            
            Status = NavOpts32_SetDwordValue(m_hOptions, lpszName, dwVal);
            hrx << ((Status != NAVOPTS32_OK) ? E_FAIL : S_OK);
        }
        catch(_com_error& e)
        {
            hr = e.Error();
        }
        return hr;
    }

	HRESULT SetValue(LPCWSTR lpszName, LPCWSTR pszVal)
	{
		return this->SetValue(ATL::CW2AEX<>(lpszName), ATL::CW2AEX<>(pszVal));
	}
    HRESULT SetValue(LPCSTR lpszName, LPCSTR pszVal)
    {
        StahlSoft::HRX hrx;
        HRESULT hr = S_OK;
        
        try
        {
            NAVOPTS32_STATUS Status = NAVOPTS32_OK;

            hrx << (lpszName ? S_OK : E_INVALIDARG);
            hrx << ((m_hOptions!=NULL) ? S_OK : E_POINTER);
            hrx << ((pszVal!=NULL) ? S_OK : E_INVALIDARG);
            
            
            Status = NavOpts32_SetStringValue(m_hOptions, lpszName, pszVal);
            hrx << ((Status != NAVOPTS32_OK) ? E_FAIL : S_OK);
        }
        catch(_com_error& e)
        {
            hr = e.Error();
        }
        return hr;
    }

	HRESULT SetValue(LPCWSTR lpszName, LPBYTE pBuf, DWORD dwSize)
	{
		return this->SetValue(ATL::CW2AEX<>(lpszName), pBuf, dwSize);
	}
    HRESULT SetValue(LPCSTR lpszName, LPBYTE pBuf, DWORD dwSize)
    {
        StahlSoft::HRX hrx;
        HRESULT hr = S_OK;
        
        try
        {
            NAVOPTS32_STATUS Status = NAVOPTS32_OK;

            hrx << (lpszName ? S_OK : E_INVALIDARG);
            hrx << ((m_hOptions!=NULL) ? S_OK : E_POINTER);
            hrx << ((pBuf!=NULL) ? S_OK : E_INVALIDARG);
            
            
            Status = NavOpts32_SetBinaryValue(m_hOptions, lpszName, pBuf, dwSize);
            hrx << ((Status != NAVOPTS32_OK) ? E_FAIL : S_OK);
        }
        catch(_com_error& e)
        {
            hr = e.Error();
        }
        return hr;
    }

    /////////////////////////////////////////////////////////////////////////////
    //
    // Function:	GetValuesInfo
    //
    // Description:	Obtain a handle to a values info structure for the loaded
    //              options structure. The client is responsible to call the
    //              free function to free this data
    //
    /////////////////////////////////////////////////////////////////////////////
    HRESULT GetValuesInfo(PNAVOPT32_VALUE_INFO* ppInfo, unsigned int * pulCount)
    {
        StahlSoft::HRX hrx;
        HRESULT hr = S_OK;
        
        try
        {
            NAVOPTS32_STATUS Status = NAVOPTS32_OK;

            hrx << (ppInfo ? S_OK : E_INVALIDARG);
            hrx << (&ppInfo ? S_OK : E_INVALIDARG);
            hrx << ((m_hOptions!=NULL) ? S_OK : E_POINTER);
            hrx << ((pulCount!=NULL) ? S_OK : E_INVALIDARG);
            
            
            Status = NavOpts32_GetValuesInfo( m_hOptions, ppInfo, pulCount );
            hrx << ((Status != NAVOPTS32_OK) ? E_FAIL : S_OK);
        }
        catch(_com_error& e)
        {
            hr = e.Error();
        }
        return hr;
    }

    /////////////////////////////////////////////////////////////////////////////
    //
    // Function:	FreeValuesInfo
    //
    // Description:	free a handle to a values info structure for the loaded
    //              options structure.
    //
    /////////////////////////////////////////////////////////////////////////////
    HRESULT FreeValuesInfo(PNAVOPT32_VALUE_INFO pInfo)
    {
        StahlSoft::HRX hrx;
        HRESULT hr = S_OK;
        
        try
        {
            NAVOPTS32_STATUS Status = NAVOPTS32_OK;

            hrx << (pInfo ? S_OK : E_INVALIDARG);
            hrx << ((m_hOptions!=NULL) ? S_OK : E_POINTER);
            
            
            Status = NavOpts32_FreeValuesInfo(pInfo);
            hrx << ((Status != NAVOPTS32_OK) ? E_FAIL : S_OK);
        }
        catch(_com_error& e)
        {
            hr = e.Error();
        }
        return hr;
    }

private:
	HNAVOPTS32 m_hOptions;
    TCHAR m_szFileName[MAX_PATH];
};