////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// This file contains Legacy Strucures and definitions for the old Quar32 file
// format to use in converting over the old quarantine file format over the the new QSPAK
// file format

#ifndef QUAR32LEGACY_H
#define QUAR32LEGACY_H

// Structures pluued form Quaradd.h

#define MAX_QUARANTINE_EXTENSION_LENGTH     260
#define MAX_QUARANTINE_FILENAME_LEN         260
#define MAX_QUARANTINE_MACHINE_DOMAIN_LEN   50
#define MAX_QUARANTINE_MACHINE_NAME_LEN     50
#define MAX_QUARANTINE_OWNER_NAME_LEN       50
#define MAX_QUARANTINE_FILES                0xFFFFFFF

#define QUARANTINE_SIGNATURE                0xAAAA20CD

#define QUARANTINE_TRANSFER_BUFFER_SIZE         0x8000

#pragma pack(push)
#pragma pack(1)

typedef struct  tagQFILE_INFO_STRUCT
{
    UUID                uniqueID;
    DWORD               dwFileStatus;
    DWORD               dwFileType;
    SYSTEMTIME          stDateQuarantined;
    SYSTEMTIME          stOriginalFileDateCreated;
    SYSTEMTIME          stOriginalFileDateAccessed;
    SYSTEMTIME          stOriginalFileDateWritten;
    SYSTEMTIME          stDateOfLastScan;
    SYSTEMTIME          stDateOfLastScanDefs;
    SYSTEMTIME          stDateSubmittedToSARC;
    DWORD               dwOriginalFileSize;
    char                szCurrentFilename[MAX_QUARANTINE_FILENAME_LEN];
    char                szOriginalAnsiFilename[MAX_QUARANTINE_FILENAME_LEN];
    char                szOriginalOwnerName[MAX_QUARANTINE_OWNER_NAME_LEN];
    char                szOriginalMachineDomain[MAX_QUARANTINE_MACHINE_DOMAIN_LEN];
    char                szOriginalMachineName[MAX_QUARANTINE_MACHINE_NAME_LEN];

} QFILE_INFO_STRUCT, FAR*  LPQFILE_INFO_STRUCT;
#define SQFILE_INFO_STRUCT sizeof(QFILE_INFO_STRUCT)

typedef struct  tagQFILE_HEADER_STRUCT      // actual physical file header
{
    DWORD                           dwSignature;
    DWORD                           dwVersion;
    DWORD                           dwHeaderBytes;
    QFILE_INFO_STRUCT               FileInfo;

} QFILE_HEADER_STRUCT, FAR*  LPQFILE_HEADER_STRUCT;
#define SQFILE_HEADER_STRUCT sizeof(QFILE_HEADER_STRUCT)

#pragma pack( pop )

#endif