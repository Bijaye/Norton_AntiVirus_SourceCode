#include "StdAfx.h"

#include "resource.h"
#include "HPPRes.h"

#include "HPPEventsInterface.h"
#include "HPPAppInterface.h"

#include "SSPProcessHPPFirstRun.h"

#include "ccAlertLoader.h"

#include "AVRESBranding.h"
#include "NAVHelpLauncher.h"    // NAVToolbox

#include "SymHelp.h"

using namespace ccEvtMgr;

CSSPProcessHPPFirstRun::CSSPProcessHPPFirstRun(void)
{
}

CSSPProcessHPPFirstRun::~CSSPProcessHPPFirstRun(void)
{
}

HRESULT CSSPProcessHPPFirstRun::Run()
{
    HRESULT hrReturn = S_OK;
    CCTRACEI(_T("CSSPProcessHPPFirstRun::Run() BEGIN.\r\n"));

    //
    //  Protect ourselves from the evil that
    //   we are capable of.
    //
    try
    {
        IHPPSessionAppInterfaceQIPtr spApp = m_spUnknownApp;
        if(!spApp)
        {
            CCTRACEE(_T("CSSPProcessHPPFirstRun::Run - unable to QI m_spUnknownApp for IHPPSessionAppInterface.\n"));
            return S_OK;
        }

        IHPPSettingsInterfaceQIPtr spSettings = m_spUnknownApp;
        if(!spSettings)
        {
            CCTRACEE(_T("CSSPProcessHPPFirstRun::Run - unable to QI m_spUnknownApp for IHPPSettingsInterface.\n"));
            return S_OK;
        }

        HRESULT hr;
        LONG lFirstRunDialog;
        hr = spSettings->GetFirstRunDialog(&lFirstRunDialog);
        if(FAILED(hr))
        {
            CCTRACEE(_T("CSSPProcessHPPFirstRun::Run - spSettings->GetFirstRunDialog failed.\n"));
            return S_OK;
        }

        if(HPP::HPP_FIRST_RUN_DIALOG_SHOW != lFirstRunDialog)
            // User has already seen first run dialog
            return S_OK;

        LONG lBlockBehavior = HPP::HPP_ACTION_BLOCK;
        hr = spSettings->GetDefaultBlockAction(&lBlockBehavior);
        if(FAILED(hr))
        {
            CCTRACEE(_T("CSSPProcessHPPFirstRun::Run - spSettings->GetDefaultBlockAction failed.\n"));
            return S_OK;
        }

        DWORD dwExplorerThreadId = NULL;
        CHPPEventCommonInterfaceQIPtr spCommon = m_spEventObj;
        if(spCommon)
        {
            LONG lThreadId;
            spCommon->GetPropertyLONG(HPP::Event_IE_Started_propThreadID, &lThreadId);
            dwExplorerThreadId = lThreadId;
        }

        cc::ccAlert_IAlert2 ccAlertLoader;
        cc::IAlert2Ptr spAlert;

        SYMRESULT sr;

        sr = ccAlertLoader.Initialize();
        if(SYM_FAILED(sr))
            return S_OK;

        sr = ccAlertLoader.CreateObject(&spAlert);
        if(SYM_FAILED(sr) || !spAlert)
            return S_OK;

        AddAlertDetail(spAlert, lBlockBehavior);

        int iAction = 0;
        iAction = spAlert->DisplayAlert();

        if(0 == iAction)
        {
            // user chose not to change their home page
        }
        else if(1 == iAction)
        {
            // user chose to change their home page to security response
            spApp->SwitchHomePage(m_spEventObj, _S(IDS_HPPALERT_URL_HOME_PAGE_HELP), true, dwExplorerThreadId);
        }
        else if(2 == iAction)
        {
            // user chose to change their home page to about:blank
            spApp->SwitchHomePage(m_spEventObj, _S(IDS_HPPALERT_URL_ABOUT_BLANK), true, dwExplorerThreadId);
        }
        else if(3 == iAction)
        {
            // user chose to change their home page with IE options
            spApp->ShowInternetExplorerControlPanel();

            // navigate the browser instance to the new home page
            if(NULL != dwExplorerThreadId)
                spApp->NavigateToHomePageByThread(dwExplorerThreadId);
        }
        spAlert.Release();

        spSettings->SetFirstRunDialog(HPP::HPP_FIRST_RUN_DIALOG_SUPPRESS);
        spSettings->WriteSettings();

    }
    catch(...)
    {
        CCTRACEE("CSSPProcessHPPFirstRun::Run() - Unhandled exception caught.\r\n");
    }

    CCTRACEI(_T("CSSPProcessHPPFirstRun::Run() END.\r\n"));

    return hrReturn;
}

