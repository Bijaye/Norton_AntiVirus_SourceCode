// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 1998 - 2003, 2005 Symantec Corporation. All rights reserved. 
//*************************************************************************
// copystream.cpp - created 6/10/98 12:25:16 PM
//
// $Header:   S:/NAVNTUTL/VCS/copystream.cpv   1.0   10 Jun 1998 15:47:42   DBuches  $
//
// Description:  Contains implementation for CopyStream() routine.
//
//*************************************************************************
// $Log:   S:/NAVNTUTL/VCS/copystream.cpv  $
// 
//    Rev 1.0   10 Jun 1998 15:47:42   DBuches
// Initial revision.
//*************************************************************************

//*************************************************************************
// Includes

#include "windows.h"
#include "tchar.h"
#include "copystream.h"
#include "SymSaferStrings.h"

//xor copied from vbin.h. 
//used to keep files from being detected in quarantine
inline void my_xor(PBYTE data,int len) {
    while(len--)
        data[len] ^= 0x5a;
}


//*************************************************************************
// CopyStream()
//
// CopyStream(
//      IN LPCWSTR wszStreamName
//      IN ULONG ulStreamNameLength
//      IN ULONG ulStreamSize
//      IN LPCTSTR lpszSourceName
//      IN LPCTSTR lpszDestName )
//
// Description: Copies the specified stream from the source file to the 
//              destination file.
//
// Returns: ULONG - TRUE on success
//                  FALSE on error.  Call GetLastError() for details.
//
//*************************************************************************
// 6/10/98 DBUCHES, created - header added.
//*************************************************************************

ULONG CopyStream( IN LPCWSTR lpwszStreamName,
                  IN ULONG ulStreamNameLength,
                  IN ULONG ulStreamLength, 
                  IN LPCTSTR lpszSourceName, 
                  IN LPCTSTR lpszDestName,
                  BOOL bXORStreams)
{
    TCHAR               szStreamName[MAX_PATH];
    TCHAR               szBuffer[MAX_PATH * 2];
    HANDLE              hSourceStream = INVALID_HANDLE_VALUE;
    HANDLE              hDestStream = INVALID_HANDLE_VALUE;
    HANDLE              hDestFile = INVALID_HANDLE_VALUE;
    ULONG               ulRet = FALSE;
    LPBYTE              pXferBuffer = NULL;
    ULONG               ulXferSize;
    DWORD               dwBytesRead;
    DWORD               dwBytesWritten;

    //
    // Perform UNICODE translation if neccessary
    //
#ifndef UNICODE
    ZeroMemory( szStreamName, sizeof( TCHAR ) * MAX_PATH );
    WideCharToMultiByte( CP_ACP,
                         0,
                         lpwszStreamName,
                         ulStreamNameLength / sizeof( WCHAR ),
                         szStreamName,
                         MAX_PATH,
                         NULL,
                         NULL );
#else
    lstrcpyn( szStreamName, lpwszStreamName, MAX_PATH );
#endif


    __try
    {
        //
        // Open source stream.
        //
        sssnprintf( szBuffer, sizeof(szBuffer), _T("%s%s"), lpszSourceName, szStreamName );
        hSourceStream = CreateFile( szBuffer,
                                    GENERIC_READ,
                                    FILE_SHARE_READ | FILE_SHARE_WRITE,
                                    NULL,
                                    OPEN_EXISTING,
                                    FILE_FLAG_RANDOM_ACCESS,
                                    NULL );

        if( hSourceStream == INVALID_HANDLE_VALUE )
        {
            __leave;
        }   

        //we want to make sure the destination file exists
        hDestFile = CreateFile( lpszDestName,
                                GENERIC_WRITE,
                                0,
                                NULL,
                                OPEN_EXISTING,
                                FILE_FLAG_RANDOM_ACCESS,
                                NULL );
        if( hDestFile == INVALID_HANDLE_VALUE )
        {
            __leave;
        }

        //
        // Open destnation stream.
        //
        sssnprintf( szBuffer, sizeof(szBuffer), _T("%s%s"), lpszDestName, szStreamName );
        hDestStream = CreateFile( szBuffer,
                                    GENERIC_WRITE,
                                    0,
                                    NULL,
                                    OPEN_ALWAYS,
                                    FILE_FLAG_RANDOM_ACCESS,
                                    NULL );

        if( hDestStream == INVALID_HANDLE_VALUE )
        {
            __leave;
        }   

        // 
        // Allocate Xfer buffer
        //
        pXferBuffer = new BYTE[0xFFFF];
        if( pXferBuffer == NULL )
        {
            __leave;
        }

        // 
        // Transfer stream data
        //
        while( ulStreamLength )
        {
            //
            // Compute how much to transfer this time thru
            //
            ulXferSize = ulStreamLength < 0xFFFF ? ulStreamLength : 0xFFFF;

            // 
            // Read data from source
            //
            if( FALSE == ReadFile( hSourceStream,
                                   pXferBuffer,
                                   ulXferSize,
                                   &dwBytesRead,
                                   NULL ) || dwBytesRead != ulXferSize )
            {
                __leave;
            }


            //xor copied from vbin.h. 
            //used to keep files from being detected in quarantine
            if( TRUE == bXORStreams )
            {
                my_xor(pXferBuffer,ulXferSize);
            }

            // 
            // Write data to destination
            //
            if( FALSE == WriteFile( hDestStream,
                                    pXferBuffer,
                                    ulXferSize,
                                    &dwBytesWritten,
                                    NULL ) || dwBytesWritten != ulXferSize )
            {
                __leave;
            }

            //
            // Decrement counter.
            //
            ulStreamLength -= ulXferSize;
        }


        //
        // All good.
        //
        ulRet = TRUE;
    }
    __finally 
    {
        //
        // Cleanup
        //
        if( hSourceStream != INVALID_HANDLE_VALUE )
        {
            CloseHandle( hSourceStream );
        }

        if( hDestStream != INVALID_HANDLE_VALUE )
        {
            CloseHandle( hDestStream );
        }

        if( hDestFile != INVALID_HANDLE_VALUE )
        {
            CloseHandle(hDestFile);
        }

        if( pXferBuffer )
        {
            delete [] pXferBuffer;
        }

    }

    return ulRet;
}
