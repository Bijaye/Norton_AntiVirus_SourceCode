// Copyright 1998 Symantec Corporation. 
//*************************************************************************
// streams.cpp - created 6/9/98 5:58:31 PM
//
// $Header:   S:/NAVNTUTL/VCS/streams.cpv   1.0   10 Jun 1998 15:47:44   DBuches  $
//
// Description: Contains code for manipulating alternate data streams
//              on NTFS volumes.
//
// Contains:
//    FileHasAlternateDataStreams
//
//*************************************************************************
// $Log:   S:/NAVNTUTL/VCS/streams.cpv  $
// 
//    Rev 1.0   10 Jun 1998 15:47:44   DBuches
// Initial revision.
//*************************************************************************

//*************************************************************************
// Includes

// From NTSRCINC
extern "C" {
#include "nt.h"
#include "ntrtl.h"
};

#include "stdlib.h"
#include "navntutl.h"
#include "copystream.h"

//*************************************************************************
// Local functions.

NTSTATUS GetFileStreamInfo( HANDLE hSourceFile,
                            PFILE_STREAM_INFORMATION* pStreamInfo );
NTSTATUS FreeFileStreamInfo( PFILE_STREAM_INFORMATION pStreamInfo );



//*************************************************************************
// FileHasAlternateDataStreams()
//
// FileHasAlternateDataStreams(
//      IN HANDLE  hSourceFile )
//
// Description: This routine will return the number of alternate data
//              streams for a given file object.  This does not include
//              the default data stream.
//
// Returns: ULONG  - number of alternate data streams.
//
//*************************************************************************
// 6/9/98 DBUCHES, created - header added.
//*************************************************************************

ULONG FileHasAlternateDataStreams( IN HANDLE  hSourceFile )
{
    ULONG                       ulNumStreams = 0;
    PFILE_STREAM_INFORMATION    pStreamInfo;
    PFILE_STREAM_INFORMATION    pStreamInfoStart = NULL;
    NTSTATUS                    status;

    //
    // Get file stream information for this object.
    //
    status = GetFileStreamInfo( hSourceFile, &pStreamInfoStart );
    ASSERT( pStreamInfoStart );

    // 
    // Count number of additionl streams in this file.
    //  
    if( NT_SUCCESS( status ) )
        {
        pStreamInfo = pStreamInfoStart;
        
        while( TRUE ) 
            {
            //
            // Make sure we skip over the default stream.  We identify 
            // the default stream by the following criteria:
            //      1) streamNameLength <= 1
            //      2) streamName[1] == ':'
            //
            if( pStreamInfo->StreamNameLength <= sizeof(WCHAR) ||
                pStreamInfo->StreamName[1] == ':' ) 
                {
                
                // 
                // If no more streams, break out of loop
                //
                if( pStreamInfo->NextEntryOffset == 0 )
                    break;
                
                //
                // Move to next stream.
                //
                pStreamInfo = (PFILE_STREAM_INFORMATION)((PCHAR) pStreamInfo + pStreamInfo->NextEntryOffset);
                ASSERT( pStreamInfo );
                continue;
                }

            //
            // Bump stream counter
            //
            ulNumStreams ++;

            //
            // Are we done yet?
            //
            if( pStreamInfo->NextEntryOffset == 0 ) 
                {
                break;
                }
            
            //
            // Advance to next stream
            //
            pStreamInfo = (PFILE_STREAM_INFORMATION)((PCHAR) pStreamInfo + pStreamInfo->NextEntryOffset);
            ASSERT( pStreamInfo );
            }
        }


    // 
    // Cleanup
    //
    if( pStreamInfoStart )
        {
        FreeFileStreamInfo( pStreamInfoStart );
        }


    return ulNumStreams;
}


//*************************************************************************
// CopyAlternateDataStreams()
//
// CopyAlternateDataStreams(
//      HANDLE hSourceFile
//      LPCTSTR lpszSource
//      LPCTSTR lpszDest )
//
// Description: This routine will copy any alternate data streams from
//              the source file to the destination file.  This routine will
//              not, however copy the default data stream.  
//
//              Note that both lpszSource and lpszDest must be existing
//              files.
//
// Returns: ULONG - TRUE on success.
//                  FALSE on failure.  Call GetLastError() for details.
//
//*************************************************************************
// 6/10/98 DBUCHES, created - header added.
//*************************************************************************

