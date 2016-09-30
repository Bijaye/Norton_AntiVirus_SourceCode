// Copyright 1996 Symantec Corporation, Peter Norton Computing Group
//*****************************************************************************
//
// $Header:   S:/ALERTSVC/VCS/alertstr.h_v   1.5   14 Aug 1997 18:21:50   DSACKIN  $
//
// Description:
//
//****************************************************************************
// $Log:   S:/ALERTSVC/VCS/alertstr.h_v  $
// 
//    Rev 1.5   14 Aug 1997 18:21:50   DSACKIN
// Added IDs for NOSAP.DAT strings
//
//    Rev 1.4   09 Jun 1997 17:12:50   DSACKIN
// Only watch NavOpts.Dat and NavAlert.Dat
//
//    Rev 1.3   20 May 1997 14:10:54   GDZIECI
// Defined IDS_SNMP_MSG_FORMAT.
//
//    Rev 1.2   08 May 1997 14:14:08   GDZIECI
// Defined errors for AppleTalk and IPX alerts.
//
//    Rev 1.1   18 Apr 1997 13:40:42   DSACKIN
// Ported from Tesla
//
//    Rev 1.0   13 Mar 1997 15:06:06   RFULLER
// Initial revision
//
//    Rev 1.5   27 Nov 1996 05:33:38   DSACKIN
// Added IDs for Definition Monitor Messages.
//
//    Rev 1.4   26 Nov 1996 06:22:00   DSACKIN
// Added an ID for "dynamic"
//
//    Rev 1.3   21 Nov 1996 18:14:56   DSACKIN
// Added strings for Sender and Subject
//
//    Rev 1.2   11 Nov 1996 16:29:04   DSACKIN
// Changed the IDS_EVENT_LOG ... to IDS_NT_EVENT_LOG ...
//
//    Rev 1.1   08 Nov 1996 17:27:14   TSMITH
// Moved string resource IDs regarding virus found and virus response actions
// here from NAVAPSVC.
//
//    Rev 1.7   18 Oct 1996 18:25:56   TSmith
// Removed obsolete #ifdefs.
//
//    Rev 1.6   23 Jul 1996 13:49:24   DSACKIN
// Added string IDs for new error codes associated with RPC server support.
//
//    Rev 1.5   21 Jun 1996 16:19:20   PVENKAT
// Added new constants for NETAPI32.DLL, and for errormessages.
//
//
//    Rev 1.4   06 Jun 1996 14:01:10   PVENKAT
// Added "Messenger" String.
//
//
//    Rev 1.3   16 Apr 1996 16:18:10   DSACKIN
// MC Constant changed from 0x10000 to 0x8000.
//
//    Rev 1.2   16 Apr 1996 14:49:58   DSACKIN
// Added defines for Error handling.
//
//    Rev 1.1   15 Apr 1996 15:20:58   DSACKING
// Added IDS_ERR_LOADSTRING and IDS_ERR_GETCOMPUTERNAME.
//
//    Rev 1.0   29 Mar 1996 14:48:30   PVENKAT
// Initial revision.
//****************************************************************************

#if !defined(__ALERTSTR_H)
#define __ALERTSTR_H

#if !defined(__SERVICE_H)
#  include "service.h"
#endif

//
// General utility string IDs for internal use
//
#define IDS_NAV_ALERT_SERVICE_NAME              ( 101 )
#define IDS_NAV_ALERT_EVENT_NAME                ( 102 )
#define IDS_NAV_PIPE_NAME                       ( 103 )
#define IDS_DOT_STRING                          ( 104 )
#define IDS_ALERT_PATH_FORMAT                   ( 105 )
#define IDS_MESSENGER_SERVICE_NAME              ( 106 )
#define IDS_NAV_ALERT_NETAPI                    ( 107 )
#define IDS_NAV_ALERT_NETMSGBUFSEND             ( 108 )
#define IDS_NAV_REG_ALERT_NAME                  ( 109 )
#define IDS_NAV_REG_KEY_NAME                    ( 110 )
#define IDS_SNMP_MSG_FORMAT                     ( 111 )
#define IDS_NAV_ALERT_PROGRAM_NAME              ( 112 )
#define IDS_NAV_ALERT_NOSAP_DAT                 ( 113 )

//////////////////////////////////////////////////////////////////////
// NOTE:															//
//																	//
// Virus found string IDs are defined in NAVAPLOG.H and are used by	//
// by both this module as well as NAVXUTIL where a duplicate set of	//
// string resources are stored.										//
//																	//
// Proper string resource resolution for the NAV activity log relies//
// on the duplicate set of string resources and the common IDs.		//
//																	//
//////////////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////////
// This section of strings are for the Alerts

#define IDS_NAV_EMAIL_SENDER            ( 0x300 )
#define IDS_NAV_EMAIL_SUBJECT           ( 0x301 )

#define IDS_NAV_FORWARD_DYNAMIC         ( 0x302 )


/////////////////////////////////////////////////////////////////////
// The following defines are for filenames used in def watchine

