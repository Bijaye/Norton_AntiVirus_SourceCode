////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// SelectionList.cpp: Implementation of the CSelectionList class

#include "StdAfx.h"
#include "NavTrust.h"
#include "Resource.h"
#include "..\\NavShellExtExRes\\resource.h"
#include "SelectionList.h"
#include "strsafe.h"

using namespace std;
const TCHAR szTaskFileExtension [] = _T("sca");

using namespace ccVerifyTrust;

#define GetPIDLFolder(pida) (LPCITEMIDLIST)(((LPBYTE)pida)+(pida)->aoffset[0])
#define GetPIDLItem(pida, i) (LPCITEMIDLIST)(((LPBYTE)pida)+(pida)->aoffset[i+1])

// static initialization
eTrustState CSelectionListBase::m_TrustState = UNDEFINED;

#ifdef DEBUG_NAVSH
void	ErrorMessage(LPTSTR szErr)
{
	LPVOID lpMsgBuf;
	DWORD dwError = GetLastError();
	if (!FormatMessage( 
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM | 
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dwError,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR) &lpMsgBuf,
		0,
		NULL ))
	{
		// Handle the error.
		return;
	}

	// Process any inserts in lpMsgBuf.
	// ...
	TCHAR szE[500] = {NULL};
	swprintf(szE,_T("%s,%x"),szErr,dwError);
	// Display the string.
	MessageBox( NULL, (LPCTSTR)lpMsgBuf, szE, MB_OK | MB_ICONINFORMATION );

	// Free the buffer.
	LocalFree( lpMsgBuf );
}
#endif


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
	HRESULT hr = S_OK;
	CStgMedium me;
	FORMATETC fe = { CF_SHELLIDLIST, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };

	// Clean previous selections
	m_vItems.clear();

	// Get the files associated with this object, if applicable.
	if(FAILED(hr = pDataObj->GetData(&fe, &me)))
	{
		CCTRACEE(_T("ERROR!!! CSelectionListBase::GetSelectionList(): pDataObj->GetData() Failed with 0x%08X"),hr);
		return hr;
	}

	CComPtr<IShellFolder> spISF;

	 // first time around get a hold of the Desktop
	if (!m_spDesktopFolder && FAILED(hr = ::SHGetDesktopFolder(&m_spDesktopFolder)))
	{
		CCTRACEE(_T("ERROR!!! CSelectionListBase::GetSelectionList(): SHGetDesktopFolder() Failed with 0x%08X"),hr);
		return hr;
	}

	// Get lte list of selected files form the IShellView
	LPIDA pida = reinterpret_cast<LPIDA>(me.hGlobal);
	LPCITEMIDLIST pidl = GetPIDLFolder(pida);
	// Get the IShellFolder of the parent
	if (FAILED(hr = m_spDesktopFolder->BindToObject(pidl, NULL
	                                                    , IID_IShellFolder
	                                                    , reinterpret_cast<void**>(&spISF))))
	{
		if (E_INVALIDARG != hr)
		{
			CCTRACEE(_T("ERROR!!! CSelectionListBase::GetSelectionList():  Failed to get the IShellFolder m_spDesktopFolder->BindToObject() 0x%08X"),hr);
			return hr;
		}

		// Dhuh! We got an invalid argument error because this PIDL is the desktop itself
		spISF = m_spDesktopFolder;
	}

	 // first time around get a hold of the system allocator
	if (!m_spMalloc && FAILED(hr = ::SHGetMalloc(&m_spMalloc)))
	{
		CCTRACEE(_T("ERROR!!! CSelectionListBase::GetSelectionList():  Failed SHGetMalloc() 0x%08X"),hr);
		return hr; 
	}

	// Add all applicable selected object to the list
	for (UINT i = 0; i < pida->cidl; i++)
	{
		LPCITEMIDLIST pidl = GetPIDLItem(pida, i);
		if (FAILED(hr = Add2List(spISF, pidl)))
		{
			CCTRACEE(_T("ERROR!!! CSelectionListBase::GetSelectionList(): Add2List() Failed 0x%08X"),hr);
			return hr;
		}
	}

	return hr;
}

