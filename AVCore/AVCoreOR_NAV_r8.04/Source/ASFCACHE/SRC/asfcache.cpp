//**********************************************************************
//
// $Header:   S:/ASFCACHE/VCS/asfcache.cpv   1.2   21 May 1997 18:58:42   BILL  $
//
// Description: Contains implementation of the Already Scanned File
//              Cache module. Copied from NAVNLM, see the 'CACHE'
//              project of MARIO or JATAYU for significant history.
//
//**********************************************************************
// $Log:   S:/ASFCACHE/VCS/asfcache.cpv  $
// 
//    Rev 1.2   21 May 1997 18:58:42   BILL
// Added stub routines for X86 asm routines for Alpha.  Need second pass to implement bodies 
// (this stub will allow other projects to build)
// 
//    Rev 1.1   13 May 1997 17:46:12   JBRENNA
// Ported from Tesla.
// 
//    Rev 1.0   13 Mar 1997 15:06:56   RFULLER
// Initial revision
// 
//    Rev 1.0   08 Nov 1996 12:14:30   DKEEFER
// Initial revision
// 
//    Rev 1.8   23 Aug 1996 19:54:02   TSmith
// Added APIs to query cache by file identifier.
//
//    Rev 1.7   11 Jul 1996 21:22:14   TSmith
// 'AsfFlushCache' now properly reinitializes the max entries field of the
// file blocks that it clears.
//
//    Rev 1.6   11 Jul 1996 12:40:54   TSmith
// Changed 'GetFileIdentifier' to open files using only FILE_ATTRIBUTES access.
// Removed redundant calls to MEMSET. Modified 'AsfInitCache' to accept
// number of cache entries rather than number of blocks. 'ValidateCache' now
// requires VALIDATE_ASFCACHE to be defined.
//
//    Rev 1.5   06 Jun 1996 11:36:56   RSTANEV
// Debug messages will now show only if MONITOR_ASFCACHE_STATUS is defined.
//
//    Rev 1.4   17 May 1996 16:38:00   RSTANEV
// Replaced DbgPrint() calls with KdPrint().
//
//    Rev 1.3   16 May 1996 07:24:02   TSmith
// Modified 'AsfAddFileToCache' for better synchronization, removed some calls
// to 'ValidateCache' to restrict its usage to exported routines.
//
//    Rev 1.2   14 May 1996 09:18:46   RSTANEV
// Changed to use SYMSYNC.H.
//
//    Rev 1.1   13 May 1996 18:49:48   TSmith
// Added synchronization code. Appropriate functions now
// lock cache access with a fast mutex object.
//
//    Rev 1.0   02 May 1996 16:18:06   TSmith
// Initial revision.
//**********************************************************************
#include <platform.h>                   // Quake standard
#include <file.h>                       // FileOpen(), FileClose()

#pragma pack( push, 1 )                 // Set struct packing to 1

#include "asfcache.h"

//**********************************************************************
//**** Defines *********************************************************
//**********************************************************************

// error return codes
#define COK                     0x0000
#define FUNCTION_ERROR          0x0001

#define CACHE_INIT_FAIL         0x0001

// Flag bytes telling ReadFileStatus() & SetFileStatus() where call originated
#define LOC_OPEN_FILE           0x01
#define LOC_CLOSE_FILE          0x02

// control code for memory allocation limits
#define MAX_DIR_BLOCKS          0x04
#define DIR_BLOCK_SIZE          0x1000

// status bytes for FindFile(), FindDir()
#define ID_FOUND                0x0002
#define ID_NOT_FOUND            0x0004
#define ID_NOT_FOUND_NO_DATA    0x0008

// status bytes for LocateDirBlock()
#define PFILE_IN_BLOCK          0x0002
#define PFILE_NOT_IN_BLOCK      0x0003
#define PFILE_ABOVE_BLOCK       0x0004
#define PFILE_BELOW_BLOCK       0x0008

// status bytes for BinSearch()
#define NO_DATA_FOUND           0x0002
#define MATCH_NOT_FOUND         0x0004
#define MATCH_FOUND             0x0008


// binary search function is called with pointer to this
typedef struct
    {
    ULONGLONG nSearchID;
    DWORD    dwBlockStart;
    DWORD    dwStructSize;
    DWORD    dwStructCount;
    DWORD    dwIDOffset;
    DWORD*   dwMatchAddr;
    } BINSEARCH, *PBINSEARCH;

//
// The following 2 structs have parallel declarations in the
// file 'CACHEMOV.ASM' Any changes to the structs here should
// also be made in that file.
//

typedef struct
    {
    ULONGLONG nFileID;                   // file unique id
    ULONGLONG nFileTime;                 // last access time (offset from base)
    } FILEPTR, *PFILEPTR;

// this thing is overlaid (laid over) the first entrys of DIRPTR and FILEPTR
//  to store upper and lower ID ranges in the cell.
typedef struct
    {
    ULONGLONG nUpper;                    // highest file id in block
    ULONGLONG nLower;                    // lowest file id in block
    ULONGLONG nTimeBase;                 // last access time base
    DWORD    dwReserved;
    WORD     wFlag;                     // flag bits
    WORD     wMaxEntries;               // max number of entries
    WORD     wCount;                    // number of entries
    WORD     wOffset;                   // offset of first entry
    } LIMIT, *PLIMIT;

typedef LIMIT   FAR *LPLIMIT;
typedef LPLIMIT FAR *LPLPLIMIT;

// image of a cache buffer - including the header
typedef struct tagCACHEBUFFER
    {
    LIMIT   Header;                     // buffer header
    FILEPTR FileEntries[1];             // cache entries - variable size
    } CACHEBUFFER, *PCACHEBUFFER;

#define LockCache() ExAcquireFastMutex( &AsfCacheMutex )
#define UnLockCache() ExReleaseFastMutex( &AsfCacheMutex )
#define CACHE_ENTRY_SIZE ( sizeof( FILEPTR ) )
#define MIN_CACHE_ENTRIES ( ( FILE_BLOCK_SIZE * DEFAULT_FILE_BLOCKS ) / CACHE_ENTRY_SIZE )
#define MAX_CACHE_ENTRIES ( ( FILE_BLOCK_SIZE * MAX_FILE_BLOCKS ) / CACHE_ENTRY_SIZE )


//**********************************************************************
//**** Local Function Protos *******************************************
//**********************************************************************

WORD FindFile(
    IN  ULONGLONG  nFileID,
    OUT PFILEPTR* pFile,
    OUT DWORD*    dwBlockNum
    );

VOID InsertFile(
    IN ULONGLONG nFileID
    );

VOID DeleteOldFiles(
    VOID
    );

WORD LocateFileBlock(
    IN  ULONGLONG nFileID,
    OUT DWORD*   dwFileBlock
    );

WORD BinSearch(
    IN PBINSEARCH pBin
    );

WORD BalancePFILECells(
    IN DWORD dwBlockNum
    );

VOID Balance(
    VOID
    );

VOID Adjust(
    IN DWORD dwTo,                      // destination
    IN DWORD dwFrom,                    // source
    IN WORD  wCount                     // number of elements desired in dest
    );

VOID Center(
    IN PLIMIT pLimit                    // center file block
    );

NTSTATUS GetFileIdentifier(
    IN  LPCTSTR    FileName,            // fully qualified file name
    OUT PULONGLONG FileIndex
    );


//**********************************************************************
//** ASM routines that use 32 bit cpu instructions to manage buffers ***
//**********************************************************************
extern "C"
    {
    WORD WINAPI MOVEDOWNANDINSERT(
        PFILEPTR pLowPFILE,
        DWORD    dwMatchAddr,
        WORD     wSize
        );

    WORD WINAPI MOVEUPANDINSERT(
        PFILEPTR pHighPFILE,
        DWORD    dwMatchAddr,
        WORD     wSize
        );

    WORD WINAPI MOVEDOWNANDCLOSE(
        PFILEPTR pHighPFILE,
        PFILEPTR pFilePtr,
        WORD     wSize
        );

    WORD WINAPI MOVEUPANDCLOSE(
        PFILEPTR pLowPFILE,
        PFILEPTR pFilePtr,
        WORD     wSize
        );

    VOID WINAPI BackCopy(
        IN LPVOID pDest,                    // copy backwards (top to bottom)
        IN LPVOID pSrc,
        IN WORD   wCount
        );
    }   // extern "C"


