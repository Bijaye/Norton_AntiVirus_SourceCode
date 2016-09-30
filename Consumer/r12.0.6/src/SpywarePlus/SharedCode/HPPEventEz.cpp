#include "StdAfx.h"
#include "HPPEventEz.h"

#include "WindowsSecurityAccount.h"

#define WSZ_REGKEY_HKLM L"\\REGISTRY\\MACHINE"
#define WSZ_REGKEY_HKU  L"\\REGISTRY\\USER"

#define WSZ_REGKEY_IE_HOME_PAGE  L"software\\microsoft\\internet explorer\\main"
#define WSZ_REGVAL_IE_HOME_PAGE  L"start page"

const TCHAR g_cszCCApp[]        = _T("\\ccApp.exe");      // AP agent exe

CStringW CHPPEventEz::m_cwszCurrentUserName;
CStringW CHPPEventEz::m_cwszCurrentUserSID;
CStringW CHPPEventEz::m_cwszHKCUHomePageKey;
CStringW CHPPEventEz::m_cwszHKLMHomePageKey;
CStringW CHPPEventEz::m_cwszHKCUHomePageKeyExt;
CStringW CHPPEventEz::m_cwszHKLMHomePageKeyExt;
CStringW CHPPEventEz::m_cwszInternetExplorerPath;
CStringW CHPPEventEz::m_cwszHPPAppPath;
CStringW CHPPEventEz::m_cwszNAVW32Path;

DWORD CHPPEventEz::m_dwCurrentSessionID = NULL;
BOOL CHPPEventEz::m_bInit = FALSE;

CHPPEventEz::CHPPEventEz(void)
{
}

CHPPEventEz::~CHPPEventEz(void)
{
}

void CHPPEventEz::InitStatics()
{
    if(m_bInit)
        return;

    CWindowsSecurityAccount cWinSecAcct;
    
    HRESULT hr;

    CString cszTemp;
    hr = cWinSecAcct.GetUserName(cszTemp);
    m_cwszCurrentUserName = CT2W(cszTemp, CP_ACP);
    
    cszTemp.Empty();
    hr = cWinSecAcct.GetCurrentUserSID(cszTemp);
    m_cwszCurrentUserSID = CT2W(cszTemp);

    hr = cWinSecAcct.GetSessionId(m_dwCurrentSessionID);
    hr = CreateHKCHomePageKey(m_cwszCurrentUserSID, m_cwszHKCUHomePageKey);
    hr = CreateHKLMHomePageKey(m_cwszHKLMHomePageKey);
    hr = CreateHKCHomePageKeyExt(m_cwszCurrentUserSID, m_cwszHKCUHomePageKeyExt);
    hr = CreateHKLMHomePageKeyExt(m_cwszHKLMHomePageKeyExt);
    hr = CreateInternetExplorerPath(m_cwszInternetExplorerPath);
    hr = CreateHPPAppPath(m_cwszHPPAppPath);
	hr = CreateNAVW32Path(m_cwszNAVW32Path);

    m_bInit = TRUE;

    return;
}

CStringW CHPPEventEz::GetCurrentUserName()
{
    InitStatics();

    return m_cwszCurrentUserName;
}

CStringW CHPPEventEz::GetCurrentUserSID()
{
    InitStatics();

    return m_cwszCurrentUserSID;
}

CStringW CHPPEventEz::GetHKCUHomePageKey()
{
    InitStatics();

    return m_cwszHKCUHomePageKey;
}

CStringW CHPPEventEz::GetHKLMHomePageKey()
{
    InitStatics();

    return m_cwszHKLMHomePageKey;
}

CStringW CHPPEventEz::GetHKCUHomePageKeyExt()
{
    InitStatics();

    return m_cwszHKCUHomePageKeyExt;
}

CStringW CHPPEventEz::GetHKLMHomePageKeyExt()
{
    InitStatics();

    return m_cwszHKLMHomePageKeyExt;
}

CStringW CHPPEventEz::GetInternetExplorerPath()
{
    InitStatics();

    return m_cwszInternetExplorerPath;
}

CStringW CHPPEventEz::GetHPPAppPath()
{
    InitStatics();

    return m_cwszHPPAppPath;
}

