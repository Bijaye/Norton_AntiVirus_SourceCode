/////////////////////////////////////////////////////////////////////////////
// $  $
/////////////////////////////////////////////////////////////////////////////
//
// factory.cpp - contains IClassFactory implementation for QsSnd project
//
/////////////////////////////////////////////////////////////////////////////
// $Log:  $
// 
//    Rev 1.1   16 Mar 1998 18:45:44   SEDWARD
// Include 'Quar32.h', added CQuarantineOptsFactory.
//
//    Rev 1.0   24 Feb 1998 17:33:10   DBuches
// Initial revision.
/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "factory.h"
#include "qssnd.h"

extern LONG g_dwObjs;
extern LONG g_dwLocks;



////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// CQuarantineItemFactory implementation
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
//
// Function name	: CQuarantineItemFactory::CQuarantineItemFactory
// Description	    : Constructor for class factory object
// Return type		:
//
////////////////////////////////////////////////////////////////////////////////
// 8/28/97 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
CQuarantineItemFactory::CQuarantineItemFactory()
{
    InterlockedIncrement( &g_dwObjs );
    m_dwRef = 0;
}


////////////////////////////////////////////////////////////////////////////////
//
// Function name	: CQuarantineItemFactory::~CQuarantineItemFactory
// Description	    : Destructor for class factory object
// Return type		:
//
////////////////////////////////////////////////////////////////////////////////
// 8/28/97 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
CQuarantineItemFactory::~CQuarantineItemFactory()
{
    InterlockedDecrement( &g_dwObjs );
}


///////////////////////////////////////////////////////////////////
// IUnknown implementation

////////////////////////////////////////////////////////////////////////////////
//
// Function name	: CQuarantineItemFactory::QueryInterface
// Description	    : This function will return a requested COM interface
// Return type		: STDMETHODIMP
// Argument         : REFIID riid - REFIID of interface requested
// Argument         : void** ppv - pointer to requested interface
//
////////////////////////////////////////////////////////////////////////////////
// 8/28/97 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CQuarantineItemFactory::QueryInterface( REFIID riid, void** ppv )
{
    *ppv = NULL;

    if( IsEqualIID( riid, IID_IUnknown )||
        IsEqualIID( riid, IID_IClassFactory) )
        *ppv = this;

    if( *ppv )
        {
        ((LPUNKNOWN)*ppv)->AddRef();
        return ResultFromScode( S_OK );
        }

    return ResultFromScode( E_NOINTERFACE );
}


////////////////////////////////////////////////////////////////////////////////
//
// Function name	: CQuarantineItemFactory::AddRef()
// Description	    : Increments reference count for this object
// Return type		: ULONG
//
////////////////////////////////////////////////////////////////////////////////
// 8/28/97 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(ULONG) CQuarantineItemFactory::AddRef()
{
    return ++m_dwRef;
}


////////////////////////////////////////////////////////////////////////////////
//
// Function name	: CQuarantineItemFactory::Release()
// Description	    : Decrements reference count for this object
// Return type		: ULONG
//
////////////////////////////////////////////////////////////////////////////////
// 8/28/97 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(ULONG) CQuarantineItemFactory::Release()
{
    if( --m_dwRef )
        return m_dwRef;

    delete this;
    return 0;
}

///////////////////////////////////////////////////////////////////
// IClassFactory implementation


////////////////////////////////////////////////////////////////////////////////
//
// Function name	: CQuarantineItemFactory::CreateInstance
// Description	    : Creates in instance of a FileIterator object
// Return type		: STDMETHODIMP
// Argument         : LPUNKNOWN pUnk
// Argument         : REFIID riid
// Argument         : void**
//
////////////////////////////////////////////////////////////////////////////////
// 8/28/97 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CQuarantineItemFactory::CreateInstance(LPUNKNOWN pUnk, REFIID riid, void** ppvObj)
{
    HRESULT hr;

    // CQuarantineItem *pObj = NULL;
    CQsSnd *pObj = NULL;

    //
    // Return an error code if pUnkOuter is not NULL, because we don't
    // support aggregation.
    //
    if (pUnk != NULL)
        return ResultFromScode (CLASS_E_NOAGGREGATION);

    *ppvObj = NULL;

    // Create new object
    pObj = new CQsSnd;

    if( pObj == NULL )
        return ResultFromScode( E_OUTOFMEMORY );

    hr = pObj->QueryInterface( riid, ppvObj );

    if( FAILED( hr ) )
        delete pObj;

    return hr;
}

////////////////////////////////////////////////////////////////////////////////
//
// Function name	: CQuarantineItemFactory::LockServer
// Description	    : Adjusts global lock count.  This is used by COM for
//                    resource optimization.
// Return type		: STDMETHODIMP
// Argument         : BOOL fLock
//
////////////////////////////////////////////////////////////////////////////////
// 8/28/97 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CQuarantineItemFactory::LockServer(BOOL fLock)
{
    if( fLock )
        InterlockedIncrement( &g_dwLocks );
    else
        InterlockedDecrement( &g_dwLocks );

    return NOERROR;
}






