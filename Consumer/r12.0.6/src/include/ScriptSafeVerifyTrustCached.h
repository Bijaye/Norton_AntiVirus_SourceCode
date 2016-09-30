// /////////////////////////////////////////////////////////////////////////////////////////////////
// Symantec copyright header start
// //////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2004 Symantec Corporation.
// All rights reserved.
//
// /////////////////////////////////////////////////////////////////////////////////////////////////
// Symantec copyright header stop
// //////////////////////
//
// ScriptSafeVerifyTrustCached.h
//
// Derive from this class to force embedded objects to run only from
// the installed directory (or below)
//
// The script must call "IsItSafe()" to determine if the caller is authorized to use
// object
//
//

#ifndef SCRIPTSAFE_H
#define SCRIPTSAFE_H

#include <exdisp.h>
#include <Wininet.h>
#include "symscriptsafe_h.h"
#include <COMDEF.H>
#include "ccVerifyTrustLoader.h"
#include "ccSymInstalledApps.h"

//  mlo 2001.09.05: redefine Reg calls to SymReg calls
//#include "RegToSymReg.h"

#pragma comment(lib, "Wininet.lib")

template<class T>
class CScriptSafe : 
    public IOleObject,
    public ISymScriptSafe,
    public IObjectWithSiteImpl<T>
{
private:
    BOOL    bSafe;

    BOOL lookupDLLFullPath(LPTSTR szFile, LPTSTR szFull, DWORD dwBufLen);
    BOOL testFileExists(LPCTSTR szPath);
    void appendPath(LPTSTR szDir, LPTSTR szFile);

public:
    CScriptSafe():bSafe(FALSE) {}
    ~CScriptSafe()
    {
        m_pVerifyTrust.Release();
        m_pCachedVerifyTrust.Release();
    }

    BOOL IsItSafe() {return bSafe;};

    // ISymScriptSafe
    STDMETHODIMP SetAccess(DWORD dwAccess, DWORD dwKey)
    {
        if(dwAccess == ISYMSCRIPTSAFE_UNLOCK_KEYA && dwKey == ISYMSCRIPTSAFE_UNLOCK_KEYB)
            bSafe = TRUE;
        else
            bSafe = FALSE;

        return S_OK;
    }
    
    // IOleObject
    virtual HRESULT __stdcall SetClientSite(IOleClientSite* pClientSite)
    {
        HRESULT hr;

        if (pClientSite)
        {
            IServiceProvider *pISP;
            hr = pClientSite->QueryInterface(IID_IServiceProvider, (void **) &pISP);
            if (SUCCEEDED(hr))
            {
                IWebBrowserApp *pIWebBrowser;
                hr = pISP->QueryService(IID_IWebBrowserApp, IID_IWebBrowser2, (void **) &pIWebBrowser);
                if (SUCCEEDED(hr))
                {
                    CComBSTR bstrLocURL;
                    hr = pIWebBrowser->get_LocationURL(&bstrLocURL);
                    if (SUCCEEDED(hr))
                    {
                        _bstr_t bstrLoc = bstrLocURL;
                        TCHAR lpszUrlPath[MAX_PATH];
                        DWORD dwLength = sizeof(lpszUrlPath);
                        
                        BOOL bRet = InternetCanonicalizeUrl(bstrLoc, lpszUrlPath, &dwLength, ICU_DECODE | ICU_NO_ENCODE);
                        if(bRet)
                        {
                            TCHAR* pszPath = _tcsstr(lpszUrlPath, _T("://"));
                            if(pszPath)
                            {
                                pszPath += 3;

                                // If there is an HTML or JS filename tacked onto the path, then remove it.
                                TCHAR *pszFile = _tcsrchr(pszPath, _T('/'));

                                while(pszFile != NULL)
                                {
                                    *pszFile = _T('\0');

                                    pszFile = _tcsrchr(pszPath, _T('/'));
                                }

                                // Need to allow blank strings as valid paths to fix a bug that occurs
                                // only on Korean XP
                                if('\0' != pszPath[0])
                                {
                                    // If this is not a full path, get the CWD and build a path...
                                    TCHAR lpszFullPath[MAX_PATH] = {0};
                                    if(pszPath[1] != _T(':'))
                                    {
                                        if(lookupDLLFullPath(pszPath, lpszFullPath, MAX_PATH))
                                            pszPath = lpszFullPath;
                                    }

                                    bSafe = VerifySymantecImage(pszPath);
                                }
                                else
                                    bSafe = TRUE;
                            }
                        }
                    }
                    pIWebBrowser->Release();
                }
                pISP->Release();
            }
        }

        return S_OK;
    }
    HRESULT __stdcall GetClientSite(IOleClientSite **ppClientSite)
    {
        return E_NOTIMPL;
    }

    HRESULT __stdcall SetHostNames(LPCOLESTR szContainerApp, LPCOLESTR szContainerObj)
    {
        return E_NOTIMPL;
    }

    HRESULT __stdcall Close(DWORD dwSaveOption)
    {
        return E_NOTIMPL;
    }

    HRESULT __stdcall SetMoniker(DWORD dwWhichMoniker, IMoniker *pmk)
    {
        return E_NOTIMPL;
    }

    HRESULT __stdcall GetMoniker(DWORD dwAssign, DWORD dwWhichMoniker, IMoniker **ppmk)
    {
        return E_NOTIMPL;
    }

    HRESULT __stdcall InitFromData(IDataObject *pDataObject, BOOL fCreation, DWORD dwReserved)
    {
        return E_NOTIMPL;
    }

    HRESULT __stdcall GetClipboardData(DWORD dwReserved, IDataObject **ppDataObject)
    {
        return E_NOTIMPL;
    }

    HRESULT __stdcall DoVerb(LONG iVerb, LPMSG lpmsg, IOleClientSite *pActiveSite, LONG lindex,
        HWND hwndParent, LPCRECT lprcPosRect)
    {
        return E_NOTIMPL;
    }
        
    HRESULT __stdcall EnumVerbs(IEnumOLEVERB** ppEnumOleVerb)
    {
        return E_NOTIMPL;
    }

    HRESULT __stdcall Update()
    {
        return E_NOTIMPL;
    }

    HRESULT __stdcall IsUpToDate()
    {
        return E_NOTIMPL;
    }

    HRESULT __stdcall GetUserClassID(CLSID* pClsid)
    {
        return E_NOTIMPL;
    }

    HRESULT __stdcall GetUserType(DWORD dwFormOfType, LPOLESTR *pszUserType)
    {
        return E_NOTIMPL;
    }

    HRESULT __stdcall SetExtent(DWORD dwDrawAspect, SIZEL *psizel)
    {
        return E_NOTIMPL;
    }

    HRESULT __stdcall GetExtent(DWORD dwDrawAspect, SIZEL *psizel)
    {
        return E_NOTIMPL;
    }

    HRESULT __stdcall Advise(IAdviseSink *pAdvSink, DWORD *pdwConnection)
    {
        return E_NOTIMPL;
    }

    HRESULT __stdcall Unadvise(DWORD dwConnection)
    {
        return E_NOTIMPL;
    }

    HRESULT __stdcall EnumAdvise(IEnumSTATDATA **ppenumAdvise)
    {
        return E_NOTIMPL;
    }

    HRESULT __stdcall GetMiscStatus(DWORD dwAspect, DWORD* pdwStatus)
    {
        return E_NOTIMPL;
    }

    HRESULT __stdcall SetColorScheme(LOGPALETTE *pLogpal)
    {
        return E_NOTIMPL;
    }

    // IObjectWithSite
    HRESULT __stdcall SetSite(IUnknown* pUnkSite)
    {
        USES_CONVERSION;
        HRESULT hr;
        TCHAR lpszUrlPath[MAX_PATH];
        DWORD dwLength = sizeof(lpszUrlPath);

        if (FAILED(hr = IObjectWithSiteImpl<T>::SetSite(pUnkSite)))
            return hr;

        if (!pUnkSite)
            return S_OK;

        CComPtr<IServiceProvider> spISP;
        if (FAILED(hr = pUnkSite->QueryInterface(&spISP)))
            return S_OK;

        CComPtr<IWebBrowserApp> spIWebBrowser;
        if (FAILED(hr = spISP->QueryService(IID_IWebBrowserApp, IID_IWebBrowser2, (void **) &spIWebBrowser)))
            return S_OK;

        CComBSTR bstrLocURL;
        if (FAILED(hr = spIWebBrowser->get_LocationURL(&bstrLocURL)))
            return hr;

        if(InternetCanonicalizeUrl(W2T(bstrLocURL), lpszUrlPath, &dwLength, ICU_DECODE | ICU_NO_ENCODE))
        {
            TCHAR* pszPath = _tcsstr(lpszUrlPath, _T("://"));
            if(pszPath)
            {
                pszPath += 3;

                // If there is an HTML or JS filename tacked onto the path, then remove it.
                TCHAR *pszFile = _tcsrchr(pszPath, _T('/'));

                while(pszFile != NULL)
                {
                    *pszFile = _T('\0');

                    pszFile = _tcsrchr(pszPath, _T('/'));
                }

                // Need to allow blank strings as valid paths to fix a bug that occurs
                // only on Korean XP
                if('\0' != pszPath[0])
                {
                    // If this is not a full path, get the CWD and build a path...
                    TCHAR lpszFullPath[MAX_PATH] = {0};
                    if(pszPath[1] != _T(':'))
                    {
                        if(lookupDLLFullPath(pszPath, lpszFullPath, MAX_PATH))
                            pszPath = lpszFullPath;
                    }

                    bSafe = VerifySymantecImage(pszPath);
                }
                else
                    bSafe = TRUE;
            }
        }

        return S_OK;
    }

protected:
    void InitilizeVerifyTrust()
    {
        if (m_pCachedVerifyTrust == NULL)
        {
            if (m_pVerifyTrust != NULL)
            {
                m_pVerifyTrust.Release();
                m_pVerifyTrust = NULL;
            }

            if (SYM_FAILED(m_CachedVerifyTrustLoader.CreateObject(m_pCachedVerifyTrust.m_p)) ||
                m_pCachedVerifyTrust == NULL)
                return;
        }

        if (m_pVerifyTrust == NULL)
        {
            // query the interface.
            m_pVerifyTrust = m_pCachedVerifyTrust;
            if (m_pVerifyTrust && m_pVerifyTrust->Create( false ) != ccVerifyTrust::eNoError)
                m_pVerifyTrust = NULL;
        }
    }
    BOOL VerifySymantecImage(LPCTSTR szFile)
    {
        InitilizeVerifyTrust();
        if (m_pVerifyTrust == NULL)
        {
            return TRUE;
        }
        if (m_pVerifyTrust->VerifyFile(szFile, ccVerifyTrust::eSymantecSignature) != ccVerifyTrust::eNoError)
        {
            return FALSE;
        }
        return TRUE;
    }

protected:
    ccVerifyTrust::ccVerifyTrust_ICachedVerifyTrust m_CachedVerifyTrustLoader;
    ccVerifyTrust::ICachedVerifyTrustPtr m_pCachedVerifyTrust;
    ccVerifyTrust::IVerifyTrustQIPtr m_pVerifyTrust;
};

