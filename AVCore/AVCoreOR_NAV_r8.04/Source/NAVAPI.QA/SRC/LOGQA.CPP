// Copyright 1997 Symantec Corporation, Peter Norton Product Group
//***************************************************************************
//  $Header:   S:/NAVAPI.QA/VCS/logqa.cpv   1.0   05 Aug 1998 07:59:14   mmendon  $
//
//***************************************************************************
//  $Log:   S:/NAVAPI.QA/VCS/logqa.cpv  $
// 
//    Rev 1.0   05 Aug 1998 07:59:14   mmendon
// Initial revision.
// 
//    Rev 1.0   24 Nov 1997 20:15:00   ERAHHAL
// Initial revision.
// 
//***************************************************************************

// Log.cpp: implementation of the CLog class.
//
//////////////////////////////////////////////////////////////////////

#include "Logqa.h"
#include <stdio.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLog::~CLog()
{
#ifdef SYM_WIN32
    cCriticalSectionLock csl(&m_pCriticalSection);
#endif
    Close();
}

void CLog::LogEvent(const CEventObject &Event)
{
#ifdef SYM_WIN32
    cCriticalSectionLock csl(&m_pCriticalSection);
#endif
    fprintf(m_fLog, "%s %s -> %s\n", Event.GetTime(), Event.GetDate(), Event.GetDescription());
    fflush(m_fLog);
}

void CLog::Open()
{
    
    m_fLog = fopen(m_sFileName, "a");

}

BOOL CLog::IsLogOpen()
{
    if (m_fLog)
        return TRUE;
    else
        return FALSE;
}

void CLog::Close()
{
    fclose(m_fLog);
}

CLog::CLog(char * sFileName)
{
    m_bNTEventLog = FALSE;
    m_fLog = NULL;
    strcpy(m_sFileName, sFileName);

    Open();
}

#ifdef SYM_WIN32

CLog::CLog(wchar_t * sFileName)
{
    m_bNTEventLog = FALSE;
    m_fLog = NULL;

    wcstombs( m_sFileName, sFileName, MAX_PATH );

    Open();

}

#endif
