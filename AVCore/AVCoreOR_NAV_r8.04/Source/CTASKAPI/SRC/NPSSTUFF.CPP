// Copyright 1998 Symantec Corporation
//////////////////////////////////////////////////////////////////////////////
// $Header:   S:/CTASKAPI/VCS/NPSSTUFF.CPv   1.4   12 Jun 1998 16:36:40   mdunn  $
//
// Description: Functions for interfacing with the NPS task database.
//
// Contains:
//
// See Also: MTSStuff.cpp, CTask.cpp
//
//////////////////////////////////////////////////////////////////////////////
// $Log:   S:/CTASKAPI/VCS/NPSSTUFF.CPv  $ *
// 
//    Rev 1.4   12 Jun 1998 16:36:40   mdunn
// Added support for enabling/disabling events
// 
//    Rev 1.3   17 Apr 1998 17:28:42   mdunn
// In SaveToNPS(), now calling CalculateNextRunTime() when adding a 
// repeating task.
// 
//    Rev 1.2   17 Apr 1998 15:32:56   mdunn
// Fixed NPS event enumeration - now returning the next-run time for an
// NPS event.
// 
//    Rev 1.1   09 Apr 1998 10:45:48   mdunn
// Removed #include <mstask.h> since that's now done in CTask.h
// 
//    Rev 1.0   06 Apr 1998 17:37:20   mdunn
// Initial revision.
//
//////////////////////////////////////////////////////////////////////////////

#include "platform.h"
#include <tchar.h>
#include <commctrl.h>
#include <afxpriv.h>
#include <string.h>

#include "ctask.h"

                                        // This fn is in nps.h but including
                                        // that header causes some build errors
                                        // that I really don't have the time to
                                        // investigate right now....&^
void CalculateNextRunTime ( NPSEVNT* );

//////////////////////////////////////////////////////////////////////////////

