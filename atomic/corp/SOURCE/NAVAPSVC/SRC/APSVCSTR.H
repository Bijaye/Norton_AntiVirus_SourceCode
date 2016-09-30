// Copyright 1996 Symantec Corporation, Peter Norton Computing Group
//***************************************************************************
//
//	$Header:   S:/NAVAPSVC/VCS/apsvcstr.h_v   1.6   17 Aug 1998 10:56:04   mdunn  $
//
//	Description:
//		Header file contains all the constants defined for Auto-Protect Service
//
//***************************************************************************
// $Log:   S:/NAVAPSVC/VCS/apsvcstr.h_v  $
// 
//    Rev 1.6   17 Aug 1998 10:56:04   mdunn
// 
//    Rev 1.5   28 May 1998 05:58:36   RStanev
// Added ID for longmsg.txt.
//
//    Rev 1.4   13 Jun 1997 10:21:14   JBRENNA
// Remove IDS_NAVAP_NAVEX_SVC_KEY, IDS_NAVAP_NAVENG_SVC_KEY,
// IDS_NAVAP_SVC_IMAGEPATH_FMT, and IDS_NAVAP_IMAGE_PATH string values. They
// are no longer used.
//
//    Rev 1.3   06 Jun 1997 11:15:54   JBRENNA
// Remove IDS_NAVAP_VIRSCAN_DAT and IDS_NAVAP_VIRSCAN2_DAT. Add
// IDS_NAVAP_NAVENG_SYS, IDS_NAVAPNAVEX_SVC_KEY, IDS_NAVAP_NAVENG_SVC_KEY,
// IDS_NAVAP_SVC_IMAGE_PATH, IDS_NAVAP_IMAGE_PATH.
//
//    Rev 1.2   02 Jun 1997 19:04:44   RStanev
// Added support for new NAVAP driver.
//
//    Rev 1.1   18 Apr 1997 13:59:08   DSACKIN
// Ported from Tesla
//
//    Rev 1.0   13 Mar 1997 15:12:16   RFULLER
// Initial revision
//
//    Rev 1.3   22 Jan 1997 15:14:24   TSmith
// Defined IDS_ERR_GETDOMAINNAME_FAILED and IDM_ERR_GETDOMAINNAME_FAILED.
//
//    Rev 1.2   27 Nov 1996 03:46:26   DSACKIN
// Added string IDs for the new filename strings in file watching.
//
//    Rev 1.1   08 Nov 1996 17:45:50   TSMITH
// Removed string IDs for virus found and virus action events.
//
//    Rev 1.19   08 Sep 1996 15:41:00   DSACKIN
// Added IDs for preview version.
//
//    Rev 1.18   22 Aug 1996 00:41:18   DSACKIN
// Added strings for our definition monitor.
//
//    Rev 1.17   28 Jul 1996 10:37:22   PVENKAT
// Added new string constants for Registry entries for default user/domain name.
//
//    Rev 1.16   12 Jul 1996 19:42:44   pvenkat
// Added new string # 57230 fixed.
//
//    Rev 1.15   10 Jul 1996 10:34:54   PVENKAT
// Added two strings "Unknwon User" & "Unknown Domain".
//
//
//    Rev 1.14   02 Jul 1996 14:17:20   PVENKAT
// Added Default Desktop name.
//
//    Rev 1.13   28 Jun 1996 18:32:30   DSACKIN
// Made the Error closing Service Handle to LOG instead of MessageBox.
//
//    Rev 1.12   28 Jun 1996 18:13:48   DSACKIN
// Removed the ID for STOPSERVICEFAILURE.
//
//    Rev 1.11   24 Jun 1996 23:07:48   DSACKIN
// Added IDs for the four new Boot Virus logging strings.
//
//    Rev 1.10   06 Jun 1996 14:31:14   PVENKAT
// Changed the value of ID_ERR_NAVAP_VIRUS_FOUND_FORMAT so NT Log will pick up
// correct message.
//
//
//    Rev 1.9   16 May 1996 20:35:00   DSACKIN
// Added a new error Message ID.
//
//    Rev 1.8   16 May 1996 11:06:34   DSACKIN
// Added new IDs for new error messages.
//
//    Rev 1.7   08 May 1996 20:00:48   DSACKIN
// Added IDs for all of my new Messages.
//
//    Rev 1.6   03 May 1996 10:04:58   PVENKAT
// Renumbered String IDs.  0x01xx for both Strings & Messages &
// 0x0200 for String only.
//
//    Rev 1.5   29 Apr 1996 20:24:50   DSACKIN
// Added a couple of new IDs for Error codes with failed Mutexes.
//
//    Rev 1.4   26 Apr 1996 13:37:26   DSACKIN
// Added an ID for unable to re-load AP config messages.
//
//    Rev 1.3   24 Apr 1996 20:47:46   DSACKIN
// Added String IDs for Loading and Unloading Driver Messages.
//
//    Rev 1.2   18 Apr 1996 12:56:36   PVENKAT
// CService Version: ErrID= CatID + ResID with MC file.
//
//    Rev 1.1   18 Apr 1996 12:45:54   DSACKIN
// Added an ifdef around the include file so it doesn't get included more than
// one time.
//
//    Rev 1.0   29 Mar 1996 14:58:58   PVENKAT
// Initial revision.
//***************************************************************************


