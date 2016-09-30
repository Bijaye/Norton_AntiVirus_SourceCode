////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

namespace AV
{
#define CC_AV_EVENT_ID_BASE 3000

enum Event_IDs // these are ccEvent IDs!!!
{
    Event_ID_Base = CC_AV_EVENT_ID_BASE,
    Event_ID_Threat,                // Virus and non-viral alert events
    Event_ID_ScanAction,            // Scanner started/stopped
    Event_ID_Error,                 // Error occured
    Event_ID_Container_ScanStatus,  // SAVRT Container scan status info (start/stop, etc.)
    Event_ID_ManualScan_Spyware_Detection,  // Manual scan detection to track in AP alerting queue
    Event_ID_StatusRequest,         // Request for current status data from client
    Event_ID_StatusALU,             // Status update event for Automatic LiveUpdate
    Event_ID_StatusAP,              // Status update event for Auto-Protect
    Event_ID_StatusDefs,            // Status update event for virus definitions
    Event_ID_StatusFSS,             // Status update event for Full System Scan
    Event_ID_StatusIWP,             // Status update event for IWP
    Event_ID_StatusEmail,           // Status update event for Email
    Event_ID_StatusLicensing,       // Status update event for Licensing
    Event_ID_StatusIMScan,          // Event ID for IM scanner status (updates not supported)
    Event_ID_StatusSpyware,         // Status update event for "Spyware" threat category
    Event_ID_StatusInstall,         // Status update event for install/cfgwiz changes
    Event_ID_LAST                   // ALWAYS THE LAST EVENT - so we know where to stop
};

enum Event_Base_properties // S = string, L = long
{
    Event_Base_propType = 0,
    Event_Base_propUserName,           // S - Added by NAVEvents when the event is broadcast
    Event_Base_propSessionID,          // L - Session ID that generated the event
    Event_Base_propVersion,            // L - Event version, different for each type
    Event_Base_propUniqueID,           // B - log entry GUID
    Event_Base_propLastProperty = 100  // So any derived classes know where to start (for expansion)
};

enum Event_Threat_properties
{
    Event_Threat_propBase = Event_Base_propLastProperty,
    Event_Threat_propProductName,       // S - Product Name?
    Event_Threat_propProductVersion,    // S - Product Version (xx.xx.xx.xx)?
    Event_Threat_propFeature,           // L - AutoProtect, Office scanner (use ccModuleId.h)
    Event_Threat_propFeatureVersion,    // L - Feature Version (xx.xx.xx.xx)?
    Event_Threat_propDefsRevision,      // S - Revision of the defs that found the infection. YYYYMMDDRRRR
    Event_Threat_propObjectType,	    // L - File, Boot Record, Master Boot Record (AV_Event_VirusAlert_Types)
    Event_Threat_propEventSubType,      // L - Threat, Virus-like, Script Activity (Event_Threat_EventType_properties)
    Event_Threat_propProcessID,         // L - Process ID that wrote the file (optional)
    Event_Threat_propAvModuleData,      // B - serialize AvModule Event Map (AvModule::IAVMapDwordData)
    Event_Threat_propAvModuleDispInfo,  // B - serialize AvModule Event Map (AvModule::IAVArrayData)
    Event_Threat_propSubFileData,       // ED - array of Event_Threat_SubFile 's 
    Event_Threat_propRemediationData,   // ED - array of AvModule DisplayData (eRemediationDisplayDataKeys in AvDefines.h)

    Event_Threat_propLastProperty	// Always the last
};

enum Event_Threat_SubFile_props
{
    Event_ThreatSubFile_propFileName,          // S - filename (empty for single file threats)
    Event_ThreatSubFile_propFileDescription,   // S - (optional) for extended formatted descriptions like: folder\folder\file.exe
    Event_ThreatSubFile_propThreatData,        // ED - array of Event_ThreatEntry's
};

// One for every threat in a file
//
enum Event_ThreatEntry
{
    Event_ThreatEntry_propVirusID,          // L - Virus ID
    Event_ThreatEntry_propVirusName,        // S - Virus name
    Event_ThreatEntry_propActionCount,      // L - Count of Event_Threat_Actions performed
    Event_ThreatEntry_propActionData,       // B - Array of L's - Repaired, quarantined, etc. (see Action Type)
    Event_ThreatEntry_propStillPresent,     // L - 0 == fully removed, non-zero == still present
    Event_ThreatEntry_propThreatCatCount,   // L - # of threat categories
    Event_ThreatEntry_propThreatCategories, // S - Space delimited threat categories.
    Event_ThreatEntry_propThreatFlags,      // L - Threat flags from the engine