#define IDS_ALERT_NAVOPTS_DAT                      0x0280
#define IDS_ALERT_NAVALERT_DAT                     0x0281


// Error Strings
//
// Special handling is needed for String Table vs. MC Resource
//
// I'm representing these in HEX for easier conversion to the MC ID
//   MC ID = ERROR_ID | ID_ERR_MCBASE (0x8000)
//
// Example:
//
// String ID for IDS_REGOPEN_FAILED:   0x202
// MC ID for IDS_REGOPEN_FAILED:      0x8202

#define IDS_ERR_TITLE                           ( 0x200 ) // 512
#define IDS_ERR_REGQUERY_FAILED                 ( 0x201 )
#define IDS_ERR_REGOPEN_FAILED                  ( 0x202 )
#define IDS_ERR_READFILE_FAILED                 ( 0x203 )
#define IDS_ERR_CONNECTNAMEDPIPE_FAILED         ( 0x204 )
#define IDS_ERR_SETSERVICESTATUS_FAILED         ( 0x205 )
#define IDS_ERR_INITSECDESC_FAILED              ( 0x206 )
#define IDS_ERR_SETSECDESC_FAILED               ( 0x207 )
#define IDS_ERR_CREATENAMEDPIPE_FAILED          ( 0x208 )
#define IDS_ERR_BEGINTHREAD_FAILED              ( 0x209 )
#define IDS_ERR_CREATEEVENT_FAILED              ( 0x20A )
#define IDS_ERR_REGISTERSERVICECTRLR_FAILED     ( 0x20B )
#define IDS_ERR_STARTSERVICECTRLDISP_FAILED     ( 0x20C )
#define IDS_ERR_LOADSTRING                      ( 0x20D )
#define IDS_ERR_GETCOMPUTERNAME                 ( 0x20E )
#define IDS_ERR_NETAPI_NOT_FOUND                ( 0x20F )
#define IDS_ERR_NETMSGBUFSEND_NOT_FOUND         ( 0x210 )
#define IDS_ERR_WIN95                           ( 0x211 )
#define IDS_ERR_MEMALLOC                        ( 0x212 )
#define IDS_ERR_GETALERTSERVER                  ( 0x213 )
#define IDS_ERR_CHECKMESSENGER                  ( 0x214 )
#define IDS_ERR_STARTING_RPC_SERVER             ( 0x215 )
#define IDS_ERR_UNDEFINED                       ( 0x216 )
#define IDS_ERR_NAMENOTFOUND                    ( 0x217 )
#define IDS_ERR_PAUSEDREMOTE                    ( 0x218 )
#define IDS_ERR_BADRECEIVE                      ( 0x219 )

#define IDS_NT_EVENT_LOG_ENTRY_FILE             ( 0x21A )
#define IDS_NT_EVENT_LOG_ENTRY_BR               ( 0x21B )
#define IDS_NT_EVENT_LOG_ENTRY_MBR              ( 0x21C )

#define IDS_ERR_LOADALERTOPTS                   ( 0x21D )
#define IDS_ERR_INITNLMMESSENGER                ( 0x21E )

#define IDS_ERR_GETMODULEFILENAME               ( 0x21F )
#define IDS_ERR_GETDEFFILEDATES                 ( 0x220 )
#define IDS_ERR_STARTDEFINITIONMONITOR          ( 0x221 )

#define IDS_ERR_NBP_REGISTRATION                ( 0x222 )
#define IDS_ERR_ATP_THREAD_START                ( 0x223 )
#define IDS_ERR_SAP_START                       ( 0x224 )
#define IDS_ERR_IPX_THREAD_START                ( 0x225 )


/////////////////////////////////////////////////////////////////////
// Error Handling IDs
//
// For each possible error we also need to define an error ID that
// will determine how the error is handled.
#define     ID_ERR_TITLE                        IDS_ERR_TITLE | \
                                                 ID_ERR_ALL
#define     ID_ERR_REGQUERY_FAILED              IDS_ERR_REGQUERY_FAILED | \
                                                 ID_ERR_ALL
#define     ID_ERR_REGOPEN_FAILED               IDS_ERR_REGOPEN_FAILED | \
                                                 ID_ERR_ALL
#define     ID_ERR_READFILE_FAILED              IDS_ERR_READFILE_FAILED | \
                                                 ID_ERR_ALL
#define     ID_ERR_CONNECTNAMEDPIPE_FAILED      IDS_ERR_CONNECTNAMEDPIPE_FAILED | \
                                                 ID_ERR_ALL
#define     ID_ERR_SETSERVICESTATUS_FAILED      IDS_ERR_SETSERVICESTATUS_FAILED | \
                                                 ID_ERR_ALL
#define     ID_ERR_INITSECDESC_FAILED           IDS_ERR_INITSECDESC_FAILED | \
                                                 ID_ERR_ALL
#define     ID_ERR_SETSECDESC_FAILED            IDS_ERR_SETSECDESC_FAILED | \
                                                 ID_ERR_ALL
