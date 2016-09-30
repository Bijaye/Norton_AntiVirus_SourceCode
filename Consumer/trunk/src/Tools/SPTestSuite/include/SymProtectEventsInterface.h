////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ccEventId.h"
#include "ccSerializableEventEx.h"
#include "SymProtectEventTypes.h"

//
// CC team no longer maintains the event IDs for individual groups.
// they do reserve event id ranges for each group
// the BB team's IDs are reserved between 6000-6999
//
#define CC_SYMPROTECT_EVENT_ID_BASE                 6000

// The SP 1.0 (Pavlov) events
#define CC_SYMPROTECT_POLICY_EVENT                  (CC_SYMPROTECT_EVENT_ID_BASE)
#define CC_SYMPROTECT_START_EVENT                   (CC_SYMPROTECT_EVENT_ID_BASE + 1)
#define CC_SYMPROTECT_STOP_EVENT                    (CC_SYMPROTECT_EVENT_ID_BASE + 2)
#define CC_SYMPROTECT_QUERY_EVENT                   (CC_SYMPROTECT_EVENT_ID_BASE + 3)
#define CC_SYMPROTECT_RELOAD_EVENT                  (CC_SYMPROTECT_EVENT_ID_BASE + 4)
#define CC_SYMPROTECT_MANIFEST_EVENT                (CC_SYMPROTECT_EVENT_ID_BASE + 5)

// The BB 1.5 (Skinner) events
#define CC_BB_SETTINGS_BASE_EVENT                   (CC_SYMPROTECT_EVENT_ID_BASE + 6)
#define CC_BB_SETTINGS_VALIDATION_EVENT             (CC_SYMPROTECT_EVENT_ID_BASE + 7)
#define CC_BB_CONFIGURATION_UPDATE_EVENT            (CC_SYMPROTECT_EVENT_ID_BASE + 8)
#define CC_BB_SETTINGS_FORCE_REFRESH_EVENT          (CC_SYMPROTECT_EVENT_ID_BASE + 9)
#define CC_BB_SYSTEM_NOTIFY_EVENT                   (CC_SYMPROTECT_EVENT_ID_BASE + 10)
#define CC_SYMPROTECT_SET_CONFIG_EVENT              (CC_SYMPROTECT_EVENT_ID_BASE + 11)
#define CC_SYMPROTECT_QUERY_CONFIG_EVENT            (CC_SYMPROTECT_EVENT_ID_BASE + 12)
#define CC_BB_AUTHORIZED_USER_QUERY_EVENT           (CC_SYMPROTECT_EVENT_ID_BASE + 13)
#define CC_BB_AUTHORIZED_USER_SET_EVENT             (CC_SYMPROTECT_EVENT_ID_BASE + 14)
#define CC_BB_AUTHORIZED_HASH_QUERY_EVENT           (CC_SYMPROTECT_EVENT_ID_BASE + 15)
#define CC_BB_AUTHORIZED_HASH_SET_EVENT             (CC_SYMPROTECT_EVENT_ID_BASE + 16)
#define CC_BB_AUTHORIZED_LOCATION_QUERY_EVENT       (CC_SYMPROTECT_EVENT_ID_BASE + 17)
#define CC_BB_AUTHORIZED_LOCATION_SET_EVENT         (CC_SYMPROTECT_EVENT_ID_BASE + 18)
#define CC_BB_AUTHORIZED_DEVICE_QUERY_EVENT         (CC_SYMPROTECT_EVENT_ID_BASE + 19)
#define CC_BB_AUTHORIZED_DEVICE_SET_EVENT           (CC_SYMPROTECT_EVENT_ID_BASE + 20)
#define CC_BB_BEHAVIORBLOCKING_DETECTION_EVENT      (CC_SYMPROTECT_EVENT_ID_BASE + 21)
#define CC_BB_BEHAVIORBLOCKING_DEBUG_EVENT          (CC_SYMPROTECT_EVENT_ID_BASE + 22)
#define CC_BB_AUTHORIZED_MSI_SET_EVENT              (CC_SYMPROTECT_EVENT_ID_BASE + 23)
#define CC_BB_AUTHORIZED_MSI_QUERY_EVENT            (CC_SYMPROTECT_EVENT_ID_BASE + 24)
#define CC_BB_BEHAVIORBLOCKING_STATUS_EVENT         (CC_SYMPROTECT_EVENT_ID_BASE + 25)
#define CC_BB_BBFRAMEWORK_CONFIG_EVENT              (CC_SYMPROTECT_EVENT_ID_BASE + 26)


