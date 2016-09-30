// Copyright 1996-1997 Symantec, Peter Norton Product Group
//****************************************************************************
//
// $Header:   S:/NAVLU/VCS/MyUtils.cpv   1.11   28 Aug 1998 16:16:42   JBRENNA  $
//
// Description:
//   Implementation file.
//
// Contains:
//
// See Also:
//****************************************************************************
// $Log:   S:/NAVLU/VCS/MyUtils.cpv  $
// 
//    Rev 1.11   28 Aug 1998 16:16:42   JBRENNA
// Port changes from QAKN branch to trunk:
// 1. Rev 1.10.1.1   21 Jul 1998 10:01:52   mdunn
//    AddNpsSchedLuEvent() now explicitly sets the bEnabled flag to TRUE on events
//    it adds.
// 
//    Rev 1.10   13 Apr 1998 22:28:28   mdunn
// Removed calls to CTask::SetMTS/NPSMode() since CTask itself handles
// setting the mode now.
// 
//    Rev 1.9   09 Apr 1998 17:09:32   mdunn
// Now using CTaskAPI to add scheduled events.  
// 
//    Rev 1.8   23 Mar 1998 23:03:16   mdunn
// Changed NPS recurrent event time constants (eg HOURLY is now NPS_FREQ_HOURLY).
// 
//    Rev 1.7   29 Dec 1997 11:54:12   JBRENNA
// Use the resource NPS description rather than hard-coding it.
// 
//    Rev 1.6   25 Jul 1997 20:51:38   JBRENNA
// Perform exception handling for COleException in AddNpsSchedLuEvent. Did this
// because the function is documented to throw COleException execeptions.
// 
//    Rev 1.5   08 Jul 1997 22:13:04   KSACKIN
// Changed the name of the LiveUpdate event to be "Run LiveUpdate (for Norton
// AntiVirus).
//
//    Rev 1.4   10 Jun 1997 15:08:24   JBRENNA
// In AddNpsSchedLuEvent, add the /prompt switch to the szPromptParams rather
// than the szCmdLine. "szCmdLine" is meant only for the executable and its
// default arguments. "szPromptParams" are for optional arguments.
//
//    Rev 1.3   10 Jun 1997 09:17:52   JBRENNA
// Put a "Prompt" checkbox on the ScheduleAdd dialog. When checked, the
// NPS Scheduled LiveUpdate command line includes the /PROMPT option.
//
//    Rev 1.2   03 Jun 1997 13:09:44   JBRENNA
// 1. Initialize m_strNpsEventName using resource string rather than hard
//    coding the "Scheduled LiveUpdate" string.
// 2. Change IDS_CMDOPT_* to IDS_CMDLINE_OPT_*.
//
//    Rev 1.1   02 Jun 1997 18:06:00   JBRENNA
// Add function comment for AddNpsSchedLuRetryEvent().
//
//    Rev 1.0   01 Jun 1997 18:48:58   JBRENNA
// Initial revision.
//
//    Rev 1.0   23 May 1997 21:26:34   JBRENNA
// Initial revision.
//
//****************************************************************************

#include "stdafx.h"
#include "afxdisp.h"
#include "platform.h"

#include <stdlib.h>
#include <time.h>

#include "MyUtils.h"
#include "resource.h"

#include "nstdhelp.h"                   // For NpsApi.H
#include "NpsApi.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//************************************************************************
// CMyUtils implementation ***********************************************
//************************************************************************

static CTask      g_task;
static SYMTASKREC g_rTaskInfo;

//////////////////////////////////////////////////////////////////////////
// Description:
//   Constructor ... initialize our member variables.
//////////////////////////////////////////////////////////////////////////
CMyUtils::CMyUtils()
    : m_nRetryMinuteOffset (30 /* minutes */)
{
    m_strNpsEventName.LoadString   (IDS_NPS_EVENT_NAME);
    m_strNpsDescription.LoadString (IDS_NPS_DESCRIPTION);
}


//////////////////////////////////////////////////////////////////////////
// Description:
//   Seed the "rand" function based on the current time. Note that we only
//   seed once per run. Re-seeded on every call limits the number of random
//   numbers returned by "rand".
//////////////////////////////////////////////////////////////////////////
void CMyUtils::SeedRand () const
{
    static BOOL bRandSeeded = FALSE;

    if (!bRandSeeded)
    {
        // Seed the random-number generator with current time so that
        // the numbers will be different every time we run.
        srand ((unsigned)time(NULL));

        bRandSeeded = TRUE;
    }
}

//////////////////////////////////////////////////////////////////////////
// Description:
//   Return a random number. This function ensures that "rand" is seeded
//   before calling "rand".
//
// Returns
//   A random integer number.
//////////////////////////////////////////////////////////////////////////
int CMyUtils::GetRandomNum () const
{
    SeedRand ();
    return rand ();
}

//////////////////////////////////////////////////////////////////////////
// Description:
//   Return a random number within the specified range.
//
// Returns
//   A random integer number.
//////////////////////////////////////////////////////////////////////////
int CMyUtils::GetRandomNumRange (int nStart, int nEnd) const
{
    const   int  nRange   = nEnd - nStart + 1;
    auto    int  nRand;

    nRand = (GetRandomNum() % nRange) + nStart;

    return nRand;
}

//////////////////////////////////////////////////////////////////////////
// Description:
//   Look at the NPS data file to determine if we have any scheduled
//   LiveUpdate events listed.
//
// Returns:
//   TRUE  - when there is one or more Sched LU events already in NPS.
//   FALSE - when there are no Sched LU events in NPS.
//////////////////////////////////////////////////////////////////////////

BOOL CMyUtils::HasNpsSchedLuEvent () const
{
    BOOL   bFound   = FALSE;

    // Init our task object.  This will auto-select the scheduling mode.
    g_task.Init();
    
    if ( g_task.EnumTasks() )
        {
        while ( g_task.NextNAVTask ( &g_rTaskInfo ) )
            {
            if ( g_rTaskInfo.nTaskType == CTT_NAVLU )
                {
                bFound = TRUE;
                break;
                }
            }
        }

    g_task.ResetEnum();

    return bFound;
}

//////////////////////////////////////////////////////////////////////////
// Description:
//   Add a single event to the scheduler (NPS).
//////////////////////////////////////////////////////////////////////////
BOOL CMyUtils::AddNpsSchedLuEvent (BOOL bPrompt, int nDay, const CString &m_strTime) const
{
    BOOL         bStatus     = TRUE;
    COleDateTime DateTime;

    try
    {
        if (!DateTime.ParseDateTime (m_strTime, VAR_TIMEVALUEONLY))
            bStatus = FALSE;
        else
        {
            CTimeSpan TimeSpan (nDay, DateTime.GetHour(), DateTime.GetMinute(), 0);

            AddNpsSchedLuEvent (bPrompt, TimeSpan, CTF_MONTHLY);
        }

    }
    catch (COleException *e)
    {
        // handle the exception here. Blow away the exception so that we do
        // not leak memory.
        e->Delete ();

        bStatus = FALSE;
    }

    return bStatus;
}

//////////////////////////////////////////////////////////////////////////
// Description:
//   Add a single event to the scheduler (NPS).
//////////////////////////////////////////////////////////////////////////
void CMyUtils::AddNpsSchedLuEvent (      BOOL      bPrompt,
                                   const CTimeSpan &TimeSpan,
                                     TASKFREQUENCY nFreq,
                                         BOOL      bRetrySwitch /*= FALSE*/) const
{
    CString      strCmdOpt;

    // Set up the event parameters.
    // ***NOTE*** This will NOT work for weekly events! The code does
    // not calculate the day of the week parameter.

    g_rTaskInfo.nTaskType = CTT_NAVLU;
    g_rTaskInfo.nFrequency = nFreq;
    
    GetLocalTime ( &g_rTaskInfo.rStartTime );

    g_rTaskInfo.rStartTime.wDay    = (WORD)TimeSpan.GetDays();
    g_rTaskInfo.rStartTime.wHour   = TimeSpan.GetHours();
    g_rTaskInfo.rStartTime.wMinute = TimeSpan.GetMinutes();

    lstrcpy ( g_rTaskInfo.szDisplayName, m_strNpsDescription );

    *g_rTaskInfo.szParams = '\0';

    if (bRetrySwitch)
    {
        strCmdOpt.LoadString (IDS_CMDLINE_OPT_INTERNALRETRY);

        lstrcpy ( g_rTaskInfo.szParams, " /" );
        lstrcat ( g_rTaskInfo.szParams, strCmdOpt );
    }

    if (bPrompt)
    {
        strCmdOpt.LoadString (IDS_CMDLINE_OPT_PROMPT);

        lstrcat ( g_rTaskInfo.szParams, " /");
        lstrcat ( g_rTaskInfo.szParams, strCmdOpt );
    }

    g_rTaskInfo.bEnabled = TRUE;
    g_task.SetTaskData ( &g_rTaskInfo );
}

//////////////////////////////////////////////////////////////////////////
// Description:
//   Add some monthly events to the scheduler (NPS) based on the parameter
//   list.
//
// Parameters:
//   nDay1      Day  for 1st event.
//   strTime1   Time for 1st event.
//   nDay2      Optional, Day  for 2nd event.
//   strTime2   Optional, Time for 2nd event.
//
// Returns
//   0      success
//   -1     strTime1 is invalid.
//   -2     strTime2 is invalid.
//   -3     error occured in a CTask function.
//////////////////////////////////////////////////////////////////////////
int CMyUtils::AddNpsSchedLuEvents (BOOL bPrompt,
                                   int nDay1,         const CString &strTime1,
                                   int nDay2 /*= 0*/, const CString &strTime2 /*= ""*/) const
{
    // Initialize our task object.  This will auto-select the scheduling
    // mode.
    if ( !g_task.Init() )
        return -3;

    // Add the Day1 event to NPS always.
    if (!AddNpsSchedLuEvent (bPrompt, nDay1, strTime1))
    {
        return -1;
    }

    if ( !g_task.Save ( CTSA_RENAME_IF_EXISTS ) )
        return -3;


    // Only add Day2 if the caller specified it.
    if (nDay2 != 0)
    {
        if (!AddNpsSchedLuEvent (bPrompt, nDay2, strTime2))
        {
            return -2;
        }

        if ( !g_task.Save ( CTSA_RENAME_IF_EXISTS ) )
            return -3;
    }


    return 0;
}

//////////////////////////////////////////////////////////////////////////
// Description:
//   Add a LiveUpdate retry event to NPS that occurs 1/2 hour from now.
//////////////////////////////////////////////////////////////////////////
void CMyUtils::AddNpsSchedLuRetryEvent () const
{
    struct tm   *curtime;
    time_t      long_time;

    time( &long_time );                /* Get time as long integer. */
    curtime = localtime( &long_time ); /* Convert to local time. */

    // Init the task object.  This will auto-select the scheduling mode.
    if ( !g_task.Init() )
        return;

    CTimeSpan   TimeSpan   (curtime->tm_mday, curtime->tm_hour, curtime->tm_min, 0);
    CTimeSpan   TimeOffset (m_nRetryMinuteOffset * 60 /* convert to secs*/);

    TimeSpan += TimeOffset;

    AddNpsSchedLuEvent (FALSE, TimeSpan, CTF_ONCE, TRUE);

    g_task.Save ( CTSA_OVERWRITE_IF_EXISTS );
}
