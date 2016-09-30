// Copyright 1992-1996 Symantec, Peter Norton Product Group
//***********************************************************************
//
// $Header:   S:/NAVC/VCS/selusers.c_v   1.1   26 May 1997 16:15:18   BGERHAR  $
//
// Description:
//  Displays Network User Browse dialog
//      Stolen from NAV 2.1's NAVNET.
//
// See Also:
//***********************************************************************
// $Log:   S:/NAVC/VCS/selusers.c_v  $
// 
//    Rev 1.1   26 May 1997 16:15:18   BGERHAR
// Fix warnings during compile
// 
//    Rev 1.0   06 Feb 1997 20:56:20   RFULLER
// Initial revision
// 
//    Rev 1.0   31 Dec 1996 15:19:54   MKEATIN
// Initial revision.
// 
//    Rev 1.2   07 Nov 1996 12:19:48   JBELDEN
// this file is just a stub for a routine called in an external
// library.
// 
//    Rev 1.1   28 Oct 1996 11:21:04   JBELDEN
// stubbed out all the routines.  Not needed for ttyscan
// 
//    Rev 1.0   02 Oct 1996 12:30:32   JBELDEN
// Initial revision.
//***********************************************************************

#include "platform.h"
#include "xapi.h"
#include "stddlg.h"
#include "dosnet.h"
#include "stddos.h"

#include "navdprot.h"
#define BINDERY_OBJECT_TIME FAKE_BINDERY_OBJECT_TIME
#include "netobj.h"
#undef BINDERY_OBJECT_TIME

MODULE_NAME;


/************************************************************************
 *                                                                      *
 * Description:                                                         *
 *                                                                      *
 ************************************************************************
 * 09/01/91 GREG Function Created.                                      *
 ************************************************************************/

BOOL PASCAL StdDlgSelectNetworkUsers(NETSELECTOBJECTSDOS *lprSelectObjects)
{
    return FALSE;                       // No changes
}