//**********************************************************************
//**** Local Data ******************************************************
//**********************************************************************

static DWORD      FileBlockCount;
static DWORD      FileBlockSize;
static DWORD      MaxFileBlocks;
static LPLPLIMIT  pFileBlocks      = NULL;
static BOOLEAN    CacheInitialized = FALSE;
static FAST_MUTEX AsfCacheMutex    = { 0 };


//**********************************************************************
//**** Begin Functions *************************************************
//**********************************************************************

//
// Make 'ValidateCache()' disappear if we're not
// debugging. This reduces the number of
// #ifdef ( SYM_DEBUG ) statements floating
// around the code.
//
#if !defined( VALIDATE_ASFCACHE )
#define ValidateCache()
#endif

//
// Make debug print statements disappear if
// MONITOR_ASFCACHE_STATUS not defined.
//
#if defined( MONITOR_ASFCACHE_STATUS )
#define AsfCacheDbgMsg(x,y,z) DbgPrint(x,y,z)
#else
#define AsfCacheDbgMsg(x,y,z)
#endif

//***************************************************************************
// ValidateCache()
//
// Description: verifies the cache - asserts if anything is amiss!
//
//***************************************************************************
// 11/21/1994 JMILLARD Function Created.
//***************************************************************************
#if defined( VALIDATE_ASFCACHE )
VOID ValidateCache( VOID )
    {
    PCACHEBUFFER    pCacheBuffer;
    PLIMIT          pHeader;
    PFILEPTR        CacheEntries;
    long int        i,j,k;

    // see if there is a cache

    if ( pFileBlocks != NULL )
        {
        // walk through the blocks and check them
        for ( i = 0; i < FileBlockCount; i++ )
            {
            pCacheBuffer = ( PCACHEBUFFER )pFileBlocks[ i ];

            if ( pCacheBuffer != NULL )
                {
                pHeader = &pCacheBuffer->Header;
                CacheEntries = pCacheBuffer->FileEntries;

                // verify the header parameters

                SYM_ASSERT( pHeader->nUpper >= pHeader->nLower );
                SYM_ASSERT( pHeader->wMaxEntries ==
                            ( FileBlockSize - sizeof( LIMIT ) ) / sizeof( FILEPTR ) );
                SYM_ASSERT( pHeader->wOffset <= pHeader->wMaxEntries );
                SYM_ASSERT( pHeader->wOffset + pHeader->wCount <=
                            pHeader->wMaxEntries );

                // verify the upper and lower links are consistent

                if ( i > 0 )
                    {
                    PCACHEBUFFER pPrevBuffer = ( PCACHEBUFFER )pFileBlocks[ i - 1 ];
                    PLIMIT       pPrevHeader = &pPrevBuffer->Header;

                    if ( pPrevHeader->wCount != 0 && pHeader->wCount != 0 )
                        {
                        SYM_ASSERT( pHeader->nLower > pPrevHeader->nUpper );
                        }
                    }

                if ( i < (long int)FileBlockCount - 2 && pFileBlocks[ i + 1 ] )
                    {
                    PCACHEBUFFER pNextBuffer = ( PCACHEBUFFER )pFileBlocks[ i + 1 ];
                    PLIMIT       pNextHeader = &pNextBuffer->Header;

                    if ( pNextHeader->wCount != 0 && pHeader->wCount != 0 )
                        {
                        SYM_ASSERT( pHeader->nUpper < pNextHeader->nLower );
                        }
                    }

                // verify all unused entries are 0

                for ( j = 0; j < pHeader->wMaxEntries; j++ )
                    {
                    if ( j < pHeader->wOffset || j > ( pHeader->wOffset + pHeader->wCount - 1 ) )
                        {
                        SYM_ASSERT( CacheEntries[ j ].nFileID   == 0 &&
                                    CacheEntries[ j ].nFileTime == 0 );
                        }
                    }

                // verify the entries themselves

                for ( j = 0; j < pHeader->wCount; j++ )
                    {
                    k = j + pHeader->wOffset;

                    // verify sort, and consistency with upper and lower limits
                    if ( j == 0 )
                        {
                        SYM_ASSERT( CacheEntries[ k ].nFileID = pHeader->nLower );
                        }
                    if ( j == pHeader->wCount-1 )
                        {
                        SYM_ASSERT( CacheEntries[ k ].nFileID = pHeader->nUpper );
                        }
                    if ( j > 0 )
                        {
                        SYM_ASSERT( CacheEntries[ k ].nFileID > CacheEntries[ k - 1 ].nFileID );
                        }
                    if ( j < pHeader->wCount-2 )
                        {
                        SYM_ASSERT( CacheEntries[ k ].nFileID < CacheEntries[ k + 1 ].nFileID );
                        }
                    }
                }
            }
        }
    }   // ValidateCache
#endif  // VALIDATE_ASFCACHE



//**********************************************************************
// AsfIsFileInCache()
//
// Description: Determines if a given file is present in the cache
//              structure.
//
// Entry: A fully qualified file name such as:
//        'C:\WINNT35\SYSTEM32\KERNEL32.DLL'
//
// Exit: Returns TRUE if the file is present in the cache, FALSE if not
//       or if an error occurs retrieving the unique file identifier or
//       reading the cache.
//
//**********************************************************************
// 05/02/1996 Converted for use with WinNT kernel mode - TSMITH
// 06/15/1993 Created - DDREW
//**********************************************************************
BOOLEAN AsfIsFileInCache( IN LPCTSTR FileName, OUT PULONGLONG UniqueFileId )
    {
    NTSTATUS    Status;
    ULONGLONG   FileIndex;
    PFILEPTR    pFilePtr;
    DWORD       dwBlockNum;
    DWORD       dwResult;

    SYM_ASSERT( FileName != NULL );
    SYM_ASSERT( CacheInitialized );

    //
    // Return FALSE if there's no cache
    //
    if ( ! CacheInitialized )
        {
        return FALSE;
        }

    //
    // Retrieve unique file identifier returning FALSE
    // if we can't get it
    //
    Status = GetFileIdentifier( FileName, &FileIndex );

    if ( ! NT_SUCCESS( Status ) )
        {
        return FALSE;
        }

    //
    // If UniqueFileId is not NULL, assume that the caller
    // wants the 64-bit file identifier returned in its
    // buffer.
    //
    if ( UniqueFileId != NULL )
        {
        *UniqueFileId = FileIndex;
        }

    //
    // Look for the file ID in the cache
    //
    LockCache( );
    AsfCacheDbgMsg( "%s: Searching cache for %S\n", __FILE__, FileName );
    dwResult = FindFile( FileIndex, &pFilePtr, &dwBlockNum );
    ValidateCache( );
    UnLockCache( );

    if ( dwResult == ID_FOUND )
        {
        AsfCacheDbgMsg( "%s: %S cached\n", __FILE__, FileName );
        return TRUE;
        }
    else
        {
        AsfCacheDbgMsg( "%s: %S not cached\n", __FILE__, FileName );
        return FALSE;
        }
    }   // AsfIsFileInCache