namespace SymProtectEvt
{

class CPolicyEventEx : public ccEvtMgr::CSerializableEventEx
{
public:
    virtual SYSTEMTIME GetTimeStamp() const throw() = 0;

    virtual SYMRESULT GetActor( DWORD& dwActorPID, LPWSTR wszActorName, DWORD& dwSize ) const throw() = 0;
    virtual SYMRESULT GetActor( LPWSTR wszActorName, DWORD& dwSize ) const throw() = 0;
    virtual SYMRESULT GetActor( DWORD& dwActorPID ) const throw() = 0;

    virtual SYMRESULT GetAction( SP_ACTION_TYPE& eAction ) const throw() = 0;

    virtual SYMRESULT GetTarget( DWORD& dwTargetPID, LPWSTR wszTargetName, DWORD& dwSize ) const throw() = 0;
    virtual SYMRESULT GetTarget( LPWSTR wszTargetName, DWORD& dwSize ) const throw() = 0;
    virtual SYMRESULT GetTarget( DWORD& dwTargetPID ) const throw() = 0;

    virtual SYMRESULT GetTerminalSession( DWORD& dwSession ) const throw() = 0;

    virtual SYMRESULT WasActionBlocked( bool& bWasBlocked ) const throw() = 0;

    enum { TypeId = CC_SYMPROTECT_POLICY_EVENT };
};
SYM_DEFINE_INTERFACE_ID(IID_PolicyEvent, 
                        0xd6034305, 0xe137, 0x462d, 0xa5, 0x03, 0xd5, 0x89, 0xa8, 0xac, 0x15, 0xd6);

typedef CSymPtr<CPolicyEventEx> CPolicyEventExPtr;
typedef CSymQIPtr<CPolicyEventEx, &IID_PolicyEvent> CPolicyEventExQIPtr;


class CReloadDllEventEx : public ccEvtMgr::CSerializableEventEx
{
public:
    enum { TypeId = CC_SYMPROTECT_RELOAD_EVENT }; 
};
SYM_DEFINE_INTERFACE_ID(IID_ReloadDllEvent, 
                        0xfc651426, 0xde30, 0x4de0, 0x8e, 0xdd, 0x62, 0x38, 0x71, 0x35, 0x1, 0x57);

typedef CSymPtr<CReloadDllEventEx> CReloadDllEventExPtr;
typedef CSymQIPtr<CReloadDllEventEx, &IID_ReloadDllEvent> CReloadDllEventExQIPtr;


class CManifestChangeEventEx : public ccEvtMgr::CSerializableEventEx
{
public:
    enum { TypeId = CC_SYMPROTECT_MANIFEST_EVENT }; 
};
SYM_DEFINE_INTERFACE_ID(IID_ManifestChangeEvent, 
                        0xa77304a2, 0xf5c2, 0x4646, 0x90, 0x8e, 0x6b, 0x31, 0x77, 0xe4, 0xe9, 0x8c);

typedef CSymPtr<CManifestChangeEventEx> CManifestChangeEventExPtr;
typedef CSymQIPtr<CManifestChangeEventEx, &IID_ManifestChangeEvent> CManifestChangeEventExQIPtr;


// This interface is supported for legacy compatibility
// Newer clients should use ISymProtectSetConfigEvent
class CStopEventEx : public ccEvtMgr::CSerializableEventEx
{
public:
    enum { TypeId = CC_SYMPROTECT_STOP_EVENT }; 
};
SYM_DEFINE_INTERFACE_ID(IID_StopEvent, 
                        0xd513fff6, 0x8a23, 0x424e, 0x90, 0xef, 0x7, 0x96, 0x24, 0xae, 0xa4, 0x56);

typedef CSymPtr<CStopEventEx> CStopEventExPtr;
typedef CSymQIPtr<CStopEventEx, &IID_StopEvent> CStopEventExQIPtr;


// This interface is supported for legacy compatibility
// Newer clients should use ISymProtectQueryConfigEvent
class CQueryStateEventEx : public ccEvtMgr::CSerializableEventEx
{
public:
    enum SP_QUERY_STATE
    {
        SP_STATE_OFF = 0x0,        // SymProtect is operating correctly, driver and service are not loaded.
        SP_STATE_ON,               // SymProtect is operating correctly, driver and service are loaded.
        SP_STATE_UPDATING,         // SymProtect is being updated, driver and service are not loaded.
        SP_STATE_ERROR_ON,         // SymProtect is intended to be active, but not operating correctly, driver and service are not loaded.
        SP_STATE_ERROR_OFF,        // SymProtect is not operating correctly, driver and service are not loaded. 
        SP_STATE_ERROR_UNKNOWN,    // Either the state is not present in ccSettings, it's corrupt, or no one responded to the query.
    };

