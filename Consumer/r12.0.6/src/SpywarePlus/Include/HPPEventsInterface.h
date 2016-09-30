#pragma once

#include "ccEventId.h"
#include "ccSerializableEventEx.h"

namespace HPP
{
    #define CC_HPP_EVENT_ID_BASE 3200

	enum Event_IDs // these are ccEvent IDs!!!
	{
		Event_ID_Base = CC_HPP_EVENT_ID_BASE,
		
		Event_ID_HPPNotifyHomePage,                 // Registry notification event
		Event_ID_IE_Started,                        // IE Started notification event
        Event_ID_LogEntry_HomePageActivity,         // IE Home Page Activity
        Event_ID_LogEntry_AppActivity,              // Home Page Protection Application Activity
		Event_ID_HPPChangeHomePage,
		Event_ID_OptionsChanged,                     // Home Page Protection Options have changed

		Event_ID_LAST                               // ALWAYS THE LAST EVENT - so we know where to stop
	};

    enum Event_Base_properties                      // S = string, L = long
	{
		Event_Base_propType = 0,
		
		Event_Base_propUserName,                    // S - Added by HPPEvents when the event is broadcast
		Event_Base_propUserSID,                     // S - Added by HPPEvents when the event is broadcast
		Event_Base_propSessionID,                   // L - Session ID that generated the event
		Event_Base_propVersion,                     // L - Event version, different for each type
		
		Event_Base_propLastProperty = 100           // So any derived classes know where to start (for expansion)
	};

    enum Event_HPPNotifyHomePage_properties         // S = string, L = long 
	{
		Event_HPPNotifyHomePage_propBase = Event_Base_propLastProperty,
		
		Event_HPPNotifyHomePage_propUserName,       // S - User name associated with the 
													//     process that originated the 
													//     action
		Event_HPPNotifyHomePage_propUserSID,        // S - SID associated with the process
													//     that originated the action
		Event_HPPNotifyHomePage_propSessionID,      // L - Session ID associated with the 
													//     process that originated the 
													//     action
		Event_HPPNotifyHomePage_propProcessID,      // L - Process ID that wrote the 
													//     file (optional)
		Event_HPPNotifyHomePage_propProcessName,    // S - Process Name that originated 
													//     the action
		Event_HPPNotifyHomePage_propProcessPath,    // S - Full Path of the process that
													//     originated the action
		Event_HPPNotifyHomePage_propModuleName,     // S - Module Name that originated the 
													//     action, if available
		Event_HPPNotifyHomePage_propModulePath,     // S - Module Path that originated the 
													//     action, if available
		Event_HPPNotifyHomePage_propStartValue,     // S - Original home page value
		Event_HPPNotifyHomePage_propActionValue,    // S - Original home page value
		Event_HPPNotifyHomePage_propActionResult,   // L - 1 - blocked, 2 - allowed
        Event_HPPNotifyHomePage_propActionLocation, // L - 1 - HKLM, 2 - HKCU

		Event_HPPNotifyHomePage_propLastProperty	// Always the last
	};
    
    enum Event_LogEntry_HomePageActivity_properties         // S = string, L = long, T = timestamp
    {
        Event_LogEntry_HomePageActivity_propBase = Event_HPPNotifyHomePage_propLastProperty,

        Event_LogEntry_HomePageActivity_propUserName,       // S - User name associated with the 
                                                            //     process that originated the 
                                                            //     action
        Event_LogEntry_HomePageActivity_propUserSID,        // S - SID associated with the process
                                                            //     that originated the action
        Event_LogEntry_HomePageActivity_propSessionID,      // L - Session ID associated with the 
                                                            //     process that originated the 
                                                            //     action
        Event_LogEntry_HomePageActivity_propProcessID,      // L - Process ID that wrote the 
                                                            //     file (optional)
        Event_LogEntry_HomePageActivity_propProcessName,    // S - Process Name that originated 
                                                            //     the action
        Event_LogEntry_HomePageActivity_propProcessPath,    // S - Full Path of the process that
                                                            //     originated the action
        Event_LogEntry_HomePageActivity_propModuleName,     // S - Module Name that originated the 
                                                            //     action, if available
        Event_LogEntry_HomePageActivity_propModulePath,     // S - Module Path that originated the 
                                                            //     action, if available
        Event_LogEntry_HomePageActivity_propStartValue,     // S - Original home page value
        Event_LogEntry_HomePageActivity_propActionValue,    // S - Original home page value
        Event_LogEntry_HomePageActivity_propActionResult,   // L - 1 - blocked, 2 - allowed
        Event_LogEntry_HomePageActivity_propTimestamp,      // T - Timestamp
        Event_LogEntry_HomePageActivity_propActionLocation, // L - 1 - HKLM, 2 - HKCU

