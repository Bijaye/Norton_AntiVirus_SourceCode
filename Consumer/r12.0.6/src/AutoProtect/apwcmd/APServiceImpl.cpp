// APServiceImpl.cpp: implementation of the CAPServiceImpl class.
//
//////////////////////////////////////////////////////////////////////

#include "APServiceImpl.h"
#include "platform.h"
#include "SavRT32.h"
#include "apw95cmd.h"
#include "navapcommands.h"

//////////////////////////////////////////////////////////////////////
// CAPServiceImpl::CAPServiceImpl()

CAPServiceImpl::CAPServiceImpl( CApw95Cmd& cmdObj ) :
	m_dwRef( 0 ),
	m_cmdObj( cmdObj )
{
	// Intentionally empty.
}

//////////////////////////////////////////////////////////////////////
// CAPServiceImpl::~CAPServiceImpl()

CAPServiceImpl::~CAPServiceImpl()
{
	// Intentionally empty.
}

//////////////////////////////////////////////////////////////////////
// CAPServiceImpl::QueryInterface()

STDMETHODIMP CAPServiceImpl::QueryInterface( REFIID iid, void ** ppvObject )
{
	*ppvObject = NULL;
	
    if( IsEqualIID( iid, IID_IUnknown )||														
		IsEqualIID( iid, __uuidof( INAVAPService ) ) )
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
// CAPServiceImpl::AddRef()

STDMETHODIMP_(ULONG) CAPServiceImpl::AddRef()
{
	return InterlockedIncrement( (long*) &m_dwRef );
}

//////////////////////////////////////////////////////////////////////
// CAPServiceImpl::Release()

STDMETHODIMP_(ULONG) CAPServiceImpl::Release()
{
	if( InterlockedDecrement( (long*) &m_dwRef ) == 0 )
	{
		delete this;
		return 0;
	}

	return m_dwRef;
}

//////////////////////////////////////////////////////////////////////
// CAPServiceImpl::RegisterHandler()

STDMETHODIMP CAPServiceImpl::RegisterHandler( INAVAPEventHandler* pHandler )
{
	return E_NOTIMPL;
}

//////////////////////////////////////////////////////////////////////
// CAPServiceImpl::UnRegisterHandler()

STDMETHODIMP CAPServiceImpl::UnRegisterHandler( INAVAPEventHandler* pHandler )
{
	return E_NOTIMPL;
}

//////////////////////////////////////////////////////////////////////
// CAPServiceImpl::GetAutoProtectEnabled()

STDMETHODIMP CAPServiceImpl::GetAutoProtectEnabled( BOOL* pbEnabled )
{
    switch ( m_cmdObj.GetStatusOfDriver() )
    {
    case AP_STATE_ENABLED:
        *pbEnabled = TRUE;
        break;

    default:
        *pbEnabled = FALSE;
        break;
	}

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
// CAPServiceImpl::SendCommand()

STDMETHODIMP CAPServiceImpl::SendCommand( BSTR pCommand, VARIANT vArrayData )
{
	// Figure out which command was issued.
	if( wcscmp( NAVAPCMD_ENABLE, pCommand ) == 0 )
	{
		if( m_cmdObj.EnableDriver() == FALSE )
        {
            // If the defs are bad then the user will already be alerted with
            // a CED explaining the problem
            if( m_cmdObj.GetDefsBad() )
                return S_FALSE;
			
            return E_FAIL;
        }
	}
	else if( wcscmp( NAVAPCMD_DISABLE, pCommand ) == 0 )
	{
        if( m_cmdObj.DisableDriver() == FALSE )
			return E_FAIL;
	}
	else if( wcscmp( NAVAPCMD_RESTART, pCommand ) == 0 )
	{
		// Tell the driver to reload.
        if( m_cmdObj.SvcReloadApConfig() == FALSE )
			return E_FAIL;

        // Just refreshing the Savrt.dat settings
        BOOL bEnableHeuristicScanSave = m_cmdObj.m_bEnableHeuristicScan;
        UINT uHeuristicLevelSave = m_cmdObj.m_uHeuristicLevel;

		m_cmdObj.SvcLoadAPOptions();

        if ( bEnableHeuristicScanSave != m_cmdObj.m_bEnableHeuristicScan ||
             uHeuristicLevelSave != m_cmdObj.m_uHeuristicLevel )
            {
            // Reload the virus definitions if heuristics have changed
            m_cmdObj.SvcReloadApEngine(TRUE);
            }
	}
	else if( wcscmp( NAVAPCMD_RELOADSETTINGS, pCommand ) == 0 )
	{
		// Reload the savrt.dat options.
		if( m_cmdObj.SvcLoadAPOptions() == FALSE )
			return E_FAIL;
	}
	else if( wcscmp( NAVAPCMD_WAITFORSTARTUP, pCommand ) == 0 )
	{
		return S_OK;	
	}
	else
		return E_INVALIDARG; // Unknown command.
	
	return S_OK;
}



