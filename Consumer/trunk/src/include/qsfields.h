////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2004 Symantec Corporation.
// All rights reserved.
//////////////////////////////////////////////////////////////////////////////
//
// QSFIELDS.H - This file contains the names and types of the standard
//              fields in quarantine server files.
//

#ifndef _QSFIELDS_H
#define _QSFIELDS_H

#if defined(WIN32)
#pragma once
#endif

#if defined(UNIX)
#define FAR
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned long DWORD;
#endif

#if defined(__GNUC__)
#define PACKED  __attribute__ ((packed))
#else
#define PACKED
#endif


/////////////////////////////////////////////////////////////////////////////
//
// QServer item info names - For clarity and debugging purposes, all field names
// shoudl be in the following form:
//
//    XXXX_FieldName where XXXX can be one of the following prefixes
//
//    AVIS - IBM AntiVirus Immune System attributes.
//    QUAR - Symantec Quarantine attributes.
//
/////////////////////////////////////////////////////////////////////////////

/**** NOTE IMPORTANT ALL STRINGS ARE CASE SENSITIVE ****/

#define QSERVER_ITEM_INFO_USERNAME              "X-Platform-User"        // String
#define QSERVER_ITEM_INFO_MACHINENAME           "X-Platform-Computer"    // String
#define QSERVER_ITEM_INFO_DOMAINNAME            "X-Platform-Domain"      // String
//#define QSERVER_ITEM_INFO_MACHINE_ADDRESS       "X-Platform-Address"     // String
//#define QSERVER_ITEM_INFO_PROCESSOR             "X-Platform-Processor"   // String
//#define QSERVER_ITEM_INFO_DISTRIBUTOR           "X-Platform-Distributor" // String
//#define QSERVER_ITEM_INFO_HOST                  "X-Platform-Host"        // String
//#define QSERVER_ITEM_INFO_SYSTEM                "X-Platform-System"      // String
//#define QSERVER_ITEM_INFO_LANGUAGE              "X-Platform-Language"    // String
//#define QSERVER_ITEM_INFO_OWNER                 "X-Platform-Owner"       // String
//#define QSERVER_ITEM_INFO_SCANNER               "X-Platform-Scanner"     // String
//#define QSERVER_ITEM_INFO_CORRELATOR            "X-Platform-Correlator"  // String
//#define QSERVER_ITEM_INFO_PLATFORM_GUID         "X-Platform-GUID"        // String ( GUID format {xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx})
//#define QSERVER_ITEM_INFO_DELIVER               "X-Platform-Deliver"     // String, new in Jedi release  "vdb" or "legacy"
   
#define QSERVER_ITEM_INFO_FILE_CREATED_TIME     "X-Date-Created"         // QSDATE (Binary) / OLE DATE
#define QSERVER_ITEM_INFO_FILE_MODIFIED_TIME    "X-Date-Modified"        // QSDATE (Binary) / OLE DATE
#define QSERVER_ITEM_INFO_FILE_ACCESSED_TIME    "X-Date-Accessed"        // QSDATE (Binary) / OLE DATE
#define QSERVER_ITEM_INFO_QUARANTINE_QDATE      "X-Date-Quarantined"     // QSDATE (Binary) / OLE DATE
#define QSERVER_ITEM_INFO_SUBMIT_QDATE          "X-Date-Submitted"       // QSDATE (Binary) / OLE DATE

#define QSERVER_ITEM_INFO_VIRUSID               "X-Scan-Virus-Identifier" // DWORD
//#define QSERVER_ITEM_INFO_DEF_SEQUENCE          "X-Scan-Signatures-Sequence" // DWORD
//#define QSERVER_ITEM_INFO_DEF_VERSION           "X-Scan-Signatures-Version"  // String
#define QSERVER_ITEM_INFO_VIRUSNAME             "X-Scan-Virus-Name"      // String
//#define QSERVER_ITEM_INFO_SCAN_RESULT           "X-Scan-Result"          // String (completed)

