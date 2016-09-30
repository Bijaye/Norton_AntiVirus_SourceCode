#include "StdAfx.h"
#include "ccCoInitialize.h"
#include "RegKeyMonitor.h"
#include "WindowsSecurityAccount.h"

#pragma comment(lib, "Wbemuuid.lib")

using namespace StahlSoft;

CRegKeyMonitorWMI::CRegKeyMonitorWMI(void)
{
}

CRegKeyMonitorWMI::~CRegKeyMonitorWMI(void)
{
    Destroy();
}

void CRegKeyMonitorWMI::Destroy()
{
    m_spWbemServices.Release();
    m_spIWbemLocator.Release();
    m_spProcObjEnum.Release();

    return;
}

BOOL CRegKeyMonitorWMI::Initialize(HKEY hKey, LPCSTR szPath, LPCTSTR szKeyName)
{
    HRESULT hr;
    HRESULT hRes = CoCreateInstance (
        CLSID_WbemAdministrativeLocator,
        NULL ,
        CLSCTX_INPROC_SERVER | CLSCTX_LOCAL_SERVER , 
        IID_IUnknown ,
        ( void ** ) &m_spIWbemLocator
        ) ;

    if(FAILED(hRes))
        return FALSE;

    hr = m_spIWbemLocator->ConnectServer(CComBSTR(L"\\\\.\\root\\default"), NULL, NULL, NULL, 0, NULL, NULL, &m_spWbemServices);
    if(FAILED(hRes))
        return FALSE;

    CString cszRootName;
    if(HKEY_LOCAL_MACHINE == hKey)
    {
        cszRootName = _T("HKEY_LOCAL_MACHINE");
    }
    else if(HKEY_CURRENT_USER == hKey)
    {
        cszRootName = _T("HKEY_CURRENT_USER");
    }

    
    // Create an instance of a suitable sink object, and init
    m_spProcObjEnum = new CQueryObjectSink;
    m_spProcObjEnum->Initialize(hKey, cszRootName, szPath, szKeyName);

    CStringW cwszQueryString;
    
    // Create our WMI query
    cwszQueryString.Format(L"Select * from RegistryKeyChangeEvent Where Hive = '%S' and KeyPath = '%S' ", cszRootName, szPath);
    
    // Request an async notification for updates.
    hr = m_spWbemServices->ExecNotificationQueryAsync( CComBSTR(L"WQL"), 
        CComBSTR(cwszQueryString),
        WBEM_FLAG_SEND_STATUS,
        NULL,
        (CQueryObjectSink*)m_spProcObjEnum);

    switch(hr)
    {
    case WBEM_E_FAILED:
        break;

    case WBEM_E_INVALID_CLASS:
        break;

    case WBEM_E_INVALID_PARAMETER:
        break;

    case WBEM_E_INVALID_QUERY:
        break;

    case WBEM_E_INVALID_QUERY_TYPE:
        break;

    case WBEM_E_OUT_OF_MEMORY:
        break;

    case WBEM_E_SHUTTING_DOWN:
        break;

    case WBEM_E_TRANSPORT_FAILURE:
        break;

    case WBEM_E_UNPARSABLE_QUERY:
        break;

    case WBEM_S_NO_ERROR:
        break;
    }

    return SUCCEEDED(hr);
}


ULONG CQueryObjectSink::AddRef()
{
    return InterlockedIncrement(&m_lRef);
}

ULONG CQueryObjectSink::Release()
{
    LONG lRef = InterlockedDecrement(&m_lRef);
    if(lRef == 0)
        delete this;
    return lRef;
}

HRESULT CQueryObjectSink::QueryInterface(REFIID riid, void** ppv)
{
    if (riid == IID_IUnknown || riid == IID_IWbemObjectSink)
    {
        *ppv = (IWbemObjectSink *) this;
        AddRef();
        return WBEM_S_NO_ERROR;
    }
    else return E_NOINTERFACE;
}


HRESULT CQueryObjectSink::Indicate(long lObjCount, IWbemClassObject **pArray)
{
    for (long i = 0; i < lObjCount; i++)
    {
        IWbemClassObject *pObj = pArray[i];

        // ... use the object.

        // AddRef() is only required if the object will be held after
        // the return to the caller.
    }

    CCTRACEI(_T("CQueryObjectSink::Indicate lObjCount = 0x%X"), lObjCount);
    
    CString cszValue;
    GetValue(cszValue);

    if(NULL != cszValue.CompareNoCase(m_cszOldValue))
    {
        CString cszOutput;
        cszOutput.Format(_T("%s\\%s:%s value changed from '%s' to '%s'."), m_cszRootName, m_cszPath, m_cszKeyName, m_cszOldValue, cszValue);
        CCTRACEI(cszOutput);
        CCTRACEI(cszOutput);

        m_cszOldValue = cszValue;
    }

    return WBEM_S_NO_ERROR;
}

