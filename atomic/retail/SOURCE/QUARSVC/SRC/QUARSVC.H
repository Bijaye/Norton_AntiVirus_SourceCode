/////////////////////////////////////////////////////////////////////////////
// Copyright 1998 Symantec Corporation.
/////////////////////////////////////////////////////////////////////////////
// QuarSvc.h
//
// $Header:   S:/QUARSVC/VCS/QuarSvc.h_v   1.2   21 Apr 1998 19:26:48   SEDWARD  $
//
// Description: Header file for the CQuarantineService class
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/QUARSVC/VCS/QuarSvc.h_v  $
// 
//    Rev 1.2   21 Apr 1998 19:26:48   SEDWARD
// Check in work in progress (FileMon stuff).
//
//    Rev 1.1   06 Apr 1998 13:58:56   SEDWARD
// New stuff.
//
/////////////////////////////////////////////////////////////////////////////


#ifndef __QUARSVC_H__
#define __QUARSVC_H__

#if !defined(__SERVICE_H)
#   include "service.h"
#endif


#include "FileMon.h"    // file handle monitor object



#define MAX_THREAD_STACK_SIZE (10240)   // Max Stack size.
#define _LOGFILE_TRACE_                 // generates a debug logfile in Windows directory


// function prototypes
VOID CDECL  LTrace( LPCSTR lpszFmt, ... );
BOOL        FileExists(LPTSTR  lpszFile);
VOID        WorkerThreadProc(LPVOID lpNotUsed);
BOOL _declspec(dllimport) StartFileServer( void );  // from FileSrv.dll




/////////////////////////////////////////////////////////////////////
// CQuarantineService

class CQuarantineService : public CService
{
public:
    CQuarantineService(LPSERVICE_MAIN_FUNCTIONW  pMain, LPHANDLER_FUNCTION  pHandler);
    ~CQuarantineService();

    VOID    InitService(void);
    VOID    ServiceControlStop(void);

    BOOL                m_bExitingService;
    HANDLE              m_hEvent;
    CFileHandleMonitor  m_handleMonitor;
};

#endif //__QUARSVC_H__