    // Threat matrix (damage flag) values from ccEraser
    Event_ThreatEntry_propThreatMatrixOverall,       // S - (optional) threat matrix text
    Event_ThreatEntry_propThreatMatrixPrivacy,       // S - (optional) threat matrix text
    Event_ThreatEntry_propThreatMatrixRemoval,       // S - (optional) threat matrix text
    Event_ThreatEntry_propThreatMatrixStealth,       // S - (optional) threat matrix text
    Event_ThreatEntry_propThreatMatrixPerformance,    // S - (optional) threat matrix text
    Event_ThreatEntry_propThreatMatrixDependency     // S - (optional) threat matrix text
};

enum Event_Threat_EventSubType_properties
{
    Event_Threat = 0,
    //Event_VirusLike,
    //Event_ScriptBlocking,
    Event_Threat_Container = 3,
    Event_APBlockedThreat
};

enum Event_ScanAction_properties
{
    Event_ScanAction_propBase = Event_Base_propLastProperty,
    Event_ScanAction_propAction,         // L - Scan started, stopped, cancelled ...
    Event_ScanAction_propProductName,       // S - Product Name?
    Event_ScanAction_propProductVersion,    // S - Product Version (xx.xx.xx.xx)?
    Event_ScanAction_propFeature,           // L - AutoProtect, Office scanner (use ccModuleId.h)
    Event_ScanAction_propFeatureVersion,    // L - Feature Version (xx.xx.xx.xx)?
    Event_ScanAction_propDefsRevision,      // S - Revision of the defs that found the infection. YYYYMMDDRRRR
    Event_ScanAction_propTaskName,       // S - My Computer, custom name, A:, ...
    Event_ScanAction_propAvModuleData,      // B - serialize AvModule Event Map 
	
	// The following DWORD properties are only present 
	// if Event_ScanAction_propAction == Event_ScanAction_ScanCompleted.
	Event_ScanAction_MBRsRepaired, 
	Event_ScanAction_MBRsTotalInfected,
    Event_ScanAction_MBRsTotalScanned,
	Event_ScanAction_BootRecsRepaired, 
	Event_ScanAction_BootRecsTotalInfected,
	Event_ScanAction_BootRecsTotalScanned,
	Event_ScanAction_VirusesDetected,
	Event_ScanAction_VirusesRemoved,
	Event_ScanAction_NonVirusesDetected,
	Event_ScanAction_NonVirusesRemoved,
	Event_ScanAction_ItemTotalScanned,
	Event_ScanAction_FilesRepaired,        
	Event_ScanAction_FilesQuarantined,     
	Event_ScanAction_FilesDeleted,         
	Event_ScanAction_FilesTotalInfected,
	Event_ScanAction_FilesTotalScanned,
	Event_ScanAction_propLastProperty,
	Event_ScanAction_FilesExcluded,
	Event_ScanAction_ScanTime,
   