        Event_LogEntry_HomePageActivity_propLastProperty	// Always the last
    };

    enum Event_HPPChangeHomePage_properties         // S = string, L = long, T = timestamp
    {
        Event_HPPChangeHomePage_propBase = Event_LogEntry_HomePageActivity_propLastProperty,

        Event_HPPChangeHomePage_propUserName,       // S - User name associated with the 
                                                    //     process that originated the 
                                                    //     action
        Event_HPPChangeHomePage_propUserSID,        // S - SID associated with the process
                                                    //     that originated the action
        Event_HPPChangeHomePage_propSessionID,      // L - Session ID associated with the 
                                                    //     process that originated the 
                                                    //     action
        Event_HPPChangeHomePage_propProcessID,      // L - Process ID that wrote the 
                                                    //     file (optional)
        Event_HPPChangeHomePage_propProcessName,    // S - Process Name that originated 
                                                    //     the action
        Event_HPPChangeHomePage_propProcessPath,    // S - Full Path of the process that
                                                    //     originated the action
        Event_HPPChangeHomePage_propModuleName,     // S - Module Name that originated the 
                                                    //     action, if available
        Event_HPPChangeHomePage_propModulePath,     // S - Module Path that originated the 
                                                    //     action, if available
        Event_HPPChangeHomePage_propNewHomePage,    // S - Original home page value
        Event_HPPChangeHomePage_propLocation,       // L - 1 = HKLM, 2 = HKCU
        
        Event_HPPChangeHomePage_propLastProperty	// Always the last
    };

    enum Event_HPPOptionsChanged_properties         // S = string, L = long, T = timestamp
    {
        Event_HPPOptionsChanged_propBase = Event_HPPChangeHomePage_propLastProperty,

        Event_HPPOptionsChanged_propUserName,       // S - User name associated with the 
                                                    //     process that originated the 
                                                    //     action
        Event_HPPOptionsChanged_propUserSID,        // S - SID associated with the process
                                                    //     that originated the action
        Event_HPPOptionsChanged_propSessionID,      // L - Session ID associated with the 
                                                    //     process that originated the 
                                                    //     action
        Event_HPPOptionsChanged_propProcessID,      // L - Process ID that wrote the 
                                                    //     file (optional)
        Event_HPPOptionsChanged_propProcessName,    // S - Process Name that originated 
                                                    //     the action
        Event_HPPOptionsChanged_propProcessPath,    // S - Full Path of the process that
                                                    //     originated the action
    
        Event_HPPOptionsChanged_propLastProperty	// Always the last
    };

    enum Event_IE_Started_properties         // S = string, L = long, T = timestamp
    {
        Event_IE_Started_propBase = Event_HPPOptionsChanged_propLastProperty,

        Event_IE_Started_propUserName,       // S - User name associated with the 
                                             //     process that originated the 
                                             //     action
        Event_IE_Started_propUserSID,        // S - SID associated with the process
                                             //     that originated the action
        Event_IE_Started_propSessionID,      // L - Session ID associated with the 
                                             //     process that originated the 
                                             //     action
        Event_IE_Started_propProcessID,      // L - Process ID that originated the 
                                             //     action
        Event_IE_Started_propThreadID,       // L - Thread ID that originated the 
                                             //     action
    
