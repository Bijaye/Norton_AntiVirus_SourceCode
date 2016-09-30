// HMIProducts.cpp: implementation of the CHMIProducts class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HMIProducts.h"
#include "OSInfo.h"
#include <shlobj.h>
#include "StartBrowser.h"

/////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHMIProducts::CHMIProducts()
{
    SetTextID ( IDS_PRODUCTS_TEXT );
}

CHMIProducts::~CHMIProducts()
{

}


HRESULT CHMIProducts::DoWork (HWND hMainWnd)
{
	TCHAR szURL[MAX_PATH] = {0};
	if(Upsell(IDS_UPSELL_STRING, szURL))
	{
		NAVToolbox::CStartBrowser browser;
		if(browser.ShowURL(szURL))
			return S_OK;
	}

	return E_FAIL;
}
bool CHMIProductsFactory::ShouldShow()
{
	bool bRet = false;

	STAHLSOFT_HRX_TRY(hr)
	{
		TCHAR szShortcutPath [MAX_PATH] = {0};

		// Check to see if the link "More Symantec Solutions" exists in Start\Programs menu.
		CRegKey regMoreSolutions;
		if ( ERROR_SUCCESS == regMoreSolutions.Open ( HKEY_LOCAL_MACHINE, _T("Software\\Symantec\\Norton AntiVirus"), KEY_READ ))
		{
			DWORD dwSize = MAX_PATH;
			if ( ERROR_SUCCESS == regMoreSolutions.QueryStringValue ( _T("ShortcutPath"), szShortcutPath, &dwSize ))
			{
				TCHAR szTempFileName [MAX_PATH] = {0};
				::LoadString ( _getPrimaryResourceInstance(),
					IDS_SYMANTEC_PRODUCTS_FILE,      // "More Symantec Solutions.lnk"
					szTempFileName,
					MAX_PATH );

				// Get the second to last char in the path.
				//
				TCHAR* pszDirNull = _tcsrchr ( szShortcutPath, '\0');

				TCHAR* pszLastSlash = CharPrev( szShortcutPath, pszDirNull );

				// If the ending character is not a slash, slash it.
				//            
				if ( 0 != _tcscmp ( pszLastSlash, _T("\\")))
				{
					_tcscat ( szShortcutPath, _T("\\"));
				}

				_tcscat ( szShortcutPath, szTempFileName );

				FILE* pFile = NULL;
				pFile = fopen ( szShortcutPath, "r" );

				if (pFile)
				{
					fclose(pFile);
					bRet = true;
				}
			}
		}
	}
	STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr);

	return bRet;   
}