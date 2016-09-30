// Copyright 1998 Symantec Corporation
//////////////////////////////////////////////////////////////////////////////
// $Header:   S:/NTASKMGR/VCS/NTaskMgr.cpv   1.4   21 Jul 1998 10:13:16   mdunn  $
//
// Description: The NTaskMgr app!
//
//////////////////////////////////////////////////////////////////////////////
// $Log:   S:/NTASKMGR/VCS/NTaskMgr.cpv  $ *
// 
//    Rev 1.4   21 Jul 1998 10:13:16   mdunn
// AddWeeklyScheduledScan() now explicitly sets the bEnabled flag to TRUE when
// adding the event.
// 
//    Rev 1.3   12 Jun 1998 11:45:58   mdunn
// Bugfixes! Delete message display events in DeleteNAVTasks(), delete user-
// created NAV scan/NAVLU events in DeleteNAVTasks(), don't add a weekly
// scan event in AddWeeklyScheduledScan() if one already exists.
// 
//    Rev 1.2   13 Apr 1998 19:44:54   mdunn
// Added function descriptions so the rest of the world knows what's going on.
// 
//    Rev 1.1   09 Apr 1998 11:11:56   mdunn
// Removed #include <mstask.h> since that's now done in CTask.h
// 
//    Rev 1.0   06 Apr 1998 17:56:12   mdunn
// Initial revision.
//
//////////////////////////////////////////////////////////////////////////////

#include "platform.h"
#include <windows.h>

#define INITGUID
#include <objbase.h>
#include <objidl.h>

#include "ctask.h"
#include "resource.h"


//////////////////////////////////////////////////////////////////////////////
// Function prototypes

BOOL DeleteNAVTasks();
BOOL ConvertNPSTasks();
BOOL AddWeeklyScheduledScan ( BOOL bUseMTS );

//////////////////////////////////////////////////////////////////////////////
// Globals

extern "C" __declspec(dllimport) int    __argc;
extern "C" __declspec(dllimport) char** __argv;


//////////////////////////////////////////////////////////////////////////////
// Ya gots to have a WinMain....  This app doesn't create any windows, but
// we're using a WinMain because building as console mode causes an ugly
// console window to pop up when the app runs.

int WINAPI WinMain ( HINSTANCE hInst, HINSTANCE hPrevInst,
                     LPSTR lpCmdLine, int nCmdShow )
{
int&      argc = __argc;
char**&   argv = __argv;
int       i;
BOOL      bConvertTasks      = FALSE;
BOOL      bAddWeeklyScan     = FALSE;
BOOL      bDeleteAllNAVTasks = FALSE;
BOOL      bUseMTS            = FALSE;   // Save to Win 98 task scheduler?
BOOL      bSilentMode        = FALSE;
BOOL      bMsgEventTitleSet  = FALSE;
BOOL      bMsgEventMsgSet    = FALSE;
char      szCaption [64];
char      szMsg     [1024];
char      szMsgEventTitle [128];
char      szMsgEventMsg [128];

    for ( i = 1; i < argc; i++ )
        {
        if ( argv[i][0] == '-'  ||  argv[i][0] == '/' )
            {                           // process the command line switch
            switch ( _totlower ( argv[i][1] ) )
                {
                case 'c':
                    bConvertTasks = TRUE;
                break;

                case 'd':
                    bDeleteAllNAVTasks = TRUE;
                break;

                case 'n':
                    bAddWeeklyScan = TRUE;
                break;

                case 'w':
                    bUseMTS = TRUE;
                break;

                case 's':
                    bSilentMode = TRUE;
                break;

                case 'm':
                    if ( i+1 < argc )  // there must be at least one param left
                        {
                        lstrcpyn ( szMsgEventMsg, argv[++i],
                                   sizeof(szMsgEventMsg) - 1 );

                        bMsgEventMsgSet = TRUE;
                        }
                break;

                case 't':
                    if ( i+1 < argc )  // there must be at least one param left
                        {
                        lstrcpyn ( szMsgEventTitle, argv[++i],
                                   sizeof(szMsgEventTitle) - 1 );

                        bMsgEventTitleSet = TRUE;
                        }
                break;

                // Ignore other switches
                }
            }   // end if
        }   // end for


    if ( bMsgEventMsgSet )
        {
        if ( !bMsgEventTitleSet )       // Load a default msg title if a title
            {                           // wasn't given.
            LoadString ( hInst, IDS_DEFAULT_MSG_EVENT_TITLE,
                         szMsgEventTitle, sizeof(szMsgEventTitle) );
            }

        MessageBox ( NULL, szMsgEventMsg, szMsgEventTitle,
                     MB_ICONINFORMATION );
        }


    if ( bDeleteAllNAVTasks )
        {
        if ( !DeleteNAVTasks() && !bSilentMode )
            {
            LoadString ( hInst, IDS_MSGBOX_CAPTION, szCaption,
                         sizeof(szCaption) );

            LoadString ( hInst, IDS_DELETE_TASKS_ERROR_MSG, szMsg,
                         sizeof(szMsg) );

            MessageBox ( NULL, szMsg, szCaption, MB_ICONSTOP );
            }

        goto main_end;                  // If the /d switch was present, then
                                        // don't do anything else, regardless of
                                        // what switches were present.
        }


    if ( bConvertTasks )
        {
        if ( !ConvertNPSTasks() && !bSilentMode )
            {
            LoadString ( hInst, IDS_MSGBOX_CAPTION, szCaption,
                         sizeof(szCaption) );

            LoadString ( hInst, IDS_CONVERT_ERROR_MSG, szMsg,
                         sizeof(szMsg) );

            MessageBox ( NULL, szMsg, szCaption, MB_ICONSTOP );
            }
        }


    if ( bAddWeeklyScan )
        {
        if ( !AddWeeklyScheduledScan ( bUseMTS ) && !bSilentMode )
            {
            LoadString ( hInst, IDS_MSGBOX_CAPTION, szCaption,
                         sizeof(szCaption) );

            if ( bUseMTS )
                {
                LoadString ( hInst, IDS_ADD_SCAN_TO_MTS_ERROR_MSG, szMsg,
                             sizeof(szMsg) );
                }
            else
                {
                LoadString ( hInst, IDS_ADD_SCAN_TO_NPS_ERROR_MSG, szMsg,
                             sizeof(szMsg) );
                }

            MessageBox ( NULL, szMsg, szCaption, MB_ICONSTOP );
            }
        }


main_end:
    return 0;                           // Return 0 since we're not entering
                                        // a message loop.
}


//////////////////////////////////////////////////////////////////////////
//
// Function:    DeleteNAVTasks()
//
// Description:
//  Deletes NAVLU and NAV scan events from MTS.
//
// Input:
//  Nothing.
//
// Returns:
//  TRUE if successful, FALSE if not.
//
// Note:
//  This does *not* do anything with NPS since uninstall will remove
//  NPS and SCHEDULE.DAT itself.
//
//////////////////////////////////////////////////////////////////////////
// 4/6/98  Mike Dunn -- function created
//////////////////////////////////////////////////////////////////////////

