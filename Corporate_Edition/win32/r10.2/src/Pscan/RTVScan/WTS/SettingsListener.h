// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2006 Symantec Corporation, All rights reserved.
// settingslistner.h
#pragma once

#include <windows.h>
#include "IWtsSessionListener.h"
#include "wtssessionevent.h"
#include "guard.h"

//singleton class
class CSettingsListener: public IWtsSessionListener
{
public:
    static CSettingsListener& GetInstance();
    ~CSettingsListener(void);

    //session change notification
    virtual void SessionChange(const CWtsSessionEvent& objEvent);

protected:
    CSettingsListener(void);
    CSettingsListener(const CSettingsListener&);
    void operator=(const CSettingsListener&);

    //process logon notifications
    void ProcessLogon(const CWtsSessionEvent& objEvent);

    //functions used to create user settings
    DWORD GetUserKey(const CWtsSessionEvent& objEvent, HKEY& hUserKey);
    DWORD CopyDefaultScanOptions(HKEY hUserKey);
    bool IsCopiedDefaultScanOptions(HKEY hUserKey, PHKEY phCustomTasksKey);

private:
    static CSettingsListener g_SettingsListenerInstance;
    bool                            m_bInitialized;
    CLock                           m_objLock;
};
