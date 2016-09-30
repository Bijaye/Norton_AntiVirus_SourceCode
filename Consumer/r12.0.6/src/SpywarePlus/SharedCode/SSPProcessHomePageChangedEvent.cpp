#include "StdAfx.h"
#include "resource.h"

#include "HPPEventsInterface.h"
#include "HPPAppInterface.h"

#include "SSPProcessHomePageChangedEvent.h"

#include "ccAlertInterface.h"
#include "ccAlertLoader.h"

#include "AVRESBranding.h"
#include "NAVHelpLauncher.h"    // NAVToolbox

#include "SymHelp.h"

#pragma messsage(AUTO_FUNCNAME "TODO: Code Review Fixups")
//  1.) Move hard coded string to resources
//  2.) Add activity log entries
//  3.) Add statistics?

CSSPProcessHomePageChangedEvent::CSSPProcessHomePageChangedEvent(void)
{
}

CSSPProcessHomePageChangedEvent::~CSSPProcessHomePageChangedEvent(void)
{
}

HRESULT CSSPProcessHomePageChangedEvent::Run()
{
    HRESULT hrReturn = S_OK;
    CCTRACEI(_T("CSSPProcessHomePageChangedEvent::Run() BEGIN.\r\n"));

    //
    //  Protect ourselves from the evil that
    //   we are capable of.
    //
    try
    {
        CHPPEventCommonInterfaceQIPtr spCommon = m_spEventObj;
        if(spCommon)
        {
            if(HPP::Event_ID_HPPNotifyHomePage != spCommon->GetType())
            {
                CCTRACEE(_T("CSSPProcessHomePageChangedEvent::Run() - spCommon != HPP::Event_ID_HPPNotifyHomePage"));
                return S_OK;
            }
            
            HRESULT hr = E_FAIL;
            IHPPSessionAppInterfaceQIPtr spApp = m_spUnknownApp;
            if(!spApp)
            {
                CCTRACEE(_T("CSSPProcessHomePageChangedEvent::Run - unable to QI m_spUnknownApp for IHPPSessionAppInterface.\n"));
                return S_OK;
            }

            IHPPSettingsInterfaceQIPtr spSettings = m_spUnknownApp;
            if(!spSettings)
            {
                CCTRACEE(_T("CSSPProcessHomePageChangedEvent::Run - unable to QI m_spUnknownApp for IHPPSettingsInterface.\n"));
                return S_OK;
            }

            LONG lAlertFrequency;
            hr = spSettings->GetAlertFrequency(&lAlertFrequency);
            if(FAILED(hr))
            {
                CCTRACEE(_T("CSSPProcessHomePageChangedEvent::Run - spSettings->GetAlertFrequency failed.\n"));
                return S_OK;
            }

            if(HPP::HPP_ALERT_FREQUENCY_ON_HOME_PAGE_CHANGE != lAlertFrequency)
                // User probably changed options mid-alert, that's ok.
                return S_OK;

            bool bActionBlocked = true, bWasUserKey = true;
            long lValue = NULL;
            if(spCommon->GetPropertyLONG(HPP::Event_HPPNotifyHomePage_propActionResult, &lValue))
            {
                bActionBlocked = (HPP::HPP_ACTION_BLOCK == lValue);
            }
            
            if(spCommon->GetPropertyLONG(HPP::Event_HPPNotifyHomePage_propActionLocation, &lValue))
            {
                bWasUserKey = (HPP::HPP_LOCATIONS_HKCU == lValue);
            }

            int iAction = 0;
            
           
            cc::ccAlert_IAlert2 ccAlertLoader;
            cc::IAlert2Ptr spAlert;

            SYMRESULT sr;

            sr = ccAlertLoader.Initialize();
            if(SYM_FAILED(sr))
               return S_OK;

            sr = ccAlertLoader.CreateObject(&spAlert);
            if(SYM_FAILED(sr) || !spAlert)
               return S_OK;

            if(SUCCEEDED(AddAlertCommon(spCommon, spAlert)))
            {
               if(SUCCEEDED(AddAlertDetail(spCommon, spAlert)))
               {
                   spAlert->SetShowDetail(FALSE);
				  

                   CString cszDesc;

                   // Add action to change to security response home page help. (iAction = iFirstStockAction)
                   cszDesc.FormatMessage(_S(IDS_HPPALERT_ACTION_CHANGE), _S(IDS_HPPALERT_URL_HOME_PAGE_HELP_DESCRIPTION)); 
                   spAlert->AddAction(cszDesc);

                   // Add action to change to "about:blank". (iAction = iFirstStockAction + 1)
                   cszDesc.FormatMessage(_S(IDS_HPPALERT_ACTION_CHANGE), _S(IDS_HPPALERT_URL_ABOUT_BLANK_DESCRIPTION)); 
                   spAlert->AddAction(cszDesc);

                   // Add action to launch IE control panel (iAction = iFirstStockAction + 2)
                   spAlert->AddAction(_S(IDS_HPPALERT_FIRSTRUNDIALOG_ACTION_SHOW_IE_CPL));

                   iAction = spAlert->DisplayAlert();
               }
            }

            spAlert.Release();
            
            UINT iFirstStockAction = 0;
            if(!bActionBlocked)
            {
                _bstr_t cbszValue;

                // User chose to accept the change
                if(0 == iAction)
                {
                    // do nothing?
                }
                // User chose to undo the change
                else if(1 == iAction || 2 == iAction)
                {
                    // Get start value
                    if(spCommon->GetPropertyBSTR(HPP::Event_HPPNotifyHomePage_propStartValue, cbszValue.GetAddress()))
                    {
                        IHPPSessionAppInterfaceQIPtr spApp = m_spUnknownApp;
                        spApp->SwitchHomePage(m_spEventObj, cbszValue, bWasUserKey);
                    }
                }

                iFirstStockAction = 3;
            }
            else if(bActionBlocked)
            {
                _bstr_t cbszValue;
                CString cszNewHomePageVal;
                
                //  User chose to accept the block action
                if(0 == iAction)
                {
                    // do nothing?
                }
                //  User chose to undo the block action
                else if(1 == iAction)
                {
                    // Get start value
                    if(spCommon->GetPropertyBSTR(HPP::Event_HPPNotifyHomePage_propActionValue, cbszValue.GetAddress()))
                    {
                        cszNewHomePageVal = (LPCTSTR)cbszValue;
                    }

                    spApp->SwitchHomePage(m_spEventObj, cszNewHomePageVal, bWasUserKey);
                }
                
                iFirstStockAction = 2;
            }

            if(iFirstStockAction == iAction)
            {
                // user chose to change their home page to security response
                spApp->SwitchHomePage(m_spEventObj, _S(IDS_HPPALERT_URL_HOME_PAGE_HELP), true);
            }
            else if((iFirstStockAction + 1) == iAction)
            {
                // user chose to change their home page to about:blank
                spApp->SwitchHomePage(m_spEventObj, _S(IDS_HPPALERT_URL_ABOUT_BLANK), true);
            }
            else if((iFirstStockAction + 2) == iAction)
            {
                // user chose to change their home page with IE options
                spApp->ShowInternetExplorerControlPanel();
            }
        }


    }
    catch(...)
    {
        CCTRACEE("CSSPProcessHomePageChangedEvent::Run() - Unhandled exception caught.\r\n");
    }

    CCTRACEI(_T("CSSPProcessHomePageChangedEvent::Run() END.\r\n"));

    return hrReturn;
}

