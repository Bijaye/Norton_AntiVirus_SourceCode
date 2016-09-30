// Copyright 1992-1993 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/NAVSHELL/VCS/navshell.cpv   1.3   21 Jul 1997 01:38:44   BGERHAR  $
//
// Description:
//      This file is a inproc-server and provides the functionality
//      for the right mouse click and shellex API to support
//
//
// Contains (as exports):
//
// See Also:

//************************************************************************
// $Log:   S:/NAVSHELL/VCS/navshell.cpv  $
// 
//    Rev 1.3   21 Jul 1997 01:38:44   BGERHAR
// Now always use /HMEM so NAVW32 will display completion status
// 
//    Rev 1.2   19 Jun 1997 11:16:06   TIVANOV
// new function to get the process environment 
//
//    Rev 1.1   09 Apr 1997 14:54:46   TIVANOV
// windows version detection and some code changes related to win95 and winNT
//
//    Rev 1.0   06 Feb 1997 21:00:14   RFULLER
// Initial revision
//
//    Rev 1.1   13 Sep 1996 10:57:28   TIVANOV
// port changes form gryphone
//************************************************************************

#define INC_OLE2

#include "platform.h"
#include <initguid.h>
#include <shlobj.h>
#include <malloc.h>
#include <fcntl.h>
#include <time.h>

#include "navshell.h"
#include "resource.h"

#define WINNT 0
#define WIN95 1

//
// Global variables
//
UINT        g_cRefThisDll = 0;          // Reference count for this DLL
HINSTANCE   g_hInstance;                // Instance handle for this DLL
int         g_winVersion;

//
// DllMain is the DLL's entry point.
//
// Input parameters:
//   hInstance  = Instance handle
//   dwReason   = Code specifying the reason DllMain was called
//   lpReserved = Reserved (do not use)
//
// Returns:
//   TRUE if successful, FALSE if not
//

void GetWinVersion()
{
    DWORD dwBuild, dwVersion = GetVersion();

    // Get major and minor version numbers of Windows

    DWORD dwWindowsMajorVersion =  (DWORD)(LOBYTE(LOWORD(dwVersion)));
    DWORD dwWindowsMinorVersion =  (DWORD)(HIBYTE(LOWORD(dwVersion)));

    // Get build numbers for Windows NT or Win32s

    if (dwVersion < 0x80000000)                // Windows NT
    {
        dwBuild = (DWORD)(HIWORD(dwVersion));
        g_winVersion = WINNT;
    }
    else if (dwWindowsMajorVersion < 4)        // Win32s
        dwBuild = (DWORD)(HIWORD(dwVersion) & ~0x8000);
    else         // Windows 95 -- No build numbers provided
    {
        g_winVersion = WIN95;
    }
}

