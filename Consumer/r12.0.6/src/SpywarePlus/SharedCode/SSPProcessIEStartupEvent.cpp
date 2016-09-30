#include "StdAfx.h"

#include "resource.h"

#include "HPPEventsInterface.h"
#include "HPPAppInterface.h"

#include "SSPProcessIEStartupEvent.h"

#include "ccAlertLoader.h"

#include "AVRESBranding.h"
#include "NAVHelpLauncher.h"    // NAVToolbox

#include "SymHelp.h"

using namespace ccEvtMgr;

#pragma messsage(AUTO_FUNCNAME "TODO: Code Review Fixups")
//  1.) Move hard coded string to resources
//  2.) Add activity log entries
//  3.) Add statistics?

CSSPProcessIEStartupEvent::CSSPProcessIEStartupEvent(void)
{
}

CSSPProcessIEStartupEvent::~CSSPProcessIEStartupEvent(void)
{
}

HRESULT CSSPProcessIEStartupEvent::Run()
{
    HRESULT hrReturn = S_OK;
    CCTRACEI(_T("CSSPProcessIEStartupEvent::Run() BEGIN.\r\n"));

    //
    //  Protect ourselves from the evil that
    //   we are capable of.
    //
    try
    {
        if(m_lstEvents.size())
        {
            IHPPSessionAppInterfaceQIPtr spApp = m_spUnknownApp;
            if(!spApp)
            {
                CCTRACEE(_T("CSSPProcessIEStartupEvent::Run - unable to QI m_spUnknownApp for IHPPSessionAppInterface.\n"));
                return S_OK;
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

            AddAlertCommon(spAlert);
            
            BOOL bAddSeparator = FALSE;
            long nCount = NULL;
            long nEventCount = m_lstEvents.size();
            for(nCount = 0; nCount < nEventCount; nCount++)
            {
                CEventExPtr spEvent = m_lstEvents[nCount];

                CHPPEventCommonInterfaceQIPtr spCommonEvent = spEvent;
                if(!spCommonEvent)
                    continue;

                if(HPP::Event_ID_HPPNotifyHomePage != spCommonEvent->GetType())
                {
                    CCTRACEE(_T("CSSPProcessHomePageChangedEvent::Run() - spCommon != HPP::Event_ID_HPPNotifyHomePage"));
                    return S_OK;
                }

                bool bActionBlocked = true;
                long lValue = NULL;
                if(spCommonEvent->GetPropertyLONG(HPP::Event_HPPNotifyHomePage_propActionResult, &lValue))
                {
                    bActionBlocked = (HPP::HPP_ACTION_BLOCK == lValue);
                }

                if(bAddSeparator)
                {
                    spAlert->AddTableRow(_S(IDS_HPPALERT_MULTI_SEPARATOR_COL1), _S(IDS_HPPALERT_MULTI_SEPARATOR_COL2));
                }

                AddAlertDetail(spCommonEvent, spAlert);
                bAddSeparator = TRUE;
            }
            
            DWORD dwExplorerThreadId = NULL;
            CHPPEventCommonInterfaceQIPtr spCommon = m_spEventObj;
            if(spCommon)
            {
                LONG lThreadId;
                spCommon->GetPropertyLONG(HPP::Event_IE_Started_propThreadID, &lThreadId);
                dwExplorerThreadId = lThreadId;
            }

            CString cszDesc;

            // Add action to change to security response home page help. (iAction = m_lstHomePages.size() + 1)
            cszDesc.FormatMessage(_S(IDS_HPPALERT_ACTION_CHANGE), _S(IDS_HPPALERT_URL_HOME_PAGE_HELP_DESCRIPTION)); 
            spAlert->AddAction(cszDesc);

            // Add action to change to "about:blank". (iAction = m_lstHomePages.size() + 2)
            cszDesc.FormatMessage(_S(IDS_HPPALERT_ACTION_CHANGE), _S(IDS_HPPALERT_URL_ABOUT_BLANK_DESCRIPTION)); 
            spAlert->AddAction(cszDesc);
            
            // Add action to launch IE control panel (iAction = m_lstHomePages.size() + 3)
            spAlert->AddAction(_S(IDS_HPPALERT_FIRSTRUNDIALOG_ACTION_SHOW_IE_CPL));

            spAlert->SetShowDetail(FALSE);
            iAction = spAlert->DisplayAlert();

            if(iAction && iAction <= m_lstHomePages.size()) // user chose to change their home page
            {
                // change to the home page the user wants
                spApp->SwitchHomePage(m_spEventObj, m_lstHomePages[iAction - 1], m_lstHomePageLocations[iAction - 1], dwExplorerThreadId);
            }
            if((m_lstHomePages.size() + 1) == iAction)
            {
                // user chose to change their home page to security response
                spApp->SwitchHomePage(m_spEventObj, _S(IDS_HPPALERT_URL_HOME_PAGE_HELP), true, dwExplorerThreadId);
            }
            else if((m_lstHomePages.size() + 2) == iAction)
            {
                // user chose to change their home page to about:blank
                spApp->SwitchHomePage(m_spEventObj, _S(IDS_HPPALERT_URL_ABOUT_BLANK), true, dwExplorerThreadId);
            }
            else if((m_lstHomePages.size() + 3) == iAction)
            {
                // user chose to change their home page with IE options
                spApp->ShowInternetExplorerControlPanel();
                
                // navigate the browser instance to the new home page
                if(NULL != dwExplorerThreadId)
                    spApp->NavigateToHomePageByThread(dwExplorerThreadId);
            }

            spAlert.Release();
            
        }


    }
    catch(...)
    {
        CCTRACEE("CSSPProcessIEStartupEvent::Run() - Unhandled exception caught.\r\n");
    }

    CCTRACEI(_T("CSSPProcessIEStartupEvent::Run() END.\r\n"));

    return hrReturn;
}

HRESULT CSSPProcessIEStartupEvent::AddAlertDetail(CHPPEventCommonInterface *pEvent, cc::IAlert2 *pAlert)
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

    bool bActionBlocked = true, bWasUserKey = false;
    long lValue = NULL;
    if(spEvent->GetPropertyLONG(HPP::Event_HPPNotifyHomePage_propActionResult, &lValue))
    {
        bActionBlocked = (HPP::HPP_ACTION_BLOCK == lValue);
    }
    
    if(spEvent->GetPropertyLONG(HPP::Event_HPPNotifyHomePage_propActionLocation, &lValue))
    {
        bWasUserKey = (HPP::HPP_LOCATIONS_HKCU == lValue);
    }

    if(bActionBlocked)
    {
        if(!AlreadyInActionList(cszActionValue))
        {
            // Add actions
            CString cszDesc;
            cszDesc.FormatMessage(_S(IDS_HPPALERT_ACTION_CHANGE), cszActionValue);
            pAlert->AddAction(cszDesc);
            m_lstHomePages.push_back(cszActionValue);
            m_lstHomePageLocations.push_back(bWasUserKey);
        }

        // Description for blocked property
		pAlert->AddTableRow(_S(IDS_HPPALERT_OLDHOMEPAGEROW_NAME), cszStartValue);
        pAlert->AddTableRow(_S(IDS_HPPALERT_BLOCKEDVALROW_NAME), cszActionValue);
    }
    else
    {
        if(!AlreadyInActionList(cszStartValue))
        {
            // Add actions
            CString cszDesc;
            cszDesc.FormatMessage(_S(IDS_HPPALERT_ACTION_CHANGE), cszStartValue);
            pAlert->AddAction(cszDesc);
            m_lstHomePages.push_back(cszStartValue);
            m_lstHomePageLocations.push_back(bWasUserKey);
        }

        // Description for blocked property
        pAlert->AddTableRow(_S(IDS_HPPALERT_OLDHOMEPAGEROW_NAME), cszStartValue);
        pAlert->AddTableRow(_S(IDS_HPPALERT_NEWHOMEPAGEROW_NAME), cszActionValue);
    }
	spAlert->AddTableRow(_S(IDS_HPPALERT_PROCROW_NAME), cszProcessName);
    spAlert->AddTableRowPath(_S(IDS_HPPALERT_LOCATIONROW_NAME), cszProcessPath);
    return S_OK;
}

