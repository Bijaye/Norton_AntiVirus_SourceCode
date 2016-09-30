// NAVOPTHelper.H
//
//////////////////////////////////////////////////////////////////////

#if !defined(_NAVOPTHELPER_H__7F0BD274_CEFC_49bb_B3C8_2AC18FA8E730__INCLUDED_)
#define _NAVOPTHELPER_H__7F0BD274_CEFC_49bb_B3C8_2AC18FA8E730__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "navopt32.h" 
#include "NAVInfo.h"
#include "StahlSoft.h"

class CNAVOptFile
{
public:

    CNAVOptFile() : m_hOptions(NULL)
    {
        ::ZeroMemory(m_szFileName, sizeof(m_szFileName));
    }

	virtual ~CNAVOptFile()
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
    BOOL Init(LPCTSTR lpcOptionPath=NULL)
    {
        StahlSoft::HRX hrx;
        BOOL bSuccess = TRUE;
        
        try
        {
            if(m_hOptions == NULL)
            {
                
                // Allocates a HNAVOPTS32 object
                
                NAVOPTS32_STATUS Status = NavOpts32_Allocate(&m_hOptions);
                hrx << ((Status != NAVOPTS32_OK) ? E_FAIL : S_OK);

                // Save the file name we are loading from to allow merging to the same file
                // during saves
                _tcsncpy(m_szFileName, ((lpcOptionPath==NULL)?m_NAVInfo.GetNAVOptPath():lpcOptionPath), MAX_PATH);
                
                // Load options data from a file.
                
#ifdef _UNICODE
                Status = NavOpts32_LoadU(m_szFileName, m_hOptions, TRUE);
#else
                Status = NavOpts32_Load( m_szFileName, m_hOptions, TRUE);
#endif
                
                hrx << ((Status != NAVOPTS32_OK) ? E_FAIL : S_OK);
            }
        }
        catch(_com_error&)  // hrx throws _com_errors;
        {
            // Deallocate the member variables on a failure
            if( m_hOptions )
            {
                NavOpts32_Free(m_hOptions);
                m_hOptions = NULL;
            }

            ::ZeroMemory(m_szFileName, sizeof(m_szFileName));

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
        if(m_hOptions)
            NavOpts32_Free(m_hOptions);
        m_hOptions = NULL;

        ::ZeroMemory(m_szFileName, sizeof(m_szFileName));
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
        catch(_com_error&)  // hrx throws _com_errors;
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
    HRESULT GetValue(LPCSTR lpszName,DWORD& dwVal,DWORD dwDefault)
    {
        StahlSoft::HRX hrx;
        HRESULT hr = S_OK;
        
        try
        {
            NAVOPTS32_STATUS Status = NAVOPTS32_OK;

            hrx << (lpszName ? S_OK : E_INVALIDARG);
            hrx << ((m_hOptions!=NULL) ? S_OK : E_POINTER);
            
            hrx << ((Init() == FALSE)?E_FAIL:S_OK);
            Status = NavOpts32_GetDwordValue(m_hOptions, lpszName, &dwVal, dwDefault);
            hrx << ((Status != NAVOPTS32_OK) ? E_FAIL : S_OK);
        }
        catch(_com_error&)  // hrx throws _com_errors;
        {
            hr = E_UNEXPECTED;
        }
        return hr;
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
            
            hrx << ((Init() == FALSE)?E_FAIL:S_OK);
            Status = NavOpts32_GetStringValue(m_hOptions, lpszName, pszVal, dwSize, szDefault);
            hrx << ((Status != NAVOPTS32_OK) ? E_FAIL : S_OK);
        }
        catch(_com_error&)  // hrx throws _com_errors;
        {
            hr = E_UNEXPECTED;
        }
        return hr;
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
            
            hrx << ((Init() == FALSE)?E_FAIL:S_OK);
            Status = NavOpts32_GetBinaryValue(m_hOptions, lpszName, pBuf, dwSize);
            hrx << ((Status != NAVOPTS32_OK) ? E_FAIL : S_OK);
        }
        catch(_com_error&)  // hrx throws _com_errors;
        {
            hr = E_UNEXPECTED;
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
        catch(_com_error&)  // hrx throws _com_errors;
        {
            hr = E_UNEXPECTED;
        }
        return hr;
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
        catch(_com_error&)  // hrx throws _com_errors;
        {
            hr = E_UNEXPECTED;
        }
        return hr;
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
        catch(_com_error&)  // hrx throws _com_errors;
        {
            hr = E_UNEXPECTED;
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
        catch(_com_error&)  // hrx throws _com_errors;
        {
            hr = E_UNEXPECTED;
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
        catch(_com_error&)  // hrx throws _com_errors;
        {
            hr = E_UNEXPECTED;
        }
        return hr;
    }

private:
	HNAVOPTS32 m_hOptions;
	CNAVInfo m_NAVInfo;
    TCHAR m_szFileName[MAX_PATH];
};

#endif // !defined(_NAVOPTHELPER_H__7F0BD274_CEFC_49bb_B3C8_2AC18FA8E730__INCLUDED_)