//**********************************************************************
// AsfIsFileIdInCache()
//
// Description: Determines if a given file is present in the cache
//              structure using an identifier passed in by caller.
//
// Entry: The unique, 64-bit identifier of a file.
//
// Exit: Returns TRUE if the ID is present in the cache, FALSE if not.
//
//**********************************************************************
// 08/23/1996 Function created. - TSMITH
//**********************************************************************
BOOLEAN AsfIsFileIdInCache( IN PULONGLONG UniqueFileId )
    {
    PFILEPTR    pFilePtr;
    DWORD       dwBlockNum;
    DWORD       dwResult;

    SYM_ASSERT( CacheInitialized );
    SYM_ASSERT( UniqueFileId != NULL );

    //
    // Return FALSE if there's no cache or nothing is
    // passed in...
    //
    if ( ! CacheInitialized || ! UniqueFileId )
        {
        return FALSE;
        }

    //
    // Look for the file ID in the cache
    //
    LockCache( );
    dwResult = FindFile( *UniqueFileId, &pFilePtr, &dwBlockNum );
    ValidateCache( );
    UnLockCache( );

    if ( dwResult == ID_FOUND )
        {
        return TRUE;
        }
    else
        {
        return FALSE;
        }
    }   // AsfIsFileIdInCache



//**********************************************************************
// AsfAddFileToCache()
//
// Description: Inserts a files unique 64-bit identifier into the ASF
//              cache structure if it isn't already there.
//
// Entry: A fully qualifed file name such as:
//        'C:\WINNT\SYSTEM32\KERNEL32.DLL'
//
// Exit: Returns STATUS_SUCCESS upon insertion of file info into cache,
//       or STATUS_UNSUCCESSFUL if cache has not been initialized or the
//       file info could not be added to the cache.
//
// See Also: AsfIsFileInCache(), InsertFile()
//
//**********************************************************************
// 05/06/1996 Created - TSMITH
//**********************************************************************
NTSTATUS AsfAddFileToCache( IN LPCTSTR FileName )
    {
    ULONGLONG   FileIndex;
    NTSTATUS    Status = STATUS_SUCCESS;

    SYM_ASSERT( FileName != NULL );

    __try
        {
        if ( ! CacheInitialized )
            {
            AsfCacheDbgMsg( "%s: Cache not initialized\n", __FILE__, NULL );
            Status = STATUS_UNSUCCESSFUL;
            __leave;
            }

        Status = GetFileIdentifier( FileName, &FileIndex );

        if ( ! NT_SUCCESS( Status ) )
            {
            AsfCacheDbgMsg( "%s: Unable to retrieve index for %S\n", __FILE__, FileName );
            Status = STATUS_UNSUCCESSFUL;
            __leave;
            }

        AsfCacheDbgMsg( "%s: Inserting %S into cache\n", __FILE__, FileName );
        LockCache( );
        InsertFile( FileIndex );
        ValidateCache( );
        UnLockCache( );
        }
    __finally
        {
        return Status;
        }
    }   // AsfAddFileToCache



//**********************************************************************
// AsfAddFileIdToCache()
//
// Description: Inserts a files unique 64-bit identifier into the ASF
//              cache structure if it isn't already there.
//
// Entry: The unique, 64-bit identifier of a file.
//
// Exit: Returns STATUS_SUCCESS upon insertion of file ID into cache,
//       or STATUS_UNSUCCESSFUL if cache has not been initialized or the
//       file ID could not be added to the cache.
//
// See Also: AsfIsFileInCache(), InsertFile()
//
//**********************************************************************
// 08/23/1996 Created - TSMITH
//**********************************************************************
NTSTATUS AsfAddFileIdToCache( IN PULONGLONG UniqueFileId )
    {
    NTSTATUS    Status = STATUS_SUCCESS;

    SYM_ASSERT( CacheInitialized );
    SYM_ASSERT( UniqueFileId != NULL );

    __try
        {
        if ( ! CacheInitialized )
            {
            AsfCacheDbgMsg( "%s: Cache not initialized\n", __FILE__, NULL );
            Status = STATUS_UNSUCCESSFUL;
            __leave;
            }

        LockCache( );
        InsertFile( *UniqueFileId );
        ValidateCache( );
        UnLockCache( );
        }
    __finally
        {
        return Status;
        }
    }   // AsfAddFileIdToCache



//**********************************************************************
// FindFile()
//
// Description : Try and locate Parent Directory
//
// Entry : Parent File id
//
// Exit : Null if ok, errcode if fail
//
// See Also: InsertFile(), RemoveFile(), Binsearch()
//
//**********************************************************************
// 05/02/1996 Converted for use with WinNT kernel mode - TSMITH
// 06/15/1993 Created - DDREW
//**********************************************************************
WORD FindFile( ULONGLONG nFileID, PFILEPTR *pFile, DWORD *dwBlockNum )
    {
    LARGE_INTEGER liCurTime;             // Returned by KeQueryTickCount
    BINSEARCH     Bin;
    PFILEPTR      pLowPFILE;
    PLIMIT        pLimit;
    DWORD         dwMatchAddr;
    DWORD         dwFileBlock;
    WORD          wRet;
    WORD          wErr;

    *dwBlockNum = 0;

    //
    // Find the file block in which this pFile may reside
    //
    wErr = LocateFileBlock( nFileID, &dwFileBlock );

    // act upon result of LocateFileBlock()
    switch( wErr )
        {
        case PFILE_ABOVE_BLOCK :
            {
            wRet = ID_NOT_FOUND;
            break;
            }

        case PFILE_BELOW_BLOCK :
            {
            wRet = ID_NOT_FOUND;
            break;
            }

        case PFILE_IN_BLOCK :
            {
            pLimit = pFileBlocks[ dwFileBlock ];
            *dwBlockNum = dwFileBlock;

            // get the address of the lowest pFile in the cell
            pLowPFILE = ( PFILEPTR )( pLimit + 1 );
            pLowPFILE += pLimit->wOffset;

            if( pLimit->wCount == 0 )
                {
                *pFile = pLowPFILE;
                wRet = ID_NOT_FOUND_NO_DATA;
                goto errexit;
                }

            // do binary search to locate pFile, if findable
            Bin.dwBlockStart    = ( DWORD )pLowPFILE;
            Bin.dwStructSize    = sizeof( FILEPTR );
            Bin.dwStructCount   = pLimit->wCount;
            Bin.nSearchID       = nFileID;
            Bin.dwIDOffset      = 0;
            Bin.dwMatchAddr     = &dwMatchAddr;

            wErr = BinSearch( &Bin );

            // act upon result of BinSearch()
            switch( wErr )
                {
                case NO_DATA_FOUND :
                    {
                    // in this case of empty buffer, dwMatchAddr will point
                    //  to center of buffer

                    *pFile = ( PFILEPTR )dwMatchAddr;
                    wRet = ID_NOT_FOUND_NO_DATA;
                    break;
                    }

                case MATCH_NOT_FOUND :
                    {
                    *pFile = 0;
                    wRet = ID_NOT_FOUND;
                    break;
                    }

                case MATCH_FOUND :
                    {
                    *pFile = ( PFILEPTR )dwMatchAddr;
                    KeQueryTickCount( &liCurTime );
                    (*pFile)->nFileTime = liCurTime.QuadPart - pLimit->nTimeBase;
                    wRet = ID_FOUND;
                    break;
                    }

                default:
                    {
                    *pFile = 0;
                    wRet = FUNCTION_ERROR;
                    }

                } // switch BinSearch()
            } // case PFILE_IN_BLOCK
        } // switch LocateFileBlock()

errexit:

    return( wRet );
    }


