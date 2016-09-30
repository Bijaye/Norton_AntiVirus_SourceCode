// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// AvisEventDetection.h
//
// IPE DO NOT TRANSLATE ANY STRINGS IN THIS FILE.
// ALL ARE PROGRAMMING TOKENS AND NOT SEEN BY THE CUSTOMER.
//

#ifndef _AVISEVENTDETECT_H_INCLUDED
#define	_AVISEVENTDETECT_H_INCLUDED


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "qsregval.h"
#include "qsfields.h"
#include "avisregval.h"
#include "IcePackTokens.h"



#ifndef  STRTOUL
    //#define STRTOUL(n,e,b)      wcstoul(n,e,b)
    #define STRTOUL(n,e,b)        _tcstoul(n,e,b)
#endif



// ID THE EVENT RULES TABLE TO USE
#define EVENT_TOKEN_GENERAL_ATTENTION_TABLE    1
#define EVENT_TOKEN_SAMPLE_INTERVENTION_TABLE  2


// GatewayQueryAlert          
#define GENERAL_ATTENTION_GATEWAY_QUERY_ALERT_EVENTID       1001
#define GENERAL_ATTENTION_GATEWAY_QUERY_ALERT_TIMEOUT_SECS   500 
#define GENERAL_ATTENTION_GATEWAY_QUERY_TOKEN_LIST          _T("download,query,submit")
// General Attention
#define GENERAL_ATTENTION_DEFCAST_EVENTID                1002
#define GENERAL_ATTENTION_DISK_EVENTID                   1003
#define GENERAL_ATTENTION_SCANEXPLICIT_EVENTID           1004
#define GENERAL_ATTENTION_TARGET_EVENTID                 1005
#define GENERAL_ATTENTION_SHUTDOWN_EVENTID               1007
#define GENERAL_ATTENTION_UNAVAIABLE_EVENTID             1008
#define GENERAL_ATTENTION_GATEWAY_COMM_EVENTID           GENERAL_ATTENTION_GATEWAY_QUERY_ALERT_EVENTID
#define GENERAL_ATTENTION_SEND_TEST_EVENT                1093
#define GENERAL_ATTENTION_DISKQUOTA_LOW_WATER            1094
#define GENERAL_ATTENTION_DISKSPACE_LOW_WATER            1095
#define GENERAL_ATTENTION_CENTRAL_QUARANTINE_FULL        1096


// SampleInterventionAlert
#define SAMPLE_ALERT_HELD_EVENTID              2001
#define SAMPLE_ALERT_AVAILABLE_EVENTID         2002
#define SAMPLE_ALERT_ATTENTION_EVENTID         2003
#define SAMPLE_ALERT_ERROR_EVENTID             2004
#define SAMPLE_ALERT_NOT_INSTALLED_EVENTID     2005
#define SAMPLE_ALERT_QUARANTINED_EVENTID       2007
#define SAMPLE_ALERT_SUBMITTED_EVENTID         2008
#define SAMPLE_ALERT_RELEASED_EVENTID          2019
#define SAMPLE_ALERT_NEEDED_EVENTID            2010
#define SAMPLE_ALERT_DISTRIBUTED_EVENTID       2011
#define SAMPLE_ALERT_DISTRIBUTE_EVENTID        2012
//
#define SAMPLE_ALERT_QUARANTINE_NOT_REPAIRED_EVENT_ID 1019

#define SAMPLE_ALERT_ALERT_TIMEOUT_SECS        500 
#define SAMPLE_ALERT_TOKEN_LIST                _T("held,attention,error,notinstalled")
#define SAMPLE_ALERT_TIMEINSTATE_TOKEN_LIST    _T("distribute")
#define SAMPLE_ALERT_STATUS_TIMER_TOKEN_LIST   _T("quarantined,submitted,released,needed,available,distributed,distribute")
//#define SAMPLE_ALERT_STATUS_TOKEN_LIST       _T("quarantined,submitted,held,released,needed,available,distributed,attention,error,notinstalled,distribute")


