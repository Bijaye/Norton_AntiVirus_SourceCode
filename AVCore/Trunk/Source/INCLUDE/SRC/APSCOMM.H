// Copyright (C) 1996 Symantec Corporation
//***************************************************************************
//
// $Header:   S:/INCLUDE/VCS/apscomm.h_v   1.1   16 Mar 1998 14:38:36   tcashin  $
//
// APSCOMM.H
//
// This header file defines the control codes used to talk to
// the NAV Auto-Protect Service.
// Description: typedefs, defines, prototypes for servenfy.c
//
// See Also:
//
//***************************************************************************
// 
// $Log:   S:/INCLUDE/VCS/apscomm.h_v  $
// 
//    Rev 1.1   16 Mar 1998 14:38:36   tcashin
// Added a control code to display Navw options.
// 
//    Rev 1.0   10 May 1997 01:41:50   ksackin
// Initial revision.
// 
//    Rev 1.1   25 Apr 1997 12:53:00   JBRENNA
// Tesla Port: DSACKIN r1.1:
// Added ID for NAVAPSVC_ALERTSVC_READY.
// 
//    Rev 1.0   06 Feb 1997 20:51:26   RFULLER
// Initial revision
// 
//    Rev 1.10   26 Sep 1996 15:23:12   DSACKIN
// Ported changes from Gryphon.
// 
//    Rev 1.13   04 Sep 1996 09:31:44   jmillar
// clean up the vcs comments
// 
//    Rev 1.12   04 Sep 1996 09:29:14   jmillar
// add standard header and add all vcs comments up to this point
// 
// -----------------------------------
// Rev 1.11
// Checked in:     30 Aug 1996 01:34:28
// Author id: DSACKIN     lines deleted/added/moved: 2/2/0
// Changed NAVAPSVC_ENTERVIRUSLIST to NAVAPSVC_PAUSE_WATCHING_DEFS and
// the same for the EXITVIRUSTLIST.
// -----------------------------------
// Rev 1.10
// Checked in:     28 Aug 1996 17:22:48
// Author id: DSACKIN     lines deleted/added/moved: 0/3/0
// Added NAVAPSVC_VIRUSLIST_ENTER and NAVAPSVC_VIRUSLIST_EXIT
// -----------------------------------
// Rev 1.9
// Checked in:     04 Aug 1996 22:08:02
// Author id: DSACKIN     lines deleted/added/moved: 7/9/0
// Added NAVAPSVC_WAIT_FOR_STARTUP ID.
// -----------------------------------
// Rev 1.8
// Checked in:     22 Jul 1996 14:22:00
// Author id: DSACKIN     lines deleted/added/moved: 6/0/0
// Moved RPC defines to NAVRPC.H
// -----------------------------------
// Rev 1.7
// Checked in:     18 Jul 1996 18:42:26
// Author id: DSACKIN     lines deleted/added/moved: 2/10/0
// Removed our dependency on SYM_MAX_PATH and added a couple of string
// definitions for our RPC work.
// -----------------------------------
// Rev 1.6
// Checked in:     19 Jun 1996 19:41:46
// Author id: DSACKIN     lines deleted/added/moved: 1/1/0
// Our packet structure needed to be DWORD alligned.
// -----------------------------------
// Rev 1.5
// Checked in:     18 Jun 1996 15:39:42
// Author id: DSACKIN     lines deleted/added/moved: 0/27/0
// Added a new packet structure for communicating with the service.
// -----------------------------------
// Rev 1.4
// Checked in:     21 May 1996 15:38:48
// Author id: DSACKIN     lines deleted/added/moved: 1/5/0
// Added a new function call for scanning the floppy drive.
// -----------------------------------
// Rev 1.3
// Checked in:     16 May 1996 20:34:34
// Author id: DSACKIN     lines deleted/added/moved: 0/1/0
// Added a new control code that is used to tell the service to load the options.
// -----------------------------------
// Rev 1.2
// Checked in:     16 May 1996 11:05:58
// Author id: DSACKIN     lines deleted/added/moved: 1/1/0
// Fixed a spelling error.
// -----------------------------------
// Rev 1.1
// Checked in:     08 May 1996 20:00:02
// Author id: DSACKIN     lines deleted/added/moved: 3/23/0
// Added some IDs for getting Device status.
// -----------------------------------
// Rev 1.0
// Checked in:     26 Apr 1996 13:01:34
// Author id: DSACKIN     lines deleted/added/moved: 0/0/0
// Initial revision.
//
//***************************************************************************

#if !defined (__APSCOMM_H)
#define __APSCOMM_H

#define PAD_SIZE (512)
#define PATH_SIZE (260)

/////////////////////////////////////////////////////////////////////
// Custom control codes to tell the service what to do...
// These are sent via the ServiceControlManager

#define  NAVAPSVC_RELOADCONFIG   (128)  // Range of commands is 128-255 only!
#define  NAVAPSVC_RELOADOPTIONS  (129)


/////////////////////////////////////////////////////////////////////
// Here are the control codes that will be written into the
// pipe to request NAVAPSVC to request action from the AP

#define  NAVAPSVC_GET_STATUS       (1)
#define  NAVAPSVC_AP_ENABLE        (2)
#define  NAVAPSVC_AP_DISABLE       (3)

#define  NAVAPSVC_AP_SCANFLOPPYA   (4)

#define  NAVAPSVC_EXEC_PROGRAM     (5)

#define  NAVAPSVC_WAIT_FOR_STARTUP (6)

#define  NAVAPSVC_PAUSE_WATCHING_DEFS    (7)
#define  NAVAPSVC_CONTINUE_WATCHING_DEFS (8)

#define  NAVAPSVC_ALERTSVC_READY   (9)

#define  NAVAPSVC_DISPLAY_NAVW_OPTIONS   (10)

/////////////////////////////////////////////////////////////////////
// Return codes for NAVAPSVC requests

#define  NAVAPSVC_SUCCESS         (0)

#define  NAVAPSVC_ENABLED         (1)
#define  NAVAPSVC_DISABLED        (2)

#define  NAVAPSVC_ERROR           (3)

#define  NAVAPSVC_FLOPPYCLEAN     (4)
#define  NAVAPSVC_FLOPPYINFECTED  (5)


/////////////////////////////////////////////////////////////////////
// Structure for our NAVAPSVC_EXEC_PROGRAM packet

typedef struct tagEXEDATA
{
   char szCmdLine [PATH_SIZE];          // Program to run // NO UNICODE HERE!
   int  nCmdShow;                       // startup style
} EXEDATA, * PEXEDATA;

typedef struct tagCMDPACKET
{
   DWORD dwSize;                        // struct size
   DWORD dwCommand;                     // Command

   union
   {
      EXEDATA rExeData;                 // Program to run information
      BYTE fixed_union_size [PAD_SIZE]; // to keep our packets a contant size
   } rData;

} CMDPACKET, * PCMDPACKET;


#endif // if !defined (__APSCOMM_H)

