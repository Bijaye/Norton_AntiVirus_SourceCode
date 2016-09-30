// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// VpshellEx.cpp : Implementation of CVpshellEx
#include "stdafx.h"
#include "vpshell2.h"
#include "VpshellEx.h"
#include "Scaninf_i.c"
#include "vpcommon.h"
#include "clientreg.h"
#include "shfolder.h"
#include "sharedver.h"
#include "terminalsession.h"
#include "SymSaferRegistry.h"
#include "SymSaferStrings.h"

#define ResultFromShort(i)  ResultFromScode(MAKE_SCODE(SEVERITY_SUCCESS, 0, (USHORT)(i)))

extern CComModule _Module;

/////////////////////////////////////////////////////////////////////////////
// Local Functions
DWORD CreateNAVDataDirectory();
BOOL  IsUserDirNeeded();
BOOL  IsWinNT();
BOOL  IsWindows2000();
DWORD GetSystemDataDir(LPTSTR lpDir);
DWORD GetClientType();
BOOL  NAVDataDirectoryExists();


/////////////////////////////////////////////////////////////////////////////
// CVpshellEx


/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CVpshellEx::Initialize(LPCITEMIDLIST pidlFolder,
                          LPDATAOBJECT lpdobj, HKEY hkeyProgID)
{
    FORMATETC fmte = {CF_HDROP,
                      NULL,
                      DVASPECT_CONTENT,
                      -1,
                      TYMED_HGLOBAL 
                     };
    STGMEDIUM medium;
    HRESULT hr=E_FAIL;
    
    if (lpdobj)  //Paranoid check, m_pDataObj should have something by now...
       hr = lpdobj->GetData(&fmte, &medium);

    if (hr==S_OK) 
    {
        m_hDrop = (HDROP)medium.hGlobal;
        if (m_hDrop)
            CoInitialize(NULL);
    }

    // determine if we are nunning under windows NT
    OSVERSIONINFO os;
    os.dwOSVersionInfoSize=sizeof(OSVERSIONINFO);
    GetVersionEx(&os);
    m_bNT = (os.dwPlatformId == VER_PLATFORM_WIN32_NT); 

    return hr;
}
 

/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CVpshellEx::QueryContextMenu(HMENU hmenu,
                                UINT indexMenu,
                                UINT idCmdFirst,
                                UINT idCmdLast,
                                UINT uFlags)
{
    UINT idCmd = idCmdFirst;
    TCHAR sText[MAX_PATH] = {0};
    BOOL bIgnore = FALSE;

    // MH 03.07.01
    // Ignore cases that don't make sense for us to add our menuitem.

    //EA 05/02/2001 Ignore cases where session is remote since we do not want to show rt click scanning
    //for remote session
    CTerminalSession TSess(FALSE);
    if (TRUE == TSess.IsRemoteSession())
        bIgnore = TRUE;
    // CMF_NOVERBS is the flag for the 'send to' menu.
    // Don't want to try and insert our item into this menu.
    if (uFlags&CMF_NOVERBS)
    {
        bIgnore = TRUE;
    }

    // CMF_VERBSONLY && !CMF_CANRENAME means that
    // we're not a valid shortcut; i.e., we're probably
    // a shortcut item on the MSOffice taskbar, Start menu, etc.
    if ((uFlags&CMF_VERBSONLY) && !(uFlags&CMF_CANRENAME))
    {
        bIgnore = TRUE;
    }

    if (FALSE==bIgnore)
    {
        ::LoadString(_Module.GetResourceInstance(),IDS_VIRUS_TEXT,sText,MAX_PATH);
            
        InsertMenu(hmenu, indexMenu++, MF_SEPARATOR|MF_BYPOSITION, 0, NULL);
        
        InsertMenu(hmenu,
                   indexMenu++,
                   MF_STRING|MF_BYPOSITION,
                   idCmd,
                   sText);
        idCmd++;
    }

    return ResultFromShort(idCmd - idCmdFirst); 
}



