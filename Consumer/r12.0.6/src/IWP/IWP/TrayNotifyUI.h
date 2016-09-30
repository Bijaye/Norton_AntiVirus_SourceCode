#pragma once

#include "ccAlertInterface.h"       // Common Client IM-style alert
#include "ccAlertLoader.h"

// Make sure that you Crit Section this if it ever gets it's own thread!
//
class CTrayNotifyUI : public cc::INotifyCallback, 
                      public ISymBaseImpl<CSymThreadSafeRefCount>
{
public:
    CTrayNotifyUI(void);
    ~CTrayNotifyUI(void);

    SYM_INTERFACE_MAP_BEGIN()               
        SYM_INTERFACE_ENTRY(cc::IID_NotifyCallback, cc::INotifyCallback)
	SYM_INTERFACE_MAP_END() 

// INotifyCallback - for the More Info button
    bool Run(HWND hWndParent, unsigned long nID, cc::INotify* pAlert);

    // Show the alert
    //
    bool Start (LPCSTR lpszText, bool bMoreInfo = true, bool bTaskbar = true);

    // Did the user click "More Info"?
    bool m_bUserWantsMoreInfo;

    // Allow pass through access to the IAlert
    //
    cc::INotify2* GetAlert () { return m_pAlert; };

	static CString m_csProductName;

protected:
    cc::ccAlert_INotify2 NotifyFactory;
    cc::INotify2Ptr m_pAlert;	
};
