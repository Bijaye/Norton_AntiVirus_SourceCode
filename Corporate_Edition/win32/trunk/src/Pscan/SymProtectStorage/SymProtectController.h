// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#ifndef SYMPROTECT_CONTROLLER
#define SYMPROTECT_CONTROLLER

#ifdef SYMPROTECT_CONTROLLER_CCSUPPORT
	#include "stdafx.h"
	#include "ccEventManagerHelper.h"
	#include "ccSubscriberHelper.h"
	#include "ccEventFactoryHelper.h"
	#include "ccProviderHelper.h"
	#include "SymProtectEventsLoader.h"
#endif
#include "SymProtectEventsInterface.h"
#include "SymProtectEventTypes.h"
#include <list>
#include <string>

// A simplified interface allowing a client to manipulate and control SymProtect as they need and remain oblivious of the Common Client.  This is implemented by writing
// an interface class that does not use Common Client data types and an implementation class that is only exposed if SYMPROTECT_CONTROLLER_CCSUPPORT is defined.
// Consequently, a client of this class can use it in two ways:
//		1.  Directly Use Common Client - simply define SYMPROTECT_CONTROLLER_CCSUPPORT and include this header
//		2.  Isolate Common Client - #include this header, but make sure to add SymProtectController.cpp to your project somewhere.  All Common Client includes, symbols, etc.
//			can be restricted to this one file.
//
// In either case, you will need to do the following to use this class in your project:
//		1.  Include this CPP file in your project.
//		2.  Add the BehaviorBlocking\include path to your INCLUDE path.
//		3.  Include the following Common Client CPP files in your project:
//				ccEventFactoryHelper.cpp
//				ccEventManager.idl
//				ccEventManagerHelper.cpp
//				ccModuleNames.cpp
//				ccMultiEventFactoryHelper.cpp
//				ccProviderHelper.cpp
//				ccSubscriberHelper.cpp
//				ccSymMultiEventFactoryHelper.cpp
//		4.  Instantiate an object.  Either instantiate a CymProtectController, or an ISymProtectController* and call ISymProtectController::Create against that pointer.
//		5.  Call Initialize.
//		6.  <use the object>
//		7.  Call Shutdown.
//		8.  Destroy the object.  Either let it scope out/delete it, or call ISymProtectController::Destroy, depending on how you created it.
class ISymProtectController
{
public:
	// ** DATA TYPES **
	// The following two enums duplicate data types available in the SymProtect headers.
	// The following enums duplicate SP constants and exist to insulate clients of this class from
	// the Common Client.  At present, these constants exist in a header with no CC includes, but this
	// may change.
	typedef std::list<std::string> StringList;
    typedef enum {ProtectionType_Process, ProtectionType_File, ProtectionType_Registry, ProtectionType_NamedObject} ProtectionType;

	// Directory authorizaton
	struct LocationAuthorization
	{
		std::string		pathname;
		bool			isDirectory;		// TRUE if this location is a directory, FALSE if it is a filename
	};
	typedef std::list<LocationAuthorization> LocationAuthorizationList;

	// File contents authorization
	struct HashInfo
	{
		std::string		description;		// Description for user reference
		ULONG			method;				// A SymProtect BB_HASH constant
		BYTE*			hashData;
		DWORD			hashDataSize;
		DWORD			fileSize;
	};
	typedef std::list<HashInfo> HashInfoList;

	struct ProductCode
	{
		GUID			code;
		bool			readOnly;
	};
	typedef std::list<ProductCode> ProductCodeList;

	// Clarifies where errors occured, provided the SymProtectController object was initialized
	typedef enum {	ErrorLocaton_EventFactoryCreate,					// Creating event factory
					ErrorLocation_ProxyFactoryCreate,					// Creating proxy factory
					ErrorLocation_EventManagerCreate,					// Creating event manager
					ErrorLocation_ProviderHelperCreate,					// Creating provider helper
					ErrorLocation_NewEvent,								// Creating new event
					ErrorLocation_ConvertEvent,							// Converting event from CEventPtrEx to actual type
					ErrorLocation_EventInteraction,						// Interacting with the event to send
					ErrorLocation_SendEvent,							// Sending the event across ccEvtMgr (ccEvtMgr's CreateEvent call)
					ErrorLocation_Response_Validate_ConvertEvent,		// Converting the validation response event to a BBSettingsEvent
					ErrorLocation_Response_Validate_GetResult,			// Getting a result code from the validation response event
					ErrorLocation_Response_ConvertEvent,				// Converting the response event to the actual type
					ErrorLocation_Response_EventInteraction,			// Interacting with the response event
					ErrorLocation_Other,								// Some other location
					ErrorLocation_None									// No failure locations supported for this function
	} ErrorLocation;