//**********************************************************************
// InsertFile()
//
// Description : Lru Search routine for File structures
//
//          Note: This function is always called when a new FILE entry
//                 is needed. It determines weather or not to remove
//                  an old entry based upon space in buffer.
//
// Entry : dword ID, pointer to pointer to location of new data
//
//
//        CASE 1: No data is in the cell;
//
//              - A pointer is returned to the center of the cell
//
//        CASE 2: A FILEPTR is found with a matching ID
//
//              - A pointer is returned to the FILEPTR
//
//        CASE 3: Data is in the cell but none matches this FILEPTR
//
//              - A "hole" is created, Pointer to hole returned
//
//        CASE 4: No more room is available in any cell for this item
//
//              - the FILEPTR with the lowest Lru value is removed, the
//                 structure adjusted, and a Pointer returned to that addr
//
// Exit :
//
// See Also:
//
//**********************************************************************
// 05/02/1996 Converted for use with WinNT kernel mode - TSMITH
// 06/16/1993 Created - DDREW
//**********************************************************************
VOID InsertFile( ULONGLONG nFileID )
    {
    LARGE_INTEGER liCurTime;
    BINSEARCH     Bin;
    PFILEPTR      pNewFile;
    PFILEPTR      pLowPFILE;
    PFILEPTR      pMidPFILE;
    PFILEPTR      pHighPFILE;
    PFILEPTR      pLastPFILE;
    PLIMIT        pLimit;
    PLIMIT        pLimit2;
    DWORD         dwMatchAddr;
    DWORD         dwFileBlock;
    WORD          wErr;

start:
    wErr = LocateFileBlock( nFileID, &dwFileBlock );

    // act upon result of LocateFileBlock()
    if( wErr == PFILE_ABOVE_BLOCK )
        {
        // if ID is above this block, check size of next block and insert
        //  in smaller of the two
        pLimit = pFileBlocks[ dwFileBlock ];

        if( ( dwFileBlock + 1 ) < FileBlockCount )
            {
            pLimit2 = pFileBlocks[ dwFileBlock + 1 ];
            if( pLimit->wCount > pLimit2->wCount )
                dwFileBlock++;
            }
        }
    else if( wErr == PFILE_BELOW_BLOCK )
        {
        // if ID is below this block, check size of prev block and insert
        //  in smaller of the two
        pLimit = pFileBlocks[ dwFileBlock ];

        if ( dwFileBlock )
            {
            pLimit2 = pFileBlocks[ dwFileBlock - 1 ];

            if( pLimit->wCount > pLimit2->wCount )
                dwFileBlock--;
            }
        }

    // this case also includes the case of wErr == PFILE_IN_BLOCK
    pLimit = pFileBlocks[ dwFileBlock ];

    // get the address of the lowest pFile in the cell
    pLowPFILE =  ( PFILEPTR )( pLimit + 1 );
    pLowPFILE += pLimit->wOffset;

    // do this only if cell is empty
    if( pLimit->wCount == 0 )
        {
        // fill in cache entry info
        pLowPFILE->nFileID = nFileID;
        pLowPFILE->nFileTime = 0;

        // update file block header
        pLimit->wCount = 1;
        pLimit->nUpper = nFileID;
        pLimit->nLower = nFileID;

        // set the time of initial cache entry
        KeQueryTickCount( &liCurTime );
        pLimit->nTimeBase = liCurTime.QuadPart;

        // done with first entry for this block
        goto errexit;
        }

    // see if this cell has room, if not, try and re-organize things
    if( pLimit->wMaxEntries == pLimit->wCount )
        {
        wErr = BalancePFILECells( dwFileBlock );

        // re-do limit locate
        wErr = LocateFileBlock( nFileID, &dwFileBlock );
        pLimit = pFileBlocks[ dwFileBlock ];

        // get the address of the lowest pFile in the cell
        pLowPFILE =  ( PFILEPTR )( pLimit + 1 );
        pLowPFILE += pLimit->wOffset;
        }
    else if ( pLimit->wCount < pLimit->wMaxEntries )
        {
        pHighPFILE = pLowPFILE + pLimit->wCount - 1;

        // get the address of the last pFile in the cell
        pLastPFILE =  ( PFILEPTR )( pLimit + 1 );
        pLastPFILE += ( pLimit->wMaxEntries - 1 );

        if ( pLimit->wOffset == 0 || pHighPFILE == pLastPFILE )
            {
            Center( pLimit );
            pLowPFILE =  ( PFILEPTR )( pLimit + 1 );
            pLowPFILE += pLimit->wOffset;
            }
        }

    // do binary search to locate pFile, if findable
    Bin.dwBlockStart    = ( DWORD )pLowPFILE;
    Bin.dwStructSize    = sizeof( FILEPTR );
    Bin.dwStructCount   = pLimit->wCount;
    Bin.nSearchID       = nFileID;
    Bin.dwIDOffset      = 0;
    Bin.dwMatchAddr     = &dwMatchAddr;

    wErr = BinSearch( &Bin );

    switch( wErr )
        {
        case MATCH_FOUND:
            {
            break;
            }

        case MATCH_NOT_FOUND:
            {
            // if problem with binsearch, it will ret a null
            if( dwMatchAddr == 0 )
                break;

            // be sure buffer not full, if so find lru and use it
            if ( pLimit->wMaxEntries == pLimit->wCount )
                {
                DeleteOldFiles( );      // delete old entries
                goto start;             // and try again
                } // if buffer full, use lru

            // buffer not completely full, add to one side of center

            pHighPFILE = pLowPFILE;
            pHighPFILE += ( pLimit->wCount - 1) ;

            // get the address of the last pFile in the cell
            pLastPFILE =  ( PFILEPTR )( pLimit + 1 );
            pLastPFILE += ( pLimit->wMaxEntries - 1 );

            // get the address of the middle pFile in the cell
            pMidPFILE =  pLowPFILE;
            pMidPFILE += ( pLimit->wCount - 1 ) / 2;

            // make decision about which way to scoot data
            if( ( ( pHighPFILE == pLastPFILE ) || ( dwMatchAddr <= ( DWORD )pMidPFILE ) ) &&
                ( pLimit->wOffset != 0 ) )
                {
                if( nFileID < ( ( PFILEPTR )dwMatchAddr )->nFileID )
                    dwMatchAddr -= sizeof( FILEPTR );

                wErr = MOVEDOWNANDINSERT( pLowPFILE, dwMatchAddr, sizeof( FILEPTR ) );
                pLimit->wOffset--;
                }
            else
                {
                if( nFileID > ( ( PFILEPTR )dwMatchAddr )->nFileID )
                    dwMatchAddr += sizeof( FILEPTR );

                wErr = MOVEUPANDINSERT( pHighPFILE, dwMatchAddr, sizeof( FILEPTR ) );
                }

            // update the data in the new "hole" and block header

            pLimit->wCount++;
            KeQueryTickCount( &liCurTime );
            pNewFile = ( PFILEPTR )dwMatchAddr;
            pNewFile->nFileID = nFileID;
            pNewFile->nFileTime = liCurTime.QuadPart - pLimit->nTimeBase;

            if ( nFileID > pLimit->nUpper )
                pLimit->nUpper = nFileID;

            if ( nFileID < pLimit->nLower )
                pLimit->nLower = nFileID;

            break;
            } // case MATCH_NOT_FOUND :
        } // switch BinSearch()

errexit:

    return;
}


