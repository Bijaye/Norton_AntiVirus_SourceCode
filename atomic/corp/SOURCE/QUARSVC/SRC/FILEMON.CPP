/////////////////////////////////////////////////////////////////////////////
// Copyright 1998 Symantec Corporation.
/////////////////////////////////////////////////////////////////////////////
// FileMon.cpp
//
// $Header:   S:/QUARSVC/VCS/filemon.cpv   1.1   22 Apr 1998 18:34:24   DBuches  $
//
// Description: Class to monitor and cleanup stale file handles on the
//              Quarantine server
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/QUARSVC/VCS/filemon.cpv  $
// 
//    Rev 1.1   22 Apr 1998 18:34:24   DBuches
// Hooked up stale file handle monitor.
//
//    Rev 1.0   21 Apr 1998 19:25:02   SEDWARD
// Initial revision.
//
//
/////////////////////////////////////////////////////////////////////////////

#include "FSLink.h"
#include "FileMon.h"


// ==== CFileHandleMonitor::CFileHandleMonitor ============================
//
//
// ========================================================================
//  Function created: 4/98, SEDWARD
// ========================================================================

CFileHandleMonitor::CFileHandleMonitor(void)
{
    LTrace("CFileHandleMonitor constructor");
}



// ==== CFileHandleMonitor::~CFileHandleMonitor ============================
//
//
// ========================================================================
//  Function created: 4/98, SEDWARD
// ========================================================================

CFileHandleMonitor::~CFileHandleMonitor(void)
{
    LTrace("CFileHandleMonitor destructor");
}



// ==== AddHandleConnection ===============================================
//
//
// ========================================================================
//  Function created: 4/98, SEDWARD
// ========================================================================

BOOL    CFileHandleMonitor::AddHandleConnection(HANDLE hFileHandle)
{
    // Claim exclusive access
    Lock( SLT_EXCLUSIVE );
    CEnsureExclusiveRelease er( this );

    // Insert this item in our list
    return m_handleList.InsertFileHandle( hFileHandle );

}  // end of "CFileHandleMonitor::AddHandleConnection"



// ==== RemoveHandleConnection ============================================
//
//
// ========================================================================
//  Function created: 4/98, SEDWARD
// ========================================================================

BOOL    CFileHandleMonitor::RemoveHandleConnection(HANDLE hFileHandle)
{
    // Claim exclusive access
    Lock( SLT_EXCLUSIVE );
    CEnsureExclusiveRelease er( this );

    // Remove this item from our list
    return m_handleList.RemoveItem( hFileHandle );

}  // end of "CFileHandleMonitor::RemoveHandleConnection"



// ==== UpdateConnectionTime ==============================================
//
//
// ========================================================================
//  Function created: 4/98, SEDWARD
// ========================================================================

void    CFileHandleMonitor::UpdateConnectionTime(HANDLE hFileHandle)
{
    // Claim shared access
    Lock( SLT_SHARED );
    CEnsureSharedRelease er( this );

    // Get pointer to item we're looking for
    CFileHandleItem* pItem = m_handleList.GetItem( hFileHandle );
    if( pItem == NULL )
        return;

    // Update time
    time( &pItem->m_lastAccessTime );

}  // end of "CFileHandleMonitor::UpdateConnectionTime"



// ==== RemoveAllHandles ==================================================
//
//
// ========================================================================
//  Function created: 4/98, SEDWARD
// ========================================================================

void    CFileHandleMonitor::RemoveAllHandles(void)
{
    // Claim exclusive access
    Lock( SLT_EXCLUSIVE );
    CEnsureExclusiveRelease er( this );

    // Remove all items
    m_handleList.RemoveAllItems();

}  // end of "CFileHandleMonitor::RemoveAllHandles"



// ==== RemoveStaleHandles ================================================
//
//
// ========================================================================
//  Function created: 4/98, SEDWARD
// ========================================================================

void    CFileHandleMonitor::RemoveStaleHandles(DWORD  dwStaleTimeout)
{
    // Claim exclusive access
    Lock( SLT_EXCLUSIVE );
    CEnsureExclusiveRelease er( this );

    // Get list head
    CFileHandleItem* pItem = m_handleList.GetListHead();

    if( pItem == NULL )
        return;

    // Make sure all items are fresh.
    time_t currentTime;

    while( pItem != NULL )
        {
        // Get current time
        time( &currentTime );

        // Check for freshness
        if( currentTime - pItem->m_lastAccessTime > (time_t) dwStaleTimeout )
            {
            // Remove this item from the list.
            CFileHandleItem* pTempItem = pItem;

            // Close file handle
            CloseHandle( pItem->m_fileHandle );

            // Advance to next item.
            pItem = pTempItem->m_nextItem;

            // delete current item
            m_handleList.RemoveItem( pTempItem->m_fileHandle );
            }
        else
            {
            // Advance to next item.
            pItem = pItem->m_nextItem;
            }
        }

}  // end of "CFileHandleMonitor::RemoveStaleHandles"
