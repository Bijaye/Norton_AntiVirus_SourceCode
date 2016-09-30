// IDSUpdater.h - helper class
//
// We are not catching any exceptions here. The caller takes care of error.
///////////////////////////////////////////////////////////////////////////

#pragma once

#include "stdafx.h"

#define INITIIDS
#include "IDSLoader.h"
#include "SymIDSI.h"
#include "ccTrace.h"
#include "IDSLoaderLink.h"          // Links to IDS libs
#include "ccEvent.h"           // For CEvent
#include "IWPSettingsInterface.h"   // For event name
#include "ProductRegCOMNames.h"

#define IWP_PRODUCT_NAME "NAV_IWP"

class CIDSUpdater
{
private:
    CIDSLoader m_IDSLoader;
	CSymPtr<SymIDS::IIDSDefsUpdate> m_pIDSUpdate;
	bool m_bIDSUpdateCreated;
    WORD m_wIDSDefsYear;
    WORD m_wIDSDefsMonth;
    WORD m_wIDSDefsDay;
    DWORD m_dwIDSDefsRev;

	bool LoadIDS()
	{
		if (m_bIDSUpdateCreated)
		{
            CCTRACEI ( "CIDSUpdater - IDS has been loaded already." );
			return true;
		}

        // Don't load this in the constructor since Trust might blow up.

        SYMRESULT result = m_IDSLoader.GetObject ( SymIDS::IID_IDSDefsUpdate, (ISymBase**) &m_pIDSUpdate );

        if ( SYM_FAILED (result))
        {
            CCTRACEE ( "CIDSUpdater - Failed to load IDS - 0x%x", result );
        }
		else
		{
            CCTRACEI ( "CIDSUpdater - IDS is loaded" );

            result = m_pIDSUpdate->Initialize (IWP_PRODUCT_NAME);

            if ( SYM_FAILED (result))
            {
                CCTRACEE ( "CIDSUpdater - Failed to init update - 0x%x", result );

				m_pIDSUpdate.Release();
            }
			else
			{
	            CCTRACEI ( "CIDSUpdater - IDS initialized" );

				m_bIDSUpdateCreated = true;
			}
		}
		return m_bIDSUpdateCreated;
	}

    bool GetDefsVersion (WORD &wYear, WORD &wMonth, WORD &wDay, DWORD &dwRev)
    {
        CCTRACEI("GetDefsVersion - starting");
        CSymPtr <SymIDS::IIDSDefsManager> pIDSDefs;

        SYMRESULT result = m_IDSLoader.GetObject ( SymIDS::IID_IDSDefsManager, (ISymBase**) &pIDSDefs );

        if ( SYM_FAILED (result))
        {
            CCTRACEE ( "CIDSManager::GetDefsVersion - Failed to load IDS - 0x%x", result );
            return false;
        }

        CCTRACEI("GetDefsVersion - initialize");
        result = pIDSDefs->Initialize ();
        if ( SYM_FAILED (result))
        {
            CCTRACEE ( "CIDSManager::GetDefsVersion - Failed to init IDS defs - 0x%x", result );
            return false;
        }

        CCTRACEI("GetDefsVersion - GetActiveDefsDate");
        result = pIDSDefs->GetActiveDefsDate ( wYear, wMonth, wDay, dwRev );
        if ( SYM_FAILED (result))
        {
            CCTRACEE ( "CIDSManager::GetDefsVersion - Failed to get IDS defs date - 0x%x", result );
            return false;
        }

        CCTRACEI("GetDefsVersion - done - success");

        return true;
    }