extern "C" int APIENTRY DllMain (HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
    //
    // If dwReason is DLL_PROCESS_ATTACH, save the instance handle so it
    // can be used again later.
    //
    if (dwReason == DLL_PROCESS_ATTACH)
        g_hInstance = hInstance;

    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// In-process server functions

//
// DllGetClassObject is called by the shell to create a class factory object.
//
// Input parameters:
//   rclsid = Reference to class ID specifier
//   riid   = Reference to interface ID specifier
//   ppv    = Pointer to location to receive interface pointer
//
// Returns:
//   HRESULT code signifying success or failure
//

STDAPI DllGetClassObject (REFCLSID rclsid, REFIID riid, LPVOID *ppv)
{
    *ppv = NULL;

    //
    // Make sure the class ID is CLSID_ShellExtension. Otherwise, the class
    // factory doesn't support the object type specified by rclsid.
    //
    if (!IsEqualCLSID (rclsid, CLSID_ShellExtension))
        return ResultFromScode (CLASS_E_CLASSNOTAVAILABLE);

    //
    // Instantiate a class factory object.
    //
    CClassFactory *pClassFactory = new CClassFactory ();
    if (pClassFactory == NULL)
        return ResultFromScode (E_OUTOFMEMORY);

    //
    // Get the interface pointer from QueryInterface and copy it to *ppv.
    //
    HRESULT hr = pClassFactory->QueryInterface (riid, ppv);
    pClassFactory->Release ();

    //get the windows version
    GetWinVersion();

    return hr;
}

//
// DllCanUnloadNow is called by the shell to find out if the DLL can be
// unloaded. The answer is yes if (and only if) the module reference count
// stored in g_cRefThisDll is 0.
//
// Input parameters:
//   None
//
// Returns:
//   HRESULT code equal to S_OK if the DLL can be unloaded, S_FALSE if not
//

STDAPI DllCanUnloadNow (void)
{
    return ResultFromScode ((g_cRefThisDll == 0) ? S_OK : S_FALSE);
}

/////////////////////////////////////////////////////////////////////////////
// CClassFactory member functions

CClassFactory::CClassFactory ()
{
    m_cRef = 1;
    g_cRefThisDll++;
}

CClassFactory::~CClassFactory ()
{
    g_cRefThisDll--;
}

STDMETHODIMP CClassFactory::QueryInterface (REFIID riid, LPVOID FAR *ppv)
{
    if (IsEqualIID (riid, IID_IUnknown))
    {
        *ppv = (LPUNKNOWN) (LPCLASSFACTORY) this;
        m_cRef++;
        return NOERROR;
    }
    else if (IsEqualIID (riid, IID_IClassFactory))
    {
        *ppv = (LPCLASSFACTORY) this;
        m_cRef++;
        return NOERROR;
    }
    else
    {
        *ppv = NULL;
        return ResultFromScode (E_NOINTERFACE);
    }
}

STDMETHODIMP_(ULONG) CClassFactory::AddRef ()
{
    return ++m_cRef;
}

STDMETHODIMP_(ULONG) CClassFactory::Release ()
{
    if (--m_cRef == 0)
        delete this;
    return m_cRef;
}

//
// CreateInstance is called by the shell to create a shell extension object.
//
// Input parameters:
//   pUnkOuter = Pointer to controlling unknown
//   riid      = Reference to interface ID specifier
//   ppvObj    = Pointer to location to receive interface pointer
//
// Returns:
//   HRESULT code signifying success or failure
//

STDMETHODIMP CClassFactory::CreateInstance (LPUNKNOWN pUnkOuter, REFIID riid, LPVOID FAR *ppvObj)
{
    *ppvObj = NULL;

    //
    // Return an error code if pUnkOuter is not NULL, because we don't
    // support aggregation.
    //
    if (pUnkOuter != NULL)
        return ResultFromScode (CLASS_E_NOAGGREGATION);

    //
    // Instantiate a shell extension object.
    //
    CShellExtension *pShellExtension = new CShellExtension ();

    if (pShellExtension == NULL)
        return ResultFromScode (E_OUTOFMEMORY);

    //
    // Get the interface pointer from QueryInterface and copy it to *ppvObj.
    //
    HRESULT hr = pShellExtension->QueryInterface (riid, ppvObj);
    pShellExtension->Release ();
    return hr;
}

//
// LockServer increments or decrements the DLL's lock count.
//

STDMETHODIMP CClassFactory::LockServer (BOOL fLock)
{
    return ResultFromScode (E_NOTIMPL);
}

/////////////////////////////////////////////////////////////////////////////
// CShellExtension member functions

CShellExtension::CShellExtension ()
{
    m_cRef = 1;
    m_szFile[0] = 0;
    time(&m_MapKey);   //get the current time as a unique ID for the file mapping key
    g_cRefThisDll++;
}

CShellExtension::~CShellExtension ()
{
    g_cRefThisDll--;
}

STDMETHODIMP CShellExtension::QueryInterface (REFIID riid, LPVOID FAR *ppv)
{
    if (IsEqualIID (riid, IID_IUnknown))
    {
        *ppv = (LPUNKNOWN) (LPCONTEXTMENU) this;
        m_cRef++;
        return NOERROR;
    }
    else if (IsEqualIID (riid, IID_IContextMenu))
    {
        *ppv = (LPCONTEXTMENU) this;
        m_cRef++;
        return NOERROR;
    }
    else if (IsEqualIID (riid, IID_IShellExtInit))
    {
        *ppv = (LPSHELLEXTINIT) this;
        m_cRef++;
        return NOERROR;
    }
    else
    {
        *ppv = NULL;
        return ResultFromScode (E_NOINTERFACE);
    }
}

STDMETHODIMP_(ULONG) CShellExtension::AddRef ()
{
    return ++m_cRef;
}

STDMETHODIMP_(ULONG) CShellExtension::Release ()
{
    if (--m_cRef == 0)
        delete this;
    return m_cRef;
}

//
// QueryContextMenu is called before a context menu is displayed so the
// extension handler can add items to the menu.
//
// Input parameters:
//   hMenu      = Context menu handle
//   indexMenu  = Index for first new menu item
//   idCmdFirst = Item ID for first new menu item
//   idCmdLast  = Maximum menu item ID that can be used
//   uFlags     = Flags
//
// Returns:
//   HRESULT code signifying success or failure. If successful, the 'code'
//   field specifies the number of items added to the context menu.
//

STDMETHODIMP CShellExtension::QueryContextMenu (HMENU hMenu, UINT indexMenu, UINT idCmdFirst, UINT idCmdLast, UINT uFlags)
{
    //
    // Add new menu items to the context menu.
    //

    TCHAR b[100];
    LoadString(g_hInstance, IDS_POPUP_MENU, b, sizeof (b));
    ::InsertMenu (hMenu, indexMenu++, MF_STRING | MF_BYPOSITION, idCmdFirst + IDOFFSET_NAVNT, b);

    return ResultFromScode (MAKE_SCODE (SEVERITY_SUCCESS, 0, USHORT (IDOFFSET_NAVNT + 1)));
}

//
// InvokeCommand is called when a menu item added by the extension handler
// is selected from a NAV file's context menu.
//
// Input parameters:
//   lpcmi = Pointer to CMINVOKECOMMAND structure
//
// Returns:
//   HRESULT code signifying success or failure
//

STDMETHODIMP CShellExtension::InvokeCommand (LPCMINVOKECOMMANDINFO lpcmi)
{
    //
    // Return an error code if we've been called programmatically or
    // lpcmi->lpVerb contains an invalid offset.
    //

    if (HIWORD (lpcmi->lpVerb))
        return ResultFromScode (E_FAIL);

    if (LOWORD (lpcmi->lpVerb) > IDOFFSET_NAVNT)
        return ResultFromScode (E_INVALIDARG);

    //
    // Execute the command specified by lpcmi->lpVerb.
    //
    switch (LOWORD (lpcmi->lpVerb))
    {
        case IDOFFSET_NAVNT:
            //check if Nav is running, if not then do it
            if (FindWindow(TEXT(NAV_MAIN_WND_CLASS), NULL))
            {
                TCHAR b[200], c[30];

                if (g_winVersion == WIN95)
                    LoadString(g_hInstance, IDS_NAV_RUNNING_95,  b, sizeof (b));
                else
                    LoadString(g_hInstance, IDS_NAV_RUNNING_NT,  b, sizeof (b));

                LoadString(g_hInstance, IDS_MSGBOX_TITLE, c, sizeof (c));

                MessageBox(NULL, b, c, MB_ICONINFORMATION | MB_TASKMODAL | MB_OK);
            }
            else
            {
                //call NAV with the command line parameters
                CRunProcess run;

                if (m_MapKey)
                    {
                    //create the synx-even for releasing the memory
                    TCHAR szName[50];
                    wsprintf(szName, TEXT(EVEN_OBEJECT_NAME), m_MapKey);
                    m_hEventDataReady = CreateEvent(NULL, FALSE, FALSE, szName);

                    //create the command line
                    wsprintf(szName, TEXT(CMD_LINE_ARG), m_MapKey);

                    run.RunProcess(szName);

                    WaitForSingleObject(m_hEventDataReady, INFINITE);
                    CloseHandle(m_hEventDataReady);
                    }
            }
            break;
    }

    return NOERROR;
}

//
// GetCommandString is called to retrieve a string of help text or a
// language-independent command string for an item added to the context
// menu.
//
// Input parameters:
//   idCmd    = 0-based offset of menu item identifier
//   uFlags   = Requested information type
//   reserved = Pointer to reserved value (do not use)
//   pszName  = Pointer to buffer to receive the string  - the string must be bellow 40 chars
//   cchMax   = Buffer size
//
// Returns:
//   HRESULT code signifying success or failure
//

STDMETHODIMP CShellExtension::GetCommandString (UINT idCmd, UINT uFlags, UINT FAR *reserved, LPSTR pszName, UINT cchMax)
{
    //
    // Return an error code if idCmd contains an invalid offset.
    //
    if (idCmd > IDOFFSET_NAVNT)
        return ResultFromScode (E_INVALIDARG);

    //
    // Copy the requested string to the caller's buffer.
    //
    switch (idCmd)
    {
        case IDOFFSET_NAVNT:
            {
                if (g_winVersion == WIN95)
                    LoadString(g_hInstance, IDS_STATUS_BAR, pszName, cchMax);
                else
                {
                    TCHAR b[100];
                    LoadString(g_hInstance, IDS_STATUS_BAR, b, sizeof (b));

                    //convert to wide caracters
                    MultiByteToWideChar (CP_ACP, MB_PRECOMPOSED, b, -1, (LPWSTR)pszName, cchMax);
                }
            }
            break;
    }
    return NOERROR;
}

//
// Initialize is called by the shell to initialize a shell extension.
//
// Input parameters:
//   pidlFolder = Pointer to ID list identifying parent folder
//   lpdobj     = Pointer to IDataObject interface for selected object(s)
//   hKeyProgId = Registry key handle
//
// Returns:
//   HRESULT code signifying success or failure
//

STDMETHODIMP CShellExtension::Initialize (LPCITEMIDLIST pidlFolder, LPDATAOBJECT lpdobj, HKEY hKeyProgID)
{
    STGMEDIUM medium;
    FORMATETC fe = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };

    //
    // Fail the call if lpdobj is NULL.
    //
    if (lpdobj == NULL)
        return ResultFromScode (E_FAIL);

    //
    // Render the data referenced by the IDataObject pointer to an HGLOBAL
    // storage medium in CF_HDROP format.
    //
    HRESULT hr = lpdobj->GetData (&fe, &medium);
    if (FAILED (hr))
        return ResultFromScode (E_FAIL);

    //get the number of selected files
    UINT uFileNum = DragQueryFile ((HDROP) medium.hGlobal, 0xFFFFFFFF, NULL, 0);

    // Always use /HMEM -- NAVW uses this switch to display "no viruses" dlg

    //increase the size of each file name with 3 for the double quotes and the finishing NULL
    DWORD d = GlobalSize(medium.hGlobal) + uFileNum * 3 + 2;
    LPTSTR lp = m_MemMap.SetMem(d, m_MapKey);

    if (lp)
    {
        LPDWORD lpDW = (LPDWORD)lp;
        //get the string len and put it in the first word
        *lpDW = d + 1;

        //reserve the first 4 bytes for the string length
        lp = (LPTSTR)(lpDW + 1);

        //create a loop and get all the files
        //create the list with the following syntax - "FileName" "FileName"\0
        for (UINT i = 0; i < uFileNum; i++)
        {
            STRCAT(lp, TEXT("\""));
            DragQueryFile ((HDROP) medium.hGlobal, i, lp + STRLEN(lp), MAX_PATH);
            STRCAT(lp, TEXT("\" "));
        }
    }

    //wait here for a few seconds
    ReleaseStgMedium (&medium);
    return NOERROR;
}

