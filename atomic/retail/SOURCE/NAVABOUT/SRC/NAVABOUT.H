// Copyright 1997 Symantec
// -------------------------------------------------------------------------
// NAVABOUT.H
//
// This is a skeletal about box extension for NAV.  This the minimum you need
// to implement to add a page to the Symantec Standard About box.
//
// Declaration of CAboutExt, CAboutExtFactory, CAboutPage
//
//
// $Header:   S:/NAVABOUT/VCS/NavAbout.h_v   1.0   19 Aug 1997 14:09:52   KSACKIN  $
// -------------------------------------------------------------------------
// $Log:   S:/NAVABOUT/VCS/NavAbout.h_v  $
// 
//    Rev 1.0   19 Aug 1997 14:09:52   KSACKIN
// Initial revision.
// 
//    Rev 1.1.1.0   12 Aug 1997 22:44:52   DALVIDR
// Branch base for version QAKG
// 
//    Rev 1.1   01 Jul 1997 14:42:32   TIVANOV
// new service functions
//
//    Rev 1.0   06 Jun 1997 20:02:04   BMCCORK
// Initial revision.
/////////////////////////////////////////////////////////////////////////////

#ifndef __NAVABOUT_H__
#define __NAVABOUT_H__

// {CC5A09C0-DEA4-11d0-8547-00A0240B50F0}
DEFINE_GUID( CLSID_CNavAboutExt,
0xcc5a09c0, 0xdea4, 0x11d0, 0x85, 0x47, 0x0, 0xa0, 0x24, 0xb, 0x50, 0xf0);


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
    UINT GetNumVirusEntries ();
    DWORD GetDefinitionDate(LPTSTR lpDateStr, int size);
    BOOL IsNTServer();
};

#endif // __ABOUTEXT_H__
