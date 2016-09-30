// Copyright 1997 Symantec
// -------------------------------------------------------------------------
// STDABOUT.H
// Declaration of CAboutBox, CAboutBoxFactory, CAboutSheet for this
// particular standard about extension.
//
// $Header:   S:/NAVABOUT/VCS/stdabout.h_v   1.0   19 Aug 1997 14:09:40   KSACKIN  $
// -------------------------------------------------------------------------
// $Log:   S:/NAVABOUT/VCS/stdabout.h_v  $
// 
//    Rev 1.0   19 Aug 1997 14:09:40   KSACKIN
// Initial revision.
// 
//    Rev 1.0.1.0   12 Aug 1997 22:44:48   DALVIDR
// Branch base for version QAKG
// 
//    Rev 1.0   06 Jun 1997 20:02:06   BMCCORK
// Initial revision.
/////////////////////////////////////////////////////////////////////////////

#ifndef __STDABOUT_H__
#define __STDABOUT_H__

////////////////////////////////////////
// Forward Declarations

///////////////////////////////////////
// CAboutBox

class CAboutBox : public IProductTransaction
{
	UINT m_cRef;						// object reference count
	CRefDll m_refDll;					// auto-updates DLL ref count

public:
	CAboutBox();
	virtual ~CAboutBox();

	// IUnknown
	STDMETHODIMP QueryInterface( REFIID riid, LPVOID FAR* ppvObj );
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();

	// IProductTransaction
	STDMETHODIMP Initialize( 
		LPCTSTR pszCompany,				// company name (e.g. Symantec)
		LPCTSTR pszProduct,				// product name (e.g. Norton Utilities)
		LPCTSTR pszVersion,				// version string (e.g. 2.01)
		LPCTSTR pszLanguage,			// product language
		DWORD dwFlags,					// transaction-related flags
		LPCTSTR pszPartNum );			// product SKU

	STDMETHODIMP Invoke(
		ITransactionData* pData,		// pointer to data object (optional)
		HWND hParent,					// handle of parent window
		BOOL bRunSilent );				// whether or not to show a UI

    STDMETHODIMP AddPages(
		LPFNADDPROPSHEETPAGE lpfnAdd,	// pointer to func to add pages
		LPARAM lParam );				// param to pass to above func

protected:
	CRegistryKey m_regKey;				// associated registry key
	CString m_strCompany;				// company name
	CString m_strProduct;				// product name
	CString m_strLanguage;				// locale language
	CString m_strVersion;				// version string
	CString m_strPartNum;				// specified SKU
	DWORD m_dwFlags;					// transaction-related flags
};

///////////////////////////////////////
// CAboutBoxFactory

class CAboutBoxFactory : public IClassFactory
{
	UINT m_cRef;						// object reference count
	CRefDll m_refDll;					// auto-updates DLL ref count

public:
	CAboutBoxFactory();
	~CAboutBoxFactory();

	// IUnknown
	STDMETHODIMP QueryInterface( REFIID riid, LPVOID FAR* ppvObj );
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();

	// IClassFactory
	STDMETHODIMP CreateInstance( LPUNKNOWN pUnkOuter, REFIID riid, 
		LPVOID FAR* ppvObj );
	STDMETHODIMP LockServer( BOOL fLock );
};


#endif // __STDABOUT_H__
