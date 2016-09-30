
// Copyright 1997 Symantec Corporation
//************************************************************************
//
// $Header:   S:/APWCMD/VCS/ApwNtRes.h_v   1.0   28 May 1997 12:27:28   JBRENNA  $
//
// Description:
//      ApwNtRes.h -- This provides all of the resource ids for this
//      project.
//
//************************************************************************
// $Log:   S:/APWCMD/VCS/ApwNtRes.h_v  $
//
//    Rev 1.0   28 May 1997 12:27:28   JBRENNA
// Initial revision.
//
//************************************************************************

#ifndef _APWNTRES_H
#define _APWNTRES_H

//************************************************************************
// String IDs
//************************************************************************

#define IDS_NAVAP_FLOPPYAINFECTED   3001
#define IDS_NAVAP_INFECTION_TITLE   3002
#define IDS_VIRUS_FOUND_MESSAGE     4

//************************************************************************
// Virus alert string IDs.
//************************************************************************

#define	IDS_VIR_ACT_NONE					3500
#define	IDS_VIR_ACT_COMPUTER_SHUTDOWN   	3501
#define	IDS_VIR_ACT_DELETED             	3502
#define	IDS_VIR_ACT_NOT_DELETED         	3503
#define	IDS_VIR_ACT_REPAIRED            	3504
#define	IDS_VIR_ACT_NOT_REPAIRED        	3505
#define	IDS_VIR_ACT_QUARANTINED         	3506
#define	IDS_VIR_ACT_NOT_QUARANTINED     	3507
#define	IDS_VIR_ACT_ACCESS_DENIED       	3508
#define	IDS_VIR_ACT_ACTION_ALLOWED      	3509
#define	IDS_VIR_ACT_ACTION_NOT_ALLOWED  	3510
#define	IDS_VIR_ACT_BOOTREC_REPAIRED    	3511
#define	IDS_VIR_ACT_BOOTREC_NOT_REPAIRED	3512
#define	IDS_VIR_ACT_MBR_REPAIRED        	3513
#define	IDS_VIR_ACT_MBR_NOT_REPAIRED    	3514
#define IDS_VIR_ACT_UNDEFINED				3515
#define IDS_VER_ACT_DESTROYED				3516
#define IDS_VIR_ACT_BOOTREC_ACCESS_DENIED   3517
#define IDS_VIR_ACT_MBR_ACCESS_DENIED       3518

#define	IDS_VIR_MSG_IS_INFECTED				3600
#define	IDS_VIR_MSG_WAS_INFECTED     		3601
#define	IDS_VIR_MSG_BOOT_IS_INFECTED 		3602
#define	IDS_VIR_MSG_BOOT_WAS_INFECTED		3603
#define	IDS_VIR_MSG_MBR_IS_INFECTED  		3604
#define	IDS_VIR_MSG_MBR_WAS_INFECTED 		3605
#define	IDS_VIR_MSG_BOOT_INOC_CHANGE 		3606
#define	IDS_VIR_MSG_MBR_INOC_CHANGE  		3607
#define	IDS_VIR_MSG_VIRUS_IN_MEMORY  		3608
#define IDS_VIR_MSG_VIRUS_UNKNOWN_STATUS	3609

//************************************************************************
// Blocked app and virus alert resources
//************************************************************************
#define 	IDS_BLOCKED_APP_ALERT_TEXT      3610
#define	IDS_BLOCKED_APP_BUTTON_OK	3611
#define	IDS_BLOCKED_APP_BUTTON_ABORT	3612
#define	IDS_BLOCKED_APP_BUTTON_MOREINFO	3613
#define 	IDS_BLOCKED_APP_TITLE		3614
#define	IDS_BLOCKED_APP_UNKNOWN_APP	3615
#define 	IDS_ACTION_TAKEN_STATIC	        3616
#define 	IDS_VIRUS_NAME_STATIC           3617
#define 	IDS_OBJECT_NAME_STATIC          3618
#define 	IDS_VIRUS_ALERT_TITLE           3619
#define 	IDS_HELP_BUTTON_TEXT            3621
#define     IDS_SUBSCRIPTION_EXPIRED	  3622
#define     IDS_ACTION_RENEW_SUB            3623
#define 	IDS_ACTION_IGNORE_SUB 		  3624
#define     IDS_WINDOWSSUBSYSTEM		  3625
#define 	IDS_CONTAINER_NAME_STATIC	  3626
#define	IDS_CONTAINER_IGNORE_CONTAINER  3627
#define 	IDS_BLOCKED_APP_TASKBAR		  3628
#define 	IDS_UNKNOWN_USER			  3629
#define IDS_SUBSCRIPTION_EXPIRED_NO_ADMIN_RIGHTS 3630

#define IDB_TOPBAR_YEL                  102
#define IDS_SPYWARE_ALERT_TITLE         4004
#define IDS_SPYWARE_BRIEF_DESC          4005
#define IDS_MORE_INFORMATION            4006
#define IDS_SCAN_NOW                    4007
#define IDS_IGNORE                      4008
#define IDS_ERR_AP_SPYWARE_SCAN         4009
#define IDS_THREAT_CATEGORY             103
#define IDS_THREAT_NAME                 4010
#define IDS_SIDEEFFECTS_BRIEF_DESC      4011
#define IDS_SIDEEFFECTS_ALERT_TITLE     4012
#define IDS_EXCLUDE                     4013
#define IDS_IGNORE_TIME                 4014
#define IDS_LAUNCH_CFGWIZ               4015

//*********
// Bitmaps
//*********
#define IDB_VIRUS_ALERT_BITMAP 3700

//******
// Icon
//******
#define IDI_NAV_ICON       1007
#endif // _APWNTRES_H