// GHS: 362840 I am return the reason for the failure if it is NOT ERROR_FILE_EXISTS
DWORD_PTR CSelectionListBase::generateTempScanFile( LPTSTR szOut, int nBufferSize )
{
	CCTRACEI(_T("CSelectionListBase::generateTempScanFile() Enter"));

	TCHAR szTempDir[ MAX_PATH*2 ];
	TCHAR szTempFile[ 32 ];

    DWORD dwLastError = NOERROR;
	// Fetch temp directory.
	if( 0 == ::GetTempPath(MAX_PATH*2, szTempDir) )
    {
        dwLastError = ::GetLastError();
		CCTRACEE(_T("ERROR!!! CSelectionListBase::generateTempScanFile(): GetTempPath() Failed %d"),dwLastError);
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
		_stprintf( szTempFile, _T("scan%d.%s"), iIndex, szTaskFileExtension );
		_tcsncat( szOut, szTempFile, nBufferSize );

		// Try to create the file.
		HANDLE hFile = CreateFile( szOut, GENERIC_WRITE, 0, NULL, CREATE_NEW, 0, NULL );
		if( hFile != INVALID_HANDLE_VALUE )
		{
			// Success.  Close the handle and return.
			CloseHandle( hFile );
			CCTRACEI(_T("CSelectionListBase::generateTempScanFile() return :%s"),szOut);
			return NOERROR;
		}

		// An error occured.  If the error states that the file
		// already exists, keep processing, else bail out.
        // GHS: 362840 I am return the reason for the failure if it is NOT ERROR_FILE_EXISTS
        DWORD dwLastError = ::GetLastError();
		if( dwLastError != ERROR_FILE_EXISTS )
		{
			CCTRACEE(_T("CSelectionListBase::generateTempScanFile() return ERROR!! :%d"),dwLastError);
            return dwLastError;
		}
	}
	CCTRACEI(_T("CSelectionListBase::generateTempScanFile() Return"));

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

bool	CSelectionListBase::PutSafeArray(vector<TSTRING> vItems, VARIANTARG &pVariant)
{
	if( vItems.empty() )
	{
		return false;
	}

	HRESULT hr = S_OK;
	SAFEARRAYBOUND saBound[1];
	saBound[0].cElements = vItems.size();
	saBound[0].lLbound = 0;
	LPSAFEARRAY pSa = SafeArrayCreate(VT_BSTR, 1, saBound);
	if (pSa == NULL)
	{
		return false;
	}

	USES_CONVERSION;
	vector<TSTRING>::iterator it;
	long index = 0;
	for (it = vItems.begin(); vItems.end() != it; it++, index++)
	{
		BSTR bstr = SysAllocString((*it).c_str());
		hr = SafeArrayPutElement(pSa, &index, (void FAR*)bstr);				
		SysFreeString(bstr);
	}
	pVariant.vt = VT_BSTR | VT_ARRAY;
	pVariant.parray = pSa;

	return true;
}

int CSelectionListBase::RunNAVW(void)
{
	CCTRACEI(_T("CSelectionListBase::RunNAVW()"));

	if( !IsTrusted() ) {
		return IDS_Err_VerifyTrustFailed;
	}

	if( !m_vItems.size() && !m_vDenials.size() )
	{
		if( m_vwInvaildPaths.size() )
		{
			wstring wstrMsg;
			CString csError;
			GetResourceString(IDS_Err_InvalidPath,csError);
			wstrMsg = csError;	
			wstrMsg += L"\n";
			// Create list of files denied access to
			for (vector<wstring>::iterator it = m_vwInvaildPaths.begin(); 
					m_vwInvaildPaths.end() != it; it++)	
			{
					wstrMsg += L"\n";
					wstrMsg += *it;
			}
			MessageBox (NULL,wstrMsg.c_str(),m_csProductName,MB_OK);
			m_vwInvaildPaths.clear();
			return S_OK;
		}
		return IDS_NOITEMS;
	}
	
	HRESULT	hr = S_OK;
	CComVariant pvItems, pvDenials;
	PutSafeArray(m_vItems,pvItems);
	PutSafeArray(m_vDenials,pvDenials);

	if( m_spINavHelper == NULL )
	{
		HRESULT hr = m_spINavHelper.CoCreateInstance(CLSID_NavHelper,NULL,CLSCTX_LOCAL_SERVER);
		if(!SUCCEEDED(hr))
		{
			CCTRACEE(_T("CLSID_NavHelper Failed 0x%08X\n"), hr);
			return IDS_Err_Internal;
		}
	}
	hr = m_spINavHelper->RunNavW(pvItems,pvDenials);
	if(FAILED(hr))
	{
		ShowError(hr);
		CCTRACEE(_T("m_spINavHelper->RunNav() Failed 0x%08X\n"), hr);
	}
	return 0;
}

// ---------------------------------------------------------------------------
// Note: This is not a TCHAR routine. We may have to redo it if wchar is needed.
//       Anothr issue of the same problem is that Microsoft did not published the
//       PIF file format. This structure may change in future versions of the OS.
// ---------------------------------------------------------------------------
// Verify for x64 - @TBD

bool CSelectionListBase::ResolvePifFile(LPTSTR pszPath)
{
	CCTRACEI(_T("CSelectionListBase::ResolvePifFile"));

	struct tagPIFFORMAT
	{
		WORD    wReserved;
		TCHAR    byProgramTitle [ 30 ];  // non null terminated
		WORD    wMaxMemToPartition;     // in K
		WORD    wMinMemRequired;        // in K
		TCHAR    szProgramName [ 64 ];   // What about LFNs?
		BYTE    byDefaultDriveLetter;
		TCHAR    szDefaultDir [ 64 ];
		TCHAR    szProgParams [ 64 ];
		BYTE    byInitScreenMode;
		BYTE    byNumTextPages;
		BYTE    byNumFirstSaveInt;
		BYTE    byNumLastSaveInt;
		BYTE    byVirtScreenRows;
		BYTE    byVirtScreenCols;
		BYTE    byInitWindowsRow;
		BYTE    byInitWindowsCol;
		WORD    wSystemMemory;          // in K
		TCHAR    szSharedProgName [ 64 ];
		TCHAR    szSharedDataFile [ 64 ];
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
				_tcscpy(pszPath, Pif.szProgramName);
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
	CCTRACEI(_T("CSelectionListBase::ResolveLnkFile"));

	HRESULT hres;
	CComPtr<IShellLink> psl;
	CComPtr<IPersistFile> ppf;
	WIN32_FIND_DATA wfd;

	if (SUCCEEDED(hres = psl.CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER))
	 && SUCCEEDED(hres = psl.QueryInterface(&ppf))
	 && SUCCEEDED(hres = ppf->Load(pszPath, STGM_READ))
	 && SUCCEEDED(hres = psl->Resolve(NULL, SLR_ANY_MATCH | SLR_NO_UI)))
		hres = psl->GetPath(pszPath, MAX_PATH*2, &wfd, SLGP_SHORTPATH);

	return hres;
}

HRESULT CSelectionListBase::Add2List(CComPtr<IShellFolder>& spISF, LPCITEMIDLIST pidl)
{
	USES_CONVERSION;
	HRESULT hr = S_OK;
	TCHAR szPath[MAX_PATH*2];
	CStrRet srPath(m_spMalloc, pidl);

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
	hr = Add2List(szPath);
	if( S_FALSE ==	hr )
	{
		// Try getting Unicode path
		WCHAR	wsPath[2*MAX_PATH];
		if( SHGetPathFromIDListW(pidl,wsPath) )
		{
			m_vwInvaildPaths.push_back(wsPath);
			hr = S_OK;
		}
	}
	return hr;
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
	m_vwInvaildPaths.clear();

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

//returns:
//	S_FALSE if path is invaild
//	E_ACCESSDENIED if sharing vioaltion
//  S_OK if success
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
			m_vDenials.push_back(pszPath);
			return E_ACCESSDENIED;
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
						m_vDenials.push_back(pszPath);
					break;
				}
			} while (::FindNextFile(hFF, &wfd));

			::FindClose(hFF);
		}
	}

	return bFound;
}

