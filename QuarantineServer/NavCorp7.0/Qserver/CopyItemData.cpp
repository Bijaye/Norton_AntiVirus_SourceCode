// CopyItemData.cpp : Implementation of CCopyItemData
#include "stdafx.h"
#include "Qserver.h"
#include "CopyItemData.h"
#include "qspak.h"


/////////////////////////////////////////////////////////////////////////////
// CCopyItemData


///////////////////////////////////////////////////////////////////////////////
//
// Function name: CCopyItemData::Open
//
// Description: 
//
// Return type: STDMETHODIMP 
//
// Argument: LPSTR szFileName
//
///////////////////////////////////////////////////////////////////////////////
// 12/3/98 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CCopyItemData::Open( ULONG ulFileID )
{
USES_CONVERSION;

    HRESULT hr = S_OK;
    HQSERVERITEM hItem = NULL;
    QSPAKSTATUS status;
    try
        {
        // 
        // Close existing file.
        // 
        Close();

        // 
        // Build full path to file.
        // 
        TCHAR szFilePath[ MAX_PATH + 1];
        TCHAR szQuarantineDir[ MAX_PATH + 1 ];
        _Module.GetQuarantineDir( szQuarantineDir );
        wsprintf( szFilePath, _T("%s\\%.8X"), szQuarantineDir, ulFileID );
        
        // 
        // Open file in question.
        // 
        status = QsPakOpenItem( T2A( szFilePath ), &hItem );
        if( QSPAKSTATUS_OK != status )
            {
            switch( status )
                {
                case QSPAKSTATUS_ACCESS_DENIED:
                    return E_ACCESSDENIED;

                case QSPAKSTATUS_MEMORY_ERROR:
                    return E_OUTOFMEMORY;

                case QSPAKSTATUS_FILE_NOT_FOUND:
                    return E_INVALIDARG;

                default:
                    return E_FAIL;
                }
            }

        // 
        // Q: How large is the sample data?
        // 
        if( QSPAKSTATUS_BUFFER_TOO_SMALL == QsPakGetRawFileHere( hItem, NULL, &m_dwSize ) )
            {
            // 
		    // Create file mapping object to store data.
            // 
            m_hMap = CreateFileMapping( (HANDLE) 0xFFFFFFFF,
                                        NULL,
                                        PAGE_READWRITE,
                                        0,
                                        m_dwSize,
                                        NULL );
            if( m_hMap != INVALID_HANDLE_VALUE )
                {
                // 
                // Read raw data into memory mapped file.
                // 
                m_pFileData = (LPBYTE)MapViewOfFile( m_hMap,
                                             FILE_MAP_WRITE,
                                             0,
                                             0,
                                             m_dwSize );
                if( m_pFileData )
                    {
                    if( QSPAKSTATUS_OK != QsPakGetRawFileHere( hItem, m_pFileData, &m_dwSize ) )   
                        {
                        hr = E_FAIL;
                        }
                    }
                else
                    {
                    hr = E_OUTOFMEMORY;
                    }
                }
            else
                {
                hr = E_OUTOFMEMORY;
                }
            }
        else
            {
            hr = E_UNEXPECTED;
            }
        }
    catch(...)
        {
        hr = E_UNEXPECTED;
        }


    if( hItem )
        QsPakReleaseItem( hItem );

    return hr;

}



///////////////////////////////////////////////////////////////////////////////
//
// Function name: CCopyItemData::GetSize
//
// Description: 
//
// Return type: STDMETHODIMP 
//
// Argument: ULONG* pulFileSize
//
///////////////////////////////////////////////////////////////////////////////
// 12/3/98 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CCopyItemData::GetSize( ULONG* pulFileSize )
{
    if( pulFileSize == NULL )
        return E_INVALIDARG;

    // 
    // Get the file size
    // 
    *pulFileSize = m_dwSize;

    return S_OK;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function name: CCopyItemData::Read
//
// Description: 
//
// Return type: STDMETHODIMP 
//
// Argument:  ULONG ulBufferSize
// Argument:  ULONG* pulBytesRead
// Argument:  BYTE* lpBuffer    
//
///////////////////////////////////////////////////////////////////////////////
// 12/3/98 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CCopyItemData::Read(  
                     ULONG ulBufferSize,                       
               	     ULONG* pulBytesRead,                     
               	     BYTE* lpBuffer)
{
    // 
    // Verify input
    //     
    if( pulBytesRead == NULL || lpBuffer == NULL )
        return E_INVALIDARG;

    *pulBytesRead = 0;

    // 
    // Fire off read.
    //     
    HRESULT hr = S_OK;
    DWORD dwBytesToRead;
    try
        {
        // 
        // Figure out how much to read.
        // 
        if( m_dwCurrentOffset + ulBufferSize > m_dwSize )
            dwBytesToRead = m_dwSize - m_dwCurrentOffset;
        else
            dwBytesToRead = ulBufferSize;

        // 
        // Copy data into clients buffer
        // 
        if( dwBytesToRead )
            CopyMemory( lpBuffer, &m_pFileData[ m_dwCurrentOffset ], dwBytesToRead );
        
        // 
        // Bump our offset.
        // 
        m_dwCurrentOffset += dwBytesToRead;

        // 
        // Tell client what we did.
        // 
        *pulBytesRead = dwBytesToRead;
        }
    catch(...)
        {
        hr = E_FAIL;
        }

    return hr;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: CCopyItemData::Close
//
// Description: 
//
// Return type: STDMETHODIMP 
//
//
///////////////////////////////////////////////////////////////////////////////
// 12/3/98 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CCopyItemData::Close()
{
    // 
    // Close files.
    // 
    if( m_pFileData != NULL )
        {
        UnmapViewOfFile( m_pFileData );
        m_pFileData = NULL;
        }

    if( m_hMap != INVALID_HANDLE_VALUE )
        {
        CloseHandle( m_hMap );
        m_hMap = INVALID_HANDLE_VALUE;
        }

    m_dwSize = 0;
    m_dwCurrentOffset = 0;

    return S_OK;
}




///////////////////////////////////////////////////////////////////////////////
//
// Function name : CCopyItemData::FinalRelease
//
// Description   : 
//
// Return type   : void 
//
//
///////////////////////////////////////////////////////////////////////////////
// 2/5/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
void CCopyItemData::FinalRelease()
{
    Close();
}


