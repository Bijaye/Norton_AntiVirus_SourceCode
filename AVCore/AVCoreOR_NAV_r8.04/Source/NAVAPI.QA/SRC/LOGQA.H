// Copyright 1997 Symantec Corporation, Peter Norton Product Group
//***************************************************************************
//  $Header:   S:/NAVAPI.QA/VCS/logqa.h_v   1.0   05 Aug 1998 07:59:32   mmendon  $
//
//***************************************************************************
//  $Log:   S:/NAVAPI.QA/VCS/logqa.h_v  $
// 
//    Rev 1.0   05 Aug 1998 07:59:32   mmendon
// Initial revision.
// 
//    Rev 1.0   24 Nov 1997 20:21:06   ERAHHAL
// Initial revision.
// 
//***************************************************************************

// Logqa.h: interface for the CLog class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _LOGQA_H
#define _LOGQA_H

#ifdef SYM_WIN32
    #include "cCritSec.h"
    #include <TCHAR.H>
#endif

#include "EventObj.h"
#include <stdio.h>


class CLog  
{
public:
	CLog(wchar_t * sFileName);
    CLog(char * sFileName);
    virtual ~CLog();

    BOOL IsLogOpen();
    void LogEvent(const CEventObject &Event);

private:
    CLog();
    void Open();
    void Close();
    char m_sFileName[SYM_MAX_PATH+1];
    FILE * m_fLog;
    BOOL m_bNTEventLog;
#ifdef SYM_WIN32
        cCriticalSection m_pCriticalSection;
#endif    
    
    
};

#endif