	// ** FUNCTION MEMBERS **
	// Initialize the controller.  Returns S_OK on success, S_FALSE if already initialized, else the error code of the failure.
	virtual HRESULT Initialize( ErrorLocation* errorLocation = NULL ) = 0;
	// Shuts down the controller.  Returns S_OK on success, S_FALSE if already shutdown, else the error code of the failure.
	virtual HRESULT Shutdown( ErrorLocation* errorLocation = NULL ) = 0;

	// Settings and manifest overall control
	// Ask SymProtect to refresh it's manifests
	virtual HRESULT ManifestRefresh( ErrorLocation* errorLocation = NULL ) = 0;
	// Ask SymProtect to validate it's settings
	virtual HRESULT ValidateSettings( ErrorLocation* errorLocation = NULL ) = 0;
	// Ask SymProtect to force reload all of it's settings
	virtual HRESULT ForceRefreshSettings( ErrorLocation* errorLocation = NULL ) = 0;

	// Overall settings
    virtual HRESULT SetSymProtectAndBehaviorBlockingEnabled( bool protectionEnabled, ErrorLocation* errorLocation = NULL ) = 0;
    virtual HRESULT GetSymProtectAndBehaviorBlockingEnabled( bool* protectionEnabled, ErrorLocation* errorLocation = NULL ) = 0;
    virtual HRESULT SetBBSystemEnabled(bool bEnable, ErrorLocation* errorLocation) = 0;
    virtual HRESULT GetErrorFlag( bool* errorOccured, ErrorLocation* errorLocation = NULL ) = 0;
	virtual HRESULT TemporaryDisable( DWORD noMinutes, ErrorLocation* errorLocation = NULL ) = 0;
	virtual HRESULT IsTemporarillyDisabled( bool* isTemporaryDisable, DWORD* minutesRemaining, ErrorLocation* errorLocation = NULL ) = 0;
	virtual HRESULT CancelTemporaryDisable( ErrorLocation* errorLocation = NULL ) = 0;
	virtual HRESULT GetState( SymProtectEvt::CQueryStateEventEx::SP_QUERY_STATE* currState, ErrorLocation* errorLocation = NULL ) = 0;

	// Protection level settings
	virtual HRESULT SetFileProtection( SymProtectEvt::SP_PROTECTION_LEVEL protectionLevel, ErrorLocation* errorLocation = NULL ) = 0;
	virtual HRESULT SetRegistryProtection( SymProtectEvt::SP_PROTECTION_LEVEL protectionLevel, ErrorLocation* errorLocation = NULL ) = 0;
	virtual HRESULT SetProcessProtection( SymProtectEvt::SP_PROTECTION_LEVEL protectionLevel, ErrorLocation* errorLocation = NULL ) = 0;
    virtual HRESULT SetNamedObjectProtection( SymProtectEvt::SP_PROTECTION_LEVEL protectionLevel, ErrorLocation* errorLocation = NULL ) = 0;
    // Sets all protection levels at once
    virtual HRESULT SetAllProtection( bool spAndBBenabled, SymProtectEvt::SP_PROTECTION_LEVEL fileProtectionLevel, SymProtectEvt::SP_PROTECTION_LEVEL registryProtectionLevel, SymProtectEvt::SP_PROTECTION_LEVEL processProtectionLevel, SymProtectEvt::SP_PROTECTION_LEVEL namedObjectProtectionLevel, ErrorLocation* errorLocation = NULL ) = 0;
	virtual HRESULT GetFileProtection( SymProtectEvt::SP_PROTECTION_LEVEL* protectionLevel, ErrorLocation* errorLocation = NULL ) = 0;
	virtual HRESULT GetRegistryProtection( SymProtectEvt::SP_PROTECTION_LEVEL* protectionLevel, ErrorLocation* errorLocation = NULL ) = 0;
	virtual HRESULT GetProcessProtection( SymProtectEvt::SP_PROTECTION_LEVEL* protectionLevel, ErrorLocation* errorLocation = NULL ) = 0;
    virtual HRESULT GetNamedObjectProtection( SymProtectEvt::SP_PROTECTION_LEVEL* protectionLevel, ErrorLocation* errorLocation = NULL ) = 0;