//**********************************************************************
// AsfRemoveFileFromCache()
//
// Description: Performs the opposite operation of 'InsertFile',
//              retrieving the given files 64-bit identifier, searching
//              the cache and removing its entry if present.
//
// Entry: A fully qualified file name such as:
//        'C:\WINNT35\SYSTEM32\KERNEL32.DLL'
//
// Exit: Returns STATUS_SUCCESS upon successful completion or
//       STATUS_UNSUCCESSFUL if the 64-bit ID could not be retrieved
//       or a problem occurs reading the cache.
//
//**********************************************************************
// 05/02/1996 Converted for use with WinNT kernel mode - TSMITH
// 08/13/1993 Created - EHU
//**********************************************************************
NTSTATUS AsfRemoveFileFromCache( IN LPCTSTR FileName )
    {
    NTSTATUS    Status = STATUS_SUCCESS;
    ULONGLONG   FileIndex;
    PFILEPTR    pFile;
    PFILEPTR    pLowFile;
    PFILEPTR    pHighFile;
    PLIMIT      pLimit;
    DWORD       dwFileBlock;

    SYM_ASSERT( FileName != NULL );

    if ( ! CacheInitialized )
        {
        AsfCacheDbgMsg( "%s: Cache not initialized\n", __FILE__, NULL );
        return STATUS_UNSUCCESSFUL;
        }

    __try
        {
        Status = GetFileIdentifier( FileName, &FileIndex );
        LockCache();

        if ( ! NT_SUCCESS( Status ) )
            {
            AsfCacheDbgMsg( "%s: Unable to retrieve index for %S\n", __FILE__, FileName );
            Status = STATUS_UNSUCCESSFUL;
            __leave;
            }

        if ( FindFile( FileIndex, &pFile, &dwFileBlock ) != ID_FOUND )
            {
            AsfCacheDbgMsg( "%s: Remove of %S from cache failed\n", __FILE__, FileName );
            Status = STATUS_UNSUCCESSFUL;
            __leave;
            }

        AsfCacheDbgMsg( "%s: Balancing cache after removal of %S\n", __FILE__, FileName );
        pLimit    =  pFileBlocks[ dwFileBlock ];
        pLowFile  =  ( PFILEPTR )( pLimit + 1 );
        pLowFile  += pLimit->wOffset;
        pHighFile =  pLowFile + pLimit->wCount - 1;

        if ( pFile >= pLowFile + ( pLimit->wCount - 1 ) / 2 )
            {
            MOVEDOWNANDCLOSE( pHighFile, pFile, sizeof( FILEPTR ) );
            pLimit->nUpper = ( --pHighFile )->nFileID;
            pLimit->nLower = pLowFile->nFileID;
            }
        else
            {
            MOVEUPANDCLOSE( pLowFile, pFile, sizeof( FILEPTR ) );
            pLimit->wOffset++;
            pLimit->nUpper = pHighFile->nFileID;
            pLimit->nLower = ( ++pLowFile )->nFileID;
            }

        if ( --pLimit->wCount == 0 )
            pLimit->wOffset = pLimit->wMaxEntries / 2;

        }
    __finally
        {
        ValidateCache();
        UnLockCache();
        return Status;
        }
    }   // AsfRemoveFileFromCache



//**********************************************************************
// AsfRemoveFileIdFromCache()
//
// Description: Removes a files 64-bit identifier from the ASF cache.
//
// Entry: The files unique, 64-bit identifier.
//
// Exit: Returns STATUS_SUCCESS upon successful completion or
//       STATUS_UNSUCCESSFUL or a problem occurs reading the cache.
//
//**********************************************************************
// 08/23/1996 Function created. - TSMITH
//**********************************************************************
NTSTATUS AsfRemoveFileIdFromCache( IN PULONGLONG UniqueFileId )
    {
    NTSTATUS    Status = STATUS_SUCCESS;
    PFILEPTR    pFile;
    PFILEPTR    pLowFile;
    PFILEPTR    pHighFile;
    PLIMIT      pLimit;
    DWORD       dwFileBlock;

    SYM_ASSERT( CacheInitialized );
    SYM_ASSERT( UniqueFileId != NULL );

    if ( ! CacheInitialized )
        {
        AsfCacheDbgMsg( "%s: Cache not initialized\n", __FILE__, NULL );
        return STATUS_UNSUCCESSFUL;
        }

    __try
        {
        LockCache();

        if ( FindFile( *UniqueFileId, &pFile, &dwFileBlock ) != ID_FOUND )
            {
            AsfCacheDbgMsg( "%s: Removal of file from cache failed\n", __FILE__, NULL );
            Status = STATUS_UNSUCCESSFUL;
            __leave;
            }

        AsfCacheDbgMsg( "%s: Balancing cache after removal of file\n", __FILE__, NULL );
        pLimit    =  pFileBlocks[ dwFileBlock ];
        pLowFile  =  ( PFILEPTR )( pLimit + 1 );
        pLowFile  += pLimit->wOffset;
        pHighFile =  pLowFile + pLimit->wCount - 1;

        if ( pFile >= pLowFile + ( pLimit->wCount - 1 ) / 2 )
            {
            MOVEDOWNANDCLOSE( pHighFile, pFile, sizeof( FILEPTR ) );
            pLimit->nUpper = ( --pHighFile )->nFileID;
            pLimit->nLower = pLowFile->nFileID;
            }
        else
            {
            MOVEUPANDCLOSE( pLowFile, pFile, sizeof( FILEPTR ) );
            pLimit->wOffset++;
            pLimit->nUpper = pHighFile->nFileID;
            pLimit->nLower = ( ++pLowFile )->nFileID;
            }

        if ( --pLimit->wCount == 0 )
            pLimit->wOffset = pLimit->wMaxEntries / 2;

        }
    __finally
        {
        ValidateCache();
        UnLockCache();
        return Status;
        }
    }   // AsfRemoveFileIdFromCache



//**********************************************************************
// DeleteOldFiles()
//
// Description : Delete old file entries from all buffers
//               and update time base to current time
//
//**********************************************************************
// 05/02/1996 Converted for use with WinNT kernel mode - TSMITH
// 08/05/1993 Created - EHU
//**********************************************************************
VOID DeleteOldFiles( VOID )
    {
    LARGE_INTEGER liCurTime;
    ULONGLONG     nTimeDiff;
    ULONGLONG     nHalfTime;
    ULONGLONG     nFileTime;
    PFILEPTR      pFile;
    PFILEPTR      pHighFile;
    PFILEPTR      pLowFile;
    PFILEPTR      pNextFile;
    PFILEPTR      pLastFile;
    PLIMIT        pLimit;
    DWORD         dwBlockNum;
    WORD          wOldCount;

    KeQueryTickCount( &liCurTime );

    for ( dwBlockNum = 0; dwBlockNum < FileBlockCount; dwBlockNum++ )
        {
        pLimit = pFileBlocks[dwBlockNum];
        pLowFile = (PFILEPTR) (pLimit + 1);
        pLowFile += pLimit->wOffset;
        pHighFile = pLowFile + pLimit->wCount - 1;
        wOldCount = pLimit->wCount;

        nTimeDiff = liCurTime.QuadPart - pLimit->nTimeBase;
        nHalfTime = nTimeDiff / 2;

        for ( pFile = pLowFile; pFile <= pHighFile; )
            {
            if ( ( nFileTime = pFile->nFileTime ) && nFileTime > nHalfTime )
                {
                if ( nFileTime > nTimeDiff )
                    pFile->nFileTime -= nTimeDiff;
                else
                    pFile->nFileTime = 0;

                pFile++;
                continue;
                }

            for ( pNextFile = pFile + 1; pNextFile < pHighFile; pNextFile++ )
                {
                if ( ( nFileTime = pNextFile->nFileTime ) && nFileTime > nHalfTime )
                    break;
                }

            if ( pNextFile >= pHighFile )
                {
                if ( ( nFileTime = pHighFile->nFileTime ) == 0 ||
                     nFileTime < nHalfTime )
                    {
                    pNextFile = --pHighFile;
                    }
                }

            if ( pNextFile > pFile )
                {
                MEMCPY( pFile, pNextFile, ( pHighFile - pNextFile + 1 ) * sizeof( FILEPTR ) );
                pHighFile -= pNextFile - pFile;
                }
            }

        pLimit->wCount = pHighFile - pLowFile + 1;

                                        // always nuke at least one entry
                                        // to prevent deadlock
        if ( pLimit->wCount == pLimit->wMaxEntries )
            {
            pHighFile--;
            pLimit->wCount--;
            }

        pLastFile = ( PFILEPTR )( pLimit + 1 );
        pLastFile += pLimit->wMaxEntries - 1;

        if ( pHighFile < pLastFile )
            MEMSET( pHighFile + 1, 0, ( pLastFile - pHighFile ) * sizeof( FILEPTR ) );

        pLimit->nTimeBase += nTimeDiff;

        if ( pLimit->wCount != wOldCount )
            {
            pLimit->nUpper = pHighFile->nFileID;
            pLimit->nLower = pLowFile->nFileID;
            }
        }

    if ( FileBlockCount == 1 )
        {
        Center( pLimit );
        }
    else
        {
        Balance( );
        }

}   //DeleteOldFiles


