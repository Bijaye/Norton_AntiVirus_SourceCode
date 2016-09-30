// Copyright 1997 Symantec Corporation, Peter Norton Product Group
//***************************************************************************
//  $Header:   S:/NAVAPI.QA/VCS/eventobj.h_v   1.0   05 Aug 1998 07:58:52   mmendon  $
//
//  $Description: $
//
//***************************************************************************
//  $Log:   S:/NAVAPI.QA/VCS/eventobj.h_v  $
// 
//    Rev 1.0   05 Aug 1998 07:58:52   mmendon
// Initial revision.
// 
//    Rev 1.2   24 Nov 1997 10:32:58   ERAHHAL
// Fixed constant collision with CLink
// 
//    Rev 1.1   21 Nov 1997 17:21:08   ERAHHAL
// Now Unicode compatible
// 
//    Rev 1.0   21 Nov 1997 11:22:34   ERAHHAL
// Initial revision.
// 
//***************************************************************************


// EventObject.h: interface for the CLogObject class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _EVENTOBJ_H
#define _EVENTOBJ_H

#include "platform.h"
#include <time.h>

#ifdef SYM_WIN32
  #include <TCHAR.h>
#endif


const unsigned int MAX_LOG_STRING_LENGTH = 1024;

class CEventObject  
{
public:
    CEventObject();

#ifdef SYM_WIN32
    CEventObject(const wchar_t *lpszMessage);
#endif

    enum EventType { L_INFO, L_WARN, L_ERROR };

    CEventObject(const char *lpszMessage);
    virtual ~CEventObject();

    const char * GetDescription() const
    {
        return m_sDescription;
    }

    const char * GetTime() const
    {
        return m_sTime;
    }

    const char * GetDate() const
    {
        return m_sDate;
    }

private:

    char *m_sDate;
    char *m_sTime;
    char m_sComputerName[MAX_LOG_STRING_LENGTH];
    char *m_sDescription;
    long m_EventType;
    char m_sSource[MAX_LOG_STRING_LENGTH];
    long m_lErrorNumber;

};

#endif
