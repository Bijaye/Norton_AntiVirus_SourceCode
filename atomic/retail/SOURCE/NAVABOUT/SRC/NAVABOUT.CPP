// Copyright 1997 Symantec
// -------------------------------------------------------------------------
// navabout.CPP
//
// This is a skeletal about box extension for NAV.  This the minimum you need
// to implement to add a page to the Symantec Standard About box.
//
// Implementation of CAboutExt, CAboutExtFactory, CAboutPage
//
// $Header:   S:/NAVABOUT/VCS/NavAbout.cpv   1.1   24 Jun 1998 12:51:12   mdunn  $
// -------------------------------------------------------------------------
// $Log:   S:/NAVABOUT/VCS/NavAbout.cpv  $
// 
//    Rev 1.1   24 Jun 1998 12:51:12   mdunn
// Added #include's of slhelper.h and sloldmap.h to fix breakages caused by
// type names that changed from release 005 to 007.
// 
//    Rev 1.0   19 Aug 1997 14:09:50   KSACKIN
// Initial revision.
// 
//    Rev 1.9.1.1   14 Aug 1997 11:53:12   KSACKIN
// Removed some old code from our NavAboutDlg::InitDialog() function.  This was
// making a call to initialize the virus engine, but we are no longer using
// this property page.
//
//    Rev 1.9.1.0   12 Aug 1997 22:44:52   DALVIDR
// Branch base for version QAKG
//
//    Rev 1.9   30 Jul 1997 20:34:44   KSACKIN
// Removed our NavAbout custom about box page.
//
//    Rev 1.8   11 Jul 1997 13:29:34   DBUCHES
// Fixed problem with GetNumVirusDefinitions()
//
//    Rev 1.7   07 Jul 1997 09:11:06   TIVANOV
// remove definition version field
//
//    Rev 1.6   02 Jul 1997 09:47:14   TIVANOV
// fixing a build problem and cleanup the code
//
//    Rev 1.5   01 Jul 1997 14:39:36   TIVANOV
// new service functions to populate the about box
//
//    Rev 1.4   01 Jul 1997 11:18:26   TIVANOV
// update the product type field with the OS name
//
//    Rev 1.3   13 Jun 1997 08:49:12   TIVANOV
// display the definition version
//
//    Rev 1.2   10 Jun 1997 13:46:42   TIVANOV
// initilize the about dialog to the latest def date and # of viruses
//
//    Rev 1.1   09 Jun 1997 19:54:40   TIVANOV
// code to initialize the edit control
//
//    Rev 1.0   06 Jun 1997 20:01:12   BMCCORK
// Initial revision.
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"						// precompiled header
#include "resource.h"					// resource ID's
#include "symlean.h"					// light-weight class framework
#include "regguid.h"					// class ID's for on-line reg.
#include "regint.h"						// product transaction interfaces
#include "slhelper.h"
#include "sloldmap.h"
#include "aboutint.h"					// standard about interfaces
#include "navabout.h"					// about extension declarations

#undef  SYM_DEBUG
#include "defutils.h"                   // for NavVDefs* functions.
#include "avapi.h"
#include "xapi.h"
#define SYM_DEBUG

// Initialize GUIDS
#pragma data_seg( ".text" )
#define INITGUID
#include <initguid.h>
#include "navabout.h"
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

BOOL CAboutPage::IsNTServer()
{
    HKEY hKey = NULL;
    BOOL fRet = FALSE;

    if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("System\\CurrentControlSet\\Control\\ProductOptions"), 0, KEY_QUERY_VALUE, &hKey))
    {
        DWORD Type;
        BYTE  buff[100];
        DWORD dW = sizeof(buff);
        memset(buff, 0, dW);

        if (ERROR_SUCCESS == RegQueryValueEx(hKey, _T("Producttype"), 0, &Type, buff, &dW))
            if (strcmp((LPCTSTR)buff, _T("WinNT")))
                fRet = TRUE;
    }
    return (fRet);
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
/*	Initialize


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
	LPCTSTR pszCompany,				// company name (e.g. Symantec)
	LPCTSTR pszProduct,				// product name (e.g. Norton Utilities)
	LPCTSTR pszVersion,				// version string (e.g. 2.01)
	LPCTSTR pszLanguage,			// product language
	DWORD dwFlags,					// transaction-related flags
	LPCTSTR pszPartNum )			// product SKU
{
    // If you need the parameters, create member CStrings and keep them.
    // Otherwise, it's safe to ignore them.
	return S_OK;
}

////////////////////////////////////////
/*	SetData

    This is the transaction data object in use.  You can
    access the available properties through the data object and
    you can add your own.
*/
STDMETHODIMP CAboutExt::SetData(
	ITransactionData* pData )			// pointer to data object
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
/********************************************************************
***************************** TEMPORARY *****************************
******************** This should be removed ASAP ********************
********************************************************************/
    return NOERROR;
/********************************************************************
***************************** TEMPORARY *****************************
********************************************************************/

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