//**********************************************************************
// LocateFileBlock()
//
// Description: This function compares the supplied FileID with info
//              stored in the header of each memory cell. It returns
//              a flag indicating the relationship of the FileID with
//              a particular cell.
//
// Entry: FileID and back pointer for the block number.
//
// Exit: Status word PFILE_IN_BLOCK, PFILE_ABOVE_BLOCK, PFILE_BELOW_BLOCK.
//
//**********************************************************************
// 05/02/1996 Converted for use with WinNT kernel mode - TSMITH
// 06/16/1993 Created - DDREW
//**********************************************************************
WORD LocateFileBlock( ULONGLONG nFileID, DWORD *dwFileBlock )
    {
    PLIMIT  pLimit;
    PLIMIT  pLowerLimit;
    WORD    wRet;
    UINT    nCnt;

    wRet = 0;

    // first find the block that this Fileptr wants to reside in
    for ( nCnt = 0; nCnt < FileBlockCount; nCnt++ )
        {
        pLimit = pFileBlocks[ nCnt ];

        if( ( nFileID <= pLimit->nUpper ) && ( nFileID >= pLimit->nLower ) )
            {
            *dwFileBlock = nCnt;
            wRet = PFILE_IN_BLOCK;
            goto skipout;
            }
        }

    // see if lower than lowest block
    pLimit = pFileBlocks[ 0 ];
    if ( nFileID < pLimit->nLower )
        {
        *dwFileBlock = 0;
        wRet = PFILE_BELOW_BLOCK;
        goto skipout;
        }


    // see if higher than highest block
    pLimit = pFileBlocks[ FileBlockCount - 1 ];
    if ( nFileID > pLimit->nUpper )
        {
        *dwFileBlock = ( FileBlockCount - 1 );
        wRet = PFILE_ABOVE_BLOCK;
        goto skipout;
        }

    // see if in between any block
    for ( nCnt = 0; nCnt < FileBlockCount - 1; nCnt++ )
        {
        pLowerLimit = pFileBlocks[ nCnt ];
        pLimit = pFileBlocks[ nCnt + 1 ];

        if ( ( nFileID < pLimit->nLower || pLimit->nLower == 0 ) &&
             nFileID > pLowerLimit->nUpper )
            {
            *dwFileBlock = nCnt;
            wRet = PFILE_ABOVE_BLOCK;
            goto skipout;
            }
        }

skipout:

    return( wRet );

    }   // LocateFileBlock


//**********************************************************************
// BinSearch()
//
// Description: Binary Search routine.
//
// Entry: BINSEARCH structure.
//
// Exit: TRUE or FALSE meaning Found or NotFound. BackPointer in
//       BINSEARCH structure will contain Pointer to match
//       or next lowest address if not a match.
//
//**********************************************************************
// 05/02/1996 Converted for use with WinNT kernel mode - TSMITH
// 06/15/1993 Created - DDREW (actually EHU wrote the code)
//**********************************************************************
WORD BinSearch( PBINSEARCH pBin )
    {
    ULONGLONG nCalcID;
    LPBYTE   ptr;
    LPBYTE   pBase;
    LPBYTE   pTop;
    DWORD    dwTwoSize;
    DWORD    dwSize;
    WORD     wErr;

    dwSize    = pBin->dwStructSize;
    dwTwoSize = dwSize + dwSize;

    // point pBase at first FILEPTR in block
    pBase = ( LPBYTE )pBin->dwBlockStart;

    // point pTop at last FILEPTR in block
    pTop  = pBase + dwSize * ( pBin->dwStructCount - 1 );

    while ( pTop >= pBase )
        {
        // point ptr at FILEPTR in middle of FILEBLOCK
        ptr = pBase + dwSize * ( ( pTop - pBase ) / dwTwoSize );

        // grab the key value
        nCalcID = *( PULONGLONG )( ptr + pBin->dwIDOffset );

        // if the stored key matches search key
        if ( nCalcID == pBin->nSearchID )
            {
            wErr = MATCH_FOUND;
            // we're done
            goto found;
            }
        // if stored key is greater, look 'lower' in file block
        else if ( nCalcID > pBin->nSearchID )
            pTop = ptr - dwSize;
        else
            // else stored key is less, look 'higher' in file block
            pBase = ptr + dwSize;
        }

    wErr = MATCH_NOT_FOUND;

found:
    *pBin->dwMatchAddr = (DWORD) ptr;

    return( wErr );
    }


//**********************************************************************
// Center()
//
// Description: File block centering routine.
//
// Entry: Pointer to beginning of file block (LIMIT structure).
//
//**********************************************************************
// 08/04/1993 Created - EHU
//**********************************************************************
VOID Center( PLIMIT pLimit )
    {
    PFILEPTR pFile;
    WORD     wNewOff;

    wNewOff = ( pLimit->wMaxEntries - pLimit->wCount ) / 2;
    pFile = ( PFILEPTR )( pLimit + 1 );

    if (wNewOff < pLimit->wOffset)
        {
        MEMCPY( &pFile[ wNewOff ],         // copy down
               &pFile[ pLimit->wOffset ],
               pLimit->wCount * sizeof( FILEPTR ) );

                                        // clear to end of block
        MEMSET( &pFile[ wNewOff + pLimit->wCount ], 0,
           ( pLimit->wMaxEntries - pLimit->wCount - wNewOff ) * sizeof( FILEPTR ) );
        }
    else if ( wNewOff > pLimit->wOffset )
        {
        BackCopy( &pFile[ wNewOff ],       // copy up
                  &pFile[ pLimit->wOffset ],
                  pLimit->wCount * sizeof( FILEPTR ) );

                                        // clear to end of block
        MEMSET( pFile, 0, wNewOff * sizeof( FILEPTR ) );
        }

    pLimit->wOffset = wNewOff;

    }


//**********************************************************************
// BalancePFILECells()
//
// Description: Called when a memory cell containing FILEPTR structures
//              fills up or changes. This routine attempts to balance
//              the pfiles accross the available memory blocks.
//
// Entry: dwFileBlock - block that's full.
//
//**********************************************************************
// 06/17/1993 Created - DDREW
//**********************************************************************
WORD BalancePFILECells( DWORD dwFileBlock )
    {
    PLIMIT  pLim;
    DWORD   dwBlockNum;
    WORD    wErr = COK;

    if ( FileBlockCount < MaxFileBlocks )
        {
        AsfCacheDbgMsg( "%s: Allocating %u bytes\n", __FILE__, FileBlockSize );
        pLim = ( PLIMIT )MemAllocPtr( GHND, FileBlockSize );

        if ( pLim )
            {
            pLim->wMaxEntries = ( WORD )( FileBlockSize - sizeof( LIMIT ) ) / sizeof( FILEPTR );
            pLim->wOffset = pLim->wMaxEntries / 2;

            for ( dwBlockNum = FileBlockCount; dwBlockNum > dwFileBlock; dwBlockNum-- )
                pFileBlocks[ dwBlockNum ] = pFileBlocks[ dwBlockNum - 1 ];

            pFileBlocks[ dwFileBlock ] = pLim;
            FileBlockCount++;
            }
        }

    Balance();
    return(wErr);
    }