        Event_IE_Started_propLastProperty	 // Always the last
    };

    enum HPP_FEATURE_STATE
    {
        HPP_FEATURE_STATE_DISABLED = 0, 
        HPP_FEATURE_STATE_ENABLED 
    };

    enum HPP_STARTUP
    {
       HPP_STARTUP_MANUAL_STARTUP = 1, 
       HPP_STARTUP_RUN_AT_STARTUP 
    };

    enum HPP_ACTIONS                                // Actions for Event_HPPNotifyHomePage_propActionResult
    {
        HPP_ACTION_BLOCK = 1,
        HPP_ACTION_ALLOW
    };
    
    enum HPP_LOCATIONS           // Locations for HPPChangeHomePage, LogEntry_HomePageActivity, and HPPNotifyHomePage
    {
        HPP_LOCATIONS_HKLM = 1,
        HPP_LOCATIONS_HKCU = 2
    };
    
    enum HPP_ALERT_FREQUENCY
    {
        HPP_ALERT_FREQUENCY_ON_HOME_PAGE_CHANGE = 1,
        HPP_ALERT_FREQUENCY_ON_HOME_PAGE_ACCESS,
        HPP_ALERT_FREQUENCY_NEVER
    };
    
    enum HPP_FIRST_RUN_DIALOG
    {
        HPP_FIRST_RUN_DIALOG_INVALID_VALUE = 0,
        HPP_FIRST_RUN_DIALOG_SHOW = 1,
        HPP_FIRST_RUN_DIALOG_SUPPRESS
    };
	
#pragma message(AUTO_FUNCNAME "TODO: Code Review Item")
	// 
	//  TODO: Code Review Item
	//   Event_APP_PendingAlerts_properties properties are not used, and should be removed from this header

	enum Event_APP_PendingAlerts_properties         // S = string, L = long 
	{
		Event_APP_PendingAlerts_propBase = Event_IE_Started_propLastProperty,

		Event_APP_PendingAlerts_propAlertsType,     // L - Type of alerts to receive

		Event_APP_PendingAlerts_propLastProperty	// Always the last
	};

	enum Event_APP_OptionsChanged_properties             // S = string, L = long 
	{
		Event_APP_OptionsChanged_propBase = Event_APP_PendingAlerts_propBase,

		Event_APP_OptionsChanged_propStartup,            // L - 1 - no change, 2 - changed
		Event_APP_OptionsChanged_propNotifyFrequency,    // L - 1 - no change, 2 - changed
		Event_APP_OptionsChanged_propDefaultBlockAction, // L - 1 - no change, 2 - changed

        Event_APP_OptionsChanged_propLastProperty	     // Always the last
	};




};


class CHPPEventCommonInterface : public ccEvtMgr::CSerializableEventEx
{
public:
	virtual bool SetPropertyBSTR(long lPropIndex, BSTR pbszValue) throw() = 0;
	virtual bool GetPropertyBSTR(long lPropIndex, LPBSTR pbszValue) throw() = 0;

	virtual bool SetPropertyLONG(long lPropIndex, LONG lValue) throw() = 0;
	virtual bool GetPropertyLONG(long lPropIndex, LPLONG plValue) throw() = 0;

    virtual bool SetPropertySYSTEMTIME(long lPropindex, SYSTEMTIME *pSystemTime) throw() = 0;
    virtual bool GetPropertySYSTEMTIME(long lPropindex, SYSTEMTIME *pSystemTime) throw() = 0;

};

// {603BA2B6-E1B7-469f-A7ED-AA357438BD54}
SYM_DEFINE_INTERFACE_ID(IID_HPPEventCommon, 
                        0x603ba2b6, 0xe1b7, 0x469f, 0xa7, 0xed, 0xaa, 0x35, 0x74, 0x38, 0xbd, 0x54);

typedef CSymPtr<CHPPEventCommonInterface> CHPPEventCommonInterfacePtr;
typedef CSymQIPtr<CHPPEventCommonInterface, &IID_HPPEventCommon> CHPPEventCommonInterfaceQIPtr;
