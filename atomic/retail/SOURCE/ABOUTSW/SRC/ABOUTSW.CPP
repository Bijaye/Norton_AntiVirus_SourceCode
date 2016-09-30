// Copyright 1997 Symantec
// -------------------------------------------------------------------------
// aboutsw.CPP
//
// This is a skeletal about box System Works extension for NAV.  This the minimum 
// you need to implement to add a page to the Symantec Standard About box.
//
// Implementation of CAboutExt, CAboutExtFactory, CAboutPage
//
// $Header:   S:/ABOUTSW/VCS/aboutsw.cpv   1.4   29 Oct 1998 18:36:02   CEATON  $
// -------------------------------------------------------------------------
// $Log:   S:/ABOUTSW/VCS/aboutsw.cpv  $
// 
//    Rev 1.4   29 Oct 1998 18:36:02   CEATON
// Modified code to use parent key value passed back from GetSystemWorksKey().
// 
// 
//    Rev 1.3   22 Oct 1998 13:31:30   rchinta
// Added the new phone number and product offer
// 
//    Rev 1.2   21 Oct 1998 18:20:14   CEATON
// Changed registry key text to #define value in ABOUTSW.H. Renamed header file.
// 
// 
//    Rev 1.1   20 Oct 1998 20:24:38   CEATON
// Changed registry key for enabling SystemWorks upsell dialog box.
// 
// 
//    Rev 1.0   13 Oct 1998 18:44:20   rchinta
// Initial revision.
// 
// Initial revision.
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"                                             // precompiled header
#include "platform.h"                   // Platform independence
#include "resource.h"                                   // resource ID's
#include "symlean.h"                                    // light-weight class framework
#include "regguid.h"                                    // class ID's for on-line reg.
#include "regint.h"                                             // product transaction interfaces
#include "slhelper.h"
#include "sloldmap.h"
#include "aboutint.h"                                   // standard about interfaces
#include "aboutsw.h"
#include "aboutdlg.h"                                   // about extension declarations
#include "sysworks.h"

// Initialize GUIDS
#pragma data_seg( ".text" )
#define INITGUID
#include <initguid.h>
#include "aboutdlg.h"
#undef INITGUID
#pragma data_seg()

#if defined (WIN32)
    #define IS_WIN32 TRUE
#else
    #define IS_WIN32 FALSE
#endif

#define IS_NT      IS_WIN32 && (BOOL)(GetVersion() < 0x80000000)


/////////////////////////////////////////////////////////////////////////////
// Standard About Box Extension (Property Page)

///////////////////////////////////////
// Constructor

CAboutPage::CAboutPage() : CPropPage( IDD_ABOUT_EXT )
{
}

///////////////////////////////////////
// Destructor

CAboutPage::~CAboutPage()
{
}

BOOL CAboutPage::OnInitDialog()
{
	CslEditCtrl edtContact;

    edtContact.FromID( GetHandle(), IDC_EDIT_CONTACT );
    CslString strOffer1( IDS_PRODUCT_OFFER1 );
    CslString strOffer2( IDS_PRODUCT_OFFER2 );
    CslString strOffer3( IDS_PRODUCT_OFFER3 );
    CslString strOffer4( IDS_PRODUCT_OFFER4 );

    CslString strContact = strOffer1 + strOffer2 + strOffer3 + strOffer4;

    for ( int iString = IDS_PHONE00; iString <= IDS_PHONE37; iString++ )
    {
	CslString strPhone( iString );
	strContact += strPhone;
    }

    edtContact.SetText(strContact);

	return CPropPage::OnInitDialog();
}

///////////////////////////////////////


// UpdateData

BOOL CAboutPage::UpdateData( BOOL bSaveAndValidate )
{
	return TRUE;
}

///////////////////////////////////////
// OnSetActive

BOOL CAboutPage::OnSetActive()
{
	return CPropPage::OnSetActive();
}

///////////////////////////////////////
// OnRelease

