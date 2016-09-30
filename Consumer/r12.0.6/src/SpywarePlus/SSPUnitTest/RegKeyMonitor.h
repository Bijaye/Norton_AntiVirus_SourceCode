#pragma once

#include "Wbemcli.h"
#include "Wbemidl.h"

#include "HPPEventsInterface.h"
#include "HPPEventHelper.h"

class CQueryObjectSink : public IWbemObjectSink
{
    LONG m_lRef;
    bool bDone; 

public:
    CQueryObjectSink() { m_lRef = 0; }
    ~CQueryObjectSink() { bDone = TRUE; }

    virtual ULONG STDMETHODCALLTYPE AddRef();
    virtual ULONG STDMETHODCALLTYPE Release();        
    virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv);

    virtual HRESULT STDMETHODCALLTYPE Indicate( 
        /* [in] */ LONG lObjectCount,
        /* [size_is][in] */ IWbemClassObject __RPC_FAR *__RPC_FAR *apObjArray
        );

    virtual HRESULT STDMETHODCALLTYPE SetStatus( 
        /* [in] */ LONG lFlags,
        /* [in] */ HRESULT hResult,
        /* [in] */ BSTR strParam,
        /* [in] */ IWbemClassObject __RPC_FAR *pObjParam
        );
    
    virtual HRESULT Initialize(HKEY hKey, LPCSTR szRootName, LPCSTR szPath, LPCTSTR szKeyName);
    virtual HRESULT GetValue(CString &cszValue);

protected:
    HKEY m_hRootKey;
    CString m_cszRootName;
    CString m_cszPath;
    CString m_cszKeyName;
    CString m_cszOldValue;

};


class CRegKeyMonitorWMI
{
public:
    CRegKeyMonitorWMI(void);
    virtual ~CRegKeyMonitorWMI(void);

    BOOL Initialize(HKEY hKey, LPCSTR szPath, LPCTSTR szKeyName);
    void Destroy();

protected:
    
    CComPtr<IWbemLocator>     m_spIWbemLocator;
    CComPtr<IWbemServices>    m_spWbemServices;
    CComPtr<CQueryObjectSink> m_spProcObjEnum;
};

class CRegKeyMonitorWin32 : StahlSoft::CThread
{
public:
    CRegKeyMonitorWin32();
    ~CRegKeyMonitorWin32();

    HRESULT Initialize(HKEY hKey, LPCSTR szPath, LPCTSTR szKeyName);
    void Destroy();

    virtual STDMETHODIMP Run();
    virtual STDMETHODIMP IsRunning();
    virtual STDMETHODIMP RequestExit();

protected:
    void CheckRegKey();
    virtual HRESULT GetValue(CString &cszValue);
    virtual HRESULT GetCheckValue(CString &cszValue);
    
    void SendKeyChangedEvent(LPCTSTR szNewValue, LPCTSTR szOldValue);

protected:
    CHPPEventHelperPtr m_spEvtHelper;

    StahlSoft::CSmartHandle m_shRunning;

    HKEY m_hRootKey;
    CString m_cszRootName;
    CString m_cszPath;
    CString m_cszKeyName;
    CString m_cszOldValue;

};

