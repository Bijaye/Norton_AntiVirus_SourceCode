/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/

// FetchFileData.cpp: implementation of the CFetchFileData class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "FetchFileData.h"
#include "process.h"
#import "qserver.tlb" no_namespace
#include "macros.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
//
// Function name : CFetchFileData::CFetchFileData
//
// Description   : 
//
// Return type   : 
//
//
///////////////////////////////////////////////////////////////////////////////
// 2/23/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
CFetchFileData::CFetchFileData() 
{
    m_adwFileIDs = NULL;
    m_dwThread = 0xFFFFFFFF;
    m_hAbort = ::CreateEvent( NULL, FALSE, FALSE, NULL );
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name : CFetchFileData::~CFetchFileData
//
// Description   : 
//
// Return type   : 
//
//
///////////////////////////////////////////////////////////////////////////////
// 2/23/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
CFetchFileData::~CFetchFileData()
{
    if( m_adwFileIDs )
        delete [] m_adwFileIDs;

    // 
    // Zero buffer to write to file.
    // 
    BYTE* pZero = new BYTE[ 0xFFFF ];
    ZeroMemory( pZero, 0xFFFF );

    // 
    // Turn off AP for this operation.
    // 
    DisableAP();

    // 
    // Clean up any files we created.
    // 
    CString s;
    POSITION pos = m_FileList.GetHeadPosition();
    while( pos )
        {
        // 
        // Need to obliterate this file.
        // 
        s = m_FileList.GetNext( pos );
        try
            {
            CFile file( s, CFile::modeWrite );

            // 
            // Fill file with 0s
            // 
            ULONGLONG ullSize = file.GetLength();
            DWORD dwBytesToWrite;
    
            while( ullSize )
                {
                dwBytesToWrite = ullSize > 0xFFFF ? 0xFFFF : (DWORD) ullSize;
                file.Write( pZero, dwBytesToWrite );
                ullSize -= dwBytesToWrite;                                    
                }

            // 
            // Truncate file to 0 bytes in length.
            // 
            file.SetLength(0);
            file.Close();
            }
        catch(...)
            {
            }
        
        ::DeleteFile( s );
        }

    // 
    // Turn AP back on.
    // 
    EnableAP();

    // 
    // Clean up event resource.
    // 
    ::CloseHandle( m_hAbort );

    if( pZero )
        {
        delete [] pZero;
        }

}



///////////////////////////////////////////////////////////////////////////////
//
// Function name : CFetchFileData::Abort
//
// Description   : Aborts file transfer.
//
// Return type   : void 
//
//
///////////////////////////////////////////////////////////////////////////////
// 3/2/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
void CFetchFileData::Abort()
{
    // 
    // Set abort event.
    // 
    SetEvent( m_hAbort );

    // 
    // Wait here forever for thread to stop.
    // 
    WaitForSingleObject( (HANDLE) m_dwThread, INFINITE );

}


///////////////////////////////////////////////////////////////////////////////
//
// Function name : CFetchFileData::GetFile
//
// Description   : 
//
// Return type   : HRESULT 
//
// Argument      : DWORD dwFileID 
// Argument      : LPUNKNOWN pUnk
// Argument      : LPCTSTR szDestPath
// Argument      : LPFNFETCHCALLBACK pCallback
//
///////////////////////////////////////////////////////////////////////////////
// 2/23/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
HRESULT CFetchFileData::GetFiles( DWORD* adwFileIDs, 
                                 DWORD dwNumFiles,
                                 LPUNKNOWN pUnk, 
                                 LPCTSTR szDestPath,
                                 LPFNFETCHCALLBACK pCallback,
                                 DWORD dwUserData )
{
    HRESULT hr = S_OK;

    // 
    // Validate input.
    // 
    if( pUnk == NULL || szDestPath == NULL || pCallback == NULL )
        return E_INVALIDARG;

    try
        {
        // 
        // Save off data
        // 
        m_sPath = szDestPath;
        m_dwNumFiles = dwNumFiles;
        m_adwFileIDs = new DWORD[ dwNumFiles ];
        CopyMemory( m_adwFileIDs, adwFileIDs, sizeof(DWORD) * dwNumFiles );
        m_dwUserData = dwUserData;
        m_pCallback = pCallback;

        // 
        // Marshal copy interface.
        // 
        hr = CoMarshalInterThreadInterfaceInStream( __uuidof( ICopyItemData ), pUnk, &m_pStream );
        if( FAILED( hr ) )
            return hr;

        // 
        // Fire of background thread.
        // 
        m_dwThread = _beginthread( &CFetchFileData::ThreadFunc, 0, this );
        if( m_dwThread == 0xFFFFFFFF )
            return E_FAIL;
        }
    catch(...)
        {
        hr = E_FAIL;
        }

    return hr;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name : CFetchFileData::ThreadFunc
//
// Description   : 
//
// Return type   : void 
//
// Argument      : void* pData
//
///////////////////////////////////////////////////////////////////////////////
// 2/23/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
void CFetchFileData::ThreadFunc( void* pData )
{
    HANDLE hDestFile = INVALID_HANDLE_VALUE;
    LPBYTE pXferBuffer = NULL;
    BOOL bError = FALSE;
    CString sFileName;
    DWORD dwFileSize;
    DWORD dwTotalBytesRead;
    DWORD dwBytesRead;
    DWORD i;
    BOOL bAbort = FALSE;
    
    CoInitialize( NULL );

    // 
    // Save off data 
    // 
    CFetchFileData* pThis = ( CFetchFileData* ) pData;

    // 
    // Unmarshal our copy interface.
    // 
    CComPtr< ICopyItemData > pCopy;
    if( FAILED( CoGetInterfaceAndReleaseStream( pThis->m_pStream, __uuidof( ICopyItemData ), (LPVOID*)&pCopy ) ) )
        {
        pThis->m_pCallback( COPY_STATUS_ERROR, pThis->m_adwFileIDs[0], sFileName, pThis->m_dwUserData );
        CoUninitialize();
        return;
        }

    // 
    // Need to set proxy blanket
    //
    if( FAILED( CoSetProxyBlanket( pCopy,
                       RPC_C_AUTHN_WINNT,
                       RPC_C_AUTHZ_NONE,
                       NULL,
                       RPC_C_AUTHN_LEVEL_CONNECT,
                       RPC_C_IMP_LEVEL_IMPERSONATE,
                       (_COAUTHIDENTITY*) pThis->m_pIdentity,
                       EOAC_NONE) ) )
        {
        pThis->m_pCallback( COPY_STATUS_ERROR, pThis->m_adwFileIDs[0], sFileName, pThis->m_dwUserData );
        CoUninitialize();
        return;
        }

    // 
    // Need to turn off AP for this thread.
    // 
    DisableAP();

    try
        {
        // 
        // Allocate transfer buffer
        // 
        pXferBuffer = new BYTE[ 0xFFFF ];

        for( i = 0; i < pThis->m_dwNumFiles && bAbort == FALSE; i++ )
            {
            dwTotalBytesRead = 0;
            
            // 
            // Open file on remote machine.
            // 
            try
                {
                pCopy->Open( pThis->m_adwFileIDs[i] );
                }
            catch( _com_error e )
                {
                // 
                // Tell user that something bad has happened.
                // 
                if( FALSE == pThis->m_pCallback( COPY_STATUS_FILE_DOES_NOT_EXIST, 
                                                pThis->m_adwFileIDs[i], 
                                                sFileName, pThis->m_dwUserData ) )
                    {
                    bAbort = TRUE;
                    }
                continue;
                }

            // 
            // Get file size.
            // 
            pCopy->GetSize( &dwFileSize );

            // 
            // Create destination file
            //
            GetTempFileName( pThis->m_sPath, NULL, 0, sFileName.GetBuffer( MAX_PATH ) ); 
            sFileName.ReleaseBuffer();
            hDestFile = CreateFile( sFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL );
            if( hDestFile == INVALID_HANDLE_VALUE )
                {
                throw (DWORD) COPY_STATUS_ERROR_DISK_SPACE;            
                }

            // 
            // Add created file to list
            // 
            pThis->m_FileList.AddTail( sFileName );  

            // 
            // Copy file data
            // 
            while( dwTotalBytesRead != dwFileSize )
                {
                // 
                // Read a chunk.
                // 
                pCopy->Read( 0xFFFF, &dwBytesRead, pXferBuffer );

                // 
                // Write a chunk.
                //
                DWORD dwBytesWritten;
                if( FALSE == WriteFile( hDestFile, pXferBuffer, dwBytesRead, &dwBytesWritten, NULL ) )
                    {
                    throw (DWORD) COPY_STATUS_ERROR_DISK_SPACE;
                    }

                // 
                // Adjust counters.
                // 
                dwTotalBytesRead += dwBytesRead;

                // 
                // Tell client what happened.
                // 
                if( FALSE == pThis->m_pCallback( COPY_STATUS_OK, pThis->m_adwFileIDs[i], sFileName, pThis->m_dwUserData ) ||
                    WaitForSingleObject( pThis->m_hAbort, 0 ) != WAIT_TIMEOUT )
                    {
                    bAbort = TRUE;
                    break;
                    }
                }

            // 
            // Close file.
            // 
            if( hDestFile != INVALID_HANDLE_VALUE )
                {
                CloseHandle( hDestFile );
                hDestFile = INVALID_HANDLE_VALUE;
                }

            // 
            // Tell user that this file is done.
            // 
            if( bAbort == FALSE )
                {
                if( FALSE == pThis->m_pCallback( COPY_STATUS_FILE_DONE, pThis->m_adwFileIDs[i], sFileName, pThis->m_dwUserData ) )
                    {
                    bAbort = TRUE;
                    }
                }
            }
        }
    catch( DWORD dwError )
        {
        pThis->m_pCallback( dwError, pThis->m_adwFileIDs[i], sFileName, pThis->m_dwUserData );
        bError = TRUE;
        }
    catch( ... )
        {
        pThis->m_pCallback( COPY_STATUS_ERROR, pThis->m_adwFileIDs[i], sFileName, pThis->m_dwUserData );
        bError = TRUE;
        }

    // 
    // Tell user that we are all done.
    // 
    pThis->m_pCallback( COPY_STATUS_JOB_DONE, NULL, NULL, pThis->m_dwUserData );

    // 
    // Cleanup
    // 
    if( hDestFile != INVALID_HANDLE_VALUE )
        CloseHandle( hDestFile );

    if( pXferBuffer )
        delete [] pXferBuffer;

    CoUninitialize();

    // 
    // Need to turn AP back on.
    // 
    EnableAP();
}