//#define QSERVER_ITEM_INFO_CHECKSUM              "X-Sample-Checksum"      // String
//#define QSERVER_ITEM_INFO_INFO_FILE_EXT         "X-Sample-Extension"     // String
//#define QSERVER_ITEM_INFO_FILENAME              "X-Sample-File"          // String
#define QSERVER_ITEM_INFO_FILESIZE              "X-Sample-Size"          // DWORD

//#define QSERVER_ITEM_INFO_ITEM_SIZE              "X-Sample-Item-Size%d"   // DWORD 0: Low, 1: High

//#define QSERVER_ITEM_INFO_SAMPLE_TYPE           "X-Sample-Type"          // String (file)
//#define QSERVER_ITEM_INFO_SAMPLE_REASON         "X-Sample-Reason"        // String (unverified)
//#define QSERVER_ITEM_INFO_DISK_GEOMETRY         "X-Sample-Geometry"      // Binary GEOMETRY
#define QSERVER_ITEM_INFO_STATUS                "X-Sample-Status"        // DWORD
//#define QSERVER_ITEM_INFO_SUBMISSION_ROUTE      "X-Sample-Submission-Route" // DWORD              
#define QSERVER_ITEM_INFO_UUID                  "X-Sample-UUID"          // String ( GUID format {xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx})
//#define QSERVER_ITEM_INFO_CHANGES               "X-Sample-Changes"       // DWORD
//#define QSERVER_ITEM_INFO_PRIORITY              "X-Sample-Priority"      // DWORD
//#define QSERVER_ITEM_CHECKSUM_METHOD            "X-Checksum-Method"      // String (md5)   

// Added 12-2-99 Terrym for addational Bell attributes.
//#define QSERVER_ITEM_INFO_COMPLETED_TIME        "X-Date-Completed"       // QSDATE (Binary) / OLE DATE
//#define QSERVER_ITEM_INFO_SAMPLE_STATE          "X-Analysis-State"       // String (sample state)
//#define QSERVER_ITEM_INFO_MIN_DEF_SEQ           "X-Signatures-Sequence"  // DWORD
// Changed 12/30/99 jhill
//#define QSERVER_ITEM_INFO_ERROR                 "X-Error"		         // String (Error if any)  
//#define QSERVER_ITEM_INFO_X_ATTENTION           "X-Attention"            // String (Error if any) 
//#define QSERVER_ITEM_INFO_X_ALERT_STATUS        "X-Alert-Status"         // String - Last Alert
//#define QSERVER_ITEM_INFO_X_DATE_ALERT          "X-Date-Alert"           // QSDATE (Binary) 

//#define QSERVER_ITEM_INFO_SAMPLE_RESULT         "X-Sample-Result"        // result of proccessing sample
//#define QSERVER_ITEM_INFO_ALERT_STATUS          QSERVER_ITEM_INFO_X_ALERT_STATUS   // Alert Status of the Sample //tam 2-27-00

// added 3-15-00 
//#define QSERVER_ITEM_INFO_ANALYZED_TIME        "X-Date-Analyzed"       // STRING

// added for Common Client Quar32 support
#define QSERVER_ITEM_INFO_QUAR_PATH				"X-Sample-QuarPath"      // String
#define QSERVER_ITEM_INFO_QUAR_FILENAME         "X-Sample-QuarFile"      // String
#define QSERVER_ITEM_INFO_FILENAME_ANSI         "X-Sample-File-Ansi"     // String
#define QSERVER_ITEM_INFO_SCAN_QDATE            "X-Date-Scanned"		// QSDATE (Binary) / OLE DATE
#define	QSERVER_ITEM_INFO_SCAN_DEFS_QDATE       "X-Date-Scan_Defs"		// QSDATE (Binary) / OLE DATE
#define QSERVER_ITEM_INFO_SAMPLE_DWTYPE         "X-Sample-dwType"		// DWORD

