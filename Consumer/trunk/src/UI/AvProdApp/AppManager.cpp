////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#include <resource.h>
#include <AvProdAppRes.h>

#include "AppManager.h"
#include <AvProdCommands.h>

#include <ccInstanceFactory.h>
#include <ccSymMemoryImpl.h>
#include <ccSymMemoryStreamImpl.h>
#include <ccSymStringImpl.h>
#include <ccServiceLoader.h>
#include <ccEventManagerHelper.h>
#include <ccSettingsManagerHelper.h>

#include <SessionAppWindow.h>
#include <SessionAppWindowThread.h>
#include <SessionAppServerImpl.h>
#include <SessionAppComm.h>
#include <SessionAppClientImpl.h>

#include <AvProdElements.h>

#include <OptNames.h>

#include <ISVersion.h>
#include <isSymTheme.h>

#include <uiNumberDataInterface.h>

#include <ScanUILoader.h>

#include <ccAlertLoader.h>
#include <AvProdLoggingLoader.h>

#include <navinfo.h>

#define NAVAP_SETTINGS_PATH               _T("Norton Antivirus\\NAVOPTS.DAT\\NAVAP")
#define NAVAP_SETTINGS_TRAYICONSTATE      _T("ShowIcon")
#define NAVAP_SETTINGS_DEBUGMENU          _T("ShowDebugMenu")

using namespace AvProdApp;
using namespace AvProd;

struct ICON_ENTRY{
    DWORD dwState;
    UINT  iIconId;
    UINT  iToolTipTextId;
};

ICON_ENTRY g_pTrayIconEntry[] = { SessionApp::AppState::eNormal, IDI_TRAYICON_NORMAL, IDS_TRAYICON_TEXT_NORMAL,
                                  SessionApp::AppState::eAbnormal, IDI_TRAYICON_ABNORMAL, IDS_TRAYICON_TEXT_ABNORMAL,
                                  SessionApp::AppState::eUnknown, IDI_TRAYICON_ABNORMAL, IDS_TRAYICON_TEXT_UNKNOWN,
                                  SessionApp::NotifyState::eWorking, IDI_TRAYICON_WORKING, IDS_TRAYICON_TEXT_WORKING, 
                                  SessionApp::NotifyState::eInformation, IDI_TRAYICON_INFORMATION, IDS_TRAYICON_TEXT_INFORMATION,
                                  SessionApp::NotifyState::eWarning, IDI_TRAYICON_WARNING, IDS_TRAYICON_TEXT_WARNING
};

CAppManagerPtr CAppManagerSingleton::m_spAppManager;

//****************************************************************************
//****************************************************************************
//****************************************************************************
//****************************************************************************
CAppManager::CAppManager(void) :
m_hShutdownRequest(NULL),
m_dwExitCode(NULL),
m_dwTrayIconState(NAVAP_ShowTrayIcon_Invalid),
m_dwDebugMenuState(DBGOPTS_DebugMenu_Invalid)
{
}

CAppManager::~CAppManager(void)
{
    Destroy();
}

//****************************************************************************
//****************************************************************************
HRESULT CAppManager::Initialize()
{
    StahlSoft::HRX hrx;
    ccLib::CExceptionInfo exceptionInfo;
    try
    {
        // Need to take a LoadLibrary() lock on this to ensure
        // serialized objects from AvModule can be re-hydrated      
        AvLoaderMapStrData.Initialize();

        // Register the instance factory for IString.
        CCTRACEI( CCTRCTX _T("Getting instance factory"));
        cc::IInstanceFactoryQIPtr pInstanceFactory;
        pInstanceFactory = ccLib::CInstanceFactory::GetInstanceFactory();
        if(!pInstanceFactory)
            throw _com_error(E_UNEXPECTED);

        SYMRESULT sr;
        CCTRACEI( CCTRCTX _T("Registering IString factory"));
        if(!pInstanceFactory->HasFactory(AVModule::OBJID_AVMapStrData))
        {
            TCHAR szModulePath[512] = {0};
            size_t nModulePathSize = 512;
            AVModule::AVLoader_IAVMapStrData AVMapStrDataLoader;
            AVModule::CAVPathProvider::GetPath(szModulePath, nModulePathSize);
            PathAppend(szModulePath, AVMapStrDataLoader.GetDLLName());

            sr = pInstanceFactory->AddFactory(AVModule::OBJID_AVMapStrData, szModulePath);

            AVModule::AVLoader_IAVMapDwordData AVMapDwordDataLoader;
            AVModule::CAVPathProvider::GetPath(szModulePath, nModulePathSize);
            PathAppend(szModulePath, AVMapDwordDataLoader.GetDLLName());
            sr = pInstanceFactory->AddFactory(AVModule::OBJID_AVMapDwordData, szModulePath);
            
        }

        HMODULE hRes;
            
        hrx << SessionApp::CServerImpl::CreateObject(m_spSessionAppServer);
        hrx << CServerSinkImpl::CreateObject(m_spSessionAppServerSink);

        hrx << m_spSessionAppServer->RegisterEventSink(m_spSessionAppServerSink);
        hrx << m_spSessionAppServer->Initialize(L"AvProdSession");

        DWORD dwIndex = NULL;
        for(dwIndex = 0; dwIndex < CCDIMOF(g_pTrayIconEntry); dwIndex++)
        {
            hRes = AtlFindResourceInstance(g_pTrayIconEntry[dwIndex].iIconId, RT_ICON);
            hrx << m_spSessionAppServer->SetupTrayIcon(g_pTrayIconEntry[dwIndex].dwState, hRes, g_pTrayIconEntry[dwIndex].iIconId);

            CString cszToolTipText, cszTemp = _S(g_pTrayIconEntry[dwIndex].iToolTipTextId);
            cszToolTipText.FormatMessage(cszTemp, GetProductName());

            hrx << m_spSessionAppServer->SetupToolTipText(g_pTrayIconEntry[dwIndex].dwState, cszToolTipText);
        }
        
        ISShared::ISShared_IProvider ProviderLoader;
        sr = ProviderLoader.CreateObject(GETMODULEMGR(), &m_spISElementProvider);
        hrx << AvProdWidgets::HRESULT_FROM_SYMRESULT(sr);

        // Auto-Protect state element
        // The Element system will return failure codes under normal conditions, so don't
        // throw an execption.
        //
        HRESULT hr = m_spISElementProvider->GetElement(ISShared::CLSID_NIS_AutoProtect, m_spAutoProtectStateElement);

        if ( SUCCEEDED (hr))
        {
            ui::IUpdateQIPtr spUpdate = m_spAutoProtectStateElement;
            if(spUpdate)
            {
                ui::IUpdateQIPtr spUpdateSink = m_spSessionAppServerSink;
                spUpdate->Register(spUpdateSink, (LPVOID)m_spAutoProtectStateElement);
            }
        }
        else
            CCTRCTXE1 (_T("Failed getting AP element 0x%x"), hr);

        // NAV Options, will fail when non-admin, etc.
        //
        hr = m_spISElementProvider->GetElement(ISShared::CLSID_NAV_Options, m_spProductOptions);
        if ( FAILED(hr))
            CCTRCTXW1 (_T("Failed getting nav options 0x%x"), hr);

        hRes = AtlFindResourceInstance(IDR_MENU_TRAY, RT_MENU);
        hrx << m_spSessionAppServer->SetContextMenu(hRes, IDR_MENU_TRAY);
        hrx << m_spSessionAppServer->SetDefaultMenuItem(ID_TRAYMENU_OPENAVPRODUCT);
        
        hrx << InitializeSettings();

        sr = m_ISubjectLoader.CreateObject(&m_spSubject);
        if(SYM_SUCCEEDED(sr))
        {
            SettingsEventHelper::IObserverQIPtr spObserver = m_spSessionAppServerSink;
            sr = m_spSubject->Attach(spObserver, ccSettings::CSettingsChangeEventEx::Global, NAVAP_SETTINGS_PATH);
            if(SYM_FAILED(sr))
            {
                CCTRACEW( CCTRCTX _T("Unable to register for settings change notifications."));
            }
        }
        else
        {
            CCTRACEW( CCTRCTX _T("Unable to register for settings change notifications."));
        }


    }
    CCCATCHMEM(exceptionInfo)
    CCCATCHCOM(exceptionInfo);

    if(exceptionInfo.IsException())
        return E_FAIL;


#pragma message(_T("TODO: Setup account rights and init AutoProtect state"))
    return S_OK;
}