// SampleInterventionAlert "X-Error" alerts
#define SAMPLE_ALERT_ABANDONED_EVENTID         2031
#define SAMPLE_ALERT_CONTENT_EVENTID           2032
#define SAMPLE_ALERT_CRUMBLED_EVENTID          2033
#define SAMPLE_ALERT_DECLINED_EVENTID          2034
#define SAMPLE_ALERT_INTERNAL_EVENTID          2035
#define SAMPLE_ALERT_LOST_EVENTID              2036
#define SAMPLE_ALERT_MALFORMED_EVENTID         2037
#define SAMPLE_ALERT_MISSING_EVENTID           2038
#define SAMPLE_ALERT_OVERRUN_EVENTID           2039
#define SAMPLE_ALERT_SAMPLE_EVENTID            2040
#define SAMPLE_ALERT_SUPERCEDED_EVENTID        2041
#define SAMPLE_ALERT_TYPE_EVENTID              2042
#define SAMPLE_ALERT_UNAVAILABLE_EVENTID       2043
#define SAMPLE_ALERT_UNDERRUN_EVENTID          2044
#define SAMPLE_ALERT_UNPACKAGE_EVENTID         2045
#define SAMPLE_ALERT_UNPUBLISHED_EVENTID       2047


// DEFINITION ALERTS
#define DEF_ALERT_NOT_INSTALLED_EVENTID        SAMPLE_ALERT_NOT_INSTALLED_EVENTID  
#define DEF_ALERT_NEW_BLESSED_DEFS_EVENTID     3001
#define DEF_ALERT_NEW_UNBLESSED_DEFS_EVENTID   3002
#define DEF_ALERT_INSTATE_NEEDED_EVENTID       SAMPLE_ALERT_NEEDED_EVENTID     
#define DEF_ALERT_INSTATE_DISTRIBUTED_EVENTID  SAMPLE_ALERT_DISTRIBUTED_EVENTID
#define DEF_ALERT_INSTATE_DISTRIBUTE_EVENTID   SAMPLE_ALERT_DISTRIBUTE_EVENTID 
//#define DEF_ALERT_INSTATE_DISTRIBUTE_EVENTID 3003


// Local Quarantine Interventon Alert
#define QUARANTINE_ALERT_STATUS_TOKENS         _T("unneeded,installed")
#define QUARANTINE_ALERT_RESULT_TOKENS         _T("repaired")
#define QUARANTINE_SAMPLE_STATE_TOKENS		   _T("uninfected,encrypted,restore,delete,compressed,unrepairable")
#define QUARANTINE_ALERT_NOT_REPAIRED_EVENTID  4001




#define EVENT_GROUP_GENERAL_ERROR              1
#define EVENT_GROUP_GATEWAY_COMM_ERROR         2
#define EVENT_GROUP_SAMPLE_ERROR               3
#define EVENT_GROUP_DEFINITION_ERROR           4
#define EVENT_GROUP_NEW_DEF_ARRIVAL            5
#define EVENT_GROUP_GENERAL_INFORMATIONAL      7
#define EVENT_GROUP_DISK_WARNING               8
//#define EVENT_GROUP_TIMEINSTATE_ERROR          7  

// VALUES FOR dwAlertSeverityLevel
#define _SEVERITY_CRITICAL		    1
#define _SEVERITY_MAJOR			    2
#define _SEVERITY_MINOR			    3
#define _SEVERITY_INFORMATIONAL	    4
#define _SEVERITY_UNKNOWN		    0

// VALUES FOR dwOperationalStatusLevel
#define OP_STATUS_OPERATIONAL       1
#define OP_STATUS_DEGRADED          2
#define OP_STATUS_NON_OPERATIONAL   3
#define OP_STATUS_UNKNOWN           0