bool CSelectionListBase::ShowDenials(HWND hWnd)
{
	if(m_vDenials.size() != 0)
	{
		if(m_csProductName.IsEmpty())
		{
			m_csProductName = CISVersion::GetProductName();
		}

		// Title string
		CString csMsg;
		csMsg.LoadString(_AtlBaseModule.GetResourceInstance(), IDS_ACCESSDENIED);

		TSTRING strMsg = csMsg;
		// Create list of files denied access to
		for (vector<TSTRING>::iterator it = m_vDenials.begin(); m_vDenials.end() != it; it++)
		{
			strMsg += _T("\n");
			strMsg += *it;
		}

		// Means nothing is selected.
		MessageBox(hWnd, strMsg.c_str(), m_csProductName, MB_ICONEXCLAMATION | MB_OK);
		m_vDenials.clear();

		return true;
	}

	return false;
}

BOOL CSelectionListBase::IsTrusted() throw()
{
	if( CSelectionListBase::m_TrustState == UNDEFINED )	
	{
		CSelectionListBase::m_TrustState = FAILED;
		if( NAVToolbox::NAVTRUST_OK	== NAVToolbox::IsSymantecComServer(CLSID_IEContextMenuHelper) ) 
		{
			CSelectionListBase::m_TrustState = TRUSTED;
		}

		CCTRCTXI1(L"TrustState: %d", CSelectionListBase::m_TrustState);
	}
	return (TRUSTED == CSelectionListBase::m_TrustState );
}

