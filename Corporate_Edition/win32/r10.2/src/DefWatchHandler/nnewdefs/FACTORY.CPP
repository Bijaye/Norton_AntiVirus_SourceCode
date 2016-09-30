// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/QUAR32/VCS/factory.cpv   1.1   16 Mar 1998 18:45:44   SEDWARD  $
/////////////////////////////////////////////////////////////////////////////
//
// factory.cpp - contains IClassFactory implementation for ScanObj project
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/QUAR32/VCS/factory.cpv  $
// 
//    Rev 1.1   16 Mar 1998 18:45:44   SEDWARD
// Include 'Quar32.h', added CQuarantineOptsFactory.
//
//    Rev 1.0   24 Feb 1998 17:33:10   DBuches
// Initial revision.
/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "factory.h"
#include "newdef.h"
#include <new>
extern LONG g_dwObjs;
extern LONG g_dwLocks;


////////////////////////////////////////////////////////////////////////////////
//
// Function name	: CDefWatchEventHandlerFactory::CDefWatchEventHandlerFactory
// Description	    : Constructor for class factory object
// Return type		:
//
////////////////////////////////////////////////////////////////////////////////
// 8/28/97 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
CDefWatchEventHandlerFactory::CDefWatchEventHandlerFactory()
{
    InterlockedIncrement( &g_dwObjs );
    m_dwRef = 0;
}


////////////////////////////////////////////////////////////////////////////////
//
// Function name	: CDefWatchEventHandlerFactory::~CDefWatchEventHandlerFactory
// Description	    : Destructor for class factory object
// Return type		:
//
////////////////////////////////////////////////////////////////////////////////
// 8/28/97 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
CDefWatchEventHandlerFactory::~CDefWatchEventHandlerFactory()
{
    InterlockedDecrement( &g_dwObjs );
}

///////////////////////////////////////////////////////////////////
// IUnknown implementation

////////////////////////////////////////////////////////////////////////////////
//
// Function name	: CDefWatchEventHandlerFactory::QueryInterface
// Description	    : This function will return a requested COM interface
// Return type		: STDMETHODIMP
// Argument         : REFIID riid - REFIID of interface requested
// Argument         : void** ppv - pointer to requested interface
//
////////////////////////////////////////////////////////////////////////////////
// 8/28/97 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CDefWatchEventHandlerFactory::QueryInterface( REFIID riid, void** ppv )
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
// Function name	: CDefWatchEventHandlerFactory::AddRef()
// Description	    : Increments reference count for this object
// Return type		: ULONG
//
////////////////////////////////////////////////////////////////////////////////
// 8/28/97 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(ULONG) CDefWatchEventHandlerFactory::AddRef()
{
    return ++m_dwRef;
}


////////////////////////////////////////////////////////////////////////////////
//
// Function name	: CDefWatchEventHandlerFactory::Release()
// Description	    : Decrements reference count for this object
// Return type		: ULONG
//
////////////////////////////////////////////////////////////////////////////////
// 8/28/97 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(ULONG) CDefWatchEventHandlerFactory::Release()
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
// Function name	: CDefWatchEventHandlerFactory::CreateInstance
// Description	    : Creates in instance of a FileIterator object
// Return type		: STDMETHODIMP
// Argument         : LPUNKNOWN pUnk
// Argument         : REFIID riid
// Argument         : void**
//
////////////////////////////////////////////////////////////////////////////////
// 8/28/97 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CDefWatchEventHandlerFactory::CreateInstance(LPUNKNOWN pUnk, REFIID riid, void** ppvObj)
{
    HRESULT hr;

    CNewDefHandler *pObj = NULL;

    //
    // Return an error code if pUnkOuter is not NULL, because we don't
    // support aggregation.
    //
    if (pUnk != NULL)
        return ResultFromScode (CLASS_E_NOAGGREGATION);

    *ppvObj = NULL;

	try
	{
	    // Create new object
		pObj = new CNewDefHandler;
	}
	catch (std::bad_alloc &) {}

    if( pObj == NULL )
        return ResultFromScode( E_OUTOFMEMORY );

    hr = pObj->QueryInterface( riid, ppvObj );

    if( FAILED( hr ) )
        delete pObj;

    return hr;
}

////////////////////////////////////////////////////////////////////////////////
//
// Function name	: CDefWatchEventHandlerFactory::LockServer
// Description	    : Adjusts global lock count.  This is used by COM for
//                    resource optimization.
// Return type		: STDMETHODIMP
// Argument         : BOOL fLock
//
////////////////////////////////////////////////////////////////////////////////
// 8/28/97 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CDefWatchEventHandlerFactory::LockServer(BOOL fLock)
{
    if( fLock )
        InterlockedIncrement( &g_dwLocks );
    else
        InterlockedDecrement( &g_dwLocks );

    return NOERROR;
}