typedef struct
{
   DWORD    dwEnableGeneralErrorEvents;
   DWORD    dwEnableSampleErrorEvents;
   DWORD    dwEnableDefinitionErrorEvents;
   DWORD    dwEnableNewDefArrivalEvents;

}  AVIS_GENERAL_CONFIGURATION;



typedef struct
{
    TCHAR*      lpszIcePackToken;
    DWORD       dwIsConfigurable;             // Can the user configure this one?
	DWORD	    dwEnabled;                    // Is this event enabled?
    DWORD       dwIsConfigMinTimeInState;     // Can the user set dwMinTimeInStateSecs
    DWORD       dwMinTimeInStateMinutes;      // Save in min to the Registry
    DWORD       dwMinTimeInStateSecs;         // Is there a min interval? 0== immediate
    DWORD       dwInStateTimerExpire;         // Stop time for the timer
	BOOL	    bIsDataChanged;               // Has the record been modified?
	DWORD	    dwEventGroupType;
    DWORD       dwEventId;
	DWORD	    dwAlertSeverityLevel;         // Severity level of the alert. Table is listed below.
    DWORD	    dwOperationalStatusLevel;     // Resulting operational status of the application. 
    DWORD       dwIcePackTokenTableID;        // Which IcePack token table to use
    WCHAR*      lpszDefaultMessage;
    LPCTSTR     lpszRegValue;                 // Registry value name for this event configuration
    WCHAR*      lpszEventName;                // Name of the event
    DWORD       dwDefaultMessageID;           // Res ID of string in IcePackTokens.dll
    DWORD       dwEventNameID;                // Res ID of string in IcePackTokens.dll
    //DWORD       dwChildEventNameID;         // For overall Sample Errors, this is the actual error. Res ID in IcePackTokens.dll
    DWORD       dwNoteTextID;                 // ID of the text for the sNote field. Res ID of string in IcePackTokens.dll

}  AVIS_EVENT_RULES;



class CAlertEvent  
{
public:
	CAlertEvent()
    {
        ClearEventData();
    };

	//virtual ~CAlertEvent();
	~CAlertEvent()
    {
    };

    BOOL ClearEventData()
    {
        lpEventRuleEntry          = 0;
        dwEventGroupType          = 0;
        dwEventId                 = 0;
        dwAlertSeverityLevel      = 0;
        dwOperationalStatusLevel  = 0;
        dwEventRulesTableID       = 0;
        dwIcePackTokenTableID     = 0;
        lpEventRuleEntry          = NULL;

        sIcePackTokenString       = _T(""); 
        sIcePackParentTokenString = _T(""); 
        sMessage                  = _T(""); 
        sUser                     = _T(""); 
        sMachineName              = _T(""); 
        sDNSname                  = _T("");
        sMachineAddress           = _T(""); 
        sPlatformName             = _T(""); 
        sFileName                 = _T(""); 
        sVirusName                = _T(""); 
        sDefinitions              = _T(""); 
        sReason                   = _T(""); 
        sAgeOfSample              = _T(""); 
        sDefaultOrHelpMsg         = _T(""); 
        sErrorText                = _T("");
        sLogFileText              = _T("");
        sEventName                = _T("");
        sSampleStatus             = _T("");
        m_sSampleState            = _T("");
        m_sNote                   = _T("");

        // GET THE LOCAL TIME 
        GetLocalTime(&EventTime);
        // GetSystemTime(&EventTime);   // UTC TIME
        return(TRUE);
    }


