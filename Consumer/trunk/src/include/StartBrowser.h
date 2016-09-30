////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

namespace NAVToolbox
{
class CStartBrowser
{
public:
    CStartBrowser(void);
    ~CStartBrowser(void);

    bool ShowURL ( LPCTSTR lpszURL );

private:
    bool getExtensionApp(LPTSTR szApplication, int iSizeApp);
    bool percentArgs(LPTSTR szString, LPTSTR szFormat, ...);
    bool getRegValue(HKEY hKey, LPTSTR szSubKey, LPTSTR szValueKey, LPBYTE lpValueData, DWORD dwValueSize);
};
}