HRESULT CSSPProcessHPPFirstRun::AddAlertDetail(cc::IAlert2 *pAlert, LONG lBlockBehavior)
{
    // Ref count the objects
    cc::IAlert2Ptr spAlert = pAlert;
    
    // Sanity check
    if(!spAlert)
        return E_POINTER;
    
    CBrandingRes BrandRes;
    CString cszAppName = BrandRes.ProductName();

    // Set up the window and app info
    spAlert->SetWindowIcon(_Module.GetResourceInstance(), IDI_NAVICON);
    spAlert->SetWindowTitle(cszAppName);
    spAlert->SetAlertTitle(_S(IDS_HPPALERT_FIRSTRUNDIALOG_TITLE));

    // Set up the help link
    spAlert->SetHelpText(_S(IDS_HPPALERT_SINGLE_HELPTEXT));

    spAlert->SetTitleBitmap(_Module.GetResourceInstance(), IDB_ALERT_TITLE_BLUE);

    UINT iBriefDesc = NULL;
    if(HPP::HPP_ACTION_BLOCK == lBlockBehavior)
    {
        iBriefDesc = IDS_HPPALERT_FIRSTRUNDIALOG_BLOCK_BRIEF_DESC;
    }
    else if(HPP::HPP_ACTION_ALLOW == lBlockBehavior)
    {
        iBriefDesc = IDS_HPPALERT_FIRSTRUNDIALOG_ALLOW_BRIEF_DESC;
    }

    CString cszBriefDesc;
    cszBriefDesc.FormatMessage(_S(iBriefDesc), cszAppName);
    pAlert->SetBriefDesc(cszBriefDesc);

    pAlert->SetHelpCallback(this);

    
    CString cszDesc;

    // Add action to do nothing (iAction = 0)
    pAlert->AddAction(_S(IDS_HPPALERT_FIRSTRUNDIALOG_ACTION_DO_NOTHING));

    // Add action to change to security response home page help. (iAction = 1)
    cszDesc.FormatMessage(_S(IDS_HPPALERT_ACTION_CHANGE), _S(IDS_HPPALERT_URL_HOME_PAGE_HELP_DESCRIPTION)); 
    pAlert->AddAction(cszDesc);

    // Add action to change to "about:blank". (iAction = 2)
    cszDesc.FormatMessage(_S(IDS_HPPALERT_ACTION_CHANGE), _S(IDS_HPPALERT_URL_ABOUT_BLANK_DESCRIPTION)); 
    pAlert->AddAction(cszDesc);

    // Add action to launch IE control panel (iAction = 3)
    pAlert->AddAction(_S(IDS_HPPALERT_FIRSTRUNDIALOG_ACTION_SHOW_IE_CPL));

    return S_OK;
}

bool CSSPProcessHPPFirstRun::Run(HWND hWndParent, unsigned long nData, cc::IAlert* pAlert, cc::IAlertCallback::ALERTCALLBACK context)
{
    if(context == cc::IAlertCallback::ALERTCALLBACK_HELP)
    {
        NAVToolbox::CNAVHelpLauncher Help;
        return Help.LaunchHelp(NAVW_HOMEPAGE_ALERT, hWndParent) != NULL;
    }
    return true;
}