HRESULT CQueryObjectSink::SetStatus(
                                    /* [in] */ LONG lFlags,
                                    /* [in] */ HRESULT hResult,
                                    /* [in] */ BSTR strParam,
                                    /* [in] */ IWbemClassObject __RPC_FAR *pObjParam
                                    )
{
    CCTRACEI(_T("CQueryObjectSink::SetStatus hResult = 0x%X"), hResult);

    return WBEM_S_NO_ERROR;
}

HRESULT CQueryObjectSink::Initialize(HKEY hKey, LPCSTR szRootName, LPCSTR szPath, LPCTSTR szKeyName)
{
    
    m_hRootKey = hKey;
    m_cszRootName = szRootName;
    m_cszPath = szPath;
    m_cszKeyName = szKeyName;
    
    return GetValue(m_cszOldValue);
}

HRESULT CQueryObjectSink::GetValue(CString &cszValue)
{
    CRegKey rkCurrent;
    LONG lRet;
    lRet = rkCurrent.Open(m_hRootKey, m_cszPath, KEY_READ);
    if(lRet != ERROR_SUCCESS)
        return HRESULT_FROM_WIN32(GetLastError());

    DWORD dwSize = 0;

    lRet = rkCurrent.QueryStringValue(m_cszKeyName, NULL, &dwSize);
    if(!dwSize)
        return E_FAIL;

    dwSize++;
    lRet = rkCurrent.QueryStringValue(m_cszKeyName, cszValue.GetBuffer(dwSize + 1), &dwSize);
    cszValue.ReleaseBuffer();

    if(lRet != ERROR_SUCCESS)
        return E_FAIL;
    
    return S_OK;
}


CRegKeyMonitorWin32::CRegKeyMonitorWin32()
{
    m_shRunning = CreateEvent(NULL, TRUE, TRUE, NULL);
}

CRegKeyMonitorWin32::~CRegKeyMonitorWin32()
{
}

HRESULT CRegKeyMonitorWin32::IsRunning()
{
    HRESULT hrReturn = S_OK;

    DWORD dwRet = WaitForSingleObject(m_shRunning, 0);
    if(dwRet == WAIT_TIMEOUT)
    {
        hrReturn = S_OK;
    }
    else if(dwRet == WAIT_OBJECT_0)
    {
        hrReturn = S_FALSE;
    }
    else
    {
        hrReturn = E_FAIL;
    }

    return hrReturn;
}

STDMETHODIMP CRegKeyMonitorWin32::RequestExit()
{
    CCTRACEI(_T("CRegKeyMonitorWin32::RequestExit() - Exit Requested."));

    BOOL bFailed = SetEvent(m_shRunning);

    return bFailed ? E_FAIL : S_OK;
}

HRESULT CRegKeyMonitorWin32::Run()
{
    // Need to init COM on this thread
    ccLib::CCoInitialize coInit;
    if( FAILED( coInit.Initialize( ccLib::CCoInitialize::eMTAModel ) ) )
    {
        CCTRACEE(_T("CRegKeyMonitorWin32::Run - Could not init COM."));
        return S_OK;
    }

    
    ResetEvent(m_shRunning);

    CRegKey rkMonitor;

    // make sure we can open the key for monitoring
    LONG lRet = rkMonitor.Open(m_hRootKey, m_cszPath, KEY_READ);
    if(ERROR_SUCCESS != lRet)
        return S_OK;
    
    rkMonitor.Close();

    while(TRUE)
    {
        StahlSoft::CSmartHandle shMonitor = CreateEvent(NULL, TRUE, FALSE, NULL);

        CRegKey rkMonitorKey;

        // Need to open a new key each time we start the monitor.
        // Reportedly, the key can be closed when the monitor signals.
        LONG lRet = rkMonitorKey.Open(m_hRootKey, m_cszPath, KEY_READ);
        if(ERROR_SUCCESS != lRet)
        {
            CCTRACEE(_T("CRegKey::Open() called failed. GetLastError() returned: 0x%08X"), GetLastError());
            break;
        }
        
        // Set the monitoring on our key.
        if(ERROR_SUCCESS != RegNotifyChangeKeyValue(rkMonitorKey, FALSE, REG_NOTIFY_CHANGE_LAST_SET, shMonitor, TRUE))
        {
            CCTRACEE(_T("RegNotifyChangeKeyValue() called failed. GetLastError() returned: 0x%08X"), GetLastError());
            break;
        }

        //
        // Wait here
        //

        DWORD dwRet;
        BOOL bRunning = TRUE;
        HANDLE hWaitHandles[] = {shMonitor, m_shRunning};    
        
        // Wait indefinitely
        CCTRACEI(_T("CRegKeyMonitorWin32::Run() monitoring '%s\\%s'"), m_cszRootName, m_cszPath);
        CCTRACEI(_T("CRegKeyMonitorWin32::Run() waiting for %d events."), CCDIMOF(hWaitHandles));
        dwRet = WaitForMultipleObjects( CCDIMOF(hWaitHandles), hWaitHandles, FALSE, INFINITE);
        switch(dwRet)
        {
            case WAIT_OBJECT_0:
                // Reg key was possibly changed
                CCTRACEI(_T("CRegKeyMonitorWin32::Run() -  CRegKeyMonitorWin32() signaled.  Checking key."));
                CheckRegKey();
                break;

            case WAIT_OBJECT_0 + 1:
                // Thread has been asked to exit.
                CCTRACEI(_T("Time to leave..."));
                break;
            
            default:
                // Something went wrong.
                CCTRACEI(_T("CRegKeyMonitorWin32::Run() - Something went wrong..."));
                break;

        }
        
        if(S_OK != IsRunning())
            break;
    }

    m_spEvtHelper.Release();



    CCTRACEI(_T("CRegKeyMonitorWin32::Run() - Leaving."));
    return S_OK;
}


