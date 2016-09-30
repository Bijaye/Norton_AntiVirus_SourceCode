// SelectionList.h: Implementation of the CSelectionList class

#ifndef __SelectionList_h__
#define __SelectionList_h__

class CSelectionListBase
{
protected:
	CComPtr<IMalloc> m_spMalloc;  // Used in order to get rid of all that memory
	CComPtr<IShellFolder> m_spDesktopFolder;  // Used in GetSelectionList()
    std::vector<TSTRING> m_vItems;
	std::vector<TSTRING> m_vDeniels;

	HRESULT GetSelectionList(IDataObject* pDataObj);
	HRESULT GetTreeViewSelection(CComPtr<IWebBrowser2>& spBrowser, CComPtr<IShellBrowser>& spShellBrowser
	                                                             , TCHAR pszPath[]);
	HRESULT Add2List(PTCHAR pszPath);
	int RunNAVW(void);
	bool ShowDeniels(HWND hWnd);

private:
// GHS: 362840 I am return the reason for the failure if it is NOT ERROR_FILE_EXISTS
	DWORD_PTR generateTempScanFile( LPTSTR szOut, int nBufferSize );
	bool ResolvePifFile(LPSTR pszPath);
	HRESULT ResolveLnkFile(PTCHAR pszPath);
	HRESULT Add2List(CComPtr<IShellFolder>& spISF, LPCITEMIDLIST pidl);
	bool TryWithShortName(PTCHAR pszPath, TCHAR szPath[]);

private:
	CString m_csProductName;
};

template<class T, const CLSID* pclsid, const IID* pitfc>
class CSelectionList : public CSelectionListBase, public CComCoClass<T, pclsid>
{
protected:
	HRESULT RunNAVW(void)
	{
		int ids;

		return (ids = CSelectionListBase::RunNAVW()) ? Error(ids, *pitfc) : S_OK;
	}
};

#endif __SelectionList_h__
