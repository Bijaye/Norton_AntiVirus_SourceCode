/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/

/////////////////////////////////////////////////////////////////////////////
//
// QSFIELDS.H - This file contains the names and types of the standard
//              fields in quarantine server files.
//
/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////



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
#define QSERVER_ITEM_INFO_MACHINE_ADDRESS       "X-Platform-Address"     // String
#define QSERVER_ITEM_INFO_PROCESSOR             "X-Platform-Processor"   // String
#define QSERVER_ITEM_INFO_DISTRIBUTOR           "X-Platform-Distributor" // String
#define QSERVER_ITEM_INFO_HOST                  "X-Platform-Host"        // String
#define QSERVER_ITEM_INFO_SYSTEM                "X-Platform-System"      // String
#define QSERVER_ITEM_INFO_LANGUAGE              "X-Platform-Language"    // String
#define QSERVER_ITEM_INFO_OWNER                 "X-Platform-Owner"       // String
#define QSERVER_ITEM_INFO_SCANNER               "X-Platform-Scanner"     // String
#define QSERVER_ITEM_INFO_CORRELATOR            "X-Platform-Correlator"  // String
#define QSERVER_ITEM_INFO_PLATFORM_GUID         "X-Platform-GUID"        // String ( GUID format {xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx})
#define QSERVER_ITEM_INFO_DELIVER               "X-Platform-Deliver"     // String, new in Jedi release  "vdb" or "legacy"
// added by tam 3-07-02 new attributes for version 3.1
#define QSERVER_ITEM_PLATFORM_QSERVER_WININET   "X-Platform-QServer-WinINet"     // WinInet version running on qserver"
#define QSERVER_ITEM_PLATFORM_QSERVER_COUNTRY_CODE "X-Platform-QServer-CountryCode"     // Country Code on qserver"
#define QSERVER_ITEM_PLATFORM_QSERVER_WININET_ENCRYPTION_LEVEL "X-Platform-QServer-WinINet-Encryption"     // Country Code on qserver"
   
#define QSERVER_ITEM_INFO_FILE_CREATED_TIME     "X-Date-Created"         // QSDATE (Binary) / OLE DATE
#define QSERVER_ITEM_INFO_FILE_MODIFIED_TIME    "X-Date-Modified"        // QSDATE (Binary) / OLE DATE
#define QSERVER_ITEM_INFO_FILE_ACCESSED_TIME    "X-Date-Accessed"        // QSDATE (Binary) / OLE DATE
#define QSERVER_ITEM_INFO_QUARANTINE_QDATE      "X-Date-Quarantined"     // QSDATE (Binary) / OLE DATE
#define QSERVER_ITEM_INFO_SUBMIT_QDATE          "X-Date-Submitted"       // QSDATE (Binary) / OLE DATE

#define QSERVER_ITEM_INFO_VIRUSID               "X-Scan-Virus-Identifier" // DWORD
#define QSERVER_ITEM_INFO_DEF_SEQUENCE          "X-Scan-Signatures-Sequence" // DWORD
#define QSERVER_ITEM_INFO_DEF_VERSION           "X-Scan-Signatures-Version"  // String
#define QSERVER_ITEM_INFO_VIRUSNAME             "X-Scan-Virus-Name"      // String
#define QSERVER_ITEM_INFO_SCAN_RESULT           "X-Scan-Result"          // String (completed)
#define QSERVER_ITEM_INFO_VIRUSTYPE				"X-Scan-Virus-Type"		 // DWORD.

#define QSERVER_ITEM_INFO_CHECKSUM              "X-Sample-Checksum"      // String
#define QSERVER_ITEM_INFO_INFO_FILE_EXT         "X-Sample-Extension"     // String    
#define QSERVER_ITEM_INFO_FILENAME              "X-Sample-File"          // String
#define QSERVER_ITEM_INFO_FILESIZE              "X-Sample-Size"          // DWORD
#define QSERVER_ITEM_INFO_SAMPLE_TYPE           "X-Sample-Type"          // String (file)
#define QSERVER_ITEM_INFO_SAMPLE_REASON         "X-Sample-Reason"        // String (unverified)
#define QSERVER_ITEM_INFO_DISK_GEOMETRY         "X-Sample-Geometry"      // Binary GEOMETRY
#define QSERVER_ITEM_INFO_STATUS                "X-Sample-Status"        // DWORD
#define QSERVER_ITEM_INFO_SUBMISSION_ROUTE      "X-Sample-Submission-Route" // DWORD              
#define QSERVER_ITEM_INFO_UUID                  "X-Sample-UUID"          // String ( GUID format {xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx})
#define QSERVER_ITEM_INFO_CHANGES               "X-Sample-Changes"       // DWORD
#define QSERVER_ITEM_INFO_PRIORITY              "X-Sample-Priority"      // DWORD
#define QSERVER_ITEM_CHECKSUM_METHOD            "X-Checksum-Method"      // String (md5)    

