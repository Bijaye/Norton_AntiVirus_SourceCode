#include "platform.h"
#include "stack.hpp"

// --------------------------------------------------------------------------
// --------------------------------------------------------------------------

CStackObjects::CStackObjects ( UINT uMemSize )
{
    m_uBlockSize   = uMemSize;
    m_uCount       = 0;
    m_uBufferSize  = 0;
    m_lpBuffer     = NULL;
}

// --------------------------------------------------------------------------
// --------------------------------------------------------------------------

CStackObjects::~CStackObjects()
{
    CStackObjects::Clear();
}

// --------------------------------------------------------------------------
// --------------------------------------------------------------------------

BOOL WINAPI CStackObjects::Push  (      // Add a block of memory to the top
    LPSTR lpMem )                       // of the stack
{
    auto    BOOL    bReturn = TRUE;

    SYM_ASSERT ( m_uBlockSize );

    DWORD dwNeedBytes = ( m_uCount + 1 ) * m_uBlockSize;

    if ( m_uBufferSize < dwNeedBytes )
        {
        if ( m_uBufferSize == 0 )
            {
                                        // -----------------------------------
                                        // Allocate buffer to hold 10 items
                                        // -----------------------------------
            m_uBufferSize = m_uBlockSize * 10;

            m_lpBuffer = (LPSTR)MemAllocPtr ( GHND, m_uBufferSize );

            if ( m_lpBuffer == NULL )
                {
                m_uBufferSize = 0;
                bReturn = FALSE;
                }
            }
        else
            {
                                        // -----------------------------------
                                        // Reallocate buffer for 10 more items
                                        // -----------------------------------
            UINT uNewSize = m_uBufferSize + (m_uBlockSize * 10);

            LPSTR lpTemp = (LPSTR)MemReAllocPtr ( m_lpBuffer, uNewSize, GHND );

            if ( lpTemp )
                {
                m_uBufferSize = uNewSize;
                m_lpBuffer    = lpTemp;
                }
            else
                bReturn = FALSE;
            }
        }

    if ( bReturn == TRUE )
        {
                                        // -----------------------------------
                                        // If enough memory or reallocation
                                        // was successful.
                                        // -----------------------------------
        DWORD dwOffset = m_uCount * m_uBlockSize;

        MEMCPY ( &m_lpBuffer[dwOffset], lpMem, m_uBlockSize );

        m_uCount++;
        }

    return ( bReturn );
}

// --------------------------------------------------------------------------
// --------------------------------------------------------------------------

BOOL WINAPI CStackObjects::Pop (        // Remove a block of memory from the
    LPSTR lpMem )                       // bottom of the stack
{
    auto    BOOL    bReturn = TRUE;

    SYM_ASSERT ( m_uBlockSize );
    SYM_ASSERT ( m_lpBuffer );
    SYM_ASSERT ( lpMem );

    if ( m_uCount )
        {
        MEMCPY ( lpMem, m_lpBuffer, m_uBlockSize );

        if ( --m_uCount )
            {
            UINT uBytes = m_uCount * m_uBlockSize;

            MEMMOVE ( m_lpBuffer, &m_lpBuffer[m_uBlockSize], uBytes );
            }
        }
    else
        bReturn = FALSE;

    return ( bReturn );
}

// --------------------------------------------------------------------------
// --------------------------------------------------------------------------

BOOL WINAPI CStackObjects::GetObject (  // Get block of memory
    DWORD   dwIndex,                    // from this index location and 
    LPSTR   lpMem )                     // place it in here
{
    auto    BOOL    bReturn = TRUE;

    if ( m_uCount && m_uCount > dwIndex )
        {
        DWORD dwOffset = dwIndex * m_uBlockSize;
        MEMCPY ( lpMem, &m_lpBuffer[ dwOffset ], m_uBlockSize );
        }
    else
        bReturn = FALSE;

    return ( bReturn );
}

// --------------------------------------------------------------------------
// --------------------------------------------------------------------------

DWORD WINAPI CStackObjects::GetCount (  // Get number of blocks of memory
        VOID )                          // pushed to the stack
{
    return ( m_uCount );
}
// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
    
BOOL WINAPI CStackObjects::Clear (      // Clears the stack from everything
        VOID )
{
    m_uBlockSize  = 0;
    m_uCount      = 0;
    m_uBufferSize = 0;
    m_uBlockSize  = 0;
    
    if ( m_lpBuffer )
        MemFreePtr ( m_lpBuffer );

    return ( TRUE );
}
