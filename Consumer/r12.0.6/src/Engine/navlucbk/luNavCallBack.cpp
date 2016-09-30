// luNavCallBack.cpp : Implementation of CluNavCallBack
#include "stdafx.h"

#include "NAVSettingsHelperEx.h"
#include "OptNames.h"

#include "navlucbk.h"
#include "luNavCallBack.h"
#include "strings.h"
#include "GlobalEvents.h"
#include "GlobalEvent.h"    // For CGlobalEvent
#include "navtrust.h"
#include "PepClientBase.h"
#include "PRODINFOPEPHelper.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CluNavCallBack

STDMETHODIMP CluNavCallBack::OnLUNotify(tagLU_CALLBACK_TYPE nType, BSTR strMoniker)
{
	CCTRACEI(_T("CluNavCallBack::OnLUNotify called - notification: %x, Moniker: %s"), nType, 
															LPCTSTR( _bstr_t( strMoniker ) ) );

	if ( m_pNavLu == NULL )
	{
        CCTRACEI( "CluNavCallBack::OnLUNotify - Creating the CNavLu object" );
		m_pNavLu = new CNavLu();
		if ( m_pNavLu == NULL )
			throw E_OUTOFMEMORY;

        if( m_pNavLu->Initialize() == FALSE )
        {
            CCTRACEE( "CluNavCallBack::OnLUNotify - Failed to initialized the CNavLu object" );
            return E_FAIL;
        }
	}

	HRESULT hr = S_OK;

	try 
	{
		if ( m_ptrProductReg == NULL )
		{
            // Check the LU dll for a valid symantec signature
            if( NAVToolbox::NAVTRUST_OK != NAVToolbox::IsSymantecComServer(__uuidof(luProductReg)) )
            {
                CCTRACEI( "CluNavCallBack::OnLUNotify - Symantec digital signature check failed on product reg dll." );
                return E_ACCESSDENIED;
            }

			hr = m_ptrProductReg.CoCreateInstance( __uuidof( luProductReg ), NULL,(CLSCTX_INPROC|CLSCTX_LOCAL_SERVER ));
			if ( FAILED(hr) )
				throw hr;
		}

		switch ( nType )
		{
		case PreSession:
			CCTRACEI( "Notification type: PreSession" );

			if ( m_bPreProcessCompleted == FALSE )
			{
				m_pNavLu->PreProcess( );

                // if the defs are not going to be downloaded then don't set the preprocess complete
                // flag because we don't need to call post process if there are no defs to process
                
                DWORD dwDefAnnuityStatus = m_pNavLu->GetDefAnnuityStatus();

				if ( dwDefAnnuityStatus == DefAnnuityCancelUpdate )
					return( E_ABORT );
                else if( dwDefAnnuityStatus == DefAnnuityUpdateAll )
				    m_bPreProcessCompleted = TRUE;
			}
			break;

		case PostSession:
			CCTRACEI( "Notification type: PostSession" );
			if ( m_bPreProcessCompleted == TRUE )
			{
				if ( !m_bPostProcessCalled )
				{
					m_pNavLu->PostProcess();
					m_bPostProcessCalled = TRUE;

                    // Make sure we have newer definitions installed
					if ( m_pNavLu -> GetVirusDefsDelivered () )
					{
                        try
						{
						    CNAVOptSettingsEx NavOpts;
							if (!NavOpts.Init())
					            throw FALSE;

							DWORD dwCount = 0;
							NavOpts.GetValue(VIRUSDEFS_DownloadCount, dwCount, 0);
							dwCount++;
							NavOpts.SetValue(VIRUSDEFS_DownloadCount, dwCount);
						}
						catch ( ... )
						{
							CCTRACEE(_T("Fail to update defs download count"));
						}

						// Notify the UI that the defs may have changed.
						CGlobalEvent eventDefs;		// UI event to notify that defs have changed.

						eventDefs.Create( SYM_REFRESH_VIRUS_DEF_STATUS_EVENT );
						if ( eventDefs )
							::PulseEvent ( eventDefs );

                        // Kick off the side effect scan
                        m_pNavLu->LaunchSideEffectScan();
					}
				}
			}
            else
                CCTRACEI( "CluNavCallBack::OnLUNotify - m_bPreProcessCompleted == FALSE." );

			break;

		case WelcomeText:
			{
				STAHLSOFT_HRX_TRY(hr)
				{
					DWORD dwAggregated = 0;
	
					READ_SETTING_START()
						READ_SETTING_DWORD(LICENSE_Aggregated, dwAggregated, 0)
					READ_SETTING_END
	
					// Do online task such as SOS license renewal only if NAV is standalone product.
					if(dwAggregated == 0)
					{
						CCTRACEI(_T("Notification type: WelcomeText"));
					
						CPEPClientBase pepBase;
						hrx << pepBase.Init();
						pepBase.SetContextGuid(PRODINFO::PRODINFOGUID);
						pepBase.SetComponentGuid(V2LicensingAuthGuids::SZ_NAV_SUITEOWNERGUID);
						pepBase.SetPolicyID(PRODINFO::POLICY_ID_ONLINE_TASK);						 
						hrx << pepBase.QueryPolicy();
						CCTRACEI(_T("Query online task policy successful"));
					}
				}
				STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr)
			}
			break;

		default:
			break;
		}
	}
	catch ( ... )
	{
		CCTRACEE(_T("CluNavCallBack::OnLUNotify() - caught exception"));
		hr = E_FAIL;
	}

	return S_OK;
}
