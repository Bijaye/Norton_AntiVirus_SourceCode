/************************************************************************/
/*                                                                      */
/* $Header:   S:/NPSAPI/VCS/npsres.h_v   1.0   29 May 1997 00:45:32   ksackin  $ */
/*                                                                      */
/* Description:                                                         */
/*      This file contains all of the resource definitions for the      */
/*      Norton Program Scheduler.                                       */
/************************************************************************/
/* $Log:   S:/NPSAPI/VCS/npsres.h_v  $ */
// 
//    Rev 1.0   29 May 1997 00:45:32   ksackin
// Initial revision.
// 
//    Rev 1.0   19 May 1997 00:09:44   KSACKIN
// Initial revision.
// 
//    Rev 1.20   26 Jun 1995 15:17:10   MARKL
// Added mask bitmaps for defect 38667.
// 
//    Rev 1.19   18 Apr 1995 14:11:42   MFALLEN
// New help menu
// 
//    Rev 1.18   14 Mar 1995 16:50:46   MFALLEN
// 
//    Rev 1.17   14 Mar 1995 12:32:44   MFALLEN
// How to help
// 
//    Rev 1.16   13 Mar 1995 17:11:38   MFALLEN
// New resource IDs required due to right click help.
// 
//    Rev 1.15   13 Mar 1995 15:35:14   MFALLEN
// Added what's this help. Not tested.
// 
//    Rev 1.14   06 Mar 1995 10:12:44   MFALLEN
// Made it work for the Win95 platform.
// 
//    Rev 1.13   15 Mar 1994 11:58:48   BRUCE
// Changed EXPORT to SYM_EXPORT
// 
//    Rev 1.12   10 Feb 1994 14:34:04   PGRAVES
// Added #define for IDC_NPS_CONFIG (merging QAK2 branch into trunk).
// 
//    Rev 1.11   10 Feb 1994 12:19:40   PGRAVES
// Add QAK2 changes to trunk.
// 
//    Rev 1.10.1.2   16 Dec 1993 20:14:10   MARKL
// The button ids for the button bar were incorrect.  It cheated and
// used a base of 0 and ids of 1-5.  Unfortunately that made the
// Copy button the same as IDCANCEL.  You can guess the rest.
// 
//    Rev 1.10.1.1   13 Dec 1993 18:43:30   MARKL
// Removed ifdefs for NDW
// 
//    Rev 1.10.1.0   19 Oct 1993 08:14:52   SCOTTP
// Branch base for version QAK2
// 
//    Rev 1.10   15 Oct 1993 01:58:32   BRUCEH
// Added new menu defines
// 
//    Rev 1.9   12 Oct 1993 16:06:34   BASIL
// removed config button for revolver
// 
//    Rev 1.8   29 Sep 1993 14:57:52   MARKL
// Added named event types.
// 
//    Rev 1.7   23 Jul 1993 10:26:02   MARKL
// Help menu now looks like nirvana's
// 
//    Rev 1.6   22 Jul 1993 11:58:50   MARKL
// Cleaned up dialog and menu ids.
// 
//    Rev 1.5   10 Jun 1993 17:05:36   KYECK
// Added menu IDMs for load, hide and status bar.
//
//    Rev 1.4   10 Jun 1993 13:22:28   KYECK
// Added IDC_NPS_SOUND to add/edit event dialog.
//
//    Rev 1.3   10 Jun 1993 09:34:40   KYECK
// Added support for buttonbar and configure dialog box.
//
//    Rev 1.2   21 Apr 1993 22:05:28   MARKL
// Nirvana gives the schedule a chance to take an a radically new look/feel
// in the Add/Edit dialog!!!
//
//    Rev 1.1   15 Mar 1993 20:17:34   MARKL
// Added close button.
//
//    Rev 1.0   15 Mar 1993 17:46:18   MARKL
// Initial revision.
//
//    Rev 1.0   26 Jun 1992 17:58:52   MEIJEN
// Get rid of strids.h because NWRES will eventually disappear
//
//    Rev 1.1   16 Jan 1992 10:08:06   MARKL
// Added Common CONFIG Dialog box for NPS.
//
//    Rev 1.0   26 Jul 1991  9:31:26   MARKL
// Initial revision.
//
//    Rev 1.10   06 Jun 1991  1:05:46   MARKL
// Added clock icon.
//
//    Rev 1.9   02 May 1991 11:52:52   MARKL
// Adjusted first and last bounds.
//
//    Rev 1.8   22 Apr 1991  9:01:32   MARKL
// Added ID_NPS_RUN_MINIMIZED id.
//
//    Rev 1.7   01 Apr 1991  8:41:36   MARKL
// Fixed duplicated ids.
//
//    Rev 1.6   01 Apr 1991  8:32:42   MARKL
// Updated ID_NPS_TEXT to be ID_NPS_CMD and ID_NPS_MSG where code will
// show/hide the correct prompt.
//
//    Rev 1.5   29 Mar 1991 14:06:10   MARKL
// Added expired event dialog box.
//
//    Rev 1.4   29 Mar 1991 12:41:52   MARKL
// Added ID_NPS_SHADOW since the ShadowDlgCreate() can't handle
// integral height list boxes too well.
//
//    Rev 1.3   18 Mar 1991 18:52:28   MARKL
// Added hide and autoload checkboxes.
//
//    Rev 1.2   18 Mar 1991 10:24:32   MARKL
// No more minimize or exit buttons.  Removed defines and adjusted id value
// of help button.
//
//    Rev 1.1   05 Mar 1991 11:56:32   MARKL
// Had to move base to 27500.
//
//    Rev 1.0   05 Mar 1991  8:52:00   MARKL
// Initial revision.
//
//    Rev 1.5   04 Mar 1991 12:07:34   MARKL
// No change.
//
//    Rev 1.4   01 Mar 1991 18:52:28   MARKL
// Added internationalization.  3D effect is history.  Added strings
// to string table.
//
//    Rev 1.3   28 Feb 1991  9:27:16   MARKL
// Added exit button to main window.
//
//    Rev 1.2   25 Feb 1991  8:40:18   MARKL
// No change.
//
//    Rev 1.1   18 Feb 1991 15:08:02   MARKL
// Renamed ids for Add / Modify dialog box.  One Time is now the
// first choice of frequencies.
//
//    Rev 1.0   12 Feb 1991 23:09:18   MARKL
// Initial revision.
//
//    Rev 1.0   12 Feb 1991 23:06:14   MARKL
// Initial revision.
/************************************************************************/



