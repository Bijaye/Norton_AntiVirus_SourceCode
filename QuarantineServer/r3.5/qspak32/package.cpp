// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 1998, 2005 Symantec Corporation.. All rights reserved.
//*************************************************************************
// package.cpp created 11/6/98 1:57:15 PM
//
// $Header$
//
// Description:
//
// Contains:
//
//*************************************************************************
// $Log$
//*************************************************************************



//  Note:
//  When using pre-compiled headers, for some curious reason, you cannot
//  put ifdefs around #include "stdafx.h". The precompiler will complain
//  about the endif. So I've put ifndef NLM _inside_ stdafx.h
//
#include "StdAfx.h"

#if defined(NLM)
#define FAR far
#elif defined(UNIX)
#define FAR
#endif

#if defined(NLM) || defined(UNIX)
#define MAX_PATH            260
typedef unsigned long       ULONG;
typedef unsigned char       BYTE;
typedef unsigned short      WORD;
typedef BYTE FAR            *LPBYTE;
typedef unsigned long       DWORD;
typedef int                 BOOL;
#endif

#if defined(UNIX)
#define TRUE 1
#define FALSE 0
#define O_BINARY 0
#endif

#if defined(WIN32)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#elif defined(UNIX)

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#elif defined(NLM)

#include <iostream.h>
#include <sys/types.h>
//08-16 This change is so that qsfwd.nlm references to the stat.h in watcom's compiler and not metrowerks

// ksr	1/18/02 - NetWare 6 support
// -------------------------------

// added sys/ 
#include <sys/stat.h>
#include <errno.h>

//bnm update NDK
//#include <io.h>
# include <fcntl.h>
# include <unistd.h>
# include <nwfattr.h>

# define STDIN_HANDLE   0        /* use POSIX STDIN_FILENO */
# define STDOUT_HANDLE  1        /* use POSIX STDOUT_FILENO */
# define STDERR_HANDLE  2        /* use POSIX STDERR_FILENO */

# define ACCESS_RD      0x0004   /* use POSIX R_OK */
# define ACCESS_WR      0x0002   /* use POSIX W_OK */
# define ACCESS_XQ      0x0001   /* use POSIX X_OK */
//bnm end
#endif

#include "qspak.h"
#include "qstructs.h"
#include "qsendian.h"



//*************************************************************************
// Local functions
void xorBuffer( void* pBuffer, ULONG ulSize );
LPDATAFIELD GetDataField( MAP& dataMap, const char * szFieldName );
BOOL ReadDataFields( int iOpenFile, LPQSERVERITEM pItem );
BOOL SaveDataFields( int iOpenFile, LPQSERVERITEM pItem );
void CleanupItem( LPQSERVERITEM pItem );

inline void EndianHeader( QSERVERFILEHEADER *pHeader );
inline void EndianDescriptor( FILEDATADESCRIPTOR* pDescriptor );
inline void EndianDate( LPQSPAKDATE pDate );