HRESULT CSSPProcessIEStartupEvent::AddAlertCommon(cc::IAlert2 *pAlert)
{
    // Ref count the objects
    cc::IAlert2Ptr spAlert = pAlert;

    // Sanity check
    if(!spAlert)
        return E_POINTER;

    bool bThreatLevelLow = true;

    CBrandingRes BrandRes;
    CString cszAppName = BrandRes.ProductName();

    // Set up the window and app info
    spAlert->SetWindowIcon(_Module.GetResourceInstance(), IDI_NAVICON);
    spAlert->SetWindowTitle(cszAppName);
    spAlert->SetAlertTitle(_S(IDS_HPPALERT_MULTI_TITLE));

    // Set up the help link
    spAlert->SetHelpText(_S(IDS_HPPALERT_SINGLE_HELPTEXT));

    if(bThreatLevelLow)
    {
       // spAlert->SetThreatLevel(cc::IAlert2::THREAT_LOW);
       // spAlert->SetThreatLevelBitmap(_Module.GetResourceInstance(), IDB_INFO_ICON);   
        spAlert->SetTitleBitmap(_Module.GetResourceInstance(), IDB_ALERT_TITLE_BLUE);
    }
    
    long nBlocked = 0;
    long nAllowed = 0;

    LST_EVENTPTR::iterator itCount;
    for(itCount = m_lstEvents.begin(); itCount != m_lstEvents.end(); itCount++)
    {
        CHPPEventCommonInterfaceQIPtr spCommonEvent = *itCount;
        if(spCommonEvent)
        {
            bool bActionBlocked = true;
            long lValue = NULL;
            if(spCommonEvent->GetPropertyLONG(HPP::Event_HPPNotifyHomePage_propActionResult, &lValue))
            {
                if(HPP::HPP_ACTION_BLOCK == lValue)
                {
                    nBlocked++;
                }
                else
                {
                    nAllowed++;
                }

            }
        }

    }
    
    CString cszLongDesc;
    UINT iActionText = NULL;

    if(nBlocked && nAllowed)
    {
        cszLongDesc.FormatMessage(_S(IDS_HPPALERT_MULTI_BLOCKED_ALLOWED_BRIEF_DESC), cszAppName);
        iActionText = IDS_HPPALERT_MULTI_BLOCKED_ALLOWED_ACTION_DO_NOTHING;
    }
    else if(nBlocked)
    {
		if(nBlocked > 1) // more than one attempt
			cszLongDesc.FormatMessage(_S(IDS_HPPALERT_MULTI_BLOCKED_BRIEF_DESC), cszAppName);
		else
			cszLongDesc.FormatMessage(_S(IDS_HPPALERT_SINGLE_BLOCKED_BRIEF_DESC), cszAppName);

        iActionText = IDS_HPPALERT_BLOCKED_ACTION_DO_NOTHING;

	}
    else if(nAllowed)
    {
		if(nAllowed > 1) // more than one attempt
			cszLongDesc.FormatMessage(_S(IDS_HPPALERT_MULTI_ALLOWED_BRIEF_DESC), cszAppName);
		else
			cszLongDesc.FormatMessage(_S(IDS_HPPALERT_SINGLE_ALLOWED_BRIEF_DESC), cszAppName);
        
        iActionText = IDS_HPPALERT_MULTI_ALLOWED_ACTION_DO_NOTHING;
	}	
    else
    {
        // WTF?
        cszLongDesc.FormatMessage(_S(IDS_HPPALERT_MULTI_ERROR_BRIEF_DESC), cszAppName);
        iActionText = IDS_HPPALERT_MULTI_BLOCKED_ALLOWED_ACTION_DO_NOTHING;
    }
    
    pAlert->SetBriefDesc(cszLongDesc);
 //   spAlert->SetPromptText(_S(IDS_HPPALERT_MULTI_PROMPT_TEXT));
    pAlert->SetHelpCallback(this);

    pAlert->AddAction(_S(iActionText));

    return S_OK;
}

BOOL CSSPProcessIEStartupEvent::AlreadyInActionList(CString &cszItem)
{
    // If the action value is one of the stock items, then we don't 
    //  want to add it twice.
    if(   0 == cszItem.CompareNoCase(_S(IDS_HPPALERT_URL_ABOUT_BLANK)) 
       || 0 == cszItem.CompareNoCase(_S(IDS_HPPALERT_URL_HOME_PAGE_HELP)) )
    {
        return TRUE;
    }
    
    LST_CSTRING::iterator itCount;
    for(itCount = m_lstHomePages.begin(); itCount != m_lstHomePages.end(); itCount++)
    {
        if(0 == cszItem.CompareNoCase(*itCount))
        {
            return TRUE;
        }
    }

    return FALSE;
}

bool CSSPProcessIEStartupEvent::Run(HWND hWndParent, unsigned long nData, cc::IAlert* pAlert, cc::IAlertCallback::ALERTCALLBACK context)
{
    if(context == cc::IAlertCallback::ALERTCALLBACK_HELP)
    {
        NAVToolbox::CNAVHelpLauncher Help;
        return Help.LaunchHelp(NAVW_HOMEPAGE_ALERT, hWndParent) != NULL;
    }
    return true;
}