void CSelectionListBase::ShowError(HRESULT hr)
{
	USES_CONVERSION;
    HRESULT _hr;
    
    CComBSTR sbDesc   = _T("Error in IEContextMenu Object (HRESULT = ");
    CComPtr<IErrorInfo> spErrorInfo;
    
    // Check if ErrorInfo is available
	if (S_OK == (_hr = ::GetErrorInfo(0UL, &spErrorInfo)))
	{
		_hr = spErrorInfo->GetDescription(&sbDesc);
	}
	else  // Format our own (cheesey) message
	{
		TCHAR szTmp[16] = {0};

		sbDesc += _ltot(hr, szTmp, 16);
		sbDesc += _T(")");
	}
	CCTRACEI(_T("CSelectionListBase::ShowError(0x%08X : %s"),_hr, OLE2A(sbDesc));

	MessageBox(::GetActiveWindow(),OLE2T(sbDesc), m_csProductName,MB_OK | MB_ICONERROR);
}

// Returns HRESULT with message from resource ID, to be used by ISupportErrorInfo
HRESULT	CSelectionListBase::ErrorFromResource(int iId, const IID& guid, CString& csError)
{
	HRESULT hr = E_FAIL;	
	GetResourceString(iId,csError);	
	hr = _Error(csError,guid);
	return hr;
}

HRESULT CSelectionListBase::messageBox(UINT uiIDS)
{
	CString csMsg;
	GetResourceString(uiIDS,csMsg);
	MessageBox(::GetActiveWindow(),csMsg, m_csProductName, MB_ICONEXCLAMATION | MB_OK);
	return S_OK;
}

void	CSelectionListBase::GetResourceString(UINT uiIDs,CString &csResource)
{
	CString csFormat;
	csFormat.LoadString(_AtlBaseModule.GetResourceInstance(), uiIDs);
	if( -1 != csFormat.Find(_T("%s")))
	{
		csResource.Format(csFormat, m_csProductName);
	}
	else
	{
		csResource = csFormat;
	}
}