// NOTE that I didn't make this Unicode-enabled... it seemed too tricky to 
// worry about now.  It would involve changing lstrcpy() to _mbscpy() and
// using the T2A() macro on the source string.
BOOL CTask::SaveToNPS ( TASKSAVEACTION nAction /*=CTSA_OVERWRITE_IF_EXISTS*/ )
{
BOOL     bRetVal = FALSE;
WORD     wCurEvent;
NPSEVNT* pEvent = NULL;
BOOL     bMakeNewEvent = TRUE;
TCHAR    szNewEventDescr [MAX_DESCR];   // description of the new event

    ASSERT ( m_nTaskType != CTT_INVALID );

    if ( m_nTaskType == CTT_RUNAPP &&
         ( NULL ==  m_pszAppPath || NULL ==  m_pszDisplayName   ||
           '\0' == *m_pszAppPath || '\0' == *m_pszDisplayName   ) )
        {
        TRACE("Required param not set (app path or job name).\n");
        return FALSE;
        }

    if ( m_nTaskType == CTT_NAVSCANTASK  &&
         ( NULL == m_pszScanTask || '\0' == *m_pszScanTask ) )
        {
        TRACE("Required param not set - scan task name.\n");
        return FALSE;
        }

    if ( ( m_nTaskType == CTT_NAVLU   ||
           m_nTaskType == CTT_NAVSCAN ||
           m_nTaskType == CTT_NAVSCANTASK ) &&
         !m_bNAVInstalled )
        {
        TRACE("Can't add a NAV LU or NAV scan job when NAV isn't installed.\n");
        return FALSE;
        }


#ifdef _DEBUG
    if ( m_bEnumeratingNPSTasks )
        TRACE("Warning: Saving an NPS event while enumerating NPS events.  Call ResetEnum() before Save() to be on the safe side.");
#endif


                                        // Build the job name.
    
    switch ( m_nTaskType )
        {
        // For scan tasks, the display name can be NULL, and it
        // defaults to the same as the name of the scan task.
        case CTT_NAVSCANTASK:
            {
            if ( NULL == m_pszDisplayName  ||
                 '\0' == *m_pszDisplayName )
                {
                lstrcpy ( szNewEventDescr, m_pszScanTask );
                }
            else
                {
                lstrcpy ( szNewEventDescr, m_pszDisplayName );
                }
            }
        break;

        case CTT_NAVSCAN:
            {
            if ( NULL == m_pszDisplayName  ||
                 '\0' == *m_pszDisplayName )
                {
                GetPrivateProfileString (
                    szNAVScanSection, szIniDefDesc,
                    szNAVScanSection,   //&^ default job name is same as section name
                    szNewEventDescr, 
                    MAX_DESCR, szNortonIni );
                }
            else
                lstrcpy ( szNewEventDescr, m_pszDisplayName );
            }
        break;

        case CTT_NAVLU:
            {
            if ( NULL == m_pszDisplayName  ||
                 '\0' == *m_pszDisplayName )
                {
                GetPrivateProfileString (
                    szNAVLUSection, szIniDefDesc,
                    szNAVLUSection,     //&^ default job name is same as section name
                    szNewEventDescr, 
                    MAX_DESCR, szNortonIni );
                }
            else
                lstrcpy ( szNewEventDescr, m_pszDisplayName );
            }
        break;

        case CTT_RUNAPP:
        case CTT_MESSAGE:
            lstrcpy ( szNewEventDescr, m_pszDisplayName );
        break;
        
        default:
            ASSERT(0);                  // should never get here
        break;
        }


    // This is kinda blowie, but oh well.  If I had that month back this
    // would be better code.  Anyway...
    // The Atomic version of this class didn't look at the nAction param at
    // all; all events got stuck in NPS with whatever name the caller sent in,
    // since NPS allows 2 events to have the same name.
    // Now in Odyssey, the UI can edit events with the scheduling wizard.
    // That means it has to replace events by setting nAction to
    // CTSA_OVERWRITE_IF_EXISTS.  
    // What I do here is if nAction is that value, search the Events[]
    // array for an event with that name, and if found, reuse that slot
    // in the array.

    NpsEventsGet(TRUE);                 // load events from the DAT file

    if ( CTSA_OVERWRITE_IF_EXISTS == nAction )
        {
        for ( int i = 0; i < wNumEvents; i++ )
            {
            if ( 0 == lstrcmpi ( Events[i].szDescr, szNewEventDescr ))
                {
                // Found a matching name!
                wCurEvent = i;
                bMakeNewEvent = FALSE;
                break;
                }
            }
        }

    if ( bMakeNewEvent )
        {
        wCurEvent = wNumEvents++;
        }

    MkEvent ( wCurEvent );              // Create a new blank event in the array.
    pEvent = &Events[wCurEvent];

                                        // Set the event's display name.
    lstrcpy ( pEvent->szDescr, szNewEventDescr );


                                        // Set the "name" of the event.  NPS
                                        // uses this when displaying the job in
                                        // its Add/Edit dialog.
    switch ( m_nTaskType )
        {
        case CTT_NAVSCANTASK:
            // nothing to do here - this will never appear in the NPS 
            // UI since that is now history.
        break;

        case CTT_NAVSCAN:
            lstrcpy ( pEvent->szNamed, szEventNAVScan );
        break;

        case CTT_NAVLU:
            lstrcpy ( pEvent->szNamed, szEventNAVLU );
        break;

        case CTT_RUNAPP:
            lstrcpy ( pEvent->szNamed, szEventProgram );
        break;

        case CTT_MESSAGE:
            lstrcpy ( pEvent->szNamed, szEventMessage );
        break;

        default:
            ASSERT(0);                  // should never get here
        break;
        }


                                        // Set the command line to run.
                                        // We also check that the app name
                                        // + parameters won't overflow the
                                        // NPSEVNT buffer.

    if ( m_nTaskType == CTT_NAVSCAN )
        {
        GetPrivateProfileString ( szNAVScanSection, szIniCmdLine, "",
                                  m_pszAppPath, MAX_PATH, szNortonIni );

        if ( '\0' == *m_pszParams )
            {
            GetPrivateProfileString ( szNAVScanSection, szIniDefActionText, "",
                                      m_pszParams, MAX_PATH, szNortonIni );
            }
        }
    else if ( m_nTaskType == CTT_NAVSCANTASK )
        {
        // The app and params for scan tasks are fixed, not read from
        // SCHEDULE.INI.

        wsprintf ( m_pszAppPath, _T("\"%s\\%s\""), 
                   static_cast<LPCTSTR>( m_szNAVDir ), NAV_SCAN_HANDLER_EXE );

        wsprintf ( m_pszParams, _T("\"%s\""), m_pszScanTask );
        }
    else if ( m_nTaskType == CTT_NAVLU )
        {
        GetPrivateProfileString ( szNAVLUSection, szIniCmdLine, "",
                                  m_pszAppPath, MAX_PATH, szNortonIni );

        if ( '\0' == *m_pszParams )
            {
            GetPrivateProfileString ( szNAVLUSection, szIniDefActionText, "",
                                      m_pszParams, MAX_PATH, szNortonIni );
            }
        }
    else if (m_nTaskType == CTT_RUNAPP)
    {
    	TCHAR szTempAppPath[MAX_CMDLINE] = {0};
    	
    	if (*m_pszAppPath != '"')
    	{
    		wsprintf(szTempAppPath, _T("\"%s\""), m_pszAppPath);
    		lstrcpy(m_pszAppPath, szTempAppPath);
    	}
    }


    if ( m_nTaskType != CTT_MESSAGE )
        {
                                        // The space needed for the cmd line is
                                        // the length of the app path + length of
                                        // params + 1 (the space in between).
        if ( 1 + lstrlen (m_pszAppPath) + lstrlen (m_pszParams) >= MAX_CMDLINE )
            {
            TRACE("Command line is too long, bailing out.");
            goto bailout;
            }

        if ( m_nTaskType == CTT_RUNAPP ||
             m_nTaskType == CTT_NAVSCANTASK )
            {            	
            lstrcpy ( pEvent->szCmdLine, m_pszAppPath );

            if ( NULL != m_pszParams  &&  '\0' != *m_pszParams )
                {
                lstrcat ( pEvent->szCmdLine, " " );
                lstrcat ( pEvent->szCmdLine, m_pszParams );
                }
            }
        else
            {
                                        // NAV Scan and NAVLU events have the
                                        // parameters stored in the szPromptParams
                                        // member of the NPSEVNT struct.
            lstrcpy ( pEvent->szCmdLine, m_pszAppPath );
            lstrcpy ( pEvent->szPromptParams, m_pszParams );
            }
        }


                                        // For message events, store the msg
    if ( m_nTaskType == CTT_MESSAGE )
        {
        lstrcpy ( pEvent->szMessage, m_pszMsg );
        }


                                        // Set the starting directory
    if ( m_nTaskType != CTT_MESSAGE )
        {
        if ( m_nTaskType == CTT_NAVSCAN  ||  m_nTaskType == CTT_NAVLU )
            {
                                        // We don't need a starting dir for
                                        // scan/LU events.
            *pEvent->szStartupDir = '\0';
            }
        else if ( m_nTaskType == CTT_NAVSCANTASK )
            {
                                        // ScnHndlr.exe is always in the
                                        // NAV install dir.
            wsprintf ( pEvent->szStartupDir, _T("\"%s\""), m_szNAVDir );
            }
        else if ( NULL != m_pszStartingDir && '\0' != *m_pszStartingDir )
            {
            lstrcpy ( pEvent->szStartupDir, m_pszStartingDir );
            }
        }


                                        // Set the task type so the enumeration
                                        // functions can get the type quickly.
    switch ( m_nTaskType )
        {
        case CTT_NAVSCAN:
        case CTT_NAVLU:   pEvent->wType = EVENT_TYPE_NAMED; break;

        case CTT_MESSAGE: pEvent->wType = EVENT_TYPE_MESSAGE; break;
        
        default:          pEvent->wType = EVENT_TYPE_PROGRAM; break;
        }


                                        // For apps, set the window display style.
    if ( m_nTaskType == CTT_RUNAPP )
        {
        switch ( m_nCmdShow )
            {
            case SW_MAXIMIZE:  pEvent->wRunStyle = NPS_RUN_MAXIMIZED;  break;
            case SW_MINIMIZE:  pEvent->wRunStyle = NPS_RUN_MINIMIZED;  break;
            default:           pEvent->wRunStyle = NPS_RUN_NORMAL;     break;
            }
        }
    else{}  //&^ Read in run style from INI!!!!!

        
                                        // Is the event enabled?
    pEvent->bEnabled = m_bTaskEnabled;

                                        // Sound an alert when this event runs?
    pEvent->bSound = m_bAudibleAlert;

                                        // And now the date and time fields....
    pEvent->wYear  = m_wYear;
    pEvent->wMonth = m_wMonth - 1;      // -1 cuz NPS numbers months 0-11
    pEvent->wDate  = m_wDay;
    pEvent->wDay   = m_wDayOfWeek;      // NOTE!! This assumes Sunday=0, Monday=1, etc.
                                        // Changing the NPS constants for SUN, 
                                        // MON, etc. will break this!
    pEvent->wHour  = m_wHour;
    pEvent->wMin   = m_wMinute;


                                        // Set the task frequency.
    switch ( m_nFrequency )
        {
        case CTF_ONCE:     pEvent->wFreq = NPS_FREQ_ONETIME; break;
        case CTF_HOURLY:   pEvent->wFreq = NPS_FREQ_HOURLY;  break;
        case CTF_DAILY:    pEvent->wFreq = NPS_FREQ_DAILY;   break;
        case CTF_WEEKLY:   pEvent->wFreq = NPS_FREQ_WEEKLY;  break;
        case CTF_WEEKDAYS: pEvent->wFreq = NPS_FREQ_WEEKDAY; break;
        case CTF_MONTHLY:  pEvent->wFreq = NPS_FREQ_MONTHLY; break;
        case CTF_YEARLY:   pEvent->wFreq = NPS_FREQ_YEARLY;  break;
        default:           ASSERT(0);                        break;
        }

                                        // NPS needs to have the next run
                                        // time set for recurring tasks.
    if ( CTF_ONCE != m_nFrequency )
        {
        CalculateNextRunTime ( pEvent );
        }


    if ( 0 != NpsEventsPut() )
        goto bailout;

    return TRUE;

bailout:
    if ( !bMakeNewEvent )
        {
        wNumEvents--;                   // Remove the event we tried to create.
        }

    return FALSE;
}