#if !defined(__APSVCSTR_H)
#define __APSVCSTR_H

// Get the common service module
#if !defined	(__SERVICE_H)
#include		<service.h>
#endif			//__SERVICE_H

// Generic Strings
#define IDS_DOT_STRING                             0x0201  // '.' string
#define IDS_NAVAP_SYMEVENT                         0x0202   // 'SymEvent'
#define IDS_NAVAP_NAVEX                            0x0203   // 'NAVEX15'
#define IDS_NAVAP_NAVAP                            0x0204   // 'NAVAP'
#define IDS_NAVAP_ALERTSVC                         0x0205   // 'NAV Alert Service'
#define IDS_NAVAP_SERVICE_NAME                     0x0206
#define IDS_NAVAP_EVENT_NAME                       0x0207
#define IDS_NAVAP_PIPE_NAME                        0x0208
#define IDS_NAVAP_PIPE_FORMAT                      0x0209
#define IDS_NAVAP_REQUESTPIPE_NAME                 0x020A   // '\\.\pipe\NAVAPSVC'
#define IDS_NAVAP_DESKTOP_NAME                     0x020B   // "WinSta0\\Default"
#define IDS_NAVAP_DEFUSERNAME                      0x020C   // 'Unknown User'
#define IDS_NAVAP_DEFDOMAINNAME                    0x020D   // 'Unknown Domain'
#define IDS_NAVAP_REGKEY                           0x020E
#define IDS_NAVAP_USERKEY                          0x020F
#define IDS_NAVAP_DOMAINKEY                        0x0210
#define IDS_NAVAP_NAVEX_SYS                        0x0211
#define IDS_NAVAP_NAVENG_SYS                       0x0212
#define IDS_NAVAP_NAVOPTS_DAT                      0x0214
#define IDS_NAVAP_NAVSTART_DAT                     0x0215
#define IDS_NAVAP_EXCLUDE_DAT                      0x0216
#define IDS_NAVAP_EXCLUDEL_DAT                     0x0217
#define IDS_NAVAP_NAVALERT_DAT                     0x0218
#define IDS_NAVAP_BOOTSCAN_USER                    0x0219
#define IDS_NAVAP_NAVENG                           0x0220   // 'NAVENG'
#define IDS_NAVAP_LONGALERT_TXT                    0x0221

#define IDS_ERR_REGISTERSERVICECTRLR_FAILED        0x0101
#define IDS_ERR_SETSERVICESTATUS_FAILED            0x0102
#define IDS_ERR_CALLNAMEDPIPE_FAILED               0x0103
#define IDS_ERR_AP_UNINITIALIZED                   0x0104
#define IDS_ERR_APDRIVER_FAILED                    0x0105
#define IDS_ERR_BEGINTHREAD_FAILED                 0x0106
#define IDS_ERR_CREATEEVENT_FAILED                 0x0107
#define IDS_ERR_STARTSERVICECTRLDISPATCHER_FAILED  0x0108
#define IDS_ERR_LOADSTRING_FAILED                  0x0109
#define IDS_ERR_RELOADCONFIG                       0x010A
#define IDS_ERR_CREATEMUTEX                        0x010B
#define IDS_ERR_WAITFORMUTEXFAILED                 0x010D
#define IDS_ERR_CREATENAMEDPIPE_FAILED             0x0112
#define IDS_ERR_SETSECDESC_FAILED                  0x0113
#define IDS_ERR_INITSECDESC_FAILED                 0x0114
#define IDS_ERR_READFILE_FAILED                    0x0115
#define IDS_ERR_CONNECTNAMEDPIPE_FAILED            0x0116
#define IDS_ERR_EXTRACTUSERNAME                    0x0117

                                        // Used in starting/stopping services
