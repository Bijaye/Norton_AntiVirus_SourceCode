/////////////////////////////////////////////////////////////////////////////
// Copyright 1998 Symantec Corporation.
/////////////////////////////////////////////////////////////////////////////
// FHList.h
//
// $Header:   S:/QUARSVC/VCS/fhlist.h_v   1.1   22 Apr 1998 18:34:24   DBuches  $
//
// Description: Class to manage the list of file handles used by CFileHandleMonitor.
//
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/QUARSVC/VCS/fhlist.h_v  $
// 
//    Rev 1.1   22 Apr 1998 18:34:24   DBuches
// Hooked up stale file handle monitor.
//
//    Rev 1.0   21 Apr 1998 19:25:00   SEDWARD
// Initial revision.
//
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __FILE_HANDLE_LIST_H__
#define __FILE_HANDLE_LIST_H__

#include <time.h>


// class declaration for list items
class CFileHandleItem
{
public:
    CFileHandleItem(HANDLE  hFile, CFileHandleItem*  nextItem = NULL);
    HANDLE              m_fileHandle;
    time_t              m_lastAccessTime;
    CFileHandleItem*    m_nextItem;

};





// class declaration for list
class CFileHandleList
{
public:
    CFileHandleList(void);
    ~CFileHandleList(void);

    BOOL    InsertFileHandle(HANDLE  hFile);
    int     RemoveItem(HANDLE  hFile);
    void    RemoveAllItems(void);
    BOOL    ItemInList(HANDLE  hFile);
    BOOL    IsEmpty(void);
    CFileHandleItem* GetItem( HANDLE hFile );
    CFileHandleItem* GetListHead() { return m_fileHandleList; };

protected:
    CFileHandleItem*    m_fileHandleList;
};

#endif  // __FILE_HANDLE_LIST_H__
