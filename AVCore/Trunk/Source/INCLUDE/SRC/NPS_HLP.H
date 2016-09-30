// Copyright 1993 Symantec Corporation                                  
//***************************************************************************
// 
// $Header:   S:\include\vcs\nps_hlp.h_v   1.33   29 Jun 1995 20:37:28   EANDERS  $ 
// 
// Description:                                                         
//      This include file contains all of the help ids for the Norton
//      Program Scheduler.
// 
//*************************************************************************** 
// $Log:   S:\include\vcs\nps_hlp.h_v  $ 
// 
//    Rev 1.33   29 Jun 1995 20:37:28   EANDERS
// nps_hlp.h -> S:\include\vcs\nps_hlp.h_v
// added new ID for expired help btn
// 
//    Rev 1.29   19 Jun 1995 03:05:32   EANDERS
// nps_hlp.h -> S:\include\vcs\nps_hlp.h_v
// commented out the IDH_....UNKNOWN again!
// 
//    Rev 1.28   05 Jun 1995 13:31:56   EANDERS
// No change.
// 
//    Rev 1.27   26 May 1995 12:26:20   BARRY
// 
// 
//    Rev 1.26   25 May 1995 21:12:00   EANDERS
// nps_hlp.h -> S:\include\vcs\nps_hlp.h_v
// Added a string IDH_NAVW_SCHEDULER_WHAT_TO_SCAN_TXTBX
// 
//    Rev 1.25   25 May 1995 21:08:04   EANDERS
// No change.
// 
//    Rev 1.24   25 May 1995 20:39:46   EANDERS
// No change.
// 
//    Rev 1.23   25 May 1995 20:20:10   EANDERS
// No change.
// 
//    Rev 1.22   25 May 1995 20:11:58   EANDERS
// nps_hlp.h -> S:\include\vcs\nps_hlp.h_v
// commented out "unknown" string
// 
//    Rev 1.21   16 May 1995 16:13:40   BARRY
// Added IDH_NAVW_SCHEDULER_CONFIRM_DLG_HELP_BTN, the ID produced
// by the Helb button on any of the four confirmation boxes.
// 
// 
//    Rev 1.20   16 May 1995 12:56:58   EANDERS
// No change.
// 
//    Rev 1.19   31 Mar 1995 18:30:16   DALLEE
// Added new defines xxx_WHAT_TO_SCAN, xxx_DAY_OF_WEEK_LSTBX, xxx_MONTH_LSTBX.
// Changed most of the ID's as well (okay, I think, since they didn't match
// any of the schedule.hlp's we've ever shipped anyway).
// 
//    Rev 1.18   14 Mar 1995 13:14:12   MFALLEN
// Removed test
// 
//    Rev 1.17   14 Mar 1995 13:12:50   MFALLEN
// Test
// 
//    Rev 1.16   14 Mar 1995 12:33:12   MFALLEN
// How to help
// 
//    Rev 1.15   14 Mar 1995 10:43:50   MFALLEN
// 
//    Rev 1.14   13 Mar 1995 16:59:52   MFALLEN
// Added more IDs for other dialog boxes.
// 
//    Rev 1.13   13 Mar 1995 13:57:14   EANDERS
// nps_hlp.h -> S:\include\vcs\nps_hlp.h_v
// Added new strings for tabs.
// Added one howto string for testing.
// 
//    Rev 1.12   13 Mar 1995 10:18:02   MFALLEN
// Added Elizabeth's new help IDs and commented out old help IDs.
// 
//    Rev 1.11   26 Apr 1994 16:34:30   BRAD
// Removed PVCS Logs
//*************************************************************************** 

// Main Window

#define IDH_NAVW_SCHEDULER_WHATHIS_MAIN_WINDOW                      1

//commented out the "unknown" because it shouldn't be used
#define IDH_NAVW_SCHEDULER_WHATHIS_MAIN_WINDOW_UNKNOWN              3
#define IDH_NAVW_SCHEDULER_MAIN_WINDOW_HELP_BTN                     5

#define IDH_NAVW_SCHEDULER_WHATHIS_EVENTS_LSTBX                     7
#define IDH_NAVW_SCHEDULER_WHATHIS_EVENTS_TXT                       9
#define IDH_NAVW_SCHEDULER_WHATHIS_FREQUENCY_TXT                    11
#define IDH_NAVW_SCHEDULER_WHATHIS_SCHEDULED_TXT                    13

// Add, Edit, Copy Dialog

