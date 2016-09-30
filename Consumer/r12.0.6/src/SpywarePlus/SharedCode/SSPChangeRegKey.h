#pragma once
#include "SSPProcessEventBase.h"

class CSSPChangeRegKey :
	public CSSPProcessEventBase
{
public:
	CSSPChangeRegKey(void);
	virtual ~CSSPChangeRegKey(void);

	STDMETHOD(Run)();
    void SetNewHomePage(LPCTSTR szNewHomePageValue, BOOL bChangeUserKey, DWORD dwThreadId);

protected:
    HRESULT ChangeHomePageKey(HKEY hKey, LPCTSTR szNewHomePageValue);

    CString m_cszNewHomePageValue;
    BOOL m_bChangeUserKey;
    DWORD m_dwThreadId;

};
