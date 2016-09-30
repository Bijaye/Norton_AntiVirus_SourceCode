////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <ccSymInterfaceLoader.h>
#include <ccSymPathProvider.h>
#include "SrtControlInterface.h"

//NOTE: This file lifted from the AV Component

// This class is a "dumb" wrapper around the SRTSP protect\unprotect thread functionality.
// If you ask to unprotect it will unprotect the current thread, if you ask to protect
// it will protect the current thread.  It is dumb in that it doesn't presume to know
// how you are using it (i.e. match proctect\unprotect calls).  To get "smart" implementation
// use this class with the CSmartUnProtectThread class defined below.

namespace SRTSP {

#ifdef INITIIDS
extern LPCWSTR sz_SRTSP_dll = L"SRTSP32.dll";
extern LPCWSTR sz_SRTSP_AppName = L"SRTSP";
#else
extern LPCWSTR sz_SRTSP_dll;
extern LPCWSTR sz_SRTSP_AppName;
#endif

		struct CSRTSPPathProvider
		{
			static bool GetPath(LPTSTR szPath, size_t& nSize)
			{
				ccLib::CString sPath;
				if(ccSym::CInstalledApps::GetInstAppsDirectory(sz_SRTSP_AppName, sPath))
					return ccLib::CStringConvert::Copy(sPath, szPath, nSize);
				return false;
			}
		};

    	//unmanaged
    	typedef CSymInterfaceDLLHelper<&sz_SRTSP_dll, 
    							CSRTSPPathProvider, 
    							cc::CSymInterfaceTrustedCacheLoader, 
    							ISrtControl, 
    							&OBJID_SrtControl, 
    							&IID_SrtControl> SrtControl_Loader;


class CProtectThread
{
public:
    CProtectThread(void) throw()
    {
    }

    ~CProtectThread(void) throw()
    {
    }

    // Clients call this method to have Auto-Protect stop monitoring their thread.
    // After a successful return from this method clients must call ProtectProcess()
    // on the same thread to have Auto-Protect begin monitoring their thread again.
    HRESULT UnprotectThread()
    {
        HRESULT hr = LoadISrtControl();
		if(FAILED(hr))
		{
			CCTRCTXE1(_T("Failed to load SrtControl: 0x%08X"), hr);
			return hr;
		}

        hr = m_pControl->UnprotectThread();
		if(FAILED(hr))
		{
			CCTRCTXE1(_T("UnprotectThread() == 0x%08X"), hr);
		}
		return hr;

    }

    // Clients call this method to have Auto-Protect start monitoring their process\thread
    // again.  This should only be called from a thread that had a successful return from
    // the UnprotectProcess() method.
    HRESULT ProtectThread()
    {
        HRESULT hr = LoadISrtControl();
		if(FAILED(hr))
		{
			CCTRCTXE1(_T("Failed to load SrtControl: 0x%08X"), hr);
			return hr;
		}

        hr = m_pControl->ProtectThread();
		if(FAILED(hr))
		{
			CCTRCTXE1(_T("ProtectThread() == 0x%08X"), hr);
		}
		return hr;
    }

private:
	SRTSP::SrtControl_Loader m_Loader;
    ISrtControlPtr m_pControl;

    HRESULT LoadISrtControl()
    {
		HRESULT hr = S_OK;
		if(m_pControl != NULL)
			return hr;

		SYMRESULT sr = m_Loader.CreateObject(m_pControl);
		if(SYM_FAILED(sr))
		{
			hr = E_UNEXPECTED;
			CCTRCTXE2(_T("Failed to load SrtControl: 0x%08X (hr = 0x%08X)"), sr, hr);
		}
		else if(m_pControl == NULL)
		{
			CCTRCTXE0(_T("m_pControl == NULL"));
			hr = E_POINTER;
		}
		else
		{
			CCTRCTXI0(_T("Success"));
		}
		return hr;
	}
};

// This is a smart wrapper class to use an instantiated CProtectThread class to
// protect the current executing thread for the lifetime of the wrapper class. This class
// needs to be intstantiated and released on the same threat in order to work properly. You also
// need to ensure that the CProtectThread class passed into the constructor out-lives
// this class.
class CSmartUnProtectThread
{
public:
    CSmartUnProtectThread(CProtectThread* pProtectClass) throw() : m_pProtectProcess(pProtectClass),
                                                                        m_hrStatusUnProtect(S_FALSE)
    {
        if( m_pProtectProcess != NULL )
        {
            m_hrStatusUnProtect = m_pProtectProcess->UnprotectThread();
            if( SUCCEEDED(m_hrStatusUnProtect) )
                CCTRCTXI1(_T("ISrtControl::UnprotectThread() returned 0x%08X"), m_hrStatusUnProtect);
            else
                CCTRCTXE1(_T("ISrtControl::UnprotectThread() returned 0x%08X"), m_hrStatusUnProtect);
        }
    }

    ~CSmartUnProtectThread() throw()
    {
        if( S_OK == m_hrStatusUnProtect && m_pProtectProcess != NULL)
        {
            HRESULT hr = m_pProtectProcess->ProtectThread();
			if(FAILED(hr))
				CCTRCTXE1(_T("ProtectThread() == 0x%08X"), hr);
			else
				CCTRCTXI1(_T("ProtectThread() == 0x%08X"), hr);

            m_hrStatusUnProtect = S_FALSE;
            m_pProtectProcess = NULL;
        }
		else
			CCTRCTXW1(_T("not protecting thread: 0x%08X"), m_hrStatusUnProtect);
    }

private:
    CSmartUnProtectThread(); // disallowed

    CProtectThread* m_pProtectProcess;
    HRESULT m_hrStatusUnProtect;
};

} //namespace SRTSP