// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#ifndef SYMPROTECT_STORAGE
#define SYMPROTECT_STORAGE

#include <windows.h>
#include <list>
#include <string>
#include "SymProtectEventTypes.h"
#include "CRTVScanStorage.h"
#include "SymProtectStorage.h"
#include "CsynchronizedList.h"
#include "CSymProtectEventSubscriber.h"
#include "SymProtectController.h"
#include "Authorization.h"
#include "SymProtectEventInfo.h"

// A storage that receives events from the Common Client framework and redirects them into RTVScan for further processing
class CSymProtectStorage : public CRTVScanStorage
{
public:
    // ** FUNCTION MEMBERS **
	// Storage Extension Functions (SFUNCTIONS implementations)
	virtual DWORD GetConfig( StorageConfig* config );
	virtual DWORD Initialize( void );
	virtual DWORD DeInitialize( void );

	virtual DWORD BeginRTSWatch( PROCESSRTSNODE ProcessRTSNode, PSNODE currStorageNode, void* context );
	virtual DWORD StopRTSWatches();
	virtual DWORD ProcessPacket( WORD functionCode, BYTE *sendBuffer, DWORD sendBufferSize, BYTE *replyBuffer, DWORD *replyBufferSize, CBA_Addr *address );
	virtual DWORD FormatMessage( char *resultBuffer, size_t nNumResultBufBytes, const char *messageFormat, PEVENTBLOCK thisEvent, DWORD userParam );
	virtual DWORD ReloadRTSConfig();

	virtual DWORD NodeFunction_Release( PSNODE thisNode );
	virtual DWORD NodeFunction_HasViruses( PSNODE pNode, PEVENTBLOCK pEventBlock );
	virtual DWORD NodeFunction_Rename( PSNODE pNode, char* newExt );
	virtual DWORD NodeFunction_Remove( PSNODE pNode );

	//Returns the name of the specified file or registry entry given a full pathname to that entity
	HRESULT GetNodeName( std::string pathname, std::string* nodeName );
	// Sets the DLL module handle
	static void SetModuleHandle( HINSTANCE newModuleHandle );
	static HRESULT InitResources();

	// Constructor-destructor
	CSymProtectStorage();
	virtual ~CSymProtectStorage();

private:
	// ** DATA TYPES **
    typedef std::list<SymProtectEvt::SP_ACTION_TYPE> SPActionTypeList;
    // Criteria for an exact filter, where actor, event, and target are all matched
	struct FilterCriteria
	{
        bool                            matchActor;         // TRUE if actor should be included in match criteria, else FALSE
		std::string						actorFilename;
        bool                            actorIsPathname;    // TRUE if actorFilename is a full pathname (e.g., c:\windows\notepad.exe), FALSE if file node name (e.g., notepad.exe)
        SPActionTypeList                eventTypes;
		std::string						targetPathname;
        bool                            matchTarget;        // TRUE if targetPathname should also be matched for this rule, else FALSE
	};
	typedef std::list<FilterCriteria> FilterCriteriaList;
    // The current SymProtect configuration settings
    struct Configuration
    {
        FilterCriteriaList                                  filterList;
        std::string                                         filterExplorerPathname;
        std::string                                         filterWinLogonPathname;
        std::string                                         filterWinMgmtPathname;
        std::string                                         filterWinMgmtAdapPathname;
        std::string                                         filterDSentryPathname;
        std::string                                         filterCtfmonPathname;
        std::string                                         filterCsrssPathname;
        std::string                                         filterBlastCleanPathname;
    };
    #define S_TIMEOUT       MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_ITF, 1)