#define IDS_ERR_OPENSERVICE                        0x0120
#define IDS_ERR_STARTSERVICE                       0x0121
#define IDS_ERR_CLOSESERVICE                       0x0122
#define IDS_ERR_STARTSYMEVENT                      0x0150
#define IDS_ERR_NAVSTATSETDRIVESTATE               0x0160
#define IDS_ERR_VIRUSSCANMASTERBOOT                0x0161
#define IDS_ERR_VIRUSSCANBOOT                      0x0162
#define IDS_ERR_VIRUSSCANEND                       0x0163
#define IDS_ERR_VIRUSSCANINIT                      0x0164
#define IDS_ERR_MULTITOWIDE                        0x0165
#define IDS_ERR_NAVSTATINIT                        0x0166
#define IDS_ERR_NAVSTATEND                         0x0167
#define IDS_ERR_UNABLETOREPAIRMBR                  0x0168
#define IDS_ERR_UNABLETOREPAIRBOOT                 0x0169
#define IDS_ERR_GETMODULEFILENAME                  0x0184
#define IDS_ERR_GETSYSTEMDIRECTORY                 0x0185
#define IDS_ERR_GETDEFFILEDATES                    0x0186
#define IDS_ERR_STARTDEFINITIONMONITOR             0x0188
#define IDS_ERR_ALERT_SERVICE_RPC_LINK_FAILED      0x0189
#define IDS_ERR_ALERT_SERVICE_RPC_XMT_FAILED       0x018A
#define IDS_ERR_GETDOMAINNAME_FAILED               0x018B
#define IDS_LICENSEEXPIRED                         0x0190

                                        // For all of the IDs that are used
                                        // in ErrorHandler/ErrorHandlerID, you
                                        // must put an entry here to tell the
                                        // function how to treat the error
                                        // message.

#define ID_ERR_REGISTERSERVICECTRLR_FAILED         ID_ERR_ALL | \
                                                      IDS_ERR_REGISTERSERVICECTRLR_FAILED
#define ID_ERR_SETSERVICESTATUS_FAILED             ID_ERR_ALL | \
                                                      IDS_ERR_SETSERVICESTATUS_FAILED
#define ID_ERR_CALLNAMEDPIPE_FAILED                ID_ERR_ALL | \
                                                      IDS_ERR_CALLNAMEDPIPE_FAILED
#define ID_ERR_AP_UNINITIALIZED                    ID_ERR_ALL | \
                                                      IDS_ERR_AP_UNINITIALIZED
#define ID_ERR_APDRIVER_FAILED                     ID_ERR_ALL | \
                                                      IDS_ERR_APDRIVER_FAILED
#define ID_ERR_BEGINTHREAD_FAILED                  ID_ERR_ALL | \
                                                      IDS_ERR_BEGINTHREAD_FAILED
#define ID_ERR_CREATEEVENT_FAILED                  ID_ERR_ALL | \
                                                      IDS_ERR_CREATEEVENT_FAILED
#define ID_ERR_STARTSERVICECTRLDISPATCHER_FAILED   ID_ERR_ALL | \
                                                      IDS_ERR_STARTSERVICECTRLDISPATCHER_FAILED
#define ID_ERR_LOADSTRING_FAILED                   IDS_ERR_LOADSTRING_FAILED | \
                                                      ID_ERR_ALL
#define ID_ERR_RELOADCONFIG                        IDS_ERR_RELOADCONFIG |\
                                                      ID_ERR_ALL
#define ID_ERR_CREATEMUTEX                         IDS_ERR_CREATEMUTEX |\
                                                      ID_ERR_ALL
#define ID_ERR_WAITFORMUTEXFAILED                  IDS_ERR_WAITFORMUTEXFAILED |\
                                                      ID_ERR_ALL