#define     ID_ERR_CREATENAMEDPIPE_FAILED       IDS_ERR_CREATENAMEDPIPE_FAILED | \
                                                 ID_ERR_ALL
#define     ID_ERR_BEGINTHREAD_FAILED           IDS_ERR_BEGINTHREAD_FAILED | \
                                                 ID_ERR_ALL
#define     ID_ERR_CREATEEVENT_FAILED           IDS_ERR_CREATEEVENT_FAILED | \
                                                 ID_ERR_ALL
#define     ID_ERR_REGISTERSERVICECTRLR_FAILED  IDS_ERR_REGISTERSERVICECTRLR_FAILED | \
                                                 ID_ERR_ALL
#define     ID_ERR_STARTSERVICECTRLDISP_FAILED  IDS_ERR_STARTSERVICECTRLDISP_FAILED | \
                                                 ID_ERR_ALL
#define     ID_ERR_LOADSTRING                   IDS_ERR_LOADSTRING | \
                                                 ID_ERR_ALL
#define     ID_ERR_GETCOMPUTERNAME              IDS_ERR_GETCOMPUTERNAME | \
                                                 ID_ERR_ALL
#define     ID_ERR_WIN95                        IDS_ERR_WIN95 | \
                                                 ID_ERR_LOGEVENT | \
                                                 ID_ERR_MESSAGEBOX
#define     ID_ERR_NETAPI_NOT_FOUND             IDS_ERR_NETAPI_NOT_FOUND | \
                                                 ID_ERR_MESSAGEBOX
#define     ID_ERR_NETMSGBUFSEND_NOT_FOUND      IDS_ERR_NETMSGBUFSEND_NOT_FOUND | \
                                                 ID_ERR_MESSAGEBOX
#define     ID_ERR_MEMALLOC                     IDS_ERR_MEMALLOC | \
                                                 ID_ERR_ALL
#define     ID_ERR_GETALERTSERVER               IDS_ERR_GETALERTSERVER | \
                                                 ID_ERR_ALL
#define     ID_ERR_CHECKMESSENGER               IDS_ERR_CHECKMESSENGER | \
                                                 ID_ERR_ALL
#define     ID_ERR_STARTING_RPC_SERVER          IDS_ERR_STARTING_RPC_SERVER | \
                                                 ID_ERR_ALL
#define     ID_ERR_ALERT_UNDEFINED              IDS_ERR_UNDEFINED | \
                                                 ID_ERR_LOGEVENT
#define     ID_ERR_ALERT_NAMENOTFOUND           IDS_ERR_NAMENOTFOUND | \
                                                 ID_ERR_LOGEVENT
#define     ID_ERR_ALERT_PAUSEDREMOTE           IDS_ERR_PAUSEDREMOTE | \
                                                 ID_ERR_LOGEVENT
#define     ID_ERR_ALERT_BADRECEIVE             IDS_ERR_BADRECEIVE | \
                                                 ID_ERR_LOGEVENT
#define     ID_ERR_LOADALERTOPTS                IDS_ERR_LOADALERTOPTS | \
                                                 ID_ERR_LOGEVENT | \
                                                 ID_ERR_MESSAGEBOX
#define     ID_ERR_INITNLMMESSENGER             IDS_ERR_INITNLMMESSENGER | \
                                                 ID_ERR_LOGEVENT | \
                                                 ID_ERR_MESSAGEBOX
#define     ID_EVENT_LOG_ENTRY_FILE             IDS_NT_EVENT_LOG_ENTRY_FILE | \
                                                 ID_ERR_LOGEVENT
#define     ID_EVENT_LOG_ENTRY_BR               IDS_NT_EVENT_LOG_ENTRY_BR | \
                                                 ID_ERR_LOGEVENT
#define     ID_EVENT_LOG_ENTRY_MBR              IDS_NT_EVENT_LOG_ENTRY_MBR | \
                                                 ID_ERR_LOGEVENT

#define     ID_ERR_GETMODULEFILENAME            IDS_ERR_GETMODULEFILENAME | \
                                                 ID_ERR_ALL
#define     ID_ERR_GETDEFFILEDATES              IDS_ERR_GETDEFFILEDATES | \
                                                 ID_ERR_ALL
#define     ID_ERR_STARTDEFINITIONMONITOR       IDS_ERR_STARTDEFINITIONMONITOR | \
                                                 ID_ERR_ALL

#define     ID_ERR_NBP_REGISTRATION             IDS_ERR_NBP_REGISTRATION | \
                                                 ID_ERR_ALL
#define     ID_ERR_ATP_THREAD_START             IDS_ERR_ATP_THREAD_START | \
                                                 ID_ERR_ALL
#define     ID_ERR_SAP_START                    IDS_ERR_SAP_START | \
                                                 ID_ERR_ALL
#define     ID_ERR_IPX_THREAD_START             IDS_ERR_IPX_THREAD_START | \
                                                 ID_ERR_ALL

#endif // #if !defined(__ALERTSTR_H)
