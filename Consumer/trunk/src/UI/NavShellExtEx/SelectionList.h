////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// SelectionList.h: Implementation of the CSelectionList class

#ifndef __SelectionList_h__
#define __SelectionList_h__
#include <vector>
#include "ccVerifyTrustTypes.h"
#include "Softpub.h"

#include "ISVersion.h"

#import "NavShellExtHelper.TLB" raw_interfaces_only, raw_native_types, no_namespace, named_guids


typedef LONG (WINAPI *pWinVerifyTrust)(
		HWND hWnd,
		GUID* pgActionID,
		WINTRUST_DATA* pWinTrustData
		);


typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE hProcess,PBOOL Wow64Process);
typedef UINT (WINAPI *LPFN_GETSYSTEMWOW64DIRECTORY)(LPTSTR lpBuffer,UINT uSize);

enum eTrustState { UNDEFINED, TRUSTED, FAILED };

class CSelectionListBase
{
protected:
	CSelectionListBase(): m_hWinTrust(NULL)
						,m_hCrypt32(NULL)
						,m_pWinVerifyTrust(NULL)							   
						,m_bCheckRevocation(false)
	{
		CCTRACEI(_T("CSelectionListBase()"));
		m_csProductName = CISVersion::GetProductName();
	}
	~CSelectionListBase()
	{
		if( m_hWinTrust ) {
			FreeLibrary(m_hWinTrust);
			m_hWinTrust = NULL;
		}
	}
	virtual HRESULT	_Error(int iId, const IID& guid) = 0;
	virtual HRESULT	_Error(CString csError, const IID& guid) = 0;

	HRESULT GetSelectionList(IDataObject* pDataObj);
	HRESULT GetTreeViewSelection(CComPtr<IWebBrowser2>& spBrowser, CComPtr<IShellBrowser>& spShellBrowser
	                                                             , TCHAR pszPath[]);
	HRESULT Add2List(PTCHAR pszPath);
	int RunNAVW(void);
	bool ShowDenials(HWND hWnd);
	static eTrustState	m_TrustState;
	void ShowError(HRESULT hr);
	BOOL IsTrusted() throw();
	HRESULT	ErrorFromResource(int iId, const IID& guid, CString& csError);
	void	GetResourceString(UINT iId,CString &csResource);
	HRESULT messageBox(UINT uiIDS);

	bool	m_bCheckRevocation;
	HMODULE	m_hWinTrust;
	HMODULE	m_hCrypt32;
	pWinVerifyTrust m_pWinVerifyTrust;
	CComPtr<IMalloc> m_spMalloc;  // Used in order to get rid of all that memory
	CComPtr<IShellFolder> m_spDesktopFolder;  // Used in GetSelectionList()
    std::vector<TSTRING> m_vItems;
	std::vector<TSTRING> m_vDenials;
	std::vector<WSTRING> m_vwInvaildPaths;
	CComPtr<INavHelper>m_spINavHelper;
	CString m_csProductName;

private:
// GHS: 362840 I am return the reason for the failure if it is NOT ERROR_FILE_EXISTS
	DWORD_PTR generateTempScanFile( LPTSTR szOut, int nBufferSize );
	bool ResolvePifFile(LPTSTR pszPath);
	HRESULT ResolveLnkFile(PTCHAR pszPath);
	HRESULT Add2List(CComPtr<IShellFolder>& spISF, LPCITEMIDLIST pidl);
	bool TryWithShortName(PTCHAR pszPath, TCHAR szPath[]);	
	bool PutSafeArray(std::vector<TSTRING> vItems, VARIANTARG &pVariant);
};

template<class T, const CLSID* pclsid, const IID* pitfc>
class CSelectionList : public CSelectionListBase, public CComCoClass<T, pclsid>
{
protected:
	HRESULT RunNAVW(void)
	{
		HRESULT hr = S_OK;
		int ids = 0;
		ids = CSelectionListBase::RunNAVW();
		if(ids)
		{
			CString csError;
			hr = ErrorFromResource(ids, *pitfc, csError);
		}
		return hr;
	}
};

#endif __SelectionList_h__
