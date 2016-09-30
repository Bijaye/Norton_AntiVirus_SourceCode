#pragma once

namespace NAVToolbox
{
class CNAVHelpLauncher
{
public:
    CNAVHelpLauncher(void);
    virtual ~CNAVHelpLauncher(void);

    HWND LaunchHelp ( DWORD dwHelpID = 0, HWND hParentWnd = NULL, bool bTopMost = false );
    LPTSTR GetHelpPath ();

private:
    TCHAR m_szNAVHelpFile[MAX_PATH];
    bool InitGetNAVHelpFile();

    // Local functions
    BOOL   LaunchHelpAsUser( LPCTSTR pszHelpFile, DWORD dwHelpID );
    BOOL   RunningAsAService();
    HANDLE GetCurrentUserToken();

};
}
