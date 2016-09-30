/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/SCANDLVR/VCS/factory.cpv   1.0   13 Apr 1998 15:14:24   JTaylor  $
/////////////////////////////////////////////////////////////////////////////
//
// factory.cpp - contains IClassFactory implementation for ScanObj project
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/SCANDLVR/VCS/factory.cpv  $
// 
//    Rev 1.0   13 Apr 1998 15:14:24   JTaylor
// Initial revision.
// 
//    Rev 1.1   16 Mar 1998 18:45:44   SEDWARD
// Include 'Quar32.h', added CQuarantineOptsFactory.
//
//    Rev 1.0   24 Feb 1998 17:33:10   DBuches
// Initial revision.
/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "factory.h"
#include "ScanDeliverDLL.h"

extern LONG g_dwObjs;
extern LONG g_dwLocks;


////////////////////////////////////////////////////////////////////////////////
//
// Function name	: CQuarantineDLLFactory::CQuarantineDLLFactory
// Description	    : Constructor for class factory object
// Return type		:
//
////////////////////////////////////////////////////////////////////////////////
// 8/28/97 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
CScanDeliverDLLFactory::CScanDeliverDLLFactory()
{
    InterlockedIncrement( &g_dwObjs );
    m_dwRef = 0;
}


////////////////////////////////////////////////////////////////////////////////
//
// Function name	: CQuarantineDLLFactory::~CQuarantineDLLFactory
// Description	    : Destructor for class factory object
// Return type		:
//
////////////////////////////////////////////////////////////////////////////////
// 8/28/97 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
CScanDeliverDLLFactory::~CScanDeliverDLLFactory()
{
    InterlockedDecrement( &g_dwObjs );
}

///////////////////////////////////////////////////////////////////
// IUnknown implementation

////////////////////////////////////////////////////////////////////////////////
//
// Function name	: CQuarantineDLLFactory::QueryInterface
// Description	    : This function will return a requested COM interface
// Return type		: STDMETHODIMP
// Argument         : REFIID riid - REFIID of interface requested
// Argument         : void** ppv - pointer to requested interface
//
////////////////////////////////////////////////////////////////////////////////
// 8/28/97 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CScanDeliverDLLFactory::QueryInterface( REFIID riid, void** ppv )
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
// Function name	: CScanDeliverDLLFactory::AddRef()
// Description	    : Increments reference count for this object
// Return type		: ULONG
//
////////////////////////////////////////////////////////////////////////////////
// 8/28/97 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(ULONG) CScanDeliverDLLFactory::AddRef()
{
    return ++m_dwRef;
}


////////////////////////////////////////////////////////////////////////////////
//
// Function name	: CScanDeliverDLLFactory::Release()
// Description	    : Decrements reference count for this object
// Return type		: ULONG
//
////////////////////////////////////////////////////////////////////////////////
// 8/28/97 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(ULONG) CScanDeliverDLLFactory::Release()
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
// Function name	: CScanDeliverDLLFactory::CreateInstance
// Description	    : Creates in instance of a FileIterator object
// Return type		: STDMETHODIMP
// Argument         : LPUNKNOWN pUnk
// Argument         : REFIID riid
// Argument         : void**
//
////////////////////////////////////////////////////////////////////////////////
// 8/28/97 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CScanDeliverDLLFactory::CreateInstance(LPUNKNOWN pUnk, REFIID riid, void** ppvObj)
{
    HRESULT hr;

    CScanDeliverDLL *pObj = NULL;

    //
    // Return an error code if pUnkOuter is not NULL, because we don't
    // support aggregation.
    //
    if (pUnk != NULL)
        return ResultFromScode (CLASS_E_NOAGGREGATION);

    *ppvObj = NULL;

    // Create new object
    pObj = new CScanDeliverDLL;

    if( pObj == NULL )
        return ResultFromScode( E_OUTOFMEMORY );

    hr = pObj->QueryInterface( riid, ppvObj );

    if( FAILED( hr ) )
        delete pObj;

    return hr;
}

////////////////////////////////////////////////////////////////////////////////
//
// Function name	: CScanDeliverDLLFactory::LockServer
// Description	    : Adjusts global lock count.  This is used by COM for
//                    resource optimization.
// Return type		: STDMETHODIMP
// Argument         : BOOL fLock
//
////////////////////////////////////////////////////////////////////////////////
// 8/28/97 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CScanDeliverDLLFactory::LockServer(BOOL fLock)
{
    if( fLock )
        InterlockedIncrement( &g_dwLocks );
    else
        InterlockedDecrement( &g_dwLocks );

    return NOERROR;
}