ULONG CopyAlternateDataStreams( HANDLE hSourceFile, LPCTSTR lpszSourceName, LPCTSTR lpszDestName )
{
    PFILE_STREAM_INFORMATION    pStreamInfo;
    PFILE_STREAM_INFORMATION    pStreamInfoStart = NULL;
    NTSTATUS                    status;
    ULONG                       ulRet = TRUE;

    //
    // Get file stream information for this object.
    //
    status = GetFileStreamInfo( hSourceFile, &pStreamInfoStart );
    if( !NT_SUCCESS( status ) )
        {
        // 
        // This means that there are no alternate date
        // streams in this file
        return TRUE;
        }
    
    pStreamInfo = pStreamInfoStart;
    
    while( TRUE ) 
        {
        //
        // Make sure we skip over the default stream.  We identify 
        // the default stream by the following criteria:
        //      1) streamNameLength <= 1
        //      2) streamName[1] == ':'
        //
        if( pStreamInfo->StreamNameLength <= sizeof(WCHAR) ||
            pStreamInfo->StreamName[1] == ':' ) 
            {
            
            // 
            // If no more streams, break out of loop
            //
            if( pStreamInfo->NextEntryOffset == 0 )
                break;
            
            //
            // Move to next stream.
            //
            pStreamInfo = (PFILE_STREAM_INFORMATION)((PCHAR) pStreamInfo + pStreamInfo->NextEntryOffset);
            ASSERT( pStreamInfo );
            continue;
            }

        //
        // Copy this stream.
        //
        ulRet = CopyStream( pStreamInfo->StreamName, 
                            pStreamInfo->StreamNameLength,
                            pStreamInfo->StreamSize.LowPart, 
                            lpszSourceName, 
                            lpszDestName );
        if( ulRet == FALSE )
            break;


        //
        // Are we done yet?
        //
        if( pStreamInfo->NextEntryOffset == 0 ) 
            {
            break;
            }
        
        //
        // Advance to next stream
        //
        pStreamInfo = (PFILE_STREAM_INFORMATION)((PCHAR) pStreamInfo + pStreamInfo->NextEntryOffset);
        }

    // 
    // Cleanup
    //
    if( pStreamInfoStart )
        {
        FreeFileStreamInfo( pStreamInfoStart );
        }

    return ulRet;
}



//*************************************************************************
// GetFileStreamInfo()
//
// GetFileStreamInfo(
//      IN HANDLE hSourceFile - 
//      OUT PFILE_STREAM_INFORMATION pStreamInfo )
//
// Description: Retrieves the file stream information block for given 
//              file system object.
//
// Returns: NTSTATUS 
//
//*************************************************************************
// 6/9/98 DBUCHES, created - header added.
//*************************************************************************

NTSTATUS GetFileStreamInfo( IN  HANDLE hSourceFile,
                            OUT PFILE_STREAM_INFORMATION *pStreamInfo )
{
    NTSTATUS                    status;
    IO_STATUS_BLOCK             IoStatus;
    ULONG                       streamBufferSize = 4096;
    
    //
    // Allocate and load file stream info block.
    //
    do
        {
        //
        // Allocate stream information block.
        //
        *pStreamInfo = (PFILE_STREAM_INFORMATION) malloc( streamBufferSize );
        if( *pStreamInfo == NULL )
            return STATUS_NO_MEMORY;

        //
        // Get FileStreamInformation for this file object.
        //
        status = NtQueryInformationFile(
                    hSourceFile,
                    &IoStatus,
                    (PVOID) *pStreamInfo,
                    streamBufferSize,
                    FileStreamInformation
                    );

        //
        // Check for failure.  Allocate more memory if neccessary.
        //
        if ( !NT_SUCCESS(status) ) 
            {
            free( (PVOID) *pStreamInfo );
            *pStreamInfo = NULL;
            streamBufferSize *= 2;
            }
        }
    while ( status == STATUS_BUFFER_OVERFLOW || status == STATUS_BUFFER_TOO_SMALL );

    return status;
}

//*************************************************************************
// FreeFileStreamInfo()
//
// FreeFileStreamInfo(
//      IN PFILE_STREAM_INFORMATION pStreamInfo )
//
// Description: Frees memory allocated by previous call to 
//              GetFileStreamInfo();
//
// Returns: NTSTATUS 
//
//*************************************************************************
// 6/9/98 DBUCHES, created - header added.
//*************************************************************************

NTSTATUS FreeFileStreamInfo( IN PFILE_STREAM_INFORMATION pStreamInfo )
{
    ASSERT( pStreamInfo );

    free((PVOID) pStreamInfo );

    return STATUS_SUCCESS;
}