HRESULT CSSPProcessHomePageChangedEvent::AddAlertDetail(CHPPEventCommonInterface *pEvent, cc::IAlert2 *pAlert)
{
    // Ref count the objects
    cc::IAlert2Ptr spAlert = pAlert;
    CHPPEventCommonInterfacePtr spEvent = pEvent;

    // Sanity check
    if(!spAlert || !spEvent)
        return E_POINTER;

    _bstr_t cbszValue;
    CString cszProcessName, cszProcessPath, cszActionValue, cszStartValue;

    //
    // Fill in the details of Event
    //

    // Get process name
    if(spEvent->GetPropertyBSTR(HPP::Event_HPPNotifyHomePage_propProcessName, cbszValue.GetAddress()))
    {
        cszProcessName = (LPCTSTR)cbszValue;
    }
    else
    {
        cszProcessName = _S(IDS_UNKNOWN_STRING_VALUE);
        CCTRACEI(_T("CSSPProcessHomePageChangedEvent::AddAlertDetail - unable to get process name."));
    }

    // Get process path
    if(spEvent->GetPropertyBSTR(HPP::Event_HPPNotifyHomePage_propProcessPath, cbszValue.GetAddress()))
    {
        cszProcessPath = (LPCTSTR)cbszValue;
    }
    else
    {
        cszProcessPath = _S(IDS_UNKNOWN_STRING_VALUE);
        CCTRACEI(_T("CSSPProcessHomePageChangedEvent::AddAlertDetail - unable to get process path."));
    }



    // Get blocked value
    if(spEvent->GetPropertyBSTR(HPP::Event_HPPNotifyHomePage_propActionValue, cbszValue.GetAddress()))
    {
        cszActionValue = (LPCTSTR)cbszValue;
    }
    else
    {
        cszActionValue = _S(IDS_UNKNOWN_STRING_VALUE);
    }

    // Get start value
    if(spEvent->GetPropertyBSTR(HPP::Event_HPPNotifyHomePage_propStartValue, cbszValue.GetAddress()))
    {
        cszStartValue = (LPCTSTR)cbszValue;
    }
    else
    {
        cszStartValue = _S(IDS_UNKNOWN_STRING_VALUE);
    }

    bool bActionBlocked = true;
    long lValue = NULL;
    if(spEvent->GetPropertyLONG(HPP::Event_HPPNotifyHomePage_propActionResult, &lValue))
    {
        bActionBlocked = (HPP::HPP_ACTION_BLOCK == lValue);
    }

    CBrandingRes BrandRes;
    CString cszAppName = BrandRes.ProductName();

    if(bActionBlocked)
    {
        CString cszBriefDesc;
        
        cszBriefDesc.FormatMessage(_S(IDS_HPPALERT_SINGLE_BLOCKED_BRIEF_DESC), cszAppName);
        
        // Default action was to block
        pAlert->SetBriefDesc(cszBriefDesc);

        // Add actions
		pAlert->AddAction(_S(IDS_HPPALERT_BLOCKED_ACTION_DO_NOTHING));

		// If the action value is one of our stock values, don't add to the list.
        if(   0 != cszActionValue.CompareNoCase(_S(IDS_HPPALERT_URL_ABOUT_BLANK)) 
           && 0 != cszActionValue.CompareNoCase(_S(IDS_HPPALERT_URL_HOME_PAGE_HELP)) )
        {
            CString cszDesc;
            cszDesc.FormatMessage(_S(IDS_HPPALERT_ACTION_CHANGE), cszActionValue); 
            pAlert->AddAction(cszDesc);
        }

        // Description for blocked property
		pAlert->AddTableRow(_S(IDS_HPPALERT_OLDHOMEPAGEROW_NAME), cszStartValue);
        pAlert->AddTableRow(_S(IDS_HPPALERT_BLOCKEDVALROW_NAME), cszActionValue);

    }
    else
    {
        CString cszBriefDesc;

        cszBriefDesc.FormatMessage(_S(IDS_HPPALERT_SINGLE_ALLOWED_BRIEF_DESC), cszAppName);

        // Default action was to block
        pAlert->SetBriefDesc(cszBriefDesc);

        // Add actions
		pAlert->AddAction(_S(IDS_HPPALERT_SINGLE_ALLOWED_ACTION_DO_NOTHING));
        pAlert->AddAction(_S(IDS_HPPALERT_ALLOWED_ACTION_REVERT));

		CString cszDesc;
		cszDesc.FormatMessage(_S(IDS_HPPALERT_ACTION_CHANGE), cszStartValue); 
		pAlert->AddAction(cszDesc);

        // Description for blocked property
        pAlert->AddTableRow(_S(IDS_HPPALERT_OLDHOMEPAGEROW_NAME), cszStartValue);
        pAlert->AddTableRow(_S(IDS_HPPALERT_NEWHOMEPAGEROW_NAME), cszActionValue);

    }
	spAlert->AddTableRowPath(_S(IDS_HPPALERT_PROCROW_NAME), cszProcessName);
    spAlert->AddTableRow(_S(IDS_HPPALERT_LOCATIONROW_NAME), cszProcessPath);
    return S_OK;
}

