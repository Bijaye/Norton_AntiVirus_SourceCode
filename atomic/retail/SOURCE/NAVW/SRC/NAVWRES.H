// Copyright 1992-1993 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/NAVW/VCS/navwres.h_v   1.16   13 Oct 1998 20:46:08   rchinta  $
//
// Description:
//      This include file defines the resource identifiers for the
//      Norton AntiVirus for Windows Clinic.
//
//************************************************************************

#ifndef _NAVWRES_H
#define _NAVWRES_H

//************************************************************************
// Bitmaps
//************************************************************************

#define IDB_OPENING_16_BMP                      100
#define IDB_OPENING_256_BMP                     101
#define IDB_NAVTEAM                             102
#define IDB_OPENING_SCAN_16_BMP                 103
#define IDB_OPENING_SCAN_256_BMP                104
#define IDB_BROWSE_CHECKS                       105
#define IDB_SYSTEM_WORKS                        106

#define IDB_SCANMAIN_BBAR_OPTIONS               200
#define IDB_SCANMAIN_BBAR_OPTIONS_95            202
#define IDB_SCANMAIN_BBAR_VIRUSLIST             204
#define IDB_SCANMAIN_BBAR_SCHEDULER             206
#define IDB_SCANMAIN_BBAR_ACTIVITYLOG           208
#define IDB_SCANMAIN_BBAR_ONRAMP                210
#define IDB_SCANMAIN_BBAR_QUARANTINE            212

//************************************************************************
// Icons
//************************************************************************

#define IDI_NAVW_APP                            100
#define IDI_INOC_ICON                           101
#define IDI_NAVW_APP_SMALL                      102
#define IDI_VIRUS_INFECTED                      103
#define IDI_STATUS_AP_ENABLED                   104
#define IDI_STATUS_AP_DISABLED                  105
#define IDI_STATUS_QUARANTINE                   106
#define IDI_STATUS_DEFINITIONS                  107
#define IDI_STATUS_DEFSOLD                      108

//************************************************************************
// Dialog Template and Control IDs
//************************************************************************

#define IDD_SCANMAIN                            100
#define ID_SCANMAIN_DRIVE_LIST                  200
#define ID_SCANMAIN_DRIVES_FLOPPY               201
#define ID_SCANMAIN_DRIVES_HARD                 202
#define ID_SCANMAIN_DRIVES_NETWORK              203
#define ID_SCANMAIN_DRIVES_SELECTED             204
#define ID_SCANMAIN_STATUS                      205
#define ID_SCANMAIN_EXIT                        207
#define ID_SCANMAIN_DRIVETYPE_TEXT              208
#define ID_SCANMAIN_DRIVES_GROUP                209
#define ID_SCANMAIN_BUTTONS_GROUP               210
#define ID_SCANMAIN_TOOLBAR                     211
#define ID_SCANMAIN_REBAR                       212
#define IDC_STATUS_AP_ICON                      213
#define IDC_STATUS_2_ICON                       214
#define IDC_STATUS_AP_TEXT                      215
#define IDC_STATUS_2_TEXT                       216
#define IDC_STATUS_AP_BUTTON                    217
#define IDC_STATUS_2_BUTTON                     218
#define IDC_SYSTEM_WORKS                        219
#define IDD_SCANMAIN_SYSWORKS                   220


#define IDD_SCAN_FOLDERS                        300
#define IDC_BROWSE                              301
#define IDC_SCAN_FOLDERS_TEXT                   302
#define IDC_SCAN_FOLDERS_CHKBOX                 303
#define IDC_SCAN_FOLDERS_EDIT                   304

#define IDD_SCAN_PATH_DIALOG                    1000
#define IDC_PATH_EDIT                           1001
#define IDC_ICON_STATIC                         1002
#define IDC_SUB_CHECK                           1003

#define IDC_INFOTEXT                            1004
#define IDC_INFOICON                            1005

//************************************************************************
//************************************************************************

#define IDD_NAVEGG                              400
#define IDD_INFO                                401


//************************************************************************
// Menu IDs
//************************************************************************

#define IDM_NAVW_MENU                   100

#define IDM_SCAN_SELECTED            500
#define IDM_SCAN_DIRECTORY           501
#define IDM_SCAN_FILE                502
#define IDM_SCAN_PATH                503
#define IDM_SCAN_EXIT                504

#define IDM_TOOLS_OPTIONS           701
#define IDM_TOOLS_VIRUSLIST         702
#define IDM_TOOLS_SCHEDULER         703
#define IDM_TOOLS_ACTIVITYLOG       704
#define IDM_TOOLS_QUARANTINE        705
#define IDM_TOOLS_ONRAMP            706

#define IDM_HELP_INFODESK               800
#define IDM_HELP_PSO_CONNECT                    801
#define IDM_HELP_PSO_AVSUPPORT              802
#define IDM_HELP_PSO_ONLINESUPPORT      803
#define IDM_HELP_PSO_SARC                           804
#define IDM_HELP_ABOUT                  999

#define IDS_NAVOPTS_STR_NAVAP       24000
#define IDS_NAVOPTS_STR_STARTUP     24001

#define IDS_INFO_QUARANTINE1        1101
#define IDS_INFO_QUARANTINE2        1102
#define IDS_INFO_DEFSCURRENT        1103
#define IDS_INFO_DEFSOLD            1104
#define IDS_INFO_DEFSNEW            1105
#define IDS_INFO_DEFSOLDOEM         1106
#endif  // _NAVWRES_H


