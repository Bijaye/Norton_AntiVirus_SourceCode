#pragma once

#include "HPPEventsInterface.h"
#include "WindowsSecurityAccount.h"

class CHPPEventEz
{
public:
    CHPPEventEz(void);
    virtual ~CHPPEventEz(void);
    
    enum { PROPTYPE_INVALID = 0, PROPTYPE_LONG, PROPTYPE_BSTR, PROPTYPE_TIME };
    struct PROPERTY_MAP
    {
        DWORD dwTargetProp;
        DWORD dwSourceProp;
        DWORD dwPropType;
        HRESULT hrResult;
    };
    
    typedef PROPERTY_MAP* LPPROPERTY_MAP;

    static HRESULT AddCurrentSystemTime(CHPPEventCommonInterface *pEvent, DWORD dwProp) throw();
    static HRESULT AddCurrentUserSID(CHPPEventCommonInterface *pEvent, DWORD dwProp) throw();
    static HRESULT AddCurrentUserName(CHPPEventCommonInterface *pEvent, DWORD dwProp) throw();
    static HRESULT AddCurrentTerminalSessionId(CHPPEventCommonInterface *pEvent, DWORD dwProp) throw();
    
    static HRESULT CopyEventData(CHPPEventCommonInterface *pTargetEvent, CHPPEventCommonInterface *pSourceEvent, LPPROPERTY_MAP pMap, DWORD dwMapSize) throw();
    
    static HRESULT CreateHKCHomePageKey(LPCWSTR wszUserSID, CStringW &cwszHKCUHomePageKey);
    static HRESULT CreateHKLMHomePageKey(CStringW &cwszHKLMHomePageKey);
    static HRESULT CreateHKCHomePageKeyExt(LPCWSTR wszUserSID, CStringW &cwszHKCUHomePageKeyExt);
    static HRESULT CreateHKLMHomePageKeyExt(CStringW &cwszHKLMHomePageKeyExt);
    static HRESULT CreateInternetExplorerPath(CStringW &cwszInternetExplorerPath);
    static HRESULT CreateHPPAppPath(CStringW &cwszHPPAppPath);
	static HRESULT CreateNAVW32Path(CStringW &cwszNAVW32Path);

    static CStringW GetCurrentUserName();
    static CStringW GetCurrentUserSID();
    static CStringW GetHKCUHomePageKey();
    static CStringW GetHKLMHomePageKey();
    static CStringW GetHKCUHomePageKeyExt();
    static CStringW GetHKLMHomePageKeyExt();
    static CStringW GetInternetExplorerPath();
    static CStringW GetHPPAppPath();
	static CStringW GetNAVW32Path();

    static DWORD GetCurrentSessionID();

protected:
    static void InitStatics();
    static BOOL GetPathAppl(LPOLESTR szApp, LPSTR szPath, ULONG cSize);
	static BOOL GetSymcAppPath(LPCTSTR szSymcApp, CString &cszPath);

    static CStringW m_cwszCurrentUserName;
    static CStringW m_cwszCurrentUserSID;
    static CStringW m_cwszHKCUHomePageKey;
    static CStringW m_cwszHKLMHomePageKey;
    static CStringW m_cwszHKCUHomePageKeyExt;
    static CStringW m_cwszHKLMHomePageKeyExt;
    static CStringW m_cwszInternetExplorerPath;
    static CStringW m_cwszHPPAppPath;
	static CStringW m_cwszNAVW32Path;

    static DWORD m_dwCurrentSessionID;
    
    static BOOL m_bInit;
};