/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CVpshellEx::ActuallyScan(LPCMINVOKECOMMANDINFO lpici)
{
DWORD CreateNAVDataDirectory();
BOOL  IsUserDirNeeded();
BOOL  IsWinNT();
DWORD GetSystemDataDir(LPTSTR lpDir);
DWORD GetClientType();
BOOL  NAVDataDirectoryExists();

    HRESULT hr          = S_OK;
    IScanDialogs *pScan = NULL;

    // get and IScanDialogs Interface 
    hr=CoCreateInstance(CLSID_ScanDialogs, NULL, CLSCTX_INPROC_SERVER,
                        IID_IScanDialogs, (LPVOID*)&pScan);
    
    if (SUCCEEDED(hr))
    {
        // See if a private user data directory is needed and does it already exist.
        if ( IsUserDirNeeded() && !NAVDataDirectoryExists() )
        {
            // Need to create it
            CreateNAVDataDirectory();
        }

        hr = pScan->ScanDropFiles((long)m_hDrop);
        pScan->Release();
    }

    return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
bool CVpshellEx::IsRtvScanRunning () const
{
    bool bRunning = false;
	HKEY hMain;

    if (ERROR_SUCCESS == RegOpenKeyEx (HKEY_LOCAL_MACHINE, szReg_Key_Main,
                                       0, KEY_QUERY_VALUE, &hMain))
    {
        DWORD dwSize    = sizeof(DWORD);
        DWORD dwRunning = 0;

        //Read the ServiceRunning value
        if (SymSaferRegQueryValueEx (hMain, szReg_Val_ServiceRunning, NULL, NULL,
                             (BYTE*)&dwRunning, &dwSize) == ERROR_SUCCESS )
        {
            bRunning = (dwRunning == KEYVAL_NOTRUNNING)? false : true;
        }
        RegCloseKey (hMain);
    }
    return bRunning;
}

/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CVpshellEx::InvokeCommand(LPCMINVOKECOMMANDINFO lpici)
{
    // Do the scan when RtvScan is already running.
    if (IsRtvScanRunning())
    {
        return ActuallyScan (lpici);
    }
    // Else, display an 'start RtvScan' error message when it is not running.
    else
    {
        CComBSTR bstrTitle;
        CComBSTR bstrMsg;

        bstrTitle.LoadString(IDS_NO_RTVSCAN_TITLE);
        bstrMsg  .LoadString(IDS_NO_RTVSCAN_DESC);

        ::MessageBox(NULL, CW2CT(bstrMsg), CW2CT(bstrTitle), MB_OK|MB_ICONERROR);
    }
        
    return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CVpshellEx::GetCommandString(UINT idCmd,
                                UINT        uType,
                                UINT      * pwReserved,
                                LPSTR       pszName,
                                UINT        cchMax)
{

    TCHAR sText[512];
    ::LoadString(_Module.GetResourceInstance(),IDS_VIRUS_DESC,sText,MAX_PATH);

    
    // if running under NT Convert to unicode
    if (m_bNT)
        MultiByteToWideChar(CP_ACP, 0,sText, -1,(LPWSTR)pszName,cchMax);
    else
        lstrcpyn(pszName,sText,cchMax);
    return S_OK;
}




/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CVpshellEx::AddPages(LPFNADDPROPSHEETPAGE lpfnAddPage, LPARAM lParam)
{

    HRESULT hr = S_OK;
    IScanDialogs *pScan;

    int PageType=CONFIG_TYPE_IDLE;

    if (m_hDrop)
        PageType= CONFIG_TYPE_MANUAL;
    


    hr=CoCreateInstance(CLSID_ScanDialogs, 
                     NULL,
                     CLSCTX_INPROC_SERVER,
                     IID_IScanDialogs,
                     (LPVOID*)&pScan);


    SAVASSERT(hr==S_OK);

    
    if (hr==S_OK)
    {
        HPROPSHEETPAGE hPage;
        hr = pScan->CreateConfigPage(PageType,(LONG*)&hPage);
        SAVASSERT(hr==S_OK);
        if (hr==S_OK)
            lpfnAddPage(hPage ,lParam);

        pScan->Release();

    }

    return S_OK;
}



/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CVpshellEx::ReplacePage(UINT uPageID, LPFNADDPROPSHEETPAGE lpfnReplaceWith, LPARAM lParam)
{
    return S_OK;
}



///////////////////////////////////////////////////////////////////////////////
//
// Function name: NAVDataDirectoryExists
//
// Description  : Load the shfolder DLL and calls SHGetFolderPath()
//
// Parameters   : LPTSTR lpSystemDataDir - buffer to hold the directory 
//                       name returned by SHGetFolderPath(). We use
//                       CSIDL_LOCAL_APPDATA to get the non-roaming
//                       user directory. 
//
// Return Values: ERROR_SUCCESS or other standard error code
//
///////////////////////////////////////////////////////////////////////////////
// 3/12/2000 -   TCASHIN: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
BOOL NAVDataDirectoryExists()
{
    BOOL            bDirExists = FALSE;
    DWORD           dwError = ERROR_SUCCESS;
    TCHAR           szTemp1[IMAX_PATH] = {0};
    TCHAR           szTemp2[IMAX_PATH] = {0};
    HANDLE          hDir = NULL;
    WIN32_FIND_DATA finddata = {0};


    if ( IsWinNT() )
    {
        dwError = GetSystemDataDir( szTemp1 );

        if (dwError == ERROR_SUCCESS )
        {
            sssnprintf( szTemp2, sizeof(szTemp2), "%s\\%s\\%s\\%s", szTemp1, 
                SYMANTEC_COMMON_DATA_DIR, SHARED_VERSION_DATA_DIRECTORY ,_T("Logs")  );

            hDir = FindFirstFile( szTemp2, &finddata );

            if ( hDir && hDir != INVALID_HANDLE_VALUE )
            {
                bDirExists = TRUE;
                FindClose( hDir );
            }
        }
    }
    
    return bDirExists;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function name: CreateNAVDataDirectory
//
// Description  : Makes the NAV directory structure to the users private data
//                (currently just logs).
//
// Return Values: ERROR_SUCCESS if the directory structure is created successfully
//
///////////////////////////////////////////////////////////////////////////////
// 3/12/2000 -   TCASHIN: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
DWORD CreateNAVDataDirectory()
{
    DWORD           dwReturn = ERROR_SUCCESS;
    DWORD           dwClientType = 0;
    TCHAR           szTemp1[IMAX_PATH] = {0};
    TCHAR           szTemp2[IMAX_PATH] = {0};


    dwClientType = GetClientType();

    // Have to be on a WinNT machine
    if ( IsWinNT() )
    {
        // Get the path to the the users non-roaming directory.
        dwReturn = GetSystemDataDir( szTemp1 );

        if (dwReturn == ERROR_SUCCESS )
        {
            // "Symantec"
            sssnprintf( szTemp2, sizeof(szTemp2), "%s\\%s", szTemp1, SYMANTEC_COMPANY_NAME );
            if ( !CreateDirectory(szTemp2, NULL) )
            {
                dwReturn = GetLastError();

                if ( dwReturn != ERROR_ALREADY_EXISTS )
                    goto All_Done;
            }

            _tcscpy(szTemp1, szTemp2);

            // "Symantec Antivirus Corporate Edition"
            sssnprintf( szTemp2, sizeof(szTemp2), "%s\\%s", szTemp1, NAVCORP_DIRECTORY_NAME );
            if ( !CreateDirectory(szTemp2, NULL) )
            {
                dwReturn = GetLastError();

                if ( dwReturn != ERROR_ALREADY_EXISTS )
                    goto All_Done;
            }

            _tcscpy(szTemp1, szTemp2);

            // "7.5" or whatever version
            sssnprintf( szTemp2, sizeof(szTemp2), "%s\\%s", szTemp1, SHARED_VERSION_DATA_DIRECTORY );
            if ( !CreateDirectory(szTemp2, NULL) )
            {
                dwReturn = GetLastError();

                if ( dwReturn != ERROR_ALREADY_EXISTS )
                    goto All_Done;            }

            _tcscpy(szTemp1, szTemp2);

            // And the "Logs" directory
            sssnprintf( szTemp2, sizeof(szTemp2), "%s\\%s", szTemp1, _T("Logs") );
            if ( !CreateDirectory(szTemp2, NULL) )
            {
                if ( dwReturn != ERROR_ALREADY_EXISTS )
                    goto All_Done;
            }

            dwReturn = ERROR_SUCCESS;
        }
    }
    else
    {
        dwReturn = ERROR_NOT_SUPPORTED;
    }

All_Done:

    return dwReturn;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: GetSystemDataDir
//
// Description  : Load the shfolder DLL and calls SHGetFolderPath(). We use
//                CSIDL_LOCAL_APPDATA to get the non-roaming user directory. 
//
// Parameters   : LPTSTR lpSystemDataDir - buffer to hold the directory 
//                       name returned by SHGetFolderPath(). 
//
// Return Values: ERROR_SUCCESS or other standard error code
//
///////////////////////////////////////////////////////////////////////////////
// 3/12/2000 -   TCASHIN: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
DWORD GetSystemDataDir(LPTSTR lpSystemDataDir)
{
    auto PFNSHGETFOLDERPATH pfnSHGetFolderPath = NULL;

    auto DWORD          dwError = ERROR_SUCCESS;
    auto DWORD          dwClientType = 0;
    auto TCHAR          szAppData[MAX_PATH] = {0};
    auto TCHAR          szSystemDir[MAX_PATH] = {0};
    auto TCHAR          szSHFolderDLL[MAX_PATH] ={0};
    auto HINSTANCE      hFolderDLL = NULL;
    auto HRESULT        hr ;
    auto int            len = 0;


    if ( !lpSystemDataDir )
    {
        return ERROR_INVALID_PARAMETER;
    }

    dwClientType = GetClientType();

    // Have to be on a WinNT machine
    if ( IsWinNT() )
    {
        // Get the system directory
        if ( GetSystemDirectory(szSystemDir, sizeof(szSystemDir)) )
        {
            // Append the DLL name
            sssnprintf ( szSHFolderDLL, sizeof(szSHFolderDLL), _T("%s\\shfolder.dll"), szSystemDir );

            // Load it.
            hFolderDLL = LoadLibrary( szSHFolderDLL );

            if ( hFolderDLL )
            {
                // Get the function
                pfnSHGetFolderPath = (PFNSHGETFOLDERPATH)GetProcAddress( hFolderDLL, _T("SHGetFolderPathA") );

                if ( pfnSHGetFolderPath )
                {
                    if ( IsWindows2000() )
                    {
                        // Per machine, specific user, non-roaming, create it here
                        hr = pfnSHGetFolderPath( NULL, CSIDL_LOCAL_APPDATA|CSIDL_FLAG_CREATE, 
                                                 NULL, 0, szAppData);
                    }
                    else
                    {
                        // Per machine, specific user, non-roaming, create it here
                        hr = pfnSHGetFolderPath( NULL, CSIDL_APPDATA|CSIDL_FLAG_CREATE, 
                                                 NULL, 0, szAppData);
                    }
                    
                    if ( SUCCEEDED(hr) )
                    {
                        _tcscpy( lpSystemDataDir, szAppData ); 
                    }
                    else
                        dwError = GetLastError();
                }
                FreeLibrary( hFolderDLL );
            }
            else
                dwError = GetLastError();
        }
        else
            dwError = GetLastError();
    }
    else
    {
        // Not Windows 2000.
        dwError = ERROR_NOT_SUPPORTED;
    }

    return dwError;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: IsWinNT
//
// Description  : Checks to see if the operating system is Windows NT
//
// Return Values: TRUE is the OS is WinNT
//
///////////////////////////////////////////////////////////////////////////////
// 3/12/2000 -   TCASHIN: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
BOOL IsWinNT()
{
    OSVERSIONINFO   osInfo;
    BOOL            bRet = FALSE;

    osInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO); 

    if ( GetVersionEx (&osInfo) )
    {
        if ( osInfo.dwPlatformId == VER_PLATFORM_WIN32_NT )
        {   
            bRet = TRUE;
        }
    }

    return bRet;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: IsWindows2000
//
// Description  : Checks to see if the operating system is Windows 2000
//
// Return Values: TRUE is the OS is Windows 2000
//
///////////////////////////////////////////////////////////////////////////////
// 3/12/2000 -   TCASHIN: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
BOOL IsWindows2000()
{
    OSVERSIONINFO   osInfo;
    BOOL            bRet = FALSE;

    osInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO); 

    if ( GetVersionEx (&osInfo) )
    {
        if ( osInfo.dwMajorVersion >= 5 )
        {   
            bRet = TRUE;
        }
    }

    return bRet;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: GetClientType
//
// Description  : Returns the client type
//
// Return Values: 0 if we fail, otherwise 1 or 2 for clients, 4 for server
//
///////////////////////////////////////////////////////////////////////////////
// 3/12/2000 -   TCASHIN: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
DWORD GetClientType()
{
    DWORD           dwClientType = 0;
    CRegKey         reg;

    if( ERROR_SUCCESS == reg.Open( HKEY_LOCAL_MACHINE, szReg_Key_Main, KEY_READ ) )
    {
        reg.QueryDWORDValue( szReg_Val_Client_Type, dwClientType );
        reg.Close();
    }

    return dwClientType;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: IsUserDirNeeded
//
// Description  : Are we a LPVP client on a Windows 2000 machine? Then we
//                need to create private user directories.
//
// Return Values: TRUE is user dirs are needed
//
///////////////////////////////////////////////////////////////////////////////
// 3/12/2000 -   TCASHIN: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
BOOL IsUserDirNeeded()
{
    DWORD dwClientType = 0;

    dwClientType = GetClientType();

    if ( IsWinNT() )
        return TRUE;
    else
        return FALSE;

}
