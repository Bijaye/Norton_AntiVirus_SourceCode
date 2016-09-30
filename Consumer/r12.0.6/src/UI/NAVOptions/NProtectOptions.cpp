
#include "Stdafx.h"
#include "NProtectOptions.h"
#include "Resource.h"
#include "ShlObj.h"
#include "..\navoptionsres\resource.h"

#include <initguid.h>
DEFINE_GUID(CLSID_WasteBasket,
			0x263913e0,0x07b9,0x106a,0x86,0x74,0x00,0xaa,0x00,0x0d,0x6f,0x0d);

IShellPropSheetExt* g_pIShellExt = NULL;	

BOOL DisplayNProtectOptions(HWND hWndParent, int iPage)
{
	//CoInitialize(NULL);

	HRESULT hResult = CoCreateInstance(CLSID_WasteBasket, NULL,
		CLSCTX_INPROC_SERVER, IID_IShellPropSheetExt, (void**) &g_pIShellExt);

	if (FAILED(hResult))
	{
		//CoUninitialize();
		return FALSE;
	}
	_Module.SetResourceInstance(g_ResModule);
	HINSTANCE hInstance = _Module.GetModuleInstance();
	
	TCHAR szOptionsTitle[MAX_PATH];
	LoadString(g_ResModule, IDS_NPROTECT_OPTIONS, szOptionsTitle, MAX_PATH);
	
	PROPSHEETPAGE Page;
	ZeroMemory(&Page, sizeof(PROPSHEETPAGE));

	Page.dwSize = sizeof(PROPSHEETPAGE);
	Page.dwFlags = PSP_DEFAULT;
	Page.hInstance = hInstance;
	Page.pszTemplate = MAKEINTRESOURCE(IDD_NPROTECT_PAGE);
	Page.pszIcon = NULL;
	Page.pfnDlgProc = NULL;
	
	PROPSHEETHEADER Header;
	ZeroMemory(&Header, sizeof(PROPSHEETHEADER));
	
	Header.dwSize = sizeof(PROPSHEETHEADER);
	Header.dwFlags = PSH_PROPSHEETPAGE | PSH_USECALLBACK | PSH_NOAPPLYNOW;
	Header.hwndParent = hWndParent;
	Header.hInstance = hInstance;
	Header.pszCaption = szOptionsTitle;
	Header.nPages = 1;
	Header.ppsp = (LPCPROPSHEETPAGE) &Page;
	Header.pfnCallback = PropSheetCallback;
	Header.nStartPage = iPage;
	
	PropertySheet(&Header);
	
	if (g_pIShellExt != NULL)
		g_pIShellExt->Release();

	//CoUninitialize();
	_Module.SetResourceInstance(_Module.GetModuleInstance());
	return TRUE;
}

int WINAPI PropSheetCallback(HWND hwnd, UINT uMsg, LPARAM lParam)
{
	if (uMsg == PSCB_INITIALIZED)
	{
		g_pIShellExt->AddPages(AddPages, (LPARAM) hwnd);
		
		PropSheet_RemovePage(hwnd, 0, NULL);
	}
	
	return 0;
}

BOOL WINAPI AddPages(HPROPSHEETPAGE hPropSheetPage, LPARAM lParam)
{
	::SendMessage((HWND) lParam, PSM_ADDPAGE, 0, (LPARAM) hPropSheetPage);

	return TRUE;
}

