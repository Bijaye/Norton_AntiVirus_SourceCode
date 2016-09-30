// NAVOPTHelperEx.H
//
//////////////////////////////////////////////////////////////////////
#pragma once
#include "NavOptHelper.h"
#include <stdlib.h>
#include "StahlSoft.h"
#include "SyncHelper.h"
#include "StahlSoft_EventHelper.h"
#include "NAVInfo.h"

class CNAVOptFileEx:
	protected CNAVOptFile
{
public:

	CNAVOptFileEx():m_bInit(FALSE)
	{
	}
	virtual ~CNAVOptFileEx()
	{
	}

    //
    // Synchronized Init function.
    //
    // Clients can pass in any path to a navopts32 primitive dat file, by default
    // navopts.dat is loaded.
    // If clients pass in bBusinessRules as FALSE then the business model will not
    // be called to calculate the business rules.
    BOOL Init(LPCTSTR lpcOptionPath=NULL, BOOL bBusinessRules=TRUE)
	{
        if(m_bInit)
		{
			return TRUE;
		}
        else
        {
            // Create a mutex unique to the file being loaded
            TCHAR szMutexName[MAX_PATH*2] = {0};
            TCHAR szFilePath[MAX_PATH] = {0};
            _tcscpy(szFilePath, (lpcOptionPath ? lpcOptionPath : m_NAVInfo.GetNAVOptPath()));

            // Remove everything but the file name for the mutex name
            _tsplitpath(szFilePath, NULL, NULL, szMutexName, NULL);

            // Append a GUID to the mutex name
            _tcscat(szMutexName, _T("_CNAVOptFileEx_{182860C6-6167-44ff-90C0-DF6BF69B3E91}"));
            m_shMutex = StahlSoft::CreateMutexEx(NULL,FALSE,szMutexName,TRUE);
            if( (HANDLE)m_shMutex == NULL )
                 return FALSE;
            m_lockMutex.Attach(m_shMutex);
        }

        //--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--
		StahlSoft::CSmartLock smLock(&m_lockMutex);
	    //--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--

		m_bInit = CNAVOptFile::Init(lpcOptionPath);

	    return m_bInit;
	}
	void Cleanup()
    {
        CNAVOptFile::Cleanup();
        m_bInit = false;
    }

    //
    // Synchronized save function - the base class will merge and save the dat file
    // then the buisness rules will be calculated on the new option values
    //
    BOOL Save()
	{
        BOOL bSave = TRUE;
        StahlSoft::HRX hrx;

        try
        {
            //--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--
		    StahlSoft::CSmartLock smLock(&m_lockMutex);
	        //--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--

            // Merge and save the options
            bSave = CNAVOptFile::Save();
        }
        catch( _com_error &)
        {
            return FALSE;
        }

        return bSave;
    }

    // Pass through functions to the base class
    HRESULT GetValue(LPCSTR lpszName,DWORD& dwVal,DWORD dwDefault)
	{
		return CNAVOptFile::GetValue(lpszName,dwVal,dwDefault);
	}
    HRESULT GetValue(LPCSTR lpszName,LPSTR pszVal,DWORD dwSize,LPSTR szDefault)
    {
        return CNAVOptFile::GetValue(lpszName,pszVal,dwSize,szDefault);
    }
    HRESULT GetValue(LPCSTR lpszName,LPBYTE pBuf,DWORD dwSize)
    {
        return CNAVOptFile::GetValue(lpszName,pBuf,dwSize);
    }
    HRESULT SetValue(LPCSTR lpszName, DWORD dwVal)
	{
        return CNAVOptFile::SetValue(lpszName,dwVal);
	}
    HRESULT SetValue(LPCSTR lpszName, LPCSTR pszVal)
	{
        return CNAVOptFile::SetValue(lpszName,pszVal);
	}
    HRESULT SetValue(LPCSTR lpszName, LPBYTE pBuf, DWORD dwSize)
	{
        return CNAVOptFile::SetValue(lpszName,pBuf,dwSize);
	}
    HRESULT GetValuesInfo(PNAVOPT32_VALUE_INFO* ppInfo, unsigned int * pulCount)
    {
        return CNAVOptFile::GetValuesInfo(ppInfo,pulCount);
    }
    HRESULT FreeValuesInfo(PNAVOPT32_VALUE_INFO pInfo)
    {
        return CNAVOptFile::FreeValuesInfo(pInfo);
    }


private:
	StahlSoft::CMutexLock	m_lockMutex;
	StahlSoft::CSmartHandle m_shMutex;
    CNAVInfo m_NAVInfo;
	BOOL m_bInit;
};

#define READ_OPTION_START()                                 \
        {                                                   \
        StahlSoft::HRX hrx;                                 \
        CNAVOptFileEx navFeature;                           \
        hrx << (SUCCEEDED(navFeature.Init())?S_OK:E_FAIL);  \
		DWORD dwValue = 0 ;

#define READ_OPTION_bool(_name,_var,_default)                                       \
    _var = _default;                                                                \
    if(SUCCEEDED(navFeature.GetValue(_name,dwValue,static_cast<DWORD>(_default))))  \
        _var = (dwValue != 0)

#define READ_OPTION_DWORD(_name,_var,_default)                                      \
    _var = _default;                                                                \
    if(SUCCEEDED(navFeature.GetValue(_name,dwValue,static_cast<DWORD>(_default))))  \
        _var = dwValue;

#define READ_OPTION_long(_name,_var,_default)                                       \
    _var = _default;                                                                \
    if(SUCCEEDED(navFeature.GetValue(_name,dwValue,static_cast<DWORD>(_default))))  \
        _var = static_cast<long>(dwValue);

#define READ_OPTION_END }