	DWORD	    dwEventGroupType;
    DWORD       dwEventId;
	DWORD	    dwAlertSeverityLevel;         //  Severity level of the alert. Table is listed below.
    DWORD	    dwOperationalStatusLevel;     //  Resulting operational status of the application. 
    DWORD       dwEventRulesTableID;          //  which rules table? g_AvisEventRulesGeneralAttention or g_AvisEventRulesSampleIntervention
    DWORD       dwIcePackTokenTableID;
    CString     sIcePackTokenString;          // Token recieved from IcePack
    CString     sIcePackParentTokenString;    // Token of the parent for grouped events
    CString     sMessage; 
    CString     sUser; 
    CString     sMachineName; 
    CString     sDNSname; 
    CString     sMachineAddress; 
    CString     sPlatformName; 
    CString     sFileName; 
    CString     sVirusName; 
    CString     sDefinitions;
    CString     sReason; 
    CString     sAgeOfSample; 
    CString     sDefaultOrHelpMsg; 
    CString     sErrorText; 
    CString     sLogFileText;
    CString     sEventName;
    CString     sSampleStatus;
    CString     m_sSampleState;
    CString     m_sNote;

    SYSTEMTIME  EventTime; 
    AVIS_EVENT_RULES*  lpEventRuleEntry;
};


// GLOBAL DATA
AVIS_EVENT_RULES g_AvisEventRulesGeneralAttention[];
AVIS_EVENT_RULES g_AvisEventRulesSampleIntervention[];



// ICEPACK CURRENT KEY
const TCHAR REGKEY_QSERVER_AVIS_CURRENT[] = _T( "Software\\Symantec\\Quarantine\\Server\\Avis\\current" );

// AVIS CURRENT KEY
const TCHAR REGKEY_QSERVER_AVIS[] = _T( "Software\\Symantec\\Quarantine\\Server\\Avis" );



#define REGVALUE_ALERT_INTERVAL                            REGVALUE_ALERTINTERVAL   // _T( "alertInterval" );
const TCHAR REGVALUE_ALERT_ICEPACK_ATTENTION_LAST[]            = _T( "alertIcepackAttentionLast" );
const TCHAR REGVALUE_ALERT_ICEPACK_DATE_LAST[]                 = _T( "alertIcepackDateLast" );
const TCHAR REGVALUE_ALERT_ICEPACK_ATTENTION_LAST_ALERT[]      = _T( "alertIcepackAttentionLastAlert" );
const TCHAR REGVALUE_ALERT_ICEPACK_ATTENTION_LAST_ALERT_DATE[] = _T( "alertIcepackAttentionLastAlertDate" );
// NEW DEF ARRIVED
const TCHAR REGVALUE_DEFS_IS_ACTIVE_BLESSED[]              = _T( "definitionActiveBlessed" );
const TCHAR REGVALUE_DEFS_ACTIVE_SEQUENCE_NUM_LAST[]       = _T( "definitionActiveSequenceLast" );
const TCHAR REGVALUE_DEFS_ACTIVE_BLESSED_LAST[]            = _T( "definitionActiveBlessedLast" );
const TCHAR REGVALUE_DEFS_BLESSED_SEQUENCE_NUM_LAST[]      = _T( "definitionBlessedSequenceLast" );

// EVENT CONFIG SETTINGS - GENERAL ERRORS
const TCHAR REGVALUE_GENERAL_ERROR_UNAVAIABLE[]            = _T( "alertGeneralEventUnavailable");
const TCHAR REGVALUE_GENERAL_ERROR_SHUTDOWN[]              = _T( "alertGeneralEventShutdown");
const TCHAR REGVALUE_GENERAL_ERROR_DEFCAST[]               = _T( "alertGeneralEventDefcast");
const TCHAR REGVALUE_GENERAL_ERROR_DISK[]                  = _T( "alertGeneralEventDisk");
const TCHAR REGVALUE_GENERAL_ERROR_GATEWAY_COMM[]          = _T( "alertGeneralEventGatewayComm");
const TCHAR REGVALUE_GENERAL_ERROR_SCANEXPLICIT[]          = _T( "alertGeneralEventScanexplicit");
const TCHAR REGVALUE_GENERAL_ERROR_TARGET[]                = _T( "alertGeneralEventTarget");
const TCHAR REGVALUE_GENERAL_ERROR_NEWBLESSEDDEF[]         = _T( "alertGeneralEventNewBlessedDef");
const TCHAR REGVALUE_GENERAL_ERROR_NEWUNBLESSEDDEF[]       = _T( "alertGeneralEventNewUnblessedDef");

