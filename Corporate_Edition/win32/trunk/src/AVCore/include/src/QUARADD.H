// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
// $Header:   S:/INCLUDE/VCS/quaradd.h_v   1.4   08 Jul 1998 18:44:18   jtaylor  $
//-----------------------------------------------------------------------------
// $Log:   S:/INCLUDE/VCS/quaradd.h_v  $
// 
//    Rev 1.4   08 Jul 1998 18:44:18   jtaylor
// removed the original quarantine signature value.
//
//    Rev 1.3   08 Jul 1998 12:25:50   jtaylor
// Changed the Quarantine file signature and updated the code to work with both signatures and upgrade the old signature.
//
//    Rev 1.2   30 Jun 1998 22:34:36   sedward
// Changed QUARANTINE_TRANSFER_BUFFER_SIZE to 0x8000; this allows DEC/Alpha
// machines to forward to the Quarantine service.
//
//    Rev 1.1   08 Jun 1998 17:42:20   RStanev
// Splint AddFileToQuarantine() into two functions.
//
//    Rev 1.0   26 May 1998 14:29:22   DBuches
// Initial revision.
//
//    Rev 1.21   20 May 1998 21:57:40   jtaylor
// Enhanced to support long file extensions.
//
//    Rev 1.20   12 May 1998 17:30:22   DALLEE
// Added Status parameter to AddFileToQuarantine().
// Heh heh... nice to know whether to put it into Quarantine or
// Backups, or whatever.
//
//    Rev 1.19   27 Apr 1998 16:57:16   jtaylor
// Changed the date quarantined back into a system time.
//
//    Rev 1.18   26 Apr 1998 19:11:22   jtaylor
// Added an AP header version number.
// Converted the AP header to use FILETIME strucures.
// Upped the version number for the main Quarantine header to 2.
//
//    Rev 1.17   25 Apr 1998 23:26:34   jtaylor
// Added/modified the original file data/time structures.
//
//    Rev 1.16   24 Apr 1998 15:40:02   jtaylor
// Added AP header structure, cleaned up some fields.
//
//    Rev 1.15   10 Apr 1998 00:11:32   SEDWARD
// Added REGVAL_QUARANTINE_OPTIONS_PATH_STR macro.
//
//    Rev 1.14   09 Apr 1998 21:05:48   JTaylor
// Added support for NTK and VXD applications
// Removed AddFileToQuarantine from SYM_WIN applications.
//
//    Rev 1.13   02 Apr 1998 17:07:00   JTaylor
// Added a default header size define.
//
//    Rev 1.12   02 Apr 1998 14:24:46   JTaylor
// Added a header size field to the header
//
//    Rev 1.11   23 Mar 1998 14:30:20   SEDWARD
// Include 'rpc.h'.
//
//    Rev 1.10   16 Mar 1998 19:21:44   SEDWARD
// Include 'Quar32.h'.
//
//    Rev 1.8   11 Mar 1998 18:49:10   SEDWARD
// Updated DWORD status/type values, 'fileStatus' now 'dwFileStatus'.
//
//    Rev 1.7   11 Mar 1998 18:11:16   SEDWARD
// Added 'type' DWORD flag #defines; added 'type' DWORD member to the file
// info structure; removed the appID member from the file info structure.
//
//    Rev 1.6   09 Mar 1998 22:34:14   SEDWARD
// Added 'GetQuarantinePath'.
//
//    Rev 1.5   09 Mar 1998 19:40:18   SEDWARD
// Added 1-byte pragma around structures.
//
//    Rev 1.4   04 Mar 1998 16:06:28   DBuches
// Changed DWORD date fields to SYSTEMTIME structures.
//
//    Rev 1.3   26 Feb 1998 22:24:26   jtaylor
// Changed AddFileToQuarantine to take 2 arguments, Changed data types in AddFileToQuarantine to not be pointers.
//
//    Rev 1.2   26 Feb 1998 12:53:06   jtaylor
// Updated the header on the AddFileToQuarantine method
//
//    Rev 1.1   26 Feb 1998 11:21:16   jtaylor
// Moved the function declarations to the h file.
//
//    Rev 1.0   26 Feb 1998 11:10:18   jtaylor
// Initial revision.
//
//-----------------------------------------------------------------------------
#ifndef QUARADD_H
#define QUARADD_H