#define IDH_NAVW_SCHEDULER_WHATHIS_TYPE_OF_EVENT_LSTBX              21
#define IDH_NAVW_SCHEDULER_WHATHIS_DESCRIPTION                      23
#define IDH_NAVW_SCHEDULER_WHATHIS_MESSAGE_TO_DISPLAY               25
#define IDH_NAVW_SCHEDULER_WHATHIS_WHAT_TO_SCAN                     27
#define IDH_NAVW_SCHEDULER_WHATHIS_COMMAND_LINE_TO_RUN_LSTBX        29
#define IDH_NAVW_SCHEDULER_WHATHIS_STARTUP_FOLDER_LSTBX             31
#define IDH_NAVW_SCHEDULER_WHATHIS_RUN_STYLE_LSTBX                  33
#define IDH_NAVW_SCHEDULER_WHATHIS_SCHEDULE_INFORMATION_GRPBX       35
#define IDH_NAVW_SCHEDULER_WHATHIS_FREQUENCY_LSTBX                  37
#define IDH_NAVW_SCHEDULER_WHATHIS_DATE_LSTBX                       38
#define IDH_NAVW_SCHEDULER_WHATHIS_TIME_LSTBX                       39
#define IDH_NAVW_SCHEDULER_ADD_EVENT_DLG_HELP_BTN                   41
#define IDH_NAVW_SCHEDULER_WHATHIS_ENABLE_THIS_EVENT_CHKBX          43
#define IDH_NAVW_SCHEDULER_WHATHIS_AUDIBLE_ALARM_CHKBX              45
#define IDH_NAVW_SCHEDULER_WHATHIS_MINUTES_LSTBX                    47
#define IDH_NAVW_SCHEDULER_WHATHIS_EDIT_COPY_EVENT_TIME_LSTBX       49
#define IDH_NAVW_SCHEDULER_WHATHIS_DAY_OF_MONTH_LSTBX               51
#define IDH_NAVW_SCHEDULER_WHATHIS_DAY_OF_WEEK_LSTBX                53
#define IDH_NAVW_SCHEDULER_WHATHIS_MONTH_LSTBX                      55
#define IDH_NAVW_SCHEDULER_EDIT_EVENT_HLP_BTN                       57

// Options - Appearance Dialog

#define IDH_NAVW_SCHEDULER_WHATHIS_DISPLAY_STATUS_BAR_CHKBX         61
#define IDH_NAVW_SCHEDULER_WHATHIS_DISPLAY_TOOLBAR_CHKBX            63
#define IDH_NAVW_SCHEDULER_WHATHIS_TOOLBAR_STYLE_GRPBX              65
#define IDH_NAVW_SCHEDULER_WHATHIS_TEXT_ONLY_BTN                    67
#define IDH_NAVW_SCHEDULER_WHATHIS_PICTURE_ONLY_BTN                 69
#define IDH_NAVW_SCHEDULER_WHATHIS_TEXT_AND_PICTURE_BTN             71
#define IDH_NAVW_SCHEDULER_WHATHIS_SAMPLE_TOOLBAR                   73
#define IDH_NAVW_SCHEDULER_APPEARANCE_SETTINGS_HLP_BTN              75

// Options - Confirmation Dialog

#define IDH_NAVW_SCHEDULER_EXIT_APPLICATION_LAUNCHER_CHKBX          77
#define IDH_NAVW_SCHEDULER_DELETE_SCHEDULED_EVENT_CHKBX             79
#define IDH_NAVW_SCHEDULER_SAVE_A_SCHEDULED_EVENT                   81
#define IDH_NAVW_SCHEDULER_EDIT_A_SCHEDULED_EVENT                   83
#define IDH_NAVW_SCHEDULER_CONFIRMATION_SETTINGS_HLP_BTN            85

// Options - General Dialog

#define IDH_NAVW_SCHEDULER_WHATHIS_LOAD_WITH_WINDOWS_CHKBX          87
#define IDH_NAVW_SCHEDULER_WHATHIS_HIDE_WHEN_ICONIZED_CHKBX         89
#define IDH_NAVW_SCHEDULER_WHATHIS_SHOW_CURRENT_DATE_IN_TITLEBAR    91
#define IDH_NAVW_SCHEDULER_GENERAL_SETTINGS_HLP_BTN                 93

// Notification dialog box

#define IDH_NAVW_SCHEDULER_WHATHIS_NOTIFY_DESCRIPTION               101
#define IDH_NAVW_SCHEDULER_WHATHIS_NOTIFY_MESSAGE                   103

// Expired dialog box.

#define IDH_NAVW_SCHEDULER_WHATHIS_EXPIRED_INFO_TEXT                111
#define IDH_NAVW_SCHEDULER_WHATHIS_EXPIRED_INFO_TEXT2               113
#define IDH_NAVW_SCHEDULER_WHATHIS_EXPIRED_DESCRIPTION              115
#define IDH_NAVW_SCHEDULER_WHATHIS_SCHEDTIME_MESSAGE                117
#define IDH_NAVW_SCHEDULER_WHATHIS_EXPIRED_MESSAGE                  119
#define IDH_NAVW_SCHEDULER_WHATHIS_EXPIRED_HLP_BTN                  120
// Ask for confirmation dialog box.

#define IDH_NAVW_SCHEDULER_WHATHIS_CONFIRM_MESSAGE                  121
#define IDH_NAVW_SCHEDULER_WHATHIS_CONFIRM_CHECKBOX                 123
#define IDH_NAVW_SCHEDULER_CONFIRM_DLG_HELP_BTN						125
#define  IDH_NAVW_SCHEDULER_WHAT_TO_SCAN_TXTBX   127
// --------------------------------------------------------------------------
//  How To... identification numbers. Very important that all these ID
//  numbers must be 2000 or larger.
// --------------------------------------------------------------------------

#define IDH_NAVW_HOWTO_SCHEDULE_SCANS                               2000