//****************************************************************************
//****************************************************************************
void CAppManager::Destroy()
{
    m_spSessionAppServer->Destroy();
    return;
}

//****************************************************************************
//****************************************************************************
HRESULT CAppManager::InitializeSettings()
{
    ccSettings::CSettingsManagerHelper ccSettingsHelper;
    ccSettings::ISettingsManagerPtr spSettingsManager;

    SYMRESULT sr;

    // Setup the settings manager helper
    sr = ccSettingsHelper.Create(spSettingsManager);
    if(SYM_FAILED(sr) || !spSettingsManager)
    {
        CCTRACEW( CCTRCTX _T("CSettingsManagerHelper::Create() failed. sr = 0x%08X"), sr);
        return E_FAIL;
    }

    // Retrieve our settings block
    ccSettings::ISettingsPtr spSettings;
    sr = spSettingsManager->GetSettings(NAVAP_SETTINGS_PATH, &spSettings);
    if(SYM_SUCCEEDED(sr) || spSettings)
    {
        OnSettingsChanged(NAVAP_SETTINGS_PATH, spSettings);
        spSettings.Release();
    }
    else
    {
        CCTRACEW( CCTRCTX _T("CreateSettings(NAVAP_SETTINGS_PATH) failed. sr = 0x%08X"), sr);
        
    }
    

    return S_OK;
}

//****************************************************************************
//****************************************************************************
void CAppManager::OnSettingsChanged(LPCWSTR szSettingsKey, ccSettings::ISettings* pSettings)
{   
    // if the ccSettings key is removed or changed, the pSettings parameter is NULL
    if(!pSettings)
        return;

    BOOL bTrayStateChanged = FALSE, bAutoProtectStateChanged = FALSE;
    
    if(0 == _tcsicmp(szSettingsKey, NAVAP_SETTINGS_PATH))
    {
        SYMRESULT sr;
        DWORD dwShowTrayIcon = NAVAP_ShowTrayIcon_Invalid;
        sr = pSettings->GetDword(NAVAP_SETTINGS_TRAYICONSTATE, dwShowTrayIcon);
        if(SYM_SUCCEEDED(sr))
        {
            if(m_dwTrayIconState != dwShowTrayIcon)
            {
                m_dwTrayIconState = dwShowTrayIcon;
                bTrayStateChanged = TRUE;
            }
        }

        DWORD dwShowDebugMenu = DBGOPTS_DebugMenu_Invalid;
        sr = pSettings->GetDword(NAVAP_SETTINGS_DEBUGMENU, dwShowDebugMenu);
        if(SYM_SUCCEEDED(sr))
        {
            m_dwDebugMenuState = dwShowDebugMenu;
        }

        if(bTrayStateChanged)
            UpdateTrayAppState();
    }

    return;
}

//****************************************************************************
//****************************************************************************
void CAppManager::UpdateTrayAppState()
{
    DWORD dwAutoProtectState = GetAutoProtectState();
    DWORD dwTrayIconState = m_dwTrayIconState;

    if(NAVAP_ShowTrayIcon_Invalid == dwTrayIconState || NAVAP_ShowTrayIcon_FALSE == dwTrayIconState)
    {
        m_spSessionAppServer->HideTrayApp();
    }
    else if(AutoProtectState_Invalid == dwAutoProtectState || AutoProtectState_Unknown == dwAutoProtectState)
    {
        m_spSessionAppServer->ShowTrayApp(SessionApp::AppState::eUnknown);
    }
    else if(AutoProtectState_Disabled == dwAutoProtectState)
    {
        m_spSessionAppServer->ShowTrayApp(SessionApp::AppState::eAbnormal);
    }
    else if(AutoProtectState_Enabled == dwAutoProtectState)
    {
        m_spSessionAppServer->ShowTrayApp(SessionApp::AppState::eNormal);
    }
    else
    {
        CCTRACEW( CCTRCTX _T("Unexpected state!! dwTrayIconState: %d, dwAutoProtectState: %d"), dwTrayIconState, dwAutoProtectState);
    }

    return;
}

//****************************************************************************
//****************************************************************************
HRESULT CAppManager::OpenAvProduct()
{
    LaunchElement(ISShared::CLSID_NIS_MainUI);

    return S_OK;
}

void CAppManager::LaunchElement (SYMGUID guidElementId)
{

    StahlSoft::HRX hrx;
    ccLib::CExceptionInfo exceptionInfo;
    try
    {
        ui::IElementPtr pElement;
        hrx << m_spISElementProvider->GetElement ( guidElementId, pElement);
        hrx << pElement->Configure (::GetDesktopWindow(), NULL);
        
        CCTRACEI( CCTRCTX _T("Element successfully launched"));
    }
    CCCATCHMEM(exceptionInfo)
    CCCATCHCOM(exceptionInfo);

    if(exceptionInfo.IsException())
    {
        CCTRCTXE1(_T("Unable to launch element. Error: %s"), exceptionInfo.GetDescription());
    }

    return;
}