//Get the path and the file name from the
void CRunProcess::GetFileName(TCHAR* szTempW, int size)
{
    HKEY hKey = 0;
    //MEMSET (szTempW, 0, size);
    memset (szTempW, 0, size);

    TCHAR b[100];
    LoadString(g_hInstance, IDS_REGKEY_APPS, b, sizeof (b));

    if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, b, 0, KEY_READ, &hKey))
    {
        if (g_winVersion == WIN95)
            LoadString(g_hInstance, IDS_REGKEY_APP_95, b, sizeof (b));
        else
            LoadString(g_hInstance, IDS_REGKEY_APP_NT, b, sizeof (b));

        if (ERROR_SUCCESS == RegQueryValueEx(hKey, b, 0, 0, (LPBYTE)szTempW, (LPDWORD)&size))
        {
            // Get Unicode string for command line.
            if (g_winVersion == WIN95)
                LoadString(g_hInstance, IDS_APP_NAME_95, b, sizeof (b));
            else
                LoadString(g_hInstance, IDS_APP_NAME_NT, b, sizeof (b));

            STRCAT(szTempW, b);

            RegCloseKey(hKey);
        }
    }
}

//Get the path and the file name from the
void CRunProcess::GetEnv(TCHAR* szTempW, int size)
{
    HKEY hKey = 0;
    memset (szTempW, 0, size);

    TCHAR b[100];

    if (g_winVersion == WIN95)
        LoadString(g_hInstance, IDS_NAV_ENV_95, b, sizeof (b));
    else
        LoadString(g_hInstance, IDS_NAV_ENV_NT, b, sizeof (b));

    if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, b, 0, KEY_READ, &hKey))
    {
        LoadString(g_hInstance, IDS_NAV_ENV, b, sizeof (b));
        LoadString(g_hInstance, IDS_NAV_PROCESSPATH, szTempW, size);
        if (ERROR_SUCCESS != RegQueryValueEx(hKey, b, 0, 0, (LPBYTE)szTempW+STRLEN(szTempW), (LPDWORD)&size))
        {
            RegCloseKey(hKey);

            GetEnv(szTempW, size); //call again if the function fails the first time
            return;
        }
        RegCloseKey(hKey);
    }
}