    virtual SYMRESULT GetSymProtectState( SP_QUERY_STATE& eState ) const throw() = 0;

    enum { TypeId = CC_SYMPROTECT_QUERY_EVENT }; 
};
SYM_DEFINE_INTERFACE_ID(IID_QueryStateEvent, 
                        0xc72a3460, 0xe3a8, 0x4d1a, 0x81, 0x22, 0x72, 0x44, 0x1b, 0x15, 0xf3, 0x9b);

typedef CSymPtr<CQueryStateEventEx> CQueryStateEventExPtr;
typedef CSymQIPtr<CQueryStateEventEx, &IID_QueryStateEvent> CQueryStateEventExQIPtr;


// Base BB settings event interface
class ISymBBSettingsEvent : public ccEvtMgr::CSerializableEventEx
{
public:
    enum { TypeId = CC_BB_SETTINGS_BASE_EVENT }; 

    // enum of error types currently matching severity enum in error
    // stack if error stack severity types are later extended, new
    // values can be exposed in a new event interface
    enum ErrorTypes
    {
        UNRECOVERABLE_ERROR,
        RECOVERED_FROM_ERROR,
        NOT_REALLY_AN_ERROR
    };

    // Returns the most severe error from the contained error stack
    virtual SYMRESULT GetResult(ErrorTypes& eMostSevereError) const = 0;

    // Gets the contents of the error stack in human-readable form
    virtual SYMRESULT GetErrorString(wchar_t* pszErrorData,
                                     size_t& bufferSize) const = 0;

    // Sets the result value for the requested operation (represented
    // by the derived event type)
    virtual void SetOperationResult( HRESULT ) = 0;