//**********************************************************************
// Balance()
//
// Description: Balance the pfiles in all the memory blocks.
//
//**********************************************************************
// 06/22/1993 Created - EHU
//**********************************************************************
VOID Balance( VOID )
    {
    BOOLEAN bMoveBack = TRUE;
    WORD wAvgCount;
    INT  i, j, k, lastidx;
    INT  nAdjust, nAdj;

    for ( i = 0, wAvgCount = 0; i < FileBlockCount; i++ )
        wAvgCount += pFileBlocks[ i ]->wCount;

    wAvgCount = ( WORD )( ( wAvgCount + FileBlockCount - 1 ) / FileBlockCount );
    lastidx = -1;

    for ( i = 0, nAdjust = 0; i < FileBlockCount - 1; i++ )
        {
        nAdjust += pFileBlocks[ i ]->wCount - wAvgCount;

        if ( nAdjust > 0 )
            {
            if ( lastidx == - 1 )
                lastidx = i;

            bMoveBack = FALSE;
            continue;
            }

        if ( bMoveBack == FALSE )
            {
            k = i;
            if ( nAdjust < 0 )
                {
                j = 1;
                do
                    {
                    Adjust( k, k - j, wAvgCount + nAdjust );
                    nAdj = pFileBlocks[ k ]->wCount - wAvgCount - nAdjust;
                    }

                while ( nAdj < 0 && k - ++j >= lastidx )
                    /* NULL Statement */ ;

                k--;
                }

            for ( ; k > lastidx; k-- )
                {
                j = 1;
                do
                    {
                    Adjust( k, k - j, wAvgCount );
                    nAdj = pFileBlocks[ k ]->wCount - wAvgCount;
                    }

                while ( nAdj < 0 && k - ++j >= lastidx )
                    /* NULL Statement */ ;

                }

            lastidx = -1;
            bMoveBack = TRUE;
            }

        if ( nAdjust < 0 )
            {
            j = 1;
            do
                {
                Adjust( i, i + j, wAvgCount );
                nAdjust = pFileBlocks[ i ]->wCount - wAvgCount;
                }

            while ( nAdjust < 0 && i + ++j < FileBlockCount )
                /* NULL Statement */ ;

            }
        }
    }   // Balance


//**********************************************************************
// Adjust()
//
// Description: Move pfiles from one block to another.
//
// Entry: pTo - destination block
//        pFrom - source block
//        wCount - number of elements destination block should have
//
//**********************************************************************
// 06/22/1993 Created - EHU
//**********************************************************************
VOID Adjust(
    DWORD   dwTo,                       // destination index
    DWORD   dwFrom,                     // source index
    WORD    wCount)                     // number of elements desired in dest
{
    PFILEPTR pToFile;
    PFILEPTR pFromFile;
    PLIMIT   pTo;
    PLIMIT   pFrom;
    WORD     wMoveCnt;
    WORD     wNewToOff;
    WORD     wNewFromOff;

    pTo         = pFileBlocks[ dwTo ];
    pFrom       = pFileBlocks[ dwFrom ];
    pToFile     = ( PFILEPTR )( pTo + 1 );
    pFromFile   = ( PFILEPTR )( pFrom + 1 );

    if ( wCount <= pTo->wCount )
        goto done;

    wMoveCnt = wCount - pTo->wCount;

    if ( wMoveCnt > pFrom->wCount )
        wMoveCnt = pFrom->wCount;

    wNewToOff = ( pTo->wMaxEntries - pTo->wCount - wMoveCnt ) / 2;

    if ( dwFrom > dwTo )
        {
        if ( wNewToOff < pTo->wOffset )
            {
            MEMCPY( &pToFile[ wNewToOff ],                 // copy down
                    &pToFile[ pTo->wOffset ],
                    pTo->wCount * sizeof( FILEPTR ) );
            }
        else if ( wNewToOff > pTo->wOffset )
            {
            BackCopy( &pToFile[ wNewToOff ],               // copy up
                      &pToFile[ pTo->wOffset ],
                      pTo->wCount * sizeof( FILEPTR ) );
            }

        MEMCPY( &pToFile[ wNewToOff + pTo->wCount ],
                &pFromFile[ pFrom->wOffset ],
                wMoveCnt * sizeof( FILEPTR ) );

        pFrom->wCount -= wMoveCnt;
        wNewFromOff = ( pFrom->wMaxEntries - pFrom->wCount ) / 2;

        MEMCPY( &pFromFile[ wNewFromOff ],                 // copy down
                &pFromFile[ pFrom->wOffset + wMoveCnt ],
                pFrom->wCount * sizeof( FILEPTR ) );
        }
    else if ( dwFrom < dwTo )
        {
        if ( wNewToOff + wMoveCnt < pTo->wOffset )
            {
            MEMCPY( &pToFile[ wNewToOff + wMoveCnt ],      // copy down
                    &pToFile[ pTo->wOffset ],
                    pTo->wCount * sizeof( FILEPTR ) );
            }
        else
            {
            BackCopy( &pToFile[ wNewToOff + wMoveCnt ],    // copy up
                      &pToFile[ pTo->wOffset ],
                      pTo->wCount * sizeof( FILEPTR ) );
            }

        MEMCPY( &pToFile[ wNewToOff ],
                &pFromFile[ pFrom->wOffset + pFrom->wCount - wMoveCnt ],
                wMoveCnt * sizeof( FILEPTR ) );

        pFrom->wCount -= wMoveCnt;
        wNewFromOff = ( pFrom->wMaxEntries - pFrom->wCount ) / 2;

        BackCopy( &pFromFile[ wNewFromOff ],               // copy up
                 &pFromFile[ pFrom->wOffset ],
                 pFrom->wCount * sizeof( FILEPTR ) );
        }

    MEMSET( &pFromFile[ wNewFromOff + pFrom->wCount ], 0,
           ( pFrom->wMaxEntries - pFrom->wCount - wNewFromOff ) * sizeof( FILEPTR ) );

    MEMSET( pFromFile, 0, wNewFromOff * sizeof( FILEPTR ) );

    pFrom->wOffset = wNewFromOff;

    pTo->wOffset = wNewToOff;
    pTo->wCount += wMoveCnt;

    pTo->nUpper = pToFile[ pTo->wOffset + pTo->wCount - 1 ].nFileID;
    pTo->nLower = pToFile[ pTo->wOffset ].nFileID;

    pFrom->nUpper = pFromFile[ pFrom->wOffset + pFrom->wCount - 1 ].nFileID;
    pFrom->nLower = pFromFile[ pFrom->wOffset ].nFileID;

done:

    return;
}   // Adjust


//**********************************************************************
// AsfInitCache()
//
// Description: Creates and initializes the Already Scanned File Cache.
//
// Entry: Maximum number of entries to cache.
//
// Exit: STATUS_SUCCESS on successful completion, or
//       STATUS_INSUFFICIENT_RESOURCES on memory allocation failure.
//
//**********************************************************************
// 05/02/1996 Function created - TSMITH
//**********************************************************************
NTSTATUS AsfInitCache( IN DWORD MaxEntries )
    {
    NTSTATUS Status = STATUS_SUCCESS;
    BOOLEAN  Init   = TRUE;

    SYM_ASSERT( ( MaxEntries >= MIN_CACHE_ENTRIES ) && ( MaxEntries <= MAX_CACHE_ENTRIES ) );

    //
    // Only initialize once
    //
    if ( CacheInitialized )
        {
        return Status;
        }

    //
    // Initialize our synchronization object
    //
    ExInitializeFastMutex( &AsfCacheMutex );

	//
    // Determine the maximum number of file blocks to ever allocate
    // based on the number of cache entries requested. Use default settings
    // if function parameter is out of range.
    //
    if ( ( MaxEntries < MIN_CACHE_ENTRIES ) || ( MaxEntries > MAX_CACHE_ENTRIES ) )
        {
        MaxFileBlocks = DEFAULT_FILE_BLOCKS;
        }
    else
        {
        //
        // Convert 'MaxEntries' parameter to number of file blocks
        // (memory pages) required, rounding up to the next highest.
        //
        MaxFileBlocks = ( DWORD )( ( MaxEntries * CACHE_ENTRY_SIZE ) / FILE_BLOCK_SIZE );
        MaxFileBlocks++;
        }

    __try
        {
        //
        // Allocate memory to hold an array of LIMIT structure pointers
        //
        AsfCacheDbgMsg( "%s: Allocating %u bytes\n", __FILE__, MaxFileBlocks * sizeof( *pFileBlocks ) );
        pFileBlocks = ( LPLPLIMIT )MemAllocPtr( GHND, MaxFileBlocks * sizeof( *pFileBlocks ) );

        if ( pFileBlocks == NULL )
            {
            AsfCacheDbgMsg( "%s: Can't allocate memory for cache pointers\n", __FILE__, NULL );
            Status = STATUS_INSUFFICIENT_RESOURCES;
            Init = FALSE;
            __leave;
            }

        FileBlockCount = 1;

        //
        // Allocate memory for the first data block
        //
        FileBlockSize = FILE_BLOCK_SIZE;
        AsfCacheDbgMsg( "%s: Allocating %u bytes\n", __FILE__, FileBlockSize );
        pFileBlocks[ 0 ] = ( PLIMIT )MemAllocPtr( GHND, FileBlockSize );

        if ( pFileBlocks[ 0 ] == NULL )
            {
            AsfCacheDbgMsg( "%s: Can't allocate memory for cache data\n", __FILE__, NULL );
            AsfCacheDbgMsg( "%s: Releasing %u bytes\n", __FILE__, MaxFileBlocks * sizeof( *pFileBlocks ) );
            MemFreePtr( pFileBlocks );
            pFileBlocks = NULL;
            Status = STATUS_INSUFFICIENT_RESOURCES;
            Init = FALSE;
            __leave;
            }

        //
        // Initialize max entries for header of first file block.
        //
        ( ( PLIMIT )pFileBlocks[ 0 ] )->wMaxEntries = ( WORD )( FileBlockSize - sizeof( LIMIT ) ) / sizeof( FILEPTR );
        }
    __finally
        {
        CacheInitialized = Init;

        #if defined( SYM_DEBUG )

        if ( CacheInitialized )
            {
            LockCache();
            ValidateCache();
            UnLockCache();
            }

        #endif

        return Status;
        }
    }   // AsfInitCache


