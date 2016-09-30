// SelectionList.cpp: Implementation of the CSelectionList class

#include "StdAfx.h"
#include "platform.h"
#include "Resource.h"
#include "ScanTask.h"
#include "SymInterfaceLoader.h"
#include "SelectionList.h"
#include "switches.h"
#include "AVRESBranding.h"

using namespace std;


#define GetPIDLFolder(pida) (LPCITEMIDLIST)(((LPBYTE)pida)+(pida)->aoffset[0])
#define GetPIDLItem(pida, i) (LPCITEMIDLIST)(((LPBYTE)pida)+(pida)->aoffset[i+1])
class CStrRet : public STRRET  // Cleanup utility class
{
	CComPtr<IMalloc>& m_spMalloc;
	PTCHAR            m_pidl;

public:
	CStrRet(CComPtr<IMalloc>& spMalloc, LPCITEMIDLIST pidl) : m_spMalloc(spMalloc)
	                                                        , m_pidl(PTCHAR(pidl)) {}
	~CStrRet(void)
	{
		// Free memory
		if (STRRET_WSTR == uType)
			m_spMalloc->Free(pOleStr);
	}
	void CopyTo(PTCHAR psz)
	{
		USES_CONVERSION;

		switch(uType)
		{
		case STRRET_WSTR:
			::_tcsncpy(psz, CW2T(pOleStr, CP_ACP), MAX_PATH*2 );
			break;
		case STRRET_CSTR:
			::_tcsncpy(psz, A2T(cStr), MAX_PATH*2 );
			break;
		case STRRET_OFFSET:
			::_tcsncpy(psz, m_pidl + uOffset, MAX_PATH*2 );
			break;
		default:
			*psz = _T('\0');
			break;
		}
	}
	bool CStrRet::operator==(PTCHAR psz)
	{
		USES_CONVERSION;

		return STRRET_WSTR   == uType && !::wcscmp(T2W(psz), pOleStr)
		    || STRRET_CSTR   == uType && !::strcmp(CT2A(psz, CP_ACP), cStr)
		    || STRRET_OFFSET == uType && !::_tcscmp(psz, m_pidl + uOffset);
	}
};

class CStgMedium : public STGMEDIUM  // Cleanup utility class
{
public:
	CStgMedium(void)
	{
		::memset(static_cast<STGMEDIUM*>(this), sizeof(STGMEDIUM), 0);
	}
	~CStgMedium(void)
	{
		Clear();
	}
	void Clear(void)
	{
		if (hGlobal)
			::ReleaseStgMedium(this);
		hGlobal = NULL;
	}
};

class CItemIDListPtr // Cleanup for 
{
	LPITEMIDLIST m_p;
	CComPtr<IMalloc>& m_spMalloc;

public:
	CItemIDListPtr(CComPtr<IMalloc>& spMalloc) : m_spMalloc(spMalloc), m_p(NULL) {}
	~CItemIDListPtr(void) { Clear(); }

	void Clear(void)
	{
		if (m_p)
		{
			if (m_spMalloc)
			{
				// Release system memory
				m_spMalloc->Free(m_p);
			}

			// Mark it as free
			m_p = NULL;
		}
	}
	operator LPCITEMIDLIST(void) { return m_p; }
	LPITEMIDLIST* operator&(void) { return &m_p; }
};

// Shell Clipboard format
static const CF_SHELLIDLIST = ::RegisterClipboardFormat(CFSTR_SHELLIDLIST);

HRESULT CSelectionListBase::GetSelectionList(IDataObject* pDataObj)
{
	HRESULT hr;
	CStgMedium me;
	FORMATETC fe = { CF_SHELLIDLIST, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };

	// Clean previous selections
	m_vItems.clear();

	// Get the files associated with this object, if applicable.
	if(FAILED(hr = pDataObj->GetData(&fe, &me)))
		return hr;

	CComPtr<IShellFolder> spISF;

	 // first time around get a hold of the Desktop
	if (!m_spDesktopFolder && FAILED(hr = ::SHGetDesktopFolder(&m_spDesktopFolder)))
		return hr;

	// Get lte list of selected files form the IShellView
	LPIDA pida = reinterpret_cast<LPIDA>(me.hGlobal);
	LPCITEMIDLIST pidl = GetPIDLFolder(pida);
	// Get the IShellFolder of the parent
	if (FAILED(hr = m_spDesktopFolder->BindToObject(pidl, NULL
	                                                    , IID_IShellFolder
	                                                    , reinterpret_cast<void**>(&spISF))))
	{
		if (E_INVALIDARG != hr)
			return hr;

		// Dhuh! We got an invalid argument error because this PIDL is the desktop itself
		spISF = m_spDesktopFolder;
	}

	 // first time around get a hold of the system allocator
	if (!m_spMalloc && FAILED(hr = ::SHGetMalloc(&m_spMalloc)))
		return hr; 

	// Add all applicable selected object to the list
	for (int i = 0; i < pida->cidl; i++)
	{
		LPCITEMIDLIST pidl = GetPIDLItem(pida, i);

		if (FAILED(hr = Add2List(spISF, pidl)))
			return hr;
	}

	return hr;
}

