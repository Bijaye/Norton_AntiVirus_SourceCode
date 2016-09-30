
// NuAboutExtension.h : Declaration of the CNuAboutExtension

#ifndef __NUABOUTEXTENSION_H_
#define __NUABOUTEXTENSION_H_

#include "aboutplg.h"
#include "resource.h"       // main symbols
#include "Password.h"	// Added by ClassView
#include "StringHelper.h"   // For loading strings
#include "BrandingImpl.h"

DEFINE_GUID(CLSID_SAPExtension ,0x0E470E79,0xC6D1,0x4603,0x88,0x90,0xB3,0xC3,0x0F,0xFB,0xE2,0x42);



class ATL_NO_VTABLE CNav2kAboutExtension : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CNav2kAboutExtension, &CLSID_Nav2kAboutExtension>,
	public INav2kAboutExtension,
	public IShellPropSheetExt,
	public IAboutProperties,
	public CBrandingImpl
{
public:
	CNav2kAboutExtension()
	{
	}

	DECLARE_REGISTRY_RESOURCEID(IDR_Nav2kABOUTEXTENSION)
	DECLARE_NOT_AGGREGATABLE(CNav2kAboutExtension)
	DECLARE_PROTECT_FINAL_CONSTRUCT()
	HRESULT FinalConstruct();

	BEGIN_COM_MAP(CNav2kAboutExtension)
		COM_INTERFACE_ENTRY(INav2kAboutExtension)
		COM_INTERFACE_ENTRY(IShellPropSheetExt)
		COM_INTERFACE_ENTRY(IAboutProperties)
	END_COM_MAP()

public:
	// IShellPropSheetExt
	STDMETHODIMP AddPages(LPFNADDPROPSHEETPAGE lpfnAddPage, LPARAM lParam);
	STDMETHODIMP ReplacePage(UINT uPageID, LPFNADDPROPSHEETPAGE lpfnReplacePage, LPARAM lParam); 

	// IAboutProperties
	STDMETHODIMP SetValue(DWORD dwValueID, DWORD dwValue);
	STDMETHODIMP GetValue(DWORD dwValueID, DWORD* pdwValue);

public:
	static BOOL CALLBACK ExtPageDlgProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam);
	static void OnInitDialog(HWND hWnd);

private:
	static WNDPROC m_OldWndProc;
	static WNDPROC m_OldResetWndProc;
	static WNDPROC m_OldHelpWndProc; 
	static HWND m_hWndDlg;
	static CString m_csProductName;

	//static void OnDrawItem(HWND hWnd, UINT nID, LPDRAWITEMSTRUCT pDrawItem);
	static void OnKeyDown(HWND hWnd, int nVirtualKey);
	static BOOL OnCommand(UINT nMsg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK KeyProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK ButtonProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam);
	static BOOL SetBoldfaceFont(HWND hWnd);
	static void OnResetPassword ();
	static HWND m_hWndResetPassword;
	static HWND m_hWndHelp;
	static CPassword m_Password;
	static void OnHelp ();
};

#endif //__NUABOUTEXTENSION_H_