DWORD CRunProcess::RunProcess(TCHAR* lpCmdLine)
{
    DWORD dwExitCode = 0;
    SECURITY_ATTRIBUTES  saThread, saProcess;
    PROCESS_INFORMATION  ProcessInformation;
    STARTUPINFO          si;
    TCHAR szTempW[1024];
    TCHAR szOptionsW[64];

    // Only if we get the default desktop name do the following
    memset (&si,        0, sizeof (si));
    memset (&saProcess, 0, sizeof (saProcess));
    memset (&saThread,  0, sizeof (saThread));

    if (g_winVersion == WINNT)
    {
        SECURITY_DESCRIPTOR  sdSec;

        InitializeSecurityDescriptor (&sdSec, SECURITY_DESCRIPTOR_REVISION);
        SetSecurityDescriptorDacl (&sdSec, TRUE, (PACL) NULL, FALSE);

        si.lpDesktop = TEXT("WinSta0\\Default");
        saThread.lpSecurityDescriptor  = &sdSec;
        saProcess.lpSecurityDescriptor = &sdSec;
    }
    else
    {
        si.lpDesktop = NULL;
        saThread.lpSecurityDescriptor  = NULL;
        saProcess.lpSecurityDescriptor = NULL;
    }

    // Set the startup Info.
    si.cb = sizeof (si);
    si.dwFlags = 0;
    si.wShowWindow = SW_SHOWNORMAL;

    saProcess.nLength = sizeof (saProcess);
    saProcess.bInheritHandle = TRUE;

    saThread.nLength = sizeof (saThread);
    saThread.bInheritHandle = TRUE;

    GetFileName(szTempW, sizeof(szTempW) / sizeof(szTempW[0]));

    STRCAT (szTempW, TEXT(" "));
    STRCAT (szTempW, lpCmdLine);

    szOptionsW[0] = '\0';
    LoadString(g_hInstance, IDS_NAV_OPTIONS, szOptionsW, sizeof (szOptionsW));

    if (szOptionsW[0] != '\0')
        {
        STRCAT (szTempW, TEXT(" "));
        STRCAT (szTempW, szOptionsW);
        }

    //load the environment
    TCHAR env[1024] = {0};
    GetEnv(env, sizeof(env) / sizeof(env[0]));

    dwExitCode = CreateProcess ( NULL,
                        szTempW,
                        &saProcess,
                        &saThread,
                        TRUE,
                        HIGH_PRIORITY_CLASS | CREATE_DEFAULT_ERROR_MODE | CREATE_NEW_PROCESS_GROUP,
                        env,
                        NULL,
                        &si,
                        &ProcessInformation);
    if (!dwExitCode)
    {
        switch (GetLastError())
        {
            case ERROR_FILE_NOT_FOUND:
            case ERROR_PATH_NOT_FOUND:
            case ERROR_INVALID_NAME:
            {
                TCHAR b[200], c[80];
                if (g_winVersion == WIN95)
                    LoadString(g_hInstance, IDS_NAV_MISSING_95,  b, sizeof (b));
                else
                    LoadString(g_hInstance, IDS_NAV_MISSING_NT,  b, sizeof (b));

                LoadString(g_hInstance, IDS_MSGBOX_TITLE, c, sizeof (c));

                MessageBox(NULL, b, c, MB_ICONINFORMATION | MB_TASKMODAL | MB_OK);
            }
            break;
        }
    }
    return (dwExitCode);
}

