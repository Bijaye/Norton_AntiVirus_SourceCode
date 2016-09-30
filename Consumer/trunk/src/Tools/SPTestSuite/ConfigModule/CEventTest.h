////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "CEventProvider.h"
#include "ccEventManagerHelper.h"
#include "ccProviderHelper.h"
#include "SymProtectEventsLoader.h"
#include "SymProtectEventsInterfacePrivate.h"
#include "BBEventID.h"
#include "ccEventEx.h"

class CEventTest
{
public:
    CEventTest(void);
    ~CEventTest(void);
    enum HASHTYPE
        {
            MD5 = 0,
            SHA,
            UNKOWN
        };

    bool Initialize();
    bool SendAddUser(const wchar_t* szAccountName);
    bool SendDeleteUser(const wchar_t* szAccountName);
    bool SendGetUserCount();
    bool SendGetUserName(ULONG ulIndex);
    bool SendAddMD5Hash(const wchar_t* szHashName, const wchar_t* pHash, unsigned long ulFileLength);
    bool SendAddSHA1Hash(const wchar_t* szHashName, const wchar_t* pHash, unsigned long ulFileLength);
    bool SendDeleteHash(const wchar_t* szName);
    bool SendGetHashCount();
    bool SendGetHashName(ULONG ulIndex);
    bool SendAddLocation(const wchar_t* szLocation, bool bFileOnly);
    bool SendDeleteLocation(const wchar_t* szLocation);
    bool SendGetLocationCount(void);
    bool SendGetLocationName(ULONG ulIndex);
    bool SendAddMSILocation(const wchar_t* szInstallLocation,const wchar_t* szInstallLocation2);
    bool SendAddMSIGUID(const wchar_t* guid,const wchar_t* guid2);
    bool SendAddMSIGUIDLocation(const wchar_t* guid,const wchar_t* szInstallLocation);
    bool SendDeleteMSIGUID(const wchar_t* guid);
    bool SendDeleteMSILocation(const wchar_t* szInstallLocation);
    //bool SendDeleteInstallLocation(const wchar_t* szLocation);
    bool SendAddDeviceEnabled(unsigned long ulDeviceType);
    bool SendGetDeviceStatus(unsigned long ulDeviceType);
    bool SendAddDeviceDisabled(unsigned long ulDeviceType);
    bool sendSetInstallSecurityLevel(int level);
    bool sendQueryInstallSecurityLevel(void);
    bool sendQueryMSIAuthorizationCount(void);

    bool SendSetFileProtection(bool bEnabled, bool bLogOnly);
    bool SendSetRegistryProtection(bool bEnabled, bool bLogOnly);
    bool SendSetProcessProtection(bool bEnabled, bool bLogOnly);
	bool SendSetNamedObjectProtection(bool bEnabled, bool bLogOnly);

    bool SendStopSymProtect();

    // Asks SymProtect to start.
    bool SendStartSymProtect();

    // Sends a notice to SymProtect that the contents of the Manifests
    // folder has been updated and should be incorporated into the active
    // protection rules
    bool SendRefreshManifests();

    // Sends a "fake" SymProtect reload event to all subscribers. This
    // functionality should only be used by SymProtect itself.
    bool SendUpdateEvent();

    // Retrieves the current status of SymProtect. 
    bool SendGetSymProtectStatus();

    bool SendQueryFileProtectionLevel();

    bool SendQueryRegistryProtectionLevel();

    bool SendQueryProcessProtectionLevel();

	bool SendQueryNamedObjectProtectionLevel();

    bool SendSettingsValidation();

    bool SendConfigurationUpdate();

    bool SendForceRefreshEvent();

    bool SendGarbage();

    bool SendSystemNotify();

    bool SendOldStartEvent();

    bool SendOldStopEvent();

	bool SendIsSymProtectTemporarilyDisabled();

	bool SendGetDisabledTimeRemaining();

	bool SendGetSymProtectComponentState();

	bool SendDisableSymProtect(unsigned int uMinutes);

	bool SendCancelDisableSymProtect();

	bool SendSetSymProtectComponentState(const wchar_t* enabled);

    // Notification Filter
    bool SendAddNotificationFilter ( const wchar_t* szFilterPath ) ;
    bool SendDeleteNotificationFilter ( const wchar_t* szFilterPath ) ;
    bool SendGetNotificationFilterCount ( void ) ;
    bool SendGetNotificationFilter ( ULONG ulIndex ) ;

	bool SendDetectedEvent();

private:
    CEventProvider m_EventProvider;

    // Retrieves the current status of SymProtect. SP_STATE_UNKNOWN is a
    // possible return value from this function.
    SymProtectEvt::CQueryStateEventEx::SP_QUERY_STATE GetSymProtectStatus();
    bool SendSetSymProtectEnabled(bool bEnabled);
    bool SendConfigEvent(long nEventType);
    bool SendOldConfigEvent(long nEventType);
    bool SendAddMD5Hash(const wchar_t* szHashName, const void* pHash, unsigned long ulFileLength);
    bool SendAddSHA1Hash(const wchar_t* szHashName, const void* pHash, unsigned long ulFileLength);
    const void* CharToBhash(const wchar_t* pHash_arg,const HASHTYPE algorithm_arg);
    bool SendQueryEvent(long EventType,ccEvtMgr::CEventEx*& pOutEvent);
};
