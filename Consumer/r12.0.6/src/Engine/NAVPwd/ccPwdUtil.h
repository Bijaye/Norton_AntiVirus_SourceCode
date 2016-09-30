// /////////////////////////////////////////////////////////////////////////////////////////////////
// Symantec copyright header start
// //////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2004 Symantec Corporation.
// All rights reserved.
//
// /////////////////////////////////////////////////////////////////////////////////////////////////
// Symantec copyright header stop
// //////////////////////
#ifndef _CCPWDUTIL_H
#define _CCPWDUTIL_H

#include <algorithm>
#include <list>
#include <queue>
#include <strstream>
#include "sha_func.h"
#include <vector>
#include "resource.h"

// This class is used by the function that hashes a file.
template < class typeItem > struct HexStringInserter : public std::unary_function<typeItem,void>
{
	HexStringInserter ( std::strstream & string ) :
		m_string(string)
		{ return; }
		result_type operator()(argument_type const & item) const
	{
		m_string.width(2*sizeof(item));
		m_string.fill('0');
		m_string << std::right << std::hex;
		m_string << (unsigned int)item;
		return;
	}
private:
	std::strstream & m_string;
};


/////////////////////////////////////////////////////////////////////////////
//
// Define prototypes for local functions
#define CCPW_INVALID_LENGTH			MAKE_HRESULT(1, FACILITY_ITF, 0xc0)

#define CCPW_PASSWORD_MISMATCH		MAKE_HRESULT(1, FACILITY_ITF, 0xf0)
#define CCPW_PASSWORD_INVALID		MAKE_HRESULT(1, FACILITY_ITF, 0xf1)
#define CCPW_PASSWORD_INVALID_OLD	MAKE_HRESULT(1, FACILITY_ITF, 0xf2)
#define CCPW_PASSWORD_INVALID_SAME	MAKE_HRESULT(1, FACILITY_ITF, 0xf3)
#define CCPW_PASSWORD_INVALID_KEY	MAKE_HRESULT(1, FACILITY_ITF, 0xf4)

HRESULT LoadPassword(long lProductID, LPCTSTR szUserName, TCHAR *pszOldPwd, DWORD dwLen);
HRESULT SavePassword(long lProductID, LPCTSTR szUserName, TCHAR *pszNewPwd);
BOOL GetProductPasswordPath(long lProductID, TCHAR *pszPath, DWORD dwLen);
HRESULT ValidatePassword(LPCTSTR szPassword);
void GetPasswordErrorString(HRESULT hr, LPTSTR szError, DWORD dwBufLen);
void DisplayError(HWND hWndParent, HRESULT hr);
void WipeBuffer(PBYTE pbBuf, DWORD dwLen);
void SecureDelete(PBYTE pbBuf, DWORD dwLen);
//BOOL VerifyWindowsAccountCreds(LPCTSTR szUser, LPCTSTR szPassword);

HRESULT HashPassword(TCHAR *pszPwd, BYTE *bHash, DWORD &dwSize);
HRESULT HashTime(BYTE *bHash, DWORD &dwSize);
HRESULT GenerateRandomString(TCHAR szString[11]);
HMODULE GetMyModule();

/////////////////////////////////////////////////////////////////////////////
// Base class for dialogs.

template <class T> class CPasswordDlgBase :
	public CAxDialogImpl<T>
{
public:
	CPasswordDlgBase()
	{ 
		m_szFeatureName[0] = 0;
		m_szProductName[0] = 0;
		m_szNewPwd[0] = 0;
	}

	virtual ~CPasswordDlgBase()
	{
		WipeBuffer((PBYTE)m_szNewPwd, (MAX_PATH+1)*sizeof(TCHAR));
	}

	void SetFeatureName(TCHAR *pszFeatureName)
	{
		if(	pszFeatureName == NULL || lstrlen( pszFeatureName ) == 0 )
		{
			::LoadString( _Module.GetResourceInstance(), IDS_THIS_FEATURE, m_szFeatureName, sizeof( m_szFeatureName ) );
		}
		else
		{
			ZeroMemory(m_szFeatureName, sizeof( m_szFeatureName ) );
			_tcsncpy(m_szFeatureName, pszFeatureName, (sizeof( m_szFeatureName ) / sizeof( TCHAR )) - 1 );
		}
	}

	void SetProductName(TCHAR *pszProductName)
	{
		ZeroMemory(m_szProductName, sizeof( m_szProductName ) );
		_tcsncpy(m_szProductName, pszProductName, (sizeof( m_szProductName ) / sizeof( TCHAR )) - 1 );
	}

	void GetNewPwd(TCHAR *pszNewPwd, DWORD dwSize)
	{
		if((_tcslen(m_szNewPwd) + 1) < dwSize)
			_tcscpy(pszNewPwd, m_szNewPwd);
		else
		{
			ZeroMemory(pszNewPwd, dwSize);
			_tcsncpy(pszNewPwd, m_szNewPwd, dwSize - 1);
		}
	}

	// Dialog message handlers
	virtual LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		CenterWindow();	
		switchToThisWindow( m_hWnd, FALSE );
		return 1;   // Let the system set the focus
	}

	LRESULT OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		return 0;
	}

	LRESULT OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		return OnCancelPwd(wNotifyCode, wID, hWndCtl, bHandled);
	}

	LRESULT OnCancelPwd(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		EndDialog(wID);	
		return 0;
	}

private:
	// Activates the specified window and puts the thread that created
	// this window into the foreground (SCOOLEY)
	//
	//   HWND hWnd,    // Handle to the window that should be activated
	//   BOOL bRestore // Restore the window if it is minimized
	void switchToThisWindow(HWND hWnd, BOOL bRestore)
	{
		typedef void (WINAPI *PROCSWITCHTOTHISWINDOW)(HWND, BOOL);
		PROCSWITCHTOTHISWINDOW lpfnSwitchToThisWindow;

		HINSTANCE hUser32 = GetModuleHandle("user32");
		if(!hUser32)
			return;
		
		lpfnSwitchToThisWindow = (PROCSWITCHTOTHISWINDOW)GetProcAddress(hUser32, "SwitchToThisWindow");
		if(!lpfnSwitchToThisWindow)
			return;
		
		lpfnSwitchToThisWindow(hWnd, bRestore);
	}


protected:
	TCHAR m_szFeatureName[MAX_PATH+1];
	TCHAR m_szProductName[MAX_PATH+1];
	TCHAR m_szNewPwd[MAX_PATH+1];
};


#endif
