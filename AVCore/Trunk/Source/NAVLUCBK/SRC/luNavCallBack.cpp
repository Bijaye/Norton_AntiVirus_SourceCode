// luNavCallBack.cpp : Implementation of CluNavCallBack
#include "stdafx.h"
#include "DefAnnuity.h"
#include "navlucbk.h"
#include "luNavCallBack.h"
#include "strings.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


/////////////////////////////////////////////////////////////////////////////
// CluNavCallBack

STDMETHODIMP CluNavCallBack::OnLUNotify(tagLU_CALLBACK_TYPE nType, BSTR strMoniker)
{
	ATLTRACE(_T("CluNavCallBack::OnLUNotify called - notification: %x, Moniker: %s\n"), nType, LPCTSTR( _bstr_t( strMoniker ) ) );

	if ( ! m_pNavLu )
	{
		m_pNavLu = new CNavLu( _Module.GetResourceInstance(), FALSE, FALSE );
	}

	HRESULT hr = S_OK;
	BOOL bReboot = FALSE;
	_variant_t vValue;
	CString sTitle;
	CString sStatusText;
	try 
	{
		if ( m_ptrProductReg == NULL )
         		hr = m_ptrProductReg.CreateInstance( __uuidof( luProductReg ),NULL,CLSCTX_INPROC );
		switch ( nType )
		{
		case PreSession:
                        ATLTRACE( "Notification type: PreSession\r\n" );
			m_pNavLu->PreProcess( );
			// Update command line parameters based on pre-processing
			m_pNavLu->DisableCommandLines( m_pNavLu->GetDefAnnuityStatus() );

			if ( m_pNavLu->GetDefAnnuityStatus() == DefAnnuityCancelUpdate )
                            return( E_ABORT );

			// Clear any previous status text:
			hr = m_ptrProductReg->DeleteProperty( strMoniker, _bstr_t( g_szStatusText ) );

            m_bPreProcessCompleted = TRUE;

			break;

		case PostSession:
                        ATLTRACE( "Notification type: PostSession\r\n" );
			if ( m_bPreProcessCompleted == TRUE )
			if ( !m_bPostProcessCalled )
				{				
				// hr = m_ptrProductReg->GetProperty( strMoniker, _bstr_t( g_szLiveUpdateRetCode ), &vValue );
				// ASSERT( vValue.vt != VT_EMPTY );
				m_pNavLu->PostProcess( m_pNavLu->GetStatusCodes() );
				m_bPostProcessCalled = TRUE;
				m_pNavLu->GetStatusText( sStatusText, sTitle, &bReboot );
				if ( bReboot )
					TRACE( _T("Nav says it needs a reboot!\n"));
				hr = m_ptrProductReg->SetProperty( strMoniker, _bstr_t( g_szStatusText ), _bstr_t( LPCTSTR( sStatusText ) ) );
				}
			break;

		case WelcomeText:
                        ATLTRACE( "Notification type: WelcomeText\r\n" );
			break;

		case FinishText:
                        ATLTRACE( "Notification type: FinishText\r\n" );
			if ( m_bPreProcessCompleted == TRUE )
			if ( !m_bPostProcessCalled )
				{				
				// hr = m_ptrProductReg->GetProperty( strMoniker, _bstr_t( g_szLiveUpdateRetCode ), &vValue );
				// ASSERT( vValue.vt != VT_EMPTY );
				m_pNavLu->PostProcess( m_pNavLu->GetStatusCodes() );
				m_bPostProcessCalled = TRUE;
				m_pNavLu->GetStatusText( sStatusText, sTitle, &bReboot );
				if ( bReboot )
					TRACE( _T("Nav says it needs a reboot!\n"));
				hr = m_ptrProductReg->SetProperty( strMoniker, _bstr_t( g_szStatusText ), _bstr_t( LPCTSTR( sStatusText ) ) );
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
