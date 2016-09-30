/////////////////////////////////////////////////////////////////////////////
// Copyright 1998 Symantec Corporation.
/////////////////////////////////////////////////////////////////////////////
// FileMon.h
//
// $Header:   S:/QUARSVC/VCS/filemon.h_v   1.1   22 Apr 1998 18:34:24   DBuches  $
//
// Description: Class to monitor and cleanup stale file handles on the
//              Quarantine server
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/QUARSVC/VCS/filemon.h_v  $
// 
//    Rev 1.1   22 Apr 1998 18:34:24   DBuches
// Hooked up stale file handle monitor.
//
//    Rev 1.0   21 Apr 1998 19:25:02   SEDWARD
// Initial revision.
//
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __FILE_MON_H__
#define __FILE_MON_H__



#include "eresource.h"
#include "FHList.h"

#define MAX_STALE_TIME      ( 60 * 5 )  // Stale time in seconds.


////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////

class CFileHandleMonitor : private CEResource
{
public:
    CFileHandleMonitor();
    ~CFileHandleMonitor();

    // File Operations
    BOOL AddHandleConnection( HANDLE hFileHandle );    // Need exclusive access; call Lock() with SLT_EXCLUSIVE
    BOOL RemoveHandleConnection( HANDLE hFileHandle ); // Need exclusive access; call Lock() with SLT_EXCLUSIVE
    void UpdateConnectionTime( HANDLE hFileHandle );   // Need shared access; call Lock() with SLT_SHARED

    // Global operations
    void RemoveAllHandles();                           // Need exclusive access; call Lock() with SLT_EXCLUSIVE

    void RemoveStaleHandles(DWORD  dwStaleTimeout);    // Need exclusive access; call Lock() with SLT_EXCLUSIVE

protected:
    CFileHandleList     m_handleList;   // object to manipulate file handle list
};

#endif  // __FILE_MON_H__