HRESULT CRegKeyMonitorWin32::Initialize(HKEY hKey, LPCSTR szPath, LPCTSTR szKeyName)
{
    m_hRootKey = hKey;
    m_cszPath = szPath;
    m_cszKeyName = szKeyName;
    
    // Set a user-readable string indicating the root key
    if(HKEY_LOCAL_MACHINE == hKey)
        m_cszRootName = _T("HKEY_LOCAL_MACHINE");
    else if(HKEY_CURRENT_USER == hKey)
        m_cszRootName = _T("HKEY_CURRENT_USER");
    else
        m_cszRootName = _T("UNKNOWN_ROOT");

    // seed our reference value
    GetValue(m_cszOldValue);

    // create the thread to perform the monitoring
    return CreateThread();
}

void CRegKeyMonitorWin32::Destroy()
{
    if(S_OK != IsRunning())
        return;

    // Ask nicely
    CCTRACEI(_T("CRegKeyMonitorWin32::Destroy() - asking nicely"));
    RequestExit();
    
    // Insure compliance
    CCTRACEI(_T("CRegKeyMonitorWin32::Destroy() - insuring compliance"));
    DWORD dwRet = WaitForSingleObject(GetThreadHandle(), 5000);

    // Insist
    if(dwRet == WAIT_TIMEOUT)
    {
        CCTRACEI(_T("CRegKeyMonitorWin32::Destroy() - Thread 0x%08X still active, terminating."), GetThreadId());
        TerminateThread(NULL);
    }
    
    return;
}

HRESULT CRegKeyMonitorWin32::GetValue(CString &cszValue)
{
    CRegKey rkCurrent;
    LONG lRet;
    lRet = rkCurrent.Open(m_hRootKey, m_cszPath, KEY_READ);
    if(lRet != ERROR_SUCCESS)
        return HRESULT_FROM_WIN32(GetLastError());

    DWORD dwSize = 0;

    lRet = rkCurrent.QueryStringValue(m_cszKeyName, NULL, &dwSize);
    if(!dwSize)
        return E_FAIL;

    dwSize++;
    lRet = rkCurrent.QueryStringValue(m_cszKeyName, cszValue.GetBuffer(dwSize + 1), &dwSize);
    cszValue.ReleaseBuffer();

    if(lRet != ERROR_SUCCESS)
        return E_FAIL;

    return S_OK;
}

HRESULT CRegKeyMonitorWin32::GetCheckValue(CString &cszValue)
{
    CRegKey rkCurrent;
    LONG lRet;

    HKEY hkCheck = HKEY_CURRENT_USER;
    LPCTSTR szCheckPathName = {_T("Software\\Symantec\\SpywarePlus\\CheckVal")};

    lRet = rkCurrent.Open(hkCheck, szCheckPathName, KEY_READ);
    if(lRet != ERROR_SUCCESS)
        return HRESULT_FROM_WIN32(GetLastError());

    DWORD dwSize = 0;

    lRet = rkCurrent.QueryStringValue(m_cszKeyName, NULL, &dwSize);
    if(!dwSize)
        return E_FAIL;

    dwSize++;
    lRet = rkCurrent.QueryStringValue(m_cszKeyName, cszValue.GetBuffer(dwSize + 1), &dwSize);
    cszValue.ReleaseBuffer();

    if(lRet != ERROR_SUCCESS)
        return E_FAIL;

    return S_OK;
}