void CAboutPage::OnRelease()
{
	delete this;
}

///////////////////////////////////////
// OnCommand

BOOL CAboutPage::OnCommand( int nID, HWND hCtrl, UINT uNotify )
{
	return CPropPage::OnCommand( nID, hCtrl, uNotify );
}


////////////////////////////////////////////////////////////////////////////
// CAboutExt

///////////////////////////////////////
// Construction

CAboutExt::CAboutExt() : m_cRef( 0 )
{
}

///////////////////////////////////////
// Destruction

CAboutExt::~CAboutExt()
{
}

////////////////////////////////////////////////////////////////////////////
// IUnknown

///////////////////////////////////////
// QueryInterface

STDMETHODIMP CAboutExt::QueryInterface( REFIID riid, LPVOID FAR* ppvObj )
{
	*ppvObj = NULL;

	if ( riid == IID_IUnknown )
		*ppvObj = (IUnknown*)( (ISymAboutExtInit*)this );

	else if ( riid == IID_ISymAboutExtInit )
		*ppvObj = (IUnknown*)( (ISymAboutExtInit*)this );

	else if ( riid == IID_IShellPropSheetExt )
		*ppvObj = (IUnknown*)( (IShellPropSheetExt*)this );

	// add tests for additional interfaces here...

	if ( *ppvObj )
	{
		( (LPUNKNOWN)*ppvObj )->AddRef();
		return NOERROR;
	}

	return E_NOINTERFACE;
}

///////////////////////////////////////
// AddRef

STDMETHODIMP_(ULONG) CAboutExt::AddRef()
{
	TRACE1( _T( "CAboutExt::AddRef() - %ld\n" ), m_cRef + 1 );
	return ( ++m_cRef );
}

///////////////////////////////////////
// Release

STDMETHODIMP_(ULONG) CAboutExt::Release()
{
	UINT uRef = --m_cRef;
	TRACE1( _T( "CAboutExt::Release() - %ld\n" ), uRef );

	if ( uRef == 0 )
		delete this;

	return uRef;
}

/////////////////////////////////////////////////////////////////////////////
// ISymAboutExtInit

////////////////////////////////////////
/*      Initialize


	Can be used to initialize the product transaction with the values
    necessary to construct a registry key in which to store any persistent data.
	You must call this method before calling any of the remaining methods
	in this interface.

    It is safe to ignore these parameters, unless you have a use for them
    (such as the example given above)

	Returns: S_OK if the operation succeeded, E_INVALIDARG if invalid
		arguments were passed, a storage-related error code if the
		trial storage medium could not be initialized.
*/
STDMETHODIMP CAboutExt::Initialize(
	LPCTSTR pszCompany,                             // company name (e.g. Symantec)
	LPCTSTR pszProduct,                             // product name (e.g. Norton Utilities)
	LPCTSTR pszVersion,                             // version string (e.g. 2.01)
	LPCTSTR pszLanguage,                    // product language
	DWORD dwFlags,                                  // transaction-related flags
	LPCTSTR pszPartNum )                    // product SKU
{
    // If you need the parameters, create member CStrings and keep them.
    // Otherwise, it's safe to ignore them.
	return S_OK;
}

////////////////////////////////////////
/*      SetData

    This is the transaction data object in use.  You can
    access the available properties through the data object and
    you can add your own.
*/
STDMETHODIMP CAboutExt::SetData(
	ITransactionData* pData )                       // pointer to data object
{
	return E_NOTIMPL;
}

////////////////////////////////////////////////////////////////
// IShellPropSheetExt

///////////////////////////////////////
// AddPages