	// ** FUNCTION MEMBERS **
	// Performs SFUNCTION_PROCESSPACKET_FUNCTIONCODE_GETEVENTBLOCK
	DWORD ProcessPacket_GetEventBlock( WORD functionCode, BYTE *sendBuffer, DWORD sendBufferSize, BYTE *replyBuffer, DWORD *replyBufferSize, CBA_Addr *address );
	// Performs SFUNCTION_PROCESSPACKET_FUNCTIONCODE_GETLOCATIONSTRING
	DWORD ProcessPacket_GetLocationString( WORD functionCode, BYTE *sendBuffer, DWORD sendBufferSize, BYTE *replyBuffer, DWORD *replyBufferSize, CBA_Addr *address );
	// Performs SFUNCTION_PROCESSPACKET_FUNCTIONCODE_GETACTIONSTRING
	DWORD ProcessPacket_GetActionString( WORD functionCode, BYTE *sendBuffer, DWORD sendBufferSize, BYTE *replyBuffer, DWORD *replyBufferSize, CBA_Addr *address );
	// SFUNCTION_PROCESSPACKET_FUNCTIONCODE_GETFULLLOGGERSTRING
	DWORD ProcessPacket_GetFullLoggerString( WORD functionCode, BYTE *sendBuffer, DWORD sendBufferSize, BYTE *replyBuffer, DWORD *replyBufferSize, CBA_Addr *address );
	// SFUNCTION_PROCESSPACKET_FUNCTIONCODE_GETEVENTNAME
	DWORD ProcessPacket_GetEventName( WORD functionCode, BYTE *sendBuffer, DWORD sendBufferSize, BYTE *replyBuffer, DWORD *replyBufferSize, CBA_Addr *address );
    // SFUNCTION_PROCESSPACKET_FUNCTIONCODE_GETDISPLAYALERTFOREVENT
    DWORD ProcessPacket_GetDisplayAlertForEvent( WORD functionCode, BYTE *sendBuffer, DWORD sendBufferSize, BYTE *replyBuffer, DWORD *replyBufferSize, CBA_Addr *address );

    // Creates commonClientSubscriber and connects it.
    HRESULT SubscriberInitialize( void );
    // Disconnects commonClientSubscriber and deletes it.
    HRESULT SubscriberDeInitialize( void );
    
	// Imports SymProtect settings from the SAVCorp registry into SymProtect
	// Calling thread MUST have called CoInitialize with multithreaded apartment model!
	// Returns S_OK on success, else the error code of the first failure.  Always attempts to import all settings as directed, regardless of number of errors.
	HRESULT ImportSettings( void );
	// Filters out certain standard innocuous system events.
	// Returns TRUE if event should be filtered out, else FALSE
	bool ExcludeEvent( SymProtectEventInfo& eventInfo );
    // Ensures that all needed dependant services are running
    // Returns S_OK if they are, S_FALSE if not yet ready to, else the error code of the failure
    HRESULT EnsureServiceDependanciesRunning( void );
    // Determines if all the settings event manager needs to start up correctly have been written out to ccSettings
    // Returns S_OK if event manager is good to go, S_FALSE if not but timeout from 1st check time hasn't elapsed, S_TIMEOUT if not and timeout has elapsed,
    // else the error code of the failure.
    HRESULT IsEventManagerConfigured( DWORD timeout );
	// Ensures serviceName is running
	// Returns S_OK if service was started, S_FALSE if it is already running, HFW32(WAIT_TIMEOUT) if timed out before finished starting, else the error code of the failure
	HRESULT EnsureServiceRunning( LPCTSTR serviceName, DWORD timeout = 10000 );
    // Returns S_OK if service serviceName is running, S_FALSE if not
    HRESULT IsServiceRunning( LPCTSTR serviceName, DWORD timeOut = 0 );
    // Handles an authorization requested by an install or other component.  The action taken is determined by the
    // value of valueName, which must be an AuthorizationRequest enum value.  Logs failure.
    // filenameToAuthorizeDeterminedResult should be ERROR_SUCCESS if filenameToAuthorize was determined OK, otherwise the error that occured while determining it
    HRESULT HandleRequestedAuthorization( HKEY settingsKeyHandle, LPCTSTR valueName, LPCTSTR filenameToAuthorize, DWORD filenameToAuthorizeDeterminedResult, ISymProtectController* spController );
    // Ensures location is in the SymProtect location authorization list.
    // Returns S_OK if authorization added, S_FALSE if already present, else the error code of the failure
    HRESULT AddLocationAuthorization( const std::string& location, const std::string& locationLogDescription, ISymProtectController* spController );
    // Removes location from the SymProtect location authorization list.
    // Returns S_OK if location was removed, S_FALSE is not present to begin with, else the error code of the failure
    HRESULT RemoveLocationAuthorization( const std::string& location, const std::string& locationLogDescription, ISymProtectController* spController );

