// NavAPICallbacks.cpp: implementation of the CNavAPIMemCallbacks class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "NavAPICallbacks.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
//
// Function name : CNavAPIMemCallbacks::CNavAPIMemCallbacks
//
// Description   : Constructor.
//
// Argument      : BOOL bMemory /* = TRUE */
//
///////////////////////////////////////////////////////////////////////////////
// 3/8/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
CNavAPIMemCallbacks::CNavAPIMemCallbacks( )
{
    Open        = CNavAPIMemCallbacks::MemoryOpen;            
    Close       = CNavAPIMemCallbacks::MemoryClose;      
    Seek        = CNavAPIMemCallbacks::MemorySeek;       
    Read        = CNavAPIMemCallbacks::MemoryRead;       
    Write       = CNavAPIMemCallbacks::MemoryWrite;      
    Delete      = CNavAPIMemCallbacks::MemoryDelete;     
    GetDateTime = CNavAPIMemCallbacks::MemoryGetDateTime;
    SetDateTime = CNavAPIMemCallbacks::MemorySetDateTime;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name : CNavAPIMemCallbacks::~CNavAPIMemCallbacks
//
// Description   : Destructor.
//
//
///////////////////////////////////////////////////////////////////////////////
// 3/8/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
CNavAPIMemCallbacks::~CNavAPIMemCallbacks()
{

}



///////////////////////////////////////////////////////////////////////////////
//
// Function name : CNavAPIMemCallbacks::MemoryOpen
//
// Description   : 
//
// Return type   : BOOL 
//
// Argument      :  LPVOID lpvFileInfo
// Argument      : DWORD dwOpenMode
// Argument      : LPVOID FAR *lplpvHandle
//
///////////////////////////////////////////////////////////////////////////////
// 3/8/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
BOOL CNavAPIMemCallbacks::MemoryOpen( LPVOID lpvFileInfo, DWORD dwOpenMode, LPVOID FAR *lplpvHandle )
{
    // 
    // Don't really need to do anything here except reset seek position pointer
    // 
    CMemoryScanData* p = (CMemoryScanData*) lpvFileInfo;
    p->m_dwCurrentPosition = 0;

    *lplpvHandle = p;

    return TRUE;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name : CNavAPIMemCallbacks::MemoryClose
//
// Description   : 
//
// Return type   : BOOL 
//
// Argument      : LPVOID lpvHandle
//
///////////////////////////////////////////////////////////////////////////////
// 3/8/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
BOOL CNavAPIMemCallbacks::MemoryClose( LPVOID lpvHandle )
{
    // 
    // Don't really need to do anything here.
    // 
    return TRUE;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name : CNavAPIMemCallbacks::MemorySeek
//
// Description   : 
//
// Return type   : DWORD 
//
// Argument      :  LPVOID lpvHandle
// Argument      : LONG lOffset
// Argument      : int nFrom
//
///////////////////////////////////////////////////////////////////////////////
// 3/8/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
DWORD CNavAPIMemCallbacks::MemorySeek( LPVOID lpvHandle, LONG lOffset, int nFrom )
{
    CMemoryScanData* p = (CMemoryScanData*) lpvHandle;

    // 
    // Set starting offset
    // 
    switch( nFrom )
        {
        case FILE_BEGIN:
            if( lOffset < 0 || (DWORD) lOffset > p->m_dwDataSize )
                return 0xFFFFFFFF;
            p->m_dwCurrentPosition = lOffset;
            break;

        case FILE_END:
            if( lOffset > 0 || (DWORD) labs( lOffset ) > p->m_dwDataSize )
                return 0xFFFFFFFF;                                    
            p->m_dwCurrentPosition = p->m_dwDataSize - labs( lOffset );
            break;

        case FILE_CURRENT:
            if( lOffset < 0 )
                {
                LONG absOffset = labs( lOffset );
                if( p->m_dwCurrentPosition < (DWORD) absOffset )
                    return 0xFFFFFFFF;
                p->m_dwCurrentPosition -= absOffset;
                }
            else
                {
                if( p->m_dwCurrentPosition + lOffset > p->m_dwDataSize )
                    return 0xFFFFFFFF;
                p->m_dwCurrentPosition += lOffset;
                }
            break;
        }

    // 
    // Return current seek position index.
    // 
    return p->m_dwCurrentPosition;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name : CNavAPIMemCallbacks::MemoryRead
//
// Description   : 
//
// Return type   : UINT 
//
// Argument      :  LPVOID lpvHandle
// Argument      : LPVOID lpvBuffer
// Argument      : UINT uBytesToRead
//
///////////////////////////////////////////////////////////////////////////////
// 3/8/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
UINT CNavAPIMemCallbacks::MemoryRead( LPVOID lpvHandle, LPVOID lpvBuffer, UINT uBytesToRead )
{
    CMemoryScanData* p = (CMemoryScanData*) lpvHandle;

    // 
    // Compute number of bytes to read.
    // 
    DWORD dwBytesToRead = ( p->m_dwCurrentPosition + uBytesToRead > p->m_dwDataSize ) ?
        p->m_dwDataSize - p->m_dwCurrentPosition : uBytesToRead;
    
    // 
    // Copy memory into user buffer.
    // 
    CopyMemory( lpvBuffer, &p->m_pData[ p->m_dwCurrentPosition ], dwBytesToRead );

    // 
    // Bump position.
    // 
    p->m_dwCurrentPosition += dwBytesToRead;

    return dwBytesToRead;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name : CNavAPIMemCallbacks::MemoryWrite
//
// Description   : 
//
// Return type   : UINT 
//
// Argument      :  LPVOID lpvHandle
// Argument      : LPVOID lpvBuffer
// Argument      : UINT uBytesToWrite
//
///////////////////////////////////////////////////////////////////////////////
// 3/8/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
UINT CNavAPIMemCallbacks::MemoryWrite( LPVOID lpvHandle, LPVOID lpvBuffer, UINT uBytesToWrite )
{
    return 0;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name : CNavAPIMemCallbacks::MemoryDelete
//
// Description   : 
//
// Return type   : BOOL 
//
// Argument      : LPVOID lpvFileInfo
//
///////////////////////////////////////////////////////////////////////////////
// 3/8/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
BOOL CNavAPIMemCallbacks::MemoryDelete( LPVOID lpvFileInfo )
{
    return TRUE;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name : CNavAPIMemCallbacks::MemoryGetDateTime
//
// Description   : 
//
// Return type   : BOOL 
//
// Argument      :  LPVOID lpvHandle
// Argument      : UINT uType
// Argument      : LPWORD lpwDate
// Argument      : LPWORD lpwTime
//
///////////////////////////////////////////////////////////////////////////////
// 3/8/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
BOOL CNavAPIMemCallbacks::MemoryGetDateTime( LPVOID lpvHandle, UINT uType, LPWORD lpwDate, LPWORD lpwTime )
{
    return TRUE;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name : CNavAPIMemCallbacks::MemorySetDateTime
//
// Description   : 
//
// Return type   : BOOL 
//
// Argument      :  LPVOID lpvHandle
// Argument      : UINT uType
// Argument      : WORD wDate
// Argument      : WORD wTime
//
///////////////////////////////////////////////////////////////////////////////
// 3/8/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
BOOL CNavAPIMemCallbacks::MemorySetDateTime( LPVOID lpvHandle, UINT uType, WORD wDate, WORD wTime )
{
    return TRUE;
}

