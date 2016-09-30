#pragma once

#include "ccEventUtil.h"

//
// Home Page Protection Session SymApp Plugin Class ID
//
// {2907D306-30DF-4234-8F9E-8060139A1392}
SYM_DEFINE_INTERFACE_ID(CLSID_HPPSessionSymAppPlugin, 
                        0x2907d306, 0x30df, 0x4234, 0x8f, 0x9e, 0x80, 0x60, 0x13, 0x9a, 0x13, 0x92);


class IHPPSessionAppInterface : public ISymBase
{
public:
    virtual HRESULT Initialize() throw() = 0;
    virtual void Destroy() throw() = 0;

    virtual HRESULT HandleEventManagerShutdown() throw() = 0;
    virtual HRESULT ResetEventManagerConnections() throw() = 0;

    virtual HRESULT PushIncomingEventToQueue(ccEvtMgr::CEventEx* pEvent) throw() = 0;
    virtual HRESULT ProcessEvent(ccEvtMgr::CEventEx* pEvent) throw() = 0;
    virtual HRESULT SwitchHomePage(ccEvtMgr::CEventEx* pEvent, LPCTSTR szNewHomePageValue, BOOL bChangeUserKey, DWORD dwThreadId = NULL) throw() = 0;
    virtual void ConfirmSysMonRules() throw() = 0;
    virtual void ShowInternetExplorerControlPanel() throw() = 0;
    virtual void NavigateToHomePageByThread(DWORD dwThreadId) throw() = 0;

};

// {E6F26122-E834-47ad-AF3D-8DE6ED5E70CA}
SYM_DEFINE_INTERFACE_ID(IID_HPPSessionAppInterface, 
                        0xe6f26122, 0xe834, 0x47ad, 0xaf, 0x3d, 0x8d, 0xe6, 0xed, 0x5e, 0x70, 0xca);

typedef CSymPtr<IHPPSessionAppInterface> IHPPSessionAppInterfacePtr;
typedef CSymQIPtr<IHPPSessionAppInterface, &IID_HPPSessionAppInterface> IHPPSessionAppInterfaceQIPtr;


class IHPPSettingsInterface : public ISymBase
{
public:
    
    virtual HRESULT SetFeatureState(LONG lFeatureState) throw() = 0;
    virtual HRESULT GetFeatureState(LPLONG plFeatureState) throw() = 0;

    virtual HRESULT SetRunAtStartup(LONG lStartupBehavior) throw() = 0;
    virtual HRESULT GetRunAtStartup(LPLONG plStartupBehavior) throw() = 0;

    virtual HRESULT SetDefaultBlockAction(LONG lBlockBehavior) throw() = 0;
    virtual HRESULT GetDefaultBlockAction(LPLONG plBlockBehavior) throw() = 0;
    
    virtual HRESULT SetAlertFrequency(LONG lAlertFrequency) throw() = 0;
    virtual HRESULT GetAlertFrequency(LPLONG plAlertFrequency) throw() = 0;

    virtual HRESULT SetFirstRunDialog(LONG lFirstRunDialog) throw() = 0;
    virtual HRESULT GetFirstRunDialog(LPLONG plFirstRunDialog) throw() = 0;

    virtual HRESULT ReadSettings() throw() = 0;
    virtual HRESULT WriteSettings() throw() = 0;

};

// {7C19E3DC-5975-4471-8B55-CCB534DC4D65}
SYM_DEFINE_INTERFACE_ID(IID_HPPSettingsInterface, 
                        0x7c19e3dc, 0x5975, 0x4471, 0x8b, 0x55, 0xcc, 0xb5, 0x34, 0xdc, 0x4d, 0x65);

typedef CSymPtr<IHPPSettingsInterface> IHPPSettingsInterfacePtr;
typedef CSymQIPtr<IHPPSettingsInterface, &IID_HPPSettingsInterface> IHPPSettingsInterfaceQIPtr;