///////////////////////////////////////////////////////////////////////////////
//
// Function name: QsPakOpenItem
//
// Description  : This routine will attempt to open an existing Quarantine
//                server file.
//
// Return type  : QSPAKSTATUS possible return vaules:
//                  QSPAKSTATUS_OK
//                  QSPAKSTATUS_ERROR
//                  QSPAKSTATUS_MEMORY_ERROR
//                  QSPAKSTATUS_FILE_NOT_FOUND
//                  QSPAKSTATUS_INVALID_ARG
//
// Argument     : [ in ] char * pszFile - File to open
// Argument     : [ out ] HQSERVERITEM* phItem - recieves handle item
//
///////////////////////////////////////////////////////////////////////////////
// 12/29/98 - DBUCHES: Function created / header added
///////////////////////////////////////////////////////////////////////////////
QSPAKSTATUS QsPakOpenItem( const char * pszFile, HQSERVERITEM* phItem )
{
    int iOpenFile;
    LPQSERVERITEM pItem = NULL;
    QSPAKSTATUS status = QSPAKSTATUS_OK;

    //
    // Validate input
    //
    if( pszFile == NULL || phItem == NULL )
        return QSPAKSTATUS_INVALID_ARG;

    *phItem = NULL;

    //
    // Check signature.
    //
    status = QsPakIsQserverFile( pszFile );
    if( QSPAKSTATUS_OK != QsPakIsQserverFile( pszFile ) )
        {
        return status;
        }

    //
    // Open file for read/write access.
    //
#if defined(WIN32)
     iOpenFile = _open( pszFile, _O_BINARY | _O_RDWR );
#else
	 iOpenFile = open( pszFile, O_BINARY | O_RDWR );
#endif

    if( iOpenFile == -1 )
        {
        switch( errno )
            {
            case EACCES:
                return QSPAKSTATUS_ACCESS_DENIED;

            default:
                return QSPAKSTATUS_FILE_NOT_FOUND;
            }
        }

    //
    // Allocate user data.
    //
    pItem = new QSERVERITEM;
    if( pItem == NULL )
        {
        status = QSPAKSTATUS_MEMORY_ERROR;
        goto BailOut;
        }

    //
    // Save off file name
    //
    strncpy( pItem->szFileName, pszFile, MAX_PATH );

    //
    // Allocate header.
    //
    pItem->pHeader = new QSERVERFILEHEADER;
    if( pItem->pHeader == NULL )
        {
        status = QSPAKSTATUS_MEMORY_ERROR;
        goto BailOut;
        }

    //
    // Read header.
    //

#if defined(WIN32)
    if( sizeof( QSERVERFILEHEADER ) != _read( iOpenFile, pItem->pHeader, sizeof( QSERVERFILEHEADER ) ) )
#else
    if( sizeof( QSERVERFILEHEADER ) != read( iOpenFile, pItem->pHeader, sizeof( QSERVERFILEHEADER ) ) )
#endif

    	{
        goto BailOut;
        }

    xorBuffer( pItem->pHeader, sizeof( QSERVERFILEHEADER ) );

    //
    // Perform Endian conversion.
    //
    EndianHeader( pItem->pHeader );

    //
    // Seek to start of our data area.
    //
#if defined(WIN32)
    if( -1 == _lseek( iOpenFile, pItem->pHeader->ulOriginalFileSize, SEEK_CUR ) )
#else
    if( -1 == lseek( iOpenFile, pItem->pHeader->ulOriginalFileSize, SEEK_CUR ) )
#endif

        {
        //
        // Bad things have happened.
        //
        status = QSPAKSTATUS_BOGUS_FILE;
        goto BailOut;
        }


    //
    // Read in all data fields
    //
    if( FALSE == ReadDataFields( iOpenFile, pItem ) )
        {
        status = QSPAKSTATUS_BOGUS_FILE;
        goto BailOut;
        }

    //
    // Save off pointer.
    //
    *phItem = (HQSERVERITEM)pItem;

BailOut:

#if defined(WIN32)
    _close( iOpenFile );
#else
	close( iOpenFile );
#endif

    //
    // Cleanup
    //
    if( status != QSPAKSTATUS_OK )
        {
        if( pItem )
            {
            if( pItem->pHeader )
                delete pItem->pHeader;
            delete pItem;
            }
        }

    return status;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: QsPakSaveItem
//
// Description  : This routine will commit changes made to the file header
//                to disk.
//
// Return type  : QSPAKSTATUS possible return values
//                  QSPAKSTATUS_OK
//                  QSPAKSTATUS_MEMORY_ERROR
//                  QSPAKSTATUS_FILE_NOT_FOUND
//                  QSPAKSTATUS_INVALID_ARG
//                  QSPAKSTATUS_OUT_OF_DISK_SPACE
//
// Argument     : HQSERVERITEM hItem
//
///////////////////////////////////////////////////////////////////////////////
// 12/29/98 - DBUCHES: Function created / header added
///////////////////////////////////////////////////////////////////////////////
QSPAKSTATUS QsPakSaveItem( HQSERVERITEM hItem )
{
    int iOpenFile;
    LPQSERVERITEM pItem = (LPQSERVERITEM) hItem;
    QSPAKSTATUS status = QSPAKSTATUS_OK;

    //
    // Validate input
    //
    if( pItem == NULL )
        {
        return QSPAKSTATUS_INVALID_ARG;
        }

    //
    // Open file for read/write access.
    //
#if defined(WIN32)
    iOpenFile = _open( pItem->szFileName, _O_BINARY | _O_RDWR );
#else
    iOpenFile = open( pItem->szFileName, O_BINARY | O_RDWR );
#endif

    if( iOpenFile == -1 )
        {
        switch( errno )
            {
            case EACCES:
                return QSPAKSTATUS_ACCESS_DENIED;

            default:
                return QSPAKSTATUS_FILE_NOT_FOUND;
            }
        }

    //
    // Count number of attributes attached to this item
    //
    pItem->pHeader->ulNumDataItems = pItem->dataMap.size();

    //
    // Write file header.
    //
    EndianHeader( pItem->pHeader );
    xorBuffer( pItem->pHeader, sizeof( QSERVERFILEHEADER ) );

#if defined(WIN32)
    if( sizeof( QSERVERFILEHEADER ) != _write( iOpenFile, pItem->pHeader, sizeof( QSERVERFILEHEADER ) ) )
#else
    if( sizeof( QSERVERFILEHEADER ) != write( iOpenFile, pItem->pHeader, sizeof( QSERVERFILEHEADER ) ) )
#endif

        {
        status = ( errno == ENOSPC ) ? QSPAKSTATUS_OUT_OF_DISK_SPACE : QSPAKSTATUS_ACCESS_DENIED;
        goto BailOut;
        }
    xorBuffer( pItem->pHeader, sizeof( QSERVERFILEHEADER ) );
    EndianHeader( pItem->pHeader );

    //
    // Seek to start of our data area.
    //
#if defined(WIN32)
    if( -1 == _lseek( iOpenFile, pItem->pHeader->ulOriginalFileSize, SEEK_CUR ) )
#else
    if( -1 == lseek( iOpenFile, pItem->pHeader->ulOriginalFileSize, SEEK_CUR ) )
#endif

        {
        status = QSPAKSTATUS_ACCESS_DENIED;
        goto BailOut;
        }

    //
    // Save off data fields
    //
    if( FALSE == SaveDataFields( iOpenFile, pItem ) )
        {
        status = QSPAKSTATUS_OUT_OF_DISK_SPACE;
        }

BailOut:

#if defined(WIN32)
    _close( iOpenFile );
#else
    close( iOpenFile );
#endif

    return status;
}



///////////////////////////////////////////////////////////////////////////////
//
// Function name: QsPakCreateFile
//
// Description  : This function will create a new Quarantine Server file on
//                disk, and return a handle to it.
//
// Return type  : QSPAKSTATUS possible return values
//                  QSPAKSTATUS_OK
//                  QSPAKSTATUS_MEMORY_ERROR
//                  QSPAKSTATUS_FILE_NOT_FOUND
//                  QSPAKSTATUS_INVALID_ARG
//                  QSPAKSTATUS_ACCESS_DENIED
//
// Argument     : HQSERVERITEM hItem
// Argument     : const char* pszInfectedFileName
// Argument     : const char* pszDestFileName
//
///////////////////////////////////////////////////////////////////////////////
// 12/29/98 - DBUCHES: Function created / header added
///////////////////////////////////////////////////////////////////////////////
QSPAKSTATUS QsPakCreateFile( HQSERVERITEM* phItem,
                             const char* pszInfectedFileName,
                             const char* pszDestFileName )
{
    int iBytesRead;

#if defined(WIN32)
    struct _stat stats;
#else
    struct stat stats;
#endif

    unsigned int uBytesRemaining;
    BYTE* pXferBuffer = NULL;
    QSPAKSTATUS status = QSPAKSTATUS_OK;
    LPQSERVERITEM    pItem = NULL;
    unsigned int uXferSize = 0x7fff;
    int iNewFile = -1;
    int iSourceFile = -1;

    //
    // Validate input.
    //
    if( phItem == NULL || pszInfectedFileName == NULL || pszDestFileName == NULL )
        {
        return QSPAKSTATUS_INVALID_ARG;
        }

    *phItem = NULL;


    //
    // Allocate transfer buffer
    //
    pXferBuffer = new BYTE[ 0x7fff ];
    if( pXferBuffer == NULL )
        {
        return QSPAKSTATUS_MEMORY_ERROR;
        }

    //
    // Allocate user data.
    //
    pItem = new QSERVERITEM;
    if( pItem == NULL )
        {
        status = QSPAKSTATUS_MEMORY_ERROR;
        goto BailOut;
        }
    pItem->pHeader = NULL;
    pItem->szFileName[0] = 0;

    //
    // Open source file.
    //
#if defined(WIN32)
    iSourceFile = _open( pszInfectedFileName, _O_SEQUENTIAL | _O_BINARY | _O_RDONLY );
#else
	iSourceFile = open( pszInfectedFileName, O_BINARY | O_RDONLY );
#endif

    if( iSourceFile == -1 )
        {
        switch( errno )
            {
            case EACCES:
                status = QSPAKSTATUS_ACCESS_DENIED;
            case ENOENT:
                status = QSPAKSTATUS_FILE_NOT_FOUND;
            default:
                status = QSPAKSTATUS_ERROR;
            }
        goto BailOut;
        }

    //
    // Get size of source file.
    //
#if defined(WIN32)
    if( _fstat( iSourceFile, &stats ) != 0 )
#else
    if( fstat( iSourceFile, &stats ) != 0 )
#endif

        {
        status = QSPAKSTATUS_ACCESS_DENIED;
        goto BailOut;
        }

    //
    // Create new file for writing.
    //
#if defined(WIN32)
    iNewFile = _open( pszDestFileName, _O_CREAT | _O_WRONLY | _O_BINARY, _S_IWRITE );
#else
    iNewFile = open( pszDestFileName, O_CREAT | O_WRONLY | O_BINARY | O_TRUNC, 0);
#endif

    if( iNewFile == -1 )
        {
        status = errno == EACCES ? QSPAKSTATUS_ACCESS_DENIED : QSPAKSTATUS_ERROR;
        goto BailOut;
        }

    //
    // Create new header and write it to disk.
    //
    pItem->pHeader = new QSERVERFILEHEADER;
    if( pItem->pHeader == NULL )
        {
        status = QSPAKSTATUS_MEMORY_ERROR;
        goto BailOut;
        }
    memset( pItem->pHeader, 0, sizeof( QSERVERFILEHEADER ) );
    pItem->pHeader->ulSignature = QSERVER_FILE_SIGNATURE;
    pItem->pHeader->ulVersion = QSERVER_FILE_VERSION;
    pItem->pHeader->ulOriginalFileSize = stats.st_size;

    //
    // Write file header.
    //
    EndianHeader( pItem->pHeader );
    xorBuffer( pItem->pHeader, sizeof( QSERVERFILEHEADER ) );
#if defined(WIN32)
    if( sizeof( QSERVERFILEHEADER ) != _write( iNewFile, pItem->pHeader, sizeof( QSERVERFILEHEADER ) ) )
#else
    if( sizeof( QSERVERFILEHEADER ) != write( iNewFile, pItem->pHeader, sizeof( QSERVERFILEHEADER ) ) )
#endif
        {
        status = ( errno == ENOSPC ) ? QSPAKSTATUS_OUT_OF_DISK_SPACE : QSPAKSTATUS_ACCESS_DENIED;
        goto BailOut;
        }
    xorBuffer( pItem->pHeader, sizeof( QSERVERFILEHEADER ) );
    EndianHeader( pItem->pHeader );

    //
    // Copy data
    //
    uBytesRemaining = stats.st_size;
    while( uBytesRemaining )
        {
        //
        // Read chunk from source
        //
#if defined(WIN32)
        iBytesRead = _read( iSourceFile, pXferBuffer, uXferSize );
#else
        iBytesRead = read( iSourceFile, pXferBuffer, uXferSize );
#endif

        if( iBytesRead == -1 )
            {
            status = QSPAKSTATUS_ERROR;
            goto BailOut;
            }

        //
        // Write chunk to destination
        //
        xorBuffer( pXferBuffer, iBytesRead );

#if defined(WIN32)
        if( iBytesRead != _write( iNewFile, pXferBuffer, iBytesRead ) )
#else
        if( iBytesRead != write( iNewFile, pXferBuffer, iBytesRead ) )
#endif
            {
            status = ( errno == ENOSPC ) ? QSPAKSTATUS_OUT_OF_DISK_SPACE : QSPAKSTATUS_ACCESS_DENIED;
            goto BailOut;
            }

        //
        // Decrement count.
        //
        uBytesRemaining -= (unsigned int )iBytesRead;
        }


    strncpy( pItem->szFileName, pszDestFileName, MAX_PATH );
    *phItem = (HQSERVERITEM) pItem;

BailOut:
    if( status != QSPAKSTATUS_OK && pItem != NULL )
        {
        if( pItem->pHeader )
            delete pItem->pHeader;
        delete pItem;
        }

    if( pXferBuffer != NULL )
        delete [] pXferBuffer;

    if( iSourceFile != -1 )
#if defined(WIN32)
        _close( iSourceFile );
#else
        close( iSourceFile );
#endif

    if( iNewFile != -1 )
#if defined(WIN32)
        _close( iNewFile );
#else
        close( iNewFile );
#endif
    return status;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: QsPakCreateFileFromBuffer
//
// Description  : This function will create a new Quarantine Server file on
//                disk, and return a handle to it.
//
// Return type  : QSPAKSTATUS possible return values
//                  QSPAKSTATUS_OK
//                  QSPAKSTATUS_MEMORY_ERROR
//                  QSPAKSTATUS_FILE_NOT_FOUND
//                  QSPAKSTATUS_INVALID_ARG
//                  QSPAKSTATUS_ACCESS_DENIED
//
// Argument     : HQSERVERITEM hItem
// Argument     : unsigned char* lpDataBuffer
// Argument     : DWORD dwBufferSize
// Argument     : const char* pszDestFileName
//
///////////////////////////////////////////////////////////////////////////////
// 12/29/98 - DBUCHES: Function created / header added
///////////////////////////////////////////////////////////////////////////////
QSPAKSTATUS QsPakCreateFileFromBuffer ( HQSERVERITEM*  phItem,
                                        unsigned char* lpDataBuffer,
                                        DWORD          dwDataBufferSize,
                                        const char*    pszDestFileName )
{
    QSPAKSTATUS     status = QSPAKSTATUS_OK;
    LPQSERVERITEM   pItem = NULL;
    int             iNewFile = -1;
    int             iBytesWritten = 0;


    //
    // Validate input.
    //
    if( phItem == NULL || lpDataBuffer == NULL || dwDataBufferSize == 0 )
        {
        return QSPAKSTATUS_INVALID_ARG;
        }

    *phItem = NULL;

    //
    // Allocate user data.
    //
    pItem = new QSERVERITEM;
    if( pItem == NULL )
        {
        status = QSPAKSTATUS_MEMORY_ERROR;
        goto BailOut;
        }
    pItem->pHeader = NULL;
    pItem->szFileName[0] = 0;

    //
    // Create new file for writing.
    //
#if defined(WIN32)
    iNewFile = _open( pszDestFileName, _O_CREAT | _O_WRONLY | _O_BINARY, _S_IWRITE );
#else
    iNewFile = open( pszDestFileName, O_CREAT | O_WRONLY | O_BINARY | O_TRUNC, 0);
#endif

    if( iNewFile == -1 )
        {
        status = errno == EACCES ? QSPAKSTATUS_ACCESS_DENIED : QSPAKSTATUS_ERROR;
        goto BailOut;
        }

    //
    // Create new header and write it to disk.
    //
    pItem->pHeader = new QSERVERFILEHEADER;
    if( pItem->pHeader == NULL )
        {
        status = QSPAKSTATUS_MEMORY_ERROR;
        goto BailOut;
        }
    memset( pItem->pHeader, 0, sizeof( QSERVERFILEHEADER ) );
    pItem->pHeader->ulSignature = QSERVER_FILE_SIGNATURE;
    pItem->pHeader->ulVersion = QSERVER_FILE_VERSION;
    pItem->pHeader->ulOriginalFileSize = dwDataBufferSize;

    //
    // Write file header.
    //
    EndianHeader( pItem->pHeader );
    xorBuffer( pItem->pHeader, sizeof( QSERVERFILEHEADER ) );
#if defined(WIN32)
    if( sizeof( QSERVERFILEHEADER ) != _write( iNewFile, pItem->pHeader, sizeof( QSERVERFILEHEADER ) ) )
#else
    if( sizeof( QSERVERFILEHEADER ) != write( iNewFile, pItem->pHeader, sizeof( QSERVERFILEHEADER ) ) )
#endif
        {
        status = ( errno == ENOSPC ) ? QSPAKSTATUS_OUT_OF_DISK_SPACE : QSPAKSTATUS_ACCESS_DENIED;
        goto BailOut;
        }
    xorBuffer( pItem->pHeader, sizeof( QSERVERFILEHEADER ) );
    EndianHeader( pItem->pHeader );

    //
    // Write the input buffer to destination
    //
    xorBuffer( lpDataBuffer, dwDataBufferSize );

#if defined(WIN32)
    if( dwDataBufferSize != _write( iNewFile, lpDataBuffer, dwDataBufferSize ) )
#else
    if( dwDataBufferSize != write( iNewFile, lpDataBuffer, dwDataBufferSize ) )
#endif
        {
        status = ( errno == ENOSPC ) ? QSPAKSTATUS_OUT_OF_DISK_SPACE : QSPAKSTATUS_ACCESS_DENIED;
        goto BailOut;
        }

    strncpy( pItem->szFileName, pszDestFileName, MAX_PATH );
    *phItem = (HQSERVERITEM) pItem;

BailOut:
    if( status != QSPAKSTATUS_OK && pItem != NULL )
        {
        if( pItem->pHeader )
            delete pItem->pHeader;
        delete pItem;
        }

    if( iNewFile != -1 )
#if defined(WIN32)
        _close( iNewFile );
#else
        close( iNewFile );
#endif
    return status;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: QsPakUnpackageItem
//
// Description  : This function will extract the orignal infected file from
//                a given quarantine server file item.
//
// Return type  : QSPAKSTATUS possible return values
//                  QSPAKSTATUS_OK
//                  QSPAKSTATUS_MEMORY_ERROR
//                  QSPAKSTATUS_FILE_NOT_FOUND
//                  QSPAKSTATUS_INVALID_ARG
//                  QSPAKSTATUS_ACCESS_DENIED
//
// Argument     : QSERVERITEM hItem
// Argument     : const char * pszDestFileName
//
///////////////////////////////////////////////////////////////////////////////
// 12/29/98 - DBUCHES: Function created / header added
///////////////////////////////////////////////////////////////////////////////
QSPAKSTATUS QsPakUnpackageItem(
                          HQSERVERITEM hItem,
                          const char * pszDestFileName )
{
    int iBytesRead;
    int iNewFile = -1;
    int iSourceFile = -1;
    BYTE* pXferBuffer = NULL;
    QSPAKSTATUS status = QSPAKSTATUS_OK;
    LPQSERVERITEM pItem = (LPQSERVERITEM) hItem;
    unsigned int uXferSize = 0x7fff;
    unsigned int uBytesLeft;
    ULONG ulBytesRead = 0;
    //
    // Validate input
    //
    if( pItem == NULL || pszDestFileName == NULL )
        {
        return QSPAKSTATUS_INVALID_ARG;
        }

    //
    // Allocate transfer buffer
    //
    pXferBuffer = new BYTE[ uXferSize ];
    if( pXferBuffer == NULL )
        {
        return QSPAKSTATUS_MEMORY_ERROR;
        }

    //
    // Open source file.
    //

#if defined(WIN32)
    iSourceFile = _open( pItem->szFileName, _O_SEQUENTIAL | _O_BINARY | _O_RDONLY );
#else
    iSourceFile = open( pItem->szFileName, O_BINARY | O_RDONLY );
#endif
    if( iSourceFile == -1 )
        {
        switch( errno )
            {
            case EACCES:
                status = QSPAKSTATUS_ACCESS_DENIED;
            case ENOENT:
                status = QSPAKSTATUS_FILE_NOT_FOUND;
            default:
                status = QSPAKSTATUS_ERROR;
            }
        goto BailOut;
        }

    //
    // Seek past header
    //
#if defined(WIN32)
    if( -1 == _lseek( iSourceFile, sizeof( QSERVERFILEHEADER ), SEEK_SET ) )
#else
    if( -1 == lseek( iSourceFile, sizeof( QSERVERFILEHEADER ), SEEK_SET ) )
#endif
        {
        status = QSPAKSTATUS_BOGUS_FILE;
        goto BailOut;
        }

    //
    // Create new file for writing.
    //
#if defined(WIN32)
    iNewFile = _open( pszDestFileName, _O_CREAT | _O_WRONLY | _O_BINARY, _S_IWRITE );
#else
    iNewFile = open( pszDestFileName, O_CREAT | O_WRONLY | O_BINARY | O_TRUNC, 0);
#endif
    if( iNewFile == -1 )
        {
        status = errno == EACCES ? QSPAKSTATUS_ACCESS_DENIED : QSPAKSTATUS_ERROR;
        goto BailOut;
        }

    //
    // Save off bytes to file
    //
    while( pItem->pHeader->ulOriginalFileSize != ulBytesRead )
        {
        //
        // Compute proper transfer size.
        //
        uBytesLeft = pItem->pHeader->ulOriginalFileSize - ulBytesRead;
        uXferSize = uBytesLeft < 0x7fff ? uBytesLeft : 0x7fff;

        //
        // Read chunk from source
        //
#if defined(WIN32)
        iBytesRead = _read( iSourceFile, pXferBuffer, uXferSize );
#else
        iBytesRead = read( iSourceFile, pXferBuffer, uXferSize );
#endif
        if( iBytesRead == -1 )
            {
            status = QSPAKSTATUS_ERROR;
            goto BailOut;
            }

        //
        // Write chunk to destination
        //
        xorBuffer( pXferBuffer, iBytesRead );
#if defined(WIN32)
        if( iBytesRead != _write( iNewFile, pXferBuffer, iBytesRead ) )
#else
        if( iBytesRead != write( iNewFile, pXferBuffer, iBytesRead ) )
#endif
            {
            status = ( errno == ENOSPC ) ? QSPAKSTATUS_OUT_OF_DISK_SPACE : QSPAKSTATUS_ACCESS_DENIED;
            goto BailOut;
            }

        //
        // Decrement counter
        //
        ulBytesRead += (ULONG)iBytesRead;
        }


BailOut:
    if( pXferBuffer != NULL )
        delete [] pXferBuffer;

    if( iSourceFile != -1 )
#if defined(WIN32)
        _close( iSourceFile );
#else
      	close( iSourceFile );
#endif

    if( iNewFile != -1 )
#if defined(WIN32)
        _close( iNewFile );
#else
		close( iNewFile );
#endif

    return status;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: QsPakReleaseItem
//
// Description  : Cleans up our allocations
//
// Return type  : QSPAKSTATUS possible return values
//                  QSPAKSTATUS_OK
//
// Argument     : HQSERVERITEM hItem
//
///////////////////////////////////////////////////////////////////////////////
// 12/29/98 - DBUCHES: Function created / header added
///////////////////////////////////////////////////////////////////////////////
QSPAKSTATUS QsPakReleaseItem( HQSERVERITEM hItem )
{
    LPQSERVERITEM pItem = (LPQSERVERITEM) hItem;


    if( pItem == NULL )
        return QSPAKSTATUS_OK;

    CleanupItem( pItem );

    //
    // Clean up the rest.
    //
    delete pItem->pHeader;
    delete pItem;

    return QSPAKSTATUS_OK;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: QsPakQueryItemValue
//
// Description  : This function returns the data specified by by eValue.  If
//                QSPAKSTATUS_BUFFER_TOO_SMALL is returned, then the value
//                pointed to by piBufferLength will be the required size in
//                in bytes, of the data requested.
//
// Return type  : QSPAKSTATUS possible return values
//                  QSPAKSTATUS_OK
//                  QSPAKSTATUS_INVALID_ARG
//                  QSPAKSTATUS_BUFFER_TOO_SMALL
//
// Argument     : HQSERVERITEM hItem
// Argument     : QSERVERITEMINFO eValue
// Argument     : void* pBuffer
// Argument     : int* piBufferLength
//
///////////////////////////////////////////////////////////////////////////////
// 12/29/98 - DBUCHES: Function created / header added
///////////////////////////////////////////////////////////////////////////////
QSPAKSTATUS QsPakQueryItemValue( HQSERVERITEM hItem,
                            const char * szValue,
                            QSPAKDATATYPE eType,
                            void* pBuffer,
                            unsigned long *pulBufferLength )
{
    LPDATAFIELD pDataField;
    LPQSERVERITEM pItem = (LPQSERVERITEM) hItem;

    //
    // Validate input.
    //
    if( hItem == NULL || pulBufferLength == NULL )
        {
        return QSPAKSTATUS_INVALID_ARG;
        }

    //
    // Try to find data item.
    //
    pDataField = GetDataField( pItem->dataMap, szValue );
    if( pDataField == NULL )
        return QSPAKSTATUS_NO_SUCH_FIELD;

    //
    // Make sure user buffer is big enough.
    //
    if( pDataField->ulDataLength > *pulBufferLength )
        {
        *pulBufferLength = pDataField->ulDataLength;

        //
        // If this is a string based attribute, add extra byte
        // for NULL terminator.
        //
        if( eType == QSPAK_TYPE_STRING )
            {
            *pulBufferLength ++;
            }

        return QSPAKSTATUS_BUFFER_TOO_SMALL;
        }

    //
    // Make sure we have the correct type.
    //
    if( eType != pDataField->ulType )
        return QSPAKSTATUS_INVALID_TYPE;

    //
    // Copy data to user buffer.
    //
    memset( pBuffer, 0 ,*pulBufferLength );
    memcpy( pBuffer, pDataField->pbyData, pDataField->ulDataLength );

    return QSPAKSTATUS_OK;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: QsPakSetItemValue
//
// Description  : This function will set the data in the specified value with
//                the data pointed to by pData.
//
// Return type  : QSPAKSTATUS possible return values:
//                  QSPAKSTATUS_OK
//                  QSPAKSTATUS_INVALID_ARG
//
// Argument     : HQSERVERITEM hItem
// Argument     : QSERVERITEMINFO eValue
// Argument     : void* pData
// Argument     : int iDataLength
//
///////////////////////////////////////////////////////////////////////////////
// 12/29/98 - DBUCHES: Function created / header added
///////////////////////////////////////////////////////////////////////////////
QSPAKSTATUS QsPakSetItemValue( HQSERVERITEM hItem,
                          const char * szValue,
                          QSPAKDATATYPE eType,
                          void* pData,
                          unsigned long ulDataLength )
{
    LPDATAFIELD pDataField;
    LPQSERVERITEM pItem = (LPQSERVERITEM) hItem;

    //
    // Validate input.
    //
    if( hItem == NULL || pData == NULL || ulDataLength == 0 )
        {
        return QSPAKSTATUS_INVALID_ARG;
        }

    //
    // Try to find data item.
    //
    pDataField = GetDataField( pItem->dataMap, szValue );
    if( pDataField == NULL )
        {
        //
        // Allocate a new data field item.
        //
        pDataField = new DATAFIELD;
        if( pDataField == NULL )
            return QSPAKSTATUS_MEMORY_ERROR;

        pDataField->pbyData = new BYTE[ ulDataLength ];
        if( pDataField->pbyData == NULL )
            {
            delete pDataField;
            return QSPAKSTATUS_MEMORY_ERROR;
            }
        pDataField->ulDataLength = ulDataLength;

        //
        // Save off name of field.
        //
        strncpy( pDataField->szFieldName, szValue, MAX_FIELD_NAME_LENGTH );

        //
        // Save off type information
        //
        pDataField->ulType = eType;

        //
        // Insert this element into our list.
        //
        pItem->dataMap[ pDataField->szFieldName ] = pDataField;

        //
        // Recalc size of array.
        //
        pItem->pHeader->ulNumDataItems = pItem->dataMap.size();
        }
    else
        {
        //
        // Check to see if we have enough memory allocated to hold the new
        // value.
        //
        if( pDataField->ulDataLength != ulDataLength )
            {
            delete [] pDataField->pbyData;
            pDataField->pbyData = new BYTE[ ulDataLength ];
            if( pDataField->pbyData == NULL )
                {
                return QSPAKSTATUS_MEMORY_ERROR;
                }
            pDataField->ulDataLength = ulDataLength;
            }
        }

    //
    // Copy data.
    //
    memcpy( pDataField->pbyData, pData, pDataField->ulDataLength );

    //
    // Save off type info
    //
    pDataField->ulType = eType;

    return QSPAKSTATUS_OK;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name : QsPakIsQserverFile
//
// Description   : This function will check to see if the given file is indeed
//                 a Quarantine Server file.
//
// Return type   : QSPAKSTATUS
//                  QSPAKSTATUS_OK - if file is a Quarantine server file
//                  QSPAKSTATUS_BOGUS_FILE - if not.
//                  QSPAKSTATUS_INVALID_ARG
//                  QSPAKSTATUS_FILE_NOT_FOUND
//                  QSPAKSTATUS_FILE_NOT_FOUND
//
// Argument      : const char * pszFileName
//
///////////////////////////////////////////////////////////////////////////////
// 12/30/98 - DBUCHES: Function created / header added
///////////////////////////////////////////////////////////////////////////////
QSPAKSTATUS QsPakIsQserverFile( const char * pszFileName )
{
    int iOpenFile;
#if defined(WIN32)
    struct _stat stats;
#else
    struct stat stats;
#endif
    char buffer[ sizeof(DWORD) * 2 ];
    QSPAKSTATUS status = QSPAKSTATUS_OK;

    //
    // Validate input
    //
    if( pszFileName == NULL )
        return QSPAKSTATUS_INVALID_ARG;

    //
    // Open file for read access.
    //
#if defined(WIN32)
    iOpenFile = _open( pszFileName, _O_BINARY | _O_RDONLY );
#else
    iOpenFile = open( pszFileName,O_BINARY | O_RDONLY );
#endif

    if( iOpenFile == -1 )
        {
        switch( errno )
            {
            case EACCES:
                return QSPAKSTATUS_ACCESS_DENIED;

            default:
                return QSPAKSTATUS_FILE_NOT_FOUND;
            }
        }

    //
    // Check size of file.  Size must be at least the size of the header + 1;
    //
#if defined(WIN32)
    if( _fstat( iOpenFile, &stats ) == 0 )
#else
	  if( fstat( iOpenFile, &stats ) == 0 )
#endif
        {
        if( stats.st_size < sizeof( QSERVERFILEHEADER ) + 1)
            {
#if defined(WIN32)
            _close( iOpenFile );
#else
            close( iOpenFile );
#endif
            return QSPAKSTATUS_BOGUS_FILE;
            }
        }
    else
        {
#if defined(WIN32)
        _close( iOpenFile );
#else
        close( iOpenFile );
#endif
        return QSPAKSTATUS_ACCESS_DENIED;
        }

    //
    // Read in start of header.
    //
#if defined(WIN32)
    if( sizeof( DWORD ) * 2 == _read( iOpenFile, buffer, sizeof( DWORD ) * 2 ) )
#else
    if( sizeof( DWORD ) * 2 == read( iOpenFile, buffer, sizeof( DWORD ) * 2 ) )
#endif
        {
        xorBuffer( buffer, sizeof( DWORD ) * 2 );
        //
        // Check signature and version.
        //
        if( QSPAKSTATUS_OK != QsPakIsQserverHeaderBuffer( (unsigned char *) buffer, sizeof(DWORD) * 2 ) )
            {
            status = QSPAKSTATUS_BOGUS_FILE;
            }
        }
    else
        {
        //
        // Couldn't read header, must be bogus.
        //
        status = QSPAKSTATUS_BOGUS_FILE;
        }

    //
    // Cleanup.
    //
#if defined(WIN32)
    _close( iOpenFile );
#else
    close( iOpenFile );
#endif
    return status;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name : QsPakIsQserverHeaderBuffer
//
// Description   : This function will return QSPAKSTATUS_OK if the buffer passed
//                 in has the correct Quarantine Server file signature and version.
//
// Return type   : QSPAKSTATUS
//
// Argument      : const unsigned char * pBuffer
// Argument      : int iBufferSize
//
///////////////////////////////////////////////////////////////////////////////
// 12/30/98 - DBUCHES: Function created / header added
///////////////////////////////////////////////////////////////////////////////
QSPAKSTATUS QsPakIsQserverHeaderBuffer( const unsigned char * pBuffer, unsigned int uBufferSize )
{
    LPQSERVERFILEHEADER pHeader = ( LPQSERVERFILEHEADER ) pBuffer;

    //
    // Starting signature is 2 DWORDs in size.
    //
    if( uBufferSize < sizeof( DWORD ) * 2 )
        return QSPAKSTATUS_ERROR;

    //
    // Make sure version and signature are here.
    //
    if( DWENDIAN( pHeader->ulSignature ) != QSERVER_FILE_SIGNATURE ||
        DWENDIAN( pHeader->ulVersion ) != QSERVER_FILE_VERSION )
        {
        return QSPAKSTATUS_ERROR;
        }

    return QSPAKSTATUS_OK;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: QsPakGetRawFileHere
//
// Description  : This routine will copy the actual file bytes into the specified
//                buffer.  This routine should be called twice.  The first call
//                will return QSPAKSTATUS_BUFFER_TOO_SMALL and *piBufferLength
//                will contain the size of the buffer needed to hold the file data.
//
// Return type  : QSPAKSTATUS
//                  QSPAKSTATUS_OK
//                  QSPAKSTATUS_BUFFER_TOO_SMALL
//                  QSPAKSTATUS_FILE_NOT_FOUND
//                  QSPAKSTATUS_INVALID_ARG
//                  QSPAKSTATUS_ACCESS_DENIED
//
// Argument     : HQSERVERITEM hItem
// Argument     : const unsigned HUGE * pBuffer
// Argument     : int * piBufferLength
//
///////////////////////////////////////////////////////////////////////////////
// 2/1/99 - DBUCHES: Function created / header added
///////////////////////////////////////////////////////////////////////////////
QSPAKSTATUS QsPakGetRawFileHere( HQSERVERITEM hItem,
                             unsigned char * pBuffer,
                             unsigned long * pulBufferLength )
{
    QSPAKSTATUS status = QSPAKSTATUS_OK;
    int iSourceFile = -1;
    BYTE * p = pBuffer;
    LPQSERVERITEM pItem = (LPQSERVERITEM) hItem;
    ULONG ulBytesRead = 0;
    int iBytesRead;

    //
    // Validate input
    //
    if( pItem == NULL || pulBufferLength == NULL )
        {
        return QSPAKSTATUS_INVALID_ARG;
        }

    //
    // Make sure specified buffer is large enough.
    //
    if( *pulBufferLength < pItem->pHeader->ulOriginalFileSize )
        {
        *pulBufferLength = pItem->pHeader->ulOriginalFileSize;
        return QSPAKSTATUS_BUFFER_TOO_SMALL;
        }

    //
    // Open source file.
    //
#if defined(WIN32)
    iSourceFile = _open( pItem->szFileName, _O_SEQUENTIAL | _O_BINARY | _O_RDONLY );
#else
    iSourceFile = open( pItem->szFileName, O_BINARY | O_RDONLY );
#endif
    if( iSourceFile == -1 )
        {
        switch( errno )
            {
            case EACCES:
                status = QSPAKSTATUS_ACCESS_DENIED;
            case ENOENT:
                status = QSPAKSTATUS_FILE_NOT_FOUND;
            default:
                status = QSPAKSTATUS_ERROR;
            }
        goto BailOut;
        }

    //
    // Seek past header
    //
#if defined(WIN32)
    if( -1 == _lseek( iSourceFile, sizeof( QSERVERFILEHEADER ), SEEK_SET ) )
#else
    if( -1 == lseek( iSourceFile, sizeof( QSERVERFILEHEADER ), SEEK_SET ) )
#endif
        {
        status = QSPAKSTATUS_BOGUS_FILE;
        }
    else
        {
        //
        // Read chunk from source
        //
#if defined(WIN32)
        iBytesRead = _read( iSourceFile, p, (unsigned int) *pulBufferLength );
#else
        iBytesRead = read( iSourceFile, p, (unsigned int) *pulBufferLength );
#endif
        if( iBytesRead == -1 )
            {
            status = QSPAKSTATUS_ERROR;
            }
        else
            {
            xorBuffer( p, iBytesRead );
            }
        }

BailOut:
    if( iSourceFile != -1 )
#if defined(WIN32)
       _close( iSourceFile );
#else
       close( iSourceFile );
#endif

    return status;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name : QsPakGetItemFieldInfo
//
// Description   : This routine will construct a array of field description
//                 objects.  Free this array with a call to QsPakFreeItemFieldInfo()
//
// Return type   : QSPAKSTATUS
//                 QSPAKSTATUS_OK
//                 QSPAKSTATUS_INVALID_ARG
//                 QSPAKSTATUS_MEMORY_ERROR
//
// Argument      : HQSERVERITEM hItem
// Argument      : int * piNumFields
// Argument      : LPFIELDINFO * pFieldInfo
//
///////////////////////////////////////////////////////////////////////////////
// 2/5/99 - DBUCHES: Function created / header added
///////////////////////////////////////////////////////////////////////////////
QSPAKSTATUS QsPakGetItemFieldInfo( HQSERVERITEM hItem,
                                   unsigned int * puNumFields,
                                   LPFIELDINFO * pFieldInfo )
{
    LPDATAFIELD pCur;
    LPFIELDINFO pInfo;
    QSPAKSTATUS status = QSPAKSTATUS_OK;
    LPQSERVERITEM pItem = (LPQSERVERITEM) hItem;
    MAP::iterator MapIterator;
    int i = 0;

    //
    // Validate input
    //
    if( pItem == NULL || puNumFields == NULL || pFieldInfo == NULL )
        {
        return QSPAKSTATUS_INVALID_ARG;
        }

    //
    // Allocate array of data items.
    //
    pInfo = new FIELDINFO [ pItem->pHeader->ulNumDataItems ];
    if( pInfo == NULL )
        return QSPAKSTATUS_MEMORY_ERROR;

    //
    // Save off output fields.
    //
    *puNumFields = (unsigned int)pItem->pHeader->ulNumDataItems;
    *pFieldInfo = pInfo;

    //
    // Copy data.
    //
    MapIterator = pItem->dataMap.begin();
    while( MapIterator != pItem->dataMap.end() )
        {
        //
        // Get pointer to object
        //
        pCur = (LPDATAFIELD)(*MapIterator).second;

        //
        // Copy data attributes.
        //
        strncpy( pInfo[i].szFieldName, pCur->szFieldName, MAX_FIELD_NAME_LENGTH );
        pInfo[i].eType = (QSPAKDATATYPE)pCur->ulType;
        pInfo[i].uDataLength = pCur->ulDataLength;

        //
        // Need to add extra byte for strings for null termination.
        //
        if( pInfo[i].eType == QSPAK_TYPE_STRING )
            {
            pInfo[i].uDataLength ++;
            }

        //
        // Advance to next element.
        //
        MapIterator++;
        i++;
        }

    return status;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name : QsPakFreeItemFieldInfo
//
// Description   : Frees data allocated by QsPakGetItemFieldInfo.
//
// Return type   : QSPAKSTATUS
//
// Argument      : LPFIELDINFO *pFieldInfo
//
///////////////////////////////////////////////////////////////////////////////
// 2/5/99 - DBUCHES: Function created / header added
///////////////////////////////////////////////////////////////////////////////
QSPAKSTATUS QsPakFreeItemFieldInfo( LPFIELDINFO pFieldInfo )
{
    //
    // Validate input.
    //
    if( pFieldInfo == NULL )
        {
        return QSPAKSTATUS_OK;
        }

    //
    // Blow away data.
    //
    delete [] pFieldInfo;

    //
    // All done.
    //
    return QSPAKSTATUS_OK;
}



/////////////////////////////////////////////////////////////////////////////////
//
// Helper routines.
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
// Function name: xorBuffer
//
// Description  : This routine will simply XOR all the bytes of the specified
//                buffer
//
// Return type  : void
//
// Argument     : void* pBuffer
// Argument     : int iSize
//
///////////////////////////////////////////////////////////////////////////////
// 12/29/98 - DBUCHES: Function created / header added
///////////////////////////////////////////////////////////////////////////////
void xorBuffer( void* pBuffer, ULONG ulSize )
{
    for( ULONG i = 0; i < ulSize; i++ )
        ((BYTE*)pBuffer)[i] ^= (BYTE) 0xFF;
}



///////////////////////////////////////////////////////////////////////////////
//
// Function name : GetDataField
//
// Description   : This function loops thru our list looking for matching data
//                 items
//
// Return type   : LPDATAFIELD
//
// Argument      :  LPDATAFIELD pListHead
// Argument      : const char * szFieldName
//
///////////////////////////////////////////////////////////////////////////////
// 1/15/99 - DBUCHES: Function created / header added
///////////////////////////////////////////////////////////////////////////////
LPDATAFIELD GetDataField( MAP& dataMap, const char * szFieldName )
{
    //
    // Set up iterator.
    //
    MAP::iterator MapIterator;

    //
    // If map is empty, why are we here?
    //
    if( dataMap.empty() )
        return NULL;
    //
    // Search for this entry.
    //
    if((MapIterator = dataMap.find( szFieldName )) != dataMap.end() )
        return (*MapIterator).second;

    return NULL;
}



///////////////////////////////////////////////////////////////////////////////
//
// Function name : ReadDataFields
//
// Description   : Populate the list of properties for this item.
//
// Return type   : BOOL
//
// Argument      : int iOpenFile - open file with position set to begining of
//                  quarantine server data.
// Argument      : LPQSERVERITEM pItem
//
///////////////////////////////////////////////////////////////////////////////
// 1/15/99 - DBUCHES: Function created / header added
///////////////////////////////////////////////////////////////////////////////
BOOL ReadDataFields( int iOpenFile, LPQSERVERITEM pItem )
{
    LPDATAFIELD pDataField = NULL;
    FILEDATADESCRIPTOR descriptor;
    BOOL bError = FALSE;

    for( ULONG i = 0; i < pItem->pHeader->ulNumDataItems; i++ )
        {
        //
        // Allocate new data item
        //
        pDataField = new DATAFIELD;
        if( pDataField == NULL )
            {
            bError = TRUE;
            break;
            }

        //
        // Read Descriptor.
        //
#if defined(WIN32)
        if( sizeof( FILEDATADESCRIPTOR ) != _read( iOpenFile, &descriptor, sizeof( FILEDATADESCRIPTOR ) ) )
#else
        if( sizeof( FILEDATADESCRIPTOR ) != read( iOpenFile, &descriptor, sizeof( FILEDATADESCRIPTOR ) ) )
#endif
            {
            bError = TRUE;
            break;
            }
        xorBuffer( &descriptor, sizeof( FILEDATADESCRIPTOR ) );
        EndianDescriptor( &descriptor );

        //
        // Copy field attributes
        //
        strncpy( pDataField->szFieldName, descriptor.szFieldName, MAX_FIELD_NAME_LENGTH );
        pDataField->ulDataLength = descriptor.ulDataLength;
        pDataField->ulType = descriptor.ulType;

        //
        // Allocate space for data.
        //
        pDataField->pbyData = new BYTE[ pDataField->ulDataLength ];
        if( pDataField->pbyData == NULL )
            {
            bError = TRUE;
            break;
            }

        //
        // Read in data from file.
        //
#if defined(WIN32)
        if( pDataField->ulDataLength != (ULONG)_read( iOpenFile, pDataField->pbyData, pDataField->ulDataLength ) )
#else
        if( pDataField->ulDataLength != (ULONG)read( iOpenFile, pDataField->pbyData, pDataField->ulDataLength ) )
#endif
            {
            bError = TRUE;
            break;
            }
        xorBuffer( pDataField->pbyData, pDataField->ulDataLength );

        if( pDataField->ulType == QSPAK_TYPE_DWORD )
            *(DWORD*) pDataField->pbyData = DWENDIAN( *(DWORD*)pDataField->pbyData );
        else if( pDataField->ulType == QSPAK_TYPE_DATE )
            EndianDate( (LPQSPAKDATE) pDataField->pbyData );

        //
        // Insert into list.
        //
        pItem->dataMap[ pDataField->szFieldName ] = pDataField;
        }

    //
    // If something bad happened, clean up after ourselves
    //
    if( bError )
        {
        CleanupItem( pItem );
        if( pDataField )
            {
            if( pDataField->pbyData )
                delete [] pDataField->pbyData;
            delete [] pDataField;
            }
        }

    return !bError;
}



///////////////////////////////////////////////////////////////////////////////
//
// Function name : SaveDataFields
//
// Description   : This routine will save data attributes to a file
//
// Return type   : BOOL
//
// Argument      :  int iOpenFile
// Argument      : LPQSERVERITEM pItem
//
///////////////////////////////////////////////////////////////////////////////
// 1/15/99 - DBUCHES: Function created / header added
///////////////////////////////////////////////////////////////////////////////
BOOL SaveDataFields( int iOpenFile, LPQSERVERITEM pItem )
{
    FILEDATADESCRIPTOR descriptor;
    BOOL bError = FALSE;
    LPDATAFIELD pCur;
    MAP::iterator MapIterator;

    MapIterator = pItem->dataMap.begin();

    while( MapIterator != pItem->dataMap.end() )
        {
        //
        // Get pointer to object
        //
        pCur = (LPDATAFIELD)(*MapIterator).second;

        //
        // Set data fields of descriptor
        //
        strncpy( descriptor.szFieldName, pCur->szFieldName, MAX_FIELD_NAME_LENGTH );
        descriptor.ulDataLength = pCur->ulDataLength;
        descriptor.ulType = pCur->ulType;

        EndianDescriptor( &descriptor );
        xorBuffer( &descriptor, sizeof( FILEDATADESCRIPTOR ) );

#if defined(WIN32)
        if( sizeof( FILEDATADESCRIPTOR ) != _write( iOpenFile, &descriptor, sizeof( FILEDATADESCRIPTOR ) ) )
#else
	    if( sizeof( FILEDATADESCRIPTOR ) != write( iOpenFile, &descriptor, sizeof( FILEDATADESCRIPTOR ) ) )
#endif
            {
            bError = TRUE;
            break;
            }

        //
        // Write data to disk.
        //
        if( pCur->ulType == QSPAK_TYPE_DWORD )
            *(DWORD*) pCur->pbyData = DWENDIAN( *(DWORD*) pCur->pbyData );
        else if( pCur->ulType == QSPAK_TYPE_DATE )
            EndianDate( (LPQSPAKDATE) pCur->pbyData );

        xorBuffer( pCur->pbyData, pCur->ulDataLength );
#if defined(WIN32)
        if( pCur->ulDataLength != (ULONG) _write( iOpenFile, pCur->pbyData, pCur->ulDataLength ) )
#else
        if( pCur->ulDataLength != (ULONG) write( iOpenFile, pCur->pbyData, pCur->ulDataLength ) )
#endif
            {
            bError = TRUE;
            break;
            }
        xorBuffer( pCur->pbyData, pCur->ulDataLength );

        //
        // Advance to next element.
        //
        MapIterator++;
        }

    return !bError;
}



///////////////////////////////////////////////////////////////////////////////
//
// Function name : CleanupItem
//
// Description   : Deletes the linked list of attirbutes from a given item.
//
// Return type   : void
//
// Argument      : LPQSERVERITEM pItem
//
///////////////////////////////////////////////////////////////////////////////
// 1/15/99 - DBUCHES: Function created / header added
///////////////////////////////////////////////////////////////////////////////
void CleanupItem( LPQSERVERITEM pItem )
{
    //
    // Clean up linked list
    //
    LPDATAFIELD pCur;
    MAP::iterator MapIterator;
    MapIterator = pItem->dataMap.begin();

    while( MapIterator != pItem->dataMap.end() )
        {
        //
        // Get pointer to object
        //
        pCur = (LPDATAFIELD)(*MapIterator).second;

        //
        // Delete data.
        //
        if( pCur->pbyData )
            {
            delete [] pCur->pbyData;
            }

        //
        // delete actual object
        //
        delete pCur;

        //
        // Advance to next element
        //
        MapIterator++;
        }
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: EndianHeader
//
// Description  :
//
// Return type  : void
//
// Argument     : QSERVERFILEHEADER *pHeader
//
///////////////////////////////////////////////////////////////////////////////
// 11/17/99 - DBUCHES: Function created / header added
///////////////////////////////////////////////////////////////////////////////
void EndianHeader( QSERVERFILEHEADER *pHeader )
{
    //
    // Perform Endian conversion.
    //
    pHeader->ulSignature = DWENDIAN( pHeader->ulSignature );
    pHeader->ulVersion = DWENDIAN( pHeader->ulVersion );
    pHeader->ulOriginalFileSize = DWENDIAN( pHeader->ulOriginalFileSize );
    pHeader->ulNumDataItems = DWENDIAN( pHeader->ulNumDataItems );
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: EndianDescriptor
//
// Description  :
//
// Return type  : void
//
// Argument     : FILEDATADESCRIPTOR* pDescriptor
//
///////////////////////////////////////////////////////////////////////////////
// 11/17/99 - DBUCHES: Function created / header added
///////////////////////////////////////////////////////////////////////////////
void EndianDescriptor( FILEDATADESCRIPTOR* pDescriptor )
{
    pDescriptor->ulDataLength = DWENDIAN( pDescriptor->ulDataLength );
    pDescriptor->ulType = DWENDIAN( pDescriptor->ulType );
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: EndianDate
//
// Description  : 
//
// Return type  : void 
//
// Argument     : LPQSPAKDATE pDate
//
///////////////////////////////////////////////////////////////////////////////
// 12/10/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
void EndianDate( LPQSPAKDATE pDate )
{
    pDate->wYear = WENDIAN( pDate->wYear );
}