    // Gets the result value for the requested operation (represented
    // by the derived event type)
    virtual HRESULT GetOperationResult() const = 0;
};

// {214D1E11-AFC7-402c-8CDD-BC6EE1BA315F}
SYM_DEFINE_INTERFACE_ID(IID_BBSettingsEvent, 
0x214d1e11, 0xafc7, 0x402c, 0x8c, 0xdd, 0xbc, 0x6e, 0xe1, 0xba, 0x31, 0x5f);

typedef CSymPtr<ISymBBSettingsEvent>   ISymBBSettingsEventPtr;
typedef CSymQIPtr<ISymBBSettingsEvent, &IID_BBSettingsEvent> ISymBBSettingsEventQIPtr;

// Public event to cause a settings validity check
class ISymBBSettingsValidationEvent : public ISymBBSettingsEvent
{
    // Generic subclass of Settings Event
public:
    enum { TypeId = CC_BB_SETTINGS_VALIDATION_EVENT }; 
};
// {54A60184-1AE4-44e6-867B-EC1D41931346}
SYM_DEFINE_INTERFACE_ID(IID_BBSettingsValidationEvent, 
0x54a60184, 0x1ae4, 0x44e6, 0x86, 0x7b, 0xec, 0x1d, 0x41, 0x93, 0x13, 0x46);

typedef CSymPtr<ISymBBSettingsValidationEvent>   ISymBBSettingsValidationEventPtr;
typedef CSymQIPtr<ISymBBSettingsValidationEvent, 
                  &IID_BBSettingsValidationEvent> ISymBBSettingsValidationEventQIPtr;

// Public event to force a settings refresh
class ISymBBSettingsForceRefreshEvent : public ISymBBSettingsEvent
{
    // Generic subclass of Settings Event
public:
    enum { TypeId = CC_BB_SETTINGS_FORCE_REFRESH_EVENT }; 
};
// {D997B6EA-96F8-417c-AA50-95A6DC0E66F3}
SYM_DEFINE_INTERFACE_ID(IID_BBSettingsForceRefreshEvent, 
0xd997b6ea, 0x96f8, 0x417c, 0xaa, 0x50, 0x95, 0xa6, 0xdc, 0xe, 0x66, 0xf3);

typedef CSymPtr<ISymBBSettingsForceRefreshEvent>   ISymBBSettingsForceRefreshEventPtr;
typedef CSymQIPtr<ISymBBSettingsForceRefreshEvent, 
                  &IID_BBSettingsForceRefreshEvent> ISymBBSettingsForceRefreshEventQIPtr;

// Forward class declaration
class CInternalSymProtectConfigEventImpl;

// On/Off and other configuration settings
class IBBSymProtectSetConfigEvent : public ISymBBSettingsEvent
{
    // Generic subclass of Settings Event
public:
    enum { TypeId = CC_SYMPROTECT_SET_CONFIG_EVENT };

    // Control the settings for the individual protection categories
    virtual SYMRESULT SetFileProtectionLevel(SP_PROTECTION_LEVEL eLevel) throw() = 0;
    virtual SYMRESULT SetProcessProtectionLevel(SP_PROTECTION_LEVEL eLevel) throw() = 0;
    virtual SYMRESULT SetRegistryProtectionLevel(SP_PROTECTION_LEVEL eLevel) throw() = 0;

    // Control the "temporary disable" feature
    virtual SYMRESULT DisableSymProtect(unsigned int uMinutes) throw() = 0;
    virtual SYMRESULT CancelDisableSymProtect() throw() = 0;

    // Turn the entire SymProtect component on/off
    virtual SYMRESULT SetSymProtectComponentState(bool bEnable = true) throw() = 0;

    // Internal data-access function
    virtual CInternalSymProtectConfigEventImpl& GetStateData() throw() = 0;
};
// {2C93EDC6-A262-4376-9D88-9424E6254B93}
SYM_DEFINE_INTERFACE_ID(IID_SymProtectSetConfigEvent, 
0x2c93edc6, 0xa262, 0x4376, 0x9d, 0x88, 0x94, 0x24, 0xe6, 0x25, 0x4b, 0x93);

typedef CSymPtr<IBBSymProtectSetConfigEvent>   IBBSymProtectSetConfigEventPtr;
typedef CSymQIPtr<IBBSymProtectSetConfigEvent, 
                  &IID_SymProtectSetConfigEvent> IBBSymProtectSetConfigEventQIPtr;

// On/Off and other configuration settings
class IBBSymProtectQueryConfigEvent : public ISymBBSettingsEvent
{
    // Generic subclass of Settings Event
public:
    enum { TypeId = CC_SYMPROTECT_QUERY_CONFIG_EVENT };
    
    // Retrieve individual protection category configurations
    virtual SYMRESULT GetFileProtectionLevel(SP_PROTECTION_LEVEL& eLevel) const throw() = 0;
    virtual SYMRESULT GetRegistryProtectionLevel(SP_PROTECTION_LEVEL& eLevel) const throw() = 0;
    virtual SYMRESULT GetProcessProtectionLevel(SP_PROTECTION_LEVEL& eLevel) const throw() = 0;

