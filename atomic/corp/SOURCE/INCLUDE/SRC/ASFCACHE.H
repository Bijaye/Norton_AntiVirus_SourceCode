// Copyright 1996 - 1997 Symantec, Peter Norton Group
//**********************************************************************
//
// $Header:   S:/INCLUDE/VCS/asfcache.h_v   1.2   25 Apr 1997 08:39:06   JBRENNA  $
//
// Description: Contains public definitions and function protoypes for
//              the Already Scanned File Cache module.
//
//**********************************************************************
// $Log:   S:/INCLUDE/VCS/asfcache.h_v  $
// 
//    Rev 1.2   25 Apr 1997 08:39:06   JBRENNA
// Tesla Port: Added prototypes for AsfIsFileIdInCache, AsfAddFileIdToCache,
// and AsfRemoveFileIdFromCache.
// 
//    Rev 1.1   25 Feb 1997 23:06:44   RSTANEV
// Added support for platforms other than SYM_NTK.  Since the code is
// almost "universal", with the only NT-specific parts being the type
// NTSTATUS, the macro NT_STATUS(), some of the NTSTATUS_xxx defines,
// ULONGLONG, and PULONGLONG, these were added directly in this file.
//
//    Rev 1.0   06 Feb 1997 20:51:44   RFULLER
// Initial revision
//
//**********************************************************************

#if !defined( _CACHE_H )
#define _CACHE_H

#if !defined(SYM_NTK)

typedef DWORD NTSTATUS;

#define NT_SUCCESS(x) ((x)<0x80000000L)
#define STATUS_SUCCESS 0
#define STATUS_UNSUCCESSFULL
#define STATUS_INSUFFICIENT_RESOURCES

typedef unsigned __int64 ULONGLONG;
typedef ULONGLONG *PULONGLONG;

#endif

//
// Public defines
//
#define FILE_BLOCK_SIZE         0x1000  // should be MM page size: 4k for Intel CPU
#define MAX_FILE_BLOCKS         0x187   // max file blocks: 100,000 16 byte entries
#define DEFAULT_FILE_BLOCKS     0x10

//
// Public function prototypes
//
NTSTATUS AsfInitCache(
    IN DWORD MaxEntries
);

BOOLEAN AsfIsFileInCache(
    IN  LPCTSTR FileName,
    OUT PULONGLONG UniqueFileId
);

BOOLEAN AsfIsFileIdInCache(
    IN  PULONGLONG UniqueFileId
    );

NTSTATUS AsfAddFileToCache(
    IN LPCTSTR FileName
);

NTSTATUS AsfAddFileIdToCache(
    IN PULONGLONG UniqueFileId
    );

NTSTATUS AsfFlushCache(
    VOID
);

NTSTATUS AsfRemoveFileFromCache(
    IN LPCTSTR FileName
);

NTSTATUS AsfRemoveFileIdFromCache(
    IN PULONGLONG UniqueFileId
    );

NTSTATUS AsfDeInitCache(
    VOID
);

#endif  // _CACHE_H