// added 1-15-05 tmarles
#define QSERVER_ITEM_PACKAGE_TYPE				"X-Sample-Package-Format" // type if format for snd package sent.
#define QSERVER_ITEM_PACKAGE_CORE_ITEM_COUNT	"X-Sample-Package-Core-Item-Count" // number of samples in the SND package.
#define QSERVER_ITEM_PACKAGE_CORE_ITEM_CHECKSUM	"X-Sample-Package-Core-Item-Checksums" // checksum of the sample in the SND package.
#define QSERVER_ITEM_PACKAGE_CORE_ITEM_SIZES    "X-Sample-Package-Core-Item-Sizes"      // size of the file for the sampel


// Added 12-2-99 Terrym for addational Bell attributes.
#define QSERVER_ITEM_INFO_COMPLETED_TIME        "X-Date-Completed"       // QSDATE (Binary) / OLE DATE
#define QSERVER_ITEM_INFO_SAMPLE_STATE          "X-Analysis-State"       // String (sample state)
#define QSERVER_ITEM_INFO_MIN_DEF_SEQ           "X-Signatures-Sequence"  // DWORD
// Changed 12/30/99 jhill
#define QSERVER_ITEM_INFO_ERROR                 "X-Error"		         // String (Error if any)  
#define QSERVER_ITEM_INFO_X_ATTENTION           "X-Attention"            // String (Error if any) 
#define QSERVER_ITEM_INFO_X_ALERT_STATUS        "X-Alert-Status"         // String - Last Alert
#define QSERVER_ITEM_INFO_X_DATE_ALERT          "X-Date-Alert"           // QSDATE (Binary) 

#define QSERVER_ITEM_INFO_SAMPLE_RESULT         "X-Sample-Result"        // result of proccessing sample
#define QSERVER_ITEM_INFO_ALERT_STATUS          QSERVER_ITEM_INFO_X_ALERT_STATUS   // Alert Status of the Sample //tam 2-27-00

// added 3-15-00 
#define QSERVER_ITEM_INFO_ANALYZED_TIME        "X-Date-Analyzed"       // STRING

// 
// Internal Symantec fields.
// 
#define QSERVER_ITEM_INFO_FILEID                "X-Sample-FileID"        // DWORD

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
#define STATUS_RISK		       15               // new in Kepler release 8-4-03 tmarles
#define STATUS_HACKTOOL        16               // new in Kepler release 8-4-03 tmarles
#define STATUS_SPYWARE	       17               // new in Kepler release 8-4-03 tmarles
#define STATUS_TRACKWARE       18               // new in Kepler release 8-4-03 tmarles
#define STATUS_DIALER	       19               // new in Kepler release 8-4-03 tmarles
#define STATUS_REMOTE	       20               // new in Kepler release 8-4-03 tmarles
#define STATUS_ADWARE	       21               // new in Kepler release 8-4-03 tmarles
#define STATUS_PRANK	       22               // new in Kepler release 8-4-03 tmarles

// 
// Values for QSERVER_ITEM_INFO_SUBMISSION_ROUTE
// 
#define SUBMISSION_SCANANDDELIVER   0
#define SUBMISSION_AVIS             1

// 
// Values for QSERVER_ITEM_INFO_SCAN_RESULT
// 
#define RESULT_CLEAN                0
#define RESULT_DETECTED             1
#define RESULT_VARIANT              2
#define RESULT_VERIFIED             3
#define RESULT_REPAIRED             4    

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

#if !defined(PACKED)
#pragma pack(push, 1)
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
#endif


#endif