    // "Temporary disable" inspection functions
    virtual SYMRESULT IsSymProtectTemporarilyDisabled(bool& bEnabled) const throw() = 0;
    virtual SYMRESULT GetDisabledTimeRemaining(unsigned int& uMinutes) const throw() = 0;

    // Query the global on/off state (and errors)
    virtual SYMRESULT GetSymProtectComponentState(bool& bEnabled, bool& bError) const throw() = 0;

    // Internal data-access function
    virtual CInternalSymProtectConfigEventImpl& GetStateData() throw() = 0;
};
// {47590D66-A26A-4aaf-B681-A769303A39C1}
SYM_DEFINE_INTERFACE_ID(IID_SymProtectQueryConfigEvent, 
0x47590d66, 0xa26a, 0x4aaf, 0xb6, 0x81, 0xa7, 0x69, 0x30, 0x3a, 0x39, 0xc1);

typedef CSymPtr<IBBSymProtectQueryConfigEvent>   IBBSymProtectQueryConfigEventPtr;
typedef CSymQIPtr<IBBSymProtectQueryConfigEvent, 
                  &IID_SymProtectQueryConfigEvent> IBBSymProtectQueryConfigEventQIPtr;


/////////////////////////////////////////////////////////////////////////////
// Authorization-related events

// Forward declaration for the internal data representation class
class CAuthorizationEventData;

// Edit the list of authorized users
class IBBAuthorizedUsersSetEvent : public ISymBBSettingsEvent
{
    // Generic subclass of Settings Event
public:
    enum { TypeId = CC_BB_AUTHORIZED_USER_SET_EVENT };

    // Add a new user to the authorized users list
    virtual SYMRESULT AddUser(const wchar_t* pszAccountName) throw() = 0;
    // Remove an authorized user
    virtual SYMRESULT DeleteUser(const wchar_t* pszAccountName) throw() = 0;

    // Internal data-access function
    virtual CAuthorizationEventData& GetEventData() throw() = 0;
};
// {E492AA16-73ED-4241-A5AD-5993B58D7691}
SYM_DEFINE_INTERFACE_ID(IID_BBAuthorizedUsersSetEvent, 
0xE492AA16, 0x73ED, 0x4241, 0xA5, 0xAD, 0x59, 0x93, 0xB5, 0x8D, 0x76, 0x91);

typedef CSymPtr<IBBAuthorizedUsersSetEvent> IBBAuthorizedUsersSetEventPtr;
typedef CSymQIPtr<IBBAuthorizedUsersSetEvent, 
                  &IID_BBAuthorizedUsersSetEvent> IBBAuthorizedUsersSetEventQIPtr;


// Querying the authorized user list
class IBBAuthorizedUsersQueryEvent : public ISymBBSettingsEvent
{
    // Generic subclass of Settings Event
public:
    enum { TypeId = CC_BB_AUTHORIZED_USER_QUERY_EVENT };

    // get the authorized user list
    virtual SYMRESULT GetUserCount(ULONG& ulCount) const throw() = 0;
    virtual SYMRESULT GetUser(ULONG ulIndex, const wchar_t** pszAccountName) = 0;

    // Internal data-access function
    virtual CAuthorizationEventData& GetEventData() throw() = 0;
};
// {EF0EE594-494C-4a6f-9D09-48550FBE8089}
SYM_DEFINE_INTERFACE_ID(IID_BBAuthorizedUsersQueryEvent, 
0xEF0EE594, 0x494C, 0x4a6f, 0x9D, 0x09, 0x48, 0x55, 0x0F, 0xBE, 0x80, 0x89);

typedef CSymPtr<IBBAuthorizedUsersQueryEvent>   IBBAuthorizedUsersQueryEventPtr;
typedef CSymQIPtr<IBBAuthorizedUsersQueryEvent, 
                  &IID_BBAuthorizedUsersQueryEvent> IBBAuthorizedUsersQueryEventQIPtr;

// Modify the list of authorized hashes
class IBBAuthorizedHashesSetEvent : public ISymBBSettingsEvent
{
public:
    enum { TypeId = CC_BB_AUTHORIZED_HASH_SET_EVENT };