CStringW CHPPEventEz::GetNAVW32Path()
{
	InitStatics();

	return m_cwszNAVW32Path;
}


DWORD CHPPEventEz::GetCurrentSessionID()
{
    InitStatics();

    return (DWORD)m_dwCurrentSessionID;
}

HRESULT CHPPEventEz::AddCurrentSystemTime(CHPPEventCommonInterface *pEvent, DWORD dwProp)
{   
    if(!pEvent)
        return E_INVALIDARG;
 
    SYSTEMTIME stTime;
    ZeroMemory(&stTime, sizeof(SYSTEMTIME));
    GetSystemTime(&stTime);
    
    bool bRet;
    bRet = pEvent->SetPropertySYSTEMTIME(dwProp, &stTime);

    return bRet ? S_OK : E_FAIL;
}

HRESULT CHPPEventEz::AddCurrentUserSID(CHPPEventCommonInterface *pEvent, DWORD dwProp)
{
    if(!pEvent)
        return E_INVALIDARG;

    HRESULT hr = E_FAIL;
    
    ccLib::CExceptionInfo cException;
    try
    {
        _bstr_t cbstrCurrentUserSID;
        cbstrCurrentUserSID = GetCurrentUserSID();

        BOOL bRet;
        bRet = pEvent->SetPropertyBSTR(dwProp, cbstrCurrentUserSID);
        hr = bRet ? S_OK : E_FAIL;
    }
    CCCATCH_COM(cException)
    
    if(cException.IsException() != false)
    {
        hr = E_OUTOFMEMORY;
    }

    return hr;
}

HRESULT CHPPEventEz::AddCurrentUserName(CHPPEventCommonInterface *pEvent, DWORD dwProp)
{
    if(!pEvent)
        return E_INVALIDARG;

    HRESULT hr = E_FAIL;

    ccLib::CExceptionInfo cException;
    try
    {
        _bstr_t cbstrCurrentUserName;
        cbstrCurrentUserName = GetCurrentUserName();

        BOOL bRet;
        bRet = pEvent->SetPropertyBSTR(dwProp, cbstrCurrentUserName);
        hr = bRet ? S_OK : E_FAIL;
    }
    CCCATCH_COM(cException);

    if(cException.IsException() != false)
    {
        hr = E_OUTOFMEMORY;
    }

    return hr;
}

HRESULT CHPPEventEz::AddCurrentTerminalSessionId(CHPPEventCommonInterface *pEvent, DWORD dwProp)
{
    if(!pEvent)
        return E_INVALIDARG;

    HRESULT hr = E_FAIL;

    BOOL bRet;
    bRet = pEvent->SetPropertyLONG(dwProp, GetCurrentSessionID());
    hr = bRet ? S_OK : E_FAIL;

    return hr;
}


HRESULT CHPPEventEz::CopyEventData(CHPPEventCommonInterface *pTargetEvent, CHPPEventCommonInterface *pSourceEvent, LPPROPERTY_MAP pMap, DWORD dwMapSize)
{
    BOOL bRet;
    LONG lSizeNeeded;
    for(DWORD dwCount = 0; dwCount < dwMapSize; dwCount++)
    {
        pMap[dwCount].hrResult = E_FAIL;

        if(PROPTYPE_LONG == pMap[dwCount].dwPropType)
        {
            LONG lValue = NULL;
            bRet = pSourceEvent->GetPropertyLONG(pMap[dwCount].dwSourceProp, &lValue);
            if(bRet)
            {
                bRet = pTargetEvent->SetPropertyLONG(pMap[dwCount].dwTargetProp, lValue);
                if(bRet)
                {
                    pMap[dwCount].hrResult = S_OK;
                }
            }

        }
        else if(PROPTYPE_BSTR == pMap[dwCount].dwPropType)
        {
            _bstr_t cbstrValue;
            bRet = pSourceEvent->GetPropertyBSTR(pMap[dwCount].dwSourceProp, cbstrValue.GetAddress());
            if(bRet)
            {
                bRet = pTargetEvent->SetPropertyBSTR(pMap[dwCount].dwTargetProp, cbstrValue);
                if(bRet)
                {
                    pMap[dwCount].hrResult = S_OK;
                }
            }
        }
        else if(PROPTYPE_TIME == pMap[dwCount].dwPropType)
        {
            SYSTEMTIME stTime;
            ZeroMemory(&stTime, sizeof(SYSTEMTIME));
            bRet = pSourceEvent->GetPropertySYSTEMTIME(pMap[dwCount].dwSourceProp, &stTime);
            if(bRet)
            {
                bRet = pTargetEvent->SetPropertySYSTEMTIME(pMap[dwCount].dwTargetProp, &stTime);
                if(bRet)
                {
                    pMap[dwCount].hrResult = S_OK;
                }
            }

        }

    }
    
    
    return S_OK;
}