#ifndef _NPSRES_H
#define _NPSRES_H

#define IDC_WHOAMI                      98
#define STR_WHOAMI_DEFAULTS             "Sched:Defaults"
#define STR_WHOAMI_LOOK                 "Sched:Look"
#define STR_WHOAMI_CONFIRMATION         "Sched:Confirmation"


/************************************************************************/
/* Dialog IDs                                                           */
/************************************************************************/

#define IDD_NPS_LIST                    27500
#define IDD_NPS_EVENT                   27501
#define IDD_NPS_ABOUT                   27504
#define IDD_NPS_TOOLBAR                 27505

/************************************************************************/
/* NPS Icon                                                             */
/************************************************************************/

#define IDI_NPS_ICON                    700
#define IDI_NPS_ICONCLK                 701

/**********************************************************************
** NPS Button Bar Bitmaps (Small/Large)
**********************************************************************/

#define IDB_NPS_BBAR_CLOSE              800
#define IDB_NPS_BBAR_ADD                801
#define IDB_NPS_BBAR_EDIT               802
#define IDB_NPS_BBAR_COPY               803
#define IDB_NPS_BBAR_REMOVE             804
#define IDB_NPS_BBAR_CONFIG             805
#define IDB_NPS_BBAR_HELP               806
#define IDB_NPS_BBAR_CLOSE_MASK         850
#define IDB_NPS_BBAR_ADD_MASK           851
#define IDB_NPS_BBAR_EDIT_MASK          852
#define IDB_NPS_BBAR_COPY_MASK          853
#define IDB_NPS_BBAR_REMOVE_MASK        854
#define IDB_NPS_BBAR_CONFIG_MASK        855
#define IDB_NPS_BBAR_HELP_MASK          856

/************************************************************************/
/* Controls in the dialog boxes                                         */
/************************************************************************/

// Main dialog...
#define IDC_NPS_LIST                    100
#define IDC_NPS_EVENT_TXT               101
#define IDC_NPS_FREQ_TXT                102
#define IDC_NPS_SCHED_TXT               103
#define IDC_NPS_BUTTONBAR               104
#define IDC_NPS_STATUS                  105
#define IDC_NPS_ADD                     900
#define IDC_NPS_EDIT                    901
#define IDC_NPS_COPY                    902
#define IDC_NPS_REMOVE                  903
#define IDC_NPS_HELP                    904
#define IDC_NPS_CLOSE                   905
#define ID_NPS_RANGE_FIRST              IDC_NPS_ADD
#define ID_NPS_RANGE_LAST               IDC_NPS_CLOSE
#define ID_TRAY_ICON                    200