//
//Memory share - takes the size of the memory requaried, creats a memory map file
//
CMemShare::CMemShare()
{
    m_hFileMap = NULL;
    m_lpS = NULL;
}

CMemShare::~CMemShare()
{
    // Unmap the view - this also protects the data
    // from wayward pointers.
    UnmapViewOfFile(m_lpS);
    CloseHandle(m_hFileMap);
}

LPTSTR CMemShare::SetMem(DWORD dwMemSize, LONG& MapKey)
{
    // Create an in-memory memory mapped file using the contents of
    // the EDIT control as the contents of the file.
    // The is at most 4K big and has a name of "MMFSharedData".
    TCHAR szName[128];
    wsprintf(szName, TEXT(WNDD_SHAREMEM_DATA_NAME), MapKey);
    m_hFileMap = CreateFileMapping((HANDLE) 0xFFFFFFFF, NULL, PAGE_READWRITE, 0, dwMemSize, szName);

    if ((m_hFileMap != NULL) && (GetLastError() != ERROR_ALREADY_EXISTS))
    {
        // File mapping created successfully.
        // Map a view of the file into the address space.
        m_lpS = (LPTSTR)MapViewOfFile(m_hFileMap, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);
        if (m_lpS != NULL)
        {
            // View mapped successfully, put contents in the buffer
            memset (m_lpS,  0, dwMemSize);
        }
        else
            MapKey = 0;
    }
    else
        MapKey = 0;

    return (m_lpS);
}
