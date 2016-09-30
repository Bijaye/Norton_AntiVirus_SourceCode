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
    bool getRegValue(HKEY hKey, LPSTR szSubKey, LPSTR szValueKey, LPBYTE lpValueData, DWORD dwValueSize);
};
}