	// Authorizations
	virtual HRESULT AuthorizationDeviceAdd( ULONG device, ErrorLocation* errorLocation = NULL ) = 0;
	virtual HRESULT AuthorizationDeviceDelete( ULONG device, ErrorLocation* errorLocation = NULL ) = 0;
	virtual HRESULT AuthorizationDeviceGet( ULONG device, bool* authorized, ErrorLocation* errorLocation = NULL ) = 0;
	virtual HRESULT AuthorizationHashAdd( LPCTSTR name, ULONG method, BYTE* hashData, DWORD hashDataSize, DWORD fileSize, ErrorLocation* errorLocation = NULL ) = 0;
	virtual HRESULT AuthorizationHashDelete( LPCTSTR name, ErrorLocation* errorLocation = NULL ) = 0;
	virtual HRESULT AuthorizationHashEnum( HashInfoList* hashes, ErrorLocation* errorLocation = NULL ) = 0;
	virtual HRESULT AuthorizationLocationAdd( LPCTSTR pathname, bool isFile, ErrorLocation* errorLocation = NULL ) = 0;
	virtual HRESULT AuthorizationLocationDelete( LPCTSTR pathname, ErrorLocation* errorLocation = NULL ) = 0;
	virtual HRESULT AuthorizationLocationEnum( LocationAuthorizationList* locations, ErrorLocation* errorLocation = NULL ) = 0;
	virtual HRESULT AuthorizationUserAdd( LPCTSTR accountName, ErrorLocation* errorLocation = NULL ) = 0;
	virtual HRESULT AuthorizationUserDelete( LPCTSTR accountName, ErrorLocation* errorLocation = NULL )= 0;
	virtual HRESULT AuthorizationUserEnum( StringList* accounts, ErrorLocation* errorLocation = NULL ) = 0;

	// Miscellaneous
    // Sets the message communication timeouts to use when communicating with SymProtect via ccEventManager.
    // If no response is received within this timeout, the ccEventMAnager fails the send.
    // defaultTimeout applies to overall control events
	virtual HRESULT SetEventTimeout( DWORD newQueryTimeout, DWORD newSetTimeout, DWORD newDefaultTimeout ) = 0;
	// If provided, sets queryTimeoutValue = the query event timeout and *setTimeoutValue = the timeout for
	// modification events, *defaultTimeoutValue equal to the timeout for all other events
	virtual HRESULT GetEventTimeout( DWORD* queryTimeoutValue, DWORD* setTimeoutValue, DWORD* defaultTimeoutValue ) = 0;

	// Miscellaneous statics
	// Creates a new SymProtectController object
	static HRESULT Create( ISymProtectController** newObject );
	// Destroys a SymProtectController object
	static HRESULT Destroy( ISymProtectController** targetObject );
	// Returns S_OK if actionType equals the value of a valid SP_ACTION_TYPE enum, else E_INVALIDARG
	static HRESULT IsValidActionType( DWORD actionType );
	// Returns S_OK if actionType equals the value of a valid SP_PROTECTION_LEVEL enum, else E_INVALIDARG
	static HRESULT IsValidProtectionLevel( DWORD protectionLevel );
	// Returns S_OK if actionType equals the value of a valid BB_DEVICE constant, else E_INVALIDARG
	static HRESULT IsValidDeviceID( DWORD deviceID );
	// Returns S_OK if hashMethod equals the value of a valid BB_HASH constant, else E_INVALIDARG
	static HRESULT IsValidHashMethod( DWORD hashMethod );
	// Returns a string name of the specified error location for debug logging
    static LPCTSTR ErrorLocationToString( ISymProtectController::ErrorLocation location );
    // Sets *spProtectionType equal to the protection type corresponding to the specified SymProtect event action type code
    // Returns S_OK on success, E_INVALIDARG if actionCode not recognized
    static HRESULT GetActionProtectionType( DWORD actionCode, ProtectionType* spProtectionType);
};