HRESULT CHPPEventEz::CreateHKCHomePageKey(LPCWSTR wszUserSID, CStringW &cwszHKCUHomePageKey)
{
    cwszHKCUHomePageKey.Format(L"%s\\%s\\%s", WSZ_REGKEY_HKU, wszUserSID, WSZ_REGKEY_IE_HOME_PAGE);
    return S_OK;
}

HRESULT CHPPEventEz::CreateHKLMHomePageKey(CStringW &cwszHKLMHomePageKey)
{
    cwszHKLMHomePageKey.Format(L"%s\\%s", WSZ_REGKEY_HKLM, WSZ_REGKEY_IE_HOME_PAGE);
    return S_OK;
}

HRESULT CHPPEventEz::CreateHKCHomePageKeyExt(LPCWSTR wszUserSID, CStringW &cwszHKCUHomePageKeyExt)
{
    cwszHKCUHomePageKeyExt.Format(L"%s\\%s\\%s\\%s", WSZ_REGKEY_HKU, wszUserSID, WSZ_REGKEY_IE_HOME_PAGE, WSZ_REGVAL_IE_HOME_PAGE);
    return S_OK;
}

HRESULT CHPPEventEz::CreateHKLMHomePageKeyExt(CStringW &cwszHKLMHomePageKeyExt)
{
    cwszHKLMHomePageKeyExt.Format(L"%s\\%s\\%s", WSZ_REGKEY_HKLM, WSZ_REGKEY_IE_HOME_PAGE, WSZ_REGVAL_IE_HOME_PAGE);
    return S_OK;
}

HRESULT CHPPEventEz::CreateInternetExplorerPath(CStringW &cwszInternetExplorerPath)
{
    CString cszLocation;
    GetPathAppl(L"InternetExplorer.Application", cszLocation.GetBuffer(MAX_PATH), MAX_PATH);
    cszLocation.ReleaseBuffer();

    PathUnquoteSpaces(cszLocation.GetBuffer());
    cszLocation.ReleaseBuffer();

    PathMakePretty(cszLocation.GetBuffer());
    cszLocation.ReleaseBuffer();

	CCTRACEI(_T("CHPPEventEz::CreateInternetExplorerPath - ExplorerPath == %s"), cszLocation);
    cwszInternetExplorerPath = CT2W(cszLocation, CP_ACP);

    return S_OK;
}

HRESULT CHPPEventEz::CreateHPPAppPath(CStringW &cwszHPPAppPath)
{
    // Get the path to the current process (should be ccApp.exe)

    CString cszPath;
    GetModuleFileName(NULL, cszPath.GetBuffer(MAX_PATH), MAX_PATH);
    cszPath.ReleaseBuffer();

    PathMakePretty(cszPath.GetBuffer());
    cszPath.ReleaseBuffer();
	
	CCTRACEI(_T("CHPPEventEz::CreateHPPAppPath - HPPAppPath == %s"), cszPath);
    cwszHPPAppPath = CT2W(cszPath, CP_ACP);

    return S_OK;
}

HRESULT CHPPEventEz::CreateNAVW32Path(CStringW &cwszNAVW32Path)
{
	CString cszPath;
	if(!GetSymcAppPath(_T("NAVNT"), cszPath))
	{
		cwszNAVW32Path.Empty();
		CCTRACEE(_T("CHPPEventEz::CreateNAVW32Path - GetSymcAppPath() returned false."));
		return E_FAIL;
	}

	TCHAR szNAVW32[] = _T("NAVW32.exe");
	DWORD dwSize = cszPath.GetLength() + _tcslen(szNAVW32) + 1;
	PathAppend(cszPath.GetBuffer(dwSize), szNAVW32);
	cszPath.ReleaseBuffer();

	PathMakePretty(cszPath.GetBuffer());
	cszPath.ReleaseBuffer();

	CCTRACEI(_T("CHPPEventEz::CreateNAVW32Path - NAVW32Path == %s"), cszPath);
	cwszNAVW32Path = CT2W(cszPath, CP_ACP);

	return S_OK;
}

BOOL CHPPEventEz::GetSymcAppPath(LPCTSTR szSymcApp, CString &cszPath)
{
	// Get the path to the NAV directory from
	//  HKLM\Software\Symantec\InstalledApps
	CRegKey crkInstalledApps;
	DWORD dwRet = NULL;

	// Open up the SYMC installed apps key
	dwRet = crkInstalledApps.Open(HKEY_LOCAL_MACHINE, _T("Software\\Symantec\\InstalledApps"), KEY_READ);
	if(ERROR_SUCCESS != dwRet)
	{
		CCTRACEE(_T("CHPPEventEz::GetSymcAppPath - couldn't open installed apps key"));
		cszPath.Empty();
		return FALSE;
	}

	// Query the size of the data in the NAVNT value
	DWORD dwSize = NULL;
	dwRet = crkInstalledApps.QueryStringValue(szSymcApp, NULL, &dwSize);
	if(ERROR_SUCCESS != dwRet || NULL == dwSize)
	{
		CCTRACEE(_T("CHPPEventEz::GetSymcAppPath - couldn't query data size for %s value. dwRet=%d, dwSize=%d"), szSymcApp, dwRet, dwSize);
		cszPath.Empty();
		return FALSE;
	}

	// Get the path information
	dwSize++;
	dwRet = crkInstalledApps.QueryStringValue(szSymcApp, cszPath.GetBuffer(dwSize), &dwSize);
	cszPath.ReleaseBuffer();
	if(ERROR_SUCCESS != dwRet)
	{
		CCTRACEE(_T("CHPPEventEz::GetSymcAppPath - couldn't query data for %s value. dwRet=%d, dwSize=%d"), szSymcApp, dwRet, dwSize);
		cszPath.Empty();
		return FALSE;
	}
	
	return TRUE;
}


BOOL CHPPEventEz::GetPathAppl(LPOLESTR szApp, LPSTR szPath, ULONG cSize)
{
    CLSID clsid;
    LPOLESTR pwszClsid;
    CString cszKey;
    CString  cszCLSID;

    CRegKey rkKey;

    // szPath must be at least 255 char in size
    if (cSize < 255)
        return FALSE;

    // Get the CLSID using ProgID
    HRESULT hr = CLSIDFromProgID(szApp, &clsid);
    if (FAILED(hr))
    {
        return FALSE;
    }

    // Convert CLSID to String
    hr = StringFromCLSID(clsid, &pwszClsid);
    if (FAILED(hr))
    {
        return FALSE;
    }

    cszCLSID = pwszClsid;

    // Free memory used by StringFromCLSID
    CoTaskMemFree(pwszClsid);

    // Format Registry Key string
    cszKey.Format("CLSID\\%s\\LocalServer32", cszCLSID);

    // Open key to find path of application
    LONG lRet = rkKey.Open(HKEY_CLASSES_ROOT, cszKey, KEY_READ);
    if (lRet != ERROR_SUCCESS)
    {
        // If LocalServer32 does not work, try with LocalServer
        cszKey.Format("CLSID\\%s\\LocalServer", cszCLSID);
        lRet = rkKey.Open(HKEY_CLASSES_ROOT, cszKey, KEY_READ);
        if (lRet != ERROR_SUCCESS)
        {
            return FALSE;
        }
    }

    // Query value of key to get Path and close the key
    lRet = rkKey.QueryStringValue(NULL, szPath, &cSize);
    if (lRet != ERROR_SUCCESS)
    {
        return FALSE;
    }

    // Strip off the '/Automation' switch from the path
    char *x = strrchr(szPath, '/');
    if(0!= x) // If no /Automation switch on the path
    {
        int result = x - szPath;
        szPath[result]  = '\0';  // If switch there, strip it
    }

    return TRUE;
}