//**********************************************************************
// AsfDeInitCache()
//
// Description: Clears contents of Already Scanned File Cache and
//              releases dynamically allocated memory.
//
// Exit: Returns STATUS_SUCCESS
//
// Note: This function is safe to call if the cache has not been
//       initialized. Any modifications should retain a check for
//       cache status.
//
//**********************************************************************
// 05/02/1996 Function created - TSMITH
//**********************************************************************
NTSTATUS AsfDeInitCache( VOID )
    {
    NTSTATUS Status = STATUS_SUCCESS;
    PLIMIT   pLimit;
    UINT     i;

    //
    // Don't deinit something that's not there
    //
    if ( ! CacheInitialized )
        {
        return( Status );
        }

    LockCache();
    CacheInitialized = FALSE;

    // clear File block ptr array to null
    if ( pFileBlocks != NULL )
        {

        // release the memory of each file block
        for ( i = 0; i < FileBlockCount; i++ )
            {
            pLimit = pFileBlocks[ i ];

            if ( pLimit != NULL )
                {
                AsfCacheDbgMsg( "%s: Releasing %u bytes\n", __FILE__, sizeof( *pLimit ) );
                MemFreePtr( pLimit );
                pFileBlocks[ i ] = NULL;
                }
            else
                break;
            }

        // release the memory for the array of LIMIT pointers
        AsfCacheDbgMsg( "%s: Releasing %u bytes\n", __FILE__, MaxFileBlocks * sizeof( *pFileBlocks ) );
        MemFreePtr( pFileBlocks );
        pFileBlocks = NULL;
        FileBlockCount = 0;
        }

    UnLockCache();
    return Status;
    }


//**********************************************************************
// GetFileIdentifier()
//
// Description: Retrieves the unique, 64-bit file identifier associated
//              with each file on a Windows NT system.
//
// Entry: A fully qualified file name such as:
//        'C:\WINNT40\SYSTEM32\KERNEL32.DLL'.
//
//        A pointer to an __int64 (ULONGLONG) to receive the identifier.
//
// Exit: Returns STATUS_UNSUCCESSFUL if the file cannot be opened or the
//       identifier retrieved. Otherwise returns STATUS_SUCCESS.
//
//       The OUT parameter is set to 0 ( zero ) on failure.
//
//**********************************************************************
//  05/01/96 TSmith - Function created.
//**********************************************************************
NTSTATUS GetFileIdentifier( IN LPCTSTR FileName, OUT PULONGLONG FileIndex )
    {
    FILE_INTERNAL_INFORMATION FileInfo;
    IO_STATUS_BLOCK           IoStatus;
    ULONGLONG                  Identifier;
    NTSTATUS                  Status;
    HANDLE                    FileHandle = INVALID_HANDLE_VALUE;

    SYM_ASSERT( FileName != NULL );
    SYM_ASSERT( FileIndex != NULL );

    __try
        {
        //
        // Call Quake to do ugly NTK file open procedures
        //
        FileHandle = ( HANDLE )FileOpen( FileName, 0 );

        if ( FileHandle == INVALID_HANDLE_VALUE )
            {
            AsfCacheDbgMsg( "%s: Unable to open file %S\n", __FILE__, FileName );
            Status = STATUS_UNSUCCESSFUL;
            Identifier = 0;
            __leave;
            }

        //
        // Get the unique file indentifer from WinNT
        //
        AsfCacheDbgMsg( "%s: Retrieving 64-bit index for %S\n", __FILE__, FileName );
        Status = ZwQueryInformationFile( FileHandle,
                                         &IoStatus,
                                         &FileInfo,
                                         sizeof( FILE_INTERNAL_INFORMATION ),
                                         FileInternalInformation );

        //
        // If ZwQueryInformationFile was successful, extract the 64-bit
        // file identifier from the internal info structure
        //
        if ( NT_SUCCESS( Status ) )
            {
            Identifier = FileInfo.IndexNumber.QuadPart;
            AsfCacheDbgMsg( "%s: Retrieved unique index for %S\n", __FILE__, FileName );
            Status = STATUS_SUCCESS;
            }
        else
            {
            AsfCacheDbgMsg( "%s: Unable to retrieve index for %S\n", __FILE__, FileName );
            Identifier = 0;
            Status = STATUS_UNSUCCESSFUL;
            }
        }
    __finally
        {
        if ( FileHandle != INVALID_HANDLE_VALUE )
            FileClose( ( HFILE )FileHandle );

        *FileIndex = Identifier;
        return Status;
        }

    }   // GetFileIdentifier


//**********************************************************************
// AsfFlushCache()
//
// Description: Clears the contents of all file blocks
//
// Exit: Returns STATUS_UNSUCCESSFUL if the cache doesn't exist,
//       otherwise STATUS_SUCCESS.
//
//**********************************************************************
//  05/02/96 TSmith - Function created.
//**********************************************************************
NTSTATUS AsfFlushCache( VOID )
    {
    WORD MaxEntries;

    if ( ! CacheInitialized )
        {
        return STATUS_UNSUCCESSFUL;
        }

    //
    // Precalculate maximum entries for a file block
    //
    MaxEntries = ( WORD )( FileBlockSize - sizeof( LIMIT ) ) / sizeof( FILEPTR );

    LockCache();

    for ( INT i = 0; i < FileBlockCount; i++ )
        {
        MEMSET( pFileBlocks[ i ], 0, FileBlockSize );

        //
        // Initialize max entries for header of first file block.
        //
        ( ( PLIMIT )pFileBlocks[ i ] )->wMaxEntries = MaxEntries;
        }

    UnLockCache();
    return STATUS_SUCCESS;
    }

#ifndef _X86_
WORD WINAPI MoveDownAndInsert( PFILEPTR pLowPFILE, DWORD dwMatchAddr, WORD wSize )
{
	return 0;
}

WORD WINAPI MoveUpAndInsert( PFILEPTR pHighPFILE, DWORD dwMatchAddr, WORD wSize )
{
	return 0;
}

WORD WINAPI MoveDownAndClose( PFILEPTR pHighPFILE, PFILEPTR pFilePtr, WORD wSize )
{
	return 0;
}

WORD WINAPI MoveUpAndClose( PFILEPTR pLowPFILE )
{
	return 0;
}

// copy backwards (top to bottom)
VOID WINAPI BackCopy(LPVOID pDest, LPVOID pSrc, WORD wCount)
{
	return;
}
#endif

#pragma pack( pop )
