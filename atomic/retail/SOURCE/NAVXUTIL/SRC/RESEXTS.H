// Copyright 1992-1993 Symantec, Peter Norton Product Group                          
//************************************************************************
//									
// $Header:   S:/navxutil/VCS/resexts.h_v   1.0   06 Feb 1997 21:05:08   RFULLER  $
//									
// Description:	  							
//      This include file defines the resource identifiers for the
//      Program Files Control Panel Entry for the Norton AntiVirus for Windows 
//      Clinic.
//									
//************************************************************************
#ifndef _RESEXTS_H
#define _RESEXTS_H									



//************************************************************************
// Icon IDs
//************************************************************************

#define IDI_NAVW_APP                    100                 

//************************************************************************
// Dialog Template and Control IDs
//************************************************************************

#define IDD_PROGRAMFILES         100
#define ID_LIST                  111
#define ID_ADD                   112
#define ID_DELETE                113
#define ID_DEFAULT               114
#define ID_RAP                   118

#define IDD_PROGRAMFILESADD      120
#define ID_PROGRAMFILESADD_EXT   121
                                                

//************************************************************************
// Print Dialog Box ID numbers 
//************************************************************************

#define IDD_PRINT               200
#define IDI_PR_PRINTER          101
#define IDI_PR_FILE             102
#define IDI_PR_USER             103
#define IDI_PR_FILENAME         104

#define IDD_YESNO               300
#define ID_YESNO_MSG            100
#define ID_YESNO_BOX            101

//************************************************************************
// Notify user on possible infection 
//************************************************************************

#define IDD_NOTIFY              400
#define ID_NOTI_RAP             101
#define ID_NOTI_ALERT           102
#define ID_NOTI_BOX             103

#define IDD_UPDATETSR           500
#define ID_UPDATETSR_TEXT       101


//************************************************************************
// Drive access problem
//************************************************************************

#define IDD_DRIVEACCESS         600
#define ID_RETRY                IDOK
#define ID_CONTINUE             IDCANCEL
#define ID_SKIP                 104
#define ID_DRIVEACCESS_MSG      105

#endif