// EVENT CONFIG SETTINGS - SAMPLE ERRORS
// STATUS ERRORS
const TCHAR REGVALUE_SAMPLE_ERROR_HELD[]                   = _T( "alertStatusEventHeld" );
const TCHAR REGVALUE_SAMPLE_ERROR_ATTENTION[]              = _T( "alertStatusEventAttention" );
const TCHAR REGVALUE_SAMPLE_ERROR_ERROR[]                  = _T( "alertStatusEventError" );
const TCHAR REGVALUE_SAMPLE_ERROR_NOTINSTALLED[]           = _T( "alertStatusEventNotinstalled" );
// STATUS - TOO LONG IN STATE
const TCHAR REGVALUE_SAMPLE_ERROR_AVAILABLE[]              = _T( "alertStatusEventAvailable" );
const TCHAR REGVALUE_SAMPLE_ERROR_QUARANTINED[]            = _T( "alertStatusEventQuarantined" );
const TCHAR REGVALUE_SAMPLE_ERROR_SUBMITTED[]              = _T( "alertStatusEventSubmitted" );
const TCHAR REGVALUE_SAMPLE_ERROR_RELEASED[]               = _T( "alertStatusEventReleased" );
const TCHAR REGVALUE_SAMPLE_ERROR_NEEDED[]                 = _T( "alertStatusEventNeeded" );
const TCHAR REGVALUE_SAMPLE_ERROR_DISTRIBUTED[]            = _T( "alertStatusEventDistributed" );
const TCHAR REGVALUE_SAMPLE_ERROR_DISTRIBUTE[]             = _T( "alertStatusEventDistribute" );


// NOT USING AS SEPARATE EVENTS 1/30/00
// SAMPLE ERRORS  USED BY THE ERROR STATUS ABOVE
const TCHAR REGVALUE_SAMPLE_ERROR_ABANDONED[]              = _T( "alertSampleEventAbandoned" );
const TCHAR REGVALUE_SAMPLE_ERROR_CONTENT[]                = _T( "alertSampleEventContent" );
const TCHAR REGVALUE_SAMPLE_ERROR_CRUMBLED[]               = _T( "alertSampleEventCrumbled" );
const TCHAR REGVALUE_SAMPLE_ERROR_DECLINED[]               = _T( "alertSampleEventDeclined" );
const TCHAR REGVALUE_SAMPLE_ERROR_INTERNAL[]               = _T( "alertSampleEventInternal" );
const TCHAR REGVALUE_SAMPLE_ERROR_LOST[]                   = _T( "alertSampleEventLost" );
const TCHAR REGVALUE_SAMPLE_ERROR_MALFORMED[]              = _T( "alertSampleEventMalformed" );
const TCHAR REGVALUE_SAMPLE_ERROR_MISSING[]                = _T( "alertSampleEventMissing" );
const TCHAR REGVALUE_SAMPLE_ERROR_OVERRUN[]                = _T( "alertSampleEventOverrun" );
const TCHAR REGVALUE_SAMPLE_ERROR_SAMPLE[]                 = _T( "alertSampleEventSample" );
const TCHAR REGVALUE_SAMPLE_ERROR_SUPERCEDED[]             = _T( "alertSampleEventSuperceded" );
const TCHAR REGVALUE_SAMPLE_ERROR_TYPE[]                   = _T( "alertSampleEventType" );
const TCHAR REGVALUE_SAMPLE_ERROR_UNAVAILABLE[]            = _T( "alertSampleEventUnavailable" );
const TCHAR REGVALUE_SAMPLE_ERROR_UNDERRUN[]               = _T( "alertSampleEventUnderrun" );
const TCHAR REGVALUE_SAMPLE_ERROR_UNPACKAGE[]              = _T( "alertSampleEventUnpackage" );
const TCHAR REGVALUE_SAMPLE_ERROR_UNPUBLISHED[]            = _T( "alertSampleEventUnpublished" );
const TCHAR REGVALUE_SAMPLE_ERROR_NOTREPAIRED[]            = _T( "alertSampleEventNotRepaired" );
const TCHAR REGVALUE_SEND_TEST_EVENT[]                     = _T( "QsSendTestEvent" );
const TCHAR REGVALUE_WRITE_TO_NT_EVENT_LOG[]               = _T( "QsWriteAlertsToNTeventLog");
const TCHAR REGVALUE_DISKQUOTA_LOW_WATER[]                 = _T( "alertGeneralDiskQuota" );
const TCHAR REGVALUE_DISKSPACE_LOW_WATER[]                 = _T( "alertGeneralDiskSpace" );
const TCHAR REGVALUE_DISKQUOTA_CQ_FULL[]				   = _T( "alertGeneralDiskQuotaCQFull" );





