////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

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
#include "CLTProductHelper.h"
//#include "PRODINFOPEPHelper.h"

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
        CCTRACEI( _T("CluNavCallBack::OnLUNotify - Creating the CNavLu object") );
		m_pNavLu = new CNavLu();
		if ( m_pNavLu == NULL )
			throw E_OUTOFMEMORY;

        if( m_pNavLu->Initialize() == FALSE )
        {
            CCTRACEE( _T("CluNavCallBack::OnLUNotify - Failed to initialized the CNavLu object") );
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
                CCTRACEI( _T("CluNavCallBack::OnLUNotify - Symantec digital signature check failed on product reg dll.") );
                return E_ACCESSDENIED;
            }

			hr = m_ptrProductReg.CoCreateInstance( __uuidof( luProductReg ), NULL,(CLSCTX_INPROC|CLSCTX_LOCAL_SERVER ));
			if ( FAILED(hr) )
				throw hr;
		}

		switch ( nType )
		{
		case PreSession:
			CCTRACEI( _T("Notification type: PreSession") );

			if ( m_bPreProcessCompleted == FALSE )
			{
                // If PreProcess() returns true it means it performed the deftuils pre-processing
                // so we will set a flag to ensure post-process is called.
                if( m_pNavLu->PreProcess() )
				    m_bPreProcessCompleted = TRUE;
			}
			break;

		case PostSession:
			CCTRACEI( _T("Notification type: PostSession") );
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
						catch ( exception &ex )
						{
                            CCTRCTXE1(_T("Fail to update defs download count; exception: %s"), ex.what());
						}

						// Notify the UI that the defs may have changed.
						CGlobalEvent eventDefs;		// UI event to notify that defs have changed.

						eventDefs.CreateW( SYM_REFRESH_VIRUS_DEF_STATUS_EVENT );
						if ( eventDefs )
							::PulseEvent ( eventDefs );
					}
				}
			}
            else
                CCTRACEI( _T("CluNavCallBack::OnLUNotify - m_bPreProcessCompleted == FALSE.") );

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

					    CCLTProductHelper cltHelper;
                        hrx << cltHelper.Initialize();
                        hrx << cltHelper.DoOnLineTask();
	
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
    catch( _com_error &e )
    {
        CCTRCTXE1(_T("COM Exception: %lu"), e.Error());
        hr = e.Error();
    }
	catch ( exception &ex )
	{
        CCTRCTXE1(_T("Unknown exception: %s"), ex.what());
		hr = E_FAIL;
	}

	return S_OK;
}