    // Add an authorized hash 
    // (hash algorithms are listed in SymProtectEventTypes.h)
    virtual SYMRESULT AddHash( const wchar_t* pszName,
                               ULONG ulHashAlgorithm,
                               const void* pHash,
                               ULONG ulHashLength,
                               ULONG ulFileLength ) throw() = 0;

    // Remove an authorized hash by name
    virtual SYMRESULT DeleteHash( const wchar_t* pszName ) throw() = 0;

    // Internal data-access function
    virtual CAuthorizationEventData& GetEventData() throw() = 0;
};
// {A6552A66-B845-4dbc-B43C-FA96A672BFFD}
SYM_DEFINE_INTERFACE_ID(IID_BBAuthorizedHashesSetEvent, 
0xa6552a66, 0xb845, 0x4dbc, 0xb4, 0x3c, 0xfa, 0x96, 0xa6, 0x72, 0xbf, 0xfd);

typedef CSymPtr<IBBAuthorizedHashesSetEvent> IBBAuthorizedHashesSetEventPtr;
typedef CSymQIPtr<IBBAuthorizedHashesSetEvent, 
                  &IID_BBAuthorizedHashesSetEvent> IBBAuthorizedHashesSetEventQIPtr;


// Query the list of authorized hashes
class IBBAuthorizedHashesQueryEvent : public ISymBBSettingsEvent
{
public:
    enum { TypeId = CC_BB_AUTHORIZED_HASH_QUERY_EVENT };

    // Get the number of authorized hashes
    virtual SYMRESULT GetHashCount( ULONG& ulCount ) const throw() = 0;

    // Retrieve a particular hash (hash algorithms are listed 
    // in SymProtectEventTypes.h)
    virtual SYMRESULT GetHash( ULONG ulIndex,
                               ULONG& ulHashAlgorithm,
                               const wchar_t** ppszName,
                               const void** ppHash,
                               ULONG& ulHashLength,
                               ULONG& ulFileLength ) throw() = 0;

    // Internal data-access function
    virtual CAuthorizationEventData& GetEventData() throw() = 0;
};
// {D7681773-1CA6-4494-8A63-6642D5FA5BCF}
SYM_DEFINE_INTERFACE_ID(IID_BBAuthorizedHashesQueryEvent, 
0xd7681773, 0x1ca6, 0x4494, 0x8a, 0x63, 0x66, 0x42, 0xd5, 0xfa, 0x5b, 0xcf);

typedef CSymPtr<IBBAuthorizedHashesQueryEvent> IBBAuthorizedHashesQueryEventPtr;
typedef CSymQIPtr<IBBAuthorizedHashesQueryEvent, 
                  &IID_BBAuthorizedHashesQueryEvent> IBBAuthorizedHashesQueryEventQIPtr;


// Query the list of authorized locations
class IBBAuthorizedLocationsQueryEvent : public ISymBBSettingsEvent
{
public:
    enum { TypeId = CC_BB_AUTHORIZED_LOCATION_QUERY_EVENT };

    // Get the number of authorized locations
    virtual SYMRESULT GetLocationCount(ULONG& ulCount) const throw() = 0;

    // Get an individual authorized location
    virtual SYMRESULT GetLocation(ULONG ulIndex, const wchar_t** ppszPath, 
                                  bool& bDontAuthSubdirs) throw() = 0;