void CTask::ReturnNPSScheduleInfo ( const NPSEVNT* pEvent,
                                    SYMTASKREC*    pInfo )
{
    switch ( pEvent->wFreq )
        {
        case NPS_FREQ_HOURLY:
            pInfo->nFrequency = CTF_HOURLY;
        break;

        case NPS_FREQ_DAILY:
            pInfo->nFrequency = CTF_DAILY;
        break;

        case NPS_FREQ_WEEKLY:
            pInfo->nFrequency = CTF_WEEKLY;
            pInfo->wDayOfWeek = pEvent->wDay;
                                        // NOTE!! I'm assuming that the NPS
                                        // day-of-week constants are SUN=0,
                                        // MON=1, etc. Change those and this will
                                        // break!!
        break;

        case NPS_FREQ_WEEKDAY:
            pInfo->nFrequency = CTF_WEEKDAYS;
        break;

        case NPS_FREQ_MONTHLY:
            pInfo->nFrequency = CTF_MONTHLY;
        break;

        case NPS_FREQ_YEARLY:
            pInfo->nFrequency = CTF_YEARLY;
        break;

        case NPS_FREQ_ONETIME:
            pInfo->nFrequency = CTF_ONCE;
        break;

        default:
            ASSERT(0);                  // should never get here
        break;
        }
}