// GHS: 362840 I am return the reason for the failure if it is NOT ERROR_FILE_EXISTS
DWORD_PTR CSelectionListBase::generateTempScanFile( LPTSTR szOut, int nBufferSize )
{
	TCHAR szTempDir[ MAX_PATH*2 ];
	TCHAR szTempFile[ 32 ];

    DWORD dwLastError = NOERROR;
	// Fetch temp directory.
	if( 0 == ::GetTempPath(MAX_PATH*2, szTempDir) )
    {
        dwLastError = ::GetLastError();
        return (dwLastError == NOERROR)?(DWORD_PTR)(-1):dwLastError;
    }

	// Keep trying to generate a temp file.
	int iIndex = 0;
	for(;; ++iIndex)
	{
		// Copy directory name to output buffer.
		_tcsncpy( szOut, szTempDir, nBufferSize );

		// Construct file name.  File will be in the form
		// scanxxxx.sca.
		wsprintf( szTempFile, "scan%d.%s", iIndex, szTaskFileExtension );
		_tcsncat( szOut, szTempFile, nBufferSize );

		// Try to create the file.
		HANDLE hFile = CreateFile( szOut, GENERIC_WRITE, 0, NULL, CREATE_NEW, 0, NULL );
		if( hFile != INVALID_HANDLE_VALUE )
		{
			// Success.  Close the handle and return.
			CloseHandle( hFile );
			return NOERROR;
		}

		// An error occured.  If the error states that the file
		// already exists, keep processing, else bail out.
        // GHS: 362840 I am return the reason for the failure if it is NOT ERROR_FILE_EXISTS
        DWORD dwLastError = ::GetLastError();
		if( dwLastError != ERROR_FILE_EXISTS )
            return dwLastError;
	}

	// Return error status.
	return NOERROR;
}


// GHS: 362840 I am checking for low disk space and return that as an error when it occurs.
bool _g_cIsDiskFull(LPCTSTR lpcRoot)
{
    
    ULARGE_INTEGER uliFreeBytesAvailable        = {0};  // bytes available to caller
    ULARGE_INTEGER uliTotalNumberOfBytes        = {0};  // bytes on disk
    ULARGE_INTEGER uliTotalNumberOfFreeBytes    = {0};  // free bytes on disk
    
    
    BOOL bRet = ::GetDiskFreeSpaceEx(
        lpcRoot,                 // directory name
        &uliFreeBytesAvailable,    // bytes available to caller
        &uliTotalNumberOfBytes,    // bytes on disk
        &uliTotalNumberOfFreeBytes // free bytes on disk
        );
    
    if(!bRet || (uliFreeBytesAvailable.QuadPart < 1000) )
        return true;

    return false;
}

