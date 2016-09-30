// luNavCallBack.cpp : Implementation of CluNavCallBack
#include "stdafx.h"
#include "DefAnnuity.h"
#include "navlucbk.h"
#include "luNavCallBack.h"
#include "strings.h"
#include "GlobalEvents.h"
//#include "GlobalEvent.h"    // For CGlobalEvent


// NOTE : Why are we bringing in the source for GlobalEvent instead
// of linking to the toolbox.lib ???? Well, this wonder project
// still use MFC and link it statically. Probably because it either
// gets called or used to get called during install. However, you
// can't mix static MFC and the non-static run-time C++ library that 
// Toolbox uses without linker errors and run-time bugs. SO, we
// are just building the GlobalEvent code ourselves. 
// 
// This should be resolved by removing the MFC dependency in this
// app. I think the only thing we use is CString anyway.
//
// GPOLK - 6/2/2001 - Trying to go Alpha!
//
//

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CluNavCallBack

STDMETHODIMP CluNavCallBack::OnLUNotify(tagLU_CALLBACK_TYPE nType, BSTR strMoniker)
{
	ATLTRACE(_T("CluNavCallBack::OnLUNotify called - notification: %x, Moniker: %s\n"), nType, 
															LPCTSTR( _bstr_t( strMoniker ) ) );

	if ( m_pNavLu == NULL )
	{
		m_pNavLu = new CNavLu( _Module.GetResourceInstance(), FALSE, FALSE );
		if ( m_pNavLu == NULL )
			throw E_OUTOFMEMORY;
	}

	HRESULT hr = S_OK;
	_variant_t vValue;
	CString sTitle;
	CString sStatusText;
	try 
	{
		if ( m_ptrProductReg == NULL )
		{
			hr = m_ptrProductReg.CreateInstance( __uuidof( luProductReg ), NULL,(CLSCTX_INPROC|CLSCTX_LOCAL_SERVER ));
			if ( FAILED(hr) )
				throw hr;
		}

		switch ( nType )
		{
		case PreSession:
			ATLTRACE( "Notification type: PreSession\r\n" );

			// If the PreSession callback has been called before this session, set the NAV 
			// command lines based on the result of the first call.
			if ( m_bPreProcessCompleted == FALSE )
			{
				m_pNavLu->PreProcess( );

				// Update command line parameters based on pre-processing
				m_pNavLu->DisableCommandLines( m_pNavLu->GetDefAnnuityStatus() );

				if ( m_pNavLu->GetDefAnnuityStatus() == DefAnnuityCancelUpdate )
					return( E_ABORT );

				// Clear any previous status text:
				hr = m_ptrProductReg->DeleteProperty( strMoniker, _bstr_t( g_szStatusText ) );

				m_bPreProcessCompleted = TRUE;
			}
			else
			{
				m_pNavLu->DisableCommandLines( m_pNavLu->GetDefAnnuityStatus() );
				if ( m_pNavLu->GetDefAnnuityStatus() == DefAnnuityCancelUpdate )
					return( E_ABORT );
			}
			break;

		case PostSession:
			ATLTRACE( "Notification type: PostSession\r\n" );
			if ( m_bPreProcessCompleted == TRUE )
			{
				if ( !m_bPostProcessCalled )
				{				
					// hr = m_ptrProductReg->GetProperty( strMoniker, _bstr_t( g_szLiveUpdateRetCode ), &vValue );
					// ASSERT( vValue.vt != VT_EMPTY );
					m_pNavLu->PostProcess( m_pNavLu->GetStatusCodes() );
					m_bPostProcessCalled = TRUE;
					m_pNavLu->GetStatusText( sStatusText, sTitle );
					hr = m_ptrProductReg->SetProperty( strMoniker, _bstr_t( g_szStatusText ), _bstr_t( LPCTSTR( sStatusText ) ) );
			
					// Notify the UI that the defs may have changed.
                    //CGlobalEvent eventDefs;		// UI event to notify that defs have changed.

					//eventDefs.Create( SYM_REFRESH_VIRUS_DEF_STATUS_EVENT );
					HANDLE eventDefs = OpenEvent ( EVENT_MODIFY_STATE, FALSE, SYM_REFRESH_VIRUS_DEF_STATUS_EVENT );

					if ( eventDefs )
                        ::PulseEvent ( eventDefs );

					CloseHandle ( eventDefs );
				}
			}
			break;

		case WelcomeText:
			ATLTRACE( "Notification type: WelcomeText\r\n" );
			break;

		case FinishText:
			ATLTRACE( "Notification type: FinishText\r\n" );
			if ( m_bPreProcessCompleted == TRUE )
			{
				if ( !m_bPostProcessCalled )
				{				
					// hr = m_ptrProductReg->GetProperty( strMoniker, _bstr_t( g_szLiveUpdateRetCode ), &vValue );
					// ASSERT( vValue.vt != VT_EMPTY );
					m_pNavLu->PostProcess( m_pNavLu->GetStatusCodes() );
					m_bPostProcessCalled = TRUE;
					hr = m_ptrProductReg->SetProperty( strMoniker, _bstr_t( g_szStatusText ), _bstr_t( LPCTSTR( sStatusText ) ) );

					// Notify the UI that the defs may have changed.
                    //CGlobalEvent eventDefs;		// UI event to notify that defs have changed.

					//eventDefs.Create( SYM_REFRESH_VIRUS_DEF_STATUS_EVENT );
					HANDLE eventDefs = OpenEvent ( EVENT_MODIFY_STATE, FALSE, SYM_REFRESH_VIRUS_DEF_STATUS_EVENT );

					if ( eventDefs )
                        ::PulseEvent ( eventDefs );

					CloseHandle ( eventDefs );
				}
			}
			break;

		default:
			break;
		}
	}
	catch ( ... )
	{
		ATLTRACE(_T("CluNavCallBack::OnLUNotify() - caught exception\n"));
		hr = E_FAIL;
	}

	return S_OK;
}
