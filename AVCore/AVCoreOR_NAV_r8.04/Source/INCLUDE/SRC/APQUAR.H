#ifndef QUARADD_H
#define QUARADD_H

// Quarantine file status (out of QUAR32.H)
#define QFILE_STATUS_HEADER_INCOMPLETE                      0x00000000
#define QFILE_STATUS_HEADER_COMPLETE                        0x00000001
#define QFILE_STATUS_QUARANTINED                            0x00000002
#define QFILE_STATUS_SUBMITTED                              0x00000004
#define QFILE_STATUS_BACKUP_FILE                            0x00000008
#define QFILE_STATUS_ACKNOWLEDGED                           0x00000010

#define MAX_QUARANTINE_EXTENSION_LENGTH     260
#define MAX_QUARANTINE_FILENAME_LEN         260

#define QUARANTINE_SIGNATURE                0xAAAA20CD
#define QUARANTINE_AP_VERSION               0x1

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

typedef struct  tagQFILE_AP_HEADER_STRUCT      // actual physical file header
{
    DWORD                           dwSignature;
    DWORD                           dwVersion;
    DWORD                           dwHeaderBytes;
    QFILE_AP_INFO_STRUCT            FileInfo;

} QFILE_AP_HEADER_STRUCT, FAR*  LPQFILE_AP_HEADER_STRUCT;

#pragma pack( pop )                     // Return packing to previous value

                                        // This is the size of the quarantine header.  8000 bytes.
#define DEFAULT_HEADER_BUFFER_SIZE 8000

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

#endif QUARADD_H
