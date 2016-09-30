
#ifndef DEFALERT_PLUGIN_H
#define DEFALERT_PLUGIN_H

#include "AgentPlugin.h"
#include "GlobalEvent.h"
#include "AlertList.h"
#include "syminterface.h"
#include "ccappplugin.h"
#include "cltLicenseConstants.h"

class CDefAlertPlugin : 
        public ISymBaseImpl< CSymThreadSafeRefCount >,
	    public ICCAppPlugin
{
public:
	// Constructor.
	CDefAlertPlugin();

	// Destructor.
	~CDefAlertPlugin();

	SYM_INTERFACE_MAP_BEGIN()               
		SYM_INTERFACE_ENTRY( IID_CCAppPlugin, ICCAppPlugin )
	SYM_INTERFACE_MAP_END()                 

	// ICCAppPlugin overrides.
	virtual void Run( ICCApp* pCCApp );
	virtual bool CanStopNow();
	virtual void RequestStop();
	virtual const char * GetObjectName();
    virtual unsigned long OnMessage( int iMessage, unsigned long ulParam );

	// This function display a popup alert to let the user know that
	// AutoLiveUpdate has performed a silen update of virus definitions.
	VOID OnVirusDefsLUEvent();

	// This function checks licensing, subscription, and 
	//  virus definitions status.
	VOID OnTimeOut();

	// This function checks virus definition age.
	VOID CheckVirusDefsStatus();

	// This function checks virus subscription status.
	VOID CheckVirusSubscriptionStatus();

	// This function checks licensing status.
	HRESULT CheckLicensingStatus(CHECKLICENSE_REQUESTTYPE rqt, HWND hWndParent);

	// This function determines how often DefAlerts
	//  should perfrom status checking.
	DWORD GetRepeatTimeOut();

	// This function checks licensing status, and display
	//  a licensing alert if conditions met.
	HRESULT OnLicenseRefreshed();

	// This function checks if subscription has been renewed
	//  and if ALU is disabled, it will enable ALU.
	HRESULT OnSubscriptionChanged();

	// This function check if CfgWiz is complete.
	bool IsNAVCfgWizFinished();

	// This function display ALU complete notification.
	HRESULT ShowALUCompleteNotification();

private:
    void delayedInit ();
    bool m_bInit;

	// Event used to stop the DefAlert thread.
	StahlSoft::CSmartHandle m_shKillEvent;

	// LiveUpdate event.
	CGlobalEvent m_VirusDefsLUEvent;

	// License updated event.
	StahlSoft::CSmartHandle m_LicenseRefreshedEvent;

	// Subscription updated event.
	StahlSoft::CSmartHandle m_SubsRefreshedEvent;

	// Alert object that maintains alert queue.
	CAlertList* m_pAlertList;

	// Licensing alert message ID.
	UINT m_uLicenseAlertMsgID;

	// How often DefAlert should check status
	//  in ALERT_SECOND_TIMEOUT and ALERT_THIRD_TIMEOUT.
	DWORD m_dwRepeatTimeOut;

	// Licensing object to check licensing status and alert condition.
	SIMON::CSimonPtr<ILicensingAlert> m_spLicense;

	// Subscription object to check subscription status and alert condition.
	SIMON::CSimonPtr<ISubscriptionAlert> m_spSubscription;

	// VirusDefs object to check subscription status and alert condition.
	SIMON::CSimonPtr<ISymAlert> m_spDefinitions;

	// DefAlert window
	HWND m_hWnd;

	// DefAlert class atom.
	ATOM m_atomWndClass;

	DJSMAR_LicenseType m_LicenseType;

	DJSMAR00_LicenseState m_LicenseState;

	STATUS_SUBSCRIPTION m_SubStatus;

	BOOL m_bAggregated;
	bool m_shutdown;
	BOOL m_bCfgwizFinished;
	BOOL m_bInitTimeOut;

private:

	static unsigned __stdcall Monitor(void* pArgument); 	// Former DefAlert thread procedure.

	// Events waited on by monitor thread.
	enum
	{	
		EVENT_QUIT					= 0 - WAIT_OBJECT_0
		,EVENT_VIRUSDEFS_LU			= 1 - WAIT_OBJECT_0
		,EVENT_LICENSE_REFRESHED	= 2 - WAIT_OBJECT_0
		,EVENT_SUBSCRIPTION_REFRESHED = 3 - WAIT_OBJECT_0
		,EVENT_WINDOWS_MSG			= EVENT_SUBSCRIPTION_REFRESHED + 1
	};
};

#endif

