// Copyright 1997 Symantec
// -------------------------------------------------------------------------
// ABOUTDLG.H
//
// This is a skeletal about box System Works extension for NAV.  This the minimum 
// you need to implement to add a page to the Symantec Standard About box Extension.
//
// Declaration of CAboutExt, CAboutExtFactory, CAboutPage
//
//
// $Header:   S:/ABOUTSW/VCS/aboutdlg.h_v   1.1   21 Oct 1998 22:39:14   rchinta  $
// -------------------------------------------------------------------------
// $Log:   S:/ABOUTSW/VCS/aboutdlg.h_v  $
// 
//    Rev 1.1   21 Oct 1998 22:39:14   rchinta
// Changed the __ABOUTSW_H to __ABOUTDLG_H to match the name of the
// file.
// 
//    Rev 1.0   21 Oct 1998 18:19:50   CEATON
// Initial revision.
// 
//    Rev 1.0   13 Oct 1998 18:44:58   rchinta
// Initial revision.
// 
// Initial revision.
/////////////////////////////////////////////////////////////////////////////

#ifndef __ABOUTDLG_H__
#define __ABOUTDLG_H__

// {C7523151-5997-11d2-B6C0-00104B882D98}
DEFINE_GUID(CLSID_CNavAboutSWExt, 
0xc7523151, 0x5997, 0x11d2, 0xb6, 0xc0, 0x0, 0x10, 0x4b, 0x88, 0x2d, 0x98);


///////////////////////////////////////
// CAboutExt

class CAboutExt : public ISymAboutExtInit, IShellPropSheetExt
{
	UINT m_cRef;						// object reference count
	CRefDll m_refDll;					// auto-updates DLL ref count

public:
	CAboutExt();
	virtual ~CAboutExt();

	// IUnknown
	STDMETHODIMP QueryInterface( REFIID riid, LPVOID FAR* ppvObj );
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();

	// ISymAboutExtInit
	STDMETHODIMP Initialize(
		LPCTSTR pszCompany,				// company name (e.g. Symantec)
		LPCTSTR pszProduct,				// product name (e.g. Norton Utilities)
		LPCTSTR pszVersion,				// version string (e.g. 2.01)
		LPCTSTR pszLanguage,			// product language
		DWORD dwFlags,					// transaction-related flags
		LPCTSTR pszPartNum );			// product SKU

	STDMETHODIMP SetData(
		ITransactionData* pData );		// pointer to data object

	// IShellPropSheetExt
	STDMETHODIMP AddPages(
		LPFNADDPROPSHEETPAGE lpfnAddPage,
		LPARAM lParam );

	STDMETHODIMP ReplacePage(
		UINT uPageID,
		LPFNADDPROPSHEETPAGE lpfnReplacePage,
		LPARAM lParam );

protected:
};

///////////////////////////////////////
// CAboutExtFactory

class CAboutExtFactory : public IClassFactory
{
	UINT m_cRef;						// object reference count
	CRefDll m_refDll;					// auto-updates DLL ref count

public:
	CAboutExtFactory();
	~CAboutExtFactory();

	// IUnknown
	STDMETHODIMP QueryInterface( REFIID riid, LPVOID FAR* ppvObj );
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();

	// IClassFactory
	STDMETHODIMP CreateInstance( LPUNKNOWN pUnkOuter, REFIID riid,
		LPVOID FAR* ppvObj );
	STDMETHODIMP LockServer( BOOL fLock );
};

/////////////////////////////////////////////////////////////////////////////
// Standard About Box Extension (Property Page)

///////////////////////////////////////
// CAboutPage

class CAboutPage : public CPropPage
{
public:
	CAboutPage();
	virtual ~CAboutPage();

// overrideables
	virtual BOOL OnInitDialog();
	virtual BOOL UpdateData( BOOL bSaveAndValidate = TRUE );
	virtual BOOL OnCommand( int nID, HWND hCtrl, UINT uNotify );

	virtual BOOL OnSetActive();
	virtual void OnRelease();

// attributes
protected:

private:
};

#endif // __ABOUTDLG_H__