    void AbortIDSCmdLine()
    {
        CCTRACEI(_T("CIDSUpdater::AbortIDSCmdLine() - Begin."));
        IluProductReg *pReg = NULL;
		HRESULT hr = CoCreateInstance( CLSID_luProductReg, NULL, CLSCTX_INPROC, IID_IluProductReg, (LPVOID *)&pReg );

		if (SUCCEEDED(hr))
		{
            //
			// Set the LU Abort Property to YES for the Hub & MicroDefs
            //

            TCHAR szMoniker[101] = {0};
            unsigned long ulSize = 100;
			_bstr_t bstrHubMoniker;
			_bstr_t bstrMicroDefsMoniker;
			_bstr_t bstrDescriptiveName(LUPR_ABORT);
			_bstr_t bstrDescriptiveNameValue("YES");

            // Get the hub moniker from the registry
            CRegKey regIWPCmdLine;
            if( ERROR_SUCCESS == regIWPCmdLine.Open(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Symantec\\Norton AntiVirus\\LiveUpdate\\IWPCmdLines\\CmdLine1"), KEY_QUERY_VALUE) &&
                ERROR_SUCCESS == regIWPCmdLine.QueryStringValue(_T("ProductMoniker"), szMoniker, &ulSize) )
            {
                bstrHubMoniker = szMoniker;
            }
            else
            {
                // Hard-code the known NAV 2005 IWP IDS defs hub commandline
                CCTRACEE(_T("CIDSUpdater::AbortIDSCmdLine() - Failed to get the hub moniker from the registry, using hard-coded hub moniker."));
                bstrHubMoniker = "{E608DE78-7C7C-41af-9466-EBBD707FE3D2}";
            }

            // Close the hub key
            regIWPCmdLine.Close();

            // Get the curdefs moniker from the registry
            if( ERROR_SUCCESS == regIWPCmdLine.Open(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Symantec\\Norton AntiVirus\\LiveUpdate\\IWPCmdLines\\CmdLine2"), KEY_QUERY_VALUE) &&
                ERROR_SUCCESS == regIWPCmdLine.QueryStringValue(_T("ProductMoniker"), szMoniker, &ulSize) )
            {
                bstrMicroDefsMoniker = szMoniker;
            }
            else
            {
                // Hard-code the known NAV 2005 IWP IDS defs hub commandline
                CCTRACEE(_T("CIDSUpdater::AbortIDSCmdLine() - Failed to get the curdefs moniker from the registry, using hard-coded curdefs moniker."));
                bstrMicroDefsMoniker = "{7FD40184-A905-4f7d-8078-BE634AB384DA}";
            }

            // Close the curdefs key
            regIWPCmdLine.Close();

			if ( FAILED(pReg->SetProperty( bstrHubMoniker, bstrDescriptiveName, bstrDescriptiveNameValue )))
			{
				CCTRACEE ( _T("CIDSUpdater::AbortIDSCmdLine() - Failed to set Hub LU Abort property to YES.") );
			}

			if ( FAILED(pReg->SetProperty( bstrMicroDefsMoniker, bstrDescriptiveName, bstrDescriptiveNameValue )))
			{
				CCTRACEE ( _T("CIDSUpdater::AbortIDSCmdLine() - Failed to set MicroDefs LU Abort property to YES.") );
			}

			pReg->Release();
		}
		else
		{
			CCTRACEE( _T("Failed to create LiveUpdate object.") );
		}

        CCTRACEI(_T("CIDSUpdater::AbortIDSCmdLine() - End."));
    }

public:

    CIDSUpdater(void) : 
          m_wIDSDefsYear (0),
          m_wIDSDefsMonth (0),
          m_wIDSDefsDay (0),
          m_dwIDSDefsRev (0),
          m_bIDSUpdateCreated(false)
	{
	}

    ~CIDSUpdater(void)
    {
        CCTRACEI ("~CIDSUpdater - exiting");
    }

    void PreSession (void)
	{
		CCTRACEI(">> CIDSUpdater::PreSession()");

        if (LoadIDS())
        {
             // Get the current sig version
            //
            if ( 0 == m_wIDSDefsYear )
            {
                // Reset the year if it fails so we know something went wrong
                //
                if ( !GetDefsVersion ( m_wIDSDefsYear, m_wIDSDefsMonth, m_wIDSDefsDay, m_dwIDSDefsRev ))
                {
                    m_wIDSDefsYear = m_wIDSDefsMonth = m_wIDSDefsDay = m_dwIDSDefsRev = 0;
                }
            }

            SYMRESULT result = m_pIDSUpdate->PreDownload();
            if ( SYM_FAILED (result) )
            {
                CCTRACEE ( "CIDSUpdater::PreSession - Failed to PreDownload - 0x%x. Aborting commandlines", result );
                AbortIDSCmdLine();
            }
        }

		CCTRACEI("<< CIDSUpdater::PreSession()");
	}

    void PostSession (void)
	{
		CCTRACEI(">> CIDSUpdater::PostSession()");

        if (LoadIDS())
        {
			CCTRACEI("m_pIDSUpdate->PostDownload()");

            SYMRESULT result = m_pIDSUpdate->PostDownload();
            if ( SYM_FAILED (result) )
            {
                CCTRACEE ( "CIDSUpdater::PostSession - Failed to PostDownload - 0x%x. Aborting commandlines", result );
                AbortIDSCmdLine();
            }

            CCTRACEI("Getting defs version");

            // Get the new sig version and notify if they are different
            //
            WORD wYearNew = 0;
            WORD wMonthNew = 0;
            WORD wDayNew = 0;
            DWORD dwRevNew = 0;

            // If the year is zero we had an error getting it in the first place, so don't
            // bother comparing.
            //
            if ( GetDefsVersion ( wYearNew, wMonthNew, wDayNew, dwRevNew ))
            {
                if ( wYearNew != m_wIDSDefsYear ||
                     wMonthNew != m_wIDSDefsMonth ||
                     wDayNew != m_wIDSDefsDay ||
                     dwRevNew != m_dwIDSDefsRev )
                {
                    // Notify IWP that the IDS sigs have been updated.
                    //
                    ccLib::CEvent eventUpdate ( NULL, TRUE, TRUE, IWP::SYM_REFRESH_IDS_IWP_SIGS_EVENT, TRUE ); // Global
                    if (eventUpdate.PulseEvent ())
                    {
                        CCTRACEI("CIDSUpdater::PostSession - New IDS sigs - notify IWP");
                    }
                    else
                        CCTRACEE("CIDSUpdater::PostSession - New IDS sigs - failed to notify IWP");
                }
                else
                    CCTRACEI ( "CIDSUpdater::PostSession - IDS sig versions match" );
            }
            else
                CCTRACEE("FAILED - Getting defs version");
        }

		CCTRACEI("<< CIDSUpdater::PostSession()");
    }
};
