#pragma once
#include "AllNavEvents.h"   // Base defines

namespace AVStatus
{
    enum enumStatusRequestEvent
    {
        propRequestedFeatures = AV::Event_Base_propLastProperty,   // Array of longs - List of requested features status
        propCurrentStatus,                          // ED - Feature status for all requested features
        propRequesterID,                            // L - *optional* module ID of the requester
    };

    // Feature ID that updated is based on the event ID
    enum enumStatusUpdateEvent
    {
        propNewStatus = AV::Event_Base_propLastProperty,   // ED - Feature status for updated feature (and others)
    };

    /////////////////////////////////////////////////////////////////////////////
	// The various states a status item can be in.
    enum enumSTATUS
    {
		statusNone = -1,    // Uninitialized - ERROR  (Objects should never return this)
		statusError = 0,	// Couldn't get state - ERROR 
		statusNotInstalled, // Should be installed but isn't, tried to init but failed - ERROR
		statusNotRunning,   // Not loaded
		statusEnabled,      // Default OK setting
		statusDisabled,     // Loaded but turned off
		statusNotAvailable, // Not supposed to exist ( e.g. Rescue Disk on NT ) Not an error!
		statusRental		// Managed rental   &$ ditch this!!
    };


    enum propsStatus
    {
        propALUStatus,      // L - Status

        propAPStatus,       // L - Status
        propAPSpywareStatus,// L - Spyware status (must support for NPF/NIS 8.2 compatibility)

        propDefsStatus,     // L - Status
        propDefsTime,       // L - Virus Defs date in time_t format
        propDefsDate,       // S - Virus Defs date in localized short date format
        propDefsUpdateType, // L - Reason for the update, see enumDefsUpdateType

        propFSSStatus,      // L - Status - Dynamic
        propFSSDate,        // B - Date of FSS in DATE format
        propFSSDateS,       // S - Date of FSS in localized short date format

        propIWPStatus,      // L - Status - Dynamic

        // Querying Email status can take up to 10 sec if email is supposed to 
        // be on but isn't. This can be the case during startup.
        //
        propEmailStatus,    // L - Status - Dynamic
        propEmailSMTP,      // L - Is SMTP being scanned? - Options
        propEmailPOP,       // L - Is POP being scanned? - Options
        propEmailOEH,       // L - Is OEH (Worm blocking) on? - Options

        propLicStatus,      // L - Status - Dynamic
        propLicType,        // L - Licensing type - Options
        propLicState,       // L - Licensing state - Options
        propLicZone,        // L - Licensing zone - Options
        propLicValid,       // L - Is license valid? - Dynamic
        propLicVendorID,    // L - License vendor id

        // Subscriptions
        propSubWarning,     // L - Is sub. in WARNING state?
        propSubExpired,     // L - Is sub. in EXPIRED state?
        propSubDateS,       // S - Date in short date format
        propSubDaysLeft,    // L - # days until expired
        propSubCanRenew,    // L - Is this a renewable subscription?
        propSubDate,        // L - Date of expiration in DATE format     

        propIMMSN,          // L - MSN IM client status (enumIMStatus)
        propIMYIM,          // L - Yahoo IM client status (enumIMStatus)
        propIMAOL,          // L - AOL IM client status (enumIMStatus)
        propIMTOM ,         // L - T-Online IM client status (enumIMStatus)

        propSpywareCat,     // L - Spyware category on?
        propSpywareInstalled,   // L - Spyware feature installed? (saves an extra call to Options)

        propLAST            // Always last!!
    };

    enum enumIMStatus
    {
        IM_Status_NotInstalled = 0,
        IM_Status_Protected,
        IM_Status_NotProtected
    };

    enum enumDefsUpdateType
    {
        Defs_Update_Type_None = 0,  // No reason, just initial startup query
        Defs_Update_Type_ALU,       // ALU updated defs
        Defs_Update_Type_LU,        // LiveUpdate updated the defs
        Defs_Update_Type_IU,        // IntelligentUpdater, or other directory write activity
    };

    // IWP saves it's status information in this key as a 'backdoor' for NAVStatus
    // to read status changes easier than implementing a IWPStateChangeSink. If
    // you want IWP status updates use the sink or NAVStatus. Don't rely on this key, please
    // it might change without warning.
    //
    const WCHAR szIWPStatusKey [] = L"Internet Worm Protection\\Status";
    // Internet Worm Protection
    //
    const WCHAR szIWPStatus [] =       L"IWP:Status";       // L - Status
};