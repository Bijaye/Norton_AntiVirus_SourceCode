#pragma once

class CHPPUtility
{
public:
    CHPPUtility(void);
    virtual ~CHPPUtility(void);

    static HRESULT SetHomePage(BSTR bszNewHomePage, BOOL bCurrentUser);
    static HRESULT SetHomePage(LPCSTR szNewHomePage, BOOL bCurrentUser);
    static HRESULT ResetOptions(void);
    static HRESULT PrepHPPForUninstall();

};