    Event_ScanAction_Display,                // S - For memory effects - Process termination text
    Event_ScanAction_ResolvedByCategory,     // ED - array of category counts
    Event_ScanAction_UnResolvedByCategory,   // ED - array of category counts
    Event_ScanAction_EraserScanResults       // ED - ERASER scan results (see ccEraserInterface.h eObjectType)
};

enum Event_OEHDetection_properties
{
    Event_OEHDetection_propBase = Event_Threat_propLastProperty,
    Event_OEHDetection_propThreatTrackId,  // B - AvModule ThreatTrack ID
    Event_OEHDetection_propSubject,        // S - Email Subject
    Event_OEHDetection_propSender,         // S - Email Sender
    Event_OEHDetection_propRecipient,      // S - Email Recipient
    Event_OEHDetection_propWormFile,       // S - Worm File Name
    Event_OEHDetection_propAction,         // L - AvModule Action ID
    Event_OEHDetection_propLastProperty
};

enum Event_Error_properties
{
    Event_Error_propBase = Event_Base_propLastProperty,
    Event_Error_propErrorID,        // L - Internal error ID
    Event_Error_propModuleID,       // L - Internal module ID
    Event_Error_propResult,         // L - Error result code (E_FAIL)
    Event_Error_propMessage,        // S - Error messsage ("File not found")
    Event_Error_propProductVersion, // S - Product Version
    Event_Error_propLastProperty
};

enum Event_Action_Types
{
    Event_Action_Access_Denied = 0,  // "Access was denied"
    Event_Action_Allowed,            // Script Allowed
    Event_Action_Blocked,            // "Blocked"
    Event_Action_Authorized,         // "Authorized"
    Event_Action_Repaired,           // "Repaired"
    Event_Action_RepairFailed,       // "Unable to repair"
    Event_Action_Quarantined,        // "Quarantined"
    Event_Action_QuarantinedFailed,  // "Unable to quarantine"
    Event_Action_Deleted,            // "Deleted"
    Event_Action_DeleteFailed,       // "Unable to delete"
    Event_Action_BlockFailed,        // "Unable to block"
    Event_Action_AuthorizedFailed,   // "Unable to authorize"
    Event_Action_NoActionTaken,	 	 // "No Action Taken" == "Detected"
    Event_Action_Excluded,			 // "Excluded"
    Event_Action_Exclude_Failed,	 // "Unable to Exclude"
    Event_Action_Shutdown,			 // "Shutdown"
    Event_Action_Unknown,			 // "Unknown"
    Event_Action_Allowed_Method,     // One method allowed
    Event_Action_Auto_Deleted,		 // "Automatically Deleted"
    Event_Action_Removed,            // "Removed"
	Event_Action_Restored,			 // "Restored" (by Quarantine)
	Event_Action_FullyRemoved,			 // "ThreatState_FullyRemoved" 
	Event_Action_PartiallyRemoved,
	Event_Action_RemoveNotAttempted,
	Event_Action_CompressedInProcessing,
	Event_Action_BackupOnly,
	Event_Action_DoNotDelete,
    Event_Action_RemoveOnRebootFailed,
    Event_Action_CannotRemediate
};
  
enum Event_Threat_ObjectTypes
{
	Event_Threat_ObjectType_File = 0,
	Event_Threat_ObjectType_BootRecord,
	Event_Threat_ObjectType_MasterBootRecord,
      Event_Threat_ObjectType_Memory,
      //Event_Threat_ObjectType_File_Closed,
	Event_Threat_ObjectType_Unknown    
};

enum Event_VirusLike_Attempted_Actions
{
	Event_VirusLike_Attempted_Action_HD_Format = 0,
	Event_VirusLike_Attempted_Action_HD_MBR_Write,
	Event_VirusLike_Attempted_Action_HD_Boot_Write,
	Event_VirusLike_Attempted_Action_FD_Boot_Write,
};

enum Event_ScanAction_Actions
{
	Event_ScanAction_ScanStarted = 0,
	Event_ScanAction_ScanAborted,
	Event_ScanAction_ScanCompleted,
    Event_ScanAction_MemorySideEffect,
};

// For Container status reports
enum Event_Container_ScanStatus
{
    Event_Container_Status = Event_Base_propLastProperty,    // L - see enumEvent_Container_Status
    Event_Container_Cookie,                                     // B - 64-bit value SAVRT_ROUS64
    Event_Container_ObjectName,                                 // S - file name
    Event_Container_ProcessID,                                  // L - Process ID that wrote the file
};

//////////////////////////////////////////////////////////////////////
// Values for Event_Container_Status.

enum enumEvent_Container_Status 
{
    Event_Container_Status_STARTED = 0,      // Container scan started
    Event_Container_Status_BLOCKED,          // Application was blocked
    Event_Container_Status_COMPLETED,        // Container scan finished OK
    Event_Container_Status_CANCELLED,        // The user cancelled a scan
    Event_Container_Status_ABORTED           // The scan took too long and timeout out
};

enum enumEvent_ManualScan_Spyware_Detection
{
    Event_ManualScan_Spyware_propBase = Event_Base_propLastProperty,
    Event_ManualScan_Spyware_propCookie,           // L - Cookie of scan
    Event_ManualScan_Spyware_propEnd,              // L - Indicates manual scan has completed for this cookie
    Event_ManualScan_Spyware_propFileName,         // S - wide string file name
    Event_ManualScan_Spyware_propVirusID           // L - VID (actually it's an unsigned long so casting is required by the client)
};

}; // end namespace AV