HRESULT CSSPProcessHomePageChangedEvent::AddAlertCommon(CHPPEventCommonInterface *pEvent, cc::IAlert2 *pAlert)
{
    // Ref count the objects
    cc::IAlert2Ptr spAlert = pAlert;
    CHPPEventCommonInterfacePtr spEvent = pEvent;

    // Sanity check
    if(!spAlert || !spEvent)
        return E_POINTER;

    bool bThreatLevelLow = true;

    CBrandingRes BrandRes;
    CString cszAppName = BrandRes.ProductName();

    // Set up the window and app info
    spAlert->SetWindowIcon(_Module.GetResourceInstance(), IDI_NAVICON);
    spAlert->SetWindowTitle(cszAppName);
    spAlert->SetAlertTitle(_S(IDS_HPPALERT_SINGLE_TITLE));

    // Set up the help link
    spAlert->SetHelpText(_S(IDS_HPPALERT_SINGLE_HELPTEXT));

    if(bThreatLevelLow)
    {
     //   spAlert->SetThreatLevel(cc::IAlert2::THREAT_LOW);
      //  spAlert->SetThreatLevelBitmap(_Module.GetResourceInstance(), IDB_INFO_ICON);   
        spAlert->SetTitleBitmap(_Module.GetResourceInstance(), IDB_ALERT_TITLE_BLUE);
    }

  //  spAlert->SetPromptText(_S(IDS_HPPALERT_SINGLE_PROMPT_TEXT));
    pAlert->SetHelpCallback(this);

    return S_OK;
}

bool CSSPProcessHomePageChangedEvent::Run(HWND hWndParent, unsigned long nData, cc::IAlert* pAlert, cc::IAlertCallback::ALERTCALLBACK context)
{
    if(context == cc::IAlertCallback::ALERTCALLBACK_HELP)
    {
        NAVToolbox::CNAVHelpLauncher Help;
        return Help.LaunchHelp(NAVW_HOMEPAGE_ALERT, hWndParent) != NULL;
    }
    return true;
}