	// Returns TRUE if running on Windows NT, else FALSE
	static bool IsWinNT( void );
	// Returns TRUE if running on Windows 2000, else FALSE
	static bool IsWin2000( void );
	// Sets *ownerSID equal to a copy of the SID for the user account that processID is logged in under
	HRESULT GetProcessOwner( DWORD processID, PSID ownerSID, DWORD ownerSIDsize );
	// Returns the user descriptive name for the specified action code
	static HRESULT GetActionDescriptions( SymProtectEvt::SP_ACTION_TYPE actionCode, std::string* actionName, std::string* entityType );
	// Returns a description of the action taken for this event
	static std::string GetActionTakenString( SymProtectEventInfo& eventInfo );
    // Loads a string resource and sets *stringBuffer equal to it as an STL string
    static HRESULT LoadString( UINT resourceID, std::string* stringBuffer );
	// Reads in a string value from the registry
	DWORD GetStr( HKEY key, LPCTSTR valueName, LPTSTR valueData, DWORD valueDataSize, LPCTSTR defaultValueData );
	// Reads in a DWORD value from the registry
	DWORD GetVal( HKEY key, LPCTSTR valueName, DWORD defaultValue );
	// Sets *authorizationsOfType equal to an in-order list of all authorizations members whose typeid is typeToSelect
	// Returns S_OK on success, else the error code of the failure
	HRESULT AuthorizationListFilterByType( AuthorizationList authorizations, const type_info& typeToSelect, AuthorizationList* authorizationsOfType );

	// ** CONSTANTS **
	// NOTE:  While these letters are not currently used by any other storage, they are outside of the general set and so are storage-specific.
	static const char Replacement_Token_ActionTaken					= 'G';
	static const char Replacement_Token_ActionName					= 'H';
	static const char Replacement_Token_EntityType					= 'J';
	static const char Replacement_Token_ActorPID					= 'K';
	static const char Replacement_Token_ActorProcessName			= 'M';
	static const char Replacement_Token_TargetPathname				= 'Q';
	static const char Replacement_Token_TargetPID					= 'W';
	static const char Replacement_Token_TargetTerminalSessionID		= 'X';
	static const DWORD DefaultEventTimeout							= 60000;
    static const LPCTSTR FilterExecutable_XTAgent;
	static const LPCTSTR FilterExecutable_NalAgent;
    static const LPCTSTR FilterExecutable_WMIadap;
    static const LPCTSTR FilterExecutable_WMIapsvr;
    static const LPCTSTR FilterExecutable_WMIwmic;
    static const LPCTSTR FilterExecutable_WMIprvse;
    static const LPCTSTR FilterExecutable_BlastCln;
    static const LPCTSTR FilterExecuteable_MicrosoftUpdate;
    static const LPCTSTR FilterExecutable_Accessibility_Narrator;
    static const LPCTSTR FilterExecutable_Accessibility_Magnify;
    static const LPCTSTR DefaultAuthorization_DrWatson;
    static const LPCTSTR DefaultAuthorization_DumpPrep;
    static const LPCTSTR DefaultAuthorization_CSRSS_Prefix;				// Required for Dr. Watson
    static const LPCTSTR DefaultAuthorization_CSRSS_Filename;			// Required for Dr. Watson
    static const LPCTSTR DefaultAuthorization_SMSS_Prefix;				// Required for Dr. Watson
    static const LPCTSTR DefaultAuthorization_SMSS_Filename;			// Required for Dr. Watson
	static const LPCTSTR WriteCcSettingsDoneFlagKey;

	// ** DATA MEMBERS **
	CSynchronizedList<ccEvtMgr::CEventEx*>*				eventList;
	CSymProtectEventSubscriber*							commonClientSubscriber;
	static HINSTANCE									moduleHandle;               // DLL module handle
	static HINSTANCE									resHandle;                  // DLL Resource handle
	HANDLE												rtsThreadInterrupt;         // Set when the storage RTS thread should interrupted
    bool                                                runRTSwatch;                // TRUE if storage RTS thread is supposed to be running, else FALSE
    CRITICAL_SECTION                                    importSettingsLock;         // Prevents ImportSettings from being called twice simultaneously
    CRITICAL_SECTION                                    configLock;                 // Serializes access to config
    Configuration                                       config;
};

#endif // SYMPROTECT_STORAGE