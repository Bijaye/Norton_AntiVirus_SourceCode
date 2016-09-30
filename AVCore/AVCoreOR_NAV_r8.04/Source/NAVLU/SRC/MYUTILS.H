// Copyright 1997 Symantec, Peter Norton Product Group
//****************************************************************************
//
// $Header:   S:/NAVLU/VCS/MyUtils.h_v   1.4   13 Apr 1998 22:28:50   mdunn  $
//
// Description:
//   Provides the header file for CMyUtils.
//
//****************************************************************************
// $Log:   S:/NAVLU/VCS/MyUtils.h_v  $
// 
//    Rev 1.4   13 Apr 1998 22:28:50   mdunn
// Removed calls to CTask::SetMTS/NPSMode() since CTask itself handles
// setting the mode now.
// 
//    Rev 1.3   09 Apr 1998 17:09:06   mdunn
// Now using CTaskAPI to add scheduled events.
// 
//    Rev 1.2   10 Jun 1997 09:19:08   JBRENNA
// Put a "Prompt" checkbox on the ScheduleAdd dialog. When checked, the
// NPS Scheduled LiveUpdate command line includes the /PROMPT option.
// 
//    Rev 1.1   03 Jun 1997 13:11:10   JBRENNA
// Rename m_szNps* to m_strNps*  ... then are now CStrings.
// 
//    Rev 1.0   01 Jun 1997 18:49:00   JBRENNA
// Initial revision.
// 
//****************************************************************************

#ifndef __MYUTILS_H
#define __MYUTILS_H

#include "ctask.h"

class CMyUtils
{
// Public functions
public:
    // constructor
    CMyUtils();

    // Random number related functions
    void    SeedRand () const;
    int     GetRandomNum () const;
    int     GetRandomNumRange (int nStart, int nEnd) const;

    // NPS related functions
	BOOL    HasNpsSchedLuEvent () const;
    BOOL    AddNpsSchedLuEvent  (BOOL bPrompt, int nDay, const CString &m_strTime) const;

    int     AddNpsSchedLuEvents (BOOL bPrompt,
                                 int nDay1,     const CString &strTime1,
                                 int nDay2 = 0, const CString &strTime2 = "") const;

    void    AddNpsSchedLuRetryEvent () const;

// Private Functions
private:
    void    AddNpsSchedLuEvent (      BOOL      bPrompt,
                                const CTimeSpan &TimeSpan,
                                  TASKFREQUENCY nFreq,
                                      BOOL      bRetrySwitch = FALSE) const;

// Private Data
private:
    CString     m_strNpsEventName;
    CString     m_strNpsDescription;
    int         m_nRetryMinuteOffset;
};

#endif // __MYUTILS_H