#define ID_ERR_OPENSERVICE                         IDS_ERR_OPENSERVICE |\
                                                      ID_ERR_ALL
#define ID_ERR_STARTSERVICE                        IDS_ERR_STARTSERVICE |\
                                                      ID_ERR_ALL
#define ID_ERR_CLOSESERVICE                        IDS_ERR_CLOSESERVICE |\
                                                      ID_ERR_LOGEVENT
#define ID_ERR_CREATENAMEDPIPE_FAILED              IDS_ERR_CREATENAMEDPIPE_FAILED |\
                                                      ID_ERR_ALL
#define ID_ERR_SETSECDESC_FAILED                   IDS_ERR_SETSECDESC_FAILED |\
                                                      ID_ERR_ALL
#define ID_ERR_INITSECDESC_FAILED                  IDS_ERR_INITSECDESC_FAILED |\
                                                      ID_ERR_ALL
#define ID_ERR_READFILE_FAILED                     IDS_ERR_READFILE_FAILED |\
                                                      ID_ERR_ALL
#define ID_ERR_CONNECTNAMEDPIPE_FAILED             IDS_ERR_CONNECTNAMEDPIPE_FAILED |\
                                                      ID_ERR_ALL
#define ID_ERR_NAVSTATSETDRIVESTATE                IDS_ERR_NAVSTATSETDRIVESTATE |\
                                                      ID_ERR_LOGEVENT |\
                                                      ID_ERR_MESSAGEBOX
#define ID_ERR_VIRUSSCANMASTERBOOT                 IDS_ERR_VIRUSSCANMASTERBOOT |\
                                                      ID_ERR_ALL
#define ID_ERR_VIRUSSCANBOOT                       IDS_ERR_VIRUSSCANBOOT |\
                                                      ID_ERR_ALL
#define ID_ERR_VIRUSSCANEND                        IDS_ERR_VIRUSSCANEND |\
                                                      ID_ERR_ALL
#define ID_ERR_VIRUSSCANINIT                       IDS_ERR_VIRUSSCANINIT |\
                                                      ID_ERR_ALL
#define ID_ERR_MULTITOWIDE                         IDS_ERR_MULTITOWIDE |\
                                                      ID_ERR_ALL
#define ID_ERR_NAVSTATINIT                         IDS_ERR_NAVSTATINIT |\
                                                      ID_ERR_ALL
#define ID_ERR_NAVSTATEND                          IDS_ERR_NAVSTATEND |\
                                                      ID_ERR_ALL
#define ID_ERR_UNABLETOREPAIRMBR                   IDS_ERR_UNABLETOREPAIRMBR |\
                                                      ID_ERR_ALL
#define ID_ERR_UNABLETOREPAIRBOOT                  IDS_ERR_UNABLETOREPAIRBOOT |\
                                                      ID_ERR_ALL
#define ID_ERR_GETMODULEFILENAME                   IDS_ERR_GETMODULEFILENAME |\
                                                      ID_ERR_ALL
#define ID_ERR_GETSYSTEMDIRECTORY                  IDS_ERR_GETSYSTEMDIRECTORY |\
                                                      ID_ERR_ALL
#define ID_ERR_GETDEFFILEDATES                     IDS_ERR_GETDEFFILEDATES |\
                                                      ID_ERR_ALL
#define ID_ERR_STARTDEFINITIONMONITOR              IDS_ERR_STARTDEFINITIONMONITOR |\
                                                      ID_ERR_ALL
#define ID_ERR_ALERT_SERVICE_RPC_LINK_FAILED       IDS_ERR_ALERT_SERVICE_RPC_LINK_FAILED | \
                                                      ID_ERR_ALL
#define ID_ERR_ALERT_SERVICE_RPC_XMT_FAILED        IDS_ERR_ALERT_SERVICE_RPC_XMT_FAILED | \
                                                      ID_ERR_ALL
#define ID_LICENSEEXPIRED                          IDS_LICENSEEXPIRED |\
                                                      ID_ERR_LOGEVENT
#define ID_ERR_GETDOMAINNAME_FAILED                IDS_ERR_GETDOMAINNAME_FAILED |\
                                                      ID_ERR_LOGEVENT
#endif		// __APSVCSTR_H__
