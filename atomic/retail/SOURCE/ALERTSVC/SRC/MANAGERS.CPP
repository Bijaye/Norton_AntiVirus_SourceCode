/////////////////////////////////////////////////////////////////////
// MANAGERS.CPP - Created - 01/16/97
//
// Copyright 1992-1997 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/ALERTSVC/VCS/managers.cpv   1.0   18 Apr 1997 13:41:20   DSACKIN  $
//
// Description:
//    This file contains the declarations of all of the
//    CAlertQueueManager objects.  This also has a global variable
//    that will contain a list of these objects that will be used
//    in the main manager to control flow of the program.
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/ALERTSVC/VCS/managers.cpv  $
// 
//    Rev 1.0   18 Apr 1997 13:41:20   DSACKIN
// Ported from Tesla
// 
//    Rev 1.0   13 Mar 1997 15:06:12   RFULLER
// Initial revision
// 
//    Rev 1.0   18 Jan 1997 06:58:48   DSACKIN
// Initial revision.
//
/////////////////////////////////////////////////////////////////////

#define _WIN32_WINNT 0x0400

/////////////////////////////////////////////////////////////////////
//      Includes

#include "afxtempl.h"
#include "platform.h"
#include "process.h"
#include "winnetwk.h"
#include "lmcons.h"
#include "lmmsg.h"
#include "lmerr.h"
#include "linksrv.h"
#include "service.h"
#include "svcctrl.h"
#include "navrpc.h"
#include "xapi.h"
#include "at_opts.h"                    // For CAlertOptions class
#include "alertsvc.h"
#include "alertstr.h"

#include "CMail.h"
#include "CSMTP.h"

#include "QueueMgr.h"
#include "Managers.h"

/////////////////////////////////////////////////////////////////////
// This file contains all of the manager functions that don't belong
// in a specific target type