BOOL CTask::FindNPSTask ( SYMTASKREC* pTaskInfo )
{
BOOL            bRetVal = FALSE;
SYMTASKREC      rTaskInfo;

    ASSERT ( m_bNPSMode );
    ASSERT ( pTaskInfo != NULL );
    ASSERT ( *pTaskInfo->szDisplayName != '\0' );

    if ( !m_bInitialized )
        {
        TRACE("Task object wasn't initialized.\n");
        return FALSE;
        }

    if ( NULL == pTaskInfo  ||
         '\0' == *pTaskInfo->szDisplayName )
        {
        TRACE("Required param in task rec not set (NULL rec pointer or display name was empty)\n");
        return FALSE;
        }

    if ( m_bEnumeratingNPSTasks )
        {
        TRACE("FindNPSTask() will enumerate NPS tasks. You *MUST* restart any enumeration you were doing. You should call ResetEnum() first.\n");
        ResetEnum();
        }

    if ( !EnumNPSTasks() )
        {
        TRACE("CTask::EnumNPSTasks() returned FALSE.\n");
        return FALSE;
        }


    while ( NextNPSTask ( &rTaskInfo ) ) 
        {
        if ( 0 == lstrcmpi ( rTaskInfo.szDisplayName,
                             pTaskInfo->szDisplayName ) )
            {
                                        // Found a match!
            CopyMemory ( pTaskInfo, &rTaskInfo, sizeof(SYMTASKREC) );
        
            bRetVal = TRUE;
            break;
            }
        }

    ResetEnum();

    return bRetVal;
}


BOOL CTask::EnumNPSTasks()
{
    ASSERT ( m_bNAVInstalled );
    ASSERT ( m_bNPSMode );

    if ( !m_bInitialized )
        {
        TRACE("Task object not initialized.\n");
        return FALSE;
        }

    if ( m_bEnumeratingNPSTasks )
        {
        TRACE("Warning: restarting NPS task enumeration.");
        ResetEnum();
        }

    m_bEnumeratingNPSTasks = TRUE;
    m_wNextTaskToEnum = 0;

    NpsEventsGet(TRUE);

                                        // Return TRUE only if there are any
                                        // events scheduled.
    return wNumEvents != 0 ? TRUE: FALSE;
}


BOOL CTask::NextNPSTask ( SYMTASKREC* pInfo )
{
BOOL     bRetVal = FALSE;
NPSEVNT* pEvent = NULL;
TASKTYPE nTaskType;
TCHAR    szCmdLine [MAX_CMDLINE];
LPTSTR   pszTemp;
    
    USES_CONVERSION;

    ASSERT ( NULL != pInfo );
    ASSERT ( m_bNPSMode );
    ASSERT ( m_bEnumeratingNPSTasks );


    if ( !m_bEnumeratingNPSTasks )
        {
        TRACE("CTask - CTask::NextNPSTask() called before EnumTasks()\n");
        return FALSE;
        }

    if ( m_wNextTaskToEnum >= wNumEvents )
        {
        TRACE("CTask - Out of NPS events.\n");
        return FALSE;
        }

    pEvent = &Events [ m_wNextTaskToEnum ];


    // First get the task's type.

    switch ( pEvent->wType )
        {
        case EVENT_TYPE_MESSAGE:
            nTaskType = CTT_MESSAGE;
        break;

        case EVENT_TYPE_PROGRAM:
            {
            // A NAV 6 scan task is a PROGRAM type, so search the app name
            // for the ScnHndlr name.

            TCHAR szScanHandlerName [MAX_PATH];

                                        // Grab the first token, the app name.
                                        // There are 2 cases. 1: LFN containing
                                        // spaces will be quoted, and 2: any
                                        // other filename will be bare.

                                        // Save the command line since we'll
                                        // be modifying it with strtok().
            lstrcpy ( szCmdLine, A2T(pEvent->szCmdLine) );
            _tcslwr ( szCmdLine );

            if ( *pEvent->szCmdLine == '"' )
                {                       // Case 1: quoted LFN.
                pszTemp = _tcstok ( szCmdLine, _T("\"") );
                }
            else
                {                       // case 2: bare name.
                pszTemp = _tcstok ( szCmdLine, _T(" ") );
                }

            // pszTemp now points at the program name.  See if it contains
            // the name of the ScnHndlr app.  If so, then it's a NAV 6 scan 
            // task.

            lstrcpy ( szScanHandlerName, NAV_SCAN_HANDLER_EXE );
            _tcslwr ( szScanHandlerName );

            if ( NULL != pszTemp  &&
                 NULL != _tcsstr ( pszTemp, szScanHandlerName ) )
                {
                nTaskType = CTT_NAVSCANTASK;
                }
            else
                {
                nTaskType = CTT_RUNAPP;
                }
            }
        break;  // end case EVENT_TYPE_PROGRAM

        case EVENT_TYPE_NAMED:
            if ( 0 == lstrcmpi ( A2T(pEvent->szNamed), szEventNAVScan ) )
                nTaskType = CTT_NAVSCAN;
            else if ( 0 == lstrcmpi ( A2T(pEvent->szNamed), szEventNAVLU ) )
                nTaskType = CTT_NAVLU;
            else
                nTaskType = CTT_RUNAPP; // bogus event name, fall back to RUNAPP type.
        break;

        default:
            ASSERT(0);                  // should never get here
        break;
        }

    pInfo->nTaskType = nTaskType;


    // Copy all the supported fields to the SYMTASKREC.

    lstrcpy ( pInfo->szDisplayName, A2T(pEvent->szDescr) );

    if ( nTaskType != CTT_MESSAGE )
        {
                                        // Grab the first token, the app name.
                                        // There are 2 cases. 1: LFN containing
                                        // spaces will be quoted, and 2: any
                                        // other filename will be bare.

                                        // Save the command line since we'll
                                        // be modifying it with strtok().
        lstrcpy ( szCmdLine, A2T(pEvent->szCmdLine) );


        if ( *pEvent->szCmdLine == '"' )
            {                           // Case 1: quoted LFN.
            pszTemp = _tcstok ( szCmdLine, _T("\"") );
            }
        else
            {                           // case 2: bare name.
            pszTemp = _tcstok ( szCmdLine, _T(" ") );
            }

        if ( NULL != pszTemp )
            {                       // The app name was there, so save it.
            lstrcpy ( pInfo->szAppPath, pszTemp );

                                    // Now grab the rest of the string.
            pszTemp = _tcstok ( NULL, _T("") );

            if ( NULL != pszTemp )
                {                   // There were cmd line parameters.
                                    // Skip any intervening spaces and
                                    // then save the params.
                while ( _istspace ( *pszTemp ))
                    pszTemp = CharNext ( pszTemp );

                lstrcpy ( pInfo->szParams, pszTemp );
                }
            else
                {
                *pInfo->szParams = '\0';
                }
            }
        else
            {
                                        // Something was wrong in the command
                                        // line field.
            return FALSE;
            }

                                        // For NAV scan tasks, the parameter 
                                        // will be "/s:<taskname>" including 
                                        // the quotes. Pick out the task name
                                        // for use by UIs.
        if ( nTaskType == CTT_NAVSCANTASK )
            {
                                        // Copy the task name...
            lstrcpy ( pInfo->szScanTask, &pInfo->szParams[1] );

                                        // and get rid of the closing quote.
            TCHAR* pQuote = _tcschr ( pInfo->szScanTask, '"' );

            if ( NULL != pQuote )
                *pQuote = '\0';
            }

                                    // For named events (ie NAVLU and NAV
                                    // scans), there may be additional
                                    // parameters....
        if ( nTaskType == CTT_NAVSCAN  ||
             nTaskType == CTT_NAVLU )
            {
            if ( '\0' != *pEvent->szPromptParams )
                {
                if ( '\0' != *pInfo->szParams )
                    lstrcat ( pInfo->szParams, _T(" ") );

                lstrcat ( pInfo->szParams, pEvent->szPromptParams );
                }
            }
        }   // end if ( nTaskType != CTT_MESSAGE )
    else
        {
        lstrcpy ( pInfo->szMsg, A2T(pEvent->szMessage) );
        }


                                        // Copy the startup directory.
    if ( nTaskType != CTT_MESSAGE )
        lstrcpy ( pInfo->szStartingDir, A2T(pEvent->szStartupDir) );


                                        // No comment field for NPS events.
    *pInfo->szComment = '\0';

                                        // Is the event enabled?
    pInfo->bEnabled = pEvent->bEnabled;

                                        // Sound an alert when the event runs?
    pInfo->bAudibleAlert = pEvent->bSound;

                                        // Get the run style.
    if ( nTaskType != CTT_MESSAGE )
        {
        switch ( pEvent->wRunStyle )
            {
            case NPS_RUN_NORMAL: 
                pInfo->nCmdShow = SW_SHOWNORMAL;
            break;

            case NPS_RUN_MINIMIZED:
                pInfo->nCmdShow = SW_MINIMIZE;
            break;

            case NPS_RUN_MAXIMIZED:
                pInfo->nCmdShow = SW_MAXIMIZE;
            break;
            }
        }
    else
        {
        pInfo->nCmdShow = SW_SHOWNORMAL;
        }


                                        // No last run date/time for NPS events
    ZeroMemory ( &pInfo->rLastRunTime, sizeof(SYSTEMTIME) );

                                        // Get next run time for repeating
                                        // events.  For one-time events,
                                        // NPS does not save the next-run time.
    if ( NPS_FREQ_ONETIME != pEvent->wFreq )
        {
        CopyMemory ( &pInfo->rNextRunTime, &pEvent->rNextRunTime,
                     sizeof(SYSTEMTIME) );
        }
    else
        {
        ZeroMemory ( &pInfo->rNextRunTime, sizeof(SYSTEMTIME) );
        }


                                        // And now... the time/date fields.
    ZeroMemory ( &pInfo->rStartTime, sizeof(SYSTEMTIME) );

    pInfo->rStartTime.wYear   = pEvent->wYear;
    pInfo->rStartTime.wMonth  = pEvent->wMonth+1;
    pInfo->rStartTime.wDay    = pEvent->wDate;
    pInfo->rStartTime.wHour   = pEvent->wHour;
    pInfo->rStartTime.wMinute = pEvent->wMin;


                                        // Fill out other schedule info.
    ReturnNPSScheduleInfo ( pEvent, pInfo );

    m_wNextTaskToEnum++;

    return TRUE;
}


