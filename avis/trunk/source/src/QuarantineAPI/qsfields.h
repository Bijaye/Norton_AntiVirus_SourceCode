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

#pragma once

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

#pragma pack(push, 1 )
//
// Disk geometry structure.
//
typedef struct tag_DISKGEOMETRY
    {
    DWORD       dwCylinders;
    WORD        wHeads;
    WORD        wSectorsPerTrack;
    WORD        wBytesPerSector;
    }
DISKGEOMETRY, FAR* LPDISKGEOMETRY;

#pragma pack(pop)


#endif