#define QSITEMINFO_X_SAMPLE_STATUS        _T( QSERVER_ITEM_INFO_STATUS )          // "X-Sample-Status"
#define QSITEMINFO_X_SCAN_RESULT          _T( QSERVER_ITEM_INFO_SCAN_RESULT )     // "X-Scan-Result" 
#define QSITEMINFO_X_ERROR                _T( QSERVER_ITEM_INFO_ERROR )           // "X-Error"
#define QSITEMINFO_X_ATTENTION            _T( QSERVER_ITEM_INFO_X_ATTENTION )     // "X-Attention"
#define QSITEMINFO_X_ALERT_STATUS         _T( QSERVER_ITEM_INFO_X_ALERT_STATUS )  // "X-Alert-Status" 
#define QSITEMINFO_X_ALERT_STATUS_LAST    _T( "X-zAlert-Status-Last" )  
#define QSITEMINFO_X_DATE_ALERT           _T( QSERVER_ITEM_INFO_X_DATE_ALERT )    // "X-Date-Alert"
#define QSITEMINFO_X_ALERT_RESULT         _T( "X-Alert-Result" )  
#define QSITEMINFO_X_ALERT_STATUS_TIMER   _T( "X-Alert-Status-Timer" )  
//#define QSITEMINFO_X_ALERT_TIME_IN_STATE  _T( "X-Alert-TimeInState" )  


#define QSITEMINFO_X_PLATFORM_USER        _T(QSERVER_ITEM_INFO_USERNAME       )  //  "X-Platform-User"        // String
#define QSITEMINFO_X_PLATFORM_COMPUTER    _T(QSERVER_ITEM_INFO_MACHINENAME    )  //  "X-Platform-Computer"    // String
#define QSITEMINFO_X_PLATFORM_HOSTNAME    _T(QSERVER_ITEM_INFO_HOST)             //  "X-Platform-Host"        // String
#define QSITEMINFO_X_PLATFORM_ADDRESS     _T(QSERVER_ITEM_INFO_MACHINE_ADDRESS)  //  "X-Platform-Address"     // String
#define QSITEMINFO_X_PLATFORM_SYSTEM      _T(QSERVER_ITEM_INFO_SYSTEM         )  //  "X-Platform-System"      // String
#define QSITEMINFO_X_SAMPLE_FILE          _T(QSERVER_ITEM_INFO_FILENAME       )  //  "X-Sample-File"          // String
#define QSITEMINFO_X_SCAN_VIRUS_NAME      _T(QSERVER_ITEM_INFO_VIRUSNAME      )  //  "X-Scan-Virus-Name"      // String
#define QSITEMINFO_X_SAMPLE_REASON        _T(QSERVER_ITEM_INFO_SAMPLE_REASON  )  //  "X-Sample-Reason"        // String (unverified)
#define QSITEMINFO_X_ANALYSIS_VIRUS_NAME  _T("X-Analysis-Virus_Name")
#define QSITEMINFO_X_SIGNATURES_SEQUENCE  _T("X-Signatures-Sequence")

