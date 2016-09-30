#define IDOFFSET_NAVNT              0x00   //menu offset

//key names to share the maped files
#define WNDD_SHAREMEM_DATA_NAME     "SYMCP4_WNDD_DATA_%lx"
#define EVEN_OBEJECT_NAME           "FM4_SpawnData_%lx"
#define CMD_LINE_ARG                "/HMEM=%lx"

//NavNT class name
#define NAV_MAIN_WND_CLASS      "nav"

// {067DF822-EAB6-11cf-B56E-00A0244D5087}
DEFINE_GUID (CLSID_ShellExtension, 0x67df822, 0xeab6, 0x11cf, 0xb5, 0x6e, 0x0, 0xa0, 0x24, 0x4d, 0x50, 0x87);

//
//CMemShare creates a memory map file and shres that file with onother process
//
class CMemShare
{
public:
    CMemShare();
    ~CMemShare();
    LPTSTR SetMem(DWORD dwMemSize, LONG& MapKey);

private:
    HANDLE m_hFileMap;
    LPTSTR m_lpS;
};

//
// CClassFactory defines a shell extension class factory object.
//
class CClassFactory : public IClassFactory
{
protected:
    ULONG   m_cRef;         // Object reference count

public:
    CClassFactory ();
    ~CClassFactory();

    // IUnknown methods
    STDMETHODIMP         QueryInterface (REFIID, LPVOID FAR *);
    STDMETHODIMP_(ULONG) AddRef ();
    STDMETHODIMP_(ULONG) Release ();

    // IClassFactory methods
    STDMETHODIMP CreateInstance (LPUNKNOWN, REFIID, LPVOID FAR *);
    STDMETHODIMP LockServer (BOOL);
};

//
// CShellExtension defines a context menu shell extension object.
//
class CShellExtension : public IContextMenu, IShellExtInit
{
protected:
    ULONG     m_cRef;                 // Object reference count
    TCHAR     m_szFile[MAX_PATH];     // File name
    LONG      m_MapKey;
    CMemShare m_MemMap;
    HANDLE    m_hEventDataReady;  //the even handle

public:
    CShellExtension ();
    ~CShellExtension();

    // IUnknown methods
    STDMETHODIMP         QueryInterface (REFIID, LPVOID FAR *);
    STDMETHODIMP_(ULONG) AddRef ();
    STDMETHODIMP_(ULONG) Release ();

    // IContextMenu methods
    STDMETHODIMP QueryContextMenu (HMENU hMenu, UINT indexMenu, UINT idCmdFirst, UINT idCmdLast, UINT uFlags);
    STDMETHODIMP InvokeCommand (LPCMINVOKECOMMANDINFO lpcmi);
    STDMETHODIMP GetCommandString (UINT idCmd, UINT uFlags, UINT FAR *reserved, LPSTR pszName, UINT cchMax);

    // IShellExtInit method
    STDMETHODIMP Initialize (LPCITEMIDLIST pidlFolder, LPDATAOBJECT lpdobj, HKEY hKeyProgID);
};

//
// CRunProcess gets filename and path from the registry; creats process and runs it
//
class CRunProcess
{
public:
    DWORD RunProcess(TCHAR* lpCmdLine);

private:
    void GetFileName(TCHAR* szTempW, int size);  //retuns the path + file name
    void GetEnv(TCHAR* szTempW, int size);
};
