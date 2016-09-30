// Factory.cpp: implementation of the CFactory class.
//
//////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <ole2.h>
#include "Factory.h"
#include "APServiceImpl.h"
#include "NAVTrust.h"

//////////////////////////////////////////////////////////////////////
// CFactory::CFactory()

CFactory::CFactory( CApw95Cmd& cmdObj) :
	m_dwRef( 0 ),
	m_cmdObj( cmdObj )
{
	// Intentionally empty.
}

//////////////////////////////////////////////////////////////////////
// CFactory::CFactory()

CFactory::~CFactory()
{
	// Intentionally empty.
}

//////////////////////////////////////////////////////////////////////
// CFactory::QueryInterface()

STDMETHODIMP CFactory::QueryInterface( REFIID iid, void ** ppvObject )
{
	*ppvObject = NULL;
	
    if( IsEqualIID( iid, IID_IUnknown )||
		IsEqualIID( iid, IID_IClassFactory ) )
	{
		*ppvObject = this;
	}

    if( *ppvObject )
    {
        ((LPUNKNOWN)*ppvObject)->AddRef();
        return S_OK;
    }

    return E_NOINTERFACE;
}

//////////////////////////////////////////////////////////////////////
// CFactory::AddRef()

STDMETHODIMP_(ULONG) CFactory::AddRef()
{
	return InterlockedIncrement( (long*) &m_dwRef );
}

//////////////////////////////////////////////////////////////////////
// CFactory::Release()

STDMETHODIMP_(ULONG) CFactory::Release()
{
	if( InterlockedDecrement( (long*) &m_dwRef ) == 0 )
	{
		delete this;
		return 0;
	}

	return m_dwRef;
}

//////////////////////////////////////////////////////////////////////
// CFactory::CreateInstance()

STDMETHODIMP CFactory::CreateInstance( IUnknown * pUnkOuter, REFIID iid, void ** ppvObject )
{
	// Fail query interface if the loading application is not signed
	// by Symantec.
	if(NAVToolbox::NAVTRUST_OK != NAVToolbox::IsSymantecApplication())
		return E_FAIL;

	// We don't support aggregation.
	if( pUnkOuter )
		return CLASS_E_NOAGGREGATION; 
	
	// Create instance of INAVAPService object.
	CAPServiceImpl* pObj = new CAPServiceImpl( m_cmdObj );
	if( pObj == NULL )
		return E_OUTOFMEMORY;

	// Ask for interface.
	HRESULT hr = pObj->QueryInterface( iid, ppvObject );
	if( FAILED( hr ) )
	{
		delete pObj;
	}

	return hr;
}

//////////////////////////////////////////////////////////////////////
// CFactory::LockServer()

STDMETHODIMP CFactory::LockServer( BOOL fLock )
{
	// Do nothing, since this isn't an embedded object.
	return S_OK;
}

