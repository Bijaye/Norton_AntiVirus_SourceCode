#pragma once

#include "HPPAppInterface.h"


class CHPPSettingsImpl : public IHPPSettingsInterface
{
public:
    CHPPSettingsImpl(void);
    virtual ~CHPPSettingsImpl(void);

    virtual HRESULT SetFeatureState(LONG lFeatureEnabled);
    virtual HRESULT GetFeatureState(LPLONG plFeatureEnabled);

    virtual HRESULT SetRunAtStartup(LONG lStartupBehavior);
    virtual HRESULT GetRunAtStartup(LPLONG plStartupBehavior);

    virtual HRESULT SetDefaultBlockAction(LONG lBlockBehavior);
    virtual HRESULT GetDefaultBlockAction(LPLONG plBlockBehavior);

    virtual HRESULT SetAlertFrequency(LONG lAlertFrequency);
    virtual HRESULT GetAlertFrequency(LPLONG plAlertFrequency);

    virtual HRESULT SetFirstRunDialog(LONG lFirstRunDialog);
    virtual HRESULT GetFirstRunDialog(LPLONG plFirstRunDialog);

    virtual HRESULT ReadSettings();
    virtual HRESULT WriteSettings();

protected:
    LONG m_lFeatureState;
    LONG m_lStartupBehavior;
    LONG m_lBlockBehavior;
    LONG m_lAlertFrequency;
    LONG m_lFirstRunDialog;
};