#include <rpc.h>                        // For UUID definition
#include "Quar32.h"
#include "n30type.h"

#define MAX_QUARANTINE_EXTENSION_LENGTH     260
#define MAX_QUARANTINE_FILENAME_LEN         260
#define MAX_QUARANTINE_MACHINE_DOMAIN_LEN   50
#define MAX_QUARANTINE_MACHINE_NAME_LEN     50
#define MAX_QUARANTINE_OWNER_NAME_LEN       50
#define MAX_QUARANTINE_FILES                0xFFFFFFF

#define QUARANTINE_SIGNATURE                0xAAAA20CD
#define QUARANTINE_VERSION                  0x2
#define QUARANTINE_AP_VERSION               0x1

// values for getting the Quarantine path values
#define REGKEY_QUARANTINE_PATH                  _T("SOFTWARE\\Symantec\\Symantec AntiVirus\\Quarantine")
#define REGVAL_QUARANTINE_PATH_STR              _T("QuarantinePath")
#define REGVAL_QUARANTINE_INCOMING_PATH_STR     _T("IncomingPath")
#define REGVAL_QUARANTINE_TEMP_PATH_STR         _T("TempPath")
#define REGVAL_QUARANTINE_OPTIONS_PATH_STR      _T("OptionsPath")
#define REGVAL_QUARANTINE_PRODUCT_NAME          _T("Symantec AntiVirus")
#define REGVAL_QUARANTINE_PATHKEY               _T("Quarantine")
#define TARGET_QUARANTINE_PATH                  1
#define TARGET_QUARANTINE_INCOMING_PATH         2
#define TARGET_QUARANTINE_TEMP_PATH             3


#define QUARANTINE_TRANSFER_BUFFER_SIZE         0x8000

#pragma pack( push, 1 )                 // Ensure structure packing is set to 1

typedef struct  tagQFILE_AP_INFO_STRUCT
{
    DWORD               dwFileStatus;
    DWORD               dwFileType;
    SYSTEMTIME          stDateQuarantined;
    FILETIME            ftOriginalFileDateCreated;
    FILETIME            ftOriginalFileDateAccessed;
    FILETIME            ftOriginalFileDateWritten;
    DWORD               dwOriginalFileSize;
    char                szCurrentFilename[MAX_QUARANTINE_FILENAME_LEN];
    char                szOriginalAnsiFilename[MAX_QUARANTINE_FILENAME_LEN];

} QFILE_AP_INFO_STRUCT, FAR*  LPQFILE_AP_INFO_STRUCT;
#define SQFILE_AP_INFO_STRUCT sizeof(QFILE_AP_INFO_STRUCT)

typedef struct  tagQFILE_AP_HEADER_STRUCT      // actual physical file header
{
    DWORD                           dwSignature;
    DWORD                           dwVersion;
    DWORD                           dwHeaderBytes;
    QFILE_AP_INFO_STRUCT            FileInfo;

} QFILE_AP_HEADER_STRUCT, FAR*  LPQFILE_AP_HEADER_STRUCT;
#define SQFILE_AP_HEADER_STRUCT sizeof(QFILE_AP_HEADER_STRUCT)

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
    N30                 VirusInfo;

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

#pragma pack( pop )                     // Return packing to previous value

                                        // This is the size of the quarantine header.  8000 bytes.
#define DEFAULT_HEADER_BUFFER_SIZE 8000

#ifndef SYM_WIN

BOOL AddFileToQuarantine (
    PTSTR  pszSourceFileName,
    DWORD  dwStatus,
    PTSTR  pszQuarantinePath,
    PTSTR  pszQuarFileName,
    PHFILE phQuarFile
    );

BOOL AddFileToQuarantineComp (
    PTSTR pszSourceFileName,
    PTSTR pszQuarFileName,
    HFILE hQuarFile,
    BOOL  bCancel
    );


#endif

#ifdef SYM_WIN
    HANDLE CreateUniqueFile
    (   LPTSTR          lpszTargetPath,
        LPTSTR          lpszExtension,
        LPTSTR          lpszFileName
    );
#endif

void ReadQFileInfo( LPQFILE_INFO_STRUCT lpqfiInfo, HANDLE lphFile );

BOOL GetQuarantinePath(LPTSTR          lpszOutBuf,
                       DWORD           nBufSize,
                       int             nTargetPathInfo);


#endif QUARADD_H