// Deleting from NPS overwrites the Events[] array, cuz that's now
// NPSAPI is written.

BOOL CTask::DeleteNPSTask ( const SYMTASKREC* pTask, BOOL bDeleteByName ) 
{
    USES_CONVERSION;

    ASSERT ( m_bNAVInstalled );
    ASSERT ( m_bNPSMode );
    ASSERT ( NULL != pTask );

    if ( NULL == pTask )
        return FALSE;

#ifdef _DEBUG
    if ( m_bEnumeratingNPSTasks )
        TRACE("CTask - Warning: Calling DeleteNPSTask() while enumerating NPS tasks.  Call ResetEnum() first to be safe.\n");
#endif

    if ( bDeleteByName )
        return DeleteNPSTask ( &pTask->szDisplayName[0] );
    else
        return DeleteNPSTask ( pTask->nTaskType );
}


BOOL CTask::DeleteNPSTask ( TASKTYPE nType )
{
SYMTASKREC rTaskRec;

    ASSERT ( m_bNAVInstalled );
    ASSERT ( m_bNPSMode );
    ASSERT ( CTT_INVALID != nType );

#ifdef _DEBUG
    if ( m_bEnumeratingNPSTasks )
        TRACE("CTask - Warning: Calling DeleteNPSTask(TASKTYPE) whilst enumerating NPS tasks.  You *MUST* restart any enumeration you were doing after this function returns!\n");
#endif

    if ( CTT_INVALID == nType )
        return FALSE;

    ResetEnum();

    NpsEventsGet(TRUE);

    if ( EnumTasks() )
        {
        while ( NextTask ( &rTaskRec ) )
            {
            if ( nType == rTaskRec.nTaskType )
                {
                TRACE1("Removing task at index %d\n", (int)(m_wNextTaskToEnum - 1));

                                        // NpsEventRemove() calls NpsEventsPut()
                NpsEventRemove ( m_wNextTaskToEnum - 1 );
                return TRUE;
                }
            }
        }

    TRACE("Event not found in NPS database.\n");

    return FALSE;                       // Event not found
}