// Added 3-21-03 for Scan and deliver field information
#define QSERVER_ITEM_INFO_CATEGORIES            "X-Sample-Categories"   // string categories for infected item
#define QSERVER_ITEM_INFO_MD5                   "X-Sample-Md5"          // string md5 hash for file


// 10/14/2004 - ccGSER side effects are now obsolete, but must still be
//		supported so we can restore and display side effects from quarantine items
//		created by earlier versions.
// Added 3-12-04 for side effect field information
//
// The number of side effects stored for this quarantine item
#define QSERVER_ITEM_SE_COUNT                   "X-Sample-SECount"      // DWORD

// The side effect type for side effect item %d
#define QSERVER_ITEM_SE_TYPE             "X-Sample-SE%d-Type"           // DWORD

// Side effect type definitions.
enum SE_TYPE
{
    SE_TYPE_REGKEY,
    SE_TYPE_STARTUP,
    SE_TYPE_INI,
    SE_TYPE_BATCH
};

// The actual data for side effect type %d. There can be multiple data fields set
// depending on the type (i.e: X-Sample-SE0-Data0, X-Sample-SE0-Data1, etc.). The
// data fields for each type is described below
#define QSERVER_ITEM_SE_DATA             "X-Sample-SE%d-Data%d"         // VARIES (see descriptions below)

// SE_TYPE_REGKEY:
// X-Sample-SE%d-Data0: Registry key (string value)
// X-Sample-SE%d-Data1: Registry key type (dword value)
// X-Sample-SE%d-Data2: Marker to identify this value as the main side effect key so the restore will
//                      know to use the restored file name in addition to the data3 value when restoring
//                      this registry side effect (dword value 0 or 1)
// X-Sample-SE%d-Data3: Registry key data (binary value)

// Parameters to the registry key value. This will only exist
// if there are parameters to the registry key (string value)

// SE_TYPE_STARTUP:
// X-Sample-SE%d-Data0: Startup folder (string value)

// SE_TYPE_INI:
// X-Sample-SE%d-Data0: Ini file (string value)
// X-Sample-SE%d-Data1: Section (string value)
// X-Sample-SE%d-Data2: Value (string value)

// SE_TYPE_BATCH:
// X-Sample-SE%d-Data0: Batch file (string value)
// X-Sample-SE%d-Data1: Line Number (dword value)

// The user friendly description for side effect item %d. This is what gets
// displayed to the user in the Quarantine console properties panel
#define QSERVER_ITEM_SE_DESC             "X-Sample-SE%d-Desc"           // STRING


///////////////////////////////////////////////
// ccEraser support

//Anomaly fields
#define QSERVER_ITEM_INFO_ANOMALY_ID "X-Sample-Anomaly-GUID" // String ( GUID format {xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx})
#define QSERVER_ITEM_INFO_ANOMALY_NAME "X-Sample-Anomaly-Name" // String
#define QSERVER_ITEM_INFO_ANOMALY_CATEGORIES "X-Sample-Anomaly-Categories" // String - categories for infected item

#define QSERVER_ITEM_INFO_ANOMALY_DAMAGE_FLAG "X-Sample-Anomaly-Damage-Flag%d" // String - anomaly damage flags
#define QSERVER_ITEM_INFO_ANOMALY_DEPENDENCY_FLAG "X-Sample-Anomaly-Dependency-Flag" // DWORD - 1: Has known dependencies, 0: No known dependencies

//////////////////////////////////////////////////////////////////
// Quarantine Internal  - Remediation fields
#define QSERVER_ITEM_REMEDIATION_COUNT "X-Sample-Remediation-Count" // DWORD - Number of remediation actions stored in this quarantine item
#define QSERVER_ITEM_REMEDIATION_FAILURE_COUNT "X-Sample-Remediation-Failure-Count" // DWORD - Number of remediation actions that failed to apply correctly