STDMETHODIMP CAboutExt::AddPages( LPFNADDPROPSHEETPAGE lpfnAddPage,
	LPARAM lParam )
{
    HKEY hKey= 0,hParentKey;
	DWORD dwEnable, dwType, dwSize;
    TCHAR szKey[ SYM_MAX_PATH ];
    BOOL bUpsell = FALSE;
     
    dwEnable = 0;
    dwSize = sizeof(dwEnable);

    GetSystemWorksKey( &hParentKey,szKey );

    if (ERROR_SUCCESS == RegOpenKeyEx(hParentKey, szKey, 0, KEY_READ, &hKey))
    {
	if (ERROR_SUCCESS == RegQueryValueEx(hKey, TEXT("Enable"), 0, &dwType, (LPBYTE)&dwEnable, &dwSize ))
	{
	    if(dwEnable == 1)
		bUpsell = TRUE;
	    else
		bUpsell = FALSE;
	    RegCloseKey(hKey);
	}
    }


    if (!bUpsell)
	return NOERROR;

	TRACE( "CAboutExt::AddPages\n" );
    CAboutPage* pPage = new CAboutPage();

	if ( pPage != NULL )
	{
	    HPROPSHEETPAGE hPage =
			CreatePropertySheetPage( (LPCPROPSHEETPAGE)pPage );

		if ( hPage != NULL )
		{
			if ( lpfnAddPage( hPage, lParam ))
			{
				// release handled by page's OnRelease() method
				AddRef();
				return NOERROR;
			}
		}
	}

	if ( pPage != NULL )
		delete pPage;

	return E_OUTOFMEMORY;
}

///////////////////////////////////////
// ReplacePages

STDMETHODIMP CAboutExt::ReplacePage( UINT uPageID,
	LPFNADDPROPSHEETPAGE lpfnReplaceWith, LPARAM lParam )
{
	TRACE( "CAboutExt::ReplacePage\n" );
	return NOERROR;
}

/////////////////////////////////////////////////////////////////////////////
// CAboutExtFactory

///////////////////////////////////////
// Construction

CAboutExtFactory::CAboutExtFactory() : m_cRef( 0 )
{
}

///////////////////////////////////////
// Destruction

CAboutExtFactory::~CAboutExtFactory()
{
}

///////////////////////////////////////////////////////////
// IUnknown

///////////////////////////////////////
// QueryInterface

STDMETHODIMP CAboutExtFactory::QueryInterface( REFIID riid, LPVOID FAR* ppvObj )
{
	// any interface on this object is the object pointer
	if (( riid == IID_IUnknown ) || ( riid == IID_IClassFactory ))
	{
		*ppvObj = (LPVOID)this;
		AddRef();

		return NOERROR;
	}
	else
	{
		*ppvObj = NULL;
		return ( ResultFromScode( E_NOINTERFACE ));
	}
}

///////////////////////////////////////
// AddRef

STDMETHODIMP_(ULONG) CAboutExtFactory::AddRef()
{
	return ( ++m_cRef );
}

///////////////////////////////////////
// Release

STDMETHODIMP_(ULONG) CAboutExtFactory::Release()
{
	UINT uRef = --m_cRef;

	if ( uRef == 0 )
		delete this;

	return uRef;
}

///////////////////////////////////////////////////////////
// IClassFactory

///////////////////////////////////////
// CreateInstance

STDMETHODIMP CAboutExtFactory::CreateInstance( LPUNKNOWN pUnkOuter,
	REFIID riid, LPVOID FAR* ppvObj )
{
	// do not support aggregation
	if ( pUnkOuter != NULL )
		return CLASS_E_NOAGGREGATION;

	// instantiate item object
	CAboutExt* pAboutExt = new CAboutExt();

	// attempt to initialize interface
	if ( pAboutExt == NULL )
		return E_OUTOFMEMORY;

	pAboutExt->AddRef();
	HRESULT hResult = pAboutExt->QueryInterface( riid, ppvObj );
	pAboutExt->Release();

	return hResult;
}

///////////////////////////////////////
// LockServer

STDMETHODIMP CAboutExtFactory::LockServer( BOOL fLock )
{
	if ( fLock )
		InterlockedIncrement( &g_cLock );
	else
		InterlockedDecrement( &g_cLock );

	return NOERROR;
}