// TM 5-8-00
#define QSITEMINFO_X_SAMPLE_FILEID        _T( QSERVER_ITEM_INFO_FILEID )          // "X-Sample-Status"

// tm 6-20-02
#define QSITEMINFO_X_ANALYSIS_STATE		_T("X-Analysis-State")


#if 0
// DEFINES FOR ICEPACK TOKENS
// ATTENTION TABLE   
#define TOKEN_DEFCAST           _T("defcast")        
#define TOKEN_DISK              _T("disk")           
#define TOKEN_SCANEXPLICIT      _T("scanexplicit")   
#define TOKEN_TARGET            _T("target")         
#define TOKEN_GATEWAYCOMM       _T("gatewayComm")    
// NEW DEFINITIONS ARRIVAL
#define TOKEN_NEWBLESSEDDEF     _T("newBlessedDef")  
#define TOKEN_NEWUNBLESSEDDEF   _T("newUnblessedDef")
// FOR A SEARCH FUNCTION in ClearGeneralAttentionTimer()
#define TOKEN_DOWNLOAD_GATEWAY  _T("download")
#define TOKEN_QUERY_GATEWAY     _T("query")
#define TOKEN_SUBMIT_GATEWAY    _T("submit")
// SEND TEST EVENT
#define TOKEN_SEND_TEST_EVENT   _T("sendTestEvent")


// SAMPLE TABLE
// ICEPACK STATUS TOKENS
#define TOKEN_HELD              _T("held")        
#define TOKEN_ATTENTION         _T("attention")   
#define TOKEN_ERROR             _T("error")       
#define TOKEN_NOTINSTALLED      _T("notinstalled")
//   
#define TOKEN_QUARANTINED       _T("quarantined")
#define TOKEN_SUBMITTED         _T("submitted")
#define TOKEN_RELEASED          _T("released")
#define TOKEN_UNNEEDED          _T("unneeded")
#define TOKEN_NEEDED            _T("needed")
#define TOKEN_AVAILABLE         _T("available")   
#define TOKEN_DISTRIBUTED       _T("distributed")
#define TOKEN_INSTALLED         _T("installed")
#define TOKEN_RESTART           _T("restart")
#define TOKEN_DISTRIBUTE        _T("distribute")  


// ICEPACK ERROR TOKENS
#define TOKEN_ABANDONED         _T("abandoned")   
#define TOKEN_CONTENT           _T("content")     
#define TOKEN_CRUMBLED          _T("crumbled")    
#define TOKEN_DECLINED          _T("declined")    
#define TOKEN_INTERNAL          _T("internal")    
#define TOKEN_LOST              _T("lost")        
#define TOKEN_MALFORMED         _T("malformed")   
#define TOKEN_MISSING           _T("missing")     
#define TOKEN_OVERRUN           _T("overrun")     
#define TOKEN_SAMPLE            _T("sample")      
#define TOKEN_SUPERCEDED        _T("superceded")  
#define TOKEN_TYPE              _T("type")        
#define TOKEN_UNAVAILABLE       _T("unavailable") 
#define TOKEN_UNDERRUN          _T("underrun")    
#define TOKEN_UNPACKAGE         _T("unpackage")   
#define TOKEN_UNPUBLISHED       _T("unpublished") 

// Local Quarantine
#define TOKEN_NOTREPAIRED       _T("norepair") 
#endif











#endif





#if 0
// SAMPLE STATUS
quarantined
submitted
held
released
needed
available
distributed
attention
error
notinstalled
distribute


quarantined,submitted,held,released,needed,available,distributed,attention,error,notinstalled,distribute
#endif