template <class T>
BOOL CScriptSafe<T>::lookupDLLFullPath(LPTSTR szFile, LPTSTR szFull, DWORD dwBufLen)
{
    BOOL bRet = FALSE;

    // If there is a path associated with the file, then use it
    TCHAR *p = _tcsrchr(szFile, '\\');

    if(NULL != p)
    {
        _tcscpy(szFull, szFile);
        return TRUE;
    }

    // Do path lookup
    
    // First check the system dir
    CAtlString sFullPath;
    TCHAR szFullPath[MAX_PATH+1] = {0};
    DWORD dwSize = MAX_PATH;

    if(0 != GetSystemDirectory(szFullPath, MAX_PATH))
    {   
        appendPath(szFullPath, szFile);
        bRet = testFileExists(szFullPath);
    }

    // Check the Common Files directory
    if(!bRet)
    {
        sFullPath.Empty();
        szFullPath[0] = '\0';
        dwSize = MAX_PATH;

        if (ccSym::CInstalledApps::GetCCDirectory(sFullPath))
        {
            _tcsncpy(szFullPath, sFullPath, dwSize);
            appendPath(szFullPath, szFile);
            bRet = testFileExists(szFullPath);
        }
    }

    // Check the NAV directory
    if(!bRet)
    {
        sFullPath.Empty();
        szFullPath[0] = '\0';
        dwSize = MAX_PATH;

        if (ccSym::CInstalledApps::GetNAVDirectory(sFullPath))
        {
            _tcsncpy(szFullPath, sFullPath, dwSize);
            appendPath(szFullPath, szFile);
            bRet = testFileExists(szFullPath);
        }
    }

    // Check the NIS directory
    if(!bRet)
    {
        sFullPath.Empty();
        szFullPath[0] = '\0';
        dwSize = MAX_PATH;

        if (ccSym::CInstalledApps::GetNISDirectory(sFullPath))
        {
            _tcsncpy(szFullPath, sFullPath, dwSize);
            appendPath(szFullPath, szFile);
            bRet = testFileExists(szFullPath);
        }
    }

    if(bRet)
        _tcsncpy(szFull, szFullPath, dwBufLen);

    return bRet;
}

template <class T>
BOOL CScriptSafe<T>::testFileExists(LPCTSTR szPath)
{
    return ::GetFileAttributes( szPath ) != 0xFFFFFFFF;
}

template <class T>
void CScriptSafe<T>::appendPath(LPTSTR szDir, LPTSTR szFile)
{
    TCHAR *p = szDir;
    TCHAR *pLast = NULL;

    while( *p )
    {
        pLast = p;
        p = CharNext(p);
    }

    if('\\' != *pLast)
    {
        *p = '\\';
        p++;
        *p = '\0';
    }

    _tcscat(szDir, szFile);
}

#endif //SCRIPTSAFE_H