void CRegKeyMonitorWin32::CheckRegKey()
{
    CString cszValue, cszCheckValue;
    
    GetValue(cszValue);
    GetCheckValue(cszCheckValue);

    if(NULL != cszValue.CompareNoCase(cszCheckValue))
    {
        CString cszOutput;
        cszOutput.Format(_T("%s\\%s:%s value changed from '%s' to '%s'."), m_cszRootName, m_cszPath, m_cszKeyName, cszCheckValue, cszValue);
        CCTRACEI(cszOutput);
        CCTRACEI(cszOutput);
        
        SendKeyChangedEvent(cszValue, cszCheckValue);

        m_cszOldValue = cszValue;
    }
    else
    {
        CString cszOutput;
        cszOutput.Format(_T("CRegKeyMonitorWin32::CheckRegKey() - False alarm."));
        CCTRACEI(cszOutput);
        CCTRACEI(cszOutput);
    }
    
    return;
}

void CRegKeyMonitorWin32::SendKeyChangedEvent(LPCTSTR szNewValue, LPCTSTR szOldValue)
{
    ccEvtMgr::CEventEx *pEvent;

    if(!m_spEvtHelper)
    {
        m_spEvtHelper = new CHPPEventHelper;
    }


    SYMRESULT sr = m_spEvtHelper->NewHPPEvent(HPP::Event_ID_HPPNotifyHomePage, &pEvent);
    if(SYM_FAILED(sr))
        return;

    CHPPEventCommonInterfaceQIPtr spCommonEvent = pEvent;
    if(!spCommonEvent)
        return;

    CWindowsSecurityAccount cSecurityInfo;

    CString cszValue;
    DWORD lValue;
    BOOL bRet;
    HRESULT hr;

    // Get current user name
    cszValue.Empty();
    hr = cSecurityInfo.GetUserName(cszValue);
    if(SUCCEEDED(hr))
    {
        _bstr_t cbszValue = cszValue;
        spCommonEvent->SetPropertyBSTR(HPP::Event_HPPNotifyHomePage_propUserName, cbszValue);
    }

    // Get current user SID
    cszValue.Empty();
    hr = cSecurityInfo.GetCurrentUserSID(cszValue);
    if(SUCCEEDED(hr))
    {
        _bstr_t cbszValue = cszValue;
        spCommonEvent->SetPropertyBSTR(HPP::Event_HPPNotifyHomePage_propUserSID, cbszValue);
    }

    // Get current session id
    lValue = 0;
    hr = cSecurityInfo.GetSessionId(lValue);
    if(SUCCEEDED(hr))
    {
        spCommonEvent->SetPropertyLONG(HPP::Event_HPPNotifyHomePage_propSessionID, lValue);
    }

    // Set current process id
    spCommonEvent->SetPropertyLONG(HPP::Event_HPPNotifyHomePage_propProcessID, GetCurrentProcessId());

    // Set current process path and name
    bRet = GetModuleFileName(NULL, cszValue.GetBuffer(MAX_PATH), MAX_PATH);
    cszValue.ReleaseBuffer();
    if(bRet)
    {
        _bstr_t cbszValue = cszValue;
        spCommonEvent->SetPropertyBSTR(HPP::Event_HPPNotifyHomePage_propProcessPath, cbszValue);

        PathStripPath(cszValue.GetBuffer(MAX_PATH));
        cszValue.ReleaseBuffer();

        cbszValue = cszValue;
        spCommonEvent->SetPropertyBSTR(HPP::Event_HPPNotifyHomePage_propProcessName, cbszValue);
    }

    _bstr_t cbszHomePageStartValue = (LPCTSTR)szOldValue;
    _bstr_t cbszHomePageActionValue = (LPCTSTR)szNewValue;

    spCommonEvent->SetPropertyBSTR(HPP::Event_HPPNotifyHomePage_propStartValue, cbszHomePageStartValue);
    spCommonEvent->SetPropertyBSTR(HPP::Event_HPPNotifyHomePage_propActionValue, cbszHomePageActionValue);
    spCommonEvent->SetPropertyLONG(HPP::Event_HPPNotifyHomePage_propActionResult, HPP::HPP_ACTION_ALLOW);

    long lEventId = 0;
    if(m_spEvtHelper->CreateEvent(*pEvent, true, lEventId) != ccEvtMgr::CError::eNoError)
    {
        CCTRACEI(_T("CreateEvent() failed."));
    }
    
    return;
}
