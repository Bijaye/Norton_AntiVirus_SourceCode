/////////////////////////////////////////////////////////////////////////////
// Copyright 1998 Symantec Corporation.
/////////////////////////////////////////////////////////////////////////////
// FHList.cpp
//
// $Header:   S:/QUARSVC/VCS/fhlist.cpv   1.1   22 Apr 1998 18:34:22   DBuches  $
//
// Description: Class to manage the list of file handles used by CFileHandleMonitor.
//
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/QUARSVC/VCS/fhlist.cpv  $
// 
//    Rev 1.1   22 Apr 1998 18:34:22   DBuches
// Hooked up stale file handle monitor.
//
//    Rev 1.0   21 Apr 1998 19:25:00   SEDWARD
// Initial revision.
//
//
/////////////////////////////////////////////////////////////////////////////



#include <assert.h>
#include "windows.h"
#include "FHList.h"




// constructors / destructors

// ==== CFileHandleItem::CFileHandleItem ==================================
//
//
// ========================================================================
//  Function created: 4/98, SEDWARD
// ========================================================================

CFileHandleItem::CFileHandleItem(HANDLE  hFile, CFileHandleItem*  nextItem)
{
    m_fileHandle = hFile;
    m_nextItem = nextItem;
    time( &m_lastAccessTime );
}



// ==== CFileHandleList::CFileHandleList ==================================
//
//
// ========================================================================
//  Function created: 4/98, SEDWARD
// ========================================================================

CFileHandleList::CFileHandleList(void)
{
    m_fileHandleList = NULL;
}



// ==== CFileHandleList::~CFileHandleList =================================
//
//
// ========================================================================
//  Function created: 4/98, SEDWARD
// ========================================================================

CFileHandleList::~CFileHandleList(void)
{
    RemoveAllItems();
}










// class implementation

// ==== InsertFileHandle ==================================================
//
//
// ========================================================================
//  Function created: 4/98, SEDWARD
// ========================================================================

BOOL    CFileHandleList::InsertFileHandle(HANDLE  hFile)
{
    auto    CFileHandleItem*    newItem;

    newItem = new CFileHandleItem(hFile, m_fileHandleList);
    if( newItem == NULL )
        {
        assert(FALSE);
        return FALSE;
        }

    m_fileHandleList = newItem;

    return TRUE;

}  // end of "CFileHandleList::InsertFileHandle"



// ==== RemoveItem ========================================================
//
//
// ========================================================================
//  Function created: 4/98, SEDWARD
// ========================================================================

BOOL    CFileHandleList::RemoveItem(HANDLE  hFile)
{
    auto    BOOL                bResult = FALSE;
    auto    CFileHandleItem*    itemPtr1;
    auto    CFileHandleItem*    itemPtr2;


    // if list is empty, return FALSE
    if (NULL == m_fileHandleList)
        {
        goto  Exit_Function;
        }

    // if target handle is at head of list, remove that item from the list
    if (m_fileHandleList->m_fileHandle == hFile)
        {
        itemPtr1 = m_fileHandleList;
        m_fileHandleList = m_fileHandleList->m_nextItem;
        delete  itemPtr1;
        bResult = TRUE;
        goto  Exit_Function;
        }

    // if we got here, we gotta look for the target handle
    itemPtr1 = m_fileHandleList;
    itemPtr2 = m_fileHandleList->m_nextItem;
    while (NULL != itemPtr2)
        {
        if (itemPtr2->m_fileHandle == hFile)
            {
            itemPtr1->m_nextItem = itemPtr2->m_nextItem;
            delete  itemPtr2;
            bResult = TRUE;
            break;
            }
        else
            {
            itemPtr1 = itemPtr2;
            itemPtr2 = itemPtr2->m_nextItem;
            }
        }


Exit_Function:

    return (bResult);

}  // end of "CFileHandleList::RemoveItem"



// ==== RemoveAllItems ====================================================
//
//
// ========================================================================
//  Function created: 4/98, SEDWARD
// ========================================================================

void    CFileHandleList::RemoveAllItems(void)
{
    auto    CFileHandleItem*    currentItem;
    auto    CFileHandleItem*    targetPtr;

    currentItem = m_fileHandleList;
    while (currentItem != NULL)
        {
        targetPtr = currentItem;
        currentItem = currentItem->m_nextItem;
        delete  targetPtr;
        }

    m_fileHandleList = NULL;

}  // end of "CFileHandleList::RemoveAllItems"



// ==== ItemInList ========================================================
//
//
// ========================================================================
//  Function created: 4/98, SEDWARD
// ========================================================================

BOOL    CFileHandleList::ItemInList(HANDLE  hFile)
{
    // if list is empty, return FALSE
    if (NULL == m_fileHandleList)
        {
        return  FALSE;
        }

    // if target handle is at head of list, return TRUE
    if (m_fileHandleList->m_fileHandle == hFile)
        {
        return  TRUE;
        }

    // if we got here, we gotta look for the target handle
    auto    CFileHandleItem*    currentItem;
    currentItem = m_fileHandleList->m_nextItem;
    for (; NULL != currentItem; currentItem = currentItem->m_nextItem)
        {
        if (currentItem->m_fileHandle == hFile)
            {
            return  TRUE;
            }
        }

    return  FALSE;

}  // end of "CFileHandleList::ItemInList"



// ==== IsEmpty ===========================================================
//
//
// ========================================================================
//  Function created: 4/98, SEDWARD
// ========================================================================

BOOL    CFileHandleList::IsEmpty(void)
{
    return ((NULL == m_fileHandleList) ? TRUE : FALSE);

}  // end of "CFileHandleList::IsEmpty"



// ==== GetItem ===========================================================
//
//
// ========================================================================
//  Function created: 4/98, DBUCHES
// ========================================================================

CFileHandleItem* CFileHandleList::GetItem( HANDLE hFile )
{
    // if list is empty, return FALSE
    if (NULL == m_fileHandleList)
        {
        return  NULL;
        }

    // if target handle is at head of list, return TRUE
    if (m_fileHandleList->m_fileHandle == hFile)
        {
        return  m_fileHandleList;
        }

    // if we got here, we gotta look for the target handle
    auto    CFileHandleItem*    currentItem;
    currentItem = m_fileHandleList->m_nextItem;
    for (; NULL != currentItem; currentItem = currentItem->m_nextItem)
        {
        if (currentItem->m_fileHandle == hFile)
            {
            return  currentItem;
            }
        }

    return  NULL;

}// end of "CFileHandleList::GetItem"