int CSelectionListBase::RunNAVW(void)
{
    USES_CONVERSION;
    TCHAR szTempFilePath[MAX_PATH*2];

    CSymInterfaceLoader taskLoader;
	IScanTaskPtr pScanTask;

    // Load the scan task object
    TCHAR szTasksDll[MAX_PATH] = {0};
    CNAVInfo NavInfo;
    _stprintf(szTasksDll, _T("%s\\NAVTasks.dll"), NavInfo.GetNAVDir());
    if( SYM_OK != taskLoader.Initialize(szTasksDll) )
        return 0;

    if( SYM_OK != taskLoader.CreateObject(IID_IScanTask, IID_IScanTask, (void**)&pScanTask) )
        return 0;

	// Show inaccessible items.
	ShowDeniels(::GetActiveWindow());

	// Make sure we have items to work with.
	if(m_vItems.size() == 0)
	{
		if(m_csProductName.IsEmpty())
		{
			CBrandingRes BrandRes;
			m_csProductName = BrandRes.ProductName();
		}

		CString csFormat, csMessage;
		csFormat.LoadString(_Module.GetResourceInstance(), IDS_NOITEMS);
		csMessage.Format(csFormat, m_csProductName);

		// Means nothing is selected.
		MessageBox(::GetActiveWindow(), csMessage, m_csProductName, MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	pScanTask->SetType(scanCustom);
	// Save the list of the files to scan into a task file.
	for (vector<TSTRING>::iterator it = m_vItems.begin(); m_vItems.end() != it; it++)
	{
		// Mark everything as a file even folders and drives.
		// Since this will be picked by NAVW.EXE we don't care what type of an item it is.
		// NAVW feed the information to the scanner and it'll figure out what to do.
		if (!pScanTask->AddItem(typeFile, subtypeNone, NULL, (*it).c_str(), 0))
			return IDS_Err_ScanTaskItem;
	}

	// Get a temp file name.
    DWORD_PTR dwError = generateTempScanFile( szTempFilePath, MAX_PATH*2 );
    if(dwError != NOERROR)
		return IDS_Err_ScanTaskFile;
	
    pScanTask->SetPath ( szTempFilePath );
	
	if (!pScanTask->Save())
	{
        DWORD dwLastError = ::GetLastError();
		DeleteFile ( szTempFilePath );

        // Tell the user we couldn't save the task
        //
        int nReturn = 0;
        if(ERROR_DISK_FULL == dwLastError || ERROR_HANDLE_DISK_FULL == dwLastError)
        {
            nReturn = IDS_Err_Cant_Save_Task;
        }
        else
        {
		    TCHAR szRoot[] = _T("?:\\");

			szRoot[0] = szTempFilePath[0];
			nReturn = ( _g_cIsDiskFull(szRoot)?IDS_Err_Cant_Save_Task:IDS_Err_ScanTaskFile );
        }
#ifdef _DEBUG
		CString csError;
		CString csFormat;

        csFormat.LoadString ( nReturn );
		csError.Format(csFormat, m_csProductName);

		if(m_csProductName.IsEmpty())
		{
			CBrandingRes BrandRes;
			m_csProductName = BrandRes.ProductName();
		}
        ::MessageBox ( ::GetDesktopWindow (), csError, m_csProductName, MB_OK | MB_ICONERROR );
#endif
        
        return nReturn;
	}

    // Command line is c:\progra~1\norton~1\navw32.exe /SE- /ttask:"c:\...\task.sca"
    //
    std::string strExePath;
    std::string strParameters;

    CNAVInfo NAVInfo;

    TCHAR szShortNAVDir [MAX_PATH] = {0};
    TCHAR szShortTaskName [MAX_PATH] = {0};

    ::GetShortPathName ( NAVInfo.GetNAVDir (), szShortNAVDir, MAX_PATH );
    ::GetShortPathName ( szTempFilePath, szShortTaskName, MAX_PATH );

    strExePath = szShortNAVDir;
    strExePath += _T("\\navw32.exe");
    strParameters = SWITCH_DISABLE_ERASER_SCAN;
    strParameters += _T(" /ttask:\"");
    strParameters += szShortTaskName;
    strParameters += _T("\"");

	if (HINSTANCE (32) >= ::ShellExecute(   ::GetDesktopWindow(),
                                            _T("open"),         // Verb
	                                        strExePath.c_str(),     // File
                                            strParameters.c_str(),  // Parameters
                                            NULL,                   // Directory
	                                        SW_SHOW))               // ShowCmd
    {
		return IDS_Err_ScanTaskRun;
    }

	return 0;
}

// ---------------------------------------------------------------------------
// Note: This is not a TCHAR routine. We may have to redo it if wchar is needed.
//       Anothr issue of the same problem is that Microsoft did not published the
//       PIF file format. This structure may change in future versions of the OS.
// ---------------------------------------------------------------------------

bool CSelectionListBase::ResolvePifFile(LPSTR pszPath)
{
	struct tagPIFFORMAT
	{
		WORD    wReserved;
		char    byProgramTitle [ 30 ];  // non null terminated
		WORD    wMaxMemToPartition;     // in K
		WORD    wMinMemRequired;        // in K
		char    szProgramName [ 64 ];   // What about LFNs?
		BYTE    byDefaultDriveLetter;
		char    szDefaultDir [ 64 ];
		char    szProgParams [ 64 ];
		BYTE    byInitScreenMode;
		BYTE    byNumTextPages;
		BYTE    byNumFirstSaveInt;
		BYTE    byNumLastSaveInt;
		BYTE    byVirtScreenRows;
		BYTE    byVirtScreenCols;
		BYTE    byInitWindowsRow;
		BYTE    byInitWindowsCol;
		WORD    wSystemMemory;          // in K
		char    szSharedProgName [ 64 ];
		char    szSharedDataFile [ 64 ];
		BYTE    byFlags1;
		BYTE    byFlags2;
	} Pif;


	bool        bReturn = false;
	HANDLE      hFile;

	hFile = ::CreateFile(pszPath, GENERIC_READ            // access mode
	                            , FILE_SHARE_READ         // share mode
	                            , NULL                    // Sec. Desc.
	                            , OPEN_EXISTING           // how to create
	                            , FILE_ATTRIBUTE_READONLY // file attributes
	                            , NULL);                  // handle to template file

	if (INVALID_HANDLE_VALUE != hFile)
	{
		DWORD dwRead;

		if (::ReadFile(hFile, &Pif, sizeof(Pif), &dwRead, NULL) && sizeof(Pif) == dwRead)
		{
			// -----------------------------------
			// Better safe than sorry
			// -----------------------------------

			if (Pif.szProgramName[1] == ':' && Pif.szProgramName[2] == '\\')
			{
				// Resolve only an absolute path
				// (otherwise will have to do too much speculation where the file is located)
				::strcpy(pszPath, Pif.szProgramName);
				bReturn = true;
			}
		}

		::CloseHandle(hFile);
	}

	return ( bReturn );
}

// ---------------------------------------------------------------------------
// find the destination of a shortcut
//
// in/out:
//   pszPath        return buffer, NULL on failure
//
// returns:
//   standard hres codes
//
//
// assumes:
//   CoInitialize() has been called...
// ---------------------------------------------------------------------------

HRESULT CSelectionListBase::ResolveLnkFile(PTCHAR pszPath)
{
	USES_CONVERSION;
	HRESULT hres;
	CComPtr<IShellLink> psl;
	CComPtr<IPersistFile> ppf;
	WIN32_FIND_DATA wfd;

	if (SUCCEEDED(hres = psl.CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER))
	 && SUCCEEDED(hres = psl.QueryInterface(&ppf))
	 && SUCCEEDED(hres = ppf->Load(A2W(pszPath), STGM_READ))
	 && SUCCEEDED(hres = psl->Resolve(NULL, SLR_ANY_MATCH | SLR_NO_UI)))
		hres = psl->GetPath(pszPath, MAX_PATH*2, &wfd, SLGP_SHORTPATH);

	return hres;
}

HRESULT CSelectionListBase::Add2List(CComPtr<IShellFolder>& spISF, LPCITEMIDLIST pidl)
{
	USES_CONVERSION;
	HRESULT hr;
	TCHAR szPath[MAX_PATH*2];
	CStrRet srPath(m_spMalloc, pidl);

	// Get full path
	if (FAILED(hr = spISF->GetDisplayNameOf(pidl, SHGDN_FORPARSING, &srPath)))
	{
		switch(hr)
		{   // Eliminate all bogus errors
		case E_NOTIMPL:    // Occures on network under NT 4.0 & Win98
		case E_INVALIDARG: // ftp site items under NT 4.0
			return S_FALSE;
		default:
			return hr;
		}
	}

	srPath.CopyTo(szPath);

	return Add2List(szPath);
}

HRESULT CSelectionListBase::GetTreeViewSelection(CComPtr<IWebBrowser2>& spBrowser, CComPtr<IShellBrowser>& spShellBrowser
                                                                                 , TCHAR pszPath[])
{
	USES_CONVERSION;
	HRESULT hr;
	int i;
	PWCHAR pwcsProtocol;
	CComBSTR sbURL
		   , sbPath;

	m_vItems.clear();  // Reset

	// Get the path of the hilited item
	if (FAILED(hr = spBrowser->get_LocationURL(&sbURL)))
		return hr;

	pwcsProtocol = ::wcschr(sbURL, L':');  

	if (!pwcsProtocol)
		// Unscannable item is selected.
		return S_FALSE;

	 // first time around get a hold of the Desktop
	if (!m_spDesktopFolder && FAILED(hr = ::SHGetDesktopFolder(&m_spDesktopFolder)))
		return hr;

	pwcsProtocol++;
	if (!::wcsnicmp(pwcsProtocol, L"///", 3))
		i = 3;
	else if (!::wcsnicmp(pwcsProtocol, L"//", 2))
		i = 0;
	else
		// Unscannable item is selected.
		return S_FALSE;

	// Translate URL encoding to normalized path name
	for (WCHAR wch; wch = pwcsProtocol[i]; i++)
	{
		WCHAR wszChar[3] = { wch }
			, *pNotUsed;

		switch(wch)
		{
		default:  // No translation
			break;
		case L'/':  // Convert forword slash to backslash
			wszChar[0] = L'\\';
			break;
		case L'%':  // Convert '%xx' to equvivalent char
			wszChar[0] = pwcsProtocol[++i];
			wszChar[1] = pwcsProtocol[++i];
			wszChar[0] = ::wcstol(wszChar, &pNotUsed, 16);
			wszChar[1] = L'\0';;
			break;
		}

		sbPath += wszChar;
	}
 
	::_tcsncpy(pszPath, OLE2T(sbPath), MAX_PATH*2);
	return S_OK;
}

HRESULT CSelectionListBase::Add2List(PTCHAR pszPath)
{
	HRESULT hr;
	TCHAR szExt[MAX_PATH];
	TCHAR szPath[MAX_PATH*2];

	// Q: Can we access it? 
	if(::GetFileAttributes(pszPath) == 0xFFFFFFFF)
	{
		// Inaccessable item
		if (ERROR_SHARING_VIOLATION == ::GetLastError())
		{
			m_vDeniels.push_back(pszPath);
			return S_FALSE;
		}
		else if (!TryWithShortName(pszPath, szPath))
			return S_FALSE;
		else
			pszPath = szPath;
	}

	m_vItems.push_back(pszPath);

	// follow .lnk & .pif files & add the "real" file as well
	::_tsplitpath(pszPath, NULL, NULL, NULL, szExt);
	if (!::_tcsicmp(szExt, _T(".lnk")))
	{
		if (SUCCEEDED(hr = ResolveLnkFile(pszPath)))
			m_vItems.push_back(pszPath);
	}
	else if (!::_tcsicmp(szExt, _T(".pif")))
	{
		if (ResolvePifFile(pszPath))
			m_vItems.push_back(pszPath);
	}

	return S_OK;
}

bool CSelectionListBase::TryWithShortName(PTCHAR pszPath, TCHAR szPath[])
{
	WIN32_FIND_DATA wfd;
	bool bFound = false;
	PTCHAR p;

	/*
	 * On DBCS systems file names that have hi ASCII characters
	 * are missed since we can't use that name to get the file
	 * attributes. Unfortunately GetShortPathName() will not figure
	 * the path correctly. So if we fail we try one last attempt:
	 * Itterate through all the items in the directory and
	 * find a file with at matching long file name. Use the
	 * short file name to scan the item.
	 * Note: This startegy will fail if any of the path components other
	 *       then the file name are hi ASCII. I didn't want to do too
	 *       much processing and examine the enteir path.
	 */
	if (p = ::_tcsrchr(pszPath, _T('\\')))
	{
		// Isolate the search path
		p++;
		::_tcsncpy(szPath, pszPath, p - pszPath);
		szPath[p - pszPath] = _T('\0');
		::_tcscat(szPath, _T("*.*"));

		HANDLE hFF = ::FindFirstFile(szPath, &wfd);
		if (INVALID_HANDLE_VALUE != hFF)
		{
			// Itterate the items in this directory.
			do
			{
				// Q: Is this a long file anme match?
				if (!::_tcscmp(p, wfd.cFileName))
				{
					// Lets see if we can access the item using the short file name.
					::_tcscpy(::_tcsrchr(szPath, _T('\\')) + 1, wfd.cAlternateFileName);
					if (false == (bFound = ::GetFileAttributes(szPath) != 0xFFFFFFFF)
					 && ERROR_SHARING_VIOLATION == ::GetLastError())  // Inaccessable
						m_vDeniels.push_back(pszPath);
					break;
				}
			} while (::FindNextFile(hFF, &wfd));

			::FindClose(hFF);
		}
	}

	return bFound;
}

bool CSelectionListBase::ShowDeniels(HWND hWnd)
{
	if(m_vDeniels.size() != 0)
	{
		TCHAR szMsg[256];

		// Title string
		::LoadString(_Module.GetResourceInstance(), IDS_ACCESSDENIED, szMsg,   SIZEOF(szMsg));

		TSTRING strMsg = szMsg;
		// Save the list of the files to scan into a task file.
		for (vector<TSTRING>::iterator it = m_vDeniels.begin(); m_vDeniels.end() != it; it++)
		{
			strMsg += _T("\n");
			strMsg += *it;
		}

		// Means nothing is selected.
		if(m_csProductName.IsEmpty())
		{
			CBrandingRes BrandRes;
			m_csProductName = BrandRes.ProductName();
		}

		MessageBox(hWnd, strMsg.c_str(), m_csProductName, MB_ICONEXCLAMATION | MB_OK);
		m_vDeniels.clear();

		return true;
	}

	return false;
}