    // Internal data-access function
    virtual CAuthorizationEventData& GetEventData() throw() = 0;
};
// {5A43ECF9-6687-4d5c-BD3E-BA9AB638B436}
SYM_DEFINE_INTERFACE_ID(IID_BBAuthorizedLocationsQueryEvent, 
0x5a43ecf9, 0x6687, 0x4d5c, 0xbd, 0x3e, 0xba, 0x9a, 0xb6, 0x38, 0xb4, 0x36);

typedef CSymPtr<IBBAuthorizedLocationsQueryEvent> IBBAuthorizedLocationsQueryEventPtr;
typedef CSymQIPtr<IBBAuthorizedLocationsQueryEvent, 
                  &IID_BBAuthorizedLocationsQueryEvent> IBBAuthorizedLocationsQueryEventQIPtr;


// Modify the list of authorized locations
class IBBAuthorizedLocationsSetEvent : public ISymBBSettingsEvent
{
public:
    enum { TypeId = CC_BB_AUTHORIZED_LOCATION_SET_EVENT };

    // Add an authorized location
    virtual SYMRESULT AddLocation(const wchar_t* pszPath, bool bDontAuthSubdirs) throw() = 0;

    // Remove an authorized location
    virtual SYMRESULT DeleteLocation(const wchar_t* pszPath) throw() = 0;

    // Internal data-access function
    virtual CAuthorizationEventData& GetEventData() throw() = 0;
};
// {121246C0-6F5B-4d64-83D5-E8B7C0974C87}
SYM_DEFINE_INTERFACE_ID(IID_BBAuthorizedLocationsSetEvent, 
0x121246c0, 0x6f5b, 0x4d64, 0x83, 0xd5, 0xe8, 0xb7, 0xc0, 0x97, 0x4c, 0x87);

typedef CSymPtr<IBBAuthorizedLocationsSetEvent> IBBAuthorizedLocationsSetEventPtr;
typedef CSymQIPtr<IBBAuthorizedLocationsSetEvent, 
                  &IID_BBAuthorizedLocationsSetEvent> IBBAuthorizedLocationsSetEventQIPtr;


// Modify the list of authorized device types
class IBBAuthorizedDevicesSetEvent : public ISymBBSettingsEvent
{
public:
    enum { TypeId = CC_BB_AUTHORIZED_DEVICE_SET_EVENT };

    // Modify the authorization level for a device type (device types are 
    // listed in SymProtectEventTypes.h)
    virtual SYMRESULT SetDeviceTypeAuthorization(ULONG ulDeviceType, 
                                                 bool bAuthorized) throw() = 0;

    // Internal data-access function
    virtual CAuthorizationEventData& GetEventData() throw() = 0;
};
// {A2DFFC96-8553-4d86-8404-29B4D07EB919}
SYM_DEFINE_INTERFACE_ID(IID_BBAuthorizedDevicesSetEvent, 
0xa2dffc96, 0x8553, 0x4d86, 0x84, 0x4, 0x29, 0xb4, 0xd0, 0x7e, 0xb9, 0x19);

typedef CSymPtr<IBBAuthorizedDevicesSetEvent> IBBAuthorizedDevicesSetEventPtr;
typedef CSymQIPtr<IBBAuthorizedDevicesSetEvent, 
                  &IID_BBAuthorizedDevicesSetEvent> IBBAuthorizedDevicesSetEventQIPtr;


// Query the authorized device list
class IBBAuthorizedDevicesQueryEvent : public ISymBBSettingsEvent
{
public:
    enum { TypeId = CC_BB_AUTHORIZED_DEVICE_QUERY_EVENT };

    // Retrieve the authorization status for a device type (types are listed
    // in SymProtectEventTypes.h)
    virtual SYMRESULT GetDeviceTypeAuthorization(ULONG ulDeviceType, 
                                                 bool& bAuthorized) const throw() = 0;

    // Internal data-access function
    virtual CAuthorizationEventData& GetEventData() throw() = 0;
};
// {64DB6A38-0EE9-4147-8326-38B6EB6760F9}
SYM_DEFINE_INTERFACE_ID(IID_BBAuthorizedDevicesQueryEvent, 
0x64db6a38, 0xee9, 0x4147, 0x83, 0x26, 0x38, 0xb6, 0xeb, 0x67, 0x60, 0xf9);

typedef CSymPtr<IBBAuthorizedDevicesQueryEvent> IBBAuthorizedDevicesQueryEventPtr;
typedef CSymQIPtr<IBBAuthorizedDevicesQueryEvent, 
                  &IID_BBAuthorizedDevicesQueryEvent> IBBAuthorizedDevicesQueryEventQIPtr;


// Enumeration for MSI Authorization types
// You must only add to this enumeration otherwise you may break older clients.
enum MSI_AUTH_TYPE
{
    authGUID    = 0,    // Authorize by GUID (pAuthData type: GUID)
    authPath    = 1,    // Authorize a path for installation (type: PCWSTR)