BOOL CTask::DeleteNPSTask ( LPCSTR szJobName )
{
    ASSERT ( m_bNAVInstalled );
    ASSERT ( m_bNPSMode );
    ASSERT ( NULL != szJobName );

    if ( NULL == szJobName )
        return FALSE;

#ifdef _DEBUG
    if ( m_bEnumeratingNPSTasks )
        TRACE("Warning: Calling DeleteNPSTask() whilst enumerating NPS tasks.  Call ResetEnum() first to be safe.\n");
#endif

    for ( WORD wCnt = 0; wCnt < wNumEvents; wCnt++ )
        {
        if ( 0 == lstrcmpi ( szJobName, Events[wCnt].szDescr ) )
            {
            NpsEventRemove ( wCnt );

            return 0 == NpsEventsPut();
            }
        }

    TRACE("Event not found in NPS database.\n");

    return FALSE;                       // Event not found
}


BOOL CTask::DeleteNPSTask ( LPCWSTR wszJobName )
{
    USES_CONVERSION;

    ASSERT ( m_bNAVInstalled );
    ASSERT ( m_bNPSMode );
    ASSERT ( NULL != wszJobName );

    if ( NULL == wszJobName )
        return FALSE;

#ifdef _DEBUG
    if ( m_bEnumeratingNPSTasks )
        TRACE("Warning: Calling DeleteNPSTask() whilst enumerating NPS tasks.  Call ResetEnum() first to be safe.\n");
#endif

    return DeleteNPSTask ( W2CA(wszJobName) );
}
