// Copyright 1998 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/NAVAPI.QA/VCS/fileserv.h_v   1.1   18 Aug 1998 17:05:32   mmendon  $
//
// Description: Header for CFileServer class.
//
//************************************************************************
// $Log:   S:/NAVAPI.QA/VCS/fileserv.h_v  $
// 
//    Rev 1.1   18 Aug 1998 17:05:32   mmendon
// Added MTRequestFile and cCriticalSection object for multithreading
// 
//    Rev 1.0   05 Aug 1998 15:19:34   mmendon
// Initial revision.
//
//    Rev 1.0   04 Aug 1998 11:22:08   NMILLER
// Initial revision.
//
//************************************************************************

// CFileServer class.  Pass in a root directory at construction time,
// and the file server class will construct a list of all files
// within that directory (including subdirectories).  Then use the
// Request file method to request the current file, or the next file.
// This was specifically written for automating live virus testing, but
// has been kept general enough that it can be used elsewhere.

// Created 06/14/98 - NMILLER

//#ifndef _FILESERV_H  wra has a problem with this and the corresponding endif
//#define _FILESERV_H


#include "logqa.h"



#define FILE_ARRAY_MAX_SIZE     10000
#define FILE_SERVER_LOGFILE		_T("FileServer.txt")


class CFileServer
{
public:		// Methods for public consumption

    CFileServer();
    ServerInit(LPTSTR lpszRootDirectory);
	~CFileServer();
    LPTSTR RequestFile(BOOL bNewFile = TRUE);
#ifdef SYM_WIN32
    LPTSTR MTRequestFile();
#endif

    DWORD  GetCount(void) {return dwCount;};

private:
  // Private methods

	BOOL Log(LPCTSTR lpszFileName);
	BOOL ConstructList(LPCTSTR lpszPath);
    // Private data
	DWORD dwCount;
	DWORD dwCurrentFile;
    LPTSTR lpFilenameArray[FILE_ARRAY_MAX_SIZE];
	CLog * lpLog;
#ifdef SYM_WIN32
    cCriticalSection CritSection;
#endif

};