//****************************************************************************
//****************************************************************************
HRESULT CAppManager::ViewRecentHistory()
{
    LaunchElement(ISShared::CLSID_NIS_MessageCenter);

    return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CAppManager::EnableAutoProtect(BOOL bEnabled)
{
    
    StahlSoft::HRX hrx;
    ccLib::CExceptionInfo exceptionInfo;
    try
    {
        ui::IDataPtr spData;
        hrx << m_spAutoProtectStateElement->GetData(spData);

        ui::INumberDataQIPtr spNumber = spData;
        if(!spNumber)
            hrx << E_UNEXPECTED;

        hrx << spNumber->SetNumber((LONGLONG)bEnabled);
        
        CCTRACEI( CCTRCTX _T("AutoProtect state set to: %s"), bEnabled ? _T("Enabled") : _T("Disabled"));
    }
    CCCATCHMEM(exceptionInfo)
    CCCATCHCOM(exceptionInfo);

    if(exceptionInfo.IsException())
    {
        CCTRCTXE1(_T("Unable to set AutoProtect state. Error: %s"), exceptionInfo.GetDescription());
        return E_FAIL;
    }

    return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CAppManager::OpenAvProductOptions()
{
    LaunchElement(ISShared::CLSID_NAV_Options);

    return S_OK;
}

//****************************************************************************
//****************************************************************************
DWORD CAppManager::GetAutoProtectState()
{
    DWORD dwRet = AutoProtectState_Unknown;

    if ( m_spAutoProtectStateElement )
    {
        StahlSoft::HRX hrx;
        ccLib::CExceptionInfo exceptionInfo;
        try
        {
            ui::IDataPtr spData;
            hrx << m_spAutoProtectStateElement->GetData(spData);
            
            ui::INumberDataQIPtr spNumber = spData;
            if(!spNumber)
                hrx << E_UNEXPECTED;

            LONGLONG qdwNumber = NULL;
            hrx << spNumber->GetNumber(qdwNumber);
            
            if(ISShared::i64StateError == qdwNumber)
                dwRet = AutoProtectState_Unknown;
            else if(ISShared::i64StateOn == qdwNumber)
                dwRet = AutoProtectState_Enabled;
            else if(ISShared::i64StateOff == qdwNumber)
                dwRet = AutoProtectState_Disabled;

        }
        CCCATCHMEM(exceptionInfo)
        CCCATCHCOM(exceptionInfo);

        if(exceptionInfo.IsException())
        {
            CCTRCTXE1(_T("Unable to initialize. Error: %s"), exceptionInfo.GetDescription());
            return E_FAIL;
        }
    }

    return dwRet;
}

//****************************************************************************
//****************************************************************************
bool CAppManager::IsAutoProtectConfigurable()
{
    bool bRet = FALSE;

    if ( m_spAutoProtectStateElement )
        m_spAutoProtectStateElement->GetConfigurable(bRet);

    return bRet;
}

//****************************************************************************
//****************************************************************************
bool CAppManager::IsProductOptionsConfigurable()
{
    bool bRet = FALSE;

    if ( m_spProductOptions)
        m_spProductOptions->GetConfigurable(bRet);

    return bRet;
}

//****************************************************************************
//****************************************************************************
bool CAppManager::ShouldShowDebugMenu()
{
    return DBGOPTS_ShowDebugMenu_TRUE == m_dwDebugMenuState;
}

//****************************************************************************
//****************************************************************************
bool CAppManager::ShouldShowUI()
{
    bool bRet = false;

    StahlSoft::HRX hrx;
    ccLib::CExceptionInfo exceptionInfo;
    try
    {
        ui::IElementPtr spShowUIElement;
        hrx << m_spISElementProvider->GetElement(ISShared::CLSID_NIS_CanShowUI, spShowUIElement);

        ui::IDataPtr spData;
        hrx << spShowUIElement->GetData(spData);

        ui::INumberDataQIPtr spNumber = spData;
        if(!spNumber)
            hrx << E_UNEXPECTED;

        LONGLONG qdwNumber = NULL;
        hrx << spNumber->GetNumber(qdwNumber);

        if(ISShared::i64StateError == qdwNumber)
            bRet = false;
        else if(ISShared::i64StateOn == qdwNumber)
            bRet = true;
        else if(ISShared::i64StateOff == qdwNumber)
            bRet = false;

    }
    CCCATCHMEM(exceptionInfo)
    CCCATCHCOM(exceptionInfo);

    if(exceptionInfo.IsException())
    {
        CCTRCTXE1(_T("Unable to determine if UI should be shown. Error: %s"), exceptionInfo.GetDescription());
    }

    return bRet;
}

//****************************************************************************
//****************************************************************************
LPCTSTR CAppManager::GetProductName()
{
    if(m_cszProductName.IsEmpty())
    {
        m_cszProductName = CISVersion::GetProductName();
    }

    return (LPCTSTR)m_cszProductName;
}

//****************************************************************************
//****************************************************************************
bool CAppManager::IsAdministratorAccount()
{
    return true;
}

bool CAppManager::IsGuestAccount()
{
    return false;
}

//****************************************************************************
//****************************************************************************
void CAppManager::SetShutdownRequest(HANDLE hShutdownRequest)
{
    m_hShutdownRequest = hShutdownRequest;
    return;
}

//****************************************************************************
//****************************************************************************
void CAppManager::RequestExit(DWORD dwExitCode)
{
    m_dwExitCode = dwExitCode;
    SetEvent(m_hShutdownRequest);
}

//****************************************************************************
//****************************************************************************
void CAppManager::OnSessionAppServerExit(DWORD dwExitCode)
{
    // Make sure we don't anything re-entrant, since this
    //  is coming from the SessionAppServer instance.
    RequestExit(dwExitCode);
}

//****************************************************************************
//****************************************************************************
DWORD CAppManager::GetExitCode()
{
    return m_dwExitCode;
}

//****************************************************************************
//****************************************************************************
void CAppManager::OnElementUpdate(ui::IUpdate* pSource, LPVOID pCookie)
{
    if(pCookie == m_spAutoProtectStateElement)
        UpdateTrayAppState();
    
    return;
}

//****************************************************************************
//****************************************************************************
//****************************************************************************
//****************************************************************************
CServerSinkImpl::CServerSinkImpl(void)
{
    m_spAppManager = CAppManagerSingleton::GetAppManager();
}

CServerSinkImpl::~CServerSinkImpl(void)
{
}

//****************************************************************************
//****************************************************************************
void CServerSinkImpl::OnAppServerReady(SessionApp::ISessionAppServer* pAppServer)
{
    StahlSoft::HRX hrx;
    ccLib::CExceptionInfo exceptionInfo;
    try
    {
        m_spAppManager->UpdateTrayAppState();
    }
    CCCATCHMEM(exceptionInfo)
    CCCATCHCOM(exceptionInfo);

}

//****************************************************************************
//****************************************************************************
void CServerSinkImpl::OnTrayItemDblClick(SessionApp::ISessionAppServer* pAppServer)
{
    StahlSoft::HRX hrx;
    ccLib::CExceptionInfo exceptionInfo;
    try
    {
        m_spAppManager->OpenAvProduct();
    }
    CCCATCHMEM(exceptionInfo)
    CCCATCHCOM(exceptionInfo);

}

//****************************************************************************
//****************************************************************************
void CServerSinkImpl::OnAppServerExit(SessionApp::ISessionAppServer* pAppServer, DWORD dwExitCode)
{
    m_spAppManager->OnSessionAppServerExit(dwExitCode);
    return;
}

//****************************************************************************
//****************************************************************************
void CServerSinkImpl::OnPrepareMenu(SessionApp::ISessionAppServer* pAppServer, HMENU &hMenu)
{
    HMODULE hRes = AtlFindResourceInstance(IDR_MENU_TRAY, RT_MENU);
    if(!hRes)
    {
        CCTRCTXE1(_T("Unable to load menu: hRes=0x%08X"), hRes);
        return;
    }

    hMenu = ::LoadMenu(hRes, MAKEINTRESOURCE(IDR_MENU_TRAY));
    if(!hMenu)
    {
        CCTRCTXE1(_T("Unable to load menu: MenuId=%d"), IDR_MENU_TRAY);
        return;
    }
    
    CMenuHandle hAppMenu(hMenu);
    CMenuHandle hTrayMenu(hAppMenu.GetSubMenu(0));
    
    bool bShouldShowUI = m_spAppManager->ShouldShowUI();
    bool bShouldShowDebugMenu = m_spAppManager->ShouldShowDebugMenu();

    CString cszProductName = m_spAppManager->GetProductName();
    
    UINT nItemId = NULL;
    INT nItemTextLen = NULL;
    
    // Set up OpenAvProduct menu item
    nItemId = ID_TRAYMENU_OPENAVPRODUCT;
    hTrayMenu.SetMenuDefaultItem(ID_TRAYMENU_OPENAVPRODUCT);
    nItemTextLen = hTrayMenu.GetMenuStringLen(nItemId, MF_BYCOMMAND);
    if(nItemTextLen)
    {
        nItemTextLen = nItemTextLen + 1; // account for NULL term

        CString cszMenuItemText;
        hTrayMenu.GetMenuString(nItemId, cszMenuItemText.GetBuffer(nItemTextLen), nItemTextLen, MF_BYCOMMAND);
        cszMenuItemText.ReleaseBuffer();

        CString cszFormattedMenuItemText;
        cszFormattedMenuItemText.FormatMessage(cszMenuItemText, cszProductName);

        hTrayMenu.ModifyMenu(nItemId, MF_BYCOMMAND|MF_STRING, nItemId, (LPCTSTR)cszFormattedMenuItemText);
        
    }


    // Set up OpenAvProductOptions menu item
    nItemId = ID_TRAYMENU_OPENAVPRODUCTOPTIONS;
    nItemTextLen = hTrayMenu.GetMenuStringLen(nItemId, MF_BYCOMMAND);
    if(nItemTextLen)
    {
        nItemTextLen = nItemTextLen + 1; // account for NULL term

        CString cszMenuItemText;
        hTrayMenu.GetMenuString(nItemId, cszMenuItemText.GetBuffer(nItemTextLen), nItemTextLen, MF_BYCOMMAND);
        cszMenuItemText.ReleaseBuffer();

        CString cszFormattedMenuItemText;
        cszFormattedMenuItemText.FormatMessage(cszMenuItemText, cszProductName);

        hTrayMenu.ModifyMenu(nItemId, MF_BYCOMMAND|MF_STRING, nItemId, (LPCTSTR)cszFormattedMenuItemText);
        
        // Disable product options if user is not allowed to change them.
        if ( bShouldShowUI && m_spAppManager->IsProductOptionsConfigurable())
            hTrayMenu.EnableMenuItem(nItemId, MF_BYCOMMAND | MF_ENABLED );
        else
            hTrayMenu.RemoveMenu(nItemId, MF_BYCOMMAND);
    }

    // Set up ShowRecentHistory menu item
    if(!bShouldShowUI)
        hTrayMenu.RemoveMenu(ID_TRAYMENU_VIEWRECENTHISTORY, MF_BYCOMMAND);
    
    // Set up debug menu
    if(!bShouldShowDebugMenu)
    {
        // Remove the bottom two entries
        hTrayMenu.RemoveMenu(hTrayMenu.GetMenuItemCount() - 1, MF_BYPOSITION);
    }

    DWORD dwAutoProtectState = m_spAppManager->GetAutoProtectState();
    bool bAutoProtectIsConfigurable = m_spAppManager->IsAutoProtectConfigurable();

    // Set admin options
    hTrayMenu.EnableMenuItem(ID_TRAYMENU_ENABLEAUTOPROTECT,  MF_BYCOMMAND | MF_ENABLED );
    hTrayMenu.EnableMenuItem(ID_TRAYMENU_DISABLEAUTOPROTECT, MF_BYCOMMAND | MF_ENABLED );

    // Set up auto-protect menu items
    if(!bShouldShowUI || !bAutoProtectIsConfigurable)
    {
        hTrayMenu.RemoveMenu(ID_TRAYMENU_ENABLEAUTOPROTECT, MF_BYCOMMAND);
        hTrayMenu.RemoveMenu(ID_TRAYMENU_DISABLEAUTOPROTECT, MF_BYCOMMAND);
    }
    if(dwAutoProtectState == AvProdApp::AutoProtectState_Enabled)
    {
        hTrayMenu.RemoveMenu(ID_TRAYMENU_ENABLEAUTOPROTECT, MF_BYCOMMAND);
    }
    else if(dwAutoProtectState == AvProdApp::AutoProtectState_Disabled)
    {
        hTrayMenu.RemoveMenu(ID_TRAYMENU_DISABLEAUTOPROTECT, MF_BYCOMMAND);
    }
    else // dwAutoProtectState == AppManager::AutoProtectState_Unknown
    {
        // hTrayMenu.RemoveMenu(ID_TRAYMENU_ENABLEAUTOPROTECT, MF_BYCOMMAND);
        hTrayMenu.RemoveMenu(ID_TRAYMENU_DISABLEAUTOPROTECT, MF_BYCOMMAND);
    }

    // If the last item is a separator, remove it
    CMenuItemInfo cMenuItemInfo;
    DWORD dwItemCount = hTrayMenu.GetMenuItemCount();
    cMenuItemInfo.fMask = MIIM_TYPE;
    hTrayMenu.GetMenuItemInfo(hTrayMenu.GetMenuItemCount() - 1, MF_BYPOSITION, &cMenuItemInfo);
    if(cMenuItemInfo.fMask & MIIM_TYPE && cMenuItemInfo.fType & MF_SEPARATOR)
        hTrayMenu.RemoveMenu(hTrayMenu.GetMenuItemCount() - 1, MF_BYPOSITION);
}

//****************************************************************************
//****************************************************************************
void CServerSinkImpl::OnMenuCommand(SessionApp::ISessionAppServer* pAppServer, UINT nId)
{
    switch(nId)
    {
        case ID_TRAYMENU_ENABLEAUTOPROTECT:
            m_spAppManager->EnableAutoProtect(TRUE);
            break;

        case ID_TRAYMENU_DISABLEAUTOPROTECT:
            m_spAppManager->EnableAutoProtect(FALSE);
            break;
        
        case ID_TRAYMENU_OPENAVPRODUCT:
            m_spAppManager->OpenAvProduct();
            break;
            
        case ID_TRAYMENU_OPENAVPRODUCTOPTIONS:
            m_spAppManager->OpenAvProductOptions();
            break;

        case ID_TRAYMENU_VIEWRECENTHISTORY:
            m_spAppManager->ViewRecentHistory();
            break;

        case ID_ADDITIONALOPTIONS_CLOSE:
            OnMenuCommand_Close();
            break;
        
        case ID_ADDITIONALOPTIONS_SETNORMALSTATE:
            OnMenuCommand_SetTrayIconState(SessionApp::AppState::eNormal, NULL, NULL);
            break;

        case ID_ADDITIONALOPTIONS_SETABNORMALSTATE:
            OnMenuCommand_SetTrayIconState(SessionApp::AppState::eAbnormal, NULL, NULL);
            break;
        
        case ID_ADDITIONALOPTIONS_SETUNKNOWNSTATE:
            OnMenuCommand_SetTrayIconState(SessionApp::AppState::eUnknown, NULL, NULL);
            break;

        case ID_ADDITIONALOPTIONS_SETWORKINGSTATE:
            OnMenuCommand_SetTrayIconState(SessionApp::NotifyState::eWorking, 10000, NULL);
            break;

        case ID_ADDITIONALOPTIONS_SETINFORMATIONSTATE:
            OnMenuCommand_SetTrayIconState(SessionApp::NotifyState::eInformation, 10000, NULL);
            break;
        
        case ID_ADDITIONALOPTIONS_SETWARNINGSTATE:
            OnMenuCommand_SetTrayIconState(SessionApp::NotifyState::eWarning, 10000, NULL);
            break;
        
        default:
            CCTRCTXW1(_T("Unhandled Menu Command: nId=%d"), nId);
            break;
    }

    return;
}

//****************************************************************************
//****************************************************************************
void CServerSinkImpl::OnCommand(SessionApp::ISessionAppServer* pAppServer, const SYMGUID& guidCommand, ISymBase *pCommand, ISymBase **ppReturn)
{   
    if(SymIsEqualIID(guidCommand, AvProd::CMDID_OnEmailThreatsDetected))
    {
        OnCommand_EmailThreatsDetected(pCommand, ppReturn);
    }
    else if(SymIsEqualIID(guidCommand, AvProd::CMDID_OnOEHDetection))
    {
        OnCommand_OEHDetection(pCommand, ppReturn);
    }
    else if(SymIsEqualIID(guidCommand, AvProd::CMDID_OnRebootRequired))
    {
        OnCommand_RebootRequired(pCommand, ppReturn);
    }
    else if(SymIsEqualIID(guidCommand, AvProd::CMDID_OnProcTermRequired))
    {
        OnCommand_ProcTermRequired(pCommand, ppReturn);
    }
    else if(SymIsEqualIID(guidCommand, AvProd::CMDID_OnAskUserForThreatRemediation))
    {
        OnCommand_AskUserForThreatRemediation(pCommand, ppReturn);
    }
    else if(SymIsEqualIID(guidCommand, AvProd::CMDID_OnLaunchManualScanner))
    {
        OnCommand_LaunchManualScanner(pCommand, ppReturn);
    }
}

//****************************************************************************
//****************************************************************************
void CServerSinkImpl::ShowOEHAlert(AVModule::IAVMapStrData* pOEHInfo, DWORD& dwOEHAction)
{   
    StahlSoft::HRX hrx;
    ccLib::CExceptionInfo exceptionInfo;
    try
    {
        HRESULT hr = S_OK;

        // Pessimistic, err on security
        dwOEHAction = AVModule::EmailScanner::OEHACTION_Quarantine;

        cc::IAlert2Ptr spOEHAlert;
        cc::ccAlertMgd_IAlert2::CreateObject(GETMODULEMGR(), spOEHAlert);
        if(!spOEHAlert)
            hrx << E_UNEXPECTED;
        
        // Apply SymTheme. Do not bail out if failed.
        CISSymTheme isSymTheme;
        hr = isSymTheme.Initialize(GetModuleHandle(NULL));
        if(FAILED(hr))
        {
            CCTRCTXE1(L"CISSymTheme::Initialize() failed. Error: 0x%08X", hr);
        }
        
        // Get details from OEH info
        CString cszSubject, cszSender, cszRecipient, cszWormPath;
        cc::IStringPtr spTempString;
        
        pOEHInfo->GetValue(AVModule::EmailScanner::AV_EMAILOEH_SUBJECT, spTempString);
        cszSubject = CW2T(spTempString->GetStringW());
        spTempString.Release();

        pOEHInfo->GetValue(AVModule::EmailScanner::AV_EMAILOEH_SENDER, spTempString);
        cszSender = CW2T(spTempString->GetStringW());
        spTempString.Release();

        pOEHInfo->GetValue(AVModule::EmailScanner::AV_EMAILOEH_RECIPIENT, spTempString);
        cszRecipient = CW2T(spTempString->GetStringW());
        spTempString.Release();

        pOEHInfo->GetValue(AVModule::EmailScanner::AV_EMAILOEH_WORMPATH, spTempString);
        cszWormPath = CW2T(spTempString->GetStringW());
        spTempString.Release();
        
        CString cszAlertTitle;
        cszAlertTitle.FormatMessage(_S(IDS_OEHALERT_TITLE), _S(IDS_OEH_FEATURE_NAME));
        
        CString cszBriefDesc;
        cszBriefDesc = _S(IDS_OEHALERT_BRIEF_DESC);

        // Add Alert Title
        spOEHAlert->SetWindowTitle(_S(IDS_OEH_FEATURE_NAME));
        spOEHAlert->SetTitleBitmap(_Module.GetResourceInstance(), IDB_OEHALERT_TITLE_RED);
        spOEHAlert->SetAlertTitle(cszAlertTitle);
        spOEHAlert->SetBriefDesc(cszBriefDesc);

        // Add alert details
        spOEHAlert->AddTableRow(_S(IDS_OEHALERT_SUBJECT), cszSubject);
        spOEHAlert->AddTableRow(_S(IDS_OEHALERT_SENDER), cszSender);
        spOEHAlert->AddTableRow(_S(IDS_OEHALERT_RECIPIENT), cszRecipient);
        spOEHAlert->AddTableRowPath(_S(IDS_OEHALERT_WORMPATH), cszWormPath);
        spOEHAlert->SetPromptText(_S(IDS_OEHALERT_PROMPT_TEXT));

        // Add Action
        spOEHAlert->AddAction(_S(IDS_OEHALERT_ACTION_QUARANTINE));
        spOEHAlert->AddAction(_S(IDS_OEHALERT_ACTION_ABORT));
        spOEHAlert->AddAction(_S(IDS_OEHALERT_ACTION_IGNORE));
        spOEHAlert->AddAction(_S(IDS_OEHALERT_ACTION_AUTHORIZE));
        
        spOEHAlert->SetRecAction(0);

        UINT iRet = spOEHAlert->DisplayAlert();

        if(0 == iRet)
            dwOEHAction = AVModule::EmailScanner::OEHACTION_Quarantine;
        else if(1 == iRet)
            dwOEHAction = AVModule::EmailScanner::OEHACTION_Abort;
        else if(2 == iRet)
            dwOEHAction = AVModule::EmailScanner::OEHACTION_Ignore;
        else if(3 == iRet)
            dwOEHAction = AVModule::EmailScanner::OEHACTION_Authorize;

    }
    CCCATCHMEM(exceptionInfo)
    CCCATCHCOM(exceptionInfo);


    return;
    
}

//****************************************************************************
//****************************************************************************
void CServerSinkImpl::OnCommand_OEHDetection(ISymBase *pCommand, ISymBase **ppReturn)
{   
    for(;;)
    {
        cc::IKeyValueCollectionQIPtr spCommand = pCommand;
        if(!spCommand)
        {
            CCTRACEE( CCTRCTX _T("Unable to get key/value object"));
            break;
        }

        ISymBaseQIPtr spTemp;
        spCommand->GetValue(AvProd::OnOEHDetection::eOEHInfo, spTemp);

        AVModule::IAVMapStrDataQIPtr spOEHInfo = spTemp;
        if(!spOEHInfo)
        {
            CCTRACEE( CCTRCTX _T("Unable to get OEH Info from key/value object."));
            break;
        }
        
        // Free up temp var
        spTemp.Release();

        // Query for the OEH Action value
        DWORD dwOEHAction = NULL;
        if(!spCommand->GetValue(AvProd::OnOEHDetection::eOEHAction, dwOEHAction))
        {
            CCTRACEE( CCTRCTX _T("Unable to get OEH Action from key/value object."));
            break;
        }

        ShowOEHAlert(spOEHInfo, dwOEHAction);

        spTemp.Attach(ccLib::CInstanceFactory::CreateKeyValueCollectionImpl());
        cc::IKeyValueCollectionQIPtr spReturnData = spTemp;
        if(!spReturnData)
        {
            CCTRACEE( CCTRCTX _T("Unable to create return data object."));
            break;
        }
        
        // Free up temp var
        spTemp.Release();

        spReturnData->SetValue(AvProd::OnOEHDetection::eOEHAction, dwOEHAction);
        spReturnData->QueryInterface(IID_SymBase, (void**)ppReturn);
        CCTRACEI( CCTRCTX _T("OnEmailThreatsDetect commanded handled successfully."));
        break;
    }
}

//****************************************************************************
//****************************************************************************
void CServerSinkImpl::OnCommand_EmailThreatsDetected(ISymBase *pCommand, ISymBase **ppReturn)
{   
    for(;;)
    {
        HRESULT hr = S_OK;

        cc::IKeyValueCollectionQIPtr spCommand = pCommand;
        if(!spCommand)
        {
            CCTRACEE( CCTRCTX _T("Unable to get key/value object"));
            break;
        }

        ISymBaseQIPtr spTemp;
        spCommand->GetValue(AvProd::OnEmailThreatsDetected::eEmailInfo, spTemp);

        AVModule::IAVMapStrDataQIPtr spEmailInfo = spTemp;
        if(!spEmailInfo)
        {
            CCTRACEE( CCTRCTX _T("Unable to get Email Info from key/value object."));
            return;
        }

        spTemp.Release();
        spCommand->GetValue(AvProd::OnEmailThreatsDetected::eReturnData, spTemp);

        AVModule::IAVMapStrDataQIPtr spReturnData = spTemp;
        if(!spReturnData)
        {
            CCTRACEE( CCTRCTX _T("Unable to get Return Data from key/value object."));
            return;
        }

        spTemp.Release();

        // Apply SymTheme. Do not bail out if failed.
        CISSymTheme isSymTheme;
        hr = isSymTheme.Initialize(GetModuleHandle(NULL));
        if(FAILED(hr))
        {
            CCTRCTXE1(L"CISSymTheme::Initialize() failed. Error: 0x%08X", hr);
        }

        avScanUI::sui_IEmailScanUI ScanUILoader;
        avScanUI::IEmailScanUIPtr spEmailScanUI;
        SYMRESULT sr = ScanUILoader.CreateObject(spEmailScanUI);
        if(!spEmailScanUI)
        {
            CCTRACEE( CCTRCTX _T("Unable to create AvScanUI object."));
            break;
        }

        if(FAILED(spEmailScanUI->DisplayThreatUI(spEmailInfo, spReturnData)))
        {
            CCTRACEE( CCTRCTX _T("IEmailScanUI::DisplayThreatUI() returned false."));
            break;
        }

        SessionApp::CSymObject2BIN<&AVModule::IID_AVMapStrData, AVModule::IAVMapStrData> AVMapStrData2BIN;
        AVMapStrData2BIN.SetSymObject(spReturnData);

        ISymBaseQIPtr spMemReturn = AVMapStrData2BIN.GetStreamObj();
        spMemReturn->QueryInterface(IID_SymBase, (void**)ppReturn);
        CCTRACEI( CCTRCTX _T("OnEmailThreatsDetect commanded handled successfully."));
        break;
    }
    
    return;
}

DWORD QueryUserForReboot(AVModule::IAVMapDwordData* pThreatInfo)
{
    DWORD dwAction = 0;

    StahlSoft::HRX hrx;
    ccLib::CExceptionInfo exceptionInfo;
    try
    {
        HRESULT hr = S_OK;

        cc::IAlert2Ptr spRebootAlert;
        cc::ccAlertMgd_IAlert2::CreateObject(GETMODULEMGR(), spRebootAlert);
        if(!spRebootAlert)
            hrx << E_UNEXPECTED;

        // Apply SymTheme. Do not bail out if failed.
        CISSymTheme isSymTheme;
        hr = isSymTheme.Initialize(GetModuleHandle(NULL));
        if(FAILED(hr))
        {
            CCTRCTXE1(L"CISSymTheme::Initialize() failed. Error: 0x%08X", hr);
        }

        // Add Alert Title
        spRebootAlert->SetWindowTitle(CISVersion::GetProductName());
        spRebootAlert->SetTitleBitmap(_Module.GetResourceInstance(), IDB_OEHALERT_TITLE_RED);
        spRebootAlert->SetAlertTitle(_S(IDS_REBOOTREQUEST_TITLE));
        spRebootAlert->SetBriefDesc(_S(IDS_REBOOTREQUEST_BRIEF_DESC));
        spRebootAlert->SetPromptText(_S(IDS_REBOOTREQUEST_PROMPT_TEXT));

        // Add Action
        spRebootAlert->AddAction(_S(IDS_REBOOTREQUEST_REBOOT_NOW));
        spRebootAlert->AddAction(_S(IDS_REBOOTREQUEST_REBOOT_LATER));

        spRebootAlert->SetRecAction(0);

        UINT iRet = spRebootAlert->DisplayAlert();

        if(0 == iRet)
            dwAction = 1;
        else if(1 == iRet)
            dwAction = 2;

    }
    CCCATCHMEM(exceptionInfo)
    CCCATCHCOM(exceptionInfo);


    return dwAction;
}


//****************************************************************************
//****************************************************************************
void CServerSinkImpl::OnCommand_RebootRequired(ISymBase *pCommand, ISymBase **ppReturn)
{   
    for(;;)
    {
        // Query for interesting data
        cc::IKeyValueCollectionQIPtr spCommand = pCommand;
        if(!spCommand)
        {
            CCTRACEE( CCTRCTX _T("Unable to get key/value object"));
            break;
        }

        ISymBaseQIPtr spTemp;
        spCommand->GetValue(AvProd::OnRebootRequired::eThreatInfo, spTemp);

        AVModule::IAVMapDwordDataQIPtr spThreatInfo = spTemp;
        if(!spThreatInfo)
        {
            CCTRACEE( CCTRCTX _T("Unable to get Threat Info from key/value object."));
        }

        // Free up temp var
        spTemp.Release();

        // Do some work
        DWORD dwAction = 1; // arbitrary        
        dwAction = QueryUserForReboot(spThreatInfo);

        // Prepare to return data
        spTemp.Attach(ccLib::CInstanceFactory::CreateKeyValueCollectionImpl());
        cc::IKeyValueCollectionQIPtr spReturnData = spTemp;
        if(!spReturnData)
        {
            CCTRACEE( CCTRCTX _T("Unable to create return data object."));
            break;
        }

        // Free up temp var
        spTemp.Release();

        spReturnData->SetValue(AvProd::OnRebootRequired::eAction, dwAction);
        spReturnData->QueryInterface(IID_SymBase, (void**)ppReturn);
        CCTRACEI( CCTRCTX _T("OnRebootRequired commanded handled successfully."));
        break;
    };
};

DWORD QueryUserForProcTerm(AVModule::IAVMapDwordData* pThreatInfo)
{
    DWORD dwAction = 0;

    StahlSoft::HRX hrx;
    ccLib::CExceptionInfo exceptionInfo;
    try
    {
        cc::INotify2Ptr spProcTermNotify;
        cc::ccAlertMgd_INotify2::CreateObject(GETMODULEMGR(), spProcTermNotify);
        if(!spProcTermNotify)
            hrx << E_UNEXPECTED;
        
        // Setup notify callback
        cc::INotifyCallbackPtr spNotifyCallback;
        CNotifySink_ViewRecentHistory::CreateObject(spNotifyCallback);
        if(!spNotifyCallback)
            hrx << E_UNEXPECTED;
        
        long lHeight = _I(IDS_REBOOTREQUEST_HEIGHT);
        long lWidth  = _I(IDS_REBOOTREQUEST_WIDTH);

        // Add Alert Title
        spProcTermNotify->SetTitle(_S(IDS_PROCTERM_TITLE));
        spProcTermNotify->SetText(_S(IDS_PROCTERM_PROMPT_TEXT));

        // Set properties
        spProcTermNotify->SetProperty(cc::INotify::PROPERTY_OK_BUTTON, true);
        spProcTermNotify->SetProperty(cc::INotify::PROPERTY_CANCEL_BUTTON, true);
        spProcTermNotify->SetProperty(cc::INotify::PROPERTY_TOPMOST, true);

        // Add Action
        spProcTermNotify->SetButton(cc::INotify::BUTTONTYPE_OK, _S(IDS_PROCTERM_OK_BUTTON_TEXT));
        spProcTermNotify->SetButton(cc::INotify::BUTTONTYPE_CANCEL, _S(IDS_PROCTERM_CANCEL_BUTTON_TEXT));
        spProcTermNotify->AddCheckbox(_S(IDS_PROCTERM_DONTASKAGAIN_TEXT));

        // Set timeout
        spProcTermNotify->SetTimeout(30000); // 30 seconds
        
        // Set dimensions
        spProcTermNotify->SetSize(lWidth, lHeight);

        UINT iRet = spProcTermNotify->Display(GetDesktopWindow());

        if(cc::INotify::RESULT_CANCEL == iRet)
            dwAction = 0x0;
        else if(cc::INotify::RESULT_OK == iRet || cc::INotify::RESULT_TIMEOUT == iRet)
            dwAction = 0x01;
        
        if(spProcTermNotify->GetCheckboxState())
        {
            dwAction = dwAction | 0x10;
        }
    }
    CCCATCHMEM(exceptionInfo)
    CCCATCHCOM(exceptionInfo);

    return dwAction;
}

//****************************************************************************
//****************************************************************************
void CServerSinkImpl::OnCommand_ProcTermRequired(ISymBase *pCommand, ISymBase **ppReturn)
{   
    for(;;)
    {
        // Query for interesting data
        cc::IKeyValueCollectionQIPtr spCommand = pCommand;
        if(!spCommand)
        {
            CCTRACEE( CCTRCTX _T("Unable to get key/value object"));
            break;
        }

        ISymBaseQIPtr spTemp;
        spCommand->GetValue(AvProd::OnProcTermRequired::eThreatInfo, spTemp);

        AVModule::IAVMapDwordDataQIPtr spThreatInfo = spTemp;
        if(!spThreatInfo)
        {
            CCTRACEE( CCTRCTX _T("Unable to get Threat Info from key/value object."));
            break;
        }

        // Free up temp var
        spTemp.Release();

        // Do some work
        DWORD dwAction = 1; // arbitrary        
        dwAction = QueryUserForProcTerm(spThreatInfo);

        // Prepare to return data
        spTemp.Attach(ccLib::CInstanceFactory::CreateKeyValueCollectionImpl());
        cc::IKeyValueCollectionQIPtr spReturnData = spTemp;
        if(!spReturnData)
        {
            CCTRACEE( CCTRCTX _T("Unable to create return data object."));
            break;
        }

        // Free up temp var
        spTemp.Release();

        spReturnData->SetValue(AvProd::OnProcTermRequired::eAction, dwAction);
        spReturnData->QueryInterface(IID_SymBase, (void**)ppReturn);
        CCTRACEI( CCTRCTX _T("OnProcTermRequired commanded handled successfully."));
        break;
    };
};

//****************************************************************************
//****************************************************************************
void CServerSinkImpl::OnCommand_AskUserForThreatRemediation(ISymBase *pCommand, ISymBase **ppReturn)
{   
    for(;;)
    {
        cc::IKeyValueCollectionQIPtr spCommand = pCommand;
        if(!spCommand)
        {
            CCTRACEE( CCTRCTX _T("Unable to get key/value object"));
            break;
        }

        ISymBaseQIPtr spTemp;
        spCommand->GetValue(AvProd::OnAskUserForThreatRemediation::eThreatInfo, spTemp);

        AVModule::IAVMapDwordDataQIPtr spThreatInfo = spTemp;
        if(!spThreatInfo)
        {
            CCTRACEE( CCTRCTX _T("Unable to get ThreatTrack Info from key/value object."));
            break;
        }

        // Free up temp var
        spTemp.Release();

        // Query for the Action value
        DWORD dwAction = NULL;
        if(!spCommand->GetValue(AvProd::OnAskUserForThreatRemediation::eAction, dwAction))
        {
            CCTRACEE( CCTRCTX _T("Unable to get Remediation Action from key/value object."));
            break;
        }

        ShowThreatRemediationAlert(spThreatInfo, dwAction);

        spTemp.Attach(ccLib::CInstanceFactory::CreateKeyValueCollectionImpl());
        cc::IKeyValueCollectionQIPtr spReturnData = spTemp;
        if(!spReturnData)
        {
            CCTRACEE( CCTRCTX _T("Unable to create return data object."));
            break;
        }

        // Free up temp var
        spTemp.Release();

        spReturnData->SetValue(AvProd::OnAskUserForThreatRemediation::eAction, dwAction);
        spReturnData->QueryInterface(IID_SymBase, (void**)ppReturn);
        CCTRACEI( CCTRCTX _T("Command handled successfully."));
        break;
    }
}

//****************************************************************************
//****************************************************************************
void CServerSinkImpl::ShowThreatRemediationAlert(AVModule::IAVMapDwordData* pThreatTrackInfo, DWORD& dwAction)
{   
    StahlSoft::HRX hrx;
    ccLib::CExceptionInfo exceptionInfo;
    try
    {
        cc::IAlert2Ptr spThreatRemediationAlert;
        cc::ccAlertMgd_IAlert2::CreateObject(GETMODULEMGR(), spThreatRemediationAlert);
        if(!spThreatRemediationAlert)
            hrx << E_UNEXPECTED;

        HRESULT hr;

        // Apply SymTheme. Do not bail out if failed.
        CISSymTheme isSymTheme;
        hr = isSymTheme.Initialize(GetModuleHandle(NULL));
        if(FAILED(hr))
        {
            CCTRCTXE1(L"CISSymTheme::Initialize() failed. Error: 0x%08X", hr);
        }

        // Get details from OEH info
        CString cszRiskName, cszRiskCategory, cszThreatMatrix;
        DWORD dwRiskMatrixLevel = NULL;
        cc::IStringPtr spTempString;

        pThreatTrackInfo->GetValue(AVModule::ThreatTracking::TrackingData_NAME, spTempString);
        cszRiskName = CW2T(spTempString->GetStringW());
        spTempString.Release();

        hr = pThreatTrackInfo->GetValue(AVModule::ThreatTracking::TrackingData_OVERALL_THREAT_LEVEL, dwRiskMatrixLevel);
        if(SUCCEEDED(hr))
        {
            cszThreatMatrix.Format(_T("%d"), dwRiskMatrixLevel);
        }

        { // scoped
            // Get Categories string
            AvModuleLogging::IThreatDataEzPtr spThreatDataEz;
            SYMRESULT sr = AvModuleLogging::ThreatDataEz_Loader::CreateObject(GETMODULEMGR(), &spThreatDataEz);
            if(SYM_SUCCEEDED(sr) && spThreatDataEz)
            {
                cc::IStringPtr spCategoriesString;
                hr = spThreatDataEz->GetCategoriesString(pThreatTrackInfo, spCategoriesString);
                if(SUCCEEDED(hr) && spCategoriesString)
                {
                    cszRiskCategory = CW2T(spCategoriesString->GetStringW());
                }
                
            }
            
        }
        
        CString cszWindowTitle = _T("--> <--"); // intentionally hard coded
        if(CAppManagerSingleton::GetAppManager())
        {
            cszWindowTitle = CAppManagerSingleton::GetAppManager()->GetProductName();
        }

        // Add Alert Title
        spThreatRemediationAlert->SetWindowTitle(cszWindowTitle);
        spThreatRemediationAlert->SetTitleBitmap(_Module.GetResourceInstance(), IDB_OEHALERT_TITLE_YELLOW);
        spThreatRemediationAlert->SetAlertTitle(_S(IDS_RISKALERT_TITLE));
        spThreatRemediationAlert->SetBriefDesc(_S(IDS_RISKALERT_BRIEF_DESC));

        // Add alert details
        spThreatRemediationAlert->AddTableRow(_S(IDS_RISKALERT_RISK_NAME), cszRiskName);
        spThreatRemediationAlert->AddTableRow(_S(IDS_RISKALERT_RISK_CATEGORY), cszRiskCategory);
        spThreatRemediationAlert->SetPromptText(_S(IDS_RISKALERT_PROMPT_TEXT));

        // Add Action
        spThreatRemediationAlert->AddAction(_S(IDS_RISKALERT_ACTION_REMOVE));
        spThreatRemediationAlert->AddAction(_S(IDS_RISKALERT_ACTION_IGNORE_ONCE));
        spThreatRemediationAlert->AddAction(_S(IDS_RISKALERT_ACTION_IGNORE_ALWAYS));

        spThreatRemediationAlert->SetRecAction(0);

        UINT iRet = spThreatRemediationAlert->DisplayAlert();

        if(0 == iRet)
            dwAction = AvProd::OnAskUserForThreatRemediation::eRemediate;
        else if(1 == iRet)
            dwAction = AvProd::OnAskUserForThreatRemediation::eIgnoreOnce;
        else if(2 == iRet)
            dwAction = AvProd::OnAskUserForThreatRemediation::eIgnoreAlways;

    }
    CCCATCHMEM(exceptionInfo)
    CCCATCHCOM(exceptionInfo);


    return;

}

//****************************************************************************
//****************************************************************************
void CServerSinkImpl::OnCommand_LaunchManualScanner(ISymBase *pCommand, ISymBase **ppReturn)
{   
    for(;;)
    {
        cc::IKeyValueCollectionQIPtr spCommand = pCommand;
        if(!spCommand)
        {
            CCTRACEE( CCTRCTX _T("Unable to get key/value object"));
            break;
        }

        ISymBaseQIPtr spTemp;
        spCommand->GetValue(AvProd::OnLaunchManualScanner::eAdditionalParameters, spTemp);

        cc::IStringQIPtr spAdditionalParameters = spTemp;
        if(!spAdditionalParameters)
        {
            CCTRACEE( CCTRCTX _T("Unable to get Additional Parameters from key/value object."));
            break; // command line is require
        }

        // Free up temp var
        spTemp.Release();

        TCHAR szNAVW32[] = _T("NAVW32.exe");
        CString cszScannerCmdLine;
        STARTUPINFO rSI;
        PROCESS_INFORMATION rPI;

        ZeroMemory(&rSI, sizeof(rSI));
        rSI.cb = sizeof(rSI);
        ZeroMemory(&rPI, sizeof(rPI));

        CNAVInfo navInfo;
        cszScannerCmdLine = navInfo.GetNAVDir();

        PathAppend(cszScannerCmdLine.GetBuffer(cszScannerCmdLine.GetLength() + _countof(szNAVW32) + 1), szNAVW32);
        cszScannerCmdLine.ReleaseBuffer();

        PathQuoteSpaces(cszScannerCmdLine.GetBuffer(cszScannerCmdLine.GetLength() + 3));
        cszScannerCmdLine.ReleaseBuffer();

        cszScannerCmdLine += _T(" ");
        cszScannerCmdLine += CW2T(spAdditionalParameters->GetStringW());

        CCTRACEI( CCTRCTX _T("Launching Manual Scanner: %s"), cszScannerCmdLine);
        BOOL bRet = CreateProcess(NULL, (LPWSTR)(LPCWSTR)cszScannerCmdLine, NULL, NULL, FALSE, 
            CREATE_NEW_PROCESS_GROUP | IDLE_PRIORITY_CLASS, 
            NULL, NULL, &rSI, &rPI);

        if (bRet)
        {
            CloseHandle(rPI.hProcess);
            CloseHandle(rPI.hThread);
        }
        
        break;
    }
}

//****************************************************************************
//****************************************************************************
void CServerSinkImpl::Update(const ccSettings::CSettingsChangeEventEx *pEvent,
                    ccSettings::ISettings *pSettings) throw()
{
    CString cszKeyName;
    DWORD dwKeyNameSize = 512;
    pEvent->GetKey(cszKeyName.GetBuffer(dwKeyNameSize), dwKeyNameSize);
    cszKeyName.ReleaseBuffer();

    if(!cszKeyName.IsEmpty())
        m_spAppManager->OnSettingsChanged(cszKeyName, pSettings);
}

//****************************************************************************
//****************************************************************************
void CServerSinkImpl::OnMenuCommand_Close()
{
    m_spAppManager->RequestExit(1);
    return;
}

//****************************************************************************
//****************************************************************************
void CServerSinkImpl::OnMenuCommand_SetTrayIconState(DWORD dwState, DWORD dwDuration, LPCWSTR wszToolTipText)
{
    StahlSoft::HRX hrx;
    ccLib::CExceptionInfo exceptionInfo;
    try
    {
        SessionApp::ISessionAppClientPtr spClient;
        hrx << SessionApp::CClientImpl::CreateObject(spClient);
        hrx << spClient->Initialize(L"AvProdSession");

        if(dwState > SessionApp::AppState::eFirst && dwState < SessionApp::AppState::eLast)
            hrx << spClient->SetCurrentAppState(dwState);

        if(dwState > SessionApp::NotifyState::eFirst && dwState < SessionApp::NotifyState::eLast)
            hrx << spClient->SetCurrentNotifyState(dwState, dwDuration, NULL);

    }
    CCCATCHMEM(exceptionInfo)
    CCCATCHCOM(exceptionInfo);


    return;
}

//****************************************************************************
//****************************************************************************
HRESULT CServerSinkImpl::Update(IUpdate* pSource, LPVOID pCookie) throw()
{
    m_spAppManager->OnElementUpdate(pSource, pCookie);
    return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CServerSinkImpl::Register(IUpdate* pUpdate, LPVOID pCookie) throw()
{
    return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CServerSinkImpl::Unregister(IUpdate* pUpdate) throw()
{
    return S_OK;
}