BOOL DeleteNAVTasks()
{
BOOL       bRetVal = TRUE;
CTask      task;
SYMTASKREC rTaskRec;

    if ( !task.Init() )
        return FALSE;

    if ( !task.IsMTSAvailable() )
        return TRUE;                    // If MTS isn't around, then we don't
                                        // have anything to do.  Uninstall will
                                        // take care of removing NPS.

    task.SetMTSMode();


    rTaskRec.nTaskType = CTT_NAVSCAN;

    while ( task.DeleteTask ( &rTaskRec, FALSE ) )
        {}


    rTaskRec.nTaskType = CTT_NAVLU;

    while ( task.DeleteTask ( &rTaskRec, FALSE ) )
        {}


    rTaskRec.nTaskType = CTT_MESSAGE;

    while ( task.DeleteTask ( &rTaskRec, FALSE ) )
        {}


                                        // The above DeleteTask() calls 
                                        // delete only tasks that NTaskMgr
                                        // creates.  Now we loop thru the
                                        // tasks and search for any tasks
                                        // that the user created and delete
                                        // those as well.
    if ( task.EnumTasks() )
        {
        while ( task.NextTask ( &rTaskRec ) )
            {
            _tcslwr ( rTaskRec.szAppPath );

            if ( _tcsstr ( rTaskRec.szAppPath, _T("navw32.exe") )  ||
                 _tcsstr ( rTaskRec.szAppPath, _T("navwnt.exe") )  ||
                 _tcsstr ( rTaskRec.szAppPath, _T("navlu32.exe") ) )
                {
                task.DeleteTask ( &rTaskRec, TRUE );
                }
            }
        }


    return bRetVal;
}


//////////////////////////////////////////////////////////////////////////
//
// Function:    ConvertNPSTasks()
//
// Description:
//  Converts all existing NPS tasks to MTS tasks.
//
// Input:
//  Nothing.
//
// Returns:
//  TRUE if all tasks were converted successfully, FALSE if not.
//
//////////////////////////////////////////////////////////////////////////
// 4/6/98  Mike Dunn -- function created
//////////////////////////////////////////////////////////////////////////

BOOL ConvertNPSTasks()
{
BOOL       bRetVal = TRUE;
CTask      task;
SYMTASKREC rTaskRec;

    if ( !task.Init() )
        {
        bRetVal = FALSE;
        goto Convert_bailout;
        }

    if ( !task.IsNPSAvailable() )
        {
        goto Convert_bailout;
        }

    task.SetNPSMode();

    if ( task.EnumTasks() )
        {
        while ( task.NextTask ( &rTaskRec ) )
            {
            task.SetTaskData ( &rTaskRec );

            if ( !task.ConvertAndSave ( CTSA_RENAME_IF_EXISTS ) )
                {
                bRetVal = FALSE;
                }
            }
        }

Convert_bailout:

    return bRetVal;
}


//////////////////////////////////////////////////////////////////////////
//
// Function:    AddWeeklyScheduledScan()
//
// Description:
//  Adds a NAV scan event for Fridays at 8 PM starting on the current date.
//
// Input:
//  bUseMTS: [in] TRUE indicates the event should go in NPS, FALSE means
//           it should go in MTS.
//
// Returns:
//  TRUE if successful, FALSE if not.
//
//////////////////////////////////////////////////////////////////////////
// 4/6/98  Mike Dunn -- function created.
//////////////////////////////////////////////////////////////////////////

BOOL AddWeeklyScheduledScan ( BOOL bUseMTS )
{
CTask      task;
SYMTASKREC rTaskRec;

    if ( !task.Init() )
        return FALSE;

    bUseMTS ? task.SetMTSMode() : task.SetNPSMode();


                                        // First search for an existing
                                        // weekly scan task, and if one
                                        // exists we don't need to add
                                        // another.

    if ( task.EnumTasks() )
        {
        while ( task.NextNAVTask ( &rTaskRec ) )
            {
            if ( CTT_NAVSCAN == rTaskRec.nTaskType &&
                 CTF_WEEKLY  == rTaskRec.nFrequency )
                {
                return TRUE;
                }
            }
        }


    rTaskRec.szDisplayName[0] = '\0';
    rTaskRec.nTaskType = CTT_NAVSCAN;
    rTaskRec.nFrequency = CTF_WEEKLY;
    GetLocalTime ( &rTaskRec.rStartTime );
    rTaskRec.rStartTime.wHour = 20;
    rTaskRec.rStartTime.wMinute = 0;
    rTaskRec.wDayOfWeek = 5;            // Fridays
    rTaskRec.bEnabled = TRUE;

    task.SetTaskData ( &rTaskRec );

    return task.Save ( CTSA_RENAME_IF_EXISTS );
}

