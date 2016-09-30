// ----------------------------------------------------------------------------
// PROPRIETARY/CONFIDENTIAL.  Use of this product is subject to license terms.
// Copyright (C) 2005 Symantec Corporation.  All rights reserved.
// ----------------------------------------------------------------------------
#pragma once

#include <nscIClientCallback.h>

// will rvw nscLib implementation
class CClientCallback : public ISymBaseImpl<CSymThreadSafeRefCount>, 
    public nsc::IClientCallback
{
public:
    SYM_INTERFACE_MAP_BEGIN()
        SYM_INTERFACE_ENTRY(nsc::IID_nscIClientCallback,nsc::IClientCallback)
    SYM_INTERFACE_MAP_END()

    // cookie enum 
    enum e_CallbackCookie
    {
        eCOOKIE_INVALID,

        eCOOKIE_ENABLE_AP,
        eCOOKIE_DISABLE_AP,
        eCOOKIE_CONFIGURE_AUTO_PROTECT,

        eCOOKIE_ENABLE_EMAIL_SCANNING,
        eCOOKIE_DISABLE_EMAIL_SCANNING,
        eCOOKIE_CONFIGURE_EMAIL_SCANNING,

        eCOOKIE_ENABLE_SPYWARE_PROTECTION,
        eCOOKIE_DISABLE_SPYWARE_PROTECTION,
        eCOOKIE_CONFIGURE_SPYWARE_PROTECTION,

        eCOOKIE_ENABLE_IFP,
        eCOOKIE_DISABLE_IFP,
        eCOOKIE_CONFIGURE_IFP,

        eCOOKIE_ENABLE_IM_SCANNING,
        eCOOKIE_DISABLE_IM_SCANNING,
        eCOOKIE_CONFIGURE_IM_SCANNING,

        eCOOKIE_ENABLE_WORM_BLOCKING,
        eCOOKIE_DISABLE_WORM_BLOCKING,
        eCOOKIE_CONFIGURE_WORM_BLOCKING,

        eCOOKIE_LAUNCH_RUN_LU_AND_WAIT,
        eCOOKIE_CONFIGURE_RUN_LU,

        eCOOKIE_LAUNCH_FSS_AND_WAIT,

        eCOOKIE_LAUNCH_QUARANTINE,

		eCOOKIE_NON_ADMIN_MESSAGE
    };

    virtual nsc::NSCRESULT Execute(HWND window_in,
							  DWORD cookie_in) 
							  const throw();

    // ctor
    CClientCallback (void) {};

private:
    // hide default methods
    CClientCallback(const CClientCallback&);
    CClientCallback& operator=(const CClientCallback&) throw();

    nsc::NSCRESULT InitNavOptions(CComPtr<INAVOptions> &spNavOptions, 
        CComPtr<ISymScriptSafe> &spSymScriptSafe,
        nsc::NSCRESULT &result) const throw();
    nsc::NSCRESULT InitScriptableAP(CComPtr<IScriptableAutoProtect> &spAP, 
        CComPtr<ISymScriptSafe> &spSymScriptSafeAP,
        nsc::NSCRESULT &result) const throw();
    nsc::NSCRESULT InitAppLauncher(CComPtr<IAppLauncher> &spAppLauncher, 
        CComPtr<ISymScriptSafe> &spSymScriptSafeAppLnch,
        nsc::NSCRESULT &result) const throw();
    nsc::NSCRESULT InitScanTasks(CComPtr<INAVScanTasks> &spTasks, 
        CComPtr<ISymScriptSafe> &spSymScriptSafeScan,
        nsc::NSCRESULT &result) const throw();

    // Helper functions for waiting for features to update
    bool WaitForChange (HANDLE hEvent, DWORD dwWaitTimeout /*ms*/) const;
    void WaitForAPChange () const;
    void WaitForEmailChange () const;
    void WaitForIWPChange () const;
};