// String
#define QSERVER_ITEM_INFO_DATA_SUBFOLDER "X-Sample-Data-Subfolder"
#define QSERVER_ITEM_REMEDIATION_ACTION_FILE "X-Sample-Remediation-Action-File%d"
#define QSERVER_ITEM_REMEDIATION_UNDO_FILE "X-Sample-Remediation-Undo-File%d"
#define QSERVER_ITEM_REMEDIATION_DESCRIPTION "X-Sample-Remediation-Description%d"
#define QSERVER_ITEM_REMEDIATION_ORIGINAL_FILENAME "X-Sample-Remediation-Original-Filename%d"

// DWORD
#define QSERVER_ITEM_REMEDIATION_ACTION_SUCCEEDED "X-Sample-Remediation-Action-Succeeded%d"
#define QSERVER_ITEM_REMEDIATION_ACTION_TYPE "X-Sample-Remediation-Action-Type%d"  // i.e. RegistryRemediationActionType, InfectionRemediationActionType
#define QSERVER_ITEM_REMEDIATION_OPERATION_TYPE "X-Sample-Remediation-Operation-Type%d" // i.e. Delete, Terminate, HandleThreat

// QSDATE
#define	QSERVER_ITEM_INFO_ORIGINAL_SCAN_DEFS_QDATE "X-Date-Original_Scan_Defs"		// QSDATE (Binary) / OLE DATE

// String – Name of system snapshot data file (without path) taken at the time of quarantine
#define QSERVER_ITEM_SYSTEM_SNAPSHOT_FILE "X-Sample-SystemSnapshot-File"

// DWORD
#define QSERVER_ITEM_USING_DUMMY_FILE "X-Sample-Dummy-File"  // Indicates that the main quarantine item file is a dummy file
///////////////////////////////////////////////////////////////////


// 
// Internal Symantec fields.
// 
//#define QSERVER_ITEM_INFO_FILEID                "X-Sample-FileID"        // DWORD

// 
// Values for QSERVER_ITEM_INFO_STATUS
// 
#define STATUS_QUARANTINED     0        
#define STATUS_SUBMITTED       1
#define STATUS_HELD            2
#define STATUS_RELEASED        3
#define STATUS_UNNEEDED        4
#define STATUS_NEEDED          5
#define STATUS_AVAILIABLE      6
#define STATUS_DISTRIBUTED     7                 
#define STATUS_INSTALLED       8
#define STATUS_ATTENTION       9
#define STATUS_ERROR           10 
#define STATUS_NOTINSTALLED    11               // new in Jedi release
#define STATUS_RESTART         12               // new in Jedi release
#define STATUS_LEGACY          13               // new in Jedi release
#define STATUS_DISTRIBUTE      14               // new in Jedi release

// 
// Values for QSERVER_ITEM_INFO_SUBMISSION_ROUTE
// 
//#define SUBMISSION_SCANANDDELIVER   0
//#define SUBMISSION_AVIS             1

// 
// Values for QSERVER_ITEM_INFO_SCAN_RESULT
// 
//#define RESULT_CLEAN                0
//#define RESULT_DETECTED             1
//#define RESULT_VARIANT              2
//#define RESULT_VERIFIED             3
//#define RESULT_REPAIRED             4    

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

/*#if !defined(PACKED)
#pragma pack(push)
#pragma pack(1)
#endif
//
// Disk geometry structure.
//
typedef struct tag_DISKGEOMETRY
    {
    DWORD       dwCylinders PACKED;
    WORD        wHeads PACKED;
    WORD        wSectorsPerTrack PACKED;
    WORD        wBytesPerSector PACKED;
    }
DISKGEOMETRY, FAR* LPDISKGEOMETRY;

#if !defined(PACKED)
#pragma pack(pop)
#endif*/


#endif