    // Some possible alternate authorizations
    // authHash    = 2, // Authorize by hash
    // authUser    = 3, // Authorize by user
};

// Enumeration for install policy
// Items in this enumeration may never be removed, only added
enum MSI_SECURITY_LEVEL
{
    UNKNOWN_LEVEL       = 0,
    ALLOW_NONE          = 1,
    ALLOW_ALL           = 2,
    ALLOW_AUTHORIZED    = 3,
    
    // Add any new levels before the INVALID_LEVEL
    INVALID_LEVEL       = 4
};

// Examine the list of authorized install GUIDs
class IBBAuthorizedMSIQueryEvent : public ISymBBSettingsEvent
{
public:
    enum { TypeId = CC_BB_AUTHORIZED_MSI_QUERY_EVENT };

    // Retrieves which MSI actions are currently allowed
    virtual SYMRESULT GetInstallSecurityLevel(MSI_SECURITY_LEVEL& eLevel) const throw() = 0;

    // Get the number of authorized MSIs
    virtual SYMRESULT GetAuthorizationCount(ULONG& ulCount) const throw() = 0;

    // Retrieves a specific MSI authorization from the list
    virtual SYMRESULT GetAuthorization(ULONG ulIndex, MSI_AUTH_TYPE& eAuthType, const void*& pAuthorization, ULONG& ulSize) throw() = 0;
};
// {3D6C3701-0614-477d-B9AD-914733B8C92C}
SYM_DEFINE_INTERFACE_ID(IID_BBAuthorizedMSIQueryEvent, 
0x3d6c3701, 0x614, 0x477d, 0xb9, 0xad, 0x91, 0x47, 0x33, 0xb8, 0xc9, 0x2c);

typedef CSymPtr<IBBAuthorizedMSIQueryEvent> IBBAuthorizedMSIQueryEventPtr;
typedef CSymQIPtr<IBBAuthorizedMSIQueryEvent, 
                  &IID_BBAuthorizedMSIQueryEvent> IBBAuthorizedMSIQueryEventQIPtr;


// Add and remove authorized MSIs
class IBBAuthorizedMSISetEvent : public ISymBBSettingsEvent
{
public:
    enum { TypeId = CC_BB_AUTHORIZED_MSI_SET_EVENT };

    // Allows/disallows certain MSI actions
    virtual SYMRESULT SetInstallSecurityLevel(MSI_SECURITY_LEVEL eLevel) throw() = 0;

    // Add an MSI Authorization to the list
    virtual SYMRESULT AddAuthorization(MSI_AUTH_TYPE eAuthType, const void* pAuthData, ULONG ulSize) throw() = 0;

    // Removes an MSI Authorization from the list
    virtual SYMRESULT DeleteAuthorization(MSI_AUTH_TYPE eAuthType, const void* pAuthData, ULONG ulSize) throw() = 0;
};
// {EFDFC51D-955B-4735-A6B5-35E96D70C010}
SYM_DEFINE_INTERFACE_ID(IID_BBAuthorizedMSISetEvent, 
0xefdfc51d, 0x955b, 0x4735, 0xa6, 0xb5, 0x35, 0xe9, 0x6d, 0x70, 0xc0, 0x10);

typedef CSymPtr<IBBAuthorizedMSISetEvent> IBBAuthorizedMSISetEventPtr;
typedef CSymQIPtr<IBBAuthorizedMSISetEvent, 
                  &IID_BBAuthorizedMSISetEvent> IBBAuthorizedMSISetEventQIPtr;

} // SymProtectEvt