#ifdef SYMPROTECT_CONTROLLER_CCSUPPORT
	// Implements ISymProtectController
	class CSymProtectController : public ISymProtectController
	{
	public:
		// ** FUNCTION MEMBERS **
		// Initialize the controller.  Returns S_OK on success, S_FALSE if already initialized, else the error code of the failure.
		virtual HRESULT Initialize( ErrorLocation* errorLocation = NULL );
		// Shuts down the controller.  Returns S_OK on success, S_FALSE if already shutdown, else the error code of the failure.
		virtual HRESULT Shutdown( ErrorLocation* errorLocation = NULL );

		// Settings and manifest overall control
		// Ask SymProtect to refresh it's manifests
		virtual HRESULT ManifestRefresh( ErrorLocation* errorLocation = NULL );
		// Ask SymProtect to validate it's settings
		virtual HRESULT ValidateSettings( ErrorLocation* errorLocation = NULL );
		// Ask SymProtect to force reload all of it's settings
		virtual HRESULT ForceRefreshSettings( ErrorLocation* errorLocation = NULL );

		// Overall settings
        virtual HRESULT SetSymProtectAndBehaviorBlockingEnabled( bool protectionEnabled, ErrorLocation* errorLocation = NULL );
        virtual HRESULT GetSymProtectAndBehaviorBlockingEnabled( bool* protectionEnabled, ErrorLocation* errorLocation = NULL );
        virtual HRESULT SetBBSystemEnabled(bool bEnable, ErrorLocation* errorLocation);
        virtual HRESULT GetErrorFlag( bool* errorOccured, ErrorLocation* errorLocation = NULL );
		virtual HRESULT TemporaryDisable( DWORD noMinutes, ErrorLocation* errorLocation = NULL );
		virtual HRESULT IsTemporarillyDisabled( bool* isTemporaryDisable, DWORD* minutesRemaining, ErrorLocation* errorLocation = NULL );
		virtual HRESULT CancelTemporaryDisable( ErrorLocation* errorLocation = NULL );
        virtual HRESULT GetState( SymProtectEvt::CQueryStateEventEx::SP_QUERY_STATE* currState, ErrorLocation* errorLocation = NULL );

		// Protection level settings
		virtual HRESULT SetFileProtection( SymProtectEvt::SP_PROTECTION_LEVEL protectionLevel, ErrorLocation* errorLocation = NULL );
		virtual HRESULT SetRegistryProtection( SymProtectEvt::SP_PROTECTION_LEVEL protectionLevel, ErrorLocation* errorLocation = NULL );
		virtual HRESULT SetProcessProtection( SymProtectEvt::SP_PROTECTION_LEVEL protectionLevel, ErrorLocation* errorLocation = NULL );
        virtual HRESULT SetNamedObjectProtection( SymProtectEvt::SP_PROTECTION_LEVEL protectionLevel, ErrorLocation* errorLocation = NULL );
        virtual HRESULT SetAllProtection( bool spAndBBenabled, SymProtectEvt::SP_PROTECTION_LEVEL fileProtectionLevel, SymProtectEvt::SP_PROTECTION_LEVEL registryProtectionLevel, SymProtectEvt::SP_PROTECTION_LEVEL processProtectionLevel, SymProtectEvt::SP_PROTECTION_LEVEL namedObjectProtectionLevel, ErrorLocation* errorLocation );
		virtual HRESULT GetFileProtection( SymProtectEvt::SP_PROTECTION_LEVEL* protectionLevel, ErrorLocation* errorLocation = NULL );
		virtual HRESULT GetRegistryProtection( SymProtectEvt::SP_PROTECTION_LEVEL* protectionLevel, ErrorLocation* errorLocation = NULL );
		virtual HRESULT GetProcessProtection( SymProtectEvt::SP_PROTECTION_LEVEL* protectionLevel, ErrorLocation* errorLocation = NULL );
        virtual HRESULT GetNamedObjectProtection( SymProtectEvt::SP_PROTECTION_LEVEL* protectionLevel, ErrorLocation* errorLocation = NULL );

		// Authorizations
		virtual HRESULT AuthorizationDeviceAdd( ULONG device, ErrorLocation* errorLocation = NULL );
		virtual HRESULT AuthorizationDeviceDelete( ULONG device, ErrorLocation* errorLocation = NULL );
		virtual HRESULT AuthorizationDeviceGet( ULONG device, bool* authorized, ErrorLocation* errorLocation = NULL );
		virtual HRESULT AuthorizationHashAdd( LPCTSTR name, ULONG method, BYTE* hashData, DWORD hashDataSize, DWORD fileSize, ErrorLocation* errorLocation = NULL );
		virtual HRESULT AuthorizationHashDelete( LPCTSTR name, ErrorLocation* errorLocation = NULL );
		virtual HRESULT AuthorizationHashEnum( HashInfoList* hashes, ErrorLocation* errorLocation = NULL );
		virtual HRESULT AuthorizationLocationAdd( LPCTSTR pathname, bool isFile, ErrorLocation* errorLocation = NULL );
		virtual HRESULT AuthorizationLocationDelete( LPCTSTR pathname, ErrorLocation* errorLocation = NULL );
		virtual HRESULT AuthorizationLocationEnum( LocationAuthorizationList* locations, ErrorLocation* errorLocation = NULL );
		virtual HRESULT AuthorizationUserAdd( LPCTSTR accountName, ErrorLocation* errorLocation = NULL );
		virtual HRESULT AuthorizationUserDelete( LPCTSTR accountName, ErrorLocation* errorLocation = NULL );
		virtual HRESULT AuthorizationUserEnum( StringList* accounts, ErrorLocation* errorLocation = NULL );

		// Constructor-destructor
		CSymProtectController();
		virtual ~CSymProtectController();

		// Miscellaneous
		virtual HRESULT SetEventTimeout( DWORD newQueryTimeout, DWORD newSetTimeout, DWORD newDefaultTimeout );
		virtual HRESULT GetEventTimeout( DWORD* queryTimeoutValue, DWORD* setTimeoutValue, DWORD* defaultTimeoutValue );
	private:
		// ** FUNCTION MEMBERS **
		// Sends an event to SymProtect and waits for a response
		// On success, sets *eventResponse equal to a copy of the response event, *responseEventType to the type ID of the response event.
		// If validateResponse is TRUE, looks for an ISymBBSettingsEvent-compatible response event and verifies that the error code is != UNRECOVERABLE_ERROR
		// Returns S_OK on success.  If validateResponse is TRUE, returns S_OK if an ISymBBSettingsEvent-compatible response event is received and the error code is != UNRECOVERABLE_ERROR
		// Returns S_FALSE if validateResponse is TRUE, an ISymBBSettingsEvent-compatible response event is received, and the error code is == UNRECOVERABLE_ERROR
		HRESULT SendEvent( ccEvtMgr::CEventExPtr eventToSend, ccEvtMgr::CEventEx** eventResponse = NULL, bool validateResponse = false, long* responseEventType = NULL, DWORD timeout = 3000, ErrorLocation* errorLocation = NULL );
		// Sends a new event object of the specified type to SymProtect, no response wait
		HRESULT SendEvent( long eventID, DWORD timeout = 3000, ErrorLocation* errorLocation = NULL );
        // Sends a QueryClass-type event to SymProtect and returns a response of the same type
        // iid is the interface ID corresponding to QueryClass.  This is typically a constant named IID_QueryClass
        // Returns S_OK on success, else the error code of the failure.
        template<class QueryClass, const SYMGUID& iid> HRESULT SendQueryEvent( QueryClass** queryResponse, ErrorLocation* errorLocation = NULL );
		// Sends a IBBSymProtectQueryConfigEvent event to SymProtect and gets the response
		// Returns S_OK on success, else the error code of the failure.
		HRESULT SendQueryConfigEvent( SymProtectEvt::IBBSymProtectQueryConfigEvent** queryResponse, ErrorLocation* errorLocation = NULL );
        // Sends a IBBSymProtectQueryConfigEvent2 event to SymProtect and gets the response
        // Returns S_OK on success, else the error code of the failure.
        HRESULT SendQueryConfigEvent2( SymProtectEvt::IBBSymProtectQueryConfigEvent2** queryResponse, ErrorLocation* errorLocation = NULL );
        // Sends the BB system enabled event
        HRESULT SendSPEvent( ccEvtMgr::CEventExPtr pEvent, ccEvtMgr::CEventEx** ppReturnEvent, ErrorLocation* errorLocation );
        
		// ** HELPERS **
		// Translates a SymResult code to an equivalent Windows HRESULT
		static HRESULT SYMRESULT_TO_HRESULT( SYMRESULT originalCode );
		// Translates a CError code to an equivalent Windows HRESULT
		static HRESULT CERROR_TO_HRESULT( ccEvtMgr::CError::ErrorType originalCode );

		// ** CONSTANTS **
		// Default timeout for messages sent via ccEvtMgr
		static const DWORD									DefaultTimeout = 10000;

		// ** DATA MEMBERS **
		ccEvtMgr::CEventFactoryEx2Ptr						eventFactory;
		SymProtectEvt::SymProtectEvents_CEventFactoryEx2	eventFactoryLoader;
		ccEvtMgr::CProxyFactoryExPtr						proxyFactory;
		SymProtectEvt::SymProtectEvents_CProxyFactoryEx		proxyFactoryLoader;
        CSavStaticRefCount<ccEvtMgr::CEventManagerHelper>   eventManager;
        CSavStaticRefCount<ccEvtMgr::CProviderHelper>       providerHelper;
		bool												initialized;			// Set to TRUE when initialized, else FALSE
		DWORD												timeoutQuery;			// Timeout for query events
		DWORD												timeoutSet;				// Timeout for set events
		DWORD												timeoutDefault;			// Timeout for all other events
	};
#endif // SYMPROTECT_CONTROLLER_CCSUPPORT

#endif // SYMPROTECT_CONTROLLER