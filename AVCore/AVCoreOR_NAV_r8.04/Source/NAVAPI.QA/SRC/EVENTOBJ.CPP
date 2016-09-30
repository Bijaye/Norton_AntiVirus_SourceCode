// Copyright 1997 Symantec Corporation, Peter Norton Product Group
//***************************************************************************
//  $Header:   S:/NAVAPI.QA/VCS/eventobj.cpv   1.0   05 Aug 1998 07:58:38   mmendon  $
//
//  $Description: $
//
//***************************************************************************
//  $Log:   S:/NAVAPI.QA/VCS/eventobj.cpv  $
// 
//    Rev 1.0   05 Aug 1998 07:58:38   mmendon
// Initial revision.
// 
//    Rev 1.1   21 Nov 1997 17:20:04   ERAHHAL
// Now Unicode compatible
// 
//    Rev 1.0   21 Nov 1997 11:21:34   ERAHHAL
// Initial revision.
// 
//***************************************************************************


// EventObject.cpp: implementation of the CEventObject class.
//
//////////////////////////////////////////////////////////////////////

#include "EventObj.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CEventObject::~CEventObject()
{
    delete m_sTime;
    delete m_sDate;
    delete m_sDescription;
}

CEventObject::CEventObject(const char *lpszMessage)
{
    time_t DateTime;
    struct tm *tmToday;
    char ampm[] = "AM";
    char sTemp[128];
    
    time( &DateTime );
    _tzset();
    tmToday = localtime(&DateTime);
    if (tmToday->tm_hour > 11)
    {
        strcpy(ampm, "PM");
        tmToday->tm_hour -= 12;
    }
    if (tmToday->tm_hour == 0)
    tmToday->tm_hour = 12;
    sprintf(sTemp, "%.8s %s", asctime(tmToday) + 11, ampm);
    
    m_sTime = new char[strlen(sTemp)+1];
    strcpy(m_sTime, sTemp);
 
    _strdate(sTemp);
    m_sDate = new char[strlen(sTemp)+1];
    strcpy(m_sDate, sTemp);

    m_lErrorNumber = NULL;
    m_EventType = L_INFO;

    strcpy(m_sComputerName, "");
    strcpy(m_sSource, "");

    m_sDescription = new char[strlen(lpszMessage)+1];
    strcpy(m_sDescription, lpszMessage);
}


#ifdef SYM_WIN32

CEventObject::CEventObject(const wchar_t * lpszMessage)
{
    time_t DateTime;
    struct tm *tmToday;
    char ampm[] = "AM";
    char sTemp[128];
    long Length;
    
    time( &DateTime );
    _tzset();
    tmToday = localtime(&DateTime);
    if (tmToday->tm_hour > 11)
    {
        strcpy(ampm, "PM");
        tmToday->tm_hour -= 12;
    }
    if (tmToday->tm_hour == 0)
    tmToday->tm_hour = 12;
    sprintf(sTemp, "%.8s %s", asctime(tmToday) + 11, ampm);
    
    m_sTime = new char[strlen(sTemp)+1];
    strcpy(m_sTime, sTemp);
 
    _strdate(sTemp);
    m_sDate = new char[strlen(sTemp)+1];
    strcpy(m_sDate, sTemp);

    m_lErrorNumber = NULL;
    m_EventType = L_INFO;

    strcpy(m_sComputerName, "");
    strcpy(m_sSource, "");
    
    Length = wcslen(lpszMessage);    
    m_sDescription = new char[Length+1];
    wcstombs( m_sDescription, lpszMessage, Length );
    m_sDescription[Length] = NULL;
   
}

#endif