// Add/Edit dialog...
#define IDC_NPS_ACTIONTYPE              200
#define IDC_NPS_ACTIONTYPE_TEXT         201
#define IDC_NPS_DESCRIPTION             202
#define IDC_NPS_DESCRIPTION_TEXT        203
#define IDC_NPS_MSG_PROMPT              204
#define IDC_NPS_MSG_TEXT                205
#define IDC_NPS_CMD_PROMPT              206
#define IDC_NPS_CMD_TEXT                207
#define IDC_NPS_ACTION_PROMPT           208
#define IDC_NPS_ACTION_TEXT             209
#define IDC_NPS_STARTUPDIR_PROMPT       210
#define IDC_NPS_STARTUPDIR_TEXT         211
#define IDC_NPS_RUNSTYLE_PROMPT         212
#define IDC_NPS_RUNSTYLE                213
#define IDC_NPS_FREQ                    214
#define IDC_NPS_FREQ_PROMPT             215
#define IDC_NPS_DATE_PROMPT             216
#define IDC_NPS_DATE                    217
#define IDC_NPS_TIME_PROMPT             218
#define IDC_NPS_TIME                    219
#define IDC_NPS_DAY_PROMPT              220
#define IDC_NPS_DAY                     221
#define IDC_NPS_MINUTES_PROMPT          222
#define IDC_NPS_MINUTES                 223
#define IDC_NPS_ENABLEEVENT             224
#define IDC_NPS_TEST                    225
#define IDC_NPS_TEXT                    226
#define IDC_NPS_SCHEDTIME               227
#define IDC_NPS_MONTH_PROMPT            228
#define IDC_NPS_MONTH                   229
#define IDC_NPS_DAYOFMONTH_PROMPT       230
#define IDC_NPS_DAYOFMONTH              231
#define IDC_NPS_SCHEDULE_GROUP          232
#define IDC_NPS_SOUND                   233


#define IDD_OPTIONS_CONFIRMATION        300
#define IDC_CONFIRM_EXIT                101
#define IDC_CONFIRM_DELETE              102
#define IDC_CONFIRM_SAVE                103
#define IDC_CONFIRM_EDIT                104


#define IDD_OPTIONS_LOOK                400
#define IDC_LOOK_SHOWSTATUSBAR          101
#define IDC_LOOK_SHOWTOOLBAR            102
#define IDC_LOOK_TEXTONLY               103
#define IDC_LOOK_PICONLY                104
#define IDC_LOOK_TEXTPIC                105
#define IDC_LOOK_SAMPLETB               106
#define IDC_LOOK_SAMPLETB_TEXT          107
#define IDC_LOOK_TBSTYLE_GRPBOX         108

#define IDD_OPTIONS_DEFAULTS            500
#define IDC_DEF_HIDE                    101
#define IDC_DEF_LOAD                    102
#define IDC_DEF_DATETIME                103

#define IDD_OPTIONS_GLOBAL              600
#define IDC_GLOBAL_HELP                 101

#define IDD_ASKCONFIRMATION             700
#define IDC_CONFIRM_MESSAGE             701
#define IDC_CONFIRM_CHECKBOX            702
#define IDC_CONFIRM_MINIMIZE            703

#define IDD_NPS_EXPIRED                 800
#define IDC_NPS_EXPIRED_INFO_TEXT       101
#define IDC_NPS_EXPIRED_INFO_TEXT2      102
#define IDC_NPS_EXPIRED_EVENT_TEXT      103
#define IDC_NPS_EXPIRED_DESCR_TEXT      104
#define IDC_NPS_EXPIRED_SCHEDTIME_TEXT  105
#define IDC_NPS_EXPIRED_SCHEDTIME       106
#define IDC_NPS_EXPIRED_TEXT_TXT        107
#define IDC_NPS_EXPIRED_TEXT            108

#define IDD_NPS_NOTIFY                  900
#define IDC_NPS_NOTIFY_DESCR_TEXT       101
#define IDC_NPS_NOTIFY_TEXT             102


/**********************************************************************
** NPS menu items
**********************************************************************/

#define IDM_NPS_EVENTCLOSE              400
#define IDM_NPS_EVENTADD                401
#define IDM_NPS_EVENTEDIT               402
#define IDM_NPS_EVENTCOPY               403
#define IDM_NPS_EVENTREMOVE             404
#define IDM_NPS_OPTSLOAD                405
#define IDM_NPS_INFODESK                406
#define IDM_NPS_HELPABOUT               407

#define ID_TRAY_MENU_RESTORE            500

/************************************************************************/
/* Accelerator table                                                    */
/************************************************************************/

#define IDA_NPS_ACCELERATOR_TABLE       